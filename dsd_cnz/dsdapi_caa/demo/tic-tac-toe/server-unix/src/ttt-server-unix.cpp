#include <errno.h>
#include <cstdio>
#include <stdlib.h>

#include "ace/Time_Value.h"
#include "ace/INET_Addr.h"

//#include "ACS_DSD_SocketAcceptor.h" //TO BE DELETED
//#include "ACS_DSD_NumberSequencer.h"

//#include "ACS_TRA_trace.h"

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

//	ACS_TRA_trace trace_test("TRACE_TEST", "C512");
}

int main (int argc, char * argv[]) {
/*
	ACS_DSD_SocketAcceptor acceptor_1;
	ACS_DSD_SocketAcceptor acceptor_2;
	ACE_INET_Addr addr("192.168.169.1:12345");
	char buf[1024] = {0};

	printf("program_invocation_name == '%s'\n", program_invocation_name);
	printf("program_invocation_short_name == '%s'\n", program_invocation_short_name);
	printf("my_process_name == '%s'\n", ACS_DSD_ConfigurationHelper::my_process_name());

	printf("getpid == %d\n", getpid());
	printf("my_process_id == %d\n", ACS_DSD_ConfigurationHelper::my_process_id());

	errno = ENOENT;
	int result = acceptor_1.open(addr);
	printf("result 1 == %d, system error == %d, system error text == '%s'\n", result, errno, strerror_r(errno, buf, 1024));
	getchar();

	errno = 0;
	result = acceptor_2.open(addr);
	printf("result 2 == %d, system error == %d, system error text == '%s'\n", result, errno, strerror_r(errno, buf, 1024));
	getchar();
*/

/*
	ACS_DSD_NumberSequencer sequencer(33, 42);
	for (int i = 0; i < 20; ++i)
		printf("SEQUENCER #%02d == %u\n", i, sequencer.next());
	getchar();
	getchar();
	getchar();
*/

/*
	printf("##### TEST TRA API #####\n\n");

	if (trace_test.ACS_TRA_ON()) {
		trace_test.ACS_TRA_event(1, "#### TEST TRA API - method ACS_TRA_event(...) ####\n");
		printf("#### TEST 'trace_test.ACS_TRA_event(...' OK ####\n");
	}
*/


	int call_result = 0;

/*
	ACS_DSD_CPStateNotifier notifier;
	acs_dsd::CPStateConstants cp_state = acs_dsd::CP_STATE_UNKNOWN;

	printf("notifier.system_id() == %d, notifier.state() == %d\n", notifier.system_id(), notifier.state());

	call_result = notifier.attach_to(1001);
	if (call_result) {
		printf("### ERROR: 'notifier.attach_to(...' failed: last error == %d, last error text == '%s'\n",
			notifier.last_error(), notifier.last_error_text());
		return -__LINE__;
	}
*/


	ACS_DSD_Server server;
	ACS_DSD_Session session;
	call_result = 0;

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





	printf("Opening...");
	call_result = server.open(acs_dsd::SERVICE_MODE_UNIX_SOCKET);
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'server.open(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());
		return -1;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'server.open(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());

	printf("Publishing as 'ttt-unix@games' service...");
	call_result = server.publish("ttt-unix", "games");
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'server.publish(...' failed!\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());
		printf("Hit RETURN key..."); getchar();
		return -2;
	}

	printf("OK!\n");
	if (call_result)
		printf(
				"## WARNING: 'server.publish(...' returned a warning\n"
				"      last error == %d\n"
				"      last error text == '%s'\n", server.last_error(), server.last_error_text());

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
