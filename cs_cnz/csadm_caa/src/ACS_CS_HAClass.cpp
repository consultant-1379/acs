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
#include <syslog.h>
#include "acs_aeh_signalhandler.h"

#include "ACS_CS_HAClass.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_Util.h"
ACS_CS_Trace_TDEF(ACS_CS_HAClass_TRACE);

ACE_THR_FUNC_RETURN svc_run(void *);

HAClass::HAClass(const char* daemon_name, const char* username) :
	ACS_APGCC_ApplicationManager(daemon_name, username) {

	cs_ha_application_thread_id = 0;

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: Creating pipes..."));
	/* create the pipe for shutdown handler */
	Is_terminated = FALSE;
	if ((pipe(readWritePipe)) < 0) {

		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_FATAL,	"ACS_CS_HAClass: ERROR - pipe creation FAILED"));
	}

	if ((fcntl(readWritePipe[0], F_SETFL, O_NONBLOCK)) < 0) {

		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_FATAL,	"ACS_CS_HAClass: ERROR - pipe fcntl on readn"));
	}

	if ((fcntl(readWritePipe[1], F_SETFL, O_NONBLOCK)) < 0) {
		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_FATAL,	"ACS_CS_HAClass: ERROR - pipe fcntl on writen"));
	}

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: Pipes created."));

	csService = 0;

	acs_aeh_setSignalExceptionHandler(ACS_CS_NS::ACS_CS_DAEMON);
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToActiveJobs(
		ACS_APGCC_AMF_HA_StateT previousHAState) {

	/* Check if we have received the ACTIVE State Again.
	 * This means that, our application is already Active and
	 * again we have got a callback from AMF to go active.
	 * Ignore this case anyway. This case should rarely happens
	 */
	ACE_TCHAR state[1] = { 'A' };
	if (ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
		return ACS_APGCC_SUCCESS;

	/* Our application has received state ACTIVE from AMF.
	 * Start off with the activities needs to be performed
	 * on ACTIVE
	 */
	/* Check if it is due to State Transition ( Passive --> Active)*/
	if (ACS_APGCC_AMF_HA_UNDEFINED != previousHAState) {

		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO,	"ACS_CS_HAClass: State Transition. Becoming Active now"));
		syslog(LOG_INFO, "State Transition. Becoming Active now");

		/* Inform the thread to go "active" state */
		write(readWritePipe[1], &state, sizeof(state));
		return ACS_APGCC_SUCCESS;
	}

	/* Handle here what needs to be done when you are given ACTIVE State */
	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: received ACTIVE state assignment!!!"));
	syslog(LOG_INFO, "CS received ACTIVE state assignment!!!");

	/* Create a thread with the state machine (active, passive, stop states)
	 * and start off with "active" state activities.
	 */

	/* spawn thread */
	const ACE_TCHAR* thread_name = "ApplicationThread";
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&svc_run,
			(void *) this, THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, &cs_ha_application_thread_id,
			0, ACE_DEFAULT_THREAD_PRIORITY, -1, 0,
			ACE_DEFAULT_THREAD_STACKSIZE, &thread_name);

	if (threadHandle == -1) {
		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_FATAL,	"ACS_CS_HAClass: ERROR - when creating the application thread"));
		return ACS_APGCC_FAILURE;
	}
	Is_terminated = FALSE;
	write(readWritePipe[1], &state, sizeof(state));

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToPassiveJobs(
		ACS_APGCC_AMF_HA_StateT previousHAState) {

	/* Check if we have received the PASSIVE State Again.
	 * This means that, our application was already Passive and
	 * again we have got a callback from AMF to go passive.
	 * Ignore this case anyway. This case should rarely happens.
	 */

	ACE_TCHAR state[1] = { 'P' };
	if (ACS_APGCC_AMF_HA_STANDBY == previousHAState)
		return ACS_APGCC_SUCCESS;

	/* Our application has received state PASSIVE from AMF.
	 * Check if the state received is due to State Transition.
	 * (Active->Passive).
	 */
	if (ACS_APGCC_AMF_HA_UNDEFINED != previousHAState) {
		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO,	"ACS_CS_HAClass: State Transition. Becoming Passive now"));
		syslog(LOG_INFO, "State Transition. Becoming Passive now");

		/* Inform the thread to go "passive" state*/
		write(readWritePipe[1], &state, sizeof(state));
		return ACS_APGCC_SUCCESS;
	}

	/* Create a thread with the state machine (active, passive, stop states)
	 * and start off the thread with "passive" state.
	 */

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO,	"ACS_CS_HAClass: received STANDBY state assignment!!!"));
	syslog(LOG_INFO, "CS received STANDBY state assignment!!!");

	const ACE_TCHAR* thread_name = "ApplicationThread";
	ACE_HANDLE threadHandle = ACE_Thread_Manager::instance()->spawn(&svc_run,
			(void *) this, THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, &cs_ha_application_thread_id,
			0, ACE_DEFAULT_THREAD_PRIORITY, -1, 0,
			ACE_DEFAULT_THREAD_STACKSIZE, &thread_name);
	if (threadHandle == -1) {
		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_FATAL,	"ACS_CS_HAClass: ERROR - when creating the application thread"));
		//syslog(LOG_ERR, "Error creating the application thread");
		return ACS_APGCC_FAILURE;
	}
	Is_terminated = FALSE;
	write(readWritePipe[1], &state, sizeof(state));
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToQueisingJobs(
		ACS_APGCC_AMF_HA_StateT previousHAState) {
	(void) previousHAState;
	ACE_TCHAR state[1] = { 'S' };
	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: received QUIESING state assignment!!!"));
	syslog(LOG_INFO, "CS received QUIESING state assignment!!!");

	/* Inform the thread to go "stop" state */
	if (!Is_terminated)
		write(readWritePipe[1], &state, sizeof(state));

	ACS_APGCC_ReturnType returnCode = ACS_APGCC_SUCCESS;
	int retval = wait_ha_application_thread_termination();	// TR HX28206 - Wait for HA thread termination before returning success or failure to AMF
	if(retval < 0)
	{
		returnCode = ACS_APGCC_FAILURE;
		syslog(LOG_ERR, "performStateTransitionToQuiescingJobs(): Quiescing assignment failed!");
	}
	else
	{
		returnCode = ACS_APGCC_SUCCESS;
		syslog(LOG_INFO, "performStateTransitionToQuiescingJobs(): Quiescing assignment  successful!");
	}
	return returnCode;
}

ACS_APGCC_ReturnType HAClass::performStateTransitionToQuiescedJobs(
		ACS_APGCC_AMF_HA_StateT previousHAState) {
	(void) previousHAState;
	ACE_TCHAR state[1] = { 'S' }; /* We were Active and now losting Active state due to Lock admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: received QUIESCED state assignment!!!"));
	syslog(LOG_INFO, "CS received QUIESCED state assignment!");

	/* Inform the thread to go "stop" state */
	if (!Is_terminated)
		write(readWritePipe[1], &state, sizeof(state));

	ACS_APGCC_ReturnType returnCode = ACS_APGCC_SUCCESS;
	int retval = wait_ha_application_thread_termination();	// TR HX28206 - Wait for HA thread termination before returning success or failure to AMF
	if(retval < 0)
	{
		returnCode = ACS_APGCC_FAILURE;
		syslog(LOG_ERR, "performStateTransitionToQuiescedJobs(): Quiesced assignment failed!");
	}
	else
	{
		returnCode = ACS_APGCC_SUCCESS;
		syslog(LOG_INFO, "performStateTransitionToQuiescedJobs(): Quiesced assignment  successful!");
	}
	return returnCode;
}

ACS_APGCC_ReturnType HAClass::performComponentHealthCheck(void) {

	/* Application has received health check callback from AMF. Check the
	 * sanity of the application and reply to AMF that you are ok.
	 */
	ACS_CS_TRACE((ACS_CS_HAClass_TRACE, "ACS_CS_HAClass: received healthcheck query!!!"));
	//ACS_CS_FTRACE((LOG_LEVEL_INFO, "ACS_CS_HAClass: received healthcheck query!!!"));
	//syslog(LOG_INFO, "CS received healthcheck query!!!");

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HAClass::performComponentTerminateJobs(void) {
	/* Application has received terminate component callback due to
	 * LOCK-INST admin opreration perform on SU. Terminate the thread if
	 * we have not terminated in performComponentRemoveJobs case or double
	 * check if we are done so.
	 */
	ACE_TCHAR state[1] = { 'S' };

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: Received terminate callback!!!"));
	syslog(LOG_INFO, "CS received terminate callback!!!");

	/* Inform the thread to go "stop" state */
	if (!Is_terminated)
		write(readWritePipe[1], &state, sizeof(state));

	ACS_APGCC_ReturnType returnCode = ACS_APGCC_SUCCESS;
	int retval = wait_ha_application_thread_termination();	// TR HX28206 - Wait for HA thread termination before returning success or failure to AMF
	if(retval < 0)
	{
		returnCode = ACS_APGCC_FAILURE;
		syslog(LOG_ERR, "performComponentTerminateJobs(): terminate jobs failed!");
	}
	else
	{
		returnCode = ACS_APGCC_SUCCESS;
		syslog(LOG_INFO, "performComponentTerminateJobs(): terminate jobs successful!");
	}
	return returnCode;
}

ACS_APGCC_ReturnType HAClass::performComponentRemoveJobs(void) {

	/* Application has received Removal callback. State of the application
	 * is neither Active nor Standby. This is with the result of LOCK admin operation
	 * performed on our SU. Terminate the thread by informing the thread to go "stop" state.
	 */

	ACE_TCHAR state[1] = { 'S' };

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: Application Assignment is removed now"));
	syslog(LOG_INFO, "Application Assignment is removed now");

	/* Inform the thread to go "stop" state */
	if (!Is_terminated)
		write(readWritePipe[1], &state, sizeof(state));

	ACS_APGCC_ReturnType returnCode = ACS_APGCC_SUCCESS;
	int retval = wait_ha_application_thread_termination();	// TR HX28206 - Wait for HA thread termination before returning success or failure to AMF
	if(retval < 0)
	{
		returnCode = ACS_APGCC_FAILURE;
		syslog(LOG_ERR, "performComponentRemoveJobs(): removing jobs failed!");
	}
	else
	{
		returnCode = ACS_APGCC_SUCCESS;
		syslog(LOG_INFO, "performComponentRemoveJobs(): removing jobs successful!");
	}
	return returnCode;
}

ACS_APGCC_ReturnType HAClass::performApplicationShutdownJobs() {
	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass: Shutting down the application"));
	syslog(LOG_ERR, "Shutting down the application");

	ACE_TCHAR state[1] = { 'S' };

	if (!Is_terminated)
		write(readWritePipe[1], &state, sizeof(state));

	ACS_APGCC_ReturnType returnCode = ACS_APGCC_SUCCESS;
	int retval = wait_ha_application_thread_termination();	// TR HX28206 - Wait for HA thread termination before returning success or failure to AMF
	if(retval < 0)
	{
		returnCode = ACS_APGCC_FAILURE;
		syslog(LOG_ERR, "performApplicationShutdownJobs(): shutdown jobs failed!");
	}
	else
	{
		returnCode = ACS_APGCC_SUCCESS;
		syslog(LOG_INFO, "performApplicationShutdownJobs(): shutdown jobs successful!");
	}
	return returnCode;
}

ACE_THR_FUNC_RETURN svc_run(void *ptr) {
	HAClass *haObj = (HAClass*) ptr;
	haObj->svc();
	return 0;
}

ACS_APGCC_ReturnType HAClass::svc() {

	struct pollfd fds[1];
	nfds_t nfds = 1;
	ACE_INT32 ret;
	ACE_Time_Value timeout;
	bool state_transition = false;

	ACE_INT32 retCode;

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass::svc() Starting Application Thread"));
	syslog(LOG_INFO, "Starting Application Thread");

	__time_t secs = 5;
	__suseconds_t usecs = 0;
	timeout.set(secs, usecs);

	fds[0].fd = readWritePipe[0];
	fds[0].events = POLLIN;

	while (true) {
		ret = ACE_OS::poll(fds, nfds, &timeout); // poll can also be a blocking call, such case timeout = 0

		if (ret == -1) {
			if (errno == EINTR)
				continue;
			ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_ERROR, "ACS_CS_HAClass::svc() ERROR - poll Failed - %s, Exiting...", strerror(errno)));
			//syslog(LOG_ERR,"poll Failed - %s, Exiting...",strerror(errno));
			kill(getpid(), SIGTERM);
			return ACS_APGCC_FAILURE;
		}

		if (ret == 0) {
			ACS_CS_TRACE((ACS_CS_HAClass_TRACE, "ACS_CS_HAClass::svc() timeout on ACE_OS::poll"));
			//ACS_CS_FTRACE((LOG_LEVEL_INFO, "ACS_CS_HAClass::svc() timeout on ACE_OS::poll"));
			//syslog(LOG_INFO, "timeout on ACE_OS::poll");
			continue;
		}
		ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_WARN, "ACS_CS_HAClass::svc() Thread:: state assignment received!!!"));
		if (fds[0].revents & POLLIN) {
			ACE_TCHAR ha_state[1] = { '\0' };
			ACE_TCHAR* ptr = (ACE_TCHAR*) &ha_state;
			ACE_INT32 len = sizeof(ha_state);

			while (len > 0) {
				retCode = read(readWritePipe[0], ptr, len);
				if (retCode < 0 && errno != EINTR) {
					ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_ERROR, "ACS_CS_HAClass::svc() ERROR - Read interrupted by error: [%s]",strerror(errno)));
					//syslog(LOG_ERR, "Read interrupted by error: [%s]",strerror(errno));
					kill(getpid(), SIGTERM);
					return ACS_APGCC_FAILURE;
				} else {
					ptr += retCode;
					len -= retCode;
				}
				if (retCode == 0)
					break;
			}

			if (len != 0) {
				ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_ERROR, "ACS_CS_HAClass::svc() ERROR - Improper Msg Len Read [%d]", len));
				//syslog(LOG_ERR, "Improper Msg Len Read [%d]", len);
				kill(getpid(), SIGTERM);
				return ACS_APGCC_FAILURE;
			}
			len = sizeof(ha_state);

			if (ha_state[0] == 'A') {
				ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass::svc() Thread:: Application is Active"));
				syslog(LOG_ERR, "Thread:: Application is Active");
				state_transition = true;

				/* start application work */

				ACS_CS_EventHandle handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);
				(void) ACS_CS_Event::ResetEvent(handle);

				handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);
				(void) ACS_CS_Event::ResetEvent(handle);

				csService = new ACS_CS_ServiceHandler();
				bool result = csService->activate();

				ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO,  "ACS_CS_HAClass::svc() Thread:: csService->activate() == %b", result));

			}

			if (ha_state[0] == 'P') {
				state_transition = false;
				ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass::svc() Thread:: Application is Passive"));
				syslog(LOG_ERR, "Thread:: Application is Passive");

			}

			if (ha_state[0] == 'S') {
				ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass::svc() Request to stop application"));
				syslog(LOG_ERR, "Thread:: Request to stop application");

				// start of HX59136
				if(state_transition)
				{
					ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_WARN, "CS service - state transition from ACTIVE to SHUTDOWN!"));
					if(csService)
					{
						if(!csService->isRunningThread()) {
							ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_WARN, "ACS_CS_HAClass::svc() Shutdown received but ServiceHandler thread has not entered while()"));
							csService->deActivate();
							ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_WARN, "ACS_CS_HAClass::svc() ServiceHandler thread deactivation(exit_) flag set to TRUE!"));
						}
					}
				}
				// end of HX59136

				while ( state_transition && !csService->isRunningThread())
				{
					sleep(1);
					ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_WARN, "ACS_CS_HAClass::svc() ACS_CS_ServiceHandler is still running. Sleep for 1 second"));
				}

				ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass::svc() ACS_CS_ServiceHandler thread_running set to "));

				ACS_CS_EventHandle handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);
				if (handle >= 0)
					(void) ACS_CS_Event::SetEvent(handle);

				handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);
				if (handle >= 0)
					(void) ACS_CS_Event::SetEvent(handle);

				if(csService)
				{                    
					csService->wait(ACS_CS_NS::Daemon_Timeout);
					delete csService;
					csService = NULL;
				}

				break;
			}
		}
	}

	ACS_CS_FTRACE((ACS_CS_HAClass_TRACE, LOG_LEVEL_INFO, "ACS_CS_HAClass::svc() Application Thread Terminated successfully"));
	syslog(LOG_INFO, "Application Thread Terminated successfully");

	return ACS_APGCC_SUCCESS;
}

// TR HX28206 - wait for HA Application thread execution to be completed - exit from svc_run()
int HAClass::wait_ha_application_thread_termination()
{
	int retCode = -1;
	if(cs_ha_application_thread_id == 0)
	{
		syslog(LOG_INFO, "HA Application Thread already terminated!");
		retCode = 1;
		Is_terminated = TRUE;
		return retCode;
	}
	int resThrJoin = ACE_Thread_Manager::instance()->join(cs_ha_application_thread_id);
	if(resThrJoin == 0)
	{
		retCode = 0;
		Is_terminated = TRUE;
		syslog(LOG_INFO, "HA Application Thread successfully joined!");
		cs_ha_application_thread_id = 0;
	}
	else
		syslog(LOG_ERR, "Failed to join HA Application Thread! - join() -> return code == %d", resThrJoin);

	return retCode;
}
