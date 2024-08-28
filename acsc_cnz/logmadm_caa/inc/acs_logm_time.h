/*=================================================================== */
/**
@file		acs_logm_time.h

@brief		Header file of Time related Functionality

This module contains all the declarations useful to specify the class.

@version 	1.0.0

 **/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       20/08/2012     XCSSATA       Initial Release
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_LOGM_TIME_H_
#define ACS_LOGM_TIME_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */

#include<iostream>
#include<string>

using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include<unistd.h>
#include<time.h>

#include<acs_logm_definitions.h>
#include<acs_logm_tra.h>
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_time

@brief		This class represents all the functionalities related to Time
 **/
/*=================================================================== */
class acs_logm_time
{
protected:
	/**
	 * @brief	The Raw time. The number of seconds since EPOCH .

	 */

	time_t theTime;
	/**
	 * @brief	The time in seconds,minutes,hours,days,year

	 */
	tm* theTimeTM;
	/**
	 * @brief	Seconds in a day.

	 */
	static const unsigned int SECONDS_IN_DAY = 86400;
	/**
	 * @brief	Seconds in a Hour.

	 */
	static const unsigned int SECONDS_IN_HOUR = 3600;
	/**
	 * @brief	Seconds in a Minute.

	 */
	static const unsigned int SECONDS_IN_MINUTE = 60;


public:
	/**
	 * @brief	Constructor.

	 */
	acs_logm_time();
	/**
	 * @brief	Destructor.

	 */
	~acs_logm_time();
	/**
	 * @brief	This method is overloaded less than operator for time objects.

	 */
	bool 	operator < (const acs_logm_time& aTime);
	/**
	 * @brief	This method is overloaded greater than operator for time objects.
	 */
	bool 	operator > (const acs_logm_time& aTime);
	/**
	 * @brief	This method used to determine is the time object is elapsed.

	 * @return	true - If time is elapased. false - If time is not elapased.
	 */
	bool 	isTimeElapsed ();
	/**
	 * @brief	This method used to determine is the time object is in future.

	 * @return	true - If time is future. false - If time is not in future.
	 */
	bool 	isTimeInFuture();
	/**
	 * @brief	This method used to determine is the time object is elapsed w.r.t given time.
	 * @param   aTime - Given time
	 * @return	elapsed number of seconds.
	 */
	double 	elapsedTime(const acs_logm_time& aTime);
	/**
	 * @brief	This method used to calculate the number of seconds elapsed w.r.t current time.

	 * @return	elapsed number of seconds.
	 */
	double 	elapsedTimeWithCurrentTime();
	/**
	 * @brief	This method used to add given seconds.
	 * @param   aSeconds - Number of seconds
	 */
	void    addSeconds(int aSeconds);
	/**
	 * @brief	This method used to add given Minutes.
	 * @param   aSeconds - Number of Minutes
	 */
	void    addMinutes(int aMinutes);
	/**
	 * @brief	This method used to add given Hours.
	 * @param   aSeconds - Number of Hours
	 */
	void    addHours(int aHours);
	/**
	 * @brief	This method used to add given Days.
	 * @param   aSeconds - Number of Days
	 */
	void    addDays(int aDays);
	/**
	 * @brief	This method used to set the time
	 * @param   aHours - Hour data
	 * @param   aMinutes - Minutes data
	 * @param   aSeconds - seconds data
	 */
	void    setTime(int aHours,int aMinutes,int aSeconds);
	/**
	 * @brief	This method used to calculate the diffrence time w.r.t given time.
	 * @param   aTime - Given time
	 * @return	value in seconds
	 */
	double  diffTime (const acs_logm_time& aTime);
	/**
	 * @brief	This method used to log the time data

	 */
	void    logTimeData();

};

#endif /* ACS_LOGM_TIME_H_ */
