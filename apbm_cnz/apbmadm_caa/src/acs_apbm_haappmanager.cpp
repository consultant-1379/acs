#include <syslog.h>

#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_programconstants.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_haappmanager.h"

ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToActiveJobs (ACS_APGCC_AMF_HA_StateT previous_state) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performStateTransitionToActiveJobs invoked");

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_ACTIVE) return ACS_APGCC_SUCCESS;

	// Starting working depending on the node state
	send_start_work_operation_by_node_state();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToPassiveJobs (ACS_APGCC_AMF_HA_StateT previous_state) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performStateTransitionToPassiveJobs invoked");

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_STANDBY) return ACS_APGCC_SUCCESS;

	// Starting working depending on the node state
	send_start_work_operation_by_node_state();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToQueisingJobs (ACS_APGCC_AMF_HA_StateT /*previous_state*/) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performStateTransitionToQueisingJobs invoked");

/* NOTHING TO DO
	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_QUIESCING) return ACS_APGCC_SUCCESS;

	if (previous_state != ACS_APGCC_AMF_HA_UNDEFINED) {
		// Performing the following possible state transitions
		//   ACTIVE ------> QUIESCING

		ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,
				"Service Unit State transition happened (from state %d to state %d): APBM will ignore this HA signal because it is a No-Redundancy "
				"application linked to node state by NTF", previous_state, ACS_APGCC_AMF_HA_QUIESCING);

		return ACS_APGCC_SUCCESS;
	}

	// Service Unit first instantiation: starting working depending on the node state
	send_start_work_operation_by_node_state();
*/

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToQuiescedJobs (ACS_APGCC_AMF_HA_StateT /*previous_state*/) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performStateTransitionToQuiescedJobs invoked");

/* NOTHING TO DO: We'll go quiesced on "RemoveJobs" signal
	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_QUIESCED) return ACS_APGCC_SUCCESS;

	if (previous_state != ACS_APGCC_AMF_HA_UNDEFINED) {
		// Performing the following possible state transitions
		//   ACTIVE --------> QUIESCED
		//   QUIESCING -----> QUIESCED

		ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,
				"Service Unit State transition happened (from state %d to state %d): APBM will ignore this HA signal because it is a No-Redundancy "
				"application linked to node state by NTF", previous_state, ACS_APGCC_AMF_HA_QUIESCED);

		return ACS_APGCC_SUCCESS;
	}

	// Service Unit first instantiation: starting working depending on the node state
	send_start_work_operation_by_node_state();
*/

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performComponentHealthCheck () {
	//ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performStateTransitionToQuiescedJobs invoked");

	// Nothing to do fpr now

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performComponentTerminateJobs () {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performComponentTerminateJobs invoked: stopping APBM server!");
	ACS_APGCC_ReturnType retCode = ACS_APGCC_SUCCESS;
	bool done = false;
	int counter = 0;
	send_stop_work_operation_to_exit_program();
	do
	{																		//start of TR HX65505
		if (!_server_working_set->main_reactor->reactor_event_loop_done())
		{
			if(counter < 20)
			{
				counter++;
				ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "APBM main_reactor is still running! ");
				ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM main_reactor is still running! Sleeping 1 sec before main_reactor status is checked again!");
				sleep(1);
				continue;
			}

			ACS_APBM_LOG(LOG_LEVEL_ERROR, "APBM main_reactor still running but COUNT == 20! Ending the main_reactor event loop from performComponentTerminateJobs()!");
			ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,"APBM main_reactor still running but COUNT == 20! Ending the main_reactor event loop from performComponentTerminateJobs()!");
			// Reactor is not closed even after sending stop_exit work operation to operation_pipe_event_handler
			// So end the reactor event loop and return success. reactor will be closed forcefully from serverwork.work() once HA activate() returns
			if (_server_working_set->main_reactor->end_reactor_event_loop())
			{
				retCode = ACS_APGCC_FAILURE;
				ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,"end_reactor_event_loop failed!");
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'end_reactor_event_loop' failed: trying to end the main reactor event loop upon terminate jobs callback!");
			}
			else
			{
				ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,"end_reactor_event_loop successful!");
				ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'end_reactor_event_loop' successful - main_reactor closed!");
			}
			done = true;
			break;
		}
		else
		{
			ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,"'reactor_event_loop_done()' is TRUE!");
			ACS_APBM_LOG(LOG_LEVEL_WARN, "'reactor_event_loop_done()' is TRUE - main_reactor is already closed!");
			done = true;
			break;
		}
	} while (!done && (counter <= 20));										//end of TR HX65505

	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Terminate jobs completed!");
	return retCode;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performComponentRemoveJobs () {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performComponentRemoveJobs invoked: APBM server will be quiesced");

	send_start_work_quiesced_operation();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performApplicationShutdownJobs () {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback performApplicationShutdownJobs invoked: stopping APBM server");

	send_stop_work_operation_to_exit_program();

	return ACS_APGCC_SUCCESS;
}

void __CLASS_NAME__::send_start_work_operation_by_node_state () {
	// Retrieving my node state
	acs_apbm::NodeStateConstants node_state = acs_apbm::NODE_STATE_UNDEFINED;
	if (const int call_result = _server_working_set->get_node_state(node_state, acs_apbm_programconfiguration::get_node_state_retries))
		ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "Call 'get_node_state' failed: retrieving my node state: call_result == %d", call_result);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "My node state is '%s'", _server_working_set->node_state_str(node_state));

	// Dispatch a working operation signal depending on the node state: at least one thread in the
	// main reactor will take care of the request.
	acs_apbm::ServerWorkOperationCodeConstants op;
	switch (node_state) {
	case acs_apbm::NODE_STATE_ACTIVE: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_ACTIVE; break;
	case acs_apbm::NODE_STATE_PASSIVE: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_PASSIVE; break;
	default: op = acs_apbm::SERVER_WORK_OPERATION_START_WORK_NODE_UNDEFINED;
	}

	if (const int call_result = _server_working_set->operation_send(op)) {
		ACS_APBM_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR, "Call 'operation_send' failed: trying to send the operation code '%d' to activate the working "
				"job depending upon the node state '%s': call_result == %d", op, _server_working_set->node_state_str(node_state), call_result);
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "sent SERVER_WORK_OPERATION_START_WORK_NODE_'status' ");
}

void __CLASS_NAME__::send_stop_work_operation_to_exit_program () {
	// sending the STOP_WORK_TO_EXIT_PROGRAM operation to the operation handler
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_EXIT_PROGRAM)) { // ERROR: sending the operation
		ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Sending send_stop_work_operation_to_exit_program failed!! ");
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'operation_send' failed: cannot send the STOP_WORK_TO_EXIT_PROGRAM operation to the server work object");

		// Signal the program termination
		_server_working_set->program_state = acs_apbm::PROGRAM_STATE_EXIT_PROGRAM;

		// In case of error I will force ending the reactor event loop
		if (!_server_working_set->main_reactor->reactor_event_loop_done()) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ending the main reactor event loop...");
			if (_server_working_set->main_reactor->end_reactor_event_loop())
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "call 'end_reactor_event_loop' failed: trying to end the main reactor event loop upon termination signal received");
		}
	}
}

void __CLASS_NAME__::send_start_work_quiesced_operation () {
	// sending the STOP_WORK_TO_CHANGE_STATE_TO_QUIESCED operation to the operation handler
	if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_QUIESCED)) { // ERROR: sending the operation
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'operation_send' failed: cannot send the STOP_WORK_TO_CHANGE_STATE_TO_QUIESCED operation to the server work object");
	}
}
