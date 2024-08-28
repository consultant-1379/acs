//******************************************************************************
//
//  NAME
//     acs_lm_haservice.cpp
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
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <poll.h>
#include "acs_lm_haservice.h"
#include "acs_lm_common.h"
#include "acs_lm_server.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

//ACS_LM_Server* theLMServerPtr = 0;
/*=================================================================
	ROUTINE: ACS_LM_HAService constructor
=================================================================== */
ACS_LM_HAService::ACS_LM_HAService( const char* daemon_name,
				const char* username):
				ACS_APGCC_ApplicationManager(daemon_name, username)
{
	INFO("%s", "Entering ACS_LM_HAService::ACS_LM_HAService");
	m_lmApplObj=0;
	INFO("%s", "Leaving ACS_LM_HAService::ACS_LM_HAService");
}//end of constructor


/*=================================================================
	ROUTINE: ACS_LM_HAService destructor
=================================================================== */
ACS_LM_HAService::~ACS_LM_HAService()
{

	INFO("%s", "Entering ACS_LM_HAService::~ACS_LM_HAService()");
	this->passifyApp();
	INFO("%s", "Leaving ACS_LM_HAService::~ACS_LM_HAService()");
}//end of destructor


/*================================================================================ 
	ROUTINE: performStateTransitionToActiveJobs
 ================================================================================ */
ACS_APGCC_ReturnType ACS_LM_HAService::performStateTransitionToActiveJobs(
					ACS_APGCC_AMF_HA_StateT previousHAState)
{
	INFO( "%s", "Entering ACS_LM_HAService::performStateTransitionToActiveJobs");
	(void) previousHAState;
	INFO("%s", "Leaving ACS_LM_HAService::performStateTransitionToActiveJobs");
	return this->activateApp();
}//end of performStateTransitionToActiveJobs

/*=================================================================================== 
	ROUTINE: performStateTransitionToPassiveJobs
 ================================================================================== */
ACS_APGCC_ReturnType ACS_LM_HAService::performStateTransitionToPassiveJobs(
				ACS_APGCC_AMF_HA_StateT previousHAState)
{
	INFO("%s", "Entering ACS_LM_HAService::performStateTransitionToPassiveJobs");
	(void) previousHAState;
	INFO("%s", "Leaving ACS_LM_HAService::performStateTransitionToPassiveJobs");
	return this->passifyApp();
}//end of performStateTransitionToPassiveJobs

/*============================================================================================ 
	ROUTINE: performStateTransitionToQueisingJobs
 ============================================================================================ */
ACS_APGCC_ReturnType ACS_LM_HAService::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	INFO("%s", "Entering ACS_LM_HAService::performStateTransitionToQueisingJobs");
	(void) previousHAState;
	INFO("%s", "Leaving ACS_LM_HAService::performStateTransitionToQueisingJobs");
	return this->passifyApp();
}//end of lm_service_run

/*============================================================================================= 
	ROUTINE: performStateTransitionToQuiescedJobs
 ============================================================================================= */
ACS_APGCC_ReturnType ACS_LM_HAService::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s", "Entering ACS_LM_HAService::performStateTransitionToQueiscedJobs");
	(void)previousHAState;
	DEBUG("%s", "Leaving ACS_LM_HAService::performStateTransitionToQueiscedJobs");
	return this->passifyApp();
}//end of performStateTransitionToQuiescedJobs 

/*==================================================================================== 
	ROUTINE: performComponentHealthCheck
 =================================================================================== */
ACS_APGCC_ReturnType ACS_LM_HAService::performComponentHealthCheck(void)
{
	return ACS_APGCC_SUCCESS;	
}//end of performComponentHealthCheck


/*===================================================================== 
	ROUTINE: performComponentTerminateJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_LM_HAService::performComponentTerminateJobs(void)
{
	INFO("%s", "Entering ACS_LM_HAService::performComponentTerminateJobs");
	return this->passifyApp();
	INFO("%s", "Leaving ACS_LM_HAService::performComponentTerminateJobs");
}//end of performComponentTerminateJobs

/*=======================================================================
	ROUTINE: performComponentRemoveJobs
 ======================================================================= */
ACS_APGCC_ReturnType ACS_LM_HAService::performComponentRemoveJobs(void)
{
	INFO("%s", "Entering ACS_LM_HAService::performComponentRemoveJobs");
	return this->passifyApp();
	INFO("%s", "Leaving ACS_LM_HAService::performComponentRemoveJobs");

}//end of performComponentRemoveJobs

/*====================================================================== 
	ROUTINE: performApplicationShutdownJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_LM_HAService::performApplicationShutdownJobs() 
{
	INFO("%s", "Entering ACS_LM_HAService::performApplicationShutdownJobs");
	return this->passifyApp();
	INFO("%s", "Leaving ACS_LM_HAService::performApplicationShutdownJobs");
}//end of performApplicationShutdownJobs


ACS_APGCC_ReturnType ACS_LM_HAService::activateApp()
{

	DEBUG("Entering %s","ACS_LM_HAService::activateApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if ( 0 != this->m_lmApplObj)
	{
		INFO("ACS_LM_HAService::activateApp() %s","application is already active");
		rCode = ACS_APGCC_SUCCESS;
	}
	else
	{
		ACE_NEW_NORETURN(this->m_lmApplObj, ACS_LM_Server());
		if (0 == this->m_lmApplObj)
		{
			ERROR("ACS_LM_HAService::activateApp() %s","failed to create the lm instance");
		}
		else
		{
			this->m_lmApplObj->activate();
			INFO("ACS_LM_HAService::activateApp() %s","ha-class: application is now activated by HA");
			rCode = ACS_APGCC_SUCCESS;
		}
	}
	DEBUG("Leaving %s"," ACS_LM_HAService::activateApp()");
	return rCode;
}



ACS_APGCC_ReturnType ACS_LM_HAService::passifyApp() {

	DEBUG("Entering %s","ACS_LM_HAService::passifyApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if (0 == this->m_lmApplObj)
	{
		INFO("ACS_LM_HAService::passifyApp() %s","application is already passive");
		rCode = ACS_APGCC_SUCCESS;
	}
	else
	{
		INFO("ACS_LM_HAService::passifyApp() %s","Ordering lm Application to passify");
		//this->m_lmApplObj->stop();

		ACE_Message_Block * myMessageBlock = new ACE_Message_Block(0,ACE_Message_Block::MB_HANGUP) ;
		this->m_lmApplObj->putq(myMessageBlock);
		
		this->m_lmApplObj->setShudownSignalFlag(true);

        INFO("ACS_LM_HAService::passifyApp() isShutdownSignalled flag value is : %d",this->m_lmApplObj->getShudownSignalFlag());

		INFO("ACS_LM_HAService::passifyApp() %s","Waiting for lm Application thread to go down");
		this->m_lmApplObj->wait();

		INFO("ACS_LM_HAService::passifyApp() %s","Deleting lm App instance");
		delete this->m_lmApplObj;

		this->m_lmApplObj=0;

		INFO("ACS_LM_HAService::passifyApp() %s","App is now passivated by HA!!");

		rCode = ACS_APGCC_SUCCESS;
	}
	DEBUG("Leaving %s","ACS_LM_HAService::passifyApp()");
	return rCode;
}
		
					




		



