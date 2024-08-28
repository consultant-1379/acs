/*=================================================================== */
/**
   @file   acs_chb_clock_sync.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the ACS_CHB_ClockSync class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.

==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_CLOCKSYNC_H
#define ACS_CHB_CLOCKSYNC_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include "acs_chb_common.h"
#include <ace/Event.h>
#include <ace/ACE.h>

/*=====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Clock Synchronization result
 */
/*=================================================================== */

typedef char SyncResultType;                    

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Clock synchronization successful
 */
/*=================================================================== */

const SyncResultType SYNC_OK =     0;           

/*=================================================================== */
/**
   @brief Clock synchronization failed
 */
/*=================================================================== */

const SyncResultType SYNC_NOT_OK = 1;           

/*=================================================================== */
/**
   @brief Bit flags for checking event replication
 */
/*=================================================================== */

const int GETSYSTEMTIME_FAILED				= 1<<0;

/*=================================================================== */
/**
   @brief Bit flags for checking event replication
 */
/*=================================================================== */

const int SETPRIVILEGES_FAILED				= 1<<1;

/*=================================================================== */
/**
   @brief Bit flags for checking event replication
 */
/*=================================================================== */

const int GETSYSTEMTIMEADJUSTMENT_FAILED		= 1<<2;

/*=================================================================== */
/**
   @brief Bit flags for checking event replication
 */
/*=================================================================== */

const int SETSYSTEMTIMEADJUSTMENT_FAILED		= 1<<3;

/*=================================================================== */
/**
   @brief Bit flags for checking event replication
 */
/*=================================================================== */

const int DIFFTIME_TOO_BIG				= 1<<4;

/*=================================================================== */
/**
   @brief Bit flags for checking event replication
 */
/*=================================================================== */

const int CALLTIME_TOO_BIG				= 1<<5;

/*=================================================================== */
/**
   @brief Bit flags for checking event replication
 */
/*=================================================================== */

const int RESETSYSTEMTIMEADJUSTMENT_FAILED		= 1<<6;

/*=================================================================== */
/**
   @brief Reminder time in micro seconds.
 */
/*=================================================================== */

const ACE_UINT32 REMINDER_TIME_MS = 20000;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @class     ACS_CHB_ClockSync

      @brief     This class implements the synchronization between the 
		 AP and the CP time
 */
/*=================================================================== */

class ACS_CHB_ClockSync : public ACS_CHB_Event
{
        /*=====================================================================
                                        PUBLIC DECLARATION SECTION
        ==================================================================== */
public:
        /*=====================================================================
                                        CLASS CONSTRUCTOR
        ==================================================================== */
        /*=================================================================== */
        /**
              @brief     Constructor for ACS_CHB_ClockSync class

                             Constructor for the Clock Synchronizer synchronization class.

        */
        /*=================================================================== */

  	ACS_CHB_ClockSync (ACE_HANDLE stopEvent);
	
	/*=====================================================================
                                CLASS DESTRUCTOR
        ==================================================================== */
        /*=================================================================== */
        /**
                @brief     Destructor for ACS_CHB_ClockSync class.

                                   Destructor for the Clock Synchronizer synchronization class.

        */
        /*=================================================================== */

  	~ACS_CHB_ClockSync ();

        /*===================================================================
                                   PUBLIC METHODS
        =================================================================== */
        /*=================================================================== */
        /**
                @brief     Read system (AP) clock. The read system time is stored
			   in the private variable ap_time[index].

                @param     index
				This is index of the private variable ap_time.
				Values 0 and 1 are allowed.

                @return    SyncResultType
				SYNC_OK		Reading successful.
				SYNC_NOT_OK	Reading not successful.

        */
        /*=================================================================== */

  	SyncResultType readTime (const unsigned char index);

        /*=================================================================== */
        /**
                @brief     Checks for privileges to adjust the time.

                @return    Return status
				TRUE	Success
				FALSE	Failure

        */
        /*=================================================================== */

	bool CheckForPrivileges(); 

        /*=================================================================== */
        /**
                @brief     This module synchronizises the AP clock time with the
			   CP time. Sets time if required and correct the time
			   to allow synchronization of the system clock.

                @param     timebuf
				           The CP time to which the AP time is to be synchronized.

                @return    void

        */
        /*=================================================================== */

  	void synchronize (unsigned char* timebuf);

        /*=================================================================== */
        /**
                @brief     Read parameters from IMM.

		@param	   aOmHandler

                @return    ACS_CC_ReturnType
			   ACS_CC_FAILURE	Parameters not read
			   ACS_CC_SUCCESS	Parameters read

        */
        /*=================================================================== */

  	ACS_CC_ReturnType getParameters (OmHandler & aOmHandler);

        /*=================================================================== */
        /**
                @brief     Read AlarmList private data.

                @return    Return Value
				TRUE	Success
				FALSE	Failure	

        */
        /*=================================================================== */

  	bool AlarmIssued();

        /*=================================================================== */
        /**
                @brief     Issues a ClockSync Alarm.
			   Alarms are issues in the following cases:
			   1. Difference between CP and AP time exceeds "maxdeviation".
			   2. JTP connection to calltime exceeds "maxcalltime" for >10 times.
			   3. Failed to read CPtime >10 times.
			   4. Failed to initiate the JTP connection to the CP for >10 times.
			   Ceasing is issued when AP is Synced with the CP.

                @param     AlarmCause

                @return    void

        */
        /*=================================================================== */

  	void IssueAlarm(char AlarmCause[]);

private:

        /*===================================================================
                                   PRIVATE METHODS
        =================================================================== */
        /*=================================================================== */
        /**
                @brief     Cease the "clock not synchronized" alarm.
			   This method is only called when AP is Synchronized with CP.

                @return    void

        */
        /*=================================================================== */

  	void CeaseAlarm();

	/*=================================================================== */
        /**
                @brief     Converts the time read from the CP. The time is received
			   in a buffer of seven bytes containing year, month, day,
			   hour, minute, second and deciseconds. The time is translated 
			   to elapsed seconds since 00:00:00 UTC, January 1, 1970.

                @param     timebuf
				Data buffer containing date and time.

                @return    Timeval structure
				Elapsed seconds since 00:00:00 UTC, January 1, 1970.

        */
        /*=================================================================== */

  	timeval convertCPtime (unsigned char* timebuf) const;

	/*=================================================================== */
        /**
                @brief     Tries to adjust the AP time and if adjustment is done
			   we will go to sleep for 10s ... 30s

                @param     adj
				The time to be adjusted.

                @return    Return status
				TRUE	Success
				FALSE	Failure

        */
        /*=================================================================== */

  	bool AdjustTime(long adj);
  	/*=================================================================== */
        /**
                        @brief     timerAdd

                        @param     t1

                        @param     t2

                        @return    timeval
        */
        /*=================================================================== */
	timeval timerAdd( timeval t1,timeval t2) const ;
	/*=================================================================== */
        /**
                        @brief     timerdiff

                        @param     t1

                        @param     t2

                        @return    timeval
        */
        /*=================================================================== */
	timeval timerdiff(timeval t1, timeval t2) const;
	/*=================================================================== */
        /**
                        @brief     timernorm

                        @param     t1

                        @return    timeval
        */
        /*=================================================================== */
	inline timeval timernorm(timeval t1) const;

 	/*===================================================================
                                   PRIVATE ATTRIBUTES
        =================================================================== */
	/*=================================================================== */
        /**
                @brief   Maximum time in microseconds that a request to read the CP
			 clock may take unless the result is discarded.

        */
        /*=================================================================== */

  	ACE_INT64 maxcalltime;			

 	/*=================================================================== */
        /**
                @brief   Minimum time in microseconds that the CP clock and the AP
			 clock may differ in order to adjust the time.

        */
        /*=================================================================== */

  	ACE_INT64 mindeviation;			

        /*=================================================================== */
        /**
                @brief   Maximum time in seconds that the CP clock and the AP clock
			 may differ before an APZ alarm is issued.

        */
        /*=================================================================== */

  	unsigned short maxdeviation;		

        /*=================================================================== */
        /**
                @brief   Contains the system time immediately before sending a clock
			 inquiry to the CP and the system time immediately after 
			 receiving data from the CP.

        */
        /*=================================================================== */

 
  	timeval ap_time [2];		

        /*=================================================================== */
        /**
                @brief  Used these to check if there is been a jump in either time 

        */
        /*=================================================================== */

	timeval	cpTimeWas, apTimeWas; 

        /*=================================================================== */
        /**
                @brief   These flag the cause of any sudden jump in time discrepancy

        */
        /*=================================================================== */

	bool cpTimeJumped, apTimeJumped; 

        /*=================================================================== */
        /**
                @brief   True if an APZ alarm has been issued.

        */
        /*=================================================================== */
 
  	bool alarmInList;		

        /*=================================================================== */
        /**
                @brief   Will hold the clock interrupt period, in 100-ns units

        */
        /*=================================================================== */

	unsigned long	tick; 

        /*=================================================================== */
        /**
                @brief   Used to adjust the time.

        */
        /*=================================================================== */

	unsigned long	timeAdj;

        /*=================================================================== */
        /**
                @brief   Used to adjust the time.

        */
        /*=================================================================== */

        bool timeAdjDisabled;

        /*=================================================================== */
        /**
                @brief   Check this flag to see whether access token mask has been set already

        */
        /*=================================================================== */

	bool privilegesSet;  

        /*=================================================================== */
        /**
                @brief   Used to see if a reminder needs sending for an unresolved event

        */
        /*=================================================================== */

	ACE_UINT32 reminderFlags; //Used to see if a reminder needs sending for an unresolved event
public:
 	/*===================================================================
                                   PUBLIC ATTRIBUTES
        =================================================================== */
        /*=================================================================== */
        /**
                @brief   Sleep time used while adjusting the time(in ms).

        */
        /*=================================================================== */

	ACE_INT32 sleepTime; 

        /*=================================================================== */
        /**
                @brief   Count of successive times the reading of CP time been too slow

        */
        /*=================================================================== */

	int reminderCnt; 

        /*=================================================================== */
        /**
                @brief   Used for synchronization

        */
        /*=================================================================== */

	unsigned short	cycletime; 

        /*=================================================================== */
        /**
                @brief   Flag used to set that time adjustment hasn't been completed

        */
        /*=================================================================== */

	bool amAdjusting;	
	

	/*=================================================================== */
	/**
                @brief   ACE_HANDLE stopPipeHandle

	 */
	/*=================================================================== */

	ACE_HANDLE stopPipeHandle;

        /*=================================================================== */
        /**
                @brief   Used to force a terminate/initiate sequence

        */
        /*=================================================================== */

	bool jtpResponseTooSlow; 
};


#endif
