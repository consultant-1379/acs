//******************************************************************************
//
//  NAME
//     acs_logm_logmservice.cpp
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
//     2012-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <poll.h>
#include "acs_logm_logmservice.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

//ACS_LM_Server* theLMServerPtr = 0;
/*=================================================================
	ROUTINE: acs_logm_logmservice constructor
=================================================================== */
acs_logm_logmservice::acs_logm_logmservice( const char* daemon_name,
				const char* username):
				ACS_APGCC_ApplicationManager(daemon_name, username),
				thecleanupActionPtr(0)
{
	//INFO("%s", "Entering acs_logm_logmservice::acs_logm_logmservice");
	//INFO("%s", "Leaving acs_logm_logmservice::acs_logm_logmservice");
}//end of constructor


/*=================================================================
	ROUTINE: acs_logm_logmservice destructor
=================================================================== */
acs_logm_logmservice::~acs_logm_logmservice()
{

	INFO("%s", "Entering acs_logm_logmservice::~acs_logm_logmservice()");
	this->passifyApp();
	INFO("%s", "Leaving acs_logm_logmservice::~acs_logm_logmservice()");
}//end of destructor


/*================================================================================ 
	ROUTINE: performStateTransitionToActiveJobs
 ================================================================================ */
ACS_APGCC_ReturnType acs_logm_logmservice::performStateTransitionToActiveJobs(
					ACS_APGCC_AMF_HA_StateT previousHAState)
{
	INFO( "%s", "Entering acs_logm_logmservice::performStateTransitionToActiveJobs");
	(void) previousHAState;
	INFO("%s", "Leaving acs_logm_logmservice::performStateTransitionToActiveJobs");
	return this->activateApp();
}//end of performStateTransitionToActiveJobs

/*=================================================================================== 
	ROUTINE: performStateTransitionToPassiveJobs
 ================================================================================== */
ACS_APGCC_ReturnType acs_logm_logmservice::performStateTransitionToPassiveJobs(
				ACS_APGCC_AMF_HA_StateT previousHAState)
{
	INFO("%s", "Entering acs_logm_logmservice::performStateTransitionToPassiveJobs");
	(void) previousHAState;
	INFO("%s", "Leaving acs_logm_logmservice::performStateTransitionToPassiveJobs");
	return this->activateApp();
}//end of performStateTransitionToPassiveJobs

/*============================================================================================ 
	ROUTINE: performStateTransitionToQueisingJobs
 ============================================================================================ */
ACS_APGCC_ReturnType acs_logm_logmservice::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	INFO("%s", "Entering acs_logm_logmservice::performStateTransitionToQueisingJobs");

	(void) previousHAState;
	INFO("%s", "Leaving acs_logm_logmservice::performStateTransitionToQueisingJobs");
	return this->passifyApp();
}//end of lm_service_run

/*============================================================================================= 
	ROUTINE: performStateTransitionToQuiescedJobs
 ============================================================================================= */
ACS_APGCC_ReturnType acs_logm_logmservice::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	INFO("%s", "Entering acs_logm_logmservice::performStateTransitionToQueiscedJobs");
	(void)previousHAState;
	INFO("%s", "Leaving acs_logm_logmservice::performStateTransitionToQueiscedJobs");
	return this->passifyApp();
}///end of performStateTransitionToQuiescedJobs
/*====================================================================================
	ROUTINE: performComponentHealthCheck
 =================================================================================== */
ACS_APGCC_ReturnType acs_logm_logmservice::performComponentHealthCheck(void)
{
	//INFO("%s", "Entering acs_logm_logmservice::performComponentHealthCheck");
	return ACS_APGCC_SUCCESS;
}//end of performComponentHealthCheck

/*===================================================================== 
	ROUTINE: performComponentTerminateJobs
 ===================================================================== */
ACS_APGCC_ReturnType acs_logm_logmservice::performComponentTerminateJobs(void)
{
	INFO("%s", "Entering acs_logm_logmservice::performComponentTerminateJobs");
	INFO("%s", "Leaving acs_logm_logmservice::performComponentTerminateJobs");
//	return ACS_APGCC_SUCCESS;
	return this->passifyApp();
}//end of performComponentTerminateJobs

/*=======================================================================
	ROUTINE: performComponentRemoveJobs
 ======================================================================= */
ACS_APGCC_ReturnType acs_logm_logmservice::performComponentRemoveJobs(void)
{
	INFO("%s", "Entering acs_logm_logmservice::performComponentRemoveJobs");
	INFO("%s", "Leaving acs_logm_logmservice::performComponentRemoveJobs");
	return this->passifyApp();
}//end of performComponentRemoveJobs

/*====================================================================== 
	ROUTINE: performApplicationShutdownJobs
 ===================================================================== */
ACS_APGCC_ReturnType acs_logm_logmservice::performApplicationShutdownJobs()
{
	INFO("%s", "Entering acs_logm_logmservice::performApplicationShutdownJobs");
	INFO("%s", "Leaving acs_logm_logmservice::performApplicationShutdownJobs");
	return this->passifyApp();

}//end of performApplicationShutdownJobs

ACS_APGCC_ReturnType acs_logm_logmservice::activateApp()
{

	INFO("%s","Entering acs_logm_logmservice::activateApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if ( 0 != this->thecleanupActionPtr)
	{
		INFO("%s","application is already active");
		rCode = ACS_APGCC_SUCCESS;
    }
	else
	{
		ACE_NEW_NORETURN(this->thecleanupActionPtr, acs_logm_cleanupAction());
		if (0 == this->thecleanupActionPtr)
		{
			ERROR("%s","failed to create the acs_logm_cleanupAction instance");
		}
		else
		{
			this->thecleanupActionPtr->start();
			INFO("%s","LOGM Application is now activated by HA");
			rCode = ACS_APGCC_SUCCESS;
		}
	}
	INFO("%s","Exiting acs_logm_logmservice::activateApp()");
	return rCode;
}



ACS_APGCC_ReturnType acs_logm_logmservice::passifyApp()
{

	INFO("%s","Entering acs_logm_logmservice::passifyApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

        if (0 == this->thecleanupActionPtr)
        {
                INFO("%s","application is already passive");
                rCode = ACS_APGCC_SUCCESS;
        }
        else
        {
                INFO("%s","Ordering logm Application to passify");
                this->thecleanupActionPtr->stop();

                INFO("%s","Waiting for logm Application to become passive...");
                this->thecleanupActionPtr->wait();

                INFO("%s","Deleting logm App instance...");
                delete this->thecleanupActionPtr;
                this->thecleanupActionPtr=0;
                INFO("%s","App is now passivated by HA!!");
		rCode = ACS_APGCC_SUCCESS;
        }
        INFO("%s","Exiting acs_logm_logmservice::passifyApp()");
        return rCode;
}


		
					




		



