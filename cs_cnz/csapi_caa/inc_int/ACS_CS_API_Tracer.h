#ifndef ACS_CS_API_TRACER_H_
#define ACS_CS_API_TRACER_H_


#include <string> // Need to include this to make ACS_APGCC_Exceptions.H
#include <stdio.h>
#include <iostream>
#include <ace/Mutex.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include <stdarg.h>


#ifndef ACS_CS_API_TRACER_ARRAY_SIZE
#	define ACS_CS_API_TRACER_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif

#ifndef ACS_CS_API_TRACER_MESSAGE_SIZE_MAX
#	define ACS_CS_API_TRACER_MESSAGE_SIZE_MAX 8192
#endif

#ifndef ACS_CS_API_TRACER_DEFAULT_DUMP_LINE_LENGTH
#	define ACS_CS_API_TRACER_DEFAULT_DUMP_LINE_LENGTH 16
#endif

#ifndef ACS_CS_API_TRACER_STRINGIZE
#	define ACS_CS_API_TRACER_STRINGIZER(s) #s
#	define ACS_CS_API_TRACER_STRINGIZE(s) ACS_CS_API_TRACER_STRINGIZER(s)
#endif


#define ACS_CS_API_TRACER_DEFINE(tag) namespace { acs_cs_api_tracer __acs_cs_api_tracer_object__(ACS_CS_API_TRACER_STRINGIZE(tag)); }
#define ACS_CS_API_TRACER_MESSAGE_IMPL_SOURCE(file, line,function, ...) __acs_cs_api_tracer_object__.trace_source(file, line, function, __VA_ARGS__)
#define ACS_CS_API_TRACER_MESSAGE(...) ACS_CS_API_TRACER_MESSAGE_IMPL_SOURCE(__FILE__, __LINE__,__func__, __VA_ARGS__)

#define ACS_CS_API_TRACER_DUMP(buffer, size, ...) __mcs_adh_tracer_object__.dump(buffer, size, __VA_ARGS__)

#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_cs_api_tracer

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
		int call_result = vtrace(file, line,function, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (const char * file, int line, const char * format, ...) __attribute__ ((format (printf, 4, 5))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, line,0, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (const char * file, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, -1, 0,format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (int line, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(0, line, 0, format, argp);
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

	void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = ACS_CS_API_TRACER_DEFAULT_DUMP_LINE_LENGTH);

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



#endif /* ACS_CS_API_TRACER_H_ */
