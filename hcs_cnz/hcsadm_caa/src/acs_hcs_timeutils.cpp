//======================================================================
//
// NAME
//      TimeUtils.cpp
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
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-09-07 by EEDSTL
// CHANGES
//     
//======================================================================

#include <string>

#include <ace/ACE.h>

#include "acs_hcs_timeutils.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Class TimeUtils
	//================================================================================

	TimeUtils::TimeUtils()
	{
	}

	TimeUtils::~TimeUtils()
	{
	}

	time_t TimeUtils::now()
	{
		return /*ACE_OS::*/time(0);
	}

	time_t TimeUtils::mkTime(const string& timeStr, bool isdst)
	{
		struct tm timeinfo;

		// Format of timeStr: YYYYmmssHHMMSS
		timeinfo.tm_year = ACE_OS::atoi(timeStr.substr(0, 4).c_str()) - 1900;
		timeinfo.tm_mon  = atoi(timeStr.substr(4, 2).c_str()) - 1;
		timeinfo.tm_mday = atoi(timeStr.substr(6, 2).c_str());
		timeinfo.tm_hour = atoi(timeStr.substr(8, 2).c_str());
		timeinfo.tm_min  = atoi(timeStr.substr(10, 2).c_str());
		timeinfo.tm_sec  = atoi(timeStr.substr(12, 2).c_str());

		timeinfo.tm_isdst = isdst;

		return ::mktime(&timeinfo);
	}

	time_t TimeUtils::mkTime(string timeInStr)
	{
		time_t timeIn = TimeUtils::mkTime(timeInStr, false);
		string timeOutStr = TimeUtils::timeToStr(timeIn);

		if (timeOutStr == timeInStr)
			return timeIn;

		return TimeUtils::mkTime(timeInStr, true);
	}

	string TimeUtils::timeToStr(time_t timeVal, const char* format)
	{
		struct tm lt;
		char      timeBuf[32];

		/*ACE_OS::*/localtime_r(&timeVal, &lt);
		/*ACE_OS::*/strftime(timeBuf, sizeof(timeBuf) - 1, format, &lt);

		return timeBuf;
	}
}
