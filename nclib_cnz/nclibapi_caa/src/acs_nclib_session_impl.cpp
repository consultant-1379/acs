#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "acs_nclib_trace.h"
#include "acs_nclib_session_impl.h"

#include "acs_nclib_rpc.h"
#include "acs_nclib_rpc_reply.h"
#include "acs_nclib_builder.h"
#include <sys/time.h>
#include <fcntl.h>

#define RCV_BUFFER_SIZE 256
#define RPC_CLOSE_TAG "]]>]]>"

typedef struct timeval TIMEVAL;

ACS_NCLIB_TRACE_DEFINE(acs_nclib_session_impl);


int acs_nclib_session_impl::setPassword(const char * pwd){

	ACS_NCLIB_TRACE_FUNCTION;

	set_error_info(acs_nclib::ERR_NO_ERRORS);

	if (pwd != 0) {
		unsigned long len = strlen(pwd);

		if (len > MAXSIZE_PWD){
			set_error_info(acs_nclib::ERR_BUFFER_EXCEEDED, "Password provided is longer than %d", MAXSIZE_PWD);
			ACS_NCLIB_TRACE_MESSAGE("ERROR: setPassword failed! Password provided is longer than %d", MAXSIZE_PWD);

			return acs_nclib::ERR_SIGNALS_ERROR;
		}

		memset(_pwd, 0, MAXSIZE_PWD + 1);
		memcpy(_pwd, pwd, len);

	}
	else{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "NULL pointer to pwd");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::setPassphrase(const char * passphrase){

	ACS_NCLIB_TRACE_FUNCTION;

	set_error_info(acs_nclib::ERR_NO_ERRORS);

	if (passphrase != 0) {
		unsigned long len = strlen(passphrase);

		if (len > MAXSIZE_PPHRASE){
			set_error_info(acs_nclib::ERR_BUFFER_EXCEEDED, "Passphrase provided is longer than %d", MAXSIZE_PPHRASE);
			ACS_NCLIB_TRACE_MESSAGE("ERROR: setPassphrase failed! Passphrase provided is longer than %d", MAXSIZE_PPHRASE);

			return acs_nclib::ERR_SIGNALS_ERROR;
		}

		memset(_passphrase, 0, MAXSIZE_PPHRASE + 1);
		memcpy(_passphrase, passphrase, len);
	}
	else{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "NULL pointer to passphrase");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::setPubicKey(const char * publickey){

	ACS_NCLIB_TRACE_FUNCTION;

	set_error_info(acs_nclib::ERR_NO_ERRORS);

	if (publickey != 0) {
		unsigned long len = strlen(publickey);

		if (len > MAXSIZE_PUBKEY){
			set_error_info(acs_nclib::ERR_BUFFER_EXCEEDED, "Path to public key is longer than %d", MAXSIZE_PUBKEY);
			ACS_NCLIB_TRACE_MESSAGE("ERROR: setPublicKey failed! Path to public key is longer than %d", MAXSIZE_PUBKEY);

			return acs_nclib::ERR_SIGNALS_ERROR;
		}

		memset(_publickey, 0, MAXSIZE_PUBKEY + 1);
		memcpy(_publickey, publickey, len);
	}
	else{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "NULL pointer to publickey");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::setPrivateKey(const char * privatekey){

	ACS_NCLIB_TRACE_FUNCTION;

	set_error_info(acs_nclib::ERR_NO_ERRORS);

	if (privatekey != 0) {
		unsigned long len = strlen(privatekey);

		if (len > MAXSIZE_PRIKEY){
			set_error_info(acs_nclib::ERR_BUFFER_EXCEEDED, "Path to private key is longer than %d", MAXSIZE_PRIKEY);
			ACS_NCLIB_TRACE_MESSAGE("ERROR: setPrivateKey failed! Path to private key is longer than %d", MAXSIZE_PRIKEY);

			return acs_nclib::ERR_SIGNALS_ERROR;
		}

		memset(_privatekey, 0, MAXSIZE_PRIKEY + 1);
		memcpy(_privatekey, privatekey, len);
	}
	else{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "NULL pointer to privatekey");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	return acs_nclib::ERR_NO_ERRORS;
}


/*
 * Here the server_host_port must be in host-byte-order (human form)
 */
int acs_nclib_session_impl::open (const char * server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	in_addr in_srv_addr;

	if (!::inet_aton(server_host_addr, &in_srv_addr)) { // ERROR: server_host_addr not valid
		set_error_info(acs_nclib::ERR_IP_ADDRESS_INVALID, "The server host IP address is not valid");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: session open failed! The server host IP address %s is not valid", server_host_addr);

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return open(::ntohl(in_srv_addr.s_addr), server_host_port, auth_method, username);
}

/*
 * Here server_host_addr and server_host_port parameters must be in host-byte-order (human form)
 */
int acs_nclib_session_impl::open (uint32_t server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (connect(::htonl(server_host_addr), ::htons(server_host_port)) != acs_nclib::ERR_NO_ERRORS)
		return acs_nclib::ERR_SIGNALS_ERROR;

	if (ssh2_session_open(auth_method, username, _pwd) != acs_nclib::ERR_NO_ERRORS) {
		::shutdown(_sock, SHUT_RDWR); ::close(_sock); _sock = -1;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (send_hello() != acs_nclib::ERR_NO_ERRORS || receive_hello() != acs_nclib::ERR_NO_ERRORS)
	{
		ssh2_channel_free(); _ssh2_channel = 0;
		ssh2_session_disconnect("Failed to exchange hello messages with the server", acs_nclib::ERR_NO_ERRORS);ssh2_session_free();_ssh2_session = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: session open failed! Failed to exchange hello messages with the server");

		::shutdown(_sock, SHUT_RDWR); ::close(_sock); _sock = -1;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	ACS_NCLIB_TRACE_MESSAGE("Session %d succesfully opened!", _session_id);

	set_error_info(acs_nclib::ERR_NO_ERRORS);

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::close () {
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = acs_nclib::ERR_NO_ERRORS;
	set_error_info(acs_nclib::ERR_NO_ERRORS);

	//Close and free the ssh2 session
    if (_ssh2_session != 0) {

    	//Close and free the ssh2 channel
		if (_ssh2_channel != 0)
		{
			acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_CLOSESESSION);

			if (send(rpc) == acs_nclib::ERR_NO_ERRORS)
			{
				acs_nclib_message* answer = 0;
				if (receive(answer) == acs_nclib::ERR_NO_ERRORS && answer->get_msg_type() == acs_nclib::RPC_REPLY_MSG)
				{
					acs_nclib_rpc_reply * reply = dynamic_cast<acs_nclib_rpc_reply*>(answer);
					if (!reply || reply->get_reply_type() != acs_nclib::REPLY_OK)
					{
						set_error_info(acs_nclib::ERR_SESSION_CLOSE_FAILED, "Reply not OK when closing session");
						ACS_NCLIB_TRACE_MESSAGE("ERROR: session close failed! Reply not OK when closing session");

						call_result = acs_nclib::ERR_SIGNALS_ERROR;
					}
					else
					{
						ACS_NCLIB_TRACE_MESSAGE("Session %d succesfully closed!", _session_id);
					}

				} else
				{
					set_error_info(acs_nclib::ERR_SESSION_CLOSE_FAILED, "Receive answer to <close-session> rpc failed");
					ACS_NCLIB_TRACE_MESSAGE("ERROR: session close failed! Receive answer to <close-session> rpc failed");

					call_result = acs_nclib::ERR_SIGNALS_ERROR;
				}
				if (answer)
					// free answer message;
					acs_nclib_factory::dereference(answer);
			}
			else{
				//std::cout << "Sending <close-session> rpc failed" << std::endl;
				set_error_info(acs_nclib::ERR_SESSION_CLOSE_FAILED, "Sending <close-session> rpc failed");
				ACS_NCLIB_TRACE_MESSAGE("ERROR: session close failed! Sending <close-session> rpc failed");

				call_result = acs_nclib::ERR_SIGNALS_ERROR;
			}

			if (rpc)
				// free answer message;
				acs_nclib_factory::dereference(rpc);

			ssh2_channel_free();
			_ssh2_channel = 0;
        }

		ssh2_session_disconnect("SSH2 Session normally closed.", acs_nclib::ERR_NO_ERRORS);
		ssh2_session_free();
		_ssh2_session = 0;
	}

	//Close socket
	if (_sock != -1)
	{
		::shutdown(_sock, SHUT_RDWR);
		::close(_sock);
		_sock = -1;
	}

	for (unsigned int i = 0; i < _server_cap.size(); i++)
	{
		delete[] (_server_cap.at(i));
	}
	_server_cap.clear();

	//Delete saved buffer
	if (_previous_buffer)
	{
		delete[] _previous_buffer;
		_previous_buffer = 0;
	}


	return call_result;
}

/*
 * Here server_host_addr and server_host_port parameters must be in network-byte-order
 */
int acs_nclib_session_impl::connect (uint32_t server_host_addr, uint16_t server_host_port) {
	ACS_NCLIB_TRACE_FUNCTION;

	if (_sock != -1) { // ERROR: session already open
		set_error_info(acs_nclib::ERR_SESSION_OPENED, "This session is already opened");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: connect failed! This session is already opened");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	errno = 0;
	if ((_sock = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) { // ERROR: socket creation
		_sock = -1;
		set_error_info_errno(acs_nclib::ERR_SOCKET_CREATION_FAILED, "Socket creation failed");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: connect failed! Socket creation failed");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	sockaddr_in srv_addr;

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = server_host_port;
	srv_addr.sin_addr.s_addr = server_host_addr;

	errno = 0;

	int opts;
	opts = ::fcntl(_sock,F_GETFL);
	if (opts < 0) {
		set_error_info_errno(acs_nclib::ERR_CONNECT_FAILED, "Connecting to the NETCONF server failed");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: connect failed! ::fcntl(_sock,F_GETFL) returned %d", opts);

		::close(_sock); _sock = -1;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	int noblock_opts = (opts | O_NONBLOCK);

	if (fcntl(_sock,F_SETFL,noblock_opts) < 0) {
		set_error_info_errno(acs_nclib::ERR_CONNECT_FAILED, "Connecting to the NETCONF server failed");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: connect failed! ::fcntl(_sock,F_SETFL,noblock_opts) returned failure");
		::close(_sock); _sock = -1;
		//std::cout << "fcntl(_sock,F_SETFL,noblock_opts) failed" << std::endl;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	//Connect now works aynchronously so there's no need to check on the return code
	::connect(_sock, reinterpret_cast<struct sockaddr *>(&srv_addr), sizeof(sockaddr_in));



	if (fcntl(_sock,F_SETFL,opts) < 0) {
		set_error_info_errno(acs_nclib::ERR_CONNECT_FAILED, "Connecting to the NETCONF server failed");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: connect failed! ::fcntl(_sock,F_SETFL,opts) returned failure");
		::close(_sock); _sock = -1;
		//std::cout << "fcntl(_sock,F_SETFL,opts) failed" << std::endl;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	TIMEVAL Timeout;
	Timeout.tv_sec = 3;
	Timeout.tv_usec = 0;

	fd_set Write, Err;
	FD_ZERO(&Write);
	FD_ZERO(&Err);
	FD_SET(_sock, &Write);
	FD_SET(_sock, &Err);

	// check if the socket is ready
	::select(_sock + 1,NULL,&Write,&Err,&Timeout);

	if(FD_ISSET(_sock, &Write))
	{
		ACS_NCLIB_TRACE_MESSAGE("Socket Successfully connected!");
		return acs_nclib::ERR_NO_ERRORS;
	}
	else
	{
		//std::cout << "Connect Failed" << std::endl;
		set_error_info_errno(acs_nclib::ERR_CONNECT_FAILED, "Connecting to the NETCONF server failed");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: connect failed! Socket connection is impossible");
		::close(_sock); _sock = -1;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

#ifdef ACS_NCLIB_SET_SO_KEEPALIVE_ON_CLIENT_SOCKET
	int so_keepalive = 1;

	errno = 0;
	if (::setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, &so_keepalive, sizeof(so_keepalive)) < 0) { // ERROR: setting the SO_KEEPALIVE socket option
		set_error_info_errno(acs_nclib::ERR_SET_SOCK_OPTION_FAILED, "Setting the SO_KEEPALIVE socket option failed");
		::shutdown(_sock, SHUT_RDWR); ::close(_sock); _sock = -1;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
#endif

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::ssh2_session_open (acs_nclib::UserAuthMethods auth_method, const char * username, ...) {

	ACS_NCLIB_TRACE_FUNCTION;

  // Create a session instance and start it up.
	if (!(_ssh2_session = libssh2_session_init_ex(NULL,NULL,NULL,this))) { // ERROR: Initializing a new ssh2 session object
		set_error_info(acs_nclib::ERR_LIBSSH_SESSION_INIT_FAILED, "Underlying SSH library cannot create and initialize a new SSH session object");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 session open failed! Underlying SSH library cannot create and initialize a new SSH session object");
		_ssh2_session = 0;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	int call_result = 0;

    // set non-blocking libssh2 sessions
    libssh2_session_set_blocking(_ssh2_session, 0);

	// Start the session up
	errno = 0;

#if LIBSSH2_VERSION_NUM >= 0x010208

	unsigned int tries = 0;
	while ((call_result = libssh2_session_handshake(_ssh2_session, _sock)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if(call_result < 0) {
		// ERROR: Handshaking the SSH session up
		set_error_info_errno(
			acs_nclib::ERR_LIBSSH_SESSION_HANDSHAKE_FAILED,
			"Underlying SSH library cannot handshake the session with the NETCONF server: call result == %d, SSH last errno == %d",
			call_result,
			libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 session open failed! Underlying SSH library cannot handshake the session with the NETCONF server: call result == %d, SSH last errno == %d",
			call_result,
			libssh2_session_last_errno(_ssh2_session));

		ssh2_session_free();
		_ssh2_session = 0;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}
#else

#warning "libssh2_session_startup is deprecated for latest version of the LIBSSH2 library"

	unsigned int tries = 0;
	while ((call_result = libssh2_session_startup(_ssh2_session, _sock)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if(call_result  < 0) {
		// ERROR: Starting the SSH session up
		set_error_info_errno(
			acs_nclib::ERR_LIBSSH_SESSION_STARTUP_FAILED,
			"Underlying SSH library cannot start the session up with the NETCONF server: call result == %d, SSH last errno == %d",
			call_result,
			libssh2_session_last_errno(_ssh2_session));

		ssh2_session_free();
		_ssh2_session = 0;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}
#endif

	// Retrieve the authentication list from the server
	if ((call_result = ssh2_get_userauth_list(username)) != acs_nclib::ERR_NO_ERRORS) {
		// ERROR: getting the authentication list
		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 session open failed! Failure on retrieving the authentication method list");
		ssh2_session_disconnect("Failure on retrieving the authentication method list");
		ssh2_session_free();
		_ssh2_session = 0;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	// NONE authentication method will not be allowed by NCLIB library
	if (auth_method == acs_nclib::USER_AUTH_NONE) {
		set_error_info(acs_nclib::ERR_SSH_USERAUTH_AUTH_METHOD_NOT_SUPPORTED,
			"Authentication method NONE is not allowed by NCLIB library.");

		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 session open failed! Authentication method NONE is not allowed by NCLIB library.");
		ssh2_session_disconnect("NONE authentication method requested but not allowed.");
		ssh2_session_free();
		_ssh2_session = 0;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (!(auth_method & _user_auth_mask)) { // ERROR: Authentication method not supported by the server
		set_error_info(acs_nclib::ERR_LIBSSH_USERAUTH_AUTH_METHOD_NOT_SUPPORTED_BY_SERVER,
			"Authentication method not supported by the remote server: user auth requested == %d: "
			"SSH server authentication list == '%s'", auth_method, _user_auth_list);

		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 session open failed! Authentication method not supported by the remote server: user auth requested == %d: "
			"SSH server authentication list == '%s'", auth_method, _user_auth_list);

		ssh2_session_disconnect("The requested authentication method not supported on the SSH server");
		ssh2_session_free();
		_ssh2_session = 0;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	// Try to authenticate using the auth_method in input
	va_list arg;

	switch (auth_method) {
	case acs_nclib::USER_AUTH_PASSWORD:
		va_start (arg, username);
		call_result = ssh2_authenticate_password(username, va_arg(arg, const char *));
		va_end (arg);
		break;
	case acs_nclib::USER_AUTH_KBD_INTERACTIVE:
		call_result = ssh2_authenticate_kbd_interactive(username);
		break;
	case acs_nclib::USER_AUTH_PUBLIC_KEY:
		call_result = ssh2_authenticate_publickey();
		break;
	case acs_nclib::USER_AUTH_PUBLIC_KEY_FROM_FILE:
		call_result = ssh2_authenticate_publickey_fromfile(username);
		break;
	default: // Authentication method unknown and not supported by this library
		call_result = acs_nclib::ERR_SIGNALS_ERROR;
		set_error_info(acs_nclib::ERR_SSH_USERAUTH_AUTH_METHOD_NOT_SUPPORTED,
			"The requested authentication method is unknown or not supported by this library: auth_method == %d", auth_method);
		break;
	}

	if (call_result != acs_nclib::ERR_NO_ERRORS) { // ERROR: while authentication process
		ssh2_session_disconnect("SSH server authentication failed");
		ssh2_session_free();
		_ssh2_session = 0;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	if ((call_result = ssh2_channel_open()) != acs_nclib::ERR_NO_ERRORS)
	{
		ssh2_session_disconnect("SSH channel open failed");
		ssh2_session_free();
		_ssh2_session = 0;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	//If we get through here than netconf channel is up and working (apparently)
	return acs_nclib::ERR_NO_ERRORS;

}

int acs_nclib_session_impl::ssh2_channel_open()
{

	if (_ssh2_session == 0)
	{
		//Something bad happened
		set_error_info(acs_nclib::ERR_SESSION_INVALID,
			"Trying to open a channel over an invalid session");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 channel open failed! Trying to open a channel over an invalid session");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (_ssh2_channel)
	{
		//A channel is already up. Close it and open a new one
		 ssh2_channel_free();
		 _ssh2_channel = 0;
	}

	unsigned int tries = 0;
	int call_result = 0;

	//Open a brand new channel
	while((_ssh2_channel = libssh2_channel_open_session(_ssh2_session)) == 0 && tries < NCLIB_MAX_RETRIES)
	{
		call_result = libssh2_session_last_errno(_ssh2_session);
		if (call_result != LIBSSH2_ERROR_EAGAIN )
		{
			set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_OPEN_FAILED, "Underlying SSH Library failed to open new channel. call_result == %d, SSH last errno == %d",
						call_result, libssh2_session_last_errno(_ssh2_session));

			ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 channel open failed! Underlying SSH Library failed to open new channel. call_result == %d, SSH last errno == %d",
						call_result, libssh2_session_last_errno(_ssh2_session));

			return acs_nclib::ERR_SIGNALS_ERROR;
		}

		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}


	if (_ssh2_channel == 0)
	{
		set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_OPEN_FAILED, "Underlying SSH Library failed to open new channel. call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 channel open failed! Underlying SSH Library failed to open new channel. call_result == %d, SSH last errno == %d",
								call_result, libssh2_session_last_errno(_ssh2_session));

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	tries = 0;
	while((call_result = libssh2_channel_subsystem(_ssh2_channel, "netconf")) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}


	if (call_result != LIBSSH2_ERROR_NONE  || libssh2_session_last_errno(_ssh2_session) == LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED)
	{
		//Failed to start subsystem netconf on channel
		set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_OPEN_FAILED, "Underlying SSH Library failed to execute netconf subsystem. call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 channel open failed! Underlying SSH Library failed to execute netconf subsystem. call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		ssh2_channel_free();
		_ssh2_channel = 0;
		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	return acs_nclib::ERR_NO_ERRORS;

}



int acs_nclib_session_impl::ssh2_session_free (int provide_error_info) {
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = 0;
	unsigned int tries = 0;

	while ((call_result = libssh2_session_free(_ssh2_session)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if (call_result != LIBSSH2_ERROR_NONE) {

		if (provide_error_info) set_error_info_errno(acs_nclib::ERR_LIBSSH_SESSION_FREE_FAILED,
				"SSH2 library failed to free session object resources: memory leaks are possible: call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE(
				"ERROR: Call 'libssh2_session_free' failed: SSH2 library cannot free session object resources: "
				"memory leaks are possible: call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	return acs_nclib::ERR_NO_ERRORS;
}


int acs_nclib_session_impl::ssh2_channel_free(int provide_error_info) {
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = 0;
	unsigned int tries = 0;


	errno = 0;
	while ((call_result = libssh2_channel_free(_ssh2_channel)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if (call_result != LIBSSH2_ERROR_NONE)
	{
		if (provide_error_info) set_error_info_errno(acs_nclib::ERR_LIBSSH_CHANNEL_FREE_FAILED,
				"SSH2 library failed to free channel object resources: memory leaks are possible: call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE(
				"ERROR: Call 'libssh2_channel_free' failed: SSH2 library cannot free session object resources: "
				"memory leaks are possible: call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}



int acs_nclib_session_impl::ssh2_get_userauth_list (const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	_user_auth_list = 0;
	_user_auth_mask = acs_nclib::USER_AUTH_NONE;

	errno = 0;
	unsigned int tries = 0;

	while ((_user_auth_list = libssh2_userauth_list(_ssh2_session, username, ::strlen(username)) ) == 0 &&
			libssh2_session_last_errno(_ssh2_session) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES){
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if (!_user_auth_list && !libssh2_userauth_authenticated(_ssh2_session)) { // ERROR: error on getting the user authentication methods list
		set_error_info_errno(acs_nclib::ERR_LIBSSH_USERAUTH_GET_AUTH_METHODS_LIST,
				"Underlying SSH library cannot retrieve the user authentication methods list from the server: SSH last errno == %d",
				libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE("ERROR: ssh2 get authentication methods failed! Underlying SSH library cannot retrieve the user authentication methods list from the server: SSH last errno == %d",
				libssh2_session_last_errno(_ssh2_session));

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	// Build the user auth mask
	if (_user_auth_list) {
		_user_auth_mask =
				(::strcasestr(_user_auth_list, "password") ? acs_nclib::USER_AUTH_PASSWORD : 0) |
				(::strcasestr(_user_auth_list, "keyboard-interactive") ? acs_nclib::USER_AUTH_KBD_INTERACTIVE : 0) |
				(::strcasestr(_user_auth_list, "publickey") ? (acs_nclib::USER_AUTH_PUBLIC_KEY | acs_nclib::USER_AUTH_PUBLIC_KEY_FROM_FILE) : 0);
	}

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::ssh2_session_disconnect (const char * description, int provide_error_info) {
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = 0;
	unsigned int tries = 0;

	while ((call_result = libssh2_session_disconnect(_ssh2_session, description)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if (call_result != LIBSSH2_ERROR_NONE) {

		if (provide_error_info) set_error_info_errno(acs_nclib::ERR_LIBSSH_SESSION_DISCONNECT_FAILED,
				"The library failed to disconnect from the SSH server: call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE(
				"ERROR: Call 'libssh2_session_disconnect' failed: the library cannot disconnect from the SSH server: "
				"call_result == %d, SSH last errno == %d",
				call_result, libssh2_session_last_errno(_ssh2_session));

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::ssh2_authenticate_password (const char * username, const char * password) {
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = 0;
	unsigned int tries = 0;

	while ((call_result = libssh2_userauth_password(_ssh2_session, username, password)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if (call_result != LIBSSH2_ERROR_NONE) {

		set_error_info_errno(acs_nclib::ERR_LIBSSH_USERAUTH_PASSWORD_METHOD_FAILED,
				"SSH server authentication failed using the PASSWORD method: call_result == %d, SSH last errno == %d", call_result,
				libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE("ERROR: authentication failed! SSH server authentication failed using the PASSWORD method: call_result == %d, SSH last errno == %d", call_result,
				libssh2_session_last_errno(_ssh2_session));

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	ACS_NCLIB_TRACE_MESSAGE("Authentication by password succeeded.");

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::ssh2_authenticate_kbd_interactive (const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;


	int call_result = 0;
	unsigned int tries = 0;

	while ((call_result = libssh2_userauth_keyboard_interactive(_ssh2_session, username, &ssh2_userauth_kbdint_response)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if (call_result != LIBSSH2_ERROR_NONE) {
		set_error_info_errno(acs_nclib::ERR_LIBSSH_USERAUTH_KBD_INTERACTIVE_METHOD_FAILED,
				"SSH server authentication failed using the KEYBOARD-INTERACTIVE method: call_result == %d, SSH last errno == %d", call_result,
				libssh2_session_last_errno(_ssh2_session));
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	ACS_NCLIB_TRACE_MESSAGE("Authentication by keyboard-interactive succeeded.");

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::ssh2_authenticate_publickey () {
	ACS_NCLIB_TRACE_FUNCTION;
/*
 * Interactive authentication methods are skipped.
 *
 */

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::ssh2_authenticate_publickey_fromfile (const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = 0;
	unsigned int tries = 0;

	while ((call_result = libssh2_userauth_publickey_fromfile(_ssh2_session, username, _publickey, _privatekey, _passphrase)) == LIBSSH2_ERROR_EAGAIN && tries < NCLIB_MAX_RETRIES)
	{
		usleep(NCLIB_RETRY_USLEEP);
		tries++;
	}

	if (call_result != LIBSSH2_ERROR_NONE) {
		set_error_info_errno(acs_nclib::ERR_LIBSSH_USERAUTH_KBD_INTERACTIVE_METHOD_FAILED,
				"SSH server authentication failed using the public key method: call_result == %d, SSH last errno == %d",
				call_result,
				libssh2_session_last_errno(_ssh2_session));

		ACS_NCLIB_TRACE_MESSAGE("ERROR: authentication failed! SSH server authentication failed using the public key method: call_result == %d, SSH last errno == %d",
				call_result,
				libssh2_session_last_errno(_ssh2_session));

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	ACS_NCLIB_TRACE_MESSAGE("Authentication by public key succeeded.");

	return acs_nclib::ERR_NO_ERRORS;
}



int acs_nclib_session_impl::send(acs_nclib_message* msg)
{

	if (msg == 0)
	{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "Sending a NULL rpc message");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Trying to send a NULL rpc message");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (_ssh2_channel == 0)
	{
		set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_INVALID, "Sending an rpc message over an invalid channel");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Trying to send an rpc message over an invalid channel");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (libssh2_channel_eof(_ssh2_channel))
	{
		set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_CLOSED_BY_REMOTE_HOST, "Sending an rpc message over a channel closed by remote host");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Trying to send an rpc message over a channel closed by remote host");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	const char * tmp = 0;
	char* buffer = 0;

	//Assign a new message_id to the message
	msg->set_message_id(++_msg_id_count);

	ACS_NCLIB_TRACE_MESSAGE("Sending message wiht ID: %d over Netconf session with ID: %d", msg->get_message_id(), _session_id);

	tmp = msg->dump();
	ssize_t dump_len = 0;
	ssize_t buf_len = 0;

	if (tmp)
	{
		dump_len = strlen(tmp);
		buf_len = dump_len + strlen(RPC_CLOSE_TAG);
		buffer = new char[buf_len + 1];

		memset(buffer,0,buf_len + 1);
		memcpy(buffer,tmp,dump_len);
		delete[] tmp;
	}
	else
	{
		set_error_info(acs_nclib::ERR_RPC_DUMP_FAILED, "Failure on dumping an rpc message");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Failure on dumping the rpc message %d", msg->get_message_id());

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	strcat(buffer,RPC_CLOSE_TAG);
	//buf_len = strlen(buffer);

	int i;
	ssize_t wr = 0;

	do {
		i = libssh2_channel_write(_ssh2_channel, buffer, buf_len);

		if (i < 0) {
			set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_WRITE_FAILED, "Failure on writing a buffer onto the channel");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Underlying SSH Library failed to write buffer over ssh channel");

			if (buffer)
				delete[] buffer;

			return acs_nclib::ERR_SIGNALS_ERROR;
		}
		wr += i;
	} while (i > 0 && wr < buf_len);

	if (buffer)
		delete[] buffer;
	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::receive(acs_nclib_message* &msg,int timeout)
{
	msg = 0;
	timeval t1, t2;
	double elapsedTime;

	if (_ssh2_channel == 0)
	{
		set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_INVALID, "Receiving a message over an invalid channel");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Trying to receive a message over an invalid channel");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (libssh2_channel_eof(_ssh2_channel) == 1)
	{
		set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_CLOSED_BY_REMOTE_HOST, "Receiving a message over a channel closed by remote host");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Trying to receive a message over a channel closed by remote host");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	ssize_t len;
	size_t rd = 0;
	char  *specialsequence = 0;

	char * buffer = 0;
	char * tmp = 0;

	if (_previous_buffer)
	{
		buffer = _previous_buffer;
		_previous_buffer = 0;
	}

	unsigned int tries = 0;

	gettimeofday(&t1, 0);

	while (!specialsequence) {

		rd = 0;
		size_t tmp_len = RCV_BUFFER_SIZE - 1;

		if (buffer)
		{
			char* new_buffer = new char[strlen(buffer) + tmp_len + 1];
			memset(new_buffer,0, strlen(buffer) + tmp_len + 1);
			memcpy(new_buffer,buffer,strlen(buffer));

			tmp = new_buffer + strlen(buffer);

			delete[] buffer;
			buffer = new_buffer;
		}
		else
		{
			buffer = new char[tmp_len + 1];
			memset(buffer,0,tmp_len +1);
			tmp = buffer;
		}

		do {

			len = libssh2_channel_read(_ssh2_channel, tmp + rd, tmp_len - rd);

			if (LIBSSH2_ERROR_EAGAIN == len || len == 0)
			{

				if (libssh2_channel_eof(_ssh2_channel) == 1)
				{
					set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_CLOSED_BY_REMOTE_HOST, "Receiving a message over a channel closed by remote host");
					ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Trying to receive a message over a channel closed by remote host");

					if (buffer)
						delete[] buffer;

					return acs_nclib::ERR_SIGNALS_ERROR;
				}

				usleep(NCLIB_RETRY_USLEEP);
				tries++;
			}
			else if (len < 0) {
				set_error_info(acs_nclib::ERR_LIBSSH_CHANNEL_READ_FAILED, "Failure on reading a buffer from the channel");
				ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Underlying SSH Library failed to read buffer from the channel");

				if (buffer)
					delete[] buffer;

				return acs_nclib::ERR_SIGNALS_ERROR;
			}
			else
			{
				rd += len;
				/* read more data until we see a rpc-reply closing tag followed by
				 * the special sequence ]]>]]> */

				if ((tmp - buffer) >  (int) tmp_len)
				{
					specialsequence = strstr(tmp - tmp_len, RPC_CLOSE_TAG);
				}
				else
				{
					specialsequence = strstr(buffer, RPC_CLOSE_TAG);
				}
			}

			gettimeofday(&t2, 0);

			elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
			elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

			if(elapsedTime > timeout)
			{
				if (buffer)
				{
					delete[] buffer;
				}
				set_error_info(acs_nclib::ERR_TIMEOUT_RECEIVE, "Timeout occurred while receiving");
				ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Timeout occurred while receiving");

				return acs_nclib::ERR_SIGNALS_ERROR;
			}

		} while (rd < tmp_len && !specialsequence);

	}

	if (!specialsequence) {
		set_error_info(acs_nclib::ERR_RPC_TERM_NOTFOUND, "RPC message terminator not found");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! RPC message terminator not found");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	/* discard the special sequence so that only XML is returned */
	rd = specialsequence - buffer;

	//Save previous buffer for next receive
	char* saved_buffer = specialsequence + strlen(RPC_CLOSE_TAG) + 1;
	size_t prev_buff_len = strlen(saved_buffer);
	if (prev_buff_len > 0)
	{
		_previous_buffer = new char[prev_buff_len + 1];
		memset(_previous_buffer, 0, prev_buff_len + 1);
		memcpy(_previous_buffer, saved_buffer, prev_buff_len);
	}

	buffer[rd] = 0;

	msg = acs_nclib_builder::build_message(buffer);

	delete[] buffer;

	if (msg == 0)
	{
		set_error_info(acs_nclib::ERR_RPC_BUILD_FAILED, "Failed to build rpc message from buffer content");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Failed to build rpc message from buffer content");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	return acs_nclib::ERR_NO_ERRORS;

}

int acs_nclib_session_impl::send_hello()
{
	acs_nclib_hello* hello = 0;
	hello = acs_nclib_factory::create_hello();

	if (hello)
	{
		hello->add_capability("urn:ietf:params:netconf:base:1.0");
		if (send(hello) != acs_nclib::ERR_NO_ERRORS)
		{
			set_error_info(acs_nclib::ERR_HELLO_SND_FAILED, "Failed to send hello message");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to send hello message");

			acs_nclib_factory::dereference(hello);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
		acs_nclib_factory::dereference(hello);
	}
	else
	{
		set_error_info(acs_nclib::ERR_HELLO_SND_FAILED, "Failed to create hello message");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to create hello message");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}


int acs_nclib_session_impl::receive_hello()
{

	acs_nclib_message* msg = 0;
	acs_nclib_hello* hello = 0;
	std::vector<std::string> caps;


	if (receive(msg) == acs_nclib::ERR_NO_ERRORS)
	{
		hello = dynamic_cast<acs_nclib_hello*>(msg);

		if (hello)
		{
			_session_id = hello->get_session_id();
			_server_cap.clear();

			hello->get_capabilities(caps);

			for (unsigned int i = 0; i < caps.size(); i++)
			{
				std::string cap = caps.at(i);
				char* tmp = new char[cap.size() + 1];
				memset(tmp,0,cap.size() + 1);
				memcpy(tmp,cap.c_str(),cap.size());

				_server_cap.push_back(tmp);
			}
		}
		else
		{
			set_error_info(acs_nclib::ERR_HELLO_RCV_FAILED, "Failed to receive hello message from server");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to receive hello message from server");

			if (msg)
				acs_nclib_factory::dereference(msg);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
	}
	else
	{
		set_error_info(acs_nclib::ERR_HELLO_RCV_FAILED, "Failed to receive hello message from server");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to receive hello message from server");

		if (msg)
			acs_nclib_factory::dereference(msg);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (msg)
		acs_nclib_factory::dereference(msg);

	return acs_nclib::ERR_NO_ERRORS;
}


int acs_nclib_session_impl::getId(int& ret)
{
	if (_ssh2_session && _ssh2_channel && _session_id >= 0)
	{
		ret = _session_id;
	}
	else
	{
		set_error_info(acs_nclib::ERR_SESSION_INVALID, "Trying to retrieve Session Identifier for an invalid session");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Trying to retrieve Session Identifier for an invalid session");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::getServerCapabilities(std::vector<const char*>& ret)
{
	if (_ssh2_session && _ssh2_channel && _session_id >= 0)
	{
		ret = _server_cap;
	}
	else
	{
		set_error_info(acs_nclib::ERR_SESSION_INVALID, "Trying to retrieve Capabilities for an invalid session");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Trying to retrieve Capabilities for an invalid session");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::send_sync(acs_nclib_rpc* request, acs_nclib_rpc_reply* &reply, int timeout)
{
	reply = 0;
	unsigned int retry = 0;
	bool reply_rcvd = false;

	if (send(request) == acs_nclib::ERR_NO_ERRORS)
	{
		acs_nclib_message* answer = 0;

		while (!reply_rcvd)
		{
			if (receive(answer, timeout) == acs_nclib::ERR_NO_ERRORS)
			{

				if (answer && answer->get_msg_type() == acs_nclib::RPC_REPLY_MSG && answer->get_message_id() == request->get_message_id())
				{
					reply = dynamic_cast<acs_nclib_rpc_reply*>(answer);

					if (reply == 0)
					{
						set_error_info(acs_nclib::ERR_REPLY_RCV_FAILED, "Failed to cast answer message as an rpc reply");
						ACS_NCLIB_TRACE_MESSAGE("ERROR: send_sync() failed! Unable to read rpc message received");

						return acs_nclib::ERR_SIGNALS_ERROR;
					}
					else
					{
						ACS_NCLIB_TRACE_MESSAGE("Expected rpc-reply received, send_sync() successfully completed");
						reply_rcvd = true;
					}
				}
				else
				{

					if (answer) {
						acs_nclib_factory::dereference(answer);
						answer = 0;
					}
					retry++;
					ACS_NCLIB_TRACE_MESSAGE("WARNING: Unexpected message received and discarded in send_sync() operation. "
							"Trying again to receive expected rpc-reply, attempt %d", retry);
				}
			}
			else
			{
				if (answer)
				{
					acs_nclib_factory::dereference(answer);
					answer = 0;
				}

				if (last_error_code() == acs_nclib::ERR_RPC_BUILD_FAILED)
				{
					retry++;
					ACS_NCLIB_TRACE_MESSAGE("WARNING: Unrecognized data received and discarded in send_sync() operation. "
							"Trying again to receive expected rpc-reply, attempt %d", retry);
				}
				else
				{
					return acs_nclib::ERR_SIGNALS_ERROR;
				}
			}
		}

		if (!reply_rcvd)
		{
			if (answer)
				acs_nclib_factory::dereference(answer);
			set_error_info(acs_nclib::ERR_REPLY_RCV_FAILED, "Receive of rpc reply message failed");
			return acs_nclib::ERR_SIGNALS_ERROR;
		}

	} else
	{
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;

}

int acs_nclib_session_impl::lock(acs_nclib::Datastore ds)
{
	acs_nclib_rpc* lock_rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_LOCK, ds);
	acs_nclib_rpc_reply* reply = 0;

	if (send_sync(lock_rpc, reply) != acs_nclib::ERR_NO_ERRORS || !reply)
	{
		acs_nclib_factory::dereference(lock_rpc);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	else
	{
		if (reply->get_reply_type() != acs_nclib::REPLY_OK)
		{
			if (reply->get_reply_type() == acs_nclib::REPLY_ERROR || reply->get_reply_type() == acs_nclib::REPLY_DATA_WITH_ERRORS)
			{
				std::vector<acs_nclib_rpc_error*> errors;

				reply->get_error_list(errors);

				if (errors.size() > 0)
				{
					char err_msg[512] = {0};
					sprintf(err_msg, "LOCK Operation Failed, error type %d: %s. Error Info: %s",
							errors.at(0)->get_error_type(), errors.at(0)->get_error_tag(), errors.at(0)->get_error_info());

					set_error_info(acs_nclib::ERR_LOCK_FAILED, err_msg);
					ACS_NCLIB_TRACE_MESSAGE("ERROR: %s", err_msg);

					acs_nclib_factory::dereference(errors);
				}

			}
			acs_nclib_factory::dereference(lock_rpc);
			acs_nclib_factory::dereference(reply);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
	}

	acs_nclib_factory::dereference(lock_rpc);
	acs_nclib_factory::dereference(reply);
	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_session_impl::unlock(acs_nclib::Datastore ds)
{
	acs_nclib_rpc* unlock_rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_UNLOCK, ds);
	acs_nclib_rpc_reply* reply = 0;

	if (send_sync(unlock_rpc, reply) != acs_nclib::ERR_NO_ERRORS || !reply)
	{
		acs_nclib_factory::dereference(unlock_rpc);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	else
	{
		if (reply->get_reply_type() != acs_nclib::REPLY_OK)
		{
			if (reply->get_reply_type() == acs_nclib::REPLY_ERROR || reply->get_reply_type() == acs_nclib::REPLY_DATA_WITH_ERRORS)
			{
				std::vector<acs_nclib_rpc_error*> errors;

				reply->get_error_list(errors);

				if (errors.size() > 0)
				{
					char err_msg[512] = {0};
					sprintf(err_msg, "UNLOCK Operation Failed, error type %d: %s. Error Info: %s",
							errors.at(0)->get_error_type(), errors.at(0)->get_error_tag(), errors.at(0)->get_error_info());

					set_error_info(acs_nclib::ERR_UNLOCK_FAILED, err_msg);
					ACS_NCLIB_TRACE_MESSAGE("ERROR: %s", err_msg);

					acs_nclib_factory::dereference(errors);
				}

			}
			acs_nclib_factory::dereference(unlock_rpc);
			acs_nclib_factory::dereference(reply);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
	}

	acs_nclib_factory::dereference(unlock_rpc);
	acs_nclib_factory::dereference(reply);
	return acs_nclib::ERR_NO_ERRORS;
}


// BEGIN: SSH2 library callback support

LIBSSH2_PASSWD_CHANGEREQ_FUNC(acs_nclib_session_impl::ssh2_password_change_request) {
	ACS_NCLIB_TRACE_FUNCTION;

	static_cast<void>(session);
	static_cast<void>(newpw);
	static_cast<void>(newpw_len);
	static_cast<void>(abstract);
}

LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC(acs_nclib_session_impl::ssh2_userauth_kbdint_response) {
	ACS_NCLIB_TRACE_FUNCTION;

	static_cast<void>(name);
	static_cast<void>(name_len);
	static_cast<void>(instruction);
	static_cast<void>(instruction_len);
	static_cast<void>(num_prompts);
	static_cast<void>(prompts);
	static_cast<void>(responses);
	//static_cast<void>(abstract);

	if (abstract)
	{
		acs_nclib_session_impl* session = static_cast<acs_nclib_session_impl*>(*abstract);

		//GET THE PASSWORD CHALLENGE

		if (session)
		{
			if (num_prompts == 1) {
				responses[0].text = ::strdup(session->_pwd);
				responses[0].length = ::strlen(session->_pwd);
			}
		}
	}

}

/*
 * END: SSH2 library callback support
 */



#if 0

/*
 * Here the server_host_port parameter must be in network-byte-order
 */
int __CLASS_NAME__::connect (const char * server_host_addr, uint16_t server_host_port) {
	ACS_NCLIB_TRACE_FUNCTION;

	in_addr in_srv_addr;

	if (!::inet_aton(server_host_addr, &in_srv_addr)) { // ERROR: server_host_addr not valid
		set_error_info(acs_nclib::ERR_IP_ADDRESS_INVALID, "The server host IP address is not valid");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return __CLASS_NAME__::connect(in_srv_addr.s_addr, server_host_port);
}
#endif

