//=============================================================================
/**
   @file    acs_hcs_calendarScheduling.h

   @brief Header file for HC module.
          This class consists of functions which are useful in calculating the next scheduled time for calendar based events.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A                     XMALRAO   Initial Release
 */
//=============================================================================

#ifndef JOBSCHEDULER_H
#define JOBSCHEDULER_H

/*====================================================================
                     INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Task.h>
#include "acs_hcs_calenderPeriodicEvent_objectImpl.h"

/*====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define JAN 	1
#define FEB 	2
#define DEC 	12
#define ALL 	0
#define FIRST 	1
#define SECOND 	2
#define THIRD 	3
#define FOURTH 	4
#define LAST 	5

/*====================================================================
                       STATIC VARIABLE DECLARATION SECTION
==================================================================== */
/*
* @brief   This static array consists of last day for every month
*/
static int lastDay[]={31,28,31,30,31,30,31,31,30,31,30,31};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief jobSchedule 
 * jobSchedule class is used for caliculating next schedule time of a calendar based event.
 * It is having functions to calculate next scheduled time
 *
 */
class jobSchedule
{
	/*=====================================================================
                                       PUBLIC DECLARATION SECTION
        ==================================================================== */
	public:
		/**
                * @brief
                * single parameter constructor
                */
		jobSchedule(CALENDER_Parameters *request);

		/**
                 *  calcDelay method: This method will add next day to the current day.
                 *  @return void
                 */
		void calcDelay();

		/**
                 *  checkTrigger method : This method will check whether we reach next schedule day or not.
                 *  @return bool	: 'true' if date does not satisify the condition else 'false'
                 */
		bool checkTrigger();

		/**
                 *  updateNextTriggerTime method: This method will add 1 day to the current time.
                 *  @return string  : date in 'yyyy-mm-ddThh:mm:ss' format
                 */
		string updateNextTriggerTime();

		/**
                 *  timeToQuit method: This method will check whether we got the next execution time or not.
                 *  @return bool  : 'true' if next scheduled time is reached, else false
                 */
		bool timeToQuit();

		/**
                 *  setCurrentTime method: This method set the current time.
                 *  @return void
                 */
		void setCurrentTime();

		/**
                 *  dayName method: Given a Date in tm format, return its day.
                 *  @return int  : Ex: Monday=1,Tuesday=2...
                 */
		int dayName();

		/**
                 *  nextDay method : add one day to the current day.
                 *  @return void
                 */
		void nextDay();

		/**
                 *  isLeapYear method: To check whether the year is leap year or normal year.
                 *  @return bool : 'true' if year is a leap year, else 'false'
                 */
		bool isLeapYear();

		/**
                 *  lastDayOfMonth  method: This method is used to calculate last day in a given month.
                 *  @return int : last day of given month 28/29/30/31
                 */
		int lastDayOfMonth();

		/**
                 *  WeekNoOfTheDay  method: This method is used to calculate week number.
                 *  @return int : for first week '1', for second week '2'...
                 */
		int WeekNoOfTheDay();

		/**
                 *  getDateTime method: This method is used get date in 'yyyy-mm-ddThh:mm:ss' format.
                 *  @return int : for first week '1', for second week '2'...
                 */
		std::string getDateTime();

		/**
                 *  compareDates method : This method is used to compare two dates.
		 *  @param d1		: date in 'yyyy-mm-ddThh:mm:ss' format
		 *  @param d2		: date in 'yyyy-mm-ddThh:mm:ss' format 
                 *  @return bool  	: 'true' if d1<d2 else false
                 */
		bool compareDates(string d1, string d2);

		/**
                 *  isCombinationPossible method: This method is used is used to check whether the provided combination is possible or not.
                 *  @return bool : 'true' if combination is possible, else 'false'
         	 *  Ex : if dayOfMonth = 30 and month =2 then function will return false as the combination will never be possible
                 */
		bool isCombinationPossible();

	 /*=====================================================================
                                 PRIVATE DECLARATION SECTION
         ===================================================================== */
	private:
                int dom;
                int dow;
                int dowo;
                int mon;
                tm dtme;
                //tm crntDay;
                string currentTime;
                string startTimeString;
                string stopTimeString;
                string calTime;
			
};

#endif
