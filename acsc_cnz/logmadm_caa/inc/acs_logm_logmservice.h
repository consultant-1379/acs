/*=================================================================== */
   /**
   @file acs_logm_logmservice.h

   Header file for acs_logm_logmservice class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11-AUG-2012     XCSSATA   Initial Revision for APG43L.
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#ifndef acs_logm_logmservice_H_
#define acs_logm_logmservice_H_

#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_ApplicationManager.h>
#include "acs_logm_cleanupAction.h"
#include <unistd.h>

/**
 * @brief	logm_service_run
 *
 */
ACE_THR_FUNC_RETURN  logm_service_run(void *);

/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class acs_logm_logmservice : public ACS_APGCC_ApplicationManager
{
private:

	/**
	* @brief          theLOGMCleanupPtr
	*/
	acs_logm_cleanupAction	*thecleanupActionPtr;
	/**
	* @brief	activateApp
	* @return  ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType activateApp();
	/**
	* @brief	passifyApp
	* @return  ACS_APGCC_ReturnType
	*/
	ACS_APGCC_ReturnType passifyApp();

public:

	/**
		@brief		acs_logm_logmservice

		@param		daemon_name : const char pointer

		@param		username : const char pointer

	 */

	acs_logm_logmservice(const char* daemon_name, const char* username);

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

		@return 	ACS_APGCC_ReturnType

		@exception	none
	 */
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**
		@brief		performStateTransitionToQueisingJobs

		@param		previousHAState : ACS_APGCC_AMF_HA_StateT

		@return 	ACS_APGCC_ReturnType

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

		@return 	ACS_APGCC_ReturnType

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


	/**
		@brief		lm_svc

		@return		ACS_APGCC_ReturnType

		@exception	none
	 */
	ACS_APGCC_ReturnType lm_svc();
	/**
	 * @brief
	 * destructor
	 */

	~acs_logm_logmservice();

};
#endif /* acs_logm_logmservice_H_ */
