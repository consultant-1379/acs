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
 * @file ACS_XBRM_ImmUtlis.h
 *
 * @brief
 * ACS_XBRM_ImmMapper Namespace 
 *
 * @details
 * ACS_XBRM_ImmMapper provide IMM utilities for service
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


#ifndef ACS_XBRM_IMMUTILS_H_
#define ACS_XBRM_IMMUTILS_H_


namespace ACS_XBRM_ImmMapper
{

    //SystemBrM
    static const string CLASS_SYSTEM_BRM_NAME = "SystemBrMSystemBrM";
    static const string IMPL_SYSTEM_BRM_NAME = "XBRM_SystemBrMSystemBrM_Impl";
    static const string ATTR_TRANFER_QUEUE_NAME = "transferQueue";
    static const string ATTR_TRANFER_QUEUE_DN_NAME = "transferQueueDn";
    static const string ATTR_BACKUP_PASSWORD_NAME = "backupPassword";
    static const string ATTR_BACKUP_USER_LABEL_NAME = "backupUserLabel";

    //ACTION
    static const string PARM_SYSTEM_BACKUP_TYPE_NAME = "systemBackupType";

    //SystemBrmBackupScheduler
    static const string CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME = "SystemBrMSystemBrmBackupScheduler";
    static const string IMPL_SYSTEM_BRM_BACKUP_SCHEDULER_NAME = "XBRM_SystemBrmBackupScheduler_Impl";
    static const string ATTR_ADMIN_STATE_NAME = "adminState";
    static const string ATTR_LAST_SCHEDULED_EVENT_TIME_NAME = "lastScheduledEventTime";
    static const string ATTR_NEXT_SCHEDULED_EVENT_TIME_NAME = "nextScheduledEventTime";

    //SystemBrmPeriodicEvent
    static const string CLASS_SYSTEM_BRM_PERIODIC_EVENT_NAME = "SystemBrMSystemBrmPeriodicEvent";
    static const string IMPL_SYSTEM_BRM_PERIODIC_EVENT_NAME = "XBRM_SystemBrmPeriodicEvent_Impl";
    static const string ATTR_START_TIME_NAME = "startTime";
    static const string ATTR_STOP_TIME_NAME = "stopTime";
    static const string ATTR_MINUTES_NAME = "minutes";
    static const string ATTR_MONTHS_NAME = "months";
    static const string ATTR_HOURS_NAME = "hours";
    static const string ATTR_DAYS_NAME = "days";
    static const string ATTR_WEEK_NAME = "week";
    static const string ATTR_LAST_SCHEDULED_DATE_NAME = "lastScheduledDate";
    
    //AXE BRM ASYN ACTION PROGRESS
    static const string ATTR_ACTION_ID_NAME = "actionId";
    static const string ATTR_ACTION_NAME_NAME = "actionName";
    static const string ATTR_ADDITIONAL_INFO_NAME = "additionalInfo";
    static const string ATTR_PROGRESS_INFO_NAME = "progressInfo";
    static const string ATTR_PROGRESS_PERCENTAGE_NAME = "progressPercentage";
    static const string ATTR_RESULT_NAME = "result";
    static const string ATTR_RESULT_INFO_NAME = "resultInfo";
    static const string ATTR_STATE_NAME = "state";
    static const string ATTR_TIME_ACTION_COMPLETED_NAME = "timeActionCompleted";
    static const string ATTR_TIME_ACTION_STARTED_NAME = "timeActionStarted";
    static const string ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME = "timeOfLastStatusUpdate";
  
    static const string ATTR_REPORT_PROGRESS_NAME = "reportProgress";
    static const string CLASS_SYSTEM_BRM_ASYNC_ACTION_PROGRESS_NAME = "SystemBrMAsyncActionProgress";
    static const string ATTR_INITIATING_DSET_NAME = "initiatingDestinationSet";
    static const string ATTR_RESPONDING_DSET_NAME = "respondingDestinationSet";
    static const string ATTR_INIT_DSET_DN_NAME = "initDestinationSetDn";
    static const string ATTR_TRANSFER_PROTOCOL_NAME = "transferProtocol";

} // namespace ACS_XBRM_ImmMapper

#endif