#ifndef HEADER_GUARD_CLASS__ServerCuteTester
#define HEADER_GUARD_CLASS__ServerCuteTester ServerCuteTester

#include <cstdio>
#include <errno.h>

#include <iostream>

#include "ace/Time_Value.h"

#include "cute.h"

#include "CuteTester.h"

#include "ACS_DSD_SocketConnector.h"
#include "ACS_DSD_SocketStream.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Server.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ServerCuteTester

class __CLASS_NAME__ : public CuteTester {
	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__ () : CuteTester() /*, _inet_server(), _unix_server(), _session()*/ {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual ~__CLASS_NAME__ () {}

	//===========//
	// Operators //
	//===========//
public:
	virtual void operator() ();

	//================//
	// Test Functions //
	//================//
protected:
/*
	void test_configuration_01 ();
*/

/*
	void test_server_state_01 (acs_dsd::ServerStateConstants check_value);

	void test_server_service_mode_01 (acs_dsd::ServiceModeConstants check_value);

	void test_server_open_01 (acs_dsd::ErrorConstants check_value);

	void test_server_open_02 (acs_dsd::ErrorConstants check_value);

	void test_server_open_03 (acs_dsd::ErrorConstants check_value);

	void test_server_open_04 (acs_dsd::ServiceModeConstants service_mode, const char * service_id = 0);

	void test_server_close_01 ();

	void test_server_accept_01 (acs_dsd::ErrorConstants check_value, unsigned timeout_ms = 0);
*/

	//========//
	// Fields //
	//========//
private:
	//ACS_DSD_Server _inet_server;
	//ACS_DSD_Server _unix_server;
	//ACS_DSD_Session _session;
};

#endif // HEADER_GUARD_CLASS__ServerCuteTester
