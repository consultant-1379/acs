/** @file acs_nclib_session_impl.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-10-01
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
 *	| R-001 | 2012-10-01 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <stdint.h>
#include <vector>
#include <string>

#include "libssh2.h"

#include "acs_nclib_errorinfoprovider.h"
#include "acs_nclib_message.h"

class acs_nclib_rpc;
class acs_nclib_rpc_reply;

class acs_nclib_session_impl : public acs_nclib_errorinfoprovider {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_nclib_session_impl ()
	: acs_nclib_errorinfoprovider(),
	  _sock(-1),
	  _ssh2_session(0),
	  _ssh2_channel(0),
	  _user_auth_list(0),
	  _user_auth_mask(acs_nclib::USER_AUTH_NONE),
	  _msg_id_count(0),
	  _session_id(-1),
	  _server_cap(0),
	  _previous_buffer(0)
	  {
		memset(_pwd, 0, MAXSIZE_PWD + 1);
		memset(_passphrase, 0, MAXSIZE_PPHRASE + 1);
		memset(_publickey, 0, MAXSIZE_PUBKEY + 1);
		memset(_privatekey, 0, MAXSIZE_PRIKEY + 1);
	  }

private:
	acs_nclib_session_impl (const acs_nclib_session_impl & rhs);


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_nclib_session_impl () {}


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
	int receive(acs_nclib_message*& msg,int timeout=1000);
	int send_sync(acs_nclib_rpc* request, acs_nclib_rpc_reply* &reply, int timeout = 1000);

	int lock(acs_nclib::Datastore);
	int unlock(acs_nclib::Datastore);

	int close ();

	int getId(int&);
	int getServerCapabilities(std::vector<const char*>&);

private:
	int connect (const char * server_host_addr, uint16_t server_host_port);
	int connect (uint32_t server_host_addr, uint16_t server_host_port);

	int ssh2_session_open (acs_nclib::UserAuthMethods auth_method, const char * username, ...);
	int ssh2_session_free (int provide_error_info = 0);
	int ssh2_get_userauth_list (const char * username);
	int ssh2_session_disconnect (const char * description, int provide_error_info = 0);

	int ssh2_authenticate_password (const char * username, const char * password);
	int ssh2_authenticate_kbd_interactive (const char * username);
	int ssh2_authenticate_publickey ();
	int ssh2_authenticate_publickey_fromfile (const char * username);

	int send_hello();
	int receive_hello();


	int ssh2_channel_open();
	int ssh2_channel_free (int provide_error_info = 0);


	// BEGIN: SSH2 library callback support
	static LIBSSH2_PASSWD_CHANGEREQ_FUNC(ssh2_password_change_request);
	static LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC(ssh2_userauth_kbdint_response);
	// END: SSH2 library callback support

//	void ssh2_userauth_kbdint_response();
//	void ssh2_password_change_request();

	//===========//
	// Operators //
	//===========//
private:
	acs_nclib_session_impl & operator= (const acs_nclib_session_impl & rhs);


	//========//
	// Fields //
	//========//
private:
	int _sock;
	LIBSSH2_SESSION * _ssh2_session;
	LIBSSH2_CHANNEL *_ssh2_channel;
	const char * _user_auth_list;
	unsigned _user_auth_mask;
	unsigned int _msg_id_count;
	char _pwd[MAXSIZE_PWD + 1];
	char _passphrase[MAXSIZE_PPHRASE + 1];
	char _publickey[MAXSIZE_PUBKEY + 1];
	char _privatekey[MAXSIZE_PRIKEY + 1];
	int _session_id;
	std::vector<const char*> _server_cap;
	char* _previous_buffer;

};
