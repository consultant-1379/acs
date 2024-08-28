/*=================================================================== */
/**
   @file   acs_alog_defs.h

   @brief Header file for acs_alog_cmdHandler type module.

          This module contains all the declarations useful to
          specify the class.

   @version 2.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
   N/A       07/08/2012     xbhakat               Final Release
==================================================================== */

#ifndef ACS_ALOG_DEFS_H
#define ACS_ALOG_DEFS_H

#include <acs_alog_fileDef.h>
#include <cstdlib>
#include <ace/ACE.h>
#include <list>

typedef std::list<ACS_ALOG_Exclude> ExListType;
    	 typedef std::list<ACS_ALOG_Exclude>::iterator ExListIterType;
typedef enum _enumCmdExecuteResult
{
   cerSuccess = 0,
   cerNotActivated,
   cerAlreadyActivated,
   cerNotDeactivated,
   cerAlreadyDeactivated,
   cerItemNotDefined,
   cerItemAlreadyDefined,
   cerUnknownCmd,
   cerUnknownDataUnitType,
   cerFileOpenError,
   cerFileReadError,
   cerFileWriteError,
   cerInvalidBufferSize,
   cerServerNotResponding,
   cerOtherError,
   cerPwAlreadyDefined, // PL Password Already defined
   cerPwNotDefined, // PL Passowrd Not Defined
   cerPwNotMatch, // PL Password don't Match
   cerCheckPw, // PL Check Password for alogpls
   cerServerBusy,// PL Server Busy
   cerBackupInProgress,
   cerAuthFailure,
   cerUserNotDefined,
   cerUserAlreadyDefined,
   cerUnknownUser,
} enumCmdExecuteResult;

typedef enum _enumCmdType
{
   ctActivate,
   ctDeactivate,
   ctExcludeItem,
   ctIncludeItem,
   ctSetSize,
   ctLogData,
   ctSetFileDest,
   ctIdTag,  // new 0512
   ctUndefined,
   ctSetSizeProt, //PL
   ctSetPw,	// PL
   ctSetNewPw,	//PL
   ctSetFileDestProt,// PL
   ctIdTagProt, // PL
   ctProtectItem, //PL
   ctUnprotectItem, //PL
   ctCheckPw,//PL
   ctResetPw,//PL
   ctFindInProgress, // PL
   ctFindLastInProgress, // PL
   ctFindEnded,
   ctAdminPasswordHandling,
   ctProtectUser,
   ctUnprotectUser,
} enumCmdType; // PL

typedef enum
{
not_defined 	   		= 0,  // Anything
MMLcmd 		       		= 3,  // MML command
MMLprt 		       		= 4,  // MML printout
MMLcmdpar 	       		= 5,  // MML command parameter
ApplData 	       		= 6,  // Application data
MMLalp 		       		= 7,  // MML alarm printout
Unixcmd 	       		= 8,  // Unix command/process
Unixprt 	       		= 9,  // Unix printout
SecLogData 	       		= 10, // Security log data
MMLCmdInconsistent    	= 11, // MML command causing inconsistency
MMLCmdLog             	= 12, // MML command for logging
MMLCmdLogInconsistent	= 13, // MML command for logging, with inconsistency
MMLPrtInconsistent   	= 14,  // Result printout indicating inconsistency
MMLMediationCommand      = 15
} acs_alog_dataUnitType;

typedef struct _CmdData
{
   enumCmdType           CmdType;
   acs_alog_dataUnitType DataUnitType;
   ACE_TCHAR                 Data1[512+1];
   ACE_TCHAR                 Data2[512+1];
} CmdData;

#define ALOG_USER_ROLE      "securitysystemadministrator"

#endif


#ifndef BRFC_DEFINE_H_
#define BRFC_DEFINE_H_

#define BRM_SYSTEM_BACKUP_MANAGER__CREATE_BACKUP	0
#define BRM_SYSTEM_BACKUP_MANAGER__DELETE  	1
#define BRM_SYSTEM_BACKUP__RESTORE 3
#define BRM_SYSTEM_BACKUP__CONFIRM_RESTORE 4
#define BRM_SYSTEM_BACKUP__CANCEL_CURRENT_ACTION 5

#define BRF_VERSION_PRA_

#ifndef BRF_VERSION_PRA_

#define BRM_PARTICIPANT__PERMIT_BACKUP 6
#define BRM_PARTICIPANT__COMMIT_BACKUP 7
#define BRM_PARTICIPANT__CANCEL_BACKUP 8

#define BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP 9

#define BRM_PERSISTENT_STORAGE_OWNER__CREATE_BACKUP 10
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_DELETE 11
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_DELETE 12
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_DELETE 13
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_DELETE 14
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_RESTORE 15
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_RESTORE 16
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_RESTORE 17
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_RESTORE 18

#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS 19
#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT 20

#else

#define BRM_PARTICIPANT__PERMIT_BACKUP 0
#define BRM_PARTICIPANT__COMMIT_BACKUP 3
#define BRM_PARTICIPANT__CANCEL_BACKUP 4

#define BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP 1

#define BRM_PERSISTENT_STORAGE_OWNER__CREATE_BACKUP 2
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_DELETE 5
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_DELETE 6
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_DELETE 7
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_DELETE 8
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_RESTORE 9
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_RESTORE 10
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_RESTORE 11
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_RESTORE 12

#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS 21
#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT 22

#endif

// Define status for backupType parameter

#define BRF_SUCCESS 	0
#define BRF_ERROR_DISKSPACE 1
#define BRF_ERROR_CONFLICT 	2
#define BRF_ERROR_OTHERS	99

//Define status for barathi requirement
#define NODE_A_IP_ADDRESS    "nodeAIpAddress"
#define NODE_B_IP_ADDRESS    "nodeBIpAddress"
#define DNNAME    "northBoundId=1,networkConfigurationId=1"
#define AUDITDNNAME "AxeAuditLoggingauditLoggingMId=1"

#endif /* BRFC_OPERAIONID_H_ */
/************* END - BRFC CONSTANTS DEFINITION ***************/
