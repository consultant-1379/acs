//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#ifndef ACS_SCH_HA_APPMANAGER_H
#define ACS_SCH_HA_APPMANAGER_H

#include "ACS_APGCC_ApplicationManager.h"
#include "unistd.h"
#include "syslog.h"
#include "ace/Task.h"
#include "ace/OS_NS_poll.h"
#include "ACS_SCH_Event.h"
#include "ACS_SCH_Server.h"
#include "ACS_TRA_Logging.h"
#include "ACS_SCH_Logger.h"

#define THR_SPAWNED 0x00000001
#define THR_RUNNING 0x00000002

class ACS_SCH_HA_AppManager: public ACS_APGCC_ApplicationManager {

   private:

        int readWritePipe[2];
        ACS_APGCC_BOOL Is_terminated;
        ACE_thread_t app_thread_id;

   public:
        ACS_SCH_HA_AppManager(const char* daemon_name);
        virtual ~ACS_SCH_HA_AppManager();

        ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performComponentHealthCheck(void);
        ACS_APGCC_ReturnType performComponentTerminateJobs(void);
        ACS_APGCC_ReturnType performComponentRemoveJobs (void);
        ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

        ACS_APGCC_ReturnType svc(void);

   private:

       	ACS_SCH_Server *startSRV;
        ACS_TRA_Logging *ACS_SCH_logging;
};

#endif /* APG_APP1_CLASS_H */ 
