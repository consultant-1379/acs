/*
 * MktrSignalHandler.cpp
 *
 *  Created on: Aug 30, 2010
 *      Author: xludesi
 */

#include "MktrSignalHandler.h"
#include "ace/Log_Msg.h"
#include "Mktr.h"
#include <iostream>

extern void mktruserMgmtAtStartup();
extern void mktruserMgmtCleanup();
extern bool mktruserUsageDone;

MktrSignalHandler::MktrSignalHandler(): _ignore_signals(false)
{
	// TODO Auto-generated constructor stub

}

MktrSignalHandler::~MktrSignalHandler()
{
	// TODO Auto-generated destructor stub
}


int MktrSignalHandler::handle_signal (int signum, siginfo_t *,ucontext_t *)
{
	if(_ignore_signals)
		return 0;	// a termination signal has been already received

	// Get reference to mktr singleton object
	Mktr *mktrExecutor = Mktr::getMktrCmdExecutor();

	if(mktrExecutor != NULL)
	{
		if(signum == SIGALRM)
		{
			// ALARM signal
			mktrExecutor->alarm_raised = true;
		}
		else if(signum == SIGTSTP) //CTRL+Z suspends the process, so delete MKTR user account as the process could stay suspended forever or be killed
		{
			mktruserMgmtCleanup();
		}
		else if(signum == SIGCONT)	//Process is resumed, create MKTR user account again
		{
			//Only create MKTR user if commonCmdTable, multiCpCmdTable, singleCpCmdTable logs are not collected yet
			if(!mktruserUsageDone)
				mktruserMgmtAtStartup();
		}
		else
		{
			// A termination signal occurred. Stop MKTR activity (and ignore other termination signals)
			_ignore_signals = true;
			mktrExecutor->stop(signum);
		}
	}

	return 0;
}
