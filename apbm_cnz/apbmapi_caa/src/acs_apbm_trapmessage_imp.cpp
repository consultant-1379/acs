#include <string.h>

#include "acs_apbm_trace.h"
#include "acs_apbm_trapmessage_imp.h"

ACS_APBM_TRACE_DEFINE(acs_apbm_trapmessage);

__CLASS_NAME__::__CLASS_NAME__ () : _OID(-1), _values(), _message(0), _message_length(0) {}

__CLASS_NAME__::__CLASS_NAME__ (const __CLASS_NAME__ & /*rhs*/) : _OID(-1), _values(), _message(0), _message_length(0) {}

__CLASS_NAME__::~__CLASS_NAME__ () { delete [] _message; }

int __CLASS_NAME__::get (acs_apbm::trap_handle_t /*trap_handle*/) {

//	_OID = -1;
//	_values.clear();
//	delete [] _message; _message = 0;
//	_message_length = 0;

	return 0;
}

int __CLASS_NAME__::OID () const { return _OID; }

const std::vector<int> & __CLASS_NAME__::values () const { return _values; }

const char * __CLASS_NAME__::message () const { return _message; }

unsigned __CLASS_NAME__::message_length () const { return _message_length; }

__CLASS_NAME__ & __CLASS_NAME__::operator=(const __CLASS_NAME__ & /*rhs*/) { return *this; }

void __CLASS_NAME__::set_message (const char *msg, unsigned int length){


	ACS_APBM_TRACE_MESSAGE("Ciao questo e` un tracciamento senza parametri");

	ACS_APBM_TRACE_MESSAGE("Ciao questo e` un tracciamento con parametri: msg == %s", msg);


	if(_message)
		delete [] _message;
	 _message = new char[length];
	 memcpy(_message,msg,length);
	 _message_length = length;

}

void __CLASS_NAME__::set_OID ( int oid){ _OID = oid;}

void __CLASS_NAME__::set_values (std::vector<int> value) {_values = value;}


