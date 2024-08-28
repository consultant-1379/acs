/*=================================================================== */
   /**
   @file jtp.cpp

   This module contains the implementation of methods and variables declared in
   the jtp.h file.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/04/2010     XTANAGG        APG43 on Linux.
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <jtp.h>
#include <ace/Thread.h>

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
int JSL_init = 0; //Empty slots in the list have NULL value
ACS_JTP_CriticalSection g_JSL_init_Sync;

JSLCL* CL = 0; // Client List; The list of disconnected sessions
int CLSize = 0; // The current size of the list
ACS_JTP_CriticalSection g_CLSync;
ACS_JTP_CriticalSection g_CLSync_new;//changes for sts hang issue

JTP_Session ** SL = 0; // Server List; The list of active sessions
int SLSize = 0; // The current size of the list

ACS_JTP_CriticalSection g_SLSync;
ACS_JTP_CriticalSection g_SLSync_new;//changes for sts hang issue

unsigned char JSLAPNo = 0;
unsigned char JSLAPRestartCntr = 0;

unsigned short JSLPID = 0;
unsigned short JSLConnectionCntr = 0;

/**
 * @brief	ACS_JTP_InformationTrace
 */
ACS_TRA_trace ACS_JTP_InformationTrace("ACS_JTP_InformationTrace", "C512");

/**
 * @brief	ACS_JTP_WarningTrace
 */
ACS_TRA_trace ACS_JTP_WarningTrace("ACS_JTP_WarningTrace", "C512");

/**
 * @brief	ACS_JTP_ErrorTrace
 */
ACS_TRA_trace ACS_JTP_ErrorTrace("ACS_JTP_ErrorTrace", "C512");


/*===================================================================
   ROUTINE: helperTraceInformation
=================================================================== */
void helperTraceInformation(const char* messageFormat, ...) {
	static const unsigned int TRACE_BUF_SIZE = 512U;

	if (messageFormat && *messageFormat) {
		va_list params;
		va_start(params, messageFormat);
		char traceBuffer[TRACE_BUF_SIZE];
		ACE_OS::memset(traceBuffer,0,TRACE_BUF_SIZE);
		int prefixLength = ACE_OS::snprintf(traceBuffer, TRACE_BUF_SIZE - 1, "[INFORMATION_TID_%u] ", ACE_Thread::self());
		if (prefixLength < 0) prefixLength = 0;
		ACE_OS::vsnprintf(traceBuffer + prefixLength, TRACE_BUF_SIZE - 1 - prefixLength, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE - 1] = '\0';
		if( ACS_JTP_TRACE_INFORMATION_ACTIVE )
		{
			ACS_JTP_InformationTrace.ACS_TRA_event(1, traceBuffer);

		}
		va_end(params);
	}
}

/*===================================================================
   ROUTINE: helperTraceWarning
=================================================================== */
void helperTraceWarning(const char* messageFormat, ...) {
	static const unsigned int TRACE_BUF_SIZE = 512U;

	if (messageFormat && *messageFormat) {
		va_list params;
		va_start(params, messageFormat);
		char traceBuffer[TRACE_BUF_SIZE];
		ACE_OS::memset(traceBuffer,0,TRACE_BUF_SIZE);
		int prefixLength = ACE_OS::snprintf(traceBuffer, TRACE_BUF_SIZE - 1, "[WARNING_TID_%u] ",  ACE_Thread::self());
		if (prefixLength < 0) prefixLength = 0;
		ACE_OS::vsnprintf(traceBuffer + prefixLength, TRACE_BUF_SIZE - 1 - prefixLength, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE - 1] = '\0';
		if( ACS_JTP_TRACE_WARNING_ACTIVE )
		{
			ACS_JTP_WarningTrace.ACS_TRA_event(1, traceBuffer);
		}
		va_end(params);
	}
}

/*===================================================================
   ROUTINE: helperTraceError
=================================================================== */
void helperTraceError(const char* messageFormat, ...) {
	static const unsigned int TRACE_BUF_SIZE = 512U;

	if (messageFormat && *messageFormat) {
		va_list params;
		va_start(params, messageFormat);
		char traceBuffer[TRACE_BUF_SIZE];
		ACE_OS::memset(traceBuffer,0,TRACE_BUF_SIZE);
		int prefixLength = ACE_OS::snprintf(traceBuffer, TRACE_BUF_SIZE - 1, "[ERROR_TID_%u] ",  ACE_Thread::self());
		if (prefixLength < 0) prefixLength = 0;
		ACE_OS::vsnprintf(traceBuffer + prefixLength, TRACE_BUF_SIZE - 1 - prefixLength, messageFormat, params);
		traceBuffer[TRACE_BUF_SIZE - 1] = '\0';
		if( ACS_JTP_TRACE_ERROR_ACTIVE )
		{
			ACS_JTP_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		va_end(params);
	}
}

/*===================================================================
   ROUTINE: short2AXE
=================================================================== */
short short2AXE(short I) {
	I = htons(I);
	short O;
	char* In = (char*) &I;
	char* Out= (char*) &O;

	Out[0] = In[1];
	Out[1] = In[0];

	return O;
}

/*===================================================================
   ROUTINE: AXE2short
=================================================================== */
short AXE2short(short I) {
	short O;

	char* In = (char*) &I;
	char* Out = (char*) &O;

	Out[0] = In[1];
	Out[1] = In[0];

	return ntohs(O);
}

/*===================================================================
   ROUTINE: long2RPC
=================================================================== */
unsigned long long2RPC(unsigned long I) {
	I = htonl(I);
	char* In = (char*) &I;
	In[0] |= 128;
	return I;
}

/*===================================================================
   ROUTINE: RPC2long
=================================================================== */
unsigned long RPC2long(unsigned long I) {
	char* In = (char*) &I;

	char noll = In[0];
	In[0] &= 127;
	if (noll == In[0]) return 65536;
	I = ntohl(I);
	return I;
}
