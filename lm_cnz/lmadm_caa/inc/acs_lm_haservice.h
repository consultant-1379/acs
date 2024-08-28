/*=================================================================== */
   /**
   @file acs_lm_haservice.h

   Header file for ACS_LM_HAService class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/09/2011     XTANAGG   Initial Revision for APG43L.
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#ifndef ACS_LM_HAService_H_
#define ACS_LM_HAService_H_

#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_ApplicationManager.h>
#include "acs_lm_server.h"
#include <unistd.h>

/**
 * @brief	lm_service_run
 *
 */
ACE_THR_FUNC_RETURN  lm_service_run(void *);

/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class ACS_LM_HAService : public ACS_APGCC_ApplicationManager  
{
private:
#if 0
	/**
		@brief	readWritePipe
	 */
	int readWritePipe[2];

	/**
		@brief	Is_terminated
	 */
	ACS_APGCC_BOOL Is_terminated;

	/**
		@brief	lmHAThreadId
	 */
	ACE_thread_t thelmHAThreadId;

	/**
		@brief	nodeState
	 */
	ACS_APGCC_AMF_HA_StateT nodeState;
#endif
	/**
	* @brief          m_lmApplObj
	*/
	ACS_LM_Server	*m_lmApplObj;
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
		@brief		ACS_LM_HAService

		@param		daemon_name : const char pointer

		@param		username : const char pointer

	 */

	ACS_LM_HAService(const char* daemon_name, const char* username);

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

	~ACS_LM_HAService();

};
#endif /* ACS_LM_HAService_H_ */
