/*
 * acs_chb_aadst_alarmHandler.cpp
 *
 *  Created on: Sep 23, 2014
 *      Author: xassore
 */
#include "acs_chb_tra.h"
#include "acs_aeh_error.h"

#include "acs_apgcc_paramhandling.h"
#include "acs_chb_aadst_alarmHandler.h"


acs_chb_aadst_alarmHandler::~acs_chb_aadst_alarmHandler() {
	// TODO Auto-generated destructor stub
	if(_omHandlerInitialized)
		_omHandler.Finalize();
}


int acs_chb_aadst_alarmHandler::alarm_raise(aadst_alarm_type alarm_severity )
{
	char event_problem_text [EVENT_PROBLEM_TEXT_MAX_SIZE];
	if(alarm_severity == CHB_AADST_ALARM_A2){
		::snprintf(event_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s%s", AADST_EVENT_PROBLEM_TEXT_HEAD,AADST_EVENT_A2_PROBLEM_TEXT_TAIL);
	}
	else{
		::snprintf(event_problem_text, EVENT_PROBLEM_TEXT_MAX_SIZE, "%s%s", AADST_EVENT_PROBLEM_TEXT_HEAD,AADST_EVENT_A1_PROBLEM_TEXT_TAIL);
	}
	acs_aeh_specificProblem  	event_code=(alarm_severity == CHB_AADST_ALARM_A2)?AADST_A2_EVENT_CODE:
																				  AADST_A1_EVENT_CODE;
	acs_aeh_percSeverity     	event_severity=(alarm_severity == CHB_AADST_ALARM_A2)?ACS_AEH_PERCEIVED_SEVERITY_A2:
																					ACS_AEH_PERCEIVED_SEVERITY_A1;

	acs_aeh_probableCause        event_cause = AADST_EVENT_CAUSE;
	acs_aeh_objClassOfReference  classReference = AADST_EVENT_CLASS_REFERENCE;
	acs_aeh_objectOfReference    objOfReference = AADST_EVENT_OBJ_REFERENCE;
	acs_aeh_problemData          problem_data= "";
	acs_aeh_problemText          problem_text = event_problem_text;

	ACS_AEH_ReturnType aeh_result =_aeh_eventhandler.sendEventMessage(AADST_PROCESS_NAME,
			event_code,event_severity,event_cause,classReference,objOfReference,problem_data,problem_text,true);

	if ( aeh_result != ACS_AEH_ok ){
		ERROR(1, "acs_chb_aadst_alarmHandler::alarm_raise: call 'sendEventMessage' Failed: Alarm 'AP SUMMERTIME UPDATE ' not raised! aeh_result == '%d' error-test: %s ",
				aeh_result, _aeh_eventhandler.getErrorText());
		return 1;
	}

	DEBUG(1, " Alarm 'AP SUMMERTIME UPDATE' severity level == %s successful raised! ",(alarm_severity == CHB_AADST_ALARM_A2)? "A2": "A1");

	return 0;
}


aadst_alarm_type acs_chb_aadst_alarmHandler::check_for_alarm_presence (void)
{
	acs_apgcc_paramhandling pha;
  	ACS_CC_ReturnType result;
  	ACE_UINT32 event_code;
  	int ret_code = 0;

  	/*try initialize imm connection to IMM*/
  	if(!_omHandlerInitialized && (ret_code= omHandler_init())!= 0){
  		DEBUG(1,"acs_chb_aadst_alarmHandler::check_for_alarm_presence: omHandler_init() failed: return code == %d", ret_code);
  		return CHB_AADST_ALARM_ERROR; /*exiting on IMM init failure: generic error*/
  	}

  	string class_instance_name;
  	std::vector<std::string> p_dnList;
  	result = _omHandler.getClassInstances(ALARMINFO_IMM_CLASSNAME, p_dnList);
  	if (result != ACS_CC_SUCCESS && _omHandler.getInternalLastError() != -41 ) { //TR IA38905
  		/* an error occurred  */
  		ERROR(1,"%", "acs_chb_aadst_alarmHandler::check_for_alarm_presence:  'OmHandler::getClassInstances' failed: cannot get AlarmInfo");
  		return CHB_AADST_ALARM_ERROR;
  	}
  	//aadst_alarm_type ret_value = CHB_AADST_NO_ALARMS;
  	aadst_alarm_type aadst_alarm_found = CHB_AADST_NO_ALARMS;
  	for (unsigned int i=0; i<p_dnList.size() && !aadst_alarm_found; i++){
  		class_instance_name = p_dnList[i];

		ACS_CC_ReturnType imm_result = pha.getParameter(class_instance_name.c_str(), ALARMINFO_EVENTCODE_IMM_ATTRNAME, &event_code);
		if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting parameter value from IMM
			ERROR(1, "'getParameter' failed: cannot retrieve the specifProblem attribute from IMM: "
			"internal last error == %d: internal laste error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
			return CHB_AADST_ALARM_ERROR;
		}
		if (event_code == AADST_A2_EVENT_CODE){
			 // check if the alarm is ceasing
			if(get_alarm_state(class_instance_name.c_str())== AADST_ALARM_STATE_ACTIVE){
				aadst_alarm_found = CHB_AADST_ALARM_A2;
				DEBUG(1,"%s", "acs_chb_aadst_alarmHandler::check_for_alarm_presence: Found AP SUMMER TIME 'A2' alarm");
				break;
			}
		}
		else if (event_code == AADST_A1_EVENT_CODE){
			 // check if the alarm is ceasing
			if(get_alarm_state(class_instance_name.c_str())== AADST_ALARM_STATE_ACTIVE){
				aadst_alarm_found = CHB_AADST_ALARM_A1;
				DEBUG(1,"%s", "acs_chb_aadst_alarmHandler::check_for_alarm_presence: Found AP SUMMER TIME 'A1' alarm");
				break;
			}
		}
  	}

  	return  aadst_alarm_found;
}

int acs_chb_aadst_alarmHandler::change_alarm_severity_to_A1 (void ){

	if (alarm_A2_cease() != 0)
		ERROR(1,"%s", "acs_chb_aadst_alarmHandler::change_alarm_severity_to_A1: call 'alarm_A2_cease' Failed!");
	 return alarm_raise(CHB_AADST_ALARM_A1);
}

/*************************************************/
/**************** PRIVATE METHODS  ***************/
/*************************************************/

int acs_chb_aadst_alarmHandler ::omHandler_init()
{
	ACS_CC_ReturnType result;
	result = _omHandler.Init();

  	if (result != ACS_CC_SUCCESS){
  		ERROR(1,"%s", "acs_chb_aadst_alarmHandler::check_for_alarm_presence: 'OmHandler::Init' failed: cannot get AlarmInfo!");
  		return 1;
  	}/*exiting on IMM init failure: generic error*/

  	_omHandlerInitialized=true;

	return 0;
}

int acs_chb_aadst_alarmHandler::alarm_A2_cease (void) {

	ACS_AEH_ReturnType aeh_result =_aeh_eventhandler.sendEventMessage(AADST_PROCESS_NAME,AADST_A2_EVENT_CODE,
			ACS_AEH_PERCEIVED_SEVERITY_CEASING,AADST_EVENT_CAUSE,AADST_EVENT_CLASS_REFERENCE,AADST_EVENT_OBJ_REFERENCE,"","");

	if ( aeh_result != ACS_AEH_ok ){
		ERROR(1, "acs_chb_aadst_alarmHandler::alarm_A2_cease: 'sendEventMessage' Failed: Alarm 'AP SUMMERTIME UPDATE ' not ceased! aeh_result == '%d' error-test: %s ",
			  	aeh_result, _aeh_eventhandler.getErrorText());
		return 1;
	}
	DEBUG(1, "%s"," Alarm 'AP SUMMERTIME UPDATE 'A2' successful ceased!");
	return 0;
}

int  acs_chb_aadst_alarmHandler::get_alarm_state (const char *alarmObject_name)
{
	int ret_code = 0;

	if(!alarmObject_name || !*alarmObject_name ){
		ERROR(1, "%s", "acs_chb_aadst_alarmHandler::check_isceasing: empty alarmObject_name parameter");
		return AADST_ALARM_STATE_ERROR;
	}
	/*try initialize imm connection to IMM*/
	if(!_omHandlerInitialized && (ret_code= omHandler_init())!= 0){
		DEBUG(1,"acs_chb_aadst_alarmHandler::check_for_alarm_presence: omHandler_init() failed: return code == %d", ret_code);
		return AADST_ALARM_STATE_ERROR; /*exiting on IMM init failure: generic error*/
	}

	int ceasePending = 0;
	// check if the alarm is ceasing
	ACS_CC_ImmParameter paramToFind;
	paramToFind.attrName = (char *)ALARMINFO_CEASEPENDING_IMM_ATTRNAME;

	if( _omHandler.getAttribute(alarmObject_name, &paramToFind) == ACS_CC_SUCCESS )
	{
		unsigned values_num = paramToFind.attrValuesNum;
		if(values_num == 0){ // ERROR
			ERROR(1, "%s","acs_chb_aadst_alarmHandler::check_for_alarm_presence: no values found for ceasePending attr!");
			return AADST_ALARM_STATE_ERROR;
			// ANYTHING TO DO???
		}
		ACE_INT32 **values = reinterpret_cast<int **>(paramToFind.attrValues);
		for( unsigned int i=0; i < values_num ;i++) {
			DEBUG(0, "acs_chb_aadst_alarmHandler::check_for_alarm_presence: values[%u]  == %d!", i, *values[i]);
			ceasePending += *values[i];
		}
	}
	return (ceasePending == 0)? AADST_ALARM_STATE_ACTIVE: AADST_ALARM_STATE_PENDING;
}
