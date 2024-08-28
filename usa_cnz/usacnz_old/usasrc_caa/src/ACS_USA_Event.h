//******************************************************************************
// 
// .NAME
//  	 ACS_USA_Event - Holds all data needed for AP event reports.
// .LIBRARY 3C++
// .PAGENAME ACS_USA_Event
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson AB
// .INCLUDE ACS_USA_Event.h

// .COPYRIGHT
//	COPYRIGHT Ericsson AB, Sweden 1995-2003.
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein 
//	is the property of Ericsson AB, Sweden.
//	The program(s) may be used and/or copied only with the 
//	written permission from Ericsson AB or in accordance 
//	with the terms and conditions stipulated in the 
//	agreement/contract under which the program(s) have been 
//	supplied.

// .DESCRIPTION 
// 		  This class holds data for AP Events and 
//		  sends AP Events via AP Event Handler API. 

// .ERROR HANDLING
//
//		  General rule:
//		  The error handling is specified for each method.
//
//		  No methods initiate or send error reports unless specified.

// .DOCUMENT NO
//		  19089-CAA 109 0259

// AUTHOR 
//		  1995-08-18 by ETX/TX/T XKKHEIN

// REV  DATE 	  NAME		 DESCRIPTION
// A	  950829	XKKHEIN	 First version.
// B		990915	UABDMT	 Ported to Windows NT4.0.
// C	 	020723	UABTSO	 Adapted to new AEH API.
// D    030212  UABPEK   New AP VIRUS alarms.
// E	040429	QVINKAL	  Removal of RougeWave Tools.h++

// .LINKAGE
//		  ACS_AEH_EvReport

// .SEE ALSO 
//	 	  ACS_AEH_EveReport

//******************************************************************************

#ifndef ACS_USA_EVENT_H 
#define ACS_USA_EVENT_H
#include <string>
#include "ACS_USA_Global.h"

using namespace std;
typedef string String;

// Typedefs

// Forward declaration(s)

// Member functions, constructors, destructors, operators

class ACS_USA_Event
{
	
public:
	
	ACS_USA_Event();
	// Description:
	// 		Default constructor 
	// Parameters: 
	//		None
	// Return value: 
	//		None
	// Additional information:
	//		-

	ACS_USA_Event(const String& probableCause,
				  const ACS_USA_SpecificProblem specificProblem,
				  const String& objectOfReference);
	// Description:
	// 		Constructor that creates object with data
	// Parameters: 
	//		probableCause	      Alarm printout slogan.
	//		specificProblem		  specific problem
	//		objectOfReference		object of reference
	// Return value: 
	//		None
	// Additional information:
	//		-
	
	ACS_USA_Event(const ACS_USA_Event& other);
	// Description:
	// 		Copy constructor 
	// Parameters: 
	//		other			Reference to an ACS_USA_Event object
	// Return value: 
	//		none
	// Additional information:
	//		This is required by RWTQueue template

	virtual ~ACS_USA_Event();
	// Description:
	// 		Destructor 
	// Additional information:
	//		-
	
	ACS_USA_Boolean operator==(const ACS_USA_Event& other) const;
	// Description:
	// 		Equality operator
	// Parameters: 
	//		other			Reference to an ACS_USA_Event object
	// Return value: 
	//		ACS_USA_True	Event objects are equal
	//		ACS_USA_False	Event objects are not equal
	// Additional information:
	//		This is required by RWTQueue template
	
	const ACS_USA_Event& operator=(const ACS_USA_Event& other);
	// Description:
	// 		Assignament operator 
	// Parameters: 
	//		other			Reference to an ACS_USA_Event object
	// Return value: 
	//		ACS_USA_Event&	Reference to assigned object
	// Additional information:
	//		This is required by RWTQueue template
	
	ACS_USA_StatusType report(const String& processName,
						      const String& perceivedSeverity,
						      const String& problemData, 
						      const String& problemText,
							  bool manualCease = true);

	// Description:
	// 		Reports event AP Event Handling
	// Parameters: 
	//		None
	// Return value: 
	//		ACS_USA_ok		  Reporting ok
	//		ACS_USA_error	  Reporting failed
	// Additional information:
	//		-
	
	ACS_USA_StatusType cease(const String& processName);

	// Description:
	// 		Sends ceasing to AP Event Handling block
	// Parameters: 
	//		None
	// Return value: 
	//		ACS_USA_ok		Reporting ok
	//		ACS_USA_error	Reporting failed
	// Additional information:
	//		-

	bool noPendingAlarms();

	// Description:
	// 		Determines if an object has pending alarms.
	// Parameters: 
	//		None
	// Return value: 
	//		TRUE			Object "has" no pending alarms
	//		FALSE			Object "has" alarms pending
	// Additional information:
	//		-
	
private:
	ACS_USA_SpecificProblem	specificProblem;
	String probableCause;
	String objectClassOfReference;
	String objectOfReference;
	int	alarmNumber;
};


//========================================================================================
//	Check if there are no pendingAlarms
//========================================================================================

inline
bool
ACS_USA_Event::noPendingAlarms()
{
	return (alarmNumber > 0 )? false: true;
}

#endif
