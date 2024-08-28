#ifndef HEADER_GUARD_FILE__acs_apbm_programmacros
#define HEADER_GUARD_FILE__acs_apbm_programmacros acs_apbm_programmacros.h

/** @file acs_apbm_programmacros.h
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
 *	| R-001 | 2011-07-19 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

/*
 * Logging macros
 */
#define ACS_APBM_LOGGER_CLASS acs_apbm_logger

#define ACS_APBM_LOGGER_METHOD logf
#define ACS_APBM_LOGGER_ERRNO_METHOD logf_errno
#define ACS_APBM_SYSLOGGER_METHOD syslogf
#define ACS_APBM_SYSLOGGER_ERRNO_METHOD syslogf_errno

#define ACS_APBM_LOGGER_METHOD_SOURCE logf_source
#define ACS_APBM_LOGGER_ERRNO_METHOD_SOURCE logf_errno_source
#define ACS_APBM_SYSLOGGER_METHOD_SOURCE syslogf_source
#define ACS_APBM_SYSLOGGER_ERRNO_METHOD_SOURCE syslogf_errno_source


#define ACS_APBM_LOG_IMPL(level, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_LOGGER_METHOD(level, __VA_ARGS__)
#define ACS_APBM_LOG_ERRNO_IMPL(sys_errno, level, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_LOGGER_ERRNO_METHOD(sys_errno, level, __VA_ARGS__)
#define ACS_APBM_SYSLOG_IMPL(syslog_priority, level, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_SYSLOGGER_METHOD(syslog_priority, level, __VA_ARGS__)
#define ACS_APBM_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_SYSLOGGER_ERRNO_METHOD(sys_errno, syslog_priority, level, __VA_ARGS__)

#define ACS_APBM_LOG_SOURCE_IMPL(level, source, line, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_LOGGER_METHOD_SOURCE(level, source, line, __VA_ARGS__)
#define ACS_APBM_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, source, line, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_LOGGER_ERRNO_METHOD_SOURCE(sys_errno, level, source, line, __VA_ARGS__)
#define ACS_APBM_SYSLOG_SOURCE_IMPL(syslog_priority, level, source, line, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_SYSLOGGER_METHOD_SOURCE(syslog_priority, level, source, line, __VA_ARGS__)
#define ACS_APBM_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, source, line, ...) ACS_APBM_LOGGER_CLASS::ACS_APBM_SYSLOGGER_ERRNO_METHOD_SOURCE(sys_errno, syslog_priority, level, source, line, __VA_ARGS__)

#define UNUSED(expr) do { (void)(expr); } while (0)

#ifdef ACS_APBM_LOGGING_ENABLED
#	ifdef ACS_APBM_LOGGING_HAS_SOURCE_INFO
#		ifdef ACS_APBM_LOGGING_USE_PRETTY_FUNCTION
#			define ACS_APBM_LOG(level, ...) ACS_APBM_LOG_SOURCE_IMPL(level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define ACS_APBM_LOG_ERRNO(sys_errno, level, ...) ACS_APBM_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define ACS_APBM_SYSLOG(syslog_priority, level, ...) ACS_APBM_SYSLOG_SOURCE_IMPL(syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define ACS_APBM_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_APBM_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#		else
#			define ACS_APBM_LOG(level, ...) ACS_APBM_LOG_SOURCE_IMPL(level, __func__, __LINE__, __VA_ARGS__)
#			define ACS_APBM_LOG_ERRNO(sys_errno, level, ...) ACS_APBM_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __func__, __LINE__, __VA_ARGS__)
#			define ACS_APBM_SYSLOG(syslog_priority, level, ...) ACS_APBM_SYSLOG_SOURCE_IMPL(syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#			define ACS_APBM_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_APBM_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#		endif // ACS_APBM_LOGGING_USE_PRETTY_FUNCTION
#	else
#		define ACS_APBM_LOG(level, ...) ACS_APBM_LOG_IMPL(level, __VA_ARGS__)
#		define ACS_APBM_LOG_ERRNO(sys_errno, level, ...) ACS_APBM_LOG_ERRNO_IMPL(sys_errno, level, __VA_ARGS__)
#		define ACS_APBM_SYSLOG(syslog_priority, level, ...) ACS_APBM_SYSLOG_IMPL(syslog_priority, level, __VA_ARGS__)
#		define ACS_APBM_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_APBM_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, __VA_ARGS__)
#	endif //ACS_APBM_LOGGING_HAS_SOURCE_INFO
#else
#	define ACS_APBM_LOG(level, ...) ACS_APBM_LOG_NO_LOGGING()
#	define ACS_APBM_LOG_ERRNO(sys_errno, level, ...) ACS_APBM_LOG_NO_LOGGING()
#	define ACS_APBM_SYSLOG(syslog_priority, level, ...) ACS_APBM_LOG_NO_LOGGING()
#	define ACS_APBM_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_APBM_LOG_NO_LOGGING()
#endif //ACS_APBM_LOGGING_ENABLED


inline int ACS_APBM_LOG_NO_LOGGING () { return 0; }


#ifndef ACS_APBM_CRITICAL_SECTION_GUARD_BEGIN
#	define	ACS_APBM_CRITICAL_SECTION_GUARD_BEGIN(sync) do { ACE_Guard<ACE_Recursive_Thread_Mutex> guard(sync, true)
#	define	ACS_APBM_CRITICAL_SECTION_GUARD_END()	} while (0)
#endif

#endif // HEADER_GUARD_FILE__acs_apbm_programmacros
