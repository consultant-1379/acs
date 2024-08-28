/*
 * ACS_TRAPDS_HA_AppManager.h
 *
 *  Created on: Mar 20, 2012
 *      Author: qrenbor
 */

#ifndef ACS_TRAPDS_HA_AppManager_H_
#define ACS_TRAPDS_HA_AppManager_H_

#include "ACS_APGCC_ApplicationManager.h"
#include "unistd.h"
#include "syslog.h"
#include "ace/Task.h"
#include "ace/OS_NS_poll.h"
#include "ACS_TRAPDS_Server.h"
#include "ACS_TRAPDS_Event.h"
#include "ACS_TRAPDS_Util.h"


#define THR_SPAWNED 0x00000001
#define THR_RUNNING 0x00000002

class ACS_TRAPDS_HA_AppManager: public ACS_APGCC_ApplicationManager {

   private:

        int readWritePipe[2];
        ACS_APGCC_BOOL Is_terminated;
        ACE_thread_t app_thread_id;

   public:
        ACS_TRAPDS_HA_AppManager(const char* daemon_name);
        virtual ~ACS_TRAPDS_HA_AppManager();

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

        ACS_TRAPDS_Server *startSRV;

};

#endif /* ACS_TRAPDS_HA_AppManager */
