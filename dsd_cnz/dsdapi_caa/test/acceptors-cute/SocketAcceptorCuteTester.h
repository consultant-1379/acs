#ifndef HEADER_GUARD_CLASS__SocketAcceptorCuteTester
#define HEADER_GUARD_CLASS__SocketAcceptorCuteTester SocketAcceptorCuteTester

#include <cstdio>

#include <iostream>

#include "cute.h"

#include "ACS_DSD_SocketAcceptor.h"
#include "ACS_DSD_SPipeStream.h"

class SocketAcceptorCuteTester {
	//==============//
	// Constructors //
	//==============//
public:
	inline SocketAcceptorCuteTester() : _acceptor(), _class_name("ACS_DSD_SocketAcceptor") {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual ~SocketAcceptorCuteTester() {}

	//===========//
	// Operators //
	//===========//
public:
	inline void operator()() {
		std::cout << "  --->>> PRESS ENTER TO FOLLOW THIS TEST...";
		getchar() && (std::cout << std::endl << std::endl);

		test_open_01();
		test_close_01();
		test_open_02("127.0.0.1:54321");
		test_close_02("127.0.0.1:54321");
		test_open_02("127.0.0.1:54321");
		test_get_local_addr_01("127.0.0.1:54321");
		test_remove_01();
		test_accept_01("127.0.0.1:54321", 10);
		test_accept_02("127.0.0.1:54321");
		test_close_02("127.0.0.1:54321");
	}

	//================//
	// Test Functions //
	//================//
private:
	void test_open_01() {
		char srv_addr[] = "1.2.3.4:12345";

		std::cout << _class_name << "::open(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Open a socket service on a not valid address '" << srv_addr << "'." << std::endl
							<< "  RESULT: No socket created and opened (call return code != 0) because the address cannot bound to any interface." << std::endl;

		ACE_INET_Addr address(srv_addr);

		int result_code = _acceptor.open(address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		(result_code < 0) && print_errno(2, 1) && (std::cout << std::endl);

		ASSERT(result_code);
	}

	void test_open_02(const char * srv_addr) {
		std::cout << _class_name << "::open(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Open a socket service on the address '" << srv_addr << "'." << std::endl
							<< "  RESULT: A listening INET socket will be created and opened (check it using netstat command)." << std::endl;

		ACE_INET_Addr address(srv_addr);

		int result_code = _acceptor.open(address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		(result_code < 0) && print_errno(2, 1) && (std::cout << std::endl);

		ASSERT_EQUAL(0, result_code);

		result_code || ((std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl));
	}

	void test_close_01() {
		std::cout << _class_name << "::close(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Try to close an INET service not previously opened." << std::endl
							<< "  RESULT: The method ignores the action because the object isn't opened." << std::endl;

		int result_code = _acceptor.close();

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		(result_code < 0) && print_errno(2, 1);
		std::cout << std::endl;

		ASSERT_EQUAL(0, result_code);
	}

	void test_close_02(const char * srv_addr) {
		std::cout << _class_name << "::close(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Close the service opened above in the previous test on the address '" << srv_addr << "'." << std::endl
							<< "  RESULT: The INET service is closed (check it using netstat command)." << std::endl;

		int result_code = _acceptor.close();

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		(result_code < 0) && print_errno(2, 1) && (std::cout << std::endl);

		ASSERT_EQUAL(0, result_code);

		result_code || ((std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl));
	}

	void test_accept_01(const char * srv_addr, unsigned timeout_sec) {
		std::cout << _class_name << "::accept(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Test a timeout expiration while waiting for a client to establish a connection towards the address '" << srv_addr << "'. The timeout is set to " << timeout_sec << " seconds." << std::endl
							<< "  RESULT: The accept method return with an error." << std::endl;

		ACS_DSD_SocketStream stream;
		ACE_Time_Value timeout(timeout_sec);
		int result_code = _acceptor.accept(stream, &timeout);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		(result_code < 0) && print_errno(2, 1) && (std::cout << std::endl);

		ASSERT_EQUAL(ACS_DSD_Acceptor::ERR_SYSTEM_ACCEPT, result_code);
	}

	void test_accept_02(const char * srv_addr) {
		std::cout << _class_name << "::accept(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Wait for ever a client to establish a connection towards the address '" << srv_addr << "'." << std::endl
							<< "  RESULT: The INET service will accept the client and a connection will be established (check using netstat)." << std::endl;

		ACS_DSD_SocketStream stream;
		int result_code = _acceptor.accept(stream);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		(result_code < 0) && print_errno(2, 1) && (std::cout << std::endl);

		ASSERT_EQUAL(0, result_code);

		result_code || ((std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl));
	}

	void test_get_local_addr_01(const char * srv_addr) {
		std::cout << _class_name << "::get_local_addr(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Get the local address where the service is listening." << std::endl
							<< "  RESULT: Print the local address. It should be '" << srv_addr << "'." << std::endl;

		ACE_INET_Addr address;

		int result_code = _acceptor.get_local_addr(address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl;
		if (result_code == 0) {
			char address_str[1024] = "<no address>";
			address.addr_to_string(address_str, 1024);
			std::cout << "  THE LOCAL ADDRESS IS '" << address_str << "'" << std::endl;
		} else print_errno(2, 1);
		std::cout << std::endl;

		ASSERT_EQUAL(0, result_code);
	}

	void test_remove_01() {
		std::cout << _class_name << "::remove(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Try to call remove method that is meaningless for INET sockets." << std::endl
							<< "  RESULT: The remove method returns with a warning notifying the client that the call is not allowed." << std::endl;

		int result_code = _acceptor.remove();

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		ASSERT_EQUAL(ACS_DSD_Acceptor::WAR_REMOVE_NOT_ALLOWED_ON_SOCKET, result_code);
	}

	//===========//
	// Functions //
	//===========//
private:
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
	ACS_DSD_SocketAcceptor _acceptor;
	const char * _class_name;
};

#endif // HEADER_GUARD_CLASS__SocketAcceptorCuteTester
