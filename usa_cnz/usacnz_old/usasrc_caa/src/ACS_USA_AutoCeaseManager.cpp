//******************************************************************************
//
// NAME
//      ACS_USA_AutoCeaseManager.cpp
//
// COPYRIGHT
//      Ericsson AB 2008 - All rights reserved
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
//      ACS_USA_AutoCeaseManager, a singleton running in its own thread, 
//      holds a list of events to auto cease.
//      Methods supports initialization of the object, starting/stopping the 
//      thread, and adding events to the cease lists.
//
// DOCUMENT NO
//      .
//
// AUTHOR 
//      2008-02-15  EUS/DR/DB EKAMSBA
//
// REV  DATE      NAME     DESCRIPTION
// A    080215    EKAMSBA  First version.
// B    080409    EKAMSBA  Temp. file versioning.
//
//******************************************************************************

#include "ACS_USA_AutoCeaseManager.h"
#include "ACS_USA_EventManager.h"
#include "ACS_USA_File.h"
#include <ACS_TRA_trace.H>
#include "ACS_USA_FileVersioning.h"

#include <time.h>
#include <process.h>

using namespace std;


/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
// Trace point definitions
//******************************************************************************
ACS_TRA_trace traceAutoCease  = ACS_TRA_DEF("ACS_USA_AutoCeaseManager", "C128");

const char* const ACS_USA_traceLoadEvent  = "Loading %d event(s) from disk";
const char* const ACS_USA_traceStart      = "Task started";
const char* const ACS_USA_traceAddEvent   = "Adding %d event(s) to the queue (%d) - Succeed";
const char* const ACS_USA_traceAddTimeOut = "Adding %d event(s) to the queue (%d) - TimeOut";
const char* const ACS_USA_traceSaveEvent  = "Saving %d event(s) to disk";
const char* const ACS_USA_traceCeasing    = "Alarm (time: %d, duration: %d sec) is ceased at %d";
const char* const ACS_USA_traceEnd        = "Task stopped";




//******************************************************************************
// Constants used locally
//******************************************************************************
const char* const ACS_USA_AutoCeaseAlarm  = "AUTO CEASE";
const char* const ACS_USA_AutoCeaseData   = "Auto cease (alarm time: %s, duration: %d sec)";

const char* const ACS_USA_AutoCeaseTimerError     = "CreateWaitableTimer";
const char* const ACS_USA_AutoCeaseThreadError    = "CreateThread";
const char* const ACS_USA_AutoCeaseSetTimerError  = "SetWaitableTimer";
const char* const ACS_USA_AutoCeaseWaitError      = "WaitForSingleObject";
const char* const ACS_USA_AutoCeaseCSError        = "Failed to enter Critical Section";
const char* const ACS_USA_AutoCeaseNotInit        = "Object not initialized";
const char* const ACS_USA_AutoCeaseStartedError   = "Task already running";
const char* const ACS_USA_AutoCeaseReportError    = "EventManager.report() failure";
const char* const ACS_USA_AutoCeaseFileError      = "Empty file name";
const char* const ACS_USA_AutoCeaseFindFile       = "FindFirstFile";
const char* const ACS_USA_AutoCeaseMoveFile       = "MoveFileEx";


const   DWORD   GRACE_TIME_MSEC         = 8000L;  // Time to wait for the thread to finish.
const   DWORD   TIMEOUT_MSEC            = 5000L;  // Time-out value for the timer.
const   DWORD   DEFAULT_WAITTIME_SEC    = 60L;    // Default value for the timer.
const   DWORD   ADD_TRY_COUNT           = 15;     // Number of tries to access protected queue.
const   DWORD   TRY_LOCK_DELAY_MSEC     = 1000L;  // Time to wait before next trying access critical section.


const short	ACS_USA_CeaseTempRevision = 1;	// Current 'cease.tmp' file revision.

//******************************************************************************
// Helper class local to this class
//******************************************************************************

// Auto-handling of a critical section
class CriticalSectionHandle
{

public:

    // Constructor
    CriticalSectionHandle( CRITICAL_SECTION& critSection ) 
        : _critSection(critSection), _locked(FALSE)
    {
    }

    // Destructor
    ~CriticalSectionHandle()
    {
        if( _locked == TRUE )
        {
            // Release the critical section
            LeaveCriticalSection( &_critSection );
            _locked = FALSE;
        }
    }

    // Locks the critical section if not yet.
    void Lock()
    {
        if( _locked == FALSE )
        {
            // This is a BLOCKING call.
            EnterCriticalSection( &_critSection );
            _locked = TRUE;
        }
    }
    
    // Try n-times to lock the critical section.
    // Returns true if succeed, otherwise false.
    bool TimeOutLock( DWORD timesToTry )
    {   
        if( _locked == FALSE )
        {
            // This is a NON-BLOCKING call.
            while( ( _locked = TryEnterCriticalSection( &_critSection ) ) == FALSE &&
                    timesToTry-- > 0 )
            {
                Sleep( TRY_LOCK_DELAY_MSEC ); // wait 1 second before next try.
            }
        }

        return ( _locked == TRUE );
    }

private:

    CriticalSectionHandle( const CriticalSectionHandle& );              // copy constructor
    CriticalSectionHandle& operator=( const CriticalSectionHandle&);    // copy assignment

    CRITICAL_SECTION& _critSection;     // Reference to the managed critical section.
    BOOL              _locked;          // Status of the critical section.
};


// Function object used to merge elements to the priority queue;
class MergeToQueue
{

public:

    // Constructor
    MergeToQueue ( ACS_USA_EventQueue& queue ) : _Queue ( queue ) {}

    // operator()
    void operator() ( const ACS_USA_EventInfo& elem )
    {
        _Queue.push( elem );
    }

private:

    ACS_USA_EventQueue&     _Queue;     // Reference to event queue.
};


// Function object used to remove duplicate events
class NoDuplicate
{
public:   
   NoDuplicate ( vector<ACS_USA_EventInfo>& vec ) : _Vec ( vec ) 
   {}

   void operator() ( const ACS_USA_EventInfo& elem )
   {
	   // Not found
	   if (find(_Vec.begin(), _Vec.end(), elem) == _Vec.end())
	   {
			_Vec.push_back( elem );
	   }
   }

private:
   vector<ACS_USA_EventInfo>& _Vec;
};


//******************************************************************************
//  ThreadProc()
//******************************************************************************
DWORD WINAPI ACS_USA_AutoCeaseManager::ThreadProc( LPVOID lpParam )
{       
    ((ACS_USA_AutoCeaseManager*)lpParam)->autoCeaseGenerator();
    _endthreadex( 0 );
    return 0;
}

//******************************************************************************
//  getInstance()
//******************************************************************************
ACS_USA_AutoCeaseManager& 
ACS_USA_AutoCeaseManager::getInstance()
{
    static ACS_USA_AutoCeaseManager inst;
    return inst;
}

//******************************************************************************
//  ACS_USA_AutoCeaseManager()
//******************************************************************************
ACS_USA_AutoCeaseManager::ACS_USA_AutoCeaseManager() 
                : m_hThread( NULL ), 
                  m_bContinue( false ), 
                  m_dwMillisecondsGrace( GRACE_TIME_MSEC ),
                  m_dwMillisecondsTimeOut( TIMEOUT_MSEC ),
                  m_dwSecondsDefaultWaitTime( DEFAULT_WAITTIME_SEC ),
                  m_dwAddEventTryCount( ADD_TRY_COUNT ),
                  m_hWaitTimer( NULL ),
                  m_sFileName()
{
    static const char METHOD [] = "AutoCeaseManager::ACS_USA_AutoCeaseManager()";

    // In low memory situations, 
    // InitializeCriticalSection can raise a STATUS_NO_MEMORY exception.
    InitializeCriticalSection( &m_critSection );
}

//******************************************************************************
//  ~ACS_USA_AutoCeaseManager()
//******************************************************************************
ACS_USA_AutoCeaseManager::~ACS_USA_AutoCeaseManager() 
{
    static const char METHOD [] = "AutoCeaseManager::~ACS_USA_AutoCeaseManager()";

    try
    {
        // Stops the thread, if running,
        // Then closes handles.
        stopAutoCeasingTask();
        
        // Delete critical section
        DeleteCriticalSection( &m_critSection );
    }
    catch(...)
    {
    }
}

//******************************************************************************
//  startAutoCeasingTask()
//******************************************************************************
ACS_USA_Boolean 
ACS_USA_AutoCeaseManager::startAutoCeasingTask()
{
    static const char METHOD [] = "AutoCeaseManager::startAutoCeasingTask()";

    // Create a WaitableTimer
    if( m_hWaitTimer == NULL )
    {
        m_hWaitTimer = CreateWaitableTimer(NULL, TRUE,  "Global\\AUTO_CEASE_WAITABLE_TIMER");
        if( m_hWaitTimer == NULL )
        {
            error.setError( ACS_USA_SystemCallError );
            error.setErrorText( METHOD, 
                                getLastError().c_str(),
                                ACS_USA_AutoCeaseTimerError );

            reportProblem( error );
            return ACS_USA_False;
        }

        // This call should always succeed
        CriticalSectionHandle   cs( m_critSection );
        if( cs.TimeOutLock( m_dwAddEventTryCount ) == false )
        {           
            error.setError( ACS_USA_BadState );
            error.setErrorText( METHOD,
                                ACS_USA_AutoCeaseCSError );
            
            reportProblem( error );
            return ACS_USA_False;
        }

        // There are pending events in the list
        if( !m_queueEventToCease.empty() )
        {
            // Set the timer
            if( resetTimer( ) == ACS_USA_False )
            {
                error.setError( ACS_USA_SystemCallError );
                error.setErrorText( METHOD,
                                    getLastError().c_str(),
                                    ACS_USA_AutoCeaseSetTimerError );

                reportProblem( error );
                return ACS_USA_False;
            }
        }
    }

    // Let the thread loop forever
    m_bContinue = true;

    // Start working thread.
    if( m_hThread == NULL )
    {
        
        // beginthreadex() is used since CreateThread should not be used when C-run time 
        // functions are used in the code. Memory leaks can otherwise occur.
        //
        unsigned threadID;
        m_hThread = (HANDLE)_beginthreadex( 
                                    NULL,
                                    0,
                                    (unsigned int ( __stdcall * )( LPVOID ))ACS_USA_AutoCeaseManager::ThreadProc,
                                    this,
                                    0,
                                    &threadID
                                );

        if( m_hThread == NULL )
        {
            error.setError( ACS_USA_SystemCallError );
            error.setErrorText( METHOD,
                                getLastError().c_str(),
                                ACS_USA_AutoCeaseThreadError );

            reportProblem( error );
            return ACS_USA_False;
        }
    }


    if (ACS_TRA_ON(traceAutoCease)) 
    {
        // 'Task started'
        ACS_TRA_event(&traceAutoCease, ACS_USA_traceStart);
    }

    return ACS_USA_True;
}

//******************************************************************************
//  stopAutoCeasingTask()
//******************************************************************************
ACS_USA_Boolean 
ACS_USA_AutoCeaseManager::stopAutoCeasingTask()
{
    static const char METHOD [] = "AutoCeaseManager::stopAutoCeasingTask()";

    // Stop worker thread
    if ( m_hThread != NULL )
    {
        // Order the thread to stop running.
        m_bContinue = false;       

        // Wait for thread exit...
        if( WaitForSingleObject(m_hThread, m_dwMillisecondsGrace) == WAIT_TIMEOUT )
        {
            TerminateThread(m_hThread, -1);
        }

        CloseHandle(m_hThread); 
        m_hThread = NULL;
    }

    // Release the waitable timer
    if( m_hWaitTimer != NULL )
    {
        CloseHandle( m_hWaitTimer );
        m_hWaitTimer = NULL;
    }


    if (ACS_TRA_ON(traceAutoCease)) 
    {
        // 'Task stopped'
        ACS_TRA_event(&traceAutoCease, ACS_USA_traceEnd);
    }

    return ACS_USA_True;
}

//******************************************************************************
//  addEvents()
//******************************************************************************
ACS_USA_AutoCeaseManager::ACS_USA_CeaseFaultCode 
ACS_USA_AutoCeaseManager::addEvents( const vector<ACS_USA_EventInfo>& events )
{
    static const char METHOD [] = "AutoCeaseManager::addEvents()";

    // Task not started
    if( m_hThread == NULL ||  m_hWaitTimer == NULL )
    {
        error.setError( ACS_USA_BadState );
        error.setErrorText( METHOD,
                            ACS_USA_AutoCeaseNotInit );

        reportProblem( error );
        return Fault;
    }

    // No events !
    if ( events.empty() == true )
    {
        return NoFault;
    }

    // Try n-times to get access to the critical section ...
    CriticalSectionHandle   cs( m_critSection );
    if( cs.TimeOutLock( m_dwAddEventTryCount ) == false )
    {
        if (ACS_TRA_ON(traceAutoCease)) 
        {
            const unsigned short buffSize = 63;
            char msg[buffSize+1] = {0}; // +1 for the null char

            // 'Adding %d event(s) to the queue (%d) - TimeOut'
            _snprintf( msg, buffSize, ACS_USA_traceAddTimeOut
                        , events.size()
                        , m_queueEventToCease.size() );
            
            ACS_TRA_event(&traceAutoCease, msg);             
        }

        return TimeOut;
    }

	// Remove duplicates before merging into the queue...
	vector<ACS_USA_EventInfo> tempEvents = removeDuplicates( events );
	if( tempEvents.empty() == true )
	{
		return NoFault;
	}


    // Add new elements to the priority queue.
    for_each( tempEvents.begin(), tempEvents.end(), MergeToQueue(m_queueEventToCease) );

    // Save list content to disk.
    if ( saveList() == ACS_USA_error )
    {       
        error.setError( ACS_USA_FileIOError );
        error.setErrorText( METHOD,
                            ACS_USA_WriteFailed );

        reportProblem( error );
        return Fault;
    }

    // Set the timer
    if( resetTimer() == ACS_USA_False )
    {
        error.setError( ACS_USA_SystemCallError );
        error.setErrorText( METHOD, 
                            getLastError().c_str(),
                            ACS_USA_AutoCeaseSetTimerError );

        reportProblem( error );
        return Fault;
    }


    if (ACS_TRA_ON(traceAutoCease)) 
    {
        const unsigned short buffSize = 63;
        char msg[buffSize+1] = {0}; // +1 for the null char

        // 'Adding %d event(s) to the queue (%d)'
        _snprintf( msg, buffSize, ACS_USA_traceAddEvent
                    , events.size()
                    , m_queueEventToCease.size() );
        
        ACS_TRA_event(&traceAutoCease, msg);             
    }


    return NoFault;
}

//******************************************************************************
//  removeDuplicates()
//******************************************************************************
vector<ACS_USA_EventInfo> 
ACS_USA_AutoCeaseManager::removeDuplicates( const vector<ACS_USA_EventInfo>& aEvents ) const
{
    static const char METHOD [] = "AutoCeaseManager::removeDuplicates()";

    // IMPORTANT !!!

    // This function uses a shared resources (the priority queue).
    // All calls to it are already from within a critical section.

    // Therefore, never lock the critical section here, or DeadLock.


	// Remove duplicate from the vector
	vector<ACS_USA_EventInfo> tempEvents;
	for_each( aEvents.begin(), aEvents.end(), NoDuplicate(tempEvents) );
	//
	// -> At this stage, tempEvents doesn't contain any duplicates


	// Remove from the vector what is already in the queue
    ACS_USA_EventQueue eventsToCease(m_queueEventToCease);
    while ( !eventsToCease.empty( ) )
    {
        ACS_USA_EventInfo event = eventsToCease.top();

		tempEvents.erase (
			remove (tempEvents.begin( ),tempEvents.end( ), event),
			tempEvents.end() 
			);
		
        eventsToCease.pop( ); 

		if( tempEvents.empty() == true )
		{
			break;
		}
    }   
	//
	//-> At this stage, we have unique event, not present in the queue

	return tempEvents;
}


//******************************************************************************
//  autoCeaseGenerator()
//******************************************************************************
void
ACS_USA_AutoCeaseManager::autoCeaseGenerator()
{
    static const char METHOD [] = "AutoCeaseManager::autoCeaseGenerator()";
    
    // The thread will loop as long as m_bContinue is true
	// The thread is ordred to exit, in the destructor, by setting 
	// m_bContinue to false
    while( m_bContinue )
    {
        DWORD dwWaitResult = WaitForSingleObject( m_hWaitTimer, m_dwMillisecondsTimeOut );

        switch (dwWaitResult) 
        {
        case WAIT_OBJECT_0:             
            // Timer signaled.
			// The thread could be ordered to exit, so check m_bContinue first.
            if( m_bContinue == false )
            {
                return;
            }
            else
            {   
                break; 
            }

        case WAIT_TIMEOUT: 
            // Timer time-out.
            continue; 

        default:
            // WaitForSingleObject failed !!!           
            {
                m_threadError.setError( ACS_USA_SystemCallError );
                m_threadError.setErrorText( METHOD,
                                          getLastError().c_str(),
                                          ACS_USA_AutoCeaseWaitError );

                reportProblem( m_threadError );
            }

            // EXIT THE THREAD!
            return; 
        }

        // Enter a critical section.
        CriticalSectionHandle   cs( m_critSection );
        cs.Lock();

        while ( m_bContinue && !m_queueEventToCease.empty( ) )
        {
            // Retrieve current time.
            m_clock.reset();
            ACS_USA_TimeType currentTime = m_clock.seconds();

            // Get cease time of the high priority event.
            ACS_USA_TimeType nextCeaseTime =  m_queueEventToCease.top().alarmTime() + 
                                              m_queueEventToCease.top().ceaseDuration();

            if( currentTime >= nextCeaseTime )
            {
                time_t alarmTime = m_queueEventToCease.top().alarmTime();

                const unsigned short buffSize = 127;
                char problemData[buffSize+1] = {0}; // +1 for the null char

                // 'Auto cease (alarm time: %s, duration: %d sec)'
                // 'Auto cease (alarm time: Thu Feb 07 17:15:53 2008, duration: 60 sec)'
                _snprintf( problemData, buffSize, ACS_USA_AutoCeaseData
                        , ctime( &alarmTime )
                        , m_queueEventToCease.top().ceaseDuration() );

                // Generate a cease event
                if ( eventManager.report( m_queueEventToCease.top().specificProblem()
                                        , Key_perceivedSeverity_CEASING
                                        , ""
                                        , m_queueEventToCease.top().objectOfReference()
                                        , problemData
                                        , ACS_USA_AutoCeaseAlarm
                                        , ""
                                        , 0) == ACS_USA_error) 
                {
                    // A cease event is lost !!!

                    m_threadError.setError( ACS_USA_APeventHandlingError );
                    m_threadError.setErrorText( METHOD,
                                                ACS_USA_AutoCeaseReportError ); 

                    reportProblem( m_threadError );

                    // EXIT THE THREAD!
                    return;
                }
                else
                {
                    if (ACS_TRA_ON(traceAutoCease))
                    {
                        const unsigned short buffSize = 127;
                        char msg[buffSize+1] = {0}; // +1 for the null char

                        // 'Alarm (time: %d, duration: %d sec) is ceased at %d'
                        _snprintf( msg, buffSize, ACS_USA_traceCeasing 
                                    , m_queueEventToCease.top().alarmTime()
                                    , m_queueEventToCease.top().ceaseDuration()
                                    , currentTime );

                        ACS_TRA_event(&traceAutoCease, msg);                   
                    }
                }

                // Remove processed event.
                m_queueEventToCease.pop();
            }
            else
            {
                // No candidate.
                break;
            }
        }


        // Save list content to disk.
        if ( m_bContinue && (saveList() == ACS_USA_False) )
        {
            m_threadError.setError( ACS_USA_FileIOError );
            m_threadError.setErrorText( METHOD,
                                        ACS_USA_WriteFailed );

            reportProblem( m_threadError );
            
            // Exit the thread
            return;
        }

        if( m_bContinue && (resetTimer() == ACS_USA_False) )
        {
            m_threadError.setError( ACS_USA_SystemCallError );
            m_threadError.setErrorText( METHOD, 
                                        getLastError().c_str(),
                                        ACS_USA_AutoCeaseSetTimerError );

            reportProblem( m_threadError );
            
            // Exit the thread
            return;
        }       
    }
}

//******************************************************************************
//  isCeaseThreadAlive()
//****************************************************************************
ACS_USA_Boolean 
ACS_USA_AutoCeaseManager::isCeaseThreadAlive()
{
    static const char METHOD [] = "AutoCeaseManager::isCeaseThreadAlive()";

    if( m_hThread == NULL )
    {
        return ACS_USA_False;
    }

    // Thread is running...
    if( WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT )
    {
        return ACS_USA_True;
    }

    // WAIT_OBJECT_0, or any: thread exited
    stopAutoCeasingTask();
    return ACS_USA_False;
}

//******************************************************************************
//  resetTimer()
//******************************************************************************
ACS_USA_Boolean 
ACS_USA_AutoCeaseManager::resetTimer()
{
    static const char METHOD [] = "AutoCeaseManager::resetTimer()";

    // IMPORTANT !!!

    // This function uses a shared resources (the priority queue).
    // All calls to it are already from within a critical section.

    // Therefore, never lock the critical section here, or DeadLock.


    LONGLONG nSecondsToWait = m_dwSecondsDefaultWaitTime;

    if ( m_queueEventToCease.empty() == false )
    {
        // Retrieve current time.
        m_clock.reset();
        ACS_USA_TimeType currentTime = m_clock.seconds();

        // Get cease time of the priority event.
        ACS_USA_TimeType nextCeaseTime = m_queueEventToCease.top().alarmTime() + 
                                         m_queueEventToCease.top().ceaseDuration();

        if( nextCeaseTime > currentTime )
        {
            nSecondsToWait = nextCeaseTime - currentTime;
        }
        else
        {
            nSecondsToWait = 0;
        }
    }

    // Set the timer in 100 nanosecond.
    LARGE_INTEGER liDueTime;  
    liDueTime.QuadPart=-(nSecondsToWait * 10000000); 

    if ( SetWaitableTimer(m_hWaitTimer, &liDueTime, 0, NULL, NULL, 0) == 0 )
    {
        return ACS_USA_False;
    }

    return ACS_USA_True;
}

//******************************************************************************
//  loadList()
//******************************************************************************
ACS_USA_StatusType 
ACS_USA_AutoCeaseManager::loadList(const String& fileName) 
{   
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                        !
	// ! if changing that function will impacts the file ‘cease.tmp’, !
	// ! Then increment the revision ‘ACS_USA_CeaseTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    static const char METHOD [] = "AutoCeaseManager::loadList()";

    // Task already started
    if( isCeaseThreadAlive() == ACS_USA_True )
    {
        error.setError( ACS_USA_BadState );
        error.setErrorText( METHOD,
                            ACS_USA_AutoCeaseStartedError );

        reportProblem( error );
        return ACS_USA_error;
    }

    // Empty fileName
    if( fileName.empty() )
    {        
        error.setError( ACS_USA_FileIOError );
        error.setErrorText( METHOD,
                            ACS_USA_AutoCeaseFileError );

        reportProblem( error );
        return ACS_USA_error;
    }

    // Save file name
    m_sFileName = fileName;

	// Check the file integrity
	if( integrityCheck() == ACS_USA_False )
	{
		// Bad version or old file.
		if ( error.getError() == ACS_USA_BadState )
		{
			// Clear error
			error.setError(ACS_USA_NoError);
			error.setErrorText(noErrorText);
			
			// Rename it, and continue, new one will be created
			String oldName = m_sFileName + ".old";

			BOOL ok = MoveFileEx(
									m_sFileName.data(), 
									oldName.data(),
									MOVEFILE_REPLACE_EXISTING
								);

			if ( 0 == ok )
			{
				error.setError( ACS_USA_SystemCallError );
				error.setErrorText( METHOD, 
									getLastError().c_str(),
									ACS_USA_AutoCeaseMoveFile );

				reportProblem( error );
				return ACS_USA_error;
			}
		}
		else
		{
			// An error occurred
			reportProblem( error );
			return ACS_USA_error;
		}
	}


    // Open file and read data...
    ACS_USA_EventQueue  eventsToCease;

    try
    {
        File    file(m_sFileName, "rb");

        // Is the file object valid ?
        if (file.isValid() == false) 
        {
            error.setError( ACS_USA_FileIOError );
            error.setErrorText( METHOD,
                                m_sFileName.c_str(),
                                ACS_USA_CannotOpenFile );

            reportProblem( error );
            return ACS_USA_error;
        }

        // None empty file.
        if (file.isEmpty() == false) 
        {
			ACS_USA_FileVersion	version = 0;

			// Read the file version
			if (file.Error() ||
				file.Read(version) == false) 
			{
                error.setError( ACS_USA_FileIOError );
                error.setErrorText( METHOD,
                                    m_sFileName.c_str(),
                                    ACS_USA_ReadFailed );

                reportProblem( error );
                return ACS_USA_error;
			}

			// No need to check again as 
			// Integrity check just ran above.
			
            unsigned int eventCount = 0;

            if ( file.Read(eventCount) == false )
            {
                error.setError( ACS_USA_FileIOError );
                error.setErrorText( METHOD,
                                    m_sFileName.c_str(),
                                    ACS_USA_ReadFailed );

                reportProblem( error );
                return ACS_USA_error;
            }

            // Read events to cease from tmp file
            for (unsigned int i = 0; i < eventCount; i++) 
            {
                ACS_USA_EventInfo   event;

                if (event.read(file) == ACS_USA_error) 
                {
                    error.setError( ACS_USA_FileIOError );
                    error.setErrorText( METHOD,
                                        m_sFileName.c_str(),
                                        ACS_USA_ReadFailed );

                    reportProblem( error );
                    return ACS_USA_error;
                }

                eventsToCease.push(event);
            }
        }
    }
    catch(...)
    {
        error.setError( ACS_USA_FileIOError );
        error.setErrorText( METHOD,
                            m_sFileName.c_str(),
                            ACS_USA_ReadFailed );

        reportProblem( error );
        return ACS_USA_error;          
    }


    // This call, from main thread, should always succeed
    CriticalSectionHandle   cs( m_critSection );
    if( cs.TimeOutLock( m_dwAddEventTryCount ) == false )
    {
        error.setError( ACS_USA_BadState );
        error.setErrorText( METHOD,
                            ACS_USA_AutoCeaseCSError );

        reportProblem( error );
        return ACS_USA_error;
    }


    // Update the priority queue.
    m_queueEventToCease = eventsToCease;

    if (ACS_TRA_ON(traceAutoCease))
    {
        const unsigned short buffSize = 63;
        char msg[buffSize+1] = {0}; // +1 for the null char

        // 'Loading %d event(s) from disk'
        _snprintf( msg, buffSize, ACS_USA_traceLoadEvent, m_queueEventToCease.size() );

        ACS_TRA_event(&traceAutoCease, msg);             
    }

    return ACS_USA_ok;   
}

//******************************************************************************
//  saveList()
//******************************************************************************
ACS_USA_StatusType 
ACS_USA_AutoCeaseManager::saveList() const
{   
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                        !
	// ! if changing that function will impacts the file ‘cease.tmp’, !
	// ! Then increment the revision ‘ACS_USA_CeaseTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    static const char METHOD [] = "AutoCeaseManager::saveList()";

    // IMPORTANT !!!

    // This function uses a shared resources (the priority queue).
    // All calls to it are already from within a critical section.

    // Therefore, never lock the critical section here, or DeadLock.


    if( m_sFileName.empty() )
    {
        return ACS_USA_error;
    }

    try
    {	
        File    file(m_sFileName);

        // Is the file object valid ?
        if (file.isValid() == false) 
        {
            return ACS_USA_error;
        }

		// Write the file version
		ACS_USA_FileVersioning ceaseTemp( ACS_USA_CeaseTempRevision );		
		if (file.Error() ||
			file.Write( ceaseTemp.getVersion() ) == false) 
		{
			return ACS_USA_error;
		}


        // Take a copy
        ACS_USA_EventQueue eventsToCease(m_queueEventToCease);

        // Save data to disk
        unsigned int eventCount = eventsToCease.size();

        if ( file.Write(eventCount) == false )
        {
            return ACS_USA_error;
        }

        // Write events to disk.
        while ( !eventsToCease.empty( ) )
        {
            ACS_USA_EventInfo event = eventsToCease.top();

            if( event.write(file) == ACS_USA_error )
            {
                return ACS_USA_error;
            }

            eventsToCease.pop( );       
        }
    }
    catch(...)
    {
        return ACS_USA_error;          
    }


    if (ACS_TRA_ON(traceAutoCease))
    {
        const unsigned short buffSize = 63;
        char msg[buffSize+1] = {0}; // +1 for the null char

        // 'Saving %d event(s) to disk'
        _snprintf( msg, buffSize, ACS_USA_traceSaveEvent, m_queueEventToCease.size() );

        ACS_TRA_event(&traceAutoCease, msg);             
    }

    return ACS_USA_ok;  
}

//******************************************************************************
//  getLastError()
//******************************************************************************
String 
ACS_USA_AutoCeaseManager::getLastError() const
{   
    String errorText("Call to GetLastError() failed");
    LPVOID lpMsgBuf;
    
    if( FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_MAX_WIDTH_MASK  | // buffersize(255) - ignores inserted
                                         // control sequences such as char 13.
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        0,
        (LPTSTR) &lpMsgBuf,
        0, NULL ) != 0 )
    {
        errorText = (char*)lpMsgBuf;
        LocalFree(lpMsgBuf);
    }

    return errorText; 
}

//========================================================================================
//  reportProblem()
//========================================================================================
void 
ACS_USA_AutoCeaseManager::reportProblem( const ACS_USA_Error& what ) const
{
    ACS_USA_ErrorType err = what.getError();

    String perceivedSeverity(Key_perceivedSeverity_EVENT);
    String probableCause(ACS_USA_FaultCause);

    if (eventManager.report(
                ACS_USA_FirstErrorCode + err,
                perceivedSeverity,
                probableCause,
                String(),
                String(what.getErrorText()),
                String(what.getProblemText(what.getError())),
                "",
                0) == ACS_USA_error) 
    {
        // Noting could be done!
    }
}



//========================================================================================
//	integrityCheck()
//========================================================================================
ACS_USA_Boolean
ACS_USA_AutoCeaseManager::integrityCheck() const
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                        !
	// ! if changing that function will impacts the file ‘cease.tmp’, !
	// ! Then increment the revision ‘ACS_USA_CeaseTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	static const char METHOD [] = "AutoCeaseManager::integrityCheck()";

	// Check if the file exists
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(m_sFileName.c_str(), &FindFileData);

	if( hFind == INVALID_HANDLE_VALUE )  
	{
		if( ERROR_FILE_NOT_FOUND == GetLastError() )
		{
			// File not found. 
			// That's fine, it will be created.
			return ACS_USA_True;
		}
		else
		{
			error.setError( ACS_USA_SystemCallError );
			error.setErrorText( METHOD,
                                getLastError().c_str(),
								ACS_USA_AutoCeaseFindFile );

			return ACS_USA_False;
		}
	} 
	else 
	{
		// The file exists.		
		FindClose(hFind);
	}


    try
    {
        File    file(m_sFileName);

        // Is the file object valid ?
        if (file.isValid() == false) 
        {
            error.setError( ACS_USA_FileIOError );
            error.setErrorText( METHOD,
                                m_sFileName.c_str(),
                                ACS_USA_CannotOpenFile );

            return ACS_USA_False;
        }

        // None empty file.
        if (file.isEmpty() == true)
		{
			// The file is empty.
			// That's fine, this case is handled by loadList() function
			return ACS_USA_True;
		}
	
    
		ACS_USA_FileVersion	version = 0;

		// Read the file version
		if (file.Read(version) == false) 
		{
             error.setError( ACS_USA_FileIOError );
             error.setErrorText( METHOD,
                                 m_sFileName.c_str(),
                                 ACS_USA_ReadFailed );

             return ACS_USA_False;
		}


		// Check compatibility of the file version
		ACS_USA_FileVersioning ceaseTemp( ACS_USA_CeaseTempRevision );
		if( false == ceaseTemp.isCompatible(version) )
		{
			error.setError(ACS_USA_BadState);
            error.setErrorText( METHOD,
                                m_sFileName.c_str(),
								"Bad revision or old file format." );

			return ACS_USA_False;
		}    
    }
    catch(...)
	{
        error.setError( ACS_USA_FileIOError );
        error.setErrorText( METHOD,
                            m_sFileName.c_str(),
                            ACS_USA_ReadFailed );

		return ACS_USA_False;
	}

	// File is OK.
    return ACS_USA_True;
}

//******************************************************************************