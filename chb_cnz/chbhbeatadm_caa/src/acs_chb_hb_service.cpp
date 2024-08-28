/*=================================================================== */
   /**
   @file acs_chb_hb_service.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of the main class for heartbeat.

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
#include <acs_chb_heartbeat_class.h>

/**
 * @brief	pHbObj
 */
acs_chb_heartbeat_class *pHbObj  = 0;

/*=====================================================================
	ROUTINE: sighandler
=======================================================================*/
void sighandler(int signum)
{
        if( signum == SIGINT || signum == SIGTERM || signum == SIGTSTP)
        {
        	//Handle signal in debug mode.
        	if( debug == true )
        	{
        		INFO(1, "%s", "Received signal in acs_chbheartbeatd");
        		// Signal Stop event.
        		char buf[] = { 1 , 1};
        		int bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
        		if( bytes <= 0 )
        		{
        			ERROR(1, "%s", "Error occurred while signaling stop event ");
        			ERROR(1, "%s", "Leaving signal handler");
        		}
        		else
        		{
        			INFO(1, "%s", "Stop event signaled ");
        		}
        	}
        }
}//End of sighandler

/*========================================================================
	ROUTINE:	main
==========================================================================*/
int main(int argc, char **argv)
{
	if ( argc >  1)
	{
		if( argc ==  2  && (strcmp( argv[1], "-d") == 0 ))
		{
			// Open the TRA logging file.
			ACS_CHB_Tra::ACS_CHB_Logging.Open("HBEAT");

                        //Create pipe to signal end to application thread.
			if( ACE_OS::pipe(pipeHandles) == -1 )
			{
				ACS_CHB_Common::CHB_serviceError("CANNOT OPEN PIPE FOR END EVENT.", ACE_OS::last_error());
				ERROR(1, "%s", "Error occured while creating pipe for STOP event!!");
				ERROR(1, "%s", "Exiting acs_chbheartbeatd");
				ACS_CHB_Tra::ACS_CHB_Logging.Close();
			}

			//User has executed the service in debug mode.
			debug = true;

			//Register a signal handler.
			struct sigaction sa;
			sa.sa_handler = sighandler;
			sa.sa_flags = SA_RESTART;
			sigemptyset(&sa.sa_mask );

			if( sigaction(SIGINT, &sa, NULL ) == -1)
			{
				ERROR(1, "%s", "Error occured while handling SIGINT in acs_chbheartbeatd");
				ACS_CHB_Tra::ACS_CHB_Logging.Close();
				return -1;
			}
			if( sigaction(SIGTERM, &sa, NULL ) == -1)
			{
				ERROR(1, "%s", "Error occured while handling SIGTERM in acs_chbheartbeatd");
				ACS_CHB_Tra::ACS_CHB_Logging.Close();
				return -1;
			}
			if( sigaction(SIGTSTP, &sa, NULL ) == -1)
			{
				ERROR(1, "%s", "Error occured while handling SIGTSTP in acs_chbheartbeatd");
				ACS_CHB_Tra::ACS_CHB_Logging.Close();
				return -1;
			}

			DEBUG(1, "%s", "User started the acs_chbheartbeatd service in debug mode");
			//To do . Start CHB service in debug mode.

			ACE_thread_t ACSCHBHeartBeatThread2Id = 0;
			int hbThreadGroupId = ACE_Thread_Manager::instance()->spawn(&CHB_serviceMain,
								NULL,
								THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
								&ACSCHBHeartBeatThread2Id,
								0,
								ACE_DEFAULT_THREAD_PRIORITY,
								-1,
								0, ACE_DEFAULT_THREAD_STACKSIZE );
			if (  hbThreadGroupId == -1)	//Thread Creation failed.
			{
				ERROR(1, "%s", "Error occurred while creating the ACSCHBHeartBeatThread2");
			}
			else
			{
				//Wait for the thread to exit.
				ACE_Thread_Manager::instance()->join(ACSCHBHeartBeatThread2Id);
			}

			DEBUG(1, "%s", "Ending the acs_chbheartbeatd service in debug mode");
			ThrExitHandler::cleanup();
			ERROR(1, "%s", "Leaving acs_chbheartbeatd");
			ACS_CHB_Tra::ACS_CHB_Logging.Close();

		}
		else
		{
			//User has executed service in an incorrect manner.
			cout << "Usage:\nacs_chbheartbeatd -d for debug mode" << endl;
		}
	}
	else
	{
		//User has requested Core Middleware to start the heartbeat service.
		debug = false;
		
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

		//Create an object of acs_chb_heartbeat_class with user as root.
		acs_chb_heartbeat_class *pHbObj = new acs_chb_heartbeat_class("acs_chbheartbeatd", "root");

		//Open the TRA logging file.
		ACS_CHB_Tra::ACS_CHB_Logging.Open("HBEAT");

		//Check for Memory allocation
		if( pHbObj == 0 )
		{
			ERROR(1, "%s", "Memory allocation failed for acs_chb_heartbeat_class");
		}
		else
		{
			//Register with core middleware.
			DEBUG(1, "%s", "Starting acs_chbheartbeatd service with HA.. ");
			errorCode = pHbObj->activate();
	
			if (errorCode == ACS_APGCC_HA_FAILURE)
			{
				ERROR(1, "%s", "HA Activation Failed for acs_chbheartbeatd");
			}
			else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			{
				ERROR(1, "%s", "HA Failed to close acs_chbheartbeatd gracefully");
			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS)
			{
				ERROR(1, "%s", "HA Gracefully closed acs_chbheartbeatd!!");
			}
			else
			{
				ERROR(1, "%s", "Error occured while integrating acs_chbheartbeatd with HA");
			}
			delete pHbObj;
			pHbObj = 0;
		}
		ERROR(1, "%s", "Leaving acs_chbheartbeatd");
		ACS_CHB_Tra::ACS_CHB_Logging.Close();
	}
	return 0;
}
