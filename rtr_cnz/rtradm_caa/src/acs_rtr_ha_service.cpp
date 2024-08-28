/*=================================================================== */
   /**
   @file acs_rtr_ha_service.cpp

   Class method implementation for ACS module.

   This module contains the implementation of class declared in
   the acs_rtr_ha_service.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/12/2012   XSAMECH   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_rtr_ha_service.h"
#include "acs_rtr_server.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"
#include <syslog.h>

ACS_RTR_TRACE_DEFINE(ACS_RTR_HA_Service);

/*===================================================================
   ROUTINE: ACS_RTR_HA_Service
=================================================================== */
ACS_RTR_HA_Service::ACS_RTR_HA_Service(const char* daemon_name, const char* username)
: ACS_APGCC_ApplicationManager(daemon_name, username),
  rtrAdmPtr(NULL)
{

}//End of Constructor

/*===================================================================
   ROUTINE: ~ACS_RTR_HA_Service
=================================================================== */
ACS_RTR_HA_Service::~ACS_RTR_HA_Service()
{

}//End of Destructor


/*===================================================================
   ROUTINE: performStateTransitionToActiveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT /*previousHAState*/)
{
	syslog(LOG_INFO, "performStateTransitionToActiveJobs invoked!");
	ACS_RTR_LOG(LOG_LEVEL_INFO, "performStateTransitionToActiveJobs invoked!");

	return goActive();

}//End of performStateTransitionToActiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToPassiveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT /*previousHAState*/)
{
	syslog(LOG_INFO, "performStateTransitionToPassiveJobs invoked!");
	ACS_RTR_LOG(LOG_LEVEL_INFO, "performStateTransitionToPassiveJobs invoked!");

	return goPassive();

}//End of performStateTransitionToPassiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToQueisingJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT /*previousHAState*/)
{
	syslog(LOG_INFO, "performStateTransitionToQueisingJobs invoked!");
	ACS_RTR_LOG(LOG_LEVEL_INFO, "performStateTransitionToQueisingJobs invoked!");

	return shutdown();

}//End of performStateTransitionToQueisingJobs

/*===================================================================
   ROUTINE: performStateTransitionToQuiescedJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT /*previousHAState*/)
{
	syslog(LOG_INFO, "performStateTransitionToQuiescedJobs invoked!");
	ACS_RTR_LOG(LOG_LEVEL_INFO, "performStateTransitionToQuiescedJobs invoked!");

	return shutdown();
}//End of performStateTransitionToQuiescedJobs

/*===================================================================
   ROUTINE: performComponentHealthCheck
=================================================================== */
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performComponentHealthCheck(void)
{
	ACS_APGCC_ReturnType result = ACS_APGCC_SUCCESS;
	if(NULL != rtrAdmPtr )
	{
		if( rtrAdmPtr->thr_count() == 0)
		{
			syslog(LOG_ERR, "Error acs_rtrd: No RTR server thread running!!");
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "No RTR server thread running!!");

			result = ACS_APGCC_FAILURE;
		}
	}

	return result;
}

/*===================================================================
   ROUTINE: performComponentTerminateJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performComponentTerminateJobs(void)
{
	syslog(LOG_INFO, "performComponentTerminateJobs invoked!");
	ACS_RTR_LOG(LOG_LEVEL_INFO, "performComponentTerminateJobs invoked!");

	return shutdown();
}//End of performComponentTerminateJobs

/*===================================================================
   ROUTINE: performComponentRemoveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performComponentRemoveJobs(void)
{
	syslog(LOG_INFO, "performComponentRemoveJobs invoked!");
	ACS_RTR_LOG(LOG_LEVEL_INFO, "performComponentRemoveJobs invoked!");

	return shutdown();
}

/*========================================================================
	ROUTINE: performApplicationShutdownJobs
========================================================================*/
ACS_APGCC_ReturnType ACS_RTR_HA_Service::performApplicationShutdownJobs()
{
	syslog(LOG_INFO, "performApplicationShutdownJobs invoked!");
	ACS_RTR_LOG(LOG_LEVEL_INFO, "performApplicationShutdownJobs invoked!");

	return shutdown();
}


/*========================================================================
	ROUTINE: goActive
========================================================================*/
ACS_APGCC_ReturnType ACS_RTR_HA_Service::goActive()
{
	ACS_RTR_TRACE_FUNCTION;
	syslog(LOG_INFO, "worker thread starting...");
	if(rtrAdmPtr == 0)
	{
		rtrAdmPtr = new (std::nothrow) ACS_RTR_Server();

		if( rtrAdmPtr != 0 )
		{
			if( rtrAdmPtr->open() < 0)
			{
				delete rtrAdmPtr;
				rtrAdmPtr = NULL;
				return ACS_APGCC_FAILURE;
			}
			syslog(LOG_INFO, "...worker thread started");
		}
		else
		{
			return ACS_APGCC_FAILURE;
		}
	}
	else
	{
		syslog(LOG_INFO, "worker thread already started");
	}

	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: goPassive
========================================================================*/
ACS_APGCC_ReturnType ACS_RTR_HA_Service::goPassive()
{
	ACS_RTR_TRACE_FUNCTION;

	//If the service needs to perform any functional activities on the passive node 
	//in future, please add the necessary code below.
	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: shutdown
========================================================================*/
ACS_APGCC_ReturnType ACS_RTR_HA_Service::shutdown()
{
	ACS_RTR_TRACE_FUNCTION;
	syslog(LOG_INFO, "RTR server shutdown started...");
	if(NULL != rtrAdmPtr )
	{
		ACS_RTR_TRACE_MESSAGE("shutdown!!");
		rtrAdmPtr->stop();
		ACS_RTR_TRACE_MESSAGE("wait on Server stop!!");
		syslog(LOG_INFO, "wait on worker thread termination...");
		rtrAdmPtr->wait();

		syslog(LOG_INFO, "...worker thread terminated");


		delete rtrAdmPtr;
		rtrAdmPtr = 0;

	}
	syslog(LOG_INFO, "...RTR server shutdown done");

	return ACS_APGCC_SUCCESS;
}
