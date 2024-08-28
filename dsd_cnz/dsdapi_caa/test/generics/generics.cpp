#if 0 // FOR DSD-CS-API CHECK ABOUT RETRIEVING CONFIGURATION

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "ACS_DSD_ConfigurationHelper.h"

int main (int, char * []) {
	int call_result = __COUNTER__;

	::printf("=======================================================================================\n");
	::printf("CS API TEST: test application that uses the CS-API throughout the DSD Configuration API\n");
	::printf("=======================================================================================\n\n");

	for (unsigned counter = 0; ; counter++, ::sleep(1)) {
		errno = 0;
		if ((call_result = ACS_DSD_ConfigurationHelper::load_ap_nodes_from_cs())) {
			::printf("[%u] ERROR: call 'ACS_DSD_ConfigurationHelper::load_ap_nodes_from_cs()' failed: Cannot load system AP nodes network configuration from CS: "
							 "call_result == %d, errno == %d\n", counter, call_result, call_result < 0 ? errno : 0);
			continue;
//			return __COUNTER__;
		}

		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t my_ap_host_ptr = 0;

		errno = 0;
		if ((call_result = ACS_DSD_ConfigurationHelper::load_my_ap_node())) {
			::printf("[%u] Call 'ACS_DSD_ConfigurationHelper::load_my_ap_node' failed: call_result == %d, errno == %d\n", counter, call_result, errno);
			continue;
//			return __COUNTER__;
		}

		errno = 0;
		if ((call_result = ACS_DSD_ConfigurationHelper::get_my_ap_node(my_ap_host_ptr))) {
			::printf("[%u] Call 'ACS_DSD_ConfigurationHelper::get_my_ap_node' failed: call_result == %d, errno == %d\n", counter, call_result, errno);
			continue;
//			return __COUNTER__;
		}

		::printf("[%u] system_ap_nodes_count == %u, system_cp_nodes_count == %u\n", counter,
				ACS_DSD_ConfigurationHelper::system_ap_nodes_count(), ACS_DSD_ConfigurationHelper::system_cp_nodes_count());
	}

	return 0;
}
#endif

#if 0

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>

#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

namespace {
	int print_board_info (ACS_CS_API_HWC * hwc, ACS_CS_API_IdList & board_list) {

		for (unsigned i = 0; i < board_list.size(); ++i) {
			::printf("  %02u) ", i);

			uint16_t system_id = 0;
			uint16_t system_num = 0;
			uint16_t node_side = 0xBAAD;

			unsigned short board_id = board_list[i];

			if (hwc->getSysId(system_id, board_id) ^ ACS_CS_API_NS::Result_Success) {
				::printf("\nERROR: Call 'getSysId' failed\n");
				continue;
			}
			::printf("System ID == %u", system_id);

			if (hwc->getSysNo(system_num, board_id) ^ ACS_CS_API_NS::Result_Success) {
				::printf("\nERROR: Call 'getSysNo' failed\n");
				continue;
			}
			::printf(", System NUM == %u", system_num);

			if (hwc->getSide(node_side, board_id) ^ ACS_CS_API_NS::Result_Success) {
				::printf("\nERROR: Call 'getSide' failed\n");
				continue;
			}
			::printf(", Node SIDE == %u", node_side);

			uint32_t eth_ip = 0;
			in_addr addr = {0};
			char addr_str [16] = {0};

			if (hwc->getIPEthA(eth_ip, board_id) ^ ACS_CS_API_NS::Result_Success) {
				::printf("\nERROR: Call 'getIPEthA' failed\n");
				continue;
			}
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			::printf(", IP ETH-A == '%s'", addr_str);

			if (hwc->getIPEthB(eth_ip, board_id) ^ ACS_CS_API_NS::Result_Success) {
				::printf("\nERROR: Call 'getIPEthB' failed\n");
				continue;
			}
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			::printf(", IP ETH-B == '%s'", addr_str);

			::printf("\n");
		}

		::printf("\n");

		return 0;
	}
}

int main (int, char * []) {
	ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_Success;

	::printf("===================================================================================\n");
	::printf("CS API TEST: test application that uses only the CS-API no other library linked API\n");
	::printf("===================================================================================\n\n");

	std::cout << "Reading CS configuration about AP nodes..." << std::endl;

	for (unsigned counter = 0; ; ::sleep(2)) {
		std::cout << "CONFIGURATION FETCHING SEQUENCE #" << ++counter << std::endl;

		if (ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance()) {
			ACS_CS_API_IdList board_list;

			if (ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance()) {
				// set Board search criteria
				bs->reset();
				bs->setSysType(ACS_CS_API_HWC_NS::SysType_AP);
				bs->setFBN(ACS_CS_API_HWC_NS::FBN_APUB);

				// get BoardList for selected criteria
				if ((cs_call_result = hwc->getBoardIds(board_list, bs)) == ACS_CS_API_NS::Result_Success) {
					std::cout << "The CS API has detected " << board_list.size() << " board(s) of type APUB"<< std::endl;

					print_board_info(hwc, board_list);
				} else std::cout << "Call 'getBoardIds' failed!!" << std::endl;

				ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
			} else std::cout << "ERROR: Call 'createBoardSearchInstance' failed!" << std::endl;

			ACS_CS_API::deleteHWCInstance(hwc);
		} else std::cout << "ERROR: Call 'createHWCInstance' failed!" << std::endl;

		std::cout << std::endl;
	}

	return 0;
}
#endif


#include <sys/types.h>
#include <unistd.h>

#include <iostream>


namespace {
	void printf_ids () {
		uid_t uid = ::getuid();
		uid_t euid = ::geteuid();
		gid_t gid = ::getgid();
		gid_t egid = ::getegid();

		std::cout
			<< "uid == " << uid << std::endl
			<< "euid == " << euid << std::endl
			<< "gid == " << gid << std::endl
			<< "egid == " << egid << std::endl;
	}
}


int main (int /*argc*/, char * /*argv*/ []) {
	printf_ids();

	const int call_result = ::setuid(0);

	std::cout << "Setting root uid: call_result == " << call_result << std::endl;

	printf_ids();

	return 0;
}



#if 0

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#include "ace/Time_Value.h"
#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Client.h"

int main(int argc, char * argv []) {
	const char * service_name = (argc >= 2 ? argv[1] : "NO_SERV");
	const char * service_domain = (argc >= 3 ? argv[2] : "NO_DOM");
	const char * node_name = (argc >= 4 ? argv[3] : "NO_NODE");

	ACS_DSD_Client client;

	ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
	ACE_UNIX_Addr unix_address;
	int connection_type = acs_dsd::CONNECTION_TYPE_UNKNOWN;
	int visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE;
	char process_name [acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX] = {0};
	int pid = 0;

	int call_result = 0;

	call_result = client.fetch_service_info_from_imm(service_name, service_domain, node_name, inet_addresses, unix_address, connection_type, visibility, process_name, pid);

	if (call_result) { // ERROR
		printf(
				"ERROR: call to 'client.fetch_service_info_from_imm(...' failed: error code == %d\n"
				"  error description == '%s'\n", call_result, client.last_error_text());
	} else { //OK
		printf(
				"For the service '%s@%s' on the node '%s':"
				"  process_name == '%s'\n"
				"  pid == %d\n", service_name, service_domain, node_name, process_name, pid);
	}

	if ((call_result = ACS_DSD_ConfigurationHelper::process_running(process_name, pid)) < 0) { //ERROR
		printf("ERROR: call to 'ACS_DSD_ConfigurationHelper::process_running(...' failed: error code == %d\n", call_result);
	} else printf("The IMM registration item is%s valid.\n", !call_result ? " not" : "");





/*
	const char * process_name = (argc >= 2 ? argv[1] : ACS_DSD_ConfigurationHelper::my_process_name());
	pid_t pid = (  argc >= 3 ? atoi(argv[2]) : ACS_DSD_ConfigurationHelper::my_process_id());

	//switch (int proc_running = ACS_DSD_ConfigurationHelper::process_running(ACS_DSD_ConfigurationHelper::my_process_name(), ACS_DSD_ConfigurationHelper::my_process_id())) {
	switch (int proc_running = ACS_DSD_ConfigurationHelper::process_running(process_name, pid)) {
	case 0: //Process not running
	case 1: //Process running
		printf("I'm%s running\n\n", proc_running ? "" : " not");
		break;
	default: //Error
		printf("ERROR: call 'ACS_DSD_ConfigurationHelper::process_running(...' failed: error code == %d\n\n", proc_running);
		break;
	}
*/


/*
	timeval time_of_day;

	for (int i = 0; i < 100; ++i) {
		int call_result = gettimeofday(&time_of_day, 0);

		if (!call_result) {
			tm broken_datetime;
			if (gmtime_r(&time_of_day.tv_sec, &broken_datetime)) {
				printf("%d-%02d-%02d %02d:%02d:%02d.%06ld THREAD[%lu] : MESSAGGIO\n", 1900 + broken_datetime.tm_year, broken_datetime.tm_mon, broken_datetime.tm_mday,
						broken_datetime.tm_hour, broken_datetime.tm_min, broken_datetime.tm_sec, time_of_day.tv_usec, pthread_self());
			} else printf("ERROR: call 'gmtime_r(...' failed!\n");
		} else printf("ERROR: call 'gettimeofday(...' failed!\n");

	}
*/


/*
	char system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX] = {0};
	int system_id_base[] = {0, 1000, 2000, 50000};

	for (int i = 0; i < 4; ++i) {
		int end = system_id_base[i] + 10;
		for (int system_id = system_id_base[i]; system_id < end; ++system_id) {
			call_result = ACS_DSD_ConfigurationHelper::system_id_to_system_name(system_id, system_name, acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX);
			if (call_result)
				printf("ERROR: call 'ACS_DSD_ConfigurationHelper::system_id_to_system_name(...' failed! call_result == %d\n", call_result);
			else
				printf("system_id == %d --> system name == '%s'\n", system_id, system_name);
		}
	}
*/

	return 0;
}

#endif
