#include <sys/time.h>
#include <pthread.h>

#include "ACS_DSD_LoggerTra.h"

ACS_TRA_LogResult __CLASS_NAME__::vlogf (ACS_TRA_LogLevel level, const char * format, va_list ap, bool include_header) {
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

	//Using TRA-logging
	const ACS_TRA_LogResult log_result = _log_obj.Write(buffer, level);

	//CHECK POINT
	//printf("Level == %2d, LOG_RES == %d - %s\n", level, log_result, buffer);

	return log_result;
}
