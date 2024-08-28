#include "ACS_XBRM_EventScheduler.h"
#include "ACS_XBRM_Utilities.h"
#include "ACS_XBRM_ImmUtlis.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Event.h"
#include "ACS_CS_API.h"
#include "ACS_XBRM_Server.h"
#include "ACS_XBRM_Tracer.h"
#include <iostream>
#include <set>

extern bool updateNextScheduledTime;
extern bool updateAdminState;

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_EventScheduler);

ACS_XBRM_EventScheduler::ACS_XBRM_EventScheduler()
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = immHandler.Init();
    if (ACS_CC_FAILURE == result){
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "immHandler initialization failed");
        ACS_XBRM_TRACE_MESSAGE("immHandler initialization failed");
    }

    utilities = new ACS_XBRM_Utilities();
    execute = true;
}

ACS_XBRM_EventScheduler::~ACS_XBRM_EventScheduler()
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = immHandler.Finalize();
    if (ACS_CC_FAILURE == result){
        ACS_XBRM_LOG(LOG_LEVEL_WARN, "immHandler finalization failed");
        ACS_XBRM_TRACE_MESSAGE("immHandler finalization failed");
    }
    
    if(utilities != NULL){
        delete utilities;
        utilities = NULL;
    }
}

int ACS_XBRM_EventScheduler::svc(void){
    ACS_XBRM_TRACE_FUNCTION;
    int adminState = ACS_XBRM_UTILITY::LOCKED;
    string nextScheduledTime = "";
    string triggeredEventRdn = "";
    bool onDemandBackupInterrupt = false;
    bool isRetry = false;

    int schedulerState;
    int brmState;

    //read adminState and nextScheduledTime first time (usefull in restart cases)
    ACS_CC_ImmParameter schedulerAdminState;
    schedulerAdminState.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADMIN_STATE_NAME.c_str());
    ACS_CC_ReturnType result = immHandler.getAttribute(schedulerRdn, &schedulerAdminState);
    if(ACS_CC_SUCCESS == result && schedulerAdminState.attrValuesNum)
        adminState = *reinterpret_cast<int *>(schedulerAdminState.attrValues[0]);
    
    ACS_CC_ImmParameter schedulerNextEventTime;
    schedulerNextEventTime.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_NEXT_SCHEDULED_EVENT_TIME_NAME.c_str());
    // immHandler.modifyAttribute(schedulerRdn, )
    result = immHandler.getAttribute(schedulerRdn, &schedulerNextEventTime);
    cout << "nextScheduledTime = " << nextScheduledTime << endl;
    if (ACS_CC_SUCCESS == result && schedulerNextEventTime.attrValuesNum)
        nextScheduledTime = reinterpret_cast<char *>(schedulerNextEventTime.attrValues[0]);
    cout << "nextScheduledTime = " << nextScheduledTime << endl;

    while(execute){
        ACE_OS::sleep(1);
        if(updateAdminState){
            ACS_CC_ImmParameter scheduler;
            scheduler.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADMIN_STATE_NAME.c_str());
            result = immHandler.getAttribute(schedulerRdn, &scheduler);
            if(ACS_CC_FAILURE == result){
                continue;
            }
            if(scheduler.attrValuesNum)
                adminState = *reinterpret_cast<int *>(scheduler.attrValues[0]);
            updateAdminState = false;
        }
        if(updateNextScheduledTime){
            string nextRetryScheduledTime = nextScheduledTime;
            if(onDemandBackupInterrupt){
                int day, temp;
                sscanf(nextRetryScheduledTime.c_str(), "%4d-%2d-%2dT%2d%*c%2d%*c%2d", &temp, &temp, &day, &temp, &temp, &temp);
                cout << "nextRetryScheduledTime" << nextRetryScheduledTime << endl;
                nextRetryScheduledTime = utilities->nextSchedule(nextRetryScheduledTime, "00-00-00T00:05:00", day);
                cout << "nextRetryScheduledTime" << nextRetryScheduledTime << endl;
            }
            utilities->updateNextScheduledEventTime();
            ACS_CC_ImmParameter scheduler;
            scheduler.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_NEXT_SCHEDULED_EVENT_TIME_NAME.c_str());
            result = immHandler.getAttribute(schedulerRdn, &scheduler);
            if(ACS_CC_FAILURE == result){
                continue;
            }
            if(scheduler.attrValuesNum){
                nextScheduledTime = reinterpret_cast<char *>(scheduler.attrValues[0]);
                if(onDemandBackupInterrupt && compare(nextScheduledTime, nextRetryScheduledTime)){
                    ACS_XBRM_LOG(LOG_LEVEL_WARN, "Scheduled backup retry set after 5 mins at %s", nextRetryScheduledTime.c_str());
                    ACS_XBRM_TRACE_MESSAGE("Scheduled backup retry set after 5 mins at %s", nextRetryScheduledTime.c_str());
                    /* update next Schedule in MOM */
                    ACS_CC_ImmParameter schedulerUpdate;
                    schedulerUpdate.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_NEXT_SCHEDULED_EVENT_TIME_NAME.c_str());
                    schedulerUpdate.attrType = ATTR_STRINGT;
                    schedulerUpdate.attrValuesNum = 1;
                    void* values[1] = {reinterpret_cast<void *>(const_cast<char *>(nextRetryScheduledTime.c_str()))};
                    schedulerUpdate.attrValues = values;
                    ACS_CC_ReturnType result = immHandler.modifyAttribute(schedulerRdn, &schedulerUpdate);
                    if(ACS_CC_FAILURE == result){
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Failed to modify nextRetryScheduledTime");
                    }else{
                        nextScheduledTime = nextRetryScheduledTime;
                        isRetry = true;
                    }
                }
            }
            else
                nextScheduledTime = "";
            updateNextScheduledTime = false;
            onDemandBackupInterrupt = false;
        }
        if(0 == ACE_OS::strcmp(nextScheduledTime.c_str(), "")){
            /* no upcomming scheduled events */
            continue;
        }
        if(compare(ACS_XBRM_Utilities::getCurrentTime(), nextScheduledTime)){
            if(ACS_XBRM_UTILITY::LOCKED == adminState){
                /*
                    TODO
                    Scheduling is not activated
                    update corresponding lastScheduledDate of periodic Event
                    update nextScheduledTime of scdeduler
                */
                ACS_XBRM_LOG(LOG_LEVEL_WARN, "Scheduled System Backups event is skipped since adminState is LOCKED");
                triggeredEventRdn = const_cast<char *>(getExecutingEventRdn(nextScheduledTime).c_str());
                utilities->periodic_updationAfterJobExecution(triggeredEventRdn);
                updateNextScheduledTime = true;
                continue;
            }else{
                /* can be executed */
                ACS_CC_ImmParameter reportProgress;
                reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str());
                
                ACS_CC_ImmParameter systemBrmReportProgress;
                systemBrmReportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str());
                
                immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::SCHEDULED], &reportProgress);
                schedulerState = *reinterpret_cast<int *>(reportProgress.attrValues[0]);
                // cout << "schedulerState = " << schedulerState << endl;

                immHandler.getAttribute(ACS_XBRM_UTILITY::reportProgressRdn[ACS_XBRM_UTILITY::ON_DEMAND], &systemBrmReportProgress);
                brmState = *reinterpret_cast<int *>(systemBrmReportProgress.attrValues[0]);
                // cout << "brmState = " << brmState << endl;
                
                if(reportProgress.attrValuesNum > 0 && ACS_XBRM_UTILITY::RUNNING == schedulerState){
                    /* skip event */
                    triggeredEventRdn = getExecutingEventRdn(nextScheduledTime).c_str();
                    if(0 == ACE_OS::strcmp(triggeredEventRdn.c_str(), "")){
                        updateNextScheduledTime = true;
                        continue;
                    }
                    ACS_XBRM_LOG(LOG_LEVEL_WARN, "Scheduled System Backups event of %s is skipped", triggeredEventRdn.c_str());
                    ACS_XBRM_TRACE_MESSAGE("Scheduled System Backups event of %s is skipped", triggeredEventRdn.c_str());
                    utilities->periodic_updationAfterJobExecution(triggeredEventRdn);
                    updateNextScheduledTime = true;
                }else if(systemBrmReportProgress.attrValuesNum > 0 && ACS_XBRM_UTILITY::RUNNING == brmState){
                    /* skip event */
                    triggeredEventRdn = getExecutingEventRdn(nextScheduledTime);
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Scheduled Backup is cancelled due to manual backup operation ongoing");
                    ACS_XBRM_TRACE_MESSAGE("Scheduled Backup is cancelled due to manual backup operation ongoing");
                    if(0 == ACE_OS::strcmp(triggeredEventRdn.c_str(), "")){
                        updateNextScheduledTime = true;
                        continue;
                    }
                    utilities->periodic_updationAfterJobExecution(triggeredEventRdn);
                    updateNextScheduledTime = true;
                    onDemandBackupInterrupt = true;
                }else{
                    /* execute event */
                    executingEventRdn = getExecutingEventRdn(nextScheduledTime).c_str();
                    /* 
                        TODO 
                        trigger scheduled backup
                        update corresponding lastScheduledDate of periodic Event
                        update nextScheduledTime of scdeduler
                    */
                    if(!isRetry && (0 == ACE_OS::strcmp(executingEventRdn.c_str(), ""))){
                        updateNextScheduledTime = true;
                        continue;
                    }
                    ACS_XBRM_LOG(LOG_LEVEL_WARN, "Scheduled System Backups event of %s triggered", executingEventRdn.c_str());
                    ACS_XBRM_TRACE_MESSAGE("Scheduled System Backups event of %s triggered", executingEventRdn.c_str());
                    utilities->periodic_updationAfterJobExecution(executingEventRdn);
                    utilities->updateNextScheduledEventTime();
                    triggerScheduledBackupEvent();
                    updateNextScheduledTime = true;
                    isRetry = false;
                }
            }
        }
    }
    return 0;
}

void ACS_XBRM_EventScheduler::stop(){
    execute = false;
}

string ACS_XBRM_EventScheduler::getExecutingEventRdn(string nextScheduledTime){
    std::vector<std::string> periodicEventObjNames;
    this->immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_PERIODIC_EVENT_NAME.c_str(), periodicEventObjNames);

    for(int i = 0; i < (int)periodicEventObjNames.size(); i++){
        ACS_CC_ImmParameter periodicEvent;
        periodicEvent.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_LAST_SCHEDULED_DATE_NAME.c_str());
        this->immHandler.getAttribute(periodicEventObjNames[i].c_str(), &periodicEvent);
        if(periodicEvent.attrValuesNum){
            if(0 == ACE_OS::strcmp(nextScheduledTime.c_str(), reinterpret_cast<char *>(periodicEvent.attrValues[0]))){
                return periodicEventObjNames[i];
            }
        }
    }
    return "";
}

bool ACS_XBRM_EventScheduler::compare(const std::string& currentTime, const std::string& nextScheduledTime) {
    ACS_XBRM_TRACE_FUNCTION;
    struct std::tm tm1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    struct std::tm tm2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    strptime(currentTime.c_str(), "%Y-%m-%dT%H:%M:%S", &tm1);
    strptime(nextScheduledTime.c_str(), "%Y-%m-%dT%H:%M:%S", &tm2);

    std::time_t time1 = std::mktime(&tm1);
    std::time_t time2 = std::mktime(&tm2);

    return time1 > time2;
}

void ACS_XBRM_EventScheduler::triggerScheduledBackupEvent(){
    ACS_XBRM_TRACE_FUNCTION;
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
    if(ACS_CS_API_CommonBasedArchitecture::AP2 != APtype){
        if(isMultipleCPSystem){
            // blade cluster
            backupType[ACS_XBRM_UTILITY::CPCLUSTER] = 1;
            if(cpCount == 2){
                backupType[ACS_XBRM_UTILITY::CP1] = 1;
                backupType[ACS_XBRM_UTILITY::CP2] = 1;
            }else if(cpCount == 1){
                backupType[ACS_XBRM_UTILITY::CP1] = 1;
            }
        }else{
            // singleCp system
            backupType[ACS_XBRM_UTILITY::CP] = 1;
        }
    }
    ACS_XBRM_EventHandle systemBackupsHandle;
    systemBackupsHandle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME);
    ACS_XBRM_Event::SetEvent(systemBackupsHandle);
    sleep(3);
}