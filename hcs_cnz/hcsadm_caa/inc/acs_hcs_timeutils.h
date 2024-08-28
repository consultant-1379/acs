//======================================================================
//
// NAME
//      TimeUtils.h
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      Implementation of Time Utility functions of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_TimeUtils_h
#define AcsHcs_TimeUtils_h

#include <string>

#include <ace/ACE.h>

namespace AcsHcs
{
	/**
	* TimeUtils - Time utility functions.
	*/
	class TimeUtils
	{
	public:
		/**
		* Constructor.
		*/
		TimeUtils();

		/**
		* Destructor.
		*/
		~TimeUtils();

		/*
		* Returns the time value for the current time (now).
		* @return The time value for the current time (now).
		*/
		static time_t now();

		/**
		* Converts the given time string to the appropriate time value.
		* Daylight saving time is considered.
		* @param[in] timeInStr The time string for which to get the time value. Format is YYYYmmssHHMMSS.
		* @return The time value corresponding to the given time string.
		*/
		static time_t mkTime(std::string timeInStr);

		/**
		* Returns the formatted string representation of timeVal.
		* @param[in] timeVal The time value to be converted to string.
		* @param[in] format  The format to be used for formatting.
		* @return The formatted string representation of timeVal.
		*/
		static std::string timeToStr(time_t timeVal, const char* format = "%Y%m%d%H%M%S");

	private:
		static time_t mkTime(const std::string& timeStr, bool isDst);
	};
}

#endif // AcsHcs_TimeUtils_h
