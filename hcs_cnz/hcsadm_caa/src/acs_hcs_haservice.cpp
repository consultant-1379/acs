//******************************************************************************
//
//  NAME
//     acs_hcs_haservice.cpp
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
//     2013-05-29 by XLANSRI
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_hcs_haservice.h"
#include <syslog.h>
#include <sys/capability.h>

/*=================================================================
        ROUTINE: printUsage
=================================================================*/

void ACS_HC_HAService::printUsage()
{
	DEBUG("ACS_HC_Service::printUsage() %s","Entering");
	cout<<"Usage: acs_hcs_server -d" <<endl;
	DEBUG("ACS_HC_Service::printUsage() %s","Leaving");
}
/*=================================================================
	ROUTINE: ACS_HC_HAService constructor
=================================================================== */
ACS_HC_HAService::ACS_HC_HAService( const char* daemon_name,
				const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	syslog(LOG_INFO, "Entering constructor ");
	DEBUG("%s", "Entering ACS_HC_HAService::ACS_HC_HAService");
	servicePtr = NULL;
	// Clear CAP_SYS_RESOURCE bit thus root user cannot override disk quota limits
	cap_t cap = cap_get_proc();

	if(NULL != cap)
	{
		cap_value_t cap_list[1];
		cap_list[0] = CAP_SYS_RESOURCE;

		// Clear capability CAP_SYS_RESOURCE
		if(cap_set_flag(cap, CAP_EFFECTIVE, 1, cap_list, CAP_CLEAR) == -1)
		{
			DEBUG("%s", "cap_set_flag() failed, error=%d", errno);
		}
		else
		{
			// Change process capability
			if (cap_set_proc(cap) == -1)
			{
				DEBUG("%s", "cap_set_proc() failed, error=%d", errno);
			}
		}

		if(cap_free(cap) == -1)
		{
			DEBUG("%s", "cap_free() failed, error=%d", errno);
		}
	}
	else
	{
		DEBUG("%s", "cap_get_proc() failed, error=%d", errno);
	}	

	syslog(LOG_INFO, "Leaving constructor ");
	DEBUG("%s", "Leaving ACS_HC_HAService::ACS_HC_HAService");
}//end of constructor


/*=================================================================
	ROUTINE: ACS_HC_HAService destructor
=================================================================== */
ACS_HC_HAService::~ACS_HC_HAService()
{
	DEBUG("%s", "Entering ACS_HC_HAService::~ACS_HC_HAService()");
	syslog(LOG_INFO, "Entering ACS_HC_HAService destrcutor ");
	this->shutdownApp();
	syslog(LOG_INFO, "Leaving ACS_HC_HAService destructor ");
	DEBUG("%s", "Leaving ACS_HC_HAService::~ACS_HC_HAService()");
}//end of destructor


/*================================================================================ 
	ROUTINE: performStateTransitionToActiveJobs
 ================================================================================ */
ACS_APGCC_ReturnType ACS_HC_HAService::performStateTransitionToActiveJobs(
					ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG( "%s", "Entering ACS_HC_HAService::performStateTransitionToActiveJobs");
	syslog(LOG_INFO, "Received performStateTransitionToActiveJobs");
	(void) previousHAState;
	return this->activateApp();
	//syslog(LOG_INFO, "Leaving performStateTransitionToActiveJobs ");
	//DEBUG("%s", "Leaving ACS_HC_HAService::performStateTransitionToActiveJobs");

}//end of performStateTransitionToActiveJobs

/*=================================================================================== 
	ROUTINE: performStateTransitionToPassiveJobs
 ================================================================================== */
ACS_APGCC_ReturnType ACS_HC_HAService::performStateTransitionToPassiveJobs(
				ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s", "Entering ACS_HC_HAService::performStateTransitionToPassiveJobs");
	syslog(LOG_INFO, "Received performStateTransitionToPassiveJobs");
	(void) previousHAState;
	return this->shutdownApp();
	//syslog(LOG_INFO, "performStateTransitionToPassiveJobs");
	//DEBUG("%s", "Leaving ACS_HC_HAService::performStateTransitionToPassiveJobs");
}//end of performStateTransitionToPassiveJobs

/*============================================================================================ 
	ROUTINE: performStateTransitionToQueisingJobs
 ============================================================================================ */
ACS_APGCC_ReturnType ACS_HC_HAService::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s", "Entering ACS_HC_HAService::performStateTransitionToQueisingJobs");
	syslog(LOG_INFO, "Received performStateTransitionToQueisingJobs");
	(void) previousHAState;
	ACS_APGCC_ReturnType retCode = this->shutdownApp();
	DEBUG("%s", "Leaving ACS_HC_HAService::performStateTransitionToQueisingJobs");
	return retCode;
}

/*============================================================================================= 
	ROUTINE: performStateTransitionToQuiescedJobs
 ============================================================================================= */
ACS_APGCC_ReturnType ACS_HC_HAService::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s", "Entering ACS_HC_HAService::performStateTransitionToQueiscedJobs");
	syslog(LOG_INFO, "Received performStateTransitionToQuiescedJobs");
	(void) previousHAState;
	ACS_APGCC_ReturnType retCode = this->shutdownApp();
	DEBUG("%s", "Leaving ACS_HC_HAService::performStateTransitionToQueiscedJobs");
	return retCode;

}//end of performStateTransitionToQueisingJobs

/*==================================================================================== 
	ROUTINE: performComponentHealthCheck
 =================================================================================== */
ACS_APGCC_ReturnType ACS_HC_HAService::performComponentHealthCheck(void)
{
	//DEBUG("%s", "Health Check Success.");
	DEBUG("%s", "Entering ACS_HC_HAService::performComponentHealthCheck");
	syslog(LOG_INFO, "Received performComponentHealthCheck");
	DEBUG("%s", "Leaving ACS_HC_HAService::performComponentHealthCheck");
	return ACS_APGCC_SUCCESS;	
}//end of performComponentHealthCheck

/*===================================================================== 
	ROUTINE: performComponentTerminateJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_HC_HAService::performComponentTerminateJobs(void)
{
	DEBUG("%s", "Entering ACS_HC_HAService::performComponentTerminateJobs");
	syslog(LOG_INFO, "Received performComponentTerminateJobs");
	ACS_APGCC_ReturnType retCode = this->shutdownApp();
	DEBUG("%s", "Leaving ACS_HC_HAService::performComponentTerminateJobs");
	return retCode;
}//end of performComponentTerminateJobs

/*=======================================================================
	ROUTINE: performComponentRemoveJobs
 ======================================================================= */
ACS_APGCC_ReturnType ACS_HC_HAService::performComponentRemoveJobs(void)
{
	DEBUG("%s", "Entering ACS_HC_HAService::performComponentRemoveJobs");
	syslog(LOG_INFO, "Received performComponentRemoveJobs");
	ACS_APGCC_ReturnType retCode = this->shutdownApp();
	DEBUG("%s", "Leaving ACS_HC_HAService::performComponentRemoveJobs");
	return retCode;
}//end of performComponentRemoveJobs

/*====================================================================== 
	ROUTINE: performApplicationShutdownJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_HC_HAService::performApplicationShutdownJobs()
{
	DEBUG("%s", "Entering ACS_HC_HAService::performApplicationShutdownJobs");
	syslog(LOG_INFO, "Received performApplicationShutdownJobs");
	ACS_APGCC_ReturnType retCode = this->shutdownApp();
	DEBUG("%s", "Leaving ACS_HC_HAService::performApplicationShutdownJobs");
	return retCode;

}//end of performApplicationShutdownJobs

/*===================================================================
   ROUTINE: activateApp
================================================================= */
ACS_APGCC_ReturnType ACS_HC_HAService::activateApp()
{
	syslog(LOG_INFO, "Entering activateApp");
	DEBUG("%s","Entering ACS_HC_HAService::activateApp()");
        servicePtr = new (std::nothrow) AcsHcs::HealthCheckService();
        if( servicePtr != 0 )
	{
		syslog(LOG_INFO, "calling hc thread");
		servicePtr->setupHCThread(servicePtr);
	}
	else
	{
		syslog(LOG_INFO, "failed ptr");
		return ACS_APGCC_FAILURE;
	}
        DEBUG("%s","Exiting ACS_HC_HAService::activateApp()");
        return ACS_APGCC_SUCCESS;
}/* End of activateApp */

ACS_APGCC_ReturnType ACS_HC_HAService::passifyApp()
{
        DEBUG("%s","Entering ACS_HC_HAService::passifyApp");
        DEBUG("%s","Leaving ACS_HC_HAService::passifyApp");
        //If the service needs to perform any functional activities on the passive node
        //in future, please add the neccessary code below.
        return ACS_APGCC_SUCCESS;
}


/*========================================================================
        ROUTINE: shutdown
========================================================================*/
ACS_APGCC_ReturnType ACS_HC_HAService::shutdownApp()
{
        DEBUG("%s","Entering ACS_HC_HAService::shutdownApp");
	syslog(LOG_INFO,"Entering ACS_HC_HAService::shutdownApp");
	AcsHcs::HealthCheckService::stopRequested = true;
	int waitGrpRes = 0;

	if( servicePtr != 0 )
	{
		if (servicePtr->m_threadGroupId != 0)
		{
			syslog(LOG_INFO,"Waiting for child threads to be closed first!");
			DEBUG("%s","Waiting for child threads to be closed first");
			waitGrpRes = ACE_Thread_Manager::instance()->wait_grp(servicePtr->m_threadGroupId);		// TR HX65511
			if(!waitGrpRes)
				syslog(LOG_INFO,"Child threads are now terminated!");
			else
				syslog(LOG_INFO,"Child threads are NOT termianted! wait_grp() returned -1!");
		}
		ACE_OS::sleep(1);
		delete servicePtr;
		servicePtr = 0;

		if(AcsHcs::HealthCheckService::isShutdownOngoing)	// TR HX65511
		{
			DEBUG("%s","isShutdownOngoing is TRUE... Child threads are still not joined! Sleep for 1 second.");
			ACE_OS::sleep(1);
		}
		else
			DEBUG("%s","isShutdownOngoing is FALSE... Child threads are terminated!");
	}
	DEBUG("%s","Leaving ACS_HC_HAService::shutdownApp");
        return ACS_APGCC_SUCCESS;
}
