/*
 * MktrSignalHandler.h
 *
 *  Created on: Aug 30, 2010
 *      Author: xludesi
 */

#ifndef MKTRSIGNALHANDLER_H_
#define MKTRSIGNALHANDLER_H_

#include "ace/Event_Handler.h"

class MktrSignalHandler: public ACE_Event_Handler
{
public:
	MktrSignalHandler();
	virtual ~MktrSignalHandler();

	virtual int handle_signal (int signum, siginfo_t * = 0,ucontext_t * = 0);

private:
	bool _ignore_signals;
};

#endif /* MKTRSIGNALHANDLER_H_ */
