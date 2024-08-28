//*****************************************************************************

// NAME
//      ACS_USA_SyslogAnalyser.cpp

// COPYRIGHT Ericsson AB, Sweden 1995, 1998-1999, 2003-2004.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.

// .DESCRIPTION
//      Service main program.

// DOCUMENT NO
//	    190 89-CAA 109 0545

// AUTHOR 
// 	    1995-08-07 by ETX/TX/T XKKHEIN

// REV	DATE		NAME 		 DESCRIPTION
// A		950829	XKKHEIN	 First version.
// B		981119	UABDMT	 Ported to Windows NT4.0.
// C		990609	UABDMT	 Converted USA to an NT service.
// D    030212  UABPEK   New AP VIRUS alarms.
// E	  040429	QVINKAL	 Removal of RougeWave Tools.h++.
// F    041223  UABPEK   Supervision of three new event logs.
// G    080215  EKAMSBA  Auto cease alarm.

//******************************************************************************

#include <signal.h>
#include <stdlib.h>
#include <time.h> 

#include <iostream>
#include <windows.h>
#include <process.h> 
#include <tchar.h>
#include <string>
#include <ACS_TRA_trace.H>
#include "ACS_USA_Global.h"
#include "ACS_USA_Event.h"
#include "ACS_USA_EventManager.h"
#include "ACS_USA_Control.h"
#include "ACS_USA_AutoCeaseManager.h"

#include "ACS_PRC_Process.H"
#include "ACS_ExceptionHandler.H"
using namespace std;

typedef string String;

SERVICE_STATUS			USA_serviceStatus;
SERVICE_STATUS_HANDLE	USA_serviceStatusHandle;

void				USA_serviceError			(char* string, DWORD status);  
void				USA_serviceInstall			();						
void				USA_serviceRemove			();						
DWORD				USA_serviceInitialization	(DWORD argc, LPSTR *argv);
void WINAPI			USA_serviceStart			(DWORD argc, LPSTR *argv);
unsigned int WINAPI USA_serviceMain				(void* forThread);
void WINAPI			USA_serviceControlHandler	(DWORD opcode);
ACS_USA_StatusType	saveLatestRecordNumbers		();
BOOL				checkIfLogChanged			(String logName);
BOOL				checkIfLogCleared			(String logName);
int					enterReleaseVersionWaitState(DWORD waitInterval);
int					enterDebugVersionWaitState	(DWORD waitInterval);


//////////////////////////////////////////////////////////////////////////
// Global objects. These are accessible anywhere after including 
// appropriate inlude file
//////////////////////////////////////////////////////////////////////////
ACS_USA_Error 	     error;
ACS_USA_EventManager eventManager;

// Global variables. Used only in this source file:
//
ACS_USA_Flag fLongterm = ACS_USA_False;		// modified by alarm handler
ACS_USA_Flag notFinish = ACS_USA_True;		// modified by termination handler

unsigned long	ServiceThreadHandle			= 0; // Service Thread Handle
unsigned		longTermAnalysisInterval	= 0; 
unsigned		shortTermAnalysisInterval	= 0; 
UINT			timerId						= 0;
BOOL			debugFlag					= FALSE;
String		logNameForNotification[6]	= {"System", "Application", "Security", "Directory Service", "DNS Server", "File Replication Service"};
String		triggedLogName				= "";
HANDLE		ACS_USA_SyslogAnalyserStopEvent			= NULL;
HANDLE		systemLogChangeEvent					= NULL; 
HANDLE		applicationLogChangeEvent				= NULL; 
HANDLE		securityLogChangeEvent					= NULL;
// UABPEK_P1D
HANDLE    directoryServiceLogChangeEvent = NULL;
HANDLE    DNSServerLogChangeEvent = NULL;
HANDLE    fileReplicationServiceLogChangeEvent = NULL;
HANDLE		myOwnProcessHandle						= NULL;
DWORD			ACS_USA_DefaultProcessPrioClass			= 0;
DWORD			latestSystemLogRecordNumber				= 0;
DWORD			latestApplicationLogRecordNumber		= 0;
DWORD			latestSecurityLogRecordNumber			= 0;
// UABPEK_P1D
DWORD     latestDirectoryServiceLogRecordNumber	= 0;
DWORD     latestDNSServerLogRecordNumber = 0;
DWORD     latestFileReplicationServiceLogRecordNumber	= 0;
DWORD			changedLog								= 0;
BOOL			firstShortTerm							= TRUE;
BOOL			firstLongTerm							= TRUE;
BOOL			instantAnalysis							= FALSE;

// UABPEK_P1D
// For the System, Application, Security, Directory Service, DNS Server, and File Replication Service logs.
DWORD latestRecordNumbers[6] = {0, 0, 0, 0, 0, 0};


//******************************************************************************
//	Trace point definitions
//******************************************************************************
ACS_TRA_trace traceMain = ACS_TRA_DEF("ACS_USA_Main", "C40");

ACS_TRA_trace ACS_USA_serviceStop = ACS_TRA_DEF("ACS_USA_serviceStop", "C50");
ACS_TRA_trace ACS_USA_serviceStart = ACS_TRA_DEF("ACS_USA_serviceStart", "C50");

const char* const ACS_USA_traceStart	= "USA started";
const char* const ACS_USA_traceExit		= "USA is exiting";
const char* const ACS_USA_traceActShort = "Activation: short term";
const char* const ACS_USA_traceActLong  = "Activation: long term";

const char* const ACS_USA_traceActInstant		= "Activation: instant/notification";
const char* const ACS_USA_traceServiceError		= "USA_serviceError received. Exiting.";
const char* const ACS_USA_traceTimerThreadStart = "Timer Thread started";
const char* const ACS_USA_traceTimerThreadEnd	= "Timer Thread finished";
const char* const ACS_USA_traceExitWait			= "USA is waiting to be killed";
const char* const ACS_USA_traceExitSysCallError = "USA exits after system call failure";
const char* const ACS_USA_traceEvlogError		= "Cannot receive Event Log notifications";
const char* const ACS_USA_tracePrioLow			= "Decreasing process priority";
const char* const ACS_USA_tracePrioDefault		= "Setting process priority to default";
const char* const ACS_USA_traceAutoCeaseDied	= "Auto cease thread died";


//******************************************************************************
//	timerThreadFunc
//******************************************************************************

void timerThreadFunc(void*)
{
	MSG* msg = new MSG;						// Putting the msg struct on the heap
											// instead of on the stack, to make
											// Purify shut up!

	UINT idEvent = 0, wMsgFilterMin = 0, wMsgFilterMax = 0;

	if (ACS_TRA_ON(traceMain)) 
	{
		ACS_TRA_event(&traceMain, ACS_USA_traceTimerThreadStart);
	}
    
	timerId = SetTimer (NULL, 
						idEvent,							// Ignored  
						longTermAnalysisInterval * 1000,	// Millisec
						NULL);	
	
	if (timerId == 0)						// SetTimer failure
	{
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
											// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call SetTimer() failed. Error from system: ";
		strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));			// Ends with a '\0'

		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);				// Necessary according to the 
											// FormatMessage() manual page.
		delete msg;
	    _endthread();  
	}

	
	// Pick the timer message from the message queue
	//
	while(!fLongterm) 
	{			
		if (GetMessage (msg,					// Message structure
						NULL,					// Handle of window
						wMsgFilterMin,			// First message  
						wMsgFilterMax) == -1)
		{										// GetMessage() failure
			char* lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
												// control sequences such as char 13
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);
			
			char errorString[1024] = "System call GetMessage() failed. Error from system: ";
			strncat(errorString, lpMsgBuf, 
					strlen(lpMsgBuf));			// Ends with a '\0'
			
			error.setError(ACS_USA_SystemCallError);
			error.setErrorText(errorString);			
			LocalFree(lpMsgBuf);
			break;								// break out of while
			
		} 
		else									// GetMessage() ok
		{
			switch (msg->message)
			{	   
			case WM_TIMER:						// Timer expired					
				{
					fLongterm = ACS_USA_True;	// Set the long term flag,
												// next analysis is to be 
												// a long term analysis.
					break;
				}			
			default:  
				{ 
					fLongterm = ACS_USA_True;	// Set the long term flag -
												// better to get out of the 
												// loop than to hang around  
												// here if accidentaly 
												// not WM_TIMER.
					break;
				}
			}
		}
	}											// end of while
	
	if (ACS_TRA_ON(traceMain)) 
	{
		ACS_TRA_event(&traceMain, ACS_USA_traceTimerThreadEnd);
	}	
	
	delete msg;
	_endthread();  
	
}



//************************************************************************
//	alarmHandler
//************************************************************************



//************************************************************************
//	termHandler
//************************************************************************


DEBUG_DECLARE


//******************************************************************************
//	USA_serviceMain()
//******************************************************************************
unsigned int WINAPI USA_serviceMain(void* forThread)	
											// This function is started by the 
											// NT Service Control Manager.
{
    ACS_USA_Control 	control;		
    ACS_USA_StatusType 	ret = ACS_USA_ok;
 
	// Initialise directory path constants  
    // and initialise parameters from AP  
	// parameter handling.
	//
	if ((control.initPathConstants() == ACS_USA_error)
		|| (control.initParameters() == ACS_USA_error)) 
	{
		if ((error.getError() == ACS_USA_SyntaxError)
			||(error.getError() == ACS_USA_ParameterHandlingError) 
			||(error.getError() == ACS_USA_FileIOError)
			||(error.getError() == ACS_USA_APeventHandlingError))	
			//
			// If the problem is AEH (e.g. Registry problems), no 8701 alarm will get   
			// through to the Eventlog. Instead USA will just hang, waiting to be  
			// stopped. Not a nice situation, but it is better than getting restarted  
			// time after time, eventually leading to cyclic reboots.
		{
			if (debugFlag)
			{							// Debug version is executing.
				return(ACS_USA_exitError);
				// ExitProcess(ACS_USA_exitError);			
			}
			else						// Release version is executing.
			{							// Suspend the USA process since the 
										// event requires stoping of USA via PRC.
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExitWait);
				}
			
				WaitForSingleObject(ACS_USA_SyslogAnalyserStopEvent, INFINITE);
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExit);
				}
				Sleep(3000);			// To give the ServiceControlManager 
										// a chance to catch up
				return(ACS_USA_exitError);
				// ExitProcess(ACS_USA_exitError);
			}
		}
	}
    
    // Configure USA from ACF
	//
    if (control.configure() == ACS_USA_error) 
	{
		
		if (error.getError() == ACS_USA_TmpFileIOError) 
			return(ACS_USA_exitError);	// We expect USA to be restarted which will
										// cause USA to create a new USA.TMP file.
		
		if ((error.getError() == ACS_USA_SyntaxError)
			||(error.getError() == ACS_USA_ParameterHandlingError) 
			||(error.getError() == ACS_USA_FileIOError) )
		{

			if (debugFlag)
			{							// Debug version is executing.
			    return(ACS_USA_exitError);
				// ExitProcess(ACS_USA_exitError);
			}
			else						// Release version is executing.
			{							// Suspend the USA process since the 
										// event requires stopping of USA via PRC.
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExitWait);
				}

				WaitForSingleObject(ACS_USA_SyslogAnalyserStopEvent, INFINITE);
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExit);
				}				
				Sleep(3000);			// To give the ServiceControlManager 
										// a chance to catch up
				return(ACS_USA_exitError);
				// 	ExitProcess(ACS_USA_exitError);
			}
		}		

    }
    
    // Start AutoCeaseManager
    //
    try
    {
        if( (ACS_USA_AutoCeaseManager::getInstance()).startAutoCeasingTask() == ACS_USA_False )
        {
            if (debugFlag)
            {                           // Debug version is executing.
                return(ACS_USA_exitError);
                // ExitProcess(ACS_USA_exitError);
            }
            else                        // Release version is executing.
            {                           // Suspend the USA process since the 
                // event requires stopping of USA via PRC.
                if (ACS_TRA_ON(traceMain)) 
                {
                    ACS_TRA_event(&traceMain, ACS_USA_traceExitWait);
                }

                WaitForSingleObject(ACS_USA_SyslogAnalyserStopEvent, INFINITE);
                if (ACS_TRA_ON(traceMain)) 
                {
                    ACS_TRA_event(&traceMain, ACS_USA_traceExit);
                }               
                Sleep(3000);            // To give the ServiceControlManager 
                // a chance to catch up
                return(ACS_USA_exitError);
                //  ExitProcess(ACS_USA_exitError);
            }
        }
    }
    catch( ... )
    {               
        char errorString[1024] = "Exception from ACS_USA_AutoCeaseManager::startAutoCeasingTask()";
        eventManager.report(ACS_USA_FirstErrorCode 
            + ACS_USA_SystemCallError,     // 8703
            Key_perceivedSeverity_EVENT,   // Event
            ACS_USA_FaultCause,            // AP INTERNAL FAULT
            "",     
            errorString,  
            "SYSTEM CALL FAILED",
            "", 0); 

        return(ACS_USA_exitError);
    }

    

    // Get analysis intervals
    //
  longTermAnalysisInterval  = control.getLongInterval();
  shortTermAnalysisInterval = control.getShortInterval();
 
  // Save default process priority to be able to make sure the
  // process prio is default, apart from in long term analysis
  //  
  myOwnProcessHandle	= GetCurrentProcess();		// Fetch handle to 'myself'
  ACS_USA_DefaultProcessPrioClass = GetPriorityClass(myOwnProcessHandle); 
  if (ACS_USA_DefaultProcessPrioClass == 0)			// Priority reading error  
  {										
	  char* lpMsgBuf;								// Log error to AEH
	  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				    FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
													// control sequences such as char 13
					FORMAT_MESSAGE_FROM_SYSTEM|
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, GetLastError(), 0, 
					(LPTSTR)&lpMsgBuf, 0, NULL);			
	  char errorString[1024] = "System call GetPriorityClass() failed. Setting USA process priority class to NORMAL. Error from system: ";
	  strncat(errorString, lpMsgBuf, 
			  strlen(lpMsgBuf));					// Ends with a '\0'	
	  eventManager.report(ACS_USA_FirstErrorCode 
						  + ACS_USA_SystemCallError,    // 8703
						  Key_perceivedSeverity_EVENT,	// Event
						  ACS_USA_FaultCause,		        // AP INTERNAL FAULT
						  "",		
						  errorString,	
						  "SYSTEM CALL FAILED",
						  "", 0); 
	  LocalFree(lpMsgBuf);  
	  ACS_USA_DefaultProcessPrioClass = NORMAL_PRIORITY_CLASS; // To be sure
  } 


  // Create notification events for the six event logs
  systemLogChangeEvent			= CreateEvent(NULL, TRUE, FALSE, NULL);  
  applicationLogChangeEvent		= CreateEvent(NULL, TRUE, FALSE, NULL);	 
  securityLogChangeEvent		= CreateEvent(NULL, TRUE, FALSE, NULL);
  // UABPEK_P1D
  directoryServiceLogChangeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  DNSServerLogChangeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  fileReplicationServiceLogChangeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if ((systemLogChangeEvent	== NULL)
    || (applicationLogChangeEvent == NULL) 
	  || (securityLogChangeEvent	== NULL)
    // UABPEK_P1D
    || (directoryServiceLogChangeEvent == NULL)
    || (DNSServerLogChangeEvent == NULL)
    || (fileReplicationServiceLogChangeEvent == NULL))
  {										
	  char* lpMsgBuf;								// Log error to AEH
	  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				    FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
													// control sequences such as char 13
					FORMAT_MESSAGE_FROM_SYSTEM|
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, GetLastError(), 0, 
					(LPTSTR)&lpMsgBuf, 0, NULL);			
	  char errorString[1024] = "System call CreateEvent() failed. Error from system: ";
	  strncat(errorString, lpMsgBuf, 
			  strlen(lpMsgBuf));					// Ends with a '\0'	
	  eventManager.report(ACS_USA_FirstErrorCode 
						  + ACS_USA_SystemCallError,    // 8703
						  Key_perceivedSeverity_EVENT,	// Event
						  ACS_USA_FaultCause,		        // AP INTERNAL FAULT
						  "",		
						  errorString,	
						  "SYSTEM CALL FAILED",
						  "", 0); 
	  LocalFree(lpMsgBuf);
	  if (ACS_TRA_ON(traceMain)) 
	  {
		  ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
	  }
	  ExitProcess(ACS_USA_exitError);				// Might as well die 
													// and get restarted
  } 
 

  // Set thread pointers and thread sync mechanisms
  //
  void (*func)(void*);			// Set function pointers to the functions 
  func = &timerThreadFunc;		// to execute in separate threads.		
  _beginthread(func, 0, NULL);	// Start a timer thread.
	

    if (ACS_TRA_ON(traceMain)) {
	ACS_TRA_event(&traceMain, ACS_USA_traceStart);
    }


    //--------------------- Operation loop -----------------------

	// int slask= 0;    // For Purify testing only

	int originalStartOffset = control.getStartOffset();

	// Check when the latest boot took place
	//
	if (control.operate(Activation_startUp, "", latestRecordNumbers) == ACS_USA_error) 
	{
		// Do nothing. Something went wrong and we probably didn't find 
		// the latest boot event. However, this does not prevent us from 
		// continuing execution at this stage.
	}
	

    while(notFinish == ACS_USA_True) 
	{	
		if (fLongterm == ACS_USA_True) 
		{
			//---- Do the long term analysis ----
			//
			USA_DEBUG(logMsg("MAIN:Performing LONG term analysis\n"));
			fLongterm = ACS_USA_False; 
			if (ACS_TRA_ON(traceMain)) 
			{
				ACS_TRA_event(&traceMain, ACS_USA_traceActLong);
			}
			if (saveLatestRecordNumbers() != ACS_USA_ok)
			{						
				notFinish = ACS_USA_False;
			}

			// Set process priority to low during long term analysis
			// to avoid stealing CPU-time from more important activities.
			//
			if (ACS_TRA_ON(traceMain)) 
			{
				ACS_TRA_event(&traceMain, ACS_USA_tracePrioLow);
			}
			SetPriorityClass(myOwnProcessHandle, IDLE_PRIORITY_CLASS);
			if (firstLongTerm)						// First time longterm term 
			{ 										// is performed 
				ACS_USA_ActivationType longTermAnalysisType = Activation_longTerm;
				if (control.getRaisedAlarmOnEventFrequency() == FALSE)
				{									// If no alarm has previously 
													// been sent on 'too high event 
													// frequency' it is ok to 
													// run Activation_FirstLongTerm
													// instead of Activation_longTerm.
					longTermAnalysisType = Activation_FirstLongTerm;
				}				
				if ((ret = control.operate(longTermAnalysisType,
										   triggedLogName,
										   latestRecordNumbers)) != ACS_USA_ok)
				{
					if (ret == ACS_USA_error) 
					{
						notFinish = ACS_USA_False;
					}
					if (ACS_TRA_ON(traceMain))				
					{
						ACS_TRA_event(&traceMain, ACS_USA_tracePrioDefault);
					}								// Set prio back to default
					SetPriorityClass(myOwnProcessHandle, ACS_USA_DefaultProcessPrioClass);
				}
				firstLongTerm = false;				// First long term performed
			}
			else
			{
				if ((ret = control.operate(Activation_longTerm, 
										   triggedLogName,
										   latestRecordNumbers)) != ACS_USA_ok)
				{
					if (ret == ACS_USA_error) 
					{
						notFinish = ACS_USA_False;
					}
					if (ACS_TRA_ON(traceMain))				
					{
						ACS_TRA_event(&traceMain, ACS_USA_tracePrioDefault);
					}										// Set prio back to default
					if (!SetPriorityClass(myOwnProcessHandle, ACS_USA_DefaultProcessPrioClass)) 
					{
						char* lpMsgBuf;						// Log error to AEH
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
									FORMAT_MESSAGE_MAX_WIDTH_MASK|// buffersize(255)-ignores inserted 
																  // control sequences such as char 13
									FORMAT_MESSAGE_FROM_SYSTEM|
									FORMAT_MESSAGE_IGNORE_INSERTS,
									NULL, GetLastError(), 0, 
									(LPTSTR)&lpMsgBuf, 0, NULL);			
						char errorString[1024] = "System call SetPriorityClass() failed. Error from system: ";
						strncat(errorString, lpMsgBuf, 
								strlen(lpMsgBuf));					  // Ends with a '\0'	
						eventManager.report(ACS_USA_FirstErrorCode 
											+ ACS_USA_SystemCallError,    // 8703
											Key_perceivedSeverity_EVENT,	// Event
											ACS_USA_FaultCause,		        // AP INTERNAL FAULT
											"",		
											errorString,	
											"SYSTEM CALL FAILED",
											"", 0); 
						LocalFree(lpMsgBuf);  
					}			
				} 
			}
			if (ret == ACS_USA_ok)			
			{
				if (ACS_TRA_ON(traceMain))				
				{
					ACS_TRA_event(&traceMain, ACS_USA_tracePrioDefault);
				}										// Set prio back to default
				if (!SetPriorityClass(myOwnProcessHandle, ACS_USA_DefaultProcessPrioClass)) 
				{
					char* lpMsgBuf;								// Log error to AEH
					FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
								FORMAT_MESSAGE_MAX_WIDTH_MASK|// buffersize(255)-ignores inserted 
															  // control sequences such as char 13
								FORMAT_MESSAGE_FROM_SYSTEM|
								FORMAT_MESSAGE_IGNORE_INSERTS,
								NULL, GetLastError(), 0, 
								(LPTSTR)&lpMsgBuf, 0, NULL);			
					char errorString[1024] = "System call SetPriorityClass() failed. Error from system: ";
					strncat(errorString, lpMsgBuf, 
							strlen(lpMsgBuf));					  // Ends with a '\0'	
					eventManager.report(ACS_USA_FirstErrorCode 
										+ ACS_USA_SystemCallError,    // 8703
										Key_perceivedSeverity_EVENT,	// Event
										ACS_USA_FaultCause,		        // AP INTERNAL FAULT
										"",		
										errorString,	
										"SYSTEM CALL FAILED",
										"",0); 
					LocalFree(lpMsgBuf);  
				}
				_beginthread(func, 0, NULL);		// Start executing "func"

				// If no new events - time to wait.
				if ((!checkIfLogChanged(logNameForNotification[0]))		// System log
					&& (!checkIfLogChanged(logNameForNotification[1]))	// Application log
					&& (!checkIfLogChanged(logNameForNotification[2]))	// Security log
          // UABPEK_P1D
					&& (!checkIfLogChanged(logNameForNotification[3]))	// Directory Service log.
					&& (!checkIfLogChanged(logNameForNotification[4]))	// DNS Server log.
					&& (!checkIfLogChanged(logNameForNotification[5]))) // File Replication Service log.
				{  				
					if (debugFlag)
					{							// Debug version is executing, 
												// no service termination
												// request to expect while 	
						cout << "," << flush;	// sleeping.
						
						if (enterDebugVersionWaitState(shortTermAnalysisInterval * 1000) 
							== shortInstant)
						{
							//	REMOVED 991223: 
							// control.setStartOffset(0);
							
							// New events(s) written in log.
							checkIfLogCleared(triggedLogName);
							instantAnalysis = TRUE;
							latestRecordNumbers[0] = latestSystemLogRecordNumber;	
							latestRecordNumbers[1] = latestApplicationLogRecordNumber;	
							latestRecordNumbers[2] = latestSecurityLogRecordNumber;
              // UABPEK_P1D
							latestRecordNumbers[3] = latestDirectoryServiceLogRecordNumber;
							latestRecordNumbers[4] = latestDNSServerLogRecordNumber;
							latestRecordNumbers[5] = latestFileReplicationServiceLogRecordNumber;
						}
						else				// Do a regular short term next time.
						{
							control.setStartOffset(originalStartOffset);
							latestRecordNumbers[0] = 0;	
							latestRecordNumbers[1] = 0;	
							latestRecordNumbers[2] = 0;
              // UABPEK_P1D
							latestRecordNumbers[3] = 0;	
							latestRecordNumbers[4] = 0;	
							latestRecordNumbers[5] = 0;
						}
					} 
					else
					{						// Release version (service) is 
											// executing, must check for
											// termination request from service 
											// manager and changes in event log 
											// while waiting.
											//
						if (enterReleaseVersionWaitState(shortTermAnalysisInterval * 1000) 
							== shortInstant)
						{
							//	REMOVED 991223: 
							//	control.setStartOffset(0);
							
							// New events(s) written in log		
							//
							checkIfLogCleared(triggedLogName);
							instantAnalysis = TRUE;
							latestRecordNumbers[0] = latestSystemLogRecordNumber;	
							latestRecordNumbers[1] = latestApplicationLogRecordNumber;	
							latestRecordNumbers[2] = latestSecurityLogRecordNumber;
              // UABPEK_P1D
							latestRecordNumbers[3] = latestDirectoryServiceLogRecordNumber;
							latestRecordNumbers[4] = latestDNSServerLogRecordNumber;
							latestRecordNumbers[5] = latestFileReplicationServiceLogRecordNumber;
						}
						else				// Do a regular short term next time.
						{
							control.setStartOffset(originalStartOffset);
							latestRecordNumbers[0] = 0;
							latestRecordNumbers[1] = 0;	
							latestRecordNumbers[2] = 0;
              // UABPEK_P1D
							latestRecordNumbers[3] = 0;	
							latestRecordNumbers[4] = 0;	
							latestRecordNumbers[5] = 0;
						}
					}
				} 
				else	// New events(s) written in log	during analysis	
				{
					instantAnalysis = TRUE; // new: 2000 01 07
					latestRecordNumbers[0] = latestSystemLogRecordNumber;
					latestRecordNumbers[1] = latestApplicationLogRecordNumber;
					latestRecordNumbers[2] = latestSecurityLogRecordNumber;
          // UABPEK_P1D
					latestRecordNumbers[3] = latestDirectoryServiceLogRecordNumber;
					latestRecordNumbers[4] = latestDNSServerLogRecordNumber;
					latestRecordNumbers[5] = latestFileReplicationServiceLogRecordNumber;
				} 
				if(changedLog != 0) 
				{
					changedLog = 0; 
				} 
			}
		}											// end of Long term
		else 
		{
			//---- Do the short term analysis -----
			//	
			USA_DEBUG(logMsg("MAIN:Performing SHORT term analysis\n"));


				if (saveLatestRecordNumbers() != ACS_USA_ok)
				{						
					notFinish = ACS_USA_False;
				}

				if (instantAnalysis == TRUE)		// Instant analysis is 
													// to be performed
				//	REMOVED 991223: 
				//	if (control.getStartOffset() == 0)		
				{
					if (ACS_TRA_ON(traceMain)) 
					{
						ACS_TRA_event(&traceMain, ACS_USA_traceActInstant);
					}
					if ((ret = control.operate(Activation_instantShortTerm,
											   triggedLogName,
											   latestRecordNumbers)) == ACS_USA_error) 
					{
						notFinish = ACS_USA_False;					
					} 
					instantAnalysis = FALSE;
				}
				else									// Regular short term
				{										// is to be performed
					if (ACS_TRA_ON(traceMain)) 
					{
						ACS_TRA_event(&traceMain, ACS_USA_traceActShort);
					}
					if (firstShortTerm)					// First time short term 
					{ 									// is performed 
						if ((ret = control.operate(Activation_FirstShortTerm,
												   triggedLogName,
												   latestRecordNumbers)) == ACS_USA_error) 
						{
							notFinish = ACS_USA_False;
						}
						firstShortTerm = false;			// First short term performed
					}  
					else								// Every time short term is performed, 
					{									// apart from the first one		

						// Set process priority to low during long term analysis
						// to avoid stealing CPU-time from more important activities.
						//
						if (ACS_TRA_ON(traceMain)) 
						{
							ACS_TRA_event(&traceMain, ACS_USA_tracePrioLow);
						}
						SetPriorityClass(myOwnProcessHandle, IDLE_PRIORITY_CLASS);
					
						// Do a regular short term
						//
						if ((ret = control.operate(Activation_shortTerm,
												   triggedLogName,
												   latestRecordNumbers)) == ACS_USA_error) 
						{
							notFinish = ACS_USA_False;			 
						}

						if (ACS_TRA_ON(traceMain))				
						{
							ACS_TRA_event(&traceMain, ACS_USA_tracePrioDefault);
						}	
						// Set prio back to default
						//						
						if (!SetPriorityClass(myOwnProcessHandle, ACS_USA_DefaultProcessPrioClass)) 
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
									strlen(lpMsgBuf));			// Ends with a '\0'	
							eventManager.report(ACS_USA_FirstErrorCode 
									+ ACS_USA_SystemCallError,	  // 8703
									Key_perceivedSeverity_EVENT,	// Event
									ACS_USA_FaultCause,			      // AP INTERNAL FAULT
									"",		
									errorString,	
									"SYSTEM CALL FAILED",
									"",0); 
							LocalFree(lpMsgBuf);  
							if (ACS_TRA_ON(traceMain)) 
							{
								ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
							}
							ExitProcess(ACS_USA_exitError);	// Might as well die 
															// and get restarted
						}			
					} 
				}
				if (notFinish == ACS_USA_True)
				{
					// If no new events - time to wait.
					//
					if ((!checkIfLogChanged(logNameForNotification[0]))		// System log
						&& (!checkIfLogChanged(logNameForNotification[1]))	// Application log
						&& (!checkIfLogChanged(logNameForNotification[2]))	// Security log
            // UABPEK_P1D
						&& (!checkIfLogChanged(logNameForNotification[3]))	// Directory Service log.
						&& (!checkIfLogChanged(logNameForNotification[4]))	// DNS Server log.
						&& (!checkIfLogChanged(logNameForNotification[5])))	// File Replication Service log.
 					{  		
						if (debugFlag)
						{							// Debug version is executing, no 
													// service termination request to 
													// expect while sleeping.
							cout << "." << flush;
							if (enterDebugVersionWaitState(shortTermAnalysisInterval * 1000) 
														   == shortInstant)
							{
								// REMOVED 991223: 
								// control.setStartOffset(0);

								// New events(s) written in log		
								//
								instantAnalysis = TRUE;
								checkIfLogCleared(triggedLogName);
								latestRecordNumbers[0] = latestSystemLogRecordNumber;
								latestRecordNumbers[1] = latestApplicationLogRecordNumber;
								latestRecordNumbers[2] = latestSecurityLogRecordNumber;
                // UABPEK_P1D
							  latestRecordNumbers[3] = latestDirectoryServiceLogRecordNumber;
						  	latestRecordNumbers[4] = latestDNSServerLogRecordNumber;
						  	latestRecordNumbers[5] = latestFileReplicationServiceLogRecordNumber;
							}
							else					// Do a regular short term next time.
							{
								control.setStartOffset(originalStartOffset);
								latestRecordNumbers[0] = 0;	
								latestRecordNumbers[1] = 0;	
								latestRecordNumbers[2] = 0;
                // UABPEK_P1D
		  					latestRecordNumbers[3] = 0;	
	  						latestRecordNumbers[4] = 0;	
  							latestRecordNumbers[5] = 0;
							}
						} 
						else
						{							// Release version (service) is 
													// executing, must check for 
													// termination request from service 
													// manager and changes in event log 
													// while waiting.
							if (enterReleaseVersionWaitState(shortTermAnalysisInterval * 1000) 
														     == shortInstant)						
							{
								// REMOVED 991223: 
								// control.setStartOffset(0);	 
							
								// New events(s) written in log		
								//
								instantAnalysis = TRUE;
								checkIfLogCleared(triggedLogName);
								latestRecordNumbers[0] = latestSystemLogRecordNumber;
								latestRecordNumbers[1] = latestApplicationLogRecordNumber;
								latestRecordNumbers[2] = latestSecurityLogRecordNumber;
                // UABPEK_P1D
						  	latestRecordNumbers[3] = latestDirectoryServiceLogRecordNumber;
					  		latestRecordNumbers[4] = latestDNSServerLogRecordNumber;
					  		latestRecordNumbers[5] = latestFileReplicationServiceLogRecordNumber;
							}
							else					// Do a regular short term next time.
							{
								control.setStartOffset(originalStartOffset);
								latestRecordNumbers[0] = 0;	
								latestRecordNumbers[1] = 0;	
								latestRecordNumbers[2] = 0;
                // UABPEK_P1D
		  					latestRecordNumbers[3] = 0;	
	  						latestRecordNumbers[4] = 0;	
  							latestRecordNumbers[5] = 0;
							}
						}
					}
					else		// New record(s) written during analysis	
					{ 
						instantAnalysis = TRUE;	// new: 2000 01 07
						latestRecordNumbers[0] = latestSystemLogRecordNumber;
						latestRecordNumbers[1] = latestApplicationLogRecordNumber;
						latestRecordNumbers[2] = latestSecurityLogRecordNumber;
            // UABPEK_P1D
						latestRecordNumbers[3] = latestDirectoryServiceLogRecordNumber;
						latestRecordNumbers[4] = latestDNSServerLogRecordNumber;
						latestRecordNumbers[5] = latestFileReplicationServiceLogRecordNumber;
					} 
					if(changedLog != 0) 
					{
						changedLog = 0;
					} 
				}				
				
		}  // Short term


		// 
		// Check if auto cease task is still alive...
		// 
		if( (ACS_USA_AutoCeaseManager::getInstance()).isCeaseThreadAlive() == ACS_USA_False )
		{
			notFinish = ACS_USA_False;
			error = (ACS_USA_AutoCeaseManager::getInstance()).getError();

			if (ACS_TRA_ON(traceMain)) 
			{
				ACS_TRA_event(&traceMain, ACS_USA_traceAutoCeaseDied);
			}
		}


		// For Purify testing only:
		//	slask ++;
		//	if (slask >= 3) notFinish = ACS_USA_False; 
    }													// end of while 

				// Nothing needs to be done here since the thread 
				// setting fLongterm dies when the main thread dies.
    	
    if (error.getError() != ACS_USA_APeventHandlingError) 
	{
		if ((error.getError() == ACS_USA_SyntaxError)
			||(error.getError() == ACS_USA_ParameterHandlingError)
			||(error.getError() == ACS_USA_FileIOError) )
		{

			if (debugFlag)
			{
									// Debug version is executing.			
									// Do nothing here.
			}
			else					// Release version is executing.
			{						// Suspend the USA process since the  
									// event requires stopping of USA via PRC.
									//
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExitWait);
				}

				WaitForSingleObject(ACS_USA_SyslogAnalyserStopEvent, INFINITE);
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExit);
				}
				// Stop event received, terminate process 
								
				Sleep(3000);			// To give the ServiceControlManager 
										// a chance to catch up.
				// ExitProcess(ACS_USA_exitError);	
				return(ACS_USA_exitError);  
			}									
		}
		
    }

    if (ACS_TRA_ON(traceMain)) 
	{
		ACS_TRA_event(&traceMain, ACS_USA_traceExit);
    }
    
    if (ret == ACS_USA_error) 
	{
		return(ACS_USA_exitError);  
		//	ExitProcess(ACS_USA_exitError);
	}    
    return(ACS_USA_exitOK);   
	// ExitProcess(ACS_USA_exitOK);
    return 0;
}

//*****************************************************************************
// saveLatestRecordNumbers
//*****************************************************************************
ACS_USA_StatusType	
saveLatestRecordNumbers()
{
	HANDLE eventLogHandle;
	DWORD numberOfRecords		= 0;
	BOOL  eventLogAccessible	= FALSE;
	DWORD oldestRecordNumber	= 0;
	DWORD latestRecordNumber	= 0;
	
	char	computerNameBuffer	[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD	computerNameLength = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName((char*)&computerNameBuffer, &computerNameLength); 
	int i = 0;
  // UABPEK_P1D
	for(i=1;i<=6;i++)
	{	

		eventLogHandle = OpenEventLog(computerNameBuffer, logNameForNotification[i-1].c_str());
		
		if (eventLogHandle == NULL)
		{       
			char* lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
												// control sequences such as char 13
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);
			
			char errorString[1024] = "System call OpenEventLog() failed. Machine name: ";
			strcat(errorString, computerNameBuffer);
			strcat(errorString, ". Error from system: ");	
			strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));			// Ends with a '\0'
			
			error.setError(ACS_USA_SystemCallError);
			error.setErrorText(errorString);			
			LocalFree(lpMsgBuf);			// Necessary according to the 
											// FormatMessage() manual page.
			return ACS_USA_error;
		} 
		
		// Find the number of records in log file
		//
		numberOfRecords		= 0;
		
		eventLogAccessible	= GetNumberOfEventLogRecords(eventLogHandle,
			&numberOfRecords);
		if (!eventLogAccessible)           
		{   
			char* lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
												// control sequences such as char 13
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);
			
			char errorString[1024] = "System call GetNumberOfEventLogRecords() failed. Error from system: ";
			strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));			// Ends with a '\0'
			
			error.setError(ACS_USA_SystemCallError);
			error.setErrorText(errorString);			
			LocalFree(lpMsgBuf);			// Necessary according to the 
											// FormatMessage() manual page.
			return ACS_USA_error;
		}
		
		// Find the latest record number in log file
		//
		oldestRecordNumber	= 0;
		latestRecordNumber	= 0;
		
		eventLogAccessible	= GetOldestEventLogRecord(eventLogHandle, &oldestRecordNumber);
		if (!eventLogAccessible)           
		{   
			char* lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
												// control sequences such as char 13
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);
			
			char errorString[1024] = "System call GetOldestEventLogRecord() failed. Error from system: ";
			strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));			// Ends with a '\0'
			
			error.setError(ACS_USA_SystemCallError);
			error.setErrorText(errorString);			
			LocalFree(lpMsgBuf);			// Necessary according to the 
											// FormatMessage() manual page.
			return ACS_USA_error;
		}	
		latestRecordNumber = oldestRecordNumber + numberOfRecords - 1;
		
		switch(i)
		{
		case(1):	
			latestSystemLogRecordNumber		= latestRecordNumber;
			break;
		case(2):	
			latestApplicationLogRecordNumber= latestRecordNumber;
			break;
		case(3):	
			latestSecurityLogRecordNumber	= latestRecordNumber;
			break;
    // UABPEK_P1D
		case(4):	
			latestDirectoryServiceLogRecordNumber	= latestRecordNumber;
			break;
		case(5):	
			latestDNSServerLogRecordNumber = latestRecordNumber;
			break;
		case(6):	
			latestFileReplicationServiceLogRecordNumber	= latestRecordNumber;
			break;
		}
		CloseEventLog(eventLogHandle);
	}													// end of for
	return ACS_USA_ok;
}

//*****************************************************************************
// checkIfLogChanged
//*****************************************************************************
BOOL	
checkIfLogChanged(String logName)	// Check if new events have been 
										// written in log during analysis.
{
	char	computerNameBuffer	[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD	computerNameLength = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName((char*)&computerNameBuffer, &computerNameLength);    
	HANDLE eventLogHandle = OpenEventLog(computerNameBuffer, logName.data());
	
	if (eventLogHandle == NULL)
	{       
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
											// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call OpenEventLog() failed. Machine name: ";
		strcat(errorString, computerNameBuffer);
		strcat(errorString, ". Error from system: ");	
		strncat(errorString, lpMsgBuf, 
			strlen(lpMsgBuf));				// Ends with a '\0'
		
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);				// Necessary according to the 
											// FormatMessage() manual page.
		//	return ACS_USA_error;
    } 
	
    // Find the number of records in log file
	//
	DWORD numberOfRecords		= 0;
	
	BOOL  eventLogAccessible	= GetNumberOfEventLogRecords(eventLogHandle,
		&numberOfRecords);
	if (!eventLogAccessible)           
	{   
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
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
		LocalFree(lpMsgBuf);					// Necessary according to the 
												// FormatMessage() manual page.
		//	return ACS_USA_error;
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
			FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
												// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call GetOldestEventLogRecord() failed. Error from system: ";
		strncat(errorString, lpMsgBuf, 
			strlen(lpMsgBuf));					// Ends with a '\0'
		
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);					// Necessary according to the 
												// FormatMessage() manual page.
		//	return ACS_USA_error;
	}	
	latestRecordNumber = oldestRecordNumber + numberOfRecords - 1;
	
	CloseEventLog(eventLogHandle);
	
	if (logName == logNameForNotification[0])	// System log
	{
		if (latestRecordNumber != latestSystemLogRecordNumber) 
		{
			changedLog++;
			if (changedLog <= 1) 
			{
				triggedLogName = logNameForNotification[0];
			}
			else 
			{
				triggedLogName = "";// Don't care about which logs that has been 
				changedLog = 0;		// changed. It's at least two of the three logs.
			}
			return TRUE;			// Event written to log during analysis
		}
	}
	else if (logName == logNameForNotification[1])	// Application log
	{
		if (latestRecordNumber != latestApplicationLogRecordNumber) 
		{
			changedLog++;
			if (changedLog <= 1) 
			{
				triggedLogName = logNameForNotification[1];
			}
			else
			{
				triggedLogName = "";// Don't care about which logs that has been 
				changedLog = 0;		// changed. It's at least two of the three logs.
			}
			return TRUE;			// Event written to log during analysis
		}
	}
	else if (logName == logNameForNotification[2])	// Security log
	{
		if (latestRecordNumber != latestSecurityLogRecordNumber) 
		{
			changedLog++;
			if (changedLog <= 1) 
			{
				triggedLogName = logNameForNotification[2];
			}
			else
			{
				triggedLogName = "";// Don't care about which logs that has been 
				changedLog = 0;		// changed. It's at least two of the three logs.			
			}
			return TRUE;			// Event written to log during analysis	
		}
	}
  // UABPEK_P1D
	else if (logName == logNameForNotification[3])  // Directory Service log.
	{
		if (latestRecordNumber != latestDirectoryServiceLogRecordNumber) 
		{
			changedLog++;
			if (changedLog <= 1) 
			{
				triggedLogName = logNameForNotification[3];
			}
			else
			{
				triggedLogName = "";// Don't care about which logs that has been 
				changedLog = 0;		// changed. It's at least two of the three logs.			
			}
			return TRUE;			// Event written to log during analysis	
		}
	}
  // UABPEK_P1D
	else if (logName == logNameForNotification[4])  // DNS Server log.
	{
		if (latestRecordNumber != latestDNSServerLogRecordNumber) 
		{
			changedLog++;
			if (changedLog <= 1) 
			{
				triggedLogName = logNameForNotification[4];
			}
			else
			{
				triggedLogName = "";// Don't care about which logs that has been 
				changedLog = 0;		// changed. It's at least two of the three logs.			
			}
			return TRUE;			// Event written to log during analysis	
		}
	}
  // UABPEK_P1D
	else if (logName == logNameForNotification[5])  // File Replication Service log.
	{
		if (latestRecordNumber != latestFileReplicationServiceLogRecordNumber) 
		{
			changedLog++;
			if (changedLog <= 1) 
			{
				triggedLogName = logNameForNotification[5];
			}
			else
			{
				triggedLogName = "";// Don't care about which logs that has been 
				changedLog = 0;		// changed. It's at least two of the three logs.			
			}
			return TRUE;			// Event written to log during analysis	
		}
	}
	else							// Error
	{
		return FALSE; 
	}
	
	return FALSE;
}

//*****************************************************************************
// checkIfLogCleared
//*****************************************************************************
BOOL	
checkIfLogCleared(String logName)	// Check if log has been cleared.
										// This function is called after being
										// notified about a new event in the log!
{
	char	computerNameBuffer	[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD	computerNameLength = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName((char*)&computerNameBuffer, &computerNameLength);    
	HANDLE eventLogHandle = OpenEventLog(computerNameBuffer, logName.data());
	
	if (eventLogHandle == NULL)
	{       
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
											// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call OpenEventLog() failed. Machine name: ";
		strcat(errorString, computerNameBuffer);
		strcat(errorString, ". Error from system: ");	
		strncat(errorString, lpMsgBuf, 
			strlen(lpMsgBuf));				// Ends with a '\0'
		
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);				// Necessary according to the 
											// FormatMessage() manual page.
		//	return ACS_USA_error;
    } 
	
    // Find the number of records in log file
	//
	DWORD numberOfRecords		= 0;
	
	BOOL  eventLogAccessible	= GetNumberOfEventLogRecords(eventLogHandle,
		&numberOfRecords);
	if (!eventLogAccessible)           
	{   
		char* lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
			FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
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
		LocalFree(lpMsgBuf);					// Necessary according to the 
												// FormatMessage() manual page.
		//	return ACS_USA_error;
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
			FORMAT_MESSAGE_MAX_WIDTH_MASK|		// buffersize(255)-ignores inserted 
												// control sequences such as char 13
			FORMAT_MESSAGE_FROM_SYSTEM|
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, 
			(LPTSTR)&lpMsgBuf, 0, NULL);
		
		char errorString[1024] = "System call GetOldestEventLogRecord() failed. Error from system: ";
		strncat(errorString, lpMsgBuf, 
			strlen(lpMsgBuf));					// Ends with a '\0'
		
		error.setError(ACS_USA_SystemCallError);
		error.setErrorText(errorString);			
		LocalFree(lpMsgBuf);					// Necessary according to the 
												// FormatMessage() manual page.
		//	return ACS_USA_error;
	}	
	latestRecordNumber = oldestRecordNumber + numberOfRecords - 1;
	
	CloseEventLog(eventLogHandle);
	
	if (logName == logNameForNotification[0])	// System log
	{
		// If number of records have decreased or is exactly 1, we know the log has been cleared
		if ((latestRecordNumber < latestSystemLogRecordNumber) || (latestRecordNumber == 1))
		{
			latestSystemLogRecordNumber = 0;
			return TRUE;			// Event written to log during analysis
		}
	}
	else if (logName == logNameForNotification[1])	// Application log
	{
		// If number of records have decreased or is exactly 1, we know the log has been cleared
		if ((latestRecordNumber < latestApplicationLogRecordNumber) || (latestRecordNumber == 1))
		{
			latestApplicationLogRecordNumber = 0;
			return TRUE;			// Event written to log during analysis
		}
	}
	else if (logName == logNameForNotification[2])	// Security log
	{
		// If number of records have decreased or is exactly 1, we know the log has been cleared
		if ((latestRecordNumber < latestSecurityLogRecordNumber) || (latestRecordNumber == 1)) 
		{
			latestSecurityLogRecordNumber = 0;
			return TRUE;			// Event written to log during analysis	
		}
	}
  // UABPEK_P1D
	else if (logName == logNameForNotification[3])  // Directory Service log.
	{
		// If number of records have decreased or is exactly 1, we know the log has been cleared
		if ((latestRecordNumber < latestDirectoryServiceLogRecordNumber) || (latestRecordNumber == 1)) 
		{
			latestDirectoryServiceLogRecordNumber = 0;
			return TRUE;			// Event written to log during analysis	
		}
	}
  // UABPEK_P1D
	else if (logName == logNameForNotification[4])  // DNS Server log.
	{
		// If number of records have decreased or is exactly 1, we know the log has been cleared
		if ((latestRecordNumber < latestDNSServerLogRecordNumber) || (latestRecordNumber == 1)) 
		{
			latestDNSServerLogRecordNumber = 0;
			return TRUE;			// Event written to log during analysis	
		}
	}
  // UABPEK_P1D
	else if (logName == logNameForNotification[5])  // File Replication Service log.
	{
		// If number of records have decreased or is exactly 1, we know the log has been cleared
		if ((latestRecordNumber < latestFileReplicationServiceLogRecordNumber) || (latestRecordNumber == 1)) 
		{
			latestFileReplicationServiceLogRecordNumber = 0;
			return TRUE;			// Event written to log during analysis	
		}
	}
	else							// Error
	{
		return FALSE; 
	}
	
	return FALSE;
}


//******************************************************************************
//	main()
//******************************************************************************

void main(DWORD argc, LPSTR *argv)
{
	AP_InitProcess((char *)ACS_USA_processName, AP_SERVICE);
    
	AP_SetCleanupAndCrashRoutine((char *)ACS_USA_processName, NULL);

	if (argc > 1)
	{
		if (strcmp(argv[1], "install") == 0)
		{
			USA_serviceInstall();
			ExitProcess(0);
		}
		if (strcmp(argv[1], "remove") == 0)
		{
			USA_serviceRemove();
			ExitProcess(0);
		}
		if (strcmp(argv[1], "debug") == 0)
			//
			// This option makes it possible to start USA from MS Dev Studio 
			// using "project settings|debug|program arguments|debug", for 
			// debugging/stepping the code.
		{
			cout << "Starting ACS_USA_SyslogAnalyser in debug mode." << endl;
			cout << "This mode is only intended for debugging of ACS_USA_SyslogAnalyser and " << endl;
			cout << "should be used with care." << endl;
			debugFlag = TRUE;
			USA_serviceMain(0);
			ExitProcess(0);
		}
	}
	else
	{
		debugFlag = FALSE;
		SERVICE_TABLE_ENTRY	DispatchTable[] =
		{
			{TEXT("ACS_USA_SyslogAnalyser"), (LPSERVICE_MAIN_FUNCTION)USA_serviceStart},
			{NULL, NULL}
		};
		if(!StartServiceCtrlDispatcher(DispatchTable))
		{
			USA_serviceError(" StartServiceCtrlDispatcher() failure:", 
							 GetLastError());
		}
		
	}
	cout << "Usage: " << endl << argv[0] << " install/remove" << endl;
	cout << "install: installation of ACS_USA_SyslogAnalyser"<< endl;
	cout << "remove: removal of ACS_USA_SyslogAnalyser" << endl;	
}


//******************************************************************************
//	USA_serviceStart()
//******************************************************************************
void WINAPI USA_serviceStart(DWORD argc, LPSTR *argv)
{
	DWORD	status;
	HANDLE	objects[2]={NULL, NULL};
	
	USA_serviceStatus.dwServiceType		= SERVICE_WIN32;
	USA_serviceStatus.dwCurrentState	= SERVICE_START_PENDING;
	USA_serviceStatus.dwControlsAccepted= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	USA_serviceStatus.dwWin32ExitCode	= 0;
	USA_serviceStatus.dwServiceSpecificExitCode = 0;
	USA_serviceStatus.dwCheckPoint		= 0;
	USA_serviceStatus.dwWaitHint		= 0;

	USA_serviceStatusHandle	= RegisterServiceCtrlHandler(
								TEXT("ACS_USA_SyslogAnalyser"),
								USA_serviceControlHandler);

	if (USA_serviceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		USA_serviceError(" RegisterServiceCtrlHandler() failed: ", GetLastError());
		return;
	}

	// Initialization
	//
	status = USA_serviceInitialization(argc, argv);
	if (status == ACS_USA_error)
	{
		USA_serviceStatus.dwCurrentState	= SERVICE_STOPPED;
		USA_serviceStatus.dwWin32ExitCode	= status;
		USA_serviceStatus.dwServiceSpecificExitCode = 0;
		USA_serviceStatus.dwCheckPoint		= 0;
		USA_serviceStatus.dwWaitHint		= 0;
		SetServiceStatus (USA_serviceStatusHandle, &USA_serviceStatus);
		return;
	}

	ACS_USA_SyslogAnalyserStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//
	// Initialization complete 

	USA_serviceStatus.dwCurrentState	= SERVICE_RUNNING;
	USA_serviceStatus.dwCheckPoint		= 0;
	USA_serviceStatus.dwWaitHint		= 0;
	if (!SetServiceStatus(USA_serviceStatusHandle, &USA_serviceStatus))
	{
		status = GetLastError();
		USA_serviceError(" SetServiceStatus() failed: ", status);
	}
	objects[0] = ACS_USA_SyslogAnalyserStopEvent;
	objects[1] = (void*)ServiceThreadHandle;

	DWORD waitResult = WaitForMultipleObjects(2, objects, FALSE, INFINITE);
	if (waitResult == WAIT_OBJECT_0)	
	{
		Sleep(1500);
		try
		{
			CloseHandle(ACS_USA_SyslogAnalyserStopEvent);
		}
		catch(...)
		{
			// Ignore handle problems at this stage
		}
	} 
	else if (waitResult == WAIT_OBJECT_0 + 1)
	{
		try 
		{
			USA_serviceError(" ACS_USA_SyslogAnalyser MAIN THREAD END.", 0);
			CloseHandle((void*)ServiceThreadHandle);
		}
		catch(...)
		{
			// Ignore handle problems at this stage
		}
	}
	
	USA_serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(USA_serviceStatusHandle, &USA_serviceStatus);
	
	return;
}


//******************************************************************************
//	USA_serviceControlHandler()
//******************************************************************************

void WINAPI 
USA_serviceControlHandler(DWORD opcode)
{
	DWORD status;
	//HANDLE hStopGracefully;

	switch(opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		USA_serviceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
		
	case SERVICE_CONTROL_CONTINUE:
		USA_serviceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
		
	case SERVICE_CONTROL_STOP:
		{
			USA_serviceStatus.dwCurrentState	= SERVICE_STOP_PENDING;
			USA_serviceStatus.dwWin32ExitCode	= 0;
			USA_serviceStatus.dwWaitHint		= 4000;
			USA_serviceStatus.dwCheckPoint		= 0;
			
			if(ACS_TRA_ON((ACS_USA_serviceStop)))
				ACS_TRA_event(&ACS_USA_serviceStop, "SERVICE_CONTROL_STOP received. Exiting.");

			DWORD LastUSAstopTimeStamp = 0;
			DWORD LastUSAstopTimeStampSize = 4;
			HKEY hKey;
			DWORD lResult;
			
			lResult = RegOpenKeyEx(
				HKEY_LOCAL_MACHINE,
				ACS_USA_applicationKey,
				0,
				KEY_WRITE,
				&hKey
				);
			
			if (lResult == ERROR_SUCCESS)
			{
				// Get the current time
				time((long*)&LastUSAstopTimeStamp);
				
				TCHAR* lpValueName = "LastUSAstopTime";
				
				lResult = RegSetValueEx(
					hKey,				// Handle to key to set value for
					lpValueName,		// Name of the value to set
					0,					// Reserved
					REG_DWORD,			// Flag for value type
					(unsigned char*)&LastUSAstopTimeStamp,
										// Value data
					LastUSAstopTimeStampSize
										// Size of value data
					);
				
				if (lResult != ERROR_SUCCESS)
				{
					String errorString = "Failed to write value ";
					errorString += lpValueName;
					error.setError(ACS_USA_SystemCallError);
					error.setErrorText(errorString.c_str());			
				}
				RegCloseKey(hKey);
			}
			else
			{
				String errorString = "Failed to open key ";
				errorString += ACS_USA_applicationKey;
				error.setError(ACS_USA_SystemCallError);
				error.setErrorText(errorString.c_str());			
			}
			
/*
			Decision taken by uabutj and uabdmt 990614:
			"USA shall not cease upon exit. The reason is that the alarms shall stay raised 
			even if there is some fault making USA restart. In addition, ALH ceases all alarms 
			upon reboot, making it unneccesary for USA to cease them."
			
			  As a result of this decision, the following instruction is from now on 
			  a comment for historical reasons only:
			  
			eventManager.ceaseAll();			// Cease any set alarms, 
												// since USA is terminating.
*/
			if (!SetServiceStatus(USA_serviceStatusHandle, &USA_serviceStatus))
			{
				status = GetLastError();
				USA_serviceError(" SetServiceStatus() failure: ", status);
			}
			Sleep(1000);						// Seems to be needed to make 
												// sure that the USA-exit event 
												// turns upp in the event log.
			USA_serviceError(" SERVICE_CONTROL_STOP received. Exiting.", 0);
			
			Sleep(1000);						// Seems to be needed to make 
												// sure that the USA-exit event 
												// turns upp in the event log.
			SetEvent(ACS_USA_SyslogAnalyserStopEvent);
			
			return;
	}
	case SERVICE_CONTROL_INTERROGATE:
		break;
		
	default:
		USA_serviceError(" Unknown opcode", opcode);
	}

	// Send current status
	if(!SetServiceStatus(USA_serviceStatusHandle, &USA_serviceStatus))
	{
		status = GetLastError();
		USA_serviceError(" SetServiceStatus() failure: ", status);
	}
	return;
}


//******************************************************************************
//	USA_serviceInstall()
//******************************************************************************
void USA_serviceInstall()
{
	SC_HANDLE	SCmanager;
	SC_HANDLE	service;
	
	TCHAR		buffer[500];
	TCHAR		filePath[512];
	
	if (GetModuleFileName(NULL, buffer, 500) == 0)
	{
		printf("Unable to install ACS_USA_SyslogAnalyser. ");
		return;
	}
	
	strcpy(filePath, "\"");
	strcat(filePath, buffer);
	strcat(filePath, "\"");
	
	SCmanager = OpenSCManager(NULL, NULL,
					SC_MANAGER_ALL_ACCESS);

	if (SCmanager)
	{
		service = CreateService(SCmanager,
			"ACS_USA_SyslogAnalyser",
			"ACS_USA_SyslogAnalyser",
			SERVICE_ALL_ACCESS,  
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, 
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			filePath, 
			NULL, 
			NULL, 
			"", 
			NULL, 
			NULL);
		
		if (service)
		{
			printf("ACS_USA_SyslogAnalyser installed. ");
			CloseServiceHandle(service);
		}
		else
		{
			printf("Failed to create ACS_USA_SyslogAnalyser. " );
		}
		CloseServiceHandle(SCmanager);
	}
	else
	{
		printf("OpenManager failed. ");
	}
}

//******************************************************************************
//	USA_serviceRemove()
//******************************************************************************
void USA_serviceRemove()
{
	SC_HANDLE	SCmanager;
	SC_HANDLE	service;
	
	SCmanager = OpenSCManager(NULL, NULL,
					SC_MANAGER_ALL_ACCESS);
	if (SCmanager)
	{
		service = OpenService(SCmanager, "ACS_USA_SyslogAnalyser", SERVICE_ALL_ACCESS);
		if (service)
		{
			if (ControlService(service, SERVICE_CONTROL_STOP, &USA_serviceStatus) )
			{
				printf("Stopping ACS_USA_SyslogAnalyser ");
				Sleep(1000);
				
				while (QueryServiceStatus(service, &USA_serviceStatus))
				{
					if (USA_serviceStatus.dwCurrentState == SERVICE_STOP_PENDING)
					{
						printf(". ");
						Sleep(1000);
					}
					else
					{
						break;
					}
				}
				if (USA_serviceStatus.dwCurrentState == SERVICE_STOPPED)
				{
					printf("ACS_USA_SyslogAnalyser stopped. ");
				}
				else
				{
					printf("Failed to stop ACS_USA_SyslogAnalyser.");
				}
			}
			if (DeleteService(service))
			{
				printf("ACS_USA_SyslogAnalyser removed. ");
			}
			else
			{
				printf("Failed to remove ACS_USA_SyslogAnalyser.");
			}
			
			CloseServiceHandle(service);
		}
		else
		{
			printf("Failed to open ACS_USA_SyslogAnalyser. ");
			CloseServiceHandle(service);
		}
	}
	else
	{
		printf(" OpenSCManager failed. ");
	}
}

//******************************************************************************
//	USA_serviceInitialization()
//******************************************************************************
DWORD USA_serviceInitialization(DWORD argc, LPSTR *argv)
{
	unsigned int ServiceId;
	
	// beginthreadex() is used since CreateThread should not be used when C-run time 
	// functions are used in the code. Memory leaks can otherwise occur.
	//
	ServiceThreadHandle = _beginthreadex(NULL, 0, USA_serviceMain, NULL, 0, &ServiceId);
	
	if (ServiceThreadHandle == 0)
	{
		return ACS_USA_error;
	}
	return ACS_USA_ok;
}


//******************************************************************************
//	USA_serviceError()
//******************************************************************************
void USA_serviceError (char* string, DWORD status)
{	 
	if(ACS_TRA_ON((ACS_USA_serviceStop)))
		ACS_TRA_event(&ACS_USA_serviceStop, ACS_USA_traceServiceError);
	
	char errorString[512] = "The service ACS_USA_SyslogAnalyser is exiting. Error from system: ";		
	char* errorCode	= "0";
	sprintf (errorCode, "%u", status);

	strcat (errorString, errorCode);
	strcat (errorString, " ");
	strncat (errorString, string, strlen(string));	// Ends with a '\0'

	// Report exit to AEH
	//
	error.setError(ACS_USA_ServiceError);
	error.setErrorText(errorString);
	
}

//******************************************************************************
//	enterReleaseVersionWaitState()
//******************************************************************************
int enterReleaseVersionWaitState(DWORD waitInterval)
{											// Release version (service) is 
											// executing. Must check for stop 
											// request from service manager 
											// while waiting.	

	// UABPEK_P1D
  HANDLE waitObjects[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	DWORD	computerNameLength	=	MAX_COMPUTERNAME_LENGTH + 1;
	char	computerNameBuffer		[MAX_COMPUTERNAME_LENGTH + 1];

	GetComputerName((char*)&computerNameBuffer, &computerNameLength);

	HANDLE firstEventLogHandle		= OpenEventLog(computerNameBuffer, logNameForNotification[0].data());  
	HANDLE secondEventLogHandle		= OpenEventLog(computerNameBuffer, logNameForNotification[1].data());
	HANDLE thirdEventLogHandle		= OpenEventLog(computerNameBuffer, logNameForNotification[2].data());
  // UABPEK_P1D
	HANDLE fourthEventLogHandle		= OpenEventLog(computerNameBuffer, logNameForNotification[3].data());
	HANDLE fifthEventLogHandle		= OpenEventLog(computerNameBuffer, logNameForNotification[4].data());
	HANDLE sixthEventLogHandle		= OpenEventLog(computerNameBuffer, logNameForNotification[5].data());

	if ((firstEventLogHandle == NULL) 
		|| (secondEventLogHandle == NULL) 
		|| (thirdEventLogHandle == NULL )
    // UABPEK_P1D
		|| (fourthEventLogHandle == NULL)
		|| (fifthEventLogHandle == NULL)
		|| (sixthEventLogHandle == NULL))	// OpenEventLog error
	{	
		char* lpMsgBuf;						// Log error to AEH
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
					  FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
														// control sequences such as char 13
					  FORMAT_MESSAGE_FROM_SYSTEM|
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, GetLastError(), 0, 
					  (LPTSTR)&lpMsgBuf, 0, NULL);			
		char errorString[1024] = "System call OpenEventLog() failed. Cannot receive notification of new Event Log events. Error from system: ";
		strncat(errorString, lpMsgBuf, 
				strlen(lpMsgBuf));						// Ends with a '\0'	  
		eventManager.report(ACS_USA_FirstErrorCode 
							+ ACS_USA_SystemCallError,	  // 8703
							Key_perceivedSeverity_EVENT,	// Event
							ACS_USA_FaultCause,			      // AP INTERNAL FAULT
							"",		
							errorString,	
							"SYSTEM CALL FAILED",
							"", 0); 
		LocalFree(lpMsgBuf);
		if (ACS_TRA_ON(traceMain)) 
		{
				ACS_TRA_event(&traceMain, ACS_USA_traceEvlogError);
		}
												// If eventlog cannot be opened, 
												// skip trying to wait for new events.
												// Instead, wait only for stop request.
		WaitForSingleObject(ACS_USA_SyslogAnalyserStopEvent,
							waitInterval);
		ResetEvent(ACS_USA_SyslogAnalyserStopEvent);		
		return ACS_USA_ok;
    } 
	else										// Open event to be set to signaled 
	{											// state when a new event is 
												// written to the specific log
		int notifySysOk, notifyAppOk, notifySecOk, notifyDirOk, notifyDNSOk, notifyFileOk;
		notifySysOk = false; 
		notifyAppOk = false; 
		notifySecOk = false;
    // UABPEK_P1D
		notifyDirOk = false;
		notifyDNSOk = false;
		notifyFileOk = false;

		notifySysOk = NotifyChangeEventLog(firstEventLogHandle,  systemLogChangeEvent);  
		notifyAppOk = NotifyChangeEventLog(secondEventLogHandle, applicationLogChangeEvent);
		notifySecOk = NotifyChangeEventLog(thirdEventLogHandle,  securityLogChangeEvent);
    // UABPEK_P1D
		notifyDirOk = NotifyChangeEventLog(fourthEventLogHandle, directoryServiceLogChangeEvent);
		notifyDNSOk = NotifyChangeEventLog(fifthEventLogHandle, DNSServerLogChangeEvent);
		notifyFileOk = NotifyChangeEventLog(sixthEventLogHandle, fileReplicationServiceLogChangeEvent);
	
	if ((notifySysOk == 0) || (notifyAppOk == 0)	// NotifyChangeEventLog error
						   || (notifySecOk == 0)
               // UABPEK_P1D
               || (notifyDirOk == 0) || (notifyDNSOk == 0) || (notifyFileOk == 0))
	 	{										
			char* lpMsgBuf;							// Log error to AEH
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
						  FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
															// control sequences such as char 13
						  FORMAT_MESSAGE_FROM_SYSTEM|
						  FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, GetLastError(), 0, 
						  (LPTSTR)&lpMsgBuf, 0, NULL);			
			char errorString[1024] = "System call NotifyChangeEventLog() failed. Error from system: ";
			strncat(errorString, lpMsgBuf, 
					strlen(lpMsgBuf));						// Ends with a '\0'	
			eventManager.report(ACS_USA_FirstErrorCode 
								+ ACS_USA_SystemCallError,	  // 8703
								Key_perceivedSeverity_EVENT,	// Event
								ACS_USA_FaultCause,			      // AP INTERNAL FAULT
								"",		
								errorString,	
								"SYSTEM CALL FAILED",
								"", 0); 
			LocalFree(lpMsgBuf);
			if (ACS_TRA_ON(traceMain)) 
			{
				ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
			}
			ExitProcess(ACS_USA_exitError);		// Might as well die 
												// and get restarted
		}

		waitObjects[0] = ACS_USA_SyslogAnalyserStopEvent;
		waitObjects[1] = systemLogChangeEvent;
		waitObjects[2] = applicationLogChangeEvent;		
		waitObjects[3] = securityLogChangeEvent;
    // UABPEK_P1D
    waitObjects[4] = directoryServiceLogChangeEvent;
		waitObjects[5] = DNSServerLogChangeEvent;		
		waitObjects[6] = fileReplicationServiceLogChangeEvent;  // If either a stop request is received
												// (ACS_USA_SyslogAnalyserStopEvent set to 
												// signaled state) or a new event is 
												// written to the specific log, the
												// wait state ends.
		DWORD signaledObjectNo = WaitForMultipleObjects
                // UABPEK_P1D
								(7, waitObjects,		
								FALSE, 
								(waitInterval/100));// Default: 12 seconds. 
		if 	((signaledObjectNo == WAIT_TIMEOUT)		// If we slept 12 seconds, 
			||(signaledObjectNo == WAIT_FAILED))	// normal prio is to be set.
		{	
			if (GetPriorityClass(myOwnProcessHandle) != ACS_USA_DefaultProcessPrioClass)
			{
				if (ACS_TRA_ON(traceMain))				
				{
					ACS_TRA_event(&traceMain, ACS_USA_tracePrioDefault);
				}	
				if (!SetPriorityClass(myOwnProcessHandle, // Set prio back to default
								 ACS_USA_DefaultProcessPrioClass))
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
					if (ACS_TRA_ON(traceMain)) 
					{
						ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
					}
					ExitProcess(ACS_USA_exitError);	// Might as well die 
													// and get restarted
				}		
			}			
			signaledObjectNo = WaitForMultipleObjects		// Do the long wait.														
                  // UABPEK_P1D
									(7, waitObjects,		
									FALSE, 
									(waitInterval			// Default: 20 min
									- (waitInterval/100)));	// minus 12 sec.
			
			if (signaledObjectNo == WAIT_FAILED)	// WaitForMultipleObjects error
			{
				char* lpMsgBuf;						// Log error to AEH
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_MAX_WIDTH_MASK| // buffersize(255)-ignores inserted 
														// control sequences such as char 13
							FORMAT_MESSAGE_FROM_SYSTEM|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, 
							(LPTSTR)&lpMsgBuf, 0, NULL);			
				char errorString[1024] = "System call WaitForMultipleObjects() failed. Error from system: ";
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
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
				}
				ExitProcess(ACS_USA_exitError);	// Might as well die 
												// and get restarted
			}
		}  

//  For test only:
//  char testslask[10] = "";
//	sprintf(testslask,"%u", signaledObjectNo);
//  MessageBox(NULL,testslask , "Signaled object no:", NULL);

		if ((signaledObjectNo >= WAIT_OBJECT_0 + 1)	// If a new event has been written in 
			&& (signaledObjectNo != WAIT_TIMEOUT))	// event log, read that event only. Don't
		{											// perform a regular short term analysis.
			ResetEvent (ACS_USA_SyslogAnalyserStopEvent); 
			int resetSysOk = ResetEvent (systemLogChangeEvent);		  
			int resetAppOk = ResetEvent (applicationLogChangeEvent); 
			int resetSecOk = ResetEvent (securityLogChangeEvent);
      // UABPEK_P1D 
		  int resetDirOk = ResetEvent(directoryServiceLogChangeEvent);
		  int resetDNSOk = ResetEvent(DNSServerLogChangeEvent);
		  int resetFileOk = ResetEvent(fileReplicationServiceLogChangeEvent);

 			if ((resetSysOk == 0) || (resetAppOk == 0)
								  || (resetSecOk == 0)
                  // UABPEK_P1D
                  || (resetDirOk == 0) || (resetDNSOk == 0) || (resetFileOk == 0))  // ResetEvent error.
			{										
				char* lpMsgBuf;							// Log error to AEH
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							  FORMAT_MESSAGE_MAX_WIDTH_MASK| // buffersize(255)-ignores inserted 
															 // control sequences such as char 13
							  FORMAT_MESSAGE_FROM_SYSTEM|
							  FORMAT_MESSAGE_IGNORE_INSERTS,
							  NULL, GetLastError(), 0, 
							  (LPTSTR)&lpMsgBuf, 0, NULL);				
				char errorString[1024] = "System call ResetEvent() failed. Error from system: ";
				strncat(errorString, lpMsgBuf, 
						strlen(lpMsgBuf));						// Ends with a '\0'	  
				eventManager.report(ACS_USA_FirstErrorCode 
									+ ACS_USA_SystemCallError,	  // 8703
									Key_perceivedSeverity_EVENT,	// Event
									ACS_USA_FaultCause,			      // AP INTERNAL FAULT
									"",		
									errorString,	
									"SYSTEM CALL FAILED",
									"", 0); 
				LocalFree(lpMsgBuf);
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
				}
				ExitProcess(ACS_USA_exitError);		// Might as well die 
													// and get restarted
			}

			CloseEventLog(firstEventLogHandle);			
			CloseEventLog(secondEventLogHandle);				
			CloseEventLog(thirdEventLogHandle);
      // UABPEK_P1D
			CloseEventLog(fourthEventLogHandle);
			CloseEventLog(fifthEventLogHandle);
			CloseEventLog(sixthEventLogHandle);
			switch(signaledObjectNo)
			{
				case 1:
					triggedLogName = logNameForNotification[0]; // System
					break;
				case 2:
					triggedLogName = logNameForNotification[1]; // Application
					break;
				case 3:
					triggedLogName = logNameForNotification[2]; // Security
					break;
        // UABPEK_P1D
				case 4:
					triggedLogName = logNameForNotification[3];  // Directory Service
					break;
				case 5:
					triggedLogName = logNameForNotification[4];  // DNS Server
					break;
				case 6:
					triggedLogName = logNameForNotification[5];  // File Replication Service
					break;
				default: 
					break;
			}
													// Time for instant analysis (used for 
			return shortInstant;					// notification of changes in event log).
		}
		ResetEvent(ACS_USA_SyslogAnalyserStopEvent); 		
		CloseEventLog(firstEventLogHandle);
		CloseEventLog(secondEventLogHandle);				
		CloseEventLog(thirdEventLogHandle);
    // UABPEK_P1D	
		CloseEventLog(fourthEventLogHandle);
		CloseEventLog(fifthEventLogHandle);
		CloseEventLog(sixthEventLogHandle);
	}
	triggedLogName = "";
	return ACS_USA_ok;
}

//******************************************************************************
//	enterDebugVersionWaitState()
//******************************************************************************
int enterDebugVersionWaitState(DWORD waitInterval)
{												// Debug version is executing, 
												// no service termination
												// request to expect while 		
												// sleeping.

	// UABPEK_P1D
  HANDLE waitObjects[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
	DWORD	computerNameLength	=	MAX_COMPUTERNAME_LENGTH + 1;
	char	computerNameBuffer		[MAX_COMPUTERNAME_LENGTH + 1];

	GetComputerName((char*)&computerNameBuffer, &computerNameLength);
	
	HANDLE firstEventLogHandle	= OpenEventLog(computerNameBuffer, logNameForNotification[0].data());
	HANDLE secondEventLogHandle = OpenEventLog(computerNameBuffer, logNameForNotification[1].data());
	HANDLE thirdEventLogHandle	= OpenEventLog(computerNameBuffer, logNameForNotification[2].data());
  // UABPEK_P1D
	HANDLE fourthEventLogHandle	= OpenEventLog(computerNameBuffer, logNameForNotification[3].data());
	HANDLE fifthEventLogHandle	= OpenEventLog(computerNameBuffer, logNameForNotification[4].data());
	HANDLE sixthEventLogHandle	= OpenEventLog(computerNameBuffer, logNameForNotification[5].data());

	if ((firstEventLogHandle == NULL)
		||(secondEventLogHandle == NULL)
		||(thirdEventLogHandle == NULL)
    // UABPEK_P1D
		||(fourthEventLogHandle == NULL)
		||(fifthEventLogHandle == NULL)
		||(sixthEventLogHandle == NULL))
	{									// If eventlog cannot be opened,											
										// skip trying to wait for new 			 												
		Sleep(waitInterval);			// events. Instead, just sleep.
		return ACS_USA_ok;
    } 
	else										
	{							
		int notifySysOk, notifyAppOk, notifySecOk, notifyDirOk, notifyDNSOk, notifyFileOk;
		notifySysOk = false; 
		notifyAppOk = false; 
		notifySecOk = false;
    // UABPEK_P1D
		notifyDirOk = false;
		notifyDNSOk = false;
		notifyFileOk = false;

		notifySysOk = NotifyChangeEventLog(firstEventLogHandle,  systemLogChangeEvent);  
		notifyAppOk = NotifyChangeEventLog(secondEventLogHandle, applicationLogChangeEvent);
		notifySecOk = NotifyChangeEventLog(thirdEventLogHandle,  securityLogChangeEvent);
    // UABPEK_P1D
		notifyDirOk = NotifyChangeEventLog(thirdEventLogHandle, directoryServiceLogChangeEvent);
		notifyDNSOk = NotifyChangeEventLog(fourthEventLogHandle, DNSServerLogChangeEvent);
		notifyFileOk = NotifyChangeEventLog(sixthEventLogHandle, fileReplicationServiceLogChangeEvent);
	
	  if ((notifySysOk == 0) || (notifyAppOk == 0)	// NotifyChangeEventLog error
						   || (notifySecOk == 0)
               // UABPEK_P1D
               || (notifyDirOk == 0) || (notifyDNSOk == 0) || (notifyFileOk == 0))
	 	{										
			char* lpMsgBuf;						// Log error to AEH
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
												// control sequences such as char 13
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);			
			char errorString[1024] = "System call NotifyChangeEventLog() failed. Error from system: ";
			strncat(errorString, lpMsgBuf, 
					strlen(lpMsgBuf));						// Ends with a '\0'	  
			eventManager.report(ACS_USA_FirstErrorCode 
								+ ACS_USA_SystemCallError,	  // 8703
								Key_perceivedSeverity_EVENT,	// Event
								ACS_USA_FaultCause,			      // AP INTERNAL FAULT
								"",		
								errorString,	
								"SYSTEM CALL FAILED",
								"", 0); 
			LocalFree(lpMsgBuf);
			if (ACS_TRA_ON(traceMain)) 
			{
				ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
			}
			ExitProcess(ACS_USA_exitError);
		}

		waitObjects[0] = systemLogChangeEvent;
		waitObjects[1] = applicationLogChangeEvent;	// If a new event is written to the 	
		waitObjects[2] = securityLogChangeEvent;	// specific log the wait state ends.
    // UABPEK_P1D
    waitObjects[3] = directoryServiceLogChangeEvent;
		waitObjects[4] = DNSServerLogChangeEvent;		
		waitObjects[5] = fileReplicationServiceLogChangeEvent;

		DWORD signaledObjectNo = WaitForMultipleObjects 															
                  // UABPEK_P1D
									(6, waitObjects,		
									FALSE, 
									(waitInterval/100));// Default: 12 seconds. 
		if 	((signaledObjectNo == WAIT_TIMEOUT)			// If we slept 12 seconds, 
			||(signaledObjectNo == WAIT_FAILED))		// normal prio is to be set.
		{	
			if (GetPriorityClass(myOwnProcessHandle) != ACS_USA_DefaultProcessPrioClass)
			{
				if (ACS_TRA_ON(traceMain))				
				{
					ACS_TRA_event(&traceMain, ACS_USA_tracePrioDefault);
				}	
				if (!SetPriorityClass(myOwnProcessHandle,	// Set prio back to default
								 ACS_USA_DefaultProcessPrioClass))
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
					if (ACS_TRA_ON(traceMain)) 
					{
						ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
					}
					ExitProcess(ACS_USA_exitError);	// Might as well die 
													// and get restarted
				}	
			}		
			
			signaledObjectNo = WaitForMultipleObjects	// Do the long wait.															
                // UABPEK_P1D
								(6, waitObjects,		
								FALSE, 
								(waitInterval			// Default: 20 min
								- (waitInterval/100)));	// minus 12 sec.
			
			if ((signaledObjectNo == WAIT_FAILED))		// WaitForMultipleObjects error							   	
			{										
				char* lpMsgBuf;							// Log error to AEH
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
						FORMAT_MESSAGE_MAX_WIDTH_MASK|	// buffersize(255)-ignores inserted 
														// control sequences such as char 13
						FORMAT_MESSAGE_FROM_SYSTEM|
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, GetLastError(), 0, 
						(LPTSTR)&lpMsgBuf, 0, NULL);			
				char errorString[1024] = "System call WaitForMultipleObjects() failed. Error from system: ";
				strncat(errorString, lpMsgBuf, 
						strlen(lpMsgBuf));						// Ends with a '\0'	  
				eventManager.report(ACS_USA_FirstErrorCode 
						+ ACS_USA_SystemCallError,	  // 8703
						Key_perceivedSeverity_EVENT,	// Event
						ACS_USA_FaultCause,			      // AP INTERNAL FAULT
						"",		
						errorString,	
						"SYSTEM CALL FAILED",
						"", 0); 
				LocalFree(lpMsgBuf);
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
				}
				ExitProcess(ACS_USA_exitError);
			}
		}

		if ((signaledObjectNo >= WAIT_OBJECT_0) 
			&& (signaledObjectNo != WAIT_TIMEOUT))	
											// If new events have been written in 
		{									// event log, analyse these events only next time.			
											// Don't perform a regular short term analysis.			
			int resetSysOk = ResetEvent(systemLogChangeEvent);
			int resetAppOk = ResetEvent(applicationLogChangeEvent);
			int resetSecOk = ResetEvent(securityLogChangeEvent);
      // UABPEK_P1D 
		  int resetDirOk = ResetEvent(directoryServiceLogChangeEvent);
		  int resetDNSOk = ResetEvent(DNSServerLogChangeEvent);
		  int resetFileOk = ResetEvent(fileReplicationServiceLogChangeEvent);

			if ((resetSysOk == 0) || (resetAppOk == 0)					   	
								  || (resetSecOk == 0)
                  // UABPEK_P1D
                  || (resetDirOk == 0) || (resetDNSOk == 0) || (resetFileOk == 0))  // ResetEvent error.
			{										
				char* lpMsgBuf;							// Log error to AEH
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							  FORMAT_MESSAGE_MAX_WIDTH_MASK| // buffersize(255)-ignores inserted 
															 // control sequences such as char 13
							  FORMAT_MESSAGE_FROM_SYSTEM|
							  FORMAT_MESSAGE_IGNORE_INSERTS,
							  NULL, GetLastError(), 0, 
							  (LPTSTR)&lpMsgBuf, 0, NULL);				
				char errorString[1024] = "System call ResetEvent() failed. Error from system: ";
				strncat(errorString, lpMsgBuf, 
						strlen(lpMsgBuf));						// Ends with a '\0'	  
				eventManager.report(ACS_USA_FirstErrorCode 
									+ ACS_USA_SystemCallError,	  // 8703
									Key_perceivedSeverity_EVENT,	// Event
									ACS_USA_FaultCause,			      // AP INTERNAL FAULT
									"",		
									errorString,	
									"SYSTEM CALL FAILED",
									"", 0); 
				LocalFree(lpMsgBuf);
				if (ACS_TRA_ON(traceMain)) 
				{
					ACS_TRA_event(&traceMain, ACS_USA_traceExitSysCallError);
				}
				ExitProcess(ACS_USA_exitError);
			}

			CloseEventLog(firstEventLogHandle);
			CloseEventLog(secondEventLogHandle);
			CloseEventLog(thirdEventLogHandle);
      // UABPEK_P1D
			CloseEventLog(fourthEventLogHandle);
			CloseEventLog(fifthEventLogHandle);
			CloseEventLog(sixthEventLogHandle);
			switch(signaledObjectNo)
			{
				case 0:
					triggedLogName = logNameForNotification[0]; // System
					break;
				case 1:
					triggedLogName = logNameForNotification[1]; // Application
					break;
				case 2:
					triggedLogName = logNameForNotification[2]; // Security
					break;
        // UABPEK_P1D
				case 3:
					triggedLogName = logNameForNotification[3];  // Directory Service.
					break;
				case 4:
					triggedLogName = logNameForNotification[4];  // DNS Server.
					break;
				case 5:
					triggedLogName = logNameForNotification[5];  // File Replication Service.
					break;
				default: 
					break;
			}
											// Time for instant analysis (used for
			return shortInstant;			// notification of changes in event log). 
		}
		
		CloseEventLog(firstEventLogHandle);
		CloseEventLog(secondEventLogHandle);
		CloseEventLog(thirdEventLogHandle);
    // UABPEK_P1D
		CloseEventLog(fourthEventLogHandle);
		CloseEventLog(fifthEventLogHandle);
		CloseEventLog(sixthEventLogHandle);
	}
	triggedLogName = "";
	return ACS_USA_ok;
}

//******************************************************************************
