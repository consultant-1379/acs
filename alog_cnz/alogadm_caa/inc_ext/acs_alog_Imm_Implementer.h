
/*=================================================================== */
/**
   @file   acs_alog_Imm_Implementer.h

   @brief Header file for acs_alog_Imm_Implementer type module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
   N/A       28/11/2011     xgencol               Final Release
==================================================================== */

#ifndef ACS_ALOG_IMM_IMPLEMENTER_H_
#define ACS_ALOG_IMM_IMPLEMENTER_H_

#include <ACS_APGCC_ObjectImplementerInterface.h>
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperationtypes.h>
#include <iostream>
#include <acs_alog_cmdHandler.h>
#include <acs_apgcc_objectimplementerinterface_V3.h>

#define STANDARD_TRANSFER_QUEUE    false
#define COMMAND_LOG_TRANSFER_QUEUE true

using namespace std;

typedef struct {
         int CommitType;
         string CommitCmd;
} ExclCmdCommit;

enum {
	ACS_ALOG_MODIFY_CALLBACK_alogAdmState = 0,
	ACS_ALOG_MODIFY_CALLBACK_largeEventDataSkipped = 1,
	ACS_ALOG_MODIFY_CALLBACK_commandAndSessionAdmState = 2,
	ACS_ALOG_CREATE_CALLBACK = 3,
	ACS_ALOG_DELETE_CALLBACK = 4,
	ACS_ALOG_MODIFY_CALLBACK_transferQueue = 5,
	ACS_ALOG_MODIFY_CALLBACK_templateFormat = 6,
	ACS_ALOG_MODIFY_CALLBACK_commandLogTransferQueue = 7,
	ACS_ALOG_MODIFY_CALLBACK_alogFileAdmState = 8,
	ACS_ALOG_MODIFY_CALLBACK_mmlSyslogAdmState = 9
};

class acs_alog_Imm_Implementer : public acs_apgcc_objectimplementerinterface_V3{ //ACS_APGCC_ObjectImplementerInterface{//, 
private:

	acs_alog_cmdHandler cmdReact;

	int FlagCmd[16], FlagRollback[16];

	int AddDone;
	int DelDone;
	int AddRoll;
	int DelRoll;

	int opComplete;
	
	int Type;
	string Cmd;
	int Status;
	int CLog;
	int largeData;
	string TQ,cmdCommandLogTQ;
	int FileNaming;

	int oldStatus;
	int oldAlogFileStatus;
	int oldMmlSyslogStatus;
	int oldCLog;
	int oldLargeData;
	string oldTQ,old_cmdCommandLogTQ;
	int oldFileNaming;

public :
	
	acs_alog_Imm_Implementer();

	acs_alog_Imm_Implementer(string p_impName );

	acs_alog_Imm_Implementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );

	~acs_alog_Imm_Implementer(){};

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_nameObj, const char** p_nameAttr);

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);
    ACS_CC_ReturnType RollBack();
};

#endif /* ACS_ALOG_IMM_IMPLEMENTER_H_ */
