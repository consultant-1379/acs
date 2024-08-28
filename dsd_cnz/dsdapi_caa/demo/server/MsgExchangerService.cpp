#include <cstring>
#include <ctime>

#include "ace/OS_NS_errno.h"
#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"

#include "ACS_DSD_SocketAcceptor.h"
#include "ACS_DSD_LSocketAcceptor.h"

#include "demo-server-macros.h"
#include "MsgExchangerService.h"

namespace {
	int deserialize_totals_and_chunks(const unsigned char * prim_buffer, unsigned & total_size, char & total_size_unit, unsigned & chunk_size, char & chunk_size_unit) {
		uint32_t val = ntohl(*reinterpret_cast<const uint32_t *>(prim_buffer));
		total_size = val;
		total_size_unit = prim_buffer[4];
		val = ntohl(*reinterpret_cast<const uint32_t *>(prim_buffer + 5));
		chunk_size = val;
		chunk_size_unit = prim_buffer[9];

		return 0;
	}

	long long convert_value_in_kb(long long value, char value_unit) {
		switch (::toupper(value_unit)) {
		case 'T': value *= 1024;
		case 'G': value *= 1024;
		case 'M': value *= 1024;
		}
		return value;
	}
}

int __CLASS_NAME__::create_acceptor() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::create_acceptor()");

	int result = 0;

	switch (_sap_type) {
	case SAP_TYPE_SOCK: _acceptor = new (::std::nothrow) ACS_DSD_SocketAcceptor(); break;
	case SAP_TYPE_LSOCK: _acceptor = new (::std::nothrow) ACS_DSD_LSocketAcceptor(); break;
	default:
		::std::cerr << "WARNING: Unknown service type (" << _sap_type << ")." << ::std::endl;
		result = -1;
		break;
	}

	return (result || (!_acceptor && (::std::cerr << "ERROR: Cannot create the service's acceptor object: probably the system is out of memory!" << ::std::endl))) ? -1 : 0;
}

int __CLASS_NAME__::open_acceptor() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::open_acceptor()");

	int result = (_sap_type ^ SAP_TYPE_LSOCK) ? _acceptor->open(ACE_INET_Addr(_sap_address))	// INET socket type acceptor
																						: _acceptor->open(ACE_UNIX_Addr(_sap_address));	// UNIX socket type acceptor

	result && (::std::cerr << (result < 0 ? "ERROR" : "WARNING") << ": cannot open the acceptor on the address '" << _sap_address << "': ");

	if (result < 0) print_system_error(ACE_OS::last_error()); //ERROR
	else if (result > 0) ::std::cerr << "the acceptor is not closed!" << ::std::endl; //WARNING

	return result & -1;
}

int __CLASS_NAME__::accept_client(const char * local_sap_address) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::accept_client(const char * local_sap_address)");

	::std::cout << "Message Exchanger Service is waiting a client connection..." << ::std::flush;

	if (!_stream) {
		(_sap_type ^ SAP_TYPE_LSOCK) ? _stream = new (::std::nothrow) ACS_DSD_SocketStream()
																 : _stream = new (::std::nothrow) ACS_DSD_LSocketStream();
		if (!_stream) { // ERROR: Out of memory
			::std::cerr << "\nERROR: Cannot create the service's I/O communication stream object: probably the system is out of memory!" << ::std::endl;
			return -1;
		}
	}

	int result = _acceptor->accept(*_stream);
	result ? (::std::cerr << "KO!" << ::std::endl << (result < 0 ? "ERROR" : "WARNING") << ": cannot accept a client connection on the address '" << local_sap_address << "': ")
				 : (::std::cerr << "OK!" << ::std::endl);
	if (result < 0) print_system_error(ACE_OS::last_error()); //ERROR
	else if (result > 0) ::std::cerr << " the accept(...) method returned the warning code " << result << ". Check the code for more info." << ::std::endl; //WARNING

	return result & -1;
}

int __CLASS_NAME__::do_conversation(const char * local_sap_address) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::do_conversation(const char * local_sap_address)");

	::std::cout << "A client established a connection." << ::std::flush;

	ACE_INET_Addr inet_addr;
	ACE_UNIX_Addr unix_addr;
	char peer_address[1024];

	ssize_t call_result = 0;
	int return_code = 0;

	*peer_address = '\0';
	if ((call_result = (_sap_type ^ SAP_TYPE_LSOCK) ? _stream->get_remote_addr(inet_addr) : _stream->get_remote_addr(unix_addr))) { //WARNING: getting client address
		::std::cout << ::std::endl;
		print_system_error(ACE_OS::last_error(), "cannot get client address identification");
		::std::cerr << "That couldn't be a problem so I will not drop the communication and I will try to follow the client conversation." << ::std::endl;
	} else {
		(_sap_type ^ SAP_TYPE_LSOCK) ? inet_addr.addr_to_string(peer_address, 1024) : unix_addr.addr_to_string(peer_address, 1024);
		::std::cout << " The client is incoming from '" << peer_address << "'." << ::std::endl;
	}

	size_t bytes_transferred = 0;
	unsigned char primitive_id = 0;

	do {
		call_result = _stream->recv_n(&primitive_id, 1, 0, &bytes_transferred);
		if (call_result < 0) { //ERROR: receive system error
			print_system_error(ACE_OS::last_error(), "problems on receiving command primitive from client");
			::std::cerr << "Dropping the communication with the client! I'm sorry!" << ::std::endl;
			break;
		} else if (call_result == 0) { // The peer (client) closed the connection
			print_system_error(ACE_OS::last_error(), "the peer (client) closed the connection.");
			::std::cout << "Bye bye!" << ::std::endl << ::std::endl;
			break;
		}

		if (static_cast<int>(primitive_id) >= PRIMITIVE_COUNT) {
			::std::cerr << "Client sent an unknown primitive code (" << static_cast<int>(primitive_id) << "). Dropping the communication with the client! I'm sorry!" << ::std::endl;
			break;
		}

		if (dispatch_primitive_handler(primitive_id) < 0) break;
	} while (primitive_id ^ PRIM_ID_EXIT);

	_stream->close();

	return return_code;
}

int __CLASS_NAME__::get_local_address(char * buf, size_t size) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::get_local_address(char * buf, size_t size)");

	if (buf && size) {
		ACE_INET_Addr inet_addr;
		ACE_UNIX_Addr unix_addr;

		int result = (_sap_type ^ SAP_TYPE_LSOCK) ? _acceptor->get_local_addr(inet_addr) : _acceptor->get_local_addr(unix_addr);
		if (result) { //ERROR
			print_system_error(ACE_OS::last_error(), "cannot get the SAP address information from the system");
			return -1;
		}

		*buf = 0;
		(_sap_type ^ SAP_TYPE_LSOCK) ? inet_addr.addr_to_string(buf, size) : unix_addr.addr_to_string(buf, size);
	}

	return 0;
}

int __CLASS_NAME__::run() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::run()");

	if (create_acceptor() || open_acceptor()) return __LINE__;

	int result = 0;
	char local_sap[1024];

	if (get_local_address(local_sap, 1024)) result = __LINE__; // ERROR: getting sap local address.
	else { //OK
		::std::cout << "Message Exchanger Service ready! Socket type == " << (_sap_type ^ SAP_TYPE_LSOCK ? "Standard INET" : "UNIX Domain") << ". SAP address == '" << local_sap << "'." << ::std::endl;
		while (!accept_client(local_sap) || (result = __LINE__, 0)) do_conversation(local_sap);
	}

	_acceptor->close();
	_acceptor->remove();

	return result;
}

int __CLASS_NAME__::dispatch_primitive_handler(int primitive_id) {
	static int (MsgExchangerService::* (primitive_handlers[PRIMITIVE_COUNT]))() = {
		&MsgExchangerService::prim_exit_handler,
		&MsgExchangerService::prim_get_handler,
		&MsgExchangerService::prim_put_handler
	};

	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::dispatch_primitive_handler(int primitive_id)");

	return (this->*(primitive_handlers[primitive_id]))();
}

int __CLASS_NAME__::prim_exit_handler() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::prim_00_exit_handler()");

	::std::cout << "Received primitive [EXIT]: I will stop and close the connection with the client!" << ::std::endl;

	return 0;
}

int __CLASS_NAME__::prim_get_handler() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::prim_get_handler()");

	::std::cout << "Received primitive [GET]!" << ::std::endl;

	int result_code = 0;
	ssize_t call_result = 0;
	unsigned char prim_received_buffer[10];
	size_t bytes_transferred = 0;

	do {
		call_result = _stream->recv_n(prim_received_buffer, 10, 0, &bytes_transferred);
		if (call_result <= 0) {
			result_code = -1;
			if (call_result < 0) print_system_error(ACE_OS::last_error(), "Problems while receving primitive GET info");
			else ::std::cerr << "  WARNING: It seems the client has disconnected. I will close this session." << ::std::endl;
			break;
		}
		if (bytes_transferred ^ 10) {
			result_code = -1;
			::std::cerr << "  WARNING: Mistake on primitive received size! I will close this session." << ::std::endl;
			break;
		}

		unsigned total_size = 0;
		char total_size_unit = 0;
		unsigned chunk_size = 0;
		char chunk_size_unit = 0;

		deserialize_totals_and_chunks(prim_received_buffer, total_size, total_size_unit, chunk_size, chunk_size_unit);
		::printf("  The client requested to download %u%cB of bulk data using chunks of size %u%cB.\n", total_size, total_size_unit, chunk_size, chunk_size_unit);

		long long total_size_in_kb = convert_value_in_kb(total_size, total_size_unit);
		long long chunk_size_in_kb = convert_value_in_kb(chunk_size, chunk_size_unit);
		const ::lldiv_t div = lldiv(total_size_in_kb, chunk_size_in_kb);
		long long chunk_count = div.quot;
		long long last_chunk_size_in_kb = div.rem;

		//Try to allocate the send buffer
		unsigned char * send_buffer = new (::std::nothrow) unsigned char [chunk_size_in_kb * 1024];
		if (!send_buffer) {
			::std::cerr << "  ERROR: Cannot create the send buffer in memory: probably the buffer size is to large and the system is out of memory!" << ::std::endl;
			result_code = -1;
			break;
		}

		double time_elapsed = 0.0;
		send_chunks(chunk_count, chunk_size_in_kb, last_chunk_size_in_kb, send_buffer, &time_elapsed) || ::printf("  I heve sent %u%c in %f Kb/s.\n", total_size, total_size_unit, (total_size_in_kb/(time_elapsed ?: time_elapsed + 1)));

		delete[] send_buffer;
	} while (0);

	return result_code;
}

int __CLASS_NAME__::prim_put_handler() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::prim_put_handler()");

	::std::cout << "Received primitive [PUT]!" << ::std::endl;

	int result_code = 0;
	ssize_t call_result = 0;
	unsigned char prim_received_buffer[10];
	size_t bytes_transferred = 0;

	do {
		call_result = _stream->recv_n(prim_received_buffer, 10, 0, &bytes_transferred);
		if (call_result <= 0) {
			result_code = -1;
			if (call_result < 0) print_system_error(ACE_OS::last_error(), "Problems while receving primitive PUT info");
			else ::std::cerr << "  WARNING: It seems the client has disconnected. I will close this session." << ::std::endl;
			break;
		}
		if (bytes_transferred ^ 10) {
			result_code = -1;
			::std::cerr << "  WARNING: Mistake on primitive received size! I will close this session." << ::std::endl;
			break;
		}

		unsigned total_size = 0;
		char total_size_unit = 0;
		unsigned chunk_size = 0;
		char chunk_size_unit = 0;

		deserialize_totals_and_chunks(prim_received_buffer, total_size, total_size_unit, chunk_size, chunk_size_unit);
		::printf("  The client requested to upload %u%cB of bulk data using chunks of size %u%cB.\n", total_size, total_size_unit, chunk_size, chunk_size_unit);

		long long total_size_in_kb = convert_value_in_kb(total_size, total_size_unit);
		long long chunk_size_in_kb = convert_value_in_kb(chunk_size, chunk_size_unit);
		const ::lldiv_t div = lldiv(total_size_in_kb, chunk_size_in_kb);
		long long chunk_count = div.quot;
		long long last_chunk_size_in_kb = div.rem;

		//Try to allocate the receiver buffer
		unsigned char * receiver_buffer = new (::std::nothrow) unsigned char [chunk_size_in_kb * 1024];
		if (!receiver_buffer) {
			::std::cerr << "  ERROR: Cannot create the receiver buffer in memory: probably the buffer size is to large and the system is out of memory!" << ::std::endl;
			result_code = -1;
			break;
		}

		double time_elapsed = 0.0;
		receive_chunks(chunk_count, chunk_size_in_kb, last_chunk_size_in_kb, receiver_buffer, &time_elapsed) || ::printf("  I have received %u%c in %f Kb/s.\n", total_size, total_size_unit, (total_size_in_kb/(time_elapsed ?: time_elapsed + 1)));

		delete[] receiver_buffer;
	} while (0);

	return result_code;
}

int __CLASS_NAME__::send_chunks(long long chunk_count, long long chunk_size_in_kb, long long last_chunk_size_in_kb, unsigned char * send_buffer, double * timing) {
	int result_code = 0;
	size_t bytes_transferred = 0;
	long long size = chunk_size_in_kb * 1024;
	ssize_t call_result = 0;
	unsigned chunk_sent = 0;

	time_t time_start = ::time(0);
	while (chunk_count-- > 0) {
		call_result = _stream->send_n(send_buffer, size, 0, &bytes_transferred);
		if (call_result > 0) ::printf("  Chunks sent: %u\r", ++chunk_sent);
		else {
			result_code = -1;
			if (call_result < 0) print_system_error(ACE_OS::last_error(), "send failure");
			else ::std::cerr << "  WARNING: it seems the client has disconnected!" << ::std::endl;
			break;
		}
	}
	if (!result_code && (last_chunk_size_in_kb > 0)) {
		call_result = _stream->send_n(send_buffer, last_chunk_size_in_kb * 1024, 0, &bytes_transferred);
		if (call_result > 0) ::printf("  Chunks sent: %u\r", ++chunk_sent);
		else {
			result_code = -1;
			if (call_result < 0) print_system_error(ACE_OS::last_error(), "send failure on the last chunk");
			else ::std::cerr << "  WARNING: it seems the client has disconnected!" << ::std::endl;
		}
	}
	time_t time_end = ::time(0);
	result_code || ((::std::cout << ::std::endl), (timing && (*timing = ::difftime(time_end, time_start))));

	return result_code;
}

int __CLASS_NAME__::receive_chunks(long long chunk_count, long long chunk_size_in_kb, long long last_chunk_size_in_kb, unsigned char * buffer, double * timing) {
	int result_code = 0;
	size_t bytes_transferred = 0;
	long long size = chunk_size_in_kb * 1024;
	ssize_t call_result = 0;
	unsigned chunk_received = 0;

	time_t time_start = ::time(0);
	while (chunk_count-- > 0) {
		call_result = _stream->recv_n(buffer, size, 0, &bytes_transferred);
		if (call_result > 0) ::printf("  Chunks received: %u\r", ++chunk_received);
		else {
			result_code = -1;
			if (call_result < 0) print_system_error(ACE_OS::last_error(), "receive failure");
			else ::std::cerr << "  WARNING: it seems the client has disconnected!" << ::std::endl;
			break;
		}
	}
	if (!result_code && (last_chunk_size_in_kb > 0)) {
		call_result = _stream->recv_n(buffer, last_chunk_size_in_kb * 1024, 0, &bytes_transferred);
		if (call_result > 0) ::printf("  Chunks received: %u\r", ++chunk_received);
		else {
			result_code = -1;
			if (call_result < 0) print_system_error(ACE_OS::last_error(), "receive failure on the last chunk");
			else ::std::cerr << "  WARNING: it seems the client has disconnected!" << ::std::endl;
		}
	}
	time_t time_end = ::time(0);
	result_code || ((::std::cout << ::std::endl), (timing && (*timing = ::difftime(time_end, time_start))));

	return result_code;
}
