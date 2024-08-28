/*=================================================================== */
/**
   @file acs_chb_clocksync_class.cpp

   Class method implementationn for acs_chb_clocksync_class module.

   This module contains the implementation of the main class for heartbeat.

   @version 1.0.0

 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       20/04/2011   XTANAGG   Initial Release
 */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <ace/Task.h>
#include <ace/OS_NS_poll.h>
#include <ace/ACE.h>
#include <ace/Process.h>
#include <ace/Event.h>
#include <acs_prc_api.h>
//#include <acs_chb_param_oihandler.h>
#include <acs_chb_clock_def.h>
#include <acs_chb_clocksync_class.h>

using namespace std;

unsigned long                   globalFlags=0;
int                             cycletimeInMs=0;
bool                            debug = false;
ACS_APGCC_ReturnType rcode;
//acs_chb_param_oihandler *ptrClockSyncParamOiHandler = 0;

/**
	@brief	clocksync_service_run
 */

ACE_THR_FUNC_RETURN clocksync_service_run(void *);

/**
	@brief	evh
 */
ACE_Event evh(TRUE, FALSE, USYNC_THREAD, "ACS_CHB_CLOCKSYNC_STOP_EVENT");
ACE_Recursive_Thread_Mutex ThrExitHandler::theThrMutex;
map<ACE_thread_t, bool> ThrExitHandler::theThrStatusMap;
char ACS_CHB_Common::dnOfCsyncRootObj[512] = {0};
bool acs_chb_clocksync_class::theStopSignal = false;
ACE_Recursive_Thread_Mutex acs_chb_clocksync_class::theMutex;
// TR HQ37527 - BEGIN
ACE_HANDLE pipeHandles[2];
ACE_thread_t acs_chb_clocksync_class::applicationThreadId=0;
ACS_APGCC_BOOL acs_chb_clocksync_class::Is_terminated=FALSE;
// TR HQ37527 - END

/*=======================================================================
	ROUTINE: acs_chb_clocksync_class
========================================================================*/
acs_chb_clocksync_class::acs_chb_clocksync_class(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	//DEBUG(1, "%s", "Entering acs_chb_clocksync_class::acs_chb_clocksync_class");
	//setStopSignal(false);
	//DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::acs_chb_clocksync_class");

}

/*=================================================================================
	ROUTINE: ~acs_chb_clocksync_class
================================================================================== */
acs_chb_clocksync_class::~acs_chb_clocksync_class()
{
	//DEBUG(1, "%s", "Entering acs_chb_clocksync_class::~acs_chb_clocksync_class");
	this->passifyApp();
	//DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::~acs_chb_clocksync_class");
}

/*=================================================================================
	ROUTINE: performStateTransitionToActiveJobs
================================================================================== */
ACS_APGCC_ReturnType acs_chb_clocksync_class::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performStateTransitionToActiveJobs");

	/* Check if we have received the ACTIVE State Again.
	 * This means that, our application is already Active and
	 * again we have got a callback from AMF to go active.
	 * Ignore this case anyway. This case should rarely happens
	 */
	globalFlags = 0;
	(void) previousHAState;		
	rcode = this->activateApp();
	DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performStateTransitionToActiveJobs");
	return rcode;
}

/*=================================================================================
        ROUTINE: performStateTransitionToPassiveJobs
================================================================================== */
ACS_APGCC_ReturnType acs_chb_clocksync_class::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performStateTransitionToPassiveJobs");

	/* Check if we have received the PASSIVE State Again.
	 * This means that, our application is already passive and
	 * again we have got a callback from AMF to go passive.
	 * Ignore this case anyway. This case should rarely happens
	 */
	(void) previousHAState;
	rcode= this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performStateTransitionToPassiveJobs");
	return rcode;
}



/*=====================================================================================
	ROUTINE: performStateTransitionToQueisingJobs
======================================================================================= */
ACS_APGCC_ReturnType acs_chb_clocksync_class::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performStateTransitionToQueisingJobs");
	(void)previousHAState;
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performStateTransitionToQueisingJobs");
	return rcode;
}

/*================================================================================
	ROUTINE: performStateTransitionToQuiescedJobs
=================================================================================*/
ACS_APGCC_ReturnType acs_chb_clocksync_class::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performStateTransitionToQuiescedJobs");

	(void)previousHAState;
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performStateTransitionToQuiescedJobs");
	return rcode;
}

/*============================================================================
	ROUTINE: performComponentHealthCheck
=============================================================================== */
ACS_APGCC_ReturnType acs_chb_clocksync_class::performComponentHealthCheck(void)
{
	//DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performComponentHealthCheck");

	/* Application has received health check callback from AMF. Check the
	 * sanity of the application and reply to AMF that you are ok.
	 */
	//DEBUG(1, "%s", "Health Check Success.");
	//DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performComponentHealthCheck");
	return ACS_APGCC_SUCCESS;

}

/*============================================================================
	ROUTINE: performComponentTerminateJobs
============================================================================== */
ACS_APGCC_ReturnType acs_chb_clocksync_class::performComponentTerminateJobs(void)
{
	DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performComponentTerminateJobs");

	/* Application has received terminate component callback due to 
	 * LOCK-INST admin opreration perform on SU. Terminate the thread if
	 * we have not terminated in performComponentRemoveJobs case or double 
	 * check if we are done so.
	 */
	rcode =  this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performComponentTerminateJobs");
	return rcode;
}

/*=============================================================================
	ROUTINE: performComponentRemoveJobs
============================================================================== */
ACS_APGCC_ReturnType acs_chb_clocksync_class::performComponentRemoveJobs(void)
{

	DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performComponentRemoveJobs");
	rcode =  this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performComponentRemoveJobs");
	return rcode;
}

/*===============================================================================
	ROUTINE: performApplicationShutdownJobs
================================================================================ */
ACS_APGCC_ReturnType acs_chb_clocksync_class::performApplicationShutdownJobs() 
{
	DEBUG(1, "%s", "Entering acs_chb_clocksync_class::performApplicationShutdownJobs");
	rcode =  this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_clocksync_class::performApplicationShutdownJobs");
	return rcode;
}

ACS_APGCC_ReturnType acs_chb_clocksync_class::activateApp()
{

	DEBUG(1,"%s","Entering acs_chb_clocksync_class::activateApp()");
	if (ACS_CHB_Common::isVirtualEnvironment() == true )// get environment for clock synchronizer
	{
		DEBUG(1,"%s","Inhibiting clock synchronizer in Virtual Environment");		
		return ACS_APGCC_SUCCESS;
	}
	Is_terminated=FALSE;
	evh.reset();
	const ACE_TCHAR* clocksyncThreadName = "ACSCHBClockSyncFuncThread";
	int clocksyncThreadGroupId = ACE_Thread_Manager::instance()->spawn(&ClockSyncServiceMain,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&applicationThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&clocksyncThreadName);
	if (clocksyncThreadGroupId == -1)
	{
		ERROR(1, "%s", "Error occurred while creating the ACSCHBClockSyncFuncThread");
		ERROR(1, "%s", "Leaving acs_chb_clocksync_class::svc");
		applicationThreadId=0;
		kill(getpid(), SIGTERM);
		return ACS_APGCC_FAILURE;
	}
	
	DEBUG(1,"%s","Exiting acs_chb_clocksync_class::activateApp()");
	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType acs_chb_clocksync_class::passifyApp()
{

	DEBUG(1,"%s","Entering acs_chb_clocksync_class::passifyApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
	
	/*if (0 == this->m_lmApplObj)
        {
                DEBUG("%s","application is already passive");
                rCode = ACS_APGCC_SUCCESS;
        }
        else
        {
                DEBUG("%s","Ordering lm Application to passify");
                this->m_lmApplObj->stop();

                DEBUG("%s","Waiting for lm Application to become passive...");
                this->m_lmApplObj->wait();

                DEBUG("%s","Deleting lm App instance...");
                delete this->m_lmApplObj;
                this->m_lmApplObj=0;
                DEBUG("%s","App is now passivated by HA!!");
		rCode = ACS_APGCC_SUCCESS;
        }*/
	rCode = stop();
	DEBUG(1,"%s","Exiting acs_chb_clocksync_class::passifyApp()");
	return rCode;
}

ACS_APGCC_ReturnType stop()
{
	DEBUG(1,"%s","Entering stop()");
	if (ThrExitHandler::CheckNoThreads() == false)
	{
		DEBUG(1, "%s", "stop:: Request to stop application");
		DEBUG(1, "%s", "stop:: Signalling stop event!!");
		/* Request to stop the thread, perform the graceful activities here */
		acs_chb_clocksync_class::setStopSignal(true);
		globalFlags |= KILL;

		// Sending to stop signal to PIPE
		char buf[] = { 1 , 1};
		int bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
		while( bytes <= 0 )
		{
			ERROR(1, "%s", "Error occurred while signaling stop event ");
			bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
			sleep(1);
		}
		//Now Signal the stop event
		evh.signal();		
		if((!acs_chb_clocksync_class::Is_terminated) && (acs_chb_clocksync_class::applicationThreadId!=0))
		{
		DEBUG(1, "%s", "Waiting for application Thread to join");
		ACE_Thread_Manager::instance()->join(acs_chb_clocksync_class::applicationThreadId);
		acs_chb_clocksync_class::applicationThreadId=0;
		acs_chb_clocksync_class::Is_terminated=TRUE;
		}
		//Wait until all threads have stopped.
		do
		{
			sleep(1);
			//DEBUG(1,"%s","Check that all the thread should close properly()");
		}
		while( ThrExitHandler::CheckNoThreads() == false );
		
		ThrExitHandler::cleanup();
	}
	DEBUG(1,"%s","Leaving stop()");
	return ACS_APGCC_SUCCESS;
}

/*===================================================================================
	ROUTINE: ClockSyncServiceMain
=====================================================================================*/
ACE_THR_FUNC_RETURN ClockSyncServiceMain(void *ptr)
{
	DEBUG(1, "%s", "Entering ClockSyncServiceMain");
	(void)ptr;
	
	if( ThrExitHandler::init() == false )
	{
		ERROR(1, "%s", "Error occured while registering exit handler");
		ERROR(1, "%s", "Exiting ClockSyncServiceMain");
		return 0;
	}
	//Fetch the parent object from IMM.

	if(ACS_CHB_Common::fetchDnOfCsyncRootObjFromIMM() == -1)
	{
		ERROR(1, "%s", "Fetching of DN of CHB root object from IMM is failed !!");
		ERROR(1, "%s", "Exiting ClockSyncServiceMain");
		return 0;
	}
	// TR HQ37527 - BEGIN
	if( ACE_OS::pipe(pipeHandles) == -1 )
	{
		ERROR(1, "%s", "Error occured while creating pipe for STOP event!!");
		ERROR(1, "%s", "Exiting ClockSyncServiceMain");
		return 0;
	}
	// TR HQ37527 - END


	int result = -1;
	result = checkDSDDependencyForClockSync() ;
	if( result == -1 )
	{
		ERROR(1, "%s", "DSD Server is not running, exiting");
		// TR HQ37527 - BEGIN
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);
		// TR HQ37527 - END
		return 0;
	}
	else if( result == 0 )
	{
		DEBUG(1, "%s", "Stop Event Signalled");
		// TR HQ37527 - BEGIN
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);
		// TR HQ37527 - END
		return 0;
	}

	DEBUG(1, "%s", "Creating ACS_CHB_ClockSync object");

	// TR HQ37527 - BEGIN
	ACS_CHB_ClockSync sync(pipeHandles[0]);
	// TR HQ37527 - END
	ACS_CHB_ClockComm comm;                 // Create communication object
	OmHandler myOmHandler;
	if( myOmHandler.Init() == ACS_CC_FAILURE )
	{
		ERROR(1, "%s", "Failure occured while doing Omhandler.Init()");
		// TR HQ37527 - BEGIN
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);
		// TR HQ37527 - END
		return 0;
	}

	int successiveCommFailures = 0;
	int successiveCommInitiateFailures=0;
	ACS_CHB_Event serviceMainEvents(ACS_CHB_processName);

	bool    good;
	unsigned char timebuf[100];
	ACE_OS::memset(timebuf, 0, sizeof(timebuf));

	char problemData[256];

	DEBUG(1, "%s", "Getting into the Initiate do-while loop");
	try  //Use try/catch mechanism to handle exceptions
	{
		do
		{
			DEBUG(1, "%s", "Reading the Clock sync parameters");
			if ( sync.getParameters(myOmHandler) == ACS_CC_SUCCESS )// Read parameters for clock synchronizer
			{
				ACE_Time_Value pingTime;
				timespec_t ping_tv;
				ping_tv.tv_sec = sync.cycletime;
				ping_tv.tv_nsec = 0;
				pingTime.set(ping_tv);

				DEBUG(1,"%s", "Reading the Clock comm parameters");
				if (comm.getParameters(myOmHandler) == ACS_CC_SUCCESS ) // Read parameters for communication interface
				{
					sync.cycletime = comm.cycletime;
					cycletimeInMs = comm.cycletime*1000;
					ping_tv.tv_sec = comm.cycletime;
					ping_tv.tv_nsec = 0;
					pingTime.set(ping_tv);
					// LOOP for JTP inititialization and ClockSync
					DEBUG(1, "%s", "Clock sync service running");

					//New member function initiate2 as per new JTP API
					DEBUG(1, "%s", "Calling the initiate2 ...");

					good=comm.initiate2(successiveCommInitiateFailures);

					DEBUG(1, "%s", "After calling the initiate2 ...");

					if(good)
					{
						successiveCommInitiateFailures=0;
						// LOOP for Clock Synchronization
						// This loop is entirely controlled by the outcome of comm.data2().
						DEBUG(1,"%s", "Getting into the Data do-while loop");
						do
						{
							good=false;
							if (sync.readTime (0) == SYNC_OK)       // Read AP clock
							{
								DEBUG(1, "%s", "Calling the data2 ....");
								good = comm.data2(timebuf, successiveCommFailures);
								DEBUG(1, "%s", "After Calling the data2 ....");
								if (good)
								{
									if ((good = (sync.readTime (1) == SYNC_OK)))
									{       // Read AP clock again
										DEBUG(1, "%s", "Sychronize.......");

										// Synchronize AP clock
										sync.synchronize (timebuf);
										good=!sync.jtpResponseTooSlow;
									}
									successiveCommFailures=0;
								}
								else //Here if comm.data2 failed, i.e. JTP on the blink
								{
									DEBUG(1, "%s", "Incrementing the counter for comm.data2 failure");
									successiveCommFailures++;
								}
								if(successiveCommFailures>10)
								{
									DEBUG(1, "Failed to get CP time %d times in succession", successiveCommFailures);
									ACE_OS::sprintf(problemData, "REASON\nFAILED TO GET CP TIME %d TIMES IN SUCCESSION.", successiveCommFailures);
									// This Alarm is Ceased next time we successfully synchronize
									sync.IssueAlarm(problemData);
									good = false;
									if (acs_chb_clocksync_class::getStopSignal() == true)
									{
										DEBUG(1, "%s", "REQUEST to stop the service");
										break;
									}
								}
								if(good && !sync.amAdjusting)
								{
									//If not adjusting we wait a while before trying again.
									//Sleep sleepTime, or until evh signalled, which only
									//happens if service control function receives a SERVICE_CONTROL_STOP msg
									//However, don't sleep if !good, 'cos there's a WaitFor... coming outside the while loop
									//(If still adjusting, sync.synchronize sets and does sleep time)
									DEBUG(1, "%s", "Waiting for a while before trying again");
									sync.sleepTime = cycletimeInMs;
									// TR HQ37527 - BEGIN
									// ACE_Time_Value tv1(60,0);
									ACE_Handle_Set readHandleSet;
									readHandleSet.set_bit( pipeHandles[0]);

									int status = ACE::select( readHandleSet.max_set()+1, readHandleSet, &pingTime);

									DEBUG(1, "after select status = %d",status);
									if (status > 0)
									{
										if( readHandleSet.is_set(pipeHandles[0]))
										{
											// Stop event signaled.
											char buf[] = { 0, 0 };
											ACE_OS::read( pipeHandles[0], buf, sizeof(buf ));
											DEBUG(1, "%s", "Stop event signaled in csync service while waiting for CYCLE time");
										}
									}
									// TR HQ37527 - END
								}
								sync.amAdjusting = false;
								if (acs_chb_clocksync_class::getStopSignal() == true)
								{
									DEBUG(1, "%s", "REQUEST to stop the service");
									break;
								}
								if(globalFlags&KILL) //If set e.g. by service control
								{
									DEBUG(1, "%s", "REQUEST to stop the service");
									good = false;
								}
							}
						} while (good); // End Sync LOOP
						DEBUG(1, "%s", "Coming out of the data2 do-while loop");

					}
					else //initiate2 failed, i.e. jtp ain't workin' (seen this happen)
					{
						DEBUG(1, "%s", "Incrementing the counter for JTP initiate failure");
						successiveCommInitiateFailures++;
					}

					//If unable to communicate with CP >10 times in a row, raise alarm
					if(successiveCommInitiateFailures > 10)
					{
						// This Alarm is Ceased next time we successfully synchronize
						DEBUG(1, "Failed to initiate JTP connection to the cp %d times in succession", successiveCommInitiateFailures);
						ACE_OS::sprintf(problemData, "REASON\nFAILED TO INITIATE JTP CONNECTION TO THE CP %d TIMES IN SUCCESSION.", successiveCommInitiateFailures);
						sync.IssueAlarm(problemData);
						//successiveCommInitiateFailures = 1; //Use this as flag for what the alarm was
					}
					sync.sleepTime = cycletimeInMs;
					//Only relevant in case tracing freak clock jumps (_MYLOG defined) in sync.
					if(!(globalFlags&KILL))
					{
						// TR HQ37527 - BEGIN
						//	evh.wait(&pingTime, 0);
						//ACE_Time_Value tv1(60,0);
						ACE_Handle_Set readHandleSet;
						readHandleSet.set_bit( pipeHandles[0]);

						ACE::select( readHandleSet.max_set()+1, readHandleSet, &pingTime);
						// TR HQ37527 - END
					}
					DEBUG(1, "globalFlags = %d", globalFlags);
					if(!comm.terminate()) //Terminate this conversation, try reinstating next time around, if not killed
					{
						DEBUG(1, "%s", "Termination is failed");
						globalFlags |=KILL; //but die now if termination failed too.
					}
					if (acs_chb_clocksync_class::getStopSignal() == true)
					{
						DEBUG(1, "%s", "REQUEST to stop the service");
						break;
					}

				}
			}
		} while (!(globalFlags&KILL)); // End JTP init LOOP
	} //end try
	catch(...)
	{
		//Ensure clean-up and reinstatement even if an exception forces exit
		ACE_OS::sprintf(problemData, "Process terminated due to an exception.");
		sync.~ACS_CHB_ClockSync();
		serviceMainEvents.event(ACS_CHB_FatalErr,
				ACS_CHB_EVENT_STG,
				ACS_CHB_Cause_APfault,
				problemData,
				ACS_CHB_Text_ProcTerm);
		comm.~ACS_CHB_ClockComm();
		// TR HQ37527 - BEGIN
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);
		// TR HQ37527 - END
		return 0;
	}//end catch
	myOmHandler.Finalize();
	DEBUG(1, "%s", "Coming out of the JTP init loop");
	ERROR(1, "%s", "Exiting the acs_chbclocksyncd service");
	// TR HQ37527 - BEGIN
	ACE_OS::close(pipeHandles[0]);
	ACE_OS::close(pipeHandles[1]);
	// TR HQ37527 - END
	return 0;
}

/*===============================================================================
        ROUTINE: checkDSDDependencyForClockSync
=================================================================================*/
int checkDSDDependencyForClockSync()
{
	DEBUG(1, "%s", "Entering checkDSDDependencyForClockSync");

	//ACS_CHB_Event serviceMainEvents(ACS_CHB_processName);
	//char problemData[512] = { 0 };

	while( ACS_CHB_Common::isDSDServerRunning() == false )
	{
		DEBUG(1, "%s", "Waiting for DSD to be UP and Working");
		ACE_Time_Value tv1(30,0);

		// TR HQ37527 - BEGIN
		ACE_Handle_Set readHandleSet;
		readHandleSet.set_bit( pipeHandles[0]);

		int retCode = ACE::select( readHandleSet.max_set()+1, readHandleSet, &tv1);

		if(retCode > 0)
		{
			DEBUG(1, "%s", "Stop event signalled while waiting for DSD service to come up");
			return 0;
		}
		else
		{
			DEBUG(1, "%s", "Timeout happened while waiting for DSD server");
		}
		// TR HQ37527 - END
	}
	DEBUG(1, "%s", "Leaving checkDSDDependencyForClockSync");
	return 1;
}

/*===============================================================================
        ROUTINE: checkNodeState
=================================================================================*/
int checkNodeState()
{
	ACS_PRC_API prcObj;
	int nodeState = 0;
	nodeState = prcObj.askForNodeState();

	return nodeState;
}
bool acs_chb_clocksync_class::getStopSignal()
{
	acs_chb_clocksync_class::theMutex.acquire();
	bool myStatus = theStopSignal;
	acs_chb_clocksync_class::theMutex.release();
	return myStatus;
}
void acs_chb_clocksync_class::setStopSignal(bool mySignal)
{
	DEBUG (1,"%s","acs_chb_clocksync_class::setStopSignal StopSignal is set");
	acs_chb_clocksync_class::theMutex.acquire();
	theStopSignal = mySignal;
	acs_chb_clocksync_class::theMutex.release();

}
// TR HQ37527 - BEGIN
void acs_chb_clocksync_class::sendStopToPipe()
{
	DEBUG (1,"%s","acs_chb_clocksync_class::sendStopToPipe");
	// Sending to stop signal to PIPE
	char buf[] = { 1 , 1};
	int bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
	while( bytes <= 0 )
	{
		ERROR(1, "%s", "Error occurred while signaling stop event ");
		bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
		sleep(1);
	}
}
// TR HQ37527 - END



