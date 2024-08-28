//******************************************************************************
//
//  NAME
//     acs_hcs_haservice.h
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
//     2013-04-29 by XLANSRI
//
//  SEE ALSO
//     -
//
//******************************************************************************

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#ifndef ACS_HC_HAService_H_
#define ACS_HC_HAService_H_

#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_AmfTypes.h>
#include <ACS_APGCC_ApplicationManager.h>
#include "acs_hcs_healthcheckservice.h"
using namespace AcsHcs;

class HealthCheckService;

/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class ACS_HC_HAService : public ACS_APGCC_ApplicationManager
{
private:

        /**
         * @brief       activateApp
         * @return  ACS_APGCC_ReturnType
         */

        ACS_APGCC_ReturnType activateApp();

        /**
         * @brief       passifyApp
         * @return  ACS_APGCC_ReturnType
         */

        ACS_APGCC_ReturnType passifyApp();

        /**
         * @brief       shutdownApp
         * @return  ACS_APGCC_ReturnType
         */

	void printUsage();

        ACS_APGCC_ReturnType shutdownApp();

       	AcsHcs::HealthCheckService* servicePtr;

public:

	/**
		@brief		ACS_HC_HAService

		@param		daemon_name : const char pointer

		@param		username : const char pointer

	 */

	ACS_HC_HAService(const char* daemon_name, const char* username);

	/**
		@brief		~ACS_HC_HAService

	 */

	~ACS_HC_HAService();

	/**
		@brief		performStateTransitionToActiveJobs

		@param		previousHAState : ACS_APGCC_AMF_HA_StateT

		@return		ACS_APGCC_ReturnType

		@exception	none
	 */

	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**
		@brief		performStateTransitionToPassiveJobs

		@param		previousHAState : ACS_APGCC_AMF_HA_StateT

		@return 	      ACS_APGCC_ReturnType

		@exception	none
	 */

	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**
		@brief		performStateTransitionToQueisingJobs

		@param		previousHAState : ACS_APGCC_AMF_HA_StateT

		@return 	      ACS_APGCC_ReturnType

		@exception	none
	 */

	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**
		@brief		performStateTransitionToQueiscedJobs

		@param		previousHAState : ACS_APGCC_AMF_HA_StateT

		@return		ACS_APGCC_ReturnType

		@exception	none

	 */
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**

		@brief		performComponentHealthCheck

		@return		ACS_APGCC_ReturnType

		@exception	none

	 */
	ACS_APGCC_ReturnType performComponentHealthCheck(void);

	/**
		@brief		performComponentTerminateJobs

		@return 	      ACS_APGCC_ReturnType

		@exception	none
	 */
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);

	/**
		@brief		performComponentRemoveJobs

		@return		ACS_APGCC_ReturnType

		@exception	none

	 */
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);

	/**
		@brief		performApplicationShutdownJobs

		@return		ACS_APGCC_ReturnType

		@exception	none
	 */
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

};
#endif /* ACS_HC_HAService_H_ */
