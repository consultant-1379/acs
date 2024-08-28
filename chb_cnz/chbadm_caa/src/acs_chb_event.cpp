/*=================================================================== */
   /**
   @file acs_chb_event.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_event.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/01/2011   XNADNAR   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include<ace/ACE.h>
#include <stdio.h>
#include <string>
#include<ace/Time_Value.h>
#include <stdlib.h>
#include <iostream>
#include <acs_chb_event.h>
#include <acs_chb_heartbeat_def.h>
#include <acs_chb_aeh.h>

using namespace std;
/*===================================================================
   ROUTINE: ACS_CHB_Events
=================================================================== */

ACS_CHB_Event::ACS_CHB_Event (const char* processName)

{  
	ACE_OS::snprintf (process,(sizeof(process)/sizeof(*process))-1,"%s:%ld", processName, (long) ACE_OS::getpid ());
	ACE_OS::memset(dwHighDateTimeEvents, 0, sizeof(dwHighDateTimeEvents));
} // End of constructor
/*===================================================================
   ROUTINE: ~ACS_CHB_Event
=================================================================== */
ACS_CHB_Event::~ACS_CHB_Event ()
{
} // End of destructor

/*===================================================================
   ROUTINE: event
=================================================================== */

void ACS_CHB_Event::event (ACS_AEH_SpecificProblem specificProblem,
		                   ACS_AEH_PercSeverity percSeverity,
		                   ACS_AEH_ProbableCause probableCause,
		                   ACS_AEH_ProblemData problemData,
		                   ACS_AEH_ProblemText problemText,
		                   ACS_AEH_ObjectOfReference objOfRef )
{ 
	
	// Event report

	if (checkForFrequency(specificProblem) && tooFrequent(specificProblem))
	{
		 return; // only certain events should be blocked if they come to often.
	}
	
	// Report with object of reference.
	// Default value of object of reference is ""

	ACS_CHB_AEH::ReportEvent(specificProblem,
	                         percSeverity,
	                         probableCause,
	                         objOfRef,
	                         problemData,
	                         problemText,
	                          1);

} // End of event

/*===================================================================
   ROUTINE: checkForFrequency
=================================================================== */
bool ACS_CHB_Event::checkForFrequency(ACS_AEH_SpecificProblem specificProblem) 
{
	switch (specificProblem)
	{
	case 9035: //[EVENT] ACS_CHB_CSnotAvailable
		return true; // reported each 429 sec
	case 9006: //[ALARM] ACS_CHB_ClockNotSync
	default: 
		return false; // always reported
	}
}// End of checkForFrequency

/*===================================================================
   ROUTINE: tooFrequent
=================================================================== */
bool ACS_CHB_Event::tooFrequent(ACS_AEH_SpecificProblem specificProblem)
{
	long idx = specificProblem - 9000;
	if ((idx < 0) || (idx > 99)) 
	{
		return false;
	}

	 time_t sysTime;
	 time(&sysTime);

	if((dwHighDateTimeEvents[idx]==0)||(sysTime-dwHighDateTimeEvents[idx])>429)
	{
		dwHighDateTimeEvents[idx] = sysTime;
		return false;
	}
	else
	{
	 return true;
	}

}// End of tooFrequent
