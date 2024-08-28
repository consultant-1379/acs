//******************************************************************************
//
// NAME
//      ACS_LCT_CommandHandler_Hardening_Hardening.h
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//      It acts as OI for handling IMM Callbacks.
//              
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//      2011-08-03 XSATDEE
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//			2012-25-04      XFURULL         welcomemessage system 
//							improvement(OP#345) on all ports
//
//******************************************************************************  
//=============================================================================
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_LCT_COMMANDHANDLER_H
#define ACS_LCT_COMMANDHANDLER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include "acs_lct_common.h"
#include "acs_lct_dsdserver.h"
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>

#include "acs_apgcc_objectimplementereventhandler_V3.h"
//#include "ACS_APGCC_ObjectImplementer_EventHandler.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
    @class              ACS_LCT_CommandHandler_Hardening

    @brief              This class implements the IMM callback functionality 
**/
 /*=================================================================== */

class ACS_LCT_CommandHandler_Hardening:public acs_apgcc_objectimplementereventhandler_V3,public ACE_Task_Base
{
/*=============================================================================
            PUBLIC DECLARATION SECTION
  =============================================================================*/

public:
/*=====================================================================
                                CLASS CONSTRUCTORS
 ==================================================================== */

        /*=================================================================== */

                /**

                        @brief                  Command handler constructor

                        @pre

                        @post

                        @return                 none

                        @exception              none
                */

	ACS_LCT_CommandHandler_Hardening(string szobjName,
		                   string szimpName,
		                   ACS_APGCC_ScopeT enScope );
        /*===================================================================
                                CLASS DESTRUCTOR
        =================================================================== */
        /*=================================================================== */
                /**

                        @brief                  Destructor

                        @pre                    none

                        @post                   none

                        @return                 none

                        @exception              none
                */
        /*=================================================================== */
	~ACS_LCT_CommandHandler_Hardening();

	/* ===================================================================
	   @brief  create method: This method is inherited from base class and overridden by our class.
	   This method gets invoked when IMM object is created.
           @pre                    none
           @post                   none
	   @param  oiHandle      : ACS_APGCC_OiHandle
	   @param  ccbId         : ACS_APGCC_CcbId
	   @param  className     : const char pointer
	   @param  parentname    : const char pointer
	   @param  attr          : ACS_APGCC_AttrValues
	   @return ACS_CC_ReturnType : Sucess/failure
	   @exception none
	 =========================================================================*/
	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

        /* ===================================================================
	   @brief  deleted method: This method is inherited from base class and overridden by our class.
	   This method is get invoked when IMM object deleted.
           @pre                    none
           @post                   none
	   @param  oiHandle      :	ACS_APGCC_OiHandle
	   @param  ccbId         : ACS_APGCC_CcbId
	   @return ACS_CC_ReturnType : Sucess/failure
        =========================================================================*/
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

        /* ===================================================================
	   @brief  modify method: This method is inherited from base class and overridden by our class.
	   This method is get invoked when IMM object's attribute modify.
           @pre                    none
           @post                   none
	   @param  oiHandle      : ACS_APGCC_OiHandle
	   @param  ccbId         : ACS_APGCC_CcbId
	   @param  attr          : ACS_APGCC_AttrValues
	   @return ACS_CC_ReturnType : Sucess/failure
        =========================================================================*/	 
	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

        /* ===================================================================
	   @brief  complete method: This method is inherited from base class and overridden by our class.
           @pre                    none
           @post                   none
	   @param  oiHandle      : ACS_APGCC_OiHandle
	   @param  ccbId         : ACS_APGCC_CcbId
	   @return ACS_CC_ReturnType : Sucess/failure
        =========================================================================*/	 
	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

        /* ===================================================================
	   @brief  abort method: This method is inherited from base class and overridden by our class.
           @pre                    none
           @post                   none
	   @param  oiHandle      :	ACS_APGCC_OiHandle
	   @param  ccbId         : ACS_APGCC_CcbId
	   @return 		   none
        =========================================================================*/
	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

        /* ===================================================================
	 * @brief  apply method: This method is inherited from base class and overridden by our class.
           @pre                    none
           @post                   none
	   @param  oiHandle      :	ACS_APGCC_OiHandle
	   @param  ccbId         : ACS_APGCC_CcbId
           @return                 none
         =========================================================================*/
	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

        /* ===================================================================
	   @brief  updateRuntime method: This method is inherited from base class and overridden by our class.
           @pre                    none
           @post                   none

	   @param  objName          : const char pointer
	   @param  attrName         : const char pointer
           @return                 none
         =========================================================================*/

	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

        /* ===================================================================
	   @brief  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
	 	and registers the callback function.
           @pre                    none
           @post                   none
	   @param  oiHandle      :	ACS_APGCC_OiHandle
	   @param  ccbId         : ACS_APGCC_CcbId
           @return                 none
         =========================================================================*/
	 
	ACE_INT32 svc();
	//static void handle_signal(int signum);

	/*=================================================================== 
		@brief  rollback() method
		        It is to rollback operations in linux configuration files if there is a failure
           	@pre                    none
	        @post                   none
		@param			string
		@return      		bool
		@exception   		none
	=================================================================== */
	bool rollback (string attr_name);
	acs_apgcc_oihandler_V3 oiHandler;
        void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

        ACE_INT32 shutdown();
        ACE_TP_Reactor* m_TPReactorImpl;
        ACE_Reactor* m_poReactor;

};

#endif
