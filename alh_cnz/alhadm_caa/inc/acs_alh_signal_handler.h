/*
 * acs_alh_signal_handler.h
 *
 *  Created on: Dec 5, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_SIGNAL_HANDLER_H_
#define ACS_ALH_SIGNAL_HANDLER_H_

#include "acs_alh_log.h"

#include "ace/Event_Handler.h"
#include <signal.h>

class acs_alh_signal_handler: public ACE_Event_Handler
{
public:
	acs_alh_signal_handler();
	virtual ~acs_alh_signal_handler() {}

	virtual int handle_signal (int signum, siginfo_t * = 0, ucontext_t * = 0);
//	virtual int handle_timeout ( const ACE_Time_Value &  current_time,  const void *  act = 0);
private:
	acs_alh_log log_; //Log object
};

#endif /* ACS_ALH_SIGNAL_HANDLER_H_ */
