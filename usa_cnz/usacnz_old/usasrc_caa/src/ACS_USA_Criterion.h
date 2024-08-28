//******************************************************************************
// 
// NAME 
//  	  ACS_USA_Criterion - Handling of criterions in USA.
//
//  COPYRIGHT
//      Ericsson AB 2005 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the 
//      written permission from Ericsson AB or in accordance 
//      with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION 
//      ACS_USA_Criterion holds all data conerning a criterion for a 
//	    single log file. 
//	    Methods support initialisation of the object, adding/removing 
//	    criteria, matching a record against criteria, and updating 
//	    alarm generation.
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//	    190 89-CAA 109 0259
//
// AUTHOR 
// 	    1995-06-26 by ETX/TX/T XKKHEIN
//
// REV  DATE 	  NAME	   DESCRIPTION
// A    950829	  XKKHEIN  First version.
// B    990915	  UABDMT   Ported to Windows NT4.0.
// C    030212	  UABPEK   New AP VIRUS alarms.
// D    040429	  QVINKAL  Removal of RougeWave Tools.h++
// E    051011	  UABPEK   Suppression of alarms by named PHA parameter table.
// F    071231	  EKAMSBA  General Error filtering.
// G    080215	  EKAMSBA  Auto cease alarm.
// H    080318	  EKAMSBA  PRC alarms handling.
//
//******************************************************************************

#ifndef ACS_USA_CRITERION_H
#define ACS_USA_CRITERION_H

#include <string>
#include <vector>

#include "ACS_USA_Control.h"
#include "ACS_USA_Global.h"
#include "ACS_USA_Regexp.h"
#include "ACS_USA_EventInfo.h"


using namespace std;
typedef string String;

//******************************************************************************
// Type definitions local to this class
//******************************************************************************
typedef int	  ACS_USA_MatchType; 		// Type returned from matching routine
typedef int	  ACS_USA_Occurrences;
typedef int	  ACS_USA_PatternMatch;		

//******************************************************************************
// Constants used in this class
//******************************************************************************
// const	ACS_USA_TimeType noAlarm 	 = 0;	// Alarm condition is not met
const	ACS_USA_TimeType secondsInMinute = 60;	
const	ACS_USA_TimeType minutesInHour 	 = 60;
const	ACS_USA_TimeType hoursInDay 	 = 24;

const	ACS_USA_PatternMatch unknowknownMatch   = 0;
const	ACS_USA_PatternMatch subStringMatch 	= 1;
const	ACS_USA_PatternMatch patternMatch   	= 2;

const	int subStringSize = 1024;	// Buffer size for substring matching

const	ACS_USA_MatchType Criterion_matchError = 0; // Error occurred
const   ACS_USA_MatchType Criterion_noMatch    = 1; // Didn't match
const	ACS_USA_MatchType Criterion_match      = 2; // matched the criterion
 
// Keywords for analysis type
const 	char* const	Key_substringMatch	= "SUBSTRING";
const 	char* const	Key_paternMatch		= "PATTERN";

// Keywords for general error filter
const   char* const Key_GEFilterON  = "ON";
const   char* const Key_GEFilterOFF = "OFF";

// Scope related constants
const char* const ACS_USA_timeSpecificationChars = "[DHMS]";

const	int	minScope	= 1;		// min scope
const	int	maxScope	= 10000;	// max scope

const	int minCeaseDuration = 1;      // min cease duration
const	int maxCeaseDuration = 100;    // max cease duration

const char days = 'D';
const char hours = 'H';
const char minutes = 'M';
const char seconds = 'S';
const char slashChar = '/';
const char* const objectOfRefPrefix = "LOGFILE/";

// Various constants
const	int	invalidInteger   = -1;	   // returned by toInt()
const	int	minEventsAllowed = 0;		   // min events allowed
const	int	maxEventsAllowed = 100000; // max events allowed

const char* const ACS_USA_criterionMatch = "A criterion match detected";

const long invalidLong    = -1L;

//******************************************************************************
// Forward declarations
//******************************************************************************
class ACS_USA_AnalysisObject;
class ACS_USA_Regexp;

class File;
class ACS_USA_Config;

//******************************************************************************
// Member functions, constructors, destructors, operators
//******************************************************************************


class ACS_USA_Criterion
{
friend ACS_USA_Config;

public:

  ACS_USA_Criterion();
  // Description:
  //     Constructor. Constructs the Criterion object
  // Parameters:
  //	none
  // Return value:
  //    none

  ACS_USA_Criterion(const ACS_USA_Criterion& other);
  // Description:
  //    Copy constructor. Initialises the object with data from other object
  // Parameters:
  //    other                   another ACS_USA_Criterion object
  // Return value:
  //    none

  virtual ~ACS_USA_Criterion();
  // Description:
  //     Destructor. Releases all allocated memory
  // Parameters:
  //    none
  // Return value:
  //    none

  ACS_USA_StatusType validate();
  // Description:
  //    Validates the object.
  // Parameters:
  //    none
  // Return value:
  //    ACS_USA_ok		Construction OK
  //	ACS_USA_error		Construction failed
  
  ACS_USA_MatchType match(const ACS_USA_RecordType record,
			  const String fileName, 
			  const ACS_USA_TimeType   recordTime,
			  const ACS_USA_ActivationType activation,
			  const vector<String>& insertedStrings,
			  LPBYTE dataBuffer,
			  DWORD  dataLength);
  // Description:
  //    Checks if incoming record matches the criterion. If it is true
  //    scope is checked and if number of matches within the scope have been
  //	encountered the criterion is marked with alarm status.
  // Parameters:
  //    record          System event reocord, in
  //	recordTime		the time record was logged
  //	activation		flag indicatig long or short term analysis
  //	insertedStrings	strings from the event record
  //	dataBuffer		buffer containing event-specific data
  //	dataLength		size of the buffer, in bytes
  // Return value:
  //    Criterion_matchError    Something went wrong
  //    Criterion_noMatch	didn't match
  //	Criterion_match		did match


  ACS_USA_StatusType raiseAlarm(
				ACS_USA_AnalysisObject *ao,
				unsigned int alarmFilterInterval,
				const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
				vector<ACS_USA_EventInfo>& eventsToCease);
  // Description:
  //   Checks if incoming record matches the criterion. If it is true
  //   AP event report is generated.
  // Parameters:
  //   ao						        Analysis object.
  //	 alarmFilterInterval  Min interval between alarms on the same error.
  //   suppressedAlarms     Criteria that should not raise alarm.
  //   eventsToCease		List of events to cease.
  // Return value:
  //   ACS_USA_error  Something went wrong
  //   ACS_USA_ok     Everything ok
  
  ACS_USA_StatusType raiseCustomAlarm(
				ACS_USA_AnalysisObject *ao,
				unsigned int alarmFilterInterval,
				const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
				vector<ACS_USA_EventInfo>& eventsToCease);
  // Description:
  //   Checks if incoming record matches the criterion. If it is true
  //   AP event report is generated. This function overrides ACF settings
  //   with event log data.
  // Parameters:
  //   ao					Analysis object.
  //   alarmFilterInterval  Min interval between alarms on the same error.
  //   suppressedAlarms     Criteria that should not raise alarm.
  //   eventsToCease		List of events to cease.
  // Return value:
  //   ACS_USA_error  Something went wrong
  //   ACS_USA_ok     Everything ok



  void update(const ACS_USA_Criterion& other);
  // Description:
  //    Updates alarm information in the criterion object.
  // Parameters:
  //    other                   reference to a criterion object, in
  // Return value:
  //    none  

  ACS_USA_StatusType read(File& file);
  
  // Description:
  //    Reads some data from file..
  // Parameters:
  //    file			reference to a RWFile object to read from, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok
  
  ACS_USA_StatusType write(File& file);
  // Description:
  //    Writes some data to file..
  // Parameters:
  //    file			reference to a RWFile object to write to, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok
  
  void setObject(const String& str);
  // Description:
  //    Copies str into object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    none  
  
  void setPattern(const String& str);
  // Description:
  //    Copies str into pattern.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    none  
  
  ACS_USA_StatusType setSpecificProblem(const String& str);
  // Description:
  //    Sets the value of specific problem.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_StatusType setProbableCause(String& str);
  // Description:
  //    Copies str into probableCause.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    none
  
  void setObjectOfRefSuffix(const String& str);
  // Description:
  //    Copies str into objectOfRefSuffix.
  // Parameters:
  //    str	    reference to a String object, in
  // Return value:
  //    none  

  void setProblemText(const String& str);
  // Description:
  //    Copies str into problemText.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    none  
  
  void setCommand(const String& str);
  // Description:
  //    Copies str into command.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    none

  void setNode(const String& str);
  // Description:
  //    Copies str into node variable.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    none  

  void setDisableAlarmFilter(const String& str);
  // Description:
  //    Copies str into disableAlarmFilter variable.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    none  

  String& getObject();
  // Description:
  //    returns address of the object.
  // Parameters:
  //    none
  // Return value:
  //    see description

 String& getPattern();

  ACS_USA_SpecificProblem getSpecificProblem();
  // Description:
  //    Returns the value of specific problem.
  // Parameters:
  //    none
  // Return value:
  //    see description
  
  String& getCommand();
  // Description:
  //    returns address of the command.
  // Parameters:
  //    none
  // Return value:
  //    see description

  String& getGeneralErrorFilter();
  // Description:
  //    returns the value of the generalErrorFilter
  // Parameters:
  //    none
  // Return value:
  //    see description

  ACS_USA_Boolean operator==(const ACS_USA_Criterion& other) const;
  // Description:
  //    Equality operator.
  // Parameters:
  //    str                   reference to string object, in
  // Return value:
  //    ACS_USA_False		not equal
  //	ACS_USA_True		equal

  void dump();
  // Description:
  //    Dumps the contents of criterion. Can be used when compiled
  //	with -DDEBUG option
  // Parameters:
  //    none
  // Return value:
  //    none

private:
	// Help to auto backup and restore attributes 
	// of the ACS_USA_Criterion
	struct AutoBackUpRestoreData
	{
		AutoBackUpRestoreData(ACS_USA_Criterion& obj);
		~AutoBackUpRestoreData();

		// Object to be backup/restore -ed
		ACS_USA_Criterion& obj_;

		// Attributes to backup/restore
		ACS_USA_SpecificProblem specificProblem_;
		String                  perceivedSeverity_; 
		String					probableCause_;
		String                  problemText_;
	};

  bool firstMatch (string record, Regexp_MatchType& cas);
  // Description
  // Perform a preliminary match based on:
  // Process Name, Severity code, problem type.

  int toInt(const String& str,
	    const int min,
	    const int max) const;
  // Description:
  // 	Converts String to integer
  // Parameters: 
  //   str   a string object to convert
  //	 min   minimum allowed integer value
  //	 max	 maximum allowed integer value
  // Return value: 
  //	 invalidInteger   if conversion failed or not within min and max
  //	 int			        integer value   
  // Additional information:
  //

  ACS_USA_SpecificProblem toInt(const String& str,
				const ACS_USA_SpecificProblem min,
				const ACS_USA_SpecificProblem max) const;
  // Description:
  // 	Converts String to ACS_USA_SpecificProblem
  // Parameters: 
  //   str   a string object to convert
  //	 min	 minimum allowed integer value
  //	 max	 maximum allowed integer value
  // Return value: 
  //	 invalidLong   if conversion failed or not within min and max
  //	 int		    	 integer value   
  // Additional information:
  //

  ACS_USA_StatusType setScope(String& str);
  // Description:
  //    Converts the str to appropriate type and sets the scope in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_StatusType setMaxAllowed(const String& str);
  // Description:
  //    Converts the str to appropriate type and sets the maximum in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_StatusType setMatchType(String& str);
  // Description:
  //    Converts the str to appropriate type and sets the type in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok
  
  ACS_USA_StatusType setPerceivedSeverity(String& str);
  // Description:
  //    Converts the str to appropriate type and sets the severity in the object.
  // Parameters:
  //    str   reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_StatusType setGeneralErrorFilter(String& str);
  // Description:
  //    Converts the str to appropriate type and sets the generalErrorFilter in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok


  ACS_USA_StatusType setCeaseDuration(String& str);
  // Description:
  //    Converts the str to appropriate type and sets the cease duration in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_Boolean isAutoCeaseEvent() const;
  // Description:
  //    Verify if this is an auto cease event.
  // Parameters:
  //    none
  // Return value:
  //    ACS_USA_False		not an auto cease event
  //	ACS_USA_True		an auto cease event

  bool getManualCeaseData(LPBYTE dataBuffer, DWORD  dataLength) const;
  // Description:
  //    Get the maunal cease data from the buffer.
  // Parameters:
  //	dataBuffer		buffer containing event-specific data
  //	dataLength		size of the buffer, in bytes
  // Return value:
  //    Criterion_matchError    Something went wrong
  //    ACS_USA_False		not manual cease
  //	ACS_USA_True		a manual cease


  //
  // Parameters for criterion.
  //
  String               object;             // Analysis Object reference.
  ACS_USA_TimeType        scope;              // Time scope for max allowed matches.
  ACS_USA_Occurrences     maxAllowed;         // Max allowed matches within time scope.
  ACS_USA_MatchType       matchType;          // Pattern match type.
  String               pattern;            // Regular expression for matching events.
  ACS_USA_SpecificProblem specificProblem;    // Specific problem in AP event reports.
  String               perceivedSeverity;  // Severity of event - e.g. 'A1'.
  String               probableCause;      // Alarm printout slogan.
  String               objectOfRefSuffix;  // Text making identical criterias different.
  String               problemText;        // AP event report problem text field.
  String               command;            // Declared command to be executed.
  String               node;               // Alarm raised in active or passive node.
  String               disableAlarmFilter; // Filter for alarm/cease combinations.
  String               generalErrorFilter; // Filter for General Error rules.
  ACS_USA_TimeType     ceaseDuration ;     // Time after which an auto cease event is sent.

  //
  // These are not in ACF but stored in the Temporary storage
  //
  ACS_USA_TimeType    lastAlarm;	  // last time an alarm was generated
  ACS_USA_TimeType	  counterShort;	// last time counter was reset long
  ACS_USA_TimeType	  counterLong;	// last time counter was reset short
  ACS_USA_Occurrences soFarShort;   // occurrences so far short term
  ACS_USA_Occurrences soFarLong;    // occurrences so far long term
  ACS_USA_Flag   		  subsequent;   // last record was substring matched
  String				    subString;	  // substring

  //
  // Parameters for object instance
  //
  
  vector<ACS_USA_TimeType>  encountered;    // Last time an alarm condition was met.
  vector<String>            matchedRecord;  // The matched record

  typedef vector<String> ACS_USA_InsertedStrings;
  vector<ACS_USA_InsertedStrings> rcInsertedStrings; // Inserted strings of the event record

  vector<bool>	manualCeaseData;	// Manual cease info retrieved from record info.

  ACS_USA_Regexp            regExp;         // Regular expression object
  bool                      hasRaisedAlarm; // Needed for raising alarms  
											// when zero occurrences is 
                                            // allowed of an event
                                            // uabdmt
};

//******************************************************************************
// setObject()
//******************************************************************************
inline
void
ACS_USA_Criterion::setObject(const String& str)
{
    object = str;
}

//******************************************************************************
//  setPattern()
//******************************************************************************
inline
void
ACS_USA_Criterion::setPattern(const String& str)
{
    pattern = str;
}

//******************************************************************************
// setObjectOfRefSuffix()
//******************************************************************************
inline
void
ACS_USA_Criterion::setObjectOfRefSuffix(const String& str)
{
   objectOfRefSuffix = str;
}

//******************************************************************************
// setProblemText()
//******************************************************************************
inline
void
ACS_USA_Criterion::setProblemText(const String& str)
{
    problemText = str;
}

//******************************************************************************
// setCommand()
//******************************************************************************
inline
void
ACS_USA_Criterion::setCommand(const String& str)
{
    command = str;
}

//******************************************************************************
// setNode()
//******************************************************************************
inline
void
ACS_USA_Criterion::setNode(const String& str)
{
   node = str;
}

//******************************************************************************
// setDisableAlarmFilter()
//******************************************************************************
inline
void ACS_USA_Criterion::setDisableAlarmFilter(const String& str)
{
	disableAlarmFilter = str;
}


//******************************************************************************
// getObject()
//******************************************************************************
inline
String&
ACS_USA_Criterion::getObject()
{
    return object;
}

inline
String&
ACS_USA_Criterion::getPattern()
{
    return pattern;
}

//******************************************************************************
// getSpecificProblem()
//******************************************************************************
inline
ACS_USA_SpecificProblem
ACS_USA_Criterion::getSpecificProblem()
{
    return specificProblem;
}

//******************************************************************************
// getCommand()
//******************************************************************************
inline
String&
ACS_USA_Criterion::getCommand()
{
    return command;
}

//******************************************************************************
// getGeneralErrorFilter()
//******************************************************************************
inline
String&
ACS_USA_Criterion::getGeneralErrorFilter()
{
    return generalErrorFilter;
}

//******************************************************************************
// isAutoCeaseEvent()
//******************************************************************************
inline
ACS_USA_Boolean 
ACS_USA_Criterion::isAutoCeaseEvent() const
{
    if ( ceaseDuration  > initialTime ) 
    {
        // ceaseDuration > 0, the event will be auto ceased after that time.
        return ACS_USA_True;
    } 
    
    return ACS_USA_False;   
}


#endif
