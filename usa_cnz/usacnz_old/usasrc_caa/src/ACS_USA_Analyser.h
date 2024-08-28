//******************************************************************************
//
// NAME
//  	  ACS_USA_Analyser - Analyses system log files
//
//  COPYRIGHT
//      Ericsson AB 2005-2006 - All rights reserved
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
// 	    ACS_USA_Analyser integrates all steps that are needed for 
//	    analysis of a log file. These steps include the following:
//		  - opening/closing of log files,
//		  - retrieving records,
//		  - extracting time information from records,
//		  - matching against criteria (regular expressions and time
//			  boundaries).
//	    Most of these tasks are implemented by using other classes that
//	    ACS_USA_Analyser manipulates. 
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//	    190 89-CAA 109 0545
//
// AUTHOR 
// 	    1995-06-19 by ETX/TX/T XKKHEIN
//
// REV  DATE    NAME     DESCRIPTION
// A		950829	XKKHEIN	 First version.
// B		990915	UABDMT	 Ported to Windows NT4.0.
// C		040429	QVINKAL	 Removal of RougeWave Tools.h++.
// D		041223	UABPEK	 Supervision of three new event logs.
// E		060426	UABPEK	 Suppression of alarms by named PHA parameter table.
//                       Use of event source DLL and EXE files.
// F		080318	EKAMSBA	 PRC alarms handling.
//
//******************************************************************************

#ifndef ACS_USA_ANALYSER_H 
#define ACS_USA_ANALYSER_H

#include "ACS_USA_Control.h"
#include "ACS_USA_EventSourceList.h"
#include "ACS_USA_Time.h"

class ACS_USA_RecordInfo;

typedef string String;


//******************************************************************************
// Type definitions local to this class
//******************************************************************************
typedef	char						ACS_USA_LogBuffer;			// Log file buffering
typedef	char						ACS_USA_RecordBuffer;		// Record buffering
typedef	vector<ACS_USA_RecordInfo*>	ACS_USA_RecordInfoBuffer;	// Record info buffering
typedef	int	ACS_USA_RecordStatus;			// Status of record
typedef int	Counter;						// Counter



//******************************************************************************
// Constants used in this class
//******************************************************************************
const	ACS_USA_RecordStatus	recordError = 0; // error
const	ACS_USA_RecordStatus	recordOK    = 1; // record ok
const	ACS_USA_RecordStatus	recordEOF   = 2; // EOF reached

const	ACS_USA_RecordStatus	recordOKconditional   = 3; // record ok

const   Counter maxErrors 		 = 2;		// Max errors

#define EVENT_MESSAGE_BUFFER_SIZE 4096  // Maximum length of event parameter strings.
#define MAX_NUM_OF_PARAMS           64  // Maximum number of event parameter strings.
#define PARAM_LEN                 2048  // Maximum length of event parameter string.

//******************************************************************************
// Forward declaration(s)
//******************************************************************************
class ACS_USA_AnalysisObject;
class ACS_USA_ObjectManagerCollection;

//******************************************************************************
//Member functions, constructors, destructors, operators
//******************************************************************************

class  ACS_USA_Analyser {
public:
  ACS_USA_Analyser();
  // Description:
  // 	Constructor.
  // Parameters: 
  //    none 
  // Return value: 
  //    none
  
  virtual ~ACS_USA_Analyser();
  // Description:
  // 	Destructor.
  // Parameters: 
  //    none 
  // Return value: 
  //    none
  
  ACS_USA_StatusType analyse(
						const ACS_USA_ObjectManagerCollection& managers,
						String ACS_USA_alarmSourceNameDefault,
						String triggedLogName,
						DWORD& latestBootTimeStamp,
						String bootEventRegExp,
						String clearLogRegExp,
						DWORD alarmFilterInterval,
						const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
						const DWORD latestRecordNumbers[6],
						const DWORD recordsPrioThreshold,
						const DWORD secondsPrioThreshold,
						const size_t logSize,
						const size_t recordSize,
						const ACS_USA_ActivationType activation,
						off_t offset,
						String *tmpStorage);
  // Description:
  // 	 Analyses all Analysis Objects. 
  // Parameters: 
  //   managers			                   list of Object Managers
  //	 ACS_USA_alarmSourceNameDefault  Source name of AEH events
  //	 triggedLogName						       Name of log just updated
  //	 latestBootTimeStamp					   Boot time stamp
  //	 bootEventRegExp						     Boot reg exp
  //	 clearLogRegExp				           Sec log cleared reg exp
  //	 alarmFilterInterval             Min interval between alarms on the same error.
  //   suppressedAlarms			           Criteria that should not raise alarm.
  //	 logSize			                   How mach data to analyse
  //	 recordSize		                   Number of bytes to allocate for record buffer
  //   activation		                   Long or short term activation 
  //   offset			                     Where to move file pointer before start
  //	 tmpStorage		                   Name of temporary storage
  // Return value: 
  //   ACS_USA_error  An error occurred during analysis
  //	 ACS_USA_ok		  Analysis OK

BOOL getAlarmSetOnMissedBootEvent() const;
  // Description:
  // 	Returns the boolean indicating if an alarm has been 
  //	set on the 'too high event frequency'-case.
  // Parameters: 
  //
  // Return value: 
  //    TRUE		An alarm has been raised on the 
  //				'too high event frequency'-case.
  //	FALSE		No alarm has been raised on the
  //				'too high event frequency'-case.

private:

  ACS_USA_StatusType analyseObject(
							const ACS_USA_ObjectManagerCollection& managers,
							ACS_USA_AnalysisObject* object,
							String ACS_USA_alarmSourceNameDefault,
							DWORD& latestBootTimeStamp,
							DWORD alarmFilterInterval,
							const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
							const DWORD latestRecordNumbers[6],
							const DWORD recordsPrioThreshold,
							const DWORD	secondsPrioThreshold,
							ACS_USA_ActivationType activation,
							const off_t offset);
  // Description:
  // 	 Analyses object. 
  // Parameters: 
  //   managers			                     list of Object Managers
  //   object			                     Analysis Object to be analysed
  //	 ACS_USA_alarmSourceNameDefault  Source name of AEH events
  //	 latestBootTimeStamp					   Boot time stamp
  //	 alarmFilterInterval             Min interval between alarms on the same error.
  //   suppressedAlarms			           Criteria that should not raise alarm.
  //   activation		                   Long or short term activation 
  //   offset			                     Where to move file pointer 
  // Return value: 
  //   ACS_USA_error  An error occurred during analysis
  //	 ACS_USA_ok		  Analysis OK

ACS_USA_RecordStatus getRecord(DWORD					startRecord,
							   ACS_USA_ActivationType	activation,
							   _EVENTLOGRECORD*			logrec,
							   String				logFileName,
							   String				ACS_USA_alarmSourceNameDefault,
						   	   DWORD&					latestBootTimeStamp,
							   const DWORD				latestRecordNumbers[6],
							   const DWORD				recordsPrioThreshold,
							   const DWORD				secondsPrioThreshold);
  // Description:
  // 	Retrieves a record from an open log file. Copies characters to record
  //	buffer until end of line character is reached.
  // Parameters: 
  //    log				log name 
  // Return value: 
  //    recordError 	An error occurred during retrieval
  //	recordOK		Record ok
  //	recordEOF		End of file reached

 ACS_USA_StatusType seekFromEnd	(HANDLE eventLogHandle, 
								 const off_t offset, 
								 DWORD& startRecord, 
								 ACS_USA_ActivationType  activation);

  // Description:
  // 	Moves opened file pointer to offset relative to the end of log file,
  //	The nearest newline is searched and filepointer is left there. 
  // Parameters: 
  //    offset			Where to move file pointer 
  // Return value: 
  //    ACS_USA_error 	An error occurred during analysis
  //	ACS_USA_ok		Analysis OK

ACS_USA_StatusType getRecordsFromEventlog(HANDLE eventLogHandle, 
										  _EVENTLOGRECORD* logrec,
										  DWORD startRecordNumber,
										  DWORD& bytesRead,
										  String readOrder,
 										  const DWORD latestRecordNumbers[6],
										  String logFileName,
										  const off_t offset,
										  DWORD alarmFilterInterval);
  // Description:
  //	Reads as many events from the NT Event log that there is room for in buffer.
  // Parameters: 
  //	eventLogHandle		Handle to EventLog
  //    logrec				Buffer to fill with event log records
  //	startRecordNumber	Record number to start the reading from
  //	bytesRead			Bytes read from Event log
  //	readOrder			Forward or backwards read
  // Return value: 
  //    ACS_USA_error 		An error occurred
  //	ACS_USA_ok			OK

ACS_USA_StatusType findInsertedStrings(TCHAR pEventStrings[],
									  _EVENTLOGRECORD* nextRecord,
                    ACS_USA_EventSource* pEventSource);
  // Description:
  //	Adds the event-insterted strings to pEventStrings for analysis.
  // Parameters: 
  //	pEventStrings				String to contain the insert strings
  //    nextRecord					Pointer to the event log record to analyse
  //	pEventSource    		Pointer to event source object
  // Return value: 
  //    ACS_USA_error 				A string error occurred
  //	ACS_USA_ok					OK

ACS_USA_StatusType retrieveInsertedStrings(					
					_EVENTLOGRECORD* const pEventRecord,
					ACS_USA_RecordInfo& recordInfo) const;
  // Description:
  //	Save the event-info.
  // Parameters: 
  //    pEventRecord		Pointer to the event log record to analyse
  //	recordInfo			Object receiving info
  // Return value: 
  //    ACS_USA_error 				an error occurred
  //	ACS_USA_ok					OK

ACS_USA_StatusType retrieveEventData(
					_EVENTLOGRECORD* const pEventRecord,
					ACS_USA_RecordInfo& recordInfo) const;
  // Description:
  //	Save the event data.
  // Parameters: 
  //    pEventRecord		Pointer to the event log record to analyse
  //	recordInfo			Object receiving data
  // Return value: 
  //    ACS_USA_error 				an error occurred
  //	ACS_USA_ok					OK

ACS_USA_RecordStatus addEventData(const char* const logonTypeString, 
								  const char* const eventTypeString, 
							      TCHAR pEventStrings[],
								  const WORD numberOfStringsInRecord,
							      char* eventRecordContents);	
  // Description:
  //	Adds the additional event data to the event string.
  // Parameters: 
  //	logonTypeString				String to contain the logon type
  //	eventTypeString				String to contain the event type
  //	pEventStrings				String to contain the insert strings
  //	numberOfStringsInRecord		Number of strings in record
  //    eventRecordContents			The string containing the data of the event
  // Return value: 
  //    recordError 				A string error occurred
  //	recordOK					OK
	

ACS_USA_StatusType findLogonType(char* logonTypeString, 
								const _EVENTLOGRECORD* nextRecord);
  // Description:
  //	Adds the logon type to logonTypeString for analysis.
  // Parameters: 
  //	logonTypeString				String to contain the logon type
  //    nextRecord					Pointer to the event log record to analyse
  // Return value: 
  //    ACS_USA_error 				A string error occurred
  //	ACS_USA_ok					OK

ACS_USA_StatusType findEventType(char* eventTypeString, 
								 const _EVENTLOGRECORD* nextRecord);
  // Description:
  //	Adds the event type to eventTypeString for analysis.
  // Parameters: 
  //	eventTypeString				String to contain the event type
  //    nextRecord					Pointer to the event log record to analyse
  // Return value: 
  //    ACS_USA_error 				A string error occurred
  //	ACS_USA_ok					OK

ACS_USA_StatusType findBootTime (String ACS_USA_alarmSourceNameDefault, 
								 ACS_USA_Time interpreter,
								 DWORD& latestBootTimeStamp,
								 const DWORD latestRecordNumbers[6],
								 const DWORD recordsPrioThreshold,
								 const DWORD secondsPrioThreshold);
  // Description:
  //	Finds the time stamp for the latest boot/restart.
  // Parameters: 
  //	ACS_USA_alarmSourceNameDefault	String to pass on to getRecord()
  //    offset							Record number to start serching from
  //	interpreter						For interpresting the tme stamp
  //	latestBootTimeStamp				Boot time stamp
  // Return value: 
  //    ACS_USA_error 					A string error occurred
  //	ACS_USA_ok						OK

ACS_USA_StatusType findBootTimeInLog (String ACS_USA_alarmSourceNameDefault, 
								 ACS_USA_Time interpreter,
								 DWORD& latestBootTimeStamp,
								 const DWORD latestRecordNumbers[6],
								 const DWORD recordsPrioThreshold,
								 const DWORD secondsPrioThreshold,
								 String	logFileName);
  // Description:
  //	Finds the time stamp for the latest boot/restart in specified log.
  // Parameters: 
  //	ACS_USA_alarmSourceNameDefault	String to pass on to getRecord()
  //    offset							Record number to start serching from
  //	interpreter						For interpresting the tme stamp
  //	latestBootTimeStamp				Boot time stamp
  //	logFileName						Log to be analysed
  // Return value: 
  //    ACS_USA_error 					A string error occurred
  //	ACS_USA_ok						OK

  BOOL ResizeStringArray(const short nNumOfStrings);
  // Description:
  //	 Allocate the additional number strings needed for the parameters.
  // Parameters: 
  //	 nNumOfStrings	      Number of event message strings
  // Return value: 
  //   ACS_USA_error 				A ... error occurred
  //	 ACS_USA_ok						OK

  void FreeRecordInfoBuffer();
  // Description:
  //	 Free allocated record info objects.
  // Parameters: 
  //	 none
  // Return value: 
  //   none

  size_t				logBufferSize;		// Size of record buffer
  size_t				recordBufferSize;	// Size of record buffer
  size_t				bufferEnd;			// Current end of the log buffer
  size_t				logIndex;			// Current index
  ACS_USA_LogBuffer			*logBuffer;			// Buffer for logs
  ACS_USA_RecordBuffer		*recordBuffer;		// Buffer for records
  ACS_USA_RecordInfoBuffer	recordInfoBuffer;   // Buffer for record info object;

  BOOL		sleepAwhile;
  DWORD		bytesRead;				// Number of bytes read from NT Event log.
  String	bootEventRegExpText;	// Reg Exp for finding latest boot event.
  String	clearEventRegExpText;	// Reg Exp for detecting if Security 
									// log has been cleared.
  BOOL		clearSecLogPerformed;	// States if the 'Security log has 
									// been cleared'-event has been found.
  DWORD		tmpCurrentStamp;		// Interim time stamp.
  BOOL		bootTimeWithinReach;	// For checking if there are more 
									// events to analyse than feasible in 
									// first analysis.
  BOOL		alarmSetOnMissedBootEvent;	// States if alarm has been raised  
										// when boot event is out of reach 
										// for analysis.
  DWORD		oldestRecordNoInAnalysis;	// Record numbers for checking if  
  DWORD		currentRecordNoInAnalysis;	// the event frequency is too high.
  DWORD		oldestRecordTimeInAnalysis;	// Record time stamps for checking
  DWORD		currentRecordTimeInAnalysis;// if the event frequency is too high.

  ACS_USA_EventSourceList* m_pEventSourceList[6];  // One event source list per event log.
  short m_nNumOfParams;                            // Number of parameter strings in event.
  _TCHAR* m_lpszParams[MAX_NUM_OF_PARAMS];         // Array of event parameter strings.
  _TCHAR m_szDummyParams[256];                     // Default event parameter strings.
};

//******************************************************************************
//	getAlarmSetOnMissedBootEvent
//******************************************************************************
inline
BOOL 
ACS_USA_Analyser::getAlarmSetOnMissedBootEvent() const
{	
	// Used in the 'too high event frequency'- case.
    return alarmSetOnMissedBootEvent;
}

#endif
