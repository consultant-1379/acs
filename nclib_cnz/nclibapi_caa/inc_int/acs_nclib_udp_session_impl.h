/** @file acs_nclib_udp_session_impl.h
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

#include "acs_nclib_errorinfoprovider.h"
#include "acs_nclib_message.h"
#include "acs_nclib_udp_client.h"
//#include "acs_nclib_udp.h"

class acs_nclib_udp;
class acs_nclib_udp_reply;

class acs_nclib_udp_session_impl : public acs_nclib_errorinfoprovider {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_nclib_udp_session_impl ()
	: acs_nclib_errorinfoprovider(),
	  _client(0), _msg_id_count(0)
	{
//		memset(_pwd, 0, MAXSIZE_PWD + 1);
//		memset(_msgid, 0, MSG_ID_SIZE + 1);

	}


	//============//
	// Destructor //
	//============//
public:
	virtual ~acs_nclib_udp_session_impl();


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
	int send_sync(acs_nclib_udp* request, acs_nclib_udp_reply* &reply, int timeout = 1000);

	int close ();

private:
	int wait_for_events(int count, int *eventHandles, int timeoutInMilliseconds);


private:
	acs_nclib_udp_client* _client;
//	char _pwd[MAXSIZE_PWD + 1];
	unsigned int _msg_id_count;
//	char _msgid[MSG_ID_SIZE + 1];
//	int _session_id;
//	std::vector<const char*> _server_cap;
//	char* _previous_buffer;


};

