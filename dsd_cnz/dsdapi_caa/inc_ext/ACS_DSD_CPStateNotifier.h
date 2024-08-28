#ifndef HEADER_GUARD_CLASS__ACS_DSD_CPStateNotifier
#define HEADER_GUARD_CLASS__ACS_DSD_CPStateNotifier ACS_DSD_CPStateNotifier

/** @file ACS_DSD_CPStateNotifier.h
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

#include "ACS_DSD_Communicator.h"
#include "ACS_DSD_Session.h"

class ACS_DSD_ProtocolHandler;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_CPStateNotifier

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_CPStateNotifier ACS_DSD_CPStateNotifier.h
 *	@brief ACS_DSD_CPStateNotifier class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-14
 *	@version 1.0.0
 *
 *	ACS_DSD_CPStateNotifier Class detailed description
 */
class __CLASS_NAME__ : public ACS_DSD_Communicator {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_CPStateNotifier Default constructor
	 *
	 *	ACS_DSD_CPStateNotifier Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : ACS_DSD_Communicator(), _system_id(acs_dsd::SYSTEM_ID_UNKNOWN), _cp_state(acs_dsd::CP_STATE_UNKNOWN), _session() {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

private:
	/** @brief ACS_DSD_CPStateNotifier Copy constructor
	 *
	 *	ACS_DSD_CPStateNotifier Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACS_DSD_Communicator(rhs), _system_id(rhs._system_id), _cp_state(rhs._cp_state), _session() {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		//This copy constructor is private so I do not copy internal objects.
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_CPStateNotifier Destructor
	 *
	 *	ACS_DSD_CPStateNotifier Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		_session.close();
	}

	//===========//
	// Functions //
	//===========//
public:
	/** @brief attach_to method
	 *
	 *	attach_to method detailed description
	 *
	 *	@param[in] node Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int attach_to (int32_t system_id);

	/** @brief detach method
	 *
	 *	detach method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int detach ();

	/** @brief get_cp_state method
	 *
	 *	get_cp_state method detailed description
	 *
	 *	@param[out] cp_state Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_cp_state (acs_dsd::CPStateConstants & cp_state) const;

	/** @brief get_handles method
	 *
	 *	get_handles method detailed description
	 *
	 *	@param[out] handles Description
	 *	@param[in,out] handle_count Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int get_handles (acs_dsd::HANDLE * handles, int & handle_count) const;

	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::CPNotifierStateConstants state () const {
		return (_session.state() ^ acs_dsd::SESSION_STATE_DISCONNECTED ? acs_dsd::CP_NOTIFIER_STATE_ATTACHED : acs_dsd::CP_NOTIFIER_STATE_DETACHED);
	}

	inline int32_t system_id () const { return _system_id; }

private:
	/** @brief receive_cp_state_indication method
	 *
	 *	receive_cp_state_indication method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int receive_cp_state_indication (ACS_DSD_ProtocolHandler & ph) const;
	int receive_cp_state_indication (const ACS_DSD_Session & session) const;

	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		ACS_DSD_Communicator::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	int32_t _system_id;
	mutable acs_dsd::CPStateConstants _cp_state;
	ACS_DSD_Session _session;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_CPStateNotifier
