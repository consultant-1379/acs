#ifndef HEADER_GUARD_FILE__ACS_DSD_MacrosUtils
#define HEADER_GUARD_FILE__ACS_DSD_MacrosUtils ACS_DSD_MacrosUtils.h

/** @file ACS_DSD_MacrosUtils.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-07
 *	@version 1.0.0
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.1 | 2010-10-07 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/** @brief Some useful macros.
 */

#define ACS_DSD_TEXT(s) s

#define ACS_DSD_STRINGIZER(s) #s
#define ACS_DSD_STRINGIZE(s) ACS_DSD_STRINGIZER(s)

#define ACS_DSD_CLASS_TRACER_NAMER(class_name) class_name ## Tracer
#define ACS_DSD_CLASS_TRACER_NAME(class_name) ACS_DSD_CLASS_TRACER_NAMER(class_name)

#ifdef ACS_DSD_TRACE_FUNCTION_HAS_SOURCE_INFO
#	define ACS_DSD_TRACE_FUNCTION_IMPL(tra_tracer, func_name) ACS_DSD_Trace ____ (tra_tracer, func_name, __FILE__, __LINE__)
#else
#	define ACS_DSD_TRACE_FUNCTION_IMPL(tra_tracer, func_name) ACS_DSD_Trace ____ (tra_tracer, func_name)
#endif

#define ACS_DSD_TRACE_MESSAGE_IMPL(tra_tracer, ...) tra_tracer.trace(__VA_ARGS__)

// By default ACS_DSD_TRACING_ACTIVE macro is not defined, so the tracing functionalities are turned off.
#ifdef ACS_DSD_TRACING_ACTIVE
#	ifndef ACS_DSD_HAS_TRACE
#		define ACS_DSD_HAS_TRACE
#	endif
#	define ACS_DSD_CLASS_TRACER_DECL(class_name) ACS_DSD_TraTracer ACS_DSD_CLASS_TRACER_NAME(class_name)
#	define ACS_DSD_TRACE_FUNCTION(tra_tracer, func_name) ACS_DSD_TRACE_FUNCTION_IMPL(tra_tracer, func_name)
#	define ACS_DSD_TRACE_MESSAGE(tra_tracer, ...) ACS_DSD_TRACE_MESSAGE_IMPL(tra_tracer, __VA_ARGS__)
#else
#	define ACS_DSD_CLASS_TRACER_DECL(class_name)
#	define ACS_DSD_TRACE_FUNCTION(tra_tracer, func_name)
#	define ACS_DSD_TRACE_MESSAGE(tra_tracer, ...)
#endif

//Logging macros
#ifdef ACS_DSD_LOG_USE_LOG4CPLUS
#	define ACS_DSD_Logger ACS_DSD_Logger4cplus
#	define ACS_DSD_LOG_IMPL(logger, level, ...) logger.logf(__VA_ARGS__)
#else
#	define ACS_DSD_Logger ACS_DSD_LoggerTra
#	define ACS_DSD_LOG_IMPL(logger, level, ...) logger.logf(level, __VA_ARGS__)
#endif

#ifdef ACS_DSD_LOGGING_ACTIVE
#	ifndef ACS_DSD_HAS_LOGGING
#		define ACS_DSD_HAS_LOGGING
#	endif
#	define ACS_DSD_LOG(logger, level, ...) ACS_DSD_LOG_IMPL(logger, level, ": " __VA_ARGS__)
#else
#	define ACS_DSD_LOG(logger, level, ...)
#endif


//Tracing configuration for DSD API only

#ifdef ACS_DSD_API_TRACE_FUNCTION_HAS_SOURCE_INFO
#	define ACS_DSD_API_TRACE_FUNCTION_IMPL(tra_tracer, func_name) ACS_DSD_Trace ____ (tra_tracer, func_name, __FILE__, __LINE__)
#else
#	define ACS_DSD_API_TRACE_FUNCTION_IMPL(tra_tracer, func_name) ACS_DSD_Trace ____ (tra_tracer, func_name)
#endif

#define ACS_DSD_API_TRACE_MESSAGE_IMPL(tra_tracer, ...) tra_tracer.trace(__VA_ARGS__)
#define ACS_DSD_API_TRACE_DUMP_IMPL(tra_tracer, buffer, size) tra_tracer.dump(buffer, size, acs_dsd::CONFIG_MESSAGE_DUMPING_BUFFER_SIZE, acs_dsd::CONFIG_MESSAGE_DUMPING_LINE_SIZE)

// By default ACS_DSD_API_TRACING_ACTIVE macro is defined, so the tracing functionalities in the DSD API are turned on.
#ifdef ACS_DSD_API_TRACING_ACTIVE
#	ifndef ACS_DSD_API_HAS_TRACE
#		define ACS_DSD_API_HAS_TRACE
#	endif
#	define ACS_DSD_API_CLASS_TRACER_DECL(class_name) ACS_DSD_TraTracer ACS_DSD_CLASS_TRACER_NAME(class_name)
#	ifdef ACS_DSD_API_TRACE_FUNCTION_USE_PRETTY_FUNCTION_NAME
#		define ACS_DSD_API_TRACE_FUNCTION(tra_tracer) ACS_DSD_API_TRACE_FUNCTION_IMPL(tra_tracer, __PRETTY_FUNCTION__)
#	else
#		define ACS_DSD_API_TRACE_FUNCTION(tra_tracer) ACS_DSD_API_TRACE_FUNCTION_IMPL(tra_tracer, __func__)
#	endif
#	define ACS_DSD_API_TRACE_MESSAGE(tra_tracer, ...) ACS_DSD_API_TRACE_MESSAGE_IMPL(tra_tracer, __VA_ARGS__)
#	define ACS_DSD_API_TRACE_DUMP(tra_tracer, buffer, size) ACS_DSD_API_TRACE_DUMP_IMPL(tra_tracer, buffer, size)

# define ACS_DSD_API_SET_ERROR_TRACE_RETURN(...) \
	do { \
		int err_return = set_error_info(__VA_ARGS__); \
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: error code == %d, error text == '%s'", last_error(), last_error_text()); \
		return err_return; \
	} while(0)

# define ACS_DSD_API_SET_ERROR_TRACE(var, ...) \
	do { \
		var = set_error_info(__VA_ARGS__); \
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: error code == %d, error text == '%s'", last_error(), last_error_text()); \
	} while(0)

#else
#	define ACS_DSD_API_CLASS_TRACER_DECL(class_name)
#	define ACS_DSD_API_TRACE_FUNCTION(tra_tracer)
#	define ACS_DSD_API_TRACE_MESSAGE(tra_tracer, ...)
#	define ACS_DSD_API_TRACE_DUMP(tra_tracer, buffer, size)
# define ACS_DSD_API_SET_ERROR_TRACE_RETURN(...) return set_error_info(__VA_ARGS__);
# define ACS_DSD_API_SET_ERROR_TRACE(var, ...) var = set_error_info(__VA_ARGS__);
#endif

#endif // HEADER_GUARD_FILE__ACS_DSD_MacrosUtils
