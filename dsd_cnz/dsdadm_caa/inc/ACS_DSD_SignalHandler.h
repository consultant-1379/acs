#ifndef ACS_DSD_SIGNALHANDLER_H_
#define ACS_DSD_SIGNALHANDLER_H_

#include "ace/Event_Handler.h"
#include <signal.h>

class ACS_DSD_SignalHandler: public ACE_Event_Handler
{
public:
	ACS_DSD_SignalHandler() {}
	virtual ~ACS_DSD_SignalHandler() {}

	virtual int handle_signal (int signum, siginfo_t * = 0,ucontext_t * = 0);
	virtual int handle_timeout ( const ACE_Time_Value &  current_time,  const void *  act = 0);
};

#endif /* ACS_DSD_SIGNALHANDLER_H_ */
