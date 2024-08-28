/**
   @file acs_alog_Imm_implementer.cpp

   Class method implementation for acs_alog_Imm_implementer type module.

   This module contains the implementation of class declared in
   the acs_alog_Imm_implementer.h module

   @version 2.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       02/02/2011     xgencol/xgaeerr       Initial Release
   N/A       28/11/2011     xgencol               Final Release
  =================================================================== */

#include "acs_alog_Imm_Implementer.h"

#include <iostream>
using namespace std;

ExclCmdCommit ExclAddDone[30];
ExclCmdCommit ExclDelDone[30];

string  TQdn;
string  commandLogTQdn;

extern int  validateTransferQueue (string TQ);
extern void  stringToUpper (string &str);
extern void  emptingFileNameVariables (void);
extern string  parseMPAexclItem (string item);

extern bool    MCpSystem;
extern bool flagTemplateFormatChangePending;
extern ACE_Recursive_Thread_Mutex templateFormatPendingMutex;
extern int APtype;

int mmlSyslogAdmStatus, alogFileAdmStatus = 0;

void updateMmlLogStream(int Status);

acs_alog_Imm_Implementer::acs_alog_Imm_Implementer(){

	Type = Status = CLog = largeData = FileNaming = 0;
	AddDone = DelDone = AddRoll = DelRoll = 0;

	opComplete = oldCLog = oldLargeData = oldFileNaming = 0;

	oldStatus = 1;
	//oldAlogFileStatus stores previous value of alogFileAdmState attribute
	oldAlogFileStatus = 1;
	//oldMmlSyslogStatus stores previous value of mmlSyslogAdmState attribute
	oldMmlSyslogStatus = 0;

	memset(FlagCmd,0,sizeof(FlagCmd));
	memset(FlagRollback,0,sizeof(FlagRollback));
}

acs_alog_Imm_Implementer::acs_alog_Imm_Implementer(string p_impName ):acs_apgcc_objectimplementerinterface_V3(p_impName){

	Type = Status = CLog = largeData = FileNaming = 0;
	AddDone = DelDone = AddRoll = DelRoll = 0;

	opComplete = oldCLog = oldLargeData = oldFileNaming = 0;

	oldStatus = 1;

	memset(FlagCmd,0,sizeof(FlagCmd));
	memset(FlagRollback,0,sizeof(FlagRollback));
}

acs_alog_Imm_Implementer::acs_alog_Imm_Implementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ): acs_apgcc_objectimplementerinterface_V3(p_objName, p_impName, p_scope){//ACS_APGCC_ObjectImplementerInterface(p_objName, p_impName, p_scope){

	Type = Status = CLog = largeData = FileNaming = 0;
	AddDone = DelDone = AddRoll = DelRoll = 0;

	opComplete = oldCLog = oldLargeData = oldFileNaming = 0;

	oldStatus = 1;

	memset(FlagCmd,0,sizeof(FlagCmd));
	memset(FlagRollback,0,sizeof(FlagRollback));
}

ACS_CC_ReturnType acs_alog_Imm_Implementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack create invoked"));

	string  message;
	int     i = 0;
	char    mess[250];

	message = "---------------------------------------------------\n          ObjectCreateCallback invocated           \n---------------------------------------------------\n";
	ACSALOGTRACE(message);

	sprintf(mess,"Imm Handle: %d\n ccbId: %d\n class Name: %s\n parent Name: %s\n",(int)oiHandle, (int)ccbId, className, parentName);
	ACSALOGTRACE(mess);	

	while( attr[i] )
	{
		if (!ACE_OS::strcmp(attr[i]->attrName,"type"))
		{
			Type = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
		}
		if (!ACE_OS::strcmp(attr[i]->attrName,"item"))
		{
			Cmd = reinterpret_cast<char *>(attr[i]->attrValues[0]);
			stringToUpper (Cmd);
		}
		i ++;
	}

	if (Type == 2)       	//  Type == MPA  (MML commands with parameters)
	{
		Cmd = parseMPAexclItem (Cmd);

		if (Cmd.compare("") == 0)  return ACS_CC_FAILURE;
	}

	if (AddDone < 30)
	{
		ExclAddDone[AddDone].CommitType = Type;
		ExclAddDone[AddDone].CommitCmd  = Cmd;
		AddDone++;
	}
	else 	return ACS_CC_FAILURE;

	FlagCmd[ACS_ALOG_CREATE_CALLBACK] = 1;

	message = " CreateCallback TERMINATED\n";
	ACSALOGTRACE(message);

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_alog_Imm_Implementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack deleted invoked"));

	string  message;
	char    mess[250];

	OmHandler immHandle;
	ACS_CC_ReturnType result;

	string p_dnList = objName;

	ACS_APGCC_ImmAttribute attribute_1;
	ACS_APGCC_ImmAttribute attribute_2;

	attribute_1.attrName = "type";
	attribute_2.attrName = "item";

	std::vector<ACS_APGCC_ImmAttribute *> attributes;
	attributes.push_back(&attribute_1);
	attributes.push_back(&attribute_2);

	message = "---------------------------------------------------\n          ObjectDeleteCallback invocated           \n---------------------------------------------------\n";
	ACSALOGTRACE(message);

	sprintf(mess,"Imm Handle: %d\n ccbId: %d\n object Name: %s\n",(int)oiHandle, (int)ccbId, objName);
	ACSALOGTRACE(mess);

	result = immHandle.Init();
	if (result != ACS_CC_SUCCESS)
	{
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("ERROR: Initialization FAILURE"));
		return ACS_CC_FAILURE;
	}

	result = immHandle.getAttribute(p_dnList.c_str(), attributes);
	if (result != ACS_CC_SUCCESS )
	{
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Error getParameter"));
		return ACS_CC_FAILURE;
	}

	Type = *reinterpret_cast<int *>(attributes[0]->attrValues[0]);
	Cmd  = reinterpret_cast<char *>(attributes[1]->attrValues[0]);
	stringToUpper (Cmd);

	if (DelDone < 30)
	{
		ExclDelDone[DelDone].CommitType = Type;
		ExclDelDone[DelDone].CommitCmd  = Cmd;
		DelDone++;
	}
	else 	return ACS_CC_FAILURE;

	FlagCmd[ACS_ALOG_DELETE_CALLBACK] = 1;

	message = " DeleteCallback TERMINATED\n";
	ACSALOGTRACE(message);

	return ACS_CC_SUCCESS;
}


ACS_CC_ReturnType acs_alog_Imm_Implementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack modify invoked"));

	string  message;
	char    mess[250];

	message = "---------------------------------------------------\n          ObjectModifyCallback invocated           \n---------------------------------------------------\n";
	ACSALOGTRACE(message);

	sprintf(mess,"Imm Handle: %d\n ccbId: %d\n object Name: %s\n",(int)oiHandle, (int)ccbId, objName);
	ACSALOGTRACE(mess);

	int  i = 0;
	while (attrMods[i])
	{
		if (!strcmp(attrMods[i]->modAttr.attrName,"alogAdmState"))
		{
			Status    = (*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]));

			if (MCpSystem == true  &&  Status == 0)
			{
				setExitCode (31, "Not allowed to deactivate Audit Log function in Multi-CP system");
				return ACS_CC_FAILURE;
			}
			oldStatus = cmdReact.getAlogStatus();
			FlagCmd[ACS_ALOG_MODIFY_CALLBACK_alogAdmState] = 1;
		}
		else if (!strcmp(attrMods[i]->modAttr.attrName,"alogFileAdmState"))
		{
			alogFileAdmStatus = (*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]));
			oldAlogFileStatus = cmdReact.getAlogFileStatus();
			FlagCmd[ACS_ALOG_MODIFY_CALLBACK_alogFileAdmState] = 1;
		}
		else if (!strcmp(attrMods[i]->modAttr.attrName,"mmlSyslogAdmState"))
		{
			if(cmdReact.getMmlSyslogAvailabilityStatus()){
				mmlSyslogAdmStatus = (*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]));
				oldMmlSyslogStatus = cmdReact.getMmlSyslogStatus();
				FlagCmd[ACS_ALOG_MODIFY_CALLBACK_mmlSyslogAdmState] = 1;
			}
			else {
				setExitCode(31 , "MML over syslog is not ENABLED");
				return ACS_CC_FAILURE;
			}
		}
		else if (!strcmp(attrMods[i]->modAttr.attrName,"largeEventDataSkipped"))
		{
			largeData    = (*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]));
			oldLargeData = cmdReact.getAlogDataSkipped();
			FlagCmd[ACS_ALOG_MODIFY_CALLBACK_largeEventDataSkipped] = 1;
		}

		else if (!strcmp(attrMods[i]->modAttr.attrName,"commandAndSessionAdmState"))
		{
			CLog    = (*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]));
			oldCLog = cmdReact.getAlogIDS();
			FlagCmd[ACS_ALOG_MODIFY_CALLBACK_commandAndSessionAdmState]= 1;
		}

		else if (!strcmp(attrMods[i]->modAttr.attrName,"transferQueue"))
		{
			if (attrMods[i]->modAttr.attrValuesNum == 0)  TQ = "";
			else										  TQ = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);

			oldTQ = cmdReact.getAlogTQ();
			FlagCmd[ACS_ALOG_MODIFY_CALLBACK_transferQueue]= 1;

			if (validateTransferQueue (TQ, STANDARD_TRANSFER_QUEUE) == 0)
			{
				setExitCode (31, "Transfer Queue <" + TQ + "> not defined in the output handler");
				return ACS_CC_FAILURE;
			}
		}

		else if (!strcmp(attrMods[i]->modAttr.attrName,"commandLogTransferQueue"))
		{
			if (attrMods[i]->modAttr.attrValuesNum == 0)  cmdCommandLogTQ = "";
			else										  cmdCommandLogTQ = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);

			if ( !MCpSystem ){
				setExitCode (31, "Command Log Transfer Queue configuration is allowed only in multi CP configuration");
				return ACS_CC_FAILURE;
			}

			old_cmdCommandLogTQ = cmdReact.getAlogCmdLogTQ();
			FlagCmd[ACS_ALOG_MODIFY_CALLBACK_commandLogTransferQueue]= 1;

			if (validateTransferQueue (cmdCommandLogTQ, COMMAND_LOG_TRANSFER_QUEUE) == 0)
			{
				setExitCode (31, "Transfer Queue <" + cmdCommandLogTQ + "> not defined in the output handler");
				return ACS_CC_FAILURE;
			}
		}

		else if (!strcmp(attrMods[i]->modAttr.attrName,"templateFormat"))
		{
			FileNaming    = (*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]));
			oldFileNaming = cmdReact.getFileNaming();
			FlagCmd[ACS_ALOG_MODIFY_CALLBACK_templateFormat] = 1;

			if (FileNaming == oldFileNaming)
			{
				setExitCode (31, "File Naming Template already configured");
				return ACS_CC_FAILURE;
			}
		}
		else		//  Only the create and delete operations are allowed for the ExclusionItem objects
		{
			return ACS_CC_FAILURE;
		}
		i++;
	}

	message = " ModifyCallback TERMINATED\n";
	ACSALOGTRACE(message);

	return ACS_CC_SUCCESS;
}


ACS_CC_ReturnType acs_alog_Imm_Implementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack complete invoked"));

	string  message;
	char    mess[250];

	message = "---------------------------------------------------\n          CcbCompleteCallback invocated           \n---------------------------------------------------\n";
	ACSALOGTRACE(message);

	sprintf(mess,"Imm Handle: %d\n ccbId: %d\n",(int)oiHandle, (int)ccbId);
	ACSALOGTRACE(mess);

	if(FlagCmd[ACS_ALOG_CREATE_CALLBACK] == 1 && MAXEXCLADD)		//  Previous Op :  CREATE
	{
		for (int k=0; k<AddDone; k++)
		{
			if(cmdReact.setExclCmdList(ExclAddDone[k].CommitType,ExclAddDone[k].CommitCmd))
			{
				setExitCode (31, "Exclusion item already exist with same parameters");//HY38216
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack failed on add exclude command"));
				return ACS_CC_FAILURE;
			}
			AddRoll++;
		}
		FlagCmd[ACS_ALOG_CREATE_CALLBACK] = 0;
		FlagRollback[ACS_ALOG_CREATE_CALLBACK] = 1;
		opComplete++;
	}

	if(FlagCmd[ACS_ALOG_DELETE_CALLBACK] == 1 && MAXEXCLDEL)
	{
		for (int k=0; k<DelDone; k++)
		{
			if(cmdReact.delExclCmdList(ExclDelDone[k].CommitType,ExclDelDone[k].CommitCmd))
			{
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack failed on del exclude command"));
				return ACS_CC_FAILURE;
			}
			DelRoll++;
		}
		FlagCmd[ACS_ALOG_DELETE_CALLBACK] = 0;
		FlagRollback[ACS_ALOG_DELETE_CALLBACK] = 1;
		opComplete++;
	}

	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_alogAdmState] == 1)
	{
		cmdReact.setAlogStatus(Status);
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_alogAdmState] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_alogAdmState] = 1;
		opComplete++;
	}

	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_alogFileAdmState] == 1)
	{
		cmdReact.setAlogFileStatus(alogFileAdmStatus);
		syslog(LOG_INFO, "setting alogfileAdm state to :: %d", cmdReact.getAlogFileStatus());
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_alogFileAdmState] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_alogFileAdmState] = 1;
		opComplete++;
	}

	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_mmlSyslogAdmState] == 1)
	{
		cmdReact.setMmlSyslogStatus(mmlSyslogAdmStatus);
		syslog(LOG_INFO, "setting mmlSyslogAdm state to :: %d", cmdReact.getMmlSyslogStatus());
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_mmlSyslogAdmState] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_mmlSyslogAdmState] = 1;
		opComplete++;
		updateMmlLogStream(mmlSyslogAdmStatus);
	}


	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_largeEventDataSkipped] == 1)
	{
		cmdReact.setAlogDataSkipped(largeData);
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_largeEventDataSkipped] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_largeEventDataSkipped] = 1;
		opComplete++;
	}

	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_commandAndSessionAdmState] == 1)
	{
		cmdReact.setAlogIDS(CLog);
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_commandAndSessionAdmState] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_commandAndSessionAdmState] = 1;
		opComplete++;
	}

	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_transferQueue] == 1){
		cmdReact.setAlogTQ(TQ);
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_transferQueue] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_transferQueue] = 1;
		opComplete++;

		ACS_CC_ImmParameter  attr;
		char  par[] = "transferQueueDn";

		attr.attrName = par;
		attr.attrType = ATTR_NAMET;
		void * attrValuesPtrs[1] = {0};

		if (TQ.length() > 0)
		{
			attr.attrValuesNum = 1;
			//attr.attrValues = new void* [1];
			attr.attrValues = attrValuesPtrs;
			attr.attrValues[0] = reinterpret_cast<void*> (const_cast<char*> (TQdn.c_str()));
		}
		else {
			attr.attrValuesNum = 0;
			attr.attrValues = NULL;
		}

		modifyRuntimeObj (AUDITDNNAME, &attr);
	}

	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_commandLogTransferQueue] == 1){
		cmdReact.setAlogCmdLogTQ(cmdCommandLogTQ);
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_commandLogTransferQueue] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_commandLogTransferQueue] = 1;

		ACS_CC_ImmParameter attr;
		char  par[] = "commandLogTransferQueueDn";

		attr.attrName = par;
		attr.attrType = ATTR_NAMET;
		void * attrValuesPtrs[1] = {0};

		if (cmdCommandLogTQ.length() > 0){
			attr.attrValuesNum = 1;
			attr.attrValues = attrValuesPtrs;
			attr.attrValues[0] = reinterpret_cast<void*> (const_cast<char*> (commandLogTQdn.c_str()));
		}
		else {
			attr.attrValuesNum = 0;
			attr.attrValues = NULL;
		}

		modifyRuntimeObj (AUDITDNNAME, &attr);

	}

	if (FlagCmd[ACS_ALOG_MODIFY_CALLBACK_templateFormat] == 1)
	{
		cmdReact.setFileNaming(FileNaming);
		//emptingFileNameVariables ();					// TR HW28526
		FlagCmd[ACS_ALOG_MODIFY_CALLBACK_templateFormat] = 0;
		FlagRollback[ACS_ALOG_MODIFY_CALLBACK_templateFormat] = 1;
		opComplete++;

		// TR HW28526 - Do not empty the filename variables immediately, writeTargetLog() or writeTargetPLog() might
		// be using these variables in write_file thread. Set a flag to empty the variables after the current event is processed

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_templateFormatChange (templateFormatPendingMutex);
		flagTemplateFormatChangePending = true;
		ACSALOGLOG (LOG_LEVEL_TRACE, TEXTERROR("flagTemplateFormatChangePending is now set to true"));
	}

	message = " CompleteCallback TERMINATED\n";
	ACSALOGTRACE(message);

	return ACS_CC_SUCCESS;
}

void acs_alog_Imm_Implementer::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList){


	const string password = "ir2tiw5upyxi16f7qx07xb6lojvjkbtnwnsx0qflh2jo9yqtm2xlj5stz73gooib";
	bool result = false;

	string key = paramList[0]->attrName;
	string value = reinterpret_cast<char*>(paramList[0]->attrValues);
	char* errorMsg = "Invalid Key";
	int status = 0;
	if(APtype == 2){
		errorMsg = "Operation Not Allowed on AP2";
	}
	else if(key == "key" && value == password){
		if(cmdReact.getMmlSyslogAvailabilityStatus()){
			result = false;
			errorMsg = "MML over syslog is already enabled";
		}
		else{
			result = true;
			status = 1;
		}
	}
	else if(key == "key" && value == "0" ){
		if(cmdReact.getMmlSyslogStatus() == 0){
			if(cmdReact.getMmlSyslogAvailabilityStatus() == 0){
				result = false;
				errorMsg = "MML over syslog is already disabled";
			}
			else{
				result = true;
			}
		}
		else
			errorMsg = "Cannot be disabled, MML over syslog is Activated";
	}

	if(result)
	{
		ACS_CC_ReturnType result =  adminOperationResult(oiHandle, invocation, 1);
		if(ACS_CC_SUCCESS != result)
		{
			ACSALOGLOG(LOG_LEVEL_ERROR, "error on action result reply");
		}

		ACS_CC_ImmParameter mmlSyslogAvailability;
		mmlSyslogAvailability.attrName = "mmlSyslogAvailabilityState";
		mmlSyslogAvailability.attrType = ATTR_INT32T;
		mmlSyslogAvailability.attrValuesNum = 1;
		int enabled = status;
		void* val[1] = {reinterpret_cast<void*>(&enabled)};
		mmlSyslogAvailability.attrValues = val;
		if(modifyRuntimeObj(AUDITDNNAME, &mmlSyslogAvailability) == ACS_CC_FAILURE)
			ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("Error while modifying AuditLogM logstream Object mml_audit"));
		cmdReact.setMmlSyslogAvailabilityStatus(status);
	}
	else
	{
		char errMsg[512] = {0};
		snprintf(errMsg, sizeof(errMsg),"%s%s", "@ComNbi@", errorMsg);
		ACS_APGCC_AdminOperationParamType errorMessageParameter;
		errorMessageParameter.attrName = "errorComCliMessage";
		errorMessageParameter.attrType = ATTR_STRINGT;
		errorMessageParameter.attrValues = reinterpret_cast<void*>(errMsg);
		std::vector<ACS_APGCC_AdminOperationParamType> outParameteres;
		outParameteres.push_back(errorMessageParameter);

		ACS_CC_ReturnType result =  adminOperationResult(oiHandle, invocation, 21, outParameteres);
		if(ACS_CC_SUCCESS != result)
		{
			ACSALOGLOG(LOG_LEVEL_ERROR, "error on action result reply");
		}

	}


}

void acs_alog_Imm_Implementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack abort invoked"));

	string  message;
	char    mess[250];

	message = "---------------------------------------------------\n          CcbAbortCallback invocated           \n---------------------------------------------------\n";
	ACSALOGTRACE(message);

	sprintf(mess,"Imm Handle: %d\n ccbId: %d\n",(int)oiHandle, (int)ccbId);
	ACSALOGTRACE(mess);

	if (opComplete > 0)
	{
		if (RollBack())
		{
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("rollback failed"));
		}
	}

	opComplete = 0;
	for (int i=0; i<9; i++)
	{
		FlagRollback[i] = 0;
		FlagCmd[i] = 0;
	}

	AddDone = 0;
	DelDone = 0;
	AddRoll = 0;
	DelRoll = 0;

	message = " AbortCallback TERMINATED\n";
	ACSALOGTRACE(message);
}


void acs_alog_Imm_Implementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("CallBack apply invoked"));

	string  message;
	char    mess[250];

	message = "---------------------------------------------------\n          CcbApplyCallback invocated           \n---------------------------------------------------\n";
	ACSALOGTRACE(message);

	sprintf(mess,"Imm Handle: %d\n ccbId: %d\n",(int)oiHandle, (int)ccbId);
	ACSALOGTRACE(mess);

	opComplete = 0;
	for (int i=0; i<9; i++)
	{
		FlagRollback[i] = 0;
		FlagCmd[i] = 0;
	}

	AddDone = 0;
	DelDone = 0;
	AddRoll = 0;
	DelRoll = 0;

	message = " ApplyCallback TERMINATED\n";
	ACSALOGTRACE(message);
}


ACS_CC_ReturnType acs_alog_Imm_Implementer::updateRuntime(const char* p_objName, const char** p_attrName)
{
	char  mess[250];

	ACE_OS::sprintf(mess,"%s\n ccbId: %s\n",p_objName, p_attrName);
	ACSALOGTRACE(mess);

	return ACS_CC_SUCCESS;
}


ACS_CC_ReturnType acs_alog_Imm_Implementer::RollBack()
{
	string  message;

	message = "---------------------------------------------------\n          RollBack called           \n---------------------------------------------------\n";
	ACSALOGTRACE(message);

	if (FlagRollback[ACS_ALOG_CREATE_CALLBACK] == 1)
	{
		for (int k=0; k<AddRoll; k++)
		{
			if (cmdReact.delExclCmdList(ExclAddDone[k].CommitType,ExclAddDone[k].CommitCmd))
			{
				return ACS_CC_FAILURE;
			}
		}
	}

	if (FlagRollback[ACS_ALOG_DELETE_CALLBACK] == 1)
	{
		for (int k=0; k<DelRoll; k++)
		{
			if (cmdReact.setExclCmdList(ExclDelDone[k].CommitType,ExclDelDone[k].CommitCmd))
			{
				return ACS_CC_FAILURE;
			}
		}
	}

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_alogAdmState] == 1)  cmdReact.setAlogStatus(oldStatus);

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_largeEventDataSkipped] == 1)  cmdReact.setAlogDataSkipped(oldLargeData);

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_commandAndSessionAdmState] == 1)  cmdReact.setAlogIDS(oldCLog);

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_transferQueue] == 1)  cmdReact.setAlogTQ(oldTQ);

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_commandLogTransferQueue] == 1)  cmdReact.setAlogTQ(old_cmdCommandLogTQ);

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_templateFormat] == 1)  cmdReact.setFileNaming(oldFileNaming);

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_alogFileAdmState] == 1)  cmdReact.setAlogFileStatus(oldAlogFileStatus);

	if (FlagRollback[ACS_ALOG_MODIFY_CALLBACK_mmlSyslogAdmState] == 1)  cmdReact.setMmlSyslogStatus(oldMmlSyslogStatus);

	message = " RollBack TERMINATED\n";
	ACSALOGTRACE(message);

	return ACS_CC_SUCCESS;
}

void updateMmlLogStream(int Status)
{

	OmHandler objManager;
	if(objManager.Init() == ACS_CC_FAILURE){
		ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("ObjManager instance not instantiated"));
	}
	if (Status)
	{
		std::vector<ACS_CC_ValuesDefinitionType> streamAttributeList;

		ACS_CC_ValuesDefinitionType id;
		char logId[] = "logId";
		id.attrName = logId;
		id.attrType = ATTR_STRINGT;
		id.attrValuesNum = 1;
		char tmpRDN[64] = {0};
		ACE_OS::snprintf(tmpRDN, sizeof(tmpRDN) - 1, "%s=mml_audit", id.attrName);
		void *tmpValueRDN[1] = {reinterpret_cast<void *>(tmpRDN)};
		id.attrValues = tmpValueRDN;

		ACS_CC_ValuesDefinitionType severityFilter;
		char *name = "severityFilter";
		severityFilter.attrName = name;
		severityFilter.attrType = ATTR_INT32T;
		severityFilter.attrValuesNum = 7;
		int severityFilterInfo[7] = {0, 1, 2, 3, 4, 5, 6};
		void *severityFilterVal[7];
		for (int i = 0; i < 7; i++)
			severityFilterVal[i] = reinterpret_cast<void *>(&severityFilterInfo[i]);
		severityFilter.attrValues = severityFilterVal;

		ACS_CC_ValuesDefinitionType logFacilityNumber;
		name = "logFacilityNumber";
		logFacilityNumber.attrName = name;
		logFacilityNumber.attrType = ATTR_INT32T;
		logFacilityNumber.attrValuesNum = 1;
		int logFacilityNumberInfo = 13;
		void *logFacilityNumberVal[1] = {reinterpret_cast<void *>(&logFacilityNumberInfo)};
		logFacilityNumber.attrValues = logFacilityNumberVal;

		ACS_CC_ValuesDefinitionType logRetentionDays;
		name = "logRetentionDays";
		logRetentionDays.attrName = name;
		logRetentionDays.attrType = ATTR_UINT32T;
		logRetentionDays.attrValuesNum = 1;
		int logRetentionDaysInfo = 9;
		void *logRetentionDaysVal[1] = {reinterpret_cast<void *>(&logRetentionDaysInfo)};
		logRetentionDays.attrValues = logRetentionDaysVal;

		ACS_CC_ValuesDefinitionType logRetentionHousekeeping;
		name = "logRetentionHousekeeping";
		logRetentionHousekeeping.attrName = name;
		logRetentionHousekeeping.attrType = ATTR_INT32T;
		logRetentionHousekeeping.attrValuesNum = 1;
		int logRetentionHousekeepingInfo = 0;
		void *logRetentionHousekeepingVal[1] = {reinterpret_cast<void *>(&logRetentionHousekeepingInfo)};
		logRetentionHousekeeping.attrValues = logRetentionHousekeepingVal;

		ACS_CC_ValuesDefinitionType remoteLogServer;
		name = "remoteLogServer";
		remoteLogServer.attrName = name;
		remoteLogServer.attrType = ATTR_NAMET;
		remoteLogServer.attrValuesNum = 1;
		char remoteLogServerInfo[64] = {0};
		ACE_OS::snprintf(remoteLogServerInfo, sizeof(remoteLogServerInfo) - 1, "remoteLogServerId=security,CmwLogMlogMId=1");
		void *remoteLogServerVal[1] = {reinterpret_cast<void *>(remoteLogServerInfo)};
		remoteLogServer.attrValues = remoteLogServerVal;

		streamAttributeList.push_back(id);
		streamAttributeList.push_back(severityFilter);
		streamAttributeList.push_back(logRetentionDays);
		streamAttributeList.push_back(logRetentionHousekeeping);
		streamAttributeList.push_back(remoteLogServer);
		streamAttributeList.push_back(logFacilityNumber);

		if (objManager.createObject("CmwLogMLog", "CmwLogMlogMId=1", streamAttributeList) == ACS_CC_FAILURE)
			ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("Error while creating logstream Object mml_audit"));
	}
	else
	{
		if(objManager.deleteObject("logId=mml_audit,CmwLogMlogMId=1", ACS_APGCC_SUBTREE) == ACS_CC_FAILURE)
			ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("Error while deleting logstream Object mml_audit"));
	}
	if(objManager.Finalize() == ACS_CC_FAILURE)
		ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("Error while deleting logstream Object mml_audit"));
}
