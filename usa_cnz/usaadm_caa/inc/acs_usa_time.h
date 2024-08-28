//******************************************************************************
// 
// .NAME 
//  	ACS_USA_Time - USA specific time handling
// .LIBRARY 3C++
// .PAGENAME ACS_USA_Time
// .HEADER  TMOS/ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE ACS_USA_Time.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	The class handles getting the time information from system
//	and also from strings that contain date and time according
//	"C" locale. If retrieving time from strings the characters that
//	precede the time stamp must be specified as a regular expression 

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0259

// AUTHOR 
// 	1995-06-12 by ETX/TX/T XKKHEIN

// .REVISION 
//	PA2	990915

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO	DATE 	NAME	DESCRIPTION
//	PA1		950829	XKKHEIN	First Revision
//  PA2		990915	UABDMT	Ported to Windows NT4.0
//	PA3		040429	QVINKAL	  Removal of RougeWave Tools.h++
// .LINKAGE
//

// .SEE ALSO 
// 	Standard C strftime() function

//******************************************************************************

#ifndef ACS_USA_TIME_H 
#define ACS_USA_TIME_H

#include	<time.h>		// System specific time
#include 	<sys/types.h>	// some system types
#include	<ctype.h>		// isspace, etc. macros
#include	<stdio.h>		// sscanf
#include	<string.h>		// strncmp
#include 	<string>
#include	"acs_usa_types.h"	// constants for USA

using namespace std;
//typedef string String;
//******************************************************************************
// Type definitions local to USA_Time class
//******************************************************************************
typedef int  	Flag;			 // flag type
typedef char	ConversionChar;  // Conversion specification char
typedef char*	TimeString;		 // Time related tables
typedef int	TM_Member;		     // Type for members of tm struct


//******************************************************************************
// Miscellaneous constants local to USA_Time class
//******************************************************************************
const ACS_USA_TimeType ACS_USA_Time_error = -1L; // Error from time routines

const ACS_USA_ByteCount noPrefix	 = 0;	// record starts with timestamp

const unsigned  endCharLength	 	 = 1;	// To be added when getting mem

const int	one_Integer		 = 1;	// must be returned by sscanf

const TM_Member	tm_InitialValue		 = 0;   // For initialisation of tm
const TM_Member	tm_NegativeValue 	 = -1;  // A value for struct tm
const TM_Member	tm_InvalidMember	 = -1;	// invalid member
const TM_Member tm_year_1999		 = 1999; // Last year in 20th century
const TM_Member tm_startYear		 = 1900; // First year in tm struct
const TM_Member tm_newCenturyCorrection  = 100;	// Used after year 2000

const char* const	dayLightSavings  = "DST"; // String for DST
const size_t 		DST_length	 = 3;

//******************************************************************************
// Conversion Specification charaters for time stamp interpreting.
// These characters are used in strftime() format
//******************************************************************************

const ConversionChar CS_terminateInput		= '\0';
const ConversionChar CS_perCentSign		= '%';
const ConversionChar CS_dayOfWeek		= 'a';
const ConversionChar CS_longDayOfWeek		= 'A';
const ConversionChar CS_monthName		= 'b';
const ConversionChar CS_longMonthName		= 'B';
const ConversionChar CS_localeDateTimeFormat	= 'c'; 
const ConversionChar CS_longDateTimeFormat	= 'C'; 
const ConversionChar CS_dayOfMonthZero		= 'd'; 
const ConversionChar CS_DateFormat		= 'D'; 
const ConversionChar CS_dayOfMonthSpace		= 'e'; 
const ConversionChar CS_hour24			= 'H'; 
const ConversionChar CS_hour12Zero		= 'I'; 
const ConversionChar CS_dayNumberYear		= 'j'; 
const ConversionChar CS_monthNumber		= 'm'; 
const ConversionChar CS_minute			= 'M'; 
const ConversionChar CS_anteOrPostMeridiem	= 'p';
const ConversionChar CS_timeFormat_AM_PM	= 'r';
const ConversionChar CS_timeHourMinuteFormat	= 'R';
const ConversionChar CS_seconds			= 'S';
const ConversionChar CS_fullTimeFormat		= 'T';
const ConversionChar CS_decimalWeekDayMonday	= 'u';
const ConversionChar CS_weekNumberOfYearSunday	= 'U';
const ConversionChar CS_weekNumberOfYearMonday	= 'W';
const ConversionChar CS_decimalWeekDaySunday	= 'w';
const ConversionChar CS_localeDateFormat	= 'x';
const ConversionChar CS_localeTimeFormat	= 'X';
const ConversionChar CS_yearWithinCentury	= 'y';
const ConversionChar CS_yearIncludingCentury	= 'Y';
const ConversionChar CS_timeZoneName		= 'Z';


  
//******************************************************************************
// Error Strings
//******************************************************************************
const char* const ACS_USA_Time_BadState	    = "Time Object in bad state";
const char* const ACS_USA_TimestampError    = "Timestamp didn't match";
const char* const ACS_USA_ErroneousTSFormat = "Error in time stamp format";
const char* const ACS_USA_CannotMakeTime    = "Cannot extract time from record";
const char* const ACS_USA_WarnTSFormat	    = "Possible error in time stamp format";


//******************************************************************************
//Forward declaration(s)
//******************************************************************************
class ACS_USA_Regexp;


//******************************************************************************
//Member functions, constructors, destructors, operators
//******************************************************************************

class  ACS_USA_Time
{

 public:

  ACS_USA_Time();
  // Description:
  // 	Default constructor. Gets the current time and stores it internally
  // Parameters: 
  //	none
  // Return value: 
  //	none

	ACS_USA_Time(const String& format);
  // Description:
  // 	Constructor. Constructs the object to be ready to process
  //	time stamp strings.
  // Parameters: 
  //	format			time format string according to 
  //				strftime() function	
  // Return value: 
  //	none

  
	ACS_USA_Time(const String& format,
	       const String& position);

  // Description:
  // 	Constructor. Constructs the object to be ready to process
  //	time stamp strings that have some characters preceding 
  //	the time stamp.
  // Parameters: 
  //	format			time format string according to 
  //				strftime() function, in
  //	position		regular expression pattern that matches 
  //				all characters preceding the time stamp, in
  // Return value: 
  //	none

  ACS_USA_Time(const ACS_USA_Time& other);
  // Description:
  // 	Constructor. Creates a copy from itself.
  // Parameters: 
  //	other			a reference to an existing object, in
  // Return value: 
  //	none

  virtual ~ACS_USA_Time();
  // Description:
  // 	Destructor. Releases all allocated memory.
  // Parameters: 
  //	none
  // Return value: 
  //	none

  ACS_USA_ReturnType isValid() const;
  // Description:
  // 	Destructor. Releases all allocated memory.
  // Parameters: 
  //	none
  // Return value: 
  //	none

  void reset();
  // Description:
  // 	Gets the current time from system and recalculates it 
  //	with timezone and daylight savings and stores the current
  //	local time internally in form of seconds
  // Parameters: 
  //	none
  // Return value: 
  //	none

  ACS_USA_TimeType seconds() const;
  // Description:
  // 	Returns the number of UTC secons since Jan 1 1970. Local
  //	timezone and daylight savings are included
  // Parameters: 
  //	none
  // Return value: 
  //	see description

  ACS_USA_TimeType seconds(const ACS_USA_RecordType record);
  // Description:
  // 	Returns the number of UTC secons since Jan 1 1970. Local
  //	timezone and daylight savings are included. The number of 
  //	seconds is calculated from a time stamp contained in the 
  //	record. The time stamp format and/or timestamp position expression
  //	must be	present during creation of the object.
  // Parameters: 
  //	record			character string containing time stamp 
  // Return value: 
  //	ACS_USA_TimeError 	indicates error
  //	<number of seconds>	normally returns seconds

  const ACS_USA_Time& operator=(const ACS_USA_Time& other);
  // Description:
  // 	Assignment operator
  // Parameters: 
  //	Reference to an existing ACS_USA_Time object
  // Return value: 
  //	none

private:

  ACS_USA_RecordType moveToWhiteSpace(ACS_USA_RecordType recordPtr) const;
  // Description:
  // 	Moves the recordPtr to first white-space character 
  // Parameters: 
  //	recordPtr		pointer to a record, out
  // Return value: 
  //	recordPtr		new position of record pointer

  ACS_USA_RecordType skipWhiteSpace(ACS_USA_RecordType recordPtr) const;
  // Description:
  // 	Moves the recordPtr to first non-white-space character 
  // Parameters: 
  //	recordPtr		pointer to a record, out
  // Return value: 
  //	recordPtr		new position of record pointer

  ACS_USA_RecordType moveToNonDigit(ACS_USA_RecordType recordPtr) const;
  // Description:
  // 	Moves the recordPtr to first non-digit character 
  // Parameters: 
  //	recordPtr		pointer to a record, out
  // Return value: 
  //	recordPtr		new position of record pointer

  TM_Member stringToTMmember(const ACS_USA_RecordType recordPtr) const;
  // Description:
  // 	converts sting to integer
  // Parameters: 
  //	recordPtr		pointer to a record, in
  // Return value: 
  //	tm_InvalidMember	if cannont be converted
  //	TM_Member

  TM_Member getStringIndex(
		const char* const recordPtr,
		const char* const * table,
		const TM_Member maxIndex,
		const ACS_USA_ByteCount length) const;
  // Description:
  // 	Compares length of bytes from a record with strings in table
  // Parameters: 
  //	recordPtr		pointer to a record, in
  // Return value: 
  //	tm_InvalidMember	if none of strings matced
  //	TM_Member		the index of matching string in table


  enum ObjectMode
  {
	timeOnly,			// Just for time
	timeStampFormat,		// For interpreting stamps
	timeStampFormatRegexp		// Interpreting stamps with RE
  } mode;				// Object mode 

  ACS_USA_ReturnType	status;		// Status after construction
  ACS_USA_TimeType	wallClock;	// Local time in seconds
  ACS_USA_TimeStampFormat *timeFormat;	// Time stamp format
  ACS_USA_Regexp	*timeStamp;	// Time stamp position
  struct	tm	*timeData;	// Data for time
  TM_Member		wallYear;	// Year in the object
 
};					// End of class declaration



 
//******************************************************************************
//      seconds()
//******************************************************************************
inline
ACS_USA_TimeType
ACS_USA_Time::seconds() const
{
    return wallClock;
}



//******************************************************************************
//      isValid()
//******************************************************************************

inline
ACS_USA_ReturnType
ACS_USA_Time::isValid(void) const
{
    return status;
}



//******************************************************************************
//      stringToTM()
//****************************************************************************** 
inline
TM_Member
ACS_USA_Time::stringToTMmember(const ACS_USA_RecordType recordPtr) const
{
    TM_Member member = tm_InvalidMember;

    if (sscanf(recordPtr, "%d", &member) != one_Integer) {
    	member = tm_InvalidMember;
    }

    return member;

}



#endif
