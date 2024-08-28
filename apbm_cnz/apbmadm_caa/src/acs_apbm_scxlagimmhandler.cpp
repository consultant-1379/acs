/** @file acs_apbm_scxlagimmhandler.cpp
 *      @brief
 *      @author XSUNACH (Anil Achary)
 *      @date 2015-03-11
 *
 *      COPYRIGHT Ericsson AB, 2015
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 *      REVISION INFO
 *      +=======+============+==============+=====================================+
 *      | REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *      +=======+============+==============+=====================================+
 *      | R-001 | 2015-03-11 | xsunach      | CNI 33/6-441                        |
 *      +=======+============+==============+=====================================+
 */

#include "acs_apbm_scxlagimmhandler.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_logger.h"

acs_apbm_scxlagimmhandler::acs_apbm_scxlagimmhandler(acs_apbm_serverworkingset * serverWorkingSet) {

	m_serverWorkingSet = NULL;
    if (serverWorkingSet != NULL) {
    	m_serverWorkingSet = serverWorkingSet;
    }

    m_snmpManager = NULL;
    lagImmImplementerThread = NULL;
    m_openDone = false;
}
acs_apbm_scxlagimmhandler::~acs_apbm_scxlagimmhandler() {
	//To close HWI use following methods :
	//closeListener();
	//closeClassImplementer();
	//closeImmRunTimeHandler();
}
int acs_apbm_scxlagimmhandler::openScxLagHandler(){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Begin");

	if (m_openDone) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Open already done !");
		return acs_apbm::ERR_NO_ERRORS;
	}

	if (m_serverWorkingSet != NULL) {
		if (m_serverWorkingSet->snmp_manager != NULL) {
			m_snmpManager = m_serverWorkingSet->snmp_manager;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: SNMP configured");
		} else {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "LAG IMM HANDLING: SNMP is NULL !!!");
		}
	}

	int res = acs_apbm::ERR_NO_ERRORS;
	do {
		res = this->openClassImplementer();
		if (res) { // ERROR: cannot install the HWI configuration object implementer
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: Call 'openClassImplementer' failed: res == %d", res);
			break;
		}

		res = this->openListener();
		if (res) { // ERROR:
			this->closeClassImplementer();
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: Call 'openListener' failed: res == %d", res);
			break;
		}
	} while (0); //dummy while

	//Start up is completely done!
	if (res == acs_apbm::ERR_NO_ERRORS) {
		m_immConfingCloseOK = false;
		m_openDone = true;
	}

	return res;
}
int acs_apbm_scxlagimmhandler::closeScxLagHandler(){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Begin");

	if (! m_openDone) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: HWI is already closed !");
		return acs_apbm::ERR_NO_ERRORS;
	}

	int res = acs_apbm::ERR_NO_ERRORS;
	do {
		res = this->closeListener();
		if (res) { // ERROR:
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: Call 'openListener' failed: res == %d", res);
			break;
		}

		if (! m_immConfingCloseOK) {
			res = this->closeClassImplementer();
			if (res) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: Call 'closeClassImplementer' failed: res == %d", res);
				break;
			} else {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: closeClassImplementer done !");
				m_immConfingCloseOK = true;
			}
		}
	} while (0); //dummy while

	if (res == acs_apbm::ERR_NO_ERRORS)
		m_openDone = false;

	return res;
}

int acs_apbm_scxlagimmhandler::openListener() {

	int res = 0;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Begin");
	//Start thread listener
	lagImmImplementerThread = new lagImplementerThread(this);

	if (lagImmImplementerThread->activate() >= 0 )
		res = acs_apbm::ERR_NO_ERRORS;
	else
		res = acs_apbm::ERR_HWI_IMMLISTENER_FAILED;

	return res;
}
int acs_apbm_scxlagimmhandler::closeListener() {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Begin");
	if (lagImmImplementerThread != NULL) {
		lagImmImplementerThread->stop();
		usleep(LAG_IMPLEMENTER_THREAD_POLL_TIMEOUT_MS * 1000 + 500000);
		delete lagImmImplementerThread;
		lagImmImplementerThread = NULL;
	}
	return 0;
}
int acs_apbm_scxlagimmhandler::openClassImplementer() {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Begin");

	//Set implementer
	this->setImpName(LAG_IMPLEMENTER_NAME);
	this->setScope(ACS_APGCC_ONE);

	//retry loop
	ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;
	for (int i = 0 ; i < 3 ; i++) {
		returnCode = oiHandler.addClassImpl(this, IMMCLASSNAME_LAG.c_str());
		if (returnCode == ACS_CC_SUCCESS) {
			i = 3;
		}
		else {
                          usleep(500000);

		}
	}
	if (returnCode == ACS_CC_FAILURE) {
		return acs_apbm::ERR_HWI_IMMCFG_FAILED;
	}
	return acs_apbm::ERR_NO_ERRORS;
}
int acs_apbm_scxlagimmhandler::closeClassImplementer() {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Begin");
	ACS_CC_ReturnType res = ACS_CC_FAILURE;

	//retry loop
	for (int i = 0 ; i < 3 ; i++) {
		res = oiHandler.removeClassImpl(this,IMMCLASSNAME_LAG.c_str());
		if (res == ACS_CC_SUCCESS) {
			i = 3;
		}
		else {
                             usleep(500000);

		}
	}
	if (res == ACS_CC_FAILURE) {
		return acs_apbm::ERR_HWI_IMMCFG_FAILED;
	}
	return acs_apbm::ERR_NO_ERRORS;
}
void acs_apbm_scxlagimmhandler::createIMMLagClass() {

	ACS_CC_ReturnType result;
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	OmHandler omHandler;
	result = omHandler.Init();
	if( result != ACS_CC_SUCCESS ){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: createIMMLagClass INIT failed");
		std::cout << __FUNCTION__ << "@" << __LINE__ << " INIT failed " << std::endl;
		return;
	}

	vector<ACS_CC_AttrDefinitionType> vecClass;

	defineLagAttributes(vecClass);
	result = omHandler.defineClass( const_cast<char *> (IMMCLASSNAME_LAG.c_str()), categoryClassToBeDefined, vecClass);

	if( result != ACS_CC_SUCCESS ){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: AxeEquipmentLag not created");
		std::cout << " Class: " << IMMCLASSNAME_LAG << " not created " << std::endl;
		return;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: AxeEquipmentLag successfully created");
	std::cout << " Class: " << IMMCLASSNAME_LAG << " successfully created" << std::endl;

	vecClass.clear();

	result = omHandler.Finalize();
}
void acs_apbm_scxlagimmhandler::defineLagAttributes (vector<ACS_CC_AttrDefinitionType> &classAttributes) {

	ACS_CC_AttrDefinitionType rdnlagName = {const_cast<char *>("lagId"),ATTR_STRINGT,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType strAdminState = {const_cast<char*>("administrativeState"),ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType strOperState = {const_cast<char*>("operationalState"),ATTR_INT32T,ATTR_RUNTIME|ATTR_CACHED,0};

	classAttributes.push_back(rdnlagName);
	classAttributes.push_back(strAdminState);
	classAttributes.push_back(strOperState);
}
int acs_apbm_scxlagimmhandler::createLAGRootClass(const char* my_shelf_addr,int slot) {

	OmHandler omHandler;
	ACS_CC_ReturnType res;

	res = omHandler.Init();
	if (res == ACS_CC_FAILURE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: omhandler init failed ");
		return -1;
	}
	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> attrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	std::string parentDN;

	char attrdn[]= "lagId";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_STRINGT;
	attrRdn.attrValuesNum = 1;
	char* rdnValue;
	if(slot == acs_apbm::BOARD_SCXB_LEFT){
		rdnValue = const_cast<char *>(RDN_IMMLAGROOT.c_str());
		parentDN = "otherBladeId=0,shelfId=" + std::string(my_shelf_addr) + IMMHWMGMTPATH;
	}else if(slot == acs_apbm::BOARD_SCXB_RIGHT){
		rdnValue = const_cast<char *>(RDN_IMMLAGROOT.c_str());
		parentDN = "otherBladeId=25,shelfId=" + std::string(my_shelf_addr) + IMMHWMGMTPATH;
	}
	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] = reinterpret_cast<void*>(rdnValue);

	attrList.push_back(attrRdn);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: lag object parent DN:%s",parentDN.c_str());

	res = omHandler.createObject(const_cast<char *> (IMMCLASSNAME_LAG.c_str()), parentDN.c_str(), attrList );
	if (res == ACS_CC_FAILURE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: Object :%s failed to create",parentDN.c_str());
		int last_error = omHandler.getInternalLastError();
		omHandler.Finalize();
		if(last_error == -14) //already exist
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: lag object already exist DN:%s",parentDN.c_str());
			return -14;
		}
		return -1;
	}

	omHandler.Finalize();

	return 0;
}
ACS_CC_ReturnType acs_apbm_scxlagimmhandler::updateRuntime(
									const char* objName,
									const char** p_nameAttr){

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: updateRuntime p_objName :%s; p_attrName:%s ",objName, p_nameAttr);

	int slot_number;
	acs_apbm_snmp::frontPort_status_t status;

	char objtemp[128] = {0};
	::strcpy(objtemp, objName);

	// Parse lag object DN
	char * token = ::strtok(objtemp, "=,");
	token = ::strtok(0, "=,");
	token = ::strtok(0, "=,");
	token = ::strtok(0, "=,");

	slot_number = ::atoi(token);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: updateRuntime callback slot_number:%d",slot_number);

// get LAG enabled or disabled from SNMP here....
	ACS_CC_ImmParameter parToModify;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    int scxLagState = m_snmpManager->get_lag_operative_state(slot_number);
    int call_result = m_snmpManager->get_scx_lag_vitural_port_status(slot_number,status);
    if(!call_result){
    	switch (status)
    	{
    	case acs_apbm_snmp::FRONT_PORT_UP:
    		scxLagState = acs_apbm_snmp::ENABLE_LAG;
    		break;
    	case acs_apbm_snmp::FRONT_PORT_DOWN:
    		scxLagState = acs_apbm_snmp::DISABLE_LAG;
    		break;
    	default:
    		break;

    	}
    }else if(call_result == -1981){
    	scxLagState = acs_apbm_snmp::DISABLE_LAG;
    }

    if(!m_snmpManager->get_lag_admin_state(slot_number)){
    	scxLagState = acs_apbm_snmp::DISABLE_LAG;
    }

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: updateRuntime callback scxLagState:%d",scxLagState);

	//int * lagState = new int(scxLagState);
	parToModify.attrName = (ACE_TCHAR *)OPERATIONAL_STATE_ATTR;
	parToModify.attrType = ATTR_INT32T;
	parToModify.attrValuesNum = 1;
//	void* lagState1[1]={reinterpret_cast<void*>(lagState)};
//	parToModify.attrValues =lagState1;
	parToModify.attrValues=new void*[parToModify.attrValuesNum];
	parToModify.attrValues[0] =reinterpret_cast<void*>(&scxLagState);


	if(this->modifyRuntimeObj(objName, &parToModify) == ACS_CC_FAILURE)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: Call modifyRuntimeObj failed");
		result =  ACS_CC_FAILURE;
	}
	else
	{
		m_snmpManager->set_lag_operative_state(slot_number,scxLagState);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Call 'modifyRuntimeObj' successful");
	}
	delete [] parToModify.attrValues;
	return ACS_CC_SUCCESS;
}
ACS_CC_ReturnType acs_apbm_scxlagimmhandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: modify callback oiHandle:%ld ccbId:%ld objName:%s\n",oiHandle,ccbId,objName);
	uint32_t administrativeState;
    int slot_number,call_result = -1;

	char objtemp[128] = {0};
	::strcpy(objtemp, objName);

	// Parse lag object DN
	char * token = ::strtok(objtemp, "=,");
	token = ::strtok(0, "=,");
	token = ::strtok(0, "=,");
	token = ::strtok(0, "=,");

	slot_number = ::atoi(token);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: modify callback slot_number:%d",slot_number);

	if (attrMods[0]) {

		if (!::strcmp(attrMods[0]->modAttr.attrName, ADMINSTRATIVE_STATE_ATTR)) {

			administrativeState = *(reinterpret_cast<unsigned int *> (attrMods[0]->modAttr.attrValues[0]));
			switch ( administrativeState )
			{
			case UNLOCKED:
			{
				if(m_snmpManager->get_lag_admin_state(slot_number))
				{
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Already admin state is UNLOCKED call_result:%d",call_result);
					break;
				}
				call_result = m_snmpManager->configure_front_port_scx_lag(slot_number,acs_apbm_snmp::ENABLE_LAG);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: modify callback UNLOCKED call_result:%d",call_result);
				m_snmpManager->set_lag_admin_state(slot_number,UNLOCKED);
				break;
			}
			case LOCKED:
			{
				if(!m_snmpManager->get_lag_admin_state(slot_number))
				{
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: Already admin state is LOCKED call_result:%d",call_result);
					break;
				}
				call_result = m_snmpManager->configure_front_port_scx_lag(slot_number,acs_apbm_snmp::DISABLE_LAG);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: modify callback LOCKED call_result:%d",call_result);
				m_snmpManager->set_lag_admin_state(slot_number,LOCKED);
				(void)m_serverWorkingSet->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, slot_number,SCX_LAG_FRONT_PORT2);
				(void)m_serverWorkingSet->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, slot_number,SCX_LAG_FRONT_PORT1);
				m_serverWorkingSet->monitoring_service_event_handler->reset_scx_lag_monitoring_flags_due_to_restart(slot_number);
				break;
			}
			default:
				break;
			}

		}
	}
	return ACS_CC_SUCCESS;
}
void acs_apbm_scxlagimmhandler::adminOperationCallback(
		ACS_APGCC_OiHandle /*oiHandle*/,
		ACS_APGCC_InvocationType /*invocation*/,
		const char* /*p_objName*/,
		ACS_APGCC_AdminOperationIdType /*operationId*/,
		ACS_APGCC_AdminOperationParamType** /*paramList*/) {

	//NOT USED
}


ACS_CC_ReturnType acs_apbm_scxlagimmhandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " class Name: " << className << endl;
	cout << " parent Name: " << parentName << endl;
	cout << " attribute value: " << attr <<endl;


	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_apbm_scxlagimmhandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	cout << endl;


	return ACS_CC_SUCCESS;
}
ACS_CC_ReturnType acs_apbm_scxlagimmhandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;

	return ACS_CC_SUCCESS;
}

void acs_apbm_scxlagimmhandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}

void acs_apbm_scxlagimmhandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){


	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}
