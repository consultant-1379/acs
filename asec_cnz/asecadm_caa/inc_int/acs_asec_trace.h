/*
 * acs_asec_trace.h
 *
 *  Created on: 2019/03/26
 *      Author: enungai
 */

#ifndef ACS_ASEC_TRACE_H_
#define ACS_ASEC_TRACE_H_

class ACS_TRA_trace;

// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

#define TRACE(TRACE_CLASS, FMT, ...) \
		trautil_trace(TRACE_CLASS, FMT, __VA_ARGS__);


void trautil_trace(ACS_TRA_trace* trace_class, const char* messageFormat, ...);

#endif 
