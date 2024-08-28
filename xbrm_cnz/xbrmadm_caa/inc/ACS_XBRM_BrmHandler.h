/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/
/**
*
* @file ACS_XBRM_BrmHandler.h
*
* @brief
* ACS_XBRM_BrmHandler Class for SystemBrMSystemBrM IMM Handling
*
* @details
* ACS_XBRM_BrmHandler is used as an OI (Object Implementor) interface handling
* IMM callbacks for SystemBrMSystemBrM MOC
*
* @author ZPAGSAI
*
-------------------------------------------------------------------------*/
/*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-05-01  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

#ifndef ACS_XBRM_BRMHANDLER_H
#define ACS_XBRM_BRMHANDLER_H

#include <iostream>

// ACE libraries
#include <ace/Task.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>

// ACS libraries
#include <acs_apgcc_oihandler_V3.h>
#include "acs_apgcc_objectimplementereventhandler_V3.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_CS_API.h"
#include "ACS_XBRM_ImmUtlis.h"
#include "ACS_XBRM_Utilities.h"

class ACS_XBRM_BrmHandler : public acs_apgcc_objectimplementereventhandler_V3, public ACE_Task_Base
{
public:
    /**
     * @brief
     * constructor with three parameters
     */
    ACS_XBRM_BrmHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope);
    ACS_XBRM_BrmHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope, ACE_Thread_Manager *aThrmgr);

    /**
     * @brief
     * destructor
     */
    ~ACS_XBRM_BrmHandler();

    /**
     *  create method: This method is inherited from base class and overridden by our class.
     *  This method is get invoked when IMM object created.
     *  @param  oiHandle      : ACS_APGCC_OiHandle
     *  @param  ccbId         : ACS_APGCC_CcbId
     *  @param  className     : const char pointer
     *  @param  parentname    : const char pointer
     *  @param  attr          : ACS_APGCC_AttrValues
     *  @return ACS_CC_ReturnType : Sucess/failure
     */
    ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char *parentName, ACS_APGCC_AttrValues **attr);

    /**
     *  deleted method: This method is inherited from base class and overridden by our class.
     *  This method is get invoked when IMM object deleted.
     *  @param  oiHandle      : ACS_APGCC_OiHandle
     *  @param  ccbId         : ACS_APGCC_CcbId
     *  @return ACS_CC_ReturnType : Sucess/failure
     */
    ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

    /**
     *  modify method: This method is inherited from base class and overridden by our class.
     *  This method is get invoked when IMM object's attribute modify.
     *  @param  oiHandle      : ACS_APGCC_OiHandle
     *  @param  ccbId         : ACS_APGCC_CcbId
     *  @param  attrMods          : ACS_APGCC_AttrValues
     *  @return ACS_CC_ReturnType : Sucess/failure
     */
    ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

    /**
     *  complete method: This method is inherited from base class and overridden by our class.
     *  @param  oiHandle      : ACS_APGCC_OiHandle
     *  @param  ccbId         : ACS_APGCC_CcbId
     *  @return ACS_CC_ReturnType : Sucess/failure
     */
    ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

    /**
     *  abort method: This method is inherited from base class and overridden by our class.
     *  @param  oiHandle      : ACS_APGCC_OiHandle
     *  @param  ccbId         : ACS_APGCC_CcbId
     */
    void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

    /**
     *  apply method: This method is inherited from base class and overridden by our class.
     *  @param  oiHandle      : ACS_APGCC_OiHandle
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
     *      and registers the callback function.
     * @return  :   ACE_INT32
     */
    ACE_INT32 svc();

    /**
     * adminOperationCallback method: This method is inherited from base class and overridden by our class
     * This method will be called as a callback to manage an administrative operation invoked, on the
     *      implemented object, using the adminOperationInvoke method of acs_apgcc_adminoperation class.
     * @param   oiHandler   :   ACS_APGCC_OiHandle
     * @param   invocation  :   ACS_APGCC_InvocationType
     * @param   p_objName   :   const char pointer
     * @param   operationId :   ACS_APGCC_AdminOperationIdType
     * @param   paramList   :   ACS_APGCC_AdminOperationParamType
     * @return  void
     */
    void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
                                const char *p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType **paramList);

    int addImplementer(std::string szImpName);
    int removeImplementer();

    std::string theClassName;
    acs_apgcc_oihandler_V3 *theBrmHandler;
    ACE_Reactor *theReportReactorPtr;
    ACE_TP_Reactor *theReportReactorImplPtr;
    ACS_XBRM_Utilities *utilities = NULL;
    string actionTriggeringTime;
    int availableDomains[6] = {0};
    struct timespec req, rem;

    ACE_INT32 shutdown();
    string getBackupTypeName(ACS_XBRM_UTILITY::BackupType backupType);
    ACS_CC_ReturnType augmentedReportProgressCallback(ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandle, string parentDn);
    int getReportProgressState();
    string getActionTriggeringTime();
    void setActionTriggeringTime();
    bool isBackupOngoing();

private:
    // IMMErrorRuntimeHandler* theErrorRuntimeHandlerPtr; COVERITY: 12553

    /**
     * @brief Creates the error handler object
     * @param  ACS_APGCC_CcbId      :       Error Object ID
     * @param  aErrorId             :       Error ID
     * @param  aErrorText           :       Error Text
     * @return TRUE after successful deletion
     * @return FALSE in case there is error
     */
    bool createErrorHandlerObject(ACS_APGCC_CcbId &aCCBId, unsigned int &aErrorId, std::string &aErrorText);

    /**
     * @brief Deletes the error handler object
     * @return TRUE after successful deletion
     * @return FALSE in case there is error
     */
    bool deleteErrorHandlerObject();

    /**
     * @brief Creates the Sets the error text
     * @param  ACS_APGCC_CcbId      :       Error Object ID
     * @param  aErrorId             :       Error ID
     * @param  aErrorText           :       Error Text
     * @return TRUE after successful deletion
     * @return FALSE in case there is error
     */
    bool setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText);

    void createAsyncProgressReportObj(char *currObjDN);

    bool m_isObjImplAdded;
    std::string transferQueue;
    std::string actionTriggerTime;
    OmHandler immHandle;
};
#endif
