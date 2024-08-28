//*****************************************************************************
//
// NAME
//      ACS_USA_EventManager.cpp
//
// COPYRIGHT Ericsson AB, Sweden 1995-2003.
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
//      This class handles all alarm reports in common way. All 
//	    events are reported and if an event is alarm then it is 
//	    stored in internal queue to be able to send alarm ceasings.

// DOCUMENT NO
//	    190 89-CAA 109 0259

//******************************************************************************

#include <stdio.h>
#include <sys/time.h> 
#include "acs_usa_regexp.h"
#include "acs_usa_event.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_common.h"
#include "acs_prc_api.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_logtrace.h"

const size_t pidLength = 10;


//******************************************************************************
//	Trace point definitions
//******************************************************************************
ACS_USA_Trace traEM ("ACS_USA_EventManager         ");

const char* ACS_USA_traceNodePassive  = "Running on passive side, shall not raise alarm on this event";
const char* ACS_USA_traceNodeActive  = "Running on active side, shall not raise alarm on this event";
const char* ACS_USA_traceSkippingEventFrequencyAlarm	= "Too soon for event frequency alarm";

//******************************************************************************
//	noPendingAlarms()
//******************************************************************************
bool
noPendingAlarms(ACS_USA_Event* event, void* data)
{
	(void)data;
	return event->noPendingAlarms();
}

//******************************************************************************
//	ACS_USA_EventManager()
//******************************************************************************
ACS_USA_EventManager::ACS_USA_EventManager() :
  defaultSpecificProblem(ACS_USA_defaultSpecificProblem),
  defaultObjectOfReference(ACS_USA_processName),
  processName(ACS_USA_processName),
  timeSinceLatest8714alarm(0)
{
	USA_TRACE_ENTER2("Constructor");
  	char buff[pidLength];
  	sprintf(buff, "%d", getpid()); 
  	defaultObjectOfReference += slashString;
  	defaultObjectOfReference += buff;
  
  	usacc_toUpper(defaultObjectOfReference);
  	processName += semiColonString;
  	processName += buff;
	USA_TRACE_LEAVE2("Constructor");
}  

//******************************************************************************
//	ACS_USA_EventManager()
//******************************************************************************
ACS_USA_EventManager::~ACS_USA_EventManager()
{
	USA_TRACE_ENTER2("Destructor");
	
	queue.clear();  // just in case
	DestroyPtrQueue();

	USA_TRACE_LEAVE2("Destructor");
}  

//******************************************************************************
//	DestroyPtrQueue()
//******************************************************************************
void ACS_USA_EventManager::DestroyPtrQueue()
{
	USA_TRACE_ENTER();

	try
	{
		std::list<ACS_USA_Event *>::iterator it = ptrQueue.begin();
		for(;it != ptrQueue.end();it++)
			delete(*it);
		ptrQueue.clear();
	}
	catch(...)
	{
	}
	USA_TRACE_LEAVE();

}

//******************************************************************************
//	report()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_EventManager::report(
	const ACS_USA_SpecificProblem specificProblem,
	const String& perceivedSeverity,
	const String& probableCause,
	const String& objectOfReference,
	const String& problemData,
	const String& problemText,
	const String& node,
	unsigned int alarmFilterInterval,
	bool manualCease,
	const String& userName)
{
	USA_TRACE_ENTER();
  	return logEvent(processName,  // USA process name
				  specificProblem,
				  perceivedSeverity,
				  probableCause,
				  objectOfReference,
				  problemData,
				  problemText,
				  node,
				  alarmFilterInterval,
				  manualCease,
				  userName);
	USA_TRACE_LEAVE();
} 

//******************************************************************************
//	report()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_EventManager::report(
	const String& procName,
	const ACS_USA_SpecificProblem specificProblem,
	const String& perceivedSeverity,
	const String& probableCause,
	const String& objectOfReference,
	const String& problemData,
	const String& problemText,
	const String& node,
	unsigned int alarmFilterInterval,
	bool manualCease,
	const String& userName)
{
	USA_TRACE_ENTER();

  	return logEvent(procName, // provided process name
				  specificProblem,
				  perceivedSeverity,
				  probableCause,
				  objectOfReference,
				  problemData,
				  problemText,
				  node,
				  alarmFilterInterval,
				  manualCease, 
				  userName);
	USA_TRACE_LEAVE();
} 

//******************************************************************************
//	logEvent()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_EventManager::logEvent(
							const String& procName,
							const ACS_USA_SpecificProblem specificProblem,
							const String& perceivedSeverity,
							const String& probableCause,
							const String& objectOfReference,
							const String& problemData,
							const String& problemText,
							const String& node,
							unsigned int alarmFilterInterval,
							bool manualCease,
							const String& userName)
{
	USA_TRACE_ENTER();

	// Check if more than x minutes has passed since last 8714 alarm on 
	// the event frequency. If not, skip the alarm.
	// The number of minutes for filtering the alarm is set by the 
	// alarmFilterInterval parameter.
	if (specificProblem == 8714)
	{
		// Get current time in milliseconds
		unsigned long millisecondsSinceLastBoot = 0;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		millisecondsSinceLastBoot = (int)(tv.tv_sec*1000 + (tv.tv_usec / 1000));
		
		if (timeSinceLatest8714alarm != 0)
		{	
			// alarmFilterInterval is in seconds, 
			// but here we are counting in milliseconds.
			if (alarmFilterInterval != 0)
			{
				if (millisecondsSinceLastBoot <=
					(timeSinceLatest8714alarm + (alarmFilterInterval * 1000)))
				{
					// cout << endl << "Skipping 8714 alarm " << endl;
					return ACS_USA_Ok; // Skip raising the alarm, 
										         // not enough time has passed.
				}
			}
		}
		// Using a DWORD, it is 49.7 days before wrapping around to zero, 
		// giving timeSinceLatest8714alarm a smaller value than expected.
		// However, the probability of an NT machine running
		// 49.7 days without boot is *not* high. If so, more than one 
		// 'too high event frequency'-alarm per minute nay be issued.
		timeSinceLatest8714alarm = millisecondsSinceLastBoot; 
	}
	// Check if current side (active/passive) is relevant regarding  
	// if the alarm/event is to be sent or not.
	// The text ("active" or "passive") in variable node is set  
	// in the ACS_USA_config.acf-file.
	if (node != "")	
	{
		int	 nodeState;
		ACS_PRC_API prc;
		nodeState=prc.askForNodeState();

		if ((nodeState == 1) &&
			(node == "active"))
		{
			// We are running on the passive side - don't send alarm
			// intended to be sent from the active side only.
			return ACS_USA_Ok; 			
		}
		else if ((nodeState == 2) && (node == "passive"))
		{
			// We are running on the active side - don't send alarm
			// intended to be sent from the passive side only.
			return ACS_USA_Ok; 
		}
		else
		{
			// USA is running on the side to send the alarm from.  
			// Consequently, it's ok to send the alarm, so do 
			// nothing here.
		}
	}	
	ACS_USA_SpecificProblem sp = specificProblem;
	String oor(objectOfReference);
	String user(userName);

	ACS_USA_ReturnType rc = ACS_USA_Ok;

	if (specificProblem == 0) 
	{		
		sp = defaultSpecificProblem;
	}
	// If object of Reference is not supplied use <PROCESSNAME>/<pid>
	if (oor.empty())	
	{
		oor = defaultObjectOfReference;
	}
	ACS_USA_Event* event = new ACS_USA_Event(probableCause, sp, oor, user);
	// If event is an alarm store it in the queue 
	if (perceivedSeverity != ACS_USA_PERCEIVED_SEVERITY_EVENT &&
		perceivedSeverity != ACS_USA_PERCEIVED_SEVERITY_CEASING &&
		perceivedSeverity != ACS_USA_PERCEIVED_SEVERITY_NONE) 
	{
		ACS_USA_Event* queuedEvent =	 NULL;
		
		std::list<ACS_USA_Event>::iterator iter;
		for(iter = queue.begin( );iter != queue.end();iter++)
			if ( *event == *iter) 
				queuedEvent = &(*iter);
		
		if (queuedEvent == NULL) 
		{
			rc = event->report(procName, perceivedSeverity, problemData, problemText, manualCease);
			if (rc == ACS_USA_Ok) 
			{
				queue.push_back(*event);
				ptrQueue.push_back(event);
			}
			else {
				delete event;
			}
		} 
		else 
		{
			// Event is already in the queue
			rc = queuedEvent->report(procName, perceivedSeverity, problemData, problemText, manualCease);
			delete event;
		}
  	} 
	else 
	{
		if (perceivedSeverity != ACS_USA_PERCEIVED_SEVERITY_NONE) 
		{
		  rc = event->report(procName, perceivedSeverity, problemData, problemText, manualCease);
    		}
		// Event is not an alarm.
		delete event;
  	}
	USA_TRACE_LEAVE();
	return rc;
}  

//******************************************************************************
//	ceaseAll()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_EventManager::ceaseAll()
{
	USA_TRACE_ENTER();

	size_t queueSize = queue.size();
	ACS_USA_ReturnType rc = ACS_USA_Ok;
    
    //
    // Send ceasing for all events in the queue
    //
    
	while (queueSize > 0 && rc == ACS_USA_Ok) {
		std::list<ACS_USA_Event>::iterator itQue = queue.begin();
		ACS_USA_Event event ;
		event= *(queue.begin());

		while(itQue != queue.end() && rc == ACS_USA_Ok) {
			rc = event.cease(processName);
			if (rc == ACS_USA_Error) {
				break;
			} else {
				event = *(++itQue);
			}
		}

		// Remove all elements from the queue that have no 
		// pending alarms
		
		
		std::list<ACS_USA_Event>::iterator iter;
		for(iter = queue.begin( ); iter != queue.end(); )
		{
			if (iter->noPendingAlarms() == 0) 
				iter = queue.erase(iter);
			else
				++iter;
		}
		queueSize = queue.size();	
	}
   
	USA_TRACE_LEAVE(); 
	return rc;
}  

//******************************************************************************
