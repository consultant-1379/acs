/*=================================================================== */
/**
   @file   acs_emf_haservice.h

   @brief Header file for EMF module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY   XRAMMAT   Initial Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_EMF_HASERVICE_H
#define ACS_EMF_HASERVICE_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <unistd.h>
#include <ace/Task.h>
#include <ace/OS_NS_poll.h>
#include <ACS_APGCC_ApplicationManager.h>
#include "acs_emf_server.h"
class EMF_SERVICE_MODULE;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
 	@brief	ACS_EMF_HAService
 */
/*===================================================================*/
class ACS_EMF_HAService: public ACS_APGCC_ApplicationManager
{
	/*=====================================================================
			                        PRIVATE DECLARATION SECTION
	 ==================================================================== */
private:
	/*===================================================================
		                        PRIVATE ATTRIBUTE
	  =================================================================== */
	/**
		      @brief	readWritePipe
	 */
	//int readWritePipe[2];
	/**
			@brief	Is_terminated
	 */
	//ACS_APGCC_BOOL Is_terminated;
	/**
				@brief		applicationThreadId
	 */
	//ACE_thread_t applicationThreadId;

	EMF_SERVICE_MODULE *theEMFServiceModulePtr;
	/**
		@brief          passiveToActive
	 */
	ACE_UINT32 passiveToActive;
	/*=================================================================== */
	/**
                        @brief     activateApp

                        @par       Deprecated
                                   never

                        @pre       none

                        @post      none

                        @param     none

                        @return    ACS_APGCC_ReturnType

                        @exception none
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType activateApp();
	
	/*=================================================================== */
	/**
                        @brief     passifyApp 

                        @par       Deprecated
                                   never

                        @pre       none

                        @post      none

                        @param     none

                        @return    ACS_APGCC_ReturnType

                        @exception none
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType passifyApp();
	/**
	 * @brief	shutdownApp
	 * @return  ACS_APGCC_ReturnType
	 */
	ACS_APGCC_ReturnType shutdownApp();
	/*=====================================================================
		                       PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
			                       CLASS CONSTRUCTOR
	 ==================================================================== */
	/*=================================================================== */
	/**
		      @brief     Constructor for ACS_EMF_HAService class.

			             Default constructor, used to initialize variables.

	 */
	/*=================================================================== */
	ACS_EMF_HAService(const char* daemon_name, const char* username);
	/*=====================================================================
								   CLASS DESTRUCTOR
	 ==================================================================== */
	/*=================================================================== */
	/**
		      @brief     Destructor for ACS_EMF_HAService class.

	 */
	/*=================================================================== */
	~ACS_EMF_HAService();
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
				@brief		svc

				@pre		none

				@post		none

				@return		ACS_APGCC_ReturnType

				@exception	none
	 **/
	/****************************************************************************** */
	//ACS_APGCC_ReturnType svc(void);
};

#endif /* ACS_EMF_HASERVICE_H */
