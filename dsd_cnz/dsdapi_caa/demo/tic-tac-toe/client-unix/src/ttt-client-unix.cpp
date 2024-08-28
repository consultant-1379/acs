#include <cstdio>

#include "ACS_DSD_Client.h"
#include "ACS_DSD_Session.h"

int main (int argc, char * argv[]) {
	ACS_DSD_Client client;
	ACS_DSD_Session session;
	int call_result = 0;

	printf("Connecting to the service 'ttt-unix@games'...");
	call_result = client.connect(session, "ttt-unix", "games");
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'client.connect(...' failed!\n"
				"      last error == %d\n"
				"      laste error text == '%s'\n", client.last_error(), client.last_error_text());
		return -1;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'client.connect(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", client.last_error(), client.last_error_text());

	ssize_t bytes_sent = 0;

	printf("Sending my message 'Hello server I'm the client' to the server...");
	bytes_sent = session.send("Hello server I'm the client", 28);
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'session.send(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", session.last_error(), session.last_error_text());
		return -1;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'session.send(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", session.last_error(), session.last_error_text());

	char buffer[4096] = {0};
	ssize_t bytes_received = 0;

	printf("Receiving the response from the server...");
	fflush(stdout);
	bytes_received = session.recv(buffer, ACS_DSD_ARRAY_SIZE(buffer));
	if (bytes_received < 0) {
		printf(
				"KO!!## ERROR: 'session.recv(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", session.last_error(), session.last_error_text());
		return -4;
	}

	printf(
			"OK!\n"
			"I have received %zd bytes from the server\n"
			"Server message == '%s'\n", bytes_received, buffer);

	printf("Closing the communication session...");
	call_result = session.close();
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'session.close(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", session.last_error(), session.last_error_text());
		return -5;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'session.close(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", session.last_error(), session.last_error_text());

	return 0;
}








/*
int main (int argc, char * argv[]) {
	ACS_DSD_Client client;
	ACS_DSD_Session session;
	ACS_DSD_Node local_node;

	client.get_local_node(local_node);
	printf(
			"MY LOCAL NODE INFO:\n"
			"  node_name == '%s'\n"
			"  node_side == %d\n"
			"  node_state == %d\n"
			"  system_id == %d\n"
			"  system_name == '%s'\n"
			"  system_type == %d\n\n", local_node.node_name, local_node.node_side, local_node.node_state, local_node.system_id, local_node.system_name,
			local_node.system_type);

	printf("Connecting to the service %s@%s on [SYSTEM_ID_THIS_NODE, NODE_STATE_PASSIVE]...\n\n", argv[1], argv[2]);

	int call_result = client.connect(session, argv[1], argv[2], acs_dsd::SYSTEM_ID_THIS_NODE, acs_dsd::NODE_STATE_PASSIVE);


	if (call_result)
		printf(
				"\nKO!!!\n\n"
				"ERROR: 'client.connect(...' failed!\n"
				"  call_result == %d"
				"  last error == %d\n"
				"  last error text == '%s'\n", call_result, client.last_error(), client.last_error_text());
	else {
		printf("\nOK!!!\n\n");
		session.close();
	}

	return 0;
}
*/

/*
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Server.h"

namespace {
	void print_ap_node_info(const ACS_DSD_ConfigurationHelper::NodeInfo & ap_node) {
		printf("  name == '%s'\n"
					 "  ip address count == %zu\n", ap_node.name, ap_node.ip4_adresses_count);
		for (size_t i = 0; i < ap_node.ip4_adresses_count; ++i)
			printf("  IP[%zu] == '%s' (0x%08X), NETMASK == '%s' (0x%08X)\n", i,
						 ap_node.ip4_addresses_str[i], ap_node.ip4_addresses[i], ap_node.ip4_netmasks_str[i], ap_node.ip4_netmasks[i]);
	}
}

int main(int argc, char * argv[]) {
	printf("In server main\n");

	printf("## UNIX SOCK NAME RESERVED SIZE == %d\n", acs_dsd::UNIX_SOCK_NAME_SIZE_MAX - acs_dsd::UNIX_SOCK_NAME_SIZE_USER);

	//ACS_DSD_Server ttt_server(acs_dsd::SERVICE_MODE_INET_SOCKET);

	printf("networks_count == %zd\n\n", ACS_DSD_ConfigurationHelper::networks_count());
	for (size_t index = 0; index < ACS_DSD_ConfigurationHelper::networks_count(); ++index) {
		printf("Network #%zd\n"
					 "  address_ip4 == 0x%X\n"
					 "  address_ip4_str == '%s'\n"
					 "  broadcast_ip4 == 0x%X\n"
				 	 "  broadcast_ip4_str == '%s'\n"
				 	 "  name == '%s'\n"
				 	 "  netmask_bits == %d\n"
				 	 "  netmask_ip4 == 0x%X\n"
				 	 "  netmask_ip4_str == '%s'\n", index, ACS_DSD_ConfigurationHelper::networks()[index].ip4_address, ACS_DSD_ConfigurationHelper::networks()[index].ip4_address_str,
					 ACS_DSD_ConfigurationHelper::networks()[index].ip4_broadcast, ACS_DSD_ConfigurationHelper::networks()[index].ip4_broadcast_str,
					 ACS_DSD_ConfigurationHelper::networks()[index].name, ACS_DSD_ConfigurationHelper::networks()[index].netmask_bits,
					 ACS_DSD_ConfigurationHelper::networks()[index].ip4_netmask, ACS_DSD_ConfigurationHelper::networks()[index].ip4_netmask_str);
	}

	printf("\nMY AP NODE INFO\n");
	print_ap_node_info(ACS_DSD_ConfigurationHelper::my_ap_node());
	printf("\nMY AP PEER INFO\n");
	print_ap_node_info(ACS_DSD_ConfigurationHelper::my_ap_peer());



	ACS_DSD_Server ttt_server(acs_dsd::SERVICE_MODE_INET_SOCKET);

	int call_result = ttt_server.open("65535");

	printf("call_result == %d\n", call_result);


	return 0;
}
*/
