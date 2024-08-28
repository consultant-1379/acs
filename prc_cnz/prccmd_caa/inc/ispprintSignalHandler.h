/*
 * ispprintSignalHandler.h
 *
 *  Created on: Sep 29, 2010
 *      Author: xlucpet
 */

#ifndef ISPPRINTSIGNALHANDLER_H_
#define ISPPRINTSIGNALHANDLER_H_

#include "ace/Event_Handler.h"

class ispprintSignalHandler: public ACE_Event_Handler {
public:
	ispprintSignalHandler();
	virtual ~ispprintSignalHandler();

	virtual int handle_signal (int signum, siginfo_t * = 0,ucontext_t * = 0);

};

#endif /* ISPPRINTSIGNALHANDLER_H_ */
