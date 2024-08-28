/*
 * ispprintBackupThread.h
 *
 *  Created on: Oct 15, 2010
 *      Author: xlucpet
 */

#ifndef ISPPRINTBACKUPTHREAD_H_
#define ISPPRINTBACKUPTHREAD_H_

#include "ace/Task.h"

class ACS_PRC_ispprintBackupThread : public ACE_Task_Base{
public:
	ACS_PRC_ispprintBackupThread();
	virtual ~ACS_PRC_ispprintBackupThread();
	virtual int svc ( void );

	void stop(){ sleep_thread = true; };

	void start(){ sleep_thread = false; };

	void init_logrotd( bool remove_file = false );

private:

	bool sleep_thread;
	int maxlogSize;
};

#endif /* ISPPRINTBACKUPTHREAD_H_ */
