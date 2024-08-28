#include <pthread.h>

#include "ACS_DSD_TraTracer.h"
#include "ACS_DSD_Trace.h"

__CLASS_NAME__::__CLASS_NAME__ (ACS_DSD_TraTracer & tracer, const char * function_name, const char * file_name, int line)
	: _tracer(&tracer), _function_name(function_name), _file_name(file_name), _line(line) {
	_file_name
		? _tracer->trace("THREAD[%lu]: Entering function '%s' at '%s:%d'", pthread_self(), _function_name ?: "NO_FUNCTION", _file_name, _line)
		: _tracer->trace("THREAD[%lu]: Entering function '%s'", pthread_self(), _function_name ?: "NO_FUNCTION");
}

__CLASS_NAME__::~__CLASS_NAME__ () {
	_tracer->trace("THREAD[%lu]: Leaving function '%s'", pthread_self(), _function_name ?: "NO_FUNCTION");
}
