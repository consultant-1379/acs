#include <stdio.h>

#include <ace/TP_Reactor.h>
#include <ace/Reactor.h>

#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_sanotifiereventhandler.h"

int __CLASS_NAME__::handle_input (ACE_HANDLE /*fd*/) {
	SaAisErrorT sa_call_result = SA_AIS_OK;

	do {
		if (reactor()->reactor_event_loop_done()) break; // Stop if application exit

		if ((sa_call_result = saNtfDispatch(_ntf_handle, SA_DISPATCH_ALL)) == SA_AIS_ERR_TRY_AGAIN) { // ERROR: NTF services not yet ready: retry
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfDispatch' failed with TRY_AGAIN error code: retrying after %u second(s)",
					acs_apbm_programconfiguration::ntf_dispatch_callbacks_delay);
			::sleep(acs_apbm_programconfiguration::ntf_dispatch_callbacks_delay);
		} else if (sa_call_result != SA_AIS_OK) { // ERROR: Another error
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfDispatch' failed: sa_call_result == %d: stoopping retrying", sa_call_result);
		}
	} while (sa_call_result == SA_AIS_ERR_TRY_AGAIN);

	return 0;
}

int __CLASS_NAME__::handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/) {
	return 0;
}
