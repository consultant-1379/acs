#include <string.h>

#include "acs_apbm_snmpsessioninfo.h"

namespace {
	char gs_empty_string[] = "";
}

void __CLASS_NAME__::peername (char * new_value) { snmp_session::peername = new_value ?: gs_empty_string; }

void __CLASS_NAME__::community (char * new_value) {
	snmp_session::community = reinterpret_cast<u_char *>(new_value ?: gs_empty_string);
	snmp_session::community_len = ::strlen(reinterpret_cast<char *>(snmp_session::community));
}
