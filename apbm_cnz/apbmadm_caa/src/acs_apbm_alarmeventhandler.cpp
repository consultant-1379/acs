#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_monitoringservicehandler.h"
#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_alarmeventhandler.h"
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_CLibTypes.h>
#include <acs_apbm_api.h>
#include <ACS_CS_API.h>

__CLASS_NAME__::~__CLASS_NAME__(){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entered ~acs_apbm_alarmeventhandler()");


	// start of TR HX65505
	/* Ceasing all alarms in list is moved to cease_all_alarms() - invoked when new pipe operation SERVER_WORK_OPERATION_CEASE_ALL_ALARMS_SHUTDOWN
	   is performed during service shutdown

	ACE_Guard<ACE_Thread_Mutex> guard_active_alarms(_activealarms_mutex);
	acs_apbm_alarminfo * alarm_to_cease = NULL;
	std::vector<acs_apbm_alarminfo*>::iterator iter;

	for (iter = _active_alarms.begin(); iter != _active_alarms.end(); ++iter){
		alarm_to_cease = *iter;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Found an alarm (alarm_type == %d, slot == %d, nic_num == %d) in _active_alarms list!", alarm_to_cease->get_alarm_type(), alarm_to_cease->get_slot(), alarm_to_cease->get_nic_num());
		ACS_AEH_ReturnType call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
				 alarm_to_cease->get_event_code(),EVENT_CEASING,AP_EVENT_CAUSE,EVENT_CLASS_REFERENCE, EVENT_OBJ_REFERENCE,"","");

		if ( call_result != ACS_AEH_ok ){
	                  int ret_code = ACS_APBM_AEH_SUCCESS;
			  _aeh_error_text =_alarmeventHandler.getErrorText();
			  ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm  not ceased! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
			  ret_code = ACS_APBM_AEH_ALARM_NOT_CEASED;
		}
		else
			  ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm successful ceased! ");
		  // Remove alarm
		  delete alarm_to_cease;
		  alarm_to_cease = NULL;
	}
	*/
	// end of TR HX65505
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Leaving ~acs_apbm_alarmeventhandler()");
}


int __CLASS_NAME__::raise_alarm(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type, int32_t slot_num1, int32_t slot_num2, int nic_num){
	int isCBA = _server_working_set->cs_reader->is_cba_system();
	int env;
    _server_working_set->cs_reader->get_environment(env); //SCB = 0, SCX = 1, DMX = 2, VIRTUALIZED = 3

    // check if the alarm type to be raised is applicable in the current system environment
	if(!is_alarm_applicable(alarm_type, env))
		return ACS_APBM_AEH_ALARM_NOT_APPLICABLE;

    acs_apbm_api api;
	switch (alarm_type){
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC:
			return raise_nic_alarm(slot_num1, nic_num);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_RAID:
			return raise_raid_alarm(slot_num1);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DRBD:
                        return raise_drbd_alarm(slot_num1);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM:
			//ACS_APBM_LOG(LOG_LEVEL_DEBUG,"NOT GEP4/GEP5!");
			return raise_boardremoved_alarm(slot_num1);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDFAULT:
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"BOARD FAULT ALARM ENTRY");
			return raise_boardfaulty_alarm(slot_num1);
		}
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SCBEVENT:
			return raise_scbrp_snmplinkdown_alarm(slot_num1);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN:
			 return (isCBA)? raise_scx_snmplinkdown_alarm (slot_num1): raise_scbrp_snmplinkdown_alarm(slot_num1);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN:
			if(env == ACS_CS_API_CommonBasedArchitecture::DMX)
				return raise_both_dmx_snmplinkdown_alarm();
			else if((env == ACS_CS_API_CommonBasedArchitecture::SCX) || (env == ACS_CS_API_CommonBasedArchitecture::SMX))
				return raise_both_scx_snmplinkdown_alarm();
			else
				return raise_both_scbrp_snmplinkdown_alarm(slot_num1, slot_num2);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISKCONN:
			return raise_disk_alarm(slot_num1);
                case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE:
                        return raise_disk_unavailable_alarm(slot_num1);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_THUMBDRIVE_MALFUNCTION: //Redesign as per TR-HS30773
                        {
                        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"THUMBDRIVE ALARM ENTRY");
                        return raise_thumbdrive_alarm(slot_num1);
                        }
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED:
			return raise_lag_disabled_alarm(slot_num1, nic_num);
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE:
			return raise_temperature_alarm(slot_num1,nic_num);
		default:
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Cannot raise alarm: Unknown alarm_type <alarm_type ==  %d> ", alarm_type);
			break;
	}
	return ACS_APBM_AEH_ALARM_NOT_RAISED;
}

int __CLASS_NAME__::raise_team_alarm(int32_t slot_num){
	ACS_AEH_ReturnType call_result;

		const char * alarm_problem_text  = EVENT_TEAM_PROBLEM_TEXT_HEAD EVENT_TEAM_PROBLEM_TEXT_TAIL;
		char team_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
		::snprintf(team_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %5d", alarm_problem_text, slot_num);

		// slot_num == 20 must be substituted with  a check on side. Now we assume that slot 20 is on side B.
		uint16_t side = 0;
       		_server_working_set->cs_reader->get_my_side(side);
		acs_aeh_specificProblem      event_code = (side == ACS_CS_API_HWC_NS::Side_B)? TEAM_NODEB_LINK_LOST_EVENT_CODE: TEAM_NODEA_LINK_LOST_EVENT_CODE;
		acs_aeh_percSeverity         event_severity = EVENT_NETWORK_TEAM_DOWN_SEVERITY;
		acs_aeh_probableCause        event_cause = TEAM_EVENT_CAUSE;
		acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
		acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
		acs_aeh_problemData          problem_data = EVENT_TEAM_PROBLEM_DATA;
		acs_aeh_problemText          problem_text = team_alarm_problem_text;

		call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
				event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

		if ( call_result != ACS_AEH_ok ){
			_aeh_error_text =_alarmeventHandler.getErrorText();
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'AP EXTERNAL NETWORK TEAM' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
			return ACS_APBM_AEH_ALARM_NOT_RAISED;
		}

		ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'AP EXTERNAL NETWORK TEAM' successful raised! ");
		int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_TEAM, slot_num, event_code);

		return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

int __CLASS_NAME__::raise_nic_alarm(int32_t slot_num, int nic_num){
	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_NIC_PROBLEM_TEXT_HEAD EVENT_NIC_PROBLEM_TEXT_TAIL;
	char nic_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	::snprintf(nic_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %5d %5d", alarm_problem_text, slot_num, nic_num);

	acs_aeh_specificProblem      event_code = get_nic_alarm_event_code(slot_num, nic_num);
	acs_aeh_percSeverity         event_severity = EVENT_NIC_NOT_CONNECTED_SEVERITY;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_NIC_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = nic_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'NIC NOT CONNECTED slot == %2d  nic == %2d' not raised! call_result == '%d' error-test: %s ", slot_num, nic_num, call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'NIC NOT CONNECTED slot == %2d  nic == %2d' successful raised! ", slot_num, nic_num);
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, nic_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

int __CLASS_NAME__::raise_raid_alarm(int32_t slot_num){
	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_RAID_PROBLEM_TEXT_HEAD EVENT_RAID_PROBLEM_TEXT_TAIL;
	char raid_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	::snprintf(raid_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s", alarm_problem_text);

	acs_aeh_specificProblem      event_code = RAID_NOT_WORKING_EVENT_CODE;
	acs_aeh_percSeverity         event_severity = EVENT_RAID_NOT_WORKING_SEVERITY;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_RAID_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = raid_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'Mirrored Disk not redundant' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'Mirrored Disk not redundant' successful raised! ");
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_RAID, slot_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}
/**
 * Raise Alarm Handler for DRBD alarm
 */
int __CLASS_NAME__::raise_drbd_alarm(int32_t slot_num){
        ACS_AEH_ReturnType call_result;

        const char * alarm_problem_text  = EVENT_DRBD_PROBLEM_TEXT_HEAD EVENT_DRBD_PROBLEM_TEXT_TAIL;
        char drbd_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
        ::snprintf(drbd_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s", alarm_problem_text);

        acs_aeh_specificProblem      event_code = DRBD_NOT_WORKING_EVENT_CODE;
        acs_aeh_percSeverity         event_severity = EVENT_DRBD_NOT_WORKING_SEVERITY;
        acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
        acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
        acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
        acs_aeh_problemData          problem_data = EVENT_DRBD_PROBLEM_DATA;
        acs_aeh_problemText          problem_text = drbd_alarm_problem_text;

        call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
                        event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

        if ( call_result != ACS_AEH_ok ){
                _aeh_error_text =_alarmeventHandler.getErrorText();
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'Mirrored Disk not redundant' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
                return ACS_APBM_AEH_ALARM_NOT_RAISED;
        }

        ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'Mirrored Disk not redundant' successful raised! ");
        int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DRBD, slot_num, event_code);

        return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

//Redesign as per TR-HS30773
int __CLASS_NAME__::raise_thumbdrive_alarm(int32_t slot_num){
	ACS_AEH_ReturnType call_result;
	const char * alarm_problem_text  = EVENT_THUMBDRIVE_PROBLEM_TEXT_HEAD EVENT_THUMBDRIVE_PROBLEM_TEXT_TAIL;
	char thumbdrive_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	::snprintf(thumbdrive_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s", alarm_problem_text);

	acs_aeh_specificProblem      event_code = THUMBDRIVE_MALFUNCTION_EVENT_CODE;
	acs_aeh_percSeverity         event_severity = EVENT_THUMBDRIVE_MALFUNCTION_SEVERITY;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_THUMBDRIVE_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = thumbdrive_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
					event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
			_aeh_error_text =_alarmeventHandler.getErrorText();
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'THUMBDRIVE MALFUNCTION' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
			return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'THUMBDRIVE MALFUNCTION' successful raised! ");
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_THUMBDRIVE_MALFUNCTION, slot_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}
//Redesign as per TR-HS30773

int __CLASS_NAME__::raise_boardremoved_alarm (int32_t slot_num) {
	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_BOARD_PROBLEM_TEXT_HEAD EVENT_BOARDREM_PROBLEM_TEXT_TAIL;
	char board_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	::snprintf(board_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %5d",alarm_problem_text, slot_num);

	acs_aeh_specificProblem      event_code = BOARD_FAULTY_MISSING_EVENT_CODE_START_NUM + slot_num;
	acs_aeh_percSeverity         event_severity = EVENT_BOARD_MISSING_SEVERITY;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_BOARDREM_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = board_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'Board Removed from slot %d' not raised! call_result == '%d' error-test: %s ",slot_num, call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'Board Missing in slot %d' successful raised! ", slot_num);
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

int __CLASS_NAME__::raise_boardfaulty_alarm (int32_t slot_num) {
	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_BOARD_PROBLEM_TEXT_HEAD EVENT_BOARDFAULTY_PROBLEM_TEXT_TAIL;
	char board_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	snprintf(board_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %10d",alarm_problem_text, slot_num);

	acs_aeh_specificProblem      event_code = BOARD_FAULTY_MISSING_EVENT_CODE_START_NUM + slot_num;
	acs_aeh_percSeverity         event_severity = EVENT_BOARD_MISSING_SEVERITY;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_BOARDFAULTY_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = board_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'Board Removed from  slot %d' not raised! call_result == '%d' error-test: %s ",slot_num, call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'Board Faulty in slot %d' successful raised! ", slot_num);
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDFAULT, slot_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}
int __CLASS_NAME__::raise_scbrp_snmplinkdown_alarm(int32_t slot_num){

	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_SCBRP_PROBLEM_TEXT_HEAD EVENT_SCBRP_PROBLEM_TEXT_TAIL;
	char scb_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	snprintf(scb_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %5d",alarm_problem_text, slot_num);

	acs_aeh_specificProblem      event_code = (slot_num == 0)? SCB0_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE: SCB25_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE;
	acs_aeh_percSeverity         event_severity = EVENT_SCB_NOT_WORKING;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = (slot_num == 0)? EVENT_SCBRP_0_PROBLEM_DATA:EVENT_SCBRP_25_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = scb_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'SNMP communication with Switch Board in slot %d is not working' not raised! call_result == '%d' error-test: %s ",slot_num, call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'SNMP communication with Switch Board in slot %d is not working' successful raised! ", slot_num);
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN, slot_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;

}

int __CLASS_NAME__::raise_both_scbrp_snmplinkdown_alarm(int32_t slot_num1, int32_t slot_num2){

	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_SCBRP_PROBLEM_TEXT_HEAD EVENT_SCBRP_PROBLEM_TEXT_TAIL;
	char scb_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	snprintf(scb_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %1d AND %2d",alarm_problem_text, slot_num1, slot_num2);

	acs_aeh_specificProblem      event_code = SCB_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE;
	acs_aeh_percSeverity         event_severity = EVENT_BOTH_SCB_NOT_WORKING;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_BOTH_SCBRP_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = scb_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'SNMP communication with both Switch Board not raised! call_result == '%d' error-test: %s ",slot_num1, call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'SNMP communication with both Switch Board is not working' successful raised!");
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

int __CLASS_NAME__::raise_scx_snmplinkdown_alarm(int32_t slot_num){

	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_SCX_PROBLEM_TEXT_HEAD EVENT_SCX_PROBLEM_TEXT_TAIL;
	char scb_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	snprintf(scb_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s", alarm_problem_text);

	acs_aeh_specificProblem      event_code = (slot_num == 0)? SCB0_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE: SCB25_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE;
	acs_aeh_percSeverity         event_severity = EVENT_SCB_NOT_WORKING;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_SCX_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = scb_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'SNMP communication with Switch Board in slot %d is not working' not raised! call_result == '%d' error-test: %s ",slot_num, call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'SNMP communication with Switch Board in slot %d is not working' successful raised! ", slot_num);
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN, slot_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;

}

int __CLASS_NAME__::raise_both_scx_snmplinkdown_alarm(){

	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_SCX_PROBLEM_TEXT_HEAD EVENT_SCX_PROBLEM_TEXT_TAIL;
	char scb_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	snprintf(scb_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s", alarm_problem_text);

	acs_aeh_specificProblem      event_code = SCB_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE;
	acs_aeh_percSeverity         event_severity = EVENT_BOTH_SCB_NOT_WORKING;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_BOTH_SCX_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = scb_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'SNMP communication with both Switch Board is not working' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'SNMP communication with both Switch Board is not working' successful raised! ");
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;

}

int __CLASS_NAME__::raise_both_dmx_snmplinkdown_alarm(){


	ACS_AEH_ReturnType call_result;

        const char * alarm_problem_text  = EVENT_DMX_PROBLEM_TEXT_HEAD EVENT_DMX_PROBLEM_TEXT_TAIL;
        char scb_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
        snprintf(scb_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s", alarm_problem_text);

        acs_aeh_specificProblem      event_code = SCB_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE;
        acs_aeh_percSeverity         event_severity = EVENT_BOTH_SCB_NOT_WORKING;
        acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
        acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
        acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
        acs_aeh_problemData          problem_data = EVENT_BOTH_DMX_PROBLEM_DATA;
        acs_aeh_problemText          problem_text = scb_alarm_problem_text;

        call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
                        event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

        if ( call_result != ACS_AEH_ok ){
                _aeh_error_text =_alarmeventHandler.getErrorText();
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'The communication between AP and the DMXC is not working' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
                return ACS_APBM_AEH_ALARM_NOT_RAISED;
        }

        ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'The communication between AP and DMXC is not working' successful raised! ");
        int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN, event_code);

        return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;


}


int __CLASS_NAME__::raise_disk_alarm(int32_t slot_num){
	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_DISK_PROBLEM_TEXT_HEAD EVENT_DISK_PROBLEM_TEXT_TAIL;
	char disk_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	::snprintf(disk_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s", alarm_problem_text);
// TODO: substitute (slot_num == 14) with a method that retrieves the side of AP board from slot_num
	uint16_t side = 0;
       _server_working_set->cs_reader->get_my_side(side);	
	acs_aeh_specificProblem      event_code = (side == ACS_CS_API_HWC_NS::Side_A)? DISK_NODEA_NOT_CONNECTED_EVENT_CODE: DISK_NODEB_NOT_CONNECTED_EVENT_CODE;;
	acs_aeh_percSeverity         event_severity = EVENT_DISK_NOT_WORKING_SEVERITY;
	acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = EVENT_DISK_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = disk_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'Alarm 'Disk not connected properly' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'Disk not connected properly' successful raised!");
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISKCONN, slot_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

int __CLASS_NAME__::raise_disk_unavailable_alarm(int32_t slot_num)
{
        ACS_AEH_ReturnType call_result;
        char disk_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
        ::snprintf(disk_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %d", EVENT_DISK_UNAVAILABLE_PROBLEM_TEXT, slot_num);
// TODO: substitute (slot_num == 14) with a method that retrieves the side of AP board from slot_num
        uint16_t side = 0;
       _server_working_set->cs_reader->get_my_side(side);
	acs_aeh_specificProblem      event_code = (side == ACS_CS_API_HWC_NS::Side_A)? DISK_NODEA_NOT_CONNECTED_EVENT_CODE: DISK_NODEB_NOT_CONNECTED_EVENT_CODE;;
        acs_aeh_percSeverity         event_severity = EVENT_DISK_NOT_WORKING_SEVERITY;
        acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
        acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
        acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
        acs_aeh_problemData          problem_data = EVENT_DISK_UNAVAILABLE_PROBLEM_DATA;
        acs_aeh_problemText          problem_text = disk_alarm_problem_text;

        call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
                        event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

        if ( call_result != ACS_AEH_ok ){
                _aeh_error_text =_alarmeventHandler.getErrorText();
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'Alarm 'Disk not connected properly' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
                return ACS_APBM_AEH_ALARM_NOT_RAISED;
        }

        ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'Disk not available' successfully raised!");
        int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE, slot_num, event_code);
        return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

int  __CLASS_NAME__::cease_alarm(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type, int32_t slot_num, int nic_num){

	// check if the given alarm type is applicable in the current APG shelf architecture
	if(!is_alarm_applicable(alarm_type))
		return ACS_APBM_AEH_ALARM_NOT_APPLICABLE;

	int ret_code = ACS_APBM_AEH_SUCCESS;
	ACE_Guard<ACE_Thread_Mutex> guard_active_alarms(_activealarms_mutex);

	acs_apbm_alarminfo * alarm_to_cease = NULL;
	std::vector<acs_apbm_alarminfo*>::iterator iter;

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Searching for alarm (alarm_type == %d, slot == %d, nic_num == %d) in _active_alarms list!", alarm_type, slot_num, nic_num);
	for (iter = _active_alarms.begin(); iter != _active_alarms.end(); ++iter){
		if ((((*iter)->get_alarm_type() == alarm_type) &&
				((*iter)->get_slot() == slot_num)) && ((*iter)->get_nic_num() == nic_num))
		 {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Found alarm in alarm list!");
			alarm_to_cease = *iter;
			break;
		 }
	}

	if (alarm_to_cease){
	   ACS_AEH_ReturnType call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			   alarm_to_cease->get_event_code(),EVENT_CEASING,AP_EVENT_CAUSE,EVENT_CLASS_REFERENCE,EVENT_OBJ_REFERENCE,"","");

	  if ( call_result != ACS_AEH_ok ){
		  _aeh_error_text =_alarmeventHandler.getErrorText();
		  ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm  not ceased! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
		  ret_code = ACS_APBM_AEH_ALARM_NOT_CEASED;
	  }
	  else
		  ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm successful ceased! ");

	  // Remove alarm from active alarms list!
	  _active_alarms.erase(iter);
	  delete alarm_to_cease;
	}
	else {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Alarm not ceased: Could not find alarm in _active_alarms list!!");
		ret_code = ACS_APBM_AEH_ALARM_NOT_CEASED;
	}
	return ret_code;

 }


// PRIVATE METHODS
int __CLASS_NAME__::add_alarm_in_activelist(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type,  int32_t slot, int nic_num, long event_code) {

	ACE_Guard<ACE_Thread_Mutex> guard_active_alarms(_activealarms_mutex);

	acs_apbm_alarminfo *alarm_info = new (std::nothrow )acs_apbm_alarminfo(alarm_type, slot, nic_num);
	alarm_info->set_event_code (event_code);
	_active_alarms.push_back(alarm_info);
	return 0;
}

int __CLASS_NAME__::is_alarm_in_activelist(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type,  int32_t slot, int nic_num) {

	ACE_Guard<ACE_Thread_Mutex> guard_active_alarms(_activealarms_mutex);

	bool alarm_info_find = false;
	std::vector<acs_apbm_alarminfo*>::iterator iter;

	for (iter = _active_alarms.begin(); iter != _active_alarms.end(); ++iter)
	{
		if (((*iter)->get_alarm_type() == alarm_type) && ((*iter)->get_slot() == slot) && ((*iter)->get_nic_num() == nic_num))
		{
			alarm_info_find = true;
			break;
		}
	}
	return alarm_info_find;
}

/*int __CLASS_NAME__::get_APUB_side(int32_t slot){
	
         ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
        if (!hwc) { // ERROR: creating HWC CS instance
                return acs_apbm::ERR_CS_CREATE_HWC_INSTANCE;
        }

        ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance();
        if (!bs) {
                ACS_CS_API::deleteHWCInstance(hwc);
                return acs_apbm::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;
                }

        // search criteria
        bs->reset();
        bs->setSysType(ACS_CS_API_HWC_NS::SysType_AP);
        bs->setFBN(ACS_CS_API_HWC_NS::FBN_APUB);
        bs->setSlot(slot);


        ACS_CS_API_IdList board_list;
        ACS_CS_API_NS::CS_API_Result cs_call_result = hwc->getBoardIds(board_list, bs);
        if (cs_call_result != ACS_CS_API_NS::Result_Success) { // ERROR: getting board ids from CS
                ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
	 	ACS_CS_API::deleteHWCInstance(hwc);
                return acs_apbm::ERR_CS_GET_BOARD_ID;
        }

        if (board_list.size() <= 0) { // NO switch board found in the system
                ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
                ACS_CS_API::deleteHWCInstance(hwc);
                return acs_apbm::ERR_CS_NO_BOARD_FOUND;
        }
	uint16_t side = 0;
        // Retrieving information for each switch board in my magazine
        for (size_t i = 0; i < board_list.size(); ++i) {
                BoardID board_id = board_list[i];

                if ((cs_call_result = hwc->getSide(side, board_id)) != ACS_CS_API_NS::Result_Success) { // ERROR: getting side
                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getSide' failed: cannot retrieve the side of the board id == %u from CS: cs_call_result == %d",
                                        board_id, cs_call_result);
                        ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
                        ACS_CS_API::deleteHWCInstance(hwc);
                        return acs_apbm::ERR_CS_GET_SIDE;
                  }
        	}
	return side;
	}*/


int __CLASS_NAME__::cease_all_alarms()						 //TR HX65505
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Entering acs_apbm_alarmeventhandler::cease_all_alarms()");
	ACE_Guard<ACE_Thread_Mutex> guard_active_alarms(_activealarms_mutex);

	acs_apbm_alarminfo * alarm_to_cease = NULL;
	std::vector<acs_apbm_alarminfo*>::iterator iter;

	ACS_APBM_LOG(LOG_LEVEL_WARN, "During shutdown, number of alarms in list == %d - Cease them now!", _active_alarms.size());

	for (iter = _active_alarms.begin(); iter != _active_alarms.end(); ++iter){
		alarm_to_cease = *iter;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Found an alarm (alarm_type == %d, slot == %d, nic_num == %d) in _active_alarms list!", alarm_to_cease->get_alarm_type(), alarm_to_cease->get_slot(), alarm_to_cease->get_nic_num());
		ACS_AEH_ReturnType call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
				 alarm_to_cease->get_event_code(),EVENT_CEASING,AP_EVENT_CAUSE,EVENT_CLASS_REFERENCE, EVENT_OBJ_REFERENCE,"","");

		if ( call_result != ACS_AEH_ok ){
	                  int ret_code = ACS_APBM_AEH_SUCCESS;
			  _aeh_error_text =_alarmeventHandler.getErrorText();
			  ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Alarm  not ceased! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
			  ret_code = ACS_APBM_AEH_ALARM_NOT_CEASED;
		}
		else
			  ACS_APBM_LOG(LOG_LEVEL_INFO, "Alarm successful ceased! ");
		  // Remove alarm
		  delete alarm_to_cease;
		  alarm_to_cease = NULL;
	}
	_active_alarms.clear();
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Leaving acs_apbm_alarmeventhandler::cease_all_alarms()");
	return 0;
}

int __CLASS_NAME__::get_nic_alarm_event_code(int32_t /*slot_num*/, int nic){

	int event_code ;

	uint16_t side = 0;
       _server_working_set->cs_reader->get_my_side(side); 
	
// Modified with switch case to handle new NIC cards for GEP4 or GEP5 HW
		
       switch(nic)
       {
       case 1:
       case 2:
       case 3:
       case 4:
    	   if (side == ACS_CS_API_HWC_NS::Side_B)
    		   event_code = NIC_NOT_CONN_NODEB_EVENT_CODE_START_NUM + nic -1;
    	   else
    		   event_code = NIC_NOT_CONN_NODEA_EVENT_CODE_START_NUM + nic -1;
    	   break;

       case 5:   if (side == ACS_CS_API_HWC_NS::Side_B)
    	   event_code = NIC5_NOT_CONN_NODEB_EVENT_CODE;
       else
    	   event_code = NIC5_NOT_CONN_NODEA_EVENT_CODE;
       break;

       case 6:  if (side == ACS_CS_API_HWC_NS::Side_B)
    	   event_code = NIC6_NOT_CONN_NODEB_EVENT_CODE;
       else
    	   event_code = NIC6_NOT_CONN_NODEA_EVENT_CODE;
       break;

       case 7:  if (side == ACS_CS_API_HWC_NS::Side_B)
    	   event_code = NIC7_NOT_CONN_NODEB_EVENT_CODE;
       else
    	   event_code = NIC7_NOT_CONN_NODEA_EVENT_CODE;
       break;

       case 8:  if (side == ACS_CS_API_HWC_NS::Side_B)
    	   event_code = NIC8_NOT_CONN_NODEB_EVENT_CODE;
       else
    	   event_code = NIC8_NOT_CONN_NODEA_EVENT_CODE;
       break;

       default: ACS_APBM_LOG(LOG_LEVEL_INFO, "NIC VALUE NOT VALID!!");
       }
       return event_code;
}
int __CLASS_NAME__::handle_nic_status_data(const nic_status_data &nic_data, int32_t slot_num ){

	// check if the NIC alarm type is applicable in the current APG shelf architecture
	if(!is_alarm_applicable(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC))
		return 0;	// alarm not applicable. Nothing to do

	/* MAPPING design base NIC value with Linux Eth interface
	 * NIC - ETH
	 *  1  -  3
	 *  2  -  4
	 *  3  -  0
	 *  4  -  1
	 *  5  -  2
	*/
	int physical_separation_status = _server_working_set->monitoring_service_event_handler->check_physical_separation_status();
	int teaming_status = _server_working_set->monitoring_service_event_handler->check_teaming_status();

	//ETH2 == PHYSICAL SEPARATION
	if (physical_separation_status == 1) {//configured
		if(nic_data.nics[ETH2] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			raise_nic_alarm(slot_num, NIC5);
		if(nic_data.nics[ETH2] == axe_eth_shelf::NIC_STATUS_CONNECTED)
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC5);
	}
	//ETH3 == IPNA
	if(nic_data.nics[ETH3] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		raise_nic_alarm(slot_num, NIC1);
	if(nic_data.nics[ETH3] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC1);
	//ETH4 == IPNB
	if(nic_data.nics[ETH4] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		raise_nic_alarm(slot_num, NIC2);
	if(nic_data.nics[ETH4] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC2);

	//ETH0 - ETH1 == DEBUG(FRONT CONNECTOR) - PUBLIC
	cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num); //ceasing teaming_status alarm
	if (teaming_status == 1){ //TEAMING configured
#if 0
// Not necessary anymore due to the USA_FM alarm
		if(nic_data.nics[ETH0] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED && nic_data.nics[1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED){
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC3);
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC4);
			raise_team_alarm(slot_num);
			return 0;
		}
#endif
		if (nic_data.nics[ETH0] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			raise_nic_alarm(slot_num, NIC3);
		if(nic_data.nics[ETH0] == axe_eth_shelf::NIC_STATUS_CONNECTED)
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC3);

		if (nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			raise_nic_alarm(slot_num, NIC4);
		if(nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_CONNECTED)
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC4);

		return 0;
	}
	//to cease alarm if teaming becomes not configured
	cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC3);

	if (nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		raise_nic_alarm(slot_num, NIC4);
	if(nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC4);

	return 0;
}


int __CLASS_NAME__::handle_nic_status_data(const nic_status_data_10g &nic_data, int32_t slot_num ){

	// check if the NIC alarm type is applicable in the current APG shelf architecture
	if(!is_alarm_applicable(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC))
		return 0; // alarm not applicable. Nothing to do

	/* MAPPING design base NIC value with Linux Eth interface
	 * NIC - ETH
	 *  1  -  3
	 *  2  -  4
	 *  3  -  0
	 *  4  -  1
	 *  5  -  2
	 *  6  -  5
	 *  7  -  6
	*/
	int physical_separation_status = _server_working_set->monitoring_service_event_handler->check_physical_separation_status();
	int teaming_status = _server_working_set->monitoring_service_event_handler->check_teaming_status();
	int oam_access_type = -1; // (0 == FRONTCABLE)  - (1 == NOCABLE)
	  _server_working_set->cs_reader->get_oam_access_type(oam_access_type);
	  ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Successfully Oam Status retrieved value: %d", oam_access_type);
	  //ETH2 == PHYSICAL SEPARATION
	if (physical_separation_status == 1) {//configured
		if(nic_data.nics[ETH2] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			raise_nic_alarm(slot_num, NIC5);
		if(nic_data.nics[ETH2] == axe_eth_shelf::NIC_STATUS_CONNECTED)
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC5);
	}
	//ETH3 == IPNA
	if(nic_data.nics[ETH3] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		raise_nic_alarm(slot_num, NIC1);
	if(nic_data.nics[ETH3] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC1);
	//ETH4 == IPNB
	if(nic_data.nics[ETH4] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		raise_nic_alarm(slot_num, NIC2);
	if(nic_data.nics[ETH4] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC2);

	//ETH5 and ETH6 SUPPORT
	//ETH5
	if(nic_data.nics[ETH5] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		raise_nic_alarm(slot_num, NIC6);
	if(nic_data.nics[ETH5] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC6);
	//ETH6
	if(nic_data.nics[ETH6] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		raise_nic_alarm(slot_num, NIC7);
	if(nic_data.nics[ETH6] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC7);
	
	//BOND1
	if(nic_data.nics[BOND1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
	    raise_nic_alarm(slot_num, NIC8);
	if(nic_data.nics[BOND1] == axe_eth_shelf::NIC_STATUS_CONNECTED)
	    cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC8);


	//ETH0 - ETH1 == DEBUG(FRONT CONNECTOR) - PUBLIC
	cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num); //ceasing teaming_status alarm
	if (teaming_status == 1){ //TEAMING configured
#if 0
// Not necessary anymore due to the USA_FM alarm
		if(nic_data.nics[ETH0] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED && nic_data.nics[1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED){
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC3);
			cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC4);
			raise_team_alarm(slot_num);
			return 0;
		}
#endif


		if (oam_access_type != 1/* 1 == NOCABLE*/)
		{
		    ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Oam Status: %d FRONTCABLE", oam_access_type);
		    if(nic_data.nics[ETH0] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		      raise_nic_alarm(slot_num, NIC3);
		    if(nic_data.nics[ETH0] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		      cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC3);

		    if (nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
		      raise_nic_alarm(slot_num, NIC4);
		    if(nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_CONNECTED)
		      cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC4);
		}
		return 0;
	}
	//to cease alarm if teaming becomes not configured
	cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC3);

	if (oam_access_type != 1/* 1 == NOCABLE*/)
	{
	    if (nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
	      raise_nic_alarm(slot_num, NIC4);
	    if(nic_data.nics[ETH1] == axe_eth_shelf::NIC_STATUS_CONNECTED)
	      cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC4);
	}

	return 0;
}
int __CLASS_NAME__::handle_nic_status_data(const nic_status_data_gep7 &nic_data, int32_t slot_num ){

        // check if the NIC alarm type is applicable in the current APG shelf architecture
        if(!is_alarm_applicable(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC))
                return 0; // alarm not applicable. Nothing to do

        /* MAPPING design base NIC value with Linux Eth interface
         * NIC - ETH
         *  1  -  3
         *  2  -  4
         *  3  -  0
         *  4  -  1
         *  5  -  2
         *  6  -  5
         *  7  -  6
        */
        int physical_separation_status = _server_working_set->monitoring_service_event_handler->check_physical_separation_status();
        int teaming_status = _server_working_set->monitoring_service_event_handler->check_teaming_status();
        int oam_access_type = -1; // (0 == FRONTCABLE)  - (1 == NOCABLE)
          _server_working_set->cs_reader->get_oam_access_type(oam_access_type);
          ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Successfully Oam Status retrieved value: %d", oam_access_type);
          //ETH2 == PHYSICAL SEPARATION
        if (physical_separation_status == 1) {//configured
                if(nic_data.nics[ETH2] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
                        raise_nic_alarm(slot_num, NIC5);
                if(nic_data.nics[ETH2] == axe_eth_shelf::NIC_STATUS_CONNECTED)
                        cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC5);
        }
        //ETH3 == IPNA
        if(nic_data.nics[ETH3] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
                raise_nic_alarm(slot_num, NIC1);
        if(nic_data.nics[ETH3] == axe_eth_shelf::NIC_STATUS_CONNECTED)
                cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC1);
        //ETH4 == IPNB
        if(nic_data.nics[ETH4] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
                raise_nic_alarm(slot_num, NIC2);
        if(nic_data.nics[ETH4] == axe_eth_shelf::NIC_STATUS_CONNECTED)
                cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC2);

        //ETH5 and ETH6 SUPPORT
        //ETH5
        if(nic_data.nics[ETH5] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
                raise_nic_alarm(slot_num, NIC6);
        if(nic_data.nics[ETH5] == axe_eth_shelf::NIC_STATUS_CONNECTED)
                cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC6);
        //ETH6
        if(nic_data.nics[ETH6] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
                raise_nic_alarm(slot_num, NIC7);
        if(nic_data.nics[ETH6] == axe_eth_shelf::NIC_STATUS_CONNECTED)
                cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC7);

        //BOND1
        if(nic_data.nics[BOND1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
            raise_nic_alarm(slot_num, NIC8);
        if(nic_data.nics[BOND1] == axe_eth_shelf::NIC_STATUS_CONNECTED)
            cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC8);

        //ETH0 - ETH1 == DEBUG(FRONT CONNECTOR) - PUBLIC
        cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num); //ceasing teaming_status alarm
  //      if (teaming_status == 1){ //TEAMING configured
#if 0
// Not necessary anymore due to the USA_FM alarm
                if(nic_data.nics[ETH0] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED && nic_data.nics[1] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED){
                        cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC3);
                        cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC, slot_num, NIC4);
                        raise_team_alarm(slot_num);
                        return 0;
                }
#endif

        return 0;
}

int __CLASS_NAME__::cease_alarm( int alarm_type) {

	// check if the given alarm type is applicable in the current APG shelf architecture
	
	switch (alarm_type)
	{
		case ACS_APBM_AEH_SNMP_LINK_DOWN_LEFT_SCB:
			// cease alarm SCB0
			return  _server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN,acs_apbm_programconfiguration::slot_position_switch_board_left);
		case ACS_APBM_AEH_SNMP_LINK_DOWN_RIGHT_SCB:
			return _server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN,acs_apbm_programconfiguration::slot_position_switch_board_right);
		case ACS_APBM_AEH_SNMP_LINK_DOWN_BOTH_SCB:
		{
			return  _server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN);
		}
		default:
			return ACS_APBM_AEH_ALARM_NOT_CEASED;
	}
}

int __CLASS_NAME__::raise_alarm( int alarm_type) {

	// check if the given alarm type is applicable in the current APG shelf architecture
	
	switch (alarm_type) {
		case ACS_APBM_AEH_SNMP_LINK_DOWN_LEFT_SCB:
			// cease alarm SCB0
			return  _server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN, acs_apbm_programconfiguration::slot_position_switch_board_left);

		case ACS_APBM_AEH_SNMP_LINK_DOWN_RIGHT_SCB:
			return _server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN, acs_apbm_programconfiguration::slot_position_switch_board_right);

		case ACS_APBM_AEH_SNMP_LINK_DOWN_BOTH_SCB:
		{
			const int slot_pos_left  = acs_apbm_programconfiguration::slot_position_switch_board_left;
			const int slot_pos_right = acs_apbm_programconfiguration::slot_position_switch_board_right;
			return  _server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN, slot_pos_left, slot_pos_right, -1);
		}
		default:
			return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

}

int __CLASS_NAME__::raise_unsupported_config_event()
{
        ACS_APBM_LOG(LOG_LEVEL_INFO, "raise_unsupported_config_event function called!");
        ACS_AEH_ReturnType call_result;

        acs_aeh_specificProblem      event_code = UNSUPPORTED_CONFIGURATION_EVENT_CODE;
        acs_aeh_percSeverity         event_severity = EVENT_UNSUPPORTED_CONFIGURATION_SEVERITY;
        acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
        acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
        acs_aeh_objectOfReference    objOfReference = OBJ_REFERENCE_FOR_EVENT;
        acs_aeh_problemData          problem_data = EVENT_UNSUPPORTED_CONFIGURATION_PROBLEM_DATA;
        acs_aeh_problemText          problem_text = EVENT_UNSUPPORTED_CONFIGURATION_PROBLEM_TEXT;

        call_result = _alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
                        event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

        if ( call_result != ACS_AEH_ok )
	{
                _aeh_error_text =_alarmeventHandler.getErrorText();
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Event 'Unsupported Configuration' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
                return 1;
        }

        ACS_APBM_LOG(LOG_LEVEL_INFO, " Event 'Unsupported Configuration' successfully raised!");

        return ACS_APBM_AEH_SUCCESS;
}

int __CLASS_NAME__::get_lag_alarm_event_code(int32_t slot_num, int port_number){
	int event_code ;

       switch(slot_num)
       {
       case 0:
    	   event_code = LAG_DISABLED_SCXA_ALARM_EVENT_CODE + port_number;
           break;
       case 25:
    	   event_code = LAG_DISABLED_SCXB_ALARM_EVENT_CODE + port_number;
           break;
       default: ACS_APBM_LOG(LOG_LEVEL_INFO, "VALUE NOT VALID!!");
       }
       return event_code;
}
int __CLASS_NAME__::raise_lag_disabled_alarm (int32_t slot_num,int port_num){
	ACS_AEH_ReturnType call_result;

	const char * alarm_problem_text  = EVENT_LAG_PROBLEM_TEXT_HEAD EVENT_LAGFAULTY_PROBLEM_TEXT_TAIL;
	char lag_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	snprintf(lag_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s %1d %11d", alarm_problem_text, slot_num, port_num);

	acs_aeh_specificProblem      event_code = get_lag_alarm_event_code(slot_num, port_num);
	acs_aeh_percSeverity         event_severity = EVENT_LAG_DISABLED_SEVERITY;
	acs_aeh_probableCause        event_cause = LAG_DISABLED_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data = LAG_DISABLED_PROBLEM_DATA;
	acs_aeh_problemText          problem_text = lag_alarm_problem_text;

	call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

	if ( call_result != ACS_AEH_ok ){
		_aeh_error_text =_alarmeventHandler.getErrorText();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG call '_alarmeventHandler.sendEventMessage' Failed: Alarm 'LAG disabled for  slot %d' not raised! call_result == '%d' error-test: %s ",slot_num, call_result, _aeh_error_text);
		return ACS_APBM_AEH_ALARM_NOT_RAISED;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, " Alarm 'INFRASTRUCTURE LINK AGGREGATION FAULT slot == %2d  port_num == %2d' successful raised! ", slot_num, port_num);
	int ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, slot_num, port_num, event_code);

	return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
}

int __CLASS_NAME__::raise_no_communication_event()
{
	ACS_AEH_ReturnType call_result;

        acs_aeh_specificProblem      event_code = NO_COMMUNICATION_EVENT_CODE;
        acs_aeh_percSeverity         event_severity = EVENT_NO_COMMUNICATION_SEVERITY;
        acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
        acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
        acs_aeh_objectOfReference    objOfReference = OBJ_REFERENCE_FOR_EVENT;
        acs_aeh_problemData          problem_data = EVENT_NO_COMMUNICATION_PROBLEM_DATA;
        acs_aeh_problemText          problem_text = EVENT_NO_COMMUNICATION_PROBLEM_TEXT;

        call_result = _alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
                        event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

        if ( call_result != ACS_AEH_ok )
        {
                _aeh_error_text =_alarmeventHandler.getErrorText();
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: Event 'No Communication' not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
                return 1;
        }

        ACS_APBM_LOG(LOG_LEVEL_INFO, " Event 'OaM board is not be able to communicate with the IPMI processor' successfully raised!");

        return ACS_APBM_AEH_SUCCESS;
}
bool __CLASS_NAME__::is_alarm_applicable(int alarm_type, int apg_shelf_architecture)
{
	bool alarm_applicable = true;

	int env = apg_shelf_architecture;
	if((env == -1) && (_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS))
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
	}

	switch (alarm_type)
	{
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDFAULT:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SCBEVENT:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_SBLINKDOWN:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOTHSBLINKDOWN:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISKCONN:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_THUMBDRIVE_MALFUNCTION:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_NIC:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_TEAM:
			alarm_applicable = (env != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED);			
			break;
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_RAID:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DRBD:
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE:
			alarm_applicable = true;
			break;
		case acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED:
			alarm_applicable = (env == ACS_CS_API_CommonBasedArchitecture::SCX);
			if(alarm_applicable)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"SCX architecture! LAG alarm is applicable");
			break;
		default:
			alarm_applicable = false;	// unknown alarm type	
			break;
	}
	return alarm_applicable;
}

int __CLASS_NAME__::raise_temperature_alarm(int32_t slot_num, int32_t severity)
{
	bool ret_code = is_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slot_num, severity);

	if(ret_code)
	{
		ACS_APBM_LOG(LOG_LEVEL_INFO, "already in alarm list");
		return ACS_APBM_AEH_SUCCESS;
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Raising alarm");
		ACS_AEH_ReturnType call_result;
		const char * alarm_problem_text  = EVENT_APUB_TEMPERATURE_PROBLEM_TEXT_HEAD EVENT_APUB_TEMPERATURE_PROBLEM_TEXT_TAIL;
		char apub_temperature_alarm_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
		int slot = 0;
		uint16_t side = ACS_CS_API_HWC_NS::Side_A;

		snprintf(apub_temperature_alarm_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s \t %d",alarm_problem_text, slot_num);
		_server_working_set->cs_reader->get_my_side(side);
		_server_working_set->cs_reader->get_my_slot(slot);

		if( slot !=slot_num )
		{
			side= (side == ACS_CS_API_HWC_NS::Side_A )? ACS_CS_API_HWC_NS::Side_B:ACS_CS_API_HWC_NS::Side_A;;
		}

		acs_aeh_specificProblem      event_code = (side == ACS_CS_API_HWC_NS::Side_A)? HIGH_TEMPERATURE_NODEA_EVENT_CODE: HIGH_TEMPERATURE_NODEB_EVENT_CODE;;
		acs_aeh_percSeverity         event_severity = (severity == 10)? EVENT_APUB_TEMPERATURE_HIGH: EVENT_APUB_TEMPERATURE_LOW;;
		acs_aeh_probableCause        event_cause = AP_EVENT_CAUSE;
		acs_aeh_objClassOfReference  classReference = EVENT_CLASS_REFERENCE;
		acs_aeh_objectOfReference    objOfReference = EVENT_OBJ_REFERENCE;
		acs_aeh_problemData          problem_data = EVENT_APUB_TEMPERATURE_PROBLEM_DATA;
		acs_aeh_problemText          problem_text = apub_temperature_alarm_problem_text;
		call_result =_alarmeventHandler.sendEventMessage(acs_apbm_programconfiguration::ha_daemon_name,
				event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text);

		if ( call_result != ACS_AEH_ok )
		{
			_aeh_error_text =_alarmeventHandler.getErrorText();
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "call '_alarmeventHandler.sendEventMessage' Failed: BLADE HIGH TEMPERATURE not raised! call_result == '%d' error-test: %s ", call_result, _aeh_error_text);
			return ACS_APBM_AEH_ALARM_NOT_RAISED;
		}

		ret_code = add_alarm_in_activelist(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slot_num, severity, event_code);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "BLADE HIGH TEMPERATURE successfully raised! with Severity = %d", severity);
		return (ret_code)? ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST: ACS_APBM_AEH_SUCCESS;
	}
}
