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
* @file
*
* @brief
* ACS_XBRM_Server Class for Service Implementation
*
* @details
* Implementation of ACS_XBRM_Server Class to handle service initialization/
* running/cleanup
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

#include "ACS_XBRM_Server.h"
#include "ACS_XBRM_Event.h"
#include "ACS_XBRM_ImmUtlis.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "ACS_XBRM_SystemBackupHandler.h"

#include <fstream>
#include <boost/filesystem.hpp>

bool isBackupAlarm = false;


ACS_XBRM_TRACE_DEFINE(ACS_XBRM_Server);

ACS_XBRM_Server::ACS_XBRM_Server(){
    // use util to use constant of XBRM_Util::EVENT_NAME_SHUTDOWN
    shutdownEvent = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SHUTDOWN_NAME);
    systemBackupsStartEvent = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME);
    systemBackupsEndEvent = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
    eventSchedulingStartEvent = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_EVENT_SCHEDULING_START_NAME);
    eventSchedulingEndEvent = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_EVENT_SCHEDULING_END_NAME);
}

ACS_XBRM_Server::~ACS_XBRM_Server(){
}

ACS_CC_ReturnType ACS_XBRM_Server::startWorkerThreads(){
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Starting worker threads");   
    // TRACE
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    bool handleRestore = isRestartAfterRestore();
    std::vector<std::string> reportProgressDnNames;
    if (ACS_CC_FAILURE == setupSystemBrmCallBacks()){
        // TRACE
        syslog(LOG_INFO, "setupSystemBrmCallBacks() returned failure");
        result = ACS_CC_FAILURE;
    }else{
        std::vector<std::string> brmObjNames;
        immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_NAME.c_str(), brmObjNames);
        immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_ASYNC_ACTION_PROGRESS_NAME.c_str(), reportProgressDnNames);
        if (brmObjNames.size()){
            //service restart due to restore
            bool reportProgressExist = false;
            for(int i = 0; i < (int)reportProgressDnNames.size(); i++){
                if(0 == ACE_OS::strcmp(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], reportProgressDnNames[i].c_str())){
                    reportProgressExist = true;
                    break;
                }
            }

            if(!reportProgressExist){
                /* create reportProgress */
                createReportProgress(brmObjNames[0]);
            }
            if(handleRestore){
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "Handling service restart after restore");
                int intVal = 3;
                void *values[1] = {reinterpret_cast<void *>(&intVal)};
                utilities->resetReportProgress(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND]);
               

                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

                values[0] = reinterpret_cast<void *>((char *)"");

                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

            }else{
                if (alarmHandler->readAlarmFile() && !isBackupAlarm) // Reading alarmEntry from /data/acs/data/xbrm/alarmFile
                {
                    alarmHandler->initAlarm();
                    isBackupAlarm = true;
                }
                //service restart due to restart
                ACS_CC_ImmParameter reportProgress;
                reportProgress.attrName = (char *)"state";
                immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], &reportProgress);
                if (reportProgress.attrValuesNum){
                    if(ACS_XBRM_UTILITY::RUNNING == *reinterpret_cast<int *>(reportProgress.attrValues[0])){
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Handling service shutdown during backup ongoing");
                        int intVal = ACS_XBRM_UTILITY::FAILURE;
                        void* values[1] = {reinterpret_cast<void *>(&intVal)};
                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);
                        
                        values[0] = reinterpret_cast<void *>((char *)"Operation cancelled due to System/Service restart");
                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);
                        
                        intVal = ACS_XBRM_UTILITY::CANCELLED;
                        values[0] = reinterpret_cast<void *>(&intVal);
                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);
                        
                        intVal = 100; //progressPercentage
                        values[0] = reinterpret_cast<void *>(&intVal);
                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

                        reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str());
                        immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], &reportProgress);
                        int len = reportProgress.attrValuesNum + 2;
                        void* info[len];
                        for(int i = 0; i < len - 2; i++)
                            info[i] = reportProgress.attrValues[i];
                        info[len - 2] = reinterpret_cast<void *>((char *)"[Fail Over Recovery Mode]");
                        info[len - 1] = reinterpret_cast<void *>((char *)"CREATE/EXPORT of System Backup : Failed");
                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

                        string curTime = ACS_XBRM_Utilities::getCurrentTime();
                        values[0] = reinterpret_cast<void *>(const_cast<char *>(curTime.c_str()));
                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);
                        
                        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);
                    }
                }
            }
        }
    }

    if (ACS_CC_FAILURE == setupSystemBrmBackupSchedulerCallBacks()){
        // TRACE
        syslog(LOG_INFO, "setupSystemBrmBackupSchedulerCallBacks() returned failure");
        result = ACS_CC_FAILURE;
    }else{
        std::vector<std::string> schedulerObjNames;
        immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME.c_str(), schedulerObjNames);
        immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_ASYNC_ACTION_PROGRESS_NAME.c_str(), reportProgressDnNames);
        if(schedulerObjNames.size()){
            //for restart scenario
            bool reportProgressExist = false;
            for(int i = 0; i < (int)reportProgressDnNames.size(); i++){
                if(0 == ACE_OS::strcmp(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], reportProgressDnNames[i].c_str())){
                    reportProgressExist = true;
                    break;
                }
            }

            if(!reportProgressExist){
                /* create reportProgress */
                createReportProgress(schedulerObjNames[0]);
            }
            // startEventScheduler();
            if(handleRestore){
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "Handling service restart after restore");
                int intVal = 3;
                void *values[1] = {reinterpret_cast<void *>(&intVal)};
                
                utilities->resetReportProgress(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED]);

                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

                values[0] = reinterpret_cast<void *>((char *)"");
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

                ACS_CC_ImmParameter schedulerMO;

                schedulerMO.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_NEXT_SCHEDULED_EVENT_TIME_NAME.c_str());

                schedulerMO.attrType = ATTR_STRINGT;

                schedulerMO.attrValuesNum = 1;

                schedulerMO.attrValues = values;
                if (ACS_CC_FAILURE == immHandler.modifyAttribute(schedulerObjNames[0].c_str(), &schedulerMO))
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Failed to reset %s of %s MO", schedulerMO.attrName, schedulerObjNames[0].c_str());

                utilities->updateNextScheduledEventTime();
                startEventScheduler();
              
            }else{
                utilities->updateNextScheduledEventTime();
                startEventScheduler();
                ACS_CC_ImmParameter reportProgress;
                reportProgress.attrName = (char *)"state";
                immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], &reportProgress);
                if (reportProgress.attrValuesNum){
                    if(ACS_XBRM_UTILITY::RUNNING == *reinterpret_cast<int *>(reportProgress.attrValues[0])){
                        ACS_CC_ImmParameter scheduler;
                        scheduler.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADMIN_STATE_NAME.c_str());
                        immHandler.getAttribute("systemBrmBackupSchedulerId=1,SystemBrMsystemBrMId=1", &scheduler);
                        if(scheduler.attrValuesNum && (*reinterpret_cast<int *>(scheduler.attrValues[0]) == ACS_XBRM_UTILITY::UNLOCKED)){
                      
                            /* During startup if scheduled event failed/running we can trigger */
                            ACS_XBRM_LOG(LOG_LEVEL_INFO, "Restarting Scheduled System Backups");
                            systemBackupType = ACS_XBRM_UTILITY::NODE_TYPE;
                            backupCategory = ACS_XBRM_UTILITY::SCHEDULED;
                            ACS_CS_API_CommonBasedArchitecture::ApgNumber APtype;
                            ACS_CS_API_NetworkElement::getApgNumber(APtype);
                            bool isMultipleCPSystem;
                            uint32_t cpCount;
                            ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);
                            ACS_CS_API_NetworkElement::getDoubleSidedCPCount(cpCount);
                            backupType[ACS_XBRM_UTILITY::APG] = 1;
                            backupType[ACS_XBRM_UTILITY::ALL] = 1;
                            if (ACS_CS_API_CommonBasedArchitecture::AP2 != APtype)
                            {
                                if (isMultipleCPSystem)
                                {
                                    // blade cluster
                                    backupType[ACS_XBRM_UTILITY::CPCLUSTER] = 1;
                                    if (cpCount == 2)
                                    {
                                        backupType[ACS_XBRM_UTILITY::CP1] = 1;
                                        backupType[ACS_XBRM_UTILITY::CP2] = 1;
                                    }
                                    else if (cpCount == 1)
                                    {
                                        backupType[ACS_XBRM_UTILITY::CP1] = 1;
                                    }
                                }
                                else
                                {
                                    // singleCp system
                                    backupType[ACS_XBRM_UTILITY::CP] = 1;
                                }
                            }
                            ACS_XBRM_EventHandle systemBackupsHandle;
                            systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME);
                            ACS_XBRM_Event::SetEvent(systemBackupsHandle);
                        }else{
                            //restart case if adminState LOCKED or unable to fetch adminState value
                            utilities->resetReportProgress(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED]);
                        }
                    }
                }
            }
        }
    }

    if(ACS_CC_FAILURE == setupSystemBrmPeriodicEventCallBacks()){
        // TRACE
        syslog(LOG_INFO, "setupSystemBrmPeriodicEventCallBacks() returned failure");
        result = ACS_CC_FAILURE;
    }
    handleRestore = false;
    return result;
}

ACS_CC_ReturnType ACS_XBRM_Server::stopWorkerThreads()
{
    ACS_XBRM_TRACE_FUNCTION;
    // TRACE
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Stopping Worker Threads");

    if (periodicEventHandler != NULL)
    {
        if (-1 == periodicEventHandler->shutdown())
        {
            // TRACE syslog
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SystemBrmPeriodicEvent handler thread shutdown() failed");
            return ACS_CC_FAILURE;
        }
        periodicEventHandler->wait();
    }


    if (scheduleHandler != NULL)
    {
        
        std::vector<std::string> schedulerObjNames;
        immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME.c_str(), schedulerObjNames);
        if (schedulerObjNames.size()){
            stopEventScheduler();
        }
        if (-1 == scheduleHandler->shutdown())
        {
            // TRACE syslog
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SystemBrmBackupScheduler handler thread shutdown() failed");
            return ACS_CC_FAILURE;
        }
        scheduleHandler->wait();
    }

    if (brmHandler != NULL)
    {
        std::vector<std::string> brmObjNames;
        immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_NAME.c_str(), brmObjNames);
        if (brmObjNames.size())
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "Handling service shutdown for SystemBrM OI");
            ACS_CC_ImmParameter reportProgress;
            reportProgress.attrName = (char *)"state";
            immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], &reportProgress);
            if (reportProgress.attrValuesNum)
                if(ACS_XBRM_UTILITY::RUNNING == *reinterpret_cast<int *>(reportProgress.attrValues[0])){
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Handling service graceful shutdown during backup ongoing");
                    int intVal = ACS_XBRM_UTILITY::FAILURE;
                    void *values[1] = {reinterpret_cast<void *>(&intVal)};

                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

                    values[0] = reinterpret_cast<void *>((char *)"Operation cancelled due to System/Service restart");
                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

                    intVal = ACS_XBRM_UTILITY::CANCELLED;
                    values[0] = reinterpret_cast<void *>(&intVal);
                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

                    intVal = 100; // progressPercentage
                    values[0] = reinterpret_cast<void *>(&intVal);
                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

                    reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str());
                    immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], &reportProgress);
                    int len = reportProgress.attrValuesNum + 2;
                    void *info[len];
                    for (int i = 0; i < len - 2; i++)
                        info[i] = reportProgress.attrValues[i];
                    info[len - 2] = reinterpret_cast<void *>((char *)"[Fail Over Recovery Mode]");
                    info[len - 1] = reinterpret_cast<void *>((char *)"CREATE/EXPORT of System Backup : Failed");
                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);
                    
                    string curTime = ACS_XBRM_Utilities::getCurrentTime();
                    values[0] = reinterpret_cast<void *>(const_cast<char *>(curTime.c_str()));
                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

                    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);
                }
        }
        if (-1 == brmHandler->shutdown())
        {
            // TRACE syslog
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SystemBrm handler thread shutdown() failed");
            return ACS_CC_FAILURE;
        }
        brmHandler->wait();
    }
    return ACS_CC_SUCCESS;
}

int ACS_XBRM_Server::svc()
{
    ACS_XBRM_TRACE_FUNCTION;
    utilities = new ACS_XBRM_Utilities();
    alarmHandler = new ACS_XBRM_AlarmHandler();
    if (ACS_CC_FAILURE == immHandler.Init())
    {
        // TRACE
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Initialization of immHandle is failed");
        return ACS_CC_FAILURE;
    }
    if (ACS_CC_SUCCESS == startWorkerThreads())
    {
        
        ACS_XBRM_EventHandle *eventHandlesArray = new ACS_XBRM_EventHandle[5];
        eventHandlesArray[0] = shutdownEvent;
        eventHandlesArray[1] = systemBackupsStartEvent;
        eventHandlesArray[2] = systemBackupsEndEvent;
        eventHandlesArray[3] = eventSchedulingStartEvent;
        eventHandlesArray[4] = eventSchedulingEndEvent;
        bool operationOngoing = false;
        bool shutdown =  false;
        ACS_XBRM_SystemBackupHandler* sbh;
        while(true){
            int eventIndex = ACS_XBRM_Event::WaitForEvents(5, eventHandlesArray, 0);
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "event Index = %d", eventIndex);
            switch (eventIndex)
            {
                case 0:
                ACS_XBRM_Event::ResetEvent(shutdownEvent);
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Shut Down Event received");
                    shutdown = true;
                    if(operationOngoing){
                        sbh->stop();
                        sbh->wait();
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "sbh->stop() success");
                    }
                    break;
                case 1:
                    //system backups
                    ACS_XBRM_Event::ResetEvent(systemBackupsStartEvent);
                    for(int i = 0; i < 6; i++){
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "extern backupType[%d] = %d", i, backupType[i]);
                    }
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "extern backupCategory = %d", backupCategory);
                    if(operationOngoing){
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "System Backups Started Event received but another process ongoing");
                        break;
                    }
                    sbh = new ACS_XBRM_SystemBackupHandler(backupType, (ACS_XBRM_UTILITY::BackupCategory)backupCategory, alarmHandler);
                    sbh->activate();
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "System Backups Started Event received");
                    operationOngoing  = true;
                    break;
                case 2:
                    ACS_XBRM_Event::ResetEvent(systemBackupsEndEvent);
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "System Backups Completed Event received");
                    operationOngoing = false;
                    if(sbh != NULL)
                    {
                        delete sbh;
                        sbh = NULL;
                    }
                    break;
                case 3:
                    ACS_XBRM_Event::ResetEvent(eventSchedulingStartEvent);
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Starting event scheduling");
                    startEventScheduler();
                    break;
                case 4:
                    ACS_XBRM_Event::ResetEvent(eventSchedulingEndEvent);
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Stoping event scheduling");
                    stopEventScheduler();
                    break;
                default:
                    break;
            }

            if(shutdown){
                stopWorkerThreads();
                break;
            }
        }
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to start worker threads");
        if (ACS_CC_FAILURE == immHandler.Finalize())
        {
            // TRACE
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Finalization of immHandle is failed");
            return ACS_CC_FAILURE;
        }
        return ACS_CC_FAILURE;
    }
    if (ACS_CC_FAILURE == immHandler.Finalize())
    {
        // TRACE
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Finalization of immHandle is failed");
        return ACS_CC_FAILURE;
    }
    if (NULL != utilities)
    {
        delete utilities;
        utilities = NULL;
    }
    if(alarmHandler != NULL)
    {
        delete alarmHandler;
        alarmHandler = NULL;
    }
    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_Server::setupSystemBrmCallBacks()
{
    ACS_XBRM_TRACE_FUNCTION;
    brmHandler = NULL;
    brmHandler = new ACS_XBRM_BrmHandler(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_NAME, ACS_XBRM_ImmMapper::IMPL_SYSTEM_BRM_NAME, ACS_APGCC_ONE);
    if (NULL != brmHandler)
    {
        // TRACE
        ACE_INT32 activationResult = brmHandler->activate();
        if (-1 == activationResult)
        {
            // TRACE
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SystemBrm Handler thread failed");
            return ACS_CC_FAILURE;
        }
        else
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "SystemBrm Handler thread started successfully");
    }
    // TRACE
    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_Server::setupSystemBrmBackupSchedulerCallBacks()
{
    ACS_XBRM_TRACE_FUNCTION;
    scheduleHandler = NULL;
    scheduleHandler = new ACS_XBRM_ScheduleHandler(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME, ACS_XBRM_ImmMapper::IMPL_SYSTEM_BRM_BACKUP_SCHEDULER_NAME, ACS_APGCC_ONE);
    if (NULL != scheduleHandler)
    {
        // TRACE
        ACE_INT32 activationResult = scheduleHandler->activate();
        if (-1 == activationResult)
        {
            // TRACE
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SystemBrmBackupScheduler Handler thread failed");
            return ACS_CC_FAILURE;
        }
        else
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "SystemBrmBackupScheduler Handler thread started successfully");
    }
    // TRACE
    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_Server::setupSystemBrmPeriodicEventCallBacks()
{
    ACS_XBRM_TRACE_FUNCTION;
    periodicEventHandler = NULL;
    periodicEventHandler = new ACS_XBRM_PeriodicEventHandler(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_PERIODIC_EVENT_NAME, ACS_XBRM_ImmMapper::IMPL_SYSTEM_BRM_PERIODIC_EVENT_NAME, ACS_APGCC_ONE);
    if (NULL != periodicEventHandler)
    {
        // TRACE
        ACE_INT32 activationResult = periodicEventHandler->activate();
        if (-1 == activationResult)
        {
            // TRACE
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "SystemBrmPeriodicEvent Handler thread failed");
            return ACS_CC_FAILURE;
        }
        else
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "SystemBrmPeriodicEvent Handler thread started successfully");
    }
    // TRACE
    return ACS_CC_SUCCESS;
}

string ACS_XBRM_Server::getClearPath()
{
    string clearPath = "";
    const char PSAClearFilePath[] = "/usr/share/pso/storage-paths/clear";
    int bufferLength;
    ifstream clearFileStream;
    // open the clear file
    clearFileStream.open(PSAClearFilePath, ios::binary);

    // check for open error
    if (clearFileStream.good())
    {
        /**
         * get pointer to end
         * current location of get pointer determining length
         * get pointer to start for reading
         */
        clearFileStream.seekg(0, ios::end);
        bufferLength = clearFileStream.tellg();
        clearFileStream.seekg(0, ios::beg);

        // allocate the buffer
        char buffer[bufferLength + 1];
        ACE_OS::memset(buffer, 0, bufferLength + 1);

        // read data
        clearFileStream.read(buffer, bufferLength);

        if (buffer[bufferLength - 1] == '\n')
            buffer[bufferLength - 1] = 0;

        clearPath = buffer;
        clearPath += '/';
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "clearPath = %s", clearPath.c_str());
    }

    clearFileStream.close();
    return clearPath;
}

bool ACS_XBRM_Server::isRestartAfterRestore()
{
    bool result = false;
    std::string clearPath = getClearPath();
    const char xbrmFolder[] = "acs_xbrm";
    if (0 == ACE_OS::strcmp(clearPath.c_str(), ""))
    {
        // issue in reading clearPath
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "issue in reading clearPath");
    }
    else
    {
        // check restart type
        std::string xbrmClearPath = clearPath + xbrmFolder;
        boost::filesystem::path xbrmClearFolder(xbrmClearPath);
        try
        {
            if (boost::filesystem::exists(xbrmClearFolder))
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "normal restart");
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "restart after restore");
                // create the folder
                boost::filesystem::create_directory(xbrmClearFolder);
                result = true;
            }
        }
        catch (const boost::filesystem::filesystem_error &ex)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to create clearFolder for XBRM");
        }
    }
    return result;
}

//setupCallbackups for scheduler event based
ACS_CC_ReturnType ACS_XBRM_Server::startEventScheduler()
{
    ACS_XBRM_TRACE_FUNCTION;
    eventScheduler = NULL;
    eventScheduler = new ACS_XBRM_EventScheduler();
    if (NULL != eventScheduler)
    {
        // TRACE
        ACE_INT32 activationResult = eventScheduler->activate();
        if (-1 == activationResult)
        {
            // TRACE
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "eventScheduler thread failed");
            return ACS_CC_FAILURE;
        }
        else
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "eventScheduler thread started successfully");
    }
    // TRACE
    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_Server::stopEventScheduler()
{
    ACS_XBRM_TRACE_FUNCTION;

    if(eventScheduler != NULL){
        eventScheduler->stop();
        eventScheduler->wait();
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "eventScheduler thread stopped successfully");
    // TRACE
    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_Server::createReportProgress(string parentDn){
    ACS_CC_ReturnType res = ACS_CC_SUCCESS;

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


    if(ACS_CC_SUCCESS == immHandler.createObject(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_ASYNC_ACTION_PROGRESS_NAME.c_str(), parentDn.c_str(), progressAttrList)){
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "reportProgress Object for %s created successfully", parentDn.c_str());
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "reportProgress Object for %s creation failed", parentDn.c_str());
        return ACS_CC_FAILURE;
    }

    ACS_CC_ImmParameter reportProgress;
    reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_REPORT_PROGRESS_NAME.c_str());
    reportProgress.attrType = ATTR_NAMET;
    reportProgress.attrValuesNum = 1;
    char reportProgressDn[100];
    sprintf(reportProgressDn, "id=1,%s", parentDn.c_str());
    void* values[1] = {reinterpret_cast<void *>(const_cast<char *>(reportProgressDn))};
    reportProgress.attrValues = values;

    if(ACS_CC_SUCCESS == immHandler.modifyAttribute(parentDn.c_str(), &reportProgress)){
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "reportProgress of %s modified successfully", parentDn.c_str());
    }else{
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "reportProgress of %s modification failed", parentDn.c_str());
        return ACS_CC_FAILURE;
    }

    return res;
}