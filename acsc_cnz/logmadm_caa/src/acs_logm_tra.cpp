
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <acs_logm_tra.h>

/*===================================================================
                        GLOBAL VARIABLES
=================================================================== */
/*=================================================================== */
/**
	@brief		ACS_LOGMInformationTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LOGMInformationTrace("ACS_LOGMINFO", "C1024");

/*=================================================================== */
/**
	@brief		ACS_LOGMWarningTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LOGMWarningTrace("ACS_LOGMWARNING", "C1024");

/*=================================================================== */
/**
	@brief		ACS_LOGMErrorTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LOGMErrorTrace("ACS_LOGMERROR","C1024");

/*=================================================================== */
/**
	@brief		ACS_LOGMACS_LOGMDebugTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LOGMDebugTrace("ACS_LOGMDEBUG", "C1024");

/*=================================================================== */
/**
	@brief		ACS_LOGMFatalTrace
*/
/*=================================================================== */
ACS_TRA_trace ACS_LOGMFatalTrace("ACS_LOGMFATAL", "C1024");

ACS_TRA_Logging ACS_LOGM_Logging;
#include <ace/ACE.h>

/*===================================================================
   ROUTINE: helperTraceInformationLOGM
=================================================================== */
void helperTraceInformationLOGM(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE] = {0};

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
		if( ACS_LOGMInformationTrace.ACS_TRA_ON() )
		{
			ACS_LOGMInformationTrace.ACS_TRA_event(1,myStr.c_str());
			//ACS_LOGMInformationTrace.ACS_TRA_event(1,traceBuffer);
		}
		ACS_LOGM_Logging.Write(myStr.c_str(),LOG_LEVEL_INFO);
		//ACS_LOGM_Logging.Write(traceBuffer,LOG_LEVEL_INFO);
		va_end(params);
	}
}// End of helperTraceInformationLOGM

/*===================================================================
   ROUTINE: helperTraceDebugLOGM
=================================================================== */
void helperTraceDebugLOGM(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE] = {0};

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
		if( ACS_LOGMDebugTrace.ACS_TRA_ON() )
		{
			ACS_LOGMWarningTrace.ACS_TRA_event(1,myStr.c_str());
			//ACS_LOGMDebugTrace.ACS_TRA_event(1,traceBuffer);
		}
		ACS_LOGM_Logging.Write(myStr.c_str(),LOG_LEVEL_DEBUG);

		va_end(params);
	}
}// End of helperTraceDebugLOGM

/*===================================================================
   ROUTINE: helperTraceWarningLOGM
=================================================================== */

void helperTraceWarningLOGM(const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE] = {0};

	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
		//! If Trace if ON .Send trace to TRA

		std::stringstream myostr;
		myostr << "Thread id == "<<ACE_Thread::self()<<" ";
		std::string myStr = myostr.str() + std::string(traceBuffer);

		if( ACS_LOGMWarningTrace.ACS_TRA_ON() )
		{
			ACS_LOGMWarningTrace.ACS_TRA_event(1,myStr.c_str());
			//ACS_LOGMWarningTrace.ACS_TRA_event(1,traceBuffer);
		}
		ACS_LOGM_Logging.Write(myStr.c_str(),LOG_LEVEL_WARN);
		//ACS_LOGM_Logging.Write(traceBuffer,LOG_LEVEL_WARN);
		va_end(params);
	}
}// End of helperTraceWarningLOGM

/*===================================================================
   ROUTINE: helperTraceErrorLOGM
=================================================================== */
void helperTraceErrorLOGM(const char* messageFormat, ...)
{
	static const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE] = {0};
	if ( messageFormat && *messageFormat )
	{
		va_list params;
		va_start(params, messageFormat);

		ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);

		//! If Trace if ON .Send trace to TRA
		std::stringstream myostr;
		myostr << "Thread id == "<<ACE_Thread::self()<<" ";
		std::string myStr = myostr.str() + std::string(traceBuffer);


		if( ACS_LOGMErrorTrace.ACS_TRA_ON() )
		{
			ACS_LOGMErrorTrace.ACS_TRA_event(1,myStr.c_str());
			//ACS_LOGMErrorTrace.ACS_TRA_event(1,traceBuffer);
		}
		ACS_LOGM_Logging.Write(myStr.c_str(),LOG_LEVEL_ERROR);
		//ACS_LOGM_Logging.Write(traceBuffer,LOG_LEVEL_ERROR);
		va_end(params);
	}
}// End of helperTraceErrorLOGM

/*===================================================================
   ROUTINE: helperTraceFatalLOGM
=================================================================== */
void helperTraceFatalLOGM(const char* messageFormat, ...)
{
	const unsigned int TRACE_BUF_SIZE = 1024;
	char traceBuffer[TRACE_BUF_SIZE] = {0};

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
		if(  ACS_LOGMFatalTrace.ACS_TRA_ON() )
		{
			ACS_LOGMFatalTrace.ACS_TRA_event(1,myStr.c_str());
			//ACS_LOGMFatalTrace.ACS_TRA_event(1,traceBuffer);
		}
		ACS_LOGM_Logging.Write(myStr.c_str(),LOG_LEVEL_FATAL);
		//ACS_LOGM_Logging.Write(traceBuffer,LOG_LEVEL_FATAL);

		va_end(params);
	}
}// End of helperTraceFatalLOGM




