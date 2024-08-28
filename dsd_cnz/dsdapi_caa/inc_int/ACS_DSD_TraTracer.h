#ifndef HEADER_GUARD_CLASS__ACS_DSD_TraTracer
#define HEADER_GUARD_CLASS__ACS_DSD_TraTracer ACS_DSD_TraTracer

/** @file ACS_DSD_TraTracer.h
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

#include <string>

#include "ACS_DSD_Macros.h"

#include "ACS_TRA_trace.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_TraTracer

#define ACS_DSD_TRACE_MESSAGE_SIZE_MAX 4096

/** @class ACS_DSD_TraTracer ACS_DSD_TraTracer.h
 *	@brief ACS_DSD_TraTracer class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_TraTracer Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_TraTracer Default constructor
	 *
	 *	ACS_DSD_TraTracer Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	explicit inline __CLASS_NAME__ (const char * tag = 0) : _trace_obj(tag ? tag : "UNKNOWN_TAG", "C") {}

private:
	/** @brief ACS_DSD_TraTracer Copy constructor
	 *
	 *	ACS_DSD_TraTracer Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ &) : _trace_obj("_______", "C128") {}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_TraTracer Destructor
	 *
	 *	ACS_DSD_TraTracer Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	int trace (const char * format, ...) __attribute__ ((format (printf, 2, 3)));

	void dump (uint8_t * buffer, ssize_t size, ssize_t dumping_size, ssize_t dumping_line_length);

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ &) { return *this; }

	//========//
	// Fields //
	//========//
private:
	ACS_TRA_trace _trace_obj;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_TraTracer
