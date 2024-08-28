


#include <pthread.h>
#include <iostream>
#include <sys/time.h>
#include <semaphore.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <new>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "ACS_CS_Timer.h"

#include "timertest.h"

struct ThreadStruct {
	int * fd;
	int no;
};


int main(int argc, char * argv[]) {

	ACS_CS_EventHandle timerHandle1 = ACS_CS_Timer::CreateTimer(true, "testtimer1");
	ACS_CS_EventHandle timerHandle2 = ACS_CS_Timer::CreateTimer(true, "testtimer2");
	ACS_CS_EventHandle timerHandle3 = ACS_CS_Timer::CreateTimer(true, "testtimer3");

	pthread_t tid[3];

	(void) pthread_create(&tid[0], 0, RunThread1, 0);
	(void) pthread_create(&tid[1], 0, RunThread2, 0);
	(void) pthread_create(&tid[2], 0, RunThread3, 0);


	safePrint("All threads started");
	
	safePrint("Sleeping");
	sleep(2);
	safePrint("Setting timers");
	
	ACS_CS_Timer::SetTimer(timerHandle1, 3);
	ACS_CS_Timer::SetTimer(timerHandle2, 5);
	ACS_CS_Timer::SetTimer(timerHandle3, 12);

	sleep(7);
	safePrint("Cancelling timer 3");

	ACS_CS_Timer::CancelTimer(timerHandle3);

	safePrint("Waiting for threads");

	for (int i = 0; i < 3; i++){
		
		if (pthread_equal(pthread_self(), tid[i]))
			continue;
		else
			(void) pthread_join(tid[i], NULL);
	}
	
	safePrint("All threads finished");

	sleep(10);
}


static void * RunThread1(void *arg) {

	safePrint("Thread 1 started");

	ACS_CS_EventHandle timerHandle = ACS_CS_Timer::OpenNamedTimer("testtimer1");

	ACS_CS_EventHandle arr[1];
	arr[0] = timerHandle;

	ACS_CS_Event::WaitForEvents(1, arr, 999999999);

	safePrint("Thread 1 finished");

	return 0;
}

static void * RunThread2(void *arg) {
	
	safePrint("Thread 2 started");

	ACS_CS_EventHandle timerHandle = ACS_CS_Timer::OpenNamedTimer("testtimer2");
	
	ACS_CS_EventHandle arr[1];
	arr[0] = timerHandle;

	ACS_CS_Event::WaitForEvents(1, arr, 999999999);

	safePrint("Thread 2 finished");

	return 0;
}

static void * RunThread3(void *arg) {

	safePrint("Thread 3 started");

	ACS_CS_EventHandle timerHandle = ACS_CS_Timer::OpenNamedTimer("testtimer3");

	ACS_CS_EventHandle arr[1];
	arr[0] = timerHandle;

	ACS_CS_Event::WaitForEvents(1, arr, 999999999);

	safePrint("Thread 3 finished");

	return 0;
}

void safePrint(const char * str) {
	
	std::cout << str << std::endl;
}

