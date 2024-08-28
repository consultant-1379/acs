#ifndef HEADER_GUARD_CLASS__ServerCuteTester
#define HEADER_GUARD_CLASS__ServerCuteTester ServerCuteTester

#include <cstdio>
#include <cstring>
#include <alloca.h>
#include <sys/socket.h>

#include "cute.h"

#include "CuteTester.h"

#include "ACS_DSD_SocketAcceptor.h"
#include "ACS_DSD_SocketStream.h"
#include "ACS_DSD_LSocketAcceptor.h"
#include "ACS_DSD_LSocketStream.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ServerCuteTester

template < typename acceptor_t, typename stream_t, typename addr_t >
class __CLASS_NAME__ : public CuteTester {
	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__() : CuteTester(), _acceptor(), _stream(), _acceptor_class_name("??"), _stream_class_name("??") {
		if (dynamic_cast<ACS_DSD_SocketAcceptor *>(&_acceptor)) _acceptor_class_name = "ACS_DSD_SocketAcceptor";
		else if (dynamic_cast<ACS_DSD_LSocketAcceptor *>(&_acceptor)) _acceptor_class_name = "ACS_DSD_LSocketAcceptor";

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
		std::cout << "--->>> Press ENTER key to follow the basic unit test about the '" << _acceptor_class_name << "' and '" << _stream_class_name << "' classes functionalities at server side...";
		getchar(), (std::cout << std::endl);

		const char * const sap_address = dynamic_cast<ACS_DSD_SocketAcceptor *>(&_acceptor) ? "0.0.0.0:50000" : "acs_dsd_test_srv_01";

		test_acceptor_open_01(sap_address);
		test_acceptor_get_local_addr_01();
		test_acceptor_get_option_01();
		test_acceptor_state_01(ACS_DSD_Acceptor::ACCEPTOR_STATE_OPEN);
		test_acceptor_set_option_01();
		test_acceptor_get_option_01();
		test_acceptor_accept_01();

		test_stream_state_01(ACS_DSD_IOStream::STREAM_STATE_CONNECTED);
		test_stream_get_option_01();
		test_stream_get_local_addr_01();
		test_stream_get_remote_addr_01();

		test_stream_recvv_01();
		test_stream_sendv_01<4>();
		test_stream_recvv_02();
		test_stream_sendv_01<1>();
		test_stream_recv_01();
		test_stream_send_01<127>();
		test_stream_recv_02();
		test_stream_send_02();
		test_stream_recvv_n_01();
		test_stream_sendv_n_01<5>();
		test_stream_recv_n_01();
		test_stream_send_n_01<52>();


		test_stream_close_reader_01();
		test_stream_state_01(ACS_DSD_IOStream::STREAM_STATE_CONNECTED_WRITER_ONLY);
		test_stream_close_writer_01();
		test_stream_state_01(ACS_DSD_IOStream::STREAM_STATE_DISCONNECTED);
		test_stream_close_01();

		test_acceptor_close_01(sap_address);
		test_acceptor_remove_01(sap_address);
	}

	//================//
	// Test Functions //
	//================//
protected:
	void test_acceptor_open_01(const char * sap_address) {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Open the server listening on the address '" << sap_address << "'." << ::std::endl
								<< "  RESULT: The server is opened successfully (check it using netstat command then press ENTER)." << ::std::endl;

		addr_t addr(sap_address);
		const int call_result = _acceptor.open(addr);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result < 0) && print_errno(2, ACE_OS::last_error(), 1);

		ASSERT_EQUAL(0, call_result);

		(::std::cout << "  Press ENTER key after check...") && getchar();
	}

	void test_acceptor_get_local_addr_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the SAP (Service Access Point) local address on which the service is listening." << ::std::endl
								<< "  RESULT: The acceptor object returns the address info." << ::std::endl;

		addr_t addr;
		const int call_result = _acceptor.get_local_addr(addr);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else { //OK
			char addr_buf[1024];
			addr.addr_to_string(addr_buf, 1024);
			::std::cout << "  SAP local address == '" << addr_buf << "'." << ::std::endl;
		}

		ASSERT_EQUAL(0, call_result);
	}

	void test_acceptor_get_option_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the value of the SO_KEEPALIVE socket option of the SAP acceptor object." << ::std::endl
								<< "  RESULT: The acceptor object returns the SO_KEEPALIVE option value." << ::std::endl;

		int opt_value = 0;
		int opt_length = sizeof(opt_value);
		const int call_result = _acceptor.get_option(SOL_SOCKET, SO_KEEPALIVE, &opt_value, &opt_length);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  SO_KEEPALIVE option value == " << opt_value << "." << ::std::endl; //OK

		ASSERT_EQUAL(0, call_result);
	}

	void test_acceptor_close_01(const char * sap_address) {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Close the server listening on the address '" << sap_address << "'." << ::std::endl
								<< "  RESULT: The server will be closed successfully (check it using netstat command then press ENTER)." << ::std::endl;

		const int call_result = _acceptor.close();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result < 0) && print_errno(2, ACE_OS::last_error(), 1);

		ASSERT_EQUAL(0, call_result);

		(::std::cout << "  Press ENTER key after check...") && getchar();
	}

	void test_acceptor_remove_01(const char * sap_address) {
		int expected_call_result = 0;

		if (dynamic_cast<ACS_DSD_SocketAcceptor *>(&_acceptor)) {
			expected_call_result = ACS_DSD_Acceptor::WAR_REMOVE_NOT_ALLOWED;
			::std::cout << __FUNCTION__ << ::std::endl
									<< "  TEST: Remove function has no sense for the INET server listening on the address '" << sap_address << "'." << ::std::endl
									<< "  RESULT: The function will return the warning code " << ACS_DSD_Acceptor::WAR_REMOVE_NOT_ALLOWED << " and nothing will be done." << ::std::endl;
		} else ::std::cout << __FUNCTION__ << ::std::endl
											 << "  TEST: Remove the file system I-Node associated with the local UNIX server listening on the address '" << sap_address << "'." << ::std::endl
											 << "  RESULT: The file will be removed (check it using the ls command the press ENTER)." << ::std::endl;

		const int call_result = _acceptor.remove();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result < 0) && print_errno(2, ACE_OS::last_error(), 1);

		ASSERT_EQUAL(expected_call_result, call_result);

		(::std::cout << "  Press ENTER key after check...") && getchar();
	}

	void test_acceptor_state_01(ACS_DSD_Acceptor::AcceptorStateConstants check_state) {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the state of the SAP acceptor object." << ::std::endl
								<< "  RESULT: The method returns the state value that whould be equal to " << check_state << "." << ::std::endl;

		ACS_DSD_Acceptor::AcceptorStateConstants state = _acceptor.state();
		::std::cout << "  Server acceptor state == " << state << "." << ::std::endl;

		ASSERT_EQUAL(check_state, state);
	}

	void test_acceptor_set_option_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Set the value of the SO_KEEPALIVE socket option of the SAP acceptor object to 1 (true) so that the keep alive will be active." << ::std::endl
								<< "  RESULT: The acceptor object will have the SO_KEEPALIVE option value set." << ::std::endl;

		int opt_value = 1;
		const int call_result = _acceptor.set_option(SOL_SOCKET, SO_KEEPALIVE, &opt_value, sizeof(opt_value));
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  SO_KEEPALIVE option set to " << opt_value << "." << ::std::endl; //OK

		ASSERT_EQUAL(0, call_result);
	}

	void test_acceptor_accept_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Wait for a client to connect." << ::std::endl
								<< "  PRE-CONDITIONS: The user should run the clients-cute tester application that will connect to this server." << ::std::endl
								<< "  RESULT: The server will accept the client connection (check it using netstat command then press ENTER)." << ::std::endl;

		const int call_result = _acceptor.accept(_stream);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result) print_errno(2, ACE_OS::last_error(), 1);
		else ::std::cout << "A client was connected successfully." << ::std::endl; //OK

		ASSERT_EQUAL(0, call_result);

		(::std::cout << "  Press ENTER key after check...") && getchar();
	}

	void test_stream_state_01(ACS_DSD_IOStream::IOStreamStateConstants check_state) {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Get the state of the server I/O stream object." << ::std::endl
								<< "  RESULT: The method returns the state value that whould be equal to " << check_state << "." << ::std::endl;

		ACS_DSD_IOStream::IOStreamStateConstants state = _stream.state();
		::std::cout << "  Server I/O stream state == " << state << "." << ::std::endl;

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

	void test_stream_close_reader_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Close the reader part of the I/O communication session." << ::std::endl
								<< "  PRE-CONDITIONS: You should synchronize this test with the client tester test 'test_stream_close_writer_01' that should be called before." << ::std::endl
								<< "  RESULT: The reader part is closed and the server isn't able to receive any data from the client tester application." << ::std::endl;

		(::std::cout << "  Use ENTER key to synchronize with the client...") && getchar();
		const int call_result = _stream.close_reader();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		call_result && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_close_writer_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Close the writer part of the I/O communication session." << ::std::endl
								<< "  PRE-CONDITIONS: You should synchronize this test with the client tester test 'test_stream_close_reader_01' that should be called before." << ::std::endl
								<< "  RESULT: The writer part is closed and the server isn't able to send any data to the client tester application." << ::std::endl;

		(::std::cout << "  Use ENTER key to synchronize with the client...") && getchar();
		const int call_result = _stream.close_writer();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		call_result && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT_EQUAL(0, call_result);
	}

	void test_stream_close_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Close the I/O communication session." << ::std::endl
								<< "  RESULT: The session at server side is closed." << ::std::endl;

		const int call_result = _stream.close();
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		call_result && print_errno(2, ACE_OS::last_error(), 1); //ERROR

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

	void test_stream_recvv_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a vector of data from the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[2][32];
		iovec iov[2] = {{buffers[0], 32}, {buffers[1], 32}};

		const int call_result = _stream.recvv(iov, 2);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  Data vector received:" << ::std::endl
										 << "    [0]: '" << buffers[0] << "'" << ::std::endl
										 << "    [1]: '" << buffers[1] << "'" << ::std::endl;

		ASSERT(call_result > 0);
	}

	template <int n>
	void test_stream_sendv_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a vector of " << n << " buffer(s) of data to the client." << ::std::endl
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

	void test_stream_recvv_02() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive an iovec buffer of data from the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		iovec iov;

		const int call_result = _stream.recvv(&iov);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else {
			::std::cout << "  an iovec data buffer of " << iov.iov_len << " bytes received: '" << reinterpret_cast<char *>(iov.iov_base) << "'" << ::std::endl;
			delete[] reinterpret_cast<char *>(iov.iov_base);
		}

		ASSERT(call_result > 0);
	}

	template <int n>
	void test_stream_send_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a buffer of " << n << " characters to the client." << ::std::endl
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

	void test_stream_recv_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a buffer of data from the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		unsigned char buffer[4096];

		const int call_result = _stream.recv(buffer, 4096);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  a buffer of " << call_result << " bytes received: '" << buffer << "'" << ::std::endl;

		ASSERT(call_result > 0);
	}

	void test_stream_recv_02() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a sequence of three buffers from the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[3][9];

		const int call_result = _stream.recv(6, buffers[0], 5, buffers[1], 6, buffers[2], 9);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else for (int i = 0; i < 3; ++i) ::printf("  [%d] == '%s'\n", i, buffers[i]);

		ASSERT(call_result > 0);
	}

	void test_stream_send_02() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a sequence of four buffers to the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		const int call_result = _stream.send(8, "Leone", 6, "Elefante", 9, "Pantera", 8, "Gattopardo", 11);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result <= 0) && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT(call_result > 0);
	}

	void test_stream_recvv_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a vector of data from the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[8][34];
		iovec iov[8];
		for (int i = 0; i < 8; ++i) iov[i].iov_base = buffers[i], iov[i].iov_len = 34;

		size_t bytes_transferred = 0;
		const int call_result = _stream.recvv_n(iov, 8, 0, &bytes_transferred);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else {
			::std::cout << "  Data vector received:" << ::std::endl;
			for (int i = 0; i < 8; ++i) ::printf("    [%d]: '%s'\n", i, buffers[i]);
		}

		ASSERT(call_result > 0);
	}

	template <int n>
	void test_stream_sendv_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a vector of " << n << " buffer(s) of data to the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		char buffers[n][48];
		iovec iov[n];
		for (int i = 0; i < n; ++i)
			::snprintf(reinterpret_cast<char *>(iov[i].iov_base = buffers[i]), iov[i].iov_len = 48, "test_stream_sendv_n_01: iov[%04d] by the server", i);

		size_t bytes_transferred = 0;
		const int call_result = _stream.sendv_n(iov, n, 0, &bytes_transferred);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		(call_result <= 0) && print_errno(2, ACE_OS::last_error(), 1); //ERROR

		ASSERT(call_result > 0);
	}

	void test_stream_recv_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Receive a buffer of data from the client." << ::std::endl
								<< "  RESULT: The data will be transferred successfully." << ::std::endl;

		unsigned char buffer[4096];
		size_t bytes_transferred = 0;

		const int call_result = _stream.recv_n(buffer, 27, 0, &bytes_transferred);
		::std::cout << "  CALL RESULT CODE == " << call_result << ::std::endl;
		if (call_result <= 0) print_errno(2, ACE_OS::last_error(), 1); //ERROR
		else ::std::cout << "  a buffer of " << bytes_transferred << " bytes received: '" << buffer << "'" << ::std::endl;

		ASSERT(call_result > 0);
	}

	template <int n>
	void test_stream_send_n_01() {
		::std::cout << __FUNCTION__ << ::std::endl
								<< "  TEST: Send a buffer of " << n << " characters to the client." << ::std::endl
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

	//========//
	// Fields //
	//========//
private:
	acceptor_t _acceptor;
	stream_t _stream;
	const char * _acceptor_class_name;
	const char * _stream_class_name;
};

#endif // HEADER_GUARD_CLASS__ServerCuteTester
