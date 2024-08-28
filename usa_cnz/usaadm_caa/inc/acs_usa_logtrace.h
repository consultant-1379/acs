#ifndef LOGTRACE_H
#define LOGTRACE_H

#include <syslog.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* Categories */
enum logtrace_categories {

           	CAT_LOG = 0,
                CAT_TRACE,
                CAT_TRACE1,
                CAT_TRACE2,
                CAT_TRACE3,
                CAT_TRACE4,
                CAT_TRACE5,
                CAT_TRACE6,
                CAT_TRACE7,
                CAT_TRACE8,
                CAT_TRACE_ENTER,
                CAT_TRACE_LEAVE,
                CAT_MAX
};

#define CATEGORY_ALL    0xffffffff

/**
 * logtrace_init - Initialize the logtrace system.
 *
 * @param ident An identity string to be prepended to every message. Typically
 * set to the program name.
 * @param pathname The pathname parameter should contain a valid
 * path name for a file if tracing is to be enabled. The user must have write
 * access to that file. If the file already exist, it is appended. If the file
 * name is not valid, no tracing is performed.
 * @param mask The initial trace mask. Should be set set to zero by
 *             default (trace disabled)
 *
 * @return int - 0 if OK, -1 otherwise
 */
extern int acs_usa_logtrace_init(const char *ident, const char *pathname, unsigned int mask);

/**
 * trace_category_set - Set the mask used for trace filtering.
 *
 * In libraries the category mask is typically set (to all ones) after a
 * library specific environment variable has been examined for the name of a
 * output file. The variable will not exist in a production system hence no
 * tracing will be done.
 *
 * In daemons there is no need to call this function during initialization,
 * tracing is disabled by default. In runtime, using e.g. a signal handler the
 * daemon could call this function to enable or change traced categories.
 *
 * @param category_mask The mask to set, 0 indicates no tracing.
 *
 * @return int - 0 if OK, -1 otherwise
 */
extern int acs_usa_trace_category_set(unsigned int category_mask);

/**
 * trace_category_get - Get the current mask used for trace filtering.
 *
 * @return int - The filtering mask value
 */
extern unsigned int acs_usa_trace_category_get(void);

/* internal functions, do not use directly */
extern void acs_usa_logtrace_log(const char *file, unsigned int line, int priority,
                  const char *format, ...) __attribute__ ((format(printf, 4, 5)));

extern void acs_usa_logtrace_trace(const char *file, unsigned int line, unsigned int category,
                    const char *format, ...) __attribute__ ((format(printf, 4, 5)));

/* LOG API. Use same levels as syslog */
#define USA_LOG_EM(format, args...) acs_usa_logtrace_log(__FILE__, __LINE__, LOG_EMERG, (format), ##args)
#define USA_LOG_AL(format, args...) acs_usa_logtrace_log(__FILE__, __LINE__, LOG_ALERT, (format), ##args)
#define USA_LOG_CR(format, args...) acs_usa_logtrace_log(__FILE__, __LINE__, LOG_CRIT, (format), ##args)
#define USA_LOG_ER(format, args...) acs_usa_logtrace_log(__FILE__, __LINE__, LOG_ERR, (format), ##args)
#define USA_LOG_WA(format, args...) acs_usa_logtrace_log(__FILE__, __LINE__, LOG_WARNING, (format), ##args)
#define USA_LOG_NO(format, args...) acs_usa_logtrace_log(__FILE__, __LINE__, LOG_NOTICE, (format), ##args)
#define USA_LOG_IN(format, args...) acs_usa_logtrace_log(__FILE__, __LINE__, LOG_INFO, (format), ##args)

/* TRACE API. */
#define USA_TRACE(format, args...)   acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE, (format), ##args)
#define USA_TRACE_1(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE1, (format), ##args)
#define USA_TRACE_2(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE2, (format), ##args)
#define USA_TRACE_3(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE3, (format), ##args)
#define USA_TRACE_4(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE4, (format), ##args)
#define USA_TRACE_5(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE5, (format), ##args)
#define USA_TRACE_6(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE6, (format), ##args)
#define USA_TRACE_7(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE7, (format), ##args)
#define USA_TRACE_8(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE8, (format), ##args)
#define USA_TRACE_ENTER()                acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE_ENTER, "%s ", __FUNCTION__)
#define USA_TRACE_ENTER2(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE_ENTER, "%s: "format, __FUNCTION__, ##args)
#define USA_TRACE_LEAVE()                 acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE_LEAVE, "%s ", __FUNCTION__)
#define USA_TRACE_LEAVE2(format, args...) acs_usa_logtrace_trace(__FILE__, __LINE__, CAT_TRACE_LEAVE, "%s: "format, __FUNCTION__, ##args) 


#ifdef  __cplusplus
}
#endif

#endif

