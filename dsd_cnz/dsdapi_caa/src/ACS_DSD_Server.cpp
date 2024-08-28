#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>

#include <new>

#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Guard_T.h"

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_MessageSender.h"
#include "ACS_DSD_MessageReceiver.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_SocketAcceptor.h"
#include "ACS_DSD_LSocketAcceptor.h"
#include "ACS_DSD_LSocketConnector.h"
#include "ACS_DSD_LSocketStream.h"
#include "ACS_DSD_NumberSequencer.h"
#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_Server.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

namespace {
	ACS_DSD_NumberSequencer inet_port_sequencer(acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_SELECTOR_MIN, acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_SELECTOR_MAX);
}

extern const char * const _repeated_messages[];

__CLASS_NAME__::__CLASS_NAME__ (acs_dsd::ServiceModeConstants service_mode)
: ACS_DSD_Communicator(), _state(acs_dsd::SERVER_STATE_CLOSED), _service_mode(service_mode), _acceptors(),
  _epoll_fd(acs_dsd::INVALID_HANDLE), _app_service_name(), _app_domain_name(), _accept_sync_object_ptr(new (std::nothrow) ACE_Recursive_Thread_Mutex()) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
}

__CLASS_NAME__::~__CLASS_NAME__ () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	close();
	::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;
	delete_acceptors();

	delete _accept_sync_object_ptr;
}

int __CLASS_NAME__::open (acs_dsd::ServiceModeConstants service_mode, const char * service_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check service mode
	if (acs_dsd::SERVICE_MODE_UNKNOWN == service_mode) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVICE_MODE_UNKNOWN == service_mode): no service mode specified");
		return set_error_info(acs_dsd::ERR_SERVICE_MODE_UNKNOWN);
	}

	//Check if not closed. A server should be closed before open again.
	if (state() ^ acs_dsd::SERVER_STATE_CLOSED) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(state() ^ acs_dsd::SERVER_STATE_CLOSED): server not closed: A server should be closed before open again");
		return set_error_info(acs_dsd::ERR_SERVER_NOT_CLOSED);
	}

	return (acs_dsd::SERVICE_MODE_UNIX_SOCKET ^ service_mode) && (acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE ^ service_mode)
		? open_inet(service_mode, service_id)	//open_inet(service_mode, service_id, reuse_internals)
		: open_unix(service_mode, service_id);	//open_unix(service_mode, service_id, reuse_internals);
}

int __CLASS_NAME__::publish (const char * service_name, const char * service_domain, acs_dsd::ServiceVisibilityConstants visibility) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check parameters
	if (!service_name || !service_domain || !*service_name || !*service_domain) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(!service_name || !service_domain || !*service_name || !*service_domain): bad parameters provided");
		return set_error_info(acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL, 0, "Parameters 'service_name', 'service_domain'");
	}

	if (strlen(service_name) > acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(strlen(service_name) > acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX): Parameter service_name is longer than %d characters",
				acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX - 1);
		return set_error_info(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0,
				"Parameter service_name is longer than %d characters", acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX - 1);
	}

	if (strlen(service_domain) > acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(strlen(service_domain) > acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX): Parameter service_domain is longer than %d characters",
				acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX - 1);
		return set_error_info(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0,
				"Parameter service_domain is longer than %d characters", acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX - 1);
	}

	//Check if opened. A server should be opened before can publish itself.
	if (acs_dsd::SERVER_STATE_CLOSED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVER_STATE_CLOSED == state()): a server should be opened before can publish itself");
		return set_error_info(acs_dsd::ERR_SERVER_NOT_OPEN);
	}

	//Check if already published
	if (acs_dsd::SERVER_STATE_PUBLISHED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVER_STATE_PUBLISHED == state()): server is already published");
		return set_error_info(acs_dsd::ERR_SERVER_ALREADY_PUBLISHED);
	}

	//Check: the service mode cannot be unknown
	if (acs_dsd::SERVICE_MODE_UNKNOWN == service_mode()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVICE_MODE_UNKNOWN == service_mode()): the service mode is unknown");
		return set_error_info(acs_dsd::ERR_SERVICE_MODE_UNKNOWN);
	}

	//Check the service mode chosen by client: a server can publish itself only if the service mode is not private
	if ((service_mode() == acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE) || (service_mode() == acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE)) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION((service_mode() == acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE) || (service_mode() == acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE)): "
				"the service mode is private");
		return set_error_info(acs_dsd::ERR_SERVICE_MODE_IS_PRIVATE);
	}

	//Try to connect to the DSD server.
	ACS_DSD_LSocketConnector unix_connector;
	ACS_DSD_LSocketStream unix_stream;
	const char * dsd_sap_address = ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname();
	ACE_UNIX_Addr dsd_api_sap_address(dsd_sap_address);

	errno = 0;
	if (int call_result = unix_connector.connect(unix_stream, dsd_api_sap_address)) { //ERROR: problems while connecting to the DSD server
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'unix_connector.connect(...' failed: call_result == %d, errno == %d", call_result, errno);
		return set_error_info(call_result, call_result < 0 ? errno : 0, call_result < 0 ? _repeated_messages[4] : _repeated_messages[5], dsd_sap_address);
	}

	uint8_t response_code = 0;

	int return_code = (acs_dsd::SERVICE_MODE_UNIX_SOCKET ^ service_mode())
		? publish_inet(service_name, service_domain, visibility, unix_stream, response_code)
		: publish_unix(service_name, service_domain, visibility, unix_stream, response_code);

	unix_stream.close();

	if (!return_code) {
		if (response_code >= acs_dsd::PCP_ERROR_CODE_MAX_VALUE) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(response_code >= acs_dsd::PCP_ERROR_CODE_MAX_VALUE): unexpected error code");
			return set_error_info(acs_dsd::ERR_PCP_UNEXPECTED_ERROR_CODE);
		}
		if (response_code) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(response_code): an error code received from publishing process: response_code == %u", response_code);
			return set_error_info(acs_dsd::ERR_PCP_ERROR_CODE_START_CODE_BASE - response_code);
		}

		//Service published successful
		memccpy(_app_service_name, service_name, 0, acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);
		_app_service_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX - 1] = 0;
		memccpy(_app_domain_name, service_domain, 0, acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);
		_app_domain_name[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX - 1] = 0;
		state(acs_dsd::SERVER_STATE_PUBLISHED);
	} else {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: publishing process failed: return_code == %d", return_code);
	}

	return return_code;
}

int __CLASS_NAME__::accept (ACS_DSD_Session & session, int timeout_ms, bool apply_timeout) {
	static ACE_Recursive_Thread_Mutex _accept_sync_object;

	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if opened. A server should be opened before can accept a connection.
	if (acs_dsd::SERVER_STATE_CLOSED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVER_STATE_CLOSED == state()): a server should be opened before can accept a connection");
		return set_error_info(acs_dsd::ERR_SERVER_NOT_OPEN);
	}

	//Check if the object session is free and not already used for an established communication session
	if (session.state() ^ acs_dsd::SESSION_STATE_DISCONNECTED) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(session.state() ^ acs_dsd::SESSION_STATE_DISCONNECTED): session object already in use");
		return set_error_info(acs_dsd::ERR_SESSION_ALREADY_IN_USE);
	}

	int call_result = 0;
	int errno_save = 0;
	char remote_addr_str[64];

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_accept_sync_object_ptr ? *_accept_sync_object_ptr : _accept_sync_object, true);

		//Check I/O events over the acceptors socket handles.
		int client_connecting = 0;
		epoll_event io_event;
		errno = 0;
		if ((client_connecting = epoll_wait(_epoll_fd, &io_event, 1, apply_timeout ? timeout_ms : -1)) < 0) { //ERROR
			errno_save = errno;
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERROR: call 'epoll_wait(...' failed: client_connecting == %d, errno == %d", client_connecting, errno_save);
			return set_error_info(acs_dsd::ERR_SYSTEM_EPOLL_WAIT, errno_save,
				"Cannot check I/O events triggered for accepting connections onto this server's access points. Errors waiting on epoll interface");
		}

		//If there isn't a client connecting I return with WAR_TIMEOUT_EXPIRED
		if (!client_connecting) return set_error_info(acs_dsd::WAR_TIMEOUT_EXPIRED);

		//else, I try to accept the client telling the session object to make-up to communication channel
		errno = 0;
		call_result = (io_event.data.u64 < acs_dsd::CONFIG_NETWORKS_SUPPORTED)
				? session.accept_inet(_acceptors[io_event.data.u64], errno_save, remote_addr_str)
				: session.accept_unix(_acceptors[io_event.data.u64], errno_save);
	}

	if (call_result) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(call_result): an error occurred while accepting a connection: call_result == %d, errno == %d", call_result, errno_save);
	}

	//React to errors, if any
	switch (call_result) {
	case acs_dsd::ERR_WRONG_ACCEPTOR_TYPE: //ERROR: Very strange, we are using not the right acceptor. The library is corrupted!
		return set_error_info(acs_dsd::ERR_WRONG_ACCEPTOR_TYPE, errno_save, "Bad type of internal acceptor object: very strange! %s: "
				"session last error = %d, session last error text == '%s'", _repeated_messages[3], session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY: //ERROR: Out of memory allocating the session internal stream
		return set_error_info(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save, "session last error = %d, session last error text == '%s'",
				session.last_error(), session.last_error_text());
		break;
	case acs_dsd::WAR_WRONG_STREAM_TYPE: //ERROR: the acceptor internal object has received a bad stream type object
		return set_error_info(acs_dsd::ERR_WRONG_STREAM_TYPE, errno_save,
				"Internal acceptor object has received a bad I/O stream type: very strange! %s: "
				"session last error = %d, session last error text == '%s'", _repeated_messages[3], session.last_error(), session.last_error_text());
		break;
	case acs_dsd::WAR_ACCEPTOR_NOT_OPEN: //ERROR: the internal acceptor object was not open
		return set_error_info(acs_dsd::ERR_ACCEPTOR_NOT_OPEN, errno_save, "Internal acceptor object is not open: very strange! %s: "
				"session last error = %d, session last error text == '%s'", _repeated_messages[3], session.last_error(), session.last_error_text());
		break;
	case acs_dsd::WAR_STREAM_ALREADY_IN_USE:	//ERROR: Internal I/O stream object seems to be already in use
		return set_error_info(acs_dsd::ERR_STREAM_ALREADY_IN_USE, errno_save, "Internal I/O stream object seems to be already in use: very strange! %s: "
				"session last error = %d, session last error text == '%s'", _repeated_messages[3], session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_ACCEPT:	//ERROR: on accepting the remote client
		return set_error_info(acs_dsd::ERR_SYSTEM_ACCEPT, errno_save, "Cannot accept the connection from a client: "
				"session last error = %d, session last error text == '%s'", session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_SET_OPTION: //ERROR: setting the SO_KEEPALIVE option
		return set_error_info(acs_dsd::ERR_SYSTEM_SET_OPTION, errno_save, "Cannot set the SO_KEEPALIVE option on the internal I/O stream object: "
				"session last error = %d, session last error text == '%s'", session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_SYSTEM_GET_ADDRESS: //ERROR: getting the remote client address
		return set_error_info(acs_dsd::ERR_SYSTEM_GET_ADDRESS, errno_save, "Cannot get the address of the remote connecting client: "
				"session last error = %d, session last error text == '%s'", session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_CONFIG_NODE_IP_CONFLICT: //ERROR: same IP for more than one system node
		return set_error_info(acs_dsd::ERR_CONFIG_NODE_IP_CONFLICT, errno_save,
				"There is a conflict of IPs in the node configuration from CS: at least two node have the same IP '%s': "
				"session last error = %d, session last error text == '%s'", remote_addr_str, session.last_error(), session.last_error_text());
		break;
	case acs_dsd::WAR_NODE_NOT_FOUND: //ERROR: The remote node of the connecting client not found in the system configuration from CS
		return set_error_info(acs_dsd::ERR_NODE_NOT_FOUND, errno_save,
				"The node from wich a client (addr: %s) is connecting was not found in the system configuration loaded from CS. I will refuse this connection: "
				"session last error = %d, session last error text == '%s'", remote_addr_str, session.last_error(), session.last_error_text());
		break;
	case acs_dsd::ERR_REMOTE_SYSTEM_TYPE_UNKNOWN:
		return set_error_info(acs_dsd::ERR_REMOTE_SYSTEM_TYPE_UNKNOWN, errno_save,
				"The node from wich a client (addr: %s) is connecting has an unknown type in the configuration loaded from CS. I will refuse this connection: "
				"session last error = %d, session last error text == '%s'", remote_addr_str, session.last_error(), session.last_error_text());
		break;
	default:
		if (call_result) //OTHER ERRORS
			return set_error_info((call_result < 0) ? call_result : acs_dsd::ERR_SYSTEM_ACCEPT, errno_save, "An error occurred while accepting; error code == %d: "
					"session last error = %d, session last error text == '%s'", call_result, session.last_error(), session.last_error_text());
		break;
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::unregister () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if opened. A server should be opened before can unregister itself.
	if (acs_dsd::SERVER_STATE_CLOSED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVER_STATE_CLOSED == state()): a server should be opened before can unregister itself");
		return set_error_info(acs_dsd::ERR_SERVER_NOT_OPEN);
	}

	//If already not published do nothing
	if (acs_dsd::SERVER_STATE_OPEN == state()) return set_error_info(acs_dsd::ERR_NO_ERRORS);

	//Check: the service mode cannot be unknown
	if (acs_dsd::SERVICE_MODE_UNKNOWN == service_mode()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVICE_MODE_UNKNOWN == service_mode()): service mode is unknown");
		return set_error_info(acs_dsd::ERR_SERVICE_MODE_UNKNOWN);
	}

	//Check the service mode chosen by client: a server can unregister itself only if the service mode is not private
	if ((service_mode() == acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE) || (service_mode() == acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE)) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION((service_mode() == acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE) || (service_mode() == acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE)): "
				"service mode is private");
		return set_error_info(acs_dsd::ERR_SERVICE_MODE_IS_PRIVATE);
	}

	//Try to connect to the DSD server.
	ACS_DSD_LSocketConnector unix_connector;
	ACS_DSD_LSocketStream unix_stream;
	const char * dsd_sap_address = ACS_DSD_ConfigurationHelper::dsdapi_unix_socket_sap_pathname();
	ACE_UNIX_Addr dsd_api_sap_address(dsd_sap_address);

	if (int call_result = unix_connector.connect(unix_stream, dsd_api_sap_address)) { //ERROR: problems while connecting to the DSD server
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'unix_connector.connect(...' failed: call_result == %d, errno == %d", call_result, errno);
		return set_error_info(call_result, call_result < 0 ? errno : 0, call_result < 0 ? _repeated_messages[4] : _repeated_messages[5], dsd_sap_address);
	}

	ACS_DSD_MessageSender sender(unix_stream);
	ACS_DSD_MessageReceiver receiver(unix_stream);
	ACS_DSD_ProtocolHandler ph(sender, receiver);
	int call_result = 0;

	if ((call_result = ph.send_34(_app_service_name, _app_domain_name)) < 0) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'ph.send_34(...' failed: call_result == %d, errno == %d", call_result, errno);
		return (unix_stream.close(), set_error_info(call_result, errno, "Unregistering error: cannot send unregister request to the DSD server"));
	}

	int prim_id_received = 0;
	unsigned prim_version_received = 0;
	uint8_t response_code = 0;

	call_result = ph.recv_prim(35, prim_id_received, prim_version_received, response_code); //ph.recv_35(prim_id_received, response_code);
	int errno_save = errno;
	unix_stream.close();

	if (call_result == acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'ph.recv_prim(...' failed: unexpected primitive received: call_result == %d", call_result);
		return set_error_info(acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0,
				"Unregistering error: the DSD server replied with the primitive number %d instead of the primitive number 35", prim_id_received);
	} else if (call_result < 0) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'ph.recv_prim(...' failed: unregistering error: DSD server reply error: call_result == %d, errno == %d",
				call_result, errno_save);
		return set_error_info(call_result, errno_save, "Unregistering error: DSD server reply error");
	}

	if (response_code >= acs_dsd::PCP_ERROR_CODE_MAX_VALUE) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(response_code >= acs_dsd::PCP_ERROR_CODE_MAX_VALUE): unexpected response code: response_code == %u", response_code);
		return set_error_info(acs_dsd::ERR_PCP_UNEXPECTED_ERROR_CODE);
	}
	if (response_code) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(response_code): an error code received from unregistering process: response_code == %u", response_code);

		return set_error_info(acs_dsd::ERR_PCP_ERROR_CODE_START_CODE_BASE - response_code);
	}

	*_app_service_name = 0;
	*_app_domain_name = 0;
	state(acs_dsd::SERVER_STATE_OPEN);

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::close () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (acs_dsd::SERVER_STATE_CLOSED == state()) return set_error_info(acs_dsd::ERR_NO_ERRORS);

	unregister();

	epoll_delete_acceptors(_epoll_fd, _acceptors, acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1);

	int errno_save = 0;
	int call_result_save = 0;

	errno = 0;
	for (int i = 0; i <= acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) if (_acceptors[i]) {
		call_result_save ? (_acceptors[i]->close()) : ((call_result_save = _acceptors[i]->close()) && (errno_save = errno));
		_acceptors[i]->remove();
	}
	state(acs_dsd::SERVER_STATE_CLOSED);

	//In case of errors I prefer to destroy all internal objects
	call_result_save && (delete_acceptors(), 1);

	//Close the epoll file descriptor
	::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;

	if (call_result_save) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(call_result_save): an error occurred while closing the server: call_result_save == %d", call_result_save);
	}

	return set_error_info(call_result_save ? acs_dsd::WAR_SYSTEM_CLOSE : 0, errno_save,
			call_result_save ? "Non dangerous errors during a close because all internal acceptors are destroyed. System error info follows" : 0);
}

int __CLASS_NAME__::get_handles (acs_dsd::HANDLE * handles, int & handle_count) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if opened. A server should be opened before one can retrieve internal handles.
	if (acs_dsd::SERVER_STATE_CLOSED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVER_STATE_CLOSED == state()): a server should be opened before one can retrieve internal handles");
		return set_error_info(acs_dsd::ERR_SERVER_NOT_OPEN);
	}

	//Check: the service mode cannot be unknown
	if (acs_dsd::SERVICE_MODE_UNKNOWN == service_mode()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SERVICE_MODE_UNKNOWN == service_mode()): service mode is unknown");
		return set_error_info(acs_dsd::ERR_SERVICE_MODE_UNKNOWN);
	}

	int h_count = 0;
	int handle_count_save = handle_count;

	if ((acs_dsd::SERVICE_MODE_INET_SOCKET == service_mode()) || (acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE == service_mode())) {
		int valid_acceptor[acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1] = {0};

		for (int i = 0; i <= acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i)
			h_count += (valid_acceptor[i] = ((_acceptors[i] && (_acceptors[i]->state() == acs_dsd::ACCEPTOR_STATE_OPEN)) ? 1 : 0));

		//Is the handles caller's array capable to keep my internal handles?
		if (handle_count_save < (handle_count = h_count)) { //NO: Notify the caller suggesting it the right size
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(handle_count < h_count): the user provided a not capable array for storing server handles: "
					"handle_count == %d, h_count == %d", handle_count_save, h_count);
			return set_error_info(acs_dsd::ERR_NOT_ENOUGH_SPACE);
		}

		for (int i = 0; i <= acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) valid_acceptor[i] && (*handles++ = _acceptors[i]->get_handle());
	} else {
		ACS_DSD_Acceptor * p = _acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
		h_count = (p && (p->state() == acs_dsd::ACCEPTOR_STATE_OPEN)) ? 1 : 0;

		//Is the handles caller's array capable to keep my internal handles?
		if (handle_count_save < (handle_count = h_count)) { //NO: Notify the caller suggesting it the right size
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(handle_count < h_count): the user provided a not capable array for storing server handles: "
					"handle_count == %d, h_count == %d", handle_count_save, h_count);
			return set_error_info(acs_dsd::ERR_NOT_ENOUGH_SPACE);
		}

		h_count && (*handles = p->get_handle());
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

void __CLASS_NAME__::delete_acceptors () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	for (int i = 0; i <= acs_dsd::CONFIG_NETWORKS_SUPPORTED; _acceptors[i++] = 0) delete _acceptors[i];
}

int __CLASS_NAME__::open_inet (acs_dsd::ServiceModeConstants service_mode, const char * service_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check the service_id parameter correctness: the service_id should be a positive number with no spaces
	if (service_id) {
		int i = 0;
		while ((i < 5) && isdigit(service_id[i])) ++i;
		if (service_id[i]) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(service_id[i]): access point not valid");
			return set_error_info(acs_dsd::ERR_SERVICE_ACCESS_POINT_NOT_VALID);
		}
	}
	unsigned port_number = service_id ? strtoul(service_id, 0, 10) : 0;

	//Port number is in permitted address range?
	if (port_number && ((port_number < acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MIN) || ( acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MAX < port_number))) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(port_number && ((port_number < acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MIN) || ( acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MAX < port_number))): "
				"service access point out of range: port numbers should be in the range [%d, %d]",
				acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MIN, acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MAX);

		return set_error_info(acs_dsd::ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE, 0, "Port numbers should be in the range [%d, %d]",
				acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MIN, acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MAX);
	}

	int call_result = 0;

	errno = 0;
	if ((call_result = ACS_DSD_ConfigurationHelper::load_ap_nodes_from_cs())) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'ACS_DSD_ConfigurationHelper::load_ap_nodes_from_cs()' failed: Cannot load system AP nodes network configuration from CS: "
				"call_result == %d, errno == %d", call_result, call_result < 0 ? errno : 0);

		return set_error_info(call_result < 0 ? call_result : acs_dsd::ERR_LOAD_AP_NODES_FROM_CS, call_result < 0 ? errno : 0,
				"Cannot load system AP nodes network configuration from CS");
	}

	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t my_ap_host_ptr = 0;

	errno = 0;
	if ((call_result = ACS_DSD_ConfigurationHelper::load_my_ap_node()) || (call_result = ACS_DSD_ConfigurationHelper::get_my_ap_node(my_ap_host_ptr))) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(error in configuration load): "
				"Cannot find configuration network information about the AP node DSD API is running on: calling 'load_my_ap_node()' and 'get_my_ap_node(...'");

		return set_error_info(call_result < 0 ? call_result : acs_dsd::ERR_FIND_MY_AP_NODE_HOST_INFO, call_result < 0 ? errno : 0,
				"Cannot find configuration network information about the AP node DSD API is running on: calling 'load_my_ap_node()' and 'get_my_ap_node(...'");
	}

	if (my_ap_host_ptr->system_id < 0) { //Configuration mismatch into the CS block
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(my_ap_host_ptr->system_id < 0): a bad or mismatch ap nodes configuration found into the CS about the AP node I'm running on");

		return set_error_info(acs_dsd::ERR_AP_CONFIGURATION_MISMATCH_FROM_CS, 0,
				"A bad or mismatch ap nodes configuration found into the CS about the AP node I'm running on");
	}

	if ((call_result = ACS_DSD_ConfigurationHelper::load_process_information())) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'ACS_DSD_ConfigurationHelper::load_process_information()' failed: Cannot get information about the process DSD API is running on");

		return set_error_info(call_result < 0 ? call_result : acs_dsd::ERR_GET_PROCESS_INFORMATION, call_result < 0 ? errno : 0,
				"Cannot get information about the process DSD API is running on");
	}

	//Create a new epoll interface file descriptor for future accepting
	(_epoll_fd ^ acs_dsd::INVALID_HANDLE) && ::close(_epoll_fd);
	if (-1 == (_epoll_fd = epoll_create(acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1))) {
		_epoll_fd = acs_dsd::INVALID_HANDLE;

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'epoll_create(...' failed: cannot create the internal epoll interface file descriptor");

		return set_error_info(acs_dsd::ERR_SYSTEM_CREATE, errno, "Cannot create the internal epoll interface file descriptor");
	}

	ACS_DSD_Acceptor * acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1] = {0};
	int tmp_inet_acceptors_to_delete = 0;
	int tmp_unix_acceptors_to_delete = 0;

	if (*_acceptors) //Copy internals acceptors in the acceptor_tmp array
		for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) acceptors_tmp[i] = _acceptors[i];
	else { //New internal acceptors should be created
		for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i)
			if (!(acceptors_tmp[i] = new (std::nothrow) ACS_DSD_SocketAcceptor())) { //Out of memory error: I have to destroy all preallocated acceptors and return
				int errno_save = errno;
				for (int j = 0; j < i; delete acceptors_tmp[j++]) ;
				::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;

				ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
						"ERR_ASSERTION(out of memory): Cannot allocate internal acceptor objects%s", _repeated_messages[0]);

				return set_error_info(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save, "Cannot allocate internal acceptor objects%s", _repeated_messages[0]);
			}
		tmp_inet_acceptors_to_delete = acs_dsd::CONFIG_NETWORKS_SUPPORTED;
	}
	if (_acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED]) //Copy unix acceptor to the acceptor_tmp array
		acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = _acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
	else
		tmp_unix_acceptors_to_delete = ((acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = new (std::nothrow) ACS_DSD_LSocketAcceptor()) ? 1 : 0);

	//Try to open (or reopen if reuse_internals == 1) internals acceptors
	int so_keepalive_optval = 1;
	int errno_save = 0;
	int first_not_opened_index = -1;
	size_t acceptors_not_opened = 0;
	bool use_caller_port_number = (port_number ? true : false);
	int port_search_max_retries = (acs_dsd::CONFIG_INET_PORT_SEARCH_MAX_RETRIES < 1 ? 1 : acs_dsd::CONFIG_INET_PORT_SEARCH_MAX_RETRIES);
	bool isPortReservedFlag = false;	//TR HX96291

	use_caller_port_number || (port_number = inet_port_sequencer.next());

	for (size_t i = 0; i < my_ap_host_ptr->ip4_adresses_count; ++i) {
		ACE_INET_Addr inet_addr;

		for (int port_retry = 0; port_retry < port_search_max_retries; ++port_retry, port_number = inet_port_sequencer.next()) {
			inet_addr.set(htons(port_number), my_ap_host_ptr->ip4_addresses[i], 0);
			errno = 0;
			isPortReservedFlag = isPortReserved(port_number);	//TR HX96291
			if (!(call_result = acceptors_tmp[i]->open(inet_addr, 1)) || use_caller_port_number || (errno ^ EADDRINUSE)) {
				if(use_caller_port_number || !(isPortReservedFlag))	//TR HX96291
					break;
			}
		}

		if (call_result) { //ERROR or WARNING: opening the internal acceptor access point
			errno_save || (errno_save = errno, first_not_opened_index = static_cast<int>(i));
			++acceptors_not_opened;
/* The error verification should be done after the loop
			errno_save = errno;
			for (size_t j = 0; j < i; acceptors_tmp[j++]->close()) ;
			for (int j = 0; j < tmp_inet_acceptors_to_delete; delete acceptors_tmp[j++]) ;
			if (tmp_unix_acceptors_to_delete) delete acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
			return set_error_info(acs_dsd::ERR_SYSTEM_OPEN, errno_save, "Cannot open internal acceptor object at index %zu on SAP address '%s:%u'%s", i, my_ap_node_info.ip4_addresses_str[i], port_number, (reuse_internals && *_acceptors ? ". Try to not reuse internals objects calling the open method passing 0 (zero) to the 'reuse_internals' parameter" : ""));
*/
		} else if ((call_result = acceptors_tmp[i]->set_option(SOL_SOCKET, SO_KEEPALIVE, &so_keepalive_optval, sizeof(so_keepalive_optval)))) {
/* It's not a serious error
			int errno_save = errno;
			for (size_t j = 0; j <= i; acceptors_tmp[j++]->close()) ;
			for (int j = 0; j < tmp_inet_acceptors_to_delete; delete acceptors_tmp[j++]) ;
			if (tmp_unix_acceptors_to_delete) delete acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
			return set_error_info(call_result, errno_save, "Cannot set the KEEPALIVE option on the underlying socket system object at index %zu on SAP address '%s:%u'%s", i, my_ap_node_info.ip4_addresses_str[i], port_number, (reuse_internals && *_acceptors ? ". Try to not reuse internals objects calling the open method passing 0 (zero) to the 'reuse_internals' parameter" : ""));
*/
		}
	}

	//Check Point: If at least one acceptor is opened I can continue; else an error is returned.
	if (acceptors_not_opened >= my_ap_host_ptr->ip4_adresses_count) { //No acceptor opened, clean-up and return with error
		for (int j = 0; j < tmp_inet_acceptors_to_delete; delete acceptors_tmp[j++]) ;
		if (tmp_unix_acceptors_to_delete) delete acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
		::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: Cannot open internal acceptor objects; the first acceptor SAP is at address '%s:%u': errno == %d",
				my_ap_host_ptr->ip4_addresses_str[0], port_number, errno_save);

		return set_error_info(acs_dsd::ERR_SYSTEM_OPEN, errno_save,
			"Cannot open internal acceptor objects; the system error for the first acceptor on SAP address '%s:%u' follows%s",
			my_ap_host_ptr->ip4_addresses_str[0], port_number, (*_acceptors ? _repeated_messages[2] : _repeated_messages[0]));
	}

	if (acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED]) {
		char unix_addr_str[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX];

		//Make up the name of the unix SAP using the template [PROCESS_NAME][PID][PORT]
		snprintf(unix_addr_str, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX, "%s%s_%d_%u", ACS_DSD_ConfigurationHelper::unix_socket_root_path(),
			ACS_DSD_ConfigurationHelper::my_process_name(), ACS_DSD_ConfigurationHelper::my_process_id(), port_number);

		ACE_UNIX_Addr unix_addr(unix_addr_str);
		acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED]->open(unix_addr, 1);
	}

	//Initialize and activate the epoll interface onto the acceptors opened.
	int errno_save_2 = 0;
	if (epoll_add_acceptors(_epoll_fd, acceptors_tmp, sizeof(acceptors_tmp)/sizeof(*acceptors_tmp), errno_save_2)) {
		//Close and destroy (if created in this function) all the acceptors
		for (int i = 0; i <= acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) acceptors_tmp[i] && (acceptors_tmp[i]->close(), acceptors_tmp[i]->remove());
		for (int i = 0; i < tmp_inet_acceptors_to_delete; delete acceptors_tmp[i++]) ;
		ACS_DSD_Acceptor * p = acceptors_tmp[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
		p && (p->close(), p->remove());
		if (tmp_unix_acceptors_to_delete) delete p;
		::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: It was not possible to add internal acceptors I/O (INET and UNIX) socket handles (file descriptors) to the epoll multiplexing interface for future accepting: "
				"errno == %d", errno_save_2);

		return set_error_info(acs_dsd::ERR_SYSTEM_IO_MULTIPLEXING_ACTIVATE, errno_save_2,
			"It was not possible to add internal acceptors I/O (INET and UNIX) socket handles (file descriptors) to the epoll multiplexing interface for future accepting");
	}

	memcpy(_acceptors, acceptors_tmp, sizeof(_acceptors));
	_service_mode = service_mode;

	int return_code = 0;

	if (my_ap_host_ptr->ip4_adresses_count < acs_dsd::CONFIG_NETWORKS_SUPPORTED)
		return_code = acceptors_not_opened
			? set_error_info(acs_dsd::WAR_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE, errno_save,
					"Only %zu network%s of %d %s available and functioning on the system. Moreover, some internal acceptor was not open correctly; the system error for the "
					"acceptor at index %d on SAP address '%s:%u' follows%s",
					my_ap_host_ptr->ip4_adresses_count, (my_ap_host_ptr->ip4_adresses_count ^ 1) ? "s" : "",
					acs_dsd::CONFIG_NETWORKS_SUPPORTED, (my_ap_host_ptr->ip4_adresses_count ^ 1) ? "are" : "is", first_not_opened_index,
							my_ap_host_ptr->ip4_addresses_str[first_not_opened_index], port_number,
					(*_acceptors ? _repeated_messages[2] : _repeated_messages[0]))
			: set_error_info(acs_dsd::WAR_SOME_NETWORKS_UNAVAILABLE, 0,
					"Only %zu network%s of %d %s available and functioning on the system", my_ap_host_ptr->ip4_adresses_count,
					(my_ap_host_ptr->ip4_adresses_count ^ 1) ? "s" : "", acs_dsd::CONFIG_NETWORKS_SUPPORTED, (my_ap_host_ptr->ip4_adresses_count ^ 1) ? "are" : "is");
	else if (acceptors_not_opened)
		return_code = set_error_info(acs_dsd::WAR_SOME_INTERNAL_ACCEPTORS_NOT_OPEN, errno_save,
			"Some internal acceptor was not open correctly; the system error for the acceptor at index %d on SAP address '%s:%u' follows%s",
			first_not_opened_index, my_ap_host_ptr->ip4_addresses_str[first_not_opened_index], port_number,
			(*_acceptors ? _repeated_messages[2] : _repeated_messages[0]));
	else return_code = set_error_info(0);

	state(acs_dsd::SERVER_STATE_OPEN); //The server is open

	if (return_code) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "WARNING: %s", last_error_text());
	}

	return return_code;
}

int __CLASS_NAME__::open_unix (acs_dsd::ServiceModeConstants service_mode, const char * service_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;

	if ((call_result = ACS_DSD_ConfigurationHelper::load_process_information())) {
		int err_return = set_error_info(call_result < 0 ? call_result : acs_dsd::ERR_GET_PROCESS_INFORMATION, call_result < 0 ? errno : 0,
				"Cannot get information about the process DSD API is running in");

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	//Create a new epoll interface file descriptor for future accepting
	(_epoll_fd ^ acs_dsd::INVALID_HANDLE) && ::close(_epoll_fd);
	if (-1 == (_epoll_fd = epoll_create(acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1))) {
		_epoll_fd = acs_dsd::INVALID_HANDLE;
		int err_return = set_error_info(acs_dsd::ERR_SYSTEM_CREATE, errno, "Cannot create the internal epoll interface file descriptor");

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	ACS_DSD_Acceptor * acceptor_tmp = 0;
	int tmp_unix_acceptors_to_delete = 0;

	//If the caller would like to reuse internal objects and it is possible, copy unix acceptor to the acceptor_tmp array.
	//Else a new unix acceptor will be created.
	if (_acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED]) acceptor_tmp = _acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
	else if (!(acceptor_tmp = new (std::nothrow) ACS_DSD_LSocketAcceptor())) { //ERROR: out of memory
		int errno_save = errno;
		::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;
		int err_return = set_error_info(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save, "Cannot allocate internal acceptor objects%s", _repeated_messages[0]);

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	} else tmp_unix_acceptors_to_delete = 1;

	char unix_addr_str[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX];

	if (service_id && *service_id) { //A service_id provided. Check and use it
		char service_id_copy[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX];

		//Truncate if longer than acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX
		memccpy(service_id_copy, service_id, 0, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
		service_id_copy[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX - 1] = 0;

		char * last_slash_ptr = strrchr(service_id_copy, '/');

		int path_found = last_slash_ptr ? 1 : 0;
		int name_found = 1;
		path_found && (name_found = last_slash_ptr[1] ? 1 : 0);

		//Make up the name of the unix SAP integrating the service_id info
		name_found ? snprintf(unix_addr_str, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX, "%s%s", path_found ? "" : ACS_DSD_ConfigurationHelper::unix_socket_root_path(),
										service_id_copy)
							 : snprintf(unix_addr_str, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX, "%s%s_%d",
										path_found ? service_id_copy : ACS_DSD_ConfigurationHelper::unix_socket_root_path(), ACS_DSD_ConfigurationHelper::my_process_name(),
										ACS_DSD_ConfigurationHelper::my_process_id());
	} else
		//Make up the name of the unix SAP using the template [PROCESS_NAME][PID]
		snprintf(unix_addr_str, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX, "%s%s_%d", ACS_DSD_ConfigurationHelper::unix_socket_root_path(),
				ACS_DSD_ConfigurationHelper::my_process_name(), ACS_DSD_ConfigurationHelper::my_process_id());

	//Try to open (or reopen if reuse_internals == 1) internals acceptors
	ACE_UNIX_Addr unix_addr(unix_addr_str);
	if (acceptor_tmp->open(unix_addr, 1)) { //ERROR: opening the acceptor
		int errno_save = errno;
		::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;
		if (tmp_unix_acceptors_to_delete) delete acceptor_tmp;
		int err_return = set_error_info(acs_dsd::ERR_SYSTEM_OPEN, errno_save, "Cannot open internal UNIX acceptor object on SAP address '%s'%s", unix_addr_str,
				(_acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED] ? _repeated_messages[2] : _repeated_messages[0]));

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	//Initialize and activate the epoll interface onto the acceptors opened.
	int errno_save_2 = 0;
	if (epoll_add_acceptors(_epoll_fd, &acceptor_tmp, 1, errno_save_2, acs_dsd::CONFIG_NETWORKS_SUPPORTED)) {
		//Close and destroy (if created in this function) all the acceptors
		acceptor_tmp->close();
		acceptor_tmp->remove();
		if (tmp_unix_acceptors_to_delete) delete acceptor_tmp;
		::close(_epoll_fd); _epoll_fd = acs_dsd::INVALID_HANDLE;
		int err_return = set_error_info(acs_dsd::ERR_SYSTEM_IO_MULTIPLEXING_ACTIVATE, errno_save_2,
			"It was not possible to add internal acceptor I/O UNIX socket handle (file descriptor) to the epoll multiplexing interface for future accepting");

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	_acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = acceptor_tmp;
	_service_mode = service_mode;
	state(acs_dsd::SERVER_STATE_OPEN); //The server is open

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::publish_inet (const char * service_name, const char * service_domain, acs_dsd::ServiceVisibilityConstants visibility, ACS_DSD_IOStream & unix_stream, uint8_t & response_code) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_MessageSender sender(unix_stream);
	ACS_DSD_MessageReceiver receiver(unix_stream);
	ACS_DSD_ProtocolHandler ph(sender, receiver);

	int call_result = 0;
	int i = 0;
	int addresses_found = 0;

	ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
	for (i = 0; !call_result && (i < acs_dsd::CONFIG_NETWORKS_SUPPORTED); ++i)
		if (_acceptors[i] && (acs_dsd::ACCEPTOR_STATE_CLOSED ^ _acceptors[i]->state()))
			(call_result = _acceptors[i]->get_local_addr(inet_addresses[i])) || addresses_found++;

	if (call_result) {
		int err_return = set_error_info(call_result, errno, "Publishing error: cannot get the service access point address on the internal acceptor at index %d", i);

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	if (!addresses_found) { //ERROR: No addresses found
		int err_return = set_error_info(acs_dsd::ERR_ACCEPTORS_ADDRESSES_NOT_FOUND, 0,
				"Publishing error: internal acceptors seem not open, NULL or not created: very dangerous error. This ACS_DSD_Server object is corrupted, try to close it or use another one");

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	ACE_UNIX_Addr unix_address;
	_acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED] && (_acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED]->get_local_addr(unix_address));

	if ((call_result = ph.send_31(service_name, service_domain, inet_addresses, unix_address, visibility)) < 0) {
		int err_return = set_error_info(call_result, errno, "Publishing error: cannot send publishing request to the DSD server");

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	int prim_id_received = 0;
	unsigned prim_version_received = 0;

	if (acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED == (call_result = ph.recv_prim(33, prim_id_received, prim_version_received, response_code))) { //ph.recv_33(prim_id_received, response_code)
		int err_return = set_error_info(acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0,
				"Publishing error: the DSD server replied with the primitive number %d instead of the primitive number 33", prim_id_received);

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	} else if (call_result < 0) {
		int err_return = set_error_info(call_result, errno, "Publishing error: DSD server reply error");

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());

		return err_return;
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::publish_unix (const char * service_name, const char * service_domain, acs_dsd::ServiceVisibilityConstants visibility, ACS_DSD_IOStream & unix_stream, uint8_t & response_code) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_MessageSender sender(unix_stream);
	ACS_DSD_MessageReceiver receiver(unix_stream);
	ACS_DSD_ProtocolHandler ph(sender, receiver);

	int call_result = 0;
	int err_return = 0;

	ACE_UNIX_Addr address;
	ACS_DSD_Acceptor * unix_acceptor = _acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
	if (unix_acceptor) {
		if (acs_dsd::ACCEPTOR_STATE_CLOSED == unix_acceptor->state()) {
			err_return = set_error_info(acs_dsd::ERR_OBJECT_CORRUPTED, 0,
					"Publishing error: the internal acceptor object results closed but should not be so. This ACS_DSD_Server object is corrupted, try to close it or use another one");
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());
			return err_return;
		}

		if ((call_result = unix_acceptor->get_local_addr(address))) {
			err_return = set_error_info(call_result, errno, "Publishing error: cannot get the service access point address on the unix local socket");
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());
			return err_return;
		}

		if ((call_result = ph.send_32(service_name, service_domain, address, visibility)) < 0) {
			err_return = set_error_info(call_result, errno, "Publishing error: cannot send publishing request to the DSD server");
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());
			return err_return;
		}

		int prim_id_received = 0;
		unsigned prim_version_received = 0;

		if (acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED == (call_result = ph.recv_prim(33, prim_id_received, prim_version_received, response_code))) { //ph.recv_33(prim_id_received, response_code)
			err_return = set_error_info(acs_dsd::ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED, 0, "Publishing error: the DSD server replied with the primitive number %d instead of the primitive number 33", prim_id_received);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());
			return err_return;
		} else if (call_result < 0) {
			err_return = set_error_info(call_result, errno, "Publishing error: DSD server reply error");
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());
			return err_return;
		}
	} else { //DANGEROUS ERROR: the internal acceptor is null, very strange
		err_return = set_error_info(acs_dsd::ERR_OBJECT_CORRUPTED, 0,
				"Publishing error: the internal acceptor results NULL or not created: very dangerous error. This ACS_DSD_Server object is corrupted, try to close it or use another one");
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: error code == %d, error text == '%s'", last_error(), last_error_text());
		return err_return;
	}

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::epoll_add_acceptors (int epfd, ACS_DSD_Acceptor * acceptors[], size_t size, int & errno_save, size_t start_index) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (epfd ^ acs_dsd::INVALID_HANDLE) {
		epoll_event event_to_check;
		event_to_check.events = EPOLLIN;

		errno_save = 0;

		//Initialize the internal epoll interface using the handle by acceptors in input.
		for (size_t i = 0; i < size; ++i) if (acceptors[i] && (acs_dsd::ACCEPTOR_STATE_CLOSED ^ acceptors[i]->state())) {
			event_to_check.data.u64 = start_index + i;
			if (epoll_ctl(epfd, EPOLL_CTL_ADD, acceptors[i]->get_handle(), &event_to_check)) { //ERROR: critical error, the interface cannot accept connections correctly
				errno_save = errno;
				i && epoll_delete_acceptors(epfd, acceptors, i); //Delete handles previously added

				ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: call 'epoll_ctl(...' failed: errno == %d", errno_save);

				return acs_dsd::ERR_SYSTEM_EPOLL_CTL_ADD;
			}
		}
	}

	return 0;
}

int __CLASS_NAME__::epoll_delete_acceptors (int epfd, ACS_DSD_Acceptor * acceptors[], size_t size) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (epfd ^ acs_dsd::INVALID_HANDLE) {
		epoll_event event_to_check;

		//Delete all the socket file descriptor from the epoll interface.
		for (size_t i = 0; i < size; ++i) if (acceptors[i] && (acs_dsd::ACCEPTOR_STATE_CLOSED ^ acceptors[i]->state()))
			epoll_ctl(epfd, EPOLL_CTL_DEL, acceptors[i]->get_handle(), &event_to_check);
	}

	return 0;
}

//Start of TR HX96291
bool __CLASS_NAME__::isPortReserved(int port) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	bool reserved = false;

	// Port numbers lesser than 1024 are always reserved
	if ( port < 1024 || port > 0xffff ) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Port[%d] is always reserved", port);
		return true;
	}

	struct servent* se = ::getservbyport( htons((unsigned short)port), NULL );
	if ( se )
		reserved = true;

	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), " Port[%d] is reserved ==%d", port, reserved);

	return reserved;
}
//End of TR HX96291
