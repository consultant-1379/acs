/*=================================================================== */
/**
   @file   acs_alog_cmdHandler.h

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

#ifndef ACS_ALOG_COMMAND_H
#define ACS_ALOG_COMMAND_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
//Include file for external library.
#include <acs_alog_defs.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_ApplicationManager.h>
#include <acs_alog_cmdHandler.h>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Acceptor.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

class acs_alog_command{
public:
		acs_alog_command();
		~acs_alog_command();

public:
		int CommandManagement();
		static int isBrfcPermitted();//BRFC

public:
		static int permitFlag;//BRFC


private:
		int HandleCommands();
		acs_alog_cmdHandler cmdReact;
		ACE_LSOCK_Acceptor Acceptor;
		ACE_LSOCK_Stream stream;


private:
		enumCmdExecuteResult SetNewPw(CmdData* pCmdData);
		enumCmdExecuteResult SetPw(CmdData* pCmdData);
		enumCmdExecuteResult CheckPw(CmdData* pCmdData);
		enumCmdExecuteResult ProtectItem(CmdData* pCmdData);
		enumCmdExecuteResult UnprotectItem(CmdData* pCmdData);
		enumCmdExecuteResult SetPLFileDest(CmdData* pCmdData);
		enumCmdExecuteResult SetPLIdTag(CmdData* pCmdData);
		enumCmdExecuteResult AdminPasswordHandling(CmdData* pCmdData);
		enumCmdExecuteResult ResetPw(CmdData* pCmdData);
		enumCmdExecuteResult ProtectUser(CmdData* pCmdData);
		enumCmdExecuteResult UnprotectUser(CmdData* pCmdData);
};
#endif
