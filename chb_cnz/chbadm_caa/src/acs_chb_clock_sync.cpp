/*=================================================================== */
   /**
   @file acs_chb_clock_sync.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the acs_chb_clock_sync.h module.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <sys/timex.h>
#include <ace/Process.h>
#include <acs_chb_event.h>
#include <acs_chb_clock_def.h>
#include <acs_chb_clock_sync.h>
#include <acs_chb_tra.h>
#include <acs_chb_clocksync_class.h>


using namespace std;

#define TRUE 1
#define FALSE 0


/*===================================================================
   ROUTINE: ACS_CHB_ClockSync
=================================================================== */

ACS_CHB_ClockSync::ACS_CHB_ClockSync(ACE_HANDLE stopEvent) : ACS_CHB_Event (ACS_CHB_processName), maxcalltime (0), mindeviation (0),
maxdeviation (0),
alarmInList (false)
{
	DEBUG(1,"%s", "Executing ACS_CHB_ClockSync constructor");
	cpTimeJumped = false;
	apTimeJumped =false;
	jtpResponseTooSlow =false;
	tick=0;
	timeAdj=0;
	cycletime=0;
	reminderFlags=0;
	cpTimeWas.tv_sec=cpTimeWas.tv_usec=0;
	apTimeWas.tv_sec=apTimeWas.tv_usec=0;
	amAdjusting=false;
	acs_chb_clocksync_class::setStopSignal(false);
	
	// Check for privileges to adjust the system time
	privilegesSet=CheckForPrivileges();

	timeAdjDisabled=TRUE;
    // TR HQ37527 - BEGIN
	stopPipeHandle = stopEvent;
	// TR HQ37527 - END
	reminderCnt=0;
	ap_time[0].tv_sec = ap_time[0].tv_usec = 0;	
	ap_time[1].tv_sec = ap_time[1].tv_usec = 0;	
	sleepTime=0;		

} 

/*===================================================================
   ROUTINE: ~ACS_CHB_ClockSync
=================================================================== */

ACS_CHB_ClockSync::~ACS_CHB_ClockSync ()
{
	DEBUG(1,"%s", "Executing ACS_CHB_ClockSync destructor");
} 

/*===================================================================
   ROUTINE: CheckForPrivileges
=================================================================== */

bool ACS_CHB_ClockSync::CheckForPrivileges()
{
	// Checks if it has root privileges
	// Only superuser can adjust the time in linux
	DEBUG(1,"%s", "Entered ACS_CHB_ClockSync::CheckForPrivileges()");
	uid_t uid;
    	uid = ACE_OS::getuid();

    	if( uid == 0)
    	{
	    DEBUG(1,"%s", "Privileges to adjust the system time are present");
            return true;
    	}
    	else
    	{
	    DEBUG(1,"%s", "Not the Super user, no sufficient privileges to adjust the system time");
            return false;
	}
}

/*===================================================================
   ROUTINE: readTime
=================================================================== */

SyncResultType
ACS_CHB_ClockSync::readTime (const unsigned char index)
{ 
	DEBUG(1,"%s", "Entered ACS_CHB_ClockSync::readTime()");

	time_t sysTime;
        struct timeval tv;
        gettimeofday(&tv, NULL);
	sysTime =time(NULL);
        struct tm * timeStruct;
        timeStruct = localtime(&sysTime);
	timeStruct->tm_isdst = 0;
	tv.tv_sec = mktime(timeStruct);
  	ap_time[index].tv_sec = tv.tv_sec;
	ap_time[index].tv_usec = tv.tv_usec;	

	DEBUG(1,"%s", "Exited ACS_CHB_ClockSync::readTime()");

	return SYNC_OK;
} 

/*===================================================================
   ROUTINE: synchronize
=================================================================== */

void ACS_CHB_ClockSync::synchronize (unsigned char* timebuf)
{
	DEBUG(1,"%s", "Entered ACS_CHB_ClockSync::synchronize()");

	timeval time =      {0, 0};
	timeval cp_time =   {0, 0};
	timeval call_time = {0, 0};
	timeval comp_time = {0, 0};
	timeval diff_time = {0, 0};
	char		problemData [PROBLEMDATASIZE]; 
	//char		dumStr[PROBLEMDATASIZE]; 
	
	amAdjusting=false; //Outside world can check this to see if adjustment is under way.

	call_time = timerdiff(ap_time [1], ap_time [0]);

	ACE_INT64 callTime_us = 0;
	callTime_us = (ACE_INT64)(call_time.tv_sec*1E6+call_time.tv_usec);

	// Check if the calling time is within the permitted limit
	if (callTime_us < maxcalltime)
	{
		cp_time = convertCPtime (timebuf);
		// Use same principle as above to find out whether a jump
		// took place in the AP or the CP time:
		DEBUG(1,"cp_time : %lu ", cp_time.tv_sec);
		DEBUG(1,"cpTimeWas : %lu ", cpTimeWas.tv_sec);
		diff_time = timerdiff(cp_time, cpTimeWas);
		DEBUG(1,"diff_time for cp_time : %lu ", diff_time.tv_sec);
		DEBUG(1,"sleepTime  : %lu ", sleepTime);
		long sleepTimeSecs=sleepTime/1000;
		diff_time.tv_sec=labs(diff_time.tv_sec-sleepTimeSecs);
		DEBUG(1,"diff_time after for cp : %lu ", diff_time.tv_sec);
		cpTimeJumped=(cpTimeWas.tv_sec>0 && diff_time.tv_sec>120); //true if jump by more than 2 min, kind of arbitrary but reasonable
		DEBUG(1,"ap_time[1] : %lu ", ap_time[1].tv_sec);
		DEBUG(1,"apTimeWas : %lu ", apTimeWas.tv_sec);
		diff_time = timerdiff(ap_time[1], apTimeWas);
		DEBUG(1,"diff_time for ap_time : %lu ", diff_time.tv_sec);
		diff_time.tv_sec=labs(diff_time.tv_sec-sleepTimeSecs);
		DEBUG(1,"diff_time after for ap : %lu ", diff_time.tv_sec);
		apTimeJumped=(apTimeWas.tv_sec>0 && diff_time.tv_sec>120);//true if jump by more than 2 min, kind of arbitrary but reasonable
		//Note that the mechanism of comparing AP clock increment with sleepTime breaks down if the JTP response is
		//slow enough to cause an error - therefore the !jtpResponseTooSlow term in the following predicate. jtpResponse
		//too slow is not reset until after this check
		if ((cpTimeJumped||apTimeJumped) && !jtpResponseTooSlow)
		{
			if(cpTimeJumped)
			{
				DEBUG(1,"%s", "Jump in CP time by >2 min.");
			}
			if(apTimeJumped)
			{
				DEBUG(1,"%s", "Jump in AP time by >2 min.");
			}
		}
		cpTimeWas.tv_sec=cp_time.tv_sec;
		cpTimeWas.tv_usec=cp_time.tv_usec;
		apTimeWas.tv_sec=ap_time[1].tv_sec;
		apTimeWas.tv_usec=ap_time[1].tv_usec;

		jtpResponseTooSlow=false;

		sleepTime=cycletime*1000; //Will sleep this much if sync doesn't need to adjust. Need this
							//value to check for abnormal time jumps.

		// Compensate for delays in communication

		time.tv_usec = call_time.tv_usec/2;
		comp_time = timerAdd(cp_time, time);

		// Calculate the difference between CP time and AP time

		diff_time = timerdiff(comp_time, ap_time [1]);

		char   str [5][64];
		double d_time;
		DEBUG(1,"CP time is %d secs and %d usecs", cp_time.tv_sec, cp_time.tv_usec);
		DEBUG(1,"AP time is %d secs and %d usecs", ap_time[1].tv_sec, ap_time[1].tv_usec);

		d_time = ap_time [1].tv_sec + (double)ap_time [1].tv_usec/MICROSEC;
		ACE_OS::sprintf (str [0], "\n%s%17.6f s", "AP clock............: ", d_time);  

		d_time = cp_time.tv_sec + (double)cp_time.tv_usec/MICROSEC;
		ACE_OS::sprintf (str [1], "\n%s%17.6f s", "CP clock............: ", d_time);  

		d_time = call_time.tv_sec + (double)call_time.tv_usec/MICROSEC;
		ACE_OS::sprintf (str [2], "\n%s%17.6f s", "Call time...........: ", d_time);

		d_time = comp_time.tv_sec + (double)comp_time.tv_usec/MICROSEC;
		ACE_OS::sprintf (str [3], "\n%s%17.6f s", "Compensated CP clock: ", d_time);

		d_time = diff_time.tv_sec + (double)diff_time.tv_usec/MICROSEC;
		ACE_OS::sprintf (str [4], "\n%s%17.6f s", "Time difference.....: ", d_time);

		DEBUG(1,"======== Clock synchronizer data ======== %s %s %s %s %s",str [0], str [1], str [2], str [3], str [4]);

		// Lets Synchronize if diff_time is within limits
		if(labs(diff_time.tv_sec) < maxdeviation)
		{
			// At this point we know that we are keeping the AP in Sync 

			// cease APZ alarm 
			if ( alarmInList == true )	
			{
			    CeaseAlarm();
			}
			DEBUG(1,"diff_time.tv_usec =  %d", labs(diff_time.tv_usec));
			DEBUG(1,"mindeviation = %d ", mindeviation);
			if ((diff_time.tv_sec != 0) || (labs(diff_time.tv_usec) > mindeviation)) //NB! labs inserted by qabpapi. This cannot have ver worked properly before.
			{	
				long sec_adj = diff_time.tv_sec;         
				long dwTimeAdjustment; //It can be -ve too! - qabpapi
				long usec_adj = diff_time.tv_usec;  

				sec_adj = sec_adj * 1000;    // ms

				dwTimeAdjustment = sec_adj + (usec_adj+500)/1000;  // time adjustment in ms
				//dwTimeAdjustment =  (usec_adj+500)/1000;  // time adjustment in ms
				struct timeval adj;
				adj = timerAdd(ap_time[1], diff_time);

				// Adjust CP time and sleep
				AdjustTime(dwTimeAdjustment); 
			} 
		}
		else
		{ //Do this if deviation too big, i.e. >maxdeviation (qabpapi for himself).
			
			if ( alarmInList == false )
			{// Issue APZ alarm
				ACE_OS::sprintf(problemData, "REASON\nDIFFERENCE BETWEEN CP AND AP TIME EXCEEDS %d S.\n", maxdeviation);
				if(apTimeJumped)
					strcat(problemData, "THERE WAS A JUMP IN THE AP TIME.\n");
				if(cpTimeJumped)
					strcat(problemData, "THERE WAS A JUMP IN THE CP TIME.\n");

				//
				// This Alarm is Ceased next time we successfully synchronize
				//
				IssueAlarm(problemData);
			}
		}
		reminderFlags&= ~CALLTIME_TOO_BIG;
		reminderCnt=0;
	}
	else 
	{ //Get here if it took too long to get clock data from CP

                if(reminderCnt == 9) //Let us take 10 times as the limit. If clock reading fails more than this,
                { //the time sync may be becoming dangerously large.
                        ACE_OS::sprintf(problemData,
                                "REASON\nEXCESSIVE DELAY IN GETTING CP TIME %d TIMES IN SUCCESSION.\n", reminderCnt+2);
          	        IssueAlarm(problemData);
		}
		else 
		{
                        ACE_OS::sprintf(problemData, "Took %ld ms to read CP time, > maxCallTime which is %ld ms, therefore not reliable enough",
                                callTime_us/1000, maxcalltime/1000);
		}

						
                if(reminderFlags&CALLTIME_TOO_BIG)
                {       //After use-case discussion on 18 June 1999, decided that operator doesn't need to see
                        //these reminders (but they are useful when testing).
                        reminderCnt++;
                        if(reminderCnt<10)
			{
                                strcat(problemData, " (THIS IS A REMINDER)");
				                        event(  ACS_CHB_ClockUnreliable,
                                        ACS_CHB_EVENT_STG,
                                        ACS_CHB_Cause_APfault,//cause, //
                                        problemData,
                                        ACS_CHB_Text_ProblemClock);
			}
                        //On the other hand, if the fault presists long enough, we should raise an alarm,
                        //as the time discrepancy may be increasing out of control.
                }
		if(reminderCnt == 0)
		{
                        event(  ACS_CHB_ClockUnreliable,
                                        ACS_CHB_EVENT_STG,
                                        ACS_CHB_Cause_APfault,//cause, //
                                        problemData,
                                        ACS_CHB_Text_ProblemClock);
		}

                reminderFlags|=CALLTIME_TOO_BIG;

	}
} 

/*===================================================================
   ROUTINE: getParameters
=================================================================== */

ACS_CC_ReturnType ACS_CHB_ClockSync::getParameters (OmHandler& aOmHandler)
{ 
	DEBUG(1, "%s", "Entering ACS_CHB_ClockSync::getParameters");

  	ACS_CC_ReturnType rcode = ACS_CC_SUCCESS;
	char	problemData [PROBLEMDATASIZE] = { 0 };   

	std::string dnObject("");
	//dnObject.append(CLOCKSYNC_PARAM_OBJ_DN);
	//dnObject.append(",");
        dnObject.append(ACS_CHB_Common::dnOfCsyncRootObj);

	ACS_CC_ImmParameter* paramToFind[3] = { 0 };

	paramToFind[0] = new ACS_CC_ImmParameter();

  	// Parameter `maxCallTime'
	DEBUG(1,"%s", "Fetching maxCallTime.");

	paramToFind[0]->attrName = (char*)ACS_CHB_maxCallTime;

	if( aOmHandler.getAttribute( dnObject.c_str(), paramToFind[0] ) == ACS_CC_SUCCESS )	
	{
    	    maxcalltime = *( reinterpret_cast < ACE_UINT64 *> (*(paramToFind[0]->attrValues))) ;
	    if(maxcalltime < 500000 || maxcalltime > 1000000)
	    {
	    	DEBUG(1,"%s","Receievd value is not in range. Set to dafault maxCallTime value.");
		maxcalltime = 500000;
	    }
	    DEBUG(1,"maxCallTime value is %u", maxcalltime);
  	}
  	else 
  	{
    	    // Event handling
    	    ACE_OS::snprintf (problemData, sizeof(problemData)/sizeof(*problemData) - 1,
                        ("%d:Error when reading the IMM parameter %s"),
                         __LINE__,
                         ACS_CHB_maxCallTime);

    	    event (ACS_CHB_ReadFailedPHA,
            		ACS_CHB_EVENT_STG,
            		ACS_CHB_Cause_APfault,
           		problemData, 
           		ACS_CHB_Text_ProblemPHA);

    	    rcode = ACS_CC_FAILURE;
  	}	 

	paramToFind[1] = new ACS_CC_ImmParameter();

  	// Parameter `minDeviation'
	DEBUG(1,"%s", "Fetching minDeviation ...");

	paramToFind[1]->attrName = (char*)ACS_CHB_minDeviation;

  	if ( aOmHandler.getAttribute( dnObject.c_str(), paramToFind[1] ) == ACS_CC_SUCCESS )
  	{
    	    mindeviation = *( reinterpret_cast < ACE_UINT64 *> (*(paramToFind[1]->attrValues))) ;
	    if(mindeviation < 50000)
            {
                DEBUG(1,"%s","Receievd value is not in range. Set to default value for minDeviation.");
		mindeviation = 50000;
            }
	    DEBUG(1, "minDeviation value is %u", mindeviation);
  	}
  	else 
  	{
    	    // Event handling
    	    ACE_OS::snprintf (problemData, sizeof(problemData)/sizeof(*problemData) - 1,
                        ("%d:Error when reading the IMM parameter %s"),
                         __LINE__,
                         ACS_CHB_minDeviation);

    	    event (ACS_CHB_ReadFailedPHA,
           	        ACS_CHB_EVENT_STG,
           	        ACS_CHB_Cause_APfault,
           		problemData, 
           		ACS_CHB_Text_ProblemPHA);

    	    rcode = ACS_CC_FAILURE;
  	} 

  	// Get data from the parameter handler

  	// Parameter `maxDeviation'
	paramToFind[2] = new ACS_CC_ImmParameter();

	DEBUG(1,"%s", "Fetching maxDeviation ....");

	 paramToFind[2]->attrName = (char*)ACS_CHB_maxDeviation;

	if ( aOmHandler.getAttribute( dnObject.c_str(),paramToFind[2] ) == ACS_CC_SUCCESS )
	{
		
    	    maxdeviation = *( reinterpret_cast < unsigned short *> (*(paramToFind[2]->attrValues))) ;
	    if(maxdeviation < 600)
	    {
            	DEBUG(1,"%s","Receievd value is out of range. Setting default value for maxdeviation.");
		maxdeviation = 600;
            }
	    DEBUG(1,"maxDeviation value is %u", maxdeviation);
  	}
  	else 
  	{
    	    // Event handling
    	    ACE_OS::snprintf (problemData, sizeof(problemData)/sizeof(*problemData) - 1,
                        ("%d:Error when reading the IMM parameter %s"),
                         __LINE__,
                         ACS_CHB_maxDeviation);

    	    event (ACS_CHB_ReadFailedPHA,
           		ACS_CHB_EVENT_STG,
           		ACS_CHB_Cause_APfault,
           		problemData, 
           		ACS_CHB_Text_ProblemPHA);

    	    rcode = ACS_CC_FAILURE;
  	}
	for( int ctr = 0 ; ctr < 3; ctr++ )
	{
		delete paramToFind[ctr];
		paramToFind[ctr] = 0;
	}

	DEBUG(1, "%s", "Leaving ACS_CHB_ClockSync::getParameters");

   	return rcode;
} 

/*===================================================================
   ROUTINE: AlarmIssued
=================================================================== */

bool ACS_CHB_ClockSync::AlarmIssued(void)
{
  	return alarmInList;
} 

/*===================================================================
   ROUTINE: IssueAlarm
=================================================================== */

void ACS_CHB_ClockSync::IssueAlarm(char AlarmCause[])
{
	// Do not allow more than one alarm before ceased
	if (alarmInList == true) return;

	event(	ACS_CHB_ClockNotSync,
			ACS_CHB_ALARM_STG, 
			ACS_CHB_Cause_ClockNotSync,
			"", 
			AlarmCause);

	alarmInList = true;

} // End of IssueAlarm

/*===================================================================
   ROUTINE: CeaseAlarm
==========================S========================================= */

void ACS_CHB_ClockSync::CeaseAlarm(void)
{
	if (alarmInList == false) return; // Just to be sure

	// Ceasing is issued
	event(	ACS_CHB_ClockNotSync,
			ACS_CHB_CEASING_STG,
			ACS_CHB_Cause_ClockNotSync,
			"AP CLOCK NOT SYNCHRONIZED - APZ alarm ceasing.",
			"");

	alarmInList = false;

} // End of CeaseAlarm

/*===================================================================
   ROUTINE: convertCPtime
=================================================================== */

timeval ACS_CHB_ClockSync::convertCPtime (unsigned char* timebuf) const
{
	DEBUG(1,"%s", "Executing ACS_CHB_ClockSync::convertCPtime()");
  	const unsigned char YEAR =       0;
  	const unsigned char MONTH =      1;
  	const unsigned char DAY =        2;
  	const unsigned char HOUR =       3;
  	const unsigned char MINUTE =     4;
  	const unsigned char SECOND =     5;
  	const unsigned char DECISECOND = 6;

  	tm                  timeptr;
  	timeval             time;

  	// Convert CP time to the number of elapsed seconds since 00:00:00 UTC,
  	// January 1, 1970.
	DEBUG(1,"CP time(ddmmyy hh:mm:ss.usec) is %d-%d-%d %d:%d:%d ", (int)(timebuf[2]), (int)(timebuf[1]), (int)(timebuf[0]) + 1900, (int)(timebuf[3]), (int)(timebuf[4]), (int)(timebuf[5]));

  	timeptr.tm_sec = (int) *(timebuf+SECOND);
  	timeptr.tm_min = (int) *(timebuf+MINUTE);
  	timeptr.tm_hour =(int) *(timebuf+HOUR);
  	timeptr.tm_mday =(int) *(timebuf+DAY);
  	timeptr.tm_mon =  ((int) * (timebuf+MONTH)) -1 ;
  	timeptr.tm_year =  (int) *(timebuf+YEAR);
	timeptr.tm_wday = 0;
	timeptr.tm_yday = 0;
	timeptr.tm_isdst = 0;


  	time.tv_sec = mktime (&timeptr);
	time.tv_usec = 100000 * timebuf[DECISECOND];

	DEBUG(1,"Time in secs and usecs is %d : %d", time.tv_sec, time.tv_usec);

  	return time;
} 

timeval
ACS_CHB_ClockSync::timerAdd (timeval time1, timeval time2) const
{
  // Private method
  timeval sum;

  // Add the timer values stored in `time1' and `time2'

  sum.tv_sec = time1.tv_sec + time2.tv_sec;
  sum.tv_usec = time1.tv_usec + time2.tv_usec;
  sum = timernorm (sum);
  return sum;
} // End of timerAdd

timeval
ACS_CHB_ClockSync::timerdiff (timeval time1, timeval time2) const
{
  // Private method
  timeval diff;

  // Subtract the timer value stored in `time2' from the timer value
  // stored in `time1'

  diff.tv_sec = time1.tv_sec - time2.tv_sec;
  diff.tv_usec = time1.tv_usec - time2.tv_usec;
  diff = timernorm (diff);
  return diff;
} // End of timerdiff

inline timeval
ACS_CHB_ClockSync::timernorm (timeval time) const
{
  // Normalize the timer value. Private method

  if (time.tv_usec >= MICROSEC)
  {
    time.tv_sec++;
    time.tv_usec -= MICROSEC;
  }
  else if (time.tv_usec <= -MICROSEC)
  {
    time.tv_sec--;
    time.tv_usec += MICROSEC;
  }
  if (time.tv_sec > 0)
  {
    if (time.tv_usec < 0)
    {
      time.tv_sec--;
      time.tv_usec += MICROSEC;
    }
  }
  else if (time.tv_sec < 0)
  {
    if (time.tv_usec > 0)
    {
      time.tv_sec++;
      time.tv_usec -= MICROSEC;
    }
  }
  return time;
} 

/*===================================================================
   ROUTINE: AdjustTime
=================================================================== */

bool
ACS_CHB_ClockSync::AdjustTime(long adj)
{
        char         problemData [PROBLEMDATASIZE];
        char           dumStr[PROBLEMDATASIZE];
        bool canAdjust = true; 

        // Check for privileges needed for adjusting system clock used to be done here on every call. 
        if(!privilegesSet)
        {
        	privilegesSet = CheckForPrivileges();
        }
        if(!privilegesSet)
        {
        	DEBUG(1,"%s", "Not a superuser, no sufficient privileges to adjust AP time");
        	ACE_OS::sprintf(problemData, "Not a superuser, no sufficient privileges to adjust AP time");
        	if(reminderFlags & SETPRIVILEGES_FAILED)
        	{
        		strcat(problemData, " (This is a reminder)");
        	}
        	event(ACS_CHB_SysCallFailed,
        			ACS_CHB_EVENT_STG,
        			ACS_CHB_Cause_APfault,
        			problemData,
        			ACS_CHB_Text_ProblemClock);
        	canAdjust=false;
        	reminderFlags|= SETPRIVILEGES_FAILED;
        }
        else
        {
        	DEBUG(1,"%s", "Super user, have the privileges to adjust the time");
        	reminderFlags &= ~SETPRIVILEGES_FAILED;
        }

        DEBUG(1,"Time to be adjusted to %d  milliseonds", adj);

        //Get the current clock information.
        struct timex buf;
        memset( &buf, 0, sizeof(struct timex));
        buf.modes = 0;

        int returnValue = 0;
        int newTick = 0, oldTick = 0;

        returnValue = adjtimex(&buf) ;
        if( returnValue < 0 )
        {
        	//If this fails, can't do job, send event.
        	int lastError = ACE_OS::last_error();
        	sprintf(dumStr, "%s", "Error occurred while getting clock tick");
        	ACE_OS::sprintf(problemData, "System error no. %d, %s", lastError, dumStr);
        	if(reminderFlags & GETSYSTEMTIMEADJUSTMENT_FAILED)
        	{
        		strcat(problemData, " (This is a reminder)");
        	}
        	event(ACS_CHB_SysCallFailed,
        			ACS_CHB_EVENT_STG,
        			ACS_CHB_Cause_APfault,
        			problemData,
        			ACS_CHB_Text_ProblemClock);

        	reminderFlags |= GETSYSTEMTIMEADJUSTMENT_FAILED;
        	canAdjust=false;
        }
        else
        {
        	oldTick = buf.tick;
        	DEBUG(1,"Get the original clock tick %d", oldTick);
        	reminderFlags &= ~GETSYSTEMTIMEADJUSTMENT_FAILED;
        }

        // Now we need to adjust the clock. If the adjustment is negative,
        // we will set the number of microseconds to be incremented in
        // each tick to 9000. If the adjustment is positive, we will set the number of
        // microseconds to be adjusted to 11000. If the adjustment is zero nothing to be done.

        if( adj > 0 )
        {
        	newTick = 11000;
        	DEBUG(1,"New Tick Value is %d", newTick);
        }
        else if( adj < 0)
        {
        	newTick = 9000;
        	DEBUG(1,"New Tick Value is %d", newTick);
        }
        else
        {
        	return amAdjusting;
        }

        memset( &buf, 0, sizeof(struct timex));
        buf.modes = ADJ_TICK;
        buf.tick = newTick;

        returnValue = adjtimex(&buf);
        if( returnValue < 0 )
        {
        	int lastError = ACE_OS::last_error();
        	sprintf(dumStr, "%s", "Error occurred while setting the new tick.");
        	ACE_OS::sprintf(problemData, "System error no. %d, %s", lastError, dumStr);
        	if(reminderFlags & SETSYSTEMTIMEADJUSTMENT_FAILED)
        	{
        		strcat(problemData, " (This is a reminder)");
        	}
        	event(ACS_CHB_SysCallFailed,
        			ACS_CHB_EVENT_STG,
        			ACS_CHB_Cause_APfault, //="AP INTERNAL FAULT"
        			problemData,
        			ACS_CHB_Text_ProblemClock); //="Problem with clock synchronizing"

        	reminderFlags |= SETSYSTEMTIMEADJUSTMENT_FAILED;
        	canAdjust=false; //return;
        }
        else
        {
        	reminderFlags&=~SETSYSTEMTIMEADJUSTMENT_FAILED;
        }
        //Now, calcuate the time to sleep
        if(canAdjust)
        {
        	if(newTick != 0)
        	{
        		sleepTime = 10*abs(adj) + abs( adj);
        		DEBUG(1,"Time in which complete deviation can be adjusted - %d", sleepTime);
        		//This is the time, in ms, it should take to reel in the discrepancy
        	}
        	/*
	    else
	    {
		sleepTime=cycletime*1000;
		DEBUG(1,"newTick is not zero. SleepTime = %d", sleepTime);
	    }
        	 */
        	if(sleepTime > (cycletime*1000)) //Rethink here. If ~T>cycletime then just sleep (and adjust) for one cycle,
        	{
        		sleepTime=cycletime*1000;	//then go round again. Some factor may alter the discrepancy while we are
        		DEBUG(1,"Final SleepTime = %d", sleepTime);
        	}

        	ACE_Time_Value pingTime;
        	timespec_t ping_tv;
        	ping_tv.tv_sec = sleepTime/1000;
        	ping_tv.tv_nsec = (sleepTime%1000)*1000000;
        	pingTime.set(ping_tv);

        	DEBUG(1,"SleepTime secs= %d", ping_tv.tv_sec);
        	DEBUG(1,"SleepTime nano seconds= %d", ping_tv.tv_nsec);

        	//Sleep till the clock reels in the adjustment.
        	// TR HQ37527 - BEGIN
        	ACE_Handle_Set readHandleSet;
        	readHandleSet.set_bit( stopPipeHandle);

        	int status = ACE::select( readHandleSet.max_set()+1, readHandleSet, &pingTime);

        	if (status > 0)
        	{
        		DEBUG(1,"%s", "Woke up from sleeping as stopEvent is signalled !");
        	}
        	else
        	{
        		DEBUG(1,"%s", "Woke up after sleeping");
        	}
        	// TR HQ37527 - END

        	//Set amAdjusting as true;
        	amAdjusting=true;

        	memset( &buf, 0, sizeof(struct timex));
        	buf.tick = oldTick;
        	buf.modes = ADJ_TICK;

        	returnValue = adjtimex(&buf);
        	if( returnValue < 0 )
        	{
        		int lastError = ACE_OS::last_error();
        		sprintf(dumStr, "%s", "Error occurred while setting the new tick.");
        		ACE_OS::sprintf(problemData, "System error no. %d, %s", lastError, dumStr);
        		if(reminderFlags & RESETSYSTEMTIMEADJUSTMENT_FAILED)
        		{
        			strcat(problemData, " (This is a reminder)");
        		}
        		event(ACS_CHB_SysCallFailed,
        				ACS_CHB_EVENT_STG,
        				ACS_CHB_Cause_APfault,
        				problemData,
        				ACS_CHB_Text_ProblemClock);

        		reminderFlags|=RESETSYSTEMTIMEADJUSTMENT_FAILED;
        	}
        	else
        	{
        		reminderFlags&=~RESETSYSTEMTIMEADJUSTMENT_FAILED;
        	}
        }
        DEBUG(1,"Set the clock tick back to original value %d", oldTick);
        return amAdjusting;
}
