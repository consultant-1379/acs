#include <unistd.h>
#include <cctype>
#include <cstring>
#include <arpa/inet.h>

#include <iostream>

#include "ace/Log_Msg.h"

#include "ACS_DSD_Macros.h"

#include "demo-client-macros.h"
#include "MsgExchangerClient.h"

namespace {
	const char * program_name = 0;
	const char * sap_address = 0;
	int sap_type = SAP_TYPE_UNKNOWN;

	int parse_args(int argc, char * argv[]) {
		ACS_DSD_TRACE("demo-client::parse_args(int argc, char * argv[])");

		int c = 0;
		int return_code = 0;

		::opterr = 0;
		while (!return_code && ((c = ::getopt(argc, argv, "hls")) ^ -1))
			switch (c) {
			case 'h': return_code = -1; break;
			case 'l':
				(sap_type == SAP_TYPE_UNKNOWN) || (sap_type == SAP_TYPE_LSOCK)
					? (sap_type = SAP_TYPE_LSOCK)
					: (::std::cerr << "WARNING: The two option -l and -s are exclusive." << ::std::endl << ::std::endl, return_code = -1);
				break;
			case 's':
				(sap_type == SAP_TYPE_UNKNOWN) || (sap_type == SAP_TYPE_SOCK)
					? (sap_type = SAP_TYPE_SOCK)
					: (::std::cerr << "WARNING: The two option -s and -l are exclusive options." << ::std::endl << ::std::endl, return_code = -1);
				break;
			case '?':
				::isprint(::optopt) ? ::std::cerr << "WARNING: Unknown option '-" << static_cast<char>(::optopt) << "'."
														: ::std::cerr << "WARNING: Unknown option character 0x'" << ::std::hex << ::optopt << "'."
					<< ::std::endl << ::std::endl;
			default:
				return_code = -1;
				break;
			}

		return return_code;
	}

	int check_address(const char * address) {
		ACS_DSD_TRACE("demo-client::check_address(const char * address)");

		int result = 0;

		if (SAP_TYPE_SOCK == sap_type) {
			char addr[1024];
			unsigned char buf[sizeof(struct in6_addr)];

			::memccpy(addr, address, 0, 1024) || (addr[1023] = 0);

			char * p = ::strchr(addr, ':');
			p && (*p++ = '\0');
			int port = 0;
			char * end_ptr = 0;

			result = (::inet_pton(AF_INET, addr, buf) == 1) && (!p || ((0 <= (port = ::strtol(p, &end_ptr, 10))) && (port <= 0xFFFF) && (!*end_ptr))) ? 0 : -1;
		}

		return result;
	}

	void print_help() {
		ACS_DSD_TRACE("demo-client::print_help()");

		::std::cerr << "Usage: " << program_name << " [-l or -s] ADDRESS" << ::std::endl
								<< "Start a DSD-API demo client that will connect to a demo server listening on the 'service access point' specified by ADDRESS." << ::std::endl
								<< "Example: " << program_name << " -s 192.168.0.1:53000" << ::std::endl << ::std::endl
								<< "  -h\tPrint this help." << ::std::endl
								<< "  -l\tThe application will use a UNIX Domain socket." << ::std::endl
								<< "  -s\tThe application will use a standard INET socket. This is the default." << ::std::endl;
	}
}

int main(int argc, char * argv[]) {
	ACS_DSD_TRACE("demo-client::main(int argc, char * argv[])");

	program_name = ::strrchr(argv[0], '/');
	program_name++ || (program_name = argv[0]);

	if (parse_args(argc, argv)) {
		print_help();
		return __LINE__;
	}

	//Set the default connection socket type to INET socket
	(sap_type ^ SAP_TYPE_UNKNOWN) || (sap_type = SAP_TYPE_SOCK);

	if (::optind >= argc) { // Missing ADDRESS argument
		::std::cerr << "WARNING: Missing ADDRESS program argument!" << ::std::endl;
		print_help();
		return __LINE__;
	}

	sap_address = argv[::optind];

	//Is the address correct?
	if (check_address(sap_address)) {
		::std::cerr << "WARNING: Bad address format! ADDRESS should be an IPv4 address." << ::std::endl;
		print_help();
		return __LINE__;
	}

	MsgExchangerClient msg_exchanger_client(sap_type, sap_address);

	msg_exchanger_client.run();

	return 0;
}
