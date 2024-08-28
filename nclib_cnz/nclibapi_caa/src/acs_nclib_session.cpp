#include <new>

#include "acs_nclib_constants.h"
#include "acs_nclib_trace.h"
#include "acs_nclib_session_impl.h"
#include "acs_nclib_tcp_session_impl.h"
#include "acs_nclib_udp_session_impl.h"
#include "acs_nclib_session.h"

#include <iostream>

ACS_NCLIB_TRACE_DEFINE(acs_nclib_session);
acs_nclib_session::acs_nclib_session (): _protocol(acs_nclib::SSH), _session_i(0), _tcp_session_i(0), _udp_session_i(0)
{
	_session_i = new (std::nothrow) acs_nclib_session_impl();

	if (!_session_i ) {
		// ERROR: allocating memory for internal session implementation class. Out-of-memory condition.
		ACS_NCLIB_TRACE_MESSAGE("ERROR: new operator failed: cannot allocate memory for my internal implementation object: all functionalities are missed");
	}
}



acs_nclib_session::acs_nclib_session (acs_nclib::SessionLayer prot) : _protocol(prot), _session_i(0), _tcp_session_i(0), _udp_session_i(0) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		_session_i = new (std::nothrow) acs_nclib_session_impl();
	}
	else if (_protocol == acs_nclib::TCP)
	{
		_tcp_session_i = new (std::nothrow) acs_nclib_tcp_session_impl();
	}
	else if (_protocol == acs_nclib::UDP)
	{
		_udp_session_i = new (std::nothrow) acs_nclib_udp_session_impl();
	}

	if (!_session_i && !_tcp_session_i && !_udp_session_i) {
		// ERROR: allocating memory for internal session implementation class. Out-of-memory condition.
		ACS_NCLIB_TRACE_MESSAGE("ERROR: new operator failed: cannot allocate memory for my internal implementation object: all functionalities are missed");
	}
}

acs_nclib_session::~acs_nclib_session () {
	ACS_NCLIB_TRACE_FUNCTION;

	acs_nclib_session::close();

	if (_session_i)
	{
		delete _session_i;
		_session_i = 0;
	}

	if (_tcp_session_i)
	{
		delete _tcp_session_i;
		_tcp_session_i = 0;
	}
	
	if (_udp_session_i)
	{
		delete _udp_session_i;
		_udp_session_i = 0;
	}
}


int acs_nclib_session::setPassword(const char * pwd) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, setPassword, pwd);

	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, setPassword, pwd);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::setPassphrase(const char * passphrase) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, setPassphrase, passphrase);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, setPassphrase, passphrase);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::setPubicKey(const char * publickey) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, setPubicKey, publickey);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, setPubicKey, publickey);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::setPrivateKey(const char * privatekey) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, setPrivateKey, privatekey);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, setPrivateKey, privatekey);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}


int acs_nclib_session::open (uint32_t server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, open, server_host_addr, server_host_port, auth_method, username);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, open, server_host_addr, server_host_port, auth_method, username);
	}
	else if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, open, server_host_addr, server_host_port, auth_method, username);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::open (const char * server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, open, server_host_addr, server_host_port, auth_method, username);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, open, server_host_addr, server_host_port, auth_method, username);
	}
	else if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, open, server_host_addr, server_host_port, auth_method, username);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}



int  acs_nclib_session::send(acs_nclib_message* msg)
{
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, send, msg);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, send, msg);
	}
	else if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, send, msg);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;

}
int acs_nclib_session::receive(acs_nclib_message*& msg,int timeout)
{
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, receive, msg,timeout);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, receive, msg,timeout);
	}
	else if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, receive, msg,timeout);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;

}

int acs_nclib_session::send_sync(acs_nclib_rpc* request, acs_nclib_rpc_reply* &reply, int timeout)
{
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, send_sync, request, reply, timeout);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, send_sync, request, reply, timeout);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::send_sync(acs_nclib_udp* request, acs_nclib_udp_reply* &reply,int timeout)
{
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, send_sync, request, reply, timeout);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::lock(acs_nclib::Datastore ds) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, lock, ds);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, lock, ds);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::unlock(acs_nclib::Datastore ds) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, unlock, ds);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, unlock, ds);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::close () {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, close);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, close);
	}
	else if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, close);
	}
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}

int acs_nclib_session::getId(int &ret)
{
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, getId, ret);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, getId, ret);
	}
/*	else if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, getId, ret);
	} */
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;

}

int acs_nclib_session::getServerCapabilities(std::vector<const char*>& ret)
{
	ACS_NCLIB_TRACE_FUNCTION;

	if (_protocol == acs_nclib::SSH)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_session_i, getServerCapabilities, ret);
	}
	else if (_protocol == acs_nclib::TCP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_tcp_session_i, getServerCapabilities, ret);
	}
/*	else if (_protocol == acs_nclib::UDP)
	{
		return ACS_NCLIB_TRY_CALL_IMPLEMENTATION(_udp_session_i, getServerCapabilities, ret);
	} */
	else
		return acs_nclib::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;
}


ACS_NCLIB_ERROR_INFO_DEFINE_ACCESSORS(_session_i, _tcp_session_i, _udp_session_i);
//ACS_NCLIB_ERROR_INFO_DEFINE_ACCESSORS(_tcp_session_i);
