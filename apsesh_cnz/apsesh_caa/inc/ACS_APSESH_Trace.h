//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_Trace
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

#ifndef ACS_APSESH_Trace_h
#define ACS_APSESH_Trace_h 1

#include "ACS_APSESH_EventMgr.h"

#include "ACS_TRA_Logging.h"

#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>

class ACS_TRA_Logging;
class ACS_TRA_trace;

class ACS_APSESH_Trace 
{
 public:

	friend class ACE_Singleton<ACS_APSESH_Trace, ACE_Recursive_Thread_Mutex>;

	//Trace and log
	void trace (const char* sender, const char* msg, bool enableLog = false, ACS_TRA_LogLevel logLevel = LOG_LEVEL_TRACE);

	//Trace and write an AEH event
	void trace (const char* sender, int problemNumber, const char* problemData, const char* problemText);

 private:

	ACS_APSESH_Trace();
	virtual ~ACS_APSESH_Trace();

	ACS_APSESH_Trace(const ACS_APSESH_Trace &right);
	ACS_APSESH_Trace& operator=(const ACS_APSESH_Trace &right);

	//CRITICAL_SECTION m_criticalSection;
	ACE_Recursive_Thread_Mutex m_mutex;

	//Logger
	ACS_TRA_Logging* m_logObj;

	//Tracer
	ACS_TRA_trace* m_traceObj;

	//Event Manager
	ACS_APSESH_EventMgr m_eventMgr;

};

typedef ACE_Singleton<ACS_APSESH_Trace, ACE_Recursive_Thread_Mutex> APSESH_Tracer;

#endif
