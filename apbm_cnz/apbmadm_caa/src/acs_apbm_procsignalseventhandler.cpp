#include <string.h>

#include "acs_apbm_macros.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_cmdoptionparser.h"
#include "acs_apbm_serverworkingset.h"

#include "acs_apbm_procsignalseventhandler.h"

int __CLASS_NAME__::handle_signal (int signum, siginfo_t * /*siginfo*/, ucontext_t * /*context*/) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Signal handler: received signal number %d ('%s')", signum, ::strsignal(signum));

	// Catch termination signals only
	if ((signum == SIGINT) || (signum == SIGTERM)) {
		if (_server_working_set->cmdoption_parser->noha()) { // APBM server started from shell with --noha option enabled
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Signal handler: sending STOP_WORK_TO_EXIT_PROGRAM on the operation pipe...");

			// --noha option specified: sending the STOP_WORK_TO_EXIT_PROGRAM operation to the operation handler
			if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_EXIT_PROGRAM)) { // ERROR: sending the operation
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Signal handler: call 'operation_send' failed: cannot send the STOP_WORK_TO_EXIT_PROGRAM operation to the server work object: you must brutally killing the APBM server");

				// Signal the program termination
				_server_working_set->program_state = acs_apbm::PROGRAM_STATE_EXIT_PROGRAM;

				// In case of error I will force ending the reactor event loop
				if (!reactor()->reactor_event_loop_done()) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Signal handler: ending the main reactor event loop...");
					if (reactor()->end_reactor_event_loop())
						ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Signal handler: call 'end_reactor_event_loop' failed: trying to end the main reactor event loop upon termination signal received");
				}
			}
		} else { // APBM server started by CoreMW in HA mode
			// The APBM server started by CoreMW cluster. Cluster commands should be used to stop the server.
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Signal handler: APBM server started by CoreMW in HA mode so SIGINT and SIGTERM signals are ignored! You must use CoreMW (AMF) application management commands to terminate an APBM server running without --noha option");
		}
	} else if (signum == SIGPIPE) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Signal handler: signal %d (%s) ignored by APBM server", signum, ::strsignal(signum));
	} else if (signum == SIGHUP) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Signal handler: handling of signal %d (%s) NOT YET IMPLEMENTED", signum, ::strsignal(signum));
	}

	return acs_apbm::ERR_NO_ERRORS;
}
