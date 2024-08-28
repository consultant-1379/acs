//******************************************************************************
// 
// NAME
//      ACS_USA_Event.cpp
//
// COPYRIGHT
//      Ericsson AB 1995, 1999, 2002-2004, 2006 - All rights reserved
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
// 		  This class holds data for AP Events and 
//		  sends AP Events via AP Event Handler API. 
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//		  19089-CAA 109 0545
//
// AUTHOR 
// 		  1995-06-15 by ETX/TX/T XKKHEIN
//
// REV	DATE 	  NAME		 DESCRIPTION
// A	 	950829  XKKHEIN	 First version.
// B		990915  UABDMT	 Ported to Windows NT4.0.
// C	 	020723	UABTSO	 Adapted to new AEH API.
// D    030212  UABPEK   New AP VIRUS alarms.
// E	  040429	QVINKAL	 Removal of RougeWave Tools.h++.
// F	  060619	UABPEK	 Problem Data truncated due to AEH interface limitation.
//
//******************************************************************************
#include <ACS_AEH_EvReport.H>
#include "ACS_USA_Regexp.h"
#include "ACS_USA_Event.h"
#include "ACS_USA_Error.h"
#include "ACS_USA_EventManager.h"
#include <windows.h>

//========================================================================================
//	Constructors
//========================================================================================
ACS_USA_Event::ACS_USA_Event() : 
  specificProblem(0),
  probableCause(),
  objectClassOfReference(),
  objectOfReference(),
  alarmNumber(0)
{
}  

ACS_USA_Event::ACS_USA_Event(const String& probableCause,
			  const ACS_USA_SpecificProblem specificProblem,
			  const String& objectOfReference):
  specificProblem(specificProblem),
  probableCause(probableCause),
  objectClassOfReference(),
  objectOfReference(objectOfReference),
  alarmNumber(0)
{
}  

ACS_USA_Event::ACS_USA_Event(const ACS_USA_Event& other) : 
  specificProblem(other.specificProblem),
  probableCause(other.probableCause),
  objectClassOfReference(),
  objectOfReference(other.objectOfReference),
  alarmNumber(0)
{
}  

//========================================================================================
//	Destructor
//========================================================================================

ACS_USA_Event::~ACS_USA_Event()
{
}  

//========================================================================================
//	Equality operator
//========================================================================================

ACS_USA_Boolean
ACS_USA_Event::operator==(const ACS_USA_Event& other) const
{
	if (specificProblem == other.specificProblem &&
		  probableCause == other.probableCause &&
		  objectOfReference == other.objectOfReference) 
	{
		return ACS_USA_True;
  } 
	else 
	{
		return ACS_USA_False;
  }
}

//========================================================================================
//	Assignment operator
//========================================================================================
const ACS_USA_Event&
ACS_USA_Event::operator=(const ACS_USA_Event& other)
{
  specificProblem = other.specificProblem;
  probableCause = other.probableCause;
  objectOfReference = other.objectOfReference;
  alarmNumber = other.alarmNumber;
  return *this;
}  

//========================================================================================
//	Report event
//========================================================================================
ACS_USA_StatusType 
ACS_USA_Event::report(const String& processName,
						      const String& perceivedSeverity,
						      const String& problemData, 
						      const String& problemText,
							  bool manualCease)
{
	ACS_AEH_EvReport reporter;
	
  if (probableCause == ACS_USA_DiagnosticFault) 
	{
		// If the alarm is raised because of failing to find
		// the boot time (8714), no waiting is necessary,
		// since USA is not to be stopped by PRCEVA.
		// If however, the alarm is a DIAGNOSTICS FAULT but 
		// not an 8714 alarm, USA shall sleep one minute 
		// before sending the 8701 alarm which leads to 
		// stop of USA (via PRCEVA). 
		// The reason for waiting is that the Cluster Server
		// cannot handle the case when a block requests 
		// stop directly at start up. Some time has to pass first.
		//
		if (specificProblem != 8714) 
		{
			Sleep(60000); 		
		}
	} 

  // AEH interface limitation.
  string problemData1 = problemData;
  if (problemData1.length() > 1024 - 1)
    problemData1 = problemData.substr(0, 1024 - 1);
	
	// Make sure that \n in problem text is regarded as "new line"
	//RWCRegexp re("\\\\n");	// Define regular expression  
	//problemText(re) = "\n";	// Replace matched expression
	//VRK
	string problemText1 = problemText;
	size_t pos = problemText1.find("\\n");
	if (pos != string::npos)
		problemText1.replace(pos,2,"\n");

	ACS_AEH_ReturnType ret = reporter.sendEventMessage(
											processName.data(),
											specificProblem,
											perceivedSeverity.data(),
											probableCause.data(),
											ACS_USA_objectClassOfReference,
											objectOfReference.data(),
											problemData1.data(),
											problemText1.data(),
											manualCease);
  if (ret == ACS_AEH_error) 
	{
		error.setError(ACS_USA_APeventHandlingError);
		error.setErrorText(reporter.getErrorText());
		return ACS_USA_error;	
  }
  // Increment the number of generated alarms
  if (perceivedSeverity != Key_perceivedSeverity_EVENT &&
      perceivedSeverity != Key_perceivedSeverity_CEASING) 
	{
    alarmNumber++;
  }
	return ACS_USA_ok;			  
}  

//========================================================================================
//	Cease alarm
//========================================================================================
ACS_USA_StatusType
ACS_USA_Event::cease(const String& processName)
{
  if (alarmNumber > 0) 
  {
		String empty;
		if (report(
				  processName,
				  String(Key_perceivedSeverity_CEASING),
				  empty,
				  empty) == ACS_USA_error) 
		{
			return ACS_USA_error;
		} 
		else 
		{
			--alarmNumber;
		}
  }       
  return ACS_USA_ok;
}  
