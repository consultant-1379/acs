/*=================================================================== */
   /**
   @file acs_chb_mtzln_svc.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the module

   @version 1.0.0
	*/
	/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       20/01/2011   XNADNAR   Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_chb_mtzln_implementer.h>
#include <acs_chb_mtzln_service.h>
#include <acs_apgcc_oihandler_V2.h>
#include <ace/TP_Reactor.h>
#include <ace/Reactor.h>
#include <ACS_APGCC_AmfTypes.h>
#include <ACS_APGCC_ApplicationManager.h>
#include <iostream>
#include <string>
#include <acs_prc_api.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <acs_chb_tra.h>
using namespace std;

/*=====================================================================
                        GLOBAL VARIABLE
==================================================================== */
/*=================================================================== */
/**
   @brief  g_timeZoneMap
		   This map stores all the timezones
		   monitored by APG.

 */
/*=================================================================== */
FileMapType g_timeZoneMap;
/*=================================================================== */
/**
   @brief  theMTZLNApplPtr

 */
/*=================================================================== */
 ACS_CHB_MTZLN_Service *theMTZLNApplPtr = 0;
/*=================================================================== */
/**
   @brief  theMTZLNImplementerPtr

 */
/*=================================================================== */
ACS_CHB_MTZLN_Implementer *theMTZLNImplementerPtr =0;
/*=================================================================== */
/**
   @brief  theInteractiveMode

 */
/*=================================================================== */
bool theInteractiveMode = false;


/*===================================================================
   ROUTINE: sighandler
=================================================================== */
void sighandler(int signum)
{
	if( (signum == SIGTERM) ||(signum == SIGINT) ||
		(signum == SIGTSTP) )
	{
		if(!theInteractiveMode)
		{
			//This will happen if clc script is terminated by  signal.
			if(theMTZLNApplPtr != 0)
			{
				theMTZLNApplPtr->performComponentRemoveJobs();
				theMTZLNApplPtr->performComponentTerminateJobs();
			}
		}
		else
		{
			if(theMTZLNImplementerPtr != 0)
			{
				theMTZLNImplementerPtr->shutdown();
				delete theMTZLNImplementerPtr;

				theMTZLNImplementerPtr = 0;
			}
		}
	}
}//End of sighandler


/*====================================================================
	ROUTINE: printUsage
======================================================================*/
void printUsage()
{
	cout << "Usage:\nacs_chbmtzlnd -d for debug mode" << endl;
}


/*===================================================================
   ROUTINE: ACE_TMAIN
=================================================================== */
ACE_INT32 ACE_TMAIN( ACE_INT32 argc , ACE_TCHAR* argv[])
{

	ACS_CHB_Tra::ACS_CHB_Logging.Open("MTZLN");

	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask );

	if( sigaction(SIGINT, &sa, NULL ) == -1)
	{
		ERROR(1, "%s", "Error occured while handling SIGINT in acs_chbmtzlnd");
		ACS_CHB_Tra::ACS_CHB_Logging.Close();
		return -1;
	}
	if( sigaction(SIGTERM, &sa, NULL ) == -1)
	{
		ERROR(1, "%s", "Error occured while handling SIGTERM in acs_chbmtzlnd");
		ACS_CHB_Tra::ACS_CHB_Logging.Close();
		return -1;
	}
	if( sigaction(SIGTSTP, &sa, NULL ) == -1)
	{
		ERROR(1, "%s", "Error occured while handling SIGTSTP in acs_chbmtzlnd");
		ACS_CHB_Tra::ACS_CHB_Logging.Close();
		return -1;
	}
	if(argc > 1)
	{
		if( argc == 2 && !strcmp(argv[1],"-d") )
		{
			//Check the state of the node using PRC API.
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if( nodeState == 1)	//Active Node
			{
				//Set interactive mode as true.
				theInteractiveMode = true;

				//Set the implementer name.
				ACE_TCHAR MTLNImplName[100], MTZClassName[100];
				ACE_OS::memset(MTLNImplName, 0, sizeof(MTLNImplName));
				ACE_OS::strcpy(MTLNImplName, ACS_CHB_MTZLN_IMPLEMENTER);

				//Set the class name.
				ACE_OS::memset(MTZClassName,0, sizeof(MTZClassName));
				ACE_OS::strcpy(MTZClassName, ACS_CHB_MTZLN_IMM_CLASS_NM );

				//Allocate memory for ACS_CHB_MTZLN_Implementer
				theMTZLNImplementerPtr = new ACS_CHB_MTZLN_Implementer(MTZClassName, MTLNImplName, ACS_APGCC_ONE);

				if( theMTZLNImplementerPtr != 0 )
				{
					//Start the functionality in a separate thread.
					if( ACS_CC_FAILURE == theMTZLNImplementerPtr->setupMTZThread(theMTZLNImplementerPtr))
					{
						if( theMTZLNImplementerPtr != 0 )
						{
							delete theMTZLNImplementerPtr;
							theMTZLNImplementerPtr = 0;
		
							ERROR(1, "%s", "Leaving acs_chbmtzlnd");
							ACS_CHB_Tra::ACS_CHB_Logging.Close();
							return 0;
						}
					}

					ACE_Thread_Manager::instance()->join(theMTZLNImplementerPtr->mtzFuncThreadId);

					if( theMTZLNImplementerPtr != 0 )
					{
						delete theMTZLNImplementerPtr;
						theMTZLNImplementerPtr = 0;
					}
					DEBUG(1, "%s", "Leaving acs_chbmtzlnd");
				}
				else
				{
					ERROR(1, "%s", "Memory allocation failed for theMTZLNImplementerPtr");
					ERROR(1, "%s", "Leaving acs_chbmtzlnd");
				}
			}
			else if( nodeState == 2)
			{
				DEBUG(1, "%s", "Node state is Passive!!");
				DEBUG(1, "%s", "Please run the service in debug mode on active node");	
				DEBUG(1, "%s", "Exiting....");
			}
			else
			{
				printUsage();
			}

		}
		else
		{
			printUsage();
		}
	}
	else 
	{
		theInteractiveMode = false;

		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

		//Allocate memory for ACS_CHB_MTZLN_Service
		theMTZLNApplPtr = new ACS_CHB_MTZLN_Service("acs_chbmtzlnd", "root");

		//Exit, if memory allocation fails.

		if( theMTZLNApplPtr == 0)
		{
			ERROR(1, "%s", "Memory allocation failed for ACS_CHB_MTZLN_Service");
			ERROR(1, "%s", "Leaving acs_chbmtzlnd");
		}
		else
		{
			DEBUG(1, "%s", "Starting acs_chbmtzlnd service.. ");

			errorCode = theMTZLNApplPtr->activate();

			if (errorCode == ACS_APGCC_HA_FAILURE)
			{
				ERROR(1, "%s", "HA Activation Failed for acs_chbmtzlnd");
			}
			else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			{
				ERROR(1, "%s", "HA Application Failed to Gracefullly closed for acs_chbmtzlnd");
			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS)
			{
				ERROR(1, "%s", "HA Application Gracefully closed for acs_chbmtzlnd");
			}
			else
			{
				ERROR(1, "%s", "Error occured while integrating acs_chbmtzlnd with HA");
			}
			delete theMTZLNApplPtr;
			theMTZLNApplPtr = 0;
		}
	}
	ACS_CHB_Tra::ACS_CHB_Logging.Close();
	return 0;
}//End of ACE_TMAIN

