//****************************************************************************
//
//  NAME
//     acs_alog_command.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2004. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  PR           DATE      INITIALS    DESCRIPTION
//  -----------------------------------------------------------
//  N/A       26/07/2012     xbhakat       Initial Release
//  N/A       07/08/2012     xbhakat        Final Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************


#include <acs_alog_command.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/Task.h>
#include <sstream>
#include <acs_alog_brfc_interface.h>
#include <acs_alog_utility.h>
using namespace std;

int acs_alog_command::permitFlag=1;//BRFC

acs_alog_command::acs_alog_command() {

}
acs_alog_command::~acs_alog_command() {

}

int acs_alog_command::CommandManagement()
{
	const char pipeName[] = "/var/run/ap/ACS_ALOG_MainCmdPipe";
	ACE_UNIX_Addr addr(pipeName);

	::unlink(pipeName);

	if (Acceptor.open(addr) != 0) {
		char strErr_no[1024] = {0};
		snprintf(strErr_no,1024,"acs_alog_command::CommandManagement() - Acceptor.open fails - errno = %i", ACE_OS::last_error() );
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strErr_no));

		Acceptor.remove();
		return -1;
	}

	while (true) {

		if (Acceptor.accept(stream, 0, 0, 0) == -1) {
			stream.close();

			int error = ACE_OS::last_error();
			if ( error == EINTR )
				continue;

			char strErr_no[1024] = {0};
			snprintf(strErr_no,1024,"acs_alog_command::CommandManagement() - Acceptor.accept fails - errno = %i", error );
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strErr_no));

			Acceptor.remove();
			return -2;
		}

		const int return_code = HandleCommands();
		stream.close();

		switch (return_code) {
			case 0:
				ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("acs_alog_command::CommandManagement() - Command correctly executed"));
				break;

			case 2:
				ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("acs_alog_command::CommandManagement() - Thread gracefully exiting"));
				Acceptor.remove();
				return 0;

			case -1:
			default: {
				char strErr_no[1024] = {0};
				::snprintf(strErr_no, 1024, "acs_alog_command::CommandManagement() - return_code = %d, waiting for new commands", return_code);
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strErr_no));
			}
		}
	}

	return 0;
}
//--------------------------------------------------------------------

int acs_alog_command::HandleCommands()
{
	ACE_TCHAR  szCommand[16];
	void *buffer = malloc(4096);

	if (stream.recv(buffer, 4096) < 0) {
		char strErr_no[1024] = {0};
		::snprintf(strErr_no, 1024, "acs_alog_command::HandleCommands() - recv failed, errno = %d", ACE_OS::last_error());
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strErr_no));
		return -1;
	}

	enumCmdExecuteResult ExecResult = cerSuccess;

	CmdData* myCmdDataPtr;
	myCmdDataPtr = reinterpret_cast<CmdData*>(buffer);
	if(strcmp(myCmdDataPtr->Data1,"SHUTDOWN") == 0)
	{
		free (buffer);
		return 2;
	}

	if(!acs_alog_BrfImplementer::getBrfStatus())
	{
		acs_alog_command::permitFlag--;

		switch (myCmdDataPtr->CmdType) {
			case ctCheckPw: // PL
				ExecResult = CheckPw(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogset");
				break;
			case ctSetPw: // PL
				ExecResult = SetPw(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogset");
				break;
			case ctSetNewPw: // PL
				ExecResult = SetNewPw(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogset");
				break;
			case ctResetPw:
				ExecResult = ResetPw(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogset");
				break;
			case ctProtectItem: // PL
				ExecResult = ProtectItem(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogpchg");
				break;
			case ctIdTagProt:
				ExecResult = SetPLIdTag(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogset");
				break;
			case ctUnprotectItem: // PL
				ExecResult = UnprotectItem(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogpchg");
				break;
			case ctSetFileDestProt:
				ExecResult = SetPLFileDest(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogset");
				break;
			case ctAdminPasswordHandling:
				ExecResult = AdminPasswordHandling(myCmdDataPtr);
				break;
			case ctProtectUser: // PL
				ExecResult = ProtectUser(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogpchg");
				break;
			case ctUnprotectUser: // PL
				ExecResult = UnprotectUser(myCmdDataPtr);
				ACE_OS::strcpy(szCommand, "alogpchg");
				break;
			default:
				ExecResult = cerUnknownCmd;
				ACE_OS::strcpy(szCommand, "<unknown>");
				break;
		}

		acs_alog_command::permitFlag++;
	}
	else {
		ExecResult = cerBackupInProgress;
	}

	void  *dest;
	dest = malloc(sizeof(ExecResult));

	memcpy(dest, &ExecResult, sizeof(ExecResult));

	if (stream.send_n(dest, sizeof(ExecResult)) == -1)
	{
		char strErr_no[1024] = {0};
		::snprintf(strErr_no, 1024, "acs_alog_command::HandleCommands() - send failed, errno = %d", ACE_OS::last_error());
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strErr_no));
		free (buffer);
		free (dest);
		return -1;
	}

	free (buffer);
	free (dest);
	return 0;
}
//--------------------------------------------------------------------


enumCmdExecuteResult acs_alog_command::SetPw(CmdData* pCmdData)
{
	enumCmdExecuteResult eRes = cmdReact.SetPw((const char *)(pCmdData->Data1));
	/* if (eRes == cerSuccess){
	// Alarm raise on TRANSFER QUEUE not defined
	m_pDUH->TQ_AlarmHandler();
	m_pDUH->UpdatePasswordPresent(TRUE);
	m_pDUH->CreateFileEmpty();
	}*/
	return eRes;
}
//--------------------------------------------------------------------


enumCmdExecuteResult acs_alog_command::SetNewPw(CmdData* pCmdData)
{
	enumCmdExecuteResult eRes = cmdReact.SetNewPw( (const char *)(pCmdData->Data1),(const char *)(pCmdData->Data2) );
	if (eRes == cerSuccess)
	{
		eRes = cmdReact.ChangePassword();
		if (eRes!= cerSuccess)	//restore the pw
		{
			cmdReact.SetNewPw( (const char *)(pCmdData->Data2),(const char *)(pCmdData->Data1) );
			cmdReact.UpdatePassword();
		}

	}
	return eRes;
}
//------------------------------------------------------------------------------------
enumCmdExecuteResult acs_alog_command::ResetPw(CmdData* pCmdData)
{
	enumCmdExecuteResult eRes = cmdReact.UpdatePasswordPresent(false);
	eRes = cmdReact.ResetFile("SYS");
	eRes = cmdReact.ResetFile("PMMLCMD");
	eRes = cmdReact.ResetFile("PMMLPRT");
	eRes = cmdReact.ResetFile("PUSR");
	eRes = cmdReact.ResetFile("ATTRFILEP");
	eRes = cmdReact.deleteAllPlogFiles();
	//eres = cmdReact->InitATTRFILEP();
	eRes = cmdReact.TQ_CeaseHandler();
	//eRes = cmdReact->deleteAllPlogFiles();
	//eRes = cmdReact->ResetLogBuffer

	return eRes;
}


//--------------------------------------------------------------------


enumCmdExecuteResult acs_alog_command::CheckPw(CmdData* pCmdData)
{
	//	return cerSuccess;
	return cmdReact.CheckPw((const char *)(pCmdData->Data1));
}
//--------------------------------------------------------------------

enumCmdExecuteResult acs_alog_command::ProtectItem(CmdData* pCmdData)
{
	enumCmdExecuteResult ExecResult = cerSuccess;

	// Exclude items
	switch (pCmdData->DataUnitType)
	{
		case MMLcmd:		ExecResult = cmdReact.ProtectMMLcmd((const char *)pCmdData->Data1);
					break;
		case MMLprt:
					ExecResult = cmdReact.ProtectMMLprt((const char *)pCmdData->Data1);
					break;
		default:
					ExecResult = cerUnknownDataUnitType;
					break;
	}

	return ExecResult;
}
//--------------------------------------------------------------------

enumCmdExecuteResult acs_alog_command::UnprotectItem(CmdData* pCmdData)
{
	enumCmdExecuteResult ExecResult = cerSuccess;

	// Exclude items
	switch (pCmdData->DataUnitType)
	{
		case MMLcmd:
			ExecResult = cmdReact.UnprotectMMLcmd((const char *)pCmdData->Data1);
			break;
		case MMLprt:
			ExecResult = cmdReact.UnprotectMMLprt((const char *)pCmdData->Data1);
			break;
		default:
			ExecResult = cerUnknownDataUnitType;
			break;
	}

	return ExecResult;
}
//--------------------------------------------------------------------
enumCmdExecuteResult acs_alog_command::ProtectUser(CmdData* pCmdData)
{
	enumCmdExecuteResult ExecResult = cerSuccess;

	// Exclude items
	if(pCmdData->Data1){
	   ExecResult = cmdReact.ProtectUserData((const char *)pCmdData->Data1);}
	else
	   ExecResult = cerUnknownUser;

	return ExecResult;
}
//--------------------------------------------------------------------
enumCmdExecuteResult acs_alog_command::UnprotectUser(CmdData* pCmdData)
{
	enumCmdExecuteResult ExecResult = cerSuccess;

	// Exclude items
	if(pCmdData->Data1){
	   ExecResult = cmdReact.UnprotectUserData((const char *)pCmdData->Data1);}
	else
	   ExecResult = cerUnknownUser;

	return ExecResult;
}
//--------------------------------------------------------------------
enumCmdExecuteResult acs_alog_command::SetPLFileDest(CmdData* pCmdData)
{
	enumCmdExecuteResult ExecResult = cerSuccess;
	ExecResult = cmdReact.changePlogFileDest((const char *)pCmdData->Data1);
	//  return m_pDUH->ChangePLFileDest(pCmdData->Data1);
	return ExecResult;
}

enumCmdExecuteResult acs_alog_command::SetPLIdTag(CmdData* pCmdData)
{
	enumCmdExecuteResult ExecResult = cerSuccess;
	ExecResult = cmdReact.changePlogIdTag((const char *)pCmdData->Data1);
	return ExecResult;

}

//Start BRC
int acs_alog_command::isBrfcPermitted()
{
	return permitFlag;
}
//END BRFC


enumCmdExecuteResult acs_alog_command::AdminPasswordHandling(CmdData* pCmdData)
{

	string password(pCmdData->Data1);
	string user(pCmdData->Data2);

	ACS_APGCC_CommonLib  apgccLib;

	if (apgccLib.CheckUser (user, password) == ACS_APGCC_ROLES_SUCCESS)
	{
			return cerSuccess;
	}

	return cerAuthFailure;
}



