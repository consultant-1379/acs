//=============================================================================
/**
   @file    acs_hcs_readyToUse_objectImpl.h

   @brief Header file for HC module.
          It acts as OI for handling IMM Callbacks of HC readyToUse object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012   XHARBAV   Initial Release
 */
//=============================================================================
#ifndef ACS_HC_READYTOUSE_OBJECTIMPL_H
#define ACS_HC_READYTOUSE_OBJECTIMPL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <cstdlib>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <ACS_TRA_trace.h>
#include "acs_hcs_tra.h"
#include "ACS_APGCC_RuntimeOwner_V2.h"

#define LOAD                    1

class ACS_HC_Server;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief acs_hcs_readyToUse_ObjectImpl
 * acs_hcs_readyToUse_ObjectImpl class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 */
class acs_hcs_readyToUse_ObjectImpl:public acs_apgcc_objectimplementereventhandler_V3 ,public ACE_Task_Base 
{
	/*=====================================================================
		                       PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
				                       CLASS CONSTRUCTOR
	 ==================================================================== */
	/**
	 * @brief
	 * constructor with two parameters
	 */
	acs_hcs_readyToUse_ObjectImpl( string ClassName, string p_impName, ACS_APGCC_ScopeT p_scope );
	/*=====================================================================
									   CLASS DESTRUCTOR
	  ==================================================================== */
    /**
   	* @brief
   	* destructor
   	*/
	~acs_hcs_readyToUse_ObjectImpl();

	/**
	 *  create method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object created.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  className     : const char pointer
	 *  @param  parentname    : const char pointer
	 *  @param  attr          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
	/**
	 *  deleted method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object deleted.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  objName
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	/**
	 *  modify method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object's attribute modify.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  objName
	 *  @param  attrMods          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	/**
	 *  complete method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  abort method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  apply method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  updateRuntime method: This method is inherited from base class and overridden by our class.
	 *  @param  objName          : const char pointer
	 *  @param  attrName         : const char pointer
	 */
	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

	/**
	 *  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
	 *	and registers the callback function.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  invocation         : ACS_APGCC_InvocationType
	 *  @param  p_objName
	 *  @param  operationId
	 *  @param paramList
	 */
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);
	
	 /*============================================================*/
     /**
            @brief     setErrorCode

            @param     code

            @return    void
     */

     
     /*============================================================*/
     void setErrorCode(unsigned int code);
     /*============================================================*/

	/**
	 *  updateRuntime method: This method is inherited from base class and overridden by our class.
	 *  @param  poReactor          : ACE_Reactor
	 *  @return ACE_INT32
	 */
	ACE_INT32 svc();

	ACS_CC_ReturnType setImpl();

	void shutdown();

	string theClassName;
	char errorText[100];
		
	/*=====================================================================
		                       PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*===================================================================
		                        PRIVATE ATTRIBUTE
	  =================================================================== */
	/**
	* @brief	theOiHandlerPtr
	*/
	acs_apgcc_oihandler_V3 *theOiHandlerPtr;
    /**
    * @brief   m_poTPReactorImpl
    */
    ACE_TP_Reactor *m_poTPReactorImpl;

	/**
    * @brief   m_poReactor
    */
    ACE_Reactor* m_poReactor;
	
//	ACS_HC_Server* m_poHcsServer;

	//bool m_isClassImplAdded;

};

#endif
