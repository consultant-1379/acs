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
 * @file ACS_XBRM_HA_AppManager.h
 *
 * @brief
 * ACS_XBRM_HA_AppManager Class for HA Service
 *
 * @details
 * ACS_XBRM_HA_AppManager is an interface for HA Service Management
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


#ifndef ACS_XBRM_HA_APPMANAGER_H
#define ACS_XBRM_HA_APPMANAGER_H

#include "ACS_APGCC_ApplicationManager.h"
#include "unistd.h"
#include "syslog.h"
#include "ace/Task.h"
#include "ace/OS_NS_poll.h"
#include "ACS_TRA_Logging.h"
#include "ACS_XBRM_Server.h"

#define THR_SPAWNED 0x00000001
#define THR_RUNNING 0x00000002

class ACS_XBRM_HA_AppManager : public ACS_APGCC_ApplicationManager{
    private:
        int readWritePipe[2];
        ACE_thread_t XBRM_HA_application_thread_id;
        ACS_APGCC_BOOL is_terminated;
        ACS_XBRM_Server *xbrm_server;
    public:
        ACS_XBRM_HA_AppManager(const char* daemon_name);
        virtual ~ACS_XBRM_HA_AppManager();
        ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        ACS_APGCC_ReturnType performComponentHealthCheck(void);
        ACS_APGCC_ReturnType performComponentTerminateJobs(void);
        ACS_APGCC_ReturnType performComponentRemoveJobs (void);
        ACS_APGCC_ReturnType performApplicationShutdownJobs(void);
        
        ACS_APGCC_ReturnType svc(void);
};
#endif