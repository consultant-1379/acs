//******************************************************************************
//
//******************************************************************************

#ifndef ACS_USA_ANALYSER_H 
#define ACS_USA_ANALYSER_H

#include <cstring>
#include "acs_usa_control.h"
#include "acs_usa_time.h"
#include "acs_usa_types.h"
#include "acs_usa_criterion.h"
#include "acs_usa_common.h"

#define GET_EVNT_LIST_CMD "/opt/ap/acs/bin/acs_usa_genEvtLst "

class ACS_USA_RecordInfo;

//typedef string String;

typedef enum timePrecs
{
	SECONDS1 = 0,
	SECONDS2,
	MINUTS1,
	MINUTS2,
	HOURS1,
	HOURS2
}timePrecs_t;



//******************************************************************************
// Type definitions local to this class
//******************************************************************************
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
  

ACS_USA_ReturnType analyseObject(
				ACS_USA_AnalysisObject* object,
				ACS_USA_TimeType& latestBootTimeStamp,
				ACS_USA_TimeType alarmFilterInterval,
				const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
				const size_t recordSize,
				unsigned int syslogRecordsMax,
				ACS_USA_AnalysisType activation);
		  
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
ACS_USA_MatchType analyseRecord(
				ACS_USA_AnalysisObject* object,
				ACS_USA_TimeType alarmFilterInterval,
				const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
				ACS_USA_AnalysisType activation,
				ACS_USA_RecordBuffer *record);
  
ACS_USA_Boolean getAlarmSetOnMissedBootEvent() const;
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


ACS_USA_RecordStatus getRecord( FILE *fp );
  // Description:
  // 	Retrieves a record from an open log file. Copies characters to record
  //	buffer until end of line character is reached.
  // Parameters: 
  //    log				log name 
  // Return value: 
  //    recordError 	An error occurred during retrieval
  //	recordOK		Record ok
  //	recordEOF		End of file reached


ACS_USA_ReturnType findBootTime(ACS_USA_TimeType& latestBootTimeStamp, bool& isRebootLatest);
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
  ACS_USA_ReturnType createLogFile(	ACS_USA_TimeType timeSec, 
		  			const char *tempLogFile, 
					ACS_USA_AnalysisType activation);	


  ACS_USA_ReturnType getSysBootTime(ACS_USA_TimeType& bootTime);
  
  ACS_USA_ReturnType createTempLogFile(char *timeFilter, ACS_USA_AnalysisType activation);

  ACS_USA_ReturnType getTimeFilter(ACS_USA_TimeType timeSec, char *timeFilter, int Precessin);

  ACS_USA_ReturnType getRecordTimeinSeconds(const char *record, ACS_USA_TimeType& recordTime);

  bool isTimeFilterMatched(char *timeBuf, const char *tempLogFile, ACS_USA_AnalysisType activation);
  ACS_USA_ReturnType findTimeFilter(struct tm *pstTime,  const char *tempLogFile, const int Precessin, ACS_USA_AnalysisType activation);
  bool checkSecsTimeFilterMatched(timePrecs_t type, struct tm *pstTime, const char *tempLogFile, ACS_USA_AnalysisType activation);
  bool checkMinsTimeFilterMatched(timePrecs_t type, struct tm *pstTime, const char *tempLogFile, ACS_USA_AnalysisType activation);
  bool checkHoursTimeFilterMatched(timePrecs_t type, struct tm *pstTime, const char *tempLogFile, ACS_USA_AnalysisType activation);

  size_t	recordBufferSize;	// Size of record buffer
  size_t	logIndex;			// Current index
  ACS_USA_RecordBuffer		*recordBuffer;		// Buffer for records

  ACS_USA_Boolean		sleepAwhile;
									// log has been cleared.
  ACS_USA_Boolean		clearSecLogPerformed;	// States if the 'Security log has 
									// been cleared'-event has been found.
  ACS_USA_Boolean		bootTimeWithinReach;	// For checking if there are more 
									// events to analyse than feasible in 
									// first analysis.
  ACS_USA_Boolean		alarmSetOnMissedBootEvent;	// States if alarm has been raised  
										// when boot event is out of reach 
										// for analysis.
  DWORD		oldestRecordNoInAnalysis;	// Record numbers for checking if  
  DWORD		currentRecordNoInAnalysis;	// the event frequency is too high.
  DWORD		oldestRecordTimeInAnalysis;	// Record time stamps for checking
  DWORD		currentRecordTimeInAnalysis;// if the event frequency is too high.

  short m_nNumOfParams;                            // Number of parameter strings in event.
  unsigned int m_syslogRecordsMax;
};

//******************************************************************************
//	getAlarmSetOnMissedBootEvent
//******************************************************************************
inline
ACS_USA_Boolean 
ACS_USA_Analyser::getAlarmSetOnMissedBootEvent() const
{	
	// Used in the 'too high event frequency'- case.
    return alarmSetOnMissedBootEvent;
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

