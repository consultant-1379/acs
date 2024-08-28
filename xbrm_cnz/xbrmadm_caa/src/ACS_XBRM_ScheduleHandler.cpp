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
 * @file ACS_XBRM_ScheduleHandler.cpp
 *
 * @brief
 * ACS_XBRM_ScheduleHandler Class for SystemBrmBackupScheduler IMM Handling
 *
 * @details
 * Implementation of ACS_XBRM_ScheduleHandler Class for handling IMM
 * callbacks for SystemBrmBackupScheduler MOC
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
#include "ACS_XBRM_ScheduleHandler.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "ACS_XBRM_ImmUtlis.h"
#include "ACS_XBRM_Event.h"
#include "ACS_XBRM_Utilities.h"

#include <ACS_TRA_trace.h>

#include <string>
using namespace std;

// bool updateNextScheduledTime;
bool updateAdminState;

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_ScheduleHandler);

ACS_XBRM_ScheduleHandler::ACS_XBRM_ScheduleHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope) 
:acs_apgcc_objectimplementereventhandler_V3(ClassName, p_impName, p_scope), 
theClassName(ClassName){
    //TRACE
    ACS_XBRM_TRACE_FUNCTION;
    m_isObjImplAdded = false;   
    theReportReactorImplPtr = 0;
    theReportReactorImplPtr = new ACE_TP_Reactor();

    createCallBack = false;
    deleteCallBack = false;
    modifyAdminStateCallBack = false;

    if(theReportReactorImplPtr != 0){
        theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
    }
}

ACS_XBRM_ScheduleHandler::ACS_XBRM_ScheduleHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope, ACE_Thread_Manager *aThrmgr)
:acs_apgcc_objectimplementereventhandler_V3(ClassName, p_impName, p_scope), 
theClassName(ClassName){
    //TRACE
    ACS_XBRM_TRACE_FUNCTION;
    this->thr_mgr(aThrmgr);
    m_isObjImplAdded = false; 
    theReportReactorImplPtr = 0;
    theReportReactorImplPtr = new ACE_TP_Reactor();

    createCallBack = false;
    deleteCallBack = false;
    modifyAdminStateCallBack = false;
    
    if(theReportReactorImplPtr != 0){
        theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
    }
}

ACS_XBRM_ScheduleHandler::~ACS_XBRM_ScheduleHandler(){
    ACS_XBRM_TRACE_FUNCTION;
    if(ACS_CC_SUCCESS == immHandle.Finalize()){
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "ACS_XBRM_ScheduleHandler immHandle is finalized");
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "ACS_XBRM_ScheduleHandler immHandle is finalization failed");
    }

    if(theReportReactorPtr != 0){
        delete theReportReactorPtr;
        theReportReactorPtr = 0;
    }

    if(theReportReactorImplPtr != 0){
        delete theReportReactorImplPtr;
        theReportReactorImplPtr = 0;
    }

    if(theScheduleHandler != 0){
        delete theScheduleHandler;
        theScheduleHandler = 0;
    }
}

ACE_INT32 ACS_XBRM_ScheduleHandler::svc(){
    ACS_XBRM_TRACE_FUNCTION;
    //TRACE
    theScheduleHandler = 0;
    theScheduleHandler = new acs_apgcc_oihandler_V3();
    utilities = new ACS_XBRM_Utilities();
    try{
        if(ACS_CC_SUCCESS != immHandle.Init()){
            //TRACE
            ACS_XBRM_LOG(LOG_LEVEL_WARN, "IMM Handler initialization failed");
            return -1;
        }
        if(theScheduleHandler != 0){
            for(int i = 0; i < 10; i++){
                if(ACS_CC_FAILURE == theScheduleHandler->addClassImpl(this, theClassName.c_str())){
                    ACS_XBRM_LOG(LOG_LEVEL_WARN, "Schedule Handler addClassImpl() failed");
                    sleep(1);
                }
                else{
                    //TRACE
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Schedule Handler addClassImpl() successful");
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
            ACS_XBRM_LOG(LOG_LEVEL_WARN, "Schedule Handler Reactor pointer failed to initialize ::open()");
            ACS_XBRM_TRACE_MESSAGE("Schedule Handler Reactor pointer failed to initialize ::open()");
            return -1;
        }
        dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
        theReportReactorPtr->run_reactor_event_loop();
    }
    return 0;
}

ACS_CC_ReturnType ACS_XBRM_ScheduleHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char *parentName, ACS_APGCC_AttrValues **attr){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    //TRACE
    (void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;
    string schedulerDn = "";
    for(int i = 0; attr[i] != NULL; i++){
        if(0 == ACE_OS::strcmp("systemBrmBackupSchedulerId", attr[i]->attrName)){
            schedulerDn.assign(reinterpret_cast<char *>(attr[i]->attrValues[0]));
            schedulerDn += ",SystemBrMsystemBrMId=1"; //suffix hardcoded
            
            if(0 != ACE_OS::strcmp(reinterpret_cast<char *>(attr[i]->attrValues[0]), "systemBrmBackupSchedulerId=1")){
                string errorTxt = "Invalid value for keyAttribute. Value must be '1'.\n";
                this->setExitCode(17, errorTxt);
                return ACS_CC_FAILURE;
            }
        }
    }

    ACS_APGCC_CcbHandle ccbHandle;
    ACS_APGCC_AdminOwnerHandle adminOwnerHandle;
    getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandle, &adminOwnerHandle);

    //augmented callback for reportProgress
    augmentedReportProgressCallback(ccbHandle, adminOwnerHandle, schedulerDn);

    applyAugmentCcb(ccbHandle);

    createCallBack = true;
    return result;
}

ACS_CC_ReturnType ACS_XBRM_ScheduleHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    //TRACE
    (void)oiHandle;
	(void)ccbId;
	(void)objName;

    //check if any manual action is in execution and adminState is locked
    ACS_CC_ImmParameter scheduler;
    scheduler.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADMIN_STATE_NAME.c_str());
    int state = 2;
    int adminState = 1;
    ACS_CC_ImmParameter reportProgress;
    reportProgress.attrName = (char *)"state";

    if(ACS_CC_FAILURE == immHandle.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], &reportProgress)){
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "IMM Failed to fetch state attribute of scheduler reportProgress");
        ACS_XBRM_TRACE_MESSAGE("IMM Failed to fetch state attribute of scheduler reportProgress");
        return ACS_CC_FAILURE;
    }else{
        state = *reinterpret_cast<int *>(reportProgress.attrValues[0]);
    }
    
    if(ACS_CC_FAILURE == immHandle.getAttribute(objName, &scheduler)){
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "IMM Failed to fetch state attribute of scheduler");
        ACS_XBRM_TRACE_MESSAGE("IMM Failed to fetch adminState attribute of scheduler");
        return ACS_CC_FAILURE;
    }else{
        adminState = *reinterpret_cast<int *>(scheduler.attrValues[0]);
    }
    
    if(ACS_XBRM_UTILITY::RUNNING == state || ACS_XBRM_UTILITY::UNLOCKED == adminState){
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "Cannot delete while System Backups Scheduling is activated.");
        ACS_XBRM_TRACE_MESSAGE("Cannot delete while System Backups Scheduling is activated.");
        string errorTxt = "System Backups Scheduler is not LOCKED (or) System Backups Operation is ongoing.\n";
        this->setExitCode(17, errorTxt);
        return ACS_CC_FAILURE;
    }
    
    deleteCallBack = true;
    return result;
}

ACS_CC_ReturnType ACS_XBRM_ScheduleHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    //TRACE
    (void)oiHandle;
    (void)ccbId;
    (void)objName;
    // (void)attrMods;
    for(int i = 0; attrMods[i] != NULL; i++){
        ACS_APGCC_AttrValues modifyAttribute = attrMods[i]->modAttr;
        if(0 == ACE_OS::strcmp(ACS_XBRM_ImmMapper::ATTR_ADMIN_STATE_NAME.c_str(), modifyAttribute.attrName)){
            ACS_XBRM_LOG(LOG_LEVEL_WARN, "adminState in Scheduler MO is updated");
            modifyAdminStateCallBack = true;
        }
    }
    return result;
}

ACS_CC_ReturnType ACS_XBRM_ScheduleHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    //TRACE
    (void)oiHandle;
    (void)ccbId;
    return result;
}

void ACS_XBRM_ScheduleHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
    ACS_XBRM_TRACE_FUNCTION;
    //TRACE
    (void)oiHandle;
    (void)ccbId;
}

void ACS_XBRM_ScheduleHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
    ACS_XBRM_TRACE_FUNCTION;
    //TRACE
    (void)oiHandle;
    (void)ccbId;

    if(createCallBack){
        ACS_XBRM_EventHandle startSchedulingEvent = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_EVENT_SCHEDULING_START_NAME);
        ACS_XBRM_Event::SetEvent(startSchedulingEvent);

        createCallBack = false;
    }else if(deleteCallBack){
        ACS_XBRM_EventHandle stopSchedulingEvent = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_EVENT_SCHEDULING_END_NAME);
        ACS_XBRM_Event::SetEvent(stopSchedulingEvent);

        deleteCallBack = false;
    }else if(modifyAdminStateCallBack){
        updateAdminState = true;

        modifyAdminStateCallBack = false;
    }
}

ACS_CC_ReturnType ACS_XBRM_ScheduleHandler::updateRuntime(const char *objName, const char **attrName){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    //TRACE  
    (void)objName;
    (void)attrName;
    return result;
}

void ACS_XBRM_ScheduleHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
                            const char *p_objName, ACS_APGCC_AdminOperationIdType operationId, 
                            ACS_APGCC_AdminOperationParamType **paramList)
{
    ACS_XBRM_TRACE_FUNCTION;
    (void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;
}

ACE_INT32 ACS_XBRM_ScheduleHandler::shutdown(){
    ACS_XBRM_TRACE_FUNCTION;
    if(m_isObjImplAdded == true){
        try{
            if(theScheduleHandler != 0){
                for(int i = 0; i < 10; i++){
                    if(theScheduleHandler->removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE){
                        //Trace
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Schedule Handler removeClassImpl() failed");
                        ACS_XBRM_TRACE_MESSAGE("Schedule Handler removeClassImpl() failed");
                        sleep(1);
                    }
                    else{
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Schedule Handler removeClassImpl() successful");
                        ACS_XBRM_TRACE_MESSAGE("Schedule Handler removeClassImpl() successful");
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

ACS_CC_ReturnType ACS_XBRM_ScheduleHandler::augmentedReportProgressCallback(ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandle, string parentDn)
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
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "reportProgress Object for SystemBrmScheduler created successfully");
        ACS_XBRM_TRACE_MESSAGE("reportProgress Object for SystemBrmScheduler created successfully");
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "reportProgress Object for SystemBrmScheduler creation failed");
        ACS_XBRM_TRACE_MESSAGE("reportProgress Object for SystemBrmScheduler creation failed");
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
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "reportProgress of SystemBrmScheduler modified successfully");
        ACS_XBRM_TRACE_MESSAGE("reportProgress of SystemBrmScheduler modified successfully");
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "reportProgress of SystemBrmScheduler modification failed");
        ACS_XBRM_TRACE_MESSAGE("reportProgress of SystemBrmScheduler modification failed");
        return ACS_CC_FAILURE;
    }

    return result;
}
