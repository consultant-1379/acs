//******************************************************************************
//
// NAME
//      acs_usa_autoCeaseManager.cpp
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
//
// REV  DATE      NAME     DESCRIPTION
//     16-03-18  xfurull  Fix for TR HW69050 
//******************************************************************************

#include "acs_usa_autoCeaseManager.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_file.h"
#include "acs_usa_fileVersioning.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_error.h"
#include "acs_usa_global.h"
#include "acs_usa_reactorrunner.h"
#include "acs_usa_logtrace.h"
#include <time.h>
using namespace std;


/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
// Trace point definitions
//******************************************************************************
ACS_USA_Trace traAutoCease("ACS_USA_AutoCeaseManager	");

const char* ACS_USA_traceLoadEvent  = "Loading %d event(s) from disk";
const char* ACS_USA_traceStart      = "Task started";
const char* ACS_USA_traceAddEvent   = "Adding %d event(s) to the queue (%d) - Succeed";
const char* ACS_USA_traceAddTimeOut = "Adding %d event(s) to the queue (%d) - TimeOut";
const char* ACS_USA_traceSaveEvent  = "Saving %d event(s) to disk";
const char* ACS_USA_traceCeasing    = "Alarm (time: %lu, duration: %d sec) is ceased at %lu";
const char* ACS_USA_traceEnd        = "Task stopped";




//******************************************************************************
// Constants used locally
//******************************************************************************
const char* ACS_USA_AutoCeaseAlarm  = "AUTO CEASE";
const char* ACS_USA_AutoCeaseData   = "Auto cease (alarm time: %s, duration: %d sec)";

const char* ACS_USA_AutoCeaseTimerError     = "CreateWaitableTimer";
const char* ACS_USA_AutoCeaseThreadError    = "CreateThread";
const char* ACS_USA_AutoCeaseSetTimerError  = "SetWaitableTimer";
const char* ACS_USA_AutoCeaseWaitError      = "WaitForSingleObject";
const char* ACS_USA_AutoCeaseCSError        = "Failed to enter Critical Section";
const char* ACS_USA_AutoCeaseNotInit        = "Object not initialized";
const char* ACS_USA_AutoCeaseStartedError   = "Task already running";
const char* ACS_USA_AutoCeaseReportError    = "EventManager.report() failure";
const char* ACS_USA_AutoCeaseFileError      = "Empty file name";
const char* ACS_USA_AutoCeaseFindFile       = "FindFirstFile";
const char* ACS_USA_AutoCeaseMoveFile       = "Rename";


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
    CriticalSectionHandle( ACE_thread_mutex_t& critSection ) 
        : _critSection(critSection), _locked(ACE_FALSE)
    {
	USA_TRACE_ENTER();
	USA_TRACE_LEAVE();
    }

    // Destructor
    ~CriticalSectionHandle()
    {
	USA_TRACE_ENTER2("_locked [%d]", _locked);	    
        if( _locked == ACE_TRUE )
        {
            // Release the critical section
            ACE_OS::thread_mutex_unlock( &_critSection );
            _locked = ACE_FALSE;
        }
	USA_TRACE_LEAVE2("_locked [%d]", _locked);	    
    }

    // Locks the critical section if not yet.
    void Lock()
    {
	USA_TRACE_ENTER2("_locked [%d]", _locked);	    
        if( _locked == ACE_FALSE )
        {
            // This is a BLOCKING call.
	    ACE_OS::thread_mutex_lock( &_critSection );
            _locked = ACE_TRUE;
        }
	USA_TRACE_LEAVE2("_locked [%d]", _locked);	    
    }
    
    // Try n-times to lock the critical section.
    // Returns true if succeed, otherwise false.
    bool TimeOutLock( DWORD timesToTry )
    {   
	USA_TRACE_ENTER2("_locked [%d]", _locked);	    
        if( _locked == ACE_FALSE )
        {
            // This is a NON-BLOCKING call.
            while( ( _locked = ACE_OS::thread_mutex_trylock( &_critSection ) ) == ACE_FALSE &&
                    timesToTry-- > 0 )
            {
                usacc_msec_sleep(TRY_LOCK_DELAY_MSEC); // wait 1 second before next try.
            }
        }

	USA_TRACE_LEAVE2("_locked [%d]", _locked);	    
        return ( _locked == ACE_TRUE );
    }

private:

    CriticalSectionHandle( const CriticalSectionHandle& );              // copy constructor
    CriticalSectionHandle& operator=( const CriticalSectionHandle&);    // copy assignment

    ACE_thread_mutex_t _critSection;     // Reference to the managed critical section.
    int   _locked;          		// Status of the critical section.
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
//  svc()
//******************************************************************************
int ACS_USA_AutoCeaseManager::svc()
{      
	static const char METHOD [] = "AutoCeaseManager::svc()";
    
    	// The thread will loop as long as m_bContinue is true
	// The thread is ordred to exit, in the destructor, by setting 
	// m_bContinue to false
  
	USA_TRACE_ENTER2("[%s] - Thread id:[%x]", METHOD,(unsigned int)m_hThread);

	ACE_Message_Block* mb = NULL;
	m_bContinue=true;
    	// Schedule the timer first.
    	if (setTimer() < 0){
		m_threadError.setError( ACS_USA_SystemCallError );
		m_threadError.setErrorText(     METHOD,
						getLastError().c_str(),
						ACS_USA_AutoCeaseWaitError );
		reportProblem( m_threadError );
		traAutoCease.traceAndLogFmt(ERROR, "ACS_USA_AutoCeaseManager:%s() - setTimer Failed", __func__);
		USA_TRACE_LEAVE2("[%s] - setTimer Failed",METHOD);
		return -1;
    	}

	USA_TRACE_1("ACS_USA_AutoCeaseManager Thread running..");
    
    	while( m_bContinue )
    	{
		try {
			if (this->getq(mb) < 0){
				traAutoCease.traceAndLogFmt(ERROR, "ERROR: getq() failed");
				USA_TRACE_1("[%s] - ERROR: getq() failed", METHOD);
				break;
			}

			switch(mb->msg_type()){
	
				case ACM_TIMEOUT:	
						USA_TRACE_1("[%s] - ACM_TIMEOUT Received",METHOD);
    						if (this->autoCeaseGenerator() < 0) {
							// something bad happend in autocease manager
							// exit thread here
							m_bContinue=false;
						}		
						mb->release();
						break;

				case ACM_CLOSE:
						USA_TRACE_1("[%s] - ACM_CLOSE Received", METHOD);
						mb->release();
						m_bContinue=false;
						break;
				default:

						mb->release();
						m_bContinue=false;
						m_threadError.setError( ACS_USA_SystemCallError );
						m_threadError.setErrorText( 	METHOD,
										getLastError().c_str(),
										ACS_USA_AutoCeaseWaitError );
						reportProblem( m_threadError );
						break;
			}// end switch
		}// end try
		catch(...) {
			traAutoCease.traceAndLogFmt(ERROR, "ERROR: EXCEPTION!");
			USA_TRACE_1("[%s] - ERROR: EXCEPTION!",METHOD);
		}	
   	}// end of while		

	//call 'close(u_long)' at exit of 'svc'
	USA_TRACE_LEAVE2("[%s] - Thread Exiting.", METHOD);
	return 0;
}

//******************************************************************************
//	close(u_long)
//******************************************************************************
int ACS_USA_AutoCeaseManager::close(u_long) {
	
	// remove the timer now.
	USA_TRACE_ENTER2("ACS_USA_AutoCeaseManager:close(u_long");
	if (m_timerId != -1){
		USA_TRACE_1("ACS_USA_AutoCeaseManager:close(u_long) - Removing Timer");
		ACS_USA_Global::instance()->reactor()->cancel_timer(m_timerId);
	}	
	
	m_hThread=0;		
	USA_TRACE_LEAVE2("ACS_USA_AutoCeaseManager:close(u_long");
	return 0;
}

//******************************************************************************
//	close()
//******************************************************************************
int ACS_USA_AutoCeaseManager::close(){
	
	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN( mb, ACE_Message_Block());

	USA_TRACE_ENTER();

	if (0 == mb) {
		traAutoCease.traceAndLogFmt(INFO, "%s() Failed to create mb object", __func__);
		USA_TRACE_LEAVE2("ACS_USA_AutoCeaseManager:%s() Failed to create mb object", __func__);
		return -1;
	}

	USA_TRACE_1("ACS_USA_AutoCeaseManager:%s() - Posting ACM_CLOSE to svc", __func__);
	mb->msg_type(ACM_CLOSE);
	if (this->putq(mb) < 0){
		traAutoCease.traceAndLogFmt(ERROR, "%s() Fail to post ACM_CLOSE to ourself", __func__);
		USA_TRACE_LEAVE2("ACS_USA_AutoCeaseManager:%s() Fail to post ACM_CLOSE to ourself", __func__);
		mb->release();
		return -1;
	}	
	USA_TRACE_LEAVE();

	return 0;	
}


//******************************************************************************
//  getInstance()
//******************************************************************************
ACS_USA_AutoCeaseManager& 
ACS_USA_AutoCeaseManager::getInstance()
{
	USA_TRACE_ENTER();
	
	static ACS_USA_AutoCeaseManager inst;

	USA_TRACE_LEAVE();
	return inst;
}

//******************************************************************************
//  ACS_USA_AutoCeaseManager()
//******************************************************************************
ACS_USA_AutoCeaseManager::ACS_USA_AutoCeaseManager() 
                : m_hThread(0), 
                  m_bContinue( false ), 
                  m_dwMillisecondsGrace( GRACE_TIME_MSEC ),
                  m_dwMillisecondsTimeOut( TIMEOUT_MSEC ),
                  m_dwSecondsDefaultWaitTime( DEFAULT_WAITTIME_SEC ),
                  m_dwAddEventTryCount( ADD_TRY_COUNT ),
                  m_timerId(-1),
                  m_sFileName(),
		  m_reactorRunner(0)
{
    static const char METHOD [] = "AutoCeaseManager::ACS_USA_AutoCeaseManager()";

    // In low memory situations, 
    // InitializeCriticalSection can raise a STATUS_NO_MEMORY exception.
    USA_TRACE_ENTER2("%s", METHOD);
    
    ACE_OS::thread_mutex_init( &m_critSection );

    USA_TRACE_LEAVE2("%s", METHOD);
}

//******************************************************************************
//  ~ACS_USA_AutoCeaseManager()
//******************************************************************************
ACS_USA_AutoCeaseManager::~ACS_USA_AutoCeaseManager() 
{
    	static const char METHOD [] = "AutoCeaseManager::~ACS_USA_AutoCeaseManager()";
    	USA_TRACE_ENTER2("%s", METHOD);

	try
    	{
		if(m_hThread != 0){
			// Stops the thread, if running,
			// Then closes handles.
			stopAutoCeasingTask();
			m_hThread=0;
		}
        	// Delete critical section
		ACE_OS::thread_mutex_destroy( &m_critSection );
    	}
    	catch(...)
    	{
    	}

	USA_TRACE_LEAVE2("%s", METHOD);
}

//******************************************************************************
//  startAutoCeasingTask()
//******************************************************************************
int 
ACS_USA_AutoCeaseManager::startAutoCeasingTask()
{
    	static const char METHOD [] = "AutoCeaseManager::startAutoCeasingTask()";
	
	USA_TRACE_ENTER();

        // This call should always succeed
        CriticalSectionHandle   cs( m_critSection );
        if( cs.TimeOutLock( m_dwAddEventTryCount ) == false )
        {           
            error.setError( ACS_USA_BadState );
            error.setErrorText( METHOD,
                                ACS_USA_AutoCeaseCSError );
	    
            reportProblem( error );
	    USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_AutoCeaseCSError);
            return -1;
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
	    	USA_TRACE_LEAVE2("[%s] - [%s]", METHOD,ACS_USA_AutoCeaseSetTimerError);
                return -1;
            }
        }

       	// Create the thread now
       	if (this->activate(THR_NEW_LWP|THR_JOINABLE,
		       	1,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			0,
			0,
			0,
			&m_hThread) < 0) {
		
		error.setError( ACS_USA_SystemCallError );
		error.setErrorText( 	METHOD,
					getLastError().c_str(),
					ACS_USA_AutoCeaseThreadError );
		reportProblem( error );
		USA_TRACE_LEAVE2("[%s] - [%s]",METHOD,ACS_USA_AutoCeaseThreadError);
		return -1;
       	}	

	USA_TRACE_LEAVE();       
       	return 0;
}

//******************************************************************************
//  stopAutoCeasingTask()
//******************************************************************************
ACS_USA_Boolean 
ACS_USA_AutoCeaseManager::stopAutoCeasingTask()
{
	USA_TRACE_ENTER();

    	static const char METHOD [] = "AutoCeaseManager::stopAutoCeasingTask()";
	ACS_USA_Boolean rCode=ACS_USA_True;

    	if (this->close() < 0) {
		traAutoCease.traceAndLogFmt(ERROR, "ACS_USA_AutoCeaseManager:%s() Failed close()", __func__);
		USA_TRACE_1("[%s] - Failed to close", METHOD);
		rCode=ACS_USA_False;
    	}

	USA_TRACE_1("[%s] - Waiting for ACS_USA_AutoCeaseManager to close", METHOD);
	this->wait();

	USA_TRACE_LEAVE2("ACS_USA_AutoCeaseManager thread closed successfully");
    	return rCode;
}

//******************************************************************************
//  addEvents()
//******************************************************************************
ACS_USA_AutoCeaseManager::ACS_USA_CeaseFaultCode 
ACS_USA_AutoCeaseManager::addEvents( const vector<ACS_USA_EventInfo>& events )
{
	USA_TRACE_ENTER();

	static const char METHOD [] = "AutoCeaseManager::addEvents()";

	// Task not started
	if( m_hThread == 0)
	{
		error.setError( ACS_USA_BadState );
		error.setErrorText( 	METHOD,
					ACS_USA_AutoCeaseNotInit );

		reportProblem( error );
		USA_TRACE_LEAVE2("[%s]	- [%s]", METHOD, ACS_USA_AutoCeaseNotInit);
		return Fault;
	}

	// No events !
	if ( events.empty() == true )
	{
		USA_TRACE_LEAVE2("[%s]	- No Events", METHOD);
		return NoFault;
	}

	// Try n-times to get access to the critical section ...
    	CriticalSectionHandle   cs( m_critSection );
    	if( cs.TimeOutLock( m_dwAddEventTryCount ) == false )
    	{
       		const unsigned short buffSize = 63;
		char msg[buffSize+1] = {0}; // +1 for the null char
		// 'Adding %d event(s) to the queue (%d) - TimeOut'
		snprintf( msg, buffSize, ACS_USA_traceAddTimeOut, (int)events.size(),(int)m_queueEventToCease.size()); 
		USA_TRACE_LEAVE2("[%s] - TimeOutLock: [%s]",METHOD, msg);
        	return TimeOut;
    	}

	vector<ACS_USA_EventInfo> tempEvents ( events );
#if 0 
	// Remove duplicates before merging into the queue...
	vector<ACS_USA_EventInfo> tempEvents = removeDuplicates( events );
	if( tempEvents.empty() == true )
	{
		USA_TRACE_LEAVE2("[%s] - Empty temp events", METHOD);
		return NoFault;
	}
#endif

    	// Add new elements to the priority queue.
    	for_each( tempEvents.begin(), tempEvents.end(), MergeToQueue(m_queueEventToCease) );

    	// Save list content to disk.
    	if ( saveList() == ACS_USA_Error )
    	{       
        	error.setError( ACS_USA_FileIOError );
        	error.setErrorText( 	METHOD,
                            		ACS_USA_WriteFailed );

        	reportProblem( error );
		USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_WriteFailed);
        	return Fault;
    	}

	// Set the timer
	if( resetTimer() == ACS_USA_False )
	{
		error.setError( ACS_USA_SystemCallError );
		error.setErrorText( 	METHOD, 
					getLastError().c_str(),
					ACS_USA_AutoCeaseSetTimerError );

        	reportProblem( error );
		USA_TRACE_LEAVE2("[%s] - [%s]",METHOD, ACS_USA_AutoCeaseSetTimerError);
		return Fault;
    	}
	
	// For tracing
	{	
  		const unsigned short buffSize = 63;
		char msg[buffSize+1] = {0};
		snprintf( msg, buffSize, ACS_USA_traceAddEvent, (int)events.size(), (int)m_queueEventToCease.size());
		USA_TRACE_LEAVE2("[%s] - [%s]",METHOD,msg);
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

	USA_TRACE_ENTER2("[%s]", METHOD);
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
	USA_TRACE_LEAVE2("[%s]", METHOD);

	//-> At this stage, we have unique event, not present in the queue
	return tempEvents;
}

/******************************************************************************
 * 	handle_timeout
 * 	
 *****************************************************************************/
int ACS_USA_AutoCeaseManager::handle_timeout(const ACE_Time_Value&, const void* ) {

	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block());

	USA_TRACE_ENTER();

	// Post a new ACM_TIMEOUT message to svc.
	if (mb != 0) {
		mb->msg_type(ACM_TIMEOUT);
		if (this->putq(mb) < 0) {
			mb->release();
		}
	}

	USA_TRACE_LEAVE();
	return 0;
}	
//******************************************************************************
//  setTimer()
//******************************************************************************
int ACS_USA_AutoCeaseManager::setTimer(){

	USA_TRACE_ENTER();
	static const char METHOD [] = "AutoCeaseManager::setTimer()";

	// schedule a timer now
	const ACE_Time_Value schedule_time(m_dwMillisecondsTimeOut/1000);
	m_timerId = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);
	if (this->m_timerId < 0){
		traAutoCease.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", METHOD);
		m_threadError.setError(ACS_USA_SystemCallError);
		m_threadError.setErrorText( METHOD,
					    getLastError().c_str(),
					    ACS_USA_AutoCeaseSetTimerError);
		return -1;
	}

	USA_TRACE_LEAVE();
	return 0;
}

//******************************************************************************
//  autoCeaseGenerator()
//******************************************************************************
int
ACS_USA_AutoCeaseManager::autoCeaseGenerator()
{

	static const char METHOD [] = "AutoCeaseManager::autoCeaseGenerator()";				
	USA_TRACE_ENTER2("[%s]", METHOD);

        // Enter a critical section.
	CriticalSectionHandle   cs( m_critSection );
        cs.Lock();

        while ( !m_queueEventToCease.empty( ) )
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
                snprintf( problemData, buffSize, ACS_USA_AutoCeaseData
                        , ctime( &alarmTime )
                        , (int)m_queueEventToCease.top().ceaseDuration() );
	String obj_ref = m_queueEventToCease.top().objectOfReference().data();
	if  ((obj_ref.c_str() == String("ldap"))) {
	traAutoCease.traceAndLogFmt(INFO,"Not Sending AutoCease event for [%s]",obj_ref.c_str());
	}
	else {
	traAutoCease.traceAndLogFmt(INFO,"Sending AutoCease event for [%s]",obj_ref.c_str());
                // Generate a cease event
                if ( eventManager.report( m_queueEventToCease.top().specificProblem()
                                        , ACS_USA_PERCEIVED_SEVERITY_CEASING
                                        , ""
                                        , m_queueEventToCease.top().objectOfReference()
                                        , problemData
                                        , ACS_USA_AutoCeaseAlarm
                                        , ""
                                        , 0) == ACS_USA_Error) 
                {
                    // A cease event is lost !!!

                    m_threadError.setError( ACS_USA_APeventHandlingError );
                    m_threadError.setErrorText( METHOD,
                                                ACS_USA_AutoCeaseReportError ); 

                    reportProblem( m_threadError );
		    
                    // EXIT THE THREAD!
		    USA_TRACE_LEAVE2("[%s] - [%s]",METHOD,ACS_USA_AutoCeaseReportError);
                    return -1;
                }
                else
                {
			// For trace.
			const unsigned short buffSize = 127;
			char msg[buffSize+1] = {0}; // +1 for the null char
			snprintf( msg, buffSize, ACS_USA_traceCeasing
					, (unsigned long)m_queueEventToCease.top().alarmTime()
					, (int)m_queueEventToCease.top().ceaseDuration()
					, (unsigned long)currentTime );
			USA_TRACE_1("[%s] - msg[%s]", METHOD, msg);
                }
		}
                // Remove processed event.
                m_queueEventToCease.pop();
            }
 		//}		
            else
            {
                // No candidate.
                break;
            }
        }


        // Save list content to disk.
        if ( saveList() == ACS_USA_Error) 
        {
            m_threadError.setError( ACS_USA_FileIOError );
            m_threadError.setErrorText( METHOD,
                                        ACS_USA_WriteFailed );

            reportProblem( m_threadError );
            
            // Exit the thread
	    USA_TRACE_LEAVE2("[%s] - [%s]",METHOD,ACS_USA_WriteFailed);
            return -1;
        }

        if( resetTimer() == ACS_USA_False) 
        {
            m_threadError.setError( ACS_USA_SystemCallError );
            m_threadError.setErrorText( METHOD, 
                                        getLastError().c_str(),
                                        ACS_USA_AutoCeaseSetTimerError );

            reportProblem( m_threadError );
            
            // Exit the thread
	    USA_TRACE_LEAVE2("[%s] - [%s]", METHOD,ACS_USA_AutoCeaseSetTimerError);
            return -1;
        }       
	
	USA_TRACE_LEAVE2("[%s]", METHOD);
	return 0;
}

//******************************************************************************
//  isCeaseThreadAlive()
//****************************************************************************
ACS_USA_Boolean 
ACS_USA_AutoCeaseManager::isCeaseThreadAlive()
{
    	static const char METHOD [] = "AutoCeaseManager::isCeaseThreadAlive()";
	USA_TRACE_ENTER2("[%s]",METHOD);
	ACS_USA_Boolean rCode=ACS_USA_True;

    	if( m_hThread == 0)
    	{
		rCode=ACS_USA_False;
    	}

	USA_TRACE_LEAVE2("[%s] - m_hThread: %x", METHOD, (unsigned int)m_hThread);
    	return rCode;
}

//******************************************************************************
//  resetTimer()
//******************************************************************************
ACS_USA_Boolean 
ACS_USA_AutoCeaseManager::resetTimer()
{
    	static const char METHOD [] = "AutoCeaseManager::resetTimer()";
	USA_TRACE_ENTER2("[%s]", METHOD);

    	// IMPORTANT !!!

    	// This function uses a shared resources (the priority queue).
    	// All calls to it are already from within a critical section.

    	// Therefore, never lock the critical section here, or DeadLock.

    	unsigned long nSecondsToWait = m_dwSecondsDefaultWaitTime;
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

    	ACE_Time_Value interval(nSecondsToWait); // time in seconds
    	m_timerId = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, interval);
    	if (this->m_timerId < 0){
	    	traAutoCease.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
		USA_TRACE_LEAVE2("[%s] - Unable to schedule timer.", METHOD);
	    	return ACS_USA_False;
    	}	    

	USA_TRACE_LEAVE2("[%s]",METHOD);
    	return ACS_USA_True;
}

//******************************************************************************
//  loadList()
//******************************************************************************
ACS_USA_ReturnType 
ACS_USA_AutoCeaseManager::loadList(const String& fileName) 
{   
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                        !
	// ! if changing that function will impacts the file ‘cease.tmp’, !
	// ! Then increment the revision ‘ACS_USA_CeaseTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	static const char METHOD [] = "AutoCeaseManager::loadList()";
	USA_TRACE_ENTER2("[%s]", METHOD);

	// Task already started
	if( isCeaseThreadAlive() == ACS_USA_True )
	{
		error.setError( ACS_USA_BadState );
        	error.setErrorText( METHOD,
                            	    ACS_USA_AutoCeaseStartedError );
		
        	reportProblem( error );
		USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_AutoCeaseStartedError);
       	 	return ACS_USA_Error;
    	}	

    	// Empty fileName
    	if( fileName.empty() )
    	{        
        	error.setError( ACS_USA_FileIOError );
       	 	error.setErrorText( METHOD,
                	            ACS_USA_AutoCeaseFileError );

        	reportProblem( error );
		USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_AutoCeaseFileError);
        	return ACS_USA_Error;
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

			unsigned int ok = rename( m_sFileName.data(), 
						  oldName.data() );

			if ( 0 == ok )
			{
				error.setError( ACS_USA_SystemCallError );
				error.setErrorText( 	METHOD, 
							getLastError().c_str(),
							ACS_USA_AutoCeaseMoveFile );

				reportProblem( error );
				USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_AutoCeaseMoveFile);
				return ACS_USA_Error;
			}
		}
		else
		{
			// An error occurred
			USA_TRACE_LEAVE2("[%s] - error ", METHOD);
			reportProblem( error );
			return ACS_USA_Error;
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
			USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_CannotOpenFile);
            		return ACS_USA_Error;
        	}

        	// None empty file.
        	if (file.isEmpty() == false) 
        	{
			ACS_USA_FileVersion	version = 0;

			// Read the file version
			if (file.Error() || file.Read(version) == false) 
			{
                		error.setError( ACS_USA_FileIOError );
                		error.setErrorText( METHOD,
                                		    m_sFileName.c_str(),
                                    		    ACS_USA_ReadFailed );

                		reportProblem( error );
				USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_ReadFailed);
                		return ACS_USA_Error;
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
				USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_ReadFailed);
                		return ACS_USA_Error;
            		}

            		// Read events to cease from tmp file
            		for (unsigned int i = 0; i < eventCount; i++) 
            		{
                		ACS_USA_EventInfo   event;

                		if (event.read(file) == ACS_USA_Error) 
                		{
                    			error.setError( ACS_USA_FileIOError );
                    			error.setErrorText( METHOD,
                                        			m_sFileName.c_str(),
                                        			ACS_USA_ReadFailed );

                    			reportProblem( error );
					USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_ReadFailed);
                    			return ACS_USA_Error;
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
		USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_ReadFailed);
        	return ACS_USA_Error;          
    	}


    	// This call, from main thread, should always succeed
    	CriticalSectionHandle   cs( m_critSection );
    	if( cs.TimeOutLock( m_dwAddEventTryCount ) == false )
    	{
        	error.setError( ACS_USA_BadState );
        	error.setErrorText( METHOD,
                	            ACS_USA_AutoCeaseCSError );

        	reportProblem( error );
		USA_TRACE_LEAVE2("[%s] - [%s]",METHOD, ACS_USA_AutoCeaseCSError);
        	return ACS_USA_Error;
    	}


    	// Update the priority queue.
   	m_queueEventToCease = eventsToCease;

	{
		// For trace
		const unsigned short buffSize = 63;
		char msg[buffSize+1] = {0}; // +1 for the null char
		// 'Loading %d event(s) from disk'
		snprintf( msg, buffSize, ACS_USA_traceLoadEvent, (int)m_queueEventToCease.size() );
		USA_TRACE_LEAVE2("[%s] - msg[%s]", METHOD, msg);
	}

    	return ACS_USA_Ok;   
}

//******************************************************************************
//  saveList()
//******************************************************************************
ACS_USA_ReturnType 
ACS_USA_AutoCeaseManager::saveList() const
{   
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                        !
	// ! if changing that function will impacts the file ‘cease.tmp’, !
	// ! Then increment the revision ‘ACS_USA_CeaseTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	static const char METHOD [] = "AutoCeaseManager::saveList()";
	USA_TRACE_ENTER2("[%s]", METHOD);

	// IMPORTANT !!!

	// This function uses a shared resources (the priority queue).
	// All calls to it are already from within a critical section.

	// Therefore, never lock the critical section here, or DeadLock.

    	if( m_sFileName.empty() )
    	{
		USA_TRACE_LEAVE2("[%s] - m_sFileName.empty", METHOD);
        	return ACS_USA_Error;
    	}

    	try
    	{	
        	File file(m_sFileName);

        	// Is the file object valid ?
        	if (file.isValid() == false) 
        	{
            		return ACS_USA_Error;
        	}

		// Write the file version
		ACS_USA_FileVersioning ceaseTemp( ACS_USA_CeaseTempRevision );		
		if (file.Error() ||
			file.Write( ceaseTemp.getVersion() ) == false) 
		{
			USA_TRACE_LEAVE2("[%s]- Error in File Version", METHOD);
			return ACS_USA_Error;
		}


        	// Take a copy
        	ACS_USA_EventQueue eventsToCease(m_queueEventToCease);

        	// Save data to disk
        	unsigned int eventCount = eventsToCease.size();

        	if ( file.Write(eventCount) == false )
        	{
			USA_TRACE_LEAVE2("[%s] - Sava Data to Disk Failed", METHOD);
            		return ACS_USA_Error;
        	}

        	// Write events to disk.
        	while ( !eventsToCease.empty( ) )
        	{
            		ACS_USA_EventInfo event = eventsToCease.top();

            		if( event.write(file) == ACS_USA_Error )
            		{
				USA_TRACE_LEAVE2("[%s] - Write Events to Disk Failed",METHOD);
                		return ACS_USA_Error;
            		}

            		eventsToCease.pop( );       
        	}
    	}
    	catch(...)
    	{
		USA_TRACE_LEAVE2("[%s] - Unknown Error caught", METHOD);
       	 	return ACS_USA_Error;          
    	}
	
	{
		// Trace
		const unsigned short buffSize = 63;
		char msg[buffSize+1] = {0}; // +1 for the null char

		snprintf( msg, buffSize, ACS_USA_traceSaveEvent, (int)m_queueEventToCease.size() );

   		USA_TRACE_LEAVE2("[%s] - msg[%s]", METHOD, msg);
	}
    	return ACS_USA_Ok;  
}

//******************************************************************************
//  getLastError()
//******************************************************************************
String 
ACS_USA_AutoCeaseManager::getLastError() const
{   
	USA_TRACE_ENTER();
	
    	String errorText("Call to GetLastError() failed");

	USA_TRACE_LEAVE();
    
    	return errorText; 
}

//========================================================================================
//  reportProblem()
//========================================================================================
void 
ACS_USA_AutoCeaseManager::reportProblem( const acs_usa_error& what ) const
{
	USA_TRACE_ENTER();
    	ACS_USA_ErrorType err = what.getError();

    	String perceivedSeverity(ACS_USA_PERCEIVED_SEVERITY_EVENT);
    	String probableCause(ACS_USA_INTLFAULT);

    	if (eventManager.report(
        	        ACS_USA_FirstErrorCode + err,
                	perceivedSeverity,
	                probableCause,
	                String(),
	                String(what.getErrorText()),
	                String(what.getProblemText(what.getError())),
	                "",
	                0) == ACS_USA_Error) 
    	{
        // Noting could be done!
    	}

	USA_TRACE_LEAVE();
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
	USA_TRACE_ENTER2("[%s]", METHOD);

	try
    	{
        	File    file(m_sFileName);

        	// Is the file object valid ?
        	if (file.isValid() == false) 
        	{
            		error.setError( ACS_USA_FileIOError );
            		error.setErrorText( 	METHOD,
                        	        	m_sFileName.c_str(),
                                		ACS_USA_CannotOpenFile );

			USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_CannotOpenFile);
            		return ACS_USA_False;
        	}

        	// None empty file.
        	if (file.isEmpty() == true)
		{
			// The file is empty.
			// That's fine, this case is handled by loadList() function
			USA_TRACE_LEAVE2("[%s] - File is empty", METHOD);
			return ACS_USA_True;
		}
	
    
		ACS_USA_FileVersion	version = 0;
		// Read the file version
		if (file.Read(version) == false) 
		{
             		error.setError( ACS_USA_FileIOError );
             		error.setErrorText( 	METHOD,
                        	        	m_sFileName.c_str(),
                                	 	ACS_USA_ReadFailed );

			USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_ReadFailed);
             		return ACS_USA_False;
		}

		// Check compatibility of the file version
		ACS_USA_FileVersioning ceaseTemp( ACS_USA_CeaseTempRevision );
		if( false == ceaseTemp.isCompatible(version) )
		{
			error.setError(ACS_USA_BadState);
            		error.setErrorText( 	METHOD,
                              			m_sFileName.c_str(),
						"Bad revision or old file format." );

			USA_TRACE_LEAVE2("[%s] - [Bad revision or old file format]", METHOD);
			return ACS_USA_False;
		}    
    	}
    	catch(...)
	{
        	error.setError( ACS_USA_FileIOError );
        	error.setErrorText( METHOD,
                	            m_sFileName.c_str(),
                        	    ACS_USA_ReadFailed );

		USA_TRACE_LEAVE2("[%s] - [%s]", METHOD, ACS_USA_ReadFailed);
		return ACS_USA_False;
	}

	// File is OK.
	USA_TRACE_LEAVE2("[%s]", METHOD);
    	return ACS_USA_True;
}

//******************************************************************************
