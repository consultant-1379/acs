#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <stdint.h>

#include <ace/Guard_T.h>

#include <acs_prc_api.h>

#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_operationpipeeventhandler.h"
#include "acs_apbm_procsignalseventhandler.h"
#include "acs_apbm_sanotifiermanager.h"
#include "acs_apbm_sanotifiereventhandler.h"
#include "acs_apbm_operationpipescheduler.h"
#include "acs_apbm_monitoringservicehandler.h"
#include "acs_apbm_configurationhelper.h"

#include "acs_apbm_serverworkingset.h"

const char * __CLASS_NAME__::_node_state_names [] = {"UNDEFINED", "ACTIVE", "PASSIVE"};
int __CLASS_NAME__::_proc_signals_to_catch [] = {SIGHUP, SIGINT, SIGPIPE, SIGTERM};

int __CLASS_NAME__::init_operation_pipe_and_handler () {
	int return_code = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	// Trying to initialize the HA operation pipe
	if (operation_pipe[0] == ACE_INVALID_HANDLE) { // Operation pipe has to be initialized
		// Create and initialize the operation_pipe to get signalled about operations and active/passive/stop state changes
		if (!::pipe(operation_pipe)) { //OK
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "HA operation pipe: pipe object initializated successfully");

			if (::fcntl(operation_pipe[0], F_SETFL, O_NONBLOCK) == -1) { // ERROR: Setting non block flag on the reader end of the pipe
				ACS_APBM_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "HA operation pipe: fcntl of non-block flag failed on reader part: call '::fcntl' failed");
				return_code = acs_apbm::ERR_FCNTL_CALL;
			} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "HA operation pipe: non blocking flag successfully set on the reader part");

			if (::fcntl(operation_pipe[1], F_SETFL, O_NONBLOCK) == -1) { // ERROR: Setting non block flag on the writer end of the pipe
				ACS_APBM_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "HA operation pipe: fcntl of non-block flag failed on writer part: call '::fcntl' failed");
				return_code = acs_apbm::ERR_FCNTL_CALL;
			} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "HA operation pipe: non blocking flag successfully set on the writer part");
		} else { // ERROR: Pipe creation error
			operation_pipe[0] = operation_pipe[1] = ACE_INVALID_HANDLE;
			ACS_APBM_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "HA operation pipe: initialization failed: call '::pipe' failed");
			return_code = acs_apbm::ERR_PIPE_CALL;
		}
	}

	if (operation_pipe[0] == ACE_INVALID_HANDLE)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "pipe reader part's descriptor (operation_pipe[0]) is not valid: amf operation pipe not initialized: cannot use the internal operation pipe event handler");
	else if (operation_pipe_event_handler->get_handle() == ACE_INVALID_HANDLE) { // Trying to register the handler
		// Set the pipe handle into the operation pipe event handler
		operation_pipe_event_handler->set_handle(operation_pipe[0]);

		// Check the presence of the operation pipe event handler into the main reactor
		int call_result = main_reactor->handler(operation_pipe_event_handler->get_handle(), ACE_Event_Handler::READ_MASK);

		if (call_result == 0) // Event handler already registerd
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Operation pipe event handler already registered into the main reactor");
		else { // Event handler not found
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to add the operation pipe event handler into the main reactor...");

			// Trying to register the operation pipe event handler into the main reactor
                                errno=0;
			if (main_reactor->register_handler(operation_pipe_event_handler, ACE_Event_Handler::READ_MASK) < 0) { // ERROR: registering the event handler
				operation_pipe_event_handler->set_handle(ACE_INVALID_HANDLE);
				return_code = acs_apbm::ERR_REACTOR_HANDLER_REGISTER_FAILURE;
                                int errno_save=errno;
				ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'register_handler' failed: cannot register the operation pipe event handler into the main reactor");
			} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation pipe event handler successfully added to the main reactor");
		}
	}

	return return_code;
}

int __CLASS_NAME__::reset_operation_pipe_and_handler () {
	int return_code = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	// Check the presence of the operation pipe event handler into the main reactor
	int call_result = main_reactor->handler(operation_pipe_event_handler->get_handle(), ACE_Event_Handler::READ_MASK);

	if (call_result == 0) { // There is an event handler into the main reactor. Trying to remove it
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to remove the operation pipe event handler from the main reactor...");

		if (main_reactor->remove_handler(operation_pipe_event_handler->get_handle(), ACE_Event_Handler::READ_MASK) < 0) {
			// ERROR: Removing the event handler from the reactor
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'remove_handler' failed: trying to remove the operation pipe event handler from the main reactor");
			return_code = acs_apbm::ERR_REACTOR_HANDLER_REMOVE_FAILURE;
		} else { // OK: Handler removed
			operation_pipe_event_handler->set_handle(ACE_INVALID_HANDLE);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation pipe event handler successfully removed from the main reactor");
		}
	} else { // WARNING: Handler already removed
		operation_pipe_event_handler->set_handle(ACE_INVALID_HANDLE);
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Operation pipe event handler already removed from main reactor");
	}

	// If the handler was removed from the reactor then try to close the pipe descriptors
	if (operation_pipe_event_handler->get_handle() == ACE_INVALID_HANDLE) { // OK: I can close the pipe descriptors if not already done
		if (operation_pipe[1] ^ ACE_INVALID_HANDLE) {
			// Closing writer part
			if(::close(operation_pipe[1]) < 0) {
				return_code = acs_apbm::ERR_CLOSE_CALL;
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call '::close' failed: trying to close the operation pipe writer part");
			} else operation_pipe[1] = ACE_INVALID_HANDLE;
		}

		if (operation_pipe[0] ^ ACE_INVALID_HANDLE) {
			// Closing reader part
			if(::close(operation_pipe[0]) < 0) {
				return_code = acs_apbm::ERR_CLOSE_CALL;
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call '::close' failed: trying to close the operation pipe reader part");
			} else operation_pipe[0] = ACE_INVALID_HANDLE;
		}
	} else // ERROR: handler not correctly removed from reactor. I cannot close the pipe descriptors
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot close the pipe descriptors because the operation pipe event handler was not removed from the main reactor");

	return return_code;
}

int __CLASS_NAME__::start_operation_pipe_scheduler () {
	if (!operation_pipe_scheduler) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "operation_pipe_scheduler pointer is NULL: object not initialized correctly");
		return acs_apbm::ERR_NULL_POINTER;
	}

	return operation_pipe_scheduler->start(main_reactor, acs_apbm_programconfiguration::operation_pipe_scheduler_initial_delay_ms, acs_apbm_programconfiguration::operation_pipe_scheduler_interval_ms);
}

int __CLASS_NAME__::stop_operation_pipe_scheduler () {
	return operation_pipe_scheduler->stop();
}

int __CLASS_NAME__::init_signals_handler () {
	int return_code = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	// Adding the process signal handler for the signals APBM server has to catch
	for (size_t sig_index = 0; sig_index < ACS_APBM_ARRAY_SIZE(_proc_signals_to_catch); ++sig_index) {
		int signal = _proc_signals_to_catch[sig_index];

		// Try to add the process signal handler for signal
		if (!_proc_catched_signals.is_member(signal)) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Process signal handling: adding the handler for the signal %d (%s)", signal, ::strsignal(signal));

			// signal not present, adding it
			errno = 0;
			if (main_reactor->register_handler(signal, proc_signals_event_handler) < 0) { //ERROR: adding the signal handler
				return_code |= acs_apbm::ERR_REACTOR_HANDLER_REGISTER_FAILURE;
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Process signal handling: call 'register_handler' failed: cannot register the handler for the signal %d (%s)", signal, ::strsignal(signal));
			} else _proc_catched_signals.sig_add(signal); //OK: remember this signal was added.
		}
	}

	return return_code;
}

int __CLASS_NAME__::reset_signals_handler () {
	int return_code = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	// Removing the process signal handling
	for (size_t sig_index = 0; sig_index < ACS_APBM_ARRAY_SIZE(_proc_signals_to_catch); ++sig_index) {
		int signal = _proc_signals_to_catch[sig_index];

		// Try to remove the process signal handler for signal
		if (_proc_catched_signals.is_member(signal)) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Process signal handling: removing the handler for the signal %d (%s)", signal, ::strsignal(signal));

			// signal present, removing it
			errno = 0;
			if (main_reactor->remove_handler(signal, reinterpret_cast<ACE_Sig_Action *>(0)) < 0) { //ERROR: removing the signal handler
				return_code |= acs_apbm::ERR_REACTOR_HANDLER_REMOVE_FAILURE;
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Process signal handling: call 'remove_handler' failed: cannot remove the handler for the signal %d (%s)", signal, ::strsignal(signal));
			} else _proc_catched_signals.sig_del(signal); //OK: remember this signal was removed.
		}
	}

	return return_code;
}

int __CLASS_NAME__::open_api_service_acceptor_unix () {
	int call_result = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: Opening the API service acceptor UNIX object (sap == '%s')...", acs_apbm_configurationhelper::apbmapi_unix_socket_sap_pathname());

	if (board_service_acceptor_api_unix->is_open()) { // WARNING: UNIX listener already opened
		ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: API service acceptor UNIX object already opened!");
	} else {
		if ((call_result = board_service_acceptor_api_unix->open(acs_apbm_boardserviceacceptor::UNIX_SAP, acs_apbm_configurationhelper::apbmapi_unix_socket_sap_pathname()))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service handling: Call 'open' failed: cannot open the API service acceptor UNIX object: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: API service acceptor UNIX object opened successfully!");
	}

	return call_result ? acs_apbm::ERR_OPEN_CALL : acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::close_api_service_acceptor_unix () {
	int call_result = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: Closing the API service acceptor UNIX object ...");

	if ((call_result = board_service_acceptor_api_unix->close())) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service handling: Call 'close' failed: cannot close the API service acceptor UNIX object: call_result == %d", call_result);
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: API service acceptor UNIX object closed successfully");

	return call_result ? acs_apbm::ERR_CLOSE_CALL : acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::open_api_service_acceptor_inet () {
	int call_result = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: Opening the API service acceptor INET object (sap == '%s')...", acs_apbm_configurationhelper::apbmapi_inet_socket_sap());

	if (board_service_acceptor_api_inet->is_open()) { // WARNING: INET listener already opened
		ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: API service acceptor INET object already opened!");
	} else {
		if ((call_result = board_service_acceptor_api_inet->open(acs_apbm_boardserviceacceptor::INET_SAP, acs_apbm_configurationhelper::apbmapi_inet_socket_sap()))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service handling: Call 'open' failed: cannot open the API service acceptor INET object: call_result == %d", call_result);
		} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: API service acceptor INET object opened successfully!");
	}

	return call_result ? acs_apbm::ERR_OPEN_CALL : acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::close_api_service_acceptor_inet () {
	int call_result = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: Closing the API service acceptor INET object ...");

	if ((call_result = board_service_acceptor_api_inet->close())) { // ERROR
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "API service handling: Call 'close' failed: cannot close the API service acceptor INET object: call_result == %d", call_result);
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "API service handling: API service acceptor INET object closed successfully");

	return call_result ? acs_apbm::ERR_CLOSE_CALL : acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::init_sa_notifier_manager_and_handler () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	// Trying to initialize the SA notifier manager
	if (const int call_result = acs_apbm_sanotifiermanager::open(this)) { // ERROR: cannot open
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open' failed: cannot open the SA notifier manager: call_result == %d", call_result);
		return call_result;
	}

	// Trying to subscribe to NFT services
	acs_apbm_sanotifiermanager::active_notification_filter_t active_filter;
	const acs_apbm_sanotifiermanager::notification_filter_allocation_params_t filter_allocation_params;

	active_filter.state_change = true;

	if (const int call_result = acs_apbm_sanotifiermanager::subscribe(active_filter, &filter_allocation_params)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'subscribe' failed: cannot subscribe the notifier manager to the NTF services: call_result == %d", call_result);
		return call_result;
	}

	int return_code = acs_apbm::ERR_NO_ERRORS;

	// Trying to register the SA notifier event handler into the main reactor
	if (sa_notifier_event_handler->get_handle() == ACE_INVALID_HANDLE) {
		sa_notifier_event_handler->set_handles(acs_apbm_sanotifiermanager::get_selection_handle(), acs_apbm_sanotifiermanager::get_ntf_handle());
//		sa_notifier_event_handler->set_handle(acs_apbm_sanotifiermanager::get_selection_handle());
//		sa_notifier_event_handler->set_ntf_handle(acs_apbm_sanotifiermanager::get_ntf_handle());

		// Check the presence of the SA notifier event handler into the main reactor
		const int call_result = main_reactor->handler(sa_notifier_event_handler->get_handle(), ACE_Event_Handler::READ_MASK);

		if (call_result) { // Event handler not found
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to register the SA notifier event handler into the main reactor...");

			// Trying to register the SA notifier event handler into the main reactor
                                 errno=0;
			if (main_reactor->register_handler(sa_notifier_event_handler, ACE_Event_Handler::READ_MASK) < 0) { // ERROR: registering the event handler
				sa_notifier_event_handler->set_handles(ACE_INVALID_HANDLE, 0);
//				sa_notifier_event_handler->set_handle(ACE_INVALID_HANDLE);
//				sa_notifier_event_handler->set_ntf_handle(0);
                                int errno_save=errno;
				return_code = acs_apbm::ERR_REACTOR_HANDLER_REGISTER_FAILURE;
				ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'register_handler' failed: cannot register the SA notifier event handler into the main reactor");
			} else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SA notifier event handler successfully registered to the main reactor");
		} else { // WARNING: SA notifier event handler already registered
			ACS_APBM_LOG(LOG_LEVEL_WARN, "SA notifier event handler already registered into the main reactor");
		}
	}

	return return_code;
}

int __CLASS_NAME__::reset_sa_notifier_manager_and_handler () {
	int return_code = acs_apbm::ERR_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_initialization_sync, true);

	// Check the presence of the SA notifier event handler into the main reactor
	int call_result = main_reactor->handler(sa_notifier_event_handler->get_handle(), ACE_Event_Handler::READ_MASK);

	if (call_result == 0) { // There is an event handler into the main reactor. Trying to remove it
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to remove the SA notifier event handler from the main reactor...");

		errno = 0;
		if (main_reactor->remove_handler(sa_notifier_event_handler->get_handle(), ACE_Event_Handler::READ_MASK) < 0) {
			// ERROR: Removing the event handler from the reactor
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'remove_handler' failed: trying to remove the SA notifier event handler from the main reactor");
			return_code = acs_apbm::ERR_REACTOR_HANDLER_REMOVE_FAILURE;
		} else { // OK: Handler removed
			sa_notifier_event_handler->set_handles(ACE_INVALID_HANDLE, 0);
			//sa_notifier_event_handler->set_handle(ACE_INVALID_HANDLE);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SA notifier event handler successfully removed from the main reactor");
		}
	} else { // WARNING: Handler already removed
		sa_notifier_event_handler->set_handles(ACE_INVALID_HANDLE, 0);
		//sa_notifier_event_handler->set_handle(ACE_INVALID_HANDLE);
		ACS_APBM_LOG(LOG_LEVEL_WARN, "SA notifier event handler already removed from main reactor");
	}

	// If the handler was removed from the reactor then try to close the SA notifier manager
	if (sa_notifier_event_handler->get_handle() == ACE_INVALID_HANDLE) { // OK: I can close the SA notifier manager
		if (const int call_result = acs_apbm_sanotifiermanager::unsubscribe()) { // ERROR: unsubscription error
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'unsubscribe' failed: call_result == %d", call_result);
		}

		if (const int call_result = acs_apbm_sanotifiermanager::close()) { // ERROR: closing error
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'close' failed: call_result == %d", call_result);
		}
	} else // ERROR: handler not correctly removed from reactor. I cannot close the SA notifier manager
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot close the SA notifier manager because the SA notifier event handler was not removed from the main reactor");

	return return_code;
}

int __CLASS_NAME__::get_node_state (acs_apbm::NodeStateConstants & node_state, unsigned retries) {
	int call_result = -1;
	int return_code = acs_apbm::ERR_NO_ERRORS;
	ACS_PRC_API prc_api;

	node_state = acs_apbm::NODE_STATE_UNDEFINED; // Pessimistic

	for (unsigned i = 0; (i <= retries) && (call_result < 0); ++i) {
		call_result = prc_api.askForNodeState();
		if (call_result < 0) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'askForNodeState' failed! loop == %u, call_result == %d", i, call_result);
			::sleep(acs_apbm_programconfiguration::get_node_state_delay);
		}
		else if (call_result == ACS_PRC_NODE_ACTIVE) node_state = acs_apbm::NODE_STATE_ACTIVE;
		else if (call_result == ACS_PRC_NODE_PASSIVE) node_state = acs_apbm::NODE_STATE_PASSIVE;
	}

	if (call_result < 0) {
		return_code = acs_apbm::ERR_GETTING_NODE_STATE;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Too many failures trying to get the local node state. retries == %u", retries);
	}

	return return_code;
}

int __CLASS_NAME__::operation_send (acs_apbm::ServerWorkOperationCodeConstants op_) {
	uint8_t op = static_cast<uint8_t>(op_);

	ACS_APBM_CRITICAL_SECTION_GUARD_BEGIN(_pipe_write_sync);

	if (::write(operation_pipe[1], &op, 1) < 0) { // ERROR: writing on the amf pipe
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call '::write' failed: cannot send the operation code '%d' to the AMF pipe event handler", op_);
		return acs_apbm::ERR_OPERATION_SEND_FAILURE;
	}

	ACS_APBM_CRITICAL_SECTION_GUARD_END();

	return acs_apbm::ERR_NO_ERRORS;
}

//int __CLASS_NAME__::operation_send (acs_apbm::ServerWorkOperationCodeConstants op_, const uint32_t & param) {
int __CLASS_NAME__::operation_send (acs_apbm::ServerWorkOperationCodeConstants op_, uint32_t param) {
	uint8_t op = static_cast<uint8_t>(op_);

	// Prepare the buffer
	uint8_t buffer [sizeof(uint32_t) + 1] = {op, 0};
	::memcpy(buffer + 1, &param, sizeof(uint32_t));

	ACS_APBM_CRITICAL_SECTION_GUARD_BEGIN(_pipe_write_sync);

	if (::write(operation_pipe[1], buffer, ACS_APBM_ARRAY_SIZE(buffer)) < 0) { // ERROR: writing on the amf pipe
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR,
				"Call '::write' failed: cannot send the operation code '%d' with param == 0x%X to the AMF pipe event handler", op_, param);
		return acs_apbm::ERR_OPERATION_SEND_FAILURE;
	}

	ACS_APBM_CRITICAL_SECTION_GUARD_END();

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::operation_send (acs_apbm::ServerWorkOperationCodeConstants op_, int32_t par_1, uint32_t par_2) {
	uint8_t op = static_cast<uint8_t>(op_);

	// Prepare the buffer
	uint8_t buffer [sizeof(int32_t) + sizeof(uint32_t) + 1] = {op, 0};
	::memcpy(buffer + 1, &par_1, sizeof(int32_t));
	::memcpy(buffer + 1 + sizeof(int32_t), &par_2, sizeof(uint32_t));

	ACS_APBM_CRITICAL_SECTION_GUARD_BEGIN(_pipe_write_sync);

	if (::write(operation_pipe[1], buffer, ACS_APBM_ARRAY_SIZE(buffer)) < 0) { // ERROR: writing on the amf pipe
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR,
				"Call '::write' failed: cannot send the operation code '%d' (par_1 == %d, par_2 == 0x%08X) "
				"to the AMF pipe event handler", op_, par_1, par_2);
		return acs_apbm::ERR_OPERATION_SEND_FAILURE;
	}

	ACS_APBM_CRITICAL_SECTION_GUARD_END();

	return acs_apbm::ERR_NO_ERRORS;
}

void __CLASS_NAME__::set_object_pointers (
		acs_apbm_cmdoptionparser * cmdoption_parser_,
		acs_apbm_haappmanager * ha_app_manager_,
		acs_apbm_operationpipeeventhandler * operation_pipe_event_handler_,
		ACE_Reactor * main_reactor_,
		acs_apbm_procsignalseventhandler * proc_signals_event_handler_,
		acs_apbm_immrepositoryhandler * imm_repository_handler_,
		/*acs_apbm_snmpsessionhandler * snmp_traps_acceptor_,*/
		acs_apbm_boardserviceacceptor * board_service_acceptor_api_unix_,
		acs_apbm_boardserviceacceptor * board_service_acceptor_api_inet_,
		/*acs_apbm_snmptrapeventhandler * snmp_trap_message_handler_,*/
		acs_apbm_snmpmanager * snmp_manager_,
		acs_apbm_csreader * cs_reader_,
		acs_apbm_trapsubscriptionmanager * trap_subscription_manager_,
		acs_apbm_shelvesdatamanager * shelves_data_manager_,
		acs_apbm_sanotifiereventhandler * sa_notifier_event_handler_,
		ACS_APBM_HWIHandler * hwi_handler_,
		acs_apbm_monitoringservicehandler * monitoring_service_event_handler_,
		acs_apbm_operationpipescheduler * operation_pipe_scheduler_,
		acs_apbm_snmptrapdsobserver * snmp_trapds_observer_,
		acs_apbm_cshwctablechangeobserver * cs_hwc_table_change_observer_,
		acs_apbm_alarmeventhandler * alarmevent_handler_,
		ACE_Task_Base * server_work_task_,
		acs_apbm_scxlagimmhandler * imm_lag_handler_) {
	cmdoption_parser = cmdoption_parser_;
	ha_app_manager = ha_app_manager_;
	operation_pipe_event_handler = operation_pipe_event_handler_;
	main_reactor = main_reactor_;
	proc_signals_event_handler = proc_signals_event_handler_;
	imm_repository_handler = imm_repository_handler_;
//	snmp_traps_acceptor = snmp_traps_acceptor_;
	board_service_acceptor_api_unix = board_service_acceptor_api_unix_;
	board_service_acceptor_api_inet = board_service_acceptor_api_inet_;
//	snmp_trap_message_handler = snmp_trap_message_handler_;
	snmp_manager = snmp_manager_;
	cs_reader = cs_reader_;
	trap_subscription_manager = trap_subscription_manager_;
	shelves_data_manager = shelves_data_manager_;
	sa_notifier_event_handler = sa_notifier_event_handler_;
	hwi_handler = hwi_handler_;
	monitoring_service_event_handler = monitoring_service_event_handler_;
	operation_pipe_scheduler = operation_pipe_scheduler_;
	snmp_trapds_observer = snmp_trapds_observer_;
	cs_hwc_table_change_observer = cs_hwc_table_change_observer_;
	alarmevent_handler = alarmevent_handler_;
	server_work_task = server_work_task_;
	imm_lag_handler = imm_lag_handler_;
}
