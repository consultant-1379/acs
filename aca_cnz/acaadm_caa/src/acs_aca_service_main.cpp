#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#ifdef ACS_ACA_HAS_HARD_LIMIT
#include <sys/prctl.h>
#include <sys/capability.h>
#endif

#include <iostream>

#include "acs_aca_logger.h"
#include "acs_aca_msd_haservice.h"
#include "acs_aca_common.h"
#include "acs_aca_macros.h"

using namespace std;

ACS_ACA_MSD_HAService * theACAApplPtr = 0;
ACS_MSD_Service * theACAServerInteractive = 0;
bool theInteractiveMode = false;

namespace {
	int raise_fd_limits () {
		struct rlimit rl = {0, 0};
		rl.rlim_cur = 2048;
		rl.rlim_max = 2048;

		if (::setrlimit(RLIMIT_NOFILE, &rl)) { // ERROR: Setting the new FD hard limit
			ACS_ACA_LOG(LOG_LEVEL_ERROR,
					"Call 'setrlimit' failed: cannot raise my FD hard limit: rlim_cur == %lu, rlim_max == %lu: errno == %d",
					rl.rlim_cur, rl.rlim_max, errno);
			return -1;
		}

		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "FD limits successfully set: rlim_cur == %lu, rlim_max == %lu",
				rl.rlim_cur, rl.rlim_max);

		return 0;
	}

#ifdef ACS_ACA_HAS_HARD_LIMIT
	int set_process_capability () {
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Setting capabilities for ACA process...");

		bool something_failed = false;

		if(::prctl(PR_CAPBSET_DROP, CAP_SYS_RESOURCE) < 0) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'prctl' failed, errno = %d", errno);
			something_failed = true;
		}

		cap_t cap = 0;
		cap_value_t cap_list[] = {CAP_SYS_RESOURCE};
		int cap_list_size = ACS_ACA_ARRAY_SIZE(cap_list);

		if (!(cap = ::cap_get_proc())) {
			// ERROR: got NULL cap_t pointer, impossible to go on!
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'cap_get_proc' failed, errno = %d", errno);
			return -1;
		}

		if (::cap_set_flag(cap, CAP_EFFECTIVE, cap_list_size, cap_list, CAP_CLEAR) < 0) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'cap_set_flag' failed for 'CAP_EFFECTIVE' flag, errno = %d", errno);
			something_failed = true;
		}

		if (::cap_set_flag(cap, CAP_INHERITABLE, cap_list_size, cap_list, CAP_CLEAR) < 0) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'cap_set_flag' failed for 'CAP_INHERITABLE' flag, errno = %d", errno);
			something_failed = true;
		}

		if (::cap_set_flag(cap, CAP_PERMITTED, cap_list_size, cap_list, CAP_CLEAR) < 0) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'cap_set_flag' failed for 'CAP_PERMITTED' flag, errno = %d", errno);
			something_failed = true;
		}

		if (::cap_set_proc(cap) < 0) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'cap_set_proc' failed, errno = %d", errno);
			something_failed = true;
		}

		if (::cap_free(cap) < 0) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'cap_free' failed, errno = %d", errno);
			something_failed = true;
		}

		ACS_ACA_LOG((something_failed) ? LOG_LEVEL_ERROR : LOG_LEVEL_INFO,
				"Capabilities for ACA process %s correctly set, the Hard Limit Functionality is %s WORKING!",
				(something_failed) ? "NOT" : "", (something_failed) ? "NOT" : "");
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
		return (something_failed) ? -1 : 0;
	}
#endif
}

/*==========================================================================
        ROUTINE: sighandler
========================================================================== */

void sighandler (int signum) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Signal '%s' (%d) received!", ::strsignal(signum), signum);

	if ((signum == SIGTERM) || (signum == SIGINT) || (signum == SIGTSTP)) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Signal generated to stop ACA service...");

		if (!theInteractiveMode) {
			if (theACAApplPtr) {
				theACAApplPtr->performComponentRemoveJobs();
				theACAApplPtr->performComponentTerminateJobs();
			}
		} else {
			if (theACAServerInteractive) {
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Requesting ACA Thread to stop");
				theACAServerInteractive->stop();
			}
		}
	}
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void printUsage () {
}

int ACE_TMAIN(int argc, char * argv []) {

	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);

	if (::sigaction(SIGINT, &sa, 0) == -1) {
		return -1;
	}

	if (::sigaction(SIGTERM, &sa, 0) == -1) {
		return -1;
	}

	if (::sigaction(SIGTSTP, &sa, 0) == -1) {
		return -1;
	}

	if (::sigaction(SIGPIPE, &sa, 0) == -1) {
		return -1;
	}

	if (argc > 1) {
		// If -d flag is specified, then the user has requested to start the
		// service in debug mode.

		acs_aca_logger::open("ACA");

		if ((argc == 2) && (!::strcmp(argv[1],"-d"))) {
			theInteractiveMode = true;
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Starting acs_acad in debug mode...");

			// Raising the number of possible file descriptors that ACA can open
			raise_fd_limits();

#ifdef ACS_ACA_HAS_HARD_LIMIT
			// Set the process capability to activate the Hard Limit Functionality
			if (set_process_capability()) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'set_process_capability' returned a non-zero value!");
			}
#endif

			//Allocate memory for ACA Server.
			theACAServerInteractive = new (std::nothrow) ACS_MSD_Service();

			if (!theACAServerInteractive) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for the ACS_MSD_Service object");
			}
			else {
				theACAServerInteractive->StartServer();
				delete theACAServerInteractive;	theACAServerInteractive = 0;
			}
		} else {
			ACS_MSD_Service::printUsage();
		}
	} else {

		theInteractiveMode = false;
		theACAApplPtr = new (std::nothrow) ACS_ACA_MSD_HAService("acs_acad", "root");

		acs_aca_logger::open("ACA");

		// Raising the number of possible file descriptors that ACA can open
		raise_fd_limits();

#ifdef ACS_ACA_HAS_HARD_LIMIT
		// Set the process capability to activate the Hard Limit Functionality
		if (set_process_capability()) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'set_process_capability' returned a non-zero value!");
		}
#endif

		if (!theACAApplPtr) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for the ACS_ACA_MSD_HAService object");
			acs_aca_logger::close();
			return -2;
		}

		ACS_ACA_LOG(LOG_LEVEL_INFO, "Starting acs_acad in HA mode...");

		ACS_APGCC_HA_ReturnType errorCode = theACAApplPtr->activate();
		if (errorCode == ACS_APGCC_HA_FAILURE)
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "HA activation failed for acs_acad application");
		else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to gracefully close acs_acad application");
		else if (errorCode == ACS_APGCC_HA_SUCCESS)
			ACS_ACA_LOG(LOG_LEVEL_INFO, "acs_acad application gracefully closed");
		else
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while starting acs_acad application in HA mode");

		delete theACAApplPtr;
		theACAApplPtr = 0;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "acs_acad server exiting...");

	acs_aca_logger::close();

	return 0;
}
