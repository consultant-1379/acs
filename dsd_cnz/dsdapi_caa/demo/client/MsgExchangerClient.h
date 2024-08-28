#ifndef HEADER_GUARD_CLASS__MsgExchangerClient
#define HEADER_GUARD_CLASS__MsgExchangerClient MsgExchangerClient

#include "ace/Addr.h"

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_Connector.h"

#include "demo-client-macros.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__MsgExchangerClient

class __CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
public:
	enum MsgExchangerUserCommandConstants {
		USER_COMMAND_UNKNOWN	= -1,
		USER_COMMAND_EXIT			= 0,
		USER_COMMAND_GET,
		USER_COMMAND_HELP,
		USER_COMMAND_PUT,
		USER_COMMAND_COUNT
	};

	enum MsgExchangerUserPrimitiveConstants {
		PRIM_ID_UNKNOWN	= -1,
		PRIM_ID_EXIT		= 0,
		PRIM_ID_GET,
		PRIM_ID_PUT,
		PRIMITIVE_COUNT
	};

	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__(int sap_type, const char * sap_address) : _sap_type(sap_type), _sap_address(sap_address), _connector(0), _stream(0) {
		ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::"ACS_DSD_STRINGIZE(__CLASS_NAME__)"(int sap_type, const char * sap_address)");
	}

	//============//
	// Destructor //
	//============//
public:
	virtual ~__CLASS_NAME__() {
		ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::~"ACS_DSD_STRINGIZE(__CLASS_NAME__)"()");
		delete _stream;
		delete _connector;
	}

	//===========//
	// Functions //
	//===========//
public:
	int run();

private:
	int create_connector();
	int connect_to_service();
	int do_conversation();

	inline void print_system_error(int last_error, const char * msg = 0) {
		char buf[1024];
		(msg ? (::std::cerr << "ERROR: " << msg << ": ") : (::std::cerr)) << "SYSTEM ERROR: [" << last_error << "] " << ::strerror_r(last_error, buf, 1024) << std::endl;
	}

	int read_user_command_line(char * user_input, size_t size);

	//Primitive handlers
	int dispatch_user_command_handler(int primitive_id, const char * command);

	int user_command_unknown_handler(const char * command);
	int user_command_exit_handler(const char * command);
	int user_command_get_handler(const char * command);
	int user_command_help_handler(const char * command);
	int user_command_put_handler(const char * command);

	int receive_chunks(long long chunk_count, long long chunk_size_in_kb, long long last_chunk_size, unsigned char * receiver_buffer, double * timing = 0);
	int send_chunks(long long chunk_count, long long chunk_size_in_kb, long long last_chunk_size_in_kb, unsigned char * send_buffer, double * timing = 0);

	//========//
	// Fields //
	//========//
private:
	int _sap_type;
	const char * _sap_address;
	ACS_DSD_Connector * _connector;
	ACS_DSD_IOStream * _stream;

	static const char * standard_prompt;
	static const char * user_commands[USER_COMMAND_COUNT];
};

#endif // HEADER_GUARD_CLASS__MsgExchangerClient
