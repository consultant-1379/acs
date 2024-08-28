#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <syslog.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <iostream>
#include "acs_usa_logtrace.h"

static int trace_fd = -1;
static int acs_usa_category_mask;
static char *prefix_name[] = { 	(char*)"EM", 
				(char*)"AL",
			       	(char*)"CR", 
				(char*)"ER", 
				(char*)"WA", 
				(char*)"NO", 
				(char*)"IN", 
				(char*)"DB",
        			(char*)"TR", 
				(char*)"T1", 
				(char*)"T2", 
				(char*)"T3", 
				(char*)"T4", 
				(char*)"T5", 
				(char*)"T6", 
				(char*)"T7", 
				(char*)"T8", 
				(char*)">>", 
				(char*)"<<"
};

static const char *ident;
static const char *pathname;
/**
 * HUP signal handler to enable/disable trace (toggle)
 * @param sig
 */
static void acs_usa_sighup_handler(int sig)
{

	(void)sig;

        unsigned int trace_mask;

        if (acs_usa_category_mask == 0)
                trace_mask = CATEGORY_ALL;
        else
                trace_mask = 0;

        acs_usa_trace_category_set(trace_mask);
}

static void acs_usa_output(const char *file, unsigned int line, int priority, int category, const char *format, va_list ap)
{
        int i, j;
        struct timeval tv;
        char preamble[512];
        char log_string[1024];

        assert(priority <= LOG_DEBUG && category < CAT_MAX);

        /* Create a nice syslog looking date string */
        gettimeofday(&tv, NULL);
        strftime(log_string, sizeof(log_string), "%F %k:%M:%S", localtime(&tv.tv_sec));
        i = snprintf(preamble, sizeof(preamble), "%s.%06ld %s ", log_string, tv.tv_usec, ident);

        snprintf(&preamble[i], sizeof(preamble) - i, "[%d:%s:%04u] %s %s",
                getpid(), file, line, prefix_name[priority + category], format);
        i = vsnprintf(log_string, sizeof(log_string), preamble, ap);

        /* Add line feed if not there already */
        if (log_string[i - 1] != '\n') {
                log_string[i] = '\n';
                log_string[i + 1] = '\0';
                i++;
        }

        /* If we got here without a file descriptor, trace was enabled in runtime, open the file */
        if (trace_fd == -1) {
                trace_fd = open(pathname, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (trace_fd < 0) {
                        syslog(LOG_ERR, "logtrace: open failed, file=%s (%s)", pathname, strerror(errno));
                        return;
                }
        }

write_retry:
        j = write(trace_fd, log_string, i);
        if (j == -1) {
                if (errno == EAGAIN)
                        goto write_retry;
                else
                        syslog(LOG_ERR, "logtrace: write failed, %s", strerror(errno));
        }
}

void acs_usa_logtrace_log(const char *file, unsigned int line, int priority, const char *format, ...)
{
        va_list ap;
        va_list ap2;
	int i;
	std::string str(file);

        /* Uncondionally send to syslog */
        va_start(ap, format);
        va_copy(ap2, ap);
        vsyslog(priority, format, ap);

        /* Only output to file if configured to */
        if (!(acs_usa_category_mask & (1 << CAT_LOG)))
                goto done;

	// strip only file name from the entire file path
	i=str.find_last_of('/');
	str.assign(str, i+1, str.size());
        acs_usa_output(str.data(), line, priority, CAT_LOG, format, ap2);

done:
        va_end(ap);
        va_end(ap2);
}

void acs_usa_logtrace_trace(const char *file, unsigned int line, unsigned int category, const char *format, ...)
{
        va_list ap;

        /* Filter on category */
        if (!(acs_usa_category_mask & (1 << category)))
                return;

        va_start(ap, format);

	// strip only file name from the entire file path
	std::string str(file);
        int i=str.find_last_of('/');
        str.assign(str, i+1, str.size());

        acs_usa_output(str.data(), line, LOG_DEBUG, category, format, ap);
        va_end(ap);
}

int acs_usa_logtrace_init(const char *_ident, const char *_pathname, unsigned int _mask)
{
        ident = _ident;
        pathname = strdup(_pathname);
        acs_usa_category_mask = _mask;

	// register sighup handler
	if (signal(SIGHUP, acs_usa_sighup_handler) == SIG_ERR) {
		syslog(LOG_ERR, "logtrace: registering SIGHUP failed, (%s)", strerror(errno));
		return -1;
	}	

        if (_mask != 0) {
                trace_fd = open(pathname, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (trace_fd < 0) {
                        syslog(LOG_ERR, "logtrace: open failed, file=%s (%s)", pathname, strerror(errno));
                        return -1;
                }

                syslog(LOG_INFO, "logtrace: trace enabled to file %s, mask=0x%x", pathname, acs_usa_category_mask);
        }

        return 0;
}

int acs_usa_trace_category_set(unsigned int mask)
{
        acs_usa_category_mask = mask;

        if (acs_usa_category_mask == 0) {
                if (trace_fd != -1) {
                        (void) close(trace_fd);
                        trace_fd = -1;
                }
                syslog(LOG_INFO, "logtrace: trace disabled");
        }
        else
                syslog(LOG_INFO, "logtrace: trace enabled to file %s, mask=0x%x", pathname, acs_usa_category_mask);

        return 0;
}

unsigned int trace_category_get(void)
{
        return acs_usa_category_mask;
}



