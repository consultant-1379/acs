#define ACS_DSD_TESTING_CLIENT

#ifdef PIPPO

#include <stdio.h>

#include "ACS_DSD_Session.h"
#include "ACS_DSD_Client.h"

int main (int /*argc*/, char * /*argv*/ []) {
	ACS_DSD_Session session;
	ACS_DSD_Client client;
	int call_result = 0;
	const char * service_name = "CLOCKSYNC";
	const char * service_domain = "JTP";

	printf("Connecting to service %s@%s...", service_name, service_domain);
	call_result = client.connect(session, service_name, service_domain, acs_dsd::SYSTEM_ID_CP_CLOCK_MASTER, acs_dsd::NODE_STATE_EXECUTION);
	if (call_result) {
		printf("KO!!!\n%s: call 'client.connect(session, ...)' failed: error code == %d: error text == '%s'\n",
			(call_result < 0 ? "ERROR" : "WARNING"), call_result, client.last_error_text());
		return 1;
	}

	printf("OK!!!\nConnection successfull!!!\n");

	session.close();

	return 0;
}

#endif


#ifdef ACS_DSD_TESTING_LOG4CPLUS

#include "stdio.h"

#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"

namespace {
	const char * appender_name = "ACS_DSDD";
	const char * configuartion_path = "/cluster/ACS/TRA/conf/log4cplus.properties";
}

int main (int argc, char * argv []) {
	(argc >= 2) && (appender_name = argv[1]);

	printf("Appender name == '%s'\n", appender_name);

	printf("DO CONFIGURATION: log4cplus configuration path == '%s'\n", configuartion_path);
	log4cplus::PropertyConfigurator::doConfigure(configuartion_path);

	printf("DO: getting logger instance using the appender name '%s'\n", appender_name);
	log4cplus::Logger logger = log4cplus::Logger::getInstance(appender_name);

	const char * log_message = "This is a log message";

	printf("DO: logging the message '%s'\n", log_message);
	LOG4CPLUS_DEBUG(logger, log_message);

	printf("DO: shutting down the logger\n");

	logger.shutdown();

	return 0;
}

#endif


#ifdef ACS_DSD_TESTING_TRA_LOGGING

#include <stdio.h>

#include "ace/Task.h"

#include "ACS_TRA_Logging.h"


const char * appender_name = "ACS_DSDD";
ACS_TRA_Logging tra_logger;


class MyTask : public ACE_Task_Base {
public:
	inline MyTask () : ACE_Task_Base() {}

	virtual inline ~MyTask () {}

	virtual inline int svc () {
		unsigned log_counter = 0;

		while (log_counter < 50000) {
			char buffer[1024] = {0};
			snprintf(buffer, sizeof(buffer), "Nel mezzo del cammin di nostra vita mi ritrovai per una selva oscura... log_counter == %u", log_counter++);
			tra_logger.Write(buffer, LOG_LEVEL_DEBUG);
			printf("%s\n", buffer);
		}

		return 0;
	}
};


int main (int argc, char * argv []) {
	(argc >= 2) && (appender_name = argv[1]);

	ACS_TRA_LogResult log_call_result = tra_logger.Open(appender_name);
	if (log_call_result != TRA_LOG_OK) {
		printf("ERROR: call 'tra_logger.Open(...' failed: log_call_result == %d\n", log_call_result);
		return __LINE__;
	}

	MyTask my_task;

	int call_result = my_task.activate(THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED, 64);
	if (call_result < 0) {
		printf("ERROR: call 'my_task.activate(...' failed: call_result == %d\n", call_result);
		return __LINE__;
	}

	my_task.wait();

	tra_logger.Close();

	return 0;
}

#endif //ACS_DSD_TESTING_TRA_LOGGING


#ifdef ACS_DSD_TESTING_CLIENT

#include <cstdio>

#include "ACS_DSD_Client.h"
#include "ACS_DSD_Session.h"

int main (int /*argc*/, char * /*argv*/ []) {
	ACS_DSD_Client client;
	ACS_DSD_Session session;
	int call_result = 0;

	std::vector<ACS_DSD_Node> reachable_nodes;
	std::vector<ACS_DSD_Node> unreachable_nodes;

	printf("Querying for service 'ttt-inet@games' on APs...");
	call_result = client.query("ttt-inet", "games", acs_dsd::SYSTEM_TYPE_AP, reachable_nodes, unreachable_nodes);
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'client.query(...' failed!\n"
				"      last error == %d\n"
				"      laste error text == '%s'\n", client.last_error(), client.last_error_text());
		return -1;
	}

	printf("OK!\n");
	printf("reachable_nodes.size() == %zu, unreachable_nodes.size() == %zu\n", reachable_nodes.size(), unreachable_nodes.size());

	printf("Reachable node list\n");
	for (std::vector<ACS_DSD_Node>::iterator it = reachable_nodes.begin(); it != reachable_nodes.end(); ++it)
		printf("  {node_name =='%s', node_side == %d, node_state == %d, system_id == %d, system_name == '%s', system_type == %d}\n",
				it->node_name, it->node_side, it->node_state, it->system_id, it->system_name, it->system_type);

	printf("Unreachable node list\n");
	for (std::vector<ACS_DSD_Node>::iterator it = unreachable_nodes.begin(); it != unreachable_nodes.end(); ++it)
		printf("  {node_name =='%s', node_side == %d, node_state == %d, system_id == %d, system_name == '%s', system_type == %d}\n",
				it->node_name, it->node_side, it->node_state, it->system_id, it->system_name, it->system_type);

	printf("Querying for service 'ttt-inet@games' on CPs...");
	call_result = client.query("ttt-inet", "games", acs_dsd::SYSTEM_TYPE_CP, reachable_nodes, unreachable_nodes);
	if (call_result < 0) {
		printf(
				"KO!!## ERROR: 'client.query(...' failed!\n"
				"      last error == %d\n"
				"      laste error text == '%s'\n", client.last_error(), client.last_error_text());
		return -1;
	}

	printf("OK!\n");
	printf("reachable_nodes.size() == %zu, unreachable_nodes.size() == %zu\n", reachable_nodes.size(), unreachable_nodes.size());

	printf("Reachable node list\n");
	for (std::vector<ACS_DSD_Node>::iterator it = reachable_nodes.begin(); it != reachable_nodes.end(); ++it)
		printf("  {node_name =='%s', node_side == %d, node_state == %d, system_id == %d, system_name == '%s', system_type == %d}\n",
				it->node_name, it->node_side, it->node_state, it->system_id, it->system_name, it->system_type);

	printf("Unreachable node list\n");
	for (std::vector<ACS_DSD_Node>::iterator it = unreachable_nodes.begin(); it != unreachable_nodes.end(); ++it)
		printf("  {node_name =='%s', node_side == %d, node_state == %d, system_id == %d, system_name == '%s', system_type == %d}\n",
				it->node_name, it->node_side, it->node_state, it->system_id, it->system_name, it->system_type);

	printf("Connecting to the service 'ttt-inet@games'...");
	call_result = client.connect(session, "ttt-inet", "games");
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
#endif


#ifdef ACS_DSD_QUERY_CONNECT_EXAMPLE

#define APPLICATION_BASE_ERROR_CODE (128)
#define APPLICATION_NEXT_ERROR_CODE (APPLICATION_BASE_ERROR_CODE + __COUNTER__)

#include <cstdio>

#include "ACS_DSD_Client.h"
#include "ACS_DSD_Session.h"

int main (int /*argc*/, char * /*argv*/ []) {
	ACS_DSD_Client client;
	int call_result = 0;

	const char * service_name = "SrvName"; //Put here your service name
	const char * service_domain = "SrvDom"; //Put here your service domain

	std::vector<ACS_DSD_Node> reachable_nodes;
	std::vector<ACS_DSD_Node> unreachable_nodes;

	call_result = client.query(service_name, service_domain, acs_dsd::SYSTEM_TYPE_AP, reachable_nodes, unreachable_nodes);
	if (call_result < 0) {
		printf(
				"## ERROR: 'client.query(...' failed!\n"
				"  last error == %d\n"
				"  last error text == '%s'\n", client.last_error(), client.last_error_text());
		return APPLICATION_NEXT_ERROR_CODE;
	}

	if (reachable_nodes.size() <= 0) { //No node found where the requested service is published
		printf("## WARNING: No nodes found where the service '%s@%s' is published.\n", service_name, service_domain);
		return APPLICATION_NEXT_ERROR_CODE;
	}

	//I will connect to the service on all the node where it was registered.
	//So I need reachable_nodes.size() session objects.

	//The ACS_DSD_Client doesn't have a socket of its own, i.e., it simply "borrows" the one from the
	//ACS_DSD_Session that's being connected. A nice side-effect of this is that ACS_DSD_Client objects
	//do not store state so they can be used reentrantly in multithreaded programs. You can also use
	//one ACS_DSD_Client object for many and many connect calls.

	ACS_DSD_Session session[reachable_nodes.size()]; //Allocate statically (you can dinamically also) a session for each node found.

	for (size_t i = 0; i < reachable_nodes.size(); ++i) {
		ACS_DSD_Node & node = reachable_nodes[i];
		printf("Connecting to service '%s@%s' on node %d (%s) %s...", service_name, service_domain, node.system_id,
				node.system_name, node.node_state ^ acs_dsd::NODE_STATE_ACTIVE ? "PASSIVE" : "ACTIVE");
		call_result = client.connect(session[i], service_name, service_domain, node.system_id, node.node_state);
		if (call_result < 0) { //ERROR: connection error
			printf(
					"KO!!## ERROR: 'client.connect(...' failed!\n"
					"      last error == %d\n"
					"      last error text == '%s'\n", client.last_error(), client.last_error_text());

			//Handle your errors here

		} else printf("OK!!!\n"); //OK: Connection success
	}


	//... continue the application tasks ...


	return 0;
}

#endif
