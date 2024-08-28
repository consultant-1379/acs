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

#ifndef ACS_APSESH_EventMgr_h
#define ACS_APSESH_EventMgr_h 1

#include "acs_aeh_evreport.h"
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <time.h>

class ACS_APSESH_EventMgr 
{

  public:

    typedef enum {
    			   NONE = 0,
    			   EVENT_JTP_DISCONNECT = 10900,
    			   EVENT_APSESH_SHUTDOWN = 10901,
    			   EVENT_APSESH_FAIL = 10902,
    			   EVENT_APSESH_INFO = 10903
    } ACS_APSESH_Event;

      ACS_APSESH_EventMgr(const char *processName);

      virtual ~ACS_APSESH_EventMgr();


    //## Other Operations (specified)
      //## Operation: sendEvent%4A70825A031E
      //	Sends an event report
      //
      //	@param specificProblem
      //	  The error code number of the problem.
      //	@param percSeverity
      //	  The severity level of the problem according to AXE standard.
      //	@param probableCause
      //	  Detailed explanation of the problem (same as an AXE slogan).
      //	@param objectOfRef
      //	  Shows in which instance in the object the fault was detected.
      //	@param problemData
      //	  Plain-english text description to be logged.
      //	@param problemText
      //	  Plain-english text description to be printed.
      //
      //	@return
      //	  TRUE on success and FALSE on failure.
      bool sendEvent (int specificProblem, const char *problemData, const char *problemText);

  private:

      ACS_APSESH_EventMgr();

      ACS_APSESH_EventMgr(const ACS_APSESH_EventMgr &right);

      ACS_APSESH_EventMgr & operator=(const ACS_APSESH_EventMgr &right);

      bool checkEvent (int specificProblem);

      std::vector<time_t> m_initTime;

      std::vector<int> m_count;

      acs_aeh_evreport m_eventReport;

      std::string m_processName;

      std::string m_objOfRef;
};

#endif
