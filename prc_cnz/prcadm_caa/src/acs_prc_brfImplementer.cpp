/*
 * ACS_PRC_RunTimeOwner.cpp
 *
 *  Created on: Dec 28, 2010
 *      Author: xlucpet
 */

#include "acs_prc_brfImplementer.h"
#include "acs_prc_reporting.h"
#include "acs_prc_runlevel_thread.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_TRA_Logging.h"
#include <vector>
#include <fstream>
#include <string>
#include <iterator>
#include "ace/SOCK_Connector.h"
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Connector.h>

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

bool acs_prc_brfImplementer::BackupAndRestore = false;
bool PBEdisabled = false;
acs_prc_brfImplementer::acs_prc_brfImplementer() {

}

acs_prc_brfImplementer::~acs_prc_brfImplementer() {

}

int acs_prc_brfImplementer::ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode){
	acs_apgcc_adminoperation admOp;
	//ACS_CC_ReturnType result1;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;
	char firstAttName[]= "requestId";
	char secondAttNameVal1[]= "resultCode";
	char secondAttNameVal2[]= "progressCount";
	char thirdAttName[]= "message";
	unsigned long long requestIdValue = requestId;
	int brfStatusVal = brfStatus; // BRF_SUCCESS
	char* strValue = const_cast<char*>("");
	void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
	int intAttrValue = 50; // 50 %


	if( responseCode  == BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT ){
		ACS_APGCC_AdminOperationParamType thirdElem;
		//create first Element of parameter list
		firstElem.attrName =firstAttName;
		firstElem.attrType=ATTR_UINT64T;
		// return requestId previously checked
		firstElem.attrValues=reinterpret_cast<void*>(&requestIdValue);
		// create second Element of parameter list
		secondElem.attrName =secondAttNameVal1;
		secondElem.attrType=ATTR_INT32T ;
		secondElem.attrValues=reinterpret_cast<void*>(&brfStatusVal);
		//create the third Element of parameter list
		thirdElem.attrName =thirdAttName;
		thirdElem.attrType=ATTR_STRINGT ;
		thirdElem.attrValues=valueStr;
		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);
		vectorIN.push_back(thirdElem);
	}

	else if ( responseCode == BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS ){

		//create first Element of parameter list
		firstElem.attrName =firstAttName;
		firstElem.attrType=ATTR_UINT64T;
		firstElem.attrValues=reinterpret_cast<void*>(&requestIdValue);
		// create second Element of parameter list
		secondElem.attrName =secondAttNameVal2;
		secondElem.attrType=ATTR_UINT32T ;
		secondElem.attrValues=reinterpret_cast<void*>(&intAttrValue);
		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);
	}

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	char log[512] = {0};

	const char *dnObjName1 = "brfParticipantContainerId=1";
	long long int timeOutVal_30sec = 30*(1000000000LL);
	int res1 = admOp.init();

	if (res1 != ACS_CC_SUCCESS){
		snprintf(log,512,"acs_prcmand %s - acs_prc_brfImplementer::admOp.init()FAILED - ERROR CODE = %i, ERROR MESSAGE = %s",PRCBIN_REVISION,admOp.getInternalLastError(),admOp.getInternalLastErrorText());
		Logging.Write(log,LOG_LEVEL_ERROR);
		Logging.Close();
		return -1;
	}

	int returnValue1 = 1;
	/*Wait at least one second before invoke administrative operations in order to synchronize whit BRFC*/
	sleep(1);
	/* Invoke operation  */
	int retry = 0;
	int admin_ret = 0;

	do {
		memset(log,0,sizeof(log));
		res1 = admOp.adminOperationInvoke(dnObjName1, 0 , responseCode , vectorIN , &returnValue1, timeOutVal_30sec);

		if ( res1 == ACS_CC_FAILURE ){

			admin_ret = admOp.getInternalLastError();

			if ( ( admin_ret == -14 ) || ( admin_ret == -20 ) ){
				snprintf(log,512,"acs_prcmand %s - acs_prc_brfImplementer::admOp.adminOperationInvoke()FAILED - Retry number : %i, ERROR CODE = %i, ERROR MESSAGE = %s",PRCBIN_REVISION,retry,admOp.getInternalLastError(),admOp.getInternalLastErrorText());
				Logging.Write(log,LOG_LEVEL_ERROR);
				usleep ( 500000 );
				retry++;
			}
			else {
				snprintf(log,512,"acs_prcmand %s - acs_prc_brfImplementer::admOp.adminOperationInvoke()FAILED - Retry number : %i, ERROR CODE = %i, ERROR MESSAGE = %s",PRCBIN_REVISION,retry,admOp.getInternalLastError(),admOp.getInternalLastErrorText());
				Logging.Write(log,LOG_LEVEL_ERROR);
				retry = 120; // force to exit
			}
		}
		else {
			snprintf(log,512,"acs_prcmand %s - acs_prc_brfImplementer::adminOperationInvoke OK",PRCBIN_REVISION);
			Logging.Write(log,LOG_LEVEL_WARN);
		}

	} while (( res1 == ACS_CC_FAILURE ) && ( retry < 120 ));

	memset(log,0,sizeof(log));

	if (res1 != ACS_CC_SUCCESS){
		snprintf(log,512,"acs_prcmand %s - acs_prc_brfImplementer::admOp.adminOperationInvoke()FAILED - ERROR CODE = %i, ERROR MESSAGE = %s",PRCBIN_REVISION,admOp.getInternalLastError(),admOp.getInternalLastErrorText());
		Logging.Write(log,LOG_LEVEL_ERROR);
		Logging.Close();
		admOp.finalize();
		return -1;
	}

	snprintf(log,512,"acs_prcmand %s - acs_prc_brfImplementer::ResponseToBrfc OK",PRCBIN_REVISION);
	Logging.Write(log,LOG_LEVEL_WARN);

	admOp.finalize();
	Logging.Close();

	return 0;
}

void acs_prc_brfImplementer::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* /*p_objName*/,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** paramList ) {

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	char log[512] = {0};
	const char backup_not_done_file[] = "/var/log/backup_not_done";
	const char apg_backup_not_done_file[] = "/var/log/apg_backup_not_done";
	const char apg_backup_not_done_file_cluster[] = "/cluster/etc/ap/acs/prc/conf/apg_backup_not_done";
	bool backupOn = false;
	ACS_APGCC_CommonLib getHWInfo_obj;

	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	acs_prc_runlevel_thread acs_prc_runlevel_thread;
	getHWInfo_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );
	acs_prc_report report;
    ACS_PRC_API prc_api;
	switch (operationId){
		case   BRM_PARTICIPANT__PERMIT_BACKUP:
			snprintf(log,512,"acs_prcmand %s - BRM_PARTICIPANT__PERMIT_BACKUP - BackupAndRestore = true",PRCBIN_REVISION);
			Logging.Write(log,LOG_LEVEL_ERROR);
			BackupAndRestore = true;
			backupOn = true;
			break;
		case BRM_PARTICIPANT__COMMIT_BACKUP:
			snprintf(log,512,"acs_prcmand %s - BRM_PARTICIPANT__COMMIT_BACKUP - BackupAndRestore = false;",PRCBIN_REVISION);
			Logging.Write(log,LOG_LEVEL_ERROR);
			BackupAndRestore = false;
			backupOn = false;
			report.alarmAPBackup("",ACS_PRC_ceaseSeverity);
			report.alarmAPGBackup("",ACS_PRC_ceaseSeverity);
			remove(backup_not_done_file);
			remove(apg_backup_not_done_file);
			remove(apg_backup_not_done_file_cluster);
			memset(log,0,sizeof(log));
			snprintf(log,512,"acs_prcmand %s - BRM_PARTICIPANT__COMMIT_BACKUP - Alarm AP BACK NOT CREATED ceased",PRCBIN_REVISION);
			Logging.Write(log,LOG_LEVEL_ERROR);
			memset(log,0,sizeof(log));
			snprintf(log,512,"acs_prcmand %s - BRM_PARTICIPANT__COMMIT_BACKUP - Alarm APG BACK NOT CREATED ceased",PRCBIN_REVISION);
			Logging.Write(log,LOG_LEVEL_ERROR);
			break;
		case BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP:
			snprintf(log,512,"acs_prcmand %s - BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP - BackupAndRestore = true;",PRCBIN_REVISION);
			Logging.Write(log,LOG_LEVEL_ERROR);
			BackupAndRestore = true;
			backupOn = true;
			break;
		case BRM_PARTICIPANT__CANCEL_BACKUP:
			snprintf(log,512,"acs_prcmand %s - BRM_PARTICIPANT__CANCEL_BACKUP - BackupAndRestore = false;",PRCBIN_REVISION);
			Logging.Write(log,LOG_LEVEL_ERROR);
			BackupAndRestore = false;
			backupOn = false;
			break;
	}
	
	if ((hwInfo.hwVersion == ACS_APGCC_HWVER_GEP1 || hwInfo.hwVersion == ACS_APGCC_HWVER_GEP2) && (prc_api.askForNodeState() == ACS_PRC_NODE_ACTIVE)){//CC-21559
	if(backupOn == true && !PBEdisabled){//CC-20181
		acs_prc_runlevel_thread.enablePBE(false);
		snprintf(log,512,"acs_prcmand %s - enablePBE(false) is called",PRCBIN_REVISION);
		Logging.Write(log,LOG_LEVEL_ERROR);
		acs_prc_runlevel_thread.increaseTipcTolerance();
		PBEdisabled = true;
	}	
	else if(backupOn == false){//CC-20181
		acs_prc_runlevel_thread.enablePBE(true);
		snprintf(log,512,"acs_prcmand %s - enablePBE(true) is called",PRCBIN_REVISION);
                Logging.Write(log,LOG_LEVEL_ERROR);
		acs_prc_runlevel_thread.decreaseTipcTolerance();
		PBEdisabled = false;
	}
	}
	
	/*start*/
	int dim=0;
	int i=0;
	while(paramList[i]){
		i++;
		dim++;
	}

	unsigned long long requestId = 0;
	i = 0;
	while( paramList[i] ){
		switch ( paramList[i]->attrType ) {
		/* ONLY FOR requested parameter */
		case ATTR_UINT64T:
			if(strcmp(paramList[i]->attrName,"requestId")==0){
				requestId = *reinterpret_cast<unsigned long long *>(paramList[i]->attrValues);
			}
			break;
		default:
			break;
		}

		i++;
	}

	int retVal = 1;

	this->adminOperationResult( oiHandle , invocation, retVal );

	if (operationId == BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP){
		retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}
	else{
		retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}

	Logging.Close();

}

ACS_CC_ReturnType acs_prc_brfImplementer::create(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*className*/, const char* /*parentName*/, ACS_APGCC_AttrValues **/*attr*/){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_brfImplementer::deleted(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*objName*/){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_brfImplementer::modify(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*objName*/, ACS_APGCC_AttrModification **/*attrMods*/){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_brfImplementer::complete(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

	return ACS_CC_SUCCESS;
}

void acs_prc_brfImplementer::abort(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

}

void acs_prc_brfImplementer::apply(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

}

ACS_CC_ReturnType acs_prc_brfImplementer::updateRuntime(const char* /*p_objName*/, const char** /*p_attrName*/){

	return ACS_CC_SUCCESS;
}
