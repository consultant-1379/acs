#include "acs_apbm_logger_t.h"
#include "acs_apbm_haappmanager_t.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_haappmanager


ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToActiveJobs (ACS_APGCC_AMF_HA_StateT previous_state) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked", __func__);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "previous_state == %d", previous_state);

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_ACTIVE) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state == ACS_APGCC_AMF_HA_ACTIVE]");
		return ACS_APGCC_SUCCESS;
	}

	if (previous_state != ACS_APGCC_AMF_HA_UNDEFINED) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state != ACS_APGCC_AMF_HA_UNDEFINED]");

		// Performing the following possible state transitions
		//   QUIESCED ------> ACTIVE
		//   QUIESCING -----> ACTIVE

		ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,
				"Service Unit State transition happened (from state %d to state %d): APBM will ignore this HA signal because it is a No-Redundancy "
				"application linked to node state by NTF", previous_state, ACS_APGCC_AMF_HA_ACTIVE);

		return ACS_APGCC_SUCCESS;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SU first instantiation", previous_state);

//	// Service Unit first instantiation: starting working depending on the node state
//	send_start_work_operation_by_node_state();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToPassiveJobs (ACS_APGCC_AMF_HA_StateT previous_state) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked", __func__);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "previous_state == %d", previous_state);

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_STANDBY) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state == ACS_APGCC_AMF_HA_STANDBY]");
		return ACS_APGCC_SUCCESS;
	}

	if (previous_state != ACS_APGCC_AMF_HA_UNDEFINED) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state != ACS_APGCC_AMF_HA_UNDEFINED]");

		// Performing the following possible state transitions
		//   ACTIVE -----+
		//   QUIESCED ---+--> STANDBY
		//   QUIESCING --+

		ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,
				"Service Unit State transition happened (from state %d to state %d): APBM will ignore this HA signal because it is a No-Redundancy "
				"application linked to node state by NTF", previous_state, ACS_APGCC_AMF_HA_STANDBY);

		return ACS_APGCC_SUCCESS;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SU first instantiation", previous_state);

//	// Service Unit first instantiation: starting working depending on the node state
//	send_start_work_operation_by_node_state();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToQueisingJobs (ACS_APGCC_AMF_HA_StateT previous_state) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked", __func__);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "previous_state == %d", previous_state);

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_QUIESCING) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state == ACS_APGCC_AMF_HA_QUIESCING]");
		return ACS_APGCC_SUCCESS;
	}

	if (previous_state != ACS_APGCC_AMF_HA_UNDEFINED) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state != ACS_APGCC_AMF_HA_UNDEFINED]");

		// Performing the following possible state transitions
		//   ACTIVE ------> QUIESCING

		ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,
				"Service Unit State transition happened (from state %d to state %d): APBM will ignore this HA signal because it is a No-Redundancy "
				"application linked to node state by NTF", previous_state, ACS_APGCC_AMF_HA_QUIESCING);

		return ACS_APGCC_SUCCESS;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SU first instantiation", previous_state);

//	// Service Unit first instantiation: starting working depending on the node state
//	send_start_work_operation_by_node_state();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performStateTransitionToQuiescedJobs (ACS_APGCC_AMF_HA_StateT previous_state) {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked", __func__);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "previous_state == %d", previous_state);

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_QUIESCED) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state == ACS_APGCC_AMF_HA_QUIESCED]");
		return ACS_APGCC_SUCCESS;
	}

	if (previous_state != ACS_APGCC_AMF_HA_UNDEFINED) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASSERTION-MATCH-[previous_state != ACS_APGCC_AMF_HA_UNDEFINED]");

		// Performing the following possible state transitions
		//   ACTIVE --------> QUIESCED
		//   QUIESCING -----> QUIESCED

		ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,
				"Service Unit State transition happened (from state %d to state %d): APBM will ignore this HA signal because it is a No-Redundancy "
				"application linked to node state by NTF", previous_state, ACS_APGCC_AMF_HA_QUIESCED);

		return ACS_APGCC_SUCCESS;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SU first instantiation", previous_state);

//	// Service Unit first instantiation: starting working depending on the node state
//	send_start_work_operation_by_node_state();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performComponentHealthCheck () {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked", __func__);

	// Nothing to do for now

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performComponentTerminateJobs () {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked: stopping APBM server", __func__);

	send_stop_work_operation_to_exit_program();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performComponentRemoveJobs () {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked: stopping APBM server", __func__);

	send_stop_work_operation_to_exit_program();

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType __CLASS_NAME__::performApplicationShutdownJobs () {
	ACS_APBM_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "Callback '%s' invoked: stopping APBM server", __func__);

	send_stop_work_operation_to_exit_program();

	return ACS_APGCC_SUCCESS;
}

void __CLASS_NAME__::send_stop_work_operation_to_exit_program () {
	_srv_work_ptr->stop_working();
}

/*
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
*/
