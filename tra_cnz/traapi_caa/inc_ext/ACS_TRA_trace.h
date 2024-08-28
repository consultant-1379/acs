/*
 * .NAME: ACS_TRA_trace.h
 *
 * .LIBRARY 3C++
 * .PAGENAME
 * .HEADER
 * .LEFT_FOOTER Ericsson Telecom AB
 * .INCLUDE
 *
 * .COPYRIGHT
 *  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 *  All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Utvecklings AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Utvecklings AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 * 	AP trace is intended to be a complement to the set of tools
 *      that can be used for investigations of system behaviour.
 *
 *      Traceability is provided by the traced program units through
 *      explicit usage of the trace API. Actual tracing will only take
 *      place at the explicit request of a trace user via the trace_utility
 *      program.
 *
 *	The ACS_TRA_trace structure shall be used for all trace points
 *	an application wants to define in the Adjunct Processor.
 *	An instance of ACS_TRA_trace should be created by every program
 *	that wants to define trace points.
 *
 *      To implement a trace point:
 *      - Declare a global trace control block instance of type ACS_TRA_trace
 *        and initialize it, using the ACS_TRA_DEF macro, with its name
 *        and data format.
 *      - Build an IF statement with ACS_TRA_ON macro to test if the trace
 *        point is selected.
 *      - If so, call ACS_TRA_event, which logs trace data into the shared
 *        memory.
 *
 * .ERROR HANDLING
 *
 *      The Trace API does not return any error code. Problems with tracing
 *      are reported via the trace_utility program, and/or error messages to
 *      the console. The reason for that is that trace is only a tool for
 *      analysis of problems, and the error handling for the trace itself
 *      should be as simple as possible.
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0024
 *
 * AUTHOR
 * 2010-05-06 XYV XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	        20100505	XGIOPAP		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *		  								"APG50 Tracing and MKTR"
 * PA2 	        20101104	XGIOPAP		Implementation of comment
 * PA3 	        20110120	XGIOPAP		Implementation of comment
 *
 * .LINKAGE
 *	APG43 on linux: Release: libtraapi.so
 *	APG43 on linux: Debug:	 libtraapi.so
 *
 * .SEE ALSO
 *  1/15510-CXA11003
 */

#ifndef ACS_TRA_TRACE_H_
#define ACS_TRA_TRACE_H_

#include "ACS_TRA_common.h"
#include <errno.h>
#include <stdarg.h>

/*
 * Name: ACS_TRA_trace
 * Description: This class provided by the traced program units
 * 		through explicit usage of the trace API
 */
class ACS_TRA_trace 
{
	char recordname[100];
	char dataformat[100];
	int   *traceflagptr;		// pointer to trace mask

public:
	ACS_TRA_trace(char*, char*);	// deprecated method
	ACS_TRA_trace(const char*, const char*);
	ACS_TRA_trace(std::string, std::string);
	ACS_TRA_trace(const std::string &className);
	void ACS_TRA_event (int num_par,...);
	void trace(const std::string &s);

	inline int ACS_TRA_ON() { return *traceflagptr; }
	inline bool isOn() { return (ACS_TRA_ON() != 0); }

	void do_log (trace_taskp taskp, char proc[], char record[], struct timeval * timeofday, va_list ap);
//	void do_log (relpointer trceoffs, trace_taskp taskp, struct timeval * timeofday, va_list ap);
	void install_pid(relpointer proc);
	char* getProcName(char* b);
	std::string getRecordName();
};
#endif /* ACS_TRA_TRACE_H_ */
