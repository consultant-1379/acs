#ifndef HEADER_GUARD_CLASS__ACS_DSD_Receiver
#define HEADER_GUARD_CLASS__ACS_DSD_Receiver ACS_DSD_Receiver

/** @file ACS_DSD_Receiver.h
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

#include "ace/Time_Value.h"

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_IOStream.h"
#include "ACS_DSD_Trace.h"

class ACS_DSD_TraTracer;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Receiver

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_Receiver ACS_DSD_Receiver.h
 *	@brief ACS_DSD_Receiver class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_Receiver Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	enum {
		OCP_HEADER_FIELD_MESSAGE_LENGTH_SIZE = 2,
		OCP_HEADER_SIZE = 4,
		DSD_HEADER_SIZE = 4,
		OCP_DSD_HEADERS_SIZE_TOTAL = OCP_HEADER_SIZE + DSD_HEADER_SIZE
	};


	//==============//
	// Constructors //
	//==============//
protected:
	/** @brief ACS_DSD_Receiver Default constructor
	 *
	 *	ACS_DSD_Receiver Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	inline explicit __CLASS_NAME__ (const ACS_DSD_IOStream * stream = 0) : _stream(stream) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	/** @brief ACS_DSD_Receiver Default constructor
	 *
	 *	ACS_DSD_Receiver Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	inline explicit __CLASS_NAME__ (const ACS_DSD_IOStream & stream) : _stream(&stream) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	/** @brief ACS_DSD_Receiver Copy constructor
	 *
	 *	ACS_DSD_Receiver Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : _stream(rhs._stream) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Receiver Destructor
	 *
	 *	ACS_DSD_Receiver Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief stream method
	 *
	 *	stream detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline const ACS_DSD_IOStream * stream () const { return _stream; }

	/** @brief bind_to method
	 *
	 *	bind_to detailed description
	 *
	 *	@param[in] stream Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void bind_to (const ACS_DSD_IOStream * stream = 0) { _stream = stream; }

	/** @brief bind_to method
	 *
	 *	bind_to detailed description
	 *
	 *	@param[in] stream Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void bind_to (const ACS_DSD_IOStream & stream) { _stream = &stream; }

	/** @brief recv method
	 *
	 *	recv method detailed description
	 *
	 *	@param[out] buf Description
	 *	@param[in] len Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recv (void * buf, size_t len, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const = 0;

	//void trace_dump (uint8_t * buffer, ssize_t size, ssize_t dumping_size, ssize_t dumping_line_length) const;

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
	const ACS_DSD_IOStream * _stream;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Receiver
