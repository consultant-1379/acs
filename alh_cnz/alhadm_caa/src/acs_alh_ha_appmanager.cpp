/*
 * acs_alh_appmanager.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: efabron
 */

#include "acs_alh_ha_appmanager.h"
#include "acs_alh_log.h"

extern ACE_THR_FUNC_RETURN run_alh_daemon_work(void *);   	// defined in acs_alh_main.cpp
extern void stop_alh_daemon_work();							// defined in acs_alh_main.cpp
extern bool alh_is_stopping;								// defined in acs_alh_main.cpp

ACE_THR_FUNC_RETURN svc_run(void *);

//========================================================================================
//	Constructor
//========================================================================================

acs_alh_ha_appmanager::acs_alh_ha_appmanager(const char* daemon_name) : ACS_APGCC_ApplicationManager(daemon_name)
{
	Is_terminated = FALSE;

	alh_worker_thread_id = 0;
	alh_ha_application_thread_id = 0;
	alh_worker_thread_terminated = false;
	alh_worker_thread_exit_code = -1;

	/* create the pipe for shutdown handler */
	if ( (pipe(readWritePipe)) < 0) {
		syslog(LOG_ERR, "pipe creation FAILED");
	}

	if ( (fcntl(readWritePipe[0], F_SETFL, O_NONBLOCK)) < 0) {
		syslog(LOG_ERR, "pipe fcntl on readn");
	}

	if ( (fcntl(readWritePipe[1], F_SETFL, O_NONBLOCK)) < 0) {
		syslog(LOG_ERR, "pipe fcntl on writen");
	}
}



//========================================================================================
//	performStateTransitionToActiveJobs callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	syslog(LOG_INFO, "acs_alh_ha_appmanager::performStateTransitionToActiveJobs() invoked !" );

	/* Check if we have received the ACTIVE State again.
	 * This means that, our application is already Active and
	 * again we have got a callback from AMF to go active.
	 * Ignore this case anyway. This case should rarely happen
	 */
	ACE_TCHAR state[1] = {'A'};
	if(ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
		return ACS_APGCC_SUCCESS;

	/* Our application has received state ACTIVE from AMF.
	 * Start off with the activities needs to be performed
	 * on ACTIVE
	 */

	/* Check if it is due to State Transition ( Passive --> Active)*/
	if ( ACS_APGCC_AMF_HA_UNDEFINED != previousHAState )
	{
		syslog(LOG_INFO, "State Transition happened. Becoming Active now");
		/* Inform the HA Application thread to go "active" state */
		write(readWritePipe[1], &state, sizeof(state));
		return ACS_APGCC_SUCCESS;
	}

	/* Handle here what needs to be done (only the first time) when you are given ACTIVE State */
	syslog(LOG_INFO, "ALH Daemon received ACTIVE state assignment!!!");

	/* Create a thread (let's call it "HA Application Thread") with the state machine (active, passive, stop states)
	 * and start off with "active" state activities.
	 */

	/* spawn thread */
	const ACE_TCHAR* thread_name = "ApplicationThread";
	int call_result = ACE_Thread_Manager::instance()->spawn(&svc_run,
									(void *)this ,
									THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
									& alh_ha_application_thread_id,
									0,
									ACE_DEFAULT_THREAD_PRIORITY,
									-1,
									0,
									ACE_DEFAULT_THREAD_STACKSIZE,
									&thread_name);
	if (call_result == -1)
	{
		syslog(LOG_ERR, "Error creating HA Application Thread");
		return ACS_APGCC_FAILURE;
	}

	write(readWritePipe[1], &state, sizeof(state));

	return ACS_APGCC_SUCCESS;
}



//========================================================================================
//	performStateTransitionToPassiveJobs callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	syslog(LOG_INFO, "acs_alh_ha_appmanager::performStateTransitionToPssiveJobs() invoked !" );

	/* Check if we have received the PASSIVE State Again.
	 * This means that, our application was already Passive and
	 * again we have got a callback from AMF to go passive.
	 * Ignore this case anyway. This case should rarely happen.
	 */

	ACE_TCHAR state[1] = {'P'};
	if(ACS_APGCC_AMF_HA_STANDBY == previousHAState)
		return ACS_APGCC_SUCCESS;

	/* Our application has received state PASSIVE from AMF.
	 * Check if the state received is due to State Transistion.
	 * (Active->Passive).
	 */
	if ( ACS_APGCC_AMF_HA_UNDEFINED != previousHAState ){
		syslog(LOG_INFO, "State Transition happened. Becoming Passive now");
		 /* Inform the thread to go "passive" state*/
		write(readWritePipe[1], &state, sizeof(state));
		return ACS_APGCC_SUCCESS;
	}

	/* Create a thread (let's call it "HA Application Thread) with the state machine (active, passive, stop states)
	 * and start off the thread with "passive" state.
	 */

	syslog(LOG_INFO, "ALH Daemon received STANDBY state assignment!!!");

	const ACE_TCHAR* thread_name = "HA_Application_Thread";
	int call_result = ACE_Thread_Manager::instance()->spawn(&svc_run,
									(void *)this ,
									THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
									& alh_ha_application_thread_id,
									0,
									ACE_DEFAULT_THREAD_PRIORITY,
									-1,
									0,
									ACE_DEFAULT_THREAD_STACKSIZE,
									&thread_name);
	if (call_result == -1)
	{
		syslog(LOG_ERR, "Error creating HA Application Thread");
		return ACS_APGCC_FAILURE;
	}

	write(readWritePipe[1], &state, sizeof(state));
	return ACS_APGCC_SUCCESS;
}


//========================================================================================
//	performStateTransitionToQueisingJobs callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	syslog(LOG_INFO, "acs_alh_ha_appmanager::performStateTransitionToQueisingJobs() invoked !" );

	(void)previousHAState;
	ACE_TCHAR state[1] = {'S'};

	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	syslog(LOG_INFO, "ALH Daemon received QUIESING state assignment!!!");

	/* Inform the thread to go "stop" state */
	if ( !Is_terminated )
		write(readWritePipe[1], &state, sizeof(state));
	Is_terminated = TRUE;

	return ACS_APGCC_SUCCESS;
}


//========================================================================================
//	performStateTransitionToQuiescedJobs callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	syslog(LOG_INFO, "acs_alh_ha_appmanager::performStateTransitionToQuiescedJobs() invoked !" );

	(void)previousHAState;
	ACE_TCHAR state[1] = {'S'};

	/* We were Active and now losting Active state due to Lock admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	syslog(LOG_INFO, "ALH Daemon received QUIESCED state assignment!");

	/* Inform the thread to go "stop" state */
	if ( !Is_terminated )
		write(readWritePipe[1], &state, sizeof(state));
	Is_terminated = TRUE;

	// wait for HA application thread termination
	int retval = wait_ha_application_thread_termination(false);
	if(retval < 0)
	{
		syslog(LOG_ERR, "an error occurred while waiting for HA Application Thread termination. ERRCODE == '%d'!", retval);
		return ACS_APGCC_FAILURE;
	}

	return ACS_APGCC_SUCCESS;
}


//========================================================================================
//	performComponentHealthCheck callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performComponentHealthCheck(void)
{

	/* Application has received health check callback from AMF. Check the
	 * sanity of the application and reply to AMF that you are ok.
	 */
	//syslog(LOG_INFO, "ALH Daemon received healthcheck query!!!");

	if(alh_worker_thread_terminated && (alh_worker_thread_exit_code != 0))
	{
		syslog(LOG_INFO, "ALH Daemon worker thread terminated with exit code < %d > !!!", alh_worker_thread_exit_code);

		return ACS_APGCC_FAILURE;
	}


	return ACS_APGCC_SUCCESS;
}


//========================================================================================
//	performComponentTerminateJobs callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performComponentTerminateJobs(void)
{
	/* Application has received terminate component callback due to
	 * LOCK-INST admin operation performed on SU. Terminate the thread if
	 * we have not terminated in performComponentRemoveJobs case or double
	 * check if we are done so.
	 */

	ACE_TCHAR state[1] = {'S'};

	syslog(LOG_INFO, "ALH Daemon received terminate callback!!!");
	/* Inform the thread to go "stop" state */
	if ( !Is_terminated )
		write(readWritePipe[1], &state, sizeof(state));

	Is_terminated = FALSE;

	// wait for HA application thread termination
	int retval = wait_ha_application_thread_termination(false);
	if(retval < 0)
	{
		syslog(LOG_ERR, "an error occurred while waiting for HA Application Thread termination. ERRCODE == '%d'!", retval);
		return ACS_APGCC_FAILURE;
	}

	return ACS_APGCC_SUCCESS;
}


//========================================================================================
//	performComponentRemoveJobs callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performComponentRemoveJobs(void)
{

	/* Application has received Removal callback. State of the application
	 * is neither Active nor Standby. This is with the result of LOCK admin operation
	 * performed on our SU. Terminate the thread by informing the thread to go "stop" state.
	 */

	ACE_TCHAR state[1] = {'S'};

	syslog(LOG_INFO, "Application Assignment is removed now");
	/* Inform the thread to go "stop" state */
	if ( !Is_terminated )
		write(readWritePipe[1], &state, sizeof(state));

	Is_terminated = FALSE;

	// wait for HA application thread termination
	int retval = wait_ha_application_thread_termination(false);
	if(retval < 0)
	{
		syslog(LOG_ERR, "an error occurred while waiting for HA Application Thread termination. ERRCODE == '%d'!", retval);
		return ACS_APGCC_FAILURE;
	}

	return ACS_APGCC_SUCCESS;
}


//========================================================================================
//	performApplicationShutdownJobs callback
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::performApplicationShutdownJobs() {

	syslog(LOG_ERR, "Shutting down the application");
	ACE_TCHAR state[1] = {'S'};

	if ( !Is_terminated )
		write(readWritePipe[1], &state, sizeof(state));

	Is_terminated = FALSE;

	// wait for HA application thread termination
	int retval = wait_ha_application_thread_termination(false);
	if(retval < 0)
	{
		syslog(LOG_ERR, "an error occurred while waiting for HA Application Thread termination. ERRCODE == '%d'!", retval);
		return ACS_APGCC_FAILURE;
	}

	return ACS_APGCC_SUCCESS;
}


//========================================================================================
//	svc_run method
//========================================================================================

ACE_THR_FUNC_RETURN svc_run(void *ptr){
	acs_alh_ha_appmanager *haObj = (acs_alh_ha_appmanager*) ptr;
	haObj->svc();
	return 0;
}


//========================================================================================
//	svc method
//========================================================================================

ACS_APGCC_ReturnType acs_alh_ha_appmanager::svc()
{
	struct pollfd fds[1];
	nfds_t nfds = 1;
	ACE_INT32 ret;
	ACE_Time_Value timeout;
	ACE_INT32 retCode;

	acs_alh_log log;

	syslog(LOG_INFO, "Starting HA Application Thread");
	log.write(LOG_LEVEL_DEBUG,"Starting HA Application Thread");

	__time_t secs = 5;
	__suseconds_t usecs = 0;
	timeout.set(secs, usecs);

	fds[0].fd = readWritePipe[0];
	fds[0].events = POLLIN;

	while(true)
	{
		ret = ACE_OS::poll(fds, nfds, &timeout); // poll can also be a blocking call, such case timeout = 0

		if (ret == -1)
		{
			if (errno == EINTR)
				continue;
			syslog(LOG_ERR,"poll Failed - %s, Exiting...",strerror(errno));
			kill(getpid(), SIGTERM);
			return ACS_APGCC_FAILURE;
		}

		if (ret == 0)
		{
			//syslog(LOG_INFO, "timeout on ACE_OS::poll");
			continue;
		}

		if (fds[0].revents & POLLIN){
			ACE_TCHAR ha_state[1] = {'\0'};
			ACE_TCHAR* ptr = (ACE_TCHAR*) &ha_state;
			ACE_INT32 len = sizeof(ha_state);

			while (len > 0)
			{
				retCode=read(readWritePipe[0], ptr, len);
				if ( retCode < 0 && errno != EINTR)
				{
					syslog(LOG_ERR, "Read interrupted by error: [%s]",strerror(errno));
					kill(getpid(), SIGTERM);
					return ACS_APGCC_FAILURE;
				}
				else
				{
					ptr += retCode;
					len -= retCode;
				}

				if (retCode == 0)
					break;
        	}

			if ( len != 0)
			{
                syslog(LOG_ERR, "Improper Msg Len Read [%d]", len);
                kill(getpid(), SIGTERM);
                return ACS_APGCC_FAILURE;
        	}
			len = sizeof(ha_state);

			if (ha_state[0] == 'A'){
				syslog(LOG_INFO, "HA Application Thread: Application is Active");
				log.write(LOG_LEVEL_DEBUG, "HA Application Thread: Application is Active");
				alh_worker_thread_id = 0;
				alh_worker_thread_terminated = false;
				alh_is_stopping = false;
			}
			else if (ha_state[0] == 'P'){
				syslog(LOG_INFO, "HA Application Thread: Application is Passive");
				log.write(LOG_LEVEL_DEBUG, "HA Application Thread: Application is Passive");
			}
			else if (ha_state[0] == 'S'){
				/* Request to stop the thread, perform the graceful termination activities here */
				syslog(LOG_INFO, "HA Application Thread: Request to stop application");
				log.write(LOG_LEVEL_DEBUG, "HA Application Thread: Request to stop application");

				stop_alh_daemon_work();

				syslog(LOG_INFO, "HA Application Thread: waiting for Worker Thread termination ... !");
				log.write(LOG_LEVEL_DEBUG, "HA Application Thread: waiting for Worker Thread termination ... !");

				ACE_Thread_Manager::instance()->join(alh_worker_thread_id);

				syslog(LOG_INFO, "HA Application Thread: Worker Thread terminated !");
				log.write(LOG_LEVEL_DEBUG, "HA Application Thread: Worker Thread terminated !");
				break;
			}

			if(alh_worker_thread_id == 0)
			{
				/* create ALH worker executor thread */
				const ACE_TCHAR* thread_name = "WorkerThread";
				int call_res = ACE_Thread_Manager::instance()->spawn(& run_alh_daemon_work,
														(void *)this,
														THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
														& alh_worker_thread_id,
														0,
														ACE_DEFAULT_THREAD_PRIORITY,
														-1,
														0,
														ACE_DEFAULT_THREAD_STACKSIZE,
														&thread_name);
				if (call_res == -1){
					syslog(LOG_ERR, "Error creating ALH Worker Thread");
					return ACS_APGCC_FAILURE;
				}

				syslog(LOG_INFO, "HA Application Thread: the ALH Worker Thread has been successfully created !");
				log.write(LOG_LEVEL_DEBUG, "HA Application Thread: the ALH Worker Thread has been successfully created !");
			}
		}
	}

	syslog(LOG_INFO, "HA Application Thread terminated successfully");
	log.write(LOG_LEVEL_DEBUG, "HA Application Thread terminated successfully");

	return ACS_APGCC_SUCCESS;
}

int acs_alh_ha_appmanager::wait_ha_application_thread_termination ( bool nolog )
{
	if(!nolog) syslog(LOG_INFO, "Waiting for HA Application Thread termination ... ");

	if(alh_ha_application_thread_id == 0)
	{
		if(!nolog) syslog(LOG_INFO, "HA Application Thread already terminated !");
		return 1; // HA application thread already terminated
	}

	int retval = ACE_Thread_Manager::instance()->join(alh_ha_application_thread_id);
	if(retval == 0)
	{
		if(!nolog) syslog(LOG_INFO, "HA Application Thread successfully joined ! ");
		alh_ha_application_thread_id = 0;  // HA application thread has terminated
	}
	else if(!nolog)
		syslog(LOG_ERR, "Failure joining HA Application Thread !");

	return retval;
}
