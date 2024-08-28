//#define ACS_APBM_LOGGING_HAS_SOURCE_INFO 1

//#include "ACS_PRC_Process.h"
#include "acs_prc_api.h"

#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_macros.h"

#include "acs_apbm_haappmanager_t.h"
#include "acs_apbm_logger_t.h"
#include "acs_apbm_serverwork_t.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_serverwork


int __CLASS_NAME__::work () {
	pid_t parent_pid = ::getpid();

	acs_apbm_haappmanager ha_application_manager(this, acs_apbm_programconfiguration::ha_daemon_name);

	pid_t child_pid = ::getpid();

	if (child_pid != parent_pid) { // OK: APBM server successfully demonized
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server successfuly demonized: new child PID == %d", child_pid);
	} else { // ERROR: APBM server was not demonized
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"APBM server was not demonized correctly: child process was aborted on creation: the parent process (PID == %d) continues taking the control",
				parent_pid);
	}

//	// Write into the log the system APBM server is running on
//	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server is running on a%s CBA system", _cs_reader.is_cba_system() ? "" : " not");

	// Activate this server work task with the minimum number of thread running
	errno = 0;
	const int activate_call_result = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 6, 1);
	if (activate_call_result == -1) // ERROR: activating this main task
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'activate' failed: cannot activate the main worker task.");
	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main worker task successfully activated: number of threads running in this task object == %zu", thr_count());

	// I'm in HA mode so I call the ApplicationManager activate to bind to the AMF framework
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

	// NOTE: Here the main thread is out of the AMF engine
	// TO be sure a stop working signal is raised
	stop_working();

	// Handling termination and program exit. This main thread will wait all other thread in this main task
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Main thread: waiting for task's threads exit...");
	if (wait()) { // ERROR: waiting other task's threads
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'wait' failed: main thread cannot wait other task's threads");
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "OK: all working threads terminated");

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Exiting from work function: work done");

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::svc () {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Task thread started running svc function: threads in the main task == %zu", this->thr_count());

	for (unsigned loop_count = 0; !_exit_from_svc_function; loop_count++) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LOOP COUNT == %u. I'm working for this node state: %s", loop_count, get_node_state());

		for (int waiting_count = (::rand() % 10); !_exit_from_svc_function && waiting_count; --waiting_count) ::sleep(1);
	}


/*
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
*/

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Task thread exiting from svc function");

	return acs_apbm::ERR_NO_ERRORS;
}

const char * __CLASS_NAME__::get_node_state () const {
	ACS_PRC_API prc_api;

	const int call_result = prc_api.askForNodeState();
	if (call_result < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'askForNodeState' failed! call_result == %d", call_result);
		return "NODE_STATE_ERROR";
	}

	if (call_result == ACS_PRC_NODE_ACTIVE) return "NODE_STATE_ACTIVE";
	if (call_result == ACS_PRC_NODE_PASSIVE) return "NODE_STATE_PASSIVE";

	return "NODE_STATE_UNKNOWN";
}



/*
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
*/

/*
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
*/
