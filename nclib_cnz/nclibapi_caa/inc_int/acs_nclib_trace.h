#ifndef HEADER_GUARD_FILE__acs_nclib_trace
#define HEADER_GUARD_FILE__acs_nclib_trace

/** @file acs_nclib_trace.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-10-01
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
 *	| R-001 | 2012-10-01 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


/**************************
 * BEGIN: REMARK: WARNING *
 **************************
 * Do not change the following defines
 */
#ifndef ACS_NCLIB_TRACE_CONTROL_FROM_FILE_DISABLED
#	ifndef ACS_NCLIB_HAS_TRACE
#		define ACS_NCLIB_HAS_TRACE
#	endif

#	ifndef ACS_NCLIB_TRACE_HAS_FUNCTION_TRACE
#		define ACS_NCLIB_TRACE_HAS_FUNCTION_TRACE
#	endif

#	ifndef ACS_NCLIB_TRACE_USE_PRETTY_FUNCTION
#		define ACS_NCLIB_TRACE_USE_PRETTY_FUNCTION
#	endif

#	ifndef ACS_NCLIB_TRACE_USE_SOURCE_INFO
#		define ACS_NCLIB_TRACE_USE_SOURCE_INFO
#	endif
#endif // ACS_NCLIB_TRACE_CONTROL_FROM_FILE_DISABLED
/************************
 * END: REMARK: WARNING *
 ************************/


#ifdef ACS_NCLIB_HAS_TRACE

#include <stdarg.h>
#include <stdint.h>

#include "ACS_TRA_trace.h"

#include "acs_nclib_macros.h"


#	ifndef ACS_NCLIB_TRACE_MESSAGE_SIZE_MAX
#		define ACS_NCLIB_TRACE_MESSAGE_SIZE_MAX 8192
#	endif

#	ifndef ACS_NCLIB_TRACE_DEFAULT_DUMP_LINE_LENGTH
#		define ACS_NCLIB_TRACE_DEFAULT_DUMP_LINE_LENGTH 16
#	endif


#	define ACS_NCLIB_TRACE_DEFINE(tag) namespace { acs_nclib_tracer __acs_nclib_tracer_object__(ACS_NCLIB_STRINGIZE(tag)); }
#	define ACS_NCLIB_TRACE_MESSAGE_IMPL(...) __acs_nclib_tracer_object__.trace(__VA_ARGS__)
#	define ACS_NCLIB_TRACE_MESSAGE_IMPL_SOURCE(file, line, ...) __acs_nclib_tracer_object__.trace_source(file, line, __VA_ARGS__)

#	ifdef ACS_NCLIB_TRACE_USE_SOURCE_INFO
#		define ACS_NCLIB_TRACE_MESSAGE(...) ACS_NCLIB_TRACE_MESSAGE_IMPL_SOURCE(__FILE__, __LINE__, __VA_ARGS__)
#	else
#		define ACS_NCLIB_TRACE_MESSAGE(...) ACS_NCLIB_TRACE_MESSAGE_IMPL(__VA_ARGS__)
#	endif

#	define ACS_NCLIB_TRACE_DUMP(buffer, size, ...) __acs_nclib_tracer_object__.dump(buffer, size, __VA_ARGS__)

#	ifdef ACS_NCLIB_TRACE_HAS_FUNCTION_TRACE
#		define ACS_NCLIB_TRACE_FUNCTION_IMPL(...) acs_nclib_function_tracer __acs_nclib_enter_function__(__acs_nclib_tracer_object__, __VA_ARGS__)
#	else
#		define ACS_NCLIB_TRACE_FUNCTION_IMPL(...)
#	endif

#	ifdef ACS_NCLIB_TRACE_USE_PRETTY_FUNCTION
#		ifdef ACS_NCLIB_TRACE_USE_SOURCE_INFO
#			define ACS_NCLIB_TRACE_FUNCTION ACS_NCLIB_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#		else
#			define ACS_NCLIB_TRACE_FUNCTION ACS_NCLIB_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__)
#		endif
#	else
#		ifdef ACS_NCLIB_TRACE_USE_SOURCE_INFO
#			define ACS_NCLIB_TRACE_FUNCTION ACS_NCLIB_TRACE_FUNCTION_IMPL(__func__, __FILE__, __LINE__)
#		else
#			define ACS_NCLIB_TRACE_FUNCTION ACS_NCLIB_TRACE_FUNCTION_IMPL(__func__)
#		endif
#	endif


#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_nclib_tracer

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
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	inline int trace_source (const char * file, int line, const char * format, ...) __attribute__ ((format (printf, 4, 5))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, line, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (const char * file, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, -1, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (int line, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
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

	void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = ACS_NCLIB_TRACE_DEFAULT_DUMP_LINE_LENGTH);

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
#define __CLASS_NAME__ acs_nclib_function_tracer

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ (acs_nclib_tracer & tracer, const char * function_name);
	__CLASS_NAME__ (acs_nclib_tracer & tracer, const char * function_name, const char * file_name, int line = -1);

private:
	__CLASS_NAME__ (const __CLASS_NAME__ &);


	//============//
	// Destructor //
	//============//
public:
	~__CLASS_NAME__ ();


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ &);


	//========//
	// Fields //
	//========//
private:
	acs_nclib_tracer & _tracer;
	const char * _function_name;
};

#else // ACS_NCLIB_HAS_TRACE

#	define ACS_NCLIB_TRACE_DEFINE(tag)
#	define ACS_NCLIB_TRACE_MESSAGE(...)
#	define ACS_NCLIB_TRACE_DUMP(buffer, size, ...)
#	define ACS_NCLIB_TRACE_FUNCTION

#endif // ACS_NCLIB_HAS_TRACE

#endif // HEADER_GUARD_FILE__acs_nclib_trace
