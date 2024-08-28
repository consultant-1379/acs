#ifndef HEADER_GUARD_CLASS__MsgExchangerService
#define HEADER_GUARD_CLASS__MsgExchangerService MsgExchangerService

#include "ace/Log_Msg.h"
#include "ace/Addr.h"

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_Acceptor.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__MsgExchangerService

class __CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
public:
	enum MsgExchangerPrimitiveConstants {
		PRIM_ID_EXIT	= 0,
		PRIM_ID_GET,
		PRIM_ID_PUT,
		PRIMITIVE_COUNT
	};

	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__(int sap_type, const char * sap_address) : _sap_type(sap_type), _sap_address(sap_address), _acceptor(0), _stream(0) {
		ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::"ACS_DSD_STRINGIZE(__CLASS_NAME__)"(int sap_type, const char * sap_address)");
	}

	//============//
	// Destructor //
	//============//
public:
	virtual ~__CLASS_NAME__() {
		ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::~"ACS_DSD_STRINGIZE(__CLASS_NAME__)"()");
		delete _stream;
		delete _acceptor;
	}

	//===========//
	// Functions //
	//===========//
public:
	int run();

private:
	int create_acceptor();
	int open_acceptor();
	int accept_client(const char * local_sap_address);
	int do_conversation(const char * local_sap_address);
	int get_local_address(char * buf, size_t size);
	inline void print_system_error(int last_error, const char * msg = 0) {
		char buf[1024];
		(msg ? (::std::cerr << "ERROR: " << msg << ": ") : (::std::cerr)) << "SYSTEM ERROR: [" << last_error << "] " << strerror_r(last_error, buf, 1024) << std::endl;
	}

	//Primitive handlers
	int dispatch_primitive_handler(int primitive_id);
	int prim_exit_handler();
	int prim_get_handler();
	int prim_put_handler();

	int send_chunks(long long chunk_count, long long chunk_size_in_kb, long long last_chunk_size_in_kb, unsigned char * send_buffer, double * timing = 0);
	int receive_chunks(long long chunk_count, long long chunk_size_in_kb, long long last_chunk_size_in_kb, unsigned char * buffer, double * timing = 0);

	//========//
	// Fields //
	//========//
private:
	int _sap_type;
	const char * _sap_address;
	ACS_DSD_Acceptor * _acceptor;
	ACS_DSD_IOStream * _stream;
};

#endif // HEADER_GUARD_CLASS__MsgExchangerService
