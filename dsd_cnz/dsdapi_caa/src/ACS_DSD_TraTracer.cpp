#include <cstdarg>
#include <cstdio>

#include "ACS_DSD_TraTracer.h"

int __CLASS_NAME__::trace (const char * format, ...) {
	if (_trace_obj.ACS_TRA_ON()) {
		char trace_msg[ACS_DSD_TRACE_MESSAGE_SIZE_MAX];

		va_list argp;
		va_start(argp, format);
		vsnprintf(trace_msg, ACS_DSD_ARRAY_SIZE(trace_msg), format, argp);
		trace_msg[ACS_DSD_ARRAY_SIZE(trace_msg) - 1] = 0;
		va_end(argp);

		_trace_obj.ACS_TRA_event(1, trace_msg);
	}

	return 0;
}

void __CLASS_NAME__::dump (uint8_t * buffer, ssize_t size, ssize_t dumping_size, ssize_t dumping_line_length) {
	size_t output_buffer_size = 2 + 8 + 2 + 3 * dumping_line_length + 16;
	char output_buffer[output_buffer_size];

	for (ssize_t i = 0; (i < size) && (i < dumping_size); ) {
		int chars = snprintf(output_buffer, output_buffer_size, "  %08zX:", static_cast<size_t>(i));
		for (ssize_t col = 0; (col < dumping_line_length) && (i < size) && (i < dumping_size); ++i, ++col)
			chars += snprintf(output_buffer + chars, output_buffer_size - chars, " %02X", buffer[i]);
		trace(output_buffer);
	}
}
