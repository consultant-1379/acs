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
 * @file ACS_XBRM_SRV.cpp
 *
 * @brief
 * Entry point for Service execution 
 *
 * @details
 * Implementation of ACE_TMAIN function
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

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/eventfd.h>

#include "acs_aeh_signalhandler.h"
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_Trace.H>
#include "ACS_TRA_trace.h"
#include "ACS_CS_API.h" //for isMCP
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"

#include "ACS_XBRM_HA_AppManager.h"
#include "ACS_XBRM_Event.h"
#include "ACS_XBRM_Server.h"


/*
struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t   sa_mask;
               int        sa_flags;
               void     (*sa_restorer)(void);
           };
*/

void signal_action_handler(int signal);
ACE_INT32 set_signal_handler(const struct sigaction *signal_action);
void createEvent();
void shutdown();

bool is_noha_service = false;
ACS_XBRM_Server* XBRM_Server_ptr = NULL;
ACS_XBRM_HA_AppManager* haObj = NULL;

ACS_XBRM_EventHandle shutdown_event_handle = 0;
static const char ACS_XBRM_Daemon[] = "acs_xbrmd";
ACS_XBRM_TRACE_DEFINE(ACS_XBRM_Server);


ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR *argv[]){
    ACS_XBRM_TRACE_FUNCTION;
    struct sigaction signal_action;
    signal_action.sa_handler = signal_action_handler;
    signal_action.sa_flags = SA_RESTART;

    sigemptyset(&signal_action.sa_mask);
    syslog(LOG_INFO, "Starting acs_xbrmd main started.. ");

    ACE_INT32 result = set_signal_handler(&signal_action);

    createEvent();

    if(result == 0){
        if(argc > 1){
            if(argc == 2 && (strcmp(argv[1], "--noha") == 0)){
                syslog(LOG_INFO, "Service starting with NOHA");

                ACS_XBRM_Logger::open("XBRM");
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "Service starting with NOHA");
                is_noha_service = true;
                XBRM_Server_ptr = new (std::nothrow) ACS_XBRM_Server();

                if(XBRM_Server_ptr == NULL){
                    syslog(LOG_ERR, "Memory allocated failed for ACS_XBRM_Server");
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Memory allocated failed for ACS_XBRM_Server");
					result = -1;
                }else{
                    result = XBRM_Server_ptr->activate();
                    XBRM_Server_ptr->wait();

                    if(XBRM_Server_ptr != NULL){
                        delete XBRM_Server_ptr;
                        XBRM_Server_ptr = NULL;
                    }
                    syslog(LOG_INFO, "acs_xbrmd stopped in noha mode...");
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Service stopped in noha mode");

                    ACS_XBRM_Event::CloseEvent(shutdown_event_handle);
                }
            }
        }
        
        else{
            //for ha
            syslog(LOG_INFO, "acs_xbrm started with HA");
            acs_aeh_setSignalExceptionHandler(ACS_XBRM_Daemon);

            ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
            haObj = new ACS_XBRM_HA_AppManager(ACS_XBRM_Daemon);
            ACS_XBRM_Logger::open("XBRM");
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "acs_xbrm started with HA");

            if(haObj == 0){
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for ACS_XBRM_HA_AppManager Class");
                ACS_XBRM_Logger::close();
				printf("Memory allocation failed for ACS_XBRM_HA_AppManager Class");
			}else{
                syslog(LOG_INFO, "Starting acs_xbrmd service.. ");
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Starting acs_xbrmd service.. ");
				errorCode = haObj->activate();

                if(errorCode == ACS_APGCC_HA_FAILURE){
                    syslog(LOG_ERR, "acs_xbrmd, HA Activation Failed!!");
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "acs_xbrmd, HA Activation Failed!!");
					result = ACS_APGCC_FAILURE;
                }

                if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
				{
					syslog(LOG_ERR, "acs_xbrmd, HA Application Failed to Gracefullly closed!!");
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "acs_xbrmd, HA Application Failed to Gracefullly closed!!");
					result = ACS_APGCC_FAILURE;
				}

				if (errorCode == ACS_APGCC_HA_SUCCESS)
				{
					syslog(LOG_ERR, "acs_xbrmd, HA Application Gracefully closed!!");
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "acs_xbrmd, HA Application Gracefully closed!!");
					result = ACS_APGCC_SUCCESS;
				}
            }

        }
        
    }

    if (haObj != NULL)
	{
		delete haObj;
		haObj = NULL;
	}
    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "acs_xbrmd was terminated.. ");
    ACS_XBRM_Logger::close();
	syslog(LOG_INFO, "acs_xbrmd was terminated.. ");
	return result;
} //END of MAIN

// @parms signum - SIGNAL
void signal_action_handler(int signum){
    ACS_XBRM_TRACE_FUNCTION;
    if(signum == SIGINT || signum == SIGTERM || signum == SIGTSTP){
        if(is_noha_service){
            if(XBRM_Server_ptr != NULL){
                shutdown();
            }
        }
        else{
            // for ha
            if(haObj != NULL){
                haObj->performComponentRemoveJobs();
                haObj->performComponentTerminateJobs();
            }
        }
    }
}

/**
 * int sigaction(int signum, const struct sigaction *restrict act, struct sigaction *restrict oldact);
 * Signal       Standard   Action  Comment
 * SIGINT       P1990      Term    Interrupt from keyboard
 * SIGTERM      P1990      Term    Termination signal
 * SIGTSTP      P1990      Stop    Stop typed at terminal
*/
ACE_INT32 set_signal_handler(const struct sigaction *signal_action){ 
    ACS_XBRM_TRACE_FUNCTION;
    if(sigaction(SIGINT, signal_action, NULL) == -1){
        syslog(LOG_ERR, "Error occurred while handling SIGINT in acs_xbrmd ");
        return -1;
    }

    if(sigaction(SIGTERM, signal_action, NULL) == -1){
        syslog(LOG_ERR, "Error occurred while handling SIGTERM in acs_xbrmd ");
        return -1;
    }

    if(sigaction(SIGTSTP, signal_action, NULL) == -1){
        syslog(LOG_ERR, "Error occurred while handling SIGTSTP in acs_xbrmd ");
        return -1;
    }

    return 0;
}

void createEvent(){
    ACS_XBRM_TRACE_FUNCTION;
    //use util to use constant of XBRM_Util::EVENT_NAME_SHUTDOWN
    ACS_XBRM_Event::CreateEvent(true, false, ACS_XBRM_UTILITY::EVENT_SHUTDOWN_NAME.c_str());
    ACS_XBRM_Event::CreateEvent(true, false, ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_START_NAME.c_str());
    ACS_XBRM_Event::CreateEvent(true, false, ACS_XBRM_UTILITY::EVENT_SYSTEM_BACKUPS_END_NAME.c_str());
    ACS_XBRM_Event::CreateEvent(true, false, ACS_XBRM_UTILITY::EVENT_EVENT_SCHEDULING_START_NAME.c_str());
    ACS_XBRM_Event::CreateEvent(true, false, ACS_XBRM_UTILITY::EVENT_EVENT_SCHEDULING_END_NAME.c_str());
    syslog(LOG_INFO, "Creating a shutdown event");
}

void shutdown(){
    ACS_XBRM_TRACE_FUNCTION;
    //use util to use constant of XBRM_Util::EVENT_NAME_SHUTDOWN
    shutdown_event_handle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SHUTDOWN_NAME);
    if(shutdown_event_handle >= 0)
        (void) ACS_XBRM_Event::SetEvent(shutdown_event_handle);
}