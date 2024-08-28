#ifndef ACS_PRC_EVENTMATRIXIMPLEMENTERTHREAD_H_
#define ACS_PRC_EVENTMATRIXIMPLEMENTERTHREAD_H_

#include "ace/Task.h"
#include <sys/poll.h>
#include <stdio.h>
#include <iostream>
#include "acs_prc_eventmatrix_implementer.h"

class acs_prc_eventmatrix_implementerThread : public ACE_Task_Base{

public:
	acs_prc_eventmatrix_implementerThread();
	virtual ~acs_prc_eventmatrix_implementerThread();

	void set (acs_prc_eventmatrix_implementer* p_Implementer){
		implementer = p_Implementer;
	}

	virtual int svc ( void );

	void stop(){ sleep = true; };

	void start(){ sleep = false; };

	bool isRunning(){ return !sleep; };

private:

	bool sleep;
	acs_prc_eventmatrix_implementer *implementer;

};

#endif /* ACS_PRC_EVENTMATRIXIMPLEMENTERTHREAD_H_ */
