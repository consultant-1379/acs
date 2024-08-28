#ifndef HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeApContext
#define HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeApContext AppProt_QueryExchangeApContext

/** @file ACS_DSD_AppProt_QueryExchangeApContext.h
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

#include "ACS_DSD_SrvProt_SvcHandler.h"
#include "ACS_DSD_AppProt_QueryPrimitives.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeApContext

/** @class AppProt_QueryExchangeApContext ACS_DSD_AppProt_QueryExchangeApContext.h
 *	@brief AppProt_QueryExchangeApContext class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
 *	@version 1.0.0
 *
 *	AppProt_QueryExchangeApContext Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief AppProt_QueryExchangeApContext Default constructor
	 *
	 *	AppProt_QueryExchangeApContext Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ ()
	: _srv_prot_svc_handler() , _service_handler(0), _query_response()
	{}

private:
	/** @brief AppProt_QueryExchangeApContext Copy constructor
	 *
	 *	AppProt_QueryExchangeApContext Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ (const __CLASS_NAME__ & /*rhs*/);
/*
	: _srv_prot_svc_handler(rhs._srv_prot_svc_handler), _service_handler(rhs._service_handler),
	  _query_response()
	{}
*/


	//============//
	// Destructor //
	//============//
public:
	/** @brief AppProt_QueryExchangeApContext Destructor
	 *
	 *	AppProt_QueryExchangeApContext Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual inline ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	int activate (
			ACE_Reactor & reactor,
			ACE_Event_Handler * event_handler,
			ACS_DSD_ServiceHandler * service_handler,
			const char * service_name,
			const char * service_domain,
			int32_t system_id,
			const char * node_name,
			acs_dsd::NodeStateConstants node_state,
			/*ACS_DSD_Node & node_to_connect,*/
			const uint32_t * timeout_ms = 0);

	inline ACE_HANDLE get_handle () const { return _srv_prot_svc_handler.get_handle(); }

  int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);


  //===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & /*rhs*/);
/*
	{
		return *this;
	}
*/


	//========//
	// Fields //
	//========//
private:
	ACS_DSD_SrvProt_SvcHandler _srv_prot_svc_handler;
	ACS_DSD_ServiceHandler * _service_handler;
	AppProt_query_v1_response _query_response;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeApContext
