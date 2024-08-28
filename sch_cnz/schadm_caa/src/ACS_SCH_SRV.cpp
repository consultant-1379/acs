//******************************************************************************//
//  NAME
//     ACS_SCH_SRV.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
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
//
//
//  AUTHOR
//     2017-05-04  XMANVEN
//
//  SEE ALSO
//     -
//******************************************************************************

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>

#include "ACS_SCH_HA_AppManager.h"
#include "ACS_SCH_Server.h"
#include "ACS_SCH_Event.h"
#include "ACS_SCH_Logger.h"
#include <ACS_APGCC_Trace.H>
#include "ACS_TRA_trace.h"
#include <sys/eventfd.h>
//#include "ACS_SCH_Util.h"
#include <stdarg.h>
#include "ACS_CS_API.h"
#include <acs_apgcc_omhandler.h>
#include "acs_aeh_signalhandler.h"
#define RUNNING_DIR     "/tmp"
#define LOCK_FILE       "schsrv.lock"


bool isMultipleCPSystem = false;
void sighandler(int signum);
int setupSignalHandler(const struct sigaction* sigAction);
void CreateEvents();
void shutdown();
void daemonize();

enum SystemType {
	SINGLE_CP_SYSTEM = 0,
	MULTI_CP_SYSTEM  = 1
};

ACS_SCH_Server *g_ptr_SCHServer = 0;
ACS_SCH_HA_AppManager *haObj =0;

bool g_InteractiveMode = false;
bool g_noha_flag = false;
ACS_SCH_EventHandle shutdownEventHandle = 0;
static const char ACS_SCH_Daemon[] = "acs_schd";
using namespace std;

ACE_INT32 ACE_TMAIN( ACE_INT32 argc , ACE_TCHAR* argv[])
{
	ACE_INT32 result = 0;
	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask );

	syslog(LOG_INFO, "Starting acs_schd main started.. ");


	// set the signal handler for the main
	result = setupSignalHandler(&sa);

	CreateEvents();

	if(0 == result )
	{
		if( argc > 1)
		{
			// acs_schd started by command line
			if( argc == 2 && ( 0 == strcmp(argv[1],"--noha") ) )
			{
				cout<<"no ha started"<<endl;
				g_noha_flag  = true;
				g_ptr_SCHServer = new (std::nothrow) ACS_SCH_Server() ;
				if( NULL == g_ptr_SCHServer )
				{
					cout << "Memory allocated failed for ACS_SCH_Server" << endl;
					result = -1;
				}

				else
				{
					cout << "\nacs_schd started in noha mode...\n\n" << endl;
					result=g_ptr_SCHServer->activate();

					g_ptr_SCHServer->wait();

					if(g_ptr_SCHServer != NULL)
					{
						delete g_ptr_SCHServer;
						g_ptr_SCHServer = NULL;
					}

					cout << "\nacs_schd stopped in noha mode..." << endl;

					ACS_SCH_Event::CloseEvent(shutdownEventHandle);
				}
			}
		}

		else
		{
			syslog(LOG_INFO, "acs_schd started with CoreMw.. ");

			acs_aeh_setSignalExceptionHandler(ACS_SCH_Daemon);

			ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

			haObj = new ACS_SCH_HA_AppManager(ACS_SCH_Daemon);

			//Check for Memory allocation
			if( haObj == 0 )
			{
				printf("Memory allocation failed for ACS_SCH_HA_AppManager Class");
			}
			else
			{
				syslog(LOG_INFO, "Starting acs_schd service.. ");

				errorCode = haObj->activate();

				if (errorCode == ACS_APGCC_HA_FAILURE)
				{
					syslog(LOG_ERR, "acs_schd, HA Activation Failed!!");
					//close logging
					result = ACS_APGCC_FAILURE;
				}

				if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
				{
					syslog(LOG_ERR, "acs_schd, HA Application Failed to Gracefullly closed!!");
					//close logging
					result = ACS_APGCC_FAILURE;
				}

				if (errorCode == ACS_APGCC_HA_SUCCESS)
				{
					syslog(LOG_ERR, "acs_schd, HA Application Gracefully closed!!");
					//close logging
					result = ACS_APGCC_SUCCESS;
				}
			}
		}

	}


	if (haObj != NULL)
	{
		delete haObj;
		haObj = NULL;
	}
	syslog(LOG_INFO, "acs_schd was terminated.. ");
	return result;
} //End of MAIN


void sighandler(int signum)
{
	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
		if(!g_noha_flag)
		{
			if(haObj != 0)
			{
				haObj->performComponentRemoveJobs();
				haObj->performComponentTerminateJobs();
			}
		}
		else
		{
			if(g_ptr_SCHServer != 0)
			{
				shutdown();
			}
		}
	}
}

int setupSignalHandler(const struct sigaction* sigAction)
{
	if( sigaction(SIGINT, sigAction, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occurred while handling SIGINT in acs_schd ");
		return -1;
	}

	if( sigaction(SIGTERM, sigAction, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occurred while handling SIGTERM in acs_schd ");
		return -1;
	}

	if( sigaction(SIGTSTP, sigAction, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occurred while handling SIGTSTP in acs_schd ");
		return -1;
	}

	return 0;
}


void shutdown()
{
	shutdownEventHandle = ACS_SCH_Event::OpenNamedEvent(SCH_Util::EVENT_NAME_SHUTDOWN);
	if (shutdownEventHandle >= 0)
		(void) ACS_SCH_Event::SetEvent(shutdownEventHandle);
}


void daemonize()
{
	int i,lfp;
	char str[10] = { 0 };
	syslog(LOG_INFO, "Starting acs_schd daemonize.. ");

	if(getppid()==1)
		return; /*  already a daemon */

	i=fork();

	if (i<0){
		syslog(LOG_INFO, "acs_schd --- Fork error. ");
		exit(1); /*fork error */
	}
	if (i>0)
	{syslog(LOG_INFO, "acs_schd --- parent exits   child (daemon) continues  ");
	exit(0); /* parent exits */
	/* child (daemon) continues */
	}

	setsid(); /* obtain a new process group */

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standard I/O */
	umask(027); /* set newly created file permissions */
	chdir(RUNNING_DIR); /* change running directory */
	lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);

	if (lfp<0)
		exit(1); /* can not open */

	if (lockf(lfp,F_TLOCK,0)<0)
	{
		exit(0); /* can not lock */
	}
	/* first instance continues */
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */
}


void CreateEvents()
{
	(void) ACS_SCH_Event::CreateEvent(true, false, SCH_Util::EVENT_NAME_SHUTDOWN);
}


