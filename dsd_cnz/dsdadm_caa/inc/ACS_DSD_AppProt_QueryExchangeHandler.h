#ifndef HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeHandler
#define HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeHandler AppProt_QueryExchangeHandler

/** @file ACS_DSD_AppProt_QueryExchangeHandler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
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
 *	| P0.9.0 | 2010-09-28 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ace/Event_Handler.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeHandler

/** @class AppProt_QueryExchangeHandler ACS_DSD_AppProt_QueryExchangeHandler.h
 *	@brief AppProt_QueryExchangeHandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
 *	@version 1.0.0
 *
 *	AppProt_QueryExchangeHandler Class detailed description
 */
template <typename context_t>
class __CLASS_NAME__ : public ACE_Event_Handler {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief AppProt_QueryExchangeHandler Default constructor
	 *
	 *	AppProt_QueryExchangeHandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ ()
	: ACE_Event_Handler(), _query_exchange_context(), _query_handlers_counter(0) {}

protected:
	/** @brief AppProt_QueryExchangeHandler Copy constructor
	 *
	 *	AppProt_QueryExchangeHandler Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs)
	: ACE_Event_Handler(rhs), _query_exchange_context(rhs._query_exchange_context),
	  _query_handlers_counter(rhs._query_handlers_counter)
	{}

	//============//
	// Destructor //
	//============//
public:
	/** @brief AppProt_QueryExchangeHandler Destructor
	 *
	 *	AppProt_QueryExchangeHandler Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	inline int activate (
			ACE_Reactor & reactor,
			ACS_DSD_ServiceHandler * service_handler,
			const char * service_name,
			const char * service_domain,
			int32_t system_id,
			const char * node_name,
			acs_dsd::NodeStateConstants node_state,
			/*ACS_DSD_Node & node_to_connect,*/
			volatile int * query_handlers_counter,
			const uint32_t * timeout_ms = 0) {
		_query_handlers_counter = query_handlers_counter;
		return _query_exchange_context.activate(reactor, this, service_handler, service_name, service_domain,
				system_id, node_name, node_state, timeout_ms);
	}

	virtual inline ACE_HANDLE get_handle () const { return _query_exchange_context.get_handle(); }

	virtual inline int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE) {
		return _query_exchange_context.handle_input(fd);
	}

	virtual int handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/) {
		if (__sync_sub_and_fetch(_query_handlers_counter, 1) <= 0)
			if (ACE_Reactor * my_reactor = reactor()) my_reactor->end_reactor_event_loop();

		return 0;
	}

protected:
	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		this->ACE_Event_Handler::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	context_t _query_exchange_context;
	volatile int * _query_handlers_counter;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeHandler
