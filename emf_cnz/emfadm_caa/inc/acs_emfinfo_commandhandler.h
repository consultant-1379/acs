/*=================================================================== */
/**
   @file   acs_emfget_commandhandler.h

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
#ifndef ACS_EMFINFO_COMMANDHANDLER_H_
#define ACS_EMFINFO_COMMANDHANDLER_H_
/*=====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <saImmOi.h>
#include <saImm.h>

#include <poll.h>
#include <ace/ACE.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include "acs_emf_dvdhandler.h"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The acs_emf_commandhandler used for handle the emfinfo admin operations.
*/
/*=================================================================== */
class acs_emfinfo_commandhandler //:  public ACS_APGCC_ObjectImplementer_EventHandler
{
	public:
	/*=====================================================================
						  PUBLIC DECLARATION SECTION
	 ==================================================================== */
	/*===================================================================
						  PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
							   PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/*=================================================================== */
	/**
			@brief       Default constructor for acs_emfinfo_commandhandler

			@par         None

			@pre         None

			@post        None

			@param       pDVDHandler
							 DVDHandler pointer

			@param       endEvent
							 ACE Event Handler pointer

			@exception   None
	 */
	/*=================================================================== */
	acs_emfinfo_commandhandler (ACS_EMF_DVDHandler *pDVDHandler,ACE_HANDLE endEvent);
	/*=================================================================== */
	/**
			 @brief       Default destructor for acs_emfinfo_commandhandler

			 @par         None

			 @pre         None

			 @post        None

			 @exception   None
	 */
	/*=================================================================== */
		~acs_emfinfo_commandhandler();
	/*=================================================================== */
	/**
			 @brief       Method used to handle the admin operation performed by emfinfo command

			 @par         None

			 @pre         None

			 @post        None

			 @param			immOiHandle
											contains OIHanlde
			 @param			invocation
											contains invocation
			 @param			objectName
											contains IMM Objectname
			 @param			opId
											contains Operation Id
			 @param			params
											contains IMM Parameters
			 @exception   None
	 */
	/*=================================================================== */
		 static void saImmOiAdminOperationCallback(SaImmOiHandleT immOiHandle,
								SaInvocationT invocation,
								const SaNameT *objectName,
								SaImmAdminOperationIdT opId, const SaImmAdminOperationParamsT_2 **params);
	/*=================================================================== */
	/**
			 @brief       Method used to register with IMM for emfinfo admin operations

			 @par         None

			 @pre         None

			 @post        None

			 @param		 dn

			 @param      implName

			 @exception   None
	*/
	/*=================================================================== */
			SaAisErrorT register_object(std::string  dn, std::string implName);
	/*=================================================================== */
	/**
			 @brief       Method used to release(remove) the implementer used for emfcopy admin operations

			 @par         None

			 @pre         None

			 @post        None

			 @exception   None
	*/
	/*=================================================================== */
			SaAisErrorT remove_Implementor();

	private:
	/*=====================================================================
						  PRIVATE DECLARATION SECTION
	 ==================================================================== */
	/*===================================================================
							PRIVATE ATTRIBUTE
	 =================================================================== */
	/*=================================================================== */
	/**
			  @brief  contains ACS_EMF_DVDHandler pointer
	 */
	/*=================================================================== */
			static ACS_EMF_DVDHandler* theDVDHandlePtr;
	/*=================================================================== */
	/**
			  @brief  contains the ACE event pointer
	 */
	/*=================================================================== */
			ACE_HANDLE stopThreadFds;

};
#endif /* ACS_EMFINFO_COMMANDHANDLER_H_ */
