#include "fcsc_application.h"
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#include <stdlib.h>
#include "fcsc_OI_Handler.h"
#include "fcscadm_Common.h"

bool flag = true;


ACS_APGCC_ReturnType myAPGApp1Application::myAppInitialize(void)
{
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;
	// Do any Application specific initialization here
	mysyslog(LOG_INFO, "FCSC Service Initialized!!!!!!");
	return rc;
}

void flavour(int sig)
{
   if (sig == SIGUSR1)
      return;
}

void shutdown_OI( int sig )
{
    raise(SIGUSR2);
    if(sig == SIGUSR1)
    {
       mysyslog(LOG_INFO, "OI shutdown called\n" );
    //char *m =  "OI Shutting Down";
       flag = false;
        cout << "OI Shutting Down" << endl;
    //sleep(5);
    }

}


void *start_OI_Handler(void *msg)
{
     fcsc_OI_Admin_Operator adminOperator;

     //register for signals
     struct sigaction sigact;
     sigset_t sigset;
     sigemptyset( &sigact.sa_mask );
     sigact.sa_flags = SA_NOCLDWAIT;
     sigact.sa_handler = shutdown_OI;
     sigaction(SIGUSR1, &sigact, NULL);
     sigaction(SIGCHLD, &sigact, NULL);

     adminOperator.register_object();

     pthread_exit((void*) msg);
}

int OI_thread_strt()
{
    //start OI Handler thread

    pthread_t OI_thread;
    pthread_attr_t attr;
    int returnStatus;
    char *msg = "OI Handler Started";
    void *status;

    struct sigaction sigact;
        //sigset_t sigset;
        sigemptyset( &sigact.sa_mask );
        sigact.sa_flags = SA_NOCLDWAIT;
        sigact.sa_handler = SIG_IGN;
        sigaction(SIGCHLD, &sigact, NULL);
        signal(SIGUSR1, flavour);

   /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        cout << "Spawning OI" << endl;
        returnStatus = pthread_create(&OI_thread, &attr, start_OI_Handler, (void *)msg);
        if (returnStatus)
        {
           printf("ERROR; return code from pthread_create() is %d\n", returnStatus);
           pthread_attr_destroy(&attr);
           exit(-1);
        }
        //sleep(10);
        //raise(SIGUSR1);

           /* Free attribute and wait for the other threads */
           sleep(10);
           pthread_attr_destroy(&attr);


        mysyslog(LOG_INFO, "FCSC Service is started!!!");
}

ACS_APGCC_ReturnType myAPGApp1Application::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{

	mysyslog(LOG_INFO,"performStateTransitionToActiveJobs\n");
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;
	ACS_APGCC_AMF_HA_StateT newHAState;


	newHAState = getHAState();

	if(newHAState == previousHAState)
		return rc;

	// Do any activities specific to application w.r.t ACTIVE state assignment
	//start OI Handler thread
        OI_thread_strt();
            
     fcsc_OI_Admin_Operator adminOperator;
     adminOperator.register_object();

	return rc;
}

ACS_APGCC_ReturnType myAPGApp1Application::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;
	ACS_APGCC_AMF_HA_StateT newHAState;


	newHAState = getHAState();

	if(newHAState == previousHAState)
		return rc;

	// Do any activities specific to application w.r.t STANDBY state assignment
	raise(SIGUSR1);
	mysyslog(LOG_INFO, "My Application Component received STANDBY state assignment!!!");

	return rc;
}

ACS_APGCC_ReturnType myAPGApp1Application::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

	// Do any activities specific to application w.r.t QUIESING state assignment

	mysyslog(LOG_INFO, "My Application Component received QUIESING state assignment!!!");

	return rc;
}

ACS_APGCC_ReturnType myAPGApp1Application::performComponentHealthCheck(void)
{
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

	// do any application specific health check activities before responding
	mysyslog(LOG_INFO, "My Application Component received healthcheck query!!!");
        //OI_thread_strt();

	return rc;
}


ACS_APGCC_ReturnType myAPGApp1Application::performComponentTerminateJobs(void)
{
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

	mysyslog(LOG_INFO, "My Application Component received terminate callback!!!");

	/* Do any cleanup activity */
	raise(SIGUSR1);
	return rc;
}

ACS_APGCC_ReturnType myAPGApp1Application::performComponentRemoveJobs(void)
{
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;

	mysyslog(LOG_INFO, "My Application Component received csi remove callback!!!");

	// Here we should be able to reset HA state to undefined

	return rc;
}

