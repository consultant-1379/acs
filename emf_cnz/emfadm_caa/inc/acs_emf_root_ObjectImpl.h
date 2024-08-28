//=============================================================================
/**
   @file    acs_emf_root_ObjectImpl.h

   @brief Header file for EMF module.
          It acts as OI for handling IMM Callbacks of EMF root object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       24/10/2011   XRAMMAT   Initial Release
 */
//=============================================================================
#ifndef ACS_EMF_ROOTOBJECTIMPL_H
#define ACS_EMF_ROOTOBJECTIMPL_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <acs_apgcc_objectimplementereventhandler_V2.h>
#include <acs_apgcc_oihandler_V2.h>
#include <ace/Task.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief ACS_EMF_ObjectImpl
 * ACS_EMF_ObjectImpl class is extended from acs_apgcc_objectImplementereventhandler_V2.
 * It is having functions to handle IMM callbacks
 *
 */

class ACS_EMF_ObjectImpl:public acs_apgcc_objectimplementereventhandler_V2 ,public ACE_Task_Base
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
	ACS_EMF_ObjectImpl(string szimpName );
	/*=====================================================================
									   CLASS DESTRUCTOR
	  ==================================================================== */
    /**
   	* @brief
   	* destructor
   	*/
	~ACS_EMF_ObjectImpl();

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
	ACS_CC_ReturnType updateRuntime(const char *objName, const char *attrName);

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

	/**
	 *  updateRuntime method: This method is inherited from base class and overridden by our class.
	 *  @param  poReactor          : ACE_Reactor
	 *  @return ACE_INT32
	 */
	ACE_INT32 svc(ACE_Reactor* &poReactor);

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
	acs_apgcc_oihandler_V2 *theOiHandlerPtr;
};

#endif
