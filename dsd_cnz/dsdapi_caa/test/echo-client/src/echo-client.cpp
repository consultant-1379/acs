#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "ACS_DSD_Client.h"
#include "ACS_DSD_Session.h"

namespace {
	void do_communication (ACS_DSD_Session & echo_session) {
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
					"  Server INFO:\n"
					"    system_id == %d\n"
					"    system_name == '%s'\n"
					"    node_name == '%s'\n\n", remote_node.system_id, remote_node.system_name, remote_node.node_name);

		for (unsigned message_counter = 1; ; ++message_counter) {
			::printf("#%04u > ", message_counter);

			char buffer[1024];

			// Reading message from user prompt
			::fgets(buffer, sizeof(buffer), stdin);

			if (::strcasecmp("EXIT\n", buffer) == 0) // User requested to exit
				break;

			// Sending the user message to the echo service
			ssize_t bytes_sent = echo_session.sendf(buffer, ::strlen(buffer), MSG_NOSIGNAL);
			if (bytes_sent < 0) { // ERROR: Sending client message
				::printf(
						"## ERROR: Sending client message\n"
						"     Error code == %d\n"
						"     Error text == '%s'\n", echo_session.last_error(), echo_session.last_error_text());
				break;
			}

			// Waiting server echo
			ssize_t bytes_received = echo_session.recv(buffer, sizeof(buffer));
			if (bytes_received < 0) { // ERROR: Receiving server echo
				::printf(
						"## ERROR: Receiving server echo message\n"
						"     Error code == %d\n"
						"     Error text == '%s'\n", echo_session.last_error(), echo_session.last_error_text());
				break;
			}

			buffer[bytes_received] = buffer[sizeof(buffer)] = 0;
			::printf("SERVER ECHO #%u: [%zd]: '%s'", message_counter, bytes_received, buffer);
		}
	}
}

int main (int argc, char * argv []) {
	if (argc < 4) { // Mandatory command option missed
		::printf("ECHO CLIENT: 'Service Name' , 'Service Domain' and 'Scope' mandatory command options missed.\n");
		return 1;
	}

	acs_dsd::SystemTypeConstants scope = acs_dsd::SYSTEM_TYPE_UNKNOWN;

	if (!(::strcasecmp("AP", argv[3]) && ::strcasecmp("0", argv[3])))
		scope = acs_dsd::SYSTEM_TYPE_AP;
	else if (!(::strcasecmp("CP", argv[3]) && ::strcasecmp("1", argv[3])))
		scope = acs_dsd::SYSTEM_TYPE_CP;
	else if (!(::strcasecmp("BC", argv[3]) && ::strcasecmp("2", argv[3])))
			scope = acs_dsd::SYSTEM_TYPE_BC;
	else
	{
		::printf("ECHO CLIENT: Bad Scope option provided.\n");
		return 2;
	}

	int call_result = 0;
	ACS_DSD_Client echo_client;

	std::vector<ACS_DSD_Node> reachable_nodes;
	std::vector<ACS_DSD_Node> unreachable_nodes;

	call_result = echo_client.query(argv[1], argv[2], scope, reachable_nodes, unreachable_nodes);
	if( call_result )
	{
		// ERROR: Failed query operation
		::printf(
				"KO!!!\n"
				"## ERROR: Query operation failed.\n"
				"     Error code == %d\n"
				"     Error text == '%s'\n", echo_client.last_error(), echo_client.last_error_text());
		return 3;
	}

	std::vector<ACS_DSD_Node>::iterator it;
	::printf("\nThe service < %s@%s > is available on the following nodes:\n", argv[1], argv[2]);
	for( it = reachable_nodes.begin(); it != reachable_nodes.end(); it++ )
	{
		::printf("System ID: '%d'\tNode State: '%d'\n", it->system_id, it->node_state);
	}

	::printf("\nThe service < %s@%s > is NOT available on the following nodes:\n", argv[1], argv[2]);
	for( it = unreachable_nodes.begin(); it != unreachable_nodes.end(); it++ )
	{
		::printf("System ID: '%d'\tNode State: '%d'\n", it->system_id, it->node_state);
	}

	char buffer[1024];
	::printf("\nEnter the system ID of the node to connect: ");
	::fgets(buffer, sizeof(buffer), stdin);
	int system_id = ::strtol(buffer, 0, 0);

	::printf("\nEnter the state of the node to connect: ");
	::fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer)-1] = '\0';
	acs_dsd::NodeStateConstants node_state = acs_dsd::NODE_STATE_ACTIVE;

	if (!(::strcasecmp("ACTIVE", buffer) && ::strcasecmp("A", buffer)))
		node_state = acs_dsd::NODE_STATE_ACTIVE;
	else if (!(::strcasecmp("PASSIVE", buffer) && ::strcasecmp("P", buffer)))
		node_state = acs_dsd::NODE_STATE_PASSIVE;
	else { // ERROR: Bad node state provided
		::printf("ECHO CLIENT: Bad Node-State string provided.\n");
		return 4;
	}

/*
	int system_id = acs_dsd::SYSTEM_ID_THIS_NODE;
	acs_dsd::NodeStateConstants node_state = acs_dsd::NODE_STATE_ACTIVE;

	if (argc > 3) // System ID option present
		system_id = ::strtol(argv[3], 0, 0);

	if (argc > 4) { // Node state option present
		if (!(::strcasecmp("ACTIVE", argv[4]) && ::strcasecmp("A", argv[4])))
			node_state = acs_dsd::NODE_STATE_ACTIVE;
		else if (!(::strcasecmp("PASSIVE", argv[4]) && ::strcasecmp("P", argv[4])))
			node_state = acs_dsd::NODE_STATE_PASSIVE;
		else { // ERROR: Bad node state provided
			::printf("ECHO CLIENT: Bad Node-State option provided.\n");
			return 2;
		}
	}
*/

//	int call_result = 0;
//	ACS_DSD_Client echo_client;
	ACS_DSD_Session echo_session;

	::printf("ECHO CLIENT: Connecting to the echo service..."); ::fflush(stdout);

	if ((call_result = echo_client.connect(echo_session, argv[1], argv[2], system_id, node_state))) {
		// ERROR: Connection to the echo server
		::printf(
				"KO!!!\n"
				"## ERROR: COnnecting to the echo service\n"
				"     Error code == %d\n"
				"     Error text == '%s'\n", echo_client.last_error(), echo_client.last_error_text());
		return 5;
	}

	::printf(
			"OK!!!\n"
			"ECHO CLIENT: Connection established with the echo service. Now you can send simple messages.\n\n");

	do_communication(echo_session);

	echo_session.close();

	::printf("ECHO CLIENT: Exiting, bye bye!\n");

	return 0;
}
