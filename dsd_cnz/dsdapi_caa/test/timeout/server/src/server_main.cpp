#include <stdio.h>

#include "ACS_DSD_Server.h"
#include "ACS_DSD_Session.h"

namespace {
	const char * m_sap_id = "44321";
}

int main (int /*argc*/, char * /*argv*/ []) {
	ACS_DSD_Server dsd_server;

	::printf("SERVER: Opening the INET server on the port %s...", m_sap_id);
	::fflush(stdout);

	int call_result = dsd_server.open(acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE, m_sap_id);
	if (call_result) {
		::printf("KO!!\n###-ERROR: call 'open' failed: last_error == %d, last_error_text == '%s'\n", dsd_server.last_error(), dsd_server.last_error_text());
		return 1;
	}
	::printf("OK!!\nSERVER: Server open and ready to accept connections\n");

	for (;;) {
		ACS_DSD_Session dsd_session;

		::printf("SERVER: accepting with a timeout of 5 seconds...");
		::fflush(stdout);
		call_result = dsd_server.accept(dsd_session, 5000);
		if (call_result) {
			::printf("KO!!\n###-ERROR: call 'accept' failed: last_error == %d, last_error_text == '%s'\n", dsd_server.last_error(), dsd_server.last_error_text());
			::printf("SERVER: Re-trying accepting...\n");
			continue;
		}
		::printf("OK!!\nSERVER: A client connected to this server from address 0x%08X\n", dsd_session.get_remote_ip4_address());

		char recv_buffer [1024];
		for (;;) {
			::printf("SERVER: Waiting a message from the client, with a timeout of 5 seconds...");
			::fflush(stdout);
			//call_result = dsd_session.recv(recv_buffer, sizeof(recv_buffer), 5000);
			call_result = dsd_session.recv(recv_buffer, sizeof(recv_buffer));
			if (call_result > 0) { // OK: message received
				::printf("OK!!\nSERVER: CLIENT message == '%s'\n", recv_buffer);
			} else {
				::printf("KO!!\n###-ERROR: ");

				if (call_result == acs_dsd::ERR_PEER_CLOSED_CONNECTION) { // Peer disconnected
					::printf("call 'recv' failed: PEER CLOSED THE CONNECTION: last_error == %d, last_error_text == '%s'\n",
							dsd_server.last_error(), dsd_server.last_error_text());
					break;
				} else if (call_result < 0) { // ERROR
					::printf("call 'recv' failed: last_error == %d, last_error_text == '%s'\n",
							dsd_server.last_error(), dsd_server.last_error_text());
					break;
				} else if (call_result == 0) { // Timeout
					::printf("call 'recv' failed: TIMEOUT: last_error == %d, last_error_text == '%s'\n",
							dsd_server.last_error(), dsd_server.last_error_text());
					::printf("SERVER: Re-trying receiving...\n");
					continue;
				}
			}
		}

		::printf("SERVER: Closing the client connection\n");
		dsd_session.close();
	}

	return 0;
}
