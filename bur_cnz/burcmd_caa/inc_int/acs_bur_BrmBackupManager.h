/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2014
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file acs_bur_BrmBackupManager.h
 *
 * @brief
 * Maintains local BrMBackupManager
 *
 * @author
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 02/12/2011     EGIMARR       Initial Release
 * 13/02/2014     XVENJAM       cpp check error correction
 ****************************************************************************/

#ifndef ACS_BUR_BRMBACKUPMANAGER_H_
#define ACS_BUR_BRMBACKUPMANAGER_H_

#include <string>
#include "acs_bur_Define.h"
#include <acs_apgcc_omhandler.h>
#include <ACS_TRA_trace.h>

using namespace std;

class acs_bur_BrmBackupManager
{
	public:
	acs_bur_BrmBackupManager(char *p);
	~acs_bur_BrmBackupManager();
	string  getBrmBackupManagerId();
	string  getBackupDomain();
	string  getBackupType();
	string  getAsyncActionProgress();
	int 	getOpCode();
	void	setOpCode(int code);
	void	setOpCode(int code, string functionName);
	string  getOpMessage();

	private:
	string brmBackupManagerId;
	string backupDomain;
	string backupType;
	string asyncActionProgress;
	int opCode;
	string m_functionName;
	ACS_TRA_trace* bbmTrace;
	ACS_TRA_Logging* burLog;
};

#endif /* ACS_BUR_BRMBACKUPMANAGER_H_ */
