/*=================================================================== */
/**
@file		acs_nsf_command_handler.h

@brief		Header file for common utilities for NSF.

			This module contains all the declarations useful to
			specify the class.

@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/12/2010     XCHEMAD       Initial Release
N/A	  19/04/2011	 XTANAGG       Added doxygen comments.
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
#ifndef ACS_NSF_COMMANDHANDLER_H
#define ACS_NSF_COMMANDHANDLER_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include "acs_nsf_common.h"
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_oihandler_V3.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define NSF_IMM_CONFIG_ATTR_NAME ""


/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
class ACS_NSF_Common;
/**
 * @brief	ACS_NSF_CommandHandler
 * 			ACS_NSF_CommandHandler class is extended from acs_apgcc_objectimplementereventhandler_V3.
 * 			It is having functions to handle IMM callbacks
 */
class ACS_NSF_CommandHandler:public acs_apgcc_objectimplementereventhandler_V3
{
public:
	/**
	 * @brief
	 * constructor with three parameters
	 */
   ACS_NSF_CommandHandler( string szimpName ); 
   /**
   	* @brief
   	* constructor with five parameters
   	*/
	ACS_NSF_CommandHandler( string ClassName, ACS_NSF_Common *objCommon,string szimpName, ACS_APGCC_ScopeT p_scope);
   /**
   	* @brief
   	* destructor
   	*/
	~ACS_NSF_CommandHandler();
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
	 *  @brief		deleted
	 *  			This method is inherited from base class and overridden by our class.
	 *  			This method is get invoked when IMM object deleted.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param	objName
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	/**
	 *  @brief	modify
	 *  		This method is inherited from base class and overridden by our class.
	 *  		This method is get invoked when IMM object's attribute modify.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param	objName
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
	 *  @brief	updateRuntime
	 *  		This method is inherited from base class and overridden by our class.
	 *  @param  objName          : const char pointer
	 *  @param  attrName         : const char pointer
	 */
	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

	/**
	 *  @brief	adminOperationCallback
	 *  		This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle         : ACS_APGCC_OiHandle 
	 *  @param  invocation       : ACS_APGCC_InvocationType
	 *  @param  p_objName          : const char pointer
     *  @param  operationId      : ACS_APGCC_AdminOperationIdType
     *  @param  paramList        : ACS_APGCC_AdminOperationParamType
	 */
        void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);


	/**
	 *  @brief	svc
	 *  		This method is used to initializes the object Implementer functions of the IMM service for the invoking process
				and registers the callback function.
	 *
	 */
	ACE_INT32 svc();

	/**
	 * @brief	validateChange
	 * @param 	aNetworkSurvellienceValue
	 * @param 	aErrorCode
	 * @param 	aErrorMessage
	 * @return
	 */
	bool validateNSValue(ACE_INT32 aNetworkSurvellienceValue,unsigned int & aErrorCode,std::string& aErrorMessage);


	/**
	 * @brief	validateChange
	 * @param 	aParamValue
	 * @param 	attrName
	 * @return	bool
	 */
	bool validateChange(	ACE_UINT32 aParamValue,
				char* attrName);


	/**
	 * @brief	shutdown
	 *
	 */
	 void shutdown();
private:

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

  /**
   * @brief
   * its a pointer of type ACS_NSF_FolderQuotaMon
  */
  std::string m_szRequestedConfig;

  /**
   * @brief theObjCommonPtr
   */
  ACS_NSF_Common * theObjCommonPtr;

};


#endif /** end of ACS_NSF_COMMANDHANDLER_H **/
