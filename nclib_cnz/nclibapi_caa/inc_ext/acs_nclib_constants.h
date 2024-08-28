#ifndef HEADER_GUARD_FILE__acs_nclib_constants
#define HEADER_GUARD_FILE__acs_nclib_constants

/** @file acs_nclib_constants.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-10-02
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
 *	| R-001 | 2012-10-02 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */
#define NCLIB_MAX_RETRIES 	1000
#define NCLIB_RETRY_USLEEP	1000

// max length for input parameters
#define MAXSIZE_PWD			16
#define MAXSIZE_PPHRASE		128
#define MAXSIZE_PUBKEY		256
#define MAXSIZE_PRIKEY		256
#define MAXSIZE_CAPS		64
#define MSG_ID_SIZE		4

namespace acs_nclib {
	enum WarningConstants {
		WAR_NO_WARNINGS =	0
	};

	enum ErrorConstants {
		ERR_SIGNALS_ERROR = -1,
		ERR_NO_ERRORS =	0,
		ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE,
		ERR_SESSION_OPENED,
		ERR_SESSION_INVALID,
		ERR_SESSION_CLOSE_FAILED,
		ERR_SOCKET_CREATION_FAILED,
		ERR_CONNECT_FAILED,
		ERR_IP_ADDRESS_INVALID,
		ERR_SET_SOCK_OPTION_FAILED,
		ERR_LIBSSH_INIT_FAILED,
		ERR_LIBSSH_SESSION_INIT_FAILED,
		ERR_LIBSSH_SESSION_HANDSHAKE_FAILED,
		ERR_LIBSSH_SESSION_STARTUP_FAILED,
		ERR_LIBSSH_SESSION_FREE_FAILED,
		ERR_LIBSSH_USERAUTH_GET_AUTH_METHODS_LIST,
		ERR_LIBSSH_USERAUTH_AUTH_METHOD_NOT_SUPPORTED_BY_SERVER,
		ERR_LIBSSH_USERAUTH_PASSWORD_METHOD_FAILED,
		ERR_LIBSSH_USERAUTH_KBD_INTERACTIVE_METHOD_FAILED,
		ERR_LIBSSH_SESSION_DISCONNECT_FAILED,
		ERR_SSH_USERAUTH_AUTH_METHOD_NOT_SUPPORTED,
		ERR_LIBSSH_CHANNEL_FREE_FAILED,
		ERR_LIBSSH_CHANNEL_OPEN_FAILED,
		ERR_LIBSSH_CHANNEL_SUBSYSTEM_FAILED,
		ERR_LIBSSH_CHANNEL_INVALID,
		ERR_LIBSSH_CHANNEL_CLOSED_BY_REMOTE_HOST,
		ERR_LIBSSH_CHANNEL_WRITE_FAILED,
		ERR_LIBSSH_CHANNEL_READ_FAILED,
		ERR_RPC_DUMP_FAILED,
		ERR_RPC_TERM_NOTFOUND,
        ERR_TIMEOUT_RECEIVE,
        ERR_NULL_POINTER,
        ERR_BUFFER_EXCEEDED,
        ERR_HELLO_SND_FAILED,
        ERR_HELLO_RCV_FAILED,
        ERR_REPLY_RCV_FAILED,
        ERR_LOCK_FAILED,
        ERR_UNLOCK_FAILED,
        ERR_RPC_BUILD_FAILED,
        ERR_TCP_FREE_FAILED,
        ERR_TCP_CHANNEL_OPEN_FAILED,
        ERR_TCP_CHANNEL_INVALID,
        ERR_TCP_CHANNEL_CLOSED_BY_REMOTE_HOST,
        ERR_TCP_CHANNEL_WRITE_FAILED,
        ERR_TCP_CHANNEL_READ_FAILED,
	ERR_UDP_OPEN_FAILED,
	ERR_UDP_READ_FAILED,
	ERR_UDP_WRITE_FAILED
	};

	extern const char * const ERRTEXT_NO_ERRORS;
	extern const char * const ERRTEXT_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE;

	enum UserAuthMethods {
		USER_AUTH_NONE = 0x00,
		USER_AUTH_PASSWORD = 0x01,
		USER_AUTH_KBD_INTERACTIVE = 0x02,
		USER_AUTH_PUBLIC_KEY = 0x04,
		USER_AUTH_PUBLIC_KEY_FROM_FILE = 0x08
	};
}

#endif /* HEADER_GUARD_FILE__acs_nclib_constants */
