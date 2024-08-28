/*
 * AdminOperation_Implementer_Main.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */



#include <iostream>

#include "ace/Task.h"
#include <sys/poll.h>

#include "AdminOperation_MyImplementer.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "ACS_APGCC_OiHandler.h"
#include "MyClassImplementer.h"
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

using namespace std;


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


		//cout<<"Thread AVVIATO"<<endl;

		int ret;

		//cout<<"getSelObj: "<<implementer->getSelObj()<<endl;

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

				//cout<<"FD THREAD: "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		//cout<<"Thread TERMINATO"<<endl;

		return 0;
	}

private:

	AdminOperation_MyImplementer *implementer;

	bool isStop;

};


int main ( ) {

	ACS_CC_ReturnType returnCode;
	AdminOperation_MyImplementer obj;

	char choice;

	string implName = "AdminImplementer";
	string objImplemented = "object1,safApp=safImmService";
	string classImplemented = "Test_config";


	cout<<"INIT-------"<<endl;

	returnCode = obj.init(implName);
	if (returnCode != ACS_CC_SUCCESS){

		cout<<"INIT FAILURE"<<endl;
		cout<<"obj.getLastError()"<<obj.getInternalLastError()<<endl;
		cout<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
		return -1;
	}



	std::cout << " - insert C char to be Implementer of class "<<classImplemented<<endl;
	std::cout << " - insert O char to be Implementer of object "<<objImplemented<<endl;
	std::cin >> choice;

	if (choice == 'O'){
		std::cout << " - Object Implementer";

		returnCode = obj.setObjectImplemented(objImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"FAILURE::obj.setObjectImplemented"<<endl;
			cout<<"obj.getLastError()"<<obj.getInternalLastError()<< endl;
			cout<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}

	}else if (choice == 'C'){
		std::cout << " - Class Implementer";

		returnCode = obj.setClassImplemented(classImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"FAILURE::obj.setClasstImplemented"<<endl;
			cout<<"obj.getLastError()"<<obj.getInternalLastError()<< endl;
			cout<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}


	}else {
		cout << "ERROR: Option not recognize!!! " <<  endl;
		return -1;
	}

	/********************************/


	MyThread th(&obj);

	th.activate();

	char newChoice;

	std::cout << " - insert a char to stop thread and release";
	std::cin >> newChoice;



	if (choice == 'O'){

		returnCode = obj.releaseObjectImplemented(objImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"obj.releaseObjectImplemented"<<endl;
			cout<<"obj.releaseObjectImplemented"<<obj.getInternalLastError()<< endl<<"obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}


	}else if (choice == 'C'){

		returnCode = obj.releaseClassImplemented(classImplemented);

		if (returnCode != ACS_CC_SUCCESS){

			cout<<"obj.releaseClassImplemented"<<endl;
			cout<<"obj.releaseClassImplemented"<<obj.getInternalLastError()<< endl<< "obj.getLastErrorText()"<<obj.getInternalLastErrorText()<<endl;
			return -1;
		}

	}
	th.stop();
	std::cout << " - insert a char to exit "<<endl;
	std::cin >> newChoice;


	return 0;
}
