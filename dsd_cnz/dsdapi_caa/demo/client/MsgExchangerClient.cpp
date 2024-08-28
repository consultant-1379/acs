#include <ctype.h>
#include <cstring>
#include <strings.h>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <regex.h>
#include <arpa/inet.h>

#include <iomanip>

#include "ace/OS_NS_errno.h"
#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"

#include "ACS_DSD_SocketConnector.h"
#include "ACS_DSD_LSocketConnector.h"

#include "MsgExchangerClient.h"

namespace {
	::regex_t compiled_regex_number;
	::regex_t compiled_regex_unit;

	int sscanf_total_and_chunk_sizes(const char * reading_buf, unsigned & total_size, char * total_size_unit, unsigned & chunk_size, char * chunk_size_unit) {
		static ::regex_t * compiled_regex_number_ptr = 0;
		static ::regex_t * compiled_regex_unit_ptr = 0;
		compiled_regex_number_ptr ||
			(::regcomp((compiled_regex_number_ptr = &compiled_regex_number), "[0-9]+", REG_EXTENDED | REG_ICASE),
			 ::regcomp((compiled_regex_unit_ptr = &compiled_regex_unit), "[KMGT]B", REG_EXTENDED | REG_ICASE));

		::regmatch_t pmatchs = {-1};
		unsigned * sizes[] = {&total_size, &chunk_size};
		char * units[] = {total_size_unit, chunk_size_unit};
		const char * in_buf = reading_buf;

		for (int i = 0; i < 2; ++i) {
			if (::regexec(compiled_regex_number_ptr, in_buf, 1, &pmatchs, 0) || (pmatchs.rm_so == -1) || (pmatchs.rm_so == pmatchs.rm_eo)) return -1; //ERROR: match error
			*sizes[i & 0x01] = ::strtoul(in_buf + pmatchs.rm_so, 0, 10);
			if (::regexec(compiled_regex_unit_ptr, in_buf += pmatchs.rm_eo, 1, &pmatchs, 0) || (pmatchs.rm_so == -1)) return -1; //ERROR: match error
			*units[i & 0x01] = (pmatchs.rm_eo > pmatchs.rm_so) ? ::toupper(in_buf[pmatchs.rm_so]) : 'K';
			in_buf += pmatchs.rm_eo;
		}

		return 0;
	}

	int serialize_totals_and_chunks(unsigned char * prim_buffer, unsigned total_size, char total_size_unit, unsigned chunk_size, char chunk_size_unit) {
		uint32_t val = htonl(total_size);
		*reinterpret_cast<uint32_t *>(prim_buffer) = val;
		prim_buffer[4] = static_cast<unsigned char>(::toupper(total_size_unit));
		val = htonl(chunk_size);
		*reinterpret_cast<uint32_t *>(prim_buffer + 5) = val;
		prim_buffer[9] = static_cast<unsigned char>(::toupper(chunk_size_unit));

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


const char * __CLASS_NAME__::standard_prompt = "\n> ";
const char * __CLASS_NAME__::user_commands[__CLASS_NAME__::USER_COMMAND_COUNT] = {
	"EXIT",
	"GET",
	"HELP",
	"PUT"
};

int __CLASS_NAME__::run() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::run()");

	if (create_connector()) return __LINE__;

	int result = 0;

	connect_to_service() ? result = __LINE__ : do_conversation();

	return result;
}

int __CLASS_NAME__::create_connector() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::create_connector()");

	int result = 0;

	switch (_sap_type) {
	case SAP_TYPE_SOCK: _connector = new (::std::nothrow) ACS_DSD_SocketConnector(); break;
	case SAP_TYPE_LSOCK: _connector = new (::std::nothrow) ACS_DSD_LSocketConnector(); break;
	default:
		::std::cerr << "WARNING: Unknown service type (" << _sap_type << ")." << ::std::endl;
		result = -1;
		break;
	}

	return (result || (!_connector && (::std::cerr << "ERROR: Cannot create the client's connector object: probably the system is out of memory!" << ::std::endl))) ? -1 : 0;
}

int __CLASS_NAME__::connect_to_service() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::connect_to_service()");

	::std::cout << "Connecting to the service '" << _sap_address << "'..." << ::std::flush;

	if (!_stream) {
		(_sap_type ^ SAP_TYPE_LSOCK) ? _stream = new (::std::nothrow) ACS_DSD_SocketStream()
																 : _stream = new (::std::nothrow) ACS_DSD_LSocketStream();
		if (!_stream) { // ERROR: Out of memory
			::std::cerr << "KO!\nERROR: Cannot create the client's I/O communication stream object: probably the system is out of memory!" << ::std::endl;
			return -1;
		}
	}

	int result = (_sap_type ^ SAP_TYPE_LSOCK) ? _connector->connect(*_stream, ACE_INET_Addr(_sap_address))	// INET socket type connector
																						: _connector->connect(*_stream, ACE_UNIX_Addr(_sap_address));	// UNIX socket type connector
	result ? (::std::cerr << "KO!" << ::std::endl << (result < 0 ? "ERROR" : "WARNING") << ": cannot connect to the service on the address '" << _sap_address << "': ")
				 : (::std::cerr << "OK!" << ::std::endl);
	if (result < 0) print_system_error(ACE_OS::last_error()); //ERROR
	else if (result > 0) ::std::cerr << "the connect(...) method returned the warning code " << result << ". Check the code for more info." << ::std::endl; //WARNING

	return result & -1;
}

int __CLASS_NAME__::do_conversation() {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::do_conversation()");

	::std::cout << "Connection established with the service.";

	int return_code = 0;
	int command_id = 0;
	char user_input[4096];

	do { dispatch_user_command_handler(command_id = read_user_command_line(user_input, 4096), user_input); }
	while (command_id ^ USER_COMMAND_EXIT);

	_stream->close();

	return return_code;
}

int __CLASS_NAME__::read_user_command_line(char * user_input, size_t size) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::read_user_command_line(char * user_input, size_t size)");

	::std::cout << standard_prompt;
	::std::cin >> ::std::setw(size) >> user_input;

	int command_id = 0;

	while ((command_id < USER_COMMAND_COUNT) && ::strcasecmp(user_commands[command_id], user_input)) ++command_id;

	return (command_id < USER_COMMAND_COUNT) ? command_id : USER_COMMAND_UNKNOWN;
}

int __CLASS_NAME__::dispatch_user_command_handler(int command_id, const char * command) {
	static int (MsgExchangerClient::* (user_command_handlers[USER_COMMAND_COUNT]))(const char *) = {
		&MsgExchangerClient::user_command_exit_handler,
		&MsgExchangerClient::user_command_get_handler,
		&MsgExchangerClient::user_command_help_handler,
		&MsgExchangerClient::user_command_put_handler
	};

	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::dispatch_user_command_handler(int command_id, const char * command)");

	return command_id >= 0 ? (this->*(user_command_handlers[command_id]))(command) : user_command_unknown_handler(command);
}

int __CLASS_NAME__::user_command_unknown_handler(const char * command) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::user_command_unknown_handler(const char * command)");

	::std::cout << "  WARNING: Unknow user command '" << command << "'." << std::endl;
	int unused = ::scanf("%*[^\n]%*c");
	ACE_UNUSED_ARG(unused);

	return 0;
}

int __CLASS_NAME__::user_command_exit_handler(const char * command) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::user_command_exit_handler(const char * command)");

	int unused = ::scanf("%*[^\n]%*c");
	ACE_UNUSED_ARG(unused);

	unsigned char buffer[] = {PRIM_ID_EXIT};
	size_t bytes_transferred = 0;
	int return_code = 0;

	_stream->send_n(buffer, 1, 0, &bytes_transferred);

	if (bytes_transferred ^ 1) {
		return_code = -1;
		::std::cerr << "  ERROR: cannot notify command '" << command << "' to server: ";
		print_system_error(ACE_OS::last_error());
	} else ::std::cout << ::std::endl;

	return return_code;
}

int __CLASS_NAME__::user_command_get_handler(const char * command) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::user_command_get_handler(const char * command)");

	int c = 0;
	int result_code = 0;
	unsigned char * receiver_buffer = 0;

	do {
		if (((c = ::std::cin.peek()) == ::std::istream::traits_type::eof()) || (c == '\n') || (c == '\r')) { //WARNING: Bad command syntax
			::std::cerr << "  WARNING: Bad command syntax: use 'help get' for more information!" << ::std::endl;
			result_code = -1;
			break;
		}

		char in_buffer[512];
		::std::cin.get(in_buffer, sizeof(in_buffer)/sizeof(*in_buffer));

		unsigned total_size = 0;
		unsigned chunk_size = 0;
		char total_size_unit;
		char chunk_size_unit;

		if (sscanf_total_and_chunk_sizes(in_buffer, total_size, &total_size_unit, chunk_size, &chunk_size_unit)) { //ERROR: reading values: bad format.
			::std::cerr << "  WARNING: Bad command syntax: use 'help get' for more information!" << ::std::endl;
			result_code = -1;
			break;
		}

		long long total_size_in_kb = convert_value_in_kb(total_size, total_size_unit);
		long long chunk_size_in_kb = convert_value_in_kb(chunk_size, chunk_size_unit);

		//Try to allocate the receiver buffer.
		receiver_buffer = new (::std::nothrow) unsigned char [chunk_size_in_kb * 1024];
		if (!receiver_buffer) {
			::std::cerr << "  ERROR: Cannot create the receiver buffer in memory: probably the buffer size is to large and the system is out of memory!" << ::std::endl;
			result_code = -1;
			break;
		}

		const ::lldiv_t div = lldiv(total_size_in_kb, chunk_size_in_kb);

		long long chunk_count = div.quot;
		long long last_chunk_size_in_kb = div.rem;

//		::std::cout << "I would like to get " << total_size << total_size_unit << " using chunks of size " << chunk_size << chunk_size_unit << ::std::endl;
//		::std::cout << "total_size_in_kb == " << total_size_in_kb << ::std::endl
//								<< "chunk_size_in_kb == " << chunk_size_in_kb << ::std::endl
//								<< "chunk_count == " << chunk_count << ::std::endl
//								<< "last_chunk_size_in_kb == " << last_chunk_size_in_kb << ::std::endl;

		unsigned char prim_buffer[11] = {PRIM_ID_GET};
		serialize_totals_and_chunks(prim_buffer + 1, total_size, total_size_unit, chunk_size, chunk_size_unit);

//		::std::cout << "prim_buffer ==";
//		for (int i = 0; i < 11; ::printf(" %02X", prim_buffer[i++])) ;
//		::std::cout << ::std::endl;

		//send primitive GET
		size_t bytes_transferred = 0;
		ssize_t call_result = 0;

		if ((call_result =_stream->send_n(prim_buffer, 11, 0, &bytes_transferred)) <= 0) {
			result_code = -1;
			::std::cerr << "  " << (call_result ? "ERROR" : "WARNING") << ": '_stream->send_n(...' failed: Problems communicating with the server" << (call_result ? ": " : "!");
			if (call_result) print_system_error(ACE_OS::last_error()); else ::std::cerr << ::std::endl;
			break;
		}

		double time_elapsed = 0.0;
		receive_chunks(chunk_count, chunk_size_in_kb, last_chunk_size_in_kb, receiver_buffer, &time_elapsed) || ::printf("  I have received %u%c in %f Kb/s.\n", total_size, total_size_unit, (total_size_in_kb/(time_elapsed ?: time_elapsed + 1)));
	} while (0);

	delete[] receiver_buffer;

	int unused = ::scanf("%*[^\n]%*c");
	ACE_UNUSED_ARG(unused);

	return result_code;
}

int __CLASS_NAME__::user_command_help_handler(const char * command) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::user_command_help_handler(const char * command)");

	::std::cout << "  WARNING: The help command is not implemented. That's only a demo! Just believe it :-)" << ::std::endl;

	int unused = ::scanf("%*[^\n]%*c");
	ACE_UNUSED_ARG(unused);

	return 0;
}

int __CLASS_NAME__::user_command_put_handler(const char * command) {
	ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::user_command_put_handler(const char * command)");

	int c = 0;
	int result_code = 0;
	unsigned char * send_buffer = 0;

	do {
		if (((c = ::std::cin.peek()) == ::std::istream::traits_type::eof()) || (c == '\n') || (c == '\r')) { //WARNING: Bad command syntax
			::std::cerr << "  WARNING: Bad command syntax: use 'help put' for more information!" << ::std::endl;
			result_code = -1;
			break;
		}

		char in_buffer[512];
		::std::cin.get(in_buffer, sizeof(in_buffer)/sizeof(*in_buffer));

		unsigned total_size = 0;
		unsigned chunk_size = 0;
		char total_size_unit;
		char chunk_size_unit;

		if (sscanf_total_and_chunk_sizes(in_buffer, total_size, &total_size_unit, chunk_size, &chunk_size_unit)) { //ERROR: reading values: bad format.
			::std::cerr << "  WARNING: Bad command syntax: use 'help put' for more information!" << ::std::endl;
			result_code = -1;
			break;
		}

		long long total_size_in_kb = convert_value_in_kb(total_size, total_size_unit);
		long long chunk_size_in_kb = convert_value_in_kb(chunk_size, chunk_size_unit);

		//Try to allocate the send buffer.
		send_buffer = new (::std::nothrow) unsigned char [chunk_size_in_kb * 1024];
		if (!send_buffer) {
			::std::cerr << "  ERROR: Cannot create the send buffer in memory: probably the buffer size is to large and the system is out of memory!" << ::std::endl;
			result_code = -1;
			break;
		}

		const ::lldiv_t div = lldiv(total_size_in_kb, chunk_size_in_kb);

		long long chunk_count = div.quot;
		long long last_chunk_size_in_kb = div.rem;

//		::std::cout << "I would like to put " << total_size << total_size_unit << " using chunks of size " << chunk_size << chunk_size_unit << ::std::endl;
//		::std::cout << "total_size_in_kb == " << total_size_in_kb << ::std::endl
//								<< "chunk_size_in_kb == " << chunk_size_in_kb << ::std::endl
//								<< "chunk_count == " << chunk_count << ::std::endl
//								<< "last_chunk_size_in_kb == " << last_chunk_size_in_kb << ::std::endl;

		unsigned char prim_buffer[11] = {PRIM_ID_PUT};
		serialize_totals_and_chunks(prim_buffer + 1, total_size, total_size_unit, chunk_size, chunk_size_unit);

//		::std::cout << "prim_buffer ==";
//		for (int i = 0; i < 11; ::printf(" %02X", prim_buffer[i++])) ;
//		::std::cout << ::std::endl;

		//send primitive PUT
		size_t bytes_transferred = 0;
		ssize_t call_result = 0;

		if ((call_result =_stream->send_n(prim_buffer, 11, 0, &bytes_transferred)) <= 0) {
			result_code = -1;
			if (call_result) print_system_error(ACE_OS::last_error(), "'_stream->send_n(...' failed: Problems communicating with the server");
			else ::std::cerr << "  WARNING: It seems the server has disconnected!" << ::std::endl;
			break;
		}

		double time_elapsed = 0.0;
		send_chunks(chunk_count, chunk_size_in_kb, last_chunk_size_in_kb, send_buffer, &time_elapsed) || ::printf("  I have sent %u%c in %f Kb/s.\n", total_size, total_size_unit, (total_size_in_kb/(time_elapsed ?: time_elapsed + 1)));
	} while (0);

	delete[] send_buffer;

	int unused = ::scanf("%*[^\n]%*c");
	ACE_UNUSED_ARG(unused);

	return result_code;
}

int __CLASS_NAME__::receive_chunks(long long chunk_count, long long chunk_size_in_kb, long long last_chunk_size_in_kb, unsigned char * receiver_buffer, double * timing) {
	int result_code = 0;
	size_t bytes_transferred = 0;
	long long size = chunk_size_in_kb * 1024;
	ssize_t call_result = 0;
	unsigned chunk_received = 0;

	time_t time_start = ::time(0);
	while (chunk_count-- > 0) {
		call_result = _stream->recv_n(receiver_buffer, size, 0, &bytes_transferred);
		if (call_result > 0) ::printf("  Chunks received: %u\r", ++chunk_received);
		else {
			result_code = -1;
			if (call_result < 0) { //ERROR
				::std::cerr << "  ERROR: receive failure: ";
				print_system_error(ACE_OS::last_error());
			} else ::std::cerr << "  WARNING: the server seems disconnected!" << ::std::endl; //Peer seems disconnected
			break;
		}
	}
	if (!result_code && (last_chunk_size_in_kb > 0)) {
		call_result = _stream->recv_n(receiver_buffer, last_chunk_size_in_kb * 1024, 0, &bytes_transferred);
		if (call_result > 0) ::printf("  Chunks received: %u\r", ++chunk_received);
		else {
			result_code = -1;
			if (call_result < 0) { //ERROR
				::std::cerr << "  ERROR: receive failure on the last chunk: ";
				print_system_error(ACE_OS::last_error());
			} else ::std::cerr << "  WARNING: the server seems disconnected!" << ::std::endl; //Peer seems disconnected
		}
	}
	time_t time_end = ::time(0);
	result_code ? (::std::cerr << "  I suggest you to give the exit command!" << ::std::endl)
							: ((::std::cout << ::std::endl), (timing && (*timing = ::difftime(time_end, time_start))));

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
			else ::std::cerr << "  WARNING: it seems the server has disconnected!" << ::std::endl;
			break;
		}
	}
	if (!result_code && (last_chunk_size_in_kb > 0)) {
		call_result = _stream->send_n(send_buffer, last_chunk_size_in_kb * 1024, 0, &bytes_transferred);
		if (call_result > 0) ::printf("  Chunks sent: %u\r", ++chunk_sent);
		else {
			result_code = -1;
			if (call_result < 0) print_system_error(ACE_OS::last_error(), "send failure on the last chunk");
			else ::std::cerr << "  WARNING: it seems the server has disconnected!" << ::std::endl;
		}
	}
	time_t time_end = ::time(0);
	result_code || ((::std::cout << ::std::endl), (timing && (*timing = ::difftime(time_end, time_start))));

	return result_code;
}
