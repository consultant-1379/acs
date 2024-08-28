#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <syslog.h>

#include "acs_apbm_programconstants.h"
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_haappmanager.h"

#include "acs_apbm_serverwork.h"


#define EXIT_PROGRAM_COND(var) ((var) == acs_apbm::PROGRAM_STATE_EXIT_PROGRAM)
#define RESTART_FUNCTION_COND(var) ((var) == acs_apbm::PROGRAM_STATE_RESTART_FUNCTION)
#define BREAK_LOOP_ON(cond) if ((cond)) break
#define BREAK_LOOP_ON_LOG(cond, level) \
	if ((cond)) { \
		ACS_APBM_LOG(level, "Breaking loop on [" ACS_APBM_STRINGIZE(cond) "] condition"); \
		break; \
	}

int __CLASS_NAME__::work () {
	int noha = _cmdoption_parser.noha();
	int call_result = acs_apbm::ERR_NO_ERRORS;

	// Check multiple program instance running but ony in no HA mode
	if (noha && (call_result = multiple_process_instance_running_check())) return call_result;

	pid_t parent_pid = ::getpid();
	acs_apbm_haappmanager ha_application_manager(acs_apbm_programconfiguration::ha_daemon_name, &_server_working_set);

	// Initialize the server logger
	acs_apbm_logger::open(acs_apbm_programconfiguration::logger_appender_name);

	pid_t child_pid = ::getpid();

	if (child_pid != parent_pid) { // OK: APBM server successfully demonized
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server successfuly demonized: new child PID == %d", child_pid);
	} else { // ERROR: APBM server was not demonized
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"APBM server was not demonized correctly: child process was aborted on creation: the parent process (PID == %d) continues taking the control",
				parent_pid);
	}

	acs_apbm_serverworkobjects working_objects(&_server_working_set);

	int env;
	working_objects.cs_reader.get_environment(env);
	acs_apbm_snmpmanager* shelf_manager = 0;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "get environment %d", env);
	if (env == ACS_CS_API_CommonBasedArchitecture::DMX)
	{
		shelf_manager = &working_objects.ironside_manager;
	}
	else
	{
		shelf_manager = &working_objects.snmp_manager;
	}

	//Fill up my working set
	_server_working_set.set_object_pointers(&_cmdoption_parser, &ha_application_manager,
			&working_objects.operation_pipe_event_handler, &working_objects.main_reactor, &working_objects.proc_signals_event_handler,
			&working_objects.imm_repository_handler, /*&working_objects.snmp_traps_acceptor,*/
			&working_objects.board_service_acceptor_api_unix, &working_objects.board_service_acceptor_api_inet, /*&working_objects.snmp_trap_message_handler,*/
			shelf_manager, &working_objects.cs_reader, &working_objects.trap_subscription_manager, &working_objects.shelves_data_manager,
			&working_objects.sa_notifier_event_handler, &working_objects.hwi_handler, &working_objects.monitoring_service_event_handler,
			&working_objects.operation_pipe_scheduler, &working_objects.snmp_trapds_observer,
			&working_objects.cs_hwc_table_change_observer, &working_objects.alarmevent_handler, this,&working_objects.imm_lag_handler);

	if (env == ACS_CS_API_CommonBasedArchitecture::DMX)
	{
		working_objects.ironside_manager.initialize_dmx_boardinfo();
	}

	// Write into the log the system APBM server is running on
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server is running on a%s CBA system", working_objects.cs_reader.is_cba_system() ? "" : " not");

	// Initializing operation pipe and its event handler. Enter in a loop because APBM
	// server cannot work without an operation pipe and its reactor event handler
	while (_server_working_set.init_operation_pipe_and_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'init_operation_pipe_and_handler' failed: Operation pipe and event handler initialization failure");
		::sleep(acs_apbm_programconfiguration::init_operation_pipe_and_handler_delay);
	}

	//Starting the operation pipe scheduler
	if (_server_working_set.start_operation_pipe_scheduler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'start_operation_pipe_scheduler' failed: operation pipe scheduler not started: APBM server cannot "
				"reschedule operations in a timeout fashion");
	}

	// Initializing the process signals handler
	int init_signals_handler_call_result = _server_working_set.init_signals_handler();
	if (init_signals_handler_call_result)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'init_signals_handler' failed: process signal handler initialization failure: APBM server will retry asap to reinitialize signals handler");





/* #####  ###  ###   ###  */
/*   #   #   # #  # #   # */
/*   #   #   # #  # #   # */
/*   #   #   # #  # #   # */
/*   #    ###  ###   ###  */
/* Il socket acceptor di tipo UNIX va attivato mediante operazione ed attivato solo quando ABM server
 * sta lavorando in modalita` ATTIVA o PASSIVA ma non in modalita` QUIESCED */
//	// Open the API board management service acceptor UNIX object
//	int open_api_service_acceptor_unix_call_result = _server_working_set.open_api_service_acceptor_unix();
//	if (open_api_service_acceptor_unix_call_result)
//		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_api_service_acceptor_unix' failed: cannot open the API service acceptor UNIX to handle client connections on UNIX sap: APBM server will retry asap to reopen the service");






	// TODO: For some reasons the opening of service acceptor inet do not work fine on the passive node under HA.
	// So we start it throught the operation pipe later.
	// Uncomment the following lines when the problem is solved

//	// Open the API board management service acceptor INET object
//	int open_api_service_acceptor_inet_call_result = _server_working_set.open_api_service_acceptor_inet();
//	if (open_api_service_acceptor_inet_call_result)
//		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_api_service_acceptor_inet' failed: cannot open the API service acceptor INET to handle client connections on INET sap: APBM server will retry asap to reopen the service");


	// Add here other initialization as needed, before activate thread-pool


	// Activate this server work task with the minimum number of thread running
	int activate_call_result = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, acs_apbm_programconfiguration::working_passive_thread_count, 1);
	if (activate_call_result == -1) // ERROR: activating this main task
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'activate' failed: cannot activate the main worker task: the server will try to reactivate this worker task asap");
	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main worker task successfully activated: number of threads running in this task object == %zu", thr_count());

	// If process signals handler initialization failed then try to put an INIT_SIGNALS_HANDLER operation on the operation pipe
	if (init_signals_handler_call_result && _server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_INIT_SIGNALS_HANDLER))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an INIT_SIGNALS_HANDLER operation to re-initialize the process signals handler");

	// If server work task activation failed then try to put an ACTIVATE_SERVER_WORK_TASK
	// operation on the operation pipe to reactivate the task
	if ((activate_call_result == -1) && _server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_ACTIVATE_SERVER_WORK_TASK))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an ACTIVATE_SERVER_WORK_TASK to re-activate the server work task");


/* BEGIN: TO DELETE */
//	// If the API service acceptor UNIX not open successfully then try to reopen it using the operation pipe
//	if (open_api_service_acceptor_unix_call_result && _server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_UNIX_OPEN))
//		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_UNIX_OPEN operation to reopen listening API service for clients throught APBM API");
/* END: TO DELETE */





/* #####  ###  ###   ###  */
/*   #   #   # #  # #   # */
/*   #   #   # #  # #   # */
/*   #   #   # #  # #   # */
/*   #    ###  ###   ###  */
/* Spostare l'attivazione dell'operazione seguente in quella relativa all'attivazione dell'attivazione
 * del working in modalita` ATTIVA o PASSIVA. L'apertura della socket INET va fatta solo quando il
 * server APBM lavora in modalita` ATTIVA o PASSIVA e va chiusa quando si passa` in modalita` QUIESCED */
//	// TODO: For some reasons the opening of service acceptor inet do not work fine on the passive node under HA.
//	// So we start it throught the operation pipe later.
//	// Her we call the operation pipe unconditionally
//	_server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_INET_OPEN);





	// Before starting working upon the node state the server will subscribe itself
	// to the NTF service to receive notifications about node state changes
	// As for the operation pipe, enter in a loop because APBM server cannot
	// work without node state change notifications from NTF service
	while (_server_working_set.init_sa_notifier_manager_and_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'init_sa_notifier_manager_and_handler' failed: SA notifier manager and event handler initialization failure");
		::sleep(acs_apbm_programconfiguration::init_sa_notifier_and_handler_delay);
	}

	if (noha) { // --noha option provided: APBM server is running in no-HA mode
		// Send a START_WORK_* operation to start working depending on the node state
		send_start_work_operation_by_node_state();

		// Also this main thread will enter into the reactor event loop
		enter_reactor_event_loop(working_objects.main_reactor);
	} else { // APBM server was started by CoreMW cluster: HA mode active
		// I'm in HA mode so I call the ApplicationManager activate to bind to the AMF framework
                //HA mode comment 
//	        acs_apbm_haappmanager ha_application_manager(acs_apbm_programconfiguration::ha_daemon_name, &_server_working_set);
		ACS_APGCC_HA_ReturnType ha_call_result = ha_application_manager.activate();

		switch (ha_call_result) {
		case ACS_APGCC_HA_SUCCESS:
			ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "HA Application Gracefully closing...");
			break;
		case ACS_APGCC_HA_FAILURE:
			ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "HA Activation Failed: ha_call_result == %d", ha_call_result);
			break;
		case ACS_APGCC_HA_FAILURE_CLOSE:
			ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "HA Application Failed to Gracefully closing: ha_call_result == %d", ha_call_result);
			break;
		default:
			ACS_APBM_SYSLOG(LOG_WARNING, LOG_LEVEL_WARN, "HA Application error code unknown: ha_call_result == %d", ha_call_result);
			break;
		}

		if (ha_call_result != ACS_APGCC_HA_SUCCESS) { // ERROR:
			// sending the STOP_WORK_TO_EXIT_PROGRAM operation to the operation handler
			// this will terminate also the ractor event loop
			if (_server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_EXIT_PROGRAM)) { // ERROR: sending the operation
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Signal handler: call 'operation_send' failed: cannot send the STOP_WORK_TO_EXIT_PROGRAM operation to the server work object");

				// Signal the program termination
				_server_working_set.program_state = acs_apbm::PROGRAM_STATE_EXIT_PROGRAM;

				// In case of error I will force ending the reactor event loop
				if (!reactor()->reactor_event_loop_done()) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Signal handler: ending the main reactor event loop...");
					if (reactor()->end_reactor_event_loop())
						ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Signal handler: call 'end_reactor_event_loop' failed: trying to end the main reactor event loop upon termination signal received");
				}
			}
		}
	}


	// NOTE: Here the main thread is out of the main reactor or AMF engine and so the operation pipe shouldn't be used anymore

	// Handling termination and program exit. This main thread will wait all other thread in this main task
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main thread: waiting for task's threads exit...");
	if (wait()) { // ERROR: waiting other task's threads
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'wait' failed: main thread cannot wait other task's threads");
	}

	// Resetting and closing the SA notifier manager and event handler
	if (_server_working_set.reset_sa_notifier_manager_and_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'reset_sa_notifier_manager_and_handler' failed: resetting SA notifier manager and its event handler");
	}




/* #####  ###  ###   ###  */
/*   #   #   # #  # #   # */
/*   #   #   # #  # #   # */
/*   #   #   # #  # #   # */
/*   #    ###  ###   ###  */
/* La chiusura delle socket di comunicazione deve avvenire, mediante le operazioni, nell'operazione di chiusura
 * e di uscita dal programma.
 * TOGLIERLE DA QUI */
/*
	// Closing the API service acceptor INET
	if (_server_working_set.close_api_service_acceptor_inet())
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'close_api_service_acceptor_inet' failed: cannot close the API service acceptor INET");

	// Closing the API service acceptor UNIX
	if (_server_working_set.close_api_service_acceptor_unix())
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'close_api_service_acceptor_unix' failed: cannot close the API service acceptor UNIX");
*/

	// Resetting and closing all working objects
	if (_server_working_set.reset_signals_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'reset_signals_handler' failed: resetting signals handler");
	}

	if (_server_working_set.stop_operation_pipe_scheduler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'stop_operation_pipe_scheduler' failed: stopping the operation pipe scheduler");
	}


	if (_server_working_set.reset_operation_pipe_and_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'reset_operation_pipe_and_handler' failed: resetting operation pipe and its handler");
	}

	// Delete lock file
	if(::unlink(acs_apbm_programconfiguration::apbm_server_lock_file_path) < 0)
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call '::unlink' failed: main thread failed to delete the APBM server lock file '%s'", acs_apbm_programconfiguration::apbm_server_lock_file_path);

	working_objects.main_reactor.close();

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::svc () {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Task thread started running svc function: threads in the main task == %zu", this->thr_count());

	// Check the exit condition. If it's false this task thread will enter into the reactor loop
	for (;;) {
		BREAK_LOOP_ON_LOG(EXIT_PROGRAM_COND(_server_working_set.program_state), LOG_LEVEL_DEBUG);


		// TODO: Quando il task entra nel reactor event loop (chiama la funzione run_reactor_event_loop())
		// applicare la tecnica del timeout per controllare il decremento del numero dei thread running
		// nel main task base quando si passa a lavorare nello stato passivo).


		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Task thread: entering into the main reactor event loop");
		// Letting this task thread to run the main reactor event loop. This for loop is to prevent errors
		if (_server_working_set.main_reactor->run_reactor_event_loop()) { // ERROR: entering into the main reactor event loop
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'run_reactor_event_loop' failed: task thread failed to enter into the main reactor event loop");
			::sleep(acs_apbm_programconfiguration::task_thread_enter_reactor_loop_delay);
		} else {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Task thread: out from the main reactor event loop");

			// TODO:
			// WARNING WARNING WARNING WARNING WARNING WARNING WARNING
			// Per adesso usciamo inesorabilmente: ma qui si dovrebbe controllare la condizione di uscita dipendente dallo stato
			// del nodo in cui ci si trova a lavorare. Si dovrebbe ciclare di nuovo (unica condizione di uscita e` EXIT_PROGRAM
			// ma anche nel caso in cui si deve abbassare il numero dei thread quando ci si trova a lavorare nello stato PASSIVE)
			break; // OK: task thread has ran into the main reactor event loop and now is exiting to stop working
		}
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Task thread exiting from svc function");

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::multiple_process_instance_running_check () {
	// Multiple server instance check: if there is another APBM server instance
	// already running then exit immediately

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Checking APBM server multiple instance running...");
	int lock_fd = ::open(acs_apbm_programconfiguration::apbm_server_lock_file_path, O_CREAT | O_WRONLY | O_APPEND, 0664);
	if (lock_fd < 0) {
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call '::open' failed");
		return acs_apbm::PROGRAM_EXIT_LOCK_FILE_OPEN_ERROR;
	}
	errno = 0;
	if (::flock(lock_fd, LOCK_EX | LOCK_NB) < 0) {
		int errno_save = errno;
		if (errno_save == EWOULDBLOCK) {
			::fprintf(::stderr, "Another APBM Server instance running\n");
			ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Another APBM Server instance running found");
			return acs_apbm::PROGRAM_EXIT_ANOTHER_SERVER_RUNNING;
		}

		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call '::flock' failed");
		return acs_apbm::PROGRAM_EXIT_LOCK_FILE_LOCKING_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No APBM server multiple instance running");

	return acs_apbm::ERR_NO_ERRORS;
}

void __CLASS_NAME__::send_start_work_operation_by_node_state () {
	// The following loop is to be sure an operation command is sent to the Operation pipe.
	// It's controlled by a program_exit or restart_function condition using the PROGRAM_STATE enumeration.
	for (;;) {
		// First breaking loop check
		BREAK_LOOP_ON_LOG(EXIT_PROGRAM_COND(_server_working_set.program_state) || RESTART_FUNCTION_COND(_server_working_set.program_state), LOG_LEVEL_DEBUG);

		// Retrieving my node state
		acs_apbm::NodeStateConstants node_state = acs_apbm::NODE_STATE_UNDEFINED;
		if (_server_working_set.get_node_state(node_state, acs_apbm_programconfiguration::get_node_state_retries))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_node_state' failed: retrieving my node state");

		ACS_APBM_LOG(LOG_LEVEL_INFO, "My node state is '%s'", _server_working_set.node_state_str(node_state));

		// Dispatch a working operation signal depending on the node state: at least one thread in the
		// main reactor will take care of the request.
		acs_apbm::ServerWorkOperationCodeConstants op;
		switch (node_state) {
		case acs_apbm::NODE_STATE_ACTIVE: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_ACTIVE; break;
		case acs_apbm::NODE_STATE_PASSIVE: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_PASSIVE; break;
		default: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_UNDEFINED;
		}

		// Another breaking loop check
		BREAK_LOOP_ON_LOG(EXIT_PROGRAM_COND(_server_working_set.program_state) || RESTART_FUNCTION_COND(_server_working_set.program_state), LOG_LEVEL_DEBUG);

		if (_server_working_set.operation_send(op)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: trying to send the operation code '%d' to activate the working job depending upon the node state '%s'", op, _server_working_set.node_state_str(node_state));
			::sleep(acs_apbm_programconfiguration::send_start_work_operation_on_startup_delay);
		} else break; // All right, exit loop and continue
	}
}

void __CLASS_NAME__::enter_reactor_event_loop (ACE_Reactor & reactor) {
	// Check the exit condition. If it's false this main thread also will enter into the reactor loop
//	while (_server_working_set.program_state ^ acs_apbm::PROGRAM_STATE_EXIT_PROGRAM) {
	for (;;) {
		BREAK_LOOP_ON_LOG(EXIT_PROGRAM_COND(_server_working_set.program_state) || RESTART_FUNCTION_COND(_server_working_set.program_state), LOG_LEVEL_DEBUG);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main thread: entering into the main reactor event loop");

		// Letting this main thread to run the main reactor event loop.
		if (reactor.run_reactor_event_loop()) { // ERROR: entering into the main reactor event loop
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'run_reactor_event_loop' failed: main thread failed to enter into the main reactor event loop");
			::sleep(acs_apbm_programconfiguration::main_thread_enter_reactor_loop_delay);
		} else {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main thread: out from the main reactor event loop");
			break; // OK: main thread has ran into the main reactor event loop and now is exiting
		}
	}
}



#if 0 // TODO: BEGIN: OLD CODE: To be deleted
int __CLASS_NAME__::work_noha () {
	// Check multiple program instance running
	if (int call_result = multiple_process_instance_running_check()) return call_result;


	// TODO:
	// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
	// All should be into a loop

	acs_apbm_serverworkobjects working_objects(&_server_working_set);

	//Fill up my working set
	_server_working_set.set_object_pointers(&_cmdoption_parser, &working_objects.operation_pipe_event_handler, &working_objects.main_reactor,
			&working_objects.proc_signals_event_handler, &working_objects.imm_repository_handler, &working_objects.snmp_traps_acceptor,
			&working_objects.board_service_acceptor_api, &working_objects.snmp_trap_message_handler, &working_objects.snmp_manager,
			&working_objects.cs_reader, &working_objects.trap_subscription_manager, &working_objects.shelves_data_manager, this);


	// TODO:
	// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
	// The loop could be start here keeping out the working_objects object.


	// Initializing operation pipe and its event handler. Enter in a loop because APBM
	// server cannot work without an operation pipe and its reactor event handler
	while (_server_working_set.init_operation_pipe_and_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'init_operation_pipe_and_handler' failed: Operation pipe and event handler initialization failure");
		::sleep(acs_apbm_programconfiguration::init_operation_pipe_and_handler_delay);
	}

	// Initializing the process signals handler
	int init_signals_handler_call_result = _server_working_set.init_signals_handler();
	if (init_signals_handler_call_result)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'init_signals_handler' failed: process signal handler initialization failure: APBM server will retry asap to reinitialize signals handler");

	// Open the API board management service acceptor object
	int open_api_service_acceptor_call_result = _server_working_set.open_api_service_acceptor();
	if (open_api_service_acceptor_call_result)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_api_service_acceptor' failed: cannot open the API service acceptor to handle client connections: APBM server will retry asap to reopen the service");







	// Activate this server work task with the minimum number of thread running
	int activate_call_result = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, acs_apbm_programconfiguration::working_passive_thread_count, 1);
	if (activate_call_result == -1) // ERROR: activating this main task
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'activate' failed: cannot activate the main worker task: the server will try to reactivate this worker task asap");
	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main worker task successfully activated: number of threads running in this task object == %zu", thr_count());

	// If process signals handler initialization failed then try to put an INIT_SIGNALS_HANDLER operation on the operation pipe
	if (init_signals_handler_call_result && _server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_INIT_SIGNALS_HANDLER))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an INIT_SIGNALS_HANDLER operation to re-initialize the process signals handler");

	// If server work task activation failed then try to put an ACTIVATE_SERVER_WORK_TASK
	// operation on the operation pipe to reactivate the task
	if ((activate_call_result == -1) && _server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_ACTIVATE_SERVER_WORK_TASK))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an ACTIVATE_SERVER_WORK_TASK to re-activate the server work task");

	// If the API service acceptor not open successfully then try to reopen it using the operation pipe
	if (open_api_service_acceptor_call_result && _server_working_set.operation_send(acs_apbm::SERVER_WORK_OPERATION_API_SERVICE_ACCEPTOR_OPEN))
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot order an API_SERVICE_ACCEPTOR_OPEN operation to reopen listening API service for clients throught APBM API");

	// Send a START_WORK_* operation to start working depending on the node state
	send_start_work_operation_by_node_state();

	// Also this main thread will enter into the reactor event loop
	enter_reactor_event_loop(working_objects.main_reactor);


	// NOTE: Here the main thread is out of the main reactor and so the operation pipe shouldn't be used anymore


	// Handling termination and program exit. This main thread will wait all other thread in this main task
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main thread: waiting for task's threads exit...");
	if (wait()) { // ERROR: waiting other task's threads
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'wait' failed: main thread cannot wait other task's threads");
	}

	// Closing the API service acceptor
	if (_server_working_set.close_api_service_acceptor())
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'close_api_service_acceptor' failed: cannot close the API service acceptor to handle client connections");

	// Resetting and closing all working objects
	if (_server_working_set.reset_signals_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'reset_signals_handler' failed: resetting signals handler");
	}

	if (_server_working_set.reset_operation_pipe_and_handler()) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'reset_operation_pipe_and_handler' failed: resetting operation pipe and its handler");
	}

	// Delete lock file
	if(::unlink(acs_apbm_programconfiguration::apbm_server_lock_file_path) < 0)
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call '::unlink' failed: main thread failed to delete the APBM server lock file '%s'", acs_apbm_programconfiguration::apbm_server_lock_file_path);

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::work_ha () {
	/* TODO:
	 *		Aggiungere tutta la gestione dell'HA application manager per
	 *		la registrazione del processo in AMF e l'esecuzione APBM server work.
	 *		Vedere il DSD.
	 *		Per adesso chiamo il serverwork comunque direttamente, ma questa chiamata
	 *		va tolta.
	 */
	return work_noha(); //Va tolto una volta integrato con HA
}
#endif // TODO: END: OLD CODE: To be deleted
