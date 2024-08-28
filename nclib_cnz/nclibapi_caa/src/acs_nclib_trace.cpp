#ifdef ACS_NCLIB_HAS_TRACE

#include <string.h>
#include <stdio.h>

#include "acs_nclib_constants.h"
#include "acs_nclib_trace.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_nclib_tracer

int __CLASS_NAME__::vtrace (const char * file, int line, const char * format, va_list ap) {
	if (_tra_tracer.ACS_TRA_ON()) {
		char trace_msg[ACS_NCLIB_TRACE_MESSAGE_SIZE_MAX];
		int char_wrote = 0;

		if (file || (line > 0)) {
			char_wrote = ::snprintf(trace_msg, ACS_NCLIB_ARRAY_SIZE(trace_msg), "{");

			if (file) {
				char * filename = ::strrchr(const_cast<char*>(file), '/');
				char_wrote += ::snprintf(trace_msg + char_wrote, ACS_NCLIB_ARRAY_SIZE(trace_msg) - char_wrote, "%s", filename ? (filename + 1) : "NO_FILE");
			}

			(line > 0) && (char_wrote += ::snprintf(trace_msg + char_wrote, ACS_NCLIB_ARRAY_SIZE(trace_msg) - char_wrote, "::%d", line));
			char_wrote += ::snprintf(trace_msg + char_wrote, ACS_NCLIB_ARRAY_SIZE(trace_msg) - char_wrote, "} ");
		}

		::vsnprintf(trace_msg + char_wrote, ACS_NCLIB_ARRAY_SIZE(trace_msg) - char_wrote, format, ap);
		trace_msg[ACS_NCLIB_ARRAY_SIZE(trace_msg) - 1] = 0;

		_tra_tracer.ACS_TRA_event(1, trace_msg);
	}

	return acs_nclib::ERR_NO_ERRORS;
}

void __CLASS_NAME__::dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length) {
	size_t output_buffer_size = 2 + 8 + 2 + 3*dumping_line_length + 16;
	char output_buffer[output_buffer_size];

	for (size_t i = 0; (i < size) && (i < dumping_size); ) {
		int chars = ::snprintf(output_buffer, output_buffer_size, "  %08zX:", i);
		for (size_t col = 0; (col < dumping_line_length) && (i < size) && (i < dumping_size); ++i, ++col)
			chars += ::snprintf(output_buffer + chars, output_buffer_size - chars, " %02X", buffer[i]);
		trace(output_buffer);
	}
}


#undef __CLASS_NAME__
#define __CLASS_NAME__ acs_nclib_function_tracer

__CLASS_NAME__::__CLASS_NAME__ (acs_nclib_tracer & tracer, const char * function_name)
: _tracer(tracer), _function_name(function_name) {
	_tracer.trace("Entering function '%s'", function_name);
}

__CLASS_NAME__::__CLASS_NAME__ (acs_nclib_tracer & tracer, const char * function_name, const char * file_name, int line)
: _tracer(tracer), _function_name(function_name) {
	_tracer.trace_source(file_name, line, "Entering function '%s'", _function_name ?: "NO_FUNCTION");
}

__CLASS_NAME__::~__CLASS_NAME__ () {
	_tracer.trace("Leaving function '%s'", _function_name ?: "NO_FUNCTION");
}

#endif // ACS_NCLIB_HAS_TRACE
