#ifndef HEADER_GUARD_CLASS__ACS_DSD_Client
#define HEADER_GUARD_CLASS__ACS_DSD_Client ACS_DSD_Client

/** @file ACS_DSD_Client.h
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

#include <string>
#include <vector>

#include "ACS_DSD_Communicator.h"
#include "ACS_DSD_Session.h"

//Forward declarations
class ACS_DSD_Connector;
class ACS_DSD_IOStream;
class ACS_INET_Addr;
class ACE_UNIX_Addr;
class ACE_Time_Value;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Client

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_Client ACS_DSD_Client.h
 *	@brief ACS_DSD_Client class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-14
 *	@version 1.0.0
 *
 *	ACS_DSD_Client Class detailed description
 */
class __CLASS_NAME__ : public ACS_DSD_Communicator {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_Client Default constructor
	 *
	 *	ACS_DSD_Client Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : ACS_DSD_Communicator() { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

private:
	/** @brief ACS_DSD_Client Copy constructor
	 *
	 *	ACS_DSD_Client Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACS_DSD_Communicator(rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		//This copy constructor is private so I do not copy connector internal pointers.
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Client Destructor
	 *
	 *	ACS_DSD_Client Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief get_handles method
	 *
	 *	get_handles method detailed description
	 *
	 *	@param[out] handles Description
	 *	@param[in,out] handle_count Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline virtual int get_handles (acs_dsd::HANDLE * /*handles*/, int & handle_count) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		handle_count = 0;
		return 0;
	}

	/** @brief connect method
	 *
	 *	connect method detailed description
	 *
	 *	@param[out] session Description
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] system_id Description
	 *	@param[in] node_state Description
	 *	@param[in] timeout_ms Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int connect (ACS_DSD_Session & session, const char * service_name, const char * service_domain, int system_id, acs_dsd::NodeStateConstants node_state, unsigned timeout_ms);
	inline int connect (ACS_DSD_Session & session, const std::string & service_name, const std::string & service_domain, int system_id, acs_dsd::NodeStateConstants node_state, unsigned timeout_ms) {
		return connect(session, service_name.c_str(), service_domain.c_str(), system_id, node_state, timeout_ms);
	}
	inline int connect (ACS_DSD_Session & session, const char * service_name, const char * service_domain, int system_id, acs_dsd::NodeStateConstants node_state) {
		return connect(session, service_name, service_domain, system_id, node_state, reinterpret_cast<ACE_Time_Value *>(0));
	}
	inline int connect (ACS_DSD_Session & session, const std::string & service_name, const std::string & service_domain, int system_id, acs_dsd::NodeStateConstants node_state) {
		return connect(session, service_name.c_str(), service_domain.c_str(), system_id, node_state);
	}
	int connect (ACS_DSD_Session & session, const char * service_name, const char * service_domain, unsigned timeout_ms);
	inline int connect (ACS_DSD_Session & session, const std::string & service_name, const std::string & service_domain, unsigned timeout_ms) {
		return connect(session, service_name.c_str(), service_domain.c_str(), timeout_ms);
	}
	inline int connect (ACS_DSD_Session & session, const char * service_name, const char * service_domain) {
		return connect(session, service_name, service_domain, acs_dsd::SYSTEM_ID_THIS_NODE, acs_dsd::NODE_STATE_UNDEFINED, reinterpret_cast<ACE_Time_Value *>(0));
	}
	inline int connect (ACS_DSD_Session & session, const std::string & service_name, const std::string & service_domain) {
		return connect(session, service_name.c_str(), service_domain.c_str());
	}

	/** @brief connect method
	 *
	 *	connect method detailed description
	 *
	 *	@param[out] session Description
	 *	@param[in] service_id Description
	 *	@param[in] system_id Description
	 *	@param[in] node_state Description
	 *	@param[in] timeout_ms Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int connect (ACS_DSD_Session & session, const char * service_id, int system_id, const char * node_name, unsigned timeout_ms);
	inline int connect (ACS_DSD_Session & session, const std::string & service_id, int system_id, const std::string & node_name, unsigned timeout_ms) {
		return connect(session, service_id.c_str(), system_id, node_name.c_str(), timeout_ms);
	}
	inline int connect (ACS_DSD_Session & session, const char * service_id, int system_id, const char * node_name) {
		return connect(session, service_id, system_id, node_name, reinterpret_cast<ACE_Time_Value *>(0));
	}
	inline int connect (ACS_DSD_Session & session, const std::string & service_id, int system_id, const std::string & node_name) {
		return connect(session, service_id.c_str(), system_id, node_name.c_str());
	}
	int connect (ACS_DSD_Session & session, const char * service_id, unsigned timeout_ms);
	inline int connect (ACS_DSD_Session & session, const std::string & service_id, unsigned timeout_ms) {
		return connect(session, service_id.c_str(), timeout_ms);
	}
	inline int connect (ACS_DSD_Session & session, const char * service_id) {
		return connect(session, service_id, acs_dsd::SYSTEM_ID_THIS_NODE, reinterpret_cast<char *>(0), reinterpret_cast<ACE_Time_Value *>(0));
	}
	inline int connect (ACS_DSD_Session & session, const std::string & service_id) {
		return connect(session, service_id.c_str());
	}

	/** @brief query method
	 *
	 *	query method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] scope Description
	 *	@param[out] reachable_nodes Description
	 *	@param[out] unreachable_nodes Description
	 *	@param[in] timeout_ms Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int query (
			const char * service_name,
			const char * service_domain,
			acs_dsd::SystemTypeConstants scope,
			std::vector<ACS_DSD_Node> & reachable_nodes,
			std::vector<ACS_DSD_Node> & unreachable_nodes,
			unsigned timeout_ms = 0xFFFFFFFFU);

	inline int query (
			const std::string & service_name,
			const std::string & service_domain,
			acs_dsd::SystemTypeConstants scope,
			std::vector<ACS_DSD_Node> & reachable_nodes,
			std::vector<ACS_DSD_Node> & unreachable_nodes,
			unsigned timeout_ms = 0xFFFFFFFFU) {
		return query(service_name.c_str(), service_domain.c_str(), scope, reachable_nodes, unreachable_nodes, timeout_ms);
	}

private:
	int connect (
			ACS_DSD_Session & session,
			const char * service_name,
			const char * service_domain,
			int system_id,
			acs_dsd::NodeStateConstants node_state,
			ACE_Time_Value * timeout);

	int connect_ap (
			ACS_DSD_Session & session,
			const char * service_name,
			const char * service_domain,
			int32_t system_id,
			acs_dsd::NodeStateConstants node_state,
			ACE_Time_Value * timeout);

	int connect_cp (
			ACS_DSD_Session & session,
			const char * service_name,
			const char * service_domain,
			int32_t system_id,
			acs_dsd::NodeStateConstants node_state,
			ACE_Time_Value * timeout);

	int connect (
			ACS_DSD_Session & session,
			const char * service_id,
			int system_id,
			const char * node_name,
			ACE_Time_Value * timeout);

public:
	int fetch_service_info_from_imm (
			const char * service_name,
			const char * service_domain,
			const char * node_name,
			ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED],
			ACE_UNIX_Addr & unix_address,
			int & connection_type,
			int & visibility,
			char (& process_name) [acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX],
			int & pid) const;

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
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Client
