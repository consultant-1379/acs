#ifndef HEADER_GUARD_CLASS__ACS_DSD_AcceptorProvider
#define HEADER_GUARD_CLASS__ACS_DSD_AcceptorProvider ACS_DSD_AcceptorProvider

/** @file ACS_DSD_AcceptorProvider.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-26
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
 *	| P0.9.0 | 2010-07-26 | xnicmut      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2010-08-04 | xnicmut      | Released for sprint 2               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.3 | 2010-09-28 | xnicmut      | Released for sprint 4               |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_Acceptor.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_AcceptorProvider

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_AcceptorProvider ACS_DSD_AcceptorProvider.h
 *	@brief ACS_DSD_AcceptorProvider class tamplate
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-26
 *	@version 1.0.0
 *
 *	ACS_DSD_AcceptorProvider Class template detailed description
 */
template <typename acceptor_t, typename addr_t, typename stream_t>
class __CLASS_NAME__ : public ACS_DSD_Acceptor {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_AcceptorProvider Default constructor
	 *
	 *	ACS_DSD_AcceptorProvider Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : ACS_DSD_Acceptor(), _acceptor() { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

private:
	/** @brief ACS_DSD_AcceptorProvider Copy constructor
	 *
	 *	ACS_DSD_AcceptorProvider Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACS_DSD_Acceptor(rhs), _acceptor(rhs._acceptor) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_AcceptorProvider Destructor
	 *
	 *	ACS_DSD_AcceptorProvider Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); close(); remove(); }

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
	virtual inline int open (const ACE_Addr & local_sap, int reuse_addr = 0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		//Check if not closed. An acceptor should be closed before open again.
		if (acs_dsd::ACCEPTOR_STATE_CLOSED ^ state()) return acs_dsd::WAR_ACCEPTOR_NOT_CLOSED;

		errno = 0;
		return _acceptor.open(local_sap, reuse_addr) ? acs_dsd::ERR_SYSTEM_OPEN : (state(acs_dsd::ACCEPTOR_STATE_OPEN), 0);
	}

	/** @brief accept method
	 *
	 *	accept method detailed description
	 *
	 *	@param[out] new_stream Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int accept (ACS_DSD_IOStream & new_stream, ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		stream_t * stream = dynamic_cast<stream_t *>(&new_stream);

		return stream ? accept(*stream, timeout) : acs_dsd::WAR_WRONG_STREAM_TYPE;
	}

	/** @brief accept method
	 *
	 *	accept method detailed description
	 *
	 *	@param[out] new_stream Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int accept (stream_t & new_stream, ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		//Check if opened. An acceptor should be opened before can accept a connection.
		if (acs_dsd::ACCEPTOR_STATE_OPEN ^ state()) return acs_dsd::WAR_ACCEPTOR_NOT_OPEN;

		//Check if the object new_stream is free and not already used for an established communication session
		if (acs_dsd::STREAM_STATE_DISCONNECTED ^ new_stream.state()) return acs_dsd::WAR_STREAM_ALREADY_IN_USE;

		addr_t remote_addr;

		errno = 0;
		return (_acceptor.accept(new_stream._stream, &remote_addr, timeout) ? acs_dsd::ERR_SYSTEM_ACCEPT : (new_stream.state(acs_dsd::STREAM_STATE_CONNECTED), 0));
	}

	/** @brief close method
	 *
	 *	close method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int close () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _acceptor.close() ? acs_dsd::ERR_SYSTEM_CLOSE : (state(acs_dsd::ACCEPTOR_STATE_CLOSED), 0);
	}

	/** @brief remove method
	 *
	 *	remove method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int remove () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _acceptor.remove() ? acs_dsd::ERR_SYSTEM_REMOVE : (state(acs_dsd::ACCEPTOR_STATE_CLOSED), 0);
	}

	/** @brief get_handle method
	 *
	 *	get_handle method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline acs_dsd::HANDLE get_handle () const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		return _acceptor.get_handle();
	}

	/** @brief get_local_addr method
	 *
	 *	get_local_addr method detailed description
	 *
	 *	@param[out] sa Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int get_local_addr (ACE_Addr & sa) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _acceptor.get_local_addr(sa) ? acs_dsd::ERR_SYSTEM_GET_ADDRESS : 0;
	}

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
	virtual inline int get_option (int level, int option, void * optval, int * optlen) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _acceptor.get_option(level, option, optval, optlen) ? acs_dsd::ERR_SYSTEM_GET_OPTION : 0;
	}

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
	virtual inline int set_option (int level, int option, void * optval, int optlen) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _acceptor.set_option(level, option, optval, optlen) ? acs_dsd::ERR_SYSTEM_SET_OPTION : 0;
	}

	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		ACS_DSD_Acceptor::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	acceptor_t _acceptor; ///< Acceptor internal object.
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_AcceptorProvider
