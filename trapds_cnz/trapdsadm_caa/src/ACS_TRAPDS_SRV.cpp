/*
 * ACS_TRAPDS_SRV.cpp
 *
 *  Created on: Jan 23, 2012
 *      Author: eanform
 */

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>

#include "ACS_TRAPDS_HA_AppManager.h"
//#include "ACS_TRAPDS_Server.h"
//#include "ACS_TRAPDS_Event.h"

#include "acs_aeh_signalhandler.h"

void sighandler(int signum);
int setupSignalHandler(const struct sigaction* sigAction);
void printusage();
void CreateEvents();

ACS_TRAPDS_Server *g_ptr_TRAPDSServer = 0;
ACS_TRAPDS_HA_AppManager *haObj =0;

bool g_InteractiveMode = false;

ACS_TRAPDS_EventHandle shutdownEventHandle = 0;

static const char ACS_TRAPDS_Daemon[] = "acs_trapdsd";
using namespace std;

ACE_INT32 ACE_TMAIN( ACE_INT32 argc , ACE_TCHAR* argv[])
{
	ACE_INT32 result = 0;

	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask );

	syslog(LOG_INFO, "Starting acs_trapdsd main started.. ");


	// set the signal handler for the main
	result = setupSignalHandler(&sa);

	CreateEvents();

	if(0 == result )
	{
		if( argc > 1)
		{
			// fixs_cchd started by command line
			if( argc == 2 && ( 0 == strcmp(argv[1],"-d") ) )
			{
				g_InteractiveMode = true;

				g_ptr_TRAPDSServer = new (std::nothrow) ACS_TRAPDS_Server() ;

				if( NULL == g_ptr_TRAPDSServer )
				{
					cout << "Memory allocated failed for ACS_TRAPDS_Server" << endl;
					result = -1;
				}
				else
				{
					cout << "\n acs_trapdsd started in debug mode...\n\n" << endl;

					//g_ptr_TRAPDSServer->startWorkerThreads();
					result = g_ptr_TRAPDSServer->run();

					// wait signal SIGTERM for termination
					//ACE_OS::sleep(80);
					sigpause(SIGTERM);

					shutdownEventHandle = ACS_TRAPDS_Event::OpenNamedEvent(TRAPDS_Util::EVENT_NAME_SHUTDOWN);
					int retries = 0;

				    while (ACS_TRAPDS_Event::SetEvent(shutdownEventHandle) == 0)
					{
				    	cout << "Set shutdown.... retries: " << retries << endl;
						if( ++retries > 10) break;
						sleep(1);
					}

					sleep(1);
					//g_ptr_TRAPDSServer->waitOnShotdown();
					cout << "\n acs_trapdsd stopped in debug mode..." << endl;
					ACS_TRAPDS_Event::CloseEvent(shutdownEventHandle);
					delete g_ptr_TRAPDSServer;
				}
			}
			else
			{
				printusage();
				result = -1;
			}
		}
		else
		{
			acs_aeh_setSignalExceptionHandler(ACS_TRAPDS_Daemon);

			syslog(LOG_INFO, "acs_trapdsd started with CoreMw.. ");
			g_InteractiveMode = false;

			ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

			haObj = new ACS_TRAPDS_HA_AppManager(ACS_TRAPDS_Daemon);

			//Check for Memory allocation
			if( haObj == 0 )
			{
				printf("Memory allocation failed for ACS_TRAPDS_HA_AppManager Class");
			}
			else
			{
				syslog(LOG_INFO, "Starting acs_trapdsd service.. ");

				errorCode = haObj->activate();

				if (errorCode == ACS_APGCC_HA_FAILURE){
						syslog(LOG_ERR, "acs_trapdsd, HA Activation Failed!!");
						result = ACS_APGCC_FAILURE;
				}

				if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE){
						syslog(LOG_ERR, "acs_trapdsd, HA Application Failed to Gracefully closed!!");
						result = ACS_APGCC_FAILURE;
				}

				if (errorCode == ACS_APGCC_HA_SUCCESS){
						syslog(LOG_ERR, "acs_trapdsd, HA Application Gracefully closed!!");
						result = ACS_APGCC_FAILURE;
				}
			}
		}
	}

	if (haObj != NULL)
	{
		delete haObj;
		haObj = NULL;
	}
	syslog(LOG_INFO, "acs_trapdsd was terminated.. ");
	return result;
} //End of MAIN


void sighandler(int signum)
{
	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
		if(!g_InteractiveMode)
		{
			if(haObj != 0)
			{
				haObj->performComponentRemoveJobs();
				haObj->performComponentTerminateJobs();
			}
		}
		else
		{
			if(g_ptr_TRAPDSServer != 0)
			{
				g_ptr_TRAPDSServer->stopWorkerThreads();
			}
		}
	}
}

int setupSignalHandler(const struct sigaction* sigAction)
{
	if( sigaction(SIGINT, sigAction, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occurred while handling SIGINT in acs_trapdsd ");
		return -1;
	}

	if( sigaction(SIGTERM, sigAction, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occurred while handling SIGTERM in acs_trapdsd ");
		return -1;
	}

	if( sigaction(SIGTSTP, sigAction, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occurred while handling SIGTSTP in acs_trapdsd ");
		return -1;
	}

    return 0;
}

void printusage()
{
	cout<<"Usage: acs_trapdsd -d for debug mode.\n"<<endl;
}

void CreateEvents()
{
    	(void) ACS_TRAPDS_Event::CreateEvent(true, false, TRAPDS_Util::EVENT_NAME_SHUTDOWN);
}

