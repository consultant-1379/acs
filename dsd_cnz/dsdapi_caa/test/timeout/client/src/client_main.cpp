#include <stdio.h>
#include <errno.h>

#include <iostream>

#include "ace/Thread_Mutex.h"
#include "ace/Condition_Attributes.h"
#include "ace/Condition_Thread_Mutex.h"
#include "ace/Time_Value_T.h"
#include "ace/Monotonic_Time_Policy.h"

#include "ACS_DSD_Client.h"
#include "ACS_DSD_Session.h"

#define IMM_CP_NODES_UPDATER_INTERVAL           30      // check interval (in seconds) used by ACS_DSD_ImmCpNodesUpdater::svc() method

namespace {
	const char * m_sap_id = "44321";
}

int main (int /*argc*/, char * /*argv*/ []) {
	ACE_Condition_Attributes_T<ACE_Monotonic_Time_Policy> condition_attributes;
	ACE_Thread_Mutex _thread_mutex;
	ACE_Condition<ACE_Thread_Mutex> condition(_thread_mutex, condition_attributes);

	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;

	timeout = timeout.now() + ACE_Time_Value(IMM_CP_NODES_UPDATER_INTERVAL);

	if (condition.wait(&timeout) == -1) {
		::printf("Timeout expired...\n");
	}


/*
	std::cout << "timeout == {" << timeout.sec() << "}" << std::endl;
	std::cout << "timeout.now() == {" << timeout.now().sec() << "}" << std::endl;
	std::cout << "ACE_Time_Value(IMM_CP_NODES_UPDATER_INTERVAL) == {" << ACE_Time_Value(IMM_CP_NODES_UPDATER_INTERVAL).sec() << "}" << std::endl;

	timeout = timeout.now() + ACE_Time_Value(IMM_CP_NODES_UPDATER_INTERVAL);

	std::cout << "AFTER: timeout == {" << timeout.sec() << "}" << std::endl;
*/


	::printf("HIT RETURN KEY..."); ::getchar();

	ACS_DSD_Client dsd_client;

	::printf("CLIENT: Connecting to my INET server on my partner node on port %s, with a timeout of 5 seconds...", m_sap_id);
	::fflush(stdout);

	ACS_DSD_Session dsd_session;

	errno = 0;
//	int call_result = dsd_client.connect(dsd_session, m_sap_id, 2001, "AP1A", 5000);
	int call_result = dsd_client.connect(dsd_session, m_sap_id, 2001, "AP1A");
	if (call_result && (errno != EINPROGRESS)) {
		::printf("KO!!\n###-ERROR: call 'connect' failed: last_error == %d, last_error_text == '%s'\n",
				dsd_client.last_error(), dsd_client.last_error_text());
		return 1;
	}
	::printf("OK!!\n: CLIENT: connection established: send all the messages you want\n");

	char input_buffer [1024];
	for (;;) {
		::printf("CLIENT > ");
		::fflush(stdout);
		if (::gets(input_buffer)) { // OK: sending the input line
			call_result = dsd_session.send(input_buffer, ::strlen(input_buffer) + 1);
			if (call_result < 0) {
				::printf("###-ERROR: call 'send' failed: last_error == %d, last_error_text == '%s'\n",
						dsd_client.last_error(), dsd_client.last_error_text());
				break;
			}
		} else { // ERROR:
			::printf("###-ERROR: call '::gets' failed or EOF detected\n");
			break;
		}
	}

	::printf("CLIENT: Closing the connection.\nBye!!\n");
	dsd_session.close();

	return 0;
}
