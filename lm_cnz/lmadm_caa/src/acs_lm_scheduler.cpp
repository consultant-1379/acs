//******************************************************************************
//
//  NAME
//     acs_lm_scheduler.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
//#define _WIN32_WINNT 0x0400

#include "acs_lm_scheduler.h"
//#include "acs_lm_trace.h"
#include "acs_lm_cmdclient.h"
#include "acs_lm_lksender.h"
#include "acs_lm_tra.h"
#include "acs_lm_server.h"

//============================================================================
// Thread activity of scheduler
// Handles timer functionalities by calling timer functions
//============================================================================
/*=================================================================
	ROUTINE: schedulerThread
=================================================================== */
ACE_UINT64 ACS_LM_Scheduler::schedulerThread(void* pArg)
{
	INFO("%s","In ACS_LM_Scheduler::schedulerThread()");



	ACS_LM_Scheduler* scheduler = (ACS_LM_Scheduler*) pArg;
	DEBUG("%s","start() sending LK data during service startup");
	(void)ACS_LM_LkSender::sendLks(ACS_LM_SENDER_SERVICE_STARTUP, scheduler->sentinel,scheduler->persistent,scheduler->eventHandler);
	ACS_LM_Timer* timer = scheduler->timer;

	while((!scheduler->threadStopRequested) && (ACS_LM_Server::isShutdown==false))
	{
	    ACE_UINT32 secondsLeft = (ACE_UINT32)scheduler->calculateSecondsToWait();    

	    //time_t secondsLeft = scheduler->calculateSecondsToWait();    
	    DEBUG("schedulerThread(): timer start, secondsLeft = %d",secondsLeft);
	    timer->start(secondsLeft);
            timer->waitForTimeout();

            //timer->cancel();
	}
	
	if(scheduler != NULL)
	{
    		ACE_OS::close(scheduler->hThread);
    		scheduler->hThread = NULL;
	}
	return 0;
}//end of schedulerThread
/*=================================================================
	ROUTINE: ACS_LM_Scheduler constructor
=================================================================== */
ACS_LM_Scheduler::ACS_LM_Scheduler()
:hThread(NULL),
timer(NULL),
hScheduleCompleted(NULL),
threadStopRequested(false),
sentinel(0),
testLkf(0),
persistent(0),
eventHandler(0)
{
	timer = new ACS_LM_Timer(this);
	hScheduleCompleted = new ACE_Event();
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_Scheduler destructor
=================================================================== */
ACS_LM_Scheduler::~ACS_LM_Scheduler()
{
	hScheduleCompleted->remove();
	stop();
	delete timer;
	timer = NULL;
	if(hScheduleCompleted != NULL)
	{
	delete hScheduleCompleted;
	}
}//end of destructor
//============================================================================
// Calculates the number of seconds left to reach 24th hour of day
//============================================================================
/*=================================================================
	ROUTINE: calculateSecondsToWait
=================================================================== */
time_t ACS_LM_Scheduler::calculateSecondsToWait()
{
	struct tm tm_align;
	time_t now = ACE_OS::time (NULL);
	struct tm tm_now =	*ACE_OS::localtime (&now);

	DEBUG("It is now = %s",ctime(&now));

	tm_align = tm_now;

	tm_align.tm_hour = 24;
	tm_align.tm_min = 00;
	tm_align.tm_sec = 00;

	time_t alignTime = ACE_OS::mktime(&tm_align);

	DEBUG("Scheduler Date : %s",ctime(&alignTime));

	time_t diffTime = (time_t)ACE_OS::difftime(alignTime,now);
	
	//set the time to 12:05 AM
	diffTime = diffTime + (5*60);

        return diffTime;
}//end of calculateSecondsToWait
/*=================================================================
	ROUTINE: schedulerThreadFunc
=================================================================== */
ACE_THR_FUNC_RETURN ACS_LM_Scheduler::schedulerThreadFunc(void* args)
{
	return reinterpret_cast<void*>(ACS_LM_Scheduler::schedulerThread(args));

}//end of schedulerThreadFunc

/*=================================================================
	ROUTINE: start
=================================================================== */
void ACS_LM_Scheduler::start(ACS_LM_Sentinel* sentinel,
							 ACS_LM_Persistent* persistent, 
							 ACS_LM_EventHandler* eventHandler)
{
	DEBUG("%s","In ACS_LM_Scheduler::start method");

	this->sentinel = sentinel;
	this->persistent = persistent;
	this->testLkf = persistent->testLkFile();
    this->eventHandler = eventHandler;

	threadStopRequested = false;
	hScheduleCompleted->reset();
	ACE_thread_t threadId;
	const ACE_TCHAR * pthreadName1 = "schedulerThreadFunc";


//    DEBUG("%s","start() sending LK data during service startup");
//	(void)ACS_LM_LkSender::sendLks(ACS_LM_SENDER_SERVICE_STARTUP, sentinel, persistent, eventHandler);

	if(hThread != 0)
    {
        stop();
    }
    hThread = ACE_Thread_Manager::instance()->spawn(&schedulerThreadFunc,
    		(void*)this ,
    		THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
    		&threadId,
    		0,
    		ACE_DEFAULT_THREAD_PRIORITY,
    		-1,
    		0,
    		ACE_DEFAULT_THREAD_STACKSIZE,
    		&pthreadName1);

    if(hThread==0)
    {
        ACE_UINT64 ec = ACE_OS::last_error();
        DEBUG("start(): CreateThread() failed  ec = %d",ec);
    }
  
	DEBUG("%s","Exiting start()");
}//end of start
//============================================================================
// stops the scheduler 
// Wait for thread completion and terminates thread if thread activity is not 
// finished in 2 seconds
//============================================================================
/*=================================================================
	ROUTINE: stop
=================================================================== */
void ACS_LM_Scheduler::stop()
{
	DEBUG("%s","In ACS_LM_Scheduler::stop");
	threadStopRequested = true;

    if(hThread != 0)
	{		
    	if(timer!=NULL)
        {
        	timer->cancel();
        }
        hThread = 0;
	}
	DEBUG("%s","Leaving ACS_LM_Scheduler::stop...");
}//end of stop
/*=================================================================
	ROUTINE: timeout
=================================================================== */
void ACS_LM_Scheduler::timeout()
{
	DEBUG("%s","In ACS_LM_Scheduler::timeout");
    
    ACS_LM_AppExitCode retnCode = ACS_LM_LkSender::sendLks(ACS_LM_SENDER_SCHEDULER, 
                                                           sentinel,
                                                           persistent,
														   eventHandler);    

	DEBUG("ACS_LM_Scheduler::timeout, retnCode %d = ",retnCode);
}//end of timeout

