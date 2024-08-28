//******************************************************************************
//
//  NAME
//     acs_lm_timer.cpp
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
#include "acs_lm_timer.h"
//============================================================================
// Thread activity of Timer
// Creates and sets the timer
//============================================================================
/*=================================================================
	ROUTINE: timerThread
=================================================================== */
ACE_THR_FUNC_RETURN ACS_LM_Timer::timerThread(void* pArg)
{
   	ACS_LM_Timer* myTimer = (ACS_LM_Timer*) pArg;
   	ACE_Time_Value oTimeValue(myTimer->getPeriod());

   	fd_set readfds;
   	FD_ZERO(&readfds);
   	FD_SET(myTimer->theStopRequestEventHandles[0], &readfds);
   	ACE_HANDLE maxfd = myTimer->theStopRequestEventHandles[0];
   	int myRetVal = -1;

   	myRetVal = ACE_OS::select(maxfd + 1, &readfds, NULL,NULL,oTimeValue);

   	if( myRetVal == 0)
   	{
   		myTimer->timerAPC();
   	}
   	else if (myRetVal > 0)
   	{
   	}
   	else
   	{
   	}
       
   	return 0;
}//end of timerThread
/*=================================================================
	ROUTINE: ACS_LM_Timer constructor
=================================================================== */

ACS_LM_Timer::ACS_LM_Timer(ACS_LM_TimerListener* listener)
:
theListener(listener),
thePeriod(0)
{
	ACE_OS::pipe(theStopRequestEventHandles);
//	ACE_OS::pipe(theJobCompleteEventHandles);
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_Timer destructor
=================================================================== */
ACS_LM_Timer::~ACS_LM_Timer()
{
    cancel();
    close( theStopRequestEventHandles[0] );
    close( theStopRequestEventHandles[1] );
    close( theJobCompleteEventHandles[0] );
    close( theJobCompleteEventHandles[1] );
}//end of destructor

/*=================================================================
	ROUTINE: timerThread
=================================================================== */
void ACS_LM_Timer::timerAPC()
{
    if(theListener != NULL)
	{
        theListener->timeout();
	}
	char buf[] = { 1 , 1};
	int bytes = ACE_OS::write(theJobCompleteEventHandles[1], buf, sizeof(buf ));
	if( bytes <= 0 )
	{
	}
	else
	{
	}
}//end of timerAPC


//============================================================================
// Wait function
// Timer waits till the timer elapses
//============================================================================
/*=================================================================
	ROUTINE: waitForTimeout
=================================================================== */
void ACS_LM_Timer::waitForTimeout()
{

	ACE_OS::pipe(theJobCompleteEventHandles);

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(theStopRequestEventHandles[0], &readfds);
	FD_SET(theJobCompleteEventHandles[0], &readfds);
	ACE_HANDLE maxfd = theStopRequestEventHandles[0];
	if( theJobCompleteEventHandles[0] > theStopRequestEventHandles[0] )
	{
		maxfd = theJobCompleteEventHandles[0] ;
	}
	int myRetVal = -1;

	myRetVal = ACE_OS::select(maxfd +1, &readfds, NULL,NULL,NULL);

        if(myRetVal == 1) 
        {
            FD_CLR(theJobCompleteEventHandles[0],&readfds);
            FD_CLR(theStopRequestEventHandles[0],&readfds);

            close( theJobCompleteEventHandles[0] );
            close( theJobCompleteEventHandles[1] );
       }
}//end of waitForTimeout
/*=================================================================
	ROUTINE: start
=================================================================== */
bool ACS_LM_Timer::start(ACE_UINT64 aSeconds)
{
    thePeriod = aSeconds;

    /* Start the LM Timer Thread */
    const ACE_TCHAR* myLMTimerThreadName = "LMTimerThread";

    ACE_INT32 threadId = ACE_Thread_Manager::instance()->spawn(timerThread,
							(void *)this,
                            //THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
                            THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED,
                            0,
                            0,
                            ACE_DEFAULT_THREAD_PRIORITY,
                            -1,
                            0,
                            ACE_DEFAULT_THREAD_STACKSIZE,
                            &myLMTimerThreadName);

    if (threadId == -1)
    {
        return false;
    }
    else
    {
    }
    return true;
}//end of start
/*=================================================================
	ROUTINE: cancel
=================================================================== */
void ACS_LM_Timer::cancel()
{
	 /* Reset the events */
	char buf[] = { 1 , 1};
	int bytes = ACE_OS::write(theJobCompleteEventHandles[1], buf, sizeof(buf ));
	if( bytes <= 0 )
	{
	}
	else
	{
	}
	bytes = ACE_OS::write(theStopRequestEventHandles[1], buf, sizeof(buf ));
	if( bytes <= 0 )
	{
	}
	else
	{
	}
}//end of cancel
/*=================================================================
	ROUTINE: getPeriod
=================================================================== */
ACE_UINT64 ACS_LM_Timer::getPeriod() const
{
	return thePeriod;
}//end of getPeriod
