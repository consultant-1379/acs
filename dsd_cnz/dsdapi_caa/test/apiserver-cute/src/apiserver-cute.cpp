#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ServerCuteTester.h"

namespace {
	void inet_server_test_dispatcher() {
		ServerCuteTester tester;
		tester();
	}
}

int main(int /*argc*/, char * /*argv*/[]) {
	cute::suite s;

	s.push_back(cute::test(inet_server_test_dispatcher, "'DSD API Server and Session classes' Test"));

	printf("PROCESS ID == %d\n", getpid());

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[DSD API Servers Cute Test Suite]");

	return 0;
}





/*
#include <stdio.h>
#include <unistd.h>

#include "ACS_DSD_Server.h"
#include "ACS_DSD_Session.h"

int main (int argc, char * argv[]) {
	ACS_DSD_Server server;
	ACS_DSD_Session session;

	server.open(acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE);

	server.publish("CHS", "MTAP");

	int server_handles_count = 8;
	acs_dsd::HANDLE server_handles[server_handles_count] = {acs_dsd::INVALID_HANDLE};
	server.get_handles(server_handles, server_handles_count);

	server.accept(session);


	return 0;
}
*/
