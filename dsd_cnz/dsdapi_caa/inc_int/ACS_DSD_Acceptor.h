#ifndef HEADER_GUARD_CLASS__ACS_DSD_Acceptor
#define HEADER_GUARD_CLASS__ACS_DSD_Acceptor ACS_DSD_Acceptor

/** @file ACS_DSD_Acceptor.h
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
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Acceptor

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_Acceptor ACS_DSD_Acceptor.h
 *	@brief ACS_DSD_Acceptor class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-14
 *	@version 1.0.0
 *
 *	ACS_DSD_Acceptor Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
protected:
	/** @brief ACS_DSD_Acceptor Default constructor
	 *
	 *	ACS_DSD_Acceptor Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : _state(acs_dsd::ACCEPTOR_STATE_CLOSED) { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	/** @brief ACS_DSD_Acceptor Copy constructor
	 *
	 *	ACS_DSD_Acceptor Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : _state(rhs._state) { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Acceptor Destructor
	 *
	 *	ACS_DSD_Acceptor Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief open method
	 *
	 *	open method detailed description
	 *
	 *	@param[in] local_sap Description
	 *	@param[in] reuse_addr Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int open (const ACE_Addr & local_sap, int reuse_addr = 0) = 0;

	/** @brief accept method
	 *
	 *	accept method detailed description
	 *
	 *	@param[out] new_stream Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int accept (ACS_DSD_IOStream & new_stream, ACE_Time_Value * timeout = 0) const = 0;

	/** @brief close method
	 *
	 *	close method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int close () = 0;

	/** @brief remove method
	 *
	 *	remove method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int remove () = 0;

	/** @brief get_handle method
	 *
	 *	get_handle method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual acs_dsd::HANDLE get_handle () const = 0;

	/** @brief get_local_addr method
	 *
	 *	get_local_addr method detailed description
	 *
	 *	@param[out] sa Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int get_local_addr (ACE_Addr & sa) const = 0;

	/** @brief get_option method
	 *
	 *	get_option method detailed description
	 *
	 *	@param[in] level Description
	 *	@param[in] option Description
	 *	@param[out] optval Description
	 *	@param[in,out] optlen Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int get_option (int level, int option, void * optval, int * optlen) const = 0;

	/** @brief set_option method
	 *
	 *	set_option method detailed description
	 *
	 *	@param[in] level Description
	 *	@param[in] option Description
	 *	@param[in] optval Description
	 *	@param[in] optlen Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int set_option (int level, int option, void * optval, int optlen) const = 0;

	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::AcceptorStateConstants state () const { return _state; }

protected:
	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@param[in] new_state Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::AcceptorStateConstants state (acs_dsd::AcceptorStateConstants new_state) {
		const acs_dsd::AcceptorStateConstants old_state = _state;
		return (_state = new_state, old_state);
	}

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
	acs_dsd::AcceptorStateConstants _state;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Acceptor
