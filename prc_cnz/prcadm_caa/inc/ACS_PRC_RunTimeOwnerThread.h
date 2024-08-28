/*
 * ACS_PRC_RunTimeOwnerThread.h
 *
 *  Created on: Dec 29, 2010
 *      Author: xlucpet
 */

#ifndef ACS_PRC_RUNTIMEOWNERTHREAD_H_
#define ACS_PRC_RUNTIMEOWNERTHREAD_H_

#include "ace/Task.h"
#include "ACS_PRC_RunTimeOwner.h"
#include <sys/poll.h>
#include <stdio.h>
#include <iostream>

class ACS_PRC_RunTimeOwnerThread : public ACE_Task_Base{

public:
	ACS_PRC_RunTimeOwnerThread();
	virtual ~ACS_PRC_RunTimeOwnerThread();

	void set (ACS_PRC_RunTimeOwner* p_Implementer){
		implementer = p_Implementer;
	}

	virtual int svc ( void );

	void stop(){ sleep = true; };

	void start(){ sleep = false; };

private:

	bool sleep;
	ACS_PRC_RunTimeOwner *implementer;

};

#endif /* ACS_PRC_RUNTIMEOWNERTHREAD_H_ */
