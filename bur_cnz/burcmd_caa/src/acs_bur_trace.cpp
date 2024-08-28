/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2014
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file acs_bur_trace.cpp
 *
 * @brief
 * Used to log messages into Log file
 *
 * @details
 * BUR messages like warnings, errors etc are required to log into
 * ACS_BUR.log file.
 *
 * @author
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 13/12/2011     EGIMARR       Initial Release
 * 13/02/2014     XVENJAM       cpp check error correction
 ****************************************************************************/

#include "acs_bur_trace.h"
#include <ACS_TRA_trace.h>

/*============================================================================
	ROUTINE: trautil_trace
 ============================================================================ */
void trautil_trace(ACS_TRA_trace* trace_class, const ACE_TCHAR* messageFormat, ...)
{
	const ACE_UINT32 TRACE_BUF_SIZE = 1024;
	ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
	if( trace_class->ACS_TRA_ON() )
	{
		if( messageFormat && *messageFormat )
		{
			va_list params;
			va_start(params, messageFormat);
			ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
			traceBuffer[TRACE_BUF_SIZE-1]='\0';
			trace_class->ACS_TRA_event(1,traceBuffer);
			va_end(params);
		}
	}
}
