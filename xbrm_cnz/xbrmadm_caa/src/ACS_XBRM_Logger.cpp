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
 * @file ACS_XBRM_Logger.cpp
 *
 * @brief
 * ACS_XBRM_Logger Class for TRA Logging 
 *
 * @details
 * Implementation of TRA logging for XBRM Service
 *
 * @author ZPAGSAI
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-05-01  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

#include <string.h>
#include <syslog.h>
#include <new>

#include "ACS_XBRM_Logger.h"

namespace {
	char g_tra_logging_object_buffer [sizeof(ACS_TRA_Logging)];
	ACS_TRA_Logging * g_tra_logger_ptr = 0;
}


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_XBRM_Logger


ACS_TRA_LogLevel __CLASS_NAME__::_logging_level = LOG_LEVEL_INFO;


ACS_TRA_LogResult __CLASS_NAME__::open (const char * appender_name) {
	if (g_tra_logger_ptr) return TRA_LOG_OK;

	ACS_TRA_Logging * logger_ptr = new (g_tra_logging_object_buffer) ACS_TRA_Logging;

	const ACS_TRA_LogResult return_code = logger_ptr->Open(appender_name);

	if (return_code != TRA_LOG_OK) {
		logger_ptr->~ACS_TRA_Logging();
		return return_code;
	}

	g_tra_logger_ptr = logger_ptr;

	return TRA_LOG_OK;
}

void __CLASS_NAME__::close () {
	if (g_tra_logger_ptr) g_tra_logger_ptr->~ACS_TRA_Logging();
	g_tra_logger_ptr = 0;
}

ACS_TRA_LogResult __CLASS_NAME__::syslogf_errno (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
		const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(sys_errno, syslog_priority, level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::syslogf_errno (int sys_errno, int syslog_priority, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(sys_errno, syslog_priority, _logging_level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::syslogf_errno_source (int sys_errno, int syslog_priority,
		ACS_TRA_LogLevel level, const char * source_func_name, int source_line, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(sys_errno, syslog_priority, level, source_func_name,
			source_line, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::syslogf_errno_source (int sys_errno, int syslog_priority,
		const char * source_func_name, int source_line, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(sys_errno, syslog_priority, _logging_level,
			source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

ACS_TRA_LogResult __CLASS_NAME__::syslogf (int syslog_priority, ACS_TRA_LogLevel level, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(ERRNO_DISABLED_MASK, syslog_priority, level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::syslogf (int syslog_priority, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(ERRNO_DISABLED_MASK, syslog_priority, _logging_level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::syslogf_source (int syslog_priority, ACS_TRA_LogLevel level,
		const char * source_func_name, int source_line, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(ERRNO_DISABLED_MASK, syslog_priority, level,
			source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::syslogf_source (int syslog_priority, const char * source_func_name,
		int source_line, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(ERRNO_DISABLED_MASK, syslog_priority, _logging_level,
			source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

ACS_TRA_LogResult __CLASS_NAME__::logf (ACS_TRA_LogLevel level, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK, level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::logf (const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK, _logging_level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::logf_source (ACS_TRA_LogLevel level, const char * source_func_name,
		int source_line, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK, level,
			source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::logf_source (const char * source_func_name, int source_line,
		const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK,
			_logging_level, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

ACS_TRA_LogResult __CLASS_NAME__::logf_errno (int sys_errno, ACS_TRA_LogLevel level, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(sys_errno, SYSLOG_DISABLED_MASK, level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::logf_errno (int sys_errno, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf(sys_errno, SYSLOG_DISABLED_MASK, _logging_level, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::logf_errno_source (int sys_errno, ACS_TRA_LogLevel level,
		const char * source_func_name, int source_line, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(sys_errno, SYSLOG_DISABLED_MASK, level,
			source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}
ACS_TRA_LogResult __CLASS_NAME__::logf_errno_source (int sys_errno, const char * source_func_name,
		int source_line, const char * format, ...) {
	va_list argp;
	va_start(argp, format);
	const ACS_TRA_LogResult call_result = vlogf_source(sys_errno, SYSLOG_DISABLED_MASK, _logging_level,
			source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

ACS_TRA_LogResult __CLASS_NAME__::vlogf (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
		const char * format, va_list ap) {
	if (!g_tra_logger_ptr && (syslog_priority == SYSLOG_DISABLED_MASK)) return TRA_LOG_OK;

	char buffer[8 * 1024];
	int char_count = 0;

	char_count = ::vsnprintf(buffer, ACS_XBRM_ARRAY_SIZE(buffer), format, ap);

	if (sys_errno ^ ERRNO_DISABLED_MASK) { // Add system errno information to the log message
		char errno_buf[1024];
		::snprintf(buffer + char_count, ACS_XBRM_ARRAY_SIZE(buffer) - char_count, " [errno == %d, errno_text == '%s']",
				sys_errno, ::strerror_r(sys_errno, errno_buf, ACS_XBRM_ARRAY_SIZE(errno_buf)));
	}

	if (syslog_priority ^ SYSLOG_DISABLED_MASK) ::syslog(syslog_priority, buffer);

	const ACS_TRA_LogResult return_code = (g_tra_logger_ptr ? g_tra_logger_ptr->Write(buffer, level) : TRA_LOG_OK);

	return return_code;
}

ACS_TRA_LogResult __CLASS_NAME__::vlogf_source (int sys_errno, int syslog_priority,
		ACS_TRA_LogLevel level, const char * source_func_name, int source_line, const char * format, va_list ap) {
	if (!g_tra_logger_ptr && (syslog_priority == SYSLOG_DISABLED_MASK)) return TRA_LOG_OK;

	char buffer[10 * 1024];
	int char_count = 0;

	char_count = ::snprintf(buffer, ACS_XBRM_ARRAY_SIZE(buffer), "{%s@@%d} ", source_func_name, source_line);

	char_count += ::vsnprintf(buffer + char_count, ACS_XBRM_ARRAY_SIZE(buffer) - char_count, format, ap);

	if (sys_errno ^ ERRNO_DISABLED_MASK) { // Add system errno information to the log message
		char errno_buf[1024];
		::snprintf(buffer + char_count, ACS_XBRM_ARRAY_SIZE(buffer) - char_count, " [errno == %d, errno_text == '%s']",
				sys_errno, ::strerror_r(sys_errno, errno_buf, ACS_XBRM_ARRAY_SIZE(errno_buf)));
	}

	if (syslog_priority ^ SYSLOG_DISABLED_MASK) ::syslog(syslog_priority, buffer);

	const ACS_TRA_LogResult return_code = (g_tra_logger_ptr ? g_tra_logger_ptr->Write(buffer, level) : TRA_LOG_OK);

	return return_code;
}
