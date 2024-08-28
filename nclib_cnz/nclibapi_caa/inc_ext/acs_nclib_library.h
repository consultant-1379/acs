#ifndef HEADER_GUARD_CLASS__acs_nclib_library
#define HEADER_GUARD_CLASS__acs_nclib_library acs_nclib_library

/** @file acs_nclib_library.h
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

#include "acs_nclib_errorinfoprovider.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_nclib_library

class acs_nclib_library {
	//==============//
	// Constructors //
	//==============//
private:
	acs_nclib_library ();

	acs_nclib_library (const acs_nclib_library & rhs);


	//============//
	// Destructor //
	//============//
public:
	inline ~acs_nclib_library () {}


	//===========//
	// Functions //
	//===========//
public:

	static int init ();
	static int exit ();

	static inline int last_error_code () { return _error_info.last_error_code(); }
	static inline const char * last_error_text () { return _error_info.last_error_text(); }


	//===========//
	// Operators //
	//===========//
private:
	acs_nclib_library & operator= (const acs_nclib_library & rhs);


	//========//
	// Fields //
	//========//
private:
	static acs_nclib_errorinfoprovider _error_info;
};

#endif // HEADER_GUARD_CLASS__acs_nclib_library
