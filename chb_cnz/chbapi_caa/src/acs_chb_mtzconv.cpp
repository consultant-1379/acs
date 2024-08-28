/*=================================================================== */
   /**
   @file acs_chb_mtzconv.cpp

   This module contains the implementation of class declared in
   the ACS_CHB_mtzconv.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/11/2010     XKUSATI   Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_chb_mtzconv.h>
#include <time.h>
static const char* tempDebug="ACS_CHB_MTZ_DebugTrace";
static const char* tempError="ACS_CHB_MTZ_ErrorTrace";
static const char* C512="C512";
static ACS_TRA_trace ACS_CHB_MTZ_DebugTrace(tempDebug, C512);
static ACS_TRA_trace ACS_CHB_MTZ_ErrorTrace(tempError, C512);

/*===================================================================
   ROUTINE: ACS_CHB_mtzconv
=================================================================== */
ACS_CHB_mtzconv::ACS_CHB_mtzconv(ACS_CHB_filehandler * mypfilehandler)
{
	memset(&CPtime, 0,sizeof(CPtime)); 


	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
        {
             char traceBuffer[512];
             memset(&traceBuffer, 0, sizeof(traceBuffer));
             sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv Constructor");
             ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	pfilehandler = mypfilehandler;
	pthreadhandler=0;
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
        {
             char traceBuffer[512];
             memset(&traceBuffer, 0, sizeof(traceBuffer));
             sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv Constructor");
             ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

/*===================================================================
   ROUTINE: ~ACS_CHB_mtzconv
=================================================================== */
ACS_CHB_mtzconv::~ACS_CHB_mtzconv()
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv Destructor");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv Destructor");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

}

/*===================================================================
   ROUTINE: GetDayInMonth
=================================================================== */
int ACS_CHB_mtzconv::GetDayInMonth(SYSTEMTIME &Date)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::GetDayInMonth()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	// This method extracts the day of the month
	// in absolute date, from day-in-month format.

	SYSTEMTIME AbsoluteDate;

	GetLocalTime(  &AbsoluteDate );

	// Convert to first of this month.

	AbsoluteDate.wMonth = Date.wMonth - 1;
	AbsoluteDate.wYear = 1900 - Date.wYear;
	AbsoluteDate.wDay = 1;

	int FirstDayInMonth = calDayOfDate(AbsoluteDate.wDay, AbsoluteDate.wMonth, AbsoluteDate.wYear);

	int ReturnDate;
	// Calculate first occurence of day in month.
	// First calculate what day the first in this month is.
	if( FirstDayInMonth > Date.wDayOfWeek )
	{
		// Rollover to next week.
		ReturnDate = Date.wDayOfWeek + (saturday - FirstDayInMonth + 2);
	}
	else
	{
		// This week.
		ReturnDate = Date.wDayOfWeek - FirstDayInMonth + 1;
	}

	// Calculate the exact date, working from FirstDayInMonth.

	switch (Date.wDay)
	{
		case First:
		case Second:
		case Third:
		case Fourth:
			ReturnDate = ReturnDate + 7*(Date.wDay-1);
			break;
		case Last:
			// Test if there is a fifth occurence of this day.
			AbsoluteDate.wDay = ReturnDate + 7*4;

			//satish: here its checking if the date is valid

			int test = 0;
			/*	
			if(0 != test )
			{
				ReturnDate = ReturnDate + 7*4;
			}
			*/
			if(test == 0)	
			{
				// Else return the fourth day.
				ReturnDate = ReturnDate + 7*3;
			}
			break;
	}
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "Leaving ACS_CHB_mtzconv::GetDayInMonth() with ReturnDate : %d", ReturnDate);
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	return ReturnDate;

}

/*===================================================================
   ROUTINE: check_if_dst
=================================================================== */
bool ACS_CHB_mtzconv::check_if_dst(TIME_ZONE_INFORMATION TimeZoneInfo, time_t& sec_Time)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::check_if_dst()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}


	// This method assumes that sec_Time are specified in UTC.
	// Also are parameter TimeZoneInfo passed on the stack to avoid
	// side effects of the function.
	SYSTEMTIME st;

	// Check if dst not is specified.
	if(TimeZoneInfo.StandardDate.wMonth == 0)
	{
		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Day Light saving time is not in effect");
			ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::check_if_dst()");
		     ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return 0;
	}
	// First convert to tm struct format to get
	// the year.
	sec_to_SYSTEMTIME(sec_Time,&st);

	// check if we have day-in-month format.
	// And must convert to absolute format.
	if(TimeZoneInfo.StandardDate.wYear == 0)
	{
		// Add correct year.
		TimeZoneInfo.StandardDate.wYear = st.wYear;
		TimeZoneInfo.DaylightDate.wYear = st.wYear;
		TimeZoneInfo.StandardDate.wDay = GetDayInMonth(TimeZoneInfo.StandardDate);
		TimeZoneInfo.DaylightDate.wDay = GetDayInMonth(TimeZoneInfo.DaylightDate);
	}

	// get the number of seconds since new year.
	TimeZoneInfo.DaylightDate.wYear= st.wYear;
	TimeZoneInfo.StandardDate.wYear= st.wYear;

	// check if we are in dst period or in std period of year.
	// A year are looks like this :-)
	// Type 1.
	// New Year ------------------<DST>+++++++++++<STD>--------------- New Year
	// OR LIKE THIS
	// Type 0.
	//   New Year +++++++<STD>-----------<DST>+++++++ New Year
	// Check type of year in this timezone.
	bool TypeOfYear = CheckFirstTransition(TimeZoneInfo);
	if( TypeOfYear )
	{
		// Check if we have passed <DST>.
		// DaylightDate specified in local time STD.
		// convert to UTC and compare.
		time_t temp_1 = SYSTEMTIME_to_sec(TimeZoneInfo.DaylightDate);

		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "Seconds of the DST according to TimeZoneInfo = %lu",temp_1);
			ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		if(sec_Time >= temp_1)
		{ // Then check if we have passed <STD>.
			// sec_time is UTC time.
			// The StandardDate is specified UTC.
			// To follow the clib definition of isdst handling, remove DayLightBias
			// before check !
			time_t temp_time;
			temp_time = SYSTEMTIME_to_sec(TimeZoneInfo.StandardDate);
			temp_time = temp_time + TimeZoneInfo.DaylightBias*60;

			// Then perform check.
			if (sec_Time  < temp_time)
			  {
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Day Light saving time is in effect");
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::check_if_dst()");
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

			  	return 1; // Set to 1 to indicate that DayLightSaving time is in effect
			  }
		}
		// No, sec_Time is STD and not DST.
		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Day Light saving time is not in effect");
			ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::check_if_dst()");
			ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		return 0; // Set to 0 to indicate that standard time is in effect
	}
	else
	{
		// Type 0.
		//   New Year +++++++<STD>-----------<DST>+++++++ New Year
		// Check if we have passed <STD>.
		time_t temp_std = SYSTEMTIME_to_sec(TimeZoneInfo.StandardDate);
		temp_std = temp_std + + TimeZoneInfo.DaylightBias*60;
		if(sec_Time >= temp_std)
		{ // Then check if we have passed <DST>.
			// sec_time is DayLightSaving time.
			// The DaylightDate is specified for StandardTime time.
			// Define help variable and check if we have passed DST.
			// So first convert to UTC.
			time_t temp_time;
			temp_time = SYSTEMTIME_to_sec(TimeZoneInfo.DaylightDate);

			// Then perform check.
			if (sec_Time  <= temp_time)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Day Light saving time is not in effect");
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);

					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::check_if_dst()");
                                        ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

                                return 0; // Set to 0 to indicate that standard time is in effect
			}
		}
		// No, sec_Time is DST and not STD.
		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Day Light saving time is in effect");
			ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::check_if_dst()");
			ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		return 1; // Set to 1 to indicate that DayLightSaving time is in effect
	}

}

/*===================================================================
   ROUTINE: TimeFromCP
=================================================================== */
int ACS_CHB_mtzconv::TimeFromCP(struct tm& TimeOfCP)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::TimeFromCP()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	// Returns a approximation of the CP time based on the latest
	// received HeartBeat.
	// Get the CPTime data.
	time_t	written, CPtimeSec;
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Calling ReadZoneFileCPtime()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	if ( pfilehandler->ReadZoneFileCPtime(CPtimeSec, written) )
	{	// calculate trueCPtime. That is adjust for the time
		// that has passed since CPtime was written.
		// First calculate time that has elapsed since this information was
		// written by the CP. NOTE here we work with UTC time.
		time_t currentTime = time(NULL);
		time_t diffTime    = difftime(currentTime, written);
		time_t	TrueCPtime = CPtimeSec + diffTime;
		// Convert to local time TMZ=0, Always true for CP.
		if( UTCtoLocal(TrueCPtime, TimeOfCP, 0) == 0 )
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::TimeFromCP()");
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

                        return 0;
		}
		else
		{
			if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
			{
			     char traceBuffer[512];
			     memset(&traceBuffer, 0, sizeof(traceBuffer));
			     sprintf(traceBuffer, "%s", "Conversion from UTCtoLocal is failed");
			     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
			{
			     char traceBuffer[512];
			     memset(&traceBuffer, 0, sizeof(traceBuffer));
			     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::TimeFromCP()");
			     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			return -1;
		}
	}
	else
	{	// cant read the ZoneFile.
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "ReadZoneFileCPtime is failed");
			ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::TimeFromCP()");
			ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return -1;
	}
} // End of TimeFromCP

/*===================================================================
   ROUTINE: calDayOfDate
=================================================================== */
int  ACS_CHB_mtzconv::calDayOfDate(int date,int month,int year)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::calDayOfDate()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        int returnDayofWeek;
        struct tm timeinfo;
        getCurrentTime(&timeinfo);
        timeinfo.tm_mday = date;
        timeinfo.tm_mon = month;
        timeinfo.tm_year = year;

        mktime(&timeinfo);

        returnDayofWeek = timeinfo.tm_wday;
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "Leaving ACS_CHB_mtzconv::calDayOfDate() with returnDayofWeek : %d", returnDayofWeek);
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        return returnDayofWeek;
}

/*===================================================================
   ROUTINE: sec_to_SYSTEMTIME
=================================================================== */
void ACS_CHB_mtzconv::sec_to_SYSTEMTIME(time_t sec, SYSTEMTIME *st)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::sec_to_SYSTEMTIME()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        struct tm* timeInfo;
        timeInfo = gmtime(&sec);
        st->wYear               =       timeInfo->tm_year + 1900;
        st->wMonth              =       timeInfo->tm_mon + 1;
        st->wDayOfWeek  		=       timeInfo->tm_wday;
        st->wDay                =       timeInfo->tm_mday;
        st->wHour               =       timeInfo->tm_hour;
        st->wMinute     		=       timeInfo->tm_min;
        st->wSecond    			=       timeInfo->tm_sec;

        if (timeInfo->tm_year < 70)
        {
        		st->wYear = st->wYear + 100;
        }
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::sec_to_SYSTEMTIME()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

}

/*===================================================================
   ROUTINE: CalculateTheOffset
=================================================================== */
time_t ACS_CHB_mtzconv::CalculateTheOffset(int TMZvalue, int &DSS,int &adjust)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::CalculateTheOffset()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        // Calculate the offset between TMZ timezone and systemtime.
        struct tm tz0, tzX;             // Local time
        time_t tz0_sec, tzX_sec;        // Local time in seconds
        time_t offset;

        //satish:I have changed the SYSTEMTIME structure to tm * for calculating the time in sec
        // Get the UTC time in seconds.
        //Logic for getting system time

        time_t rawtime(NULL);
        //struct tm *timeinfo;
        time(&rawtime);

        time_t CurrUTC = rawtime;
        // Get Current Local Time in TZ0 and TZX
        if (UTCtoLocal(CurrUTC, tz0, 0) < 0)
        {
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Conversion from UTCtoLocal failed");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::TimeFromCP()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
        	return -1;
        }

        if (UTCtoLocal(CurrUTC, tzX, TMZvalue) < 0)
        {
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Conversion from UTCtoLocal failed");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::TimeFromCP()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
        	return -1;
        }

        // Set DSS and return the offset.
        DSS = tzX.tm_isdst;
        //satish:I have changed the SYSTEMTIME structure to tm * for calculating the time in sec
        // Convert Local Time to (local)Seconds
        //SYSTEMTIME *tmp;
        //tm_to_SYSTEMTIME(&tz0, &tmp);
        //tz0_sec = SYSTEMTIME_to_sec(tmp);

        tz0_sec = mktime(&tz0);

        //tm_to_SYSTEMTIME(&tzX, &tmp);
        //tzX_sec = SYSTEMTIME_to_sec(tmp);

        tzX_sec = mktime(&tzX);

        // Compare and calculate the offset
        if (tzX_sec > tz0_sec)
        {
                adjust = 1;
                offset = tzX_sec - tz0_sec;
        }
        else
        {
                adjust = 0;
                offset = tz0_sec - tzX_sec;
        }

	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s %d", ACE_TEXT("Leaving ACS_CHB_mtzconv::CalculateTheOffset() with Offset :"), (int)offset);
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        return offset;
} // End of CalculateTheOffset

/*===================================================================
   ROUTINE: SYSTEMTIME_to_sec
=================================================================== */
time_t  ACS_CHB_mtzconv::SYSTEMTIME_to_sec(SYSTEMTIME& st)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::SYSTEMTIME_to_sec()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	//TR_HT72009 Changes Begin
	int leapyear=0,year=0,days=0;
	time_t sec(NULL),newsec(NULL),leapsec(NULL);
	sec = 31536000;
	leapsec = 86400;
	struct tm timeInfo;
	ACE_OS::memset(&timeInfo,0, sizeof(timeInfo));

	timeInfo.tm_year       =       st.wYear;
	timeInfo.tm_mon        =       st.wMonth;
	timeInfo.tm_wday       =       st.wDayOfWeek;
	timeInfo.tm_mday       =       st.wDay;
	timeInfo.tm_hour       =       st.wHour;
	timeInfo.tm_min        =       st.wMinute;
	timeInfo.tm_sec        =       st.wSecond;
	leapyear = ((timeInfo.tm_year-1968)/4);
	year = timeInfo.tm_year-1970;
	if (!(timeInfo.tm_year%4) && timeInfo.tm_mon <= 2)
		leapyear = leapyear - 1;
	switch (timeInfo.tm_mon-1)
	{
		case 1:
			days = 31;
			break;
		case 2:
			days = 59;
			break;
		case 3:
			days = 90;
			break;
		case 4:
			days = 120;
			break;
		case 5:
			days = 151;
			break;
		case 6:
			days = 181;
			break;
		case 7:
			days = 212;
			break;
		case 8:
			days = 243;
			break;
		case 9:
			days = 273;
			break;
		case 10:
			days = 304;
			break;
		case 11:
			days = 334;
			break;
	}
	newsec = (sec * year) + (leapsec * leapyear) + ((((((days +(timeInfo.tm_mday - 1)) * 24) + timeInfo.tm_hour) * 60) + timeInfo.tm_min) * 60) + timeInfo.tm_sec;

	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s %lu", "Leaving ACS_CHB_mtzconv::SYSTEMTIME_to_sec() with sec ",newsec);
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return newsec;
	//TR_HT72009 Changes End
}

/*===================================================================
   ROUTINE: get_TMZTime
=================================================================== */
struct tm * ACS_CHB_mtzconv::get_TMZTime(struct tm * Ttime, int TMZ, int daylightBias)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::get_TMZTime()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        // Store the CPtime in UTC and the write time stamp (=current SystemTime)
        // Get timestamp
        time_t writtenSec = time(NULL);
        // CP time always TMZ = 0.
        // Force isdst to -1, i.e. mimic APG30 behaviour
        Ttime->tm_isdst = -1;
	time_t CPtimeSec;
	ACE_OS::memset(&CPtimeSec, 0, sizeof(CPtimeSec));
        CPtimeSec = LocalToUTC( Ttime, 0, daylightBias);


        if( CPtimeSec != -1 )
        {
		// Write to file.
                pfilehandler->WriteZoneFileCPtime(CPtimeSec, writtenSec);
                // Convert System time to local time using timezone TMZ
                //
		ACE_OS::memset( &CPtime, 0, sizeof(CPtime));

                int result = UTCtoLocal(CPtimeSec,CPtime, TMZ);


                if (result < 0)
                {
			if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
			{
			     char traceBuffer[512];
			     memset(&traceBuffer, 0, sizeof(traceBuffer));
			     sprintf(traceBuffer, "%s", "Conversion from UTCtoLocal failed");
			     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
			{
			     char traceBuffer[512];
			     memset(&traceBuffer, 0, sizeof(traceBuffer));
			     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::get_TMZTime()");
			     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			return 0;
                }
                else
                {
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::get_TMZTime()");
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

                        return &CPtime;
                }
        }
        else
        {
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Conversion from LocalToUTC failed");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::get_TMZTime()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
                return 0;
        }
}

/*===================================================================
   ROUTINE: UTCtoLocal
=================================================================== */
// Transforms UTCtime to struct tm LocalTime format.
int ACS_CHB_mtzconv::UTCtoLocal(const time_t UTCtime,
                                struct tm & LocalTime,
                                int TMZ)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[513];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::UTCtoLocal()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
     

        TIME_ZONE_INFORMATION TimeZone; // time zone
        ACE_OS::memset(&TimeZone, 0, sizeof(TimeZone));

        time_t sec_localtime;
        bool   isdst = false;           // Assume not daylight saving time.

        if( TMZ < 0 || TMZ >= 24 )
        {
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "Invalid TMZValue : %d ", TMZ);
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::UTCtoLocal()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
                return -1;
        }

        // Fetch conversion information from class ACS_CHB_filehandler .

        if( !pfilehandler->GetNTZone( TMZ, TimeZone))
        { // No such timezone exist.
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "TimeZone with TMZValue : %d is not existing", TMZ);
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::UTCtoLocal()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
                return -1;
        }

        // Calculate Local time.
        // withdraw bias
        if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "TimeZone.Bias : %d", TimeZone.Bias);
                ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
         
        sec_localtime = UTCtime - (TimeZone.Bias * 60);
        if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "sec_localtime after addition of bias: %lu", sec_localtime);
                ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
         

        // Check if UTC local time is within limits for time_t
        if (sec_localtime < 0 || sec_localtime > 2147483647)
        {
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Local Time in seconds is not in the limits [0 - 2147483647");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::UTCtoLocal()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
                return -1;
        }

        // check daylight saving time
        if(check_if_dst(TimeZone, sec_localtime))
        {
          if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
          {
                  char traceBuffer[512];
                  memset(&traceBuffer, 0, sizeof(traceBuffer));
                  sprintf(traceBuffer, "DaylightBias : %d", TimeZone.DaylightBias);
                  ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
          }
                sec_localtime = sec_localtime - (TimeZone.DaylightBias * 60);
                isdst = true;
        }
        else
        { // If not daylight saving time, then we have standarddate.
          if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
          {
                  char traceBuffer[512];
                  memset(&traceBuffer, 0, sizeof(traceBuffer));
                  sprintf(traceBuffer, "StandardBias : %d", TimeZone.StandardBias);
                  ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
          }
                sec_localtime = sec_localtime - (TimeZone.StandardBias*60);
       }
 // Check if UTC local time is within limits for time_t
        if (sec_localtime < 0 || sec_localtime > 2147483647)
        {
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Local Time in seconds is not in the limits [0 - 2147483647");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::UTCtoLocal()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
                return -1;
        }

        if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "sec_localtime : %lu", sec_localtime);
                ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

        struct tm* pTime;
        ACE_OS::memset(&pTime, 0, sizeof(pTime));
         if ( TMZ == 0)
        	 pTime= localtime(&sec_localtime);
         else
        	 pTime= gmtime(&sec_localtime);
        ACE_OS::memset(&LocalTime, 0, sizeof(LocalTime));
        memcpy(&LocalTime, pTime, sizeof(tm));
        // setup daylight saving time
        LocalTime.tm_isdst = (isdst == true) ? 1 : 0;

        if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::UTCtoLocal()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        return 0;
} // End of UTCtoLocal

/*===================================================================
   ROUTINE: LocalToUTC
=================================================================== */
time_t ACS_CHB_mtzconv::LocalToUTC(struct tm * LocalTime,int TMZ,int daylightBias)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::LocalToUTC()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	TIME_ZONE_INFORMATION TimeZone; // time zone
	ACE_OS::memset(&TimeZone, 0, sizeof(TimeZone));
	time_t sec_localtime, sec_UTC;

	if( TMZ < 0 || TMZ >= 24 )
	{
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "Invalid TMZValue : %d ", TMZ);
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::LocalToUTC()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return -1;
		// Error
	}

	// Check that LocalTime not NULL
	if( LocalTime == NULL )
	{
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "LocalTime is NULL");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::LocalToUTC()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return -1;
	}

	// Fetch convertion information from class ACS_CHB_filehandler .

	if( !pfilehandler->GetNTZone( TMZ, TimeZone) )
	{ // No such timezone exist.
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "TimeZone with TMZValue : %d is not existing", TMZ);
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_ErrorTrace.ACS_TRA_ON())
		{
		     char traceBuffer[512];
		     memset(&traceBuffer, 0, sizeof(traceBuffer));
		     sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::LocalToUTC()");
		     ACS_CHB_MTZ_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return -1;

	}

	//TR_HT72009 Changes Begin

	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[1024];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "Bias : %d :: DaylightBias : %d :: wDay : %d :: wMonth : %d :: wYear : %d :: wHour : %d :: wMinute : %d :: wSecond : %d :: DaylightName : %s ",TimeZone.Bias,TimeZone.DaylightBias,TimeZone.DaylightDate.wDay,TimeZone.DaylightDate.wMonth,TimeZone.DaylightDate.wYear,TimeZone.DaylightDate.wHour,TimeZone.DaylightDate.wMinute,TimeZone.DaylightDate.wSecond,TimeZone.DaylightName);
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[1024];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "StandardBias : %d :: wDay : %d :: wMonth : %d :: wYear : %d :: wHour : %d :: wMinute : %d :: wSecond : %d :: StandardName : %s ",TimeZone.StandardBias,TimeZone.StandardDate.wDay,TimeZone.StandardDate.wMonth,TimeZone.StandardDate.wYear,TimeZone.StandardDate.wHour,TimeZone.StandardDate.wMinute,TimeZone.StandardDate.wSecond,TimeZone.StandardName);
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	int dst = LocalTime->tm_isdst;
	// Convert LocalTime to GMT/UTC, with no DST or Bias.
	SYSTEMTIME st;
	tm_to_SYSTEMTIME(LocalTime, &st);

	ACE_OS::memset(&sec_localtime, 0, sizeof(sec_localtime));
	sec_localtime = SYSTEMTIME_to_sec(st);
	//sec_localtime = mktime(LocalTime);
	//TR_HT72009 Changes End
    if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
    {
    	char traceBuffer[512];
    	memset(&traceBuffer, 0, sizeof(traceBuffer));
    	sprintf(traceBuffer, "sec_localtime = %lu", sec_localtime);
    	ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
    }
    if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
    {
    	char traceBuffer[512];
    	memset(&traceBuffer, 0, sizeof(traceBuffer));
    	sprintf(traceBuffer, "TimeZone.Bias = %d", TimeZone.Bias);
    	ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
    }

	// Add Bias
	ACE_OS::memset(&sec_UTC, 0, sizeof(sec_UTC));
    	sec_UTC = sec_localtime + (TimeZone.Bias * SECSPERMIN);
       
	// Check if we have DST/STD defined in this timezone.
	if( TimeZone.StandardDate.wMonth == 0 )
	{
		if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::LocalToUTC()");
			ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		return sec_UTC;
	}

	// check in what period of the year we are in.
	YearPeriod YearStatus;
	YearStatus = DetermineYearPeriod( LocalTime, TimeZone );
	bool TypeOfYear = CheckFirstTransition(TimeZone);
	// check daylight saving time
	switch (YearStatus)
	{
	// STD
	case PeriodOne:         // New Year to DST
	case PeriodThree :          // STD period two.
		if( TypeOfYear )
		{
			if( dst <0 || dst == 0 )
			{
				LocalTime->tm_isdst = 0;
				sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
			}
			else
			{
				sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
				LocalTime->tm_isdst = 0;
			}

		}
		else
		{

			if( dst <0 || dst == 1 )
			{
				LocalTime->tm_isdst = 1;
				sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
			}
			else
			{
				sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
				LocalTime->tm_isdst = 0;
			}
		}
		break;
		// daylight saving time
	case PeriodTwo :                // DST to STD

		if( TypeOfYear )
		{

			if( dst < 0 || dst == 1 )
			{
				sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
				LocalTime->tm_isdst = 1;
			}
			else
			{
				LocalTime->tm_isdst = 1;
				sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
			}

		}
		else
		{

			if( dst < 0 || dst == 0 )
			{
				LocalTime->tm_isdst = 0;
				sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
			}
			else
			{
				sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
				LocalTime->tm_isdst = 0;
			}

		}
		break;
		// Special overlapp case.
	case OverLapp_1 :               // STD to DST overlapp, one hour
		// ANSI states that :
		// Use isdst if non-negative, otherwise use STD.
		if( TMZ > 0 )
			dst = daylightBias;

		if( TypeOfYear )
		{
			if( dst < 0 )
			{
				// Special case here use STD
				LocalTime->tm_isdst = 0;
				sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
			}
			else if( dst == 0 )
			{
				sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
			}
			else
			{ // Use DST because it's specified.
				sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
			}
		}
		else
		{ // DST to STD Overlapp.

			if( dst < 0 )
			{
				// Assume STD here.
				LocalTime->tm_isdst = 0;
				//sec_UTC = sec_UTC + (TimeZone.DaylightBias * 60);
			}
			else
			{
				if( dst == 0 )
				{
					sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
				}
				else
				{
					sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
				}
			}
		}
		break;

	case OverLapp_2 :               // DST to STD Overlapp, one hour
		if( TMZ > 0 )
			dst = daylightBias;


		if( TypeOfYear )
		{
			if( dst == -1 )
			{  // Use STD time here.
				LocalTime->tm_isdst = 0;
				sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
			 }
			else
			{
				if( dst == 0 )
				{
					sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
				}
				else
				{
					sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
				}
			}
		}
		else
		{ // ANSI states that :
			// Use isdst if non-negative, otherwise use STD.
			if( dst < 0 )
			{ // Use STD

				LocalTime->tm_isdst = 0;
				// sec_UTC = sec_UTC + (TimeZone.StandardBias * 60);
				// Remove DST because we force the calculation to STD.
				sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
			}
			else
			{ // Use the isdst flag.
				if( dst == 0 )
				{

					sec_UTC = sec_UTC + (TimeZone.StandardBias * SECSPERMIN);
				}
				else
				{
					sec_UTC = sec_UTC + (TimeZone.DaylightBias * SECSPERMIN);
				}
			}
		}
		break;
	}
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s%d%s", ACE_TEXT("Leaving ACS_CHB_mtzconv::LocalToUTC() with sec_UTC : "),(int)sec_UTC,ACE_TEXT(" without DayLightSaving time"));
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	return sec_UTC;
} // End of LocalToUTC

/*===================================================================
   ROUTINE: tm_to_SYSTEMTIME
=================================================================== */
void ACS_CHB_mtzconv::tm_to_SYSTEMTIME(struct tm *timeInfo, SYSTEMTIME *st)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::tm_to_SYSTEMTIME()" );
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        st->wYear               =       timeInfo->tm_year + 1900;
        st->wMonth              =       timeInfo->tm_mon + 1;
        st->wDayOfWeek 			=       timeInfo->tm_wday;
        st->wDay                =       timeInfo->tm_mday;
        st->wHour               =       timeInfo->tm_hour;
        st->wMinute     		=       timeInfo->tm_min;
        st->wSecond     		=       timeInfo->tm_sec;
        if (timeInfo->tm_year < 70)
        {
        		st->wYear = st->wYear + 100;
        }
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::tm_to_SYSTEMTIME()" );
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}


}

/*===================================================================
   ROUTINE: DetermineYearPeriod
=================================================================== */
YearPeriod  ACS_CHB_mtzconv::DetermineYearPeriod( struct tm * LocalTime,
		TIME_ZONE_INFORMATION &TimeZone )
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::DetermineYearPeriod()" );
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	// First convert to SYSTEMTIME format
	SYSTEMTIME  st;

	tm_to_SYSTEMTIME(LocalTime, &st);
	//TR_HT72009 Changes Begin
	// Retrieve day of month.
	// This to handle eventual day-in-month format!
	int  DSTwDay = TimeZone.DaylightDate.wDay;
	int  STDwDay = TimeZone.StandardDate.wDay;
	int  DSTwHour = TimeZone.DaylightDate.wHour;
	int  STDwHour = TimeZone.StandardDate.wHour;
	// check if we have day-in-month format.
	// And must convert to absolute format.
	if(TimeZone.StandardDate.wYear == 0)
	{
		// Add correct year.
		TimeZone.StandardDate.wYear = st.wYear;
		TimeZone.DaylightDate.wYear = st.wYear;
		STDwDay = GetDayInMonth(TimeZone.StandardDate);
		DSTwDay = GetDayInMonth(TimeZone.DaylightDate);
	}

	if(TimeZone.DaylightDate.wMinute == 59 && TimeZone.DaylightDate.wSecond == 59)
	{
		DSTwHour = TimeZone.DaylightDate.wHour + 1;
	}
	if(TimeZone.StandardDate.wMinute == 59 && TimeZone.StandardDate.wSecond == 59)
	{
		STDwHour = TimeZone.StandardDate.wHour + 1;
	}

	// check if we are in dst period or in std period of year.
	// A year are looks like this :-)
	// Type 1.
	// New Year ------------------<DST>+++++++++++<STD>--------------- New Year
	// OR LIKE THIS
	// Type 0.
	//   New Year +++++++<STD>-----------<DST>+++++++ New Year
	// Check type of year in this timezone.
	bool TypeOfYear = CheckFirstTransition(TimeZone);

	if( TypeOfYear )
	{
		// New Year ---<DST         >+++++++++++<STD       >------------ New Year
		//  PeriodOne ,OverLapp_1,  PeriodTwo,  OverLapp_2,PeriodThree
		if( (TimeZone.DaylightDate.wMonth == st.wMonth) &&
				(DSTwDay == st.wDay)     &&
				(DSTwHour == st.wHour) )
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()1" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return OverLapp_1;
		}
		// Convert the time for TimeZone.StandardDate.wHour to minutes
		// to use the DaylightBias.
		time_t wHourMinutes = STDwHour*MINSPERHOUR ;
		time_t stwHourMinutes = st.wHour*MINSPERHOUR ;

		// Check for OverLapp_2
		if( (TimeZone.StandardDate.wMonth == st.wMonth) &&
				(STDwDay == st.wDay)     &&
				((wHourMinutes + TimeZone.DaylightBias) == stwHourMinutes) )
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()2" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return OverLapp_2;
		}
		// Check if we are in PeriodOne, PeriodTwo or PeriodThree.
		if(st.wMonth < TimeZone.DaylightDate.wMonth)
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod():PeriodOne3" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return PeriodOne;
		}
		if((st.wMonth < TimeZone.StandardDate.wMonth) &&
				(st.wMonth > TimeZone.DaylightDate.wMonth))
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()4" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return PeriodTwo;
		}
		if(st.wMonth > TimeZone.StandardDate.wMonth)
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod():PeriodThree5" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return PeriodThree;
		}

		if(TimeZone.DaylightDate.wMonth == st.wMonth)
		{// DST month
			if(DSTwDay < st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()6" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
			else if(DSTwDay > st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()7" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodOne;
			}
			else if(DSTwHour > st.wHour)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()8" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodOne;
			}
			else
			{

				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()9" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
		}
		else
		{// STD month.
			if(STDwDay < st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()10" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodThree;
			}
			else if(STDwDay > st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()11" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
			else if(STDwHour > st.wHour)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()12" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
			else
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()13" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodThree;
			}
		}
	}
	else
	{
		// Type 0.
		//   New Year +++++++<STD>-----------<DST>+++++++ New Year
		//  PeriodOne ,OverLapp_1,  PeriodTwo,  OverLapp_2,PeriodThree
		if( (TimeZone.DaylightDate.wMonth == st.wMonth) &&
				(DSTwDay == st.wDay)     &&
				(DSTwHour == st.wHour) )
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()14" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return OverLapp_2;
		}

		// Check for OverLapp_1
		// Convert the time for TimeZone.StandardDate.wHour to minutes
		// to use the DaylightBias.
		time_t wHourMinutes = STDwHour*MINSPERHOUR ;
		time_t stwHourMinutes = st.wHour*MINSPERHOUR ;

		if( (TimeZone.StandardDate.wMonth == st.wMonth) &&
				(STDwDay == st.wDay)     &&
				((wHourMinutes + TimeZone.DaylightBias) == stwHourMinutes) )
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()15" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return OverLapp_1;
		}

		// Check if we are in PeriodOne, PeriodTwo or PeriodThree.
		if(st.wMonth < TimeZone.StandardDate.wMonth)
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()16" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return PeriodOne;
		}
		if((st.wMonth < TimeZone.DaylightDate.wMonth) &&
				(st.wMonth > TimeZone.StandardDate.wMonth))
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()17" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return PeriodTwo;
		}
		if(st.wMonth > TimeZone.DaylightDate.wMonth)
		{
			if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()18" );
				ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return PeriodThree;
		}

		if(TimeZone.DaylightDate.wMonth == st.wMonth)
		{// DST month
			if(DSTwDay < st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()19" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodThree;
			}
			else if(DSTwDay > st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()20" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
			else if(DSTwHour > st.wHour)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()21" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
			else
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod()22" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodThree;
			}
		}
		else
		{// STD month.
			if(STDwDay < st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod(): PeriodTwo23" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
			else if(STDwDay > st.wDay)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod():PeriodOne24" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodOne;
			}
			else if(STDwHour > st.wHour)
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod():PeriodOne25" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodOne;
			}
			else
			{
				if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod(): PeriodTwo26" );
					ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}

				return PeriodTwo;
			}
		}
	}

	// This should never be reach, but to ensure that this method
	// never return something that not is a correct enum, this is added.
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::DetermineYearPeriod(): PeriodOne27" );
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	//TR_HT72009 Changes End
	return PeriodOne;

}

/*===================================================================
   ROUTINE: GetLocalTime
=================================================================== */
void ACS_CHB_mtzconv::GetLocalTime(SYSTEMTIME *AbsoluteDate)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
        {
             char traceBuffer[512];
             memset(&traceBuffer, 0, sizeof(traceBuffer));
             sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::GetLocalTime()");
             ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	/* Current system date time    */
	time_t rawtime(NULL);
	struct tm timeinfo;
	time(&rawtime);
	timeinfo = *ACE_OS::localtime(&rawtime);

	AbsoluteDate->wYear             =       timeinfo.tm_year + 1900;
	AbsoluteDate->wMonth    		=       timeinfo.tm_mon + 1;
	AbsoluteDate->wDayOfWeek 		=      	timeinfo.tm_wday;
	AbsoluteDate->wDay              =       timeinfo.tm_yday;
	AbsoluteDate->wHour             =       timeinfo.tm_hour;
	AbsoluteDate->wMinute   		=       timeinfo.tm_min;
	AbsoluteDate->wSecond   		=       timeinfo.tm_sec;

	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
        {
             char traceBuffer[512];
             memset(&traceBuffer, 0, sizeof(traceBuffer));
             sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::GetLocalTime()");
             ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

/*===================================================================
   ROUTINE: GetSystemTime
=================================================================== */
void ACS_CHB_mtzconv::GetSystemTime(struct tm* AbsoluteDate)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::GetSystemTime()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	/* Current system date time    */
	time_t rawtime(NULL);

	time(&rawtime);

	//AbsoluteDate = gmtime(&rawtime);

	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::GetSystemTime()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}


}

/*===================================================================
   ROUTINE: getCurrentTime
=================================================================== */
void ACS_CHB_mtzconv::getCurrentTime(struct tm* timeinfo)
{
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzconv::getCurrentTime()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	/* Current system date time    */

	time_t rawtime(NULL);

	//Get the current local time.
	time(&rawtime);

	//timeinfo = ACE_OS::localtime(&rawtime);
	if( ACS_CHB_MTZ_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzconv::getCurrentTime()");
		ACS_CHB_MTZ_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

}

