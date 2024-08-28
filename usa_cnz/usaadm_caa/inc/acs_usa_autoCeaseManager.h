//******************************************************************************
// 
// NAME 
//        ACS_USA_AutoCeaseManager - Handling the auto cease events in USA.
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
//      2008-02-15	EUS/DR/DB EKAMSBA
//
// REV  DATE      NAME     DESCRIPTION
// A    080215    EKAMSBA  First version.
// B    080409    EKAMSBA  Temp. file versioning.
//
//******************************************************************************

#ifndef ACS_USA_AUTOCEASEMANAGER_H
#define ACS_USA_AUTOCEASEMANAGER_H


#include <queue>
#include <vector>
#include <ace/OS.h>
#include <ace/Task_T.h>
#include "acs_usa_error.h"
#include <ace/TP_Reactor.h>
#include <ace/Reactor.h>
#include "acs_usa_types.h"
#include "acs_usa_time.h"
#include "acs_usa_eventInfo.h"
#include "acs_usa_common.h"

//******************************************************************************
// Structures and definitions, local to this class
//******************************************************************************

#define ACE_FALSE -1
#define ACE_TRUE 0

// Predicate used to order events in the priority queue.
struct ACS_USA_EventPredicate
{
    bool operator()(const ACS_USA_EventInfo& left, const ACS_USA_EventInfo& right) const
    {
        // order by lowest cease time.
        return ( 
		(left.alarmTime() + left.ceaseDuration()) > (right.alarmTime() + right.ceaseDuration()) 
		);
    }
};

// Data structure used to store event info elements.
typedef std::priority_queue<ACS_USA_EventInfo, 
                            std::vector<ACS_USA_EventInfo>, 
                            ACS_USA_EventPredicate>             ACS_USA_EventQueue;


//******************************************************************************
// Member functions, constructors, destructors, operators
//******************************************************************************

class ACS_USA_ReactorRunner;

class ACS_USA_AutoCeaseManager: public ACE_Task<ACE_SYNCH>
{

public:
 
	int svc();

	int close(u_long);

	int close();    

	// For addEvents status
	enum ACS_USA_CeaseFaultCode 
	{
		NoFault = 0, // Events queued.      
		Fault,       // Add operation failed.
		TimeOut      // Add operation time-out.     
	};

	static ACS_USA_AutoCeaseManager& getInstance();
	// Description:
	//   Creates and returns a reference to a singleton object.
	// Parameters: 
	//   none
	// Return value: 
	//   ACS_USA_AutoCeaseManager&      Ref. to a singleton object
    
	~ACS_USA_AutoCeaseManager();
	// Description:
	//   Destructor, stops running thread and releases ressources.
	// Parameters: 
	//   none
	// Return value: 
	//   none
    
	ACS_USA_ReturnType loadList(const String& fileName);
	// Description:
	//   Loads from the disk the content of the queue.
	// Parameters:
	//   fileName		Path to the temp file
	// Return value: 
	//   ACS_USA_error  An error occurred
	//   ACS_USA_ok     success

	ACS_USA_Boolean startAutoCeasingTask();
	// Description:
 	//   Creates and launches the working thread.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   ACS_USA_True       success
    	//   ACS_USA_False      failure
        
    	ACS_USA_Boolean stopAutoCeasingTask();
    	// Description:
    	//   Stops the workiing thread.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   ACS_USA_True       success
    	//   ACS_USA_False      failure

    	ACS_USA_CeaseFaultCode addEvents( const std::vector<ACS_USA_EventInfo>& events );
    	// Description:
    	//   Adds events to the priority queue.
    	// Parameters: 
    	//   events     list of events to handle
    	// Return value: 
    	//   ACS_USA_CeaseFaultCode       fault code

    	ACS_USA_Boolean isCeaseThreadAlive();
    	// Description:
    	//   Heartbeat of the thread.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   ACS_USA_True       alive
    	//   ACS_USA_False      died

    	acs_usa_error getError() const;
    	// Description:
    	//   Return the error that caused the thread to died.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   acs_usa_error       error


protected: 
    
    	ACS_USA_AutoCeaseManager() ;
    	// Description:
    	//   Default constructor.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   none

private: 
    

	std::vector<ACS_USA_EventInfo> 
	removeDuplicates( const std::vector<ACS_USA_EventInfo>& aEvents ) const;
    	// Description:
    	//   Clean the provided list from duplicates and existing events.
    	// Parameters: 
    	//   events     list of events.
    	// Return value: 
    	//   cleanned list.

    	int autoCeaseGenerator();
    	// Description:
    	//   Ceases the events from the priority queue. 
    	//   Runs in an independant thread.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   none

    	ACS_USA_Boolean resetTimer();
    	// Description:
    	//   Adjusts the timer according to next event to cease.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   ACS_USA_True       success
    	//   ACS_USA_False      failure

    	ACS_USA_ReturnType saveList() const;
    	// Description:
    	//   Saves the content of the queue to the disk.
    	// Parameters:
    	//   none
    	// Return value: 
    	//   ACS_USA_error  An error occurred
    	//   ACS_USA_ok     success

    	String getLastError() const;
    	// Description:
    	//   Retrieves error string for system error codes.
    	// Parameters: 
    	//   none
    	// Return value: 
    	//   String       error string

    	void reportProblem( const acs_usa_error& what ) const;
    	// Description:
    	//      Reports the problem via the AEH.
    	// Parameters: 
    	//      acs_usa_error       error to report.
    	// Return value: 
    	//      None

    	ACS_USA_Boolean integrityCheck() const;
    	// Description:
    	//      Check the integrity of the temp file. 
    	// Parameters:
    	//      none
    	// Return value: 
    	//      ACS_USA_True            file is Ok.
    	//      ACS_USA_False           an error occurred.

    	// 
    	// Parameters related to object instance
    	int setTimer();
    	int handle_timeout(const ACE_Time_Value&, const void*);

    	ACE_thread_t    m_hThread;                  // Handle to the working thread.
    	bool            m_bContinue;                // Flag to stop the thread.
    	const   DWORD   m_dwMillisecondsGrace;      // Time to wait for the thread to finish.
    	const   DWORD   m_dwMillisecondsTimeOut;    // Time-out value for the timer.
    	const   DWORD   m_dwSecondsDefaultWaitTime; // Default value for the timer.
    	const   DWORD   m_dwAddEventTryCount;       // Number of tries to access protected queue.

    	ACE_thread_mutex_t m_critSection;          // Synchronizes access to priority queue.
    	long              m_timerId;           // Handle to the timer.
    
    	ACS_USA_EventQueue  m_queueEventToCease;    // Priority queue to keep events ordered.

    	ACS_USA_Time        m_clock;                // Used to query time.

    	String              m_sFileName;            // File name of temp storage.

    	// The thread should have its own acs_usa_error object, to not interfere 
    	// with 'error', the USA global instance.
    	acs_usa_error      m_threadError;
    	ACS_USA_ReactorRunner* m_reactorRunner;

    	enum {
		ACM_CLOSE=1,
		ACM_TIMEOUT=2
   	};
};



//******************************************************************************
// getError()
//******************************************************************************
inline
acs_usa_error
ACS_USA_AutoCeaseManager::getError() const
{
	return m_threadError;
}



#endif
