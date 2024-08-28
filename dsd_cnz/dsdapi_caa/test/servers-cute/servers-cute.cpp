#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ServerCuteTester.h"

typedef ServerCuteTester<ACS_DSD_SocketAcceptor, ACS_DSD_SocketStream, ACE_INET_Addr> socket_acceptor_tester_t;
typedef ServerCuteTester<ACS_DSD_LSocketAcceptor, ACS_DSD_LSocketStream, ACE_UNIX_Addr> lsocket_acceptor_tester_t;

namespace {
	void inet_server_test_dispatcher() {
		socket_acceptor_tester_t sock_tester;
		sock_tester();
	}

	void unix_server_test_dispatcher() {
		lsocket_acceptor_tester_t lsock_tester;
		lsock_tester();
	}
}

int main(int argc, char * argv[]) {
	cute::suite s;

	s.push_back(cute::test(inet_server_test_dispatcher, "'INET Socket Type Server' Test"));
	s.push_back(cute::test(unix_server_test_dispatcher, "'UNIX Domain Socket Type Server' Test"));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[Servers Cute Test Suite]");

	return 0;
}
