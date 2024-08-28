//*****************************************************************************
//
// NAME
//      ACS_USA_Time.cpp
//
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-1999.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// DOCUMENT NO
//
// AUTHOR 

// REVISION

// CHANGES

// SEE ALSO 
// 	
//
//******************************************************************************


#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "acs_usa_time.h"
#include "acs_usa_regexp.h"
#include "acs_usa_error.h"
#include "acs_usa_tratrace.h"

//******************************************************************************
// Format table
//******************************************************************************

char* ACS_USA_formatTable[] =
{
	(char*)"",								// as if initial
	(char*)"%H:%M",						// time
	(char*)"%H:%M:%S",                     // localeTime
	(char*)"%I:%M:%S %p",					// time with AM or PM
	(char*)"%m/%d/%y",                     // localeDate
	(char*)"%a %b %d %H:%M:%S %Y",         // localeDateTime
	(char*)"%a %b %e %H:%M:%S %Z %Y"       // dateCall
};

enum ACS_USA_Time_FormatIndex          	// For index of formatTable
{
	USA_Time_initial,				// for user supplied format
	USA_Time_hourMinute,			// time with hours and minutes
	USA_Time_localeTime,            // locale's time
	USA_Time_timeAs_AM_PM,			// 12 hour with AM or PM
	USA_Time_localeDate,            // locales date
	USA_Time_localeDateTime,        // locales date and time
	USA_Time_longDateTime           // as date(1) command
};


//******************************************************************************
// AM PM table
//******************************************************************************

const TM_Member		maxAMPM	     = 1;	// Max index in table
const TM_Member		twelve	     = 12;	// Correction for hours
const TM_Member		postMeridiem = 1;	// An index value
const ACS_USA_ByteCount	ampmLength = 2;	// length of strings in table

const char* ampmTable[]  = 		// Table holding AM PM strings
{
	"AM",
	"PM"		// postMeridiem
};


//******************************************************************************
// Month table
//******************************************************************************

const TM_Member			maxMonth = 11; 		// Max index for months
const ACS_USA_ByteCount monthLength = 3;	// 3 characters

const char* monthTable[] = 			// Table for months
{
    "Jan", "Feb", "Mar", "Apr",
		"May", "Jun", "Jul", "Aug",
		"Sep", "Oct", "Nov", "Dec"
};


//******************************************************************************
//	ACS_USA_Time()
//******************************************************************************
ACS_USA_Time::ACS_USA_Time() :
mode(timeOnly),
status(ACS_USA_Ok),
wallClock(ACS_USA_Time_error),
timeFormat(0),
timeStamp(0),
timeData(0),
wallYear(tm_InitialValue)
{
	reset();
}


//******************************************************************************
//      ACS_USA_Time()
//******************************************************************************

ACS_USA_Time::ACS_USA_Time(const String& format) :
mode(timeStampFormat),
status(ACS_USA_Error),
wallClock(ACS_USA_Time_error),
//timeFormat(new ACS_USA_TimeStampFormat[format.length() + endCharLength]),
timeStamp(0),
//timeData(new struct tm),
wallYear(tm_InitialValue)
{
	timeFormat = new ACS_USA_TimeStampFormat[format.length() + endCharLength];
	timeData = new struct tm;
	if (timeFormat != 0 && timeData != 0) 
	{
		status = ACS_USA_Ok;
		strcpy(timeFormat, format.data());
		reset();
	}
}


//******************************************************************************
//      ACS_USA_Time()
//******************************************************************************
ACS_USA_Time::ACS_USA_Time(const String& format, const String& position) :
mode(timeStampFormatRegexp),
status(ACS_USA_Error),
wallClock(ACS_USA_Time_error),
//timeFormat(new ACS_USA_TimeStampFormat[format.length() + endCharLength]),
//timeStamp(new ACS_USA_Regexp),
//timeData(new struct tm),
wallYear(tm_InitialValue)
{
	timeFormat = new ACS_USA_TimeStampFormat[format.length() + endCharLength];
	timeStamp = new ACS_USA_Regexp;
	timeData = new struct tm;
	if (timeStamp->compilePattern(position) == ACS_USA_Ok) 
	{
		status = ACS_USA_Ok;
		strcpy(timeFormat, format.data());
		reset();
	}
	
	// If compilation fails then error handling is done by ACS_USA_Regexp	
}


//******************************************************************************
//      ACS_USA_Time()
//******************************************************************************
ACS_USA_Time::ACS_USA_Time(const ACS_USA_Time& other) :
mode(other.mode),
status(other.status),
wallClock(other.wallClock),
timeFormat(0),
timeStamp(0),
timeData(0),
wallYear(other.wallYear)
{
	if (mode > timeOnly) 
	{	
		// If interpreter enabled
		timeFormat = new ACS_USA_TimeStampFormat[strlen(other.timeFormat)+endCharLength];
		timeData = new struct tm;
		strcpy(timeFormat, other.timeFormat);
		memcpy(timeData, other.timeData, sizeof(struct tm)); 
	} 
	
	if (mode > timeStampFormat) 
	{ 
		// If interpreting with REs
		timeStamp = new ACS_USA_Regexp(*(other.timeStamp));
	}	
}


//******************************************************************************
//      ~ACS_USA_Time()
//******************************************************************************
ACS_USA_Time::~ACS_USA_Time()
{
	delete [] timeFormat;
	delete timeStamp;
	delete timeData;
}


//******************************************************************************
//      reset()
//******************************************************************************
void
ACS_USA_Time::reset()
{
	ACS_USA_TimeType sysTime = time(0);	// Get system time
	
	if (sysTime == ACS_USA_Time_error) 
	{	
		string a(beginQuote);
		status = ACS_USA_Error;
		error.setError(ACS_USA_SystemCallError);
		a += Sys_timeCall;
		a += endQuote;
		a += ACS_USA_systemCallResult;
		error.setErrorText(sysErrorPrefix, a.data(), errno);
		return;
	}
	
	struct tm* sysTimeData = localtime(&sysTime);// Make local time
	
	sysTimeData->tm_isdst = tm_NegativeValue;// Force calculation of TZ+DST
	wallYear = sysTimeData->tm_year;
	
	// Convert the local time into  seconds
	
	if ((wallClock = mktime(sysTimeData)) == ACS_USA_Time_error) 
	{
		string a(beginQuote);
		//RWCString a(beginQuote);
		status = ACS_USA_Error;
		error.setError(ACS_USA_SystemCallError);
		a += Sys_mktimeCall;
		a += endQuote;
		a += ACS_USA_systemCallResult;
		error.setErrorText(sysErrorPrefix, a.data());
		// error handling
	}	
}


//******************************************************************************
//      operator = ()
//******************************************************************************
const ACS_USA_Time&
ACS_USA_Time::operator=(const ACS_USA_Time& other)
{
	if (this == &other) 
	{
		// It's myself
		return *this;
	}
	
	wallClock = other.wallClock;
	mode      = other.mode;
	status    = other.status;
	wallYear  = other.wallYear;
	
	if (mode > timeOnly)
	{	// If interpreter enabled
		delete [] timeFormat;
		delete timeData;
		timeFormat = new ACS_USA_TimeStampFormat[strlen(other.timeFormat)+endCharLength];
		timeData = new struct tm;
		
		strcpy(timeFormat, other.timeFormat);
		memcpy(timeData, other.timeData, sizeof(struct tm)); 
	}
	
	if (mode > timeStampFormat) 
	{ // If interpreting with REs
		delete timeStamp;
		
		// Call the copy constructor for Regexp
		timeStamp = new ACS_USA_Regexp(*(other.timeStamp));		
	}
	
	return *this;	
}


//******************************************************************************
//      ACS_USA_seconds()
//****************************************************************************** 
ACS_USA_TimeType 
ACS_USA_Time::seconds(const ACS_USA_RecordType record)
{
	
	// Pointer of the current position in input record
	ACS_USA_RecordType	recordPointer	= record;
	ACS_USA_TimeType retTime = ACS_USA_Time_error; //time in seconds returned 
	{	
		// - This block handles status and initial position of the time stamp -
		
		ACS_USA_ByteCount	stampOffset	= noPrefix;
		
		// Check if we can interpret. If it is possible then initialise
		// local currency.
		
		if (status == ACS_USA_Error)
		{		// Just to be sure
			return ACS_USA_Time_error;
		} 
		else if (mode < timeStampFormat)
		{	// timeOnly isn't enough
			// error handling incorrect mode
			error.setError(ACS_USA_BadState);
			error.setErrorText(ACS_USA_Time_BadState);
			return ACS_USA_Time_error;
		} 
		else if (mode == timeStampFormatRegexp) 
		{ // Reg expr given
			Regexp_MatchType m = timeStamp->checkMatch(record, &stampOffset);
			if (m == ACS_USA_Regexp_error) 
			{
				// error handling is done in Regexp
				return ACS_USA_Time_error;
			}
			else if (m == ACS_USA_Regexp_noMatch) 
			{
				error.setError(ACS_USA_TimeStampMatch);
				error.setErrorText(ACS_USA_TimestampError);
				return ACS_USA_Time_error;
			}
			else if (m == ACS_USA_Regexp_match) 
			{
				// OK. Expression matched.
				// Move record ptr to time stamp
				recordPointer += stampOffset;
			} 
			else 
			{
				error.setError(ACS_USA_BadState);
				return ACS_USA_Time_error;
			}
		}
		else 
		{
			// mode is timeStampFormat. Do nothing
		}
		
	}	// ------------------------ End of block ---------------------------
	
	// Everything ok. Initialise the time structure
	
	timeData->tm_sec   = tm_InitialValue;	// maybe not present
	timeData->tm_min   = tm_InvalidMember;
	timeData->tm_hour  = tm_InvalidMember;
	timeData->tm_mday  = tm_InvalidMember;
	timeData->tm_mon   = tm_InvalidMember;
	timeData->tm_year  = tm_InvalidMember;	// maybe not present
	timeData->tm_wday  = tm_InvalidMember;
	timeData->tm_yday  = tm_InvalidMember;
	timeData->tm_isdst = tm_NegativeValue;
	
	
	{	//------------- The block for parsing format strings ----------------
		
		ACS_USA_TimeStampFormat	*formatPointer	= timeFormat;
		ACS_USA_TimeStampFormat	*formatEnd	= timeFormat + strlen(timeFormat);
		ACS_USA_TimeStampFormat *savedFormat	= timeFormat;
		ACS_USA_RecordType	recordEnd	= record + strlen(record);
		ACS_USA_Time_FormatIndex selector 	= USA_Time_initial;
		Flag			ampmFlag 	= ACS_USA_False;
		Flag 			notFinish	= ACS_USA_True;
		
		
		// *** BIG LOOP ***
		
		// parse input from different stings
		
		while(notFinish) 
		{	
			Flag switchFlag	= ACS_USA_False;
			int ampmIdx 	= 0;
			
			// Catch overflow
			if (recordPointer > recordEnd || formatPointer > formatEnd) 
			{
				error.setError(ACS_USA_BadState);
				error.setErrorText(ACS_USA_Time_BadState);
				return ACS_USA_Time_error;
			}
			
			// format index
			
			switch (*formatPointer) 
			{
				
			case CS_terminateInput:
				
				if (selector != USA_Time_initial) 
				{
					// Switch from alternate format to 
					// original format
					formatPointer = savedFormat;
					selector 	  = USA_Time_initial;
					++formatPointer;	// skip the current char
				} 
				else
				{
					// End of input format, finish the loop
					notFinish = ACS_USA_False;
				}
				
				break;
				
			case CS_perCentSign:
				
				// ---- Conversion specification ----
				
				
				++formatPointer;	// show the next char
				
				switch(*formatPointer) 
				{
					
				case 	CS_terminateInput:
					
					//error in format
					notFinish = ACS_USA_False;
					error.setError(ACS_USA_ErrorTimeStampFormat);
					error.setErrorText(ACS_USA_ErroneousTSFormat);
					// errorcode = error in format
					break;
					
				case	CS_perCentSign:
					
					if (*formatPointer != *recordPointer) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					else 
					{
						++recordPointer;
					}
					break;
					
				case	CS_dayOfWeek:
					
					// Abbreviated week day name
					recordPointer = moveToWhiteSpace(recordPointer);
					break;
					
				case	CS_longDayOfWeek:
					
					//Full week day name
					recordPointer = moveToWhiteSpace(recordPointer);
					break;
					
				case	CS_monthName:
					
					if ((timeData->tm_mon = getStringIndex(
						recordPointer,
						monthTable,
						maxMonth,
						monthLength)) == tm_InvalidMember)
					{
						// Month not found
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
					} 
					else 
					{
						recordPointer = moveToWhiteSpace(recordPointer);
					}
					break;
					
				case	CS_longMonthName: 
					
					if ((timeData->tm_mon = getStringIndex(
						recordPointer,
						monthTable,
						maxMonth,
						monthLength)) == tm_InvalidMember) 
					{
						// Month not found
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					} 
					else 
					{
						recordPointer = moveToWhiteSpace(recordPointer);
					}
					break;
					
				case	CS_localeDateTimeFormat:
					
					selector = USA_Time_localeDateTime;
					switchFlag = ACS_USA_True;
					// select new format from table
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_localeDateTime];
					break;
					
				case	CS_longDateTimeFormat:
					
					selector = USA_Time_longDateTime;
					switchFlag = ACS_USA_True;
					// select new format from table
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_longDateTime];
					break;
					
				case	CS_dayOfMonthZero:
					
					if ((timeData->tm_mday = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_DateFormat:
					
					selector = USA_Time_localeDate;
					switchFlag = ACS_USA_True;
					// Select new format
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_localeDate];
					break;
					
				case	CS_dayOfMonthSpace:
					
					if ((timeData->tm_mday = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_hour24:
					
					if ((timeData->tm_hour = stringToTMmember(
						recordPointer)) == tm_InvalidMember)
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_hour12Zero:
					
					if ((timeData->tm_hour = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					recordPointer = moveToNonDigit(recordPointer);
					ampmFlag = ACS_USA_True;
					break;
					
				case	CS_dayNumberYear:
					
					// Day number in year. Dont check
					recordPointer = moveToWhiteSpace(recordPointer);
					break;
					
				case	CS_monthNumber:
					
					if ((timeData->tm_mon = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					--timeData->tm_mon; // struct tm counts from 0
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_minute:
					
					if ((timeData->tm_min = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_anteOrPostMeridiem:
					
					if ((ampmIdx = getStringIndex(
						recordPointer,
						ampmTable,
						maxAMPM,
						ampmLength)) == tm_InvalidMember)
					{
						// neither AM not PM ?
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					} 
					else 
					{
						recordPointer = moveToWhiteSpace(recordPointer);
					}
					if (ampmFlag == ACS_USA_True) 
					{
						if (ampmIdx == postMeridiem &&
							timeData->tm_hour != twelve) 
						{
							timeData->tm_hour += twelve;
						} 
						else if (timeData->tm_hour == twelve) 
						{
							timeData->tm_hour -= twelve; 	
						} 
						else 
						{
						}
					}
					break;
					
				case	CS_timeFormat_AM_PM:
					
					selector = USA_Time_timeAs_AM_PM;
					switchFlag = ACS_USA_True;
					// Select new format
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_timeAs_AM_PM];
					break;
					
				case	CS_timeHourMinuteFormat:
					
					selector = USA_Time_hourMinute;
					switchFlag = ACS_USA_True;
					// Select new format
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_hourMinute];
					break;
					
				case	CS_seconds:
					
					if ((timeData->tm_sec = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_fullTimeFormat:
					
					selector = USA_Time_localeTime;
					switchFlag = ACS_USA_True;
					// Select new format
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_localeTime];
					break;
					
				case	CS_decimalWeekDayMonday:
					
					recordPointer = moveToWhiteSpace(recordPointer);
					break;
					
				case	CS_weekNumberOfYearSunday:
					
					recordPointer = moveToWhiteSpace(recordPointer);
					break;
					
				case	CS_weekNumberOfYearMonday:
					
					recordPointer = moveToWhiteSpace(recordPointer);
					break;
					
				case	CS_decimalWeekDaySunday:
					
					recordPointer = moveToWhiteSpace(recordPointer);
					break;
					
				case	CS_localeDateFormat:
					
					selector = USA_Time_localeDate;
					switchFlag = ACS_USA_True;
					// Select new format
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_localeDate];
					break;
					
				case	CS_localeTimeFormat:
					
					selector = USA_Time_localeTime;
					switchFlag = ACS_USA_True;
					// Select new format
					savedFormat = formatPointer;
					formatPointer = ACS_USA_formatTable[USA_Time_localeTime];
					break;
					
				case	CS_yearWithinCentury:
					
					if ((timeData->tm_year = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					// after year 2000
					if (wallYear > tm_year_1999) 
					{
						timeData->tm_year += tm_newCenturyCorrection;
					}
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_yearIncludingCentury:
					
					if ((timeData->tm_year = stringToTMmember(
						recordPointer)) == tm_InvalidMember) 
					{
						error.setError(ACS_USA_WarnTimeStampFormat);
						error.setErrorText(ACS_USA_WarnTSFormat);
						notFinish = ACS_USA_False;
					}
					timeData->tm_year -= tm_startYear;
					recordPointer = moveToNonDigit(recordPointer);
					break;
					
				case	CS_timeZoneName:
					
					// expect time zone name
					recordPointer = moveToWhiteSpace(recordPointer);
					// Can be DST as well
					recordPointer = skipWhiteSpace(recordPointer);
					if (!strncmp(recordPointer, dayLightSavings, DST_length))
					{
						recordPointer = moveToWhiteSpace(recordPointer);
					}
					
					break;
					
					
				default:
					
					//error in format
					notFinish = ACS_USA_False;
					status = ACS_USA_Error;
					// errorcode = error in format
					break;
			}
			
			// If switch took place then dont't increase formatP.
			if (switchFlag == ACS_USA_True) 
			{
				switchFlag = ACS_USA_False;
			} 
			else 
			{
				++formatPointer; // CS character processed
			}
			break;
			
		default:
			// Compare with the current char in record
			// except whitespaces
			
			if (isspace(*formatPointer)) 
			{	
				formatPointer = skipWhiteSpace(formatPointer);
				recordPointer = skipWhiteSpace(recordPointer);
			}
			else if (*formatPointer != *recordPointer) 
			{
				//error invalid record;or format?	
				notFinish = ACS_USA_False;
				error.setError(ACS_USA_WarnTimeStampFormat);
				error.setErrorText(ACS_USA_WarnTSFormat);
			}
			else 
			{
				++recordPointer;
				++formatPointer;
			}
			break;
			
		}
		
	}	// *** END OF BIG LOOP ***
	
	
    }	//----------------------- End of the parsing block ---------------------
	
    { 
		ACS_USA_Flag noYear = ACS_USA_False;
		ACS_USA_Flag dontStop = ACS_USA_True;
		
		//
		// Check if some error occurred and if all needed data is present
		//
		if (error.getError()) 
		{
			return ACS_USA_Time_error;
		}
		
		if (timeData->tm_min  == tm_InvalidMember ||
			timeData->tm_hour == tm_InvalidMember ||
			timeData->tm_mday == tm_InvalidMember ||
			timeData->tm_mon  == tm_InvalidMember) 
		{
			error.setError(ACS_USA_ErrorTimeStampFormat);
			error.setErrorText(ACS_USA_MissingFormatParameter, timeFormat);
			return ACS_USA_Time_error;
		}
		
		if (timeData->tm_year == tm_InvalidMember) 
		{
			// Year was not present in the time stamp assume current year
			timeData->tm_year = wallYear;
			noYear = ACS_USA_True;
		}	
		
		//
		// make the time and return it if ok
		//
		while (dontStop == ACS_USA_True) 
		{
			retTime  = mktime(timeData);
			if (retTime == ACS_USA_Time_error) 
			{
				error.setError(ACS_USA_CannotExtractTime);
				error.setErrorText(ACS_USA_CannotMakeTime);
				dontStop = ACS_USA_False;
			}
			else 
			{
				// If there was no year in the timestamp and
				// calculated time was bigger than the current time
				// then the year in the time stamp should be 
				// probably previous one
				//  
				if (noYear == ACS_USA_True && retTime > wallClock) 
				{
					--timeData->tm_year;
				} 
				else 
				{
					dontStop = ACS_USA_False;
				}
			}
		}
    }
	
    return retTime;
}


//******************************************************************************
//      skipWhiteSpace()
//******************************************************************************
ACS_USA_RecordType
ACS_USA_Time::skipWhiteSpace(ACS_USA_RecordType recordPtr) const
{
    while(isspace(*recordPtr)) 
	{
		++recordPtr;
    }
    return recordPtr;
}


//******************************************************************************
//      moveToWhiteSpace()
//****************************************************************************** 
ACS_USA_RecordType
ACS_USA_Time::moveToWhiteSpace(ACS_USA_RecordType recordPtr) const
{
	if(isspace(*recordPtr))
		++recordPtr;

    while(!isspace(*recordPtr)) 
	{
		++recordPtr;
    }
    return recordPtr;
}


//******************************************************************************
//      moveToNonDigit()
//******************************************************************************
ACS_USA_RecordType
ACS_USA_Time::moveToNonDigit(ACS_USA_RecordType recordPtr) const
{
    while(isdigit(*recordPtr)) 
	{
		++recordPtr;
    }
    return recordPtr;
}


//******************************************************************************
//      getStringIndex()
//******************************************************************************
TM_Member
ACS_USA_Time::getStringIndex(const char* const recordPtr,
							 const char* const * table,
							 const TM_Member maxIndex,
							 const ACS_USA_ByteCount length) const
{
	
    for(int i = 0; i <= maxIndex; ++i) 
	{
		if (!strncmp(table[i], recordPtr, length)) 
		{
			return i;
		}
    }
	
    return tm_InvalidMember;
}

