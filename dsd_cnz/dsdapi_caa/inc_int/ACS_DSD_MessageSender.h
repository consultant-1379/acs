#ifndef HEADER_GUARD_CLASS__ACS_DSD_MessageSender
#define HEADER_GUARD_CLASS__ACS_DSD_MessageSender ACS_DSD_MessageSender

/** @file ACS_DSD_MessageSender.h
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

#include "ACS_DSD_Sender.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_MessageSender

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_MessageSender ACS_DSD_MessageSender.h
 *	@brief ACS_DSD_MessageSender class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_MessageSender Class detailed description
 */
class __CLASS_NAME__ : public ACS_DSD_Sender {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_MessageSender Default constructor
	 *
	 *	ACS_DSD_MessageSender Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	inline explicit __CLASS_NAME__ (const ACS_DSD_IOStream * stream = 0) : ACS_DSD_Sender(stream) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	/** @brief ACS_DSD_MessageSender Default constructor
	 *
	 *	ACS_DSD_MessageSender Constructor detailed description
	 *
	 *	@param[in] stream Description
	 *	@remarks Remarks
	 */
	inline explicit __CLASS_NAME__ (const ACS_DSD_IOStream & stream) : ACS_DSD_Sender(stream) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

private:
	/** @brief ACS_DSD_MessageSender Copy constructor
	 *
	 *	ACS_DSD_MessageSender Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACS_DSD_Sender(rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_MessageSender Destructor
	 *
	 *	ACS_DSD_MessageSender Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief sendf method
	 *
	 *	sendf method detailed description
	 *
	 *	@param[in] buf Description
	 *	@param[in] len Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred
	 *	@return Return Description
	 *	@remarks Remarks
	 */
//	virtual ssize_t send (const void * buf, size_t len, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const;
	virtual ssize_t sendf (const void * buf, size_t len, int flags, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const;


	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		ACS_DSD_Sender::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_MessageSender
