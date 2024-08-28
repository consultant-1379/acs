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
 * @file ACS_XBRM_HA_AppManager.cpp
 *
 * @brief
 * ACS_XBRM_HA_AppManager implementation for HA Service
 *
 * @details
 * ACS_XBRM_HA_AppManager is an implementation for HA Service Management
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

#include "ACS_XBRM_HA_AppManager.h"
#include "ACS_XBRM_Event.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "ACS_XBRM_Utilities.h"

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_HA_AppManager);

ACE_THR_FUNC_RETURN svc_run(void *);

ACS_XBRM_HA_AppManager::ACS_XBRM_HA_AppManager(const char* daemon_name):ACS_APGCC_ApplicationManager(daemon_name)
{
	ACS_XBRM_TRACE_FUNCTION;
    is_terminated = FALSE;

    readWritePipe[0] = 0;
    readWritePipe[1] = 0;
    XBRM_HA_application_thread_id = 0;

    if(pipe(readWritePipe) < 0){
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "PIPE CREATION FAILED");
    }

    if(fcntl(readWritePipe[0], F_SETFL, O_NONBLOCK) < 0){
		ACS_XBRM_LOG(LOG_LEVEL_INFO, "PIPE fcntl on readn");
	}

	if(fcntl(readWritePipe[1], F_SETFL, O_NONBLOCK) < 0){
		ACS_XBRM_LOG(LOG_LEVEL_INFO, "PIPE fcntl on writen");
	}
}

ACS_XBRM_HA_AppManager::~ACS_XBRM_HA_AppManager()
{
	ACS_XBRM_TRACE_FUNCTION;
	if(xbrm_server != NULL)
	{
		delete xbrm_server;
		xbrm_server =NULL;
	}
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState){
	ACS_XBRM_TRACE_FUNCTION;
	is_terminated = FALSE;
	ACE_TCHAR state[1] = {'A'};

	if(ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
		return ACS_APGCC_SUCCESS;

	if(ACS_APGCC_AMF_HA_UNDEFINED != previousHAState){
		syslog(LOG_INFO, "State Transision happend. Becomming Active now");
		write(readWritePipe[1], &state, sizeof(state));
		return ACS_APGCC_SUCCESS;
	}

	syslog(LOG_INFO, "My Application Component received ACTIVE state assignment!!!");
	const ACE_TCHAR* thread_name = "ApplicationThread";
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&svc_run,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&XBRM_HA_application_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);
	if (threadHandle == -1){
		syslog(LOG_ERR, "Error creating the application thread");
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Error Creating Application thread");
		return ACS_APGCC_FAILURE;
	}

	ACE_UINT32 tState;
	ACE_Thread_Manager::instance()->thr_state(XBRM_HA_application_thread_id, tState);

	syslog(LOG_INFO, "Thread state [%u]", tState);

	if(tState == THR_RUNNING || tState == THR_SPAWNED){
		syslog(LOG_INFO, "Posting START to the application thread");
		write(readWritePipe[1], &state, sizeof(state));
		return ACS_APGCC_SUCCESS;
	}
	else
		syslog(LOG_INFO, "Thread state [%u]", tState);
	ACS_XBRM_LOG(LOG_LEVEL_ERROR, "State Transition to Active Job Failed");
	return ACS_APGCC_FAILURE;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState){
    ACS_XBRM_TRACE_FUNCTION;
	is_terminated = FALSE;
    ACE_TCHAR state[1] = {'P'};
    if(ACS_APGCC_AMF_HA_STANDBY == previousHAState)
        return ACS_APGCC_SUCCESS;

    if(ACS_APGCC_AMF_HA_UNDEFINED != previousHAState){
        syslog(LOG_INFO,  "State Transision happend. Becomming Passive now");
        write(readWritePipe[1], &state, sizeof(state));
        return ACS_APGCC_SUCCESS;
    }
    /* 
        Create a thread with the state machine (active, passive, stop states)
	    and start off the thread with "passive" state.
	*/

	syslog(LOG_INFO, "My Application Component received STANDBY state assignment!!!");

	const ACE_TCHAR* thread_name = "ApplicationThread";
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&svc_run,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&XBRM_HA_application_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);
	if(threadHandle == -1){
		syslog(LOG_ERR, "Error creating the application thread");
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Error Creating Application thread");
		return ACS_APGCC_FAILURE;
	}

	/* check if the thread is spawned */
	ACE_UINT32 tState;
	ACE_Thread_Manager::instance()->thr_state(XBRM_HA_application_thread_id, tState);

	syslog(LOG_INFO, "Thread state [%u]", tState);

	if(tState == THR_RUNNING || tState == THR_SPAWNED){
		syslog(LOG_INFO, "Posting START to the application thread");
		write(readWritePipe[1], &state, sizeof(state));
		return ACS_APGCC_SUCCESS;
	}
	else
		syslog(LOG_INFO, "Thread state [%u]", tState);


	ACS_XBRM_LOG(LOG_LEVEL_ERROR, "State Transition to Active Job Failed");
	return ACS_APGCC_FAILURE;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState){
    ACS_XBRM_TRACE_FUNCTION;
    ACE_TCHAR state[1] = {'S'};
    syslog(LOG_INFO, "My Application Component received QUIESING state assignment!!!");
	cout << previousHAState << endl;
    if(!is_terminated){
		write(readWritePipe[1], &state, sizeof(state));
		syslog(LOG_INFO, "Waiting for Application thread to terminate");
		ACE_Thread_Manager::instance()->join(XBRM_HA_application_thread_id);
		is_terminated = TRUE;
	}

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState){
	ACS_XBRM_TRACE_FUNCTION;
	ACE_TCHAR state[1] = {'S'};
	syslog(LOG_INFO, "My Application Component received QUIESCED state assignment!");
	cout << previousHAState << endl;
	if(!is_terminated){
		write(readWritePipe[1], &state, sizeof(state));
		syslog(LOG_INFO, "Waiting for Application thread to terminate");
		ACE_Thread_Manager::instance()->join(XBRM_HA_application_thread_id);
		is_terminated = TRUE;
	}
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performComponentHealthCheck(void){
	ACS_XBRM_TRACE_FUNCTION;
	syslog(LOG_INFO, "My Application Component received healthcheck query!!!");
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performComponentTerminateJobs(void){
	ACS_XBRM_TRACE_FUNCTION;
	syslog(LOG_INFO, "My Application Component received terminate callback!!!");
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performComponentRemoveJobs (void){
	ACS_XBRM_TRACE_FUNCTION;
	ACE_TCHAR state[1] = {'S'};
	syslog(LOG_INFO, "Application Assignment is removed now");

	if(!is_terminated){
		write(readWritePipe[1], &state, sizeof(state));
		syslog(LOG_INFO, "Waiting for Application thread to terminate");
		ACE_Thread_Manager::instance()->join(XBRM_HA_application_thread_id);
		is_terminated = TRUE;
	}
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::performApplicationShutdownJobs(void){
	ACS_XBRM_TRACE_FUNCTION;
	ACE_TCHAR state[1] = {'S'};
	syslog(LOG_ERR, "Shutting down the application");

	if(!is_terminated){
		write(readWritePipe[1], &state, sizeof(state));
		syslog(LOG_INFO, "Waiting for Application thread to terminate");
		ACE_Thread_Manager::instance()->join(XBRM_HA_application_thread_id);
		is_terminated = TRUE;
	}
	return ACS_APGCC_SUCCESS;
}

ACE_THR_FUNC_RETURN svc_run(void *ptr){
	ACS_XBRM_TRACE_FUNCTION;
	ACS_XBRM_HA_AppManager *haObj = (ACS_XBRM_HA_AppManager*) ptr;
	haObj->svc();
	return 0;
}

ACS_APGCC_ReturnType ACS_XBRM_HA_AppManager::svc()
{
	ACS_XBRM_TRACE_FUNCTION;
    struct pollfd fds[1];
	nfds_t nfds = 1; // no of fds
	ACE_INT32 ret;
	ACE_Time_Value timeout;
    ACE_INT32 buffer_length_read;
    /*
    __time_t secs = 5;
	__suseconds_t usecs = 0;
	timeout.set(secs, usecs);
    */

    syslog(LOG_INFO, "Starting HA Application Thread");
    fds[0].fd = readWritePipe[0];
	fds[0].events = POLLIN;

    while(true){
        ret = ACE_OS::poll(fds, nfds, 0); //0 should be replaced with &timeout if needs to specify
        if(ret == -1){ //polling failed
			if(errno == EINTR)
				continue;
			syslog(LOG_ERR,"Poll Failed - %s, Exiting...",strerror(errno));
			kill(getpid(), SIGTERM);
			ACS_XBRM_LOG(LOG_LEVEL_ERROR, "POLL failed - %s, Exiting...", strerror(errno));
			return ACS_APGCC_FAILURE;
        }
        if(ret == 0){
            //poll started
            continue;
        }
        if(fds[0].revents & POLLIN){
            ACE_TCHAR ha_state[1] = {'\0'};
            ACE_TCHAR* ptr = (ACE_TCHAR*) &ha_state;
			ACE_INT32 len = sizeof(ha_state);


            while(len > 0){
                buffer_length_read = read(readWritePipe[0], ptr, len);
                if(buffer_length_read < 0 && errno != EINTR){
                    syslog(LOG_ERR, "Read interrupted by error: [%s]",strerror(errno));
					kill(getpid(), SIGTERM);
					ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Read interrupted by error: [%s]", strerror(errno));
					return ACS_APGCC_FAILURE;
                }
				else{
					ptr += buffer_length_read;
					len -= buffer_length_read;
				}
				if (buffer_length_read == 0)
					break;
            }

			if(len != 0){
				syslog(LOG_ERR, "Improper Msg Len Read [%d]", len);
				ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Improper Msg Len Read [%d]", len);
				kill(getpid(), SIGTERM);
				return ACS_APGCC_FAILURE;
			}
			len = sizeof(ha_state);

            if (ha_state[0] == 'A'){
				syslog(LOG_INFO, "Thread:: Application is Active");
				if(xbrm_server == NULL)
					xbrm_server = new (std::nothrow) ACS_XBRM_Server();
				xbrm_server->activate();
			}

			if (ha_state[0] == 'P'){
				syslog(LOG_ERR, "Thread:: Application is Passive");
				/* do nothing */
			}   

            if(ha_state[0] == 'S'){
                syslog(LOG_ERR, "Thread:: Request to stop application");
				//event name to be replaced
				ACS_XBRM_EventHandle handle = ACS_XBRM_Event::OpenNamedEvent(ACS_XBRM_UTILITY::EVENT_SHUTDOWN_NAME);
				if (handle >= 0)
					(void) ACS_XBRM_Event::SetEvent(handle);
				if(xbrm_server != NULL){  
					xbrm_server->wait();
				}
				break; 
            }
        }
    }

    syslog(LOG_INFO, "HA Application Thread Terminated successfully");
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "HA Application Thread Terminated successfully");
	return ACS_APGCC_SUCCESS;
}
