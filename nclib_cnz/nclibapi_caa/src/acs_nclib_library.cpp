#include "unistd.h"

#include "libssh2.h"

#include "acs_nclib_trace.h"
#include "acs_nclib_constants.h"
#include "acs_nclib_library.h"

ACS_NCLIB_TRACE_DEFINE(acs_nclib_library);

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_nclib_library

acs_nclib_errorinfoprovider acs_nclib_library::_error_info;


int acs_nclib_library::init () {
	ACS_NCLIB_TRACE_FUNCTION;

	if (const int call_result = libssh2_init(0)) { // ERROR: Initializing the libssh2 library
		_error_info.set_error_info(acs_nclib::ERR_LIBSSH_INIT_FAILED, "SSH library initialization failed: SSH library error code == %d", call_result);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	_error_info.set_error_info(acs_nclib::ERR_NO_ERRORS);

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_library::exit () {
	ACS_NCLIB_TRACE_FUNCTION;

	libssh2_exit();

	_error_info.set_error_info(acs_nclib::ERR_NO_ERRORS);
	return acs_nclib::ERR_NO_ERRORS;
}


