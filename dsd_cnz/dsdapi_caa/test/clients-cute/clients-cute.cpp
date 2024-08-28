#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ClientCuteTester.h"

typedef ClientCuteTester<ACS_DSD_SocketConnector, ACS_DSD_SocketStream, ACE_INET_Addr> socket_connector_tester_t;
typedef ClientCuteTester<ACS_DSD_LSocketConnector, ACS_DSD_LSocketStream, ACE_UNIX_Addr> lsocket_connector_tester_t;

namespace {
	void inet_client_test_dispatcher() {
		socket_connector_tester_t sock_tester;
		sock_tester();
	}

	void unix_client_test_dispatcher() {
		lsocket_connector_tester_t lsock_tester;
		lsock_tester();
	}
}

int main(int argc, char * argv[]) {
	cute::suite s;

	s.push_back(cute::test(inet_client_test_dispatcher, "'INET Socket Type Client' Test"));
	s.push_back(cute::test(unix_client_test_dispatcher, "'UNIX Domain Socket Type Client' Test"));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[Clients Cute Test Suite]");

	return 0;
}
