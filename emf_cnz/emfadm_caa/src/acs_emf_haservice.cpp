/*=================================================================== */
   /**
   @file acs_emf_haservice.cpp

   Class method implementation for EMF module.

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
   N/A       06/04/2011   XRAMMAT  Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_emf_haservice.h"
#include "acs_emf_tra.h"

/*=====================================================================
                        GLOBAL VARIABLE
==================================================================== */
//ACE_THR_FUNC_RETURN svc_run(void *);
/*=================================================================== */
/**
   @brief  theEMFServiceModulePtr

 */
/*=================================================================== */
//EMF_SERVICE_MODULE *theEMFServiceModulePtr = NULL;
/*===================================================================
   ROUTINE: ACS_EMF_HAService
=================================================================== */
ACS_EMF_HAService::ACS_EMF_HAService(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	DEBUG(1, "%s", "ACS_EMF_HAService::ACS_EMF_HAService - Entering");
	passiveToActive=0;
	theEMFServiceModulePtr = 0;
	DEBUG(1, "%s", "ACS_EMF_HAService::ACS_EMF_HAService - Leaving");
}//End of Constructor


/*===================================================================
   ROUTINE: Destructor 
=================================================================== */
ACS_EMF_HAService::~ACS_EMF_HAService()
{
	DEBUG(1, "%s", "ACS_EMF_HAService::~ACS_EMF_HAService - Entering");
	//To be sure
	this->shutdownApp();
	DEBUG(1, "%s", "ACS_EMF_HAService::~ACS_EMF_HAService - Leaving");
}


/*===================================================================
   ROUTINE: performStateTransitionToActiveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToActiveJobs - Entering");
	(void)previousHAState;
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToActiveJobs - Leaving");
	return this->activateApp();
}//End of performStateTransitionToActiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToPassiveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToPassiveJobs - Entering");
	(void)previousHAState;
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToPassiveJobs - Leaving");
	return this->passifyApp();

}

/*===================================================================
   ROUTINE: performStateTransitionToQueisingJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToQueisingJobs - Entering");
	(void)previousHAState;
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToQueisingJobs - Leaving");
	return this->shutdownApp();
}//End of performStateTransitionToQueisingJobs

/*===================================================================
   ROUTINE: performStateTransitionToQuiescedJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToQuiescedJobs - Entering");
	(void)previousHAState;
	DEBUG(1, "%s", "ACS_EMF_HAService::performStateTransitionToQuiescedJobs - Leaving");

	return this->shutdownApp();
}//End of performStateTransitionToQuiescedJobs
/*===================================================================
   ROUTINE: performComponentHealthCheck
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performComponentHealthCheck(void)
{
	DEBUG(0, "%s", "ACS_EMF_HAService::performComponentHealthCheck - Entering");
	DEBUG(0, "%s", "ACS_EMF_HAService::performComponentHealthCheck - Leaving");
	return ACS_APGCC_SUCCESS;
}//End of performComponentHealthCheck

/*===================================================================
   ROUTINE: performComponentTerminateJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performComponentTerminateJobs(void)
{
	DEBUG(1,"%s", "ACS_EMF_HAService::performComponentTerminateJobs - Entering");
	/* Application has received terminate component callback due to 
	 * LOCK-INST admin opreration perform on SU. Terminate the thread if
	 * we have not terminated in performComponentRemoveJobs case or double 
	 * check if we are done so.
	 */
	DEBUG(1,"%s", "ACS_EMF_HAService::performComponentTerminateJobs - Leaving");
	return this->shutdownApp();
	//return ACS_APGCC_SUCCESS;
}//End of performComponentTerminateJobs

/*===================================================================
   ROUTINE: performComponentRemoveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performComponentRemoveJobs(void)
{
	DEBUG(1,"%s", "ACS_EMF_HAService::performComponentRemoveJobs - Entering");
	DEBUG(1,"%s", "ACS_EMF_HAService::performComponentRemoveJobs - Leaving");
	return this->shutdownApp();
	/* Application has received Removal callback. State of the application 
	 * is neither Active nor Standby. This is with the result of LOCK admin operation
	 * performed on our SU. Terminate the thread by informing the thread to go "stop" state. 
	 */
}//End of performComponentRemoveJobs

/*===================================================================
   ROUTINE: performApplicationShutdownJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::performApplicationShutdownJobs()
{
	DEBUG(1,"%s", "ACS_EMF_HAService::performApplicationShutdownJobs - Entering");
	DEBUG(1,"%s", "ACS_EMF_HAService::performApplicationShutdownJobs - Leaving");
	return this->shutdownApp();
}//End of performApplicationShutdownJobs


/*===================================================================
   ROUTINE: activateApp 
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::activateApp()
{

	DEBUG(1,"%s","ACS_EMF_HAService::activateApp - Entering");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
	if ( 0 != this->theEMFServiceModulePtr)
	{
		if (passiveToActive)
		{
			DEBUG(1,"%s","ACS_EMF_HAService::activateApp - transition. stop passive work before becoming active");
			this->theEMFServiceModulePtr->stop();
			this->theEMFServiceModulePtr->wait();
			passiveToActive=0;
		}
		else
		{
			DEBUG(1,"%s","ACS_EMF_HAService::activateApp - application is already active");
			rCode = ACS_APGCC_SUCCESS;
		}
	}
	else
	{
		ACE_NEW_NORETURN(this->theEMFServiceModulePtr, EMF_SERVICE_MODULE());
		if (0 == this->theEMFServiceModulePtr)
		{
			DEBUG(1,"%s","ACS_EMF_HAService::activateApp - failed to create EMF_SERVICE_MODULE instance");
		}
	}

	if ( 0 != this->theEMFServiceModulePtr)
	{
		int res = this->theEMFServiceModulePtr->active(this);
		if (res < 0)
		{
			// Failed to start
			delete this->theEMFServiceModulePtr;
			this->theEMFServiceModulePtr = 0;
		}
		else
		{
			DEBUG(1,"%s","ACS_EMF_HAService::activateApp - application is now activated by HA");
			rCode = ACS_APGCC_SUCCESS;
		}
	}

	DEBUG(1,"%s","ACS_EMF_HAService::activateApp - Leaving");
	return rCode;
}

/*===================================================================
   ROUTINE: shutdownApp 
=================================================================== */
ACS_APGCC_ReturnType ACS_EMF_HAService::shutdownApp()
{
	DEBUG(1,"%s","ACS_EMF_HAService::shutdownApp - Entering");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if (0 == this->theEMFServiceModulePtr)
	{
		DEBUG(1,"%s","ACS_EMF_HAService::shutdownApp - Shutting down is already Done!!");
		rCode = ACS_APGCC_SUCCESS;
	}
	else
	{
		//Passive functionality
		DEBUG(1,"%s","ACS_EMF_HAService::shutdownApp - Ordering application to shutdown...");
		this->theEMFServiceModulePtr->stop();
		DEBUG(1,"%s","ACS_EMF_HAService::shutdownApp - Waiting for application to complete shutdown...");
		this->theEMFServiceModulePtr->wait();
		DEBUG(1,"%s","ACS_EMF_HAService::shutdownApp - Deleting EMF App instance...");
		delete this->theEMFServiceModulePtr;
		this->theEMFServiceModulePtr = 0;
		rCode = ACS_APGCC_SUCCESS;
	}
	DEBUG(1,"%s","ACS_EMF_HAService::shutdownApp - Leaving");
	return rCode;
}

ACS_APGCC_ReturnType ACS_EMF_HAService::passifyApp()
{
	DEBUG(1,"%s","ACS_EMF_HAService::passifyApp - Entering");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
	passiveToActive=1;

	if (0 != this->theEMFServiceModulePtr)
	{
		DEBUG(1,"%s","ACS_EMF_HAService::passifyApp - application is already passive");
		rCode = ACS_APGCC_SUCCESS;
	}
	else
	{
		ACE_NEW_NORETURN(this->theEMFServiceModulePtr, EMF_SERVICE_MODULE());
		if (0 == this->theEMFServiceModulePtr)
		{
			DEBUG(1,"%s","ACS_EMF_HAService::passifyApp - Failed to create ssu application instance.");
		}
		else
		{
			int res = this->theEMFServiceModulePtr->passive(this); // This will start passive functionality and will not return until myCLass is running
			if (res < 0)
			{
				// Failed to start
				delete this->theEMFServiceModulePtr;
				this->theEMFServiceModulePtr = 0;
			}
			else
			{
				DEBUG(1,"%s","ACS_EMF_HAService::passifyApp - App is now passivated by HA");
				rCode = ACS_APGCC_SUCCESS;
			}
		}
	}
	DEBUG(1,"%s","ACS_EMF_HAService::passifyApp - Leaving");
	return rCode;
}

