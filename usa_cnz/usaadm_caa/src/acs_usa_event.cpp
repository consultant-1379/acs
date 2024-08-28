//******************************************************************************
// 
// NAME
//      acs_usa_event.cpp
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
//
// AUTHOR 
// 		  1995-06-15 by ETX/TX/T XKKHEIN
//
// REV	DATE 	  NAME		 DESCRIPTION
//
//******************************************************************************
#include "acs_aeh_evreport.h"
#include "acs_usa_regexp.h"
#include "acs_usa_event.h"
#include "acs_usa_error.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_logtrace.h"
#include <unistd.h>

//========================================================================================
//	Constructors
//========================================================================================
ACS_USA_Event::ACS_USA_Event() : 
  specificProblem(0),
  probableCause(),
  objectClassOfReference(),
  objectOfReference(),
  userName(),
  alarmNumber(0)
{
	USA_TRACE_ENTER2("Constructor");

	USA_TRACE_LEAVE2("Constructor");
}  

ACS_USA_Event::ACS_USA_Event(const String& probableCause,
			  const ACS_USA_SpecificProblem specificProblem,
			  const String& objectOfReference,
			  const String& userName):
  specificProblem(specificProblem),
  probableCause(probableCause),
  objectClassOfReference(),
  objectOfReference(objectOfReference),
  userName(userName),
  alarmNumber(0)
{
	USA_TRACE_ENTER2("Constructor");

	USA_TRACE_LEAVE2("Constructor");
}  

ACS_USA_Event::ACS_USA_Event(const ACS_USA_Event& other) : 
  specificProblem(other.specificProblem),
  probableCause(other.probableCause),
  objectClassOfReference(),
  objectOfReference(other.objectOfReference),
  userName(other.userName),
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
	if (specificProblem == other.specificProblem 	 &&
	    probableCause == other.probableCause 	 &&
	    objectOfReference == other.objectOfReference &&
	    userName == other.userName) 
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
	userName = other.userName;
	alarmNumber = other.alarmNumber;

	return *this;
}  

//========================================================================================
//	Report event
//========================================================================================
ACS_USA_ReturnType 
ACS_USA_Event::report(	const String& processName,
			const String& perceivedSeverity,
			const String& problemData, 
			const String& problemText,
			bool manualCease)
{
	USA_TRACE_ENTER();

	acs_aeh_evreport reporter;
	
  	if (probableCause == ACS_USA_DIAGFAULT) 
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
			usacc_msec_sleep(60000); // 60 secs 		
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

	ACS_AEH_ReturnType ret = reporter.sendEventMessage( 	processName.data(),
								specificProblem,
								perceivedSeverity.data(),
								probableCause.data(),
								ACS_USA_OBJ_CLASS_REFERENZE_APZ,
								objectOfReference.data(),
								problemData1.data(),
								problemText1.data(),
								manualCease);
  	if (ret == ACS_AEH_error) 
	{
		error.setError(ACS_USA_APeventHandlingError);
		error.setErrorText(reporter.getErrorText());
		USA_TRACE_LEAVE();
		return ACS_USA_Error;	
 	 }
  	// Increment the number of generated alarms
  	if (perceivedSeverity != ACS_USA_PERCEIVED_SEVERITY_EVENT &&
      		perceivedSeverity != ACS_USA_PERCEIVED_SEVERITY_CEASING) 
	{
    		alarmNumber++;
  	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;			  
}  

//========================================================================================
//	Cease alarm
//========================================================================================
ACS_USA_ReturnType
ACS_USA_Event::cease(const String& processName)
{
	USA_TRACE_ENTER();

  	if (alarmNumber > 0) 
  	{
		String empty;
		if (report(
			  processName,
			  String(ACS_USA_PERCEIVED_SEVERITY_CEASING),
			  empty,
			  empty) == ACS_USA_Error) 
		{
			return ACS_USA_Error;
		} 
		else 
		{
			--alarmNumber;
		}
  	}       

  	USA_TRACE_LEAVE();
  	return ACS_USA_Ok;
}  
//-----------------------------------------------------------------------------------------
