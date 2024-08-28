#ifndef HEADER_GUARD_CLASS__ACS_DSD_Connector
#define HEADER_GUARD_CLASS__ACS_DSD_Connector ACS_DSD_Connector

/** @file ACS_DSD_Connector.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-14
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
 *	| P0.9.0 | 2010-07-14 | xnicmut      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2010-08-04 | xnicmut      | Released for sprint 2               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.3 | 2010-09-28 | xnicmut      | Released for sprint 4               |
 *	+========+============+==============+=====================================+
 */

#include "ace/Time_Value.h"
#include "ace/Addr.h"

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_IOStream.h"
#include "ACS_DSD_Trace.h"

class ACS_DSD_TraTracer;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Connector

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_Connector ACS_DSD_Connector.h
 *	@brief ACS_DSD_Connector class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-14
 *	@version 1.0.0
 *
 *	ACS_DSD_Connector Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
protected:
	/**	@brief ACS_DSD_Connector Default constructor
	 *
	 *	ACS_DSD_Connector Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	/** @brief ACS_DSD_Connector Copy constructor
	 *
	 *	ACS_DSD_Connector Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ &) { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//============//
	// Destructor //
	//============//
public:
	/**	@brief ACS_DSD_Connector Destructor
	 *
	 *	ACS_DSD_Connector Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/**	@brief connect method
	 *
	 *	connect method detailed description
	 *
	 *	@param[out] new_stream Description
	 *	@param[in] remote_sap Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int connect (ACS_DSD_IOStream & new_stream, const ACE_Addr & remote_sap, ACE_Time_Value * timeout = 0) = 0;

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ &) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Connector
