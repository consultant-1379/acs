/*
 * MyThread.h
 *
 *  Created on: May 23, 2012
 *      Author: egimarr
 */

#ifndef MYTHREAD_H_
#define MYTHREAD_H_
// ==============================================================
// THREAD USED for Partecipant Registration Simulation invoke REPORT ACTION IN PROGRESS
// ==============================================================
#include "MyImplementer.h"
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

#include <sys/poll.h>

class MyThread : public ACE_Task_Base {
public :
	MyThread(MyImplementer *p_user1){

		implementer = p_user1;
		isStop = false;

	}

	~MyThread(){}

	void stop(){
		isStop=true;
	}

	int svc(void){

		int ret;

		struct pollfd fds[1];

		fds[0].fd = implementer->getSelObj();
		fds[0].events = POLLIN;


		while(!isStop)
		{
			ret = poll(fds, 1, 1);
			if (ret == 0)
			{

				//cout<<"timeout off"<<endl;

			}else if (ret == -1){
				printf("poll error: \n");
			}else{
				cout<<"MyThread : "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);
			}

		}

		return 0;
	}

private:
	MyImplementer *implementer;
	bool isStop;



};

#endif /* MYTHREAD_H_ */
