#ifndef HEADER_GUARD_CLASS__LSocketConnectorCuteTester
#define HEADER_GUARD_CLASS__LSocketConnectorCuteTester LSocketConnectorCuteTester

#include <iostream>

#include "cute.h"

#include "ace/UNIX_Addr.h"

#include "ACS_DSD_LSocketConnector.h"
#include "ACS_DSD_SPipeStream.h"

class LSocketConnectorCuteTester {
	//==============//
	// Constructors //
	//==============//
public:
	inline LSocketConnectorCuteTester() : _connector() {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual ~LSocketConnectorCuteTester() {}

	//===========//
	// Operators //
	//===========//
public:
	inline void operator()() {
		std::cout << "  --->>> PRESS ENTER TO FOLLOW THIS TEST...";
		getchar() && (std::cout << std::endl << std::endl);

		test_connect_01("/tmp/acs_dsd_lsock_no_service");
		test_connect_02();
		test_connect_03();
		test_connect_04("/tmp/acs_dsd_lsock_srv01");
	}

	//================//
	// Test Functions //
	//================//
private:
	void test_connect_01(const char * srv_addr) {
		std::cout << "ACS_DSD_LSocketConnector::connect(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Connect towards the UNIX local socket '" << srv_addr << "'." << std::endl
							<< "  RESULT: Connection not established (call return code != 0) because the local socket doesn't exist." << std::endl;

		ACS_DSD_LSocketStream stream;
		ACE_UNIX_Addr address(srv_addr);

		int result_code = _connector.connect(stream, address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		ASSERT(result_code);
	}

	void test_connect_02() {
		std::cout << "ACS_DSD_LSocketConnector::connect(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Try to provide a wrong stream object type to connect method." << std::endl
							<< "  RESULT: The connect method return with a warning code about the wrong type doesn't performing any connection." << std::endl;

		ACS_DSD_SPipeStream stream;
		ACE_UNIX_Addr address;

		int result_code = _connector.connect(stream, address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		ASSERT_EQUAL(ACS_DSD_Connector::WAR_CONNECT_WRONG_STREAM_TYPE, result_code);
	}

	void test_connect_03() {
		std::cout << "ACS_DSD_LSocketConnector::connect(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Try to provide a wrong address object type to connect method." << std::endl
							<< "  RESULT: The connect method return with a warning code about the wrong type doesn't performing any connection." << std::endl;

		ACS_DSD_LSocketStream stream;
		ACE_Addr address;

		int result_code = _connector.connect(stream, address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		ASSERT_EQUAL(ACS_DSD_Connector::WAR_CONNECT_WRONG_ADDRESS_TYPE, result_code);
	}

	void test_connect_04(const char * srv_addr) {
		std::cout << "ACS_DSD_LSocketConnector::connect(...) - " << __FUNCTION__ << std::endl
							<< "  TEST: Connect towards the UNIX local socket '" << srv_addr << "'." << std::endl
							<< "  RESULT: Connection established successfully (the local socket must exist)." << std::endl;

		ACS_DSD_LSocketStream stream;
		ACE_UNIX_Addr address(srv_addr);

		int result_code = _connector.connect(stream, address);

		std::cout << "  CALL RETURN CODE == " << result_code << std::endl << std::endl;

		ASSERT_EQUAL(0, result_code);

		result_code || ((std::cout << "  Press RETURN key after check...") && getchar() && (std::cout << std::endl));
	}

	//========//
	// Fields //
	//========//
private:
	ACS_DSD_LSocketConnector _connector;
};

#endif // HEADER_GUARD_CLASS__LSocketConnectorCuteTester
