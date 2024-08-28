//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_Trace
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
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

#include "ACS_APSESH_Trace.h"

#include "ACS_TRA_trace.h"

#include <ace/Guard_T.h>

#include <sstream>
#include <iostream>

namespace {
	const char ACS_APSESH_PROCESS_NAME[] = "ACS_APSESH";
	const char APSESH_LOG_APPENDER_NAME[] = "ACS_APSESHD";
	const char APSESH_TRACE_NAME[] = "ACS_APSESH_Trace";
}


ACS_APSESH_Trace::ACS_APSESH_Trace()
: m_logObj(0),
  m_traceObj(0),
  m_eventMgr(ACS_APSESH_PROCESS_NAME)
{

}

ACS_APSESH_Trace::~ACS_APSESH_Trace()
{
	if( 0 != m_logObj )
	{
		m_logObj->Close();
		delete m_logObj;
		m_logObj = 0;
	}

	if( 0!= m_traceObj)
	{
		delete m_traceObj;
		m_traceObj = 0;
	}

}

void ACS_APSESH_Trace::trace (const char* sender, const char* msg, bool enableLog, ACS_TRA_LogLevel logLevel)
{
	// Allow only one thread at the time to enter this code.
	// guard is destroyed when out of scope, automatically releasing the lock.
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	//Build log message
	std::stringstream ss;

	if (sender != 0)
	{
		std::cout << "[SENDER:] " << sender;
		ss << "[SENDER:] " << sender;
	}

	if (msg != 0)
	{
		std::cout << "[MSG:] " << msg << std::endl;
		ss << " [MSG:] " << msg;
	}

	//checks if the trace is available for the related task
	if(0 == m_traceObj)
	{
		//initialize logger
		ACE_NEW_NORETURN(m_traceObj, ACS_TRA_trace(APSESH_TRACE_NAME) );
	}

	if (m_traceObj->isOn())
	{
		//trace message
		m_traceObj->trace(ss.str());
	}

	// Check if the msg is to log
	if(enableLog)
	{
		if(0 == m_logObj)
		{
			//initialize logger
			ACE_NEW_NORETURN(m_logObj, ACS_TRA_Logging());
			m_logObj->Open(APSESH_LOG_APPENDER_NAME);
		}
		//log message
		m_logObj->Write(ss.str().c_str(), logLevel);
	}
}

void ACS_APSESH_Trace::trace(const char* sender, int problemNumber, const char* problemData, const char* problemText)
{
	// Allow only one thread at the time to enter this code.
	// guard is destroyed when out of scope, automatically releasing the lock.
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if (problemText) trace(sender, problemText, true, LOG_LEVEL_ERROR);

	m_eventMgr.sendEvent(problemNumber, (problemData ? problemData : "[NO DATA]"), (problemText ? problemText : "[NO TEXT]"));
}
