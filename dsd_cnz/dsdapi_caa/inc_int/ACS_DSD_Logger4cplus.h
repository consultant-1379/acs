#ifndef HEADER_GUARD_CLASS__ACS_DSD_Logger4cplus
#define HEADER_GUARD_CLASS__ACS_DSD_Logger4cplus ACS_DSD_Logger4cplus

/** @file ACS_DSD_Logger4cplus.h
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

#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Logger4cplus

/** @class ACS_DSD_Logger4cplus ACS_DSD_Logger4cplus.h
 *	@brief ACS_DSD_Logger4cplus class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_Logger4cplus Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_Logger4cplus Default constructor
	 *
	 *	ACS_DSD_Logger4cplus Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : _log_obj() {}

protected:
	/** @brief ACS_DSD_Logger4cplus Copy constructor
	 *
	 *	ACS_DSD_Logger4cplus Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const ACS_DSD_Logger4cplus & rhs) : _log_obj(rhs._log_obj) {}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Logger4cplus Destructor
	 *
	 *	ACS_DSD_Logger4cplus Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	static inline void configure (const char * config_path) { log4cplus::PropertyConfigurator::doConfigure(config_path); }

	inline void open (const char * appender_name) { _log_obj = log4cplus::Logger::getInstance(appender_name); }

	inline void close () { log4cplus::Logger::shutdown(); }

	inline int logf (const char * format, ...) {
		va_list argp;
		va_start(argp, format);
		const int call_result = vlogf(format, argp,
#ifdef ACS_DSD_LOG_INCLUDE_HEADER_INFO
				true
#else
				false
#endif
			);
		va_end(argp);
		return call_result;
	}

	inline int log (const char * message, bool include_header = false) { return logfh(include_header, message); }

private:
	inline int logfh (bool include_header, const char * format, ...) {
		va_list argp;
		va_start(argp, format);
		const int call_result = vlogf(format, argp, include_header);
		va_end(argp);
		return call_result;
	}

	int vlogf (const char * format, va_list ap, bool include_header = true);

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ &) {
		//(&rhs != this) && (_log_obj = rhs._log_obj);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	log4cplus::Logger _log_obj;
};


#endif // HEADER_GUARD_CLASS__ACS_DSD_Logger4cplus
