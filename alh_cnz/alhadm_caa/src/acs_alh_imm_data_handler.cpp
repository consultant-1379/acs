/*
 * acs_alh_imm_data_handler.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: efabron
 */

#include "acs_alh_imm_data_handler.h"
#include "acs_alh_imm_runtime_owner.h"
#include "stdint.h"
#include "time.h"

 extern bool alh_is_stopping;

const acs_alh_immAttributes IMM_AlarmInfoAttr[IMM_ALARM_INFO_CLASS_ATTR_MAXNUM+1]= {
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_NAME_RDN, AL_INFO_IDENTITY_RDN_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROCESS_NAME, AL_INFO_PROCESS_NAME_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SPEC_PROBL, AL_INFO_SPECIFIC_PROB_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBL_CAUSE, AL_INFO_PROBABLE_CAUSE_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CATEGORY, AL_INFO_CATEGORY_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_OBJ_REF, AL_INFO_OBJ_REF_CAUSE_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_DATA, AL_INFO_PROBLEM_DATA_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_TEXT, AL_INFO_PROBLEM_TEXT_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_SEVERITY, AL_INFO_SEVERITY_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_MANUAL_CEASE, AL_INFO_MANUAL_CEASE_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SEND_PRIORITY, AL_INFO_SEND_PRIORITY_ATTR_TYPE, 1},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF, AL_INFO_CP_ALARM_REF_ATTR_TYPE, MAX_NUMBER_OF_VALUE_ATTR},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER, AL_INFO_RETR_COUNTER_ATTR_TYPE, MAX_NUMBER_OF_VALUE_ATTR},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CEASE_PENDING, AL_INFO_CEASE_PENDING_ATTR_TYPE, MAX_NUMBER_OF_VALUE_ATTR},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, AL_INFO_ACKNOWLEDGE_ATTR_TYPE, MAX_NUMBER_OF_VALUE_ATTR},
	{ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_TIME, AL_INFO_TIME_TYPE, MAX_NUMBER_OF_VALUE_ATTR}
};

//========================================================================================
//	Constructor
//========================================================================================

acs_alh_imm_data_handler::acs_alh_imm_data_handler(acs_alh_imm_connection_handler *immConnectionHandler)
	: om_handler_(),
	util_("acs_alh_imm_data_handler")
{

	omHandlerInitialized_ = false;
	immConnectionHandler_ = immConnectionHandler;

	strcpy(alarmInfo_attr_.identity_rdn, "");
	strcpy(alarmInfo_attr_.process_name, "");
	strcpy(alarmInfo_attr_.probable_cause, "");
	strcpy(alarmInfo_attr_.category, "");
	strcpy(alarmInfo_attr_.object_of_ref, "");
	strcpy(alarmInfo_attr_.problem_data, "");
	strcpy(alarmInfo_attr_.problem_text, "");
	strcpy(alarmInfo_attr_.severity, "");

	alarmInfo_attr_.specificProblem = 0;
	alarmInfo_attr_.send_priority = 0;

	for (int i = 0; i<MAX_NUMBER_OF_VALUE_ATTR; i++)
	{
		strcpy(alarmInfo_attr_.time[i], "");
		alarmInfo_attr_.cp_alarm_ref[i] = 0;
		alarmInfo_attr_.retr_counter[i] = 0;
		alarmInfo_attr_.cease_pending[i] = 0;
		alarmInfo_attr_.acknowledge[i] = 0;
	}

}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_imm_data_handler::~acs_alh_imm_data_handler(){

	if(omHandlerInitialized_) {
		om_handler_.Finalize();
		omHandlerInitialized_ = false;
	}

}


//========================================================================================
//	omHandler_init method
//========================================================================================

int acs_alh_imm_data_handler::omHandler_init()
{
        acs_alh_log log_;
        log_.write(LOG_LEVEL_DEBUG,"In getAlarmInToList - Entry in acs_alh_imm_data_handler::omHandler_init() ");
	ACS_CC_ReturnType imm_result;

	if ( (imm_result = om_handler_.Init()) != ACS_CC_SUCCESS){
		setError(acs_alh::ERR_SAF_IMM_OM_INIT);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in acs_alh_imm_data_handler: OmHandler::Init()' failed: return code == %d", imm_result);
		setErrorText(problemData);
		return -1;
	}
        log_.write(LOG_LEVEL_DEBUG,"In getAlarmInToList - exit from acs_alh_imm_data_handler::omHandler_init() ");
	omHandlerInitialized_ = true;

	return 0;
}



//========================================================================================
//	createAlarmListNode method
//========================================================================================

int acs_alh_imm_data_handler::createAlarmListNode(const char *nodeName)
{
	int ret_code = 0;
	ACS_CC_ReturnType imm_result;

	if(!nodeName || !*nodeName ){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in createAlarmListNode: empty parameters ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}


	char rdnValue[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };
	const char * imm_dnName = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = util_.get_alh_imm_root_dn(imm_dnName)) {
		setError(call_result);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
				call_result);
		return call_result;
	}

	snprintf(rdnValue, sizeof(rdnValue) - 1, "%s=%s", ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, nodeName);


	std::vector<ACS_CC_ValuesDefinitionType> attrList;

	ACS_CC_ValuesDefinitionType srt_attribute;

	srt_attribute.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN);
	srt_attribute.attrType = ATTR_STRINGT;
	void* valueAttr[1] = {reinterpret_cast<void*>(rdnValue)};
	srt_attribute.attrValuesNum = 1;
	srt_attribute.attrValues = valueAttr;

	attrList.push_back(srt_attribute);


	imm_result = immConnectionHandler_->createRuntimeObj("AlarmListNode", imm_dnName, attrList);

	if(imm_result != ACS_CC_SUCCESS){
		setError(acs_alh::ERR_SAF_IMM_OM_CREATE);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "createAlarmListNode-createRuntimeObj failed ");
		ret_code = acs_alh::ERR_SAF_IMM_OM_CREATE;
	}


	return ret_code;
}


//========================================================================================
//	removeAlarmListNode method
//========================================================================================

int acs_alh_imm_data_handler::removeAlarmListNode(const char *dn_Node){

	int ret_code = 0;
	ACS_CC_ReturnType imm_result;

	if(!dn_Node || !*dn_Node ){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in removeAlarmListNode: empty parameters ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}

	// Get the list of SRVinfo objects that are children of the given node
	std::vector<std::string> services_dnList;
	//	if ((imm_call_result = _om_handler.getChildren(node_dn, ACS_APGCC_SUBLEVEL, & services_dnList)) == ACS_CC_SUCCESS)
	//	{
	//		// delete all service node info created under SrtNode
	//		for(unsigned int i=0; i<services_dnList.size(); i++){
	//			if ((imm_call_result = _ImmCHobj->deleteRuntimeObj(services_dnList[i].c_str()))!= ACS_CC_SUCCESS){
	//				snprintf(_last_error_text,ERROR_TEXT_SIZE,
	//									"error in removeNodeInfo: deleteRuntimeObj(%s)' failed: return code == %d",services_dnList[i].c_str(), imm_call_result);
	//				ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_DELETE;
	//			}
	//		}
	//	}

	//TODO
	//DA RIVEDERE
	if (ret_code != acs_alh::ERR_SAF_IMM_OM_DELETE){
		// Now delete AlarmListNode object
		if ((imm_result = immConnectionHandler_->deleteRuntimeObj(dn_Node))!= ACS_CC_SUCCESS){
			setError(acs_alh::ERR_SAF_IMM_OM_DELETE);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "removeAlarmListNode-deleteRuntimeObj failed ");
			ret_code = acs_alh::ERR_SAF_IMM_OM_DELETE;

			//	snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in removeAlarmListNode: deleteRuntimeObj(%s)' failed: return code == %d", node_dn, imm_call_result);

		}
	}
	return ret_code;


}


//========================================================================================
//	checkIfObjectIsPresent method
//========================================================================================

int acs_alh_imm_data_handler::checkIfObjectIsPresent(const char *dnObject, const char* className){

	/*
	 * < 0 - error occurs
	 *  0  - object not found
	 *  1  - object found
	 *
	 * */

	int ret_code = 0;
	ACS_CC_ReturnType imm_result;
        acs_alh_log log_;

	if(!dnObject || !*dnObject ){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in checkIfObjectIsPresent: empty parameters - dnObject");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}


	if(!className || !*className ){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in checkIfObjectIsPresent: empty parameters - className ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}

	if( !omHandlerInitialized_ ){
		if( omHandler_init() != 0 ){
			setError(acs_alh::ERR_SAF_IMM_OM_INIT);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "error in checkIfObjectIsPresent: initialisation of omHandler failure ");
			setErrorText(problemData);
			return acs_alh::ERR_SAF_IMM_OM_INIT;
		}
	}

	std::vector<std::string> dn_list;

	if ((imm_result = om_handler_.getClassInstances(className, dn_list)) ^ ACS_CC_SUCCESS) { //ERROR
		setError(acs_alh::ERR_SAF_IMM_OM_GET);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in checkIfObjectIsPresent: call 'om_handler.getClassInstances(...' failed: retrieving ALH parameters root from IMM: IMM error code == %d",
				om_handler_.getInternalLastError());
		setErrorText(problemData);

                om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "error in checkIfObjectIsPresent: call 'om_handler.getClassInstances(...' failed: retrieving ALH parameters root from IMM: IMM error code == %d",
                                om_handler_.getInternalLastError());
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - error in checkIfObjectIsPresent: call 'om_handler.getClassInstances(...' failed: retrieving ALH parameters root from IMM: IMM error code == %d",
                                om_handler_.getInternalLastError());
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }

		return acs_alh::ERR_SAF_IMM_OM_GET;

	} else { //OK: Checking how much dn items was found in IMM
		if (dn_list.size() == 0) { //0 (zero) object found

			return ret_code;

		} else { //OK: object found
			for(unsigned int i=0; i<dn_list.size(); i++){
				if( strcmp(dnObject, dn_list[i].c_str()) == 0 )
					return 1;
			}
		}
	}


	return ret_code;
}



//========================================================================================
//	addAlarmInfo method
//========================================================================================

int acs_alh_imm_data_handler::addAlarmInfo(const AllRecord* allPtr)
{
	int ret_code= 0;
	//ACS_CC_ReturnType imm_call_result;

	acs_alh_log log_;

	// check parameters
	if( !allPtr ){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in addAlarmInfo: empty parameter ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}


	char rdnValue[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };
	const char * root_dnName = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = util_.get_alh_imm_root_dn(root_dnName)) {
		setError(call_result);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
				call_result);
		return call_result;
	}

	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };
	void* valueAttr[IMM_ALARM_INFO_CLASS_ATTR_MAXNUM];
	void* cpValueAttr[MAX_NUMBER_OF_VALUE_ATTR];
	void* retrCounterValueAttr[MAX_NUMBER_OF_VALUE_ATTR];
	void* ceasePendValueAttr[MAX_NUMBER_OF_VALUE_ATTR];
	void* ackValueAttr[MAX_NUMBER_OF_VALUE_ATTR];
	void* timeValueAttr[MAX_NUMBER_OF_VALUE_ATTR];

	/*The DN name of the parent of object to be defined*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1,
			ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN"=%s,%s", util_.getlocalNodeName().c_str(), root_dnName);


	/*The RDN of the class to be defined*/
	if(!strcmp(allPtr->event.percSeverity, STRING_CEASING) ){
		snprintf(rdnValue, sizeof(rdnValue) - 1, "%hu:%hd:C", allPtr->identity[0], allPtr->identity[2]);
	}else{
		snprintf(rdnValue, sizeof(rdnValue) - 1, "%hu:%hd", allPtr->identity[0], allPtr->identity[2]);
	}

	attrList_.clear();
	strncpy(alarmInfo_attr_.identity_rdn, rdnValue, ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX);
	strncpy(alarmInfo_attr_.process_name, allPtr->event.processName, ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX);
	strncpy(alarmInfo_attr_.probable_cause, allPtr->event.probableCause, ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX);
	strncpy(alarmInfo_attr_.category, allPtr->event.objClassOfReference, ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX);
	strncpy(alarmInfo_attr_.object_of_ref, allPtr->event.objectOfReference, ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX);
	strncpy(alarmInfo_attr_.problem_data, allPtr->event.problemData, ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX);
	strncpy(alarmInfo_attr_.problem_text, allPtr->event.problemText, ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX);
	strncpy(alarmInfo_attr_.severity, allPtr->event.percSeverity, ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX);

	alarmInfo_attr_.specificProblem = (unsigned int)allPtr->event.specificProblem;
	alarmInfo_attr_.cp_alarm_ref[0] = (unsigned int)(allPtr->cpAlarmRef);
	alarmInfo_attr_.retr_counter[0] = (unsigned int)allPtr->retransCnt;
	alarmInfo_attr_.send_priority = (unsigned int)allPtr->sendPriority;

	if(allPtr->ceasePending){
		alarmInfo_attr_.cease_pending[0] = 1;
	}else{
		alarmInfo_attr_.cease_pending[0] = 0;
	}

	if(allPtr->acknowledge){
		alarmInfo_attr_.acknowledge[0] = 1;
	}else{
		alarmInfo_attr_.acknowledge[0] = 0;
	}

	if(allPtr->manualCease){
		alarmInfo_attr_.manual_cease = 1;
	}else{
		alarmInfo_attr_.manual_cease = 0;
	}

	char buffTime[20];
	strftime(buffTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&allPtr->time));
	strncpy(alarmInfo_attr_.time[0], buffTime, ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX);

	alh_imm_AlarmInfo_Attribute alinfo_rdn_attrib = AL_INFO_identity_rdn;
	int attrib_value_num =0;
	int shift_index = 0;
	for(int i = alinfo_rdn_attrib; i <= AL_INFO_time_attr; i++){
		switch(i){
			case AL_INFO_cpAlarmRef_attr :
				attrib_value_num = setALarmInfoAttribute((alh_imm_AlarmInfo_Attribute)i, cpValueAttr);
				break;

			case AL_INFO_retrCounter_attr :
				attrib_value_num = setALarmInfoAttribute((alh_imm_AlarmInfo_Attribute)i, retrCounterValueAttr);
				break;

			case AL_INFO_ceasePending_attr:
				attrib_value_num = setALarmInfoAttribute((alh_imm_AlarmInfo_Attribute)i, ceasePendValueAttr);
				break;

			case AL_INFO_acknowledge_attr:
				attrib_value_num = setALarmInfoAttribute((alh_imm_AlarmInfo_Attribute)i, ackValueAttr);
				break;

			case AL_INFO_time_attr:
				attrib_value_num = setALarmInfoAttribute((alh_imm_AlarmInfo_Attribute)i, timeValueAttr);
				break;
			default:
				attrib_value_num = setALarmInfoAttribute((alh_imm_AlarmInfo_Attribute)i, &valueAttr[i+shift_index]);
				break;

		}
		shift_index = attrib_value_num-1;
	}

	int counter =0;
	int imm_code = 0;

	do{
		acs_alh_imm_runtime_owner temp_OI(immConnectionHandler_);

		ret_code = immConnectionHandler_->createRuntimeObj(ACS_ALH_CONFIG_IMM_ALARM_INFO_CLASS_NAME, imm_dnName, attrList_);

		if(ret_code != ACS_CC_SUCCESS){

			imm_code = immConnectionHandler_->getInternalLastError();

			if ( ( imm_code == -5 ) || ( imm_code == -6 ) || ( imm_code == -10 ) ){
				log_.write(LOG_LEVEL_ERROR,"addAlarmInfo - createRuntimeObj failed ( %i )- Object %s - try %i", imm_code, imm_dnName, counter );
				sleep(2);
				counter++;
			}
			else if ( imm_code == -9 ){
				log_.write(LOG_LEVEL_ERROR,"addAlarmInfo - createRuntimeObj failed ( %i )- Object %s - try %i", imm_code, imm_dnName, counter );
				sleep(1);
				temp_OI.finalize_IMM();
				temp_OI.init_IMM( false );
				counter++;
			}
			else {
				setError(acs_alh::ERR_SAF_IMM_OM_CREATE);
				char problemData[DEF_MESSIZE];
				snprintf(problemData, sizeof(problemData) - 1, "addAlarmInfo(%s,%s):createRuntimeObj failed returnCode =%d", rdnValue, imm_dnName, ret_code);
				setErrorText(problemData);
				ret_code = acs_alh::ERR_SAF_IMM_OM_CREATE;
				break;
			}
		}
		else {
			break;
		}

	}while((ret_code != 0) || (counter<RETRY_COUNTER_MAX_VALUE));

	return ret_code;
}


//========================================================================================
//	setALarmInfoAttribute method
//========================================================================================

int  acs_alh_imm_data_handler::setALarmInfoAttribute(alh_imm_AlarmInfo_Attribute attrib, void **valueAttr)
{
	ACS_CC_ValuesDefinitionType attribute;
	uint16_t attr_value_num = 1;
	bool attrib_found = true;

	switch(attrib){
		case AL_INFO_identity_rdn :
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.identity_rdn);
			break;
		case AL_INFO_process_name_attr :
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.process_name);
			break;
		case AL_INFO_specProb_attr :
			valueAttr[0] = reinterpret_cast<void*>(&(alarmInfo_attr_.specificProblem));
			break;
		case AL_INFO_probCause_attr :
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.probable_cause);
			break;
		case AL_INFO_category_attr :
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.category);
			break;
		case AL_INFO_obj_ref_attr:
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.object_of_ref);
			break;
		case AL_INFO_problemData_attr :
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.problem_data);
			break;
		case AL_INFO_problemText_attr :
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.problem_text);
			break;
		case AL_INFO_severity_attr :
			valueAttr[0] = reinterpret_cast<void*>(alarmInfo_attr_.severity);
			break;
		case AL_INFO_manualCease_attr :
			valueAttr[0] = reinterpret_cast<void*>(&(alarmInfo_attr_.manual_cease));
			break;
		case AL_INFO_sendPriority_attr :
			valueAttr[0] = reinterpret_cast<void*>(&(alarmInfo_attr_.send_priority));
			break;
		case AL_INFO_cpAlarmRef_attr :
			attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
			for(int i=0; i < attr_value_num; i++){
				valueAttr[i]= reinterpret_cast<void*>(&(alarmInfo_attr_.cp_alarm_ref[i]));
			}
			break;
		case AL_INFO_retrCounter_attr :
			attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
			for(int i=0; i < attr_value_num; i++){
				valueAttr[i]= reinterpret_cast<void*>(&(alarmInfo_attr_.retr_counter[i]));
			}
			break;
		case AL_INFO_ceasePending_attr :
			attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
			for(int i=0; i < attr_value_num; i++){
				valueAttr[i]= reinterpret_cast<void*>(&(alarmInfo_attr_.cease_pending[i]));
			}
			break;
		case AL_INFO_acknowledge_attr :
			attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
			for(int i=0; i < attr_value_num; i++){
				valueAttr[i]= reinterpret_cast<void*>(&(alarmInfo_attr_.acknowledge[i]));
			}
			break;
		case AL_INFO_time_attr :
			attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
			for(int i=0; i < attr_value_num; i++){
				valueAttr[i]= reinterpret_cast<void*>(alarmInfo_attr_.time[i]);
			}
			break;
		default:
			attrib_found = false;
			break;
	}

	if(attrib_found){
		attribute.attrName = const_cast<char*>(IMM_AlarmInfoAttr[attrib].attr_name);
		attribute.attrType = (ACS_CC_AttrValueType) IMM_AlarmInfoAttr[attrib].attr_type;
		attribute.attrValuesNum = attr_value_num;
		attribute.attrValues = valueAttr;

		attrList_.push_back(attribute);
	}
	return attr_value_num;
}



//========================================================================================
//	setALarmInfoAttribute method
//========================================================================================

void acs_alh_imm_data_handler::reset_attributes(void)
{

	strcpy(alarmInfo_attr_.identity_rdn, "");
	strcpy(alarmInfo_attr_.process_name, "");
	strcpy(alarmInfo_attr_.probable_cause, "");
	strcpy(alarmInfo_attr_.category, "");
	strcpy(alarmInfo_attr_.object_of_ref, "");
	strcpy(alarmInfo_attr_.problem_data, "");
	strcpy(alarmInfo_attr_.problem_text, "");
	strcpy(alarmInfo_attr_.severity, "");

	alarmInfo_attr_.specificProblem = 0;
	alarmInfo_attr_.send_priority = 2;

	for (int i = 0; i<MAX_NUMBER_OF_VALUE_ATTR; i++)
	{
		strcpy(alarmInfo_attr_.time[i], "");
		alarmInfo_attr_.cp_alarm_ref[i] = 0;
		alarmInfo_attr_.retr_counter[i] = 0;
		alarmInfo_attr_.cease_pending[i] = 0;
		alarmInfo_attr_.acknowledge[i] = 0;
	}

}


//========================================================================================
//	getAlarmInToList method
//========================================================================================

int acs_alh_imm_data_handler::getAlarmInToList(std::vector<AllRecord>* alarmList, int cpSide){

	int ret_code = 0;
	int call_result = 0;

	const char *imm_alh_root_dn = 0;

        acs_alh_log log_;

         log_.write(LOG_LEVEL_DEBUG,"In getAlarmInToList - Entry ");

	/*Searching the parent name of the class to be defined*/
         log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList  before util_.get_alh_imm_root_dn() ");
	call_result = util_.get_alh_imm_root_dn(imm_alh_root_dn);
	if ( call_result ) {
		setError(call_result);
		char problemData[DEF_MESSIZE] = {0};
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
				call_result);
                log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList error in getAlarmInToList(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
                                call_result);
                om_handler_.Finalize();
                omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "error in checkIfObjectIsPresent: initialisation of omHandler failure ");
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - error in acs_alh_imm_data_handler::getAlarmInToList initialisation of omHandler failure ");
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}


	if(!omHandlerInitialized_ && (ret_code = omHandler_init()) != acs_alh::ERR_NO_ERRORS){
		setError(ret_code);
		char problemData[DEF_MESSIZE] = {0};
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...) - omHandler_init() failed: return code == %d", ret_code);
                log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - error in getAlarmInToList(...) - omHandler_init() failed: return code == %d", ret_code);
		return acs_alh::ERR_SAF_IMM_OM_INIT;
	}

	alarmList->clear();

	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };
	/*The DN name of the parent of object to be defined*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1, ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN"=%s,%s", util_.getlocalNodeName().c_str(), imm_alh_root_dn);


	std::vector<std::string> alarmInfo_name_list;
	// Fix for TR HX25089 starts
	int oiCounter =0, omcounter=0;
	int init_ret_code=0, om_ret_code=0;

		log_.write(LOG_LEVEL_DEBUG,"In getAlarmInToList - try to fetch the DN of AlarmInfo objects from IMM.... before om_handler_.getChildren()");
		ret_code = om_handler_.getChildren(imm_dnName, ACS_APGCC_SUBLEVEL, & alarmInfo_name_list);

		if(ret_code != ACS_CC_SUCCESS){

			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE] = {0};
			snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...): om_handler.getChildren' failed: return code == %d", ret_code);
			log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - error in getAlarmInToList(...): om_handler.getChildren' failed: return code == %d", ret_code);

			do{
				if(!alh_is_stopping){
					acs_alh_imm_runtime_owner temp_OI(immConnectionHandler_);
					sleep(1);
					temp_OI.finalize_IMM();
					init_ret_code = temp_OI.init_IMM( false );
					oiCounter ++;
					log_.write(LOG_LEVEL_WARN,"In getAlarmInToList - init_ret_code value is %d ",init_ret_code);
				}
				else{
					log_.write(LOG_LEVEL_WARN,"ALH stop signal %d is received exiting from loop.",alh_is_stopping);
					break;
				}
			}while((init_ret_code != 0) && (oiCounter < RETRY_COUNTER_MAX_VALUE));

			if((oiCounter >= RETRY_COUNTER_MAX_VALUE) && (init_ret_code != 0)){
				setError(acs_alh::ERR_SAF_IMM_OM_GET);
				char problemData[DEF_MESSIZE];
				snprintf(problemData, sizeof(problemData) - 1, "error in oiHandler_init() ");
				setErrorText(problemData);
				log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - init_IMM failed with retry of 30 times");
				return acs_alh::ERR_SAF_IMM_OM_GET;
			}
			omHandlerInitialized_ = false;
			if( !omHandlerInitialized_ ){
				do {
					if(!alh_is_stopping){
						om_handler_.Finalize();
						om_ret_code = omHandler_init();
						omcounter++;
						if(om_ret_code == 0)
							ret_code = om_handler_.getChildren(imm_dnName, ACS_APGCC_SUBLEVEL, & alarmInfo_name_list);
					}
					else {
						log_.write(LOG_LEVEL_TRACE,"ALH stop signal %d is received exiting from loop",alh_is_stopping);
						break;
					}
				}while((ret_code != ACS_CC_SUCCESS) && (omcounter < RETRY_COUNTER_MAX_VALUE));
				if((omcounter >= RETRY_COUNTER_MAX_VALUE) && (ret_code != ACS_CC_SUCCESS))
				{
					setError(acs_alh::ERR_SAF_IMM_OM_GET);
					char problemData[DEF_MESSIZE];
					snprintf(problemData, sizeof(problemData) - 1, "error in omHandler_init() ");
					setErrorText(problemData);
					log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - error in om_handler.getChildren with retry of 30 times");
					return acs_alh::ERR_SAF_IMM_OM_GET;
				}
			}
		}
		// Fix for TR HX25089 ends
		// try to fetch the DN of AlarmInfo objects from IMM
		/*if ((ret_code = om_handler_.getChildren(imm_dnName, ACS_APGCC_SUBLEVEL, & alarmInfo_name_list)) != ACS_CC_SUCCESS)
		{
			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE] = {0};
			snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...): om_handler.getChildren' failed: return code == %d", ret_code);
			return acs_alh::ERR_SAF_IMM_OM_GET;
		}*/

	// try to fetch the AlarmInfo objects from IMM
	for(int i=0; i<(int)alarmInfo_name_list.size(); i++){

		ACS_APGCC_ImmObject imm_object;
		imm_object.objName = alarmInfo_name_list[i];
		AllRecord alarm;

		if ((ret_code = om_handler_.getObject(&imm_object)) == ACS_CC_SUCCESS){

			unsigned attributes_num = imm_object.attributes.size();

			for(unsigned j=0; j<attributes_num; j++){
				const char *attr_name = imm_object.attributes[j].attrName.c_str();

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_NAME_RDN)){
					char identityString[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = {0};
					alarm.identity[0] = alarm.identity[1] = alarm.identity[2] = 0;
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						strncpy(identityString, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX);

						alarm.identity[0] = atoi(strtok(identityString, ":"));
						alarm.identity[1] = 0;
						alarm.identity[2] = atoi(strtok(0, ":"));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROCESS_NAME)){
					memset ( alarm.event.processName, 0, ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.processName, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBL_CAUSE)){
					memset ( alarm.event.probableCause, 0, ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX);
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.probableCause, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CATEGORY)){
					memset ( alarm.event.objClassOfReference, 0, ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.objClassOfReference, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_OBJ_REF)){
					memset ( alarm.event.objectOfReference, 0, ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.objectOfReference, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_DATA)){
					memset ( alarm.event.problemData, 0, ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.problemData, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_TEXT)){
					memset ( alarm.event.problemText, 0, ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.problemText, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_SEVERITY)){
					memset ( alarm.event.percSeverity, 0, ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX );
					if ( imm_object.attributes[j].attrValuesNum != 0 )
						strncpy(alarm.event.percSeverity, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX);
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_TIME)){
					char time[ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX] = {0};
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						if ( strncpy(time, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]), ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX) ){
							strncpy(time, reinterpret_cast<char *>(imm_object.attributes[j].attrValues[1]), ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX);
						}
					}
					std::string timeStr;
					if(time != 0){
						timeStr = time;
						alarm.time = convertEventTime(timeStr);
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SPEC_PROBL)){
					alarm.event.specificProblem = 0;
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						alarm.event.specificProblem = (long)(*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[0]));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF)){
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						alarm.cpAlarmRef = (unsigned short)(*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[cpSide]));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER)){
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						alarm.retransCnt = (unsigned short)(*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[cpSide]));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SEND_PRIORITY)){
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						alarm.sendPriority = (*reinterpret_cast<unsigned int *>(imm_object.attributes[j].attrValues[0]));
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CEASE_PENDING)){
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						int cease_pending = (*reinterpret_cast<int *>(imm_object.attributes[j].attrValues[cpSide]));
						if( cease_pending == 1 ){
							alarm.ceasePending = true;
						}else {
							alarm.ceasePending = false;
						}
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE)){
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						int ack = (*reinterpret_cast<int *>(imm_object.attributes[j].attrValues[cpSide]));
						if( ack == 1 ){
							alarm.acknowledge = true;
						}else {
							alarm.acknowledge = false;
						}
					}
					continue;
				}

				if (!strcmp(attr_name, ACS_ALH_CONFIG_IMM_ALARM_INFO_MANUAL_CEASE)){
					if ( imm_object.attributes[j].attrValuesNum != 0 ){
						int manual_cease = (*reinterpret_cast<int *>(imm_object.attributes[j].attrValues[cpSide]));
						if( manual_cease == 1 ){
							alarm.manualCease = true;
						}else {
							alarm.manualCease = false;
						}
					}
					continue;
				}
			}
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE] = {0};
			snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...): om_handler.getObject(...' failed to retrieve the object '%s': return code == %d",
					imm_object.objName.c_str(), ret_code);
                        om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...): om_handler.getObject(...' failed to retrieve the object '%s': return code == %d",
                                        imm_object.objName.c_str(), ret_code);
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - error in getAlarmInToList(...): om_handler.getObject(...' failed to retrieve the object '%s': return code == %d",
                                        imm_object.objName.c_str(), ret_code);
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
			return acs_alh::ERR_SAF_IMM_OM_GET;
		}

		alarmList->push_back(alarm);
	}

         log_.write(LOG_LEVEL_DEBUG,"In getAlarmInToList - exit");

	return ret_code;

}



//========================================================================================
//	updateCeasePending method
//========================================================================================

int acs_alh_imm_data_handler::updateCeasePending(const char* rd_name, int value, int cpSide)
{

	// check parameters
	if( !rd_name || !*rd_name){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in updateCeasePending: rd_name parameter empty ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}

	int ret_code = 0;
	int imm_ret_code = 0;
	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

	const char * imm_alh_root_dn = 0;
        acs_alh_log log_;

	/*Searching the parent name of the class to be defined*/
	ret_code = util_.get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		setError(ret_code);
		char problemData[DEF_MESSIZE] = { 0 };
		snprintf(problemData, sizeof(problemData) - 1, "error in getAlarmInToList(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
				ret_code);
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	/*The DN name of the object to modify*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1, "%s,%s=%s,%s",
			rd_name,ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, util_.getlocalNodeName().c_str(), imm_alh_root_dn);

	/*Check if the IMM handler is already initialized otherwise initialize it*/
	if( !omHandlerInitialized_ ){
		if( omHandler_init() != 0 ){
			setError(acs_alh::ERR_SAF_IMM_OM_INIT);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "error in updateCeasePending: initialisation of omHandler failure ");
			setErrorText(problemData);
			return acs_alh::ERR_SAF_IMM_OM_INIT;
		}
	}

	/*Get the old value of CEASE_PENDING attribute before change it*/
	ACS_CC_ImmParameter paramBeforeChange;
	int ceasePendingValue[MAX_NUMBER_OF_VALUE_ATTR] = { 0 };
	paramBeforeChange.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CEASE_PENDING);


	ret_code = om_handler_.getAttribute(imm_dnName, &paramBeforeChange );
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = om_handler_.getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):getAttribute failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):getAttribute failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
                        om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):getAttribute failed returnCode =%d", imm_dnName, ret_code);
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - updateCeasePending(%s):getAttribute failed returnCode =%d", imm_dnName, ret_code);
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
			ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		}

		return ret_code;
	}


	if( ret_code == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramBeforeChange.attrValuesNum; i++){
			ceasePendingValue[i] = *(int*)paramBeforeChange.attrValues[i];
		}
	}

	ceasePendingValue[cpSide] = value;

	/*Modify the value of CEASE_PENDING attribute*/
	ACS_CC_ImmParameter param2modify;
	void* valueAttr[MAX_NUMBER_OF_VALUE_ATTR];

	int attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
	for(int i=0; i < attr_value_num; i++){
		valueAttr[i]= reinterpret_cast<void*>(&(ceasePendingValue[i]));
	}

	param2modify.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CEASE_PENDING);
	param2modify.attrType = (ACS_CC_AttrValueType) IMM_AlarmInfoAttr[13].attr_type;
	param2modify.attrValuesNum = attr_value_num;
	param2modify.attrValues = valueAttr;

	ret_code = immConnectionHandler_->modifyRuntimeObj(imm_dnName, &param2modify);
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = immConnectionHandler_->getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):getAttribute failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_MODIFY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_OM_MODIFY;
		}

		return ret_code;
	}

	return ret_code;
}



//========================================================================================
//	updateCeasePending method
//========================================================================================

int acs_alh_imm_data_handler::ackAlarm(const char* rd_name, unsigned short cpRef, int ack, int cpSide)
{

	// check parameters
	if( !rd_name || !*rd_name){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in ackAlarm: rd_name parameter empty ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}

	int ret_code = 0;
	int imm_ret_code = 0;
	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

	acs_alh_log log_;

	log_.write(LOG_LEVEL_ERROR,"ackAlarm() Start - object : %s , ack : %i, cpSide : %i",rd_name,ack, cpSide);

	const char * imm_alh_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	ret_code = util_.get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		setError(ret_code);
		char problemData[DEF_MESSIZE] = { 0 };
		snprintf(problemData, sizeof(problemData) - 1, "error in ackAlarm(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
				ret_code);
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	/*The DN name of the object to modify*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1, "%s,%s=%s,%s",
			rd_name,ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, util_.getlocalNodeName().c_str(), imm_alh_root_dn);


	/*Check if the IMM handler is already initialized otherwise initialize it*/
	if( !omHandlerInitialized_ ){
		if( omHandler_init() != 0 ){
			setError(acs_alh::ERR_SAF_IMM_OM_INIT);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "error in ackAlarm: initialisation of omHandler failure ");
			setErrorText(problemData);
			return acs_alh::ERR_SAF_IMM_OM_INIT;
		}
	}


	/*Get the old value of CP_ALARM_REFERENCE attribute and save locally before change it*/
	ACS_CC_ImmParameter paramcpRefBeforeChange;
	unsigned int cpReference[MAX_NUMBER_OF_VALUE_ATTR] = { 0 };
	paramcpRefBeforeChange.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF);

	ret_code = om_handler_.getAttribute(imm_dnName, &paramcpRefBeforeChange );
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = om_handler_.getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF, imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF, imm_dnName, ret_code);
			setErrorText(problemData);
                        om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF, imm_dnName, ret_code);
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF, imm_dnName, ret_code);
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
			ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		}

		return ret_code;
	}


	if( ret_code == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramcpRefBeforeChange.attrValuesNum; i++){
			cpReference[i] = *(int*)paramcpRefBeforeChange.attrValues[i];
		}
	}


	/*Get from IMM the old value of ACKNOWLEDGE flag and save locally before change it */
	ACS_CC_ImmParameter paramAckBeforeChange;
	int ackValue[MAX_NUMBER_OF_VALUE_ATTR] = {0};
	paramAckBeforeChange.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE);

	ret_code = om_handler_.getAttribute(imm_dnName, &paramAckBeforeChange );
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = om_handler_.getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
			setErrorText(problemData);
                        om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - ackAlarm(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
			ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		}

		return ret_code;

	}

	if( ret_code == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAckBeforeChange.attrValuesNum; i++){
			ackValue[i] = *(int*)paramAckBeforeChange.attrValues[i];
		}
	}


	//Update the value of cpReference and acknowledge
	cpReference[cpSide] = (unsigned int)cpRef;
	ackValue[cpSide] = ack;


	/*Modify the value of CP_ALARM_REFERENCE attribute*/
	ACS_CC_ImmParameter param2modify_cpRef;
	void* valueAttr[MAX_NUMBER_OF_VALUE_ATTR];

	int attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
	for(int i=0; i < attr_value_num; i++){
		valueAttr[i]= reinterpret_cast<void*>(&(cpReference[i]));
	}

	param2modify_cpRef.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF);
	param2modify_cpRef.attrType = (ACS_CC_AttrValueType) IMM_AlarmInfoAttr[11].attr_type;
	param2modify_cpRef.attrValuesNum = attr_value_num;
	param2modify_cpRef.attrValues = valueAttr;

	ret_code = immConnectionHandler_->modifyRuntimeObj(imm_dnName, &param2modify_cpRef);
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = immConnectionHandler_->getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_MODIFY);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_OM_MODIFY;
		}

		return ret_code;
	}


	/*Modify the value of ACKNOWLEDGE attribute*/
	ACS_CC_ImmParameter param2modify_ack;
	for(int i=0; i < attr_value_num; i++){
		valueAttr[i]= reinterpret_cast<void*>(&(ackValue[i]));
	}

	param2modify_ack.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE);
	param2modify_ack.attrType = (ACS_CC_AttrValueType) IMM_AlarmInfoAttr[14].attr_type;
	param2modify_ack.attrValuesNum = attr_value_num;
	param2modify_ack.attrValues = valueAttr;

	ret_code = immConnectionHandler_->modifyRuntimeObj(imm_dnName, &param2modify_ack);
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = immConnectionHandler_->getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_MODIFY);
			char problemData[DEF_MESSIZE] = { 0 };
			snprintf(problemData, sizeof(problemData) - 1, "updateCeasePending(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_OM_MODIFY;
		}

		return ret_code;
	}


	return ret_code;
}




//========================================================================================
//	removeAlarmFromIMM method
//========================================================================================

int acs_alh_imm_data_handler::removeAlarmFromIMM(const char *rdName, bool separated){

	//returnValue:
	// 2 exit and remove the object from IMM without error
	// 0 exit without error , without remove the object from IMM
	// <0 exit whit error

	int ret_code = 0;
	int counter = 0;
	int imm_ret_code = 0;
	//	ACS_CC_ReturnType imm_result;

	if(!rdName || !*rdName ){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in removeAlarmListNode: empty parameters ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}

	acs_alh_log log_;

	log_.write(LOG_LEVEL_ERROR,"removeAlarmFromIMM() Start - object : %s , separated : %s",rdName,separated ? "true" : "false");

	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

	const char * imm_alh_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	ret_code = util_.get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		setError(ret_code);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in removeAlarmFromIMM(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
				ret_code);
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	/*The DN name of the object to modify*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1, "%s,%s=%s,%s",
			rdName,ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, util_.getlocalNodeName().c_str(), imm_alh_root_dn);

	//printf("IN removeAlarmFromIMM rdnObject: %s\n", imm_dnName);


	if ( separated ){
		//The CP working in SEPARATED mode

		/*Get from IMM the old value of ACKNOWLEDGE flag for the alarm and save locally before change it */
		ACS_CC_ImmParameter paramAck;
		int ackValue[MAX_NUMBER_OF_VALUE_ATTR];
		paramAck.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE);

		ret_code = om_handler_.getAttribute( imm_dnName, &paramAck );
		if(ret_code != ACS_CC_SUCCESS){
			imm_ret_code = om_handler_.getInternalLastError();
			if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
				setError(acs_alh::ERR_SAF_IMM_RETRY);
				char problemData[DEF_MESSIZE];
				snprintf(problemData, sizeof(problemData) - 1, "removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
				setErrorText(problemData);
				ret_code = acs_alh::ERR_SAF_IMM_RETRY;
			}else{
				setError(acs_alh::ERR_SAF_IMM_OM_GET);
				char problemData[DEF_MESSIZE];
				snprintf(problemData, sizeof(problemData) - 1, "removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
				setErrorText(problemData);
                                om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
				ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
			}
			return ret_code;
		}


		if( ret_code == ACS_CC_SUCCESS ){
			for(unsigned int i=0; i<paramAck.attrValuesNum; i++){
				ackValue[i] = *(int*)paramAck.attrValues[i];
			}
		}


		/*Check if there is a cease item for the alarm. If there is check the value of ACKNOWLEDGE flag for the item to decide
		 * if the alarm can be deleted */
		char imm_dnNameCeasing[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

		snprintf(imm_dnNameCeasing, sizeof(imm_dnName) - 1, "%s:C,%s=%s,%s",
				rdName,ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, util_.getlocalNodeName().c_str(), imm_alh_root_dn);

		//printf("IN removeAlarmFromIMM rdn of ceasing Object: %s\n", imm_dnNameCeasing);

		ACS_CC_ImmParameter paramAckCease;
		int ackCeaseValue[MAX_NUMBER_OF_VALUE_ATTR] = { 0, 0 };


		if(strstr(imm_dnNameCeasing, "C:C")==0){
			/*Get from IMM the old value of ACKNOWLEDGE flag for the cease item and save locally before change it */
			paramAckCease.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE);
			ret_code = om_handler_.getAttribute( imm_dnNameCeasing, &paramAckCease );

			if(ret_code != ACS_CC_SUCCESS){
				imm_ret_code = om_handler_.getInternalLastError();
				if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
					setError(acs_alh::ERR_SAF_IMM_RETRY);
					char problemData[DEF_MESSIZE];
					snprintf(problemData, sizeof(problemData) - 1, "removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
					setErrorText(problemData);
					ret_code = acs_alh::ERR_SAF_IMM_RETRY;
				}else{
					setError(acs_alh::ERR_SAF_IMM_OM_GET);
					char problemData[DEF_MESSIZE];
					snprintf(problemData, sizeof(problemData) - 1, "removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
					setErrorText(problemData);
                                        om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - removeAlarmFromIMM(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
					ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
				}
				return ret_code;
			}

			if( ret_code == ACS_CC_SUCCESS ){
				for(unsigned int i=0; i<paramAckCease.attrValuesNum; i++){
					ackCeaseValue[i] = *(int*)paramAckCease.attrValues[i];
				}
			}
		}


		//========================================================================================================================

		//		printf("CONTROLLO I VALORI DI ACK\n");
		//		printf("IN removeAlarmFromIMM per %s - ackCeaseValue[0]: %d\n", rdName, ackCeaseValue[0]);
		//		printf("IN removeAlarmFromIMM per %s - ackCeaseValue[1]: %d\n", rdName, ackCeaseValue[1]);
		//		printf("IN removeAlarmFromIMM per %s - ackValue[0]: %d\n", rdName, ackValue[0]);
		//		printf("IN removeAlarmFromIMM per %s - ackValue[1]: %d\n", rdName, ackValue[1]);

		//========================================================================================================================

		log_.write(LOG_LEVEL_ERROR,"ackValue[0] = %i",ackValue[0]);
		log_.write(LOG_LEVEL_ERROR,"ackValue[1] = %i",ackValue[1]);
		log_.write(LOG_LEVEL_ERROR,"ackCeaseValue[0] = %i",ackCeaseValue[0]);
		log_.write(LOG_LEVEL_ERROR,"ackCeaseValue[1] = %i",ackCeaseValue[1]);

		if( (ackValue[0] == 1) && (ackValue[1] == 1)  && (ackCeaseValue[0] == ackCeaseValue[1] )){

			//NEW VERSION
			do{
				acs_alh_imm_runtime_owner temp_OI(immConnectionHandler_);

				ret_code = immConnectionHandler_->deleteRuntimeObj(imm_dnName);

				if ( ret_code != ACS_CC_SUCCESS ){

					imm_ret_code = immConnectionHandler_->getInternalLastError();

					if ( ( imm_ret_code == -5 ) || ( imm_ret_code == -6 ) || ( imm_ret_code == -10 ) ){
						log_.write(LOG_LEVEL_ERROR,"removeAlarmFromIMM - deleteRuntimeObj failed ( %i )- Object %s - try %i", imm_ret_code, imm_dnName, counter );
						sleep(2);
						counter++;
					}
					else if ( imm_ret_code == -9 ) {
						log_.write(LOG_LEVEL_ERROR,"removeAlarmFromIMM - deleteRuntimeObj failed ( %i )- Object %s - try %i", imm_ret_code, imm_dnName, counter );
						sleep(1);
						temp_OI.finalize_IMM();
						temp_OI.init_IMM( false );
						counter++;
					}
					else{
						setError(acs_alh::ERR_SAF_IMM_OM_DELETE);
						char problemData[DEF_MESSIZE] = {0};
						snprintf(problemData, sizeof(problemData) - 1, "acs_alh_imm_data_handler.removeAlarmFromIMM - deleteRuntimeObj failed ");
						ret_code = acs_alh::ERR_SAF_IMM_OM_DELETE;
						break;
					}
				}
				else{
					break;
				}
			}while((ret_code != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

			if ( ret_code != 0  ){
				return ret_code;
			}

			return 2;

		}else{
			//Object not eliminated because attend the acknowledge from other cpSide
			//printf("ESCO DA removeAlarmFromIMM senza Cancellare l'oggetto per %s\n", rdName);
			return ret_code;
		}

	}else{ //The CP working in NORMAL mode

		//NEW VERSION
		do{
			acs_alh_imm_runtime_owner temp_OI(immConnectionHandler_);

			ret_code = immConnectionHandler_->deleteRuntimeObj(imm_dnName);

			if(ret_code != ACS_CC_SUCCESS){
				imm_ret_code = immConnectionHandler_->getInternalLastError();
				if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
					log_.write(LOG_LEVEL_ERROR,"removeAlarmFromIMM - deleteRuntimeObj failed ( %i )- Object %s - try %i", imm_ret_code, imm_dnName, counter );
					sleep(2);
					counter++;
					setError(acs_alh::ERR_SAF_IMM_RETRY);
					char problemData[DEF_MESSIZE];
					snprintf(problemData, sizeof(problemData) - 1, "acs_alh_imm_data_handler.removeAlarmFromIMM - deleteRuntimeObj failed ");
					ret_code = acs_alh::ERR_SAF_IMM_RETRY;
				}
				else if ( imm_ret_code == -9 ){
					log_.write(LOG_LEVEL_ERROR,"removeAlarmFromIMM - deleteRuntimeObj failed ( %i )- Object %s - try %i", imm_ret_code, imm_dnName, counter );
					sleep(1);
					temp_OI.finalize_IMM();
					temp_OI.init_IMM(false);
					counter++;
				}
				else{
					setError(acs_alh::ERR_SAF_IMM_OM_DELETE);
					char problemData[DEF_MESSIZE];
					snprintf(problemData, sizeof(problemData) - 1, "acs_alh_imm_data_handler.removeAlarmFromIMM - deleteRuntimeObj failed ");
					ret_code = acs_alh::ERR_SAF_IMM_OM_DELETE;
				}
			}
			else{
				break;
			}

		}while((ret_code != 0) && (counter<RETRY_COUNTER_MAX_VALUE));

		if(ret_code !=0){
			return ret_code;
		}
	}//end else

	return 2;

}



//========================================================================================
//	unAckEvent method
//========================================================================================

int acs_alh_imm_data_handler::unAckEvent(const char *rdName, int cpSide){

	// check parameters
	if( !rdName || !*rdName){
		setError(acs_alh::ERR_INPUT_PARAMETER_EMPTY);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in unAckEvent: rdName parameter empty ");
		setErrorText(problemData);
		return acs_alh::ERR_INPUT_PARAMETER_EMPTY;
	}


	int ret_code = 0;
        acs_alh_log log_;
	int imm_ret_code = 0;
	char imm_dnName[ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX] = { 0 };

	const char * imm_alh_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	ret_code = util_.get_alh_imm_root_dn(imm_alh_root_dn);
	if ( ret_code ) {
		setError(ret_code);
		char problemData[DEF_MESSIZE];
		snprintf(problemData, sizeof(problemData) - 1, "error in unAckEvent(...) - Call to 'acs_alh_util::get_alh_imm_root_dn(...' failed: while searching for ALH root parameter subtree in IMM: return code == %d",
				ret_code);
		return acs_alh::ERR_SAF_IMM_OM_GET;
	}

	/*The DN name of the object to modify*/
	snprintf(imm_dnName, sizeof(imm_dnName) - 1, "%s,%s=%s,%s",
			rdName,ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN, util_.getlocalNodeName().c_str(), imm_alh_root_dn);

	//printf("unAckEvent: rdnObject: %s\n", imm_dnName);


	/*Check if the IMM handler is already initialized otherwise initialize it*/
	if( !omHandlerInitialized_ ){
		if( omHandler_init() != 0 ){
			setError(acs_alh::ERR_SAF_IMM_OM_INIT);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "error in unAckEvent: initialisation of omHandler failure ");
			setErrorText(problemData);
			return acs_alh::ERR_SAF_IMM_OM_INIT;
		}
	}


	/*Get from IMM the old value of ACKNOWLEDGE flag and save locally before change it */
	ACS_CC_ImmParameter paramAckBeforeChange;
	int ack[MAX_NUMBER_OF_VALUE_ATTR];
	paramAckBeforeChange.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE);

	ret_code = om_handler_.getAttribute(imm_dnName, &paramAckBeforeChange );
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = om_handler_.getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);
			setErrorText(problemData);
                        om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);

                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE, imm_dnName, ret_code);

                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
			ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		}

		return ret_code;
	}


	if( ret_code == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAckBeforeChange.attrValuesNum; i++){
			ack[i] = *(int*)paramAckBeforeChange.attrValues[i];
		}
	}


	/*Get from IMM the old value of RETRASMISSION_COUNTER attribute and save locally before change it */
	ACS_CC_ImmParameter paramRetrCounterBeforeChange;
	int retrCoValue[MAX_NUMBER_OF_VALUE_ATTR];
	paramRetrCounterBeforeChange.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER);

	ret_code = om_handler_.getAttribute(imm_dnName, &paramRetrCounterBeforeChange );
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = om_handler_.getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER, imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_GET);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER, imm_dnName, ret_code);
			setErrorText(problemData);
                        om_handler_.Finalize();
                        omHandlerInitialized_ = false;


                        if( !omHandlerInitialized_ ){
                                if( omHandler_init() != 0 ){
                                        setError(acs_alh::ERR_SAF_IMM_OM_INIT);
                                        char problemData[DEF_MESSIZE];
                                        snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER, imm_dnName, ret_code);
                                        setErrorText(problemData);
                                        log_.write(LOG_LEVEL_ERROR,"In getAlarmInToList - unAckEvent(%s):getAttribute %s failed returnCode =%d", ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER, imm_dnName, ret_code);
                                        return acs_alh::ERR_SAF_IMM_OM_INIT;
                                }
                        }
			ret_code = acs_alh::ERR_SAF_IMM_OM_GET;
		}

		return ret_code;
	}

	if( ret_code == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramRetrCounterBeforeChange.attrValuesNum; i++){
			retrCoValue[i] = *(int*)paramRetrCounterBeforeChange.attrValues[i];
		}
	}


	//Update the value of cpReference and acknowledge
	retrCoValue[cpSide] = 0;
	ack[cpSide] = 0;


	/*Modify the value of ACKNOWLEDGE attribute*/
	ACS_CC_ImmParameter param2modify_ackValue;
	void* valueAttr[MAX_NUMBER_OF_VALUE_ATTR];

	int attr_value_num = MAX_NUMBER_OF_VALUE_ATTR;
	for(int i=0; i < attr_value_num; i++){
		valueAttr[i]= reinterpret_cast<void*>(&(ack[i]));
	}

	param2modify_ackValue.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE);
	param2modify_ackValue.attrType = (ACS_CC_AttrValueType) IMM_AlarmInfoAttr[14].attr_type;
	param2modify_ackValue.attrValuesNum = attr_value_num;
	param2modify_ackValue.attrValues = valueAttr;

	ret_code = immConnectionHandler_->modifyRuntimeObj(imm_dnName, &param2modify_ackValue);
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = immConnectionHandler_->getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_MODIFY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_OM_MODIFY;
		}

		return ret_code;
	}



	/*Modify the value of RETRASMISSION_COUNTER attribute*/
	ACS_CC_ImmParameter param2modify_RetrCounter;
	for(int i=0; i < attr_value_num; i++){
		valueAttr[i]= reinterpret_cast<void*>(&(retrCoValue[i]));
	}

	param2modify_RetrCounter.attrName = const_cast<char*>(ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER);
	param2modify_RetrCounter.attrType = (ACS_CC_AttrValueType) IMM_AlarmInfoAttr[12].attr_type;
	param2modify_RetrCounter.attrValuesNum = attr_value_num;
	param2modify_RetrCounter.attrValues = valueAttr;

	ret_code = immConnectionHandler_->modifyRuntimeObj(imm_dnName, &param2modify_RetrCounter);
	if(ret_code != ACS_CC_SUCCESS){
		imm_ret_code = immConnectionHandler_->getInternalLastError();
		if((imm_ret_code == -5) || (imm_ret_code == -6) || (imm_ret_code == -10)){
			setError(acs_alh::ERR_SAF_IMM_RETRY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_RETRY;
		}else{
			setError(acs_alh::ERR_SAF_IMM_OM_MODIFY);
			char problemData[DEF_MESSIZE];
			snprintf(problemData, sizeof(problemData) - 1, "unAckEvent(%s):modifyRuntimeObj failed returnCode =%d", imm_dnName, ret_code);
			setErrorText(problemData);
			ret_code = acs_alh::ERR_SAF_IMM_OM_MODIFY;
		}

		return ret_code;
	}

	return ret_code;
}



//-------------------------------------------------------------------------------------------------------------------------------------------------
//convertEventTime method
//-------------------------------------------------------------------------------------------------------------------------------------------------

time_t acs_alh_imm_data_handler::convertEventTime(std::string eventTime){

	struct tm when;

	int yy = 0;
	int mm = 0;
	int dd = 0;
	int hh = 0;
	int m = 0;
	int ss = 0;

	sscanf(eventTime.c_str(),"%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hh, &m, &ss);
	// years since 1900
	when.tm_year = yy - 1900;
	// months since January
	when.tm_mon = mm - 1;
	// day of the month
	when.tm_mday = dd;

	when.tm_sec = ss;

	when.tm_min = m;

	when.tm_hour = hh;

	when.tm_isdst = -1; // autodetect DST

	return mktime(&when);

}
