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
* @file ACS_XBRM_SystemBackupHandler.cpp
*
* @brief
* ACS_XBRM_SystemBackupHandler implements ACE Thread to manage System Backups
*
* @details
* ACS_XBRM_SystemBackupHandler
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
 * 2023-06-09  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

#include "ACS_XBRM_SystemBackupHandler.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "ACS_XBRM_Event.h"


ACS_XBRM_TRACE_DEFINE(ACS_XBRM_SystemBackupHandler);

ACS_XBRM_SystemBackupHandler::ACS_XBRM_SystemBackupHandler(int backupType[6], ACS_XBRM_UTILITY::BackupCategory backupCategory, ACS_XBRM_AlarmHandler* alarmHandler): m_alarmHandler(alarmHandler)
{
    ACS_XBRM_TRACE_FUNCTION;
    sigTermReceived = false;
    for (int i = 0; i < 6; i++)
    {
        this->backupType[i] = backupType[i];
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "In systemBackuphandler backupType[%d]", this->backupType[i]);
    }
    this->backupCategory = backupCategory;
    ACS_CC_ReturnType result = immHandle.Init();
    if (ACS_CC_FAILURE == result)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "OmHandler initialization failed");
    }

    utilities = new ACS_XBRM_Utilities();
    
}

ACS_XBRM_SystemBackupHandler::~ACS_XBRM_SystemBackupHandler()
{
    ACS_XBRM_TRACE_FUNCTION;
    
    ACS_CC_ReturnType result = immHandle.Finalize();
    if (ACS_CC_FAILURE == result)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "OmHandler finalization failed");
    }

    if (utilities != NULL)
    {
        delete utilities;
        utilities = NULL;
    }   
}

int ACS_XBRM_SystemBackupHandler::svc(void)
{
    ACS_XBRM_TRACE_FUNCTION;
    
    syslog(LOG_INFO, "System Backup(s) operation started.");
    
    void *values[1];
    ACS_CC_ImmParameter reportProgress;
    reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str());
    int len;
    // Fetching event start time
    string eventStartTime = ACS_XBRM_Utilities::getCurrentTimeFormat();
    vector<ACS_CC_ValuesDefinitionType> AttrList;
    ACS_CC_ImmParameter paramToFind;
    std::string managedElement("");
    std::vector<std::string> ListManagedElement;
    
    utilities->resetReportProgress((ACS_XBRM_UTILITY::reportProgressRdn[backupCategory]));

    string actioName = "createBackupAndExport";
    values[0] = reinterpret_cast<void *>(const_cast<char *>(actioName.c_str()));
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ACTION_NAME_NAME.c_str()), ATTR_STRINGT, 1, values);

    int intVal = 0;
    values[0] = reinterpret_cast<void *>(&intVal);
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ACTION_ID_NAME.c_str()), ATTR_UINT32T, 1, values);

    intVal = ACS_XBRM_UTILITY::RUNNING;
    values[0] = reinterpret_cast<void *>(&intVal);
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

    intVal = 0;
    values[0] = reinterpret_cast<void *>(&intVal);
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

    string timeActionStarted = ACS_XBRM_Utilities::getCurrentTime();
    values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionStarted.c_str()));
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_STARTED_NAME.c_str()), ATTR_STRINGT, 1, values);
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

    if (ACS_CC_SUCCESS == immHandle.getClassInstances("ManagedElement", ListManagedElement))
    {
        std::string managedElementId = ListManagedElement[0];
        paramToFind.attrName = const_cast<char *>("networkManagedElementId");
        ACS_CC_ReturnType status = immHandle.getAttribute(managedElementId.c_str(), &paramToFind);
        if (ACS_CC_SUCCESS == status)
        {
            int count = paramToFind.attrValuesNum;
            managedElement = std::string(reinterpret_cast<char *>(paramToFind.attrValues[--count]));
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to find managed element ID");
        }
    }

    ACS_CC_ImmParameter backupPasswordAttribute, backupUserLabelAttribute, transferQueueAttribute;
    string backupPassword, backupUserLabel, transferQueue;

    char *objName = (char *)"SystemBrMsystemBrMId=1";
    backupPasswordAttribute.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_BACKUP_PASSWORD_NAME.c_str());
    immHandle.getAttribute(objName, &backupPasswordAttribute);
    bool isSecure = false;
    if (backupPasswordAttribute.attrValuesNum > 0)
    {
        backupPassword = reinterpret_cast<char *>(backupPasswordAttribute.attrValues[0]);
        isSecure = true;
    }
    

    backupUserLabelAttribute.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_BACKUP_USER_LABEL_NAME.c_str());
    immHandle.getAttribute(objName, &backupUserLabelAttribute);
    if (backupUserLabelAttribute.attrValuesNum > 0)
    {
        backupUserLabel = reinterpret_cast<char *>(backupUserLabelAttribute.attrValues[0]);
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "backupUserLabel = %s", backupUserLabel.c_str());
    ACS_XBRM_TRACE_MESSAGE("backupUserLabel = %s", backupUserLabel.c_str());

    transferQueueAttribute.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TRANFER_QUEUE_NAME.c_str());
    immHandle.getAttribute(objName, &transferQueueAttribute);
    if (transferQueueAttribute.attrValuesNum > 0)
    {
        transferQueue = reinterpret_cast<char *>(transferQueueAttribute.attrValues[0]);
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "transferQueue = %s", transferQueue.c_str());
    ACS_XBRM_TRACE_MESSAGE("transferQueue = %s", transferQueue.c_str());

    int totalSystemBackups = 0;
    for (int i = 1; i < 6; i++)
        totalSystemBackups += backupType[i];
    int currSystemBackupNumber = 1;
    
    if (this->backupType[ACS_XBRM_UTILITY::APG] && !sigTermReceived)
    {
        
        apBackupHandler = new ACS_XBRM_ApBackupHandler(eventStartTime, backupPassword, backupUserLabel, transferQueue, ACS_XBRM_UTILITY::reportProgressRdn[backupCategory]);
        int result = apBackupHandler->execute();

        if(sigTermReceived){
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");           
            return -1;
        }

        if (result != 0)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "AP Backup Failed. Aborting the operation");
            if (apBackupHandler != NULL)
            {
                delete apBackupHandler;
                apBackupHandler = NULL;
            }
            if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED)
            {                
                if(isBackupAlarm)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Cease the existing alarm and raise new alarm");
                    ACS_XBRM_TRACE_MESSAGE("Cease the existing alarm and raise new alarm");
                    m_alarmHandler->ceaseAlarm();
                    isBackupAlarm = false;
                } 
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled backup failed for APG. Triggering Alarm");
                m_alarmHandler->initAlarm("APG");
                isBackupAlarm = true;
            }
            syslog(LOG_ERR, "System Backup(s) operation failed.");
            /* set event should be the last operation before return */ 
            ACS_XBRM_EventHandle systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
            ACS_XBRM_Event::SetEvent(systemBackupsHandle);

            return -1;
        }

        string oriBackupName = apBackupHandler->getOriBackupName();
        string oldName = "/data/opt/ap/internal_root/backup_restore/" + oriBackupName;
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Old BackupName : %s", oldName.c_str());

        ACS_XBRM_UTILITY::backupInfo tempBackupInfo;
        tempBackupInfo.backupType = ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::APG];
        tempBackupInfo.backupFileName = oriBackupName;
        tempBackupInfo.backupRelVolume = "";
        tempBackupInfo.backupPath = "/data/opt/ap/internal_root/backup_restore/";
        applicableBackupInfo.push_back(tempBackupInfo);

        int percentage = ACS_XBRM_Utilities::getPercentage(currSystemBackupNumber, totalSystemBackups);
        values[0] = reinterpret_cast<void *>(&percentage);
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

        string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

        currSystemBackupNumber++;

        if (apBackupHandler != NULL)
        {
            delete apBackupHandler;
            apBackupHandler = NULL;
        }
    }
    if (this->backupType[ACS_XBRM_UTILITY::CP] && !sigTermReceived)
    {
        string progressInfo = "CP System Backup Started";
        values[0] = reinterpret_cast<void *>(const_cast<char *>(progressInfo.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        int len = reportProgress.attrValuesNum + 1;
        void *info[len];
        for (int i = 0; i < len - 1; i++)
            info[i] = reportProgress.attrValues[i];
        info[len - 1] = reinterpret_cast<void *>((char *)"CP System Backup Started");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

        string timeAction = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeAction.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

       
        messageHandler = new ACS_XBRM_MMLMessageHandler();
        int ret = messageHandler->open(ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP]);
        if (sigTermReceived)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");
            cleanupNbiFiles();            
            return -1;
        }
        if ((ret < 0) || (messageHandler->getBackupErrorCode() != ""))
        {
            string returnError = "";
            if (ret < 0)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to open MML Session, Abort the backup operation");
                returnError = "Unable to open MML Session";
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failure while generating backup for  %s , Fault code : %s", ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP], messageHandler->getBackupErrorCode().c_str());

                returnError = messageHandler->getBackupErrorCode();
            }
            int percentage = 100, stateVal, resultVal;
            string resultInfo = "", additionalInfo = "";

            stateVal = ACS_XBRM_UTILITY::CANCELLED;
            resultVal = ACS_XBRM_UTILITY::FAILURE;
            resultInfo = "CP Backup Creation Failed";
            additionalInfo = "System Backup(s) failed to create: \n" + returnError;

            values[0] = reinterpret_cast<void *>(&percentage);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

            values[0] = reinterpret_cast<void *>(&stateVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(&resultVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
            len = reportProgress.attrValuesNum + 1;
            void *info3[len];
            for (int i = 0; i < len - 1; i++)
                info3[i] = reportProgress.attrValues[i];
            info3[len - 1] = reinterpret_cast<void *>(const_cast<char *>(additionalInfo.c_str()));

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info3);

            string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
            values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

           
            if (messageHandler != NULL)
            {
                delete messageHandler;
                messageHandler = NULL;
            }
            cleanupNbiFiles();
            if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED)
            {
                 if(isBackupAlarm)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Cease the existing alarm and raise new alarm");
                    m_alarmHandler->ceaseAlarm();
                    isBackupAlarm = false;
                }
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled backup failed for CP. Triggering Alarm");
                m_alarmHandler->initAlarm("CP");
                isBackupAlarm = true;
            }
            syslog(LOG_ERR, "System Backup(s) operation failed.");
            /* set event should be the last operation before return */ 
            ACS_XBRM_EventHandle systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
            ACS_XBRM_Event::SetEvent(systemBackupsHandle);
            return -1;
        }

        string cpSwLevel = messageHandler->getBackupSwLevel();
        string backupName = managedElement + "-" + eventStartTime + "-" + "CP" + "-" + cpSwLevel;
        ACS_XBRM_UTILITY::backupInfo tempBackupInfo;
        tempBackupInfo.backupType = "CP";
        tempBackupInfo.backupFileName = backupName;
        tempBackupInfo.backupRelVolume = messageHandler->getBackupRelVolume();
        tempBackupInfo.backupPath = "/data/opt/ap/internal_root/cp/files/";
        applicableBackupInfo.push_back(tempBackupInfo);

        int percentage = ACS_XBRM_Utilities::getPercentage(currSystemBackupNumber, totalSystemBackups);
        ACS_XBRM_TRACE_MESSAGE("currSystemBackupNumber = %d percentage = %d", currSystemBackupNumber, percentage);
        
        values[0] = reinterpret_cast<void *>(&percentage);
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);
        currSystemBackupNumber++;

        string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        len = reportProgress.attrValuesNum + 1;
        void *infoReport[len];
        for (int i = 0; i < len - 1; i++)
            infoReport[i] = reportProgress.attrValues[i];
        infoReport[len - 1] = reinterpret_cast<void *>((char *)"CP System Backup completed");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, infoReport);

        if (messageHandler != NULL)
        {
            delete messageHandler;
            messageHandler = NULL;
        }
        
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "CP Backup creation Completed");
        ACS_XBRM_TRACE_MESSAGE("CP Backup creation Completed");
    }
   
    if (this->backupType[ACS_XBRM_UTILITY::CP1] && !sigTermReceived)
    {
        string progressInfo = "CP1 System Backup Started";
        values[0] = reinterpret_cast<void *>(const_cast<char *>(progressInfo.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        int len = reportProgress.attrValuesNum + 1;
        void *info[len];
        for (int i = 0; i < len - 1; i++)
            info[i] = reportProgress.attrValues[i];
        info[len - 1] = reinterpret_cast<void *>((char *)"CP1 System Backup Started");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

        string timeActionStarted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionStarted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

        messageHandler = new ACS_XBRM_MMLMessageHandler();
        int ret = messageHandler->open(ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP1]);
        string returnError = messageHandler->getBackupErrorCode();
        if (sigTermReceived)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");
            cleanupNbiFiles();
            return -1;
        }
        
        if ((ret < 0) || ((!returnError.empty())))
        {

            if (ret < 0)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to open MML Session, Abort the backup operation");
                returnError = "Unable to open MML Session";
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failure while generating backup for  %s , Fault code : %s", ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP], messageHandler->getBackupErrorCode().c_str());
            }
            int percentage = 100, stateVal, resultVal;
            string resultInfo = "", additionalInfo = "";

            stateVal = ACS_XBRM_UTILITY::CANCELLED;
            resultVal = ACS_XBRM_UTILITY::FAILURE;
            resultInfo = "CP1 Backup Creation Failed";
            additionalInfo = "System Backup(s) failed to create: \n" + returnError;

            values[0] = reinterpret_cast<void *>(&percentage);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

            values[0] = reinterpret_cast<void *>(&stateVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(&resultVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
            len = reportProgress.attrValuesNum + 1;
            void *info3[len];
            for (int i = 0; i < len - 1; i++)
                info3[i] = reportProgress.attrValues[i];
            info3[len - 1] = reinterpret_cast<void *>(const_cast<char *>(additionalInfo.c_str()));

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info3);

            string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
            values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

           
            if (messageHandler != NULL)
            {
                delete messageHandler;
                messageHandler = NULL;
            }
            cleanupNbiFiles();
            if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED)
            {
                 if(isBackupAlarm)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Cease the existing alarm and raise new alarm");
                    ACS_XBRM_TRACE_MESSAGE("Cease the existing alarm and raise new alarm");
                    m_alarmHandler->ceaseAlarm();
                    isBackupAlarm = false;
                }
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled backup failed for CP1. Triggering Alarm");
                m_alarmHandler->initAlarm("CP1");
                isBackupAlarm = true;
            }
            syslog(LOG_ERR, "System Backup(s) operation failed.");
            /* set event should be the last operation before return */ 
            ACS_XBRM_EventHandle systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
            ACS_XBRM_Event::SetEvent(systemBackupsHandle);
            return -1;
        }

        string cpSwLevel = messageHandler->getBackupSwLevel();
        string backupName = managedElement + "-" + eventStartTime + "-" + "CP1" + "-" + cpSwLevel;
        ACS_XBRM_UTILITY::backupInfo tempBackupInfo;
        tempBackupInfo.backupType = ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP1];
        tempBackupInfo.backupFileName = backupName;
        tempBackupInfo.backupRelVolume = messageHandler->getBackupRelVolume();
        tempBackupInfo.backupPath = "/data/opt/ap/internal_root/cp/files/";
        applicableBackupInfo.push_back(tempBackupInfo);

        int percentage = ACS_XBRM_Utilities::getPercentage(currSystemBackupNumber, totalSystemBackups);
        
        values[0] = reinterpret_cast<void *>(&percentage);
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);
        currSystemBackupNumber++;

        string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        len = reportProgress.attrValuesNum + 1;
        void *info1[len];
        for (int i = 0; i < len - 1; i++)
            info1[i] = reportProgress.attrValues[i];
        info1[len - 1] = reinterpret_cast<void *>((char *)"CP1 System Backup completed");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info1);

        if (messageHandler != NULL)
        {
            delete messageHandler;
            messageHandler = NULL;
        }
        
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "CP1 Backup creation Completed");
        ACS_XBRM_TRACE_MESSAGE("CP1 Backup creation Completed");
    }
    if (this->backupType[ACS_XBRM_UTILITY::CP2] && !sigTermReceived)
    {
        string progressInfo = "CP2 System Backup Started";
        values[0] = reinterpret_cast<void *>(const_cast<char *>(progressInfo.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        int len = reportProgress.attrValuesNum + 1;
        void *info[len];
        for (int i = 0; i < len - 1; i++)
            info[i] = reportProgress.attrValues[i];
        info[len - 1] = reinterpret_cast<void *>((char *)"CP2 System Backup Started");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

        string timeActionStarted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionStarted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

       
        messageHandler = new ACS_XBRM_MMLMessageHandler();
        int ret = messageHandler->open(ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP2]);
        if (sigTermReceived)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");
            cleanupNbiFiles();
            return -1;
        }
        if ((ret < 0) || (messageHandler->getBackupErrorCode() != ""))
        {
            string returnError = "";
            if (ret < 0)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to open MML Session, Abort the backup operation");
                returnError = "Unable to open MML Session";
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failure while generating backup for  %s , Fault code : %s", ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP], messageHandler->getBackupErrorCode().c_str());

                returnError = messageHandler->getBackupErrorCode();
            }
            int percentage = 100, stateVal, resultVal;
            string resultInfo = "", additionalInfo = "";

            stateVal = ACS_XBRM_UTILITY::CANCELLED;
            resultVal = ACS_XBRM_UTILITY::FAILURE;
            resultInfo = "CP2 Backup Creation Failed";
            additionalInfo = "System Backup(s) failed to create: \n" + returnError;

            values[0] = reinterpret_cast<void *>(&percentage);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

            values[0] = reinterpret_cast<void *>(&stateVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(&resultVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
            len = reportProgress.attrValuesNum + 1;
            void *info3[len];
            for (int i = 0; i < len - 1; i++)
                info3[i] = reportProgress.attrValues[i];
            info3[len - 1] = reinterpret_cast<void *>(const_cast<char *>(additionalInfo.c_str()));

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info3);

            string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
            values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

            
            if (messageHandler != NULL)
            {
                delete messageHandler;
                messageHandler = NULL;
            }
            cleanupNbiFiles();
            if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED)
            {
                 if(isBackupAlarm)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Cease the existing alarm and raise new alarm");
                    ACS_XBRM_TRACE_MESSAGE("Cease the existing alarm and raise new alarm");
                    m_alarmHandler->ceaseAlarm();
                    isBackupAlarm = false;
                }
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled backup failed for CP2. Triggering Alarm");
                m_alarmHandler->initAlarm("CP2");
                isBackupAlarm = true;
            }
            syslog(LOG_ERR, "System Backup(s) operation failed.");
            /* set event should be the last operation before return */ 
            ACS_XBRM_EventHandle systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
            ACS_XBRM_Event::SetEvent(systemBackupsHandle);
            return -1;
        }

        string cpSwLevel = messageHandler->getBackupSwLevel();
        string backupName = managedElement + "-" + eventStartTime + "-" + "CP2" + "-" + cpSwLevel;
        ACS_XBRM_UTILITY::backupInfo tempBackupInfo;
        tempBackupInfo.backupType = ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP2];
        tempBackupInfo.backupFileName = backupName;
        tempBackupInfo.backupRelVolume = messageHandler->getBackupRelVolume();
        tempBackupInfo.backupPath = "/data/opt/ap/internal_root/cp/files/";
        applicableBackupInfo.push_back(tempBackupInfo);

        int percentage = ACS_XBRM_Utilities::getPercentage(currSystemBackupNumber, totalSystemBackups);
        values[0] = reinterpret_cast<void *>(&percentage);
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);
        currSystemBackupNumber++;

        string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        len = reportProgress.attrValuesNum + 1;
        void *infoProgress[len];
        for (int i = 0; i < len - 1; i++)
            infoProgress[i] = reportProgress.attrValues[i];
        infoProgress[len - 1] = reinterpret_cast<void *>((char *)"CP2 System Backup completed");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, infoProgress);

        if (messageHandler != NULL)
        {
            delete messageHandler;
            messageHandler = NULL;
        }
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "CP2 Backup creation Completed");
        ACS_XBRM_TRACE_MESSAGE( "CP2 Backup creation Completed");
    }
    
    if (this->backupType[ACS_XBRM_UTILITY::CPCLUSTER] && !sigTermReceived)
    {
        string progressInfo = "CPCLUSTER System Backup Started";
        values[0] = reinterpret_cast<void *>(const_cast<char *>(progressInfo.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        int len = reportProgress.attrValuesNum + 1;
        void *info[len];
        for (int i = 0; i < len - 1; i++)
            info[i] = reportProgress.attrValues[i];
        info[len - 1] = reinterpret_cast<void *>((char *)"CPCLUSTER System Backup Started");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

        string timeActionStarted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionStarted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

        
        messageHandler = new ACS_XBRM_MMLMessageHandler();
        int ret = messageHandler->open(ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CPCLUSTER]);
        if (sigTermReceived)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");
            cleanupNbiFiles();
            return -1;
        }
        if ((ret < 0) || (messageHandler->getBackupErrorCode() != ""))
        {
            string returnError = "";
            if (ret < 0)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to open MML Session, Abort the backup operation");
                returnError = "Unable to open MML Session";
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failure while generating backup for  %s , Fault code : %s", ACS_XBRM_UTILITY::cpBackupName[ACS_XBRM_UTILITY::CP], messageHandler->getBackupErrorCode().c_str());

                returnError = messageHandler->getBackupErrorCode();
            }
            int percentage = 100, stateVal, resultVal;
            string resultInfo = "", additionalInfo = "";

            stateVal = ACS_XBRM_UTILITY::CANCELLED;
            resultVal = ACS_XBRM_UTILITY::FAILURE;
            resultInfo = "CPCLUSTER Backup Creation Failed";
            additionalInfo = "System Backup(s) failed to create: \n" + returnError;

            values[0] = reinterpret_cast<void *>(&percentage);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

            values[0] = reinterpret_cast<void *>(&stateVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(&resultVal);
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

            values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
            len = reportProgress.attrValuesNum + 1;
            void *info3[len];
            for (int i = 0; i < len - 1; i++)
                info3[i] = reportProgress.attrValues[i];
            info3[len - 1] = reinterpret_cast<void *>(const_cast<char *>(additionalInfo.c_str()));

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info3);

            string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
            values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

            utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

           
            if (messageHandler != NULL)
            {
                delete messageHandler;
                messageHandler = NULL;
            }
            cleanupNbiFiles();
            if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED)
            {
                 if(isBackupAlarm)
                {
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Cease the existing alarm and raise new alarm");
                    ACS_XBRM_TRACE_MESSAGE("Cease the existing alarm and raise new alarm");
                    m_alarmHandler->ceaseAlarm();
                    isBackupAlarm = false;
                }
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled backup failed for CPCLUSTER. Triggering Alarm");
                m_alarmHandler->initAlarm( "CPCLUSTER");
                isBackupAlarm = true;
            }
            syslog(LOG_ERR, "System Backup(s) operation failed.");
            /* set event should be the last operation before return */ 
            ACS_XBRM_EventHandle systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
            ACS_XBRM_Event::SetEvent(systemBackupsHandle);
            return -1;
        }

        string cpSwLevel = messageHandler->getBackupSwLevel();
        string backupName = managedElement + "-" + eventStartTime + "-" + "CPCLUSTER" + "-" + cpSwLevel;
        ACS_XBRM_UTILITY::backupInfo tempBackupInfo;
        tempBackupInfo.backupType = "CLUSTER";
        tempBackupInfo.backupFileName = backupName;
        tempBackupInfo.backupRelVolume = messageHandler->getBackupRelVolume();
        tempBackupInfo.backupPath = "/data/opt/ap/internal_root/cp/files/";
        applicableBackupInfo.push_back(tempBackupInfo);

        int percentage = ACS_XBRM_Utilities::getPercentage(currSystemBackupNumber, totalSystemBackups);
        values[0] = reinterpret_cast<void *>(&percentage);
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);
        currSystemBackupNumber++;

        string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
        values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
        utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
        len = reportProgress.attrValuesNum + 1;
        void *info2[len];
        for (int i = 0; i < len - 1; i++)
            info2[i] = reportProgress.attrValues[i];
        info2[len - 1] = reinterpret_cast<void *>((char *)"CPCLUSTER System Backup completed");
        utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info2);

        if (messageHandler != NULL)
        {
            delete messageHandler;
            messageHandler = NULL;
        }
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "CPCLUSTER Backup creation Completed");
        ACS_XBRM_TRACE_MESSAGE("CPCLUSTER Backup creation Completed");
    }
    if (sigTermReceived)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");
        cleanupNbiFiles();
        return -1;
    }
    utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
    len = reportProgress.attrValuesNum + 1;
    void *info2[len];
    for (int i = 0; i < len - 1; i++)
        info2[i] = reportProgress.attrValues[i];
    info2[len - 1] = reinterpret_cast<void *>((char *)"System Backup(s) created successfully");

    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info2);

    string progressInfo = "System Backup(s) export to APG NBI Started";
    values[0] = reinterpret_cast<void *>(const_cast<char *>(progressInfo.c_str()));
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

    // All backups were created at this point. Export CP Backups to NBI
    string exportError = "";
    ACS_XBRM_TRACE_MESSAGE("System Backup(s) export to APG NBI Started");
    for (unsigned int i = 0; i < applicableBackupInfo.size() && !sigTermReceived; i++)
    {

        
        if ((applicableBackupInfo[i].backupType == "CP") || (applicableBackupInfo[i].backupType == "CLUSTER") || (applicableBackupInfo[i].backupType == "CP1") || (applicableBackupInfo[i].backupType == "CP2"))
        {            
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "Before export setting,  BackupName : %s", applicableBackupInfo[i].backupFileName.c_str());
            ACS_XBRM_TRACE_MESSAGE("Before export setting,  BackupName : %s", applicableBackupInfo[i].backupFileName.c_str());
            int exportRet = ACS_XBRM_Utilities::exportCpBackupToApgFileSystem(applicableBackupInfo[i].backupFileName, applicableBackupInfo[i].backupType, applicableBackupInfo[i].backupRelVolume, &exportError);
            if (exportRet < 0)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to export CP backup, aborting the operation");
                //TODO Cleanup files from NBI if already present
                cleanupNbiFiles();
                if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED)
                {
                    if (isBackupAlarm)
                    {
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Cease the existing alarm and raise new alarm");
                        ACS_XBRM_TRACE_MESSAGE("Cease the existing alarm and raise new alarm");
                        m_alarmHandler->ceaseAlarm();
                        isBackupAlarm = false;
                    }
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled backup failed because of export failed. Triggering Alarm");
                    m_alarmHandler->initAlarm(applicableBackupInfo[i].backupType.c_str());
                    isBackupAlarm = true;
                }
                int percentage = 100, stateVal, resultVal;
                string resultInfo = "", additionalInfo = "";

                stateVal = ACS_XBRM_UTILITY::CANCELLED;
                resultVal = ACS_XBRM_UTILITY::FAILURE;
                resultInfo = "Failed to export backup files to APG NBI.";
                additionalInfo = "System Backup(s) failed to export to APG NBI : \n" + exportError;

                values[0] = reinterpret_cast<void *>(&percentage);
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

                values[0] = reinterpret_cast<void *>(&stateVal);
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

                values[0] = reinterpret_cast<void *>(&resultVal);
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

                values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

                utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
                len = reportProgress.attrValuesNum + 1;
                void *info3[len];
                for (int i = 0; i < len - 1; i++)
                    info3[i] = reportProgress.attrValues[i];
                info3[len - 1] = reinterpret_cast<void *>(const_cast<char *>(additionalInfo.c_str()));

                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info3);

                string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
                values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

                utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
                
                syslog(LOG_ERR, "System Backup(s) operation failed.");
                /* set event should be the last operation before return */ 
                ACS_XBRM_EventHandle systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
                ACS_XBRM_Event::SetEvent(systemBackupsHandle);
                return -1;
            }
            string oldName = "/data/opt/ap/internal_root/cp/files/" + applicableBackupInfo[i].backupFileName + ".zip";
            applicableBackupInfo[i].backupFileName = applicableBackupInfo[i].backupFileName + ".zip";
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "Final BackupName : %s", oldName.c_str());
            ACS_XBRM_TRACE_MESSAGE("Final BackupName : %s", oldName.c_str());
        }
    }
    if (sigTermReceived)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");
        cleanupNbiFiles();
        return -1;
    }
    progressInfo = "System Backup(s) export to remote server via transfer queue started";
    values[0] = reinterpret_cast<void *>(const_cast<char *>(progressInfo.c_str()));
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

    // Export to GOH
    bool isExportFailed = false;
    ACS_XBRM_TRACE_MESSAGE("System Backup(s) export to remote server via transfer queue started");
    string failedFiles = "";
    for (unsigned int i = 0; i < applicableBackupInfo.size() && !sigTermReceived && !isExportFailed; i++)
    {
        int ret = ACS_XBRM_Utilities::sendFilesToGOH(applicableBackupInfo[i].backupPath, transferQueue, applicableBackupInfo[i].backupFileName);
       
        if (ret == TQ_ATTACH_FAILED)
        {
            if (remove(applicableBackupInfo[i].backupPath.append(applicableBackupInfo[i].backupFileName).c_str()))
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "TQ attach failed.So successfully deleted the created backup:%s", applicableBackupInfo[i].backupFileName.c_str());
            else
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "TQ attach failed.So deleteing the created backup failed: %s", applicableBackupInfo[i].backupFileName.c_str());
            isExportFailed = true;
            failedFiles = applicableBackupInfo[i].backupFileName;
        }
        else if (ret == TQ_STATE_FAILED)
        {
            if (remove(applicableBackupInfo[i].backupPath.append(applicableBackupInfo[i].backupFileName).c_str()))
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "TQ state failed.So successfully deleted the created backup:%s", applicableBackupInfo[i].backupFileName.c_str());
            else
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "TQ state failed.So deleteing the created backup failed: %s", applicableBackupInfo[i].backupFileName.c_str());
            isExportFailed = true;
            failedFiles = applicableBackupInfo[i].backupFileName;
        }
        else if (ret == TQ_SEND_FAILED)
        {
            if (remove(applicableBackupInfo[i].backupPath.append(applicableBackupInfo[i].backupFileName).c_str()))
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "TQ send failed.So successfully deleted the created backup:%s", applicableBackupInfo[i].backupFileName.c_str());
            else
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "TQ send failed.So deleteing the created backup failed: %s", applicableBackupInfo[i].backupFileName.c_str());
            isExportFailed = true;
            failedFiles = applicableBackupInfo[i].backupFileName;
        }
    }
    if (sigTermReceived)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Shutdown recieved");
        return -1;
    }
    int percentage = 100, stateVal, resultVal;
    string resultInfo = "", additionalInfo = "";
    bool updateLastScheduledEventTime = false;

    if (!isExportFailed)
    {
        if (isBackupAlarm && ((backupCategory == ACS_XBRM_UTILITY::SCHEDULED) || systemBackupType == ACS_XBRM_UTILITY::NODE_TYPE))
        {
            m_alarmHandler->ceaseAlarm();
            isBackupAlarm = false;
        }
       
        stateVal = ACS_XBRM_UTILITY::FINISHED;
        resultVal = ACS_XBRM_UTILITY::SUCCESS;
        resultInfo = "System Backup(s) created successfully and sent to transfer queue.";
        additionalInfo = "System Backup(s) sent to transfer queue";
        if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED){
            updateLastScheduledEventTime = true;
        }
        syslog(LOG_INFO, "System Backup(s) operation successful.");
    }
    else
    {
        
        cleanupNbiFiles();
        stateVal = ACS_XBRM_UTILITY::CANCELLED;
        resultVal = ACS_XBRM_UTILITY::FAILURE;
        resultInfo = "System Backup(s) failed to send to transfer queue";
        additionalInfo = "System Backup(s) failed to send to transfer queue : \n" + failedFiles;
        if (backupCategory == ACS_XBRM_UTILITY::SCHEDULED)
        {
            if (isBackupAlarm)
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "Cease the existing alarm and raise new alarm");
                ACS_XBRM_TRACE_MESSAGE("Cease the existing alarm and raise new alarm");
                m_alarmHandler->ceaseAlarm();
                isBackupAlarm = false;
            }
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled backup failed. Triggering Alarm");
            m_alarmHandler->initAlarm();
            isBackupAlarm = true;
        }
        syslog(LOG_ERR, "System Backup(s) operation failed.");
    }
    values[0] = reinterpret_cast<void *>(&percentage);
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

    values[0] = reinterpret_cast<void *>(&stateVal);
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

    values[0] = reinterpret_cast<void *>(&resultVal);
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

    values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

    if (updateLastScheduledEventTime)
    {
        std::vector<std::string> schedulerObjNames;
        immHandle.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME.c_str(), schedulerObjNames);
        if (schedulerObjNames.size())
        {
            values[0] = reinterpret_cast<void *>(const_cast<char *>(eventStartTime.c_str()));
            ACS_CC_ImmParameter schedulerMO;
            schedulerMO.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_LAST_SCHEDULED_EVENT_TIME_NAME.c_str());
            schedulerMO.attrType = ATTR_STRINGT;
            schedulerMO.attrValuesNum = 1;
            schedulerMO.attrValues = values;
            if (ACS_CC_FAILURE == immHandle.modifyAttribute(schedulerObjNames[0].c_str(), &schedulerMO))
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "Failed to update %s of %s MO", schedulerMO.attrName, schedulerObjNames[0].c_str());
        }
    }

    utilities->immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], &reportProgress);
    len = reportProgress.attrValuesNum + 1;
    void *info3[len];
    for (int i = 0; i < len - 1; i++)
        info3[i] = reportProgress.attrValues[i];
    info3[len - 1] = reinterpret_cast<void *>(const_cast<char *>(additionalInfo.c_str()));

    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info3);

    string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
    values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

    utilities->updateReportProgressAttribute(ACS_XBRM_UTILITY::reportProgressRdn[backupCategory], const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
    
    /* set event should be the last operation before return */ 
    ACS_XBRM_EventHandle systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME);
    ACS_XBRM_Event::SetEvent(systemBackupsHandle);
    return 0;
}

void ACS_XBRM_SystemBackupHandler::stop()
{
    ACS_XBRM_TRACE_FUNCTION;
    sigTermReceived = true;
    if (apBackupHandler)
    {
        pthread_t pThreadId = apBackupHandler->getPthreadId();
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "In ACS_XBRM_SystemBackupHandler  pthreadId  %lu", pThreadId);
        ACS_XBRM_TRACE_MESSAGE("In ACS_XBRM_SystemBackupHandler  pthreadId  %lu", pThreadId);
        int status = pthread_cancel(pThreadId);
        if (status < 0)
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "pthread_kill failed");
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "pthread_kill sucessfull");
        }
    }
    if (messageHandler)
    {
        // stop
        messageHandler->stop();
    }
}

void ACS_XBRM_SystemBackupHandler::cleanupNbiFiles()
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Cleaning of backup files in failure case");
    for (unsigned int i = 0; i < applicableBackupInfo.size() ; i++)
        {
            
            string fileName = applicableBackupInfo[i].backupPath + applicableBackupInfo[i].backupFileName;
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "File to be clean : %s ", fileName.c_str());
            FILE *file;
            file = fopen(fileName.c_str(),"r");
            if(file)
            {
                fclose(file);                
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "File exists - %s, remove it ", fileName.c_str());
                remove(fileName.c_str());
            }
            
        }
}
