/*
 * AdminOperation_Implementer_Main.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

//============================================================================
// Name        : RuntimeOwnerUT.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

/*#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"*/

#include "ace/Task.h"
#include <sys/poll.h>

#include "AdminOperation_MyImplementer.h"
#include "MyClassImplementer.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
//#include "ACS_APGCC_OiHandler.h"
#include "acs_apgcc_oihandler_V2.h"

//#include "MyClassImplementer.h"
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"



using namespace std;

/***/
class MyThread_Reactor : public ACE_Task_Base {

public:
	MyThread_Reactor(MyClassImplementer *p_implementer, ACE_Reactor *p_reactor){

		implementer = p_implementer;
		reactor = p_reactor;
	}

	~MyThread_Reactor(){};

	int svc(void){

		implementer->dispatch(reactor, ACS_APGCC_DISPATCH_ALL);

		reactor->run_reactor_event_loop();

		return 0;
	}


private:
	MyClassImplementer *implementer;
	ACE_Reactor *reactor;
};
/***/


class MyThread : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThread(AdminOperation_MyImplementer *p_user1){

		cout<<"Costruttore Thread"<<endl;

		implementer = p_user1;

		isStop = false;
	}


	/**Destructor**/
	~MyThread(){
		cout<<"Distruttore Thread"<<endl;
	}


	/**This method is used to stop the thread**/
	void stop(){

		cout<<"Method Stop invocated"<<endl;

		isStop=true;
	}


	/**svc method **/
	int svc(void){


		cout<<"Thread AVVIATO"<<endl;

		int ret;

		cout<<"getSelObj: "<<implementer->getSelObj()<<endl;

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

				cout<<"FD THREAD: "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		cout<<"Thread TERMINATO"<<endl;

		return 0;
	}

private:

	AdminOperation_MyImplementer *implementer;

	bool isStop;

};


int ACE_TMAIN (int, ACE_TCHAR *[]) {

	ACS_CC_ReturnType returnCode;
	//AdminOperation_MyImplementer obj;
	char choice;
	string implName = "NomeImplementer";
	string objImplemented = "provaConfObj1,safApp=safImmService";
	string classImplemented = "Test_config";


	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;


	cout<<"INIT for Class Implementer-------"<<endl;

	/***/
	acs_apgcc_oihandler_V2 oiHandler;

	/**Define my Implementer**/
	//AdminOperation_MyImplementer obj(objImplemented, implName, ACS_APGCC_ONE );
	AdminOperation_MyImplementer obj;
	obj.setObjName(objImplemented);
	obj.setImpName(implName);
	obj.setScope(scope);

	std::cout << "IN objectImplementer " << std::endl;

	/**Initialize IMM and set MyImplementer in the map**/
	//returnCode = oiHandler.addObjectImpl(&obj);
	returnCode = oiHandler.addClassImpl(&obj, classImplemented.c_str() );
	if ( returnCode == ACS_CC_FAILURE ){
		std::cout << "ERROR: Set implementer '"<< implName <<"' for the object '"<< objImplemented <<"' failure " << std::endl;
		return -1;
	}


	std::cout << " - insert a char start Thread";

	std::cin >> choice;
	MyThread th(&obj);
	th.activate();



	std::cout << " - insert a char to stop thread and release";
	std::cin >> choice;

	th.stop();

	returnCode = oiHandler.removeClassImpl(&obj, classImplemented.c_str());
		if ( returnCode == ACS_CC_FAILURE ){
			std::cout << "ERROR: oiHandler.removeObjectImpl '"<< implName <<"' for the object '"<< objImplemented <<"' failure " << std::endl;
			return -1;
		}
	/************Here Reactor*******************/
//	std::cout << " - insert a char to start thread reactor";
//	std::cin >> choice;
//
//	MyClassImplementer obj2(objImplemented, implName, ACS_APGCC_ONE );
//
//	/**Initialize IMM and set MyImplementer in the map**/
//	returnCode = oiHandler.addClassImpl(&obj2, classImplemented.c_str());
//	if ( returnCode == ACS_CC_FAILURE ){
//		std::cout << "oiHandler.addClassImpl '"<< implName <<"' for the IMM class '"<< classImplemented <<"' failure " << std::endl;
//		return -1;
//	}
//
//		/**Instance a Reactor to handle the events**/
//		ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
//		ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;
//
//		reactor->open(1);
//
//		MyThread_Reactor thread_1(&obj2, reactor);
//		thread_1.activate();
//
//
//		std::cout << " - insert a char to stop thread and release";
//		std::cin >> choice;
//
//		//thread_1.stop();
//		obj2.reactor()->end_reactor_event_loop();
//		returnCode = oiHandler.removeClassImpl(&obj2, classImplemented.c_str());


	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	return 0;
}
