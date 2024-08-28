/*=================================================================== */
   /**
   @file  acs_emf_tra.cpp

   Class method implementation for EMF module.


   @version 1.0.0
	*/
	/*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       28/02/2011   XRAMmAT  Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_emf_tra.h"

/*===================================================================
                        GLOBAL VARIABLES
=================================================================== */
const char* format = "C1024";
const char* infoTraceName = "ACS_EMF_INFO";
const char* warningTraceName = "ACS_EMF_WARNING";
const char* errorTraceName = "ACS_EMF_ERROR";
const char* debugTraceName = "ACS_EMF_DEBUG";
const char* fataTraceName = "ACS_EMF_FATAL";
/*=================================================================== */
/**
	@brief		ACS_EMF_InformationTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_EMF_Tra::ACS_EMF_InformationTrace(infoTraceName, format);

/*=================================================================== */
/**
	@brief		ACS_EMF_WarningTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_EMF_Tra::ACS_EMF_WarningTrace(warningTraceName, format);

/*=================================================================== */
/**
	@brief		ACS_EMF_ErrorTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_EMF_Tra::ACS_EMF_ErrorTrace(errorTraceName,format);

/*=================================================================== */
/**
	@brief		ACS_EMF_DebugTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_EMF_Tra::ACS_EMF_DebugTrace(debugTraceName, format);

/*=================================================================== */
/**
	@brief		ACS_EMF_FatalTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_EMF_Tra::ACS_EMF_FatalTrace(fataTraceName, format);

ACS_TRA_Logging ACS_EMF_Tra::ACS_EMF_Logging;
ACE_Mutex ACS_EMF_Tra::log_mutex;
//ACS_TRA_Logging ACS_EMF_ChildLogging;
#include <ace/ACE.h>

/*===================================================================
   ROUTINE: helperTraceInformation
=================================================================== */
void ACS_EMF_Tra::helperTraceInformation(int isLog,const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		log_mutex.acquire();
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if( ACS_EMF_InformationTrace.ACS_TRA_ON() )
		{
			ACS_EMF_InformationTrace.ACS_TRA_event(1,traceBuffer);
		}

		if (isLog)
		{
			ACS_EMF_Logging.Write(traceBuffer,LOG_LEVEL_INFO);
		}

		va_end(params);
		log_mutex.release();
	}
}// End of helperTraceInformation

/*===================================================================
   ROUTINE: helperTraceDebug
=================================================================== */
void ACS_EMF_Tra::helperTraceDebug(int isLog, const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		log_mutex.acquire();
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if( ACS_EMF_DebugTrace.ACS_TRA_ON() )
		{
			ACS_EMF_DebugTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_EMF_Logging.Write(traceBuffer,LOG_LEVEL_DEBUG);
		}
		 va_end(params);
		log_mutex.release();
	}
}// End of helperTraceDebug

/*===================================================================
   ROUTINE: helperTraceWarning
=================================================================== */

void ACS_EMF_Tra::helperTraceWarning(int isLog, const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		log_mutex.acquire();
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
		//! If Trace if ON .Send trace to TRA

		if( ACS_EMF_WarningTrace.ACS_TRA_ON() )
		{
			ACS_EMF_WarningTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_EMF_Logging.Write(traceBuffer,LOG_LEVEL_WARN);
		}
		va_end(params);
		log_mutex.release();
	}
}// End of helperTraceWarning

/*===================================================================
   ROUTINE: helperTraceError
=================================================================== */
void ACS_EMF_Tra::helperTraceError(int isLog, const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	if ( messageFormat && *messageFormat )
	{
		log_mutex.acquire();
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);

		//! If Trace if ON .Send trace to TRA

		if( ACS_EMF_ErrorTrace.ACS_TRA_ON() )
		{
			ACS_EMF_ErrorTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_EMF_Logging.Write(traceBuffer,LOG_LEVEL_ERROR);
		}
		va_end(params);
		log_mutex.release();
	}
}// End of helperTraceError

/*===================================================================
   ROUTINE: helperTraceFatal
=================================================================== */
void ACS_EMF_Tra::helperTraceFatal(int isLog,const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];

	if ( messageFormat && *messageFormat )
	{
		log_mutex.acquire();
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if(  ACS_EMF_FatalTrace.ACS_TRA_ON() )
		{
			ACS_EMF_FatalTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_EMF_Logging.Write(traceBuffer,LOG_LEVEL_FATAL);
		}
				va_end(params);

		va_end(params);
		log_mutex.release();
	}
}// End of helperTraceFatal


