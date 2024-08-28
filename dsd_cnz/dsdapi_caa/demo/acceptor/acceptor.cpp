/*acceptor.cpp
 *
 * Demo for ACS_DSD_Acceptor
 *
 * arguments: 	-s 	[IP address] (format: xxx.xxx.xxx.xxx:pppp)
 * 					create, open and accept connection on a INET socket
 *
 * 				-l	[UNIX address]
 * 					create, open and accept connection on a Local Unix socket
 *
 * 				-t  [time] in second
 * 					set a timeout for the connection
 *
 * example of use:
 * 	./acceptor-002 -s 127.0.0.1:1234
 *  ./acceptor-002 -l /tmp/dsd_demo_lSock
 * 	./acceptor-002 -s 127.0.0.1:1234 -t 10
 * 	./acceptor-002 -l /tmp/dsd_demo_lSock -t 10
 *
 */


#include <iostream>

#include "ace/UNIX_Addr.h"
#include "ACS_DSD_LSocketAcceptor.h"
#include "ACS_DSD_SocketAcceptor.h"

int main(int argc, char * argv[]) {


	ACS_DSD_Acceptor *acceptor = 0;
	ACS_DSD_IOStream *stream = 0;
	ACE_Time_Value *timeout = 0;
	ACE_Addr *addr = 0;

	char *address = 0;
	const char *typeSock = 0;
	int mux = 0;
	int c = 0;
	int tflag = 0;

	while( (c = getopt(argc, argv, "l:s:t:")) != -1){

		switch (c) {
			case 'l':
				if( mux > 0){
					std::cout << "error - Too many arguments " << std::endl;
					return -1;
				}else{
					address = optarg;

					addr = new ACE_UNIX_Addr(address);
					acceptor = new ACS_DSD_LSocketAcceptor();
					stream = new ACS_DSD_LSocketStream();
					typeSock = "UNIX local socket";

					tflag++;
					mux++;
				}
				break;

			case 's':
				if( mux > 0 ){
					std::cout << " error - Too many arguments "<< std::endl;
					return -1;
				}else {
					address = optarg;

					addr = new ACE_INET_Addr(address);
					acceptor = new ACS_DSD_SocketAcceptor();
					stream = new ACS_DSD_SocketStream();
					typeSock = "INET socket";

					tflag++;
					mux++;
				}
				break;

			case 't':
				if( tflag == 0 ){
					std::cout << "error - option -t required option -l or -s " << std::endl;
					return -1;
				}else {
					int time = ACE_OS::atoi( optarg );
					if( time > 0 ){
						timeout = new ACE_Time_Value( time );
					}else {
						std::cout << "error - option -t value not valid" << std::endl;
						return -1;
					}
				}
				break;

			case '?':
				if( mux > 0 )
					std::cout << "error - Too many arguments "<< std::endl;

				else if( optopt == 'l' )
					ACE_OS::fprintf(stdout, "error - Option -%c requires an operand\n", optopt);

				else if ( optopt == 's' )
					ACE_OS::fprintf(stdout, "error - Option -%c requires an operand\n", optopt);

				else if ( optopt == 't' )
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

	std::cout << "Try to open "<< typeSock <<"... " << std::endl;
	int return_code = acceptor->open(*addr, 0);
	if (return_code < 0)
	{
		char buff[256];
		std::cout << " open(...) CALL RETURN CODE: " << return_code << std::endl;
		std::cout << "  ERRNO == " << errno << std::endl;
		std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
	}
	else if(return_code == 0)
	{
		std::cout << "The " << typeSock << " is opened" << std::endl << std::endl;
	}

	std::cout << "Waiting for accept connection on "<< typeSock <<" ... " << std::endl;
	if( timeout == 0)
	{
		return_code = acceptor->accept(*stream);
	}
	else
	{
		return_code = acceptor->accept(*stream, timeout);
	}


	if (return_code < 0)
	{
		char buff[256];
		std::cout << " accept(...) CALL RETURN CODE: " << return_code << std::endl;
		std::cout << "  ERRNO == " << errno << std::endl;
		std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
	}
	else if (return_code == 0)
	{
		std::cout<<"Connection successfully established " << std::endl << std::endl;
		std::cout << " You can check the socket state using the 'netstate' command" << std::endl;
		(std::cout << " Press RETURN key after check...") && getchar() && (std::cout << std::endl);
	}


	return_code = acceptor->close();

	if (return_code < 0)
	{
		char buff[256];
		std::cout << " close(...) CALL RETURN CODE: " << return_code << std::endl;
		std::cout << "  ERRNO == " << errno << std::endl;
		std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
	}
	else if (return_code == 0)
	{
		std::cout<<"The "<< typeSock <<" is closed successfully" << std::endl << std::endl;
	}

	if(strcmp(typeSock, "UNIX local socket") == 0)
	{
		acceptor->remove();
	}

	delete acceptor;
	delete stream;
	delete addr;
	delete timeout;

	return 0;
}

