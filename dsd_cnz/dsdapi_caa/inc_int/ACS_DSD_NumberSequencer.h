#ifndef HEADER_GUARD_CLASS__ACS_DSD_NumberSequencer
#define HEADER_GUARD_CLASS__ACS_DSD_NumberSequencer ACS_DSD_NumberSequencer

/** @file ACS_DSD_NumberSequencer.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-14
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
 *	| P0.9.0 | 2010-10-14 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_Trace.h"

class ACS_DSD_TraTracer;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_NumberSequencer

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_NumberSequencer ACS_DSD_NumberSequencer.h
 *	@brief ACS_DSD_NumberSequencer class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-14
 *	@version 1.0.0
 *
 *	ACS_DSD_NumberSequencer Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_NumberSequencer Default constructor
	 *
	 *	ACS_DSD_NumberSequencer Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ (uint16_t start_number, uint16_t end_number, unsigned seed = 0);

private:
	/** @brief ACS_DSD_NumberSequencer Copy constructor
	 *
	 *	ACS_DSD_NumberSequencer Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ &) {
		//ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		//This copy constructor is private so I do not copy connector internal pointers.
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_NumberSequencer Destructor
	 *
	 *	ACS_DSD_NumberSequencer Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { /*ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));*/ }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief next method
	 *
	 *	next method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t next ();

	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ &) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	uint16_t _start;
	uint16_t _end;
	unsigned _seed_state;
	uint16_t _current_point;
	uint16_t _increment;
	ACE_Recursive_Thread_Mutex _sync_obj;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_NumberSequencer
