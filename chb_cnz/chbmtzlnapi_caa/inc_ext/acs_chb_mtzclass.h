/*=================================================================== */
/**
	@file		acs_chb_mtzclass.h

	@brief		Header file for MTZ  module.

				This module contains all the declarations useful to
				specify the class ACS_CHB_mtzclass.

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
#ifndef ACS_CHB_mtzclass_H
#define ACS_CHB_mtzclass_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <time.h>
using namespace std;

/*=====================================================================
					FORWARD DECLARATION SECTION
==================================================================== */
class ACS_CHB_mtzconv;
class ACS_CHB_filehandler;
class ACS_CHB_Threadhandler;

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/**
 *	@brief	ACS_CHB_CONNECTION_ERROR
 */
const int ACS_CHB_CONNECTION_ERROR     = 2;

/**
 *	@brief	ACS_CHB_TIME_OK
 */
const int ACS_CHB_TIME_OK              = 0;

/**
 *	@brief	ACS_CHB_TIME_ERROR
 */
const int ACS_CHB_TIME_ERROR           = -1;

/**
 *	@brief	ACS_CHB_CPTIME_NOT_AVAILABLE
 */
const int ACS_CHB_CPTIME_NOT_AVAILABLE = -2;

/**
 *	@brief	ACS_CHB_CEASED
 */
const int ACS_CHB_CEASED               = 0;

/**
 * 	@brief	ACS_CHB_ISSUED
 */
const int ACS_CHB_ISSUED               = 1;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief	ACS_CHB_mtzclass

				The ACS_CHB_mtzclass is the interface for time conversion.
				An instance of this class should be created  by every program that
				wants to perform time zone calculation.

				The class provides the following public methods:

				-TimeZoneAlarm			Returns the alarm status of specified time zone.
				-UTCtoLocalTime			Converts UTC-time to local time of specified time zone.
				-LocalTimeToUTC			Converts local time to UTC-time.
				-CalculateTheOffsetTime	Calculates the offset between system time & local time.
				-CPTime					Provides the CP time.
*/
/*=================================================================== */
class  ACS_CHB_mtzclass
{

	friend class ACS_CHB_Thread_JTPcomm;
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
	public:

	/**
		@brief pfilehandler
	*/

	static ACS_CHB_filehandler *pfilehandler;


	/**
		@brief	pthreadhandler
	*/
	static ACS_CHB_Threadhandler *pthreadhandler;

	/**
		@brief  MTZInit

			This method invkoes the constructor of ACS_CHB_filehandler and
			ACS_CHB_Threadhandler. ACS_CHB_Threadhandler starts a thread, which 
			monitors the ACS_TZ_links.cfg file.
	*/
	static bool MTZInit();


	/**
		@brief	MTZDestroy

			This method deallocates the memory allocated for pfilehandler 
			and pthreadHandler.
	*/
	static void MTZDestroy();

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

		@brief			Class constructor.
					The constructor of the class. Creates an internal object.
	*/
	/*=================================================================== */
	ACS_CHB_mtzclass();

	/*=====================================================================
	                        CLASS DESTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

		@brief			Class destructor.
						The destructor of the class. Destroys the internal object.
	*/
	/*=================================================================== */

	virtual ~ACS_CHB_mtzclass();

	/**
	 *	@brief		The method is used to obtain information about possible CP-AP
	 *				mismatch for a specified time zone.
	 *
	 *	@param		TMZValue
	 *
	 *	@return		int
	 *				ACS_CHB_ISSUED, which indicates if an alarm is issued
	 *				ACS_CHB_CEASED, which indicates if an alarm is ceased.
	 *				TMZValue, The time zone for which the verification has to be done.
	 * 				ACS_CHB_CONNECTION_ERROR
	 */
	int TimeZoneAlarm(const int TMZValue);

	/**
	 *	@brief		The method converts UTC-time into local time of specified time zone.
	 *
	 * 	@param 		UTCtime
	 *				The UTC-time in seconds since 1970-01-01.
	 *
	 * 	@param		LocalTime
	 * 				A reference struct which handle the converted time.
	 *
	 * 	@param		TMZ
	 * 				The time zone (value from 0 to 23) to which the conversion is done.
	 *
	 * 	@return		int
	 * 				ACS_CHB_TIME_OK if the conversion succeed
	 * 				ACS_CHB_TIME_ERROR if error is detected
	 */
	int UTCtoLocalTime(time_t UTCtime,struct tm  & LocalTime,int TMZ);


	/**
	 *	@brief		The method converts local time of specified time zone into  UTC-time.
	 *
	 *	@param 		LocalTime
	 *				The time to be converted.
	 *
	 * 	@param		TMZ
	 *				The time zone (value from 0 to 23) from which the conversion is done.
	 *
	 *	@return		time_t
	 *				The UTC-time in seconds since 1970-01-01, else ACS_CHB_TIME_ERROR.
	 */
	time_t LocalTimeToUTC(struct tm * LocalTime,int TMZ);

	/**
	 *	@brief		The method calculates the offset time between the system time
	 *				and a time zone.
	 *
	 *	@param		TMZValue
	 * 				A reference parameter used to obtain information
	 * 				of the daylight saving time status of the time zone.

	 *	@param		DSS
	 * 				DSS > 0 : DST is in effect,
	 * 				DSS = 0 : DST is not in effect
	 * 				DSS < 0 : DST information is not available.
	 *
	 *	@param		adjust
	 *				A reference parameter which indicates how the
	 *				adjustment of the offset has to be done.
	 *				(1 means forward, 0 means backward)
	 *
	 *	@return		time_t
	 *				The offset as the amount of seconds between the system time and
	 *				the time zone, else ACS_CHB_TIME_ERROR.
	 */
	time_t CalculateTheOffsetTime(int TMZValue, int &DSS,int &adjust);

	/**
	 *	@brief		The method is used to obtain an approximation of the CP time.
	 *
	 * 	@param		TimeOfCP
	 *				A reference struct which handle the CP time information.
	 *
	 *	@return		int
	 *				ACS_CHB_TIME_OK if the method terminates successfully.
	 *				ACS_CHB_CPTIME_NOT_AVAILABLE if error is detected.
	 */
	int CPTime(struct tm & TimeOfCP);

	/*===================================================================
	                        PRIVATE DECLARATION SECTION
	=================================================================== */

private:
	/*===================================================================
	                        PRIVATE ATTRIBUTE
	=================================================================== */
	/**
	 *	@brief	CPtime.
	 */
	time_t CP_time;

	/**
	 *	@brief	Pointer to internal object
	 */
	ACS_CHB_mtzconv* NTInterface;

	/*===================================================================
	                        PRIVATE METHOD
	=================================================================== */

	/**
	 *	@brief		The method is used to convert the system time into local time
	 *				of specified time zone.
	 *
	 *	@param		timeToConvert
	 * 				A struct containing information about the time to convert. (The system time).
	 *
	 *	@param		daylightStatus
	 * 				Indicates if the daylight saving time is on (>0) or off (=0).
	 *
	 *	@param		TMZ
	 * 				The time zone (value from 0 to 23) to which the conversion is done.
	 *
	 *	@return		struct tm*.
	 *				0	An error occurred
	 *				tm*	A pointer to a struct with the converted time.
	 */
	struct tm* makeSystemTimeConversion(
							struct tm * timeToConvert,
							int daylightStatus, int TMZ
							);
};

#endif /* end ACS_CHB_mtzclass_H*/
