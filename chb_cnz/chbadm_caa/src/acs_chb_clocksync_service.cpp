/*=================================================================== */
   /**
   @file acs_chb_clocksync_service.cpp

   This module contains the implementation of the main class for clocksync.

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       20/04/2011   XTANAGG   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
using namespace std;
#include <acs_chb_clocksync_class.h>

/**
 * @brief	pClockSyncObj
 */
acs_chb_clocksync_class *pClockSyncObj = 0;

/*=================================================================
	ROUTINE: sighandler
=================================================================== */
void sighandler(int signum)
{
        if( signum == SIGINT || signum == SIGTERM || signum == SIGTSTP)
        {
        	INFO(1,"%s","Received signal in acs_chbclocksyncd");
        	if( debug == true )
        	{
        		INFO(1,"%s","Signalling stop event!!");

        		globalFlags |= KILL;
        		// TR HQ37527 - BEGIN
        		acs_chb_clocksync_class::sendStopToPipe();
        		// TR HQ37527 - END
        		acs_chb_clocksync_class::setStopSignal(true);


        		//Signal the stop event.
        		evh.signal();
        	}
        }
}//End of sighandler

/*=========================================================================
	ROUTINE: main
========================================================================= */
int main(int argc, char **argv)
{
	if(argc > 1 )
	{
		if( argc ==  2  && (strcmp( argv[1], "-d") == 0 ))
		{
			debug = true;

                       	ACS_CHB_Tra::ACS_CHB_Logging.Open("CSYNC");


			struct sigaction sa;
			sa.sa_handler = sighandler;
			sa.sa_flags = 0;
			sigemptyset(&sa.sa_mask );

			if( sigaction(SIGINT, &sa, NULL ) == -1)
			{
				ERROR( 1, "%s", "Error occured while handling SIGINT in acs_chbclocksyncd");
				ACS_CHB_Tra::ACS_CHB_Logging.Close();
				return -1;
			}
			if( sigaction(SIGTERM, &sa, NULL ) == -1)
			{
				ERROR( 1, "%s", "Error occured while handling SIGTERM in acs_chbclocksyncd");
				ACS_CHB_Tra::ACS_CHB_Logging.Close();
				return -1;
			}
			if( sigaction(SIGTSTP, &sa, NULL ) == -1)
			{
				ERROR( 1, "%s", "Error occured while handling SIGTSTP in acs_chbclocksyncd");
				ACS_CHB_Tra::ACS_CHB_Logging.Close();
				return -1;
			}

			DEBUG( 1, "%s", "User started the acs_chbclocksyncd service in debug mode");
			//To do . Start Clock Sync service in debug mode.
			
			if (ACS_CHB_Common::isVirtualEnvironment() == true )// get environment for clock synchronizer
			{
				DEBUG(1,"%s","Inhibiting clock synchronizer in Virtual Environment");				
				return 0;
			}

			ACE_thread_t clockSyncThreadId = 0;
			int clockSyncThreadGroupId = ACE_Thread_Manager::instance()->spawn(&ClockSyncServiceMain,
								NULL, 
								THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
								&clockSyncThreadId,
								 0,
								 ACE_DEFAULT_THREAD_PRIORITY,
								 -1,
								 0,
								 ACE_DEFAULT_THREAD_STACKSIZE); 
			if (clockSyncThreadGroupId == -1)
			{
				ERROR(1, "%s", "Error occurred while creating ACSCHBClockSyncThread2");
			}

			ACE_Thread_Manager::instance()->join(clockSyncThreadId);
			ThrExitHandler::cleanup();

                        DEBUG( 1, "%s", "Leaving acs_chbclocksyncd service in debug mode");
                        ACS_CHB_Tra::ACS_CHB_Logging.Close();
		}
		else
		{
			cout << "Usage:\nacs_chbclocksyncd -d for debug mode" << endl;
		}
	}
	else
	{
		//Run the service with Core Middleware.
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

		//Allocate memory for pClockSyncObj.
		//Execute acs_chbclocksyncd with user "root".
		pClockSyncObj = new acs_chb_clocksync_class("acs_chbclocksyncd", "root");

                ACS_CHB_Tra::ACS_CHB_Logging.Open("CSYNC");

		
		if( pClockSyncObj == 0 )
		{
			ERROR(1, "%s", "Unable to allocate memory for acs_chb_clocksync_class");
		}
		else
		{

			DEBUG(1, "%s", "Starting acs_chbclocksyncd service with HA.. ");

			errorCode = pClockSyncObj->activate();
	
			if (errorCode == ACS_APGCC_HA_FAILURE)
			{
				ERROR(1, "%s", "HA Activation Failed for acs_chbclocksyncd");
			}
			else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			{
				ERROR(1, "%s", "HA Application Failed to close gracefully for acs_chbclocksyncd");
			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS)
			{
				ERROR(1, "%s", "HA Application Gracefully closed for acs_chbclocksyncd!!");
			}
			else
			{
				ERROR(1, "%s", "Error occured while integrating acs_chbclocksyncd with HA");
			}
			delete pClockSyncObj;
			pClockSyncObj = 0;

                        DEBUG( 1, "%s", "Leaving Clcok Sync service in HA mode");
                        ACS_CHB_Tra::ACS_CHB_Logging.Close();
		}
	}

	return 0;
}
