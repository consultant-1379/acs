/*=================================================================== */
/**
   @file   acs_rtr_ha_service.h

   @brief Header file for ACS  module.

          This module contains all the declarations useful to
          specify the class ACS_RTR_HA_Service.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       14/09/2011   XTANAGG   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_HA_Service_H_
#define ACS_RTR_HA_Service_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_AmfTypes.h>
#include <ACS_APGCC_ApplicationManager.h>


class ACS_RTR_Server;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
 	@brief	ACS_RTR_HA_Service
 */
/*===================================================================*/
class  ACS_RTR_HA_Service: public ACS_APGCC_ApplicationManager
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
	      @brief   rtrAdmPtr
	 */
	/*=================================================================== */
	ACS_RTR_Server* rtrAdmPtr;
	/*=================================================================== */
   public:

	/*===================================================================
		                      PUBLIC ATTRIBUTE
	=================================================================== */
	/*=====================================================================
		                       CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief     Constructor for ACS_RTR_HA_Service class.

		         Default constructor, used to initialize variables.

	*/
	/*=================================================================== */
	ACS_RTR_HA_Service(const char* daemon_name, const char* username);

	/*=====================================================================
				       CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
		  @brief     Destructor for ACS_RTR_HA_Service class.

	*/
	/*=================================================================== */
	~ACS_RTR_HA_Service();

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
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(
					ACS_APGCC_AMF_HA_StateT previousHAState);

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
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(
				ACS_APGCC_AMF_HA_StateT previousHAState);

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
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(
				ACS_APGCC_AMF_HA_StateT previousHAState);
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
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(
				ACS_APGCC_AMF_HA_StateT previousHAState);

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
	/*=================================================================== */
		/**
			@brief		performApplicationShutdownJobs

			@pre		none

			@post		none

			@return 	ACS_APGCC_ReturnType

			@exception	none
		**/
	/*=================================================================== */
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);
	/*=================================================================== */

	/*=================================================================== */
		/**
			@brief		goActive

			@pre		none

			@post		none

			@return 	ACS_APGCC_ReturnType

			@exception	none
		**/
	/*=================================================================== */
	ACS_APGCC_ReturnType goActive();
	/*=================================================================== */

	/*=================================================================== */
		/**
			@brief		goPassive

			@pre		none

			@post		none

			@return 	ACS_APGCC_ReturnType

			@exception	none
		**/
	/*=================================================================== */
	ACS_APGCC_ReturnType goPassive(void);
	/*=================================================================== */

	/*=================================================================== */
		/**
			@brief		shutdown

			@pre		none

			@post		none

			@return 	ACS_APGCC_ReturnType

			@exception	none
		**/
	/*=================================================================== */
	ACS_APGCC_ReturnType shutdown();
	/*=================================================================== */
};
#endif
