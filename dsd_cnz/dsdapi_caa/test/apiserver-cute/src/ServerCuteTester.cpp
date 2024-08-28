#include "ServerCuteTester.h"

/*
#define TEST_PRINT_ERROR(class_obj, method, ...) { \
		printf("\nTESTING [" ACS_DSD_STRINGIZE(class_obj) "." ACS_DSD_STRINGIZE(method) "(...)] --> "); \
		fflush(stdout); \
		if ((call_result = class_obj.method(__VA_ARGS__))) \
			printf("%s\n  %s at line %d\n" \
				"    last_error == %d\n    last_error_text == '%s'\n", (call_result < 0 ? "ERR" : "WAR"), \
				(call_result < 0 ? "ERROR" : "WARNING"), __LINE__, class_obj.last_error(), class_obj.last_error_text()); \
		else \
			printf("OK!\n"); \
	}

#define CHECK_POINT_PAUSE printf("\nCHECK-POINT --> Press RETURN when ready..."); getchar()
*/

namespace {
	void print_node_info (const ACS_DSD_Node & node) {
		printf(
			"NODE INFO == {system_id == %d, system_type == %d, system_name == '%s', node_name == '%s', node_side == %d, node_state == %d}\n",
			node.system_id, node.system_type, node.system_name, node.node_name, node.node_side, node.node_state
		);
	}
}

void __CLASS_NAME__::operator() () {
	char node_char = '\0';
	char node_name[16] = {0};

	snprintf(node_name, 16, "CP1%c", node_char);
	printf("node_name == '%s'\n", node_name);







	std::cout << "--->>> Press ENTER key to follow the basic unit test about the DSD API Server and Session classes functionalities at server side...";
	getchar();

	int call_result = 0;

	//test_configuration_01();

	ACS_DSD_Node node;
	ACS_DSD_Server server;
	ACS_DSD_Session session;

	TEST_PRINT_ERROR(server, get_local_node, node);
	ASSERT(call_result == 0);
	print_node_info(node);

	TEST_PRINT_ERROR(server, open, acs_dsd::SERVICE_MODE_INET_SOCKET, "55555");
	ASSERT(call_result >= 0);

	CHECK_POINT_PAUSE;

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_OPEN);

	TEST_PRINT_ERROR(server, publish, "CHS7", "INET");
	ASSERT(call_result == acs_dsd::ERR_NO_ERRORS);

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_PUBLISHED);

//	TEST_PRINT_ERROR(server, accept, session, 4000);
//	ASSERT(call_result == acs_dsd::WAR_TIMEOUT_EXPIRED);

//	TEST_PRINT_ERROR(server, accept, session);
//	ASSERT(call_result == 0);

//	CHECK_POINT_PAUSE;

//	TEST_PRINT_ERROR(session, state);
//	ASSERT(call_result == acs_dsd::SESSION_STATE_CONNECTED);

//	char buffer[1024];
//	TEST_PRINT_ERROR(session, recv, buffer, 1024);
//	ASSERT(call_result == 5);
//	printf("---> received the message '%s'\n", buffer);

//	CHECK_POINT_PAUSE;

//	TEST_PRINT_ERROR(session, close);
//	ASSERT(call_result == 0);

	CHECK_POINT_PAUSE;

	TEST_PRINT_ERROR(server, unregister);
	ASSERT(call_result == acs_dsd::ERR_NO_ERRORS);

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_OPEN);

	CHECK_POINT_PAUSE;

	TEST_PRINT_ERROR(server, close);
	ASSERT(call_result == 0);

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_CLOSED);

	CHECK_POINT_PAUSE;

	//UNIX LOCAL SOCKET TESTS
	TEST_PRINT_ERROR(server, open, acs_dsd::SERVICE_MODE_UNIX_SOCKET, "paperopoli");
	ASSERT(call_result >= 0);

	CHECK_POINT_PAUSE;

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_OPEN);

	TEST_PRINT_ERROR(server, publish, "CHS7", "UNIX");
	ASSERT(call_result == acs_dsd::ERR_NO_ERRORS);

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_PUBLISHED);

	CHECK_POINT_PAUSE;

	TEST_PRINT_ERROR(server, unregister);
	ASSERT(call_result == acs_dsd::ERR_NO_ERRORS);

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_OPEN);

	CHECK_POINT_PAUSE;

	TEST_PRINT_ERROR(server, close);
	ASSERT(call_result == 0);

	TEST_PRINT_ERROR(server, state);
	ASSERT(call_result == acs_dsd::SERVER_STATE_CLOSED);

	printf("\nType RETURN key..."); getchar(), (std::cout << std::endl);
}






/*
void __CLASS_NAME__::test_configuration_01 () {
	printf("TESTING : %s --> OK\n", __FUNCTION__);

	const ACS_DSD_ConfigurationHelper::NetworkInfo * networks = ACS_DSD_ConfigurationHelper::networks();

	printf("networks_count == %zu\n", ACS_DSD_ConfigurationHelper::networks_count());
	for (size_t i = 0; i < ACS_DSD_ConfigurationHelper::networks_count(); ++i) {
		printf("  NET #%zu\n"
					 "    name == '%s'\n"
					 "    IP4 == 0x%08X (%s)\n"
					 "    IP4 Broadcast == 0x%08X (%s)\n"
					 "    Netmask == 0x%08X (%s)\n"
					 "    Network bits == %d\n",
			i, networks[i].name, networks[i].ip4_address, networks[i].ip4_address_str, networks[i].ip4_broadcast,
			networks[i].ip4_broadcast_str, networks[i].ip4_netmask, networks[i].ip4_netmask_str, networks[i].netmask_bits);
	}

	const ACS_DSD_ConfigurationHelper::HostInfo * ap_info = &ACS_DSD_ConfigurationHelper::my_ap_node();

	printf("My AP Information:\n");
	printf("  Name == '%s'\n"
				 "  IP4 Address Count == %zu\n",
		ap_info->name, ap_info->ip4_adresses_count);
	for (size_t i = 0; i < ap_info->ip4_adresses_count; ++i)
		printf("    NET #%zu\n"
					 "      IP4 == 0x%08X (%s)\n"
					 "      Netmask == 0x%08X (%s)\n",
			i, ap_info->ip4_addresses[i], ap_info->ip4_addresses_str[i], ap_info->ip4_netmasks[i], ap_info->ip4_netmasks_str[i]);

	printf("My peer AP Information:\n");
	ap_info = &ACS_DSD_ConfigurationHelper::my_ap_peer();
	printf("  Name == '%s'\n"
				 "  IP4 Address Count == %zu\n",
		ap_info->name, ap_info->ip4_adresses_count);
	for (size_t i = 0; i < ap_info->ip4_adresses_count; ++i)
		printf("    NET #%zu\n"
					 "      IP4 == 0x%08X (%s)\n"
					 "      Netmask == 0x%08X (%s)\n",
			i, ap_info->ip4_addresses[i], ap_info->ip4_addresses_str[i], ap_info->ip4_netmasks[i], ap_info->ip4_netmasks_str[i]);
}
*/

/*
void __CLASS_NAME__::test_server_state_01 (acs_dsd::ServerStateConstants check_value) {
	acs_dsd::ServerStateConstants value = _inet_server.state();
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(check_value == value));
	ASSERT_EQUAL(check_value, value);
}

void __CLASS_NAME__::test_server_service_mode_01 (acs_dsd::ServiceModeConstants check_value) {
	acs_dsd::ServiceModeConstants value = _inet_server.service_mode();
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(check_value == value));
	ASSERT_EQUAL(check_value, value);
}

void __CLASS_NAME__::test_server_open_01 (acs_dsd::ErrorConstants check_value) {
	int value = _inet_server.open();
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(check_value == value));
	ASSERT_EQUAL(check_value, value);
}

void __CLASS_NAME__::test_server_open_02 (acs_dsd::ErrorConstants check_value) {
	int value = _inet_server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, "32fr");
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(check_value == value));
	ASSERT_EQUAL(check_value, value);
}

void __CLASS_NAME__::test_server_open_03 (acs_dsd::ErrorConstants check_value) {
	int value = _inet_server.open(acs_dsd::SERVICE_MODE_INET_SOCKET, "111");
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(check_value == value));
	ASSERT_EQUAL(check_value, value);
}

void __CLASS_NAME__::test_server_open_04 (acs_dsd::ServiceModeConstants service_mode, const char * service_id = 0) {
	int value = service_id ? _inet_server.open(service_mode, service_id) : _inet_server.open(acs_dsd::SERVICE_MODE_INET_SOCKET);
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(value >= 0));
	if (value) printf("  last error == %d\n"
										"  last error text == '%s'\n", _inet_server.last_error(), _inet_server.last_error_text());
	printf("-- CHECK POINT: type RETURN to continue..."); getchar();
	ASSERT(value >= 0);
}

void __CLASS_NAME__::test_server_close_01 () {
	int value = _inet_server.close();
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(0 == value));
	if (value) printf("  last error == %d\n"
										"  last error text == '%s'\n", _inet_server.last_error(), _inet_server.last_error_text());
	printf("-- CHECK POINT: type RETURN to continue..."); getchar();
	ASSERT(0 == value);
}

void __CLASS_NAME__::test_server_accept_01 (acs_dsd::ErrorConstants check_value, unsigned timeout_ms = 0) {
	int value = _inet_server.accept(_session, timeout_ms);
	printf("TESTING : %s --> %s\n", __FUNCTION__, CHECK_TEST_STR(check_value == value));
	ASSERT_EQUAL(check_value, value);
}
*/



#if 0 //OLD
virtual void __CLASS_NAME__::operator() () {

/* TEST: possible ACE bug on connect with timeout
	ACS_DSD_SocketStream stream;
	ACE_INET_Addr inet_address("192.168.169.1:44444");
	ACE_Time_Value timeout(5);

	printf("timeout mseconds == %d\n", timeout.get_msec());

	errno = 0;
	call_result = (ACS_DSD_SocketConnector()).connect(stream, inet_address, &timeout);
	printf("\n\nTEST timeout: call_result == %d, errno == %d, system error message == '%s'\n\n", call_result, errno, strerror(errno));

	errno = 0;
	call_result = (ACS_DSD_SocketConnector()).connect(stream, inet_address);
	printf("\n\nTEST timeout: call_result == %d, errno == %d, system error message == '%s'\n\n", call_result, errno, strerror(errno));

	ACE_INET_Addr inet_remote_addr;
	stream.get_remote_addr(inet_remote_addr);

	printf("REMOTE ADDRESS == %u, PORT == %u\n", inet_remote_addr.get_ip_address(), inet_remote_addr.get_port_number());
*/




	//TEST: ACS_DSD_Server::service_mode
	ACS_DSD_Server server(acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE);
	ASSERT(acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE == server.service_mode());

	//TEST: ACS_DSD_Server::service_mode
	acs_dsd::ServiceModeConstants service_mode = _inet_server.service_mode();
	ASSERT(service_mode == acs_dsd::SERVICE_MODE_UNKNOWN);

	ACS_DSD_Node local_node;

	//TEST: ACS_DSD_Server::get_local_node
	TEST_PRINT_ERROR(_inet_server, get_local_node, local_node);
	ASSERT(acs_dsd::ERR_NO_ERRORS == call_result);
	printf(
			"** LOCAL NODE INFO **\n"
			"    Node name == '%s'\n"
			"    Node state == %d\n"
			"    System ID == %d\n"
			"    System type == %d\n", local_node.node_name, local_node.node_state, local_node.system_id, local_node.system_type);

	//TEST: ACS_DSD_Server::state
	ASSERT(acs_dsd::SERVER_STATE_CLOSED == _inet_server.state());

	//TEST: ACS_DSD_Server::close
	ASSERT(0 == _inet_server.close());

	//TEST: ACS_DSD_Server::publish
	TEST_PRINT_ERROR(_inet_server, publish, "CHS7", "MTAP");
	ASSERT(acs_dsd::ERR_SERVER_NOT_OPEN == call_result);

	//TEST: ACS_DSD_Server::open inet
	TEST_PRINT_ERROR(_inet_server, open, acs_dsd::SERVICE_MODE_INET_SOCKET, "12345");
	ASSERT(call_result >= 0);

	//TEST: ACS_DSD_Server::publish inet
	TEST_PRINT_ERROR(_inet_server, publish, 0, 0);
	ASSERT(acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL == call_result);

	//TEST: ACS_DSD_Server::publish inet
	TEST_PRINT_ERROR(_inet_server, publish, "CHS7", "MTAP");
	ASSERT(acs_dsd::ERR_NO_ERRORS == call_result);




	printf("Pause: restart the dsd light server and press ENTER key..."); getchar();

	//TEST: ACS_DSD_Server::open unix
	TEST_PRINT_ERROR(_unix_server, open, acs_dsd::SERVICE_MODE_UNIX_SOCKET, "test_unix_service");
	ASSERT(call_result >= 0);

	//TEST: ACS_DSD_Server::publish unix
	TEST_PRINT_ERROR(_unix_server, publish, "pippo", "UNIX");
	ASSERT(acs_dsd::ERR_NO_ERRORS == call_result);




/*
	test_server_state_01(acs_dsd::SERVER_STATE_CLOSED);
	test_server_service_mode_01(acs_dsd::SERVICE_MODE_UNKNOWN);
	test_server_open_01(acs_dsd::ERR_SERVICE_MODE_UNKNOWN);
	test_server_open_02(acs_dsd::ERR_SERVICE_ACCESS_POINT_NOT_VALID);
	test_server_open_03(acs_dsd::ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE);

	test_server_accept_01(acs_dsd::ERR_SERVER_NOT_OPEN);

	test_server_open_04(acs_dsd::SERVICE_MODE_INET_SOCKET);
	test_server_state_01(acs_dsd::SERVER_STATE_OPEN);
	test_server_service_mode_01(acs_dsd::SERVICE_MODE_INET_SOCKET);

	test_server_accept_01(acs_dsd::ERR_NO_ERRORS, 60000);

	test_server_close_01();
	test_server_state_01(acs_dsd::SERVER_STATE_CLOSED);
*/

}
#endif
