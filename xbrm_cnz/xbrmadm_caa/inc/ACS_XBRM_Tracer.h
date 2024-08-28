/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/ /**
 *
 * @file ACS_XBRM_Tracer.cpp
 *
 * @brief
 * ACS_XBRM_Tracer Class for TRACE
 *
 * @details
 * Header file of LOG Traces for XBRM application
 *
 * @author ZPAGSAI
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-05-19  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

#ifndef HEADER_GUARD_FILE__ACS_XBRM_Tracer
#define HEADER_GUARD_FILE__ACS_XBRM_Tracer ACS_XBRM_Tracer.h

#include <unistd.h>
#include <inttypes.h>
#include <stdarg.h>

#include "ACS_TRA_trace.h"


#ifndef ACS_XBRM_ARRAY_SIZE
#	define ACS_XBRM_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif

#ifndef ACS_XBRM_STRINGIZER
#	define ACS_XBRM_STRINGIZER(s) #s
#	define ACS_XBRM_STRINGIZE(s) ACS_XBRM_STRINGIZER(s)
#endif


#ifdef ACS_XBRM_API_HAS_TRACE

#	ifndef ACS_XBRM_TRACE_MESSAGE_SIZE_MAX
#		define ACS_XBRM_TRACE_MESSAGE_SIZE_MAX 8192
#	endif

#	ifndef ACS_XBRM_TRACE_DEFAULT_DUMP_LINE_LENGTH
#		define ACS_XBRM_TRACE_DEFAULT_DUMP_LINE_LENGTH 16
#		define ACS_XBRM_TRACE_DEFAULT_DUMP_SIZE 128
#	endif

#	define ACS_XBRM_TRACE_DEFINE(tag) namespace { ACS_XBRM_Tracer __ACS_XBRM_Tracer_object__(ACS_XBRM_STRINGIZE(tag)); }

#	define ACS_XBRM_TRACE_MESSAGE_IMPL(...) __ACS_XBRM_Tracer_object__.trace(__VA_ARGS__)
#	define ACS_XBRM_TRACE_MESSAGE_SOURCE_IMPL(file, line, function, ...) __ACS_XBRM_Tracer_object__.trace_source(file, line, function, __VA_ARGS__)

#	ifdef ACS_XBRM_TRACE_USE_SOURCE_INFO
#		define ACS_XBRM_TRACE_MESSAGE(...) ACS_XBRM_TRACE_MESSAGE_SOURCE_IMPL(__FILE__, __LINE__,__func__, __VA_ARGS__)
#	else
#		define ACS_XBRM_TRACE_MESSAGE(...) ACS_XBRM_TRACE_MESSAGE_IMPL(__VA_ARGS__)
#	endif

#	define ACS_XBRM_TRACE_DUMP(buffer, size, ...) __ACS_XBRM_Tracer_object__.dump(buffer, size, __VA_ARGS__)

#	ifdef ACS_XBRM_TRACE_HAS_FUNCTION_TRACE
#		define ACS_XBRM_TRACE_FUNCTION_IMPL(...) ACS_XBRM_FUNCTION_TRACER __ACS_XBRM_ENTER_FUNCTION__(__ACS_XBRM_Tracer_object__, __VA_ARGS__)
#		define ACS_XBRM_TRACE_FUNCTION_SOURCE_IMPL(...) ACS_XBRM_FUNCTION_TRACER __ACS_XBRM_ENTER_FUNCTION__(__ACS_XBRM_Tracer_object__, __VA_ARGS__)
#	else
#		define ACS_XBRM_TRACE_FUNCTION_IMPL(...)
#		define ACS_XBRM_TRACE_FUNCTION_SOURCE_IMPL(...)
#	endif

#	ifdef ACS_XBRM_TRACE_USE_PRETTY_FUNCTION
#		ifdef ACS_XBRM_TRACE_USE_SOURCE_INFO
#			define ACS_XBRM_TRACE_FUNCTION ACS_XBRM_TRACE_FUNCTION_SOURCE_IMPL(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#		else
#			define ACS_XBRM_TRACE_FUNCTION ACS_XBRM_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__)
#		endif
#	else
#		ifdef ACS_XBRM_TRACE_USE_SOURCE_INFO
#			define ACS_XBRM_TRACE_FUNCTION ACS_XBRM_TRACE_FUNCTION_SOURCE_IMPL(__func__, __FILE__, __LINE__)
#		else
#			define ACS_XBRM_TRACE_FUNCTION ACS_XBRM_TRACE_FUNCTION_IMPL(__func__)
#		endif
#	endif


#undef __CLASS_NAME__
#define __CLASS_NAME__ ACS_XBRM_Tracer

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
	inline int trace_source (const char * file, int line, const char * function, const char * format, ...) __attribute__ ((format (printf, 5, 6))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, line, function, format, argp);
		::va_end(argp);
		return call_result;
	}

	int trace (const char * format, ...) __attribute__ ((format (printf, 2, 3))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(0, -1, 0, format, argp);
		::va_end(argp);
		return call_result;
	}

	void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = ACS_XBRM_TRACE_DEFAULT_DUMP_LINE_LENGTH);

private:
	int vtrace (const char * file, int line, const char * function, const char * format, va_list ap);


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
#define __CLASS_NAME__ ACS_XBRM_FUNCTION_TRACER

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ (ACS_XBRM_Tracer & tracer, const char * function_name);
	__CLASS_NAME__ (ACS_XBRM_Tracer & tracer, const char * function_name, const char * file_name, int line = -1);

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
	ACS_XBRM_Tracer & _tracer;
	const char * _function_name;
	bool _has_source_info;
	const char * _file_name;
};

#else // !ACS_XBRM_API_HAS_TRACE

#	define ACS_XBRM_TRACE_DEFINE(tag)
#	define ACS_XBRM_TRACE_MESSAGE(...)
#	define ACS_XBRM_TRACE_DUMP(buffer, size, ...)
#	define ACS_XBRM_TRACE_FUNCTION

#endif // ACS_XBRM_API_HAS_TRACE

#endif // HEADER_GUARD_FILE__ACS_XBRM_Tracer
