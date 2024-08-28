#include <time.h>
#include <stdlib.h>

#include <new>

#include "ACS_DSD_SocketAcceptor.h"
#include "ACS_DSD_SocketConnector.h"
#include "ACS_DSD_SocketStream.h"
#include "ACS_DSD_LSocketAcceptor.h"
#include "ACS_DSD_LSocketConnector.h"
#include "ACS_DSD_LSocketStream.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_MessageReceiver.h"
#include "ACS_DSD_MessageSender.h"
#include "ACS_DSD_OCPMessageReceiver.h"
#include "ACS_DSD_OCPMessageSender.h"
#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_Session.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

namespace {
	ssize_t send_accept_indication (ACS_DSD_SocketStream * stream, uint16_t port, ACE_Time_Value * timeout = 0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		//The OCP Accept Indication message is 6 bytes long
		//As stated in the design base, all the integer information are in network byte order (to be verified using a test).
		//The record is made in this way
		// [0][1]: message size in network (big endian) order; this should contain a value of 2 because we are sent the 2-bytes port number (view next)
		// [2]: Primitive id == 14
		// [3]: Always 0 (zero)
		// [4][5]: the application port number in network (big endian) order

		uint8_t buffer[6] = {0, 2, 14, 0, 0, 0};
		*reinterpret_cast<uint16_t *>(buffer + 4) = htons(port);

		// Default timeout value to 5 seconds
		ACE_Time_Value default_timeout(5);

		if (!timeout) timeout = &default_timeout;

		return stream->send_n(buffer, 6, timeout);
	}

	ssize_t recv_accept_confirmation (
			ACS_DSD_SocketStream * stream,
			uint16_t & ocp_error_code,
			uint16_t & ocp_message_size,
			ACE_Time_Value * timeout = 0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		//The OCP Accept Confirmation message is 6 bytes long
		//As checked in the design base, all the integer informations seem to be in network byte order (to be verified using a test).
		//The record is made in this way
		// [0][1]: message size in network byte order; this should contain a value of 2 because we are receiving a confirmation result code (see later)
		// [2]: Primitive id == 15
		// [3]: Always 0 (zero)
		// [4][5]: error code in network byte order. 0 == no faults, 1 == User not found, 2-up == User defined error code

		// Default timeout value to 5 seconds
		ACE_Time_Value default_timeout(5);

		if (!timeout) timeout = &default_timeout;

		uint8_t buffer[6] = {0};
		ssize_t bytes_received = stream->recv_n(buffer, 6, timeout);

		if (bytes_received > 0) {
			ocp_error_code = ntohs(*reinterpret_cast<uint16_t *>(buffer + 4));
			ocp_message_size = ntohs(*reinterpret_cast<uint16_t *>(buffer));

			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ocp_error_code == %u", ocp_error_code);
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ocp_message_size == %u", ocp_message_size);
		}

		return bytes_received;
	}
}

__CLASS_NAME__::~__CLASS_NAME__ () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	_stream && _stream->close();
	delete _stream;
}

int __CLASS_NAME__::get_handles (acs_dsd::HANDLE * handles, int & handle_count) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if connected. A session should be connected before it can retrieve internal handles.
	if (acs_dsd::SESSION_STATE_DISCONNECTED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SESSION_STATE_DISCONNECTED == state()): this session object is disconnected");
		return set_error_info(acs_dsd::ERR_SESSION_DISCONNECTED);
	}

	int handle_count_save = handle_count;
	int h_count = (_stream->state() == acs_dsd::STREAM_STATE_DISCONNECTED ? 0 : 1);

	//Is the handles caller's array capable to keep my internal handles?
	if (handle_count_save < (handle_count = h_count)) { //NO: Notify the caller suggesting it the right size
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(handle_count < h_count): the user provided a not capable array for storing session handles: "
				"handle_count == %d, h_count == %d", handle_count_save, h_count);
		return set_error_info(acs_dsd::ERR_NOT_ENOUGH_SPACE);
	}

	h_count && (*handles = _stream->get_handle());

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::get_remote_node (ACS_DSD_Node & node) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if connected. A session should be connected before it can retrieve the remote peer node
	if (acs_dsd::SESSION_STATE_DISCONNECTED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SESSION_STATE_DISCONNECTED == state()): this session object is disconnected");
		return set_error_info(acs_dsd::ERR_SESSION_DISCONNECTED);
	}

	node = _remote_node;

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

ssize_t __CLASS_NAME__::recv (void * buf, size_t size, const unsigned * timeout_ms, size_t * bytes_transferred) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if connected. A session should be connected before it can receive data
	if (acs_dsd::SESSION_STATE_DISCONNECTED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SESSION_STATE_DISCONNECTED == state()): this session object is disconnected");
		return set_error_info(acs_dsd::ERR_SESSION_DISCONNECTED);
	}

	char receiver_memory_stub[sizeof(ACS_DSD_OCPMessageReceiver) > sizeof(ACS_DSD_MessageReceiver) ? sizeof(ACS_DSD_OCPMessageReceiver) : sizeof(ACS_DSD_MessageReceiver)];

	const ACS_DSD_Receiver * receiver =
			(_OCP_transfert_active ? reinterpret_cast<ACS_DSD_Receiver *>(new (receiver_memory_stub) ACS_DSD_OCPMessageReceiver(_stream))
														 : reinterpret_cast<ACS_DSD_Receiver *>(new (receiver_memory_stub) ACS_DSD_MessageReceiver(_stream)));

	errno = 0;
	ssize_t bytes_received = 0;

	if (timeout_ms) {
		ACE_Time_Value timeout;
		timeout.msec(static_cast<long int>(*timeout_ms));
		bytes_received = receiver->recv(buf, size, &timeout, bytes_transferred);
	} else bytes_received = receiver->recv(buf, size, 0, bytes_transferred);

	receiver->~ACS_DSD_Receiver();

	if (bytes_received <= 0) {
		if (bytes_received) ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_received <= 0): ERROR on receiving packet: bytes_received == %zd, errno == %d", bytes_received, errno);
		else ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_received <= 0): WARNING on receiving packet: peer closed connection: errno == %d", errno);
		return set_error_info(bytes_received ?: static_cast<ssize_t>(acs_dsd::ERR_PEER_CLOSED_CONNECTION), errno);
	}

	return (set_error_info(acs_dsd::ERR_NO_ERRORS), bytes_received);
}

ssize_t __CLASS_NAME__::send (const void * buf, size_t size, const unsigned * timeout_ms, size_t * bytes_transferred) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	return sendf_imp(buf, size, timeout_ms, bytes_transferred, 0);

#if 0 // TO BE REMOVED: Using the flagged send version now
	//Check if connected. A session should be connected before it can send data
	if (acs_dsd::SESSION_STATE_DISCONNECTED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SESSION_STATE_DISCONNECTED == state()): this session object is disconnected");
		return set_error_info(acs_dsd::ERR_SESSION_DISCONNECTED);
	}

	char sender_memory_stub[sizeof(ACS_DSD_OCPMessageSender) > sizeof(ACS_DSD_MessageSender) ? sizeof(ACS_DSD_OCPMessageSender) : sizeof(ACS_DSD_MessageSender)];

	const ACS_DSD_Sender * sender =
			(_OCP_transfert_active ? reinterpret_cast<ACS_DSD_Sender *>(new (sender_memory_stub) ACS_DSD_OCPMessageSender(_stream))
														 : reinterpret_cast<ACS_DSD_Sender *>(new (sender_memory_stub) ACS_DSD_MessageSender(_stream)));

	errno = 0;
	ssize_t bytes_sent = 0;

	if (timeout_ms) {
		ACE_Time_Value timeout;
		timeout.msec(static_cast<long int>(*timeout_ms));
		bytes_sent = sender->send(buf, size, &timeout, bytes_transferred);
	} else bytes_sent = sender->send(buf, size, 0, bytes_transferred);

	sender->~ACS_DSD_Sender();

	if (bytes_sent <= 0) {
		if (bytes_sent) ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_sent <= 0): ERROR on sending packet: bytes_sent == %zd, errno == %d", bytes_sent, errno);
		else ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_sent <= 0): WARNING on sending packet: peer closed connection: errno == %d", errno);
		return set_error_info(bytes_sent ?: static_cast<ssize_t>(acs_dsd::ERR_PEER_CLOSED_CONNECTION), errno);
	}

	return (set_error_info(acs_dsd::ERR_NO_ERRORS), bytes_sent);
#endif
}

int __CLASS_NAME__::close_reader () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;
	errno = 0;

	if (_stream) {
		call_result = _stream->close_reader();

		(_stream->state() == acs_dsd::STREAM_STATE_DISCONNECTED) && (reset_remote_node(), 1);
	}

	return set_error_info(call_result, errno, "Closing the reader channel of the session");
}

int __CLASS_NAME__::close_writer () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;
	errno = 0;

	if (_stream) {
		call_result = _stream->close_writer();

		(_stream->state() == acs_dsd::STREAM_STATE_DISCONNECTED) && (reset_remote_node(), 1);
	}

	return set_error_info(call_result, errno, "Closing the writer channel of the session");
}

int __CLASS_NAME__::close () {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;
	errno = 0;

	_stream && (call_result = _stream->close());

	call_result || (reset_remote_node(), 1);

	return set_error_info(call_result, errno, "Closing the session");
}

acs_dsd::SessionStateConstants __CLASS_NAME__::state () const {
	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::state()");

	return _stream ? static_cast<acs_dsd::SessionStateConstants>(_stream->state()) : acs_dsd::SESSION_STATE_DISCONNECTED;
}

int __CLASS_NAME__::get_option (int level, int option, void * optval, int * optlen) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if connected. A session should be connected before it can retrieve socket options.
	if (acs_dsd::SESSION_STATE_DISCONNECTED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SESSION_STATE_DISCONNECTED == state()): this session object is disconnected");
		return set_error_info(acs_dsd::ERR_SESSION_DISCONNECTED);
	}

	int call_result = 0;

	errno = 0;
	_stream && (call_result = _stream->get_option(level, option, optval, optlen));

	return set_error_info(call_result, errno, "Getting underlying socket option");
}

int __CLASS_NAME__::set_option (int level, int option, void * optval, int optlen) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if connected. A session should be connected before it can set socket options.
	if (acs_dsd::SESSION_STATE_DISCONNECTED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SESSION_STATE_DISCONNECTED == state()): this session object is disconnected");
		return set_error_info(acs_dsd::ERR_SESSION_DISCONNECTED);
	}

	int call_result = 0;

	errno = 0;
	_stream && (call_result = _stream->set_option(level, option, optval, optlen));

	return set_error_info(call_result, errno, "Setting underlying socket option");
}

int __CLASS_NAME__::accept_inet (ACS_DSD_Acceptor * acceptor, int & errno_save, char (& remote_addr_str)[64]) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_SocketAcceptor * sock_acceptor = dynamic_cast<ACS_DSD_SocketAcceptor *>(acceptor);

	errno_save = 0;

	if (!sock_acceptor) { //ERROR: Very strange, the acceptor should be a socket acceptor. The library is corrupted!
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(!sock_acceptor): The caller provided an acceptor object that is not a socket acceptor: library corrupted");
		return set_error_info(acs_dsd::ERR_WRONG_ACCEPTOR_TYPE, 0, "The caller provided an acceptor object that is not a socket acceptor");
	}

	errno = 0;
	ACS_DSD_SocketStream * stream = new (std::nothrow) ACS_DSD_SocketStream();
	if (!stream) { //ERROR: Out of memory
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(!stream): system is in out of memory when creating a new socket stream");
		return (errno_save = errno, set_error_info(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno, "creating a new socket stream"));
	}

	errno = 0;
	if (const int call_result = sock_acceptor->accept(*stream)) { //ERROR: accepting the client
		errno_save = errno;
		delete stream;
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(const int call_result = sock_acceptor->accept(*stream)): error in accepting a client connection: call_result == %d",
				call_result);
		return set_error_info(call_result, errno_save, "accepting a connection");
	}

	int so_keepalive_optval = 1;

	//Set the SO_KEEPALIVE option on the underlying socket descriptor
	errno = 0;
	if (const int call_result = stream->set_option(SOL_SOCKET, SO_KEEPALIVE, &so_keepalive_optval, sizeof(so_keepalive_optval))) { //ERROR: setting the option
		errno_save = errno;
		stream->close();
		delete stream;
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(const int call_result = stream->set_option(SOL_SOCKET, SO_KEEPALIVE, &so_keepalive_optval, sizeof(so_keepalive_optval))): "
				"error on setting the keep-alive option on the socket session: call_result == %d",
				call_result);
		return set_error_info(call_result, errno_save, "setting the keep-alive option on the socket session");
	}

	ACE_INET_Addr remote_addr;
	errno = 0;
	if (const int call_result = stream->get_remote_addr(remote_addr)) { //ERROR: Retrieving remote host address
		errno_save = errno;
		stream->close();
		delete stream;
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(const int call_result = stream->get_remote_addr(remote_addr)): "
				"error trying to get the remote peer side info: call_result == %d",
				call_result);
		return set_error_info(call_result, errno_save, "trying to get the remote peer side info");
	}

	remote_addr.addr_to_string(remote_addr_str, 64);

	uint32_t remote_addr_net_order = htonl(remote_addr.get_ip_address());

#if ACS_DSD_CONFIG_DISABLE_PEER_CHECK_FOR_SEA_TEST == 1
	ACS_DSD_ConfigurationHelper::HostInfo remote_host;
#endif

	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t remote_node_ptr = 0;
	errno = 0;
	if (const int call_result = ACS_DSD_ConfigurationHelper::get_node_by_ip(remote_addr_net_order, remote_node_ptr)) {

#if ACS_DSD_CONFIG_DISABLE_PEER_CHECK_FOR_SEA_TEST == 1

		printf("\n  ###### ACS DSD API: Accepting the connection by '%s' only for SEA TEST: call_result == %d ######\n\n", remote_addr_str, call_result);
		remote_host.system_id = acs_dsd::CONFIG_CP_ID_SET_START + 1; //CP1
		remote_host.system_type = acs_dsd::SYSTEM_TYPE_CP;
		memccpy(remote_host.system_name, "CP1", 0, ACS_DSD_ARRAY_SIZE(remote_host.system_name));
		remote_host.system_name[ACS_DSD_ARRAY_SIZE(remote_host.system_name) - 1] = 0;
		memccpy(remote_host.node_name, "CP1A", 0, ACS_DSD_ARRAY_SIZE(remote_host.node_name));
		remote_host.node_name[ACS_DSD_ARRAY_SIZE(remote_host.node_name) - 1] = 0;
		remote_host.side = acs_dsd::NODE_SIDE_A;
		remote_node_ptr = &remote_host;

#else

		stream->close();
		delete stream;

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(const int call_result = ACS_DSD_ConfigurationHelper::get_node_by_ip(remote_addr_net_order, remote_node_ptr)): "
				"error getting node info by ip address (%s) from configuration helper: calling 'get_node_by_ip(...': call_result == %d",
				remote_addr_str, call_result);

		return set_error_info(call_result, errno, "getting node info by ip address (%s) from configuration helper: calling 'get_node_by_ip(...'",
				remote_addr_str);

#endif
	}

	//What's kind of remote node is connected to me?
	if (acs_dsd::SYSTEM_TYPE_AP == remote_node_ptr->system_type) _OCP_transfert_active = false; //It's an AP: deactivate the support for the OCP protocol
	else if (acs_dsd::SYSTEM_TYPE_UNKNOWN == remote_node_ptr->system_type) { //ERROR: cannot retrieve the remote system type
		stream->close();
		delete stream;

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SYSTEM_TYPE_UNKNOWN == remote_node_ptr->system_type): the remote peer node has an unknown system type");

		return set_error_info(acs_dsd::ERR_REMOTE_SYSTEM_TYPE_UNKNOWN, 0, "the remote peer node has an unknown system type");
	} else _OCP_transfert_active = true; //It's a CP/BC: activate the support for the OCP protocol

	//Set the remote node info
	set_remote_node(remote_node_ptr->system_id, remote_node_ptr->system_name, remote_node_ptr->system_type,
			acs_dsd::NODE_STATE_UNDEFINED, remote_node_ptr->node_name, remote_node_ptr->side);

	//Set the remote node ip4 address
	_remote_ip4_address = remote_addr_net_order;

	_stream && _stream->close();
	delete _stream;
	_stream = stream;

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::accept_unix (ACS_DSD_Acceptor * acceptor, int & errno_save) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACS_DSD_LSocketAcceptor * unix_acceptor = dynamic_cast<ACS_DSD_LSocketAcceptor *>(acceptor);

	errno_save = 0;

	if (!unix_acceptor) { //ERROR: Very strange, the acceptor should be a unix acceptor. The library is corrupted!
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(!unix_acceptor): The caller provided an acceptor object that is not a UNIX local socket acceptor: library corrupted");
		return set_error_info(acs_dsd::ERR_WRONG_ACCEPTOR_TYPE, 0, "The caller provided an acceptor object that is not a UNIX local socket acceptor");
	}

	errno = 0;
	ACS_DSD_LSocketStream * stream = new (std::nothrow) ACS_DSD_LSocketStream();
	if (!stream) { //ERROR: Out of memory
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(!stream): system is in out of memory when creating a new UNIX local socket stream");
		return (errno_save = errno, set_error_info(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno, "creating a new UNIX local socket stream"));
	}

	errno = 0;
	if (const int call_result = unix_acceptor->accept(*stream)) { //ERROR: accepting the client
		errno_save = errno;
		delete stream;
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(const int call_result = unix_acceptor->accept(*stream)): error in accepting a client connection: call_result == %d",
				call_result);
		return set_error_info(call_result, errno_save, "accepting a connection");
	}

	//On the unix domain we cannot have any OCP communication
	_OCP_transfert_active = false;

	//In a UNIX domain communication local node and remote node are the same
	_remote_node.reset();
	get_local_node(_remote_node);

	_remote_ip4_address = 0;

	_stream && _stream->close();
	delete _stream;
	_stream = stream;

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::connect_inet (
		const ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED],
		acs_dsd::SystemTypeConstants system_type,
		const char * node_name,
		ACE_Time_Value * timeout,
		int & errno_save
) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	errno_save = 0;

	//Check system_type parameter
	if ((system_type ^ acs_dsd::SYSTEM_TYPE_AP) && (system_type ^ acs_dsd::SYSTEM_TYPE_BC) && (system_type ^ acs_dsd::SYSTEM_TYPE_CP)) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION((system_type ^ acs_dsd::SYSTEM_TYPE_AP) && (system_type ^ acs_dsd::SYSTEM_TYPE_BC) && (system_type ^ acs_dsd::SYSTEM_TYPE_CP)): "
				"bad parameter value: system_type == %d", system_type);
		return set_error_info(acs_dsd::ERR_BAD_PARAMETER_VALUE, 0, "Parameter system_type == %d", system_type);
	}

	errno = 0;
	ACS_DSD_SocketStream * stream = new (std::nothrow) ACS_DSD_SocketStream();
	if (!stream) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(!stream): system is in out of memory when creating a new socket stream");
		return (errno_save = errno, set_error_info(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save, "creating a new socket stream")); //ERROR: Out of memory
	}

	srand(time(0));
	int i = rand() % acs_dsd::CONFIG_NETWORKS_SUPPORTED;
	ACS_DSD_SocketConnector connector;
	int error_results[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};
	char error_result_texts[acs_dsd::CONFIG_NETWORKS_SUPPORTED][LAST_ERROR_TEXT_SIZE] = {{0}};
	int errno_saves[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};
	bool connection_established = false;
	ssize_t bytes_sent = 0;
	const char * node_name_ptr = (node_name ?: "UNKNOWN");
	char address_str[32] = {0};

	for (size_t inet_count = 0; (inet_count < ACS_DSD_ARRAY_SIZE(inet_addresses)); ++inet_count, i = ((i + 1) % acs_dsd::CONFIG_NETWORKS_SUPPORTED)) {
		ACE_INET_Addr addr_to_connect = inet_addresses[i];
		if ((system_type == acs_dsd::SYSTEM_TYPE_BC) || (system_type == acs_dsd::SYSTEM_TYPE_CP))
			addr_to_connect.set_port_number(acs_dsd::CONFIG_CPAP_INET_SOCKET_SAP_PORT);

		addr_to_connect.addr_to_string(address_str, 32);

		errno_saves[i] = errno = 0;
		//I will check that the received addresses are not null (zero) also
		error_results[i] = (inet_addresses[i].get_ip_address() ? connector.connect(*stream, addr_to_connect, timeout) : acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL);
		errno_saves[i] = errno;

		if (error_results[i]) { //ERROR: close the stream and make-up the error message
			if (error_results[i] ^ acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL) {
				stream->close();
				snprintf(error_result_texts[i], ACS_DSD_ARRAY_SIZE(error_result_texts[i]),
						"Internal connector returned with error %d while connecting to '%s' address", error_results[i], address_str);
			} else snprintf(error_result_texts[i], ACS_DSD_ARRAY_SIZE(error_result_texts[i]),
								"Connecting towards an empty or null INET address. Bad information provided by the caller or retrieved from IMM or DSD server");
			error_result_texts[i][ACS_DSD_ARRAY_SIZE(error_result_texts[i]) - 1] = 0;

			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(error_results[i]): ERROR: %s: errno == %d", error_result_texts[i], errno_saves[i]);
		} else { //OK
			//At TCP/IP layer the connection is established successfully, but, in case of CP/BC machine,
			//I have to send the connection indication packet
			if (system_type == acs_dsd::SYSTEM_TYPE_AP) { //For an AP machine all is done
				_OCP_transfert_active = false;
				connection_established = true;
				break;
			}

			//Sending the OCP accept indication packet towards the CP
			errno = 0;
			if ((bytes_sent = send_accept_indication(stream, inet_addresses[i].get_port_number(), timeout)) <= 0) {
				//Error on sending or peer closed the connection
				error_results[i] = acs_dsd::ERR_OCP_SENDING_ACCEPT_INDICATION;
				(bytes_sent < 0) && (errno_saves[i] = errno);

				if (bytes_sent) {
					//char address_str[32] = {0};
					//inet_addresses[i].addr_to_string(address_str, 32);
					snprintf(error_result_texts[i], ACS_DSD_ARRAY_SIZE(error_result_texts[i]),
							"OCP accept-indication message sending error to BC/CP '%s' address '%s'", node_name_ptr, address_str);
				} else snprintf(error_result_texts[i], ACS_DSD_ARRAY_SIZE(error_result_texts[i]),
									"The BC/CP peer '%s' closed the connection while it was receiving the OCP accept indication message", node_name_ptr);
				error_result_texts[i][ACS_DSD_ARRAY_SIZE(error_result_texts[i]) - 1] = 0;

				ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
						"ERR_ASSERTION((bytes_sent = send_accept_indication(stream, inet_addresses[i].get_port_number())) <= 0): "
						"ERROR: %s: errno == %d", error_result_texts[i], errno_saves[i]);

				stream->close();
				continue;
			}

			//Receiving the accept confirmation response
			uint16_t ocp_error_code = 0;
			uint16_t ocp_message_size = 0;

			errno = 0;
			const ssize_t bytes_received = recv_accept_confirmation(stream, ocp_error_code, ocp_message_size, timeout);
			if (bytes_received <= 0) { //Error on receiving or peer closed the connection
				error_results[i] = acs_dsd::ERR_OCP_RECEIVING_ACCEPT_CONFIRMATION;
				(bytes_sent < 0) && (errno_saves[i] = errno);

				if (bytes_received) {
					//char address_str[32] = {0};
					//inet_addresses[i].addr_to_string(address_str, 32);
					snprintf(error_result_texts[i], ACS_DSD_ARRAY_SIZE(error_result_texts[i]),
							"OCP accept-confirmation message receiving error from BC/CP '%s' address '%s'", node_name_ptr, address_str);
				} else snprintf(error_result_texts[i], ACS_DSD_ARRAY_SIZE(error_result_texts[i]),
									"The BC/CP peer '%s' closed the connection while DSD API was receiving the OCP accept confirmation message", node_name_ptr);
				error_result_texts[i][ACS_DSD_ARRAY_SIZE(error_result_texts[i]) - 1] = 0;

				ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
						"ERR_ASSERTION(recv_accept_confirmation(stream, ocp_error_code, ocp_message_size) <= 0): "
						"ERROR: %s: errno == %d", error_result_texts[i], errno_saves[i]);

				stream->close();
				continue;
			}

			//Checking data into the OCP accept confirmation message
			if ((ocp_message_size ^ acs_dsd::CONFIG_OCP_ACCEPT_CONFIRM_MSG_SIZE) || ocp_error_code) {
				error_results[i] = acs_dsd::ERR_OCP_ACCEPT_CONFIRM_BAD_RESPONSE;
				//char address_str[32] = {0};
				//inet_addresses[i].addr_to_string(address_str, 32);
				snprintf(error_result_texts[i], ACS_DSD_ARRAY_SIZE(error_result_texts[i]),
						"The BC/CP machine '%s' at address '%s' has rejected the OCP accept indication; OCP message size == %u (should be %d), OCP message error code == %u (should be 0)",
						node_name_ptr, address_str, ocp_message_size, acs_dsd::CONFIG_OCP_ACCEPT_CONFIRM_MSG_SIZE, ocp_error_code);
				error_result_texts[i][ACS_DSD_ARRAY_SIZE(error_result_texts[i]) - 1] = 0;

				ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
						"ERR_ASSERTION((ocp_message_size ^ acs_dsd::CONFIG_OCP_ACCEPT_CONFIRM_MSG_SIZE) || ocp_error_code): ERROR: %s",
						error_result_texts[i]);

				stream->close();
				continue;
			}

			_OCP_transfert_active = true;
			connection_established = true;
			break;
		}
	}

	if (!connection_established) { //Cannot establish the requested connection. Return with error
		stream->close();
		delete stream;

		//return with the first error not equal to acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL, if any
		size_t i = 0;
		for (i = 0; (i < ACS_DSD_ARRAY_SIZE(error_results)) && (error_results[i] == acs_dsd::ERR_ADDRESS_EMPTY_OR_NULL); ++i) ;
		(i < ACS_DSD_ARRAY_SIZE(error_results)) || (i = 0);

		return set_error_info(error_results[i], errno_save = errno_saves[i], error_result_texts[i]);
	}

	//OK: Connection established successfully
	//I will retrieve information about the remote node reached by the connection
	uint32_t remote_addr_net_order = htonl(inet_addresses[i].get_ip_address());

	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t remote_node_ptr = 0;
	errno = 0;
	if (const int call_result = ACS_DSD_ConfigurationHelper::get_node_by_ip(remote_addr_net_order, remote_node_ptr)) {
		stream->close();
		delete stream;


		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(const int call_result = ACS_DSD_ConfigurationHelper::get_node_by_ip(remote_addr_net_order, remote_node_ptr)): "
				"error getting node info by ip address (%s) from configuration helper: calling 'get_node_by_ip(...': call_result == %d",
				address_str, call_result);

		//char address_str[32] = {0};
		//inet_addresses[i].addr_to_string(address_str, 32);
		return set_error_info(call_result, errno,
				"getting node info by ip address (%s) from configuration helper: calling 'get_node_by_ip(...'", address_str);
	}

	//Set the remote node info
	set_remote_node(remote_node_ptr->system_id, remote_node_ptr->system_name, remote_node_ptr->system_type,
			acs_dsd::NODE_STATE_UNDEFINED, remote_node_ptr->node_name, remote_node_ptr->side);

	_remote_ip4_address = remote_addr_net_order;

	_stream && _stream->close();
	delete _stream;
	_stream = stream;

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

int __CLASS_NAME__::connect_unix (const ACE_UNIX_Addr & unix_address, int & errno_save) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	errno_save = 0;

	errno = 0;
	ACS_DSD_LSocketStream * stream = new (std::nothrow) ACS_DSD_LSocketStream();
	if (!stream) { //ERROR: Out of memory
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(!stream): system is in out of memory when creating a new UNIX local socket stream");
		return (errno_save = errno, set_error_info(acs_dsd::ERR_SYSTEM_OUT_OF_MEMORY, errno_save, "creating a new UNIX local socket stream"));
	}

	errno = 0;
	if (const int call_result = (ACS_DSD_LSocketConnector()).connect(*stream, unix_address)) {
		errno_save = errno;
		delete stream;

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(const int call_result = (ACS_DSD_LSocketConnector()).connect(*stream, unix_address)): "
				"ERROR connecting using the underlying UNIX local connector: call_result == %d", call_result);

		return set_error_info(call_result, errno_save, "connecting using the underlying UNIX local connector");
	}

	//On the unix domain we cannot have any OCP communication
	_OCP_transfert_active = false;

	//In a UNIX domain communication local node and remote node are the same
	_remote_node.reset();
	get_local_node(_remote_node);

	_remote_ip4_address = 0;

	_stream && _stream->close();
	delete _stream;
	_stream = stream;

	return set_error_info(acs_dsd::ERR_NO_ERRORS);
}

ssize_t __CLASS_NAME__::sendf_imp (
		const void * buf,
		size_t size,
		const unsigned * timeout_ms,
		size_t * bytes_transferred,
		int flags) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check if connected. A session should be connected before it can send data
	if (acs_dsd::SESSION_STATE_DISCONNECTED == state()) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(acs_dsd::SESSION_STATE_DISCONNECTED == state()): this session object is disconnected");
		return set_error_info(acs_dsd::ERR_SESSION_DISCONNECTED);
	}

	char sender_memory_stub [sizeof(ACS_DSD_OCPMessageSender) > sizeof(ACS_DSD_MessageSender) ? sizeof(ACS_DSD_OCPMessageSender) : sizeof(ACS_DSD_MessageSender)];

	const ACS_DSD_Sender * sender =
			(_OCP_transfert_active ? reinterpret_cast<ACS_DSD_Sender *>(new (sender_memory_stub) ACS_DSD_OCPMessageSender(_stream))
														 : reinterpret_cast<ACS_DSD_Sender *>(new (sender_memory_stub) ACS_DSD_MessageSender(_stream)));

	errno = 0;
	ssize_t bytes_sent = 0;

	if (timeout_ms) {
		ACE_Time_Value timeout;
		timeout.msec(static_cast<long int>(*timeout_ms));

		bytes_sent = sender ->
#if !defined (ACS_DSD_API_FLAGGED_SEND_DISABLED)
				sendf(buf, size, flags, &timeout, bytes_transferred);
#else
				send(buf, size, &timeout, bytes_transferred);
		static_cast<void>(flags);
#endif
	} else {
		bytes_sent = sender ->
#if !defined (ACS_DSD_API_FLAGGED_SEND_DISABLED)
				sendf(buf, size, flags, 0, bytes_transferred);
#else
				send(buf, size, 0, bytes_transferred);
		static_cast<void>(flags);
#endif
	}

	sender->~ACS_DSD_Sender();

	if (bytes_sent <= 0) {
		if (bytes_sent) ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_sent <= 0): ERROR on sending packet: bytes_sent == %zd, errno == %d", bytes_sent, errno);
		else ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_sent <= 0): WARNING on sending packet: peer closed connection: errno == %d", errno);
		return set_error_info(bytes_sent ?: static_cast<ssize_t>(acs_dsd::ERR_PEER_CLOSED_CONNECTION), errno);
	}

	return (set_error_info(acs_dsd::ERR_NO_ERRORS), bytes_sent);
}
