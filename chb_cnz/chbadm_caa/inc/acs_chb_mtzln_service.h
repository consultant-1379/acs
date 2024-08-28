/*=================================================================== */
/**
   @file   acs_chb_mtzln_service.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_MTZLN_Service_H_
#define ACS_CHB_MTZLN_Service_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Task.h>
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_AmfTypes.h>
#include <ACS_APGCC_ApplicationManager.h>
#include "acs_chb_mtzln_implementer.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
 	@brief	ACS_CHB_MTZLN_Service
 */
/*===================================================================*/
class  ACS_CHB_MTZLN_Service: public ACS_APGCC_ApplicationManager
{
	/*=====================================================================
		                        PRIVATE DECLARATION SECTION
	==================================================================== */
   private:
	/*===================================================================
	                        PRIVATE ATTRIBUTE
	=================================================================== */
	/*=================================================================== */
	/**
	      @brief   theMTZLNImplementer
	 */
	/*=================================================================== */
	ACS_CHB_MTZLN_Implementer *theMTZLNImplementer;
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
		@brief	applicationThreadId
	*/
	ACE_thread_t	applicationThreadId;

	ACS_APGCC_AMF_HA_StateT nodeState;
#endif
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
	/*=====================================================================
	                       PUBLIC DECLARATION SECTION
	==================================================================== */
   public:

	~ACS_CHB_MTZLN_Service();

	/*===================================================================
		                      PUBLIC ATTRIBUTE
	=================================================================== */
	/*=================================================================== */
	/**
	      @brief   theMTZLNImplementer
	 */
	/*=================================================================== */
	//static bool theActiveNodeState;
	/*=====================================================================
		                       CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief     Constructor for ACS_CHB_MTZLN_Service class.

		             Default constructor, used to initialize variables.

	*/
	/*=================================================================== */
   	ACS_CHB_MTZLN_Service(const char* daemon_name, const char* username);
   	/*=================================================================== */
   	  /**
   	        @brief     performStateTransitionToActiveJobs

   	        @par       Deprecated
   	                   never

   	        @pre       none

   	        @post      none

   	        @param     previousHAState

   	        @return    ACS_APGCC_ReturnType

   	        @exception none
   	   */
   	  /*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */
	   	  /**
	   	        @brief     performStateTransitionToPassiveJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @param     previousHAState

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	   	  /*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */
	   	  /**
	   	        @brief     performStateTransitionToQueisingJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @param     previousHAState

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	   	  /*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */
                  /**
                        @brief     performStateTransitionToQueiscedJobs

                        @par       Deprecated
                                   never

                        @pre       none

                        @post      none

                        @param     previousHAState

                        @return    ACS_APGCC_ReturnType

                        @exception none
                   */
                  /*=================================================================== */

	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */
	   	  /**
	   	        @brief     performComponentHealthCheck

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	   	  /*=================================================================== */
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	/*=================================================================== */
	   	  /**
	   	        @brief     performComponentTerminateJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	   	  /*=================================================================== */
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	/*=================================================================== */
	   	  /**
	   	        @brief     performComponentRemoveJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	   	  /*=================================================================== */
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	/*=================================================================== */
	/********************************************************************************/
		/**
			@brief		performApplicationShutdownJobs
			
			@pre		none
			
			@post		none
		
			@return 	ACS_APGCC_ReturnType
		
			@exception	none
		**/
	/******************************************************************************** */
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

	/****************************************************************************** */
		/**
			@brief		mtzln_svc

			@pre		none
			
			@post		none
			
			@return		ACS_APGCC_ReturnType

			@exception	none
		**/
	/****************************************************************************** */	
	//ACS_APGCC_ReturnType mtzln_svc(void);
	
};
#endif


