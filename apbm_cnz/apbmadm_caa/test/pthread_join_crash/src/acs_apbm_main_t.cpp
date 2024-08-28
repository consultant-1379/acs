#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "acs_apbm_programconfiguration.h"

#include "acs_apbm_logger_t.h"
#include "acs_apbm_serverwork_t.h"


int main (int /*argc*/, char * /*argv*/ []) {
	// Initialize the server logger
	acs_apbm_logger::open(acs_apbm_programconfiguration::logger_appender_name);

	ACS_APBM_SYSLOG(LOG_NOTICE, LOG_LEVEL_INFO, "APBM Server started (PID = %d)", ::getpid());

	/* Initialize the random gnerator*/
	::srand(::time(0));

	acs_apbm_serverwork server_work;
	const int return_code = server_work.work();

	ACS_APBM_SYSLOG(LOG_NOTICE, LOG_LEVEL_INFO, "APBM Server exit (PID = %d)", ::getpid());

	acs_apbm_logger::close();

	return return_code;
}
