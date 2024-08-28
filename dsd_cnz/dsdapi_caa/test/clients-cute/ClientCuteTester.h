#ifndef HEADER_GUARD_CLASS__ClientCuteTester
#define HEADER_GUARD_CLASS__ClientCuteTester ClientCuteTester

#include <cstdio>
#include <cstring>
#include <alloca.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include "cute.h"

#include "CuteTester.h"

#include "ACS_DSD_SocketConnector.h"
#include "ACS_DSD_SocketStream.h"
#include "ACS_DSD_LSocketConnector.h"
#include "ACS_DSD_LSocketStream.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ClientCuteTester

template < typename connector_t, typename stream_t, typename addr_t >
class __CLASS_NAME__ : public CuteTester {
	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__() : CuteTester(), _connector(), _stream(), _connector_class_name("??"), _stream_class_name("??") {
		if (dynamic_cast<ACS_DSD_SocketConnector *>(&_connector)) _connector_class_name = "ACS_DSD_SocketConnector";
		else if (dynamic_cast<ACS_DSD_LSocketConnector *>(&_connector)) _connector_class_name = "ACS_DSD_LSocketConnector";

		if (dynamic_cast<ACS_DSD_SocketStream *>(&_stream)) _stream_class_name = "ACS_DSD_SocketStream";
		else if (dynamic_cast<ACS_DSD_LSocketStream *>(&_stream)) _stream_class_name = "ACS_DSD_LSocketStream";
	}

	//============//
	// Destructor //
	//============//
public:
	inline virtual ~__CLASS_NAME__() {}

	//===========//
	// Operators //
	//===========//
public:
	inline virtual void operator()() {
		std::cout << "--->>> Press ENTER key to follow the basic unit test about the '" << _connector_class_name << "' and '" << _stream_class_name << "' classes functionalities at client side...";
		getchar(), (std::cout << std::endl);

		const char * const sap_address = dynamic_cast<ACS_DSD_SocketConnector *>(&_connector) ? "192.168.169.1:50000" : "acs_dsd_test_srv_01";

		test_connector_connect_01(sap_address);
		test_stream_state_01(ACS_DSD_IOStream::STREAM_STATE_CONNECTED);
		test_stream_get_option_01();
		test_stream_set_option_01();
		test_stream_get_option_01();
		test_stream_get_local_addr_01();
		test_stream_get_remote_addr_01();

		test_stream_sendv_01<2>();
		test_stream_recvv_01();
		test_stream_sendv_01<1>();
		test_stream_recvv_02();
		test_stream_send_01<80>();
		test_stream_recv_01();
		test_stream_send_02();
		test_stream_recv_02();
		test_stream_sendv_n_01<8>();
		test_stream_recvv_n_01();
		test_stream_send_n_01<26>();
		test_stream_recv_n_01();


		test_stream_close_writer_01();
		test_stream_state_01(ACS_DSD_IOStream::STREAM_STATE_CONNECTED_READER_ONLY);
		test_stream_close_reader_01();
		test_stream_state_01(ACS_DSD_IOStream::STREAM_STATE_DISCONNECTED);
		test_stream_close_01();
	}

	//================//
	// Test Functions //
	//================//
protected:
	void test_connector_connect_01(const char * sap_address) {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: This client tries to connect towards the server listening on the address '" << sap_address << "'." << ::std::endl
								<< "  PRE-CONDITIONS: The user should run the servers-cute tester application that will accept this client's connection." << ::std::endl
								<< "  RESULT: A connection will be successfully established with the server (check it using netstat command then press ENTER)." << ::std::endl;

		addr_t addr(sap_address);
		const int call_result = _connector.connect(_stream, addr);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result < 0) && print_errno(2, ACE_OS::last_error(), 1);

		ASSERT_EQUAL(0, call_result);

		(::std::cout << "  Press ENTER key after check...") && getchar();
	}

	void test_stream_state_01(ACS_DSD_IOStream::IOStreamStateConstants check_state) {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the state of the client I/O stream object." << ::std::endl
								<< "  RESULT: The method returns the state value that whould be equal to " << check_state << "." << ::std::endl;

		ACS_DSD_IOStream::IOStreamStateConstants state = _stream.state();
		::std::cout << "  Client I/O stream state == " << state << "." << ::std::endl;

		ASSERT_EQUAL(check_state, state);
	}

	void test_stream_get_option_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the value of the SO_KEEPALIVE socket option of the I/O stream object." << ::std::endl
								<< "  RESULT: The I/O stream object returns the SO_KEEPALIVE option value." << ::std::endl;

		int opt_value = 0;
		int opt_length = sizeof(opt_value);
		const int call_result = _stream.get_option(SOL_SOCKET, SO_KEEPALIVE, &opt_value, &opt_length);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  SO_KEEPALIVE option value == " << opt_value << "." << ::std::endl; //OK

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_close_writer_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Close the writer part of the I/O communication session." << ::std::endl
								<< "  PRE-CONDITIONS: You should synchronize this test with the server tester test 'test_stream_close_reader_01' that should be called after." << ::std::endl
								<< "  RESULT: The writer part is closed and the client isn't able to send any data to the server tester application." << ::std::endl;

		(::std::cout << "  Use ENTER key to synchronize with the server...") && getchar();
		const int call_result = _stream.close_writer();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		call_result && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_close_reader_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Close the reader part of the I/O communication session." << ::std::endl
								<< "  PRE-CONDITIONS: You should synchronize this test with the server tester test 'test_stream_close_writer_01' that should be called after." << ::std::endl
								<< "  RESULT: The reader part is closed and the client isn't able to receive any data from the server tester application." << ::std::endl;

		(::std::cout << "  Use ENTER key to synchronize with the client...") && getchar();
		const int call_result = _stream.close_reader();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		call_result && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_close_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Close the I/O communication session." << ::std::endl
								<< "  RESULT: The session at client side is closed." << ::std::endl;

		const int call_result = _stream.close();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		call_result && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_set_option_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Set the value of the SO_KEEPALIVE socket option of the I/O stream client object to 1 (true) so that the keep alive will be active." << ::std::endl
								<< "  RESULT: The I/O stream object will have the SO_KEEPALIVE option value set." << ::std::endl;

		int opt_value = 1;
		const int call_result = _stream.set_option(SOL_SOCKET, SO_KEEPALIVE, &opt_value, sizeof(opt_value));
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  SO_KEEPALIVE option set to " << opt_value << "." << ::std::endl; //OK

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_get_local_addr_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the I/O stream local address." << ::std::endl
								<< "  RESULT: The I/O stream object returns the address info." << ::std::endl;

		addr_t addr;
		const int call_result = _stream.get_local_addr(addr);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else { //OK
			char addr_buf[1024];
			addr.addr_to_string(addr_buf, 1024);
			::std::cout << "  I/O stream local address == '" << addr_buf << "'." << ::std::endl;
		}

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_get_remote_addr_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the I/O stream remote address." << ::std::endl
								<< "  RESULT: The I/O stream object returns the address info." << ::std::endl;

		addr_t addr;
		const int call_result = _stream.get_remote_addr(addr);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else { //OK
			char addr_buf[1024];
			addr.addr_to_string(addr_buf, 1024);
			::std::cout << "  I/O stream remote address == '" << addr_buf << "'." << ::std::endl;
		}

		ASSERT_EQUAL(0, call_result);
	}

	template <int n>
	void test_stream_sendv_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a vector of " << n << " buffer(s) of data to the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[n][32];
		iovec iov[n];
		for (int i = 0; i < n; ++i)
			::snprintf(reinterpret_cast<char *>(iov[i].iov_base = buffers[i]), iov[i].iov_len = 32, "test_stream_sendv_01: iov[%04d]", i);

		const int call_result = _stream.sendv(iov, n);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result <= 0) && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT(call_result > 0);
	}

	void test_stream_recvv_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a vector of data from the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[4][32];
		iovec iov[4] = {{buffers[0], 32}, {buffers[1], 32}, {buffers[2], 32}, {buffers[3], 32}};

		const int call_result = _stream.recvv(iov, 4);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  Data vector received:" << ::std::endl
										 << "    [0]: '" << buffers[0] << "'" << ::std::endl
										 << "    [1]: '" << buffers[1] << "'" << ::std::endl
										 << "    [2]: '" << buffers[2] << "'" << ::std::endl
										 << "    [3]: '" << buffers[3] << "'" << ::std::endl;

		ASSERT(call_result > 0);
	}

	template <int n>
	void test_stream_send_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a buffer of " << n << " characters to the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffer[n + 1];
		for (int i = 0; i < n; ++i) buffer[i] = static_cast<char>('A' + (i % ('Z' - 'A' + 1)));
		buffer[n] = 0;
		::std::cout << "  Buffer to send == '" << buffer << "'" << ::std::endl;

		const int call_result = _stream.send(buffer, n + 1);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result <= 0) && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT(call_result > 0);
	}

	void test_stream_recvv_02() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive an iovec buffer of data from the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		iovec iov;

		const int call_result = _stream.recvv(&iov);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else {
			::std::cout << "  an iovec data buffer of " << iov.iov_len << " received: '" << reinterpret_cast<char *>(iov.iov_base) << "'" << ::std::endl;
			delete[] reinterpret_cast<char *>(iov.iov_base);
		}

		ASSERT(call_result > 0);
	}

	void test_stream_recv_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a buffer of data from the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		unsigned char buffer[4096];

		const int call_result = _stream.recv(buffer, 4096);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  a buffer of " << call_result << " bytes received: '" << buffer << "'" << ::std::endl;

		ASSERT(call_result > 0);
	}

	void test_stream_send_02() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a sequence of three buffers to the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		const int call_result = _stream.send(6, "Cane", 5, "Gatto", 6, "Coniglio", 9);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result <= 0) && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT(call_result > 0);
	}

	void test_stream_recv_02() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a sequence of four buffers from the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[4][11];

		const int call_result = _stream.recv(8, buffers[0], 6, buffers[1], 9, buffers[2], 8, buffers[3], 11);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else for (int i = 0; i < 4; ++i) ::printf("  [%d] == '%s'\n", i, buffers[i]);

		ASSERT(call_result > 0);
	}

	template <int n>
	void test_stream_sendv_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a vector of " << n << " buffer(s) of data to the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[n][34];
		iovec iov[n];
		for (int i = 0; i < n; ++i)
			::snprintf(reinterpret_cast<char *>(iov[i].iov_base = buffers[i]), iov[i].iov_len = 34, "test_stream_sendv_n_01: iov[%04d]", i);

		size_t bytes_transferred = 0;
		const int call_result = _stream.sendv_n(iov, n, 0, &bytes_transferred);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result <= 0) && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT(call_result > 0);
	}

	void test_stream_recvv_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a vector of data from the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[5][48];
		iovec iov[5];
		for (int i = 0; i < 5; ++i) iov[i].iov_base = buffers[i], iov[i].iov_len = 48;

		size_t bytes_transferred = 0;
		const int call_result = _stream.recvv_n(iov, 5, 0, &bytes_transferred);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else {
			::std::cout << "  Data vector received:" << ::std::endl;
			for (int i = 0; i < 5; ++i) ::printf("    [%d]: '%s'\n", i, buffers[i]);
		}

		ASSERT(call_result > 0);
	}

	template <int n>
	void test_stream_send_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a buffer of " << n << " characters to the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffer[n + 1];
		for (int i = 0; i < n; ++i) buffer[i] = static_cast<char>('A' + (i % ('Z' - 'A' + 1)));
		buffer[n] = 0;
		::std::cout << "  Buffer to send == '" << buffer << "'" << ::std::endl;

		size_t bytes_transferred = 0;
		const int call_result = _stream.send_n(buffer, n + 1, 0, &bytes_transferred);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result <= 0) && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT(call_result > 0);
	}

	void test_stream_recv_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a buffer of data from the server." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		unsigned char buffer[4096];
		size_t bytes_transferred = 0;

		const int call_result = _stream.recv_n(buffer, 53, 0, &bytes_transferred);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  a buffer of " << bytes_transferred << " bytes received: '" << buffer << "'" << ::std::endl;

		ASSERT(call_result > 0);
	}

	//========//
	// Fields //
	//========//
private:
	connector_t _connector;
	stream_t _stream;
	const char * _connector_class_name;
	const char * _stream_class_name;
};

#endif // HEADER_GUARD_CLASS__ClientCuteTester
