#ifndef HEADER_GUARD_CLASS__ACS_DSD_ProtocolHandler
#define HEADER_GUARD_CLASS__ACS_DSD_ProtocolHandler ACS_DSD_ProtocolHandler

/** @file ACS_DSD_ProtocolHandler.h
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

#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_MessageSender.h"
#include "ACS_DSD_MessageReceiver.h"
#include "ACS_DSD_Session.h"
#include "ACS_DSD_Trace.h"

class ACS_DSD_TraTracer;

namespace acs_dsd {
	enum PCP_ErrorCodesConstants {
		PCP_ERROR_CODE_SUCCESSFUL												=	0,	///< PCP_ERROR_CODE_SUCCESSFUL description
		PCP_ERROR_CODE_NODE_NOT_VALID										=	1,	///< PCP_ERROR_CODE_NODE_NOT_VALID description
		PCP_ERROR_CODE_FILE_STORAGE_FAILURE							=	2,	///< PCP_ERROR_CODE_FILE_STORAGE_FAILURE description
		PCP_ERROR_CODE_ADDRESS_NOT_VALID								=	3,	///< PCP_ERROR_CODE_ADDRESS_NOT_VALID description
		PCP_ERROR_CODE_APPLICATION_NOT_VALID						=	4,	///< PCP_ERROR_CODE_APPLICATION_NOT_VALID description
		PCP_ERROR_CODE_NAME_NOT_UNIQUE									=	5,	///< PCP_ERROR_CODE_NAME_NOT_UNIQUE description
		PCP_ERROR_CODE_SERVICE_NOT_REGISTERED						=	6,	///< PCP_ERROR_CODE_SERVICE_NOT_REGISTERED description
		PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED						=	7,	///< PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED description
		PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED		=	8,	///< PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED description
		PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE					=	9,	///< PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE description
		PCP_ERROR_CODE_NO_PORT_AVAILABLE								=	10,	///< PCP_ERROR_CODE_NO_PORT_AVAILABLE description
		PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE					=	11,	///< PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE description
		PCP_ERROR_CODE_REGISTRATION_FAILURE							=	12,	///< PCP_ERROR_CODE_REGISTRATION_FAILURE description
		PCP_ERROR_CODE_UNREGISTER_FAILURE								=	13,	///< PCP_ERROR_CODE_UNREGISTER_FAILURE description
		PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH	=	14,	///< PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH description
		PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT						=	15,	///< PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT description
		PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED		=	16,	///< PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED description
		PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE						=	17,	///< PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE description
		PCP_ERROR_CODE_HOST_UNREACHABLE									=	18,	///< PCP_ERROR_CODE_HOST_UNREACHABLE description
		PCP_ERROR_CODE_OPERATION_NOT_ALLOWED						=	19,	///< PCP_ERROR_CODE_OPERATION_NOT_ALLOWED description
		PCP_ERROR_CODE_LIST_REPLY_FAILED								=	20, ///< PCP_ERROR_CODE_LIST_REPLY_FAILED description
		PCP_ERROR_CODE_MAX_VALUE															///< PCP_ERROR_CODE_MAX_VALUE description
	};
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_ProtocolHandler

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_ProtocolHandler ACS_DSD_ProtocolHandler.h
 *	@brief ACS_DSD_ProtocolHandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.1
 *
 *	ACS_DSD_ProtocolHandler Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_ProtocolHandler Default constructor
	 *
	 *	ACS_DSD_ProtocolHandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline explicit __CLASS_NAME__ (const ACS_DSD_Session & session) : _sender(0), _receiver(0), _session(&session) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	/** @brief ACS_DSD_ProtocolHandler Default constructor
	 *
	 *	ACS_DSD_ProtocolHandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (ACS_DSD_MessageSender & sender, ACS_DSD_MessageReceiver & receiver) : _sender(&sender), _receiver(&receiver), _session(0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

private:
	/** @brief ACS_DSD_ProtocolHandler Copy constructor
	 *
	 *	ACS_DSD_ProtocolHandler Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : _sender(rhs._sender), _receiver(rhs._receiver), _session(rhs._session) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		//This copy constructor is private so I do not copy I/O stream internal pointers.
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_ProtocolHandler Destructor
	 *
	 *	ACS_DSD_ProtocolHandler Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief error_code_description method
	 *
	 *	error_code_description method detailed description
	 *
	 *	@param[in] error_code Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static const char * error_code_description (acs_dsd::PCP_ErrorCodesConstants error_code);

	/** @brief send_31 method
	 *
	 *	send_31 method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] inet_addresses Description
	 *	@param[in] unix_address Description
	 *	@param[in] visibility Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int send_31 (
			const char * service_name,
			const char * service_domain,
			ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED],
			ACE_UNIX_Addr unix_address,
			acs_dsd::ServiceVisibilityConstants visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE
	);

	/** @brief send_32 method
	 *
	 *	send_32 method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] address Description
	 *	@param[in] visibility Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int send_32 (
			const char * service_name,
			const char * service_domain,
			ACE_UNIX_Addr & address,
			acs_dsd::ServiceVisibilityConstants visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE
	);

	/** @brief send_34 method
	 *
	 *	send_34 method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int send_34 (const char * service_name, const char * service_domain);

	/** @brief send_36 method
	 *
	 *	send_36 method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] system_id Description
	 *	@param[in] node_state Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int send_36 (const char * service_name, const char * service_domain, int32_t system_id, int8_t node_state);

	/** @brief send_39 method
	 *
	 *	send_39 method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] query_order Description
	 *	@param[in] timeout_ms Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int send_39 (const char * service_name, const char * service_domain, int8_t query_order, uint32_t timeout_ms);

	/** @brief send_44 method
	 *
	 *	send_44 method detailed description
	 *
	 *	@param[in] system_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int send_44 (int32_t system_id);

	/** @brief recv_prim method
	 *
	 *	recv_prim method detailed description
	 *
	 *	@param[in] prim_to_receive Description
	 *	@param[out] prim_id_received Description
	 *	@param[out] prim_version_received Description
	 *	@param[out] response_code Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int recv_prim (int prim_to_receive, int & prim_id_received, unsigned & prim_version_received, uint8_t & response_code);

	/** @brief recv_37_38 method
	 *
	 *	recv_37_38 method detailed description
	 *
	 *	@param[out] prim_id_received Description
	 *	@param[out] prim_version_received Description
	 *	@param[out] response_code Description
	 *	@param[out] inet_addresses Description
	 *	@param[out] unix_address Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int recv_37_38 (
			int & prim_id_received,
			unsigned & prim_version_received,
			uint8_t & response_code,
			ACE_INET_Addr (& inet_addresses) [acs_dsd::CONFIG_NETWORKS_SUPPORTED],
			ACE_UNIX_Addr & unix_address
	);

	/** @brief recv_40 method
	 *
	 *	recv_40 method detailed description
	 *
	 *	@param[out] prim_id_received Description
	 *	@param[out] prim_version_received Description
	 *	@param[out] response_code Description
	 *	@param[out] system_id Description
	 *	@param[out] node_state Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int recv_40 (
			int & prim_id_received,
			unsigned & prim_version_received,
			uint8_t & response_code,
			int32_t & system_id,
			int8_t & node_state,
			unsigned timeout_ms
	);

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
	const ACS_DSD_MessageSender * _sender;
	const ACS_DSD_MessageReceiver * _receiver;
	const ACS_DSD_Session * _session;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_ProtocolHandler
