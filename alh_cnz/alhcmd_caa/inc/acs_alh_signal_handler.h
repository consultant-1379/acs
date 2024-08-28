/*
 * acs_alh_signal_handler.h
 *
 *  Created on: Nov 30, 2011
 *      Author: xgiopap
 */

#ifndef ACS_ALH_SIGNAL_HANDLER_H_
#define ACS_ALH_SIGNAL_HANDLER_H_

#include "ace/Event_Handler.h"

class acs_alh_signal_handler: public ACE_Event_Handler
{
public:
	acs_alh_signal_handler();
	virtual ~acs_alh_signal_handler();

	virtual int handle_signal (int signum, siginfo_t * = 0,ucontext_t * = 0);
};

#endif /* ACS_ALH_SIGNAL_HANDLER_H_ */
