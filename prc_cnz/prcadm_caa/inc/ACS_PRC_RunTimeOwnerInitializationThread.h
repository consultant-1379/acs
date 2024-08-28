/*
 * ACS_PRC_RunTimeOwnerInitializationThread.h
 *
 *  Created on: Jan 7, 2011
 *      Author: xlucpet
 */

#ifndef ACS_PRC_RUNTIMEOWNERINITIALIZATIONTHREAD_H_
#define ACS_PRC_RUNTIMEOWNERINITIALIZATIONTHREAD_H_

#include "ace/Task.h"

class ACS_PRC_RunTimeOwnerInitializationThread : public ACE_Task_Base{
public:
	ACS_PRC_RunTimeOwnerInitializationThread();
	virtual ~ACS_PRC_RunTimeOwnerInitializationThread();

	virtual int svc ( void );
};

#endif /* ACS_PRC_RUNTIMEOWNERINITIALIZATIONTHREAD_H_ */
