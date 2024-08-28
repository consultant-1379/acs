//******************************************************************************
//
//  NAME
//     acs_lm_tra.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_lm_tra.h"

/*===================================================================
                        GLOBAL VARIABLES
=================================================================== */
/*=================================================================== */
/**
	@brief		ACS_LM_InformationTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LM_InformationTrace("ACS_LM_INFO", "C1024");

/*=================================================================== */
/**
	@brief		ACS_LM_WarningTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LM_WarningTrace("ACS_LM_WARNING", "C1024");

/*=================================================================== */
/**
	@brief		ACS_LM_ErrorTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LM_ErrorTrace("ACS_LM_ERROR","C1024");

/*=================================================================== */
/**
	@brief		ACS_LM_ACS_LM_DebugTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LM_DebugTrace("ACS_LM_DEBUG", "C1024");

/*=================================================================== */
/**
	@brief		ACS_LM_FatalTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LM_FatalTrace("ACS_LM_FATAL", "C1024");

ACS_TRA_Logging ACS_LM_Logging;
#include <ace/ACE.h>

/*===================================================================
   ROUTINE: helperTraceInformationLM
=================================================================== */
void helperTraceInformationLM(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		std::stringstream myostr;
		myostr << "Thread id == "<<ACE_Thread::self()<<" ";
		std::string myStr = myostr.str() + std::string(traceBuffer);

		//! If Trace if ON .Send trace to TRA
		if( ACS_LM_InformationTrace.ACS_TRA_ON() )
		{
			//ACS_LM_InformationTrace.ACS_TRA_event(1,traceBuffer);
			ACS_LM_InformationTrace.ACS_TRA_event(1,myStr.c_str());
		}

		//ACS_LM_Logging.Write(traceBuffer,LOG_LEVEL_INFO);
		ACS_LM_Logging.Write(myStr.c_str(),LOG_LEVEL_INFO);
		va_end(params);
	}
}// End of helperTraceInformationLM

/*===================================================================
   ROUTINE: helperTraceDebugLM
=================================================================== */
void helperTraceDebugLM(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		std::stringstream myostr;
		myostr << "Thread id == "<<ACE_Thread::self()<<" ";
		std::string myStr = myostr.str() + std::string(traceBuffer);

		//! If Trace if ON .Send trace to TRA
		if( ACS_LM_DebugTrace.ACS_TRA_ON() )
		{
			//ACS_LM_DebugTrace.ACS_TRA_event(1,traceBuffer);
			ACS_LM_DebugTrace.ACS_TRA_event(1,myStr.c_str());
		}
		//ACS_LM_Logging.Write(traceBuffer,LOG_LEVEL_DEBUG);
		ACS_LM_Logging.Write(myStr.c_str(),LOG_LEVEL_DEBUG);
		va_end(params);
	}
}// End of helperTraceDebugLM

/*===================================================================
   ROUTINE: helperTraceWarningLM
=================================================================== */

void helperTraceWarningLM(const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		std::stringstream myostr;
		myostr << "Thread id == "<<ACE_Thread::self()<<" ";
		std::string myStr = myostr.str() + std::string(traceBuffer);
		//! If Trace if ON .Send trace to TRA

		if( ACS_LM_WarningTrace.ACS_TRA_ON() )
		{
			//ACS_LM_WarningTrace.ACS_TRA_event(1,traceBuffer);
			ACS_LM_WarningTrace.ACS_TRA_event(1,myStr.c_str());
		}
		//ACS_LM_Logging.Write(traceBuffer,LOG_LEVEL_WARN);
		ACS_LM_Logging.Write(myStr.c_str(),LOG_LEVEL_WARN);
		va_end(params);
	}
}// End of helperTraceWarningLM

/*===================================================================
   ROUTINE: helperTraceErrorLM
=================================================================== */
void helperTraceErrorLM(const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		std::stringstream myostr;
		myostr << "Thread id == "<<ACE_Thread::self()<<" ";
		std::string myStr = myostr.str() + std::string(traceBuffer);

		//! If Trace if ON .Send trace to TRA

		if( ACS_LM_ErrorTrace.ACS_TRA_ON() )
		{
			//ACS_LM_ErrorTrace.ACS_TRA_event(1,traceBuffer);
			ACS_LM_ErrorTrace.ACS_TRA_event(1,myStr.c_str());
		}

		//ACS_LM_Logging.Write(traceBuffer,LOG_LEVEL_ERROR);
		ACS_LM_Logging.Write(myStr.c_str(),LOG_LEVEL_ERROR);
		va_end(params);
	}
}// End of helperTraceErrorLM

/*===================================================================
   ROUTINE: helperTraceFatalLM
=================================================================== */
void helperTraceFatalLM(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';



		std::stringstream myostr;
		myostr << "Thread id == "<<ACE_Thread::self()<<" ";
		std::string myStr = myostr.str() + std::string(traceBuffer);

		//! If Trace if ON .Send trace to TRA
		if(  ACS_LM_FatalTrace.ACS_TRA_ON() )
		{
			//ACS_LM_FatalTrace.ACS_TRA_event(1,traceBuffer);
			ACS_LM_FatalTrace.ACS_TRA_event(1,myStr.c_str());
		}

		//ACS_LM_Logging.Write(traceBuffer,LOG_LEVEL_FATAL);
		ACS_LM_Logging.Write(myStr.c_str(),LOG_LEVEL_FATAL);

		va_end(params);
	}
}// End of helperTraceFatalLM




