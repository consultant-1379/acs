#include "ACS_DSD_AppProt_QueryProcessor.h"

int __CLASS_NAME__::svc () {
	if (_timeout_ms == 0xFFFFFFFFU) _reactor.run_reactor_event_loop();
	else {
		ACE_Time_Value timeout;
		timeout.msec(static_cast<long int>(_timeout_ms));
		_reactor.run_reactor_event_loop(timeout);
	}

	return 0;
}
