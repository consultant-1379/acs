//******************************************************************************
//
//  NAME
//     acs_nclib_session.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     acs_nclib_session class represents a session to NETCONF server via SSH2.
//
//  DOCUMENT NO
//	    190 89-CAX nnn nnnn
//
//  AUTHOR
//     2012-09-10 by EGINSAN PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************

#ifndef ACS_NCLIB_SESSION_H_
#define ACS_NCLIB_SESSION_H_

#include <stdint.h>
#include <vector>

#include "acs_nclib_message.h"
class acs_nclib_rpc;
class acs_nclib_rpc_reply;

class acs_nclib_udp;
class acs_nclib_udp_reply;
/*
 * Forward declarations
 */
class acs_nclib_session_impl;
class acs_nclib_tcp_session_impl;
class acs_nclib_udp_session_impl;

namespace acs_nclib {
enum SessionLayer
{
	SSH = 0,
	TCP = 1,
	UDP = 2
};
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_nclib_session

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:

	//Deprecated Constructor
	//No Protocol Specified, SSH is used as default.
	DEPRECATED(__CLASS_NAME__ ());

	__CLASS_NAME__ (acs_nclib::SessionLayer);

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	~__CLASS_NAME__ ();


	//===========//
	// Functions //
	//===========//
public:
	int open (const char * server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username);
	int open (uint32_t server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username);

	int setPassword(const char * pwd);
	int setPassphrase(const char * passphrase);
	int setPubicKey(const char * publickey);
	int setPrivateKey(const char * privatekey);

	int send(acs_nclib_message* msg);
	int receive(acs_nclib_message*& msg, int timeout = 1000);
	int send_sync(acs_nclib_rpc* request, acs_nclib_rpc_reply* &reply, int timeout = 1000);
	int send_sync(acs_nclib_udp* request, acs_nclib_udp_reply* &reply, int timeout = 1000);

	int lock(acs_nclib::Datastore);
	int unlock(acs_nclib::Datastore);

	int getId(int&);
	int getServerCapabilities(std::vector<const char*>&);

	int close ();


	ACS_NCLIB_ERROR_INFO_DECLARE_ACCESSORS;


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	acs_nclib::SessionLayer _protocol;
	acs_nclib_session_impl * _session_i;
	acs_nclib_tcp_session_impl * _tcp_session_i;
	acs_nclib_udp_session_impl * _udp_session_i;
};



#endif
