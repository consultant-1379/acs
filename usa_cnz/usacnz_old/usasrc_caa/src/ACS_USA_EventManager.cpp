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

// AUTHOR 
// 	    1995-08-10 by ETX/TX/T XKKHEIN

// REV	DATE		NAME 		 DESCRIPTION
// A	 	950829	XKKHEIN	 First version.
// B		990915	UABDMT	 Ported to Windows NT4.0.
// C    030212  UABPEK   New AP VIRUS alarms.
// D	040429	QVINKAL	  Removal of RougeWave Tools.h++
// SEE ALSO 
// 	
//
//******************************************************************************

#pragma comment (lib, "clusapi.lib")
#include <process.h>
#include <windows.h>
#include <clusapi.h>
#include 	<stdio.h>
#include <ACS_TRA_trace.H>
#include	"ACS_USA_Regexp.h"
#include 	"ACS_USA_Criterion.h"
#include 	"ACS_USA_Event.h"
#include	"ACS_USA_EventManager.h"
#include "ACS_USA_Common.h"

const size_t pidLength = 10;


//******************************************************************************
//	Trace point definitions
//******************************************************************************
ACS_TRA_trace traceEvMgr = ACS_TRA_DEF("ACS_USA_EventManager", "C62");
const char* const ACS_USA_traceNodePassive  = "Running on passive side, shall not raise alarm on this event";
const char* const ACS_USA_traceNodeActive  = "Running on active side, shall not raise alarm on this event";
const char* const ACS_USA_traceSkippingEventFrequencyAlarm	= "Too soon for event frequency alarm";

//******************************************************************************
//	noPendingAlarms()
//******************************************************************************
bool
noPendingAlarms(ACS_USA_Event* event, void* data)
{
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
  char buff[pidLength];
  sprintf(buff, "%d", _getpid()); 
  defaultObjectOfReference += slashString;
  defaultObjectOfReference += buff;
  
  toUpper(defaultObjectOfReference);
  processName += semiColonString;
  processName += buff;
}  

//******************************************************************************
//	ACS_USA_EventManager()
//******************************************************************************
ACS_USA_EventManager::~ACS_USA_EventManager()
{
	queue.clear();  // just in case
	DestroyPtrQueue();
}  

//******************************************************************************
//	DestroyPtrQueue()
//******************************************************************************
void ACS_USA_EventManager::DestroyPtrQueue()
{
	try
	{
		list<ACS_USA_Event *>::iterator it = ptrQueue.begin();
		for(;it != ptrQueue.end();it++)
			delete(*it);
		ptrQueue.clear();
	}
	catch(...)
	{
	}
}

//******************************************************************************
//	report()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_EventManager::report(
	const ACS_USA_SpecificProblem specificProblem,
	const String& perceivedSeverity,
	const String& probableCause,
	const String& objectOfReference,
	const String& problemData,
	const String& problemText,
	const String& node,
	unsigned int alarmFilterInterval,
	bool manualCease)
{
  return logEvent(processName,  // USA process name
				  specificProblem,
				  perceivedSeverity,
				  probableCause,
				  objectOfReference,
				  problemData,
				  problemText,
				  node,
				  alarmFilterInterval,
				  manualCease);
} 

//******************************************************************************
//	report()
//******************************************************************************
ACS_USA_StatusType
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
	bool manualCease)
{
  return logEvent(procName, // provided process name
				  specificProblem,
				  perceivedSeverity,
				  probableCause,
				  objectOfReference,
				  problemData,
				  problemText,
				  node,
				  alarmFilterInterval,
				  manualCease);
} 

//******************************************************************************
//	logEvent()
//******************************************************************************
ACS_USA_StatusType
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
							bool manualCease)
{
	// Check if more than x minutes has passed since last 8714 alarm on 
	// the event frequency. If not, skip the alarm.
	// The number of minutes for filtering the alarm is set by the 
	// alarmFilterInterval parameter.
	if (specificProblem == 8714)
	{
		// Get current time in milliseconds
		DWORD millisecondsSinceLastBoot = 0;
		millisecondsSinceLastBoot = GetTickCount();  
		
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
					if (ACS_TRA_ON(traceEvMgr)) 
					{
						ACS_TRA_event(&traceEvMgr, ACS_USA_traceSkippingEventFrequencyAlarm);
					}
					return ACS_USA_ok; // Skip raising the alarm, 
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
		ACS_USA_NodeState	 nodeState = 0;
		// Find out which side we are running on
		nodeState = clusterStatus();	
		if ((nodeState == ACS_USA_passiveNodeState) &&
			(node == "active"))
		{
			// We are running on the passive side - don't send alarm
			// intended to be sent from the active side only.
			if (ACS_TRA_ON(traceEvMgr)) 
			{
				ACS_TRA_event(&traceEvMgr, ACS_USA_traceNodePassive);
			}
			return ACS_USA_ok; 			
		}
		else if ((nodeState == ACS_USA_activeNodeState) && (node == "passive"))
		{
			// We are running on the active side - don't send alarm
			// intended to be sent from the passive side only.
			if (ACS_TRA_ON(traceEvMgr)) 
			{
				ACS_TRA_event(&traceEvMgr, ACS_USA_traceNodeActive);
			}
			return ACS_USA_ok; 
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

  ACS_USA_StatusType rc = ACS_USA_ok;
  USA_DEBUG(logMsg("EvMGR:logEvent\n"));
  // If specific problem is not given use the default one
  if (specificProblem == 0) 
	{		
		sp = defaultSpecificProblem;
  }
  // If object of Reference is not supplied use <PROCESSNAME>/<pid>
  if (oor.empty())	
  {
    oor = defaultObjectOfReference;
  }
  ACS_USA_Event* event = new ACS_USA_Event(probableCause, sp, oor);
  // If event is an alarm store it in the queue 
  if (perceivedSeverity != Key_perceivedSeverity_EVENT &&
	  perceivedSeverity != Key_perceivedSeverity_CEASING &&
	  perceivedSeverity != Key_perceivedSeverity_NONE) 
	{
		ACS_USA_Event* queuedEvent =	 NULL;
		
		list<ACS_USA_Event>::iterator iter;
		for(iter = queue.begin( );iter != queue.end();iter++)
			if ( *event == *iter) 
				queuedEvent = &(*iter);
		
		if (queuedEvent == NULL) 
		{
			rc = event->report(procName, perceivedSeverity, problemData, problemText, manualCease);
			if (rc == ACS_USA_ok) 
			{
				queue.push_back(*event);
				ptrQueue.push_back(event);
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
		if (perceivedSeverity != Key_perceivedSeverity_NONE) 
		{
		  rc = event->report(procName, perceivedSeverity, problemData, problemText, manualCease);
    }
		// Event is not an alarm.
		delete event;
  }
  return rc;
}  

//******************************************************************************
//	ceaseAll()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_EventManager::ceaseAll()
{
	size_t queueSize = queue.size();
    ACS_USA_StatusType rc = ACS_USA_ok;
    
    USA_DEBUG(logMsg("EvMGR: ceaseAll\n"));
    
    //
    // Send ceasing for all events in the queue
    //
    
    while (queueSize > 0 && rc == ACS_USA_ok) {
		list<ACS_USA_Event>::iterator itQue = queue.begin();
		ACS_USA_Event event ;
		event= *(queue.begin());

		while(itQue != queue.end() && rc == ACS_USA_ok) {
			rc = event.cease(processName);
			if (rc == ACS_USA_error) {
				break;
			} else {
				event = *(++itQue);
			}
		}

		// Remove all elements from the queue that have no 
		// pending alarms
		
		
		list<ACS_USA_Event>::iterator iter;
		for(iter = queue.begin( );iter != queue.end();iter++)
			if (iter->noPendingAlarms() == 0) queue.erase(iter);
		queueSize = queue.size();	
   }
    
    return rc;
}  



//******************************************************************************
//	clusterStatus
//******************************************************************************
ACS_USA_NodeState 
ACS_USA_EventManager::clusterStatus()		// Checks if USA is currently running 
											// on the passive or active node.
{
	ACS_USA_NodeState returnValue = 1;
	HGROUP groupHandle;
	HCLUSTER clusterHandle = OpenCluster(NULL);
	if (clusterHandle == NULL)				// Running on a machine without 
											// MS Cluster Administrator
	{
											// If there is no cluster support, we 
											// are running on a single node machine.
											// Might as well consinder USA to be on 
											// the active side since our "unclustered" 
											// node cannot enter a passive cluster state.
		returnValue = ACS_USA_activeNodeState;			
	}
	else									// Running on a machine with 
											// MS Cluster Administrator
	{
		groupHandle = OpenClusterGroup(clusterHandle, L"Cluster Group");
		if (groupHandle == NULL)			// Failure fetching cluster group. Running
											// USA in single node state instead.
		{
											// If there are cluster support problems, 
											// we might as well consinder USA to be on 
											// the active side, since our "unclustered" 
											// node cannot enter a passive cluster state.
			returnValue = ACS_USA_activeNodeState;			
		}
		else
		{	// OpenClusterGroup() ok
			//
			DWORD	namelen = 64;
			LPWSTR	currentNode = new WCHAR[namelen];
			GetComputerNameW(currentNode, &namelen);
				
			namelen = 64;
			LPWSTR	activeNode = new WCHAR[namelen];
			DWORD	status = GetClusterGroupState(groupHandle, activeNode, &namelen);

			if (status == ClusterGroupStateUnknown)	// Failure fetching cluster group state. 
													// Running USA in single node state instead.
			{					
													// If there are cluster support problems, 
													// we might as well consinder USA to be on 
													// the active side, since our "unclustered" 
													// node cannot enter a passive cluster state.
				returnValue = ACS_USA_activeNodeState;			
			}
			else 
			{	// GetClusterGroupState() ok
				//
				if (wcscmp(activeNode, currentNode) == 0)
				{
					returnValue = ACS_USA_activeNodeState;
				}
				else
				{
					returnValue = ACS_USA_passiveNodeState;
				}
			}
			delete []currentNode;
			delete []activeNode;

			BOOL clusterGroupClosed = CloseClusterGroup(groupHandle);
			BOOL clusterClosed		= CloseCluster(clusterHandle);
			if ((clusterClosed == false)			// Failure closing cluster handles
				|| (clusterGroupClosed == false))
			{					
													// Do nothing
			}
		}											
	}												// end of cluster handling
	return returnValue;
}

//******************************************************************************
