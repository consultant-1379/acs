/*=================================================================== */
   /**
   @file acs_nsf_service.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the acs_nsf_service.h module.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.
   N/A       06/02/2014     XQUYDAO        Updated to handle better AMF callback
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_nsf_server.h"
#include "acs_nsf_types.h"
#include "acs_nsf_common.h"
#include <poll.h>
#include "acs_nsf_service.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <acs_aeh_signalhandler.h>


ACS_NSF_Service *theNSFApplPtr = 0;
ACS_NSF_Server *theNSFServerInteractive = 0;
bool theInteractiveMode = false;

/*========================================================================== 
	ROUTINE: sighandler
========================================================================== */
void sighandler(int signum)
{
	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
		if(!theInteractiveMode)
		{
			if(theNSFApplPtr != 0)
			{
				theNSFApplPtr->performComponentRemoveJobs();
				theNSFApplPtr->performComponentTerminateJobs();
			}
		}
		else
		{
			if(theNSFServerInteractive != 0)
			{
				theNSFServerInteractive->StopServer();
			}
		}
	}
}

/*=============================================================================== 
	ROUTINE: ACS_NSF_Service
 =============================================================================== */
ACS_NSF_Service::ACS_NSF_Service(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	DEBUG("%s", "Entering ACS_NSF_Service::ACS_NSF_Service");	
	theNSFServerPtr = 0;
	DEBUG("%s", "Leaving ACS_NSF_Service::ACS_NSF_Service");
}

/*================================================================================ 
	ROUTINE: ~ACS_NSF_Service
 ================================================================================ */
ACS_NSF_Service::~ACS_NSF_Service()
{
	// to be sure.
	this->shutdownApp();
}


/*================================================================================ 
	ROUTINE: performStateTransitionToActiveJobs
 ================================================================================ */
ACS_APGCC_ReturnType ACS_NSF_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG( "%s", "Entering ACS_NSF_Service::performStateTransitionToActiveJobs");	
	return this->activateApp(previousHAState);
	DEBUG("%s", "Leaving ACS_NSF_Service::performStateTransitionToActiveJobs");	
}

/*=================================================================================== 
	ROUTINE: performStateTransitionToPassiveJobs
 ================================================================================== */
ACS_APGCC_ReturnType ACS_NSF_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s", "Entering ACS_NSF_Service::performStateTransitionToPassiveJobs");
	return this->passifyApp(previousHAState);
	DEBUG("%s", "Leaving ACS_NSF_Service::performStateTransitionToPassiveJobs");	
}

/*============================================================================================ 
	ROUTINE: performStateTransitionToQueisingJobs
 ============================================================================================ */
ACS_APGCC_ReturnType ACS_NSF_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s", "Entering ACS_NSF_Service::performStateTransitionToQueisingJobs");
	(void) previousHAState;
	DEBUG("%s", "Leaving ACS_NSF_Service::performStateTransitionToQueisingJobs");
	return this->shutdownApp();
}

/*============================================================================================= 
	ROUTINE: performStateTransitionToQuiescedJobs
 ============================================================================================= */
ACS_APGCC_ReturnType ACS_NSF_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s", "Entering ACS_NSF_Service::performStateTransitionToQueiscedJobs");
	(void) previousHAState;
	DEBUG("%s", "Leaving ACS_NSF_Service::performStateTransitionToQueiscedJobs");
	return this->shutdownApp();
}

/*==================================================================================== 
	ROUTINE: performComponentHealthCheck
 =================================================================================== */
ACS_APGCC_ReturnType ACS_NSF_Service::performComponentHealthCheck(void)
{
#if 0	
	DEBUG("%s", "Entering ACS_NSF_Service::performComponentHealthCheck");

	// Do any application specific health check activities before responding

	if( applicationThreadId > 0  && Is_terminated == FALSE )
	{
		int result = ThrExitHandler::CheckAllFuncThreads();
		if( result == true )
		{
			DEBUG("%s", "Health Check Success.");
			//DEBUG("%s", "Leaving ACS_NSF_Service::performComponentHealthCheck");
			return ACS_APGCC_SUCCESS;	
		}
		else
		{
			DEBUG("%s", "Health Check failed ");
			//DEBUG("%s", "Leaving ACS_NSF_Service::performComponentHealthCheck");
			return ACS_APGCC_FAILURE;
		}
	}
	DEBUG("%s", "Health Check Success.");
	DEBUG("%s", "Leaving ACS_NSF_Service::performComponentHealthCheck");
#endif	
	return ACS_APGCC_SUCCESS;	
}


/*===================================================================== 
	ROUTINE: performComponentTerminateJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_NSF_Service::performComponentTerminateJobs(void)
{
	DEBUG("%s", "Entering ACS_NSF_Service::performComponentTerminateJobs");
	DEBUG("%s", "Leaving ACS_NSF_Service::performComponentTerminateJobs");
	return this->shutdownApp();
}

/*=======================================================================
	ROUTINE: performComponentRemoveJobs
 ======================================================================= */
ACS_APGCC_ReturnType ACS_NSF_Service::performComponentRemoveJobs(void)
{
	DEBUG("%s", "Entering ACS_NSF_Service::performComponentRemoveJobs");
	DEBUG("%s", "Leaving ACS_NSF_Service::performComponentRemoveJobs");
	return this->shutdownApp();
}

/*====================================================================== 
	ROUTINE: performApplicationShutdownJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_NSF_Service::performApplicationShutdownJobs() 
{
	DEBUG("%s", "Entering ACS_NSF_Service::performApplicationShutdownJobs");
	DEBUG("%s", "Leaving ACS_NSF_Service::performApplicationShutdownJobs");
	return this->shutdownApp();
}

ACS_APGCC_ReturnType ACS_NSF_Service::activateApp(ACS_APGCC_AMF_HA_StateT previousHAState) 
{
	DEBUG("%s", "Entering ACS_NSF_Service::activateApp()");	
	
	if (previousHAState == ACS_APGCC_AMF_HA_ACTIVE)
	{
		DEBUG("%s", "application is already active");
		return ACS_APGCC_SUCCESS;
	}	
	
	if (0 != this->theNSFServerPtr)
	{		
		DEBUG("%s", "passive->active transition. stop passive work before becoming active");
		this->shutdownApp();		
	}
	
	ACE_NEW_NORETURN(this->theNSFServerPtr, ACS_NSF_Server());
	if (0 == this->theNSFServerPtr)
	{
		DEBUG("%s", "failed to create NSF_Server instance");
		return ACS_APGCC_FAILURE;
	}
	
	int res = this->theNSFServerPtr->active(this); // This will start active functionality.
	// Will not return until ACS_NSF_Server  is running
	if (res < 0)
	{
		// Failed to start
		delete this->theNSFServerPtr;
		this->theNSFServerPtr = 0;
		return ACS_APGCC_FAILURE;
	}
	else 
	{
		DEBUG("%s","application is now activated by HA");			
	}
	
	DEBUG("%s","Exiting ACS_NSF_Service::activateApp()");
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_NSF_Service::shutdownApp()
{
	if ( 0 != this->theNSFServerPtr)
	{
		DEBUG("%s","Ordering App to shutdown");
		this->theNSFServerPtr->stop(); // This will initiate the application shutdown and 
		//will not return until application is stopped completely.

		DEBUG("%s","Waiting for App to shutdown...");
		this->theNSFServerPtr->wait();

		DEBUG("%s","Deleting App instance...");
		delete this->theNSFServerPtr;
		this->theNSFServerPtr=0;
	}
	else
		DEBUG("%s","shutdownApp already done");
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_NSF_Service::passifyApp(ACS_APGCC_AMF_HA_StateT previousHAState) 
{
	DEBUG("%s","Entering ACS_NSF_Service::passifyApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if (previousHAState == ACS_APGCC_AMF_HA_STANDBY)
	{
		DEBUG("%s", "application is already passive");
		return ACS_APGCC_SUCCESS;
	}

	if (0 != this->theNSFServerPtr)
	{
		DEBUG("%s", "active->passive transition. stop active work before becoming passive");
		this->shutdownApp();	
	}

	ACE_NEW_NORETURN(this->theNSFServerPtr, ACS_NSF_Server());
	if (0 == this->theNSFServerPtr)
	{
		DEBUG("%s", "failed to create the instance");
		return ACS_APGCC_FAILURE;
	}
	
	int res = this->theNSFServerPtr->passive(this); // This will start passive functionality and
	// Will not return until myCLass is running
	if (res < 0)
	{
		// Failed to start
		delete this->theNSFServerPtr;
		this->theNSFServerPtr = 0;
		return ACS_APGCC_FAILURE;
	}
	else 
	{
		DEBUG("%s","App is now passivated by HA");		
	}
	
	DEBUG("%s","Exiting ACS_NSF_Service::passifyApp()");
	return ACS_APGCC_SUCCESS;
}

/*===============================================================
	ROUTINE: printusage()
================================================================ */
void printusage()
{
	cout<<"Usage:\nacs_nsfserverd -d for debug mode."<<endl;
}

/*=================================================================
	ROUTINE: ACE_TMAIN
=================================================================== */
ACE_INT32 ACE_TMAIN( ACE_INT32 argc , ACE_TCHAR* argv[])
{
        acs_aeh_setSignalExceptionHandler("acs_nsfserverd");  //To generate an event in case of service crash
	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask );

	ACS_NSF_Common::ACS_NSF_Logging.Open("NSF");

	if( sigaction(SIGINT, &sa, NULL ) == -1)
	{
		ERROR("%s", "Error occured while handling SIGINT in acs_nsfserverd");
		ACS_NSF_Common::ACS_NSF_Logging.Close();
		return -1;
	}
	if( sigaction(SIGTERM, &sa, NULL ) == -1)
	{
		ERROR( "%s", "Error occured while handling SIGTERM in acs_nsfserverd");
		ACS_NSF_Common::ACS_NSF_Logging.Close();
		return -1;
	}
	if( sigaction(SIGTSTP, &sa, NULL ) == -1)
	{
		ERROR( "%s", "Error occured while handling SIGTSTP in acs_nsfserverd");
		ACS_NSF_Common::ACS_NSF_Logging.Close();
		return -1;
	}

	//Check the number of arguments.
	if( argc > 1)
	{
		// If -d flag is specified, then the user has requested to start the
		// service in debug mode.

		if( argc == 2 && (!strcmp(argv[1],"-d")) )
		{
			theInteractiveMode = true;

			DEBUG("%s", "Starting acs_nsfserverd in debug mode...");

			//Allocate memory for NSF Server.

			theNSFServerInteractive = new ACS_NSF_Server() ;

			if( theNSFServerInteractive == 0)
			{
				ERROR("%s", "Memory allocated failed for ACS_NSF_Server");
				ERROR("%s", "Shutting down ACS_NSF_Server.");
			}
			else
			{
				DEBUG("%s", "acs_nsfserverd started in debug mode...");

				ACS_CC_ReturnType result = theNSFServerInteractive->setupNSFThread(theNSFServerInteractive);
				if( result == ACS_CC_SUCCESS )
				{
					if( theNSFServerInteractive->NSFFunctionalThreadId != 0 )
					{
						ACE_Thread_Manager::instance()->join(theNSFServerInteractive->NSFFunctionalThreadId);
					}
				}

				DEBUG("%s", "acs_nsfserverd stopped in debug mode...");

				delete theNSFServerInteractive;
				theNSFServerInteractive = 0;
			}
		}
		else
		{
			printusage();
		}
	}
	else
	{
		DEBUG("%s", "acs_nsfserverd started with CoreMw");

		theInteractiveMode = false;

		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

		theNSFApplPtr = new ACS_NSF_Service("acs_nsfserverd", "root");
		if (!theNSFApplPtr)
		{
			ERROR("%s", "Unable to allocate memory for ACS_NSF_Service object");
			int rCode = 0;
			rCode=-2;
			return rCode;
		}


		DEBUG("%s", "Starting acs_nsfserverd service with HA...");

		errorCode = theNSFApplPtr->activate();

		if (errorCode == ACS_APGCC_HA_FAILURE)
		{
			ERROR("%s", "HA Activation Failed for acs_nsfserverd");
		}
		else if( errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
		{
			ERROR("%s", "HA Application Failed to close gracefully for acs_nsfserverd");
		}
		else if (errorCode == ACS_APGCC_HA_SUCCESS)
		{
			DEBUG("%s", "HA Application Gracefully closed acs_nsfserverd");
		}
		else
		{
			ERROR("%s", "Error occured while starting acs_nsfserverd with HA");
		}
		delete theNSFApplPtr;
		theNSFApplPtr = 0;
	}
	ACS_NSF_Common::ACS_NSF_Logging.Close();
	return 0;
} //End of MAIN


