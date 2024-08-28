/*=================================================================== */
   /**
   @file acs_nsf_service.h

   Header file for ACS_NSF_Service class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.
   N/A       06/02/2014     XQUYDAO        Updated to handle better AMF callback
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#ifndef ACS_NSF_Service_H_
#define ACS_NSF_Service_H_

#include <ace/Task.h>
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_ApplicationManager.h>
#include <acs_nsf_server.h>

/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class  ACS_NSF_Service : public ACS_APGCC_ApplicationManager  
{
   private:	
	
	ACS_NSF_Server *theNSFServerPtr;        
	ACS_APGCC_ReturnType activateApp(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType passifyApp(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType shutdownApp();


   public:

	/**
		@brief		ACS_NSF_Service
		
		@param		daemon_name

		@param		username
	
	*/

	ACS_NSF_Service(const char* daemon_name, const char* username);

	/**
		@brief		performStateTransitionToActiveJobs

		@param		previousHAState

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**
		@brief		performStateTransitionToPassiveJobs

		@param		previousHAState

		@return 	ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**
		@brief		performStateTransitionToQueisingJobs

		@param		previousHAState

		@return 	ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**
		@brief		performStateTransitionToQueiscedJobs

		@param		previousHAState

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
		@brief		nsf_svc

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType nsf_svc();

	~ACS_NSF_Service();

};
#endif /* ACS_NSF_Service_H_ */
