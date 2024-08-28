/*
 * acs_alh_eventmgr.cpp
 *
 *  Created on: Oct 31, 2011
 *      Author: efabron
 */


#include "acs_alh_eventmgr.h"
#include <stdio.h>


//========================================================================================
//	Definition of static members
//========================================================================================

char acs_alh_eventmgr::processName_[PROCESS_NAME_MAX_LEN];


//========================================================================================
//	Constructor
//========================================================================================

acs_alh_eventmgr::acs_alh_eventmgr() : eventReport_()
{

}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_eventmgr::~acs_alh_eventmgr()
{
}


//========================================================================================
//	Set the name of the process to be used when reporting events
//========================================================================================

void acs_alh_eventmgr::setProcessName(const char processName[]) const
{
	strncpy(processName_, processName, sizeof(processName_) - 1);
}


//========================================================================================
//	Send an event
//========================================================================================

bool acs_alh_eventmgr::sendEvent(const long specificProblem, const char percSeverity[], const char objectOfRef[],
							const char problemData[], const char problemText[])
{
	if (eventReport_.sendEventMessage( processName_, specificProblem, percSeverity, ALH_DEFAULT_PROBABLE_CAUSE,
							ALH_DEFAULT_OBJECT_CLASS_OF_REF, objectOfRef, problemData, problemText ) == ACS_AEH_error)
	{
		printf("%s\n", eventReport_.getErrorText());
		return false;
    }

    return true;
}
