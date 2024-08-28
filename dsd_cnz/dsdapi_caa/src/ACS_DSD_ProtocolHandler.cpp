#include <string.h>

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_PrimitiveDataHandler.h"
#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_ProtocolHandler.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

namespace {
	const char * _err_code_descriptions[] = {
		"Successful",
		"Node not valid",
		"File storage failure",
		"Address not valid",
		"Application not valid",
		"Name not unique",
		"Service not registered",
		"Address request failed",
		"Application already registered",
		"No IP address available",
		"No port available",
		"Illegal connection type",
		"Registration failure",
		"Unregister failure",
		"Connection address num mismatch",
		"Bad IP addresses count",
		"Protocol version not supported",
		"Illegal element value",
		"Host unreachable",
		"Operation not allowed",
		"List reply failed"
	};
}

const char * __CLASS_NAME__::error_code_description (acs_dsd::PCP_ErrorCodesConstants error_code) {
	return (error_code < acs_dsd::PCP_ERROR_CODE_SUCCESSFUL) || (acs_dsd::PCP_ERROR_CODE_MAX_VALUE <= error_code)
			? "PCP error code undefined"
			: _err_code_descriptions[error_code];
}

int __CLASS_NAME__::send_31 (
		const char * service_name,
		const char * service_domain,
		ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED],
		ACE_UNIX_Addr unix_address,
		acs_dsd::ServiceVisibilityConstants visibility
) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	errno = 0;
	const char * process_name = ACS_DSD_ConfigurationHelper::my_process_name();

	if (!*process_name) { //ERROR: Process name is empty. Cannot get my process information
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(!*process_name): empty process name");
		return acs_dsd::ERR_GET_PROCESS_INFORMATION;
	}

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	char serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];
	char serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];
	char proc_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME];

	memccpy(serv_name, service_name, 0, acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME);
	serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME - 1] = 0;
	memccpy(serv_domain, service_domain, 0, acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME);
	serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME - 1] = 0;
	memccpy(proc_name, process_name, 0, acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME);
	proc_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME - 1] = 0;

	int call_result = 0;

	//Make the primitive buffer
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Building primitive with the following fields:");
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service name == '%s'", serv_name);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service domain == '%s'", serv_domain);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  process name == '%s'", proc_name);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  process ID == %d", ACS_DSD_ConfigurationHelper::my_process_id());
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  visibility == %d", visibility);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  IP-1 == 0x%08X:%u", inet_addresses[0].get_ip_address(), inet_addresses[0].get_port_number());
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  IP-2 == 0x%08X:%u", inet_addresses[1].get_ip_address(), inet_addresses[1].get_port_number());
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  UNIX address == '%s'", unix_address.get_path_name());

	errno = 0;
	if ((call_result = pcp_pdh.make_primitive(31, 1, serv_name, serv_domain, proc_name, ACS_DSD_ConfigurationHelper::my_process_id(), visibility,
												inet_addresses[0].get_ip_address(), inet_addresses[0].get_port_number(), inet_addresses[1].get_ip_address(),
												inet_addresses[1].get_port_number(), unix_address.get_path_name())))
		return call_result;

	//Now we can send the primitive buffer
	size_t bytes_transferred = 0;

	errno = 0;
	call_result = pcp_pdh.send_primitive(0, &bytes_transferred);
	if (0 == call_result) return acs_dsd::ERR_PEER_CLOSED_CONNECTION; //WARNING: Peer closed the connection
	else if (call_result < 0) return call_result; //ERROR: sending the primitive data
	else if (pcp_pdh.primitive_data_size() ^ bytes_transferred) return acs_dsd::ERR_SYSTEM_SEND;

	return 0;
}

int __CLASS_NAME__::send_32 (
		const char * service_name,
		const char * service_domain,
		ACE_UNIX_Addr & address,
		acs_dsd::ServiceVisibilityConstants visibility
) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	errno = 0;
	const char * process_name = ACS_DSD_ConfigurationHelper::my_process_name();

	if (!*process_name) { //ERROR: Process name is empty. Cannot get my process information
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(!*process_name): empty process name");
		return acs_dsd::ERR_GET_PROCESS_INFORMATION;
	}

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	char serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];
	char serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];
	char proc_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME];
	char unix_addr[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH];

	memccpy(serv_name, service_name, 0, acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME);
	serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME - 1] = 0;
	memccpy(serv_domain, service_domain, 0, acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME);
	serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME - 1] = 0;
	memccpy(proc_name, process_name, 0, acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME);
	proc_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME - 1] = 0;
	memccpy(unix_addr, address.get_path_name(), 0, acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH);
	unix_addr[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH - 1] = 0;

	int call_result = 0;

	//Make the primitive buffer
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Building primitive with the following fields:");
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service name == '%s'", serv_name);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service domain == '%s'", serv_domain);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  process name == '%s'", proc_name);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  process ID == %d", ACS_DSD_ConfigurationHelper::my_process_id());
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  visibility == %d", visibility);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  UNIX address == '%s'", unix_addr);

	errno = 0;
	if ((call_result = pcp_pdh.make_primitive(32, 1, serv_name, serv_domain, proc_name, ACS_DSD_ConfigurationHelper::my_process_id(), visibility, unix_addr)))
		return call_result;

	//Now we can send the primitive buffer
	size_t bytes_transferred = 0;

	errno = 0;
	call_result = pcp_pdh.send_primitive(0, &bytes_transferred);
	if (0 == call_result) return acs_dsd::ERR_PEER_CLOSED_CONNECTION; //WARNING: Peer closed the connection
	else if (call_result < 0) return call_result; //ERROR: sending the primitive data
	else if (pcp_pdh.primitive_data_size() ^ bytes_transferred) return acs_dsd::ERR_SYSTEM_SEND;

	return 0;
}

int __CLASS_NAME__::send_34 (const char * service_name, const char * service_domain) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	char serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];
	char serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];

	memccpy(serv_name, service_name, 0, acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME);
	serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME - 1] = 0;
	memccpy(serv_domain, service_domain, 0, acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME);
	serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME - 1] = 0;

	int call_result = 0;

	//Make the primitive buffer
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Building primitive with the following fields:");
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service name == '%s'", serv_name);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service domain == '%s'", serv_domain);

	errno = 0;
	if ((call_result = pcp_pdh.make_primitive(34, 1, serv_name, serv_domain)))
		return call_result;

	//Now we can send the primitive buffer
	size_t bytes_transferred = 0;

	errno = 0;
	//call_result = _sender.send(pcp_pdh, 0, &bytes_transferred);
	call_result = pcp_pdh.send_primitive(0, &bytes_transferred);
	if (0 == call_result) return acs_dsd::ERR_PEER_CLOSED_CONNECTION; //WARNING: Peer closed the connection
	else if (call_result < 0) return call_result; //ERROR: sending the primitive data
	else if (pcp_pdh.primitive_data_size() ^ bytes_transferred) return acs_dsd::ERR_SYSTEM_SEND;

	return 0;
}

int __CLASS_NAME__::send_36 (
		const char * service_name,
		const char * service_domain,
		int32_t system_id,
		int8_t node_state
) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	char serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];
	char serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];

	memccpy(serv_name, service_name, 0, acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME);
	serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME - 1] = 0;
	memccpy(serv_domain, service_domain, 0, acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME);
	serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME - 1] = 0;

	int call_result = 0;

	//Make the primitive buffer
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Building primitive with the following fields:");
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service name == '%s'", serv_name);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service domain == '%s'", serv_domain);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  system ID == %d", system_id);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  node state == %d", node_state);

	errno = 0;
	if ((call_result = pcp_pdh.make_primitive(36, 1, serv_name, serv_domain, system_id, node_state))) return call_result;

	//Now we can send the primitive buffer
	size_t bytes_transferred = 0;

	errno = 0;
	call_result = pcp_pdh.send_primitive(0, &bytes_transferred);
	if (0 == call_result) return acs_dsd::ERR_PEER_CLOSED_CONNECTION; //WARNING: Peer closed the connection
	else if (call_result < 0) return call_result; //ERROR: sending the primitive data
	else if (pcp_pdh.primitive_data_size() ^ bytes_transferred) return acs_dsd::ERR_SYSTEM_SEND;

	return 0;
}

int __CLASS_NAME__::send_39 (const char * service_name, const char * service_domain, int8_t query_order, uint32_t timeout_ms) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	char serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];
	char serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];

	memccpy(serv_name, service_name, 0, acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME);
	serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME - 1] = 0;
	memccpy(serv_domain, service_domain, 0, acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME);
	serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME - 1] = 0;

	int call_result = 0;

	//Make the primitive buffer
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Building primitive with the following fields:");
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service name == '%s'", serv_name);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  service domain == '%s'", serv_domain);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  query order == %d", query_order);
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  timeout == %u", timeout_ms);

	errno = 0;
	if ((call_result = pcp_pdh.make_primitive(39, 1, serv_name, serv_domain, query_order, timeout_ms))) return call_result;

	//Now we can send the primitive buffer
	size_t bytes_transferred = 0;

	errno = 0;
	ssize_t bytes_sent = pcp_pdh.send_primitive(0, &bytes_transferred);
	if (bytes_sent == 0) return acs_dsd::ERR_PEER_CLOSED_CONNECTION; //WARNING: Peer closed the connection
	else if (bytes_sent < 0) return bytes_sent; //ERROR: sending the primitive data
	else if (pcp_pdh.primitive_data_size() ^ bytes_transferred) return acs_dsd::ERR_SYSTEM_SEND;

	return 0;
}

int __CLASS_NAME__::send_44 (int32_t system_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	int call_result = 0;

	//Make the primitive buffer
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Building primitive with the following fields:");
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  system ID == %d", system_id);

	errno = 0;
	if ((call_result = pcp_pdh.make_primitive(44, 1, system_id))) return call_result;

	//Now we can send the primitive buffer
	size_t bytes_transferred = 0;

	errno = 0;
	ssize_t bytes_sent = pcp_pdh.send_primitive(0, &bytes_transferred);
	if (bytes_sent == 0) return acs_dsd::ERR_PEER_CLOSED_CONNECTION; //WARNING: Peer closed the connection
	else if (bytes_sent < 0) return bytes_sent; //ERROR: sending the primitive data
	else if (pcp_pdh.primitive_data_size() ^ bytes_transferred) return acs_dsd::ERR_SYSTEM_SEND;

	return 0;
}

int __CLASS_NAME__::recv_prim (int prim_to_receive, int & prim_id_received, unsigned & prim_version_received, uint8_t & response_code) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	size_t bytes_transferred = 0;
	ssize_t bytes_received = 0;

	errno = 0;
	if ((bytes_received = pcp_pdh.recv_primitive(0, &bytes_transferred)) < 0)
		return bytes_received;

	if (0 == bytes_received) { //WARNING: Peer closed the connection
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(0 == bytes_received): peer closed connection");
		return acs_dsd::ERR_PEER_CLOSED_CONNECTION;
	}

	//Check that the primitive received is really the primitive prim_to_receive expected
	if ((prim_id_received = pcp_pdh.primitive_id()) ^ prim_to_receive) { //ERROR: the primitive received is not the prim_to_receive
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(prim_id_received ^ prim_to_receive): not expected primitive received: prim_id_received == %d, prim_to_receive == %d",
				prim_id_received, prim_to_receive);
		return acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	//Retrieve information from primitive data by unpacking the primitive
	errno = 0;
	int call_result = pcp_pdh.unpack_primitive(prim_id_received, prim_version_received, &response_code);

	if (!call_result) { //Unpack OK
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Primitive data unpacked. Fields follow:");
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive ID == %d", prim_id_received);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive version == %u", prim_version_received);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  response code == %u", response_code);
	}

	return call_result;
}

int __CLASS_NAME__::recv_37_38 (
		int & prim_id_received,
		unsigned & prim_version_received,
		uint8_t & response_code,
		ACE_INET_Addr (& inet_addresses) [acs_dsd::CONFIG_NETWORKS_SUPPORTED],
		ACE_UNIX_Addr & unix_address
) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	size_t bytes_transferred = 0;
	ssize_t bytes_received = 0;

	errno = 0;
	if ((bytes_received = pcp_pdh.recv_primitive(0, &bytes_transferred)) < 0)
		return bytes_received;

	if (0 == bytes_received) { //WARNING: Peer closed the connection
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(0 == bytes_received): peer closed connection");
		return acs_dsd::ERR_PEER_CLOSED_CONNECTION;
	}

	//Check that the primitive received is really the primitive expected
	if (((prim_id_received = pcp_pdh.primitive_id()) ^ 37) && (prim_id_received ^ 38)) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION((prim_id_received ^ 37) && (prim_id_received ^ 38)): not expected primitive received: prim_id_received == %d", prim_id_received);
		return acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	int call_result = 0;
	char unix_sock_path[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH] = {0};

	//Retrieve information from primitive data by unpacking the primitive
	errno = 0;
	if (prim_id_received == 37) {
		uint32_t ips[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};
		uint16_t ports[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};

		call_result = pcp_pdh.unpack_primitive(prim_id_received, prim_version_received, &response_code, ips, ports, ips + 1, ports + 1, unix_sock_path);

		if (!call_result) { //Unpacking OK. Collect data
			unix_sock_path[ACS_DSD_ARRAY_SIZE(unix_sock_path) - 1] = 0;
			for (size_t i = 0; i < ACS_DSD_ARRAY_SIZE(inet_addresses); ++i) inet_addresses[i].set(ports[i], ips[i]);
			unix_address.set(unix_sock_path);

			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Primitive data unpacked. Fields follow:");
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive ID == %d", prim_id_received);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive version == %u", prim_version_received);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  response code == %u", response_code);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  IP-1 == 0x%08X:%u", ips[0], ports[0]);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  IP-1 == 0x%08X:%u", ips[1], ports[1]);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  UNIX address == '%s'", unix_sock_path);
		}
	} else {
		call_result = pcp_pdh.unpack_primitive(prim_id_received, prim_version_received, &response_code, unix_sock_path);
		if (!call_result) {
			unix_sock_path[ACS_DSD_ARRAY_SIZE(unix_sock_path) - 1] = 0;
			unix_address.set(unix_sock_path);

			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Primitive data unpacked. Fields follow:");
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive ID == %d", prim_id_received);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive version == %u", prim_version_received);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  response code == %u", response_code);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  UNIX address == '%s'", unix_sock_path);
		}
	}

	return call_result;
}

int __CLASS_NAME__::recv_40 (
		int & prim_id_received,
		unsigned & prim_version_received,
		uint8_t & response_code,
		int32_t & system_id,
		int8_t & node_state,
		unsigned timeout_ms) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh;
	_session ? pcp_pdh.bind(*_session) : pcp_pdh.bind(*_sender, *_receiver);

	size_t bytes_transferred = 0;
	ssize_t bytes_received = 0;
	ACE_Time_Value timeout;

	errno = 0;
	if ((bytes_received = pcp_pdh.recv_primitive((timeout_ms ^ 0xFFFFFFFFU) ? (timeout.msec(static_cast<long int>(timeout_ms)), &timeout) : 0, &bytes_transferred)) < 0)
		return bytes_received;

	if (0 == bytes_received) { //WARNING: Peer closed the connection
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(0 == bytes_received): peer closed connection");
		return acs_dsd::ERR_PEER_CLOSED_CONNECTION;
	}

	//Check that the primitive received is really the primitive expected
	if ((prim_id_received = pcp_pdh.primitive_id()) ^ 40) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(prim_id_received ^ 40): not expected primitive received: prim_id_received == %d", prim_id_received);
		return acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	//Retrieving information from the primitive and return
	int call_result = pcp_pdh.unpack_primitive(prim_id_received, prim_version_received, &response_code, &system_id, &node_state);

	if (!call_result) { //Unpack OK
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Primitive data unpacked. Fields follow:");
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive ID == %d", prim_id_received);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  primitive version == %u", prim_version_received);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  response code == %u", response_code);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  system ID == %d", system_id);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "  node state == %d", node_state);
	}

	return call_result;
}
