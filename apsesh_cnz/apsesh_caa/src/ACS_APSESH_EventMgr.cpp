//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_EventMgr
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************


#include "ACS_APSESH_Event.h"
#include "ACS_APSESH_EventMgr.h"

#include <iostream>

namespace APSESH_AEH{

	const int MAX_EVENTS_PER_MODULE = 100;
	const int APSESH_EVENT_BASE = 10900;
	const int EVENT_INTERVAL = 300;

	const char OBJECT_CLASS_OF_REF[] = "APZ";
	const char PERC_SEVERITY[] = "EVENT";
	const char PROBLEM_CAUSE[] = "CAUSE";

}

ACS_APSESH_EventMgr::ACS_APSESH_EventMgr(const char *processName)
  : m_initTime(APSESH_AEH::MAX_EVENTS_PER_MODULE, 0),
    m_count(APSESH_AEH::MAX_EVENTS_PER_MODULE, 0),
    m_eventReport()
{
	char tmp[50];
	snprintf(tmp, sizeof(tmp) - 1, "%s:%d", processName, getpid());
	m_objOfRef = tmp;
	m_processName = tmp;
}

ACS_APSESH_EventMgr::~ACS_APSESH_EventMgr()
{

}

bool ACS_APSESH_EventMgr::sendEvent (int specificProblem, const char *problemData, const char *problemText)
{
   if ( (specificProblem - APSESH_AEH::APSESH_EVENT_BASE) < 0 ) return false;

   if (checkEvent(specificProblem))
   {
	   char tmpProblemData[512];
	   snprintf(tmpProblemData, sizeof(tmpProblemData) - 1, "%s\n", problemData);
      
	   char tmpProblemText[256];
	   snprintf(tmpProblemText, sizeof(tmpProblemText) - 1, "%s. No. of Events Suppressed: %d", problemText, m_count[(specificProblem - APSESH_AEH::APSESH_EVENT_BASE)]);

	   if (ACS_AEH_error == m_eventReport.sendEventMessage(m_processName.c_str(),
															  specificProblem,
															  APSESH_AEH::PERC_SEVERITY,
															  APSESH_AEH::PROBLEM_CAUSE,
															  APSESH_AEH::OBJECT_CLASS_OF_REF,
															  m_objOfRef.c_str(),
															  tmpProblemData,
															  tmpProblemText))
       {
          return false;
       }
       else
       {
          m_count[(specificProblem - APSESH_AEH::APSESH_EVENT_BASE)] = 0;
          return true;
       }
   }
   else
   {
      m_count[(specificProblem - APSESH_AEH::APSESH_EVENT_BASE)]++;
      return true;
   }
}

bool ACS_APSESH_EventMgr::checkEvent (int specificProblem)
{
   double elapsedTime;
   time_t finalTime;

   if (specificProblem == ACS_APSESH_EventMgr::EVENT_APSESH_INFO )
   {
      // Quorum data events should not be filtered.
      return true;
   }

   if ((specificProblem - APSESH_AEH::APSESH_EVENT_BASE) < 0)
   {
      return false;
   }
   
   if (m_initTime[(specificProblem - APSESH_AEH::APSESH_EVENT_BASE)] == 0)
   {
      time(&m_initTime[(specificProblem - APSESH_AEH::APSESH_EVENT_BASE)]);
      return true;
   }
   time(&finalTime);
   elapsedTime = difftime(finalTime, m_initTime[(specificProblem - APSESH_AEH::APSESH_EVENT_BASE)]);

   if (elapsedTime > APSESH_AEH::EVENT_INTERVAL)
   {
      m_initTime[(specificProblem - APSESH_AEH::APSESH_EVENT_BASE)] = finalTime;
      return true;
   }
   else
   {
      return false;
   }
}
