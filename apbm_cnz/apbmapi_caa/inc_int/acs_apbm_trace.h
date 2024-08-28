#ifndef HEADER_GUARD_FILE__acs_apbm_trace
#define HEADER_GUARD_FILE__acs_apbm_trace acs_apbm_trace

/** @file acs_apbm_trace.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-07-19
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-02-19 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <stdarg.h>
#include <stdint.h>

#include <ACS_TRA_trace.h>

#include "acs_apbm_macros.h"


#define ACS_APBM_TRACE_MESSAGE_SIZE_MAX 8192
#define ACS_APBM_TRACE_DEFAULT_DUMP_LINE_LENGTH 16

#ifdef ACS_APBM_HAS_TRACE
#define ACS_APBM_TRACE_DEFINE(tag) namespace { acs_apbm_tracer __acs_apbm_tracer_object__(ACS_APBM_STRINGIZE(tag)); }
#define ACS_APBM_TRACE_MESSAGE_IMPL(...) __acs_apbm_tracer_object__.trace(__VA_ARGS__)
#ifdef ACS_APBM_TRACE_USE_FILE_INFO
#define ACS_APBM_TRACE_MESSAGE(...) ACS_APBM_TRACE_MESSAGE_IMPL(__FILE__, __LINE__, __VA_ARGS__)
#else
#define ACS_APBM_TRACE_MESSAGE(...) ACS_APBM_TRACE_MESSAGE_IMPL(__VA_ARGS__)
#endif
#define ACS_APBM_TRACE_DUMP(buffer, size, ...) __acs_apbm_tracer_object__.dump(buffer, size, __VA_ARGS__)
#ifdef ACS_APBM_TRACE_HAS_FUNCTION_TRACE
#define ACS_APBM_TRACE_FUNCTION_IMPL(...) \
	acs_apbm_function_tracer __acs_apbm_enter_function__(__acs_apbm_tracer_object__, __VA_ARGS__)
#else
#define ACS_APBM_TRACE_FUNCTION_IMPL(...)
#endif
#ifdef ACS_APBM_TRACE_USE_PRETTY_FUNCTION
#ifdef ACS_APBM_TRACE_USE_FILE_INFO
#define ACS_APBM_TRACE_FUNCTION ACS_APBM_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define ACS_APBM_TRACE_FUNCTION ACS_APBM_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__)
#endif
#else
#ifdef ACS_APBM_TRACE_USE_FILE_INFO
#define ACS_APBM_TRACE_FUNCTION ACS_APBM_TRACE_FUNCTION_IMPL(__func__, __FILE__, __LINE__)
#else
#define ACS_APBM_TRACE_FUNCTION ACS_APBM_TRACE_FUNCTION_IMPL(__func__)
#endif
#endif
#else
#define ACS_APBM_TRACE_DEFINE(tag)
#define ACS_APBM_TRACE_MESSAGE(...)
#define ACS_APBM_TRACE_DUMP(buffer, size, ...)
#define ACS_APBM_TRACE_FUNCTION
#endif


#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_apbm_tracer


class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__ (const char * tag) : _tra_tracer(tag, "C") {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ &);


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	inline int trace (const char * file, int line, const char * format, ...) __attribute__ ((format (printf, 4, 5))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, line, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace (const char * file, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, -1, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace (int line, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(0, line, format, argp);
		::va_end(argp);
		return call_result;
	}

	int trace (const char * format, ...) __attribute__ ((format (printf, 2, 3))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(0, -1, format, argp);
		::va_end(argp);
		return call_result;
	}

	void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = ACS_APBM_TRACE_DEFAULT_DUMP_LINE_LENGTH);

private:
	int vtrace (const char * file, int line, const char * format, va_list ap);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ &);


	//========//
	// Fields //
	//========//
private:
	ACS_TRA_trace _tra_tracer;
};


#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_apbm_function_tracer


class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ (acs_apbm_tracer & tracer, const char * function_name);
	__CLASS_NAME__ (acs_apbm_tracer & tracer, const char * function_name, const char * file_name, int line = -1);

private:
	__CLASS_NAME__ (const __CLASS_NAME__ &);


	//============//
	// Destructor //
	//============//
public:
	virtual ~__CLASS_NAME__ ();


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ &);


	//========//
	// Fields //
	//========//
private:
	acs_apbm_tracer & _tracer;
	const char * _function_name;
};

#endif // HEADER_GUARD_FILE__acs_apbm_trace
