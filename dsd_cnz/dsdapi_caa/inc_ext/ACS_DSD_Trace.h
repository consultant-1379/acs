#ifndef HEADER_GUARD_CLASS__ACS_DSD_Trace
#define HEADER_GUARD_CLASS__ACS_DSD_Trace ACS_DSD_Trace

/** @file ACS_DSD_Trace.h
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

#include "ACS_DSD_Macros.h"

class ACS_DSD_TraTracer;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Trace

/** @class ACS_DSD_Trace ACS_DSD_Trace.h
 *	@brief ACS_DSD_Trace class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_Trace Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_Trace Default constructor
	 *
	 *	ACS_DSD_Trace Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ (ACS_DSD_TraTracer & tracer, const char * function_name, const char * file_name = 0, int line = -1);

private:
	/** @brief ACS_DSD_Trace Copy constructor
	 *
	 *	ACS_DSD_Trace Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : _tracer(rhs._tracer), _function_name(rhs._function_name), _file_name(rhs._file_name), _line(rhs._line) {}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Trace Destructor
	 *
	 *	ACS_DSD_Trace Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~__CLASS_NAME__ ();

	//===========//
	// Functions //
	//===========//
public:

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		_tracer = rhs._tracer;
		_function_name = rhs._function_name;
		_file_name = rhs._file_name;
		_line = rhs._line;
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	ACS_DSD_TraTracer * _tracer;
	const char * _function_name;
	const char * _file_name;
	int _line;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Trace
