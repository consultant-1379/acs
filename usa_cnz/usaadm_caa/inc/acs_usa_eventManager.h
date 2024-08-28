//******************************************************************************
// 
// .NAME 
//      ACS_USA_EventManager - Handles all event reports initiated by USA
//                             in common way.
// .LIBRARY 3C++
// .PAGENAME ACS_USA_EventManager
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson AB
// .INCLUDE ACS_USA_EventManager.h

// .COPYRIGHT
//  COPYRIGHT Ericsson AB, Sweden 1995-2003.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//      This class handles all alarm reports in common way. All 
//	    events are reported and if an event is alarm then it is 
//	    stored in internal queue to be able to send alarm ceasings.

// ERROR HANDLING
//
//      General rule:
//	    The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NUMBER
//      190 89-CAA 109 0259

// AUTHOR 
//      1995-08-17 by ETX/TX/T XKKHEIN

// REV  DATE    PREPARED  DESCRIPTION
// A		950829	XKKHEIN	  First version.
// B    990915  UABDMT    Ported to Windows NT 4.0.
// C    030212  UABPEK    New AP VIRUS alarms.
// D	040429	QVINKAL	  Removal of RougeWave Tools.h++

// .LINKAGE
//	

// .SEE ALSO 
//

//******************************************************************************

#ifndef ACS_USA_EVENTMANAGER_H 
#define ACS_USA_EVENTMANAGER_H

#include <sys/types.h>
#include <string>
#include <list>
#include "acs_usa_types.h"
#include "acs_usa_event.h"

using namespace std;
//typedef string String;
// Constants
const char* const slashString = "/";
const char* const semiColonString = ":";

const size_t maxQueueSize = 20;			// maximum queue size

//Forward declaration(s)

//Member functions, constructors, destructors, operators

class  ACS_USA_EventManager
{

 public:

  ACS_USA_EventManager();
  // Description:
  // 	Default constructor 
  // Parameters: 
  //	none
  // Return value: 
  //	none
  // Additional information:
  //

  virtual ~ACS_USA_EventManager();
  // Description:
  // 	Default destructor 
  // Parameters: 
  //	none
  // Return value: 
  //	none
  // Additional information:
  //
  ACS_USA_ReturnType report(
    const ACS_USA_SpecificProblem specificProblem,
		const String& perceivedSeverity,
		const String& probableCause,
		const String& objectOfReference,
		const String& problemData,
		const String& problemText,
		const String& node,
		unsigned int alarmFilterInterval,
		bool manualCease = true,
		const String& userName = String());
  // Description:
  // 	  Proxy function to Sends an AP Event report
  // Parameters: 
  //	  none
  // Return value: 
  //	  ACS_USA_ok		  ok
  //	  ACS_USA_error	  failed
  // Additional information:
  //
  
  ACS_USA_ReturnType report(
	    const String& procName,
		const ACS_USA_SpecificProblem specificProblem,
		const String& perceivedSeverity,
		const String& probableCause,
		const String& objectOfReference,
		const String& problemData,
		const String& problemText,
		const String& node,
		unsigned int alarmFilterInterval,
		bool manualCease = true,
		const String& userName = String());
  // Description:
  // 	  Proxy function to Sends an AP Event report, using a procName
  // Parameters: 
  //	  none
  // Return value: 
  //	  ACS_USA_ok		  ok
  //	  ACS_USA_error	  failed
  // Additional information:
  //

  ACS_USA_ReturnType ceaseAll();
  // Description:
  // 	Sends ceasings for all alarms in the queue 
  // Parameters: 
  //	none
  // Return value: 
  //	ACS_USA_ok		ok
  //	ACS_USA_error		failed
  // Additional information:
  //

  void setQueueSize(size_t size);
  // Description:
  // 	Sets the que size to be used 
  // Parameters: 
  //	size			the new size
  // Return value: 
  //	none
  // Additional information:
  //

  void setDefaultSpecificProblem(ACS_USA_SpecificProblem specificProblem);
  // Description:
  // 	 Sets new default specific problem.
  // Parameters: 
  //	 specificProblem   specific problem to be used as default
  // Return value: 
  //	 none
  // Additional information:
  //	 Default problem can be changed through AP Parameter handling

  void DestroyPtrQueue();	
private:
  ACS_USA_ReturnType logEvent(
		const String& procName,
		const ACS_USA_SpecificProblem specificProblem,
		const String& perceivedSeverity,
		const String& probableCause,
		const String& objectOfReference,
		const String& problemData,
		const String& problemText,
		const String& node,
		unsigned int alarmFilterInterval,
		bool manualCease = true,
		const String& userName = String());
  // Description:
  // 	  Sends an AP Event report and stores it in the internal
  //	  queue if the severity is neither EVENT nor CEASING
  // Parameters: 
  //	  none
  // Return value: 
  //	  ACS_USA_ok	  ok
  //	  ACS_USA_error	  failed
  // Additional information:
  //

  std::list<ACS_USA_Event> queue;	              // Queue where alarm info is kept.
  std::list<ACS_USA_Event *> ptrQueue;
  ACS_USA_SpecificProblem defaultSpecificProblem; // Default problem.
  String	defaultObjectOfReference; 	          // Obj. of ref. as 'PROCCESS/<pid>'.
  String	processName;		                      // Process name as 'name:<pid>'.
  unsigned long timeSinceLatest8714alarm;
  
};

//Non-member function prototypes
bool noPendingAlarms(ACS_USA_Event* event, void* data);

// Description:
//      This function shall be passed to the queue to find events that have no
//		pending alarms
// Parameters:
//      ACS_USA_Event        	A pointer to ACS_USA_Event
//		data					A poiner to data to be passed (not used)
// Return value:
//      TRUE                 	No pending alarms 
//		FALSE					Alarms penting (not ceased yet)
// Additional information:
//    

//******************************************************************************
//	setDefaultSpecificProblem()
//******************************************************************************
inline  
void
ACS_USA_EventManager::setDefaultSpecificProblem(ACS_USA_SpecificProblem specificProblem)
{
    defaultSpecificProblem = specificProblem;
}

extern ACS_USA_EventManager eventManager;
 
#endif
