//******************************************************************************
//
// NAME
//      ACS_USA_Analyser.cpp
//
// COPYRIGHT
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
// 	    1995-06-27 by ETX/TX/T XKKHEIN
//
// REV  DATE    NAME     DESCRIPTION
// A    950829  XKKHEIN  First version.
// B    950923  XKKHEIN  LMA API stub.
// C    990915  UABDMT   Ported to Windows NT4.0.
// D    030212  UABPEK   New AP VIRUS alarms.
// E	  040429	QVINKAL	 Removal of RougeWave Tolls.h++.
// F    041223  UABPEK   Supervision of three new event logs.
// G    060426  UABPEK   Suppression of alarms by named PHA parameter table.
//                       Use of event source DLL and EXE files.
// H    080318  EKAMSBA  PRC alarms handling.
//
//******************************************************************************
#pragma warning (disable: 4018)

#include "ACS_USA_Analyser.h"
#include "ACS_USA_EventManager.h"
#include "ACS_USA_ObjectManager.h"
#include "ACS_TRA_Trace.h"
#include "ACS_USA_ObjectManagerCollection.h"
#include <memory>

#define eventTypeStringLength					30
#define logonTypeStringLength					28
#define eventRecordLength						8192
//#define numberOfSecurityEventsInInstantAnalysis	10  // Must never be smaller than 2.



//******************************************************************************
// Helper class local to this file
//******************************************************************************
class ACS_USA_RecordInfo
{
public:
	ACS_USA_RecordInfo( const String& src )
		:sourceName( src ),
		 dataLength( 0 ),
		 dataBuffer( NULL )
	{
	}

	~ACS_USA_RecordInfo()
	{
		try
		{
			insertedStrings.clear(); 
			if( NULL != dataBuffer )
			{
				delete [] dataBuffer;
			}
		}
		catch(...)
		{
		}
	}

	// Add an inserted string to the list
	void addString( const String& str )
	{
		insertedStrings.push_back( str );
	}

	// Save the data.
	bool addData(LPBYTE offSet, DWORD size)
	{	
		// Validation of inputs
		if( offSet == NULL || size == 0 )
		{
			// Nothing to copy
			return true;
		}

		// Try to allocate a buffer
		LPBYTE ptr = new BYTE[size];
		if( NULL == ptr )
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_RecordInfo::addData.");
			return false;
		}
		
		try
		{
			// Free what already is there
			if( NULL != dataBuffer )
			{
				delete [] dataBuffer;
				dataLength = 0;
			}
		}
		catch(...)
		{
			delete [] ptr;

			error.setError(ACS_USA_Memory);
			error.setErrorText("delete() failed in ACS_USA_RecordInfo::addData.");
			return false;
		}
		
		memcpy(ptr, offSet, size);
		dataBuffer = ptr;	
		dataLength = size;		
		return true;
	}


	// Data member
	String			sourceName;			// source name of the record.
	vector<String>  insertedStrings;    // inserted strings of the records.
	DWORD			dataLength;			// size of the event-specific data, in bytes. 
	LPBYTE			dataBuffer;			// Buffer receiving event-specific data. 

private:
	ACS_USA_RecordInfo( const ACS_USA_RecordInfo& );              // copy constructor
	ACS_USA_RecordInfo& operator= ( const ACS_USA_RecordInfo& );  // copy assignment
};



/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//	Trace point definitions
//******************************************************************************
ACS_TRA_trace traceAnalyser = ACS_TRA_DEF("ACS_USA_Analyser", "C40 C100");

ACS_TRA_trace traceAnalyser2 = ACS_TRA_DEF("ACS_USA_Analyser2", "C47I");
ACS_TRA_trace traceAnalyser3 = ACS_TRA_DEF("ACS_USA_Analyser3", "C16 C25");
ACS_TRA_trace traceAnalyser4 = ACS_TRA_DEF("ACS_USA_Analyser4", "C40");

const size_t maxObjectLen = 99;
const char* const ACS_USA_traceObject	= "Analysing object: ";

const char* const ACS_USA_traceReadEventLog	= "Events read from Eventlog, start event number: ";
//
// USA only analyses the events from the latest boot, to avoid failing over 
// several times between the nodes upon several fail over reqeusts.
//
const char* const ACS_USA_traceFindBootTime	= "Latest BOOT time";
const char* const ACS_USA_tracePrioLow			= "Decreasing process priority";


//******************************************************************************
//	ACS_USA_Analyser()
//******************************************************************************
ACS_USA_Analyser::ACS_USA_Analyser() :
logBufferSize(0),
recordBufferSize(0),
bufferEnd(0),
logIndex(0),
logBuffer(0),
bootTimeWithinReach(FALSE),
alarmSetOnMissedBootEvent(FALSE),
clearSecLogPerformed(FALSE),
sleepAwhile(FALSE),
oldestRecordNoInAnalysis(0),
currentRecordNoInAnalysis(0),
oldestRecordTimeInAnalysis(0),
currentRecordTimeInAnalysis(0),
recordBuffer(0),
m_nNumOfParams(0)
{
  // Create one event source list object per event log.
  m_pEventSourceList[0] = new ACS_USA_EventSourceList("System");
	m_pEventSourceList[1] = new ACS_USA_EventSourceList("Application");
	m_pEventSourceList[2] = new ACS_USA_EventSourceList("Security");
	m_pEventSourceList[3] = new ACS_USA_EventSourceList("Directory Service");
	m_pEventSourceList[4] = new ACS_USA_EventSourceList("DNS Server");
	m_pEventSourceList[5] = new ACS_USA_EventSourceList("File Replication Service");
 
  // Initialise default parameter format strings.
  // Format: %1(NULL)%2(NULL)%3(NULL)...
  for (int nIdx = 0; nIdx < MAX_NUM_OF_PARAMS; nIdx++) {
    m_lpszParams[nIdx] = &m_szDummyParams[nIdx * 4];
    if (m_lpszParams[nIdx])
      _stprintf(m_lpszParams[nIdx], _T("%%%d"), nIdx+1);
  }
}

//******************************************************************************
//	~ACS_USA_Analyser()
//******************************************************************************
ACS_USA_Analyser::~ACS_USA_Analyser()
{
  // Deallocate parameter format strings.
  for (int nIdx = 0; nIdx < m_nNumOfParams; nIdx++) {
    if (m_lpszParams[nIdx] && m_lpszParams[nIdx] != &m_szDummyParams[nIdx * 4])
		try	
		{
      ::HeapFree(::GetProcessHeap(), 0, m_lpszParams[nIdx]);
  }
		catch(...)
		{
	    continue;
	  }    
  }

  // Unload all event source DLLs and delete event source list objects.
  // TR HI60745 : EventSourceList "File Replication Service" not deleted
  for (int nIdx = 0; nIdx < 6; nIdx++)
    if (m_pEventSourceList[nIdx]) {
      m_pEventSourceList[nIdx]->UnloadEventSources();
      delete m_pEventSourceList[nIdx];
    }
}

//******************************************************************************
//	analyse()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Analyser::analyse(
						  const ACS_USA_ObjectManagerCollection& managers,
  String ACS_USA_alarmSourceNameDefault,
	String triggedLogName,
	DWORD& latestBootTimeStamp,
	String bootRexp,
	String clearLogRexp,
	DWORD alarmFilterInterval,
  const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
	const DWORD latestRecordNumbers[6],
	const DWORD recordsPrioThreshold,
	const DWORD secondsPrioThreshold,
	const size_t logSize,
	const size_t recordSize,
	const ACS_USA_ActivationType activation,
	off_t offset,
	String *tmpStorage)
{
    vector<ACS_USA_ObjectManager*> aManagers = managers.getManagers();
	if( aManagers.empty() ||
		NULL == aManagers.front()
	)
	{
		error.setError(ACS_USA_BadState);				
		error.setErrorText("ACS_USA_Analyser::analyse(), No object manager in the list"); 
		return ACS_USA_error;
	}

	// ALWAYS Consider using the default MANAGER.
	// i.e. corresponding to the main ACF file.
    ACS_USA_ObjectManager* manager = aManagers.front();

  logBufferSize = logSize;
  recordBufferSize = recordSize;
  ACS_USA_StatusType rc = ACS_USA_ok;
    
    ACS_USA_AnalysisObject *currObject = manager->getFirst();
    int no = manager->getObjectCount();

	bootEventRegExpText		= bootRexp;			// Set bootEventRegExpText and
	clearEventRegExpText	= clearLogRexp;	// clearEventRegExpText to  
													              // regexp received from ACS_USA_Control::operate().

    if (activation == Activation_startUp)   
    {
    // Start up analysis
		offset = 0;
		if (analyseObject(
                        managers,
      currObject,
			ACS_USA_alarmSourceNameDefault, 
			latestBootTimeStamp,
			alarmFilterInterval,
      suppressedAlarms,
			latestRecordNumbers,
			recordsPrioThreshold,
			secondsPrioThreshold,
			activation, 
                        offset) == ACS_USA_error) 
        {     
        return ACS_USA_error;
    }
    return ACS_USA_ok;
  }

	// Create boot time stamp for trace point, written in long term,
  // short term, and instant analysis. Not in start-up analysis.
    if (ACS_TRA_ON(traceAnalyser3)) 
    {
		tm* timeStructPtr	= 0;    
		timeStructPtr	= localtime((const time_t*)	// For daylight saving time.
										&latestBootTimeStamp);
		char* tmpTimestring = asctime(timeStructPtr);
		int len	= strlen(tmpTimestring);
		char bootTimeString[256] = ""; // For the date.
		strncpy(bootTimeString, tmpTimestring, len-1);
		bootTimeString [len-1] = ' '; // 25th pos is 24th element.
		bootTimeString [len] = '\0'; // End string with '\0'.
		ACS_TRA_event(&traceAnalyser3, ACS_USA_traceFindBootTime, bootTimeString);
	}

  // Analyse All objects stored in Object Manager.
    for (int i = 0; i < no; i++) 
    {
        if (activation == Activation_instantShortTerm) 
        {
/*		
		// NOT USED ANYMORE, 991223:
		//
		// If the object is not from the log that the instant analysis trigged on,
		// the object is to be skipped. Thus, only one of the three event logs will
		// be read in instant analysis - the one which the new event was
		// written in.

 		if ((currObject->getLogFile()).compareTo(triggedLogName, String::ignoreCase) == 0)
		{
			// Ok, do nothing. The event is to be analysed   
			// since it is the one trigging the analysis.

			if ((currObject->getLogFile()).compareTo("Security", String::ignoreCase) == 0)
			{
				if (offset == 0)
				{	
					// Magic number, how many events are to be read in the Security log?
					// Needed to be able to find patterns (several events indicating
					// a security violation attempt) in the Security log.

					offset = numberOfSecurityEventsInInstantAnalysis;  
				}				
			}
	 	}
		else // Skip the event. It is not to be analysed in this instant analysis.
		{  
				currObject = manager->getNext();
			continue;
		} 
*/
    }

        if (ACS_TRA_ON(traceAnalyser)) 
        {
	    String b(currObject->getLogFile());
            if (b.length() > maxObjectLen) 
            {
			  b= b.substr(0,maxObjectLen);
		  }
      ACS_TRA_event(&traceAnalyser, ACS_USA_traceObject, b.data());
    }

		if (analyseObject(
					managers,
      currObject,
		  ACS_USA_alarmSourceNameDefault,
		  latestBootTimeStamp,
		  alarmFilterInterval,
      suppressedAlarms,
		  latestRecordNumbers,
		  recordsPrioThreshold,
		  secondsPrioThreshold,
		  activation, 
					offset) == ACS_USA_error) 
		{
        ACS_USA_ErrorType err = error.getError();
        if (err == ACS_USA_APeventHandlingError ||
          err == ACS_USA_QueueExceeded ||
				err == ACS_USA_Memory) 
			{
            ;
        }
			else if (err == ACS_USA_FileIOError) 
			{
          String a(currObject->getLogFile());
          a += ioErrorPrefix;
          error.setErrorText(a.data(), error.getErrorText());
        }
			else 
			{
		      String a(currObject->getLogFile());
		      error.setErrorText(a.data(), error.getErrorText());
        }
        rc = ACS_USA_error;
	      break;
    }
		currObject = manager->getNext();
  }
  return rc;    
}


//******************************************************************************
//	analyseObject()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Analyser::analyseObject(
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
  const off_t offset)
{
  DWORD startRecord	= 0;
  bufferEnd	= logBufferSize;
  logIndex	= bufferEnd;

  // Initialise time stamp intepreter.
  String &position		= object->getPosition();
  String &format		= object->getFormat();
  ACS_USA_Time interpreter;
    
	if (position.empty()) {
		ACS_USA_Time t(format);	// Format only.
		interpreter	= t;
  } 
	else {
		// Initialise with time stamp position
		ACS_USA_Time t(format, position);
		interpreter = t;
  }
  if (interpreter.isValid() != ACS_USA_ok) {
		// Error set by time class.
		return ACS_USA_error;
  }
  USA_DEBUG(logMsg("Analyser::analyseObject: iterpreter ok\n"));
  if (activation == Activation_startUp) {
		return findBootTime(
      ACS_USA_alarmSourceNameDefault,
      interpreter,
			latestBootTimeStamp,
      latestRecordNumbers,
			recordsPrioThreshold,
      secondsPrioThreshold);
	}

  //---------- Open log file -------------
  // WIN32 is used for reading events from NT's Eventlog.
  // The log name is read from ACF
	String &logFileName = object->getLogFile();
	
  // Open the log (Application, Security, System, Directory Service, DNS Server, or File Replication Service).
	char computerNameBuffer [MAX_COMPUTERNAME_LENGTH + 1];
	DWORD	computerNameLength = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName((char*)&computerNameBuffer, &computerNameLength);    
	HANDLE eventLogHandle = OpenEventLog(computerNameBuffer, logFileName.data());
	
  if (eventLogHandle == NULL) {       
		char* lpMsgBuf;
		FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_MAX_WIDTH_MASK |	// buffersize(255) - ignores inserted 
													              // control sequences such as char 13.
			  FORMAT_MESSAGE_FROM_SYSTEM |
			  FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
      GetLastError(),
      0, 
			(LPTSTR)&lpMsgBuf,
      0,
      NULL);
		char errorString[1024] = "System call OpenEventLog() failed. Machine name: ";
		strcat(errorString, computerNameBuffer);
		strcat(errorString, ". Error from system: ");	
		strncat(
      errorString,
      lpMsgBuf, 
			strlen(lpMsgBuf)); // Ends with a '\0'.
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf); // Necessary according to the 
												 // FormatMessage() manual page.
		return ACS_USA_error;
  }   
    
  //---------- Allocate buffers -----------
	// logBuffer will hold the formatted (sprintf:ed) record  
	// strings read from the sizeBufPtr buffer defined below. 
	//
	try {
    logBuffer = new ACS_USA_LogBuffer[logBufferSize];	// Default from parameter file: 2.3 MB.
	}
  catch(...) { // Catches "bad_alloc" exception (generated if stl headers are used).
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
		return ACS_USA_error;	
	}
  if (logBuffer == NULL) { // If catching "bad_alloc" wasn't enough.
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
		return ACS_USA_error;	
	}

	// recordBuffer will hold one formatted record string at a time for analysis.
	try {
		recordBuffer = new ACS_USA_RecordBuffer[recordBufferSize];
	}
  catch(...) { // Catches "bad_alloc" exception 
							 // (generated if stl headers are used)
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
		delete [] logBuffer;
		return ACS_USA_error;
	}
  if (recordBuffer == NULL) {	// If catching "bad_alloc" wasn't enough.
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
		delete [] logBuffer;
		return ACS_USA_error;	
	}
 
	// clear recordInfoBuffer.
	FreeRecordInfoBuffer();

	// Find out where to start the short or long term analysis.
	if (seekFromEnd(
    eventLogHandle,
    offset, 
		startRecord,
    activation) == ACS_USA_error)
  {                   
		delete [] logBuffer;
		delete [] recordBuffer;
		return ACS_USA_error;
	}	
    
  //-------- Get records from log file and perform analysis --------
  ACS_USA_TimeType stamp = ACS_USA_Time_error;
  ACS_USA_TimeType lastStamp = initialTime;
  ACS_USA_StatusType ret = ACS_USA_error;
  ACS_USA_Flag notFinish = ACS_USA_True;

	// The buffer to put the events read from   
	// the event log in (sizeBuf/logRec) shall 
	// have space for the max size of the
	// logBufferSize/logParam set as PHA parameter.
	LPBYTE sizeBufPtr;
	try {
		sizeBufPtr = new BYTE [logBufferSize]; // Default 2.3 MB.
	}
  catch(...) { // Catches "bad_alloc" exception (generated if stl headers are used).
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
		delete [] logBuffer;
		delete [] recordBuffer;
		return ACS_USA_error;	
	}
  if (sizeBufPtr == NULL)	{	// If catching "bad_alloc" wasn't enough.
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
		delete [] logBuffer;
		delete [] recordBuffer;
		return ACS_USA_error;	
	}
	_EVENTLOGRECORD* logrec	= (_EVENTLOGRECORD*) sizeBufPtr; 
	bytesRead = 0;
	String  readOrder = "EVENTLOG_FORWARDS_READ";	// Read "from old to new"
	switch(getRecordsFromEventlog(
    eventLogHandle,
    logrec,
    startRecord, 
		bytesRead,
    readOrder,
    latestRecordNumbers,
		object->getLogFile(),
    offset,
		alarmFilterInterval))
  {
  case recordEOF:
		bufferEnd	= 0;  
		notFinish	= ACS_USA_False;
		ret	= ACS_USA_ok;
		logBuffer[0] = 0;
		recordBuffer[0]	= 0;
		FreeRecordInfoBuffer();
		break;
	case recordError:
		notFinish = ACS_USA_False; 
		break;
	case recordOKconditional:	// If an alarm on 'too high event 
								            // frequency' has bee raised.
		sleepAwhile = TRUE;
		break;
	default:
		break; // Do nothing, everything is ok.
	}

	// In this loop we read one event at the time and try
	// to match all the event against the criterion object.
	while(notFinish) 
	{
		String logFileName(object->getLogFile());

		switch(getRecord(
      startRecord,
      activation,
      logrec, 
			logFileName,
      ACS_USA_alarmSourceNameDefault,
			latestBootTimeStamp,
      latestRecordNumbers,
			recordsPrioThreshold,
      secondsPrioThreshold))
    {
	  case recordEOF: // End of file
			notFinish = ACS_USA_False;
			ret = ACS_USA_ok;
			if ((activation == Activation_FirstShortTerm)	|| (activation == Activation_FirstLongTerm)) 
			{
        // If the boot event wasn't seen in the first	short/long term,
        // more events than can be covered have been written to the log.
        // Raise an A2 alarm.
				DWORD numberOfRecords;						           
				GetNumberOfEventLogRecords(eventLogHandle, &numberOfRecords);
				if ((bootTimeWithinReach == FALSE)
          && (alarmSetOnMissedBootEvent == FALSE)
          && (numberOfRecords >= 500)) // To handle cleared eventlog case.
				{
          alarmSetOnMissedBootEvent = TRUE;	// Do not issue another "too high event 
																            // frequency"-alarm on another log, if  
																            // that alarm has already been raised.
					char tmpstring [150] = "";
					if (strcat(tmpstring, ACS_USA_EventFrequencyError) == "") 
					{
						// strcat failure.
            char* lpMsgBuf;
						FormatMessage(
              FORMAT_MESSAGE_ALLOCATE_BUFFER |
								FORMAT_MESSAGE_MAX_WIDTH_MASK |	// buffersize(255) - ignores inserted
																			          // control sequences such as char 13.
								FORMAT_MESSAGE_FROM_SYSTEM |
								FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL,
              GetLastError(),
              0, 
							(LPTSTR)&lpMsgBuf,
              0,
              NULL);
            char errorString[1024] = "System call strcat() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
						strlen(lpMsgBuf)); // Ends with a '\0'.
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						LocalFree(lpMsgBuf); // Necessary according to the FormatMessage() manual page.
						delete [] logBuffer; // Clean up.
						delete [] recordBuffer;
						FreeRecordInfoBuffer();
						delete [] sizeBufPtr; 
						return ACS_USA_error;
          }
					if (eventManager.report(
            ACS_USA_FirstErrorCode // Raise alarm.
							+ ACS_USA_EventFrequencyExceeded,	// 8714.
						Key_perceivedSeverity_A2,	// A2 alarm.
						ACS_USA_DiagnosticFault,  // AP DIAGNOSTIC FAULT.
						"",		
						tmpstring,	
						error.getProblemText(1), 
						"",
						0) == ACS_USA_error) 
					{
							error.setError(ACS_USA_APeventHandlingError);
							error.setErrorText("eventManager.report() failed in ACS_USA_Analyser::AnalyseObject()");	
							ret = ACS_USA_error;
          }
				}
				if (bootTimeWithinReach == TRUE) 
				{ // Reset flag after each log has been analysed.
					bootTimeWithinReach = FALSE;
				}
			}
      break;
    case recordOK: // Another record fetched.
      // Debug print
			// printf("%s \n", recordBuffer);
      stamp = interpreter.seconds(recordBuffer);
				
			// Catch the case when there is no year in the time stamp.
			if (stamp < lastStamp && stamp != ACS_USA_Time_error) {
        interpreter.reset();
				stamp = interpreter.seconds(recordBuffer);
			}
			if (stamp == ACS_USA_Time_error) 
			{
				switch(error.getError()) 
				{
				// The following error codes are not ery serious.
        // Complain if not reported yet.
        case ACS_USA_TimeStampMatch:
				case ACS_USA_WarnTimeStampFormat:
          break;

				// Make it a syntax error. Escalates AP DIAG FAULT.
        case ACS_USA_ErrorTimeStampFormat:
					error.setError(ACS_USA_SyntaxError);
					notFinish = ACS_USA_False;
					break;
			  default:
					notFinish = ACS_USA_False;
			    break;
        }
			}
			if (notFinish == ACS_USA_True) 
			{
				if (stamp != ACS_USA_Time_error) 
				{
				  lastStamp = stamp;
				}

				if( recordInfoBuffer.empty() ||
					 NULL == recordInfoBuffer.front()
				)
				{
					error.setError(ACS_USA_BadState);
					error.setErrorText("Missing event record info.");
					notFinish = ACS_USA_False; 
					ret = ACS_USA_error;
					break;					
				}
				
				
				ACS_USA_RecordInfoBuffer::iterator iElem = recordInfoBuffer.begin();
				ACS_USA_ObjectManager* customMgr = NULL;
				ACS_USA_AnalysisObject* customObject = NULL;

				// Check if we have a custom ObjectManager for that event source ?				
				customMgr = managers.selectManager( (*iElem)->sourceName );
				if( NULL != customMgr )
				{					
					// Retrieve the AnalysisObject for the current log file
					customObject = customMgr->selectAnalysisObject( logFileName );
					// NULL is OK, that means it's not supported by the custom ACF file.
				}

        // Check the current record
				if( NULL != customObject )
				{
					// Use the custom AnalysisObject
					if (customObject->match(recordBuffer, 
											stamp, 
											activation,
											(*iElem)->insertedStrings, // Provide the inserted strings
											(*iElem)->dataBuffer,	   // Provide the data
											(*iElem)->dataLength
											) == ACS_USA_error) 
					{
						// Error handling.
						notFinish = ACS_USA_False; 
					}					
				}
				else
				{
					// Use the default AnalysisObject
					if (object->match(recordBuffer, stamp, activation) == ACS_USA_error) 
					{
						// Error handling.
						notFinish = ACS_USA_False; 
					}
				}

				// remove the current record info
				delete *iElem;
				recordInfoBuffer.erase( iElem );  // update the buffer				
			}
			break;
    default: // Error occured.
			notFinish = ACS_USA_False; 
			break;
    } // switch.
	
  } // while.

  // Release buffers.
  delete [] logBuffer;
  delete [] recordBuffer;
  FreeRecordInfoBuffer();
	delete [] sizeBufPtr;
    
  // Close log file.
	BOOL logClosed = CloseEventLog(eventLogHandle);		
  if (!logClosed)	
  {        
		// If log cannot be closed...
		char* lpMsgBuf;
		FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_MAX_WIDTH_MASK |	// buffersize(255) - ignores inserted 
													              // control sequences such as char 13.
			  FORMAT_MESSAGE_FROM_SYSTEM |
			  FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
      GetLastError(),
      0, 
	    (LPTSTR)&lpMsgBuf,
      0,
      NULL);
		char errorString[1024] = "System call CloseEventLog() failed. Error from system: ";
		strncat(errorString, lpMsgBuf, strlen(lpMsgBuf));	// Ends with a '\0'.
    error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);	// Necessary according to the FormatMessage() manual page.
		return ACS_USA_error;
  }

  USA_DEBUG(logMsg("Analyser::analyseObject: checking alarms\n"));
    
  //
  // Check alarm conditions, in all available ObjectManagers. 
  vector<ACS_USA_ObjectManager*> aManagers = managers.getManagers();
  for(int om = 0; om < aManagers.size(); om++)
  {
	  ACS_USA_ObjectManager* mgr = aManagers[om];
	  if( NULL != mgr )
	  {
		  //
		  // Consider only AnalysisObjects for the current the log file		  
		  ACS_USA_AnalysisObject *o = mgr->selectAnalysisObject( logFileName );
		  if( NULL != o )
		  {
			  // Indicates which params will be used when logging an new APEvent
			  // -> TRUE : use the default i.e. from the ACF file.
			  // or
			  // -> FALSE: use the ones retrieved from the record info.
			  bool bDefaultLogging = true;

			  // We are under the Main object manager (main ACF file)
			  if( _stricmp( mgr->getSource().c_str(), "ALL") != 0 )
			  {
				  bDefaultLogging = false;
			  }

  // Check for alarm conditions in criteria.
			  if (o->checkAlarms(alarmFilterInterval, suppressedAlarms, bDefaultLogging) == ACS_USA_error) 
			  {
	  // Handle error.
    return ACS_USA_error;
  }
		  }
	  }
  }

  if (sleepAwhile) 
  {
		// Is some application writing events to the logs at a 
		// crazy pace? If so, sleep a short while, to give the 
		// application a chance to cool down.
		Sleep(5000); // Milliseconds
		sleepAwhile = FALSE;
	}

  return ret;
}


//******************************************************************************
//	getRecord()
//******************************************************************************


ACS_USA_RecordStatus
ACS_USA_Analyser::getRecord(DWORD					startRecord, 
							ACS_USA_ActivationType  activation,
							_EVENTLOGRECORD*		logrec,
							String				logFileName,
							String				ACS_USA_alarmSourceNameDefault,
							DWORD&					latestBootTimeStamp,
              const DWORD latestRecordNumbers[6],
							const DWORD recordsPrioThreshold,
							const DWORD secondsPrioThreshold)

{
    size_t	recordIndex					= 0;
    size_t	maxRecordIndex				= recordBufferSize - 1U;	
    ACS_USA_Flag	notFinish			= ACS_USA_True;    
    ACS_USA_RecordStatus ret			= recordError;	
    

    unsigned			totalBytesInLogBuffer		= 0;
	static unsigned		lastCharInLogBuffer			= 0;
	DWORD*				timeWrittenInSecondsPtr;
	tm*					timeStructPtr				= 0;    
	char				timeString[256];				// For the date
	LPSTR				sourceName, computer = NULL; 
  TCHAR      pEventStrings[EVENT_MESSAGE_BUFFER_SIZE] = "";
	char				logonTypeString[logonTypeStringLength]	= "";	
	char				eventTypeString[eventTypeStringLength]	= "";
	char				eventRecordContents[eventRecordLength]	= "";

	tmpCurrentStamp		= 0;

	int logNo	= 0;
	
  if (stricmp(logFileName.c_str(),"System") == 0)
  {
    logNo = 0;  // System log
	}
	else if (stricmp(logFileName.c_str(),"Application") == 0)
  {
		logNo = 1;  // Application log
	}
	else if (stricmp(logFileName.c_str(),"Security") == 0)
  {
		logNo = 2;  // Security log
	}
	else if (stricmp(logFileName.c_str(), "Directory Service") == 0) {
		logNo = 3;  // Directory Service log.
	}
	else if (stricmp(logFileName.c_str(), "DNS Server") == 0) {
		logNo = 4;  // DNS Server log.
	}
	else if (stricmp(logFileName.c_str(), "File Replication Service") == 0) {
		logNo = 5;  // File Replication Service log.
	}

	while (notFinish) 
	{		
		if (logIndex == bufferEnd)						// At the end of log buffer, time    
														// to read from NT's event log
		{  
			lastCharInLogBuffer = 0;			
		
				_EVENTLOGRECORD* nextRecord	= logrec;	// Data block containing 
														// the Event log records

				memset(eventRecordContents, '\0', 
						eventRecordLength);				// Clear string

				DWORD passedBytesSoFar			= 0;
				DWORD endPosOfCurrentRecord		= 0;
				WORD numberOfStringsInRecord	= 0;

				while (passedBytesSoFar < bytesRead)  	// Extract records from data block
				{	
					sourceName = (LPSTR) ((LPBYTE)nextRecord + sizeof(_EVENTLOGRECORD));
					computer   = (LPSTR) ((LPBYTE)sourceName + strlen(sourceName)+1);

					// Make sure we let other processes get CPU time 
					// during heavy load.
					Sleep(10);

					// --------- Skip ACS_AEH events -----------------
					//
					// Check if the event is an Alarm event 
					// written by AEH. If so, skip it, since 
					// we aren't interested in analysing it.
					//
					
					if (activation != Activation_startUp && (stricmp(logFileName.c_str(),"Application") == 0))
					{	
						while (passedBytesSoFar < bytesRead)
						{					
							if	(strcmp(sourceName,							// Is the event source APEventLog?
								ACS_USA_alarmSourceNameDefault.data()) == 0) 
							{
								// Find next record in the logrec buffer
								//
								endPosOfCurrentRecord = nextRecord->Length;	// Check how long	
																			// current record is
								passedBytesSoFar = passedBytesSoFar			// Add number of bytes
												   + endPosOfCurrentRecord; // passed in buffer
													
								if(passedBytesSoFar < bytesRead)			// If there is another record in  
																			// logrec, go for that one instead
								{
									nextRecord		 =  (_EVENTLOGRECORD*)	// Some pointer aritmetic to  
														((LPBYTE)logrec		// find address of next record
														+ passedBytesSoFar); 				
									sourceName = (LPSTR) ((LPBYTE)nextRecord + sizeof(_EVENTLOGRECORD));
									computer   = (LPSTR) ((LPBYTE)sourceName + strlen(sourceName)+1);

									// Save the oldest record number and time in this analysis, to be
									// able to check if the event frequency is too high.
									//
									oldestRecordNoInAnalysis = nextRecord->RecordNumber; 
									oldestRecordTimeInAnalysis = nextRecord->TimeWritten;
								}
								else
								{
									break;									// Last record read in buffer
								}
							}
							else 
							{
								break;										// Break out of inner while, if  
																			// source isn't the name in 
							}												// ACS_USA_alarmSourceNameDefault
						}		// end of while
						if(passedBytesSoFar >= bytesRead)
						{													// Break out of surrounding while, 
							logIndex = 0;									// since there are no more records to
							lastCharInLogBuffer = totalBytesInLogBuffer;	// extract from logrec
							break;											 
						}
					}
					
					// Convert elapsed seconds since event was Written
					// to a date on the form:	Mon Jul 10 19:52:01 1970
					//
					timeWrittenInSecondsPtr = &(nextRecord->TimeWritten);
					timeStructPtr =						// for daylight saving time
									localtime((const time_t*) timeWrittenInSecondsPtr);
					char* tmpTimestring = asctime(timeStructPtr);
					int len				= strlen(tmpTimestring);					
					strncpy(timeString, tmpTimestring, len-1);
					timeString [len-1]	= ' ';			// 25th pos is 24th element
					timeString [len]	= '\0';			// End string with '\0'				

					// --------- Skip records older than boot ------------------
					//
					// Was the record written to the log prior to the latest boot?
					// If so, skip it. We are only analysing the logs from the
					// latest boot.
					// 
					if ((latestBootTimeStamp >= mktime(timeStructPtr)) 
						&& ((activation == Activation_FirstShortTerm)
						|| (activation == Activation_FirstLongTerm)))
					{											// Does the first short or 
																// long term analysis reach 																
						if (bootTimeWithinReach == false)		// the boot time stamp?
						{
							bootTimeWithinReach = true;
						}
					}
					if ((latestBootTimeStamp > mktime(timeStructPtr))
						&& (activation != Activation_startUp))
					{
						bool skippedAEHrecord = false;

						// Skip the record if written before Boot
						//
						while ((passedBytesSoFar < bytesRead) && 
							   (latestBootTimeStamp > mktime(timeStructPtr)))
						{
							do 
							{
								// Make sure we let other processes get CPU time 
								// during heavy load.
								Sleep(0);

								// Find next record in the logrec buffer
								//
								skippedAEHrecord = false;		
								endPosOfCurrentRecord = nextRecord->Length;	// Check how long	
																			// current record is
								passedBytesSoFar = passedBytesSoFar			// Add number of bytes
												   + endPosOfCurrentRecord; // passed in buffer
								
								if(passedBytesSoFar < bytesRead)			// If there is another record   
																			// in logrec, go for that one 
								{											// instead
									nextRecord =	(_EVENTLOGRECORD*)		// Some pointer aritmetic to  
													((LPBYTE)logrec			// find address of next record
													+ passedBytesSoFar); 				
									sourceName = (LPSTR) ((LPBYTE)nextRecord + sizeof(_EVENTLOGRECORD));
									computer   = (LPSTR) ((LPBYTE)sourceName + strlen(sourceName)+1);

									// Save the oldest record number and time in this analysis, to be
									// able to check if the event frequency is too high.
									//
									oldestRecordNoInAnalysis = nextRecord->RecordNumber; 
									oldestRecordTimeInAnalysis = nextRecord->TimeWritten;

									// Assemble the time string
									//
									timeWrittenInSecondsPtr = &(nextRecord->TimeWritten);
								 	timeStructPtr		= localtime((const time_t*)  // Daylight saving
																	timeWrittenInSecondsPtr);
									char* tmpTimestring = asctime(timeStructPtr);
									int len				= strlen(tmpTimestring);					
									strncpy(timeString, tmpTimestring, len-1);
									timeString [len-1]	= ' ';				// 25th pos is 24th element
									timeString [len]	= '\0';				// End string with '\0'
									
									// Check if the event is an Alarm event 
									// written by AEH. If so, skip it and pick 
									// the next record, since we aren't interested 
									// in AEH alarms. 
									//
									
									if (stricmp(logFileName.c_str(),"Application") == 0)	
									{	
										if	(strcmp(sourceName,				// Is the event source APEventLog?
													ACS_USA_alarmSourceNameDefault.data()) == 0) 
										{
											skippedAEHrecord = true;		
										}
									}
								}
							} while (skippedAEHrecord);						// end of do-while

							if (passedBytesSoFar >= bytesRead)
							{
								break;										// Last record read in buffer, 
																			// break out of inner while
							}
						}													// end of while
						if (passedBytesSoFar >= bytesRead)
						{													// Break out of surrounding while, 
							logIndex = 0;									// since there are no more records 
							lastCharInLogBuffer = totalBytesInLogBuffer;	// to extract from logrec
							break;											 
						}
					}

					// ------------------- Skip -----------------------
					//
					// If records have been written to the event log *during* long 
					// or short term analysis, the records read in that analysis 
					// are to be skipped in the next directly following analysis 
					// for performance reasons. Only the records yet not analysed 
					// are interesting. Records are skipped this way in normal 
					// notification analysis as well. I.e. when events are written 
					// to the logs when USA is waiting, making USA wake up.
					//
					if  ((passedBytesSoFar < bytesRead)
						&& (activation != Activation_startUp)
						&& (activation != Activation_FirstShortTerm)
						&& (activation != Activation_FirstLongTerm))
          {
						if (((latestRecordNumbers[0] != 0) &&
							(stricmp(logFileName.c_str(), "System") == 0))                    // System log
							|| ((latestRecordNumbers[1]	!= 0) &&	
							(stricmp(logFileName.c_str(), "Application") == 0))               // Application log
							|| ((latestRecordNumbers[2]	!= 0) &&		
							(stricmp(logFileName.c_str(), "Security") == 0))                  // Security log
              || ((latestRecordNumbers[3]	!= 0) &&
							(stricmp(logFileName.c_str(), "Directory Service") == 0))         // Directory Service log.
							|| ((latestRecordNumbers[4]	!= 0) &&
							(stricmp(logFileName.c_str(), "DNS Server") == 0))                // DNS Server log.
							|| ((latestRecordNumbers[5]	!= 0) &&
							(stricmp(logFileName.c_str(), "File Replication Service") == 0))) // File Replication Service log.
						{
							if (latestRecordNumbers[logNo] == 0)			// If log has not been changed.
							{												// Break out of surrounding while, 
								logIndex = 0;								// since there are no more records 
								lastCharInLogBuffer = totalBytesInLogBuffer;// to extract from logrec.
								break;											 
							}
								bool skippedAEHrecord = false;
								
								// Skip the record, since
								// it has already been analysed.
								// 
								while ((passedBytesSoFar < bytesRead) && 
									(latestRecordNumbers[logNo] >= nextRecord->RecordNumber))
								{
									do 
									{
										// Make sure we let other processes get CPU time 
										// during heavy load.
										Sleep(0);

										// Discard current record. Find next record in the logrec buffer
										//
										skippedAEHrecord = false;		
										endPosOfCurrentRecord = nextRecord->Length;	// Check how long	
																					// current record is
										passedBytesSoFar = passedBytesSoFar			// Add number of bytes
											+ endPosOfCurrentRecord;				// passed in buffer
										
										if(passedBytesSoFar < bytesRead)		// If there is another record   
																				// in logrec, go for that one 
										{										// instead.
											nextRecord =	(_EVENTLOGRECORD*)	// Some pointer aritmetic to  
															((LPBYTE)logrec		// find address of next record
															+ passedBytesSoFar); 				
											sourceName = (LPSTR) ((LPBYTE)nextRecord + sizeof(_EVENTLOGRECORD));
											computer   = (LPSTR) ((LPBYTE)sourceName + strlen(sourceName)+1);

											// Save the oldest record number and time in this analysis to be
											// able to check if the event frequency is too high.
											//
											oldestRecordNoInAnalysis = nextRecord->RecordNumber; 
											oldestRecordTimeInAnalysis = nextRecord->TimeWritten;

											// Assemble the time string
											//
											timeWrittenInSecondsPtr = &(nextRecord->TimeWritten);
											timeStructPtr		= localtime((const time_t*)  // Daylight saving
												timeWrittenInSecondsPtr);
											char* tmpTimestring = asctime(timeStructPtr);
											int len				= strlen(tmpTimestring);					
											strncpy(timeString, tmpTimestring, len-1);
											timeString [len-1]	= ' ';				// 25th pos is 24th element
											timeString [len]	= '\0';				// End string with '\0'									
											
											// Check if the event is an Alarm event 
											// written by AEH. If so, skip it and pick 
											// the next record, since we aren't interested 
											// in AEH alarms. 
											//
											if (stricmp(logFileName.c_str(),"Application") == 0)
											{	
												if	(strcmp(sourceName,		// Is the event source APEventLog?
													ACS_USA_alarmSourceNameDefault.data()) == 0) 
												{
													skippedAEHrecord = true;		
												}
											}
										}
									} while (skippedAEHrecord);				// end of do-while
									
									if (passedBytesSoFar >= bytesRead)
									{
										break;								// Last record read in buffer, 
										// break out of inner while
									}
								}											// end of while
								if (passedBytesSoFar >= bytesRead)
								{												// Break out of surrounding while, 
									logIndex = 0;								// since there are no more records 
									lastCharInLogBuffer = totalBytesInLogBuffer;// to extract from logrec
									break;											 
								}
							}
					}
				
					// --------- Extract event data -----------------
					//
					// If there is string data in the record, add it.
					//
        ACS_USA_EventSource* pEventSource = NULL;

        // Add event source instance to event source list.
        if (stricmp(logFileName.c_str(), "System") == 0)
          pEventSource = m_pEventSourceList[0]->EventSource(sourceName);
        else if (stricmp(logFileName.c_str(), "Application") == 0)
          pEventSource = m_pEventSourceList[1]->EventSource(sourceName);
        else if (stricmp(logFileName.c_str(), "Security") == 0)
          pEventSource = m_pEventSourceList[2]->EventSource(sourceName);
        else if (stricmp(logFileName.c_str(), "Directory Service") == 0)
          pEventSource = m_pEventSourceList[3]->EventSource(sourceName);
        else if (stricmp(logFileName.c_str(), "DNS Server") == 0)
          pEventSource = m_pEventSourceList[4]->EventSource(sourceName);
        else if (stricmp(logFileName.c_str(), "File Replication Service") == 0)
          pEventSource = m_pEventSourceList[5]->EventSource(sourceName);

        // Load event source dll or exe files.
        if(pEventSource != NULL)
        {
            if (!pEventSource->get_IsLoaded())
            {
                if(!pEventSource->LoadSourceModules())
                {
                    // Failed to load DLL
                    error.setError(ACS_USA_SystemCallError);
                    error.setErrorText("Call to LoadSourceModules failed.");
                    return recordError; 
                }
            }
        }
        else
        {
           error.setError(ACS_USA_BadState);
           error.setErrorText("Event Source lookup failure.");
           return recordError;
        }
					
					// Keep track of event record info.				  
					auto_ptr<ACS_USA_RecordInfo> recordInfo( new ACS_USA_RecordInfo(String(sourceName)) );
					if( NULL == recordInfo.get() )
					{
						error.setError(ACS_USA_Memory);
						error.setErrorText("new() failed in ACS_USA_Analyser::getRecord.");
						ret = recordError;								
						return ret;
					}

					numberOfStringsInRecord = nextRecord->NumStrings;
					if (numberOfStringsInRecord > 0)	
					{
						if (findInsertedStrings(pEventStrings, nextRecord, pEventSource) == ACS_USA_error)
						{
							ret = recordError;
 							return ret; 
						}

						// Save the inserted strings
						if (retrieveInsertedStrings( nextRecord, *recordInfo ) == ACS_USA_error)
						{
							ret = recordError;
 							return ret; 
						}


						// Find logon type in Security event data.
						//
						if ((numberOfStringsInRecord > 2) && 
								 stricmp(logFileName.c_str(),"Security") == 0)	
						{
						// Add logontype, if any in Security log event data
							if (findLogonType(logonTypeString, nextRecord) == ACS_USA_error)
							{
								ret = recordError;
								return ret; 
							}
						}
					}

					// Retrieve event-specific data.
					//
					{
						if (retrieveEventData( nextRecord, *recordInfo ) == ACS_USA_error)
						{
							ret = recordError;
 							return ret; 
						}
					}

					// Find event type in event data
					//
					{
						if (findEventType(eventTypeString, nextRecord) == ACS_USA_error)
						{
							ret = recordError;
							return ret;
						}
					}

					// --------- Assemble event data string -----------------
					//
					// The two top bytes of the four byte NT Event indicates all sorts 
					// of things that are uninteresting to USA. However, due to an 
					// obvious lack of compentence at Microsoft,the documentation 
					// for EVENTLOGRECORD states that the Event Id is to be handled 
					// as a DWORD. But the documentation doesn't say that the two top 
					// bytes are not to be regarded as part of the event id, since 
					// this leads to unresonable event id values around 2^31. 
					// Casting DWORD to WORD cuts away the two top bytes leaving 
					// 65536 as a maximum event id for each event source.
					//
					WORD twoByteEventId =				// Throws away the two top bytes in
								static_cast<WORD>(nextRecord->EventID);	// the four byte bit sequence of DWORD.

					int t = sprintf(eventRecordContents, 
						"%s%s%u%s%s%s%u%s%u%s%s%s", 
						" ",							// Start each record string 
														// with a space to make it 
														// easy to find the time stamp.
						timeString,               
						nextRecord->RecordNumber,	" \0",
						computer,					" \0",
						twoByteEventId,				" \0",
						nextRecord->Length,			" \0", 
						sourceName,					" \0");

					if (t<1) 							// Sprintf failure
					{   
						error.setError(ACS_USA_SystemCallError);				
						error.setErrorText("sprintf(eventRecordContents, ...) failed."); 
						ret = recordError;								
						return ret;
					}

					// Add the event data to string
					//
					if  (addEventData(logonTypeString, eventTypeString, 
									  pEventStrings, numberOfStringsInRecord,
									  eventRecordContents) == recordError)
					{
						return recordError;
					}

					// Save the current record number, to be
					// able to check if the event frequency is too high.
					//
					currentRecordNoInAnalysis = nextRecord->RecordNumber; 
					currentRecordTimeInAnalysis = nextRecord->TimeWritten;

					if (activation == Activation_instantShortTerm) 
					{
						if (((currentRecordNoInAnalysis - oldestRecordNoInAnalysis) 
							> (recordsPrioThreshold - 1)) 
							&& ((currentRecordTimeInAnalysis - oldestRecordTimeInAnalysis) 
							<  secondsPrioThreshold))
							// Default: More than 100 records in less than one minute
							// renders reduced USA process priority to offload the CPU.
						{
							// Since we have a maximum number of records to read, we need to  
							// avoid stealing CPU-time from more important system activities 
							// by reducing process priority, 
							//
							HANDLE  myOwnProcessHandle	= GetCurrentProcess();	// Fetch handle to 'myself'
							
							DWORD currentProcessPrioClass = GetPriorityClass(myOwnProcessHandle); 
							if ((currentProcessPrioClass	!= 0) 
								&& (currentProcessPrioClass != IDLE_PRIORITY_CLASS)) 
							{
								if (ACS_TRA_ON(traceAnalyser4)) 
								{
									ACS_TRA_event(&traceAnalyser4, ACS_USA_tracePrioLow);
								}
								if (!SetPriorityClass(myOwnProcessHandle, IDLE_PRIORITY_CLASS))
								{
									char* lpMsgBuf;						// Log error to AEH
									FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
										FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
																		// control sequences such as char 13
										FORMAT_MESSAGE_FROM_SYSTEM|
										FORMAT_MESSAGE_IGNORE_INSERTS,
										NULL, GetLastError(), 0, 
										(LPTSTR)&lpMsgBuf, 0, NULL);			
									char errorString[1024] = "System call SetPriorityClass() failed. Error from system: ";
									strncat(errorString, lpMsgBuf, 
										strlen(lpMsgBuf));					 // Ends with a '\0'	
									eventManager.report(ACS_USA_FirstErrorCode 
										+ ACS_USA_SystemCallError,   // 8703
										Key_perceivedSeverity_EVENT, // Event
										ACS_USA_FaultCause,		       // AP INTERNAL FAULT
										"",		
										errorString,	
										"SYSTEM CALL FAILED",
										"", 0); 
									LocalFree(lpMsgBuf);  
								}	
							}
						}
					}

					// --------- Fill the buffer -----------------
					//
					// String is ready, time to fill the buffer
					//
					if (totalBytesInLogBuffer == 0)		// Is log buffer empty? 
														// - start filling.
					{
						strcpy(logBuffer, eventRecordContents);	
						lastCharInLogBuffer = totalBytesInLogBuffer = strlen(logBuffer);
						
						FreeRecordInfoBuffer(); // To be sure, we are SYNCH.
						recordInfoBuffer.push_back( recordInfo.release() );
					}				
					else								// Log buffer is not empty
					{
						if (totalBytesInLogBuffer + strlen(eventRecordContents) < logBufferSize)							
						{  					
														// If there is space enough in 
														// logBuffer, add data from read record
							strcat(logBuffer, eventRecordContents);
							totalBytesInLogBuffer = strlen(logBuffer);

							recordInfoBuffer.push_back( recordInfo.release() );
						}
						else
						{								// Buffer is full
							lastCharInLogBuffer = totalBytesInLogBuffer;
							logIndex = 0;
						}						
					}

					// Find the events that indicates boot or restart its time stamp
					//
					if (activation == Activation_startUp)
					{											// Save the current time stamp temporarily 						
						tmpCurrentStamp = mktime(timeStructPtr);// for handling the case when there is no 
																// boot event among the events read in 
																// start-up analysis
						ACS_USA_Regexp bootRegexp;
						if (bootRegexp.compilePattern(bootEventRegExpText) != ACS_USA_ok) 
						{
							ret = recordError;								
							return ret;
						}
						if (bootRegexp.checkMatch(eventRecordContents) 
												  == ACS_USA_Regexp_match)
						{
							// Check for the 'The Event log service was started'-event.
							// Indicates Boot.

							latestBootTimeStamp =			// Save the time stamp
												mktime(timeStructPtr);
							logIndex	= bufferEnd;			
							ret			= recordEOF;
							notFinish	= ACS_USA_False;	// Don't read more from logBuffer
							break;							// Break out of inner while
							//
							// Quit analysing - we have found the boot time stamp
						}
						else
							// Check for the 'The service ACS_USA_SyslogAnalyser is exiting'-event.
							// Indicates USA has been stoped and now is restarted.
						{
							ACS_USA_Regexp restartRegexp;
							if (restartRegexp.compilePattern(clearEventRegExpText) 
														   != ACS_USA_ok) 
							{
								ret = recordError;
								return ret;
							}
							if (restartRegexp.checkMatch(eventRecordContents) 
														== ACS_USA_Regexp_match)
							{						
								latestBootTimeStamp =		// Save the time stamp
												mktime(timeStructPtr);
								clearSecLogPerformed = TRUE;
								logIndex	= bufferEnd;			
								ret			= recordEOF;
								notFinish	= ACS_USA_False;// Don't read more from logBuffer
								break;						// Break out of inner while
								//
								// Quit analysing - we have found the event indicating that
								// USA has just been restarted (not rebooted!).
							}
						}
					}

					// Find next record in the logrec buffer
					//
					endPosOfCurrentRecord = nextRecord->Length;	// Check how long 
																// current record is

					passedBytesSoFar = passedBytesSoFar			// Add number of bytes
									   + endPosOfCurrentRecord; // passed in buffer

					nextRecord = (_EVENTLOGRECORD*)				// Some pointer aritmetic to  
								 ((LPBYTE)logrec				// find address of next record
								 + passedBytesSoFar); 		

					// When the the last record has been added to logBuffer,
					// logBuffer is full and it is time to start extracting data from it.
					//						
					if (passedBytesSoFar == bytesRead)
					{
						logIndex = 0;
						lastCharInLogBuffer = totalBytesInLogBuffer;
					}
				}   // end of inner while						 
		}		
		else											// logIndex is not equal to bufferEnd.
														// Still data in log buffer, no read  
														// from NT event log neccesary
		{
			if (logBuffer[logIndex] == newLineChar) 	// Not at the end of logBuffer
			{
				ret = recordOK;							// record ok, last char is newline
				notFinish = ACS_USA_False;
				++logIndex;
			} 
			else if (recordIndex < maxRecordIndex) 
			{                        					// More data to copy from logBuffer
				if (logIndex < lastCharInLogBuffer)
				{
					recordBuffer[recordIndex++] = logBuffer[logIndex++];
				}
				else 
				{
					if (logIndex == lastCharInLogBuffer)
					{	
						logIndex = bufferEnd;			// Everything is read in logBuffer
						notFinish = ACS_USA_False;  
						ret = recordEOF;            
					}
					else 
					{
						++logIndex;
					}
				}
			} 
			else 
			{
														// At the end of record buffer. 
				++logIndex;
			}
		}
	}       // end of outer while

	if (recordBuffer != NULL)							// Shouldn't be NULL, ever. 
	{
	    recordBuffer[recordIndex] = endOfStringChar;	// Everything OK, end record
														// string with a '\0'.
	} 
	else												// Some bad state has ocurred.
	{ 
		error.setError(ACS_USA_BadState);				
		error.setErrorText("ACS_USA_Analyser::getRecord(), recordBuffer is erroneously NULL"); 
		ret = recordError;
	}
	// For testing only:
	//cout << endl << "RecordBuffer  :" << recordBuffer << endl;

	return ret;
}




//******************************************************************************
//	seekFromEnd()
//******************************************************************************
 
ACS_USA_StatusType
ACS_USA_Analyser::seekFromEnd(HANDLE eventLogHandle, 
							  const off_t offset, 
							  DWORD& startRecord,
							  ACS_USA_ActivationType  activation)
{	
    // Find the number of records in log file
	//
	DWORD numberOfRecords		= 0;

	BOOL  eventLogAccessible	= GetNumberOfEventLogRecords(eventLogHandle,
															&numberOfRecords);
	if (!eventLogAccessible)           
	{   
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|			// buffersize(255)-ignores inserted 
													// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call GetNumberOfEventLogRecords() failed. Error from system: ";
		strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));					// Ends with a '\0'
		
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);						// Necessary according to the 
													// FormatMessage() manual page.
		return ACS_USA_error;
	}

	// Find the latest record number in log file
	//
	DWORD oldestRecordNumber	= 0;
	DWORD latestRecordNumber	= 0;

	eventLogAccessible	= GetOldestEventLogRecord(eventLogHandle, &oldestRecordNumber);
	if (!eventLogAccessible)           
	{   
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|			// buffersize(255)-ignores inserted 
													// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call GetOldestEventLogRecord() failed. Error from system: ";
		strncat(errorString, lpMsgBuf, 
			strlen(lpMsgBuf));						// Ends with a '\0'
		
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);						// Necessary according to the 
													// FormatMessage() manual page.
		return ACS_USA_error;
	}	
	latestRecordNumber = oldestRecordNumber + numberOfRecords - 1;

	if ((activation == Activation_longTerm)
		|| (activation == Activation_FirstLongTerm))
	{		
		// 200 is an estimate of the average size of NT event log records in bytes.
		// The divsion by two is performed to go for a number of events that are sure 
		// to fit in the logBuffer, even if the events happen to be bigger than average.
		//
		if (numberOfRecords <= ((logBufferSize/200)/2))		
		{
													// If there are fewer events in the  
			startRecord = oldestRecordNumber;		// Event log than there is room for  
													// in the buffer, start from the 
													// oldest record.
		}
		else 
		{
			// Read as many events as there is safely room for.
			//
			startRecord = latestRecordNumber - ((logBufferSize/200)/2);

			// An example: 401 = 6401 - 6000, i.e. USA reads	 
			// 6000 records, from record # 401 until 6401.
		}
	}
	else if ((activation == Activation_shortTerm)
			||(activation == Activation_instantShortTerm)
			||(activation == Activation_FirstShortTerm))
	{			
		if (numberOfRecords <= offset) 
		{
													// If fewer records in the log file  
			startRecord = oldestRecordNumber;		// than requested, start from the 
													// oldest record.
		}
		else
		{		
														// If more records in log file  
			startRecord = latestRecordNumber - offset;	// than requested, calculate  
														// which record to start with.
		}
	}
	else if (activation == Activation_startUp)			// Read the newest record first
	{
		startRecord = latestRecordNumber;				// Newest record
	}

	if (ACS_TRA_ON(traceAnalyser2)) 
	{
	  ACS_TRA_event(&traceAnalyser2, ACS_USA_traceReadEventLog, startRecord);
	}

	return ACS_USA_ok;
}


//******************************************************************************
//	getRecordsFromEventlog()
//******************************************************************************

ACS_USA_StatusType
ACS_USA_Analyser::getRecordsFromEventlog(HANDLE eventLogHandle, 
										 _EVENTLOGRECORD* logrec,
										 DWORD startRecordNumber,
										 DWORD& bytesRead,
										 String readOrder,
										 const DWORD latestRecordNumbers[6],
										 String logFileName,
										 const off_t offset,
										 DWORD alarmFilterInterval)
{
	DWORD numberOfRecords				= 0;
	DWORD oldestRecordNumber			= 0;
	DWORD bytesNext = 0, tmpBytesNext	= 0;
    ACS_USA_RecordStatus ret			= recordOK;

	int readOrderFlag;
	if (readOrder == "EVENTLOG_FORWARDS_READ")
	{
		readOrderFlag = 0X0004;
	}
	else if (readOrder == "EVENTLOG_BACKWARDS_READ")
	{
		readOrderFlag = 0X0008;

	}

	BOOL	eventLogAccessible = GetNumberOfEventLogRecords(eventLogHandle, 
															&numberOfRecords);	
	BOOL	eventLogAccessible2 = GetOldestEventLogRecord(eventLogHandle,
															&oldestRecordNumber);
	if (startRecordNumber < oldestRecordNumber) 
	{														// If fewer events in log than requested, 	
		startRecordNumber = oldestRecordNumber;				// start from the oldest one.
	}
	
	if (!eventLogAccessible || !eventLogAccessible2)
	{ 				
				char* lpMsgBuf;
				char errorString[1024] ;
				memset(errorString, '\0', 1024);				// Clear string
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
					FORMAT_MESSAGE_MAX_WIDTH_MASK|			// buffersize(255)-ignores inserted 
															// control sequences such as char 13
					FORMAT_MESSAGE_FROM_SYSTEM|
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, GetLastError(), 0, 
					(LPTSTR)&lpMsgBuf, 0, NULL);
				
				if (!eventLogAccessible)
				{
					strcpy(errorString, "System call GetNumberOfEventLogRecords() failed. Error from system: ");
				}
				else if (!eventLogAccessible2)
				{
					strcpy(errorString, "System call GetOldestEventLogRecord() failed. Error from system: ");
				}

				strncat(errorString, lpMsgBuf, 
						strlen(lpMsgBuf));					// Ends with a '\0'

				error.setError(ACS_USA_SystemCallError);
				error.setErrorText(errorString);			
				ret = recordError;
				LocalFree(lpMsgBuf);						// Necessary according to the 
															// FormatMessage() manual page.
				return ret;
				
	} 		
	else if (numberOfRecords < 1)
	{										// The NT Event log is empty 					
		return recordEOF;	
	}
						
		//--------- Set the file pointer to the first record to read. ---------
		//
		eventLogAccessible = ReadEventLog(eventLogHandle,
			EVENTLOG_SEEK_READ|readOrderFlag,
			startRecordNumber,  // Record number is returned from seekFromEnd_NT()	
			logrec, 
			0,					// Read zero bytes - just set file pointer		
			&bytesRead, 
			&bytesNext); 

			if (!eventLogAccessible) 
			{ 				
				DWORD errorCode = GetLastError();

				switch (errorCode) {
			
				case ERROR_INSUFFICIENT_BUFFER:	// Error 122
				
					// Do nothing, the buffer (0 bytes) is 
					// supposed to be insufficient, since the 
					// file pointer is only to be positioned
					break;

				default:

					char* lpMsgBuf;
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
						FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
															// control sequences such as char 13
						FORMAT_MESSAGE_FROM_SYSTEM|
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, GetLastError(), 0, 
						(LPTSTR)&lpMsgBuf, 0, NULL);
					
					char errorString[1024] = "System call ReadEventLog() failed. Error from system: ";
					strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));				// Ends with a '\0'
					
					// AFTER MERGE CN-I 1023---------------------------------------------
					if(errorCode==ERROR_EVENTLOG_FILE_CORRUPT)  //Error 1500
                    ClearEventLog(eventLogHandle,NULL);         //Clears event log when event log file is corrupt
                                                                                                                                                        
                    String perceivedSeverity(Key_perceivedSeverity_EVENT);
                    String probableCause(ACS_USA_FaultCause);
                    if (eventManager.report(
                                ACS_USA_SystemCallError + ACS_USA_FirstErrorCode,
                                perceivedSeverity,
                                probableCause,
                                String(),
                                String(errorString),
                                "SYSTEM CALL FAILED",
                                "",                             // Node
                                0) == ACS_USA_error) 
                    {
                    }
                    LocalFree(lpMsgBuf);                    // Necessary according to the 
                                                            // FormatMessage() manual page.
                    return recordEOF;
					// AFTER MERGE CN-I 1023---------------------------------------------

					// BEFORE MERGE CN-I 1023---------------------------------------------
					//error.setError(ACS_USA_SystemCallError);
					//error.setErrorText(errorString);			
					//ret = recordError;
					//LocalFree(lpMsgBuf);					// Necessary according to the 
															// FormatMessage() manual page.
					//return ret;
					// BEFORE MERGE CN-I 1023---------------------------------------------
				}
			} 					

			//------- Start reading from file pointer position --------
			//
			if (readOrder == "EVENTLOG_BACKWARDS_READ")
			{
				eventLogAccessible = ReadEventLog(eventLogHandle,
					EVENTLOG_SEQUENTIAL_READ|readOrderFlag, 
					0,						// Record number not needed.					
					logrec,					// Buffer for all the events.					
					(logBufferSize),		// Number of bytes to read (the size 
											// of the sizeBufPtr array div by
											// two - don't want to read more
											// than in long term).
					&bytesRead, 
					&bytesNext); 
			}
			else
			{
				eventLogAccessible = ReadEventLog(eventLogHandle,
					EVENTLOG_SEQUENTIAL_READ|readOrderFlag, 
					0,						// Record number not needed.					
					logrec,					// Buffer for all the events.					
					logBufferSize,			// Number of bytes to read (the 
											// size of the sizeBufPtr array 
											// (default 2.3 Mb)).
					&bytesRead, 
					&bytesNext); 
			}
			if (!eventLogAccessible) 
			{ 	
				DWORD errorCd = GetLastError();

				char* lpMsgBuf;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
					FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
													// control sequences such as char 13 
					FORMAT_MESSAGE_FROM_SYSTEM|
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, GetLastError(), 0, 
					(LPTSTR)&lpMsgBuf, 0, NULL);
				
				char errorString[1024] = "System call ReadEventLog() failed. Error from system: ";
				strncat(errorString, lpMsgBuf, 
						strlen(lpMsgBuf));			// Ends with a '\0'	

				// AFTER MERGE CN-I 1023---------------------------------------------
				if(errorCd==ERROR_EVENTLOG_FILE_CORRUPT)  //Error 1500
                ClearEventLog(eventLogHandle,NULL);         //Clears event log when event log file is corrupt
                                                                                                                                                        
                String perceivedSeverity(Key_perceivedSeverity_EVENT);
                String probableCause(ACS_USA_FaultCause);
                if (eventManager.report(
                            ACS_USA_SystemCallError + ACS_USA_FirstErrorCode,
                            perceivedSeverity,
                            probableCause,
                            String(),
                            String(errorString),
                            "SYSTEM CALL FAILED",
                            "",                             // Node
                            0) == ACS_USA_error) 
                {
                }
                LocalFree(lpMsgBuf);                    // Necessary according to the 
                                                            // FormatMessage() manual page.
                return recordEOF;
				// AFTER MERGE CN-I 1023---------------------------------------------

				// BEFORE MERGE CN-I 1023---------------------------------------------
				//error.setError(ACS_USA_SystemCallError);
				//error.setErrorText(errorString);			
				//ret = recordError;
				//LocalFree(lpMsgBuf);			// Necessary according to the 
												// FormatMessage() manual page.
				//return ret;
				// BEFORE MERGE CN-I 1023---------------------------------------------

			} 
		 
			
		// ----------- Check if the event write frequency is too high ------------
		//
		
		// For test only:
		// cout << endl << "------------- " <<logrec->RecordNumber << " ---------------" << endl;

		if (logFileName != "") 			// Not start-up analysis
		{
			if ((latestRecordNumbers[0]	!= 0) &&
        (stricmp(logFileName.c_str(), "System") == 0))  // System	log
				
			{
				if (latestRecordNumbers[0] < logrec->RecordNumber)
				{ 
					// We are missing records in this analysis, due 
					// to too high event frequency.
					//
					char diffNoString[15] = "";
					if (sprintf(diffNoString, "%u", offset) < 1)
					{
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
																// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call sprintf() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
								strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);					// Necessary according to the 
																// FormatMessage() manual page.
						return ret;
					}
					char tmpstring [150] = "";
					if ((strcat(tmpstring, "Too high event write frequency in the System log. USA has in its latest analysis only analysed the newest ") == "")
						|| (strcat(tmpstring, diffNoString) == "")
						|| (strcat(tmpstring, " events in the System log. Older events might need to be manually analysed.") == ""))
					{ 
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call strcat() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);				// Necessary according to the 
															// FormatMessage() manual page.
						return ret;
					}

					if (eventManager.report(ACS_USA_FirstErrorCode	// Raise alarm
								+ ACS_USA_EventFrequencyExceeded,	        // 8714
								Key_perceivedSeverity_A2,					        // A2 alarm
								ACS_USA_DiagnosticFault,  			          // AP DIAGNOSTIC FAULT
								"",	
								tmpstring,
								error.getProblemText(1), 
								"",
								alarmFilterInterval)  == ACS_USA_error)
					{
						error.setError(ACS_USA_APeventHandlingError);
						error.setErrorText("eventManager.report() failed in ACS_USA_Analyser::getRecordsFromEventlog()");	
						ret = ACS_USA_error;
					}
					else
					{
						ret = recordOKconditional;	// To make USA wait a few seconds, giving
													// the application writing events like 
													// crazy a chance to cool down.
					}
				}
			}
			else if ((latestRecordNumbers[1]	!= 0) &&
							(stricmp(logFileName.c_str(),"Application") == 0))  // Application log
			{
				if (latestRecordNumbers[1] < logrec->RecordNumber)
				{ 
					// We are missing records in this analysis, due 
					// to too high event frequency.
					//
					char diffNoString[15] = "";
					if (sprintf(diffNoString, "%u", offset) < 1)
					{
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
																// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call sprintf() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
								strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);					// Necessary according to the 
																// FormatMessage() manual page.
						return ret;
					}
					char tmpstring [150] = "";
					if ((strcat(tmpstring, "Too high event write frequency in the Application log. USA has in its latest analysis only analysed the newest ") == "")
						|| (strcat(tmpstring, diffNoString) == "")
						|| (strcat(tmpstring, " events in the Application log. Older events might need to be manually analysed.") == ""))
					{ 
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call strcat() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);				// Necessary according to the 
															// FormatMessage() manual page.
						return ret;
					}

					if (eventManager.report(ACS_USA_FirstErrorCode	// Raise alarm
								+ ACS_USA_EventFrequencyExceeded,	        // 8714
								Key_perceivedSeverity_A2,					        // A2 alarm
								ACS_USA_DiagnosticFault,  			          // AP DIAGNOSTIC FAULT
								"",	
								tmpstring,
								error.getProblemText(1), 
								"",
								alarmFilterInterval)  == ACS_USA_error)
					{
						error.setError(ACS_USA_APeventHandlingError);
						error.setErrorText("eventManager.report() failed in ACS_USA_Analyser::getRecordsFromEventlog()");	
						ret = ACS_USA_error;
					}
					else
					{
						ret = recordOKconditional;	// To make USA wait a few seconds, giving
													// the application writing events like 
													// crazy a chance to cool down.
					}
				}
			}
			else if ((latestRecordNumbers[2]	!= 0) &&
				(stricmp(logFileName.c_str(),"Security") == 0)) // Security log
			{
				if (latestRecordNumbers[2] < logrec->RecordNumber)
				{ 
					// We are missing records in this analysis, due 
					// to too high event frequency.
					//
					char diffNoString[15] = "";
					if (sprintf(diffNoString, "%u", offset) < 1)
					{
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
																// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call sprintf() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
								strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);					// Necessary according to the 
																// FormatMessage() manual page.
						return ret;
					}
					char tmpstring [150] = "";
					if ((strcat(tmpstring, "Too high event write frequency in the Security log. USA has in its latest analysis only analysed the newest ") == "")
						|| (strcat(tmpstring, diffNoString) == "")
						|| (strcat(tmpstring, " events in the Security log. Older events might need to be manually analysed.") == ""))
					{ 
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call strcat() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);				// Necessary according to the 
															// FormatMessage() manual page.
						return ret;
					}

					if (eventManager.report(ACS_USA_FirstErrorCode  // Raise alarm
								+ ACS_USA_EventFrequencyExceeded,	        // 8714
								Key_perceivedSeverity_A2,					        // A2 alarm
								ACS_USA_DiagnosticFault,  			          // AP DIAGNOSTIC FAULT
								"",	
								tmpstring,
								error.getProblemText(1), 
								"",
								alarmFilterInterval)  == ACS_USA_error)
					{
						error.setError(ACS_USA_APeventHandlingError);
						error.setErrorText("eventManager.report() failed in ACS_USA_Analyser::getRecordsFromEventlog()");	
						ret = ACS_USA_error;
					}
					else
					{
						ret = recordOKconditional;	// To make USA wait a few seconds, giving
													// the application writing events like 
													// crazy a chance to cool down.
					}
				}
			}
      else if ((latestRecordNumbers[3] != 0) &&
				(stricmp(logFileName.c_str(), "Directory Service") == 0)) // Directory Service log.
			{
				if (latestRecordNumbers[3] < logrec->RecordNumber)
				{ 
					// We are missing records in this analysis, due 
					// to too high event frequency.
					//
					char diffNoString[15] = "";
					if (sprintf(diffNoString, "%u", offset) < 1)
					{
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
																// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call sprintf() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
								strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);					// Necessary according to the 
																// FormatMessage() manual page.
						return ret;
					}
					char tmpstring [150] = "";
					if ((strcat(tmpstring, "Too high event write frequency in the Directory Service log. USA has in its latest analysis only analysed the newest ") == "")
						|| (strcat(tmpstring, diffNoString) == "")
						|| (strcat(tmpstring, " events in the Directory Service log. Older events might need to be manually analysed.") == ""))
					{ 
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call strcat() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);				// Necessary according to the 
															// FormatMessage() manual page.
						return ret;
					}

					if (eventManager.report(ACS_USA_FirstErrorCode  // Raise alarm
								+ ACS_USA_EventFrequencyExceeded,	        // 8714
								Key_perceivedSeverity_A2,					        // A2 alarm
								ACS_USA_DiagnosticFault,  			          // AP DIAGNOSTIC FAULT
								"",	
								tmpstring,
								error.getProblemText(1), 
								"",
								alarmFilterInterval)  == ACS_USA_error)
					{
						error.setError(ACS_USA_APeventHandlingError);
						error.setErrorText("eventManager.report() failed in ACS_USA_Analyser::getRecordsFromEventlog()");	
						ret = ACS_USA_error;
					}
					else
					{
						ret = recordOKconditional;	// To make USA wait a few seconds, giving
													// the application writing events like 
													// crazy a chance to cool down.
					}
				}
			}
      else if ((latestRecordNumbers[4] != 0) &&
				(stricmp(logFileName.c_str(), "DNS Server") == 0))	// DNS Server log.
			{
				if (latestRecordNumbers[4] < logrec->RecordNumber)
				{ 
					// We are missing records in this analysis, due 
					// to too high event frequency.
					//
					char diffNoString[15] = "";
					if (sprintf(diffNoString, "%u", offset) < 1)
					{
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
																// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call sprintf() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
								strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);					// Necessary according to the 
																// FormatMessage() manual page.
						return ret;
					}
					char tmpstring [150] = "";
					if ((strcat(tmpstring, "Too high event write frequency in the DNS Server log. USA has in its latest analysis only analysed the newest ") == "")
						|| (strcat(tmpstring, diffNoString) == "")
						|| (strcat(tmpstring, " events in the DNS Server log. Older events might need to be manually analysed.") == ""))
					{ 
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call strcat() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);				// Necessary according to the 
															// FormatMessage() manual page.
						return ret;
					}

					if (eventManager.report(ACS_USA_FirstErrorCode  // Raise alarm
								+ ACS_USA_EventFrequencyExceeded,	        // 8714
								Key_perceivedSeverity_A2,					        // A2 alarm
								ACS_USA_DiagnosticFault,  			          // AP DIAGNOSTIC FAULT
								"",	
								tmpstring,
								error.getProblemText(1), 
								"",
								alarmFilterInterval)  == ACS_USA_error)
					{
						error.setError(ACS_USA_APeventHandlingError);
						error.setErrorText("eventManager.report() failed in ACS_USA_Analyser::getRecordsFromEventlog()");	
						ret = ACS_USA_error;
					}
					else
					{
						ret = recordOKconditional;	// To make USA wait a few seconds, giving
													// the application writing events like 
													// crazy a chance to cool down.
					}
				}
			}
      else if ((latestRecordNumbers[5] != 0) &&
				(stricmp(logFileName.c_str(), "File Replication Service") == 0))	// File Replication Service log.
			{
				if (latestRecordNumbers[5] < logrec->RecordNumber)
				{ 
					// We are missing records in this analysis, due 
					// to too high event frequency.
					//
					char diffNoString[15] = "";
					if (sprintf(diffNoString, "%u", offset) < 1)
					{
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
																// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call sprintf() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
								strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);					// Necessary according to the 
																// FormatMessage() manual page.
						return ret;
					}
					char tmpstring [150] = "";
					if ((strcat(tmpstring, "Too high event write frequency in the File Replication Service log. USA has in its latest analysis only analysed the newest ") == "")
						|| (strcat(tmpstring, diffNoString) == "")
						|| (strcat(tmpstring, " events in the File Replication Service log. Older events might need to be manually analysed.") == ""))
					{ 
						char* lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13 
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);
						
						char errorString[1024] = "System call strcat() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));				// Ends with a '\0'	
						
						error.setError(ACS_USA_SystemCallError);
						error.setErrorText(errorString);			
						ret = recordError;
						LocalFree(lpMsgBuf);				// Necessary according to the 
															// FormatMessage() manual page.
						return ret;
					}

					if (eventManager.report(ACS_USA_FirstErrorCode  // Raise alarm
								+ ACS_USA_EventFrequencyExceeded,	        // 8714
								Key_perceivedSeverity_A2,					        // A2 alarm
								ACS_USA_DiagnosticFault,  			          // AP DIAGNOSTIC FAULT
								"",	
								tmpstring,
								error.getProblemText(1), 
								"",
								alarmFilterInterval)  == ACS_USA_error)
					{
						error.setError(ACS_USA_APeventHandlingError);
						error.setErrorText("eventManager.report() failed in ACS_USA_Analyser::getRecordsFromEventlog()");	
						ret = ACS_USA_error;
					}
					else
					{
						ret = recordOKconditional;	// To make USA wait a few seconds, giving
													// the application writing events like 
													// crazy a chance to cool down.
					}
				}
			}
		}	

		if (ret == recordOKconditional)
		{
			// Since we have a maximum number of records to read, we need to  
			// avoid stealing CPU-time from more important system activities, 
			// by reducing process priority, 
			//
			HANDLE  myOwnProcessHandle	= GetCurrentProcess();	// Fetch handle to 'myself'

			DWORD currentProcessPrioClass = GetPriorityClass(myOwnProcessHandle); 
			if ((currentProcessPrioClass	!= 0) 
				&& (currentProcessPrioClass != IDLE_PRIORITY_CLASS)) 
			{
				if (ACS_TRA_ON(traceAnalyser4)) 
				{
					ACS_TRA_event(&traceAnalyser4, ACS_USA_tracePrioLow);
				}
				if (!SetPriorityClass(myOwnProcessHandle, IDLE_PRIORITY_CLASS))
				{
					char* lpMsgBuf;							// Log error to AEH
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);			
					char errorString[1024] = "System call SetPriorityClass() failed. Error from system: ";
					strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));			// Ends with a '\0'	
					eventManager.report(ACS_USA_FirstErrorCode 
							+ ACS_USA_SystemCallError,	  // 8703
							Key_perceivedSeverity_EVENT,	// Event
							ACS_USA_FaultCause,			      // AP INTERNAL FAULT
							"",		
							errorString,	
							"SYSTEM CALL FAILED",
							"", 0); 
					LocalFree(lpMsgBuf);  
				}	
			}
		}

	// Save the oldest record number and time in this analysis, to be
	// able to check if the event frequency is too high.
	//
	oldestRecordNoInAnalysis	= logrec->RecordNumber; 
	oldestRecordTimeInAnalysis	= logrec->TimeWritten;

	return ret;
}


//******************************************************************************
//	findInsertedStrings()
//******************************************************************************

ACS_USA_StatusType 
ACS_USA_Analyser::findInsertedStrings(
  TCHAR pEventStrings[],
  _EVENTLOGRECORD* pEventRecord, 
  ACS_USA_EventSource* pEventSource)
{
  ACS_USA_StatusType ret;
  _TCHAR szMessage[PARAM_LEN];

  if (pEventRecord->NumStrings > MAX_NUM_OF_PARAMS)
    pEventRecord->NumStrings = MAX_NUM_OF_PARAMS;

  // Get the parameters and build an array for the event
  if (pEventRecord->NumStrings > 0 && pEventSource->get_ParameterModule()) {
    // Allocate PARAM_LEN byte long strings which will room the params.
    if ((pEventRecord->NumStrings) > m_nNumOfParams)
      ResizeStringArray(pEventRecord->NumStrings);

    // Start offset for parameter string.
    _TCHAR* lpszTemp = (_TCHAR*)(LPBYTE)pEventRecord + pEventRecord->StringOffset;

    // Walk through each parameter.
    for (int nIdx = 0; nIdx < pEventRecord->NumStrings; nIdx++) {
      _TCHAR* lpszParam = m_lpszParams[nIdx];
      _TCHAR* lpszPtr = lpszTemp;
      DWORD dwParamLen = 0;
      DWORD dwResult, dwTemp;

      if(lpszParam == &m_szDummyParams[nIdx * 4])
      {
        // No valid storage space for the string
        break;
      }

      while (*lpszPtr) {
        dwTemp = 0;

        // If the parameter begins with one or two '%'  and the value that
        // follows is a numeric value, then the parameter is an message id
        // which will be replaced by an message text. The message string is
        // fetched by call FormatMessage with the message ID.
        if (*lpszPtr == _T('%')) {
          // Move to the start of the parameter "key", i.e. after the last '%'.
          while (*lpszPtr == _T('%'))
            lpszPtr++;

          // Set a reference where the parameter value begins and move to the end
          // of the paramter value.
          _TCHAR* lpszValue = lpszPtr;
          while (
            *lpszPtr &&
            *lpszPtr != _T('%') &&
            *lpszPtr != _T(' ') &&
            *lpszPtr != _T('\r') &&
            *lpszPtr != _T('\n') &&
            *lpszPtr != _T('\t'))
          {
            lpszPtr++;
            dwTemp++;
          }

          if (m_lpszParams[nIdx] != &m_szDummyParams[nIdx * 4]) {
            // If the parameeter "key" is a numeric value, it is a message
            // id that can be used to get an message text.
            if (*lpszValue >= _T('0') && *lpszValue <= _T('9')) {
              // Save the character before set a end-of-line character.
              _TCHAR ch = *lpszPtr;

              // Mark end of parameter.
              *lpszPtr = _T('\0');
              DWORD dwEventID = _ttol(lpszValue);

              // Lookup the value of the parameter.
              dwResult = ::FormatMessage(
                FORMAT_MESSAGE_FROM_HMODULE,
                pEventSource->get_ParameterModule(),
                dwEventID,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                szMessage,
                PARAM_LEN - 1,
                NULL);

              if (dwResult > 0) {
                // The max length of the parameter that contains the
                // message text is PARAM_LEN characters.
                if ( (dwParamLen + dwResult) >= PARAM_LEN)
                  dwTemp = PARAM_LEN - dwParamLen - 1;
                else
                  dwTemp = dwResult;

                // Append data to the current parameter.
                _tcsncpy(lpszParam + dwParamLen, szMessage, dwTemp);
                dwParamLen += dwTemp;
              }
              else {
                if ((dwParamLen + dwTemp) >= PARAM_LEN)
                  dwTemp = PARAM_LEN - dwParamLen - 1;

                // Append data to the current parameter.
                _tcsncpy(lpszParam+dwParamLen, lpszValue, dwTemp);
                dwParamLen += dwTemp;
              }

              // Restore the character at the position where we marked end-of-line.
              *lpszPtr = ch;
            }
            else {
              if ((dwParamLen + (dwTemp+1)) >= PARAM_LEN)
                dwTemp = PARAM_LEN - dwParamLen - 1;

              // Append data to the current parameter.
              _tcsncpy(lpszParam + dwParamLen, (_TCHAR*)(lpszValue - 1), dwTemp + 1);
              dwParamLen += (dwTemp + 1);
            }
          }
        }
        else {
          if (dwParamLen < PARAM_LEN)
            *(lpszParam + (dwParamLen++)) = *lpszPtr;
          lpszPtr++;
        }
      }
      //edanric
      lpszParam[ min(dwParamLen, PARAM_LEN - 1) ] = _T('\0');
      _sntprintf(pEventStrings, EVENT_MESSAGE_BUFFER_SIZE, _T("%s"), lpszParam);
      pEventStrings[EVENT_MESSAGE_BUFFER_SIZE - 1] = _T('\0');
      //end edanric

      lpszTemp = lpszPtr + sizeof(_TCHAR);
    }
  }

  DWORD dwResult = 0;
  HMODULE hModule = pEventSource->get_EventModule();
  while (hModule) {
    // Get the event text formatted with the parameters.
    dwResult = ::FormatMessage(
      FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ARGUMENT_ARRAY,
      hModule,
      pEventRecord->EventID,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      pEventStrings,
      EVENT_MESSAGE_BUFFER_SIZE,
      m_lpszParams);
    if (dwResult == 0)
      hModule = pEventSource->get_NextEventModule();
    else
      break;
  }

  if (dwResult == 0) {
    // Something went wrong when fetching event message strings.
/*
    char eventID[10] = "";
    ultoa (pEventRecord->EventID & 0x0000FFFF, eventID, 10);

    char* lpMsgBuf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0,
      NULL);

    error.setError(ACS_USA_EventStringError);
    error.setErrorText(pEventSource->m_szSource, eventID, lpMsgBuf);
    LocalFree(lpMsgBuf);

		ret = ACS_USA_error;
*/
    ret = ACS_USA_ok;
  }
  else {
    // Replace each non-visible character with a space.
    char* nonVisibleChar;
    for (int c = 1; c < 32; c++) {
      while (nonVisibleChar = strchr(pEventStrings, c))
	 	    memset(nonVisibleChar, ' ', 1);
    }
    c = 127;
	  while (nonVisibleChar = strchr(pEventStrings, c))
		  memset(nonVisibleChar, ' ', 1);
    ret = ACS_USA_ok;
  }

  for (int nIdx = 0; nIdx < pEventRecord->NumStrings; nIdx++) {
    if (m_lpszParams[nIdx])
      _stprintf(m_lpszParams[nIdx], _T("%%%d"), nIdx + 1);
  }

  return ret;
}

//******************************************************************************
//	retrieveInsertedStrings()
//******************************************************************************

ACS_USA_StatusType 
ACS_USA_Analyser::retrieveInsertedStrings(
								_EVENTLOGRECORD* const pEventRecord,
								ACS_USA_RecordInfo& recordInfo
								) const
{
    ACS_USA_StatusType ret = ACS_USA_ok;

    // Start offset for parameter string.
    _TCHAR* lpszStart = (_TCHAR*)(LPBYTE)pEventRecord + pEventRecord->StringOffset;

    // Walk through each parameter.
    for (int nIdx = 0; nIdx < pEventRecord->NumStrings; nIdx++) 
    {
		recordInfo.addString(  String(lpszStart, _tcslen(lpszStart)) );

        lpszStart += _tcslen( lpszStart );
		lpszStart += sizeof(_TCHAR); // for the '\0'
    }

    return ret;
}

//******************************************************************************
//	retrieveEventData()
//******************************************************************************

ACS_USA_StatusType 
ACS_USA_Analyser::retrieveEventData(
								_EVENTLOGRECORD* const pEventRecord,
								ACS_USA_RecordInfo& recordInfo
								) const
{
	ACS_USA_StatusType ret = ACS_USA_ok;

	// Copy data from record to structure
	if( recordInfo.addData( ((LPBYTE)pEventRecord + pEventRecord->DataOffset),
							pEventRecord->DataLength 
							) == false )
	{
		ret = ACS_USA_error;
	}

	return ret;
}

//******************************************************************************
//	addEventData()
//******************************************************************************

ACS_USA_RecordStatus
ACS_USA_Analyser::addEventData(const char* const logonTypeString, 
							   const char* const eventTypeString, 
							   TCHAR pEventStrings[],
							   const WORD numberOfStringsInRecord,
							   char* eventRecordContents)							   
{
	ACS_USA_RecordStatus ret = recordOK;

	// Add the event type text 
	//
	if (strcat(eventRecordContents, eventTypeString) == "")	 
	{														
		error.setError(ACS_USA_SystemCallError);				
		error.setErrorText("strcat() failed."); 
		ret = recordError;								
		return ret;
	}
	if (strcat(eventRecordContents, " \0") == "")	 
	{														
		error.setError(ACS_USA_SystemCallError);				
		error.setErrorText("strcat() failed."); 
		ret = recordError;								
		return ret;
	}
	
	// Add the login type
	//
	if (strcat(eventRecordContents, logonTypeString) == "")	// Add the logintype pattern, 
	{														// if any, for example 
		// LOGON32_LOGON_INTERACTIVE
		error.setError(ACS_USA_SystemCallError);				
		error.setErrorText("strcat() failed."); 
		ret = recordError;								
		return ret;
	}					
	if (strlen(logonTypeString)>0)
	{	
		if(strcat(eventRecordContents, " \0") == "")
		{   
			error.setError(ACS_USA_SystemCallError);				
			error.setErrorText("strcat() failed."); 
			ret = recordError;								
			return ret;
		}
	}
	
	// Add the event strings
	//
	if (numberOfStringsInRecord > 0) 
//		&& (eventStrings != NULL))	
	{
		if (strcat(eventRecordContents, pEventStrings) == "")	// Add the inserted  
		{														// strings if any.
			error.setError(ACS_USA_SystemCallError);				
			error.setErrorText("strcat() failed."); 
			ret = recordError;								
			return ret;
		}
        pEventStrings[0] = _T('\0');
	}
	
	if(strcat(eventRecordContents, "\n") == "")				// "\n" ends the log record.
	{   
		error.setError(ACS_USA_SystemCallError);				
		error.setErrorText("strcat() failed."); 
		ret = recordError;								
		return ret;
	}

	return ret;
}



//******************************************************************************
//	findLogonType()
//******************************************************************************

ACS_USA_StatusType
ACS_USA_Analyser::findLogonType(char* logonTypeString, const _EVENTLOGRECORD* nextRecord)
{
	ACS_USA_StatusType ret = ACS_USA_ok;

	// If the third string at the end of a Security event record is either 
	// "2","3","4" or "5", logonTypeString will be written to the end of
	// the EventRecordContents string. This string will be detected and an 		
	// alarm sent (if this is set up in ACS_USA_Config.acf).
	// The string constants written to logonTypeString are defined  
	// in WINBASE.H
	
	LPSTR	firstEventString  = 
		(LPSTR) ((LPBYTE)nextRecord + nextRecord->StringOffset);
	
	LPSTR	secondEventString  = 
		(LPSTR) ((LPBYTE)nextRecord + nextRecord->StringOffset 
		+ strlen(firstEventString) +1);				// +1 skips '/0'
	
													// The third string is the interesting 
	LPSTR	thirdEventString =						// one in the Security log						 
		(LPSTR) ((LPBYTE)nextRecord + nextRecord->StringOffset 
		+ strlen(firstEventString) + 1 
		+ strlen(secondEventString) + 1);							
	
	memset(logonTypeString, '\0', logonTypeStringLength);		// Clear string.

	if (strcmp(thirdEventString, "2") == 0)			// Logon strings defined					
	{	// Failed user logon.						// in WINBASE.H.		
		//
		if (strcpy(logonTypeString, "LOGON32_LOGON_INTERACTIVE") == "")	// Add string
		{   
			error.setError(ACS_USA_SystemCallError);			// strcpy failure				
			error.setErrorText("strcpy(logonTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else if (strcmp(thirdEventString, "3") == 0)
	{
		if (strcpy(logonTypeString, "LOGON32_LOGON_NETWORK") == "")
		{   
			error.setError(ACS_USA_SystemCallError);			// strcpy failure				
			error.setErrorText("strcpy(logonTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else if (strcmp(thirdEventString, "4") == 0)
	{
		if (strcpy(logonTypeString, "LOGON32_LOGON_BATCH") == "")
		{   
			error.setError(ACS_USA_SystemCallError);			// strcpy failure				
			error.setErrorText("strcpy(logonTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}	
	}
	else if (strcmp(thirdEventString, "5") == 0)
	{
		if (strcpy(logonTypeString, "LOGON32_LOGON_SERVICE") == "")
		{   
			error.setError(ACS_USA_SystemCallError);			// strcpy failure				
			error.setErrorText("strcpy(logonTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else if (strcmp(thirdEventString, "7") == 0)
		
	{	// "7" is received when failing to logon using pcAnywhere
		// on FORCE. The string is not defined in WINBASE.H. 
		//
		if (strcpy(logonTypeString, "LOGON32_LOGON_UNKNOWN") == "")
		{   
			error.setError(ACS_USA_SystemCallError);			// strcpy failure				
			error.setErrorText("strcpy(logonTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	return ret;
}

//******************************************************************************
//	findEventType()
//******************************************************************************

ACS_USA_StatusType 
ACS_USA_Analyser::findEventType(char* eventTypeString, const _EVENTLOGRECORD* nextRecord)
{
	ACS_USA_StatusType ret	= ACS_USA_ok;
	const int eventTypeBufferSize = 30;	// In the normal case, the event type is only
										// max 2 bytes. However, to be able to handle 
										// extremely erroneous record types, a much 
										// larger buffer size is chosen.
	char eventType [eventTypeBufferSize] = "";	
	memset(eventType, '\0', eventTypeBufferSize); 
	
	if (sprintf(eventType, "%u%s", nextRecord->EventType, "\0") < 1)
	{														// Sprintf failure
		error.setError(ACS_USA_SystemCallError);				
		error.setErrorText("sprintf(eventType, ...) failed."); 
		ret = ACS_USA_error;								
		return ret;
	}
	memset(eventTypeString, '\0', eventTypeStringLength);	// Clear string
	
	// Find event type in event data
	// as defined in WINNT.H
	//
	if (strcmp(eventType, "0") == 0)							
	{   
		if (strcpy(eventTypeString, "EVENTLOG_SUCCESS") == "")
		{   
			error.setError(ACS_USA_SystemCallError);		// strcpy failure				
			error.setErrorText("strcpy(eventTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else if (strcmp(eventType, "1") == 0)					
	{   
		if (strcpy(eventTypeString, "EVENTLOG_ERROR_TYPE") == "")
		{   
			error.setError(ACS_USA_SystemCallError);		// strcpy failure				
			error.setErrorText("strcpy(eventTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}					
	else if (strcmp(eventType, "2") == 0)					
	{   
		if (strcpy(eventTypeString, "EVENTLOG_WARNING_TYPE") == "")
		{   
			error.setError(ACS_USA_SystemCallError);		// strcpy failure				
			error.setErrorText("strcpy(eventTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else if (strcmp(eventType, "4") == 0)					
	{   
		if (strcpy(eventTypeString, "EVENTLOG_INFORMATION_TYPE") == "")
		{   
			error.setError(ACS_USA_SystemCallError);		// strcpy failure				
			error.setErrorText("strcpy(eventTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else if (strcmp(eventType, "8") == 0)					
	{   
		if (strcpy(eventTypeString, "EVENTLOG_AUDIT_SUCCESS") == "")	
		{   
			error.setError(ACS_USA_SystemCallError);		// strcpy failure				
			error.setErrorText("strcpy(eventTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else if (strcmp(eventType, "16") == 0)					// 0x0010 is Decimal 16
	{   
		if (strcpy(eventTypeString, "EVENTLOG_AUDIT_FAILURE") == "")	
		{   
			error.setError(ACS_USA_SystemCallError);		// strcpy failure				
			error.setErrorText("strcpy(eventTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	else
		// This string is not defined in WINNT.H:
	{   
		if (strcpy(eventTypeString, "EVENTLOG_UNKNOWN_TYPE") == "")
		{   
			error.setError(ACS_USA_SystemCallError);		// strcpy failure				
			error.setErrorText("strcpy(eventTypeString, ...) failed."); 
			ret = ACS_USA_error;								
			return ret;
		}
	}
	return ret;
}


//******************************************************************************
//	findBootTimeInLog()
//******************************************************************************

ACS_USA_StatusType 
ACS_USA_Analyser::findBootTimeInLog(String ACS_USA_alarmSourceNameDefault, 
								  ACS_USA_Time interpreter, 
								  DWORD& latestBootTimeStamp,
								  const DWORD latestRecordNumbers[6],
								  const DWORD recordsPrioThreshold,
								  const DWORD secondsPrioThreshold,
								  String	logFileName)
{
	ACS_USA_StatusType  ret			= ACS_USA_ok;
    ACS_USA_Flag 	    notFinish = ACS_USA_True;

    bufferEnd				= logBufferSize;
    logIndex				= bufferEnd;

	// Open the Security log.
	//
	char	computerNameBuffer	[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD	computerNameLength = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName((char*)&computerNameBuffer, &computerNameLength);    
	HANDLE eventLogHandle = OpenEventLog(computerNameBuffer, logFileName.data());
	
	if (eventLogHandle == NULL)
	{       
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|			// buffersize(255)-ignores inserted 
													// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call OpenEventLog() failed. Machine name: ";
		strcat(errorString, computerNameBuffer);
		strcat(errorString, ". Error from system: ");	
		strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));					// Ends with a '\0'
		
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);						// Necessary according to the 
													// FormatMessage() manual page.
		return ACS_USA_error;
    }   
	//
    //------------- Allocate buffers -------------
    //
	// logBuffer will hold the formatted (sprintf:ed) record  
	// strings read from the sizeBufPtr buffer defined below. 
	//
	try
	{
	    logBuffer = new ACS_USA_LogBuffer[logBufferSize];	// Default from parameter file: 2.3 Mb
	}
	catch(...)												// Catches "bad_alloc" exception 
															// (generated if stl headers are used)
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::findBootTime().");
		return ACS_USA_error;	
	}
	if (logBuffer == NULL)									// If catching "bad_alloc" 
															// wasn't enough
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::findBootTime().");
		return ACS_USA_error;	
	}

	// recordBuffer will hold one formatted record string at a time for analysis.
	//
	try
	{
		recordBuffer = new ACS_USA_RecordBuffer[recordBufferSize];
	}
	catch(...)												// Catches "bad_alloc" exception 
															// (generated if stl headers are used)
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::findBootTime().");
		delete [] logBuffer;
		return ACS_USA_error;
	}
	if (recordBuffer == NULL) 								// If catching "bad_alloc" 
															// wasn't enough
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::findBootTime().");
		delete [] logBuffer;
		return ACS_USA_error;	
	}
	//
	// Find out where to start the start-up analysis.
	// In start-up analysis we are reading the latest record first.
	// 
	DWORD startRecord = 0;
	if (seekFromEnd(eventLogHandle, 0, 
					startRecord, Activation_startUp) == ACS_USA_error)
	{                   
		delete [] logBuffer;
		delete [] recordBuffer;
		return ACS_USA_error;
	}	
	
	// The buffer to put the events read from   
	// the event log in (sizeBuf/logRec) shall 
	// have space for the max size of the
	// logBufferSize/logParam set as PHA parameter.
	//
	LPBYTE sizeBufPtr;
	try
	{
		sizeBufPtr = new BYTE [logBufferSize];			// Default 2.3 Mb	
	}
	catch(...)											// Catches "bad_alloc" exception 
														// (generated if stl headers are used)
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::findBootTime().");
		delete [] logBuffer;
		delete [] recordBuffer;
		return ACS_USA_error;	
	}
	if (sizeBufPtr == NULL)								// If catching "bad_alloc" 
														// wasn't enough
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Analyser::findBootTime().");
		delete [] logBuffer;
		delete [] recordBuffer;
		return ACS_USA_error;	
	}
	//
    //---------- Get records from log file -------------
	//
    //
	_EVENTLOGRECORD* logrec = (_EVENTLOGRECORD*) sizeBufPtr; 
	bytesRead = 0;
	String  readOrder = "EVENTLOG_BACKWARDS_READ"; 

	switch(getRecordsFromEventlog(eventLogHandle,		// Gets up to 7000 records
				logrec,	startRecord, bytesRead, readOrder, 
				latestRecordNumbers, "", 0, 0)) 
	{
		case recordEOF:
			notFinish			= ACS_USA_False;
			ret					= ACS_USA_ok;
			recordBuffer[0]		= 0; 
			logBuffer[0]		= 0;
			break;

		case recordError:
			notFinish = ACS_USA_False; 
			ret = ACS_USA_error;
			break;

		default:
			break;										// Do nothing, everything is ok.
	}
	//
	// The getRecord member function sets latestBootStamp (if the boot record 
	// can be found). If not, below the last read time stamp is set   
	// as the boot stamp, since we will never read beyond that record in long 
	// term analysis anyway.
	//
	if (notFinish)
	{
		ACS_USA_RecordStatus recstat = getRecord(startRecord, Activation_startUp, logrec, 
											logFileName, ACS_USA_alarmSourceNameDefault, 
											latestBootTimeStamp,
											latestRecordNumbers,
											recordsPrioThreshold,
											secondsPrioThreshold);
		if ((recstat == recordOK)
			||(recstat == recordEOF))
		{
			ret = ACS_USA_ok;
		}
		else													// Record error
		{
			ret = ACS_USA_error;
		}
	}
	//----------------- Clean up -----------------
	//
    // Release buffers
    //
    delete [] logBuffer;
    delete [] recordBuffer;
	delete [] sizeBufPtr;    
    //
    // Close log file
    //
	BOOL logClosed = CloseEventLog(eventLogHandle);		
	if (!logClosed)	
	{        
		// If log cannot be closed
		//
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|			// buffersize(255)-ignores inserted 
													// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call CloseEventLog() failed. Error from system: ";
		strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));					// Ends with a '\0'

		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);						// Necessary according to the 
													// FormatMessage() manual page.
		return ACS_USA_error;
    }
    return ret;
}

//******************************************************************************
//	findBootTime()
//******************************************************************************

// We want to handle the following situations:
// 1. The node was rebooted and many new events has been generated
//    before USA started this analysis.
//    ACTION: Use BootEvent as starting point for analysis. 
//    Delete the tmp file.
// 2. The USA service has been restarted (taken on-line)
//    and the boot event is not found among the
//    analysed events.
//    ACTION: Use ReStartedEvent as starting point.
// 3. Eventlog was cleared (both the above events were missing.)
//    ACTION: If previously stored time value in registry is available
//    we use that. Otherwise we use the current time as starting point.


ACS_USA_StatusType 
ACS_USA_Analyser::findBootTime(String ACS_USA_alarmSourceNameDefault, 
							   ACS_USA_Time interpreter, 
							   DWORD& latestBootTimeStamp,
							   const DWORD latestRecordNumbers[6],
							   const DWORD recordsPrioThreshold,
							   const DWORD secondsPrioThreshold)
{
	DWORD LastUSAstopTimeStamp;
	DWORD LastUSAstopTimeStampSize = 4;
	HKEY hResultKey;
	DWORD lResult;
	
	lResult = RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,	
					ACS_USA_applicationKey,
					0, 
					KEY_READ, 
					&hResultKey
					);
	
	if (lResult == ERROR_SUCCESS) 
	{
		// Read the LastUSAstopTimeStamp value from the registry
		TCHAR* lpValueName = "LastUSAstopTime";
		
		lResult = RegQueryValueEx(
						hResultKey,	
						lpValueName, 
						NULL, 
						NULL, 
						(unsigned char*)&LastUSAstopTimeStamp, 
						&LastUSAstopTimeStampSize
						);
		
		if (lResult == ERROR_SUCCESS) 
		{
			RegCloseKey(hResultKey);
		}
		else if (lResult == ERROR_FILE_NOT_FOUND)
		{
			// Value is not found
			LastUSAstopTimeStamp = 0;
		}
		else
		{
			String errorString = "Failed to read value ";
			errorString += lpValueName;
			error.setError(ACS_USA_SystemCallError);
			error.setErrorText(errorString.c_str());			
			return ACS_USA_error;
		}
	}
	else if (lResult == ERROR_FILE_NOT_FOUND)
	{
		// Key not found - create it
		lResult = RegCreateKeyEx(
						HKEY_LOCAL_MACHINE, 
						ACS_USA_applicationKey, 
						0,
						"",
						REG_OPTION_NON_VOLATILE,
						KEY_WRITE,
						NULL,
						&hResultKey,
						NULL
						);
		
		if (lResult == ERROR_SUCCESS)
		{
			RegCloseKey(hResultKey);
			LastUSAstopTimeStamp = 0;
		}
		else
		{
			String errorString = "Failed to create key ";
			errorString += ACS_USA_applicationKey;
			error.setError(ACS_USA_SystemCallError);
			error.setErrorText(errorString.c_str());			
			return ACS_USA_error;
		}
	}
	else
	{
		String errorString = "Failed to open key ";
		errorString += ACS_USA_applicationKey;
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString.c_str());			
		return ACS_USA_error;
	}
	
	DWORD RestartEventTimeStamp=0;
	
	findBootTimeInLog(
			ACS_USA_alarmSourceNameDefault,
			interpreter,
			RestartEventTimeStamp, 
			latestRecordNumbers,
			recordsPrioThreshold, 
			secondsPrioThreshold, 
			"Application"
			);
	
	DWORD BootEventTimeStamp = 0;
	
	findBootTimeInLog(
			ACS_USA_alarmSourceNameDefault,
			interpreter,
			BootEventTimeStamp, 
			latestRecordNumbers,
			recordsPrioThreshold, 
			secondsPrioThreshold, 
			"System"
			);
	
	// Check if we got a LastUSAstopTimeStamp value
	if (LastUSAstopTimeStamp > 0)
	{
		latestBootTimeStamp = LastUSAstopTimeStamp;
	}
	
	// Check if USA was just restarted (taken on-line)
	if (RestartEventTimeStamp > latestBootTimeStamp)
	{
		latestBootTimeStamp = RestartEventTimeStamp;
	}
	
	// Check if the node was rebooted
	if (BootEventTimeStamp > latestBootTimeStamp)
	{
		latestBootTimeStamp = BootEventTimeStamp;
		
		// delete tmp-file ????
	}
	
	// If everything fails! (Should never happen)
	if (latestBootTimeStamp == 0)
	{
		// Get the current time
		time((long*)&latestBootTimeStamp);
	}
	
	// Make sure that we detect events logged the same sec as time stamp
	latestBootTimeStamp--;
	
	// Debug printout
	struct tm* starttime;

	//Test
	/*fstream f("c:\\USALog.txt",ios::out);
	starttime = localtime((long *)&RestartEventTimeStamp );
	string s ;
	s = "RestartEventTimeStamp is: " ;
	f << s;
	f << asctime(starttime);
	printf("RestartEventTimeStamp is: %s", asctime(starttime));
	
	starttime = localtime((long *)&BootEventTimeStamp );
	s = "BootEventTimeStamp is: " ;
	f<< s;
	f << asctime(starttime);
	printf("BootEventTimeStamp is: %s", asctime(starttime));
	
	starttime = localtime((long *)&LastUSAstopTimeStamp );
	s = "LastUSAstopTimeStamp is: ";
	f<< s;
	f << asctime(starttime);
	printf("LastUSAstopTimeStamp is: %s", asctime(starttime));
	
	starttime = localtime((long *)&latestBootTimeStamp );
	s = "The starting point for analysis of the logs is: ";
	f<< s;
	f << asctime(starttime);
	
	f.close();
	*/
	starttime = localtime((long *)&latestBootTimeStamp );
	printf("The starting point for analysis of the logs is: %s", asctime(starttime));
	
	return ACS_USA_ok;
}


//******************************************************************************
//	ResizeStringArray()
//******************************************************************************
BOOL
ACS_USA_Analyser::ResizeStringArray(const short nNumOfStrings) {
  for (int nIdx = m_nNumOfParams; nIdx < nNumOfStrings; nIdx++) {
    // String not already allocated.
    if (m_lpszParams[nIdx] == &m_szDummyParams[nIdx * 4]) {
      m_lpszParams[nIdx] = (_TCHAR*)::HeapAlloc(
        ::GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        PARAM_LEN);
      if (!m_lpszParams[nIdx]) {
        m_lpszParams[nIdx] = &m_szDummyParams[nIdx * 4];
        return FALSE;
      }
      else
        m_nNumOfParams++;
    }
  }
  return TRUE;
}


//******************************************************************************
//	FreeRecordInfoBuffer
//******************************************************************************
void
ACS_USA_Analyser::FreeRecordInfoBuffer()
{	
	ACS_USA_RecordInfoBuffer::iterator iElem = recordInfoBuffer.begin();
	for( ; iElem != recordInfoBuffer.end(); iElem++)
	{
		try
		{	
			if( NULL != *iElem )
			{
				delete *iElem;
				*iElem = NULL;
			}
		}
		catch(...)
		{
		}
	}

	recordInfoBuffer.clear();
}
