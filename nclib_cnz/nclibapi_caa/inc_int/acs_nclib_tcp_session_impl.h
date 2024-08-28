/** @file acs_nclib_tcp_session_impl.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2013-07-05
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2013-07-05 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <stdint.h>
#include <vector>
#include <string>

#include "libssh2.h"

#include "acs_nclib_errorinfoprovider.h"
#include "acs_nclib_message.h"
#include "acs_nclib_tcp_client.h"

class acs_nclib_rpc;
class acs_nclib_rpc_reply;

class acs_nclib_tcp_session_impl : public acs_nclib_errorinfoprovider {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_nclib_tcp_session_impl ()
	: acs_nclib_errorinfoprovider(),
	  _client(0),
	  _msg_id_count(0),
	  _session_id(-1),
	  _server_cap(0),
	  _previous_buffer(0)
	{
		memset(_pwd, 0, MAXSIZE_PWD + 1);

	}


	//============//
	// Destructor //
	//============//
public:
	virtual ~acs_nclib_tcp_session_impl();


	//===========//
	// Functions //
	//===========//
public:
	int open (const char * server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username);
	int open (uint32_t server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username);

	int setPassword(const char * pwd);
	int setPassphrase(const char * /*passphrase*/){return acs_nclib::ERR_SIGNALS_ERROR;}; //TODO: Not Supported
	int setPubicKey(const char * /*publickey*/) {return acs_nclib::ERR_SIGNALS_ERROR;}; //TODO: Not Supported
	int setPrivateKey(const char * /*privatekey*/) {return acs_nclib::ERR_SIGNALS_ERROR;}; //TODO: Not Supported

	int send(acs_nclib_message* msg);
	int receive(acs_nclib_message*& msg,int timeout = 1000);
	int send_sync(acs_nclib_rpc* request, acs_nclib_rpc_reply* &reply, int timeout = 1000);

	int lock(acs_nclib::Datastore);
	int unlock(acs_nclib::Datastore);

	int close ();

	int getId(int&);
	int getServerCapabilities(std::vector<const char*>&);


private:
	int send_hello();
	int receive_hello();

	int wait_for_events(int count, int *eventHandles, int timeoutInMilliseconds);


private:
	acs_nclib_tcp_client* _client;
	char _pwd[MAXSIZE_PWD + 1];
	unsigned int _msg_id_count;
	int _session_id;
	std::vector<const char*> _server_cap;
	char* _previous_buffer;


};

