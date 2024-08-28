#ifndef ACS_PRC_ISPADMINOPERATIONIMPLEMENTERTHREAD_H_
#define ACS_PRC_ISPADMINOPERATIONIMPLEMENTERTHREAD_H_

#include "ace/Task.h"
#include <sys/poll.h>
#include <stdio.h>
#include <iostream>
#include "ACS_PRC_IspAdminOperationImplementer.h"

class ACS_PRC_IspAdminOperationImplementerThread : public ACE_Task_Base{

public:
	ACS_PRC_IspAdminOperationImplementerThread();
	virtual ~ACS_PRC_IspAdminOperationImplementerThread();

	void set (ACS_PRC_IspAdminOperationImplementer* p_Implementer){
		implementer = p_Implementer;
	}

	virtual int svc ( void );

	void stop(){ sleep = true; };

	void start(){ sleep = false; };

	bool isRunning(){ return !sleep; };

private:

	bool sleep;
	ACS_PRC_IspAdminOperationImplementer *implementer;

};

#endif /* ACS_PRC_ISPADMINOPERATIONIMPLEMENTERTHREAD_H_ */
