#include <string.h>
#include <stdlib.h>

#include "ace/Time_Value.h"
#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"

#include "ACS_CS_API.h"
#include "acs_apgcc_omhandler.h"

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_LSocketConnector.h"
#include "ACS_DSD_LSocketStream.h"
#include "ACS_DSD_MessageSender.h"
#include "ACS_DSD_MessageReceiver.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_PrimitiveDataHandler.h"
#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_Client.h"

#define __ACS_DSD_SEARCH_SCOPE_NONE__	0
#define __ACS_DSD_SEARCH_SCOPE_IMM__	1
#define __ACS_DSD_SEARCH_SCOPE_DSD__	2
#define __ACS_DSD_SEARCH_SCOPE_ALL__	3

extern const char * const _repeated_messages[];
#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

int __CLASS_NAME__::connect (
		ACS_DSD_Session & session,
		const char * service_name,
		const char * service_domain,
		int system_id,
		acs_dsd::NodeStateConstants node_state,
		unsigned timeout_ms) {
//	ACE_Time_Value timeout;
//	timeout.set_msec(timeout_ms);
	ACE_Time_Value timeout(timeout_ms/1000, (timeout_ms%1000)*1000);
	return connect(session, service_name, service_domain, system_id, node_state, &timeout);
}

int __CLASS_NAME__::connect (ACS_DSD_Session & session, const char * service_name, const char * service_domain, unsigned timeout_ms) {
//	ACE_Time_Value timeout;
//	timeout.set_msec(timeout_ms);
	ACE_Time_Value timeout(timeout_ms/1000, (timeout_ms%1000)*1000);
	return connect(session, service_name, service_domain, acs_dsd::SYSTEM_ID_THIS_NODE, acs_dsd::NODE_STATE_UNDEFINED, &timeout);
}

int __CLASS_NAME__::connect (
		ACS_DSD_Session & session,
		const char * service_name,
		const char * service_domain,
		int system_id,
		acs_dsd::NodeStateConstants node_state,
		ACE_Time_Value * timeout) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check parameters
	if (!service_name || !service_domain || !*service_name || !*service_domain)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL, 0, "Parameters 'service_name' and 'service_domain'");

	if (strlen(service_name) >= acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0,
				"Parameter service_name is longer than %d characters", acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX - 1);

	if (strlen(service_domain) >= acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0,
				"Parameter service_domain is longer than %d characters", acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX - 1);

	//Check the parameter system_id
	if (system_id == acs_dsd::SYSTEM_ID_UNKNOWN)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0, "Parameter system_id == acs_dsd::SYSTEM_ID_UNKNOWN");

	//Check if the object session is free and not already used for an established communication session
	if (session.state() ^ acs_dsd::SESSION_STATE_DISCONNECTED) ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SESSION_ALREADY_IN_USE);

	acs_dsd::SystemTypeConstants system_type = acs_dsd::SYSTEM_TYPE_UNKNOWN;

	if (((acs_dsd::CONFIG_AP_ID_SET_START <= system_id) && (system_id <= acs_dsd::CONFIG_AP_ID_SET_END)) ||
			(system_id == acs_dsd::SYSTEM_ID_FRONT_END_AP) || (system_id == acs_dsd::SYSTEM_ID_PARTNER_NODE) ||
			(system_id == acs_dsd::SYSTEM_ID_THIS_NODE))
		system_type = acs_dsd::SYSTEM_TYPE_AP;
	else if (((acs_dsd::CONFIG_BC_ID_SET_START <= system_id) && (system_id <= acs_dsd::CONFIG_BC_ID_SET_END)) ||
					 ((acs_dsd::CONFIG_CP_ID_SET_START <= system_id) && (system_id <= acs_dsd::CONFIG_CP_ID_SET_END)) ||
					 (system_id == acs_dsd::SYSTEM_ID_CP_ALARM_MASTER) || (system_id == acs_dsd::SYSTEM_ID_CP_CLOCK_MASTER))
		system_type = acs_dsd::SYSTEM_TYPE_CP;

	int return_code = 0;

	switch (system_type) {
	case acs_dsd::SYSTEM_TYPE_AP: return_code = connect_ap(session, service_name, service_domain, static_cast<int32_t>(system_id), node_state, timeout); break;
	case acs_dsd::SYSTEM_TYPE_BC:
	case acs_dsd::SYSTEM_TYPE_CP: return_code = connect_cp(session, service_name, service_domain, static_cast<int32_t>(system_id), node_state, timeout); break;
	default:
		ACS_DSD_API_SET_ERROR_TRACE(return_code, acs_dsd::ERR_BAD_SYSTEM_ID, 0, "Bad system id in system_id parameter: system_id == %d", system_id);
		break;
	}

	return return_code;
}

int __CLASS_NAME__::connect (
		ACS_DSD_Session & session,
		const char * service_id,
		int system_id,
		const char * node_name,
		unsigned timeout_ms) {
//	ACE_Time_Value timeout;
//	timeout.set_msec(timeout_ms);
	ACE_Time_Value timeout(timeout_ms/1000, (timeout_ms%1000)*1000);
	return connect(session, service_id, system_id, node_name, &timeout);
}

int __CLASS_NAME__::connect (ACS_DSD_Session & session, const char * service_id, unsigned timeout_ms) {
//	ACE_Time_Value timeout;
//	timeout.set_msec(timeout_ms);
	ACE_Time_Value timeout(timeout_ms/1000, (timeout_ms%1000)*1000);
	return connect(session, service_id, acs_dsd::SYSTEM_ID_THIS_NODE, reinterpret_cast<char *>(0), &timeout);
}

int __CLASS_NAME__::connect (
		ACS_DSD_Session & session_,
		const char * service_id_,
		int system_id_,
		const char * node_name_,
		ACE_Time_Value * timeout_) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check service_id parameter
	if (!service_id_ || !*service_id_)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL, 0, "Parameter 'service_id'");

	char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};

	//If the system id is SYSTEM_ID_THIS_NODE or SYSTEM_ID_PARTNER_NODE then I can fetch the node name from
	//the configuration that I have already loaded.
	//Otherwise I peek the node name as provided by the caller
	switch (system_id_) {
	case acs_dsd::SYSTEM_ID_THIS_NODE:
	case acs_dsd::SYSTEM_ID_PARTNER_NODE: {
			ACS_DSD_Node node_search;
			int call_result = (system_id_ ^ acs_dsd::SYSTEM_ID_THIS_NODE ? get_partner_node(node_search) : get_local_node(node_search));
			if (call_result && (call_result ^ acs_dsd::ERR_GET_NODE_STATE)) return call_result;
			memccpy(node_name, node_search.node_name, 0, ACS_DSD_ARRAY_SIZE(node_name));
			node_name[ACS_DSD_ARRAY_SIZE(node_name) - 1] = 0;
		}
		break;
	default:
		//check the node_name parameter
		if (!node_name_ || !*node_name_)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL, 0, "Parameter 'node_name'");

		if (strlen(node_name_) >= acs_dsd::CONFIG_NODE_NAME_SIZE_MAX)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0,
					"Parameter node_name is longer than %d characters", acs_dsd::CONFIG_NODE_NAME_SIZE_MAX - 1);

		memccpy(node_name, node_name_, 0, ACS_DSD_ARRAY_SIZE(node_name));
		node_name[ACS_DSD_ARRAY_SIZE(node_name) - 1] = 0;
		break;
	}

	unsigned long port_ul = 0;
	char * end_ptr = 0;
	int call_result = 0;
	int errno_save = 0;

	port_ul = strtoul(service_id_, &end_ptr, 10);

	//Is the service id an INET socket port number?
	if (*end_ptr) { //NO: it's a local UNIX socket path
		ACE_UNIX_Addr unix_address(service_id_);
		call_result = session_.connect_unix(unix_address, errno_save);
	} else { //YES: it's an INET socket port number
		//Is port number value in the possible IP4 range?
		if ((port_ul <= 0) || (acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MAX < port_ul)) //Port number out of range
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE, 0,
					"The service_id parameter specifies the port number %lu that is out of possible range [1, %d]",
					port_ul, acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MAX);

		//Get the host info from the node name
		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t node_info_ptr = 0;
		errno = 0;
		switch (call_result = ACS_DSD_ConfigurationHelper::get_node_by_node_name(node_name, node_info_ptr)) {
		case acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL:
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, 0, "The calculated node name seems to be empty or null: node_name == '%s'", node_name);
			break;
		case acs_dsd::ERR_NODE_NOT_FOUND:
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, 0, "The node '%s' was not found in configuration", node_name);
			break;
		default:
			if (call_result) //Other errors
				ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno,
						"calling 'get_node_by_node_name(...' with node_name == '%s' to get node info; call_result == %d", node_name, call_result);
			break;
		}

		//Check the INET addresses loaded into the found node
		if (node_info_ptr->ip4_adresses_count <= 0)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, 0, "The node '%s' has no INET addresses associated with it", node_name);

		uint16_t port = static_cast<uint16_t>(port_ul);
		ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
		for (size_t i = 0; (i < node_info_ptr->ip4_adresses_count) && (i < ACS_DSD_ARRAY_SIZE(inet_addresses)); ++i)
			inet_addresses[i].set(htons(port), node_info_ptr->ip4_addresses[i], 0);

		call_result = session_.connect_inet(inet_addresses, node_info_ptr->system_type, node_name, timeout_, errno_save);
	}

	//Check and react to possible errors, if any
	switch (call_result) {
	case acs_dsd::ERR_BAD_PARAMETER_VALUE:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, 0,
				"The connect function has provided a bad system type parameter to the underlying session object for the connection establishment: "
				"session last error == %d, session last error text == '%s'", session_.last_error(), session_.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save,
				"session last error == %d, session last error text == '%s'", session_.last_error(), session_.last_error_text());
		break;	//errno
	case acs_dsd::WAR_STREAM_ALREADY_IN_USE:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_STREAM_ALREADY_IN_USE, 0,
				"Internal I/O stream object seems to be already in use: very strange! %s: "
				"session last error == %d, session last error text == '%s'", _repeated_messages[3], session_.last_error(), session_.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_CONNECT:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, errno_save, "Cannot connect to the service id '%s' on the node '%s': "
				"session last error == %d, session last error text == '%s'", service_id_, node_name, session_.last_error(), session_.last_error_text());
		break;
	case acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL, 0, "All the addresses associated with the node '%s' are empty or null: "
				"session last error == %d, session last error text == '%s'", node_name, session_.last_error(), session_.last_error_text());
		break;
	case acs_dsd::ERR_OCP_SENDING_ACCEPT_INDICATION:
	case acs_dsd::ERR_OCP_RECEIVING_ACCEPT_CONFIRMATION:
	case acs_dsd::ERR_OCP_ACCEPT_CONFIRM_BAD_RESPONSE:
	case acs_dsd::ERR_CONFIG_NODE_IP_CONFLICT:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save, "session last error == %d, session last error text == '%s'",
				session_.last_error(), session_.last_error_text());
		break; //errno
	case acs_dsd::WAR_NODE_NOT_FOUND:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_NODE_NOT_FOUND, 0,
				"The node '%s' the caller is trying to connect to was not found in the system configuration loaded from CS: "
				"session last error == %d, session last error text == '%s'", node_name, session_.last_error(), session_.last_error_text());
		break;	//Searching for the remote node the caller is trying to connect to
	default:
		if (call_result) //OTHER ERRORS
			ACS_DSD_API_SET_ERROR_TRACE_RETURN((call_result < 0) ? call_result : acs_dsd::ERR_SYSTEM_CONNECT, errno_save,
					"An error occurred while connecting to the service id '%s' on the node '%s'; error code == %d: "
					"session last error == %d, session last error text == '%s'", service_id_, node_name, call_result, session_.last_error(), session_.last_error_text());
		break;
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::query (
		const char * service_name,
		const char * service_domain,
		acs_dsd::SystemTypeConstants scope,
		std::vector<ACS_DSD_Node> & reachable_nodes,
		std::vector<ACS_DSD_Node> & unreachable_nodes,
		unsigned timeout_ms) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check parameters
	if (!service_name || !service_domain || !*service_name || !*service_domain)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL, 0, "Parameters 'service_name' and 'service_domain'");

	if (strlen(service_name) >= acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0,
				"Parameter service_name is longer than %d characters", acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX - 1);

	if (strlen(service_domain) >= acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0,
				"Parameter service_domain is longer than %d characters", acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX - 1);

	//Check the parameter scope
	if (scope == acs_dsd::SYSTEM_TYPE_UNKNOWN)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0, "Parameter scope == acs_dsd::SYSTEM_TYPE_UNKNOWN");

	int call_result = 0;
	int errno_save = 0;

	//Try to connect to the DSD server.
	ACS_DSD_LSocketConnector unix_connector;
	ACS_DSD_LSocketStream unix_stream;
	const char * dsd_sap_address = ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname();
	ACE_UNIX_Addr dsd_api_sap_address(dsd_sap_address);

	errno = 0;
	if ((call_result = unix_connector.connect(unix_stream, dsd_api_sap_address))) //ERROR: problems while connecting to the DSD server
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, call_result < 0 ? errno : 0, call_result < 0 ? _repeated_messages[4] : _repeated_messages[5], dsd_sap_address);

	ACS_DSD_MessageSender sender(unix_stream);
	ACS_DSD_MessageReceiver receiver(unix_stream);
	ACS_DSD_ProtocolHandler ph(sender, receiver);

	errno = 0;
	if ((call_result = ph.send_39(service_name, service_domain, scope, timeout_ms)) < 0) {
		errno_save = errno;
		unix_stream.close();
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save,
				"Query request error: cannot get query data from the DSD server for the service '%s@%s' in the scope %d",
				service_name, service_domain, scope);
	}

	reachable_nodes.clear();
	unreachable_nodes.clear();

	int return_code = acs_dsd::ERR_NO_ERRORS;
	int prim_id_received = 0;
	unsigned prim_version_received = 0;
	uint8_t response_code = 0;
	int32_t system_id = 0;
	int8_t node_state = acs_dsd::NODE_STATE_UNDEFINED;
	ACS_DSD_Node node;

	for (;;) {
		errno = 0;
		call_result = ph.recv_40(prim_id_received, prim_version_received, response_code, system_id, node_state, timeout_ms);
		errno_save = errno;

		//Check call result code
		if (call_result) {
			if (call_result == acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED) {
				ACS_DSD_API_SET_ERROR_TRACE(return_code, acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0,
						"Query request (prim 39) error: the DSD server replied with the primitive number %d instead "
						"of the primitive number 40", prim_id_received);
			} else if (call_result == acs_dsd::ERR_PEER_CLOSED_CONNECTION) {
				return_code = set_error_info(acs_dsd::ERR_NO_ERRORS);
			} else if (errno_save == ETIME) {
				// Timeout expired
				ACS_DSD_API_SET_ERROR_TRACE(return_code, acs_dsd::WAR_TIMEOUT_EXPIRED, errno_save,
						"WARNING: Timeout expired before receiving any data on the session: timeout_ms == %u", timeout_ms);
			} else {
				ACS_DSD_API_SET_ERROR_TRACE(return_code, call_result, errno_save, "DSD server query reply error");
			}

// TO BE DELETED
//			else if (call_result ^ acs_dsd::ERR_PEER_CLOSED_CONNECTION)
//				ACS_DSD_API_SET_ERROR_TRACE(return_code, call_result, errno_save, "DSD server query reply error");
//			else return_code = set_error_info(acs_dsd::ERR_NO_ERRORS);

			break;
		}

		//Fit the node structure
		node.system_id = system_id;
		node.system_type = ACS_DSD_ConfigurationHelper::system_id_to_system_type(system_id);
		ACS_DSD_ConfigurationHelper::system_id_to_system_name(system_id, node.system_name, ACS_DSD_ARRAY_SIZE(node.system_name));

		if ((node_state ^ acs_dsd::NODE_STATE_ACTIVE) &&
				(node_state ^ acs_dsd::NODE_STATE_EXECUTION) &&
				(node_state ^ acs_dsd::NODE_STATE_PASSIVE) &&
				(node_state ^ acs_dsd::NODE_STATE_STANDBY)
			) node.node_state = acs_dsd::NODE_STATE_UNDEFINED;
		else node.node_state = static_cast<acs_dsd::NodeStateConstants>(node_state);

		//Check response_code
		((response_code == acs_dsd::PCP_ERROR_CODE_SUCCESSFUL) ? reachable_nodes : unreachable_nodes).push_back(node);
	}

	unix_stream.close();

	return return_code;
}

int __CLASS_NAME__::connect_ap (
		ACS_DSD_Session & session_,
		const char * service_name_,
		const char * service_domain_,
		int32_t system_id_,
		acs_dsd::NodeStateConstants node_state_,
		ACE_Time_Value * timeout_) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_Node local_node;
	ACS_DSD_Node partner_node;

	errno = 0;
	int local_node_call_result = get_local_node(local_node);
	if (local_node_call_result && (local_node_call_result ^ acs_dsd::ERR_GET_NODE_STATE))
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(local_node_call_result, errno, "getting my local node info");

	errno = 0;
	int partner_node_call_result = get_partner_node(partner_node);
	if (partner_node_call_result && (partner_node_call_result ^ acs_dsd::ERR_GET_NODE_STATE))
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(partner_node_call_result, errno, "getting my partner node info");

	int32_t system_id = 0;
	char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};
	acs_dsd::NodeStateConstants node_state = node_state_;
	bool try_unix_channel_first = false;

	int call_result = 0;

	if (system_id_ == acs_dsd::SYSTEM_ID_PARTNER_NODE) {
		system_id = partner_node.system_id;
		memccpy(node_name, partner_node.node_name, 0, ACS_DSD_ARRAY_SIZE(node_name));
		node_name[ACS_DSD_ARRAY_SIZE(node_name) - 1] = 0;
	} else {
		if (system_id_ == acs_dsd::SYSTEM_ID_FRONT_END_AP) {
			APID front_end_ap_id = 0;						//The next case will take care of checking on system this ID
			call_result = 0;
			if ((call_result = ACS_CS_API_NetworkElement::getFrontAPG(front_end_ap_id)) ^ ACS_CS_API_NS::Result_Success)
				ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_CS_API_CALL_ERROR, 0,
									"Function failed 'ACS_CS_API_NetworkElement::getFrontAPG(...', call result == %d", call_result);
			system_id_ = front_end_ap_id;
			if ((system_id_ < acs_dsd::CONFIG_AP_ID_SET_START) || (acs_dsd::CONFIG_AP_ID_SET_END < system_id_))
				ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_BAD_SYSTEM_ID, 0,
									"Bad AP system id received from CS API for the front-end AP system: system id == %d", system_id_);
		} else if (system_id_ == acs_dsd::SYSTEM_ID_THIS_NODE) system_id_ = local_node.system_id;

		//AP system requested by caller is this AP?
		if ((system_id = system_id_) == local_node.system_id) { //YES. I will check the node state to make-up the node name to search against IMM
			if ((node_state == acs_dsd::NODE_STATE_ACTIVE) || (node_state == acs_dsd::NODE_STATE_PASSIVE)) { //The caller provided a node state. I will search the name of the node with the state provided
				if (local_node_call_result) ACS_DSD_API_SET_ERROR_TRACE_RETURN(local_node_call_result, errno, "getting my local node info");
				if (node_state ^ local_node.node_state) memccpy(node_name, partner_node.node_name, 0, ACS_DSD_ARRAY_SIZE(node_name));
				else {
					memccpy(node_name, local_node.node_name, 0, ACS_DSD_ARRAY_SIZE(node_name));
					try_unix_channel_first = true;
				}
				node_name[ACS_DSD_ARRAY_SIZE(node_name) - 1] = 0;
			} else { //The node state is undefined so I will search for this side
				memccpy(node_name, local_node.node_name, 0, ACS_DSD_ARRAY_SIZE(node_name));
				node_name[ACS_DSD_ARRAY_SIZE(node_name) - 1] = 0;
				try_unix_channel_first = true;
			}
		} else { //NO. I will check the state provided by caller and after I will forward the address request to my DSD server
			ACS_DSD_ConfigurationHelper::system_id_to_system_name(system_id, node_name, ACS_DSD_ARRAY_SIZE(node_name));
			(node_state ^ acs_dsd::NODE_STATE_ACTIVE) && (node_state ^ acs_dsd::NODE_STATE_PASSIVE) && (node_state = acs_dsd::NODE_STATE_ACTIVE);
		}
	}

	ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
	ACE_UNIX_Addr unix_address;
	int errno_save = 0;
	int connection_type = acs_dsd::CONNECTION_TYPE_UNKNOWN;
	int visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE;

	if (system_id == local_node.system_id) { //I should search for a service published on this system so I will search only in IMM metabase
		char process_name [acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX] = {0};
		int pid = 0;

		if ((call_result = fetch_service_info_from_imm(service_name_, service_domain_, node_name, inet_addresses, unix_address, connection_type, visibility, process_name, pid)))
			return call_result;

		//If I'm searching for a service onto my partner node then I must check the service visibility
		if (!try_unix_channel_first && (visibility == acs_dsd::SERVICE_VISIBILITY_LOCAL_SCOPE))
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED, 0,
					": local visibility active on the service: service requested == '%s@%s'", service_name_, service_domain_);

#if ACS_DSD_CLIENT_HAS_SERVICE_PROCESS_RUNNING_CHECK

		//Check that the service process is really running. This check can be done on the local node only.
		if (try_unix_channel_first) {
			if ((call_result = ACS_DSD_ConfigurationHelper::process_running(process_name, pid)) < 0) {
				//ERROR: on the parameter process_name. So, there was a problem retrieving info from IMM
				ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SAF_IMM_OM_GET, 0,
						": bad 'process name' attribute got from IMM service registration item: service requested == '%s@%s'", service_name_, service_domain_);
			} else if (!call_result) {
				//WARNING: The IMM service registration is not valid because there isn't any process running for the requested service
				ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED, 0,
						": the IMM service registration item is not valid: no process running for the requested service: service requested == '%s@%s'", service_name_, service_domain_);
			}
		}

#endif

	} else { //I should search for a service published on another AP system so I will query my DSD server
		//Try to connect to the DSD server.
		ACS_DSD_LSocketConnector unix_connector;
		ACS_DSD_LSocketStream unix_stream;
		const char * dsd_sap_address = ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname();
		ACE_UNIX_Addr dsd_api_sap_address(dsd_sap_address);

		errno = 0;
		if ((call_result = unix_connector.connect(unix_stream, dsd_api_sap_address))) //ERROR: problems while connecting to the DSD server
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, call_result < 0 ? errno : 0, call_result < 0 ? _repeated_messages[4] : _repeated_messages[5], dsd_sap_address);

		ACS_DSD_MessageSender sender(unix_stream);
		ACS_DSD_MessageReceiver receiver(unix_stream);
		ACS_DSD_ProtocolHandler ph(sender, receiver);

		errno = 0;
		if ((call_result = ph.send_36(service_name_, service_domain_, system_id, static_cast<int8_t>(node_state))) < 0) {
			errno_save = errno;
			unix_stream.close();
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save,
					"Address request error: cannot get address data from the DSD server for the service '%s@%s' onto the node {system_id == %d, state == %d}",
					service_name_, service_domain_, system_id, node_state);
		}

		int prim_id_received = 0;
		unsigned prim_version_received = 0;
		uint8_t response_code = 0;

		prim_id_received = 0;
		errno = 0;
		call_result = ph.recv_37_38(prim_id_received, prim_version_received, response_code, inet_addresses, unix_address);
		errno_save = errno;

		unix_stream.close();

		if (acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED == call_result)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0,
					"Address request (prim 36) error: the DSD server replied with the primitive number %d instead of the primitive numbers 37 38", prim_id_received);
		else if (call_result < 0)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save, "Address request error: DSD server reply error");

		//Check response code by DSD server
		if (response_code >= acs_dsd::PCP_ERROR_CODE_MAX_VALUE)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_UNEXPECTED_ERROR_CODE, 0,
								"unexpected error code was replied from DSD server to an address request for the service '%s@%s': DSD response code == %d",
								service_name_, service_domain_, response_code);
		if (response_code)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_ERROR_CODE_START_CODE_BASE - response_code, 0,
								"DSD server replied to an address request with the response code error == %d, service requested == '%s@%s'",
								response_code, service_name_, service_domain_);

		connection_type = (prim_id_received ^ acs_dsd::PCP_DSDAPI_ADDRESS_INET_REPLY_ID ? acs_dsd::CONNECTION_TYPE_UNIX : acs_dsd::CONNECTION_TYPE_INET);
	}

	//Finally I can try to connect to the requested service
	if (connection_type & acs_dsd::CONNECTION_TYPE_INET) { //The type of service is INET
		errno_save = 0;
		if (try_unix_channel_first) { //I try to use first a unix local channel, if possible
			errno = 0;
			call_result = (unix_address.get_path_name() && (*(unix_address.get_path_name()))) ? session_.connect_unix(unix_address, errno_save) : 1;
		}

		if (!try_unix_channel_first || call_result) {
			errno = 0;
			call_result = session_.connect_inet(inet_addresses, acs_dsd::SYSTEM_TYPE_AP, node_name, timeout_, errno_save);
		}
	} else if (connection_type & acs_dsd::CONNECTION_TYPE_UNIX) { //The type of service is UNIX
		errno = 0;
		if (unix_address.get_path_name() && (*(unix_address.get_path_name()))) call_result = session_.connect_unix(unix_address, errno_save);
		else
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL, 0,
					"Cannot connect towards an empty or null UNIX local address retrieved from DSD server or IMM metabase: service requested == '%s@%s'",
					service_name_, service_domain_);
	} else //Bad connection type retrieved from IMM
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SAF_IMM_BAD_ATTRIBUTE_VALUE, 0, "Attribute "ACS_DSD_CONFIG_IMM_CONN_TYPE_ATTR_NAME" == %d", connection_type);

	//Check and react to possible errors, if any
	switch (call_result) {
	case acs_dsd::ERR_BAD_PARAMETER_VALUE:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, 0,
				"The connect function has provided a bad system type parameter to the underlying session object for the connection establishment: "
				"session last error == %d, session last error text == '%s'", session_.last_error(), session_.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save,
				"session last error == %d, session last error text == '%s'", session_.last_error(), session_.last_error_text());
		break;	//errno
	case acs_dsd::WAR_STREAM_ALREADY_IN_USE:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_STREAM_ALREADY_IN_USE, 0,
				"Internal I/O stream object seems to be already in use: very strange! %s: "
				"session last error == %d, session last error text == '%s'", _repeated_messages[3], session_.last_error(), session_.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_CONNECT:
		if (system_id == local_node.system_id)
			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, errno_save, "Cannot connect to the service '%s@%s' on the node '%s': "
					"session last error == %d, session last error text == '%s'", service_name_, service_domain_, node_name, session_.last_error(),
					session_.last_error_text());
		else {
			char system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX] = {0};

			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, errno_save, "Cannot connect to the service '%s@%s' on the node '%s' of the system id %d (%s): "
						"session last error == %d, session last error text == '%s'",
						service_name_, service_domain_, ACS_DSD_AP_NODE_STATE_STR[node_state], system_id,
						ACS_DSD_ConfigurationHelper::system_id_to_system_name(system_id, system_name, ACS_DSD_ARRAY_SIZE(system_name)) ? "<NOT VALUED>" : system_name,
						session_.last_error(), session_.last_error_text());
		}

		break;	//errno
	case acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL, 0, "All the addresses retrieved from IMM or DSD server are empty or null: "
				"session last error == %d, session last error text == '%s'", session_.last_error(), session_.last_error_text());
		break;
	case acs_dsd::ERR_OCP_SENDING_ACCEPT_INDICATION:
	case acs_dsd::ERR_OCP_RECEIVING_ACCEPT_CONFIRMATION:
	case acs_dsd::ERR_OCP_ACCEPT_CONFIRM_BAD_RESPONSE:
	case acs_dsd::ERR_CONFIG_NODE_IP_CONFLICT:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save, "session last error == %d, session last error text == '%s'",
				session_.last_error(), session_.last_error_text());
		break; //errno
	case acs_dsd::WAR_NODE_NOT_FOUND:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_NODE_NOT_FOUND, 0,
				"The node the caller is trying to connect to was not found in the system configuration loaded from CS: "
				"session last error == %d, session last error text == '%s'", session_.last_error(), session_.last_error_text());
		break;	//Searching for the remote node the caller is trying to connect to
	default:
		if (call_result) //OTHER ERRORS
			ACS_DSD_API_SET_ERROR_TRACE_RETURN((call_result < 0) ? call_result : acs_dsd::ERR_SYSTEM_CONNECT, errno_save,
					"An error occurred while connecting to the service '%s@%s'; error code == %d: "
					"session last error == %d, session last error text == '%s'", service_name_, service_domain_, call_result,
					session_.last_error(), session_.last_error_text());
		break;
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::connect_cp (
		ACS_DSD_Session & session,
		const char * service_name,
		const char * service_domain,
		int32_t system_id,
		acs_dsd::NodeStateConstants node_state,
		ACE_Time_Value * timeout) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	(node_state ^ acs_dsd::NODE_STATE_ACTIVE) && (node_state ^ acs_dsd::NODE_STATE_PASSIVE) && (node_state = acs_dsd::NODE_STATE_ACTIVE);
	int call_result = 0;
	int errno_save = 0;

	//Try to connect to the DSD server.
	ACS_DSD_LSocketConnector unix_connector;
	ACS_DSD_LSocketStream unix_stream;
	const char * dsd_sap_address = ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname();
	ACE_UNIX_Addr dsd_api_sap_address(dsd_sap_address);

	errno = 0;
	if ((call_result = unix_connector.connect(unix_stream, dsd_api_sap_address))) //ERROR: problems while connecting to the DSD server
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, call_result < 0 ? errno : 0, call_result < 0 ? _repeated_messages[4] : _repeated_messages[5], dsd_sap_address);

	ACS_DSD_MessageSender sender(unix_stream);
	ACS_DSD_MessageReceiver receiver(unix_stream);
	ACS_DSD_ProtocolHandler ph(sender, receiver);

	errno = 0;
	if ((call_result = ph.send_36(service_name, service_domain, system_id, static_cast<int8_t>(node_state))) < 0) {
		errno_save = errno;
		unix_stream.close();
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save,
				"Address request error: cannot get address data from the DSD server for the service '%s@%s' onto the node {system_id == %d, state == %d}",
				service_name, service_domain, system_id, node_state);
	}

	int prim_id_received = 0;
	unsigned prim_version_received = 0;
	uint8_t response_code = 0;

	ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
	ACE_UNIX_Addr unix_address;

	errno = 0;
	call_result = ph.recv_37_38(prim_id_received, prim_version_received, response_code, inet_addresses, unix_address);
	errno_save = errno;

	unix_stream.close();

	if (acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED == call_result)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0,
				"Address request (prim 36) error: the DSD server replied with the primitive number %d instead of the primitive numbers 37 38", prim_id_received);
	else if (call_result < 0)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save, "Address request error: DSD server reply error");

	//Check response code by DSD server
	if (response_code >= acs_dsd::PCP_ERROR_CODE_MAX_VALUE)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_UNEXPECTED_ERROR_CODE, 0,
							"unexpected error code was replied from DSD server to an address request for the service '%s@%s': DSD response code == %d",
							service_name, service_domain, response_code);
	if (response_code)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_ERROR_CODE_START_CODE_BASE - response_code, 0,
							"DSD server replied to an address request with the response code error == %d, service requested == '%s@%s'",
							response_code, service_name, service_domain);

	if (prim_id_received ^ acs_dsd::PCP_DSDAPI_ADDRESS_INET_REPLY_ID) { //ERROR: I'm connecting to a CP but the reply is not for an INET connection
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0,
				"The caller requested to connect via INET socket towards the service '%s@%s' on the CP/BC system %d but the DSD server replied with a local UNIX socket (primitive %d)",
				service_name, service_domain, system_id, prim_id_received);
	}

	char system_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};
	ACS_DSD_ConfigurationHelper::system_id_to_system_name(system_id, system_name, ACS_DSD_ARRAY_SIZE(system_name));

	//Finally I can try to connect to the requested service
	errno = 0;
	call_result = session.connect_inet(inet_addresses, ACS_DSD_ConfigurationHelper::system_id_to_system_type(system_id), system_name, timeout, errno_save);

	//Check and react to possible errors, if any
	switch (call_result) {
	case acs_dsd::ERR_BAD_PARAMETER_VALUE:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, 0,
				"The connect function has provided a bad system type parameter to the underlying session object for the connection establishment: "
				"session last error == %d, session last error text == '%s'", session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save,
				"session last error == %d, session last error text == '%s'", session.last_error(), session.last_error_text());
		break;	//errno
	case acs_dsd::WAR_STREAM_ALREADY_IN_USE:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_STREAM_ALREADY_IN_USE, 0,
				"Internal I/O stream object seems to be already in use: very strange! %s: "
				"session last error == %d, session last error text == '%s'", _repeated_messages[3], session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_CONNECT: {
			char system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX] = {0};

			ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SYSTEM_CONNECT, errno_save, "Cannot connect to the service '%s@%s' on the node '%s' of the system id %d (%s): "
					"session last error == %d, session last error text == '%s'",
					service_name, service_domain, ACS_DSD_CP_NODE_STATE_STR[node_state], system_id,
					ACS_DSD_ConfigurationHelper::system_id_to_system_name(system_id, system_name, ACS_DSD_ARRAY_SIZE(system_name)) ? "<NOT VALUED>" : system_name,
					session.last_error(), session.last_error_text());
		}
		break;	//errno
	case acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL, 0, "All the addresses retrieved from IMM or DSD server are empty or null: "
				"session last error == %d, session last error text == '%s'", session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_OCP_SENDING_ACCEPT_INDICATION:
	case acs_dsd::ERR_OCP_RECEIVING_ACCEPT_CONFIRMATION:
	case acs_dsd::ERR_OCP_ACCEPT_CONFIRM_BAD_RESPONSE:
	case acs_dsd::ERR_CONFIG_NODE_IP_CONFLICT:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(call_result, errno_save, "session last error == %d, session last error text == '%s'",
				session.last_error(), session.last_error_text());
		break; //errno
	case acs_dsd::WAR_NODE_NOT_FOUND:
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_NODE_NOT_FOUND, 0,
				"The node the caller is trying to connect to was not found in the system configuration loaded from CS: "
				"session last error == %d, session last error text == '%s'", session.last_error(), session.last_error_text());
		break;	//Searching for the remote node the caller is trying to connect to
	default:
		if (call_result) //OTHER ERRORS
			ACS_DSD_API_SET_ERROR_TRACE_RETURN((call_result < 0) ? call_result : acs_dsd::ERR_SYSTEM_CONNECT, errno_save,
					"An error occurred while connecting to the service '%s@%s'; error code == %d: "
					"session last error == %d, session last error text == '%s'", service_name, service_domain, call_result,
					session.last_error(), session.last_error_text());
		break;
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::fetch_service_info_from_imm (
		const char * service_name,
		const char * service_domain,
		const char * node_name,
		ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED],
		ACE_UNIX_Addr & unix_address,
		int & connection_type,
		int & visibility,
		char (& process_name) [acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX],
		int & pid) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;
	const char * dsd_imm_root_dn = 0;

	int error_code = set_error_info(acs_dsd::ERR_NO_ERRORS);

	errno = 0;
	if ((error_code = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(dsd_imm_root_dn)))
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(error_code, errno,
				"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM");

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS)
		ACS_DSD_API_SET_ERROR_TRACE_RETURN(acs_dsd::ERR_SAF_IMM_OM_INIT, errno, "'OmHandler::Init()' failed: return code == %d", imm_call_result);

	char srv_info_dn[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX];

	//Make up the dn to reach the SRVInfo instance object
	snprintf(srv_info_dn, ACS_DSD_ARRAY_SIZE(srv_info_dn),
			ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
			service_name, service_domain, node_name, dsd_imm_root_dn);

	//I'm ready to fetch object from IMM repository
	ACS_APGCC_ImmObject imm_object;
	imm_object.objName = srv_info_dn;
	unsigned attrib_set_read_count = 0;

	static const char * imm_attributes_to_search[] = {
			/* 0 */ ACS_DSD_CONFIG_IMM_INET_ADDRESSES_ATTR_NAME,
			/* 1 */ ACS_DSD_CONFIG_IMM_UNIX_ADDRESSES_ATTR_NAME,
			/* 2 */ ACS_DSD_CONFIG_IMM_CONN_TYPE_ATTR_NAME,
			/* 3 */ ACS_DSD_CONFIG_IMM_VISIBILITY_ATTR_NAME,
			/* 4 */ ACS_DSD_CONFIG_IMM_PROCESS_NAME_ATTR_NAME,
			/* 5 */ ACS_DSD_CONFIG_IMM_PID_ATTR_NAME
	};

	errno = 0;
	if ((imm_call_result = om_handler.getObject(&imm_object)) == ACS_CC_SUCCESS) {
		unsigned attributes_count = imm_object.attributes.size();
		bool attributes_are_empty = true;

		for (size_t i = 0; i < ACS_DSD_ARRAY_SIZE(inet_addresses); inet_addresses[i++] = ACE_INET_Addr()) ;
		unix_address.set("");
		connection_type = acs_dsd::CONNECTION_TYPE_UNKNOWN;
		visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE;
		*process_name = 0;
		pid = 0;

		//Search for attributes
		error_code = 0;
		for (unsigned i = 0; !error_code && (attrib_set_read_count < ACS_DSD_ARRAY_SIZE(imm_attributes_to_search)) && (i < attributes_count); ++i) {
			const char * attr_name = imm_object.attributes[i].attrName.c_str();

			unsigned attrib_index_found = ACS_DSD_ARRAY_SIZE(imm_attributes_to_search);
			for (unsigned index = 0; index < attrib_index_found; ++index)
				strcmp(attr_name, imm_attributes_to_search[index]) || (attrib_index_found = index);

			if (attrib_index_found < ACS_DSD_ARRAY_SIZE(imm_attributes_to_search)) {
				++attrib_set_read_count;

				if (unsigned values_count = imm_object.attributes[i].attrValuesNum) {
					attributes_are_empty = false;

					switch (attrib_index_found) {
					case 0: { //INET tcp addresses found
							char ** values = reinterpret_cast<char **>(imm_object.attributes[i].attrValues);
							if (values)
								for (unsigned i = 0; !error_code && (i < ACS_DSD_ARRAY_SIZE(inet_addresses)) && (i < values_count); ++i)
									values[i] && inet_addresses[i].set(values[i]);
						}
						break;
					case 1: { //UNIX address found
							char ** values = reinterpret_cast<char **>(imm_object.attributes[i].attrValues);
							values && *values && unix_address.set(*values);
						}
						break;
					case 2: { //Connection type attribute found
							uint32_t ** values = reinterpret_cast<uint32_t **>(imm_object.attributes[i].attrValues);
							values && *values && (connection_type = **values);
						}
						break;
					case 3: { //Visibility attribute found
							int32_t ** values = reinterpret_cast<int32_t **>(imm_object.attributes[i].attrValues);
							values && *values && (visibility = **values);
						}
						break;
					case 4: { //Process Name attribute
							char ** values = reinterpret_cast<char **>(imm_object.attributes[i].attrValues);
							values && *values && strncpy(process_name, *values, ACS_DSD_ARRAY_SIZE(process_name));
							process_name[ACS_DSD_ARRAY_SIZE(process_name) - 1] = 0;
						}
						break;
					case 5: { //PID attribute
							int32_t ** values = reinterpret_cast<int32_t **>(imm_object.attributes[i].attrValues);
							values && *values && (pid = **values);
						}
						break;
					}
				}
			}
		}

		//Check if attributes are empty
		attributes_are_empty && (error_code = set_error_info(acs_dsd::ERR_SAF_IMM_EMPTY_ATTRIBUTE_VALUE, 0,
				"empty attributes values retrieved from IMM for the object '%s': the DSD server (IMM objects owner) could be not running", srv_info_dn));
	} else ACS_DSD_API_SET_ERROR_TRACE(error_code, acs_dsd::ERR_SAF_IMM_OM_GET, errno,
			"'OmHandler::getObject(...' failed to retrieve the object '%s': return code == %d", srv_info_dn, imm_call_result);

	om_handler.Finalize();

	if (!error_code && (attrib_set_read_count < ACS_DSD_ARRAY_SIZE(imm_attributes_to_search)))
		ACS_DSD_API_SET_ERROR_TRACE(error_code, acs_dsd::ERR_SAF_IMM_OM_GET, 0,
				"An incomplete "ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"'s object fetched from IMM DSD class instances: object path == '%s'", srv_info_dn);

	return error_code;
}
