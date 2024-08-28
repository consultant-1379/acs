#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"

#include "ACS_DSD_SocketAcceptor.h"

#include "ACS_DSD_PrimitiveDataHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Server.h"

namespace {
//	void print_configuration ();
//	void testing ();
}

int main(int /*argc*/, char * /*argv*/ []) {
	printf("\nETIMEDOUT == %d\n", ETIMEDOUT);
	printf("\nEAGAIN == %d\n", EAGAIN);
	printf("\nEWOULDBLOCK == %d\n", EWOULDBLOCK);

	printf("\n\n>>>>>>> TEST timeout<<<<<<\n\n");
	ACS_DSD_SocketAcceptor socket_acceptor;
	ACE_INET_Addr inet_address("192.168.169.1:44444");
	socket_acceptor.open(inet_address);

	printf("\n\n--->>>>>> PRESS ENTER TO ACCEPT TEST CONNECTIONS..."); getchar();







/*
	ACS_DSD_ConfigurationHelper::HostInfo host_info;
	uint32_t net_order_ip = (192) | (168 << 8) | (169 << 16) | (6 << 24);
	int call_result = ACS_DSD_ConfigurationHelper::get_node_by_ip(net_order_ip, host_info);

	if (call_result) printf("** ERROR: 'ACS_DSD_ConfigurationHelper::get_node_by_ip(...' failed: error code == %d\n", call_result);
	else {
		printf(
				"\n\n"
				"Host System Name == '%s'\n"
				"Host Node Name == '%s'\n"
				"Host Side == %d\n"
				"Host System ID == %d\n"
				"Host System Type == %d\n"
				"Host IP Addresses\n",
				host_info.system_name, host_info.node_name, host_info.side, host_info.system_id, host_info.system_type);
		for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i)
			printf("  IP[%d] == '%s'\n", i, host_info.ip4_addresses_str[i]);
	}

	const ACS_DSD_ConfigurationHelper::HostInfo & my_ap_host_info	= ACS_DSD_ConfigurationHelper::get_my_ap_node();
	const ACS_DSD_ConfigurationHelper::HostInfo & my_ap_partner_host_info = ACS_DSD_ConfigurationHelper::get_my_ap_partner_node();

	printf("AP Nodes Count == %u\n", ACS_DSD_ConfigurationHelper::system_ap_nodes_count());
	printf("CP Nodes Count == %u\n", ACS_DSD_ConfigurationHelper::system_cp_nodes_count());
	printf(
			"MY AP System Name == '%s'\n"
			"MY AP Node Name == '%s'\n"
			"MY AP Side == %d\n"
			"MY AP System ID == %d\n", my_ap_host_info.system_name, my_ap_host_info.node_name, my_ap_host_info.side, my_ap_host_info.system_id);
	printf(
			"MY AP Partner System Name == '%s'\n"
			"MY AP Partner Node Name == '%s'\n"
			"MY AP Partner Side == %d\n"
			"MY AP Partner System ID == %d\n", my_ap_partner_host_info.system_name, my_ap_partner_host_info.node_name, my_ap_partner_host_info.side,
			my_ap_partner_host_info.system_id);
*/







#if 0
	ACS_DSD_Server dsdapi_service;
	ACS_DSD_Session dsdapi_session;

	dsdapi_service.open(acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE, ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_id());

	ACS_DSD_Node my_node;
	dsdapi_service.get_local_node(my_node);
	printf(
			"\nMy Node Name == '%s'\n"
			"My Node State == %d\n"
			"My Node System ID = %d\n"
			"My Node System Name == '%s'\n"
			"My Node System Type == %d\n",	my_node.node_name, my_node.node_state, my_node.system_id, my_node.system_name, my_node.system_type);

	printf("\n\n--->>>>>> PRESS ENTER TO ACCEPT TEST CONNECTIONS..."); getchar();

	dsdapi_service.accept(dsdapi_session);

	ACS_DSD_PrimitiveDataHandler<> pcp_pdh(dsdapi_session);

	ssize_t bytes_received = pcp_pdh.recv_primitive();

	int prim_id = 0;

	if (bytes_received <= 0) {
		printf("** ERROR: 'pcp_pdh.recv_primitive(...' failed: system error == %d\n", errno);
		printf(
			"** ERROR: session last error == %d\n"
			"** ERROR: session last error test == '%s'\n", dsdapi_session.last_error(), dsdapi_session.last_error_text());
	}
	else {
		printf("Bytes received == %zd\n", bytes_received);
		printf("Primitive Data Size == %zu\n", pcp_pdh.primitive_data_size());
		printf("PRIMITIVE DATA:\n");
		for (size_t i = 0; i < pcp_pdh.primitive_data_size(); ++i) printf("%02X ", pcp_pdh.get_buffer()[i]);
		printf("\n");

		prim_id = pcp_pdh.primitive_id();
		unsigned prim_version = pcp_pdh.primitive_version();

		printf("Primitive ID == %d\n", prim_id);
		printf("Primitive Version == %u\n", prim_version);

		char app_service_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME] = {0};
		char app_domain_name[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME] = {0};
		char app_process_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME] = {0};
		uint32_t pid = 0;
		uint8_t visibility = 0;
		uint32_t inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};
		uint16_t inet_ports[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};
		char unix_address[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH] = {0};
		ACE_INET_Addr addr_1;
		ACE_INET_Addr addr_2;
		char addr_1_str[32] = {0};
		char addr_2_str[32] = {0};

		switch (prim_id) {
		case acs_dsd::PCP_DSDAPI_REGISTRATION_INET_REQUEST_ID:
			pcp_pdh.unpack_primitive(prim_id, prim_version, app_service_name, app_domain_name, app_process_name, &pid, &visibility, inet_addresses, inet_ports, inet_addresses + 1, inet_ports + 1, unix_address);
			addr_1.set(inet_ports[0], inet_addresses[0]);
			addr_2.set(inet_ports[1], inet_addresses[1]);
			addr_1.addr_to_string(addr_1_str, 32);
			addr_2.addr_to_string(addr_2_str, 32);
			break;
		case acs_dsd::PCP_DSDAPI_REGISTRATION_UNIX_REQUEST_ID:
			pcp_pdh.unpack_primitive(prim_id, prim_version, app_service_name, app_domain_name, app_process_name, &pid, &visibility, unix_address);
			break;
		default:
			break;
		}

		printf(
				"\nPrimitive ID == %d\n"
				"Primitive Version == %u\n"
				"Application Service Name == '%s'\n"
				"Application Domain Name == '%s'\n"
				"Application Process Name == '%s'\n"
				"Application PID == %d\n"
				"Visibility == %d\n",
				prim_id, prim_version, app_service_name, app_domain_name, app_process_name, pid, visibility);

		switch (prim_id) {
		case acs_dsd::PCP_DSDAPI_REGISTRATION_INET_REQUEST_ID:
			printf(
					"Inet Address 1 == '%s' (0x%08X:0x%04X)\n"
					"Inet Address 2 == '%s' (0x%08X:0x%04X)\n"
					"Internal UNIX Address == '%s'\n",
					addr_1_str, inet_addresses[0], inet_ports[0],
					addr_2_str, inet_addresses[1], inet_ports[1],
					unix_address);
			break;
		case acs_dsd::PCP_DSDAPI_REGISTRATION_UNIX_REQUEST_ID:
			printf("Unix Address == '%s'\n", unix_address);
			break;
		default:
			break;
		}
	}

	dsdapi_session.close();

	dsdapi_service.close();
#endif

	return 0;
}








#if 0
int main(int argc, char * argv[]) {
	//print_configuration();
	//testing();

	ACS_DSD_PCP_PrimitiveDataHandler<> primitive_data_handler;
	size_t primitive_size_in_bytes = 0;

	printf("primitive_data_handler.buffer_size() == %zu\n", primitive_data_handler.buffer_size());
	primitive_data_handler.primitive_size(acs_dsd::PCP_DSDAPI_REGISTRATION_UNIX_REQUEST_ID, 1, primitive_size_in_bytes);
	printf("primitive_size_in_bytes == %zu\n", primitive_size_in_bytes);

	primitive_data_handler.make_primitive(acs_dsd::PCP_DSDAPI_REGISTRATION_UNIX_REQUEST_ID, 1, "CHS", "UNIX", ACS_DSD_ConfigurationHelper::my_process_name(),
			ACS_DSD_ConfigurationHelper::my_process_id(), 0, "app-server-sap-1", "app-server-sap-2");

	for (size_t i = 0; i < primitive_size_in_bytes; ++i)
		printf("%02X ", primitive_data_handler.get_buffer()[i]);

	printf("\n\nTYPE RETURN KEY..."); getchar();







	ACS_DSD_Server server_inet(acs_dsd::SERVICE_MODE_INET_SOCKET);
	ACS_DSD_Server server_unix(acs_dsd::SERVICE_MODE_UNIX_SOCKET);
	int call_result = 0;

	//Opening services
	if ((call_result = server_inet.open("14000") < 0)) { //ERROR
		printf("ERROR: 'server_inet.open(\"14000\")' failed: %s\n", server_inet.last_error_text());
		return -1;
	}

	call_result && printf("WARNING: 'server_inet.open(\"14000\")' warning: %s\n", server_inet.last_error_text());

	if ((call_result =  server_unix.open("app-server-sap") < 0)) { //ERROR
		printf("ERROR: 'server_unix.open(\"app-server-sap\")' failed: %s\n", server_unix.last_error_text());
		server_inet.close();
		return -1;
	}

	call_result && printf("WARNING: 'server_unix.open(\"app-server-sap\")' warning: %s\n", server_unix.last_error_text());

	//Publishing services
	server_inet.publish("CHS", "INET");

	server_unix.publish("CHS", "UNIX");

	printf("TYPE RETURN KEY..."); getchar();


	return 0;
}
#endif



#if 0
namespace {
	void print_configuration () {
		printf("system_ap_nodes_count == %d\n", ACS_DSD_ConfigurationHelper::system_ap_nodes_count());
		printf("system_cp_nodes_count == %d\n", ACS_DSD_ConfigurationHelper::system_cp_nodes_count());
		printf("my_process_id == %d\n", ACS_DSD_ConfigurationHelper::my_process_id());
		printf("my_process_name == '%s'\n", ACS_DSD_ConfigurationHelper::my_process_name());
		printf("unix_socket_root_path == '%s'\n", ACS_DSD_ConfigurationHelper::unix_socket_root_path());

		printf("\nAP NODES\n\n");
		printf("INDEX |   ID |            NAME | SIDE | SYS-ID | SYS-TYPE");
		for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) printf(" |     IPN%d       ", i + 1);

		for (int i = 0; i < ACS_DSD_ConfigurationHelper::system_ap_nodes_count(); ++i) {
			const ACS_DSD_ConfigurationHelper::HostInfo & host_info = ACS_DSD_ConfigurationHelper::get_ap_node(i);
			printf("\n%5d", i);
			printf(" |%5hd", host_info.id);
			printf(" |%16s", host_info.name);
			printf(" |%5hd", host_info.side);
			printf(" |%7hd", host_info.system_id);
			printf(" |%9d", host_info.system_type);
			for (size_t i = 0; i < host_info.ip4_adresses_count; ++i)
				printf(" |%16s", host_info.ip4_addresses_str[i]);
		}

		printf("\n\n");

		printf("\nCP NODES\n\n");
		printf("INDEX |   ID |            NAME | SIDE | SYS-ID | SYS-TYPE");
		for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) printf(" |     IPN%d       ", i + 1);

		for (int i = 0; i < ACS_DSD_ConfigurationHelper::system_cp_nodes_count(); ++i) {
			const ACS_DSD_ConfigurationHelper::HostInfo & host_info = ACS_DSD_ConfigurationHelper::get_cp_node(i);
			printf("\n%5d", i);
			printf(" |%5hd", host_info.id);
			printf(" |%16s", host_info.name);
			printf(" |%5hd", host_info.side);
			printf(" |%7hd", host_info.system_id);
			printf(" |%9d", host_info.system_type);
			for (size_t i = 0; i < host_info.ip4_adresses_count; ++i)
				printf(" |%16s", host_info.ip4_addresses_str[i]);
		}

		printf("\n\n");

		const ACS_DSD_ConfigurationHelper::HostInfo & my_ap_host = ACS_DSD_ConfigurationHelper::get_my_ap_node();
		printf("MY AP NODE IS:\n");
		printf("  ID == %hd\n"
					 "  NAME == '%s'\n", my_ap_host.id, my_ap_host.name);
		for (size_t i = 0; i < my_ap_host.ip4_adresses_count; ++i)
			printf("  IP#%zu: %s (0x%08X)\n", i + 1, my_ap_host.ip4_addresses_str[i], my_ap_host.ip4_addresses[i]);
	}

	void testing () {
		int call_result = 0;
		ACS_DSD_Server server;
		assert(acs_dsd::SERVICE_MODE_UNKNOWN == server.service_mode());

		assert(acs_dsd::SERVER_STATE_CLOSED == server.state());

		assert(0 == server.close());

		(acs_dsd::ERR_SERVICE_MODE_UNKNOWN ^ (call_result = server.open())) &&
				printf("TEST: server.open()\n"
							 "  CALL RESULT == %d\n"
							 "  ERROR CODE == %d\n"
							 "  ERROR TEXT == '%s'\n", call_result, server.last_error(), server.last_error_text());
		assert(acs_dsd::ERR_SERVICE_MODE_UNKNOWN == call_result);

		(acs_dsd::ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE ^ (call_result = server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, "1000"))) &&
				printf("TEST: server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, \"1000\")\n"
							 "  CALL RESULT == %d\n"
							 "  ERROR CODE == %d\n"
							 "  ERROR TEXT == '%s'\n", call_result, server.last_error(), server.last_error_text());
		assert(acs_dsd::ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE == call_result);

		(call_result = server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, "55555")) &&
				printf("TEST: server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, \"55555\")\n"
							 "  CALL RESULT == %d\n"
							 "  ERROR CODE == %d\n"
							 "  ERROR TEXT == '%s'\n", call_result, server.last_error(), server.last_error_text());
		assert(call_result >= 0);
		printf("--->> CHECK POINT [server.open]: TYPE RETURN KEY..."); getchar();

		assert(acs_dsd::SERVICE_MODE_INET_SOCKET == server.service_mode());
		assert(acs_dsd::SERVER_STATE_OPEN == server.state());

		(call_result = server.close()) &&
				printf("TEST: server.close()\n"
							 "  CALL RESULT == %d\n"
							 "  ERROR CODE == %d\n"
							 "  ERROR TEXT == '%s'\n", call_result, server.last_error(), server.last_error_text());
		assert(acs_dsd::ERR_NO_ERRORS == call_result);
		printf("--->> CHECK POINT [server.close]: TYPE RETURN KEY..."); getchar();

		ACS_DSD_Session session;

		(acs_dsd::ERR_SERVER_NOT_OPEN ^ (call_result = server.accept(session))) &&
				printf("TEST: server.accept(session)\n"
							 "  CALL RESULT == %d\n"
							 "  ERROR CODE == %d\n"
							 "  ERROR TEXT == '%s'\n", call_result, server.last_error(), server.last_error_text());
		assert(acs_dsd::ERR_SERVER_NOT_OPEN == call_result);

		(call_result = server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, "44444")) &&
				printf("TEST: server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, \"44444\")\n"
							 "  CALL RESULT == %d\n"
							 "  ERROR CODE == %d\n"
							 "  ERROR TEXT == '%s'\n", call_result, server.last_error(), server.last_error_text());
		assert(call_result >= 0);
		printf("--->> CHECK POINT [server.open]: TYPE RETURN KEY..."); getchar();

		(call_result = server.accept(session)) &&
				printf("TEST: server.accept(session)\n"
							 "  CALL RESULT == %d\n"
							 "  ERROR CODE == %d\n"
							 "  ERROR TEXT == '%s'\n", call_result, server.last_error(), server.last_error_text());
		assert(call_result == 0);
		printf("--->> CHECK POINT [server.accept]: TYPE RETURN KEY..."); getchar();
	}
}
#endif
