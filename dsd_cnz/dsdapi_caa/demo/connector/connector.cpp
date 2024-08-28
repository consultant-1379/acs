/*connector.cpp
 *
 * Demo for ACS_DSD_Connector
 *
 * arguments: 	-s 	[IP address] (format: xxx.xxx.xxx.xxx:pppp)
 * 					Try to connect towards the INET socket service
 *
 * 				-l	[UNIX address]
 * 					Try to connect towards the Local Unix socket
 *
 *
 * example of use:
 * 	./acceptor-002 -s 127.0.0.1:1234
 *  ./acceptor-002 -l /tmp/dsd_demo_lSock
 *
 */

#include <iostream>
#include <unistd.h>

#include "ACS_DSD_SocketConnector.h"
#include "ACS_DSD_LSocketConnector.h"

int main(int argc, char * argv[]) {

	ACS_DSD_Connector *connector = 0;
	ACS_DSD_IOStream *stream = 0;
	ACE_Addr *addr = 0;
	char *address = 0;
	const char *typeSock = 0;
	int mux = 0;
	int c = 0;

	while( (c = getopt(argc, argv, "l:s:")) != -1){

		switch (c) {
			case 'l':
				if( mux > 0){
					std::cout << "error - Too many arguments " << std::endl;
					return -1;
				}else{

					address=optarg;

					connector = new ACS_DSD_LSocketConnector();
					stream = new ACS_DSD_LSocketStream();
					addr = new ACE_UNIX_Addr(address);
					typeSock = "UNIX local socket";

					mux++;
				}
				break;

			case 's':
				if( mux > 0 ){
					std::cout << " error - Too many arguments "<< std::endl;
					return -1;
				}else {

					address=optarg;

					connector = new ACS_DSD_SocketConnector();
					stream = new ACS_DSD_SocketStream();
					addr = new ACE_INET_Addr(address);
					typeSock = "INET socket";

					mux++;
				}
				break;

			case '?':
				if( mux > 0 )
					std::cout << "error - Too many arguments "<< std::endl;

				else if( optopt == 'l' )
					ACE_OS::fprintf(stdout, "error - Option -%c requires an operand\n", optopt);

				else if ( optopt == 's' )
					ACE_OS::fprintf(stdout, "error - Option -%c requires an operand\n", optopt);

				else if ( isprint(optopt) )
					ACE_OS::fprintf(stdout, "error - Unrecognized option: -%c\n", optopt);

				return -1;
				break;
			default:
				break;
		}//END switch(c)

	}//END while
	if (optind == 1)
	{
		std::cout << "error - wrong number of arguments " << std::endl;
		return -1;
	}


	std::cout << std::endl;
	std::cout <<"Try to connect towards the " << typeSock << "..." << std::endl;
	int return_code = connector->connect(*stream, *addr);
	if (return_code < 0)
	{
		char buff[256];
		std::cout << " connect(...) CALL RETURN CODE: " << return_code << std::endl;
		std::cout << "  ERRNO == " << errno << std::endl;
		std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
	}
	else if(return_code == 0)
	{
		std::cout << "Connection established successfully" << std::endl;;
		std::cout << " You can check the socket state using the 'netstate' command" << std::endl;
		(std::cout << " Press RETURN key after check...") && getchar() && (std::cout << std::endl);
	}



	delete connector;
	delete stream;
	delete addr;

	return 0;
}
