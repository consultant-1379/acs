/*=================================================================== */
   /**
   @file acs_chb_tra.cpp

   Class method implementation for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_tra.h module

   @version 1.0.0
	*/
	/*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       18/02/2011   XNADNAR   Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <acs_chb_tra.h>

/*===================================================================
                        GLOBAL VARIABLES
=================================================================== */
/*=================================================================== */
/**
	@brief		ACS_CHB_InformationTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_CHB_Tra::ACS_CHB_InformationTrace("ACS_CHB_INFO", "C1024");

/*=================================================================== */
/**
	@brief		ACS_CHB_WarningTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_CHB_Tra::ACS_CHB_WarningTrace("ACS_CHB_WARNING", "C1024");

/*=================================================================== */
/**
	@brief		ACS_CHB_ErrorTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_CHB_Tra::ACS_CHB_ErrorTrace("ACS_CHB_ERROR","C1024");

/*=================================================================== */
/**
	@brief		ACS_CHB_DebugTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_CHB_Tra::ACS_CHB_DebugTrace("ACS_CHB_DEBUG", "C1024");

/*=================================================================== */
/**
	@brief		ACS_CHB_FatalTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_CHB_Tra::ACS_CHB_FatalTrace("ACS_CHB_FATAL", "C1024");

ACS_TRA_Logging ACS_CHB_Tra::ACS_CHB_Logging;
ACE_Mutex ACS_CHB_Tra::logMutex_;
#include <ace/ACE.h>

/*===================================================================
   ROUTINE: helperTraceInformationCHB
=================================================================== */
void ACS_CHB_Tra::helperTraceInformationCHB(int isLog,const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	memset( traceBuffer, 0, sizeof( traceBuffer ));

	logMutex_.acquire();
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if( ACS_CHB_InformationTrace.ACS_TRA_ON() )
		{
			ACS_CHB_InformationTrace.ACS_TRA_event(1,traceBuffer);
		}

		if (isLog)
		{
			ACS_CHB_Logging.Write(traceBuffer,LOG_LEVEL_INFO);
		}

		va_end(params);
	}
	logMutex_.release();
}// End of helperTraceInformationCHB

/*===================================================================
   ROUTINE: helperTraceDebugCHB
=================================================================== */
void ACS_CHB_Tra::helperTraceDebugCHB(int isLog, const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	memset( traceBuffer, 0, sizeof( traceBuffer ));

	logMutex_.acquire();
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if( ACS_CHB_DebugTrace.ACS_TRA_ON() )
		{
			ACS_CHB_DebugTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_CHB_Logging.Write(traceBuffer,LOG_LEVEL_DEBUG);
		}
		 va_end(params);
	}
	logMutex_.release();
}// End of helperTraceDebugCHB

/*===================================================================
   ROUTINE: helperTraceWarningCHB
=================================================================== */

void ACS_CHB_Tra::helperTraceWarningCHB(int isLog, const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	memset( traceBuffer, 0, sizeof( traceBuffer ));

	logMutex_.acquire();
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
		//! If Trace if ON .Send trace to TRA

		if( ACS_CHB_WarningTrace.ACS_TRA_ON() )
		{
			ACS_CHB_WarningTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_CHB_Logging.Write(traceBuffer,LOG_LEVEL_WARN);
		}
		va_end(params);
	}
	logMutex_.release();
}// End of helperTraceWarningCHB

/*===================================================================
   ROUTINE: helperTraceErrorCHB
=================================================================== */
void ACS_CHB_Tra::helperTraceErrorCHB(int isLog, const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	memset( traceBuffer, 0, sizeof( traceBuffer ));
	logMutex_.acquire();
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);

		//! If Trace if ON .Send trace to TRA

		if( ACS_CHB_ErrorTrace.ACS_TRA_ON() )
		{
			ACS_CHB_ErrorTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_CHB_Logging.Write(traceBuffer,LOG_LEVEL_ERROR);
		}
		va_end(params);
	}
	logMutex_.release();
}// End of helperTraceErrorCHB

/*===================================================================
   ROUTINE: helperTraceFatalCHB
=================================================================== */
void ACS_CHB_Tra::helperTraceFatalCHB(int isLog,const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE];
	memset( traceBuffer, 0, sizeof( traceBuffer ));

	logMutex_.acquire();
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);
		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE-1]='\0';

		//! If Trace if ON .Send trace to TRA
		if(  ACS_CHB_FatalTrace.ACS_TRA_ON() )
		{
			ACS_CHB_FatalTrace.ACS_TRA_event(1,traceBuffer);
		}
		if (isLog)
		{
			ACS_CHB_Logging.Write(traceBuffer,LOG_LEVEL_FATAL);
		}
				va_end(params);

		va_end(params);
	}
	logMutex_.release();
}// End of helperTraceFatalCHB


