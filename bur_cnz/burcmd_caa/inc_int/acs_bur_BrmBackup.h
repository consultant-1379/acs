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
 * @file acs_bur_BrmBackup.h
 *
 * @brief
 * Maintains local BrMBackup
 *
 * @author
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 02/12/2011     EGIACRI       Initial Release
 * 13/02/2014     XVENJAM       cpp check error correction
 ****************************************************************************/

#ifndef ACS_BUR_BRMBACKUP_H_
#define ACS_BUR_BRMBACKUP_H_

#include <string>
#include "acs_bur_Define.h"
#include <acs_apgcc_omhandler.h>
#include "acs_bur_BrmAsyncActionProgress.h"
#include "acs_bur_BrmBackupManager.h"
#include <ACS_TRA_trace.h>
#include "acs_bur_trace.h"

using namespace std;

class acs_bur_BrmBackup
{
	public:
	acs_bur_BrmBackup(char *distName);
	~acs_bur_BrmBackup();
	string  getBrmBackupId();
	string  getBackupName();
	string  getCreationTime();
	int	getStatus();
	string  getAsyncActionProgress();
	int 	getOpCode();
	string  getOpMessage();
	void 	setOpCode(int code);
	void 	setOpCode(int code,string functionName);

	private:
	string brmBackupId;
	string backupName;
	string creationTime;
	int	   status;
	string asyncActionProgress;
	int opCode;
	ACS_TRA_trace* backup_trace;
	ACS_TRA_Logging* burLog;
	string  m_functionName;
};

#endif /* ACS_BUR_BRMBACKUP_H_ */
