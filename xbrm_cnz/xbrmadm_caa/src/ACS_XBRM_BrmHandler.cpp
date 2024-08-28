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
 ----------------------------------------------------------------------*/ /**
 *
 * @file ACS_XBRM_BrmHandler.cpp
 *
 * @brief
 * ACS_XBRM_BrmHandler Class for SystemBrMSystemBrM IMM Handling
 *
 * @details
 * Implementation of ACS_XBRM_BrmHandler class to handle the IMM
 * callbacks for SystemBrMSystemBrM MOC
 *
 * @author ZPAGSAI
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-05-01  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

//ACE libraries
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include <ace/Signal.h>

//ACS libraries
#include <ACS_APGCC_Util.H>
#include <acs_apgcc_oihandler_V3.h>
#include <ACS_APGCC_CommonLib.h>
#include "ACS_XBRM_BrmHandler.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "ACS_XBRM_ImmUtlis.h"
#include "ACS_CS_API.h"
#include "ACS_XBRM_ApBackupHandler.h"

#include <string>
#include <memory>
using namespace std;

int backupType[6] = {0};
int backupCategory;
int systemBackupType;

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_BrmHandler);

ACS_XBRM_BrmHandler::ACS_XBRM_BrmHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope) 
:acs_apgcc_objectimplementereventhandler_V3(ClassName, p_impName, p_scope), 
theClassName(ClassName){
    //TRACE
    ACS_XBRM_TRACE_FUNCTION;
    m_isObjImplAdded = false;   
    theReportReactorImplPtr = 0;
    theReportReactorImplPtr = new ACE_TP_Reactor();

    if(theReportReactorImplPtr != 0){
        theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
    }
}

ACS_XBRM_BrmHandler::ACS_XBRM_BrmHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope, ACE_Thread_Manager *aThrmgr)
:acs_apgcc_objectimplementereventhandler_V3(ClassName, p_impName, p_scope), 
theClassName(ClassName){
    //TRACE
    ACS_XBRM_TRACE_FUNCTION;
    this->thr_mgr(aThrmgr);
    m_isObjImplAdded = false; 
    theReportReactorImplPtr = 0;
    theReportReactorImplPtr = new ACE_TP_Reactor();
    
    if(theReportReactorImplPtr != 0){
        theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
    }
}

ACS_XBRM_BrmHandler::~ACS_XBRM_BrmHandler(){
    ACS_XBRM_TRACE_FUNCTION;
    if(ACS_CC_SUCCESS == immHandle.Finalize()){
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "ACS_XBRM_BrmHandler immHandle is finalized");
        ACS_XBRM_TRACE_MESSAGE("ACS_XBRM_BrmHandler immHandle is finalized");
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "ACS_XBRM_BrmHandler immHandle is finalization failed");
        ACS_XBRM_TRACE_MESSAGE("ACS_XBRM_BrmHandler immHandle is finalization failed");
    }

    if(theReportReactorPtr != 0){
        delete theReportReactorPtr;
        theReportReactorPtr = 0;
    }

    if(theReportReactorImplPtr != 0){
        delete theReportReactorImplPtr;
        theReportReactorImplPtr = 0;
    }

    if(theBrmHandler != 0){
        delete theBrmHandler;
        theBrmHandler = 0;
    }
}

ACE_INT32 ACS_XBRM_BrmHandler::svc(){
    //TRACE
    ACS_XBRM_TRACE_FUNCTION;
    theBrmHandler = 0;
    theBrmHandler = new acs_apgcc_oihandler_V3();
    utilities = new ACS_XBRM_Utilities();

    try{
        if(ACS_CC_SUCCESS != immHandle.Init()){
            //TRACE
            ACS_XBRM_LOG(LOG_LEVEL_WARN, "IMM Handler initialization failed");
            ACS_XBRM_TRACE_MESSAGE("IMM Handler initialization failed");
            return -1;
        }
        if(theBrmHandler != 0){
            for(int i = 0; i < 10; i++){
                if(ACS_CC_FAILURE == theBrmHandler->addClassImpl(this, theClassName.c_str())){
                    ACS_XBRM_LOG(LOG_LEVEL_WARN, "BrmHandler addClassImpl() failed");
                    ACS_XBRM_TRACE_MESSAGE("BrmHandler addClassImpl() failed");
                    sleep(1);
                }
                else{
                    //TRACE
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "BrmHandler addClassImpl() successful");
                    ACS_XBRM_TRACE_MESSAGE("BrmHandler addClassImpl() successful");
                    m_isObjImplAdded = true;
                    break;
                }
            }
        }
        if(false == m_isObjImplAdded)
            return -1;
        
    }
    catch(...){//catches any type of exception
        //exception handling
    }

    if(theReportReactorPtr != 0){
        //TRACE
        if(theReportReactorPtr->open(1) == 0){
            //TRACE
            ACS_XBRM_LOG(LOG_LEVEL_WARN, "BrmHandler Reactor pointer failed to initialize ::open()");
            ACS_XBRM_TRACE_MESSAGE("BrmHandler Reactor pointer failed to initialize ::open()");
            return -1;
        }
        dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
        theReportReactorPtr->run_reactor_event_loop();
    }
    return 0;
}

ACS_CC_ReturnType ACS_XBRM_BrmHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char *parentName, ACS_APGCC_AttrValues **attr){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    //TRACE
    (void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;

    bool isBackupUserLabelConfigured = false;
    bool isBackupPasswordConfigured = false;
    
    string brmDn = "";
    string transferQueueDn = "";

    for(int i = 0; attr[i] != NULL; i++){
        if(0 == (int)attr[i]->attrValuesNum) continue;
        if(0 == ACE_OS::strcmp("SystemBrMsystemBrMId", attr[i]->attrName)){
            brmDn.assign(reinterpret_cast<char *>(attr[i]->attrValues[0]));
            if(0 != ACE_OS::strcmp(reinterpret_cast<char *>(attr[i]->attrValues[0]), "SystemBrMsystemBrMId=1")){
                string errorTxt = "Invalid value for keyAttribute. Value must be '1'.\n";
                this->setExitCode(17, errorTxt);
                return ACS_CC_FAILURE;
            }
        }else if(0 == ACE_OS::strcmp(ACS_XBRM_ImmMapper::ATTR_TRANFER_QUEUE_NAME.c_str(), attr[i]->attrName)){
            string tq;
            tq.assign(reinterpret_cast<char *>(attr[i]->attrValues[0]));
            if(ACS_XBRM_UTILITY::TQ_VALID != utilities->isValidTransferQueue(tq, transferQueueDn)){
                ACS_XBRM_LOG(LOG_LEVEL_WARN, "%s is an Invalid transferQueue", tq.c_str());
                ACS_XBRM_TRACE_MESSAGE("%s is an Invalid transferQueue", tq.c_str());
                string errorTxt = "Invalid transfer queue.\n";
                this->setExitCode(17, errorTxt);
                return ACS_CC_FAILURE;
            }else{
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s is valid transferQueue", tq.c_str());
                ACS_XBRM_TRACE_MESSAGE("%s is valid transferQueue", tq.c_str());
            }
        }else if(0 == ACE_OS::strcmp(ACS_XBRM_ImmMapper::ATTR_BACKUP_USER_LABEL_NAME.c_str(), attr[i]->attrName)){
            if(0 == ACE_OS::strcmp(reinterpret_cast<char *>(attr[i]->attrValues[0]), "")){
                string errorTxt = "Backup user label cannot be empty.\n";
                this->setExitCode(17, errorTxt);
                return ACS_CC_FAILURE;
            }
            isBackupUserLabelConfigured = true;
        }else if(0 == ACE_OS::strcmp(ACS_XBRM_ImmMapper::ATTR_BACKUP_PASSWORD_NAME.c_str(), attr[i]->attrName)){
            isBackupPasswordConfigured = true;
        }
    }

    if(isBackupUserLabelConfigured && isBackupPasswordConfigured){
        //correct scenario
    }else if(isBackupUserLabelConfigured){
        string errorTxt = "Backup password not defined.\n";
        this->setExitCode(17, errorTxt);
        return ACS_CC_FAILURE;
    }else if(isBackupPasswordConfigured){
        string errorTxt = "Backup user label not defined.\n";
        this->setExitCode(17, errorTxt);
        return ACS_CC_FAILURE;
    }

    //successfuly validated case

    //augmented modify transferQueueDn
    ACS_APGCC_CcbHandle ccbHandle;
    ACS_APGCC_AdminOwnerHandle adminOwnerHandle;

    ACS_CC_ImmParameter transferQueue;
    transferQueue.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TRANFER_QUEUE_DN_NAME.c_str());
    transferQueue.attrType = ATTR_STRINGT;
    transferQueue.attrValuesNum = 1;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "transferQueueDn = %s", transferQueueDn.c_str());
    void *values[1] = {reinterpret_cast<void *>(const_cast<char *>(transferQueueDn.c_str()))};
    transferQueue.attrValues = values;

    getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandle, &adminOwnerHandle);
    if(ACS_CC_SUCCESS == modifyObjectAugmentCcb(ccbHandle, adminOwnerHandle, brmDn.c_str(), &transferQueue)){
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "transferQueueDn updated successfully with %s", transferQueueDn.c_str());
        ACS_XBRM_TRACE_MESSAGE("transferQueueDn updated successfully with %s", transferQueueDn.c_str());
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "transferQueueDn augmented modification failed");
        ACS_XBRM_TRACE_MESSAGE("transferQueueDn augmented modification failed");
        return ACS_CC_FAILURE;
    }

    //augmentCallbacks to create and modify reportProgress
    augmentedReportProgressCallback(ccbHandle, adminOwnerHandle, brmDn);

    applyAugmentCcb(ccbHandle);

    return result;
}

ACS_CC_ReturnType ACS_XBRM_BrmHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){
    ACS_XBRM_TRACE_FUNCTION;
    (void)oiHandle;
	(void)ccbId;
	(void)objName;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    
    if (isBackupOngoing())
    {
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "Another instance of backup operation is ongoing.");
        ACS_XBRM_TRACE_MESSAGE("Another instance of backup operation is ongoing.");
        string errorTxt = "System Backup(s) operation is ongoing.\n";
        this->setExitCode(17, errorTxt);
        return ACS_CC_FAILURE;
    }

    return result;
}

ACS_CC_ReturnType ACS_XBRM_BrmHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){
    ACS_XBRM_TRACE_FUNCTION;
    (void)oiHandle;
	(void)ccbId;
    (void)objName;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    //Abort peration if backup operation is ongoing
    if (isBackupOngoing())
    {
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "Another instance of backup operation is ongoing.");
        ACS_XBRM_TRACE_MESSAGE("Another instance of backup operation is ongoing.");
        string errorTxt = "System Backup(s) operation is ongoing.\n";
        this->setExitCode(17, errorTxt);
        return ACS_CC_FAILURE;
    }    
    
    bool isBackupUserLabelModified = false;
    bool isBackupPasswordModified = false;
    bool isBackupUserLabelDeleted = false;
    bool isBackupPasswordDeleted = false;
    bool isTqDnToBeModified = false;

    string backupUserLabelModified;
    string backupPasswordModified;
    string transferQueueDn = "";

    for(int i = 0; attrMods[i] != NULL; i++){
        ACS_APGCC_AttrValues modifyAttribute = attrMods[i]->modAttr;
        if(0 == ACE_OS::strcmp(ACS_XBRM_ImmMapper::ATTR_TRANFER_QUEUE_NAME.c_str(), modifyAttribute.attrName)){
            string tq;
            tq.assign(reinterpret_cast<char *>(modifyAttribute.attrValues[0]));

            if(ACS_XBRM_UTILITY::TQ_VALID != utilities->isValidTransferQueue(tq, transferQueueDn)){
                ACS_XBRM_LOG(LOG_LEVEL_WARN, "%s is an Invalid transferQueue", tq.c_str());
                ACS_XBRM_TRACE_MESSAGE("%s is an Invalid transferQueue", tq.c_str());
                string errorTxt = "Invalid transfer queue.\n";
                this->setExitCode(17, errorTxt);
                return ACS_CC_FAILURE;
            }else{
                isTqDnToBeModified = true;
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s is valid transferQueue", tq.c_str());
                ACS_XBRM_TRACE_MESSAGE("%s is valid transferQueue", tq.c_str());
            }
        }else if(0 == ACE_OS::strcmp(ACS_XBRM_ImmMapper::ATTR_BACKUP_USER_LABEL_NAME.c_str(), modifyAttribute.attrName)){
            if(modifyAttribute.attrValuesNum == 0){
                isBackupUserLabelDeleted = true;
            }else{
                isBackupUserLabelModified = true;
                backupUserLabelModified = reinterpret_cast<char *>(modifyAttribute.attrValues[0]);
                if(0 == backupUserLabelModified.length()){
                    string errorTxt = "Backup user label cannot be empty.\n";
                    this->setExitCode(17, errorTxt);
                    return ACS_CC_FAILURE;
                }
            }
        }else if(0 == ACE_OS::strcmp(ACS_XBRM_ImmMapper::ATTR_BACKUP_PASSWORD_NAME.c_str(), modifyAttribute.attrName)){
            if(modifyAttribute.attrValuesNum == 0){
                isBackupPasswordDeleted = true;
            }else{
                isBackupPasswordModified = true;
                backupPasswordModified = reinterpret_cast<char *>(modifyAttribute.attrValues[0]);
            }
        }
    }
    if(isBackupUserLabelDeleted + isBackupPasswordDeleted == 1){
        if(isBackupPasswordDeleted){
            string errorTxt = "Backup user label is configured.\n";
            this->setExitCode(17, errorTxt);
            return ACS_CC_FAILURE;
        }else if(isBackupUserLabelDeleted){
            string errorTxt = "Backup password is configured.\n";
            this->setExitCode(17, errorTxt);
            return ACS_CC_FAILURE;
        }
    }

    if (isBackupUserLabelModified && isBackupPasswordModified){
        //correct use case
    }else if(isBackupUserLabelModified){
        ACS_CC_ImmParameter backupPasswordAttribute;
        backupPasswordAttribute.attrName = const_cast<char*>(ACS_XBRM_ImmMapper::ATTR_BACKUP_PASSWORD_NAME.c_str());
        immHandle.getAttribute(objName, &backupPasswordAttribute);
        if(backupUserLabelModified.length() == 0){
            if(backupPasswordAttribute.attrValuesNum != 0){
                string errorTxt = "Backup password is configured.\n";
                this->setExitCode(17, errorTxt);
                return ACS_CC_FAILURE;
            }
        }else if(backupPasswordAttribute.attrValuesNum == 0){
            string errorTxt = "Backup password not defined.\n";
            this->setExitCode(17, errorTxt);
            return ACS_CC_FAILURE;
        }
    }
    else if(isBackupPasswordModified){
        ACS_CC_ImmParameter backupUserLabelAttribute;
        backupUserLabelAttribute.attrName = const_cast<char*>(ACS_XBRM_ImmMapper::ATTR_BACKUP_PASSWORD_NAME.c_str());
        immHandle.getAttribute(objName, &backupUserLabelAttribute);
        if(backupPasswordModified.length() == 0){
            if (backupUserLabelAttribute.attrValuesNum != 0){
                string errorTxt = "Backup user label is configured.\n";
                this->setExitCode(17, errorTxt);
                return ACS_CC_FAILURE;
            }
        }else if (backupUserLabelAttribute.attrValuesNum == 0){
            string errorTxt = "Backup user label not defined.\n";
            this->setExitCode(17, errorTxt);
            return ACS_CC_FAILURE;
        }
    }

    //successfully validated
    if(isTqDnToBeModified){
        ACS_APGCC_CcbHandle ccbHandle;
        ACS_APGCC_AdminOwnerHandle adminOwnerHandle;

        ACS_CC_ImmParameter transferQueue;
        transferQueue.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TRANFER_QUEUE_DN_NAME.c_str());
        transferQueue.attrType = ATTR_STRINGT;
        transferQueue.attrValuesNum = 1;
        void *values[1] = {reinterpret_cast<void *>(const_cast<char *>(transferQueueDn.c_str()))};
        transferQueue.attrValues = values;

        getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandle, &adminOwnerHandle);
        if(ACS_CC_SUCCESS == modifyObjectAugmentCcb(ccbHandle, adminOwnerHandle, objName, &transferQueue)){
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "transferQueueDn updated successfully with %s", transferQueueDn.c_str());
            ACS_XBRM_TRACE_MESSAGE("transferQueueDn updated successfully %s", transferQueueDn.c_str());
        }else{
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "transferQueueDn augmented modification failed");
            ACS_XBRM_TRACE_MESSAGE("transferQueueDn augmented modification failed");
        }
        applyAugmentCcb(ccbHandle);
    }

    return result;
}

ACS_CC_ReturnType ACS_XBRM_BrmHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    (void)oiHandle;
    (void)ccbId;
    return result;
}

void ACS_XBRM_BrmHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
    ACS_XBRM_TRACE_FUNCTION;
    //TRACE
    (void)oiHandle;
    (void)ccbId;
}

void ACS_XBRM_BrmHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
    ACS_XBRM_TRACE_FUNCTION;
    (void)oiHandle;
    (void)ccbId;
}

ACS_CC_ReturnType ACS_XBRM_BrmHandler::updateRuntime(const char *objName, const char **attrName){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    //TRACE
    (void)objName;
    (void)attrName;
    return result;
}

void ACS_XBRM_BrmHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
                                                 const char *p_objName, ACS_APGCC_AdminOperationIdType operationId,
                                                 ACS_APGCC_AdminOperationParamType **paramList)
{
    ACS_XBRM_TRACE_FUNCTION;
    (void)p_objName;
    // Reset applicable backupTypes
    for (int i = 0; i < 6; i++)
    {
        backupType[i] = 0;
    }
    (void)operationId;
    string backupPassword;
    string backupUserLabel;
    string transferQue;
    ACS_CC_ImmParameter backupPasswordAttribute;
    ACS_CC_ImmParameter transferQueueAttribute;
    ACS_CC_ImmParameter backuplabelAttribute;
    
    ACS_CC_ReturnType result;
   
    string taskStartTime = "";
    ACS_XBRM_EventHandle systemBackupsHandle;

    // availableDomains;
    ACS_CS_API_CommonBasedArchitecture::ApgNumber APtype;
    ACS_CS_API_NetworkElement::getApgNumber(APtype);
    bool isMultipleCPSystem;
    uint32_t cpCount;
    ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);
    ACS_CS_API_NetworkElement::getDoubleSidedCPCount(cpCount);
    availableDomains[ACS_XBRM_UTILITY::APG] = 1;
    availableDomains[ACS_XBRM_UTILITY::ALL] = 1;
    if (ACS_CS_API_CommonBasedArchitecture::AP2 != APtype)
    {
        if (isMultipleCPSystem)
        {
            // blade cluster
            availableDomains[ACS_XBRM_UTILITY::CPCLUSTER] = 1;
            if (cpCount == 2)
            {
                availableDomains[ACS_XBRM_UTILITY::CP1] = 1;
                availableDomains[ACS_XBRM_UTILITY::CP2] = 1;
            }
            else if (cpCount == 1)
            {
                availableDomains[ACS_XBRM_UTILITY::CP1] = 1;
            }
        }
        else
        {
            // singleCp system
            availableDomains[ACS_XBRM_UTILITY::CP] = 1;
        }
    }

    if (isBackupOngoing() )
    {
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "Another instance of backup operation is ongoing.");
        ACS_XBRM_TRACE_MESSAGE("Another instance of backup operation is ongoing.");
        char errMsg[512] = {0};
        snprintf(errMsg, sizeof(errMsg), "%s%s", "@ComNbi@", "System Backup(s) operation ongoing");
        ACS_APGCC_AdminOperationParamType errorMessageParameter;
        errorMessageParameter.attrName = const_cast<char *>("errorComCliMessage");
        errorMessageParameter.attrType = ATTR_STRINGT;
        errorMessageParameter.attrValues = reinterpret_cast<void *>(errMsg);
        std::vector<ACS_APGCC_AdminOperationParamType> outParameteres;
        outParameteres.push_back(errorMessageParameter);
        result = adminOperationResult(oiHandle, invocation, 30, outParameteres);
        if (ACS_CC_FAILURE == result)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "adminOperationResult() callback failed");
            ACS_XBRM_TRACE_MESSAGE("adminOperationResult() callback failed");
        }
        return;
    }
    for (int i = 0; paramList[i] != 0; i++)
    {
        if (0 == ACE_OS::strcmp(paramList[i]->attrName, ACS_XBRM_ImmMapper::PARM_SYSTEM_BACKUP_TYPE_NAME.c_str()))
        {
            setActionTriggeringTime();
            switch (*reinterpret_cast<int *>(paramList[i]->attrValues))
            {
            case ACS_XBRM_UTILITY::APG:
                // ap backup creation
                backupCategory = ACS_XBRM_UTILITY::ON_DEMAND;
                systemBackupType = ACS_XBRM_UTILITY::DOMAIN_TYPE;
                backupType[ACS_XBRM_UTILITY::APG] = 1;

                ACS_XBRM_LOG(LOG_LEVEL_WARN, "Generating APG Domain Backup");

                systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME);
                if (systemBackupsHandle >= 0)
                {
                    if (ACS_XBRM_Event::SetEvent(systemBackupsHandle))
                    {
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "SetEvent for %s successful", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                        ACS_XBRM_TRACE_MESSAGE("SetEvent for %s successful", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                    }
                    else
                    {
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SetEvent for %s failed", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                        ACS_XBRM_TRACE_MESSAGE("SetEvent for %s failed", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                    }
                }
                result = adminOperationResult(oiHandle, invocation, 1);
                if (ACS_CC_FAILURE == result)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "adminOperationResult() callback failed");
                    ACS_XBRM_TRACE_MESSAGE("adminOperationResult() callback failed");
                }

                break;
            case ACS_XBRM_UTILITY::CP:
                if (0 == availableDomains[ACS_XBRM_UTILITY::CP])
                {
                    systemBackupType = ACS_XBRM_UTILITY::DOMAIN_TYPE;
                    char errMsg[512] = {0};
                    snprintf(errMsg, sizeof(errMsg), "@ComNbi@SystemBackupType %s not supported in this system architecture", getBackupTypeName(ACS_XBRM_UTILITY::CP).c_str());
                    ACS_APGCC_AdminOperationParamType errorMessageParameter;
                    errorMessageParameter.attrName = const_cast<char *>("errorComCliMessage");
                    errorMessageParameter.attrType = ATTR_STRINGT;
                    errorMessageParameter.attrValues = reinterpret_cast<void *>(errMsg);
                    std::vector<ACS_APGCC_AdminOperationParamType> outParameteres;
                    outParameteres.push_back(errorMessageParameter);
                    result = adminOperationResult(oiHandle, invocation, 30, outParameteres);
                    if (ACS_CC_FAILURE == result)
                    {
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "adminOperationResult() callback failed");
                        ACS_XBRM_TRACE_MESSAGE("adminOperationResult() callback failed");
                    }
                    return;
                }
                ACS_XBRM_LOG(LOG_LEVEL_WARN, "Generating CP Domain Backup");
                result = adminOperationResult(oiHandle, invocation, 1);
                if (ACS_CC_FAILURE == result)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "adminOperationResult() callback failed");
                    ACS_XBRM_TRACE_MESSAGE("adminOperationResult() callback failed");
                }
                
                break;
            case ACS_XBRM_UTILITY::CP1:
                if (0 == availableDomains[ACS_XBRM_UTILITY::CP1])
                {
                    systemBackupType = ACS_XBRM_UTILITY::DOMAIN_TYPE;
                    char errMsg[512] = {0};
                    snprintf(errMsg, sizeof(errMsg), "@ComNbi@SystemBackupType %s not supported in this system architecture", getBackupTypeName(ACS_XBRM_UTILITY::CP).c_str());
                    ACS_APGCC_AdminOperationParamType errorMessageParameter;
                    errorMessageParameter.attrName = const_cast<char *>("errorComCliMessage");
                    errorMessageParameter.attrType = ATTR_STRINGT;
                    errorMessageParameter.attrValues = reinterpret_cast<void *>(errMsg);
                    std::vector<ACS_APGCC_AdminOperationParamType> outParameteres;
                    outParameteres.push_back(errorMessageParameter);
                    result = adminOperationResult(oiHandle, invocation, 30, outParameteres);
                    return;
                }
                result = adminOperationResult(oiHandle, invocation, 1);
                if (ACS_CC_FAILURE == result)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Action callback failed");
                }
                break;
            case ACS_XBRM_UTILITY::ALL:
                backupCategory = ACS_XBRM_UTILITY::ON_DEMAND;
                systemBackupType = ACS_XBRM_UTILITY::NODE_TYPE;
                for (int i = 0; i < 6; i++)
                {
                    backupType[i] = availableDomains[i];
                }
                ACS_XBRM_LOG(LOG_LEVEL_WARN, "Generating all System Backups");
                systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME);
                if (systemBackupsHandle >= 0)
                {
                    if (ACS_XBRM_Event::SetEvent(systemBackupsHandle)){
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "SetEvent for %s successful", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                        ACS_XBRM_TRACE_MESSAGE("SetEvent for %s successful", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                    }else{
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SetEvent for %s failed", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                        ACS_XBRM_TRACE_MESSAGE("SetEvent for %s failed", ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
                    }
                }
                result = adminOperationResult(oiHandle, invocation, 1);
                if (ACS_CC_FAILURE == result)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "adminOperationResult() callback failed");
                    ACS_XBRM_TRACE_MESSAGE("adminOperationResult() callback failed");
                }
                break;
            default:
                break;
            }
        }
    }
}

ACE_INT32 ACS_XBRM_BrmHandler::shutdown(){
    ACS_XBRM_TRACE_FUNCTION;
    if(m_isObjImplAdded == true){
        try{
            if(theBrmHandler != 0){
                for(int i = 0; i < 10; i++){
                    if(theBrmHandler->removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE){
                        //Trace
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "BrmHandler removeClassImpl() failed");
                        ACS_XBRM_TRACE_MESSAGE("BrmHandler removeClassImpl() failed");
                        sleep(1);
                    }
                    else{
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "BrmHandler removeClassImpl() successful");
                        ACS_XBRM_TRACE_MESSAGE("BrmHandler removeClassImpl() successful");
                        m_isObjImplAdded = false;
                        break;
                    }
                }
            }
        }catch(...){

        }
    }
    

    if(theReportReactorPtr != 0){
        theReportReactorPtr->end_reactor_event_loop();
    }

    if(utilities != NULL){
        delete utilities;
        utilities = NULL;
    }

    return m_isObjImplAdded ? -1 : 0;
}


string ACS_XBRM_BrmHandler::getBackupTypeName(ACS_XBRM_UTILITY::BackupType backupType){
    switch (backupType)
    {
        case ACS_XBRM_UTILITY::APG          : return "AP";
        case ACS_XBRM_UTILITY::CP           : return "CP";
        case ACS_XBRM_UTILITY::CP1          : return "CP1";
        case ACS_XBRM_UTILITY::CP2          : return "CP2";
        case ACS_XBRM_UTILITY::CPCLUSTER    : return "CPCLUSTER";
        
        default:
            break;
    }
    return "";
}

ACS_CC_ReturnType ACS_XBRM_BrmHandler::augmentedReportProgressCallback(ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandle, string parentDn)
{
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;

    //augmented modify reportProgress
    vector<ACS_CC_ValuesDefinitionType> progressAttrList;
    ACS_CC_ValuesDefinitionType rdnAttr;
    ACS_CC_ValuesDefinitionType actionNameAttr;
    ACS_CC_ValuesDefinitionType additionalInfoAttr;
    ACS_CC_ValuesDefinitionType progressInfoAttr;
    ACS_CC_ValuesDefinitionType progressPercentageAttr;
    ACS_CC_ValuesDefinitionType resultAttr;
    ACS_CC_ValuesDefinitionType resultInfoAttr;
    ACS_CC_ValuesDefinitionType stateAttr;
    ACS_CC_ValuesDefinitionType actionIdAttr;
    ACS_CC_ValuesDefinitionType timeActionStartedAttr;
    ACS_CC_ValuesDefinitionType timeActionCompletedAttr;
    ACS_CC_ValuesDefinitionType timeOfLastStatusUpdateAttr;

    char *idEqOne = const_cast<char *>("id=1");
    void *value1[1] = {reinterpret_cast<void *>(idEqOne)};

    char rdnAttrName[] = "id";
    rdnAttr.attrName = rdnAttrName;
    rdnAttr.attrType = ATTR_STRINGT;
    rdnAttr.attrValuesNum = 1;
    rdnAttr.attrValues = value1;

    actionNameAttr.attrName = (char *)"actionName";
    actionNameAttr.attrType = ATTR_STRINGT;
    actionNameAttr.attrValuesNum = 0;
    actionNameAttr.attrValues = 0;

    additionalInfoAttr.attrName = (char *)"additionalInfo";
    additionalInfoAttr.attrType = ATTR_STRINGT;
    additionalInfoAttr.attrValuesNum = 1;
    char* additionalInfo = (char *)"";
    void *additionalInfoValues[1] = {reinterpret_cast<void *>(additionalInfo)};
    additionalInfoAttr.attrValues = additionalInfoValues;

    progressInfoAttr.attrName = (char *)"progressInfo";
    progressInfoAttr.attrType = ATTR_STRINGT;
    progressInfoAttr.attrValuesNum = 0;
    progressInfoAttr.attrValues = 0;

    progressPercentageAttr.attrName = (char *)"progressPercentage";
    progressPercentageAttr.attrType = ATTR_UINT32T;
    progressPercentageAttr.attrValuesNum = 0;
    progressPercentageAttr.attrValues = 0;

    resultAttr.attrName = (char *)"result";
    resultAttr.attrType = ATTR_INT32T;
    resultAttr.attrValuesNum = 1;
    int NOT_AVAILABLE = 3;
    void *resultAttrValues[1] = {reinterpret_cast<void *>(&NOT_AVAILABLE)};
    resultAttr.attrValues = resultAttrValues;

    resultInfoAttr.attrName = (char *)"resultInfo";
    resultInfoAttr.attrType = ATTR_STRINGT;
    resultInfoAttr.attrValuesNum = 1;
    char* resultInfo = (char *)"";
    void *resultInfoValues[1] = {reinterpret_cast<void *>(resultInfo)};
    resultInfoAttr.attrValues = resultInfoValues;

    stateAttr.attrName = (char *)"state";
    stateAttr.attrType = ATTR_INT32T;
    stateAttr.attrValuesNum = 1;
    int state = 3;
    void *stateValues[1] = {reinterpret_cast<void *>(&state)};
    stateAttr.attrValues = stateValues;

    actionIdAttr.attrName = (char *)"actionId";
    actionIdAttr.attrType = ATTR_UINT32T;
    actionIdAttr.attrValuesNum = 0;
    actionIdAttr.attrValues = 0;

    timeActionStartedAttr.attrName = (char *)"timeActionStarted";
    timeActionStartedAttr.attrType = ATTR_STRINGT;
    timeActionStartedAttr.attrValuesNum = 1;
    char *timeActionStarted = (char *)"";
    void* timeActionStartedValues[1] = {reinterpret_cast<void *>(timeActionStarted)};
    timeActionStartedAttr.attrValues = timeActionStartedValues;

    timeActionCompletedAttr.attrName = (char *)"timeActionCompleted";
    timeActionCompletedAttr.attrType = ATTR_STRINGT;
    timeActionCompletedAttr.attrValuesNum = 1;
    char *timeActionCompleted = (char *)"";
    void* timeActionCompletedValues[1] = {reinterpret_cast<void *>(timeActionCompleted)};
    timeActionCompletedAttr.attrValues = timeActionCompletedValues;

    timeOfLastStatusUpdateAttr.attrName = (char *)"timeOfLastStatusUpdate";
    timeOfLastStatusUpdateAttr.attrType = ATTR_STRINGT;
    timeOfLastStatusUpdateAttr.attrValuesNum = 1;
    char *timeOfLastStatusUpdate = (char *)"";
    void* timeOfLastStatusUpdateValues[1] = {reinterpret_cast<void *>(timeOfLastStatusUpdate)};
    timeOfLastStatusUpdateAttr.attrValues = timeOfLastStatusUpdateValues;

    progressAttrList.push_back(rdnAttr);
    progressAttrList.push_back(actionNameAttr);
    progressAttrList.push_back(additionalInfoAttr);
    progressAttrList.push_back(progressInfoAttr);
    progressAttrList.push_back(progressPercentageAttr);
    progressAttrList.push_back(resultAttr);
    progressAttrList.push_back(resultInfoAttr);
    progressAttrList.push_back(stateAttr);
    progressAttrList.push_back(actionIdAttr);
    progressAttrList.push_back(timeActionStartedAttr);
    progressAttrList.push_back(timeActionCompletedAttr);
    progressAttrList.push_back(timeOfLastStatusUpdateAttr);


    if(ACS_CC_SUCCESS == createObjectAugmentCcb(ccbHandle, adminOwnerHandle, ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_ASYNC_ACTION_PROGRESS_NAME.c_str(), parentDn.c_str(), progressAttrList)){
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "reportProgress Object for SystemBrm created successfully");
        ACS_XBRM_TRACE_MESSAGE("reportProgress Object for SystemBrm created successfully");
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "reportProgress Object for SystemBrm creation failed");
        ACS_XBRM_TRACE_MESSAGE("reportProgress Object for SystemBrm creation failed");
        return ACS_CC_FAILURE;
    }

    //augmented modify reportProgress
    ACS_CC_ImmParameter reportProgress;
    reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_REPORT_PROGRESS_NAME.c_str());
    reportProgress.attrType = ATTR_NAMET;
    reportProgress.attrValuesNum = 1;
    char reportProgressDn[100];
    sprintf(reportProgressDn, "id=1,%s", parentDn.c_str());
    void* values[1] = {reinterpret_cast<void *>(const_cast<char *>(reportProgressDn))};
    reportProgress.attrValues = values;

    if(ACS_CC_SUCCESS == modifyObjectAugmentCcb(ccbHandle, adminOwnerHandle, parentDn.c_str(), &reportProgress)){
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "reportProgress of SystemBrm modified successfully");
        ACS_XBRM_TRACE_MESSAGE("reportProgress of SystemBrm modified successfully");
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "reportProgress of SystemBrm modification failed");
        ACS_XBRM_TRACE_MESSAGE("reportProgress of SystemBrm modification failed");
        return ACS_CC_FAILURE;
    }

    return result;
}

int ACS_XBRM_BrmHandler::getReportProgressState(){
    char *progressReportDn = (char *)"id=1,SystemBrMsystemBrMId=1"; //hard coded
    ACS_CC_ImmParameter reportProgress;
    reportProgress.attrName = (char *)"state";
    immHandle.getAttribute(progressReportDn, &reportProgress);
    if(reportProgress.attrValuesNum)
        return *reinterpret_cast<int *>(reportProgress.attrValues[0]);
    else
        return -1;
}

string ACS_XBRM_BrmHandler::getActionTriggeringTime(){
    return actionTriggeringTime;
}

void ACS_XBRM_BrmHandler::setActionTriggeringTime(){
    actionTriggeringTime = utilities->getCurrentTime();
}

bool ACS_XBRM_BrmHandler::isBackupOngoing()
{
    bool m_ongoing = false;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    // check if any manual action or Scheduled Backup event is in execution

    ACS_CC_ImmParameter reportProgress;
    int onDemandBackupStatus = 0, schedulerBackupStatus = 0;
    reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str());

    if (ACS_CC_FAILURE == immHandle.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], &reportProgress))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "IMM Failed to fetch state attribute of reportProgress for SystemBrM");
        ACS_XBRM_TRACE_MESSAGE("IMM Failed to fetch state attribute of reportProgress for SystemBrM");
    }
    else
    {
        if (reportProgress.attrValuesNum)
        {
            onDemandBackupStatus = *reinterpret_cast<int *>(reportProgress.attrValues[0]);           
        }
    }
    std::vector<std::string> schedulerObjNames;
    result = immHandle.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME.c_str(), schedulerObjNames);
    if(result != ACS_CC_SUCCESS)
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO,"Failed to fetch class instances for Scheduler class");
        ACS_XBRM_TRACE_MESSAGE("Failed to fetch class instances for Scheduler class");
    }
    if (schedulerObjNames.size())
    {
        if (ACS_CC_FAILURE == immHandle.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], &reportProgress))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "IMM Failed to fetch state attribute of reportProgress for Scheduler");
            ACS_XBRM_TRACE_MESSAGE("IMM Failed to fetch state attribute of reportProgress for Scheduler");
        }
        else
        {
            if (reportProgress.attrValuesNum)
            {
                schedulerBackupStatus = *reinterpret_cast<int *>(reportProgress.attrValues[0]);            
            }
        }
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO,"Backup ongoing check , onDemandBackupStatus : %d, schedulerBackupStatus : %d", onDemandBackupStatus, schedulerBackupStatus);
    ACS_XBRM_TRACE_MESSAGE("Backup ongoing check , onDemandBackupStatus : %d, schedulerBackupStatus : %d", onDemandBackupStatus, schedulerBackupStatus);
    if ((onDemandBackupStatus == ACS_XBRM_UTILITY::RUNNING) || (schedulerBackupStatus == ACS_XBRM_UTILITY::RUNNING))
    {
        m_ongoing = true;
    }
    return m_ongoing;
}
