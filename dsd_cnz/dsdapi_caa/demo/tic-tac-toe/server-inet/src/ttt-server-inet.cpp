//Solo per testare i problemi sul linkage usando il PRC

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include <iostream>

#include "ace/Time_Value.h"
#include "ace/INET_Addr.h"

#include "ACS_DSD_Server.h"
#include "ACS_DSD_Session.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_CPStateNotifier.h"

namespace {
	void print_host_info (ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t host_info_ptr) {
		printf(
				"HOST INFOs:\n"
				"  node_name == '%s'\n"
				"  side == %d\n"
				"  system_id == %d\n"
				"  system_name == '%s'\n"
				"  system_type == %d\n", host_info_ptr->node_name, host_info_ptr->side, host_info_ptr->system_id,
				host_info_ptr->system_name, host_info_ptr->system_type);

		printf("  IP4 ADDRESSES [%zu]\n", host_info_ptr->ip4_adresses_count);
		for (size_t i = 0; i < host_info_ptr->ip4_adresses_count; ++i)
			printf("    IP [%zu] == '%s' (0x%08X)\n", i, host_info_ptr->ip4_addresses_str[i], host_info_ptr->ip4_addresses[i]);

		printf("  IP4 ALIASES [%zu]\n", host_info_ptr->ip4_aliases_count);
		for (size_t i = 0; i < host_info_ptr->ip4_aliases_count; ++i)
			printf("    ALIAS [%zu] == '%s' (0x%08X)\n", i, host_info_ptr->ip4_aliases_str[i], host_info_ptr->ip4_aliases[i]);
		printf("----------------------------------------------------\n");
	}
}

int main (int /*argc*/, char * /*argv*/[]) {
	int call_result = 0;

	ACS_DSD_Server server;
	ACS_DSD_Session session;
	call_result = __COUNTER__;



/*
	printf("system_ap_nodes_count == %u\n", ACS_DSD_ConfigurationHelper::system_ap_nodes_count());
	printf("system_cp_nodes_count == %u\n", ACS_DSD_ConfigurationHelper::system_cp_nodes_count());

	printf("AP system id: min == %d, max ==  %d\n", ACS_DSD_ConfigurationHelper::get_ap_id_min(), ACS_DSD_ConfigurationHelper::get_ap_id_max());
	printf("BC system id: min == %d, max ==  %d\n", ACS_DSD_ConfigurationHelper::get_bc_id_min(), ACS_DSD_ConfigurationHelper::get_bc_id_max());
	printf("CP system id: min == %d, max ==  %d\n", ACS_DSD_ConfigurationHelper::get_cp_id_min(), ACS_DSD_ConfigurationHelper::get_cp_id_max());

	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t host_info_ptr = 0;

	printf(
			"\nAP NODES\n"
			"====================================================\n"
	);
	for (unsigned i = 0; i < ACS_DSD_ConfigurationHelper::system_ap_nodes_count(); ++i) {
		ACS_DSD_ConfigurationHelper::get_ap_node(i, host_info_ptr);
		print_host_info(host_info_ptr);
		printf("PRESS RETURN KEY"); getchar();
	}

	printf(
			"\nCP/BC NODES\n"
			"====================================================\n"
	);
	for (unsigned i = 0; i < ACS_DSD_ConfigurationHelper::system_cp_nodes_count(); ++i) {
		ACS_DSD_ConfigurationHelper::get_cp_node(i, host_info_ptr);
		print_host_info(host_info_ptr);
		printf("PRESS RETURN KEY"); getchar();
	}

	printf("====================================================\n\n");

	printf("PRESS RETURN KEY"); getchar();
*/


	for (unsigned counter = 1; ; ::sleep(1), ++counter) {
		std::cout << "\nLOOP " << counter << std::endl;

		std::cout << "Opening the ttt-inet@games service...";
		call_result = server.open(acs_dsd::SERVICE_MODE_INET_SOCKET);
		std::cout << (call_result < 0 ? "KO" : "OK") << "!!" << std::endl;
		if (call_result) {
			std::cout << "## " << (call_result < 0 ? "ERROR" : "WARNING") << ": calling 'open'" << std::endl;
			std::cout << "   last error == " << server.last_error() << std::endl;
			std::cout << "   last error text == '" << server.last_error_text() << "'" << std::endl;
			if (call_result < 0) continue;
		}
		std::cout << "Service ttt-inet@games opened correctly" << std::endl;

		for (unsigned counter = 1; ; ::sleep(1), ++counter) {
			std::cout << counter << ") Publishing as ttt-inet@games service...";
			call_result = server.publish("ttt-inet", "games");
			std::cout << (call_result < 0 ? "KO" : "OK") << "!!" << std::endl;
			if (call_result) {
				std::cout << "## " << (call_result < 0 ? "ERROR" : "WARNING") << ": calling 'publish'" << std::endl;
				std::cout << "   last error == " << server.last_error() << std::endl;
				std::cout << "   last error text == '" << server.last_error_text() << "'" << std::endl;
				if (call_result < 0) continue;
			}
			std::cout << "ttt-inet@games service published correctly." << std::endl
								<< "Check using the dsdls command then hit RETURN key...";
			::getchar();

			// Here the user has typed the RETURN key

			for (;;) {
				std::cout << counter << ") Unregistering the ttt-inet@games service...";
				call_result = server.unregister();
				std::cout << (call_result < 0 ? "KO" : "OK") << "!!" << std::endl;
				if (call_result) {
					std::cout << "## " << (call_result < 0 ? "ERROR" : "WARNING") << ": calling 'unregister'" << std::endl;
					std::cout << "   last error == " << server.last_error() << std::endl;
					std::cout << "   last error text == '" << server.last_error_text() << "'" << std::endl;
					if (call_result < 0) continue;
				}
				break;
			}

			std::cout << "ttt-inet@games service unregistered correctly." << std::endl
								<< "Check using the dsdls command then hit RETURN key..." << std::endl;
			::getchar();

			// Here the user has typed the RETURN key
		}

		std::cout << "Closing the ttt-inet@games service...";
		call_result = server.close();
		std::cout << (call_result < 0 ? "KO" : "OK") << "!!" << std::endl;
		if (call_result) {
			std::cout << "## " << (call_result < 0 ? "ERROR" : "WARNING") << ": calling 'close'" << std::endl;
			std::cout << "   last error == " << server.last_error() << std::endl;
			std::cout << "   last error text == '" << server.last_error_text() << "'" << std::endl;
			if (call_result < 0) return __COUNTER__;
		}
		std::cout << "Service ttt-inet@games closed correctly" << std::endl;
		break;
	}















/*
	printf("Waiting a client to connect...");
	fflush(stdout);
	call_result = server.accept(session);
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'server.accept(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());
		return -3;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'server.accept(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());



	int tcp_nodelay_flag = 1;
	printf("Setting the TCP_NODELAY flag on the session object...");
	call_result = session.set_option(IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay_flag, sizeof(tcp_nodelay_flag));
	if (call_result < 0) //ERROR
		printf(
				"KO!!## WARNING: call 'session.set_option(...' failed\n"
				"      last error == %d\n"
				"      last error text == '%s'\n"
				"      call_result == %d\n"
				"      errno == %d\n", session.last_error(), session.last_error_text(), call_result, errno);
	else printf("OK!\n\n");

	int flag_length = sizeof(tcp_nodelay_flag);
	printf("Gettint the TCP_NODELAY flag on the session object...");
	call_result = session.get_option(IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay_flag, &flag_length);
	if (call_result < 0) //ERROR
		printf(
				"KO!!## WARNING: call 'session.get_option(...' failed\n"
				"      last error == %d\n"
				"      last error text == '%s'\n"
				"      call_result == %d\n"
				"      errno == %d\n", session.last_error(), session.last_error_text(), call_result, errno);
	else
		printf(
				"OK!\n"
				"      TCP_NODELAY flag is %s\n", tcp_nodelay_flag ? "TRUE" : "FALSE");




	char buffer[4096] = {0};
	ssize_t bytes_received = 0;

	printf("Receiving client message...");
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
			"I have received %zd bytes from the client\n"
			"Client message == '%s'\n", bytes_received, buffer);

	printf("Sending my message 'Welcome client' to the client...");
	fflush(stdout);
	ssize_t bytes_sent = session.send("Welcome client", 15);
	if (bytes_sent < 0) {
		printf(
				"KO!!## ERROR: 'session.send(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", session.last_error(), session.last_error_text());
		return -5;
	}

	printf(
			"OK!\n"
			"I have sent %zd bytes to the client\n", bytes_sent);

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

	printf("Unregistering the server...");
	call_result = server.unregister();
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'server.unregister(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());
		return -6;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'server.unregister(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());

	printf("Closing the server...");
	call_result = server.close();
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'server.close(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());
		return -7;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'server.close(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());
*/


	return 0;
}
















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
