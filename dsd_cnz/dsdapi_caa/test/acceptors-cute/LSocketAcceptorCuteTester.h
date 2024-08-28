#ifndef HEADER_GUARD_CLASS__LSocketAcceptorCuteTester
#define HEADER_GUARD_CLASS__LSocketAcceptorCuteTester LSocketAcceptorCuteTester

#include <iostream>

#include "cute.h"

#include "ACS_DSD_LSocketAcceptor.h"
#include "ACS_DSD_SocketStream.h"


class LSocketAcceptorCuteTester {
	//==============//
	// Constructors //
	//==============//
public:
	inline LSocketAcceptorCuteTester() : _acceptor(), _class_name("ACS_DSD_LSocketAcceptor") {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual ~LSocketAcceptorCuteTester() {}

	//===========//
	// Operators //
	//===========//
public:
	inline void operator()() {
		std::cout << "  --->>> PRESS ENTER TO FOLLOW THIS TEST...";
		getchar() && (std::cout << std::endl << std::endl);

		// open test (on unused Unix local socket)
		test_open_01("/tmp/acs_dsd_lsock_srv01");

		// close test (on opened Unix local socket)
		test_close_01("/tmp/acs_dsd_lsock_srv01");

		// remove test (on closed Unix Local socket)
		test_remove_01("/tmp/acs_dsd_lsock_srv01");


		// open test (on Unix local socket already in use)
		//test_open_01("/tmp/acs_dsd_lsock_srv01");
		//test_close_01("/tmp/acs_dsd_lsock_srv01");
		//test_open_03("/tmp/acs_dsd_lsock_srv01");
		//test_remove_01("/tmp/acs_dsd_lsock_srv01");

		// open test (passing invalid address type)
		//test_open_04();

		// accept test
		test_open_01("/tmp/acs_dsd_lsock_srv01");
		test_get_local_addr_01("/tmp/acs_dsd_lsock_srv01");
		test_accept_01("/tmp/acs_dsd_lsock_srv01");
		test_close_01("/tmp/acs_dsd_lsock_srv01");
		test_remove_01("/tmp/acs_dsd_lsock_srv01");


		// accept test (passing invalid stream type)
		test_open_01("/tmp/acs_dsd_lsock_srv01");
		test_accept_02("/tmp/acs_dsd_lsock_srv01");

		// accept test (waiting for timeout)
		test_accept_03("/tmp/acs_dsd_lsock_srv01",10);
		test_close_01("/tmp/acs_dsd_lsock_srv01");
		test_remove_01("/tmp/acs_dsd_lsock_srv01");
	}

	//================//
	// Test Functions //
	//================//
private:
	void test_open_01(const char* local_addr) {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::open(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Open the UNIX local socket '" << local_addr << "'." << std::endl
				<< "  RESULT: the specified UNIX local socket is opened and put in listening state (check it using netstat command)" << std::endl;


		ACE_UNIX_Addr lsock_address(local_addr);

		int result_code = _acceptor.open(lsock_address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(0, result_code);

		(std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl);
	}

	void test_close_01(const char* local_addr) {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::close(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Close the UNIX local socket (previously opened in listening mode)'" << local_addr << "'." << std::endl
				<< "  RESULT: the specified UNIX local socket is closed successfully (check it using netstat command)." << std::endl;

		int result_code = _acceptor.close();

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(0, result_code);

		(std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl);
	}

	void test_remove_01(const char* local_addr) {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::remove(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Remove the UNIX local socket (previously closed)'" << local_addr << "'." << std::endl
				<< "  RESULT: the specified UNIX local socket is removed from the file system (check it using ls command)" << std::endl;

		int result_code = _acceptor.remove();

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(0, result_code);

		(std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl);
	}


	void test_open_02(const char* local_addr) {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::open(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Open a UNIX local socket already in use  specifying reuse address flag (reuse_addr == 1). Unix Local socket is : '" << local_addr << "'." << std::endl
				<< "  RESULT: the specified UNIX local socket is opened successfully." << std::endl;


		ACE_UNIX_Addr lsock_address(local_addr);

		int result_code = _acceptor.open(lsock_address, 1);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(0, result_code);
	}


	void test_open_03(const char* local_addr) {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::open(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Open a UNIX local socket already in use  NOT specifying reuse address flag (reuse_addr == 0). Unix Local socket is : '" << local_addr << "'." << std::endl
				<< "  RESULT: the open method returns an error because the address is already in use" << std::endl;


		ACE_UNIX_Addr lsock_address(local_addr);

		int result_code = _acceptor.open(lsock_address, 0);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(-2, result_code);
	}


	void test_open_04() {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::open(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Try to pass invalid address to 'open' method " <<" ." << std::endl
				<< "  RESULT: the 'open' method return(check it using netstat command).s an 'invalid address  warning' (return value is 1)." << std::endl;


		ACE_INET_Addr sock_address("127.0.0.1:9999");

		int result_code = _acceptor.open(sock_address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(1, result_code);
	}


	void test_accept_01(const char *local_addr) {
		std::cout  << std::endl
				<< "ACS_DSD_LSocketAcceptor::accept(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Accept connection on Unix local socket  " << "'" << local_addr << "'" << "." << std::endl
				<< "  RESULT: Connection successfully established (to complete the test you have to connect to the Unix local socket from another process and then check socket state using netstat command)." << std::endl;


		ACS_DSD_LSocketStream lsock_stream;

		int result_code = _acceptor.accept(lsock_stream);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(0, result_code);

		(std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl);
	}


	void test_accept_02(const char *local_addr) {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::accept(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Try to pass invalid stream type " <<" ." << std::endl
				<< "  RESULT: the method 'accept' returns a 'wrong stream type'  warning (return value is 4)" << std::endl;


		ACS_DSD_SocketStream sock_stream;

		int result_code = _acceptor.accept(sock_stream);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(ACS_DSD_Acceptor::WAR_ACCEPT_WRONG_STREAM_TYPE, result_code);
	}


	void test_accept_03(const char *local_addr, int nSecs) {
		std::cout << std::endl
				<< "ACS_DSD_LSocketAcceptor::accept(...) - " << __FUNCTION__ << std::endl
				<< "  TEST:  Waiting for timeout expiration ( " << nSecs << " seconds )" << std::endl
				<< "  RESULT: when timeout expires the method returns an error (error code -3)" << std::endl;


		ACS_DSD_LSocketStream lsock_stream;
		ACE_Time_Value timeout(nSecs);

		int result_code = _acceptor.accept(lsock_stream, &timeout);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;


		if(result_code < 0)
		{
			char buff[256];
			std::cout << "  ERRNO == " << errno << std::endl;
			std::cout << "  ERRDESC == " << strerror_r(errno, buff, sizeof(buff)) << std::endl;
		}

		ASSERT_EQUAL(-3, result_code);
	}

	void test_get_local_addr_01(const char * srv_addr) {
		std::cout << std::endl
				<< _class_name << "::get_local_addr(...) - " << __FUNCTION__ << std::endl
				<< "  TEST: Get the local address where the service is listening." << std::endl
				<< "  RESULT: Print the local address. It should be '" << srv_addr << "'." << std::endl;

		ACE_UNIX_Addr address;

		int result_code = _acceptor.get_local_addr(address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		if (result_code == 0) {
			char address_str[1024] = "<no address>";
			address.addr_to_string(address_str, 1024);
			std::cout << "  THE LOCAL ADDRESS IS '" << address_str << "'" << std::endl << std::endl;
		} else print_errno(2, 1) && (std::cout << std::endl << std::endl);

			ASSERT_EQUAL(0, result_code);
		}

		int print_errno(int line_indent, int return_code) {
			while (line_indent-- > 0) std::cout << ' ';
			char buf[1024];
			std::cout << "SYSTEM ERROR: " << strerror_r(ACE_OS::last_error(), buf, 1024) << std::endl;

			return return_code;
		}

	//========//
	// Fields //
	//========//
private:
	ACS_DSD_LSocketAcceptor _acceptor;
	const char * _class_name;
};

#endif // HEADER_GUARD_CLASS__LSocketAcceptorCuteTester
