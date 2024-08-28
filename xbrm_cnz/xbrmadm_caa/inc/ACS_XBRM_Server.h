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
 * @file ACS_XBRM_Server.h
 *
 * @brief
 * ACS_XBRM_Server Class for Service Implementation
 *
 * @details
 * ACS_XBRM_Server is an interface to handle service initialization/running/
 * cleanup
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


#ifndef ACS_XBRM_SERVER_H_
#define ACS_XBRM_SERVER_H_

#include <iostream>
#include <sys/eventfd.h>
#include <syslog.h>
#include <vector>

#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <ACS_APGCC_AmfTypes.h>

#include "ACS_XBRM_Event.h"
#include "ACS_XBRM_ScheduleHandler.h"
#include "ACS_XBRM_PeriodicEventHandler.h"
#include "ACS_XBRM_BrmHandler.h"
#include "ACS_XBRM_Utilities.h"
#include "ACS_XBRM_EventScheduler.h"
#include "ACS_XBRM_AlarmHandler.h"

extern int backupType[6];
extern int backupCategory;
extern int systemBackupType;
extern bool isBackupAlarm;


class ACS_XBRM_Server : public ACE_Task_Base{
    public: 
        ACS_XBRM_Server();
        ~ACS_XBRM_Server();
        int svc(); 
        ACS_CC_ReturnType startWorkerThreads();
        ACS_CC_ReturnType stopWorkerThreads();
        ACS_XBRM_EventHandle shutdownEvent;
        ACS_XBRM_EventHandle systemBackupsStartEvent;
        ACS_XBRM_EventHandle systemBackupsEndEvent;
        ACS_XBRM_EventHandle eventSchedulingStartEvent;
        ACS_XBRM_EventHandle eventSchedulingEndEvent;


        ACS_CC_ReturnType setupSystemBrmCallBacks();
        ACS_CC_ReturnType setupSystemBrmBackupSchedulerCallBacks();
        ACS_CC_ReturnType setupSystemBrmPeriodicEventCallBacks();
        ACS_CC_ReturnType startEventScheduler();
        ACS_CC_ReturnType stopEventScheduler();
        ACS_CC_ReturnType createReportProgress(string parentDn);            
        
        string getClearPath();
        bool isRestartAfterRestore();
    private:
        OmHandler immHandler;
        ACS_XBRM_BrmHandler* brmHandler;
        ACS_XBRM_ScheduleHandler* scheduleHandler;
        ACS_XBRM_PeriodicEventHandler* periodicEventHandler;
        ACS_XBRM_Utilities* utilities = NULL;
         ACS_XBRM_AlarmHandler* alarmHandler = NULL;
        ACS_XBRM_EventScheduler* eventScheduler = NULL;
};

#endif