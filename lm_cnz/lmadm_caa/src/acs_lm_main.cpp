//******************************************************************************
//
//  NAME
//     acs_lm_main.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>
#include "acs_lm_server.h"
#include "acs_lm_cmd.h"
#include "acs_lm_testlkf.h"
#include "acs_lm_lksender.h"
#include "acs_lm_tra.h"
#include "acs_lm_electronickey_runtimeowner.h"
#include "acs_lm_haservice.h"
#include <acs_prc_api.h>
#include <acs_aeh_signalhandler.h>

ACS_LM_Server* lmServer = NULL;
ACS_LM_HAService *theLMApplPtr = NULL;
bool theInteractiveMode = false;
ACE_Event * myEndEvent = NULL;
/*==========================================================================
        ROUTINE: sighandler
========================================================================== */
void sighandler(int signum)
{
        if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
        {
              
        	INFO("sighandler() %s","Signal generated to stop LM service...")

			if(theInteractiveMode)
            {
            	if(lmServer!=NULL)
				{
					INFO("sighandler() %s", "Requesting LM Thread to stop");
					//lmServer->stop();
					ACE_Message_Block * myMessageBlock = 0;

					INFO("sighandler() %s", "Sending message in the message queue");

					ACE_NEW(myMessageBlock,ACE_Message_Block(0,ACE_Message_Block::MB_HANGUP));

					lmServer->putq(myMessageBlock);

					INFO("sighandler() %s", "Waiting for the lmserver thread to stop");

					lmServer->wait();

					INFO("sighandler() %s", "Deleting lmserver");

					delete lmServer;

					lmServer = NULL;

					myEndEvent->signal();
				}
			}

        }
}

int setSignalHandling()
{

	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask );

	if( sigaction(SIGINT, &sa, NULL ) == -1)
	{
		ERROR("%s", "Error occured while handling SIGINT in acs_lmserverd");
		return -1;
	}
	if( sigaction(SIGTERM, &sa, NULL ) == -1)
	{
		ERROR( "%s", "Error occured while handling SIGTERM in acs_lmserverd");
		return -1;
	}
	if( sigaction(SIGTSTP, &sa, NULL ) == -1)
	{
		ERROR( "%s", "Error occured while handling SIGTSTP in acs_lmserverd");
		return -1;
	}
	return 0;

}

/*=================================================================
	ROUTINE: printUsage
=================================================================== */
void ACS_LM_Server::printUsage()
{
	cout<<"Usage: acs_lm_server -d" <<endl;
}//end of printUsage
/*=================================================================
	ROUTINE: printUsage
=================================================================== */
int ACE_TMAIN(int argc, char *argv[])
{


acs_aeh_setSignalExceptionHandler("acs_lmserverd");                     //event generated when service gets crashed
	if(argc > 1)
	{
		if(argc == 2)
		{
			if (strcmp(argv[1], "-d") == 0)
			{
				ACS_LM_Logging.Open("LM");
				myEndEvent = new ACE_Event(true,false);
				if (setSignalHandling() == -1)
				{
					ACS_LM_Logging.Close();
					return ACS_LM_RC_NOK;
				}
				DEBUG("ACE_TMAIN %s","Running in Debug Mode");
				theInteractiveMode = true;
				lmServer = new ACS_LM_Server();
				lmServer->activate();
				myEndEvent->wait();

			}
			else
			{
				ACS_LM_Server::printUsage();
				return ACS_LM_RC_INCUSAGE;
			}
		}
		else
		{
			ACS_LM_Server::printUsage();
			return ACS_LM_RC_INCUSAGE;
		}

	}
	else
	{

		theInteractiveMode = false;
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
		theLMApplPtr = new ACS_LM_HAService("acs_lmserverd", "root");
		ACS_LM_Logging.Open("LM");		//FIX for TR HX97829

		DEBUG("ACE_TMAIN %s", "acs_lmserverd started in HA Mode");

		if( theLMApplPtr != 0)
		{
			errorCode = theLMApplPtr->activate();
			if (errorCode == ACS_APGCC_HA_FAILURE)
			{
				ERROR("ACE_TMAIN %s", "HA Activation Failed for acs_lmserverd");
			}
			else if( errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			{
				ERROR("ACE_TMAIN %s", "HA Application Failed to close acs_lmserverd gracefully");
			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS)
			{
				INFO("ACE_TMAIN %s", "HA Application Gracefully closed acs_lmserverd");
			}
			else
			{
				ERROR("ACE_TMAIN %s", "Error occured while starting acs_lmserverd with HA");
			}
			delete theLMApplPtr;
			theLMApplPtr = 0;
		}
		else
		{
			ERROR("ACE_TMAIN %s", "Unable to allocate memory for ACS_LM_SERVICE object");
		}
		ACS_LM_Logging.Close();
	}
	return ACS_LM_RC_OK;
}//end of ACE_TMAIN
