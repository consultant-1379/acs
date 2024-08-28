/*
 * acs_alh_util.cpp
 *
 *  Created on: Oct 31, 2011
 *      Author: efabron
 */

#include <cstdarg>
#include <stdio.h>
#include "acs_alh_util.h"

//========================================================================================
//	Constructor
//========================================================================================

acs_alh_util::acs_alh_util(const char* class_name) : trace_obj_(class_name, "C256")
{

}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_util::~acs_alh_util()
{
}


//========================================================================================
//  trace method
//========================================================================================
int acs_alh_util::trace (const char * format, ...){

	if (trace_obj_.ACS_TRA_ON()) {
		char trace_msg[ACS_ALH_TRACE_MESSAGE_SIZE_MAX];

		va_list argp;
		va_start(argp, format);
		vsnprintf(trace_msg, ACS_ALH_ARRAY_SIZE(trace_msg), format, argp);
		trace_msg[ACS_ALH_ARRAY_SIZE(trace_msg) - 1] = 0;
		va_end(argp);

		trace_obj_.ACS_TRA_event(1, trace_msg);
	}

	return 0;
}
