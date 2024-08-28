#ifndef HEADER_GUARD_CLASS__acs_hcs_logger
#define HEADER_GUARD_CLASS__acs_hcs_logger acs_hcs_logger

/** @file acs_hcs_logger.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2013-05-10
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
 *	| R-001 | 2013-05-10 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <stdarg.h>

#include "ACS_TRA_Logging.h"


/*
 * Logging macros
 */
#if !defined (ACS_HC_LOGGER_CLASS)
#	define ACS_HC_LOGGER_CLASS acs_hcs_logger

#	define ACS_HC_LOGGER_METHOD logf
#	define ACS_HC_LOGGER_ERRNO_METHOD logf_errno
#	define ACS_HC_SYSLOGGER_METHOD syslogf
#	define ACS_HC_SYSLOGGER_ERRNO_METHOD syslogf_errno

#	define ACS_HC_LOGGER_METHOD_SOURCE logf_source
#	define ACS_HC_LOGGER_ERRNO_METHOD_SOURCE logf_errno_source
#	define ACS_HC_SYSLOGGER_METHOD_SOURCE syslogf_source
#	define ACS_HC_SYSLOGGER_ERRNO_METHOD_SOURCE syslogf_errno_source
#endif

#if !defined (ACS_HC_LOG_IMPL)
#	define ACS_HC_LOG_IMPL(level, ...) ACS_HC_LOGGER_CLASS::ACS_HC_LOGGER_METHOD(level, __VA_ARGS__)
#	define ACS_HC_LOG_ERRNO_IMPL(sys_errno, level, ...) \
	ACS_HC_LOGGER_CLASS::ACS_HC_LOGGER_ERRNO_METHOD(sys_errno, level, __VA_ARGS__)
#	define ACS_HC_SYSLOG_IMPL(syslog_priority, level, ...) \
	ACS_HC_LOGGER_CLASS::ACS_HC_SYSLOGGER_METHOD(syslog_priority, level, __VA_ARGS__)
#	define ACS_HC_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, ...) \
	ACS_HC_LOGGER_CLASS::ACS_HC_SYSLOGGER_ERRNO_METHOD(sys_errno, syslog_priority, level, __VA_ARGS__)

#	define ACS_HC_LOG_SOURCE_IMPL(level, source, line, ...) \
	ACS_HC_LOGGER_CLASS::ACS_HC_LOGGER_METHOD_SOURCE(level, source, line, __VA_ARGS__)
#	define ACS_HC_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, source, line, ...) \
	ACS_HC_LOGGER_CLASS::ACS_HC_LOGGER_ERRNO_METHOD_SOURCE(sys_errno, level, source, line, __VA_ARGS__)
#	define ACS_HC_SYSLOG_SOURCE_IMPL(syslog_priority, level, source, line, ...) \
	ACS_HC_LOGGER_CLASS::ACS_HC_SYSLOGGER_METHOD_SOURCE(syslog_priority, level, source, line, __VA_ARGS__)
#	define ACS_HC_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, source, line, ...) \
	ACS_HC_LOGGER_CLASS::ACS_HC_SYSLOGGER_ERRNO_METHOD_SOURCE(sys_errno, syslog_priority, level, source, line, __VA_ARGS__)
#endif

inline int ACS_HC_LOG_NO_LOGGING () { return 0; }

#ifdef ACS_HC_HAS_LOGGING
#	ifdef ACS_HC_LOGGING_HAS_SOURCE_INFO
#		ifdef ACS_HC_LOGGING_USE_PRETTY_FUNCTION
#			define ACS_HC_LOG(level, ...) ACS_HC_LOG_SOURCE_IMPL(level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define ACS_HC_LOG_ERRNO(sys_errno, level, ...) ACS_HC_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define ACS_HC_SYSLOG(syslog_priority, level, ...) ACS_HC_SYSLOG_SOURCE_IMPL(syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define ACS_HC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_HC_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#		else
#			define ACS_HC_LOG(level, ...) ACS_HC_LOG_SOURCE_IMPL(level, __func__, __LINE__, __VA_ARGS__)
#			define ACS_HC_LOG_ERRNO(sys_errno, level, ...) ACS_HC_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __func__, __LINE__, __VA_ARGS__)
#			define ACS_HC_SYSLOG(syslog_priority, level, ...) ACS_HC_SYSLOG_SOURCE_IMPL(syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#			define ACS_HC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_HC_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#		endif // ACS_HC_LOGGING_USE_PRETTY_FUNCTION
#	else
#		define ACS_HC_LOG(level, ...) ACS_HC_LOG_IMPL(level, __VA_ARGS__)
#		define ACS_HC_LOG_ERRNO(sys_errno, level, ...) ACS_HC_LOG_ERRNO_IMPL(sys_errno, level, __VA_ARGS__)
#		define ACS_HC_SYSLOG(syslog_priority, level, ...) ACS_HC_SYSLOG_IMPL(syslog_priority, level, __VA_ARGS__)
#		define ACS_HC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_HC_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, __VA_ARGS__)
#	endif //ACS_HC_LOGGING_HAS_SOURCE_INFO
#else
#	define ACS_HC_LOG(level, ...) ACS_HC_LOG_NO_LOGGING()
#	define ACS_HC_LOG_ERRNO(sys_errno, level, ...) ACS_HC_LOG_NO_LOGGING()
#	define ACS_HC_SYSLOG(syslog_priority, level, ...) ACS_HC_LOG_NO_LOGGING()
#	define ACS_HC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) ACS_HC_LOG_NO_LOGGING()
#endif //ACS_HC_HAS_LOGGING


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_hcs_logger


/** @class acs_hcs_logger acs_hcs_logger.h
 *	@brief acs_hcs_logger class
 *	@author xnicmut (Nicola Muto)
 *	@date 2013-05-10
 *
 *	acs_hcs_logger Class detailed description
 */
class __CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
private:
	enum {
		SYSLOG_DISABLED_MASK	=	99999,
		ERRNO_DISABLED_MASK		=	-99999
	};


	//==============//
	// Constructors //
	//==============//
private:
	/** @brief acs_hcs_logger Default constructor
	 *
	 *	acs_hcs_logger Constructor detailed description
	 *
	 *	@param[in] logging_level Description
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ ();

	/** @brief acs_hcs_logger Copy constructor
	 *
	 *	acs_hcs_logger Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_hcs_logger Destructor
	 *
	 *	acs_hcs_logger Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	static ACS_TRA_LogResult open (const char * appender_name);
	static void close ();

	static ACS_TRA_LogResult syslogf_errno (int sys_errno, int syslog_priority,
			ACS_TRA_LogLevel level, const char * format, ...);
	static ACS_TRA_LogResult syslogf_errno (int sys_errno, int syslog_priority,
			const char * format, ...);
	static ACS_TRA_LogResult syslogf_errno_source (int sys_errno, int syslog_priority,
			ACS_TRA_LogLevel level, const char * source_func_name, int source_line, const char * format, ...);
	static ACS_TRA_LogResult syslogf_errno_source (int sys_errno, int syslog_priority,
			const char * source_func_name, int source_line, const char * format, ...);

	static ACS_TRA_LogResult syslogf (int syslog_priority, ACS_TRA_LogLevel level, const char * format, ...);
	static ACS_TRA_LogResult syslogf (int syslog_priority, const char * format, ...);
	static ACS_TRA_LogResult syslogf_source (int syslog_priority, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * format, ...);
	static ACS_TRA_LogResult syslogf_source (int syslog_priority, const char * source_func_name,
			int source_line, const char * format, ...);

	static ACS_TRA_LogResult logf (ACS_TRA_LogLevel level, const char * format, ...);
	static ACS_TRA_LogResult logf (const char * format, ...);
	static ACS_TRA_LogResult logf_source (ACS_TRA_LogLevel level, const char * source_func_name,
			int source_line, const char * format, ...);
	static ACS_TRA_LogResult logf_source (const char * source_func_name, int source_line, const char * format, ...);

	static ACS_TRA_LogResult logf_errno_source (int sys_errno, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * format, ...);
	static ACS_TRA_LogResult logf_errno_source (int sys_errno, const char * source_func_name,
			int source_line, const char * format, ...);
	static ACS_TRA_LogResult logf_errno (int sys_errno, ACS_TRA_LogLevel level, const char * format, ...);
	static ACS_TRA_LogResult logf_errno (int sys_errno, const char * format, ...);

	static inline ACS_TRA_LogResult log (const char * message) { return logf(_logging_level, message); }
	static inline ACS_TRA_LogResult log (ACS_TRA_LogLevel level, const char * message) { return logf(level, message); }
	static inline ACS_TRA_LogResult log (const char * source_func_name, int source_line, const char * message) {
		return logf_source(_logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult log (ACS_TRA_LogLevel level, const char * source_func_name,
			int source_line, const char * message) {
		return logf_source(level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogResult log_errno (int sys_errno, const char * message) {
		return logf_errno(sys_errno, _logging_level, message);
	}
	static inline ACS_TRA_LogResult log_errno (int sys_errno, ACS_TRA_LogLevel level, const char * message) {
		return logf_errno(sys_errno, level, message);
	}
	static inline ACS_TRA_LogResult log_errno (int sys_errno, const char * source_func_name, int source_line,
			const char * message) {
		return logf_errno_source(sys_errno, _logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult log_errno (int sys_errno, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * message) {
		return logf_errno_source(sys_errno, level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogResult syslog (int syslog_priority, const char * message) {
		return syslogf(syslog_priority, _logging_level, message);
	}
	static inline ACS_TRA_LogResult syslog (int syslog_priority, ACS_TRA_LogLevel level, const char * message) {
		return syslogf(syslog_priority, level, message);
	}
	static inline ACS_TRA_LogResult syslog (int syslog_priority, const char * source_func_name, int source_line,
			const char * message) {
		return syslogf_source(syslog_priority, _logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult syslog (int syslog_priority, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * message) {
		return syslogf_source(syslog_priority, level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority, const char * message) {
		return syslogf_errno(sys_errno, syslog_priority, _logging_level, message);
	}
	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * message) {
		return syslogf_errno(sys_errno, syslog_priority, level, message);
	}
	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority,
			const char * source_func_name, int source_line, const char * message) {
		return syslogf_errno_source(sys_errno, syslog_priority, _logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * message) {
		return syslogf_errno_source(sys_errno, syslog_priority, level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogLevel logging_level () { return _logging_level; }
	static inline void logging_level (ACS_TRA_LogLevel level) { _logging_level = level; }


private:
	static ACS_TRA_LogResult vlogf (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * format, va_list ap);
	static ACS_TRA_LogResult vlogf_source (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * format, va_list ap);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	static ACS_TRA_LogLevel _logging_level;
};

#endif // HEADER_GUARD_CLASS__acs_hcs_logger
