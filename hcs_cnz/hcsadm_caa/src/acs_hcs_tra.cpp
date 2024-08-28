//******************************************************************************
//
//  NAME
//     ACS_HC_tra.cpp
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

#include "acs_hcs_tracer.h"
#include <ace/ACE.h>
#include "acs_hcs_tra.h"

/*===================================================================
                        GLOBAL VARIABLES
=================================================================== */
/*=================================================================== */
/**
	@brief		ACS_HC_InformationTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_HC_Tra::ACS_HC_InformationTrace("ACS_HC_INFO", "C400");

/*=================================================================== */
/**
	@brief		ACS_HC_WarningTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_HC_Tra::ACS_HC_WarningTrace("ACS_HC_WARNING", "C400");

/*=================================================================== */
/**
	@brief		ACS_HC_ErrorTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_HC_Tra::ACS_HC_ErrorTrace("ACS_HC_ERROR","C400");

/*=================================================================== */
/**
	@brief		ACS_HC_DebugTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_HC_Tra::ACS_HC_DebugTrace("ACS_HC_DEBUG", "C400");

/*=================================================================== */
/**
	@brief		ACS_HC_FatalTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_HC_Tra::ACS_HC_FatalTrace("ACS_HC_FATAL", "C400");

#include <ace/ACE.h>

/*===================================================================
   ROUTINE: helperTraceInformation
=================================================================== */
void ACS_HC_Tra::helperTraceInformation(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if( ACS_HC_InformationTrace.ACS_TRA_ON() )
		{
			ACS_HC_InformationTrace.ACS_TRA_event(1,traceBuffer);
		}

		//ACS_HC_Logging.Write(traceBuffer,LOG_LEVEL_INFO);
		ACS_HC_LOG(LOG_LEVEL_INFO, traceBuffer);
		va_end(params);
	}
}// End of helperTraceInformation

/*===================================================================
   ROUTINE: helperTraceDebug
=================================================================== */
void ACS_HC_Tra::helperTraceDebug(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	 ACE_OS::memset(traceBuffer, 0, 1024);

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if( ACS_HC_DebugTrace.ACS_TRA_ON() )
		{
			ACS_HC_DebugTrace.ACS_TRA_event(1,traceBuffer);
		}
		//ACS_HC_Logging.Write(traceBuffer,LOG_LEVEL_DEBUG);
		ACS_HC_LOG(LOG_LEVEL_DEBUG, traceBuffer);
		va_end(params);
	}
}// End of helperTraceDebug

/*===================================================================
   ROUTINE: helperTraceWarning
=================================================================== */

void ACS_HC_Tra::helperTraceWarning(const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
		//! If Trace if ON .Send trace to TRA

		if( ACS_HC_WarningTrace.ACS_TRA_ON() )
		{
			ACS_HC_WarningTrace.ACS_TRA_event(1,traceBuffer);
		}
		//ACS_HC_Logging.Write(traceBuffer,LOG_LEVEL_WARN);
		ACS_HC_LOG(LOG_LEVEL_WARN, traceBuffer);
		va_end(params);
	}
}// End of helperTraceWarning

/*===================================================================
   ROUTINE: helperTraceError
=================================================================== */
void ACS_HC_Tra::helperTraceError(const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);

		//! If Trace if ON .Send trace to TRA

		if( ACS_HC_ErrorTrace.ACS_TRA_ON() )
		{
			ACS_HC_ErrorTrace.ACS_TRA_event(1,traceBuffer);
		}

		//ACS_HC_Logging.Write(traceBuffer,LOG_LEVEL_ERROR);
		ACS_HC_LOG(LOG_LEVEL_ERROR, traceBuffer);	
		va_end(params);
	}
}// End of helperTraceError

/*===================================================================
   ROUTINE: helperTraceFatal
=================================================================== */
void ACS_HC_Tra::helperTraceFatal(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if(  ACS_HC_FatalTrace.ACS_TRA_ON() )
		{
			ACS_HC_FatalTrace.ACS_TRA_event(1,traceBuffer);
		}

		//ACS_HC_Logging.Write(traceBuffer,LOG_LEVEL_FATAL);
		ACS_HC_LOG(LOG_LEVEL_FATAL, traceBuffer);
		va_end(params);
	}
}// End of helperTraceFatal




