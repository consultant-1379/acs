#ifndef HEADER_GUARD_CLASS__ACS_DSD_LoggerTra
#define HEADER_GUARD_CLASS__ACS_DSD_LoggerTra ACS_DSD_LoggerTra

/** @file ACS_DSD_LoggerTra.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
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
 *	| P1.0.1 | 2010-11-09 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <stdarg.h>

#include "ACS_DSD_Macros.h"

#include "ACS_TRA_Logging.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_LoggerTra

/** @class ACS_DSD_TraLogger ACS_DSD_Logger.h
 *	@brief ACS_DSD_TraLogger class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_TraLogger Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_TraLogger Default constructor
	 *
	 *	ACS_DSD_TraLogger Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : _log_obj(), _logging_level(LOG_LEVEL_INFO) {}

protected:
	/** @brief ACS_DSD_TraLogger Copy constructor
	 *
	 *	ACS_DSD_TraLogger Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : _log_obj(), _logging_level(rhs._logging_level) {}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_TraLogger Destructor
	 *
	 *	ACS_DSD_TraLogger Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	inline ACS_TRA_LogResult open (const char * appender_name) { return _log_obj.Open(appender_name); }

	inline ACS_TRA_LogResult logf (bool include_header, ACS_TRA_LogLevel level, const char * format, ...) {
		va_list argp;
		va_start(argp, format);
		const ACS_TRA_LogResult call_result = vlogf(level, format, argp, include_header);
		va_end(argp);
		return call_result;
	}

	inline ACS_TRA_LogResult logf (ACS_TRA_LogLevel level, const char * format, ...) {
		va_list argp;
		va_start(argp, format);
		const ACS_TRA_LogResult call_result = vlogf(level, format, argp,
#ifdef ACS_DSD_LOG_INCLUDE_HEADER_INFO
				true
#else
				false
#endif
			);
		va_end(argp);
		return call_result;
	}

	inline ACS_TRA_LogResult logf (const char * format, ...) {
		va_list argp;
		va_start(argp, format);
		const ACS_TRA_LogResult call_result = vlogf(_logging_level, format, argp,
#ifdef ACS_DSD_LOG_INCLUDE_HEADER_INFO
				true
#else
				false
#endif
			);
		va_end(argp);
		return call_result;
	}

	inline ACS_TRA_LogResult log (const char * message, bool include_header = false) { return logf(include_header, _logging_level, message); }
	inline ACS_TRA_LogResult log (ACS_TRA_LogLevel level, const char * message, bool include_header = false) { return logf(include_header, level, message); }

	inline ACS_TRA_LogLevel logging_level () const { return _logging_level; }
	inline void logging_level (ACS_TRA_LogLevel level) { _logging_level = level; }

private:
	ACS_TRA_LogResult vlogf (ACS_TRA_LogLevel level, const char * format, va_list ap, bool include_header = true);

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		(&rhs != this) && (_logging_level = rhs._logging_level);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	ACS_TRA_Logging _log_obj;
	ACS_TRA_LogLevel _logging_level;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_LoggerTra
