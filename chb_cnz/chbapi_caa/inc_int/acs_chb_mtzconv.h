/*=================================================================== */
/**
	@file		acs_chb_mtzconv.h

	@brief		Header file for MTZ  module.

				This module contains all the declarations useful to
				specify the class ACS_CHB_mtzconv.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       26/11/2010     XKUSATI	       Initial Release
==================================================================== */
//******************************************************************************
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_mtzconv_H
#define ACS_CHB_mtzconv_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_chb_systime_inter.h"
#include "acs_chb_file_handler.h"
#include "acs_chb_thread_handler.h"
#include <acs_chb_mtzclass.h>
#include <acs_chb_mtz_common.h>
#include <ACS_TRA_trace.h>
#include <time.h>
#include <iostream>
#include <string>
using namespace std;

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/**
 *	@brief			YearPeriod
 *					One Year are divided in five periods.
 *					This is described in the enum YearPeriod.
 *					DST equals to DayLightSavingsTime.
 *					STD equals to StanddardDate
 */
typedef enum 
{
		PeriodOne,		// New Year to first transition.
		OverLapp_1,		// first overlap period, one hour
		PeriodTwo,		// Time to next transition.
		OverLapp_2,		// Second overlap period, one hour
		PeriodThree   	// time to new year.
}YearPeriod ;


/**
 *	@brief			DayOfWeek
 */
typedef enum 
{
			sunday =	0,
			monday =	1,
			tuesday =	2,
			wednesday = 3,
			thursday =	4,
			friday =	5,
			saturday =	6
}DayOfWeek ;

/**
 *	@brief			DayOfMonth
 */
typedef enum 
{
			First =		1,
			Second =	2,
			Third =		3,
			Fourth = 	4,
			Last =		5
}DayOfMonth;



/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_mtzconv

                 Internal class for performing time conversions.
*/
/*=================================================================== */
class ACS_CHB_mtzconv : protected ACS_CHB_SysTimeInter
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/**
	 *	@brief			Class constructor
	 *
	 *	@param			mypfilehandler
	 *					File Handler object.
	 *
	 *	@return			none
	 */
	ACS_CHB_mtzconv(ACS_CHB_filehandler* mypfilehandler);

	/*===================================================================
	                        CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
	/**
	 *	@brief			Class destructor.
	 *
	 *	@return			none
	 */
	~ACS_CHB_mtzconv();

	/*====================================================================
	                        PUBLIC ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
	 *	@brief		CPtime
	 *				A storage place for the result from calling get_TMZTime.
	 */
	struct tm CPtime;

	/**
	 *	@brief		Get the local time from the system time. The system time is stored
	 *				in a global storage which is then read by the method CPTime.
	 *
	 *	@param		Ttime
	 * 				The system time which is then converted to a
	 * 				local time in time zone TMZ
	 *
	 *	@param		TMZ
	 *				The time zone to which the system time is
	 *
	 *	@return		struct tm*
	 *				0 Error detected.
	 *				tm* A pointer to a struct with the converted time.
	 */
	struct tm * get_TMZTime(struct tm* Ttime,
							int TMZ, int daylightBias);

	/**
	 *	@brief		Converts UTC to local time
	 *
	 *	@param		UTCtime
	 * 				UTC time
	 *
	 *	@param		LocalTime
	 * 				The local time
	 *
	 *	@param		TMZ
	 * 				0 Ok
	 *				-1 Error detected.
	 * @return
	 */
	int UTCtoLocal(const time_t UTCtime,struct tm & LocalTime,int TMZ);

	/**
	 *	@brief		Converts local time to UTC
	 *
	 *	@param		LocalTime
	 *				Pointer to a local time structure
	 *
	 *	@param		TMZ
	 *				The time zone (0-23)
	 *
	 *	@return		0 ok
	 *				-1 Error detected.
	 */
	time_t LocalToUTC(struct tm * LocalTime,int TMZ,int daylightBias);

	/**
	 *	@brief		CalculateTheOffset
	 *				The method calculates the offset time between the system time
	 *				and a time zone.
	 *
	 *	@param		TMZvalue
	 *				The time zone (value from 0 to 23) for which the
	 *				offset is calculated.
	 *
	 *	@param		DSS
	 *				DST status
	 *
	 *	@param		adjust
	 *				1 means forward, 0 means backward
	 *
	 *	@return		time_t
	 *				>=0 Number of seconds between system time and time zone.
	 *				-1 Error detected
	 */
	time_t CalculateTheOffset(int TMZvalue, int &DSS,int &adjust);

	/**
	 *	@brief		This method is used to obtain an approximation of the CP time.
	 *
	 *	@param		TimeOfCP
	 *				A reference struct which handle the CP time information.
	 *
	 *	@return		int
	 *				0 ok
	 *				-1 Error detected.
	 */
	int TimeFromCP(struct tm & TimeOfCP);

	/*===================================================================
	                        PRIVATE DECLARATION SECTION
	=================================================================== */
private:

	/**
	 *	@brief		This method returns a pointer to local time
	 *				in struct tm format.
	 *
	 *	@param		timeinfo
	 *
	 *	@return		void
	 */
	void getCurrentTime(struct tm* timeinfo);

	/**
	 *	@brief		This method
	 *
	 *	@param		AbsoluteDate
	 */
	void GetLocalTime(SYSTEMTIME *AbsoluteDate);

	/**
	 *	@brief		This method returns the weekday for
	 *				a particular date.
	 *
	 *	@param		day
	 *				Day of month.
	 *
	 *  @param		month
	 * 				Month of year.
	 *
	 *	@param		year
	 * 				Year.
	 *
	 * 	@return		int
	 */
	int calDayOfDate(int day,int month,int year);

	/**
	 *	@brief
	 *	@param AbsoluteDate
	 */
	void GetSystemTime(struct tm *AbsoluteDate);

	/**
	 *	@brief	Pointer to file handler object.
	 */
	ACS_CHB_filehandler   *pfilehandler;

	/**
	 *	@brief	Pointer to thread handler object.
	 */
	ACS_CHB_Threadhandler *pthreadhandler;

	/**
	 *	@brief
	 *
	 *	@param buffer
	 *
	 * 	@return
	 */
	DayOfWeek MatchWeekDay(char *buffer);

	/**
	 *	@brief
	 *	@param Date
	 * @return
	 */
	int  GetDayInMonth(SYSTEMTIME &Date);

	/**
	 *	@brief		Check in what period of the year we are in.
	 *
	 * 	@param		LocalTime
	 * 				Time of year in local time
	 *
	 * @param		TimeZone
	 *
	 * @return		enum YearPeriod
	 */
	YearPeriod DetermineYearPeriod( struct tm * LocalTime, 
									TIME_ZONE_INFORMATION &TimeZone
									);

	/**
	 *	@brief		This routine checks if the specified date
	 *				are within or without daylight savings time.
	 *
	 *	@param		TimeZoneInfo
	 *				Struct describing when daylight savings time
	 *
	 *	@param		sec_Time
	 *
	 *	@return		1 Standard date is in effect.
	 *				0 Daylight savings is in effect.
	 */
	bool check_if_dst(TIME_ZONE_INFORMATION TimeZoneInfo, time_t& sec_Time);

	/**
	 *	@brief		This routine converts from SYSTEMTIME to UTC format.
	 *
	 * 	@param		st
	 *				time in a SYSTEMTIME format.
	 *
	 *	@return		time_t
	 *				time in UTC format.
	 */
	time_t SYSTEMTIME_to_sec(SYSTEMTIME& st);

	/**
	 *	@brief		This routine converts from struct tm to SYSTEMTIME format.
	 *
	 *	@param		stTm
	 *				pointer to a struct tm with time in UTC.
	 *
	 *	@param		st
	 *				pointer to a SYSTEMTIME struct that will contain the result.
	 */
	void tm_to_SYSTEMTIME(struct tm  *stTm, SYSTEMTIME* st);

	/**
	 *	@brief		This routine converts from UTC to SYSTEMTIME format.
	 *
	 *	@param		sec
	 *				Time in UTC.
	 *
	 *	@param		st
	 *				pointer to a SYSTEMTIME struct that will contain the result.
	 */
	void sec_to_SYSTEMTIME(time_t sec, SYSTEMTIME* st);

	/**
	 *	@brief		This routine checks if the year starts with STD or DST.
	 *				A year are looks like this:
	 * 				New Year -------DST+++++++++++STD------- New Year
	 *				OR LIKE THIS
	 *				New Year +++++++STD-----------DST+++++++ New Year
	 *
	 * @param		TimeZoneInfo
	 *
	 * @return		bool
	 *				1	the time after new year is Standard date.
	 *				0	the time after new year is Daylight savings
	 *					time.
	 */
	bool CheckFirstTransition(TIME_ZONE_INFORMATION& TimeZoneInfo);

};

/*===================================================================
   ROUTINE: CheckFirstTransition
=================================================================== */
inline
bool ACS_CHB_mtzconv::CheckFirstTransition(TIME_ZONE_INFORMATION &TimeZoneInfo)
{
	//   New Year -------<DST>+++++++++++<STD>------- New Year
	//   return 1 
	//    OR LIKE THIS
	//   New Year +++++++<STD>-----------<DST>+++++++ New Year
	//   return 0

	return ( TimeZoneInfo.StandardDate.wMonth > TimeZoneInfo.DaylightDate.wMonth ) ? 1 : 0;  

}

#endif /*end ACS_CHB_mtzconv_H */


