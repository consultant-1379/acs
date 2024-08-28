#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>

#include "ACS_DSD_Logger4cplus.h"

int __CLASS_NAME__::vlogf (const char * format, va_list ap, bool include_header) {
	char buffer[8 * 1024];
	int char_count = 0;

	if (include_header) {
		timeval tv;
		tm broken_datetime;

		char_count = (gettimeofday(&tv, 0) || !gmtime_r(&tv.tv_sec, &broken_datetime))
				? snprintf(buffer, ACS_DSD_ARRAY_SIZE(buffer), "\?\?\?\?-\?\?-\?\? \?\?:\?\?:\?\?.\?\?\?\?\?\? THREAD[%lu] : ", pthread_self())
				: snprintf(buffer, ACS_DSD_ARRAY_SIZE(buffer), "%d-%02d-%02d %02d:%02d:%02d.%06ld THREAD[%lu] : ",
						1900 + broken_datetime.tm_year, broken_datetime.tm_mon, broken_datetime.tm_mday,
						broken_datetime.tm_hour, broken_datetime.tm_min, broken_datetime.tm_sec, tv.tv_usec, pthread_self());
	}

	//NO newline
	vsnprintf(buffer + char_count, ACS_DSD_ARRAY_SIZE(buffer) - char_count, format, ap);

	////WITH newline
	//char_count += vsnprintf(buffer + char_count, ACS_DSD_ARRAY_SIZE(buffer) - char_count, format, ap);
	//snprintf(buffer + char_count, ACS_DSD_ARRAY_SIZE(buffer) - char_count, "\n");

	LOG4CPLUS_DEBUG(_log_obj, buffer);

	//CHECK POINT
	//printf("DEBUG - %s\n", buffer);

	return 0;
}
