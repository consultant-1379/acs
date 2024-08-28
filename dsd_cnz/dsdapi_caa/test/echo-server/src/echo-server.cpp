#include <stdio.h>
#include <stdlib.h>

#include "ACS_DSD_Server.h"
#include "ACS_DSD_Session.h"

namespace {
	void do_echo (ACS_DSD_Session & echo_session) {
		int call_result = 0;
		ACS_DSD_Node remote_node;

		if ((call_result = echo_session.get_remote_node(remote_node))) {
			// ERROR: Getting remote node info
			::printf(
					"## ERROR: Call 'get_remote_node' failed\n"
					"     Error code == %d\n"
					"     Error text == '%s'\n", echo_session.last_error(), echo_session.last_error_text());
		} else
			::printf(
					"  Client INFO:\n"
					"    system_id == %d\n"
					"    system_name == '%s'\n"
					"    node_name == '%s'\n\n", remote_node.system_id, remote_node.system_name, remote_node.node_name);

		for (unsigned message_counter = 1; ; ++message_counter) {
			char buffer[1024];

			// Waiting client message
			ssize_t bytes_received = echo_session.recv(buffer, sizeof(buffer));
			if (bytes_received < 0) { // ERROR: Receiving client message
				::printf(
						"## ERROR: Receiving client message\n"
						"     Error code == %d\n"
						"     Error text == '%s'\n", echo_session.last_error(), echo_session.last_error_text());
				break;
			}

			buffer[bytes_received] = buffer[sizeof(buffer)] = 0;
			::printf("CLIENT MESSAGE #%u: [%zd]: '%s'", message_counter, bytes_received, buffer);

			::printf("SERVER REPLY #%u: [%zd]: '%s'", message_counter, bytes_received, buffer);

			ssize_t bytes_sent = echo_session.send(buffer, bytes_received);
			if (bytes_sent < 0) { // ERROR: Sending reply
				::printf(
						"## ERROR: Sending the reply echo message\n"
						"     Error code == %d\n"
						"     Error text == '%s'\n", echo_session.last_error(), echo_session.last_error_text());
				break;
			}
		}
	}
}

int main (int argc, char * argv []) {
	ACS_DSD_Server echo_server;
	int call_result = 0;

	::printf("ECHO SERVER: Started!\n");

	unsigned max_loop_counter = 1;

	if (argc > 1) // Read the Max Loop Counter
		max_loop_counter = ::strtoul(argv[1], 0, 0);

	::printf("ECHO SERVER: Opening the service..."); ::fflush(stdout);

	if ((call_result = echo_server.open(acs_dsd::SERVICE_MODE_INET_SOCKET))) {
		// ERROR: Opening the echo server
		::printf(
				"KO!!!\n"
				"## ERROR: Opening the service (INET mode)\n"
				"     Error code == %d\n"
				"     Error text == '%s'\n", echo_server.last_error(), echo_server.last_error_text());
		return 1;
	}

	::printf("OK!!!\n");
	::printf("ECHO SERVER: Publishing the service..."); ::fflush(stdout);

	if ((call_result = echo_server.publish("echo", "test"))) {
		// ERROR: Publishing the echo service
		::printf(
				"KO!!!\n"
				"## ERROR: Publishing the service\n"
				"     Error code == %d\n"
				"     Error text == '%s'\n", echo_server.last_error(), echo_server.last_error_text());
		echo_server.close();
		return 1;
	}

	::printf("OK!!!\n");

	for (unsigned loop_counter = 1; loop_counter <= max_loop_counter; ++loop_counter) {
		::printf("ECHO SERVER: Waiting to accept a client connection (loop_counter == %u)...", loop_counter); ::fflush(stdout);

		ACS_DSD_Session echo_session;
		if ((call_result = echo_server.accept(echo_session))) {
			// ERROR: Accepting a client connection
			::printf(
					"KO!!!\n"
					"## ERROR: Accepting a client connection request\n"
					"     Error code == %d\n"
					"     Error text == '%s'\n", echo_server.last_error(), echo_server.last_error_text());
			continue;
		}

		::printf("OK!!!\n");

		do_echo(echo_session);

		echo_session.close();

		::printf("ECHO SERVER: Client disconnected: connection closed (loop_counter == %u).\n", loop_counter);
	}

	::printf("ECHO SERVER: Unregistering the service..."); ::fflush(stdout);

	if ((call_result = echo_server.unregister())) {
		// ERROR: Unregistering the echo service
		::printf(
				"KO!!!\n"
				"## ERROR: Unregistering the service\n"
				"     Error code == %d\n"
				"     Error text == '%s'\n", echo_server.last_error(), echo_server.last_error_text());
	} else
		::printf("OK!!!\n");

	echo_server.close();

	::printf("ECHO SERVER: Exiting, bye bye!\n");

	return 0;
}
