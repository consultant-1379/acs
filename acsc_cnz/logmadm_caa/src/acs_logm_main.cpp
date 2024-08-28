
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <acs_logm_tra.h>
#include <acs_logm_definitions.h>
#include <acs_logm_logmservice.h>
#include <acs_aeh_signalhandler.h>

acs_logm_cleanupAction * theCleanupPointer = NULL;
ACE_Event * myEndEvent = NULL;

/*==========================================================================
        ROUTINE: sighandler
========================================================================== */
void sighandler(int signum)
{
	DEBUG( "Entering %s", "void sighandler(int signum) ");
	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{

		DEBUG("%s","Signal generated to stop LOGM service...");
		theCleanupPointer->stop();
		DEBUG("%s","Before Wait theCleanupPointer->wait() LOGM service...");
		theCleanupPointer->wait();
		DEBUG("%s","After Wait theCleanupPointer->wait() LOGM service...");
		myEndEvent->signal();
		delete theCleanupPointer;
	}
	DEBUG( "Leaving %s", "void sighandler(int signum) ");
}

bool setSignalHandling()
{
	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask );

	if( sigaction(SIGINT, &sa, NULL ) == -1)
	{
		ERROR("%s", "Error occurred while handling SIGINT in acs_logmaintd");
		return false;
	}
	if( sigaction(SIGTERM, &sa, NULL ) == -1)
	{
		ERROR( "%s", "Error occurred while handling SIGTERM in acs_logmaintd");
		return false;
	}
	if( sigaction(SIGTSTP, &sa, NULL ) == -1)
	{
		ERROR( "%s", "Error occurred while handling SIGTSTP in acs_logmaintd");
		return false;
	}
	return true;

}
/*=================================================================
	ROUTINE: ACE_TMAIN
=================================================================== */
int ACE_TMAIN(int argc, char *argv[])
{
	signal( SIGPIPE, SIG_IGN);
	//Event will be generated whenever acs_logmaintd service is crash.
	acs_aeh_setSignalExceptionHandler("acs_logmaintd");
	if(argc > 1)
	{
		if( argc > 2)
		{
			cout<<"Usage In Debug Mode : acs_logmaintd -d" <<endl;
			return 1;
		}
		else
		{
			if (strcmp(argv[1], "-d") == 0)
			{
				ACS_LOGM_Logging.Open("LOGM");
				myEndEvent = new ACE_Event(true,false);
				if(!setSignalHandling())
				{
					return -1;
				}
				DEBUG("ACE_TMAIN %s","Entering in Debug Mode");
				theCleanupPointer = new acs_logm_cleanupAction();
				theCleanupPointer->start();
				myEndEvent->wait();
				DEBUG("ACE_TMAIN %s","Leaving in Debug Mode");
				ACS_LOGM_Logging.Close();
			}
		}
	}
	else
	{
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
		acs_logm_logmservice *theLOGMMApplPtr = NULL;
		theLOGMMApplPtr = new acs_logm_logmservice("acs_logmaintd", "root");

		ACS_LOGM_Logging.Open("LOGM");

		DEBUG("ACE_TMAIN %s", "acs_logmaintd started in HA Mode");

		if( theLOGMMApplPtr != 0)
		{
			errorCode = theLOGMMApplPtr->activate();
			if (errorCode == ACS_APGCC_HA_FAILURE)
			{
				ERROR("ACE_TMAIN %s", "HA Activation Failed for acs_logmaintd");
			}
			else if( errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			{
				ERROR("ACE_TMAIN %s", "HA Application Failed to close acs_logmaintd gracefully");
			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS)
			{
				DEBUG("ACE_TMAIN %s", "HA Application Gracefully closed acs_logmaintd");
			}
			else
			{
				ERROR("ACE_TMAIN %s", "Error occurred while starting acs_logmaintd with HA");
			}
			delete theLOGMMApplPtr;
			theLOGMMApplPtr = 0;
		}
		else
		{
			ERROR("ACE_TMAIN %s", "Unable to allocate memory for ACS_LOGM_SERVICE object");
		}
		ACS_LOGM_Logging.Close();
	}
	return 0;
}//end of ACE_TMAIN
