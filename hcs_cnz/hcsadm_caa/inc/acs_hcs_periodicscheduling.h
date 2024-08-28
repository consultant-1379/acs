//=============================================================================
/**
   @file    acs_hcs_periodicscheduling.h

   @brief Header file for HC module.
          This class consists of functions which are useful in calculating the next scheduled time for all periodic events.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       		   XMALRAO   Initial Release
 */
//=============================================================================
#ifndef PERIODIC_SCHEDULING_H
#define PERIODIC_SCHEDULING_H

#include <string>
/*====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define JAN 1
#define FEB 2
#define DEC 12
#define WK  7

/*====================================================================
                        STRUCT DECLARATION SECTION
==================================================================== */
/*
* @brief   This structure is used to hold the values of date which is of 'yyyy-mm-ddThh:mm:ss.0Z" format
*/
struct DT
{
	int dd;
	int mon;
	int yy;
	int hh;
	int mm;
	int ss;
};

/*
* @brief   This structure is used to hold the periodicity values
*/
struct TM
{
	int mon;
	int wk;
	int dd;
	int hh;
	int mm;
	int ss;
};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief acs_hcs_periodicscheduling
 * acs_hcs_periodicscheduling class is used for caliculating next schedule time of any periodic event.
 * It is having functions to calculate next scheduled time
 *
 */
class acs_hcs_periodicscheduling
{
	/*=====================================================================
                                       PUBLIC DECLARATION SECTION
        ==================================================================== */
	public:
        	/**
         	* @brief
	        * default constructor
        	*/
		acs_hcs_periodicscheduling();

		/**
                * @brief
                * default destructor
                */	
		~acs_hcs_periodicscheduling();

		/**
		 *  nextSchedule  method: This method will call the functions required to calculate next schedule time.
		 *  @param  d1  : date in "yyyy-mm-ddThh:mm:ss" format 
		 *  @param  d2  : string of periodicity in "months-weeks-daysThours:minutes:seconds" format
		 *  @param  day : value of day provided in start time
		 *  @return string : next scheduled time in "yyyy-mm-ddThh:mm:ss" format
		 */	
		std::string nextSchedule(std::string d1,std::string d2,int day);

		/**
                 *  lastDayOfMonth  method: This method is used to calculate last day in a given month.
                 *  @param  dt  : date in DT format
                 *  @return int : last day of given month 28/29/30/31
                 */
		int lastDayOfMonth(DT &dt);

		/**
                 *  addWeeks method: This method is used to add weeks to the last scheduled date.
                 *  @param  dt   : date in DT format as reference
		 *  @param  int  : number of weeks in periodicity
                 *  @return void : no return value
                 */
		void addWeeks(DT &dt,int wk);

	 	/**
                 *  addDays method: This method is used to add days to the last scheduled date.
                 *  @param  dt   : date in DT format as reference
                 *  @param  int  : number of dayss in periodicity
                 *  @return void : no return value
                 */	
		void addDays(DT &dt,int days);

		/**
                 *  addTime method: This method is used to add time(hours and minutes) to the last scheduled date.
                 *  @param  dt  : date in DT format as reference
                 *  @param  t1  : time in TM structure format
                 *  @return void : no return value
                 */
		void addTime(DT &dt,TM t1);

		/**
                 *  isLeapYear method: To check whether the year is leap year or normal year.
                 *  @param  dt   : date in DT format as reference
                 *  @return bool : 'true' if year is a leap year, else 'false'
                 */
		bool isLeapYear(DT dt);

		/** 
                 *  nextDay method  : Given a Date in crnDay, Return the next day in nxtDay .
                 *  @param  dt      : date in DT format as reference
                 *  @param  nxtDay  : reference of DT
                 *  @return void : no return value
                 */
		void nextDay(DT crntDay, DT &nxtDay);

		/**
                 *  addDates method : Given a Date in crnDay,It will add dates.
                 *  @param  dt      : date in DT format as reference
                 *  @param  nxtDay  : reference of DT
                 *  @return void    : no return value
                 */
		void addDates(DT &dt,DT dt1);

		/**
                 *  convHour method : to convert the hours.
                 *  @param  dt      : date in DT format as reference
                 *  @param  t1	    : periodicity in TM format
                 *  @return void    : no return value
                 */
		void convHour(DT &dt1,TM t1);

		/**
                 *  convMin method : to convert the minutes.
                 *  @param  dt      : date in DT format as reference
                 *  @param  t1      : periodicity in TM format
                 *  @return void    : no return value
                 */
		void convMin(DT &dt1,TM t1);

		/**
                 *  convSec method  : to convert the seconds.
                 *  @param  dt      : date in DT format as reference
                 *  @param  t1      : periodicity in TM format
                 *  @return void    : no return value
                 */
		void convSec(DT &dt1,TM t1);

	private:
};

#endif
