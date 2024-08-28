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
#include <sstream>
#include <syslog.h>

#include <ace/OS.h>
#include <ace/Manual_Event.h>
//#include <ace/Synch.h>
//#include <ace/SOCK_Stream.h>

#include "ACS_APSESH_Service.h"
#include "ACS_APSESH_EventMgr.h"
#include "ACS_APSESH_Main.h"
#include "ACS_APSESH_Trace.h"

ACS_APSESH_Main* g_mainPtr;

class apsesh_main_thread_handler
{
 private:
	bool m_apsesh_thread_running;
	ACE_Manual_Event m_apsesh_main_thread_has_stopped_;

 public:
	//---------------------------------------------------------------------------------------------------------------
	apsesh_main_thread_handler()
	{
		m_apsesh_thread_running = false;
	}

	//---------------------------------------------------------------------------------------------------------------
	int start_apsesh_main_thread()
	{
		if (!m_apsesh_thread_running)
		{
			if ( ACE_Thread::spawn(ACE_THR_FUNC(apsesh_main_thread),this,THR_NEW_LWP|THR_DETACHED) < 0 ) {
				// Failed to start thread !!
				return -1;
			}
			m_apsesh_thread_running = true;
		}
		return 0;
	}

	//---------------------------------------------------------------------------------------------------------------
	static void* apsesh_main_thread(void* this_ptr)
	{
		apsesh_main_thread_handler* this_ = reinterpret_cast<apsesh_main_thread_handler*>( this_ptr );

		APSESH_Tracer::instance()->trace(__FUNCTION__, "RunApsesh");

		g_mainPtr->runApsesh();

		APSESH_Tracer::instance()->trace(__FUNCTION__, "Exit");

		// Indicate that we have finished
		this_->m_apsesh_main_thread_has_stopped_.signal();

		return 0;
	}

	//---------------------------------------------------------------------------------------------------------------
	void kill_apsesh_main_thread()
	{
		if (m_apsesh_thread_running)
		{
			APSESH_Tracer::instance()->trace(__FUNCTION__, "About to kill RunApsesh");

			g_mainPtr->keepRunning = false;
			// Wait for apsesh main thread to finish!
			this->m_apsesh_main_thread_has_stopped_.wait();
			m_apsesh_thread_running = false;
			APSESH_Tracer::instance()->trace(__FUNCTION__, "RunApsesh killed, leaving...");
			ACE_OS::sleep(1); // A little timeout in order to let work be finished before delete
		}
	}
};

apsesh_main_thread_handler apsesh_main_;

ACE_THR_FUNC_RETURN svc_run(void *);

HAClass::HAClass(const char* daemon_name, const char* username) :
      ACS_APGCC_ApplicationManager(daemon_name, username)
{
	m_threadOn = false;
	m_IsInstantiated = false;

	m_apsesh_ha_application_thread_id = 0;

	m_eventMgr = new ACS_APSESH_EventMgr(ACS_APSESH_Main::ACS_APSESH_PROCESS_NAME);

	g_mainPtr = new ACS_APSESH_Main();

	/* create the pipe for shutdown handler */
	if ((pipe(readWritePipe)) < 0)
	{
		APSESH_Tracer::instance()->trace(__FUNCTION__, 	"ERROR - pipe creation FAILED", true, LOG_LEVEL_ERROR);
	}

	if ((fcntl(readWritePipe[READ_PIPE], F_SETFL, O_NONBLOCK)) < 0)
	{
		APSESH_Tracer::instance()->trace(__FUNCTION__, 	"ERROR - pipe fcntl on readn", true, LOG_LEVEL_ERROR);
	}

	if ((fcntl(readWritePipe[WRITE_PIPE], F_SETFL, O_NONBLOCK)) < 0)
	{
		APSESH_Tracer::instance()->trace(__FUNCTION__, 	"ERROR - pipe fcntl on writen", true, LOG_LEVEL_ERROR);
	}
}

HAClass::~HAClass()
{
	delete m_eventMgr;
	delete g_mainPtr;
}

ACE_thread_t HAClass::get_application_thread_handle(void)
{
	return m_apsesh_ha_application_thread_id;
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	ACE_TCHAR state[1] = { 'A' };
	APSESH_Tracer::instance()->trace(__FUNCTION__, 	"received performStateTransitionToActiveJobs callback", true, LOG_LEVEL_INFO);


	/* Check if we have received the ACTIVE State Again.
	 * This means that, our application is already Active and
	 * again we have got a callback from AMF to go active.
	 * Ignore this case anyway. This case should rarely happens
	 */
	if (ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
	{
		APSESH_Tracer::instance()->trace(__FUNCTION__, 	"State Transision. HA state is already active", true, LOG_LEVEL_WARN);
		return ACS_APGCC_SUCCESS;
	}

	/* Our application has received state ACTIVE from AMF.
	 * Start off with the activities needs to be performed
	 * on ACTIVE
	 */
	/* Check if it is due to State Transition ( Passive --> Active)*/
	if (ACS_APGCC_AMF_HA_STANDBY == previousHAState)
	{
		syslog(LOG_INFO, "State Transition. Becoming Active now");

		/* Inform the thread to go "active" state */
		write(readWritePipe[1], &state, sizeof(state));

		APSESH_Tracer::instance()->trace(__FUNCTION__, 	"State Transition. Becoming Active now", true, LOG_LEVEL_WARN);
		return ACS_APGCC_SUCCESS;
	}

	/* Handle here what needs to be done when you are given ACTIVE State */
	syslog(LOG_INFO, "APSESH received ACTIVE state assignment!!!");

	/* Create a thread with the state machine (active, passive, stop states)
	 * and start off with "active" state activities.
	 */

	/* spawn thread */
	const ACE_TCHAR* thread_name = "APSESHThread";
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&svc_run,
																	(void *) this,
																	THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
																	&m_apsesh_ha_application_thread_id,
																	0,
																	ACE_DEFAULT_THREAD_PRIORITY,
																	-1,
																	0,
																	ACE_DEFAULT_THREAD_STACKSIZE,
																	&thread_name);

	if (ACE_INVALID_HANDLE == threadHandle)
	{
		APSESH_Tracer::instance()->trace(__FUNCTION__, 	"ERROR - when creating the application thread", true, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}

	write(readWritePipe[1], &state, sizeof(state));
	APSESH_Tracer::instance()->trace(__FUNCTION__,  "received ACTIVE state assignment!!!", true, LOG_LEVEL_WARN);
	m_IsInstantiated = true;
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	ACE_TCHAR state[1] = { 'P' };
	APSESH_Tracer::instance()->trace(__FUNCTION__, 	"received performStateTransitionToPassiveJobs callback", true, LOG_LEVEL_INFO);

	/* Check if we have received the PASSIVE State Again.
	 * This means that, our application was already Passive and
	 * again we have got a callback from AMF to go passive.
	 * Ignore this case anyway. This case should rarely happens.
	 */
	if (ACS_APGCC_AMF_HA_STANDBY == previousHAState)
	{
		APSESH_Tracer::instance()->trace(__FUNCTION__, 	"State Transision. HA state is already passive", true, LOG_LEVEL_WARN);
		return ACS_APGCC_SUCCESS;
	}

	/* Our application has received state PASSIVE from AMF.
	 * Check if the state received is due to State Transistion.
	 * (Active->Passive).
	 */
	if (ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
	{
		syslog(LOG_INFO, "APSESH received PASSIVE state assignment!!!");

		/* Inform the thread to go "passive" state*/
		write(readWritePipe[1], &state, sizeof(state));

		APSESH_Tracer::instance()->trace(__FUNCTION__, "State Transition. Becoming Passive now", true, LOG_LEVEL_WARN);
		return ACS_APGCC_SUCCESS;
	}

	/* Create a thread with the state machine (active, passive, stop states)
	 * and start off the thread with "passive" state.
	 */
	const ACE_TCHAR* thread_name = "APSESHThread";

	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&svc_run,
																	(void *) this,
																	THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
																	&m_apsesh_ha_application_thread_id,
																	0,
																	ACE_DEFAULT_THREAD_PRIORITY,
																	-1,
																	0,
																	ACE_DEFAULT_THREAD_STACKSIZE,
																	&thread_name);

	if (ACE_INVALID_HANDLE == threadHandle)
	{
		APSESH_Tracer::instance()->trace(__FUNCTION__, "ERROR - when creating the application thread", true, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}

	write(readWritePipe[1], &state, sizeof(state));
	APSESH_Tracer::instance()->trace(__FUNCTION__, "received STANDBY state assignment!!!", true, LOG_LEVEL_WARN);
	m_IsInstantiated = true;
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToQueisingJobs(	ACS_APGCC_AMF_HA_StateT previousHAState)
{
	// To avoid warning about unused parameter
	ACE_UNUSED_ARG(previousHAState);

	syslog(LOG_INFO, "received transition to state Queising.");
	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	*/
	// Inform the thread to go "stop" state
	ACS_APGCC_ReturnType result = stopWorkerThread();

	syslog(LOG_INFO, "transition to state Queising done.");

	return result;
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToQuiescedJobs(	ACS_APGCC_AMF_HA_StateT previousHAState)
{
	// To avoid warning about unused parameter
	ACE_UNUSED_ARG(previousHAState);

	syslog(LOG_INFO, "received transition to state Quiesced.");
	/* We were Active and now losting Active state due to Lock admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	ACS_APGCC_ReturnType result = stopWorkerThread();

	syslog(LOG_INFO, "transition to state Quiesced done.");

	return result;
}

ACS_APGCC_ReturnType HAClass::performComponentHealthCheck(void)
{
	ACS_APGCC_ReturnType result = ACS_APGCC_SUCCESS;

	/* Application has received health check callback from AMF. Check the
	 * sanity of the application and reply to AMF that you are OK.
	 */
	if(m_IsInstantiated && !m_threadOn)
	{
		syslog(LOG_INFO, "APSESH worker thread not running!");

		APSESH_Tracer::instance()->trace(__FUNCTION__,  "APSESH worker thread not running!", true, LOG_LEVEL_INFO);

		result = ACS_APGCC_FAILURE;
	}
	return result;
}

ACS_APGCC_ReturnType HAClass::performComponentTerminateJobs(void)
{
	/* Application has received terminate component callback due to
	 * LOCK-INST admin opreration perform on SU. Terminate the thread if
	 * we have not terminated in performComponentRemoveJobs case or double
	 * check if we are done so.
	 */
	syslog(LOG_INFO, "received transition to state Terminate.");

	ACS_APGCC_ReturnType result = stopWorkerThread();

	syslog(LOG_INFO, "transition to state Terminate done.");

	return result;
}

ACS_APGCC_ReturnType HAClass::performComponentRemoveJobs(void)
{
	/* Application has received Removal callback. State of the application
	 * is neither Active nor Standby. This is with the result of LOCK admin operation
	 * performed on our SU. Terminate the thread by informing the thread to go "stop" state.
	 */

	syslog(LOG_INFO, "received transition to state Remove.");

	ACS_APGCC_ReturnType result = stopWorkerThread();

	syslog(LOG_INFO, "transition to state Remove done.");

	return result;
}

ACS_APGCC_ReturnType HAClass::performApplicationShutdownJobs()
{
	syslog(LOG_INFO, "received transition to state Shutdown.");

	ACS_APGCC_ReturnType result = stopWorkerThread();

	syslog(LOG_INFO, "transition to state Shutdown done.");

	return result;
}

/*======================================================================
	ROUTINE: stopWorkerThread
 ===================================================================== */
ACS_APGCC_ReturnType HAClass::stopWorkerThread()
{
	ACS_APGCC_ReturnType result = ACS_APGCC_SUCCESS;

	// Check the worker thread state
	if ( m_IsInstantiated )
	{
		m_IsInstantiated = false;
		// State Stop
		ACE_TCHAR state[1] = {'S'};
		syslog(LOG_INFO, "APSESH stopWorkerThread send stop signal.");

		// Inform the thread to go "stop" state
		// Write stop to the pipe
		if( write(readWritePipe[WRITE_PIPE], &state, sizeof(state)) <= 0 )
		{
			APSESH_Tracer::instance()->trace(__FUNCTION__, "APSESH stopWorkerThread pipe error", true, LOG_LEVEL_ERROR);
			syslog(LOG_INFO, "stopWorkerThread pipe error:<%m>");
			m_threadOn = false;
			result = ACS_APGCC_FAILURE;
		}
		else
		{
			int grpId = 0;
			ACE_Thread_Manager::instance()->get_grp(m_apsesh_ha_application_thread_id, grpId);

			syslog(LOG_INFO, "stopWorkerThread waiting on worker thread id<%d> termination", grpId);
			// Waiting on worker thread termination
			ACE_Thread_Manager::instance()->wait_grp(grpId);
		}
		syslog(LOG_INFO, "stopWorkerThread worker thread terminated");
	}

	return result;
}

ACE_THR_FUNC_RETURN svc_run(void *ptr)
{
	HAClass* haObj = static_cast<HAClass*>(ptr);
	haObj->m_threadOn = true;
	haObj->svc();
	haObj->m_threadOn = false;
	return 0;
}

ACS_APGCC_ReturnType HAClass::svc()
{
	ACS_APGCC_ReturnType result = ACS_APGCC_SUCCESS;
	struct pollfd fds[1];
	nfds_t nfds = 1;
	ACE_INT32 ret;
	ACE_Time_Value timeout;

	ACE_INT32 retCode;

	APSESH_Tracer::instance()->trace(__FUNCTION__,  "ACS_APSESH_HAClass::svc() Starting Application Thread", true, LOG_LEVEL_WARN);

	__time_t secs = 5;
	__suseconds_t usecs = 0;
	timeout.set(secs, usecs);

	fds[0].fd = readWritePipe[0];
	fds[0].events = POLLIN;

	while(m_threadOn)
	{
		ret = ACE_OS::poll(fds, nfds, &timeout); // poll can also be a blocking call, such case timeout = 0

		if (ret == -1)
		{
			if (errno == EINTR)
				continue;

			std::ostringstream o;
			o << "ACS_APSESH_HAClass::svc() ERROR - poll Failed - %s, Exiting..." << strerror(errno);
			APSESH_Tracer::instance()->trace(__FUNCTION__, o.str().c_str(), true, LOG_LEVEL_ERROR);
			result = ACS_APGCC_FAILURE;
			break;
		}

		if (ret == 0)
		{
			APSESH_Tracer::instance()->trace(__FUNCTION__,  "ACS_APSESH_HAClass::svc() timeout on ACE_OS::poll");
			continue;
		}

		if (fds[0].revents & POLLIN)
		{
			ACE_TCHAR ha_state[1] = { '\0' };
			ACE_TCHAR* ptr = (ACE_TCHAR*) &ha_state;
			ACE_INT32 len = sizeof(ha_state);

			while (len > 0)
			{
				retCode = read(readWritePipe[READ_PIPE], ptr, len);
				if (retCode < 0 && errno != EINTR)
				{
				   std::ostringstream o;
				   o << "ACS_APSESH_HAClass::svc() ERROR - Read interrupted by error: [%s]" << strerror(errno);
				   APSESH_Tracer::instance()->trace(__FUNCTION__, o.str().c_str(), true, LOG_LEVEL_ERROR);
				   break;
				}
				else
				{
					ptr += retCode;
					len -= retCode;
				}

				if (retCode == 0)
					break;
			}

			if (len != 0)
			{
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1, "ACS_APSESH_HAClass::svc() ERROR - Improper Msg Len Read [%d]", len);
				APSESH_Tracer::instance()->trace(__FUNCTION__,  msg, true, LOG_LEVEL_ERROR);
				result = ACS_APGCC_FAILURE;
				break;
			}

			len = sizeof(ha_state);

			if (ha_state[0] == 'A')
			{
				APSESH_Tracer::instance()->trace(__FUNCTION__, 	"ACS_APSESH_HAClass::svc() Application is Active. Starting APSESH service...");

				/* start application work */
				// Run service
				int resCode = apsesh_main_.start_apsesh_main_thread();

				if (resCode == -1)
				{
					APSESH_Tracer::instance()->trace(__FUNCTION__, 	"ACS_APSESH_HAClass::svc() Failed to start APSESH service!", true, LOG_LEVEL_ERROR);
					result = ACS_APGCC_FAILURE;
					break;
				}
				continue;
			}

			if (ha_state[0] == 'P')
			{
				APSESH_Tracer::instance()->trace(__FUNCTION__,  "ACS_APSESH_HAClass::svc() Application is Passive", true, LOG_LEVEL_WARN);
				continue;
				/* do nothing */
			}

			if (ha_state[0] == 'S')
			{
				APSESH_Tracer::instance()->trace(__FUNCTION__,  "ACS_APSESH_HAClass::svc() Request to stop application", true, LOG_LEVEL_WARN);
				break;
			}
		}
	}

	// Request to stop the thread, perform the graceful activities here
	// it is safe call this method in any case
	apsesh_main_.kill_apsesh_main_thread();

	APSESH_Tracer::instance()->trace(__FUNCTION__,  "ACS_APSESH_HAClass::svc() Application Thread Terminated successfully", true, LOG_LEVEL_WARN);

	return result;
}
