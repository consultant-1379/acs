#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

#include "acs_nclib_errorinfoprovider.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_nclib_errorinfoprovider

void __CLASS_NAME__::set_error_info (int code, const char * format_text, ...) {
	if (format_text) {
		va_list argp;
		::va_start(argp, format_text);

		::vsnprintf(_last_error_text, ACS_NCLIB_ARRAY_SIZE(_last_error_text), format_text, argp);
		_last_error_text[ACS_NCLIB_ARRAY_SIZE(_last_error_text) - 1] = 0;

		::va_end(argp);
	} else *_last_error_text = '\0';

	_last_error_code = code;
}

void __CLASS_NAME__::set_error_info_errno (int code, const char * format_text, ...) {
	int errno_save = errno;
	int char_count = 0;

	if (format_text) {
		va_list argp;
		::va_start(argp, format_text);

		char_count = ::vsnprintf(_last_error_text, ACS_NCLIB_ARRAY_SIZE(_last_error_text), format_text, argp);

		::va_end(argp);
	} else *_last_error_text = '\0';

	char errno_buf[ACS_NCLIB_ERROR_TEXT_SIZE_MAX];
	::snprintf(_last_error_text + char_count, ACS_NCLIB_ARRAY_SIZE(_last_error_text) - char_count, " [errno == %d, errno_text == '%s']",
			errno_save, ::strerror_r(errno_save, errno_buf, ACS_NCLIB_ARRAY_SIZE(errno_buf)));

	_last_error_text[ACS_NCLIB_ARRAY_SIZE(_last_error_text) - 1] = 0;

	_last_error_code = code;
}
