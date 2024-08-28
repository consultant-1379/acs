//******************************************************************************
//
// NAME
// acs_rtr_service.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-05 by XLANSRI
//******************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ace/ACE.h"
#include <ace/Handle_Set.h>
#include "acs_rtr_ha_service.h"
#include "acs_rtr_global.h"
#include "acs_aeh_signalhandler.h"
#include "acs_rtr_server.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"
#include "acs_aca_constants.h"
#include <syslog.h>
#include <sys/types.h>
#include <sys/capability.h>

namespace {

bool setProcessCapability()
{
	// Clear CAP_SYS_RESOURCE bit thus root user cannot override disk quota limits
	bool retStatus=true;
	cap_t cap = cap_get_proc();
	if(NULL != cap)
	{
		cap_value_t cap_list[] = {CAP_SYS_RESOURCE};
		size_t NumberOfCap = sizeof(cap_list)/sizeof(cap_list[0]);
		// Clear capability CAP_SYS_RESOURCE
		if(cap_set_flag(cap, CAP_EFFECTIVE, NumberOfCap, cap_list, CAP_CLEAR) == -1)
		{
			// handle error
			char err_buff[128] = {0};
			snprintf(err_buff, sizeof(err_buff)-1, "%s, cap_set_flag() failed, error=%s", __func__, strerror(errno) );
			retStatus=false;
			syslog(LOG_ERR, err_buff );

		}
		if (cap_set_flag(cap, CAP_INHERITABLE, NumberOfCap, cap_list, CAP_CLEAR) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_set_flag() failed, error=%s", __func__, strerror(
							errno));
			retStatus = false;
			syslog(LOG_ERR, err_buff );

		}
		// Change process capability
		if (cap_set_proc(cap) == -1)
		{
			// handle error
			char err_buff[128] = {0};
			snprintf(err_buff, sizeof(err_buff)-1, "%s, cap_set_proc() failed, error=%s", __func__, strerror(errno) );
			retStatus=false;
			syslog(LOG_ERR, err_buff );
		}


		if(cap_free(cap) == -1)
		{
			// handle error
			char err_buff[128] = {0};
			snprintf(err_buff, sizeof(err_buff)-1, "%s, cap_free() failed, error=%s", __func__, strerror(errno) );
			retStatus=false;
			syslog(LOG_ERR, err_buff );
		}
	}
	else
	{
		// handle error
		char err_buff[128] = {0};
		snprintf(err_buff, sizeof(err_buff)-1, "%s, cap_get_proc() failed, error=%s", __func__, strerror(errno) );
		retStatus=false;
		syslog(LOG_ERR, err_buff );
	}
	syslog(LOG_INFO, "Capabilities modification status: %u", retStatus);
	return retStatus;
}
int change_mq_msgmax_limit () {

	syslog(LOG_INFO, "Changing the value of the maximum number of message queues..");

	const char * const MQ_MSG_MAX_FILE_PATH = "/proc/sys/fs/mqueue/msg_max";
	int fd = ACE_INVALID_HANDLE;
	ssize_t call_result = -1;
	char buffer[256] = {0};

	// Open the file containing the max number of messages in the message queue in the /proc file system
	if ((fd = ACE_OS::open(MQ_MSG_MAX_FILE_PATH, O_RDWR,ACE_DEFAULT_OPEN_PERMS)) < 0) {
		syslog(LOG_ERR, "Call 'open' failed, errno = %d",errno);
		return -1;
	}

	// Construct the string containing the new value to be written into the file
	int size_to_write = ::snprintf(buffer, sizeof(buffer), "%d", aca_rtr_communication::MESSAGE_QUEUE_MAX_NUM_OF_MESS);

	// Write the new value into the file// Construct the string containing the new value to be written into the file
	if ((call_result = ACE_OS::write(fd, buffer, size_to_write)) < size_to_write) {

		syslog(LOG_ERR,"Call 'write' failed, call_result = %zd, errno = %d",call_result,errno);
		return -1;
	}

	// Close the file previously opened
	if (ACE_OS::close(fd) < 0) {
		syslog(LOG_ERR,"Call 'close' failed, errno = %d",errno);
		return -1;
	}

	syslog(LOG_INFO,"Maximum number of message queues value correctly changed!");
	return 0;
}

}

// the "main" function in ACS_RTR_main
ACS_RTR_HA_Service *theRTRApplPtr = 0;
bool theInteractiveMode = false;
extern int StartUpRTR(int,char**,int);
void RTRsrv_exc();
void printUsage();
extern ACS_RTR_Server *server;
ACS_RTR_TRACE_DEFINE(ACS_RTR_SERVICE)

/*==========================================================================
        ROUTINE: sighandler
========================================================================== */
void sighandler(int signum)
{
	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
		syslog(LOG_INFO, "handle signal in acs_rtrd. signum = %d. Stop server requested", signum);
		if(!theInteractiveMode)
		{
			if(theRTRApplPtr != NULL)
			{
				theRTRApplPtr->performComponentRemoveJobs();
				theRTRApplPtr->performComponentTerminateJobs();
			}
		}
		else
		{
			RTRsrv_exc();
		}
	}
	if (signum == SIGPIPE)
	{
		syslog(LOG_INFO, "Ignore SIGPIPE signal in acs_rtrd.");
	}
}


//*************************************************************************
// main

// It all starts here.
//*************************************************************************
int ACE_TMAIN(int argc, char *argv[])
{
	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask );

	if( sigaction(SIGINT, &sa, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occured while handling SIGINT in acs_rtrd. errno: %d", errno);
		return -1;
	}
	if( sigaction(SIGTERM, &sa, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occured while handling SIGTERM in acs_rtrd. errno: %d", errno);
		return -1;
	}
	if( sigaction(SIGTSTP, &sa, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occured while handling SIGTSTP in acs_rtrd. errno: %d", errno);
		return -1;
	}

	if( sigaction(SIGPIPE, &sa, NULL ) == -1)
	{
		syslog(LOG_ERR, "Error occured while handling SIGPIPE in acs_rtrd. errno: %d", errno);
		return -1;
	}

	if( argc > 1)
	{
		// If -d flag is specified, then the user has requested to start the
		// service in debug mode.

		if( argc == 2 && (!strcmp(argv[1],"-d")) )
		{
			acs_rtr_logger::open("RTR");
			theInteractiveMode = true;
			if (change_mq_msgmax_limit())
			{
				syslog(LOG_ERR,"Failed to set maximum msgmax limit");
				return -1;
			}
			if(!setProcessCapability())
			{
				syslog(LOG_ERR, "Error while setting process capabilities for acs_rtrd. errno: %d", errno);
			}

			int rc=0;
			StartUpRTR(argc,argv,rc);
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

		//The HA constructor invokes a fork
		theRTRApplPtr = new ACS_RTR_HA_Service("acs_rtrd", "root");
		if (change_mq_msgmax_limit())
		{
			syslog(LOG_ERR,"Failed to set maximum msgmax limit");
			return -1;
		}

		if(!setProcessCapability())
		{
			syslog(LOG_ERR, "Error while setting process capabilities for acs_rtrd. errno: %d", errno);
		}
		//The LOG open must be invoked after the HA instantiation
		acs_rtr_logger::open("RTR");

		if (!theRTRApplPtr)
		{
			syslog(LOG_ERR, "Error: Unable to allocate memory for HA Service. errno: %d", errno);
			acs_rtr_logger::close();
			return -2;
		}

		errorCode = theRTRApplPtr->activate();

		if (errorCode == ACS_APGCC_HA_FAILURE)
		{
			syslog(LOG_ERR, "HA Activation Failed for acs_rtrd. errno: %d.", errno);
		}
		else if( errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
		{
			syslog(LOG_ERR, "HA Application Failed to close gracefully for acs_rtrd. errno: %d", errno);
		}
		else if (errorCode == ACS_APGCC_HA_SUCCESS)
		{
			syslog(LOG_INFO, "HA Application Gracefully closed for acs_rtrd");
		}
		else
		{
			syslog(LOG_ERR, "Cannot start acs_rtrd with HA. errorCode = %d", errorCode);
		}
		delete theRTRApplPtr;
		theRTRApplPtr = 0;
	}

	acs_rtr_logger::close();
	return 0;
}

void printUsage()
{
	cout<<"Usage: acs_rtrd [-d]" <<endl;
}

//*************************************************************************
// RTRsrv_exc

// Exception handler.
//*************************************************************************
void RTRsrv_exc()
{
	ACS_RTR_TRACE_MESSAGE("In");
	if (server) 
	{
		server->stop();
		server->wait();
	}
	ACS_RTR_TRACE_MESSAGE("Out");

}	
