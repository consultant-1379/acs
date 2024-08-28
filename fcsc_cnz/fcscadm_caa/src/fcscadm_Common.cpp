
#include "fcscadm_Common.h"

void mysyslog(int priority, const char *format, ...)
{
        syslog(LOG_INFO, format);
}

