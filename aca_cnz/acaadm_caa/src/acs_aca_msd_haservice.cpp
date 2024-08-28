//******************************************************************************
//
//  NAME
//     acs_aca_msd_haservice.cpp
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
//     2012-10-08 by XLANSRI
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <poll.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>

#include "acs_aca_common.h"
#include "acs_aca_logger.h"
#include "acs_aca_msd_haservice.h"

/*=================================================================
        ROUTINE: printUsage
=================================================================*/

void ACS_MSD_Service::printUsage () {
	std::cout<<"Usage: acs_aca_server -d" <<std::endl;
}
/*=================================================================
	ROUTINE: ACS_ACA_MSD_HAService constructor
=================================================================== */
ACS_ACA_MSD_HAService::ACS_ACA_MSD_HAService (const char * daemon_name, const char * username)
: ACS_APGCC_ApplicationManager(daemon_name, username),
  _root_worker_thread(0) {
	syslog(LOG_INFO, "Entering ACS_ACA_MSD_HAService class constructor");
	syslog(LOG_INFO, "Leaving ACS_ACA_MSD_HAService class constructor ");
}//end of constructor


/*=================================================================
	ROUTINE: ACS_ACA_MSD_HAService destructor
=================================================================== */
ACS_ACA_MSD_HAService::~ACS_ACA_MSD_HAService () {
	syslog(LOG_INFO, "Entering ACS_ACA_MSD_HAService class destructor");
	passifyApp();
	syslog(LOG_INFO, "Leaving ACS_ACA_MSD_HAService class destructor");
}//end of destructor


/*================================================================================ 
	ROUTINE: performStateTransitionToActiveJobs
 ================================================================================ */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performStateTransitionToActiveJobs (ACS_APGCC_AMF_HA_StateT /*previousHAState*/) {
	syslog(LOG_INFO, "performStateTransitionToActiveJobs invoked!");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "performStateTransitionToActiveJobs invoked!");

	return activateApp();
}//end of performStateTransitionToActiveJobs

/*=================================================================================== 
	ROUTINE: performStateTransitionToPassiveJobs
 ================================================================================== */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performStateTransitionToPassiveJobs (ACS_APGCC_AMF_HA_StateT /*previousHAState*/) {
	syslog(LOG_INFO, "performStateTransitionToPassiveJobs invoked!");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "performStateTransitionToPassiveJobs invoked!");

	return passifyApp();
}//end of performStateTransitionToPassiveJobs

/*============================================================================================ 
	ROUTINE: performStateTransitionToQueisingJobs
 ============================================================================================ */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performStateTransitionToQueisingJobs (ACS_APGCC_AMF_HA_StateT /*previousHAState*/) {
	syslog(LOG_INFO, "performStateTransitionToQueisingJobs invoked!");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "performStateTransitionToQueisingJobs invoked!");

	return passifyApp();
}

/*============================================================================================= 
	ROUTINE: performStateTransitionToQuiescedJobs
 ============================================================================================= */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT /*previousHAState*/) {
	syslog(LOG_INFO, "performStateTransitionToQuiescedJobs invoked!");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "performStateTransitionToQuiescedJobs invoked!");

	return passifyApp();
}//end of performStateTransitionToQueisingJobs

/*==================================================================================== 
	ROUTINE: performComponentHealthCheck
 =================================================================================== */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performComponentHealthCheck () {
	syslog(LOG_INFO, "performComponentHealthCheck invoked!");
	return ACS_APGCC_SUCCESS;	
}//end of performComponentHealthCheck

/*===================================================================== 
	ROUTINE: performComponentTerminateJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performComponentTerminateJobs () {
	syslog(LOG_INFO, "performComponentTerminateJobs invoked!");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "performComponentTerminateJobs invoked!");

	return passifyApp();
}//end of performComponentTerminateJobs

/*=======================================================================
	ROUTINE: performComponentRemoveJobs
 ======================================================================= */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performComponentRemoveJobs () {
	syslog(LOG_INFO, "performComponentRemoveJobs invoked!");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "performComponentRemoveJobs invoked!");

	return passifyApp();
}//end of performComponentRemoveJobs

/*====================================================================== 
	ROUTINE: performApplicationShutdownJobs
 ===================================================================== */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::performApplicationShutdownJobs () {
	syslog(LOG_INFO, "performApplicationShutdownJobs invoked!");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "performApplicationShutdownJobs invoked!");

	return passifyApp();
}//end of performApplicationShutdownJobs

/*===================================================================
   ROUTINE: activateApp
================================================================= */
ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::activateApp () {
	syslog(LOG_INFO, "Entering ACS_ACA_MSD_HAService::activateApp");
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if (_root_worker_thread) { // Application already active
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Application is already active");
		rCode = ACS_APGCC_SUCCESS;
	}
	else {
		_root_worker_thread = new (std::nothrow) ACS_MSD_Service();

		if (_root_worker_thread) { // OK: instance created
			if (ACS_MSD_Service::setupACAThread(_root_worker_thread) < 0) { // ERROR: Failed to start ACA thread
				delete _root_worker_thread; _root_worker_thread = 0;
			}
			else { // OK: Application activated by HA
				ACS_ACA_LOG(LOG_LEVEL_INFO, "Application is now activated by HA");
				rCode = ACS_APGCC_SUCCESS;
			}
		}
		else { // ERROR: Memory allocation problems
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the ACS_MSD_Service instance");
		}
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return rCode;
}/* End of activateApp */

ACS_APGCC_ReturnType ACS_ACA_MSD_HAService::passifyApp () {
	syslog(LOG_INFO, "Entering ACS_ACA_MSD_HAService::passifyApp");
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (_root_worker_thread) { // Instance created: stop and destroy ACA Server
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Ordering ACA Application to passify");
		_root_worker_thread->stop();

		delete _root_worker_thread; _root_worker_thread = 0;
	}
	else { // APplication already passive
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Application is already passive");
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ACS_APGCC_SUCCESS;
}
