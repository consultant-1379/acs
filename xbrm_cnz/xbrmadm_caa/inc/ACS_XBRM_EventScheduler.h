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
 * @file ACS_XBRM_Scheduling.h
 *
 * @brief
 * ACS_XBRM_Scheduling Thread to manage Scheduled System Backup Handling
 *
 * @details
 * ACS_XBRM_Scheduling 
 *
 * @author ZPAGSAI
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-07-31  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

#ifndef ACS_XBRM_EVENTSCHEDULER_H
#define ACS_XBRM_EVENTSCHEDULER_H

#include "ace/OS.h"
#include "ace/Task.h"
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include "ACS_XBRM_Utilities.h"
#include "acs_apgcc_omhandler.h"
// #include <chrono>
#include <sstream>
#include <ctime>

struct eventSchedulerDetails
{
    char* periodicEventRdn;
    /* int state; */
    int retryNumber = 0;
    eventSchedulerDetails(string rdn){
        periodicEventRdn = const_cast<char *>(rdn.c_str());
    }
};

class ACS_XBRM_EventScheduler : public ACE_Task_Base{
    // friend class ACE_Singleton<ACS_XBRM_EventScheduler, ACE_Thread_Mutex>;
    public:
        ACS_XBRM_EventScheduler();
        ~ACS_XBRM_EventScheduler();
        int svc(void);
        void stop();
        bool compare(const std::string& currentTime, const std::string& nextScheduledTime);
        void triggerScheduledBackupEvent();
        // priority_queue<eventDetails> upcommingEvents;
        // auto comp = [](const eventDetails& l, const eventDetails& r) {
        //     return l.upcomingEventTime > r.upcomingEventTime; 
        // };
        // auto compare = [](const string& l, const string& r) {
        //     return l > r; 
        // };
        // priority_queue<eventDetails, vector<eventDetails>, decltype(compare)> upcommingEvents(compare);
        // map<string, bool> activePeriodicEvents;
        const char* schedulerRdn = "systemBrmBackupSchedulerId=1,SystemBrMsystemBrMId=1";
        std::string getExecutingEventRdn(std::string nextScheduledTime);
        OmHandler immHandler;
        ACS_XBRM_Utilities* utilities = NULL;
        bool execute = true;
        string executingEventRdn;
        eventSchedulerDetails* currentEvent = NULL;
};

#endif