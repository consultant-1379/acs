//******************************************************************************
//
// NAME
//      acs_usa_criterion.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//     code for usa service.
//
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//                      2015-02-03      XNAZBEG         syslog alarm handling
//                                                      for TR HT42889 fix
//                      2018-03-16      XFURULL         Fix for TR HW69050 
//                      2018-06-01	XNAZBEG		Cleanup of changes done for HT42889
//******************************************************************************

#ifndef ACS_USA_CRITERION_H
#define ACS_USA_CRITERION_H

#include <string>
#include <vector>

#include "acs_usa_control.h"
#include "acs_usa_types.h"
#include "acs_usa_regexp.h"
#include "acs_usa_eventInfo.h"

using namespace std;
//typedef string String;

#define DISABLE_ALARM_FILTER "YES"
//******************************************************************************
// Type definitions local to this class
//******************************************************************************
typedef int	  ACS_USA_MatchType; 		// Type returned from matching routine
typedef int	  ACS_USA_Occurrences;
typedef int       ACS_USA_PatternMatch;

//******************************************************************************
// Constants used in this class
//******************************************************************************
const	ACS_USA_PatternMatch unknowknownMatch   = 0;
const	ACS_USA_PatternMatch subStringMatch 	= 1;
const	ACS_USA_PatternMatch patternMatch   	= 2;

const	ACS_USA_MatchType Criterion_matchError = 0; // Error occurred
const   ACS_USA_MatchType Criterion_noMatch    = 1; // Didn't match
const	ACS_USA_MatchType Criterion_match      = 2; // matched the criterion

const	int	minScope	= 1;		// min scope
const	int	maxScope	= 10000;	// max scope

const	int minCeaseDuration = 1;      // min cease duration
const	int maxCeaseDuration = 10000;    // max cease duration

// Keywords for analysis type
const       char* const     Key_substringMatch      = "SUBSTRING";
const   char* const     Key_paternMatch         = "PATTERN";

const char slashChar = '/';

const char* const objectOfRefPrefix = "LOGFILE/";

// Various constants
const	int	minEventsAllowed = 0;		// min events allowed
const	int	maxEventsAllowed = 100000; 	// max events allowed
const	long 	invalidLong    	= -1L;
const 	int 	invalidInteger	= -1;

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


class ACS_USA_Criterion {

	friend class ACS_USA_Config;

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

        ACS_USA_Criterion& operator=(const ACS_USA_Criterion& other);
        // Description:
        //    Assignment operator. Initialises the object with data from other object
        // Parameters:
        //    other                   another ACS_USA_Criterion object
        // Return value:
        //   Reference to self


  virtual ~ACS_USA_Criterion();
  // Description:
  //     Destructor. Releases all allocated memory
  // Parameters:
  //    none
  // Return value:
  //    none

  ACS_USA_ReturnType validate();
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
			  const ACS_USA_ActivationType act,
			  const vector<String>& insertedStrings,
			  LPSTR dataBuffer,
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


  ACS_USA_ReturnType raiseAlarm(
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
  
  ACS_USA_ReturnType raiseCustomAlarm(
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

  ACS_USA_ReturnType read(File& file);
  
  // Description:
  //    Reads some data from file..
  // Parameters:
  //    file			reference to a RWFile object to read from, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok
  
  ACS_USA_ReturnType write(File& file);
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
  
  ACS_USA_ReturnType setSpecificProblem(ACS_USA_SpecificProblem value);
  // Description:
  //    Sets the value of specific problem.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_ReturnType setProbableCause(String& str);
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
  ACS_USA_ReturnType reset();
  ACS_USA_ReturnType clearInstAlarmsQue(ACS_USA_TimeType lastShortRecordTime);

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

  bool isObjectMatched (string &record);
  // Description:
  // 	Finds the matching Object from the record.
  // Parameters: 
  //   record  a string object to fetch the object name, in
  // Return value:
  //    ACS_USA_False		Object not matched
  //	ACS_USA_True		Object matched

  
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

  ACS_USA_ReturnType setScope(ACS_USA_TimeType value);
  // Description:
  //    Converts the str to appropriate type and sets the scope in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_ReturnType setMaxAllowed(ACS_USA_Occurrences value);
  // Description:
  //    Converts the str to appropriate type and sets the maximum in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_ReturnType setMatchType(String& str);
  // Description:
  //    Converts the str to appropriate type and sets the type in the object.
  // Parameters:
  //    str			reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok
  
  ACS_USA_ReturnType setPerceivedSeverity(String& str);
  // Description:
  //    Converts the str to appropriate type and sets the severity in the object.
  // Parameters:
  //    str   reference to a String object, in
  // Return value:
  //    ACS_USA_error           Something went wrong
  //    ACS_USA_ok              Everything ok

  ACS_USA_ReturnType setCeaseDuration(ACS_USA_TimeType value);
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

   bool getManualCeaseData(LPSTR dataBuffer, DWORD  dataLength) const;
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
  String		object;             // Analysis Object reference.
  ACS_USA_TimeType	scope;              // Time scope for max allowed matches.
  ACS_USA_Occurrences	maxAllowed;         // Max allowed matches within time scope.
  ACS_USA_MatchType	matchType;          // Pattern match type.
  String		pattern;            // Regular expression for matching events.
  ACS_USA_SpecificProblem specificProblem;  // Specific problem in AP event reports.
  String 		perceivedSeverity;  // Severity of event - e.g. 'A1'.
  String            	probableCause;      // Alarm printout slogan.
  String             	objectOfRefSuffix;  // Text making identical criterias different.
  String             	problemText;        // AP event report problem text field.
  String             	command;            // Declared command to be executed.
  String              	node;               // Alarm raised in active or passive node.
  String              	disableAlarmFilter; // Filter for alarm/cease combinations.
  ACS_USA_TimeType    	ceaseDuration;      // Time after which an auto cease event is sent.

  //
  // These are not in ACF but stored in the Temporary storage
  //
  ACS_USA_TimeType	lastAlarm;	  // last time an alarm was generated
  ACS_USA_TimeType	counterShort;	// last time counter was reset long
  ACS_USA_TimeType      counterLong;
  ACS_USA_TimeType      counterInst;
  ACS_USA_Occurrences 	soFarShort;   // occurrences so far short term
  ACS_USA_Occurrences   soFarLong;
  ACS_USA_Occurrences   soFarInst;
  ACS_USA_Flag		subsequent;   // last record was substring matched
  String		subString;	  // substring

  //
  // Parameters for object instance
  //
  
  vector<ACS_USA_TimeType>  encountered;    // Last time an alarm condition was met.
  vector<String>            matchedRecord;  // The matched record
  vector<String>            userNameQue;  // The matched record

  typedef vector<String> ACS_USA_InsertedStrings;
  vector<ACS_USA_InsertedStrings> rcInsertedStrings; // Inserted strings of the event record

  vector<bool>	manualCeaseData;	// Manual cease info retrieved from record info.

  ACS_USA_Regexp            regExp;         // Regular expression object
  bool                      hasRaisedAlarm; // Needed for raising alarms  
					    // when zero occurrences is 
                                            // allowed of an event
                                            // uabdmt
  String		userName;	  
  typedef std::map<std::string, ACS_USA_TimeType> ACS_USA_UserTimeMap;
  typedef std::pair<std::string, ACS_USA_TimeType> ACS_USA_UserTimePair;

  ACS_USA_UserTimeMap userRecordList;
  vector<ACS_USA_TimeType>  instAlarmTimeStamp;    //Instant matched record time stamps 
  unsigned int nAlarmFilterInterval;
  bool manualCeaseAlarm;
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
String& ACS_USA_Criterion::getObject()
{
    return object;
}

inline
String& ACS_USA_Criterion::getPattern()
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

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------


