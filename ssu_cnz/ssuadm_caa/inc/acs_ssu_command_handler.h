//=============================================================================
/**
 *  @file    ACS_SSU_CommandHandler.h
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *
 *  @author 2010-10-27 by XNADNAR
 *
 *  @documentno
 *
 *  @brief  It acts as OI for handling IMM Callbacks.
 *
 */
//=============================================================================
#ifndef ACS_SSU_COMMANDHANDLER_H
#define ACS_SSU_COMMANDHANDLER_H
/*Include Section */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <acs_apgcc_objectimplementereventhandler_V2.h>
#include <acs_apgcc_oihandler_V2.h>

#include "acs_ssu_types.h"
#include "acs_ssu_folder_quota_mon.h"

/**
 * @brief ACS_SSU_CommandHandler
 * ACS_SSU_CommandHandler class is extended from acs_apgcc_objectImplementereventhandler_V2.
 * It is having functions to handle IMM callbacks
 *
 */

class ACS_SSU_CommandHandler:public acs_apgcc_objectimplementereventhandler_V2,public ACE_Task_Base
{
public:
	/**
	 * @brief
	 * constructor with three parameters
	 */
   ACS_SSU_CommandHandler( ACE_Event* poEvent,
		                   ACS_SSU_FolderQuotaMon* pFolderQuotaMon,
		                   string szimpName );
   /**
   	 * @brief
   	 * destructor
   	 */
   ~ACS_SSU_CommandHandler();
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
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	/**
	 *  modify method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object's attribute modify.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  attr          : ACS_APGCC_AttrValues
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
        
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

	//int handle_signal(ACE_INT32 signum, siginfo_t * siginfo, ucontext_t * ucontext);
	/**
	 *  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
	 *	and registers the callback function.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	ACE_INT32 svc(ACE_Reactor*& poReactor);

private:
	/**
	 * @brief
	 * its a pointer of type ACE_Event
	 */
  ACE_Event*                  m_phEndEvent;
  /**
   * @brief
   * its a pointer of type ACS_SSU_FolderQuotaMon
   */
  ACS_SSU_FolderQuotaMon*     m_pFolderQuotaMon;
  /**
   * @brief
   * its a reference of type string
   */
  std::string m_szRequestedConfig;

  /*!
   * @brief Creates the Sets the error text
   * @param  ACS_APGCC_CcbId	:	Error Object ID
   * @param  aErrorId      		:	Error ID
   * @param  aErrorText      	:	Error Text
   * @return TRUE after successful deletion
   * @return FALSE in case there is error
   */
  bool setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText);
  ACE_TCHAR* getConfigString(ACE_INT32 configValue);

  /**
  	* @brief	theOiHandlerPtr
  	*/
  	acs_apgcc_oihandler_V2 *theOiHandlerPtr;

};



#endif
