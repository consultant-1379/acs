#ifndef HEADER_GUARD_CLASS__acs_nclib_errorinfoprovider
#define HEADER_GUARD_CLASS__acs_nclib_errorinfoprovider acs_nclib_errorinfoprovider

/** @file acs_nclib_errorinfoprovider.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-10-03
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
 *	| R-001 | 2012-10-03 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <string.h>

#include "acs_nclib_constants.h"
#include "acs_nclib_macros.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_nclib_errorinfoprovider

class __CLASS_NAME__ {
	//=========//
	// Friends //
	//=========//
friend class acs_nclib_library;


	//===========//
	// Constants //
	//===========//
protected:
	enum {
		ACS_NCLIB_ERROR_TEXT_SIZE_MAX = 512
	};

	//==============//
	// Constructors //
	//==============//
protected:
	inline __CLASS_NAME__ () : _last_error_code(acs_nclib::ERR_NO_ERRORS), _last_error_text() {
		::strncpy(_last_error_text, acs_nclib::ERRTEXT_NO_ERRORS, ACS_NCLIB_ARRAY_SIZE(_last_error_text));
	}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	inline int last_error_code () const { return _last_error_code; }

	inline const char * last_error_text () const { return _last_error_text; }

protected:
	inline void set_error_info (int code) { set_error_info(code, 0); }
	void set_error_info (int code, const char * format_text, ...) __attribute__ ((format (printf, 3, 4)));

	inline void set_error_info_errno (int code) { set_error_info(code, 0); }
	void set_error_info_errno (int code, const char * format_text, ...) __attribute__ ((format (printf, 3, 4)));


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	int _last_error_code;
	char _last_error_text[ACS_NCLIB_ERROR_TEXT_SIZE_MAX];
};

#endif // HEADER_GUARD_CLASS__acs_nclib_errorinfoprovider
