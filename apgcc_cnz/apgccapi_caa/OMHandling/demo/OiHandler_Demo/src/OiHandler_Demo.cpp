//============================================================================
// Name        : OiHandler_Demo.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "ACS_APGCC_OiHandler.h"
#include "MyImplementer.h"

#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"

#include <iostream>


class MyThread : public ACE_Task_Base {

public:
	MyThread(MyImplementer *p_implementer, ACE_Reactor *p_reactor){

		implementer = p_implementer;
		reactor = p_reactor;
	}

	~MyThread(){};

	int svc(void){

		implementer->dispatch(reactor, ACS_APGCC_DISPATCH_ALL);

		reactor->run_reactor_event_loop();

		return 0;
	}


private:
	MyImplementer *implementer;
	ACE_Reactor *reactor;
};

int printMenu();
int classImplementer(const char* implName, const char* className);
int objectImplementer(const char* implName, const char* objName);

int printMenu(){

	int value;

	std::cout<<"Select an operation: "
			<<"\n 1. Set an Object Implementer for a IMM class "
			<<"\n 2. Set an Object Implementer for a IMM object"
			<<"\n - ";

	std::cin >> value;

	return value;
}


int classImplementer(const char* implName, const char* className){

	ACS_CC_ReturnType errorCode;

	ACS_APGCC_OiHandler oiHandler;

	char choice;
	bool exit = false;

	/**Define my Implementer**/
	MyImplementer implementer(implName);

	/**Initialize IMM and set MyImplementer in the map**/
	errorCode = oiHandler.addClassImpl(&implementer, className);
	if ( errorCode == ACS_CC_FAILURE ){
		std::cout << "ERROR: Set implementer '"<< implName <<"' for the IMM class '"<< className <<"' failure " << std::endl;
		return -1;
	}

	/**Instance a Reactor to handle the events**/
	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer, reactor);
	thread_1.activate();

	std::cout << "Implementer run, attends change request... " << std::endl;
	std::cout << "Press 'r' to release Implementer " << std::endl;

	do{
		std::cout << " - ";
		std::cin >> choice;
		if(choice == 'r'){
			implementer.reactor()->end_reactor_event_loop();
			errorCode = oiHandler.removeClassImpl(&implementer, className);
			if ( errorCode == ACS_CC_FAILURE ){
				std::cout << "ERROR: Release implementer '"<< implName <<"' for the IMM class '"<< className <<"' failure " << std::endl;
				return -1;
			} else {
				std::cout << implName <<" no longer implementer for the class " << className << std::endl;
			}

			exit = true;
		}else {
			std::cout << "Value not correct, retry...\n";
		}
	}while(!exit);

	exit = false;
	std::cout << "\nDigit 'q' to exit " << std::endl;
	do{
		std::cout << " - ";
		std::cin >> choice;
		if(choice == 'q'){
			exit = true;
		}else {
			std::cout << "Value not correct, retry...\n";
		}
	}while(!exit);

	return 0;

}


int objectImplementer(const char* implName, const char* objName){

	ACS_CC_ReturnType errorCode;

	ACS_APGCC_OiHandler oiHandler;

	char choice;
	bool exit = false;

	/**Define my Implementer**/
	MyImplementer implementer(objName, implName, ACS_APGCC_ONE );

	/**Initialize IMM and set MyImplementer in the map**/
	errorCode = oiHandler.addObjectImpl(&implementer);
	if ( errorCode == ACS_CC_FAILURE ){
		std::cout << "ERROR: Set implementer '"<< implName <<"' for the object '"<< objName <<"' failure " << std::endl;
		return -1;
	}

	/**Instance a Reactor to handle the events**/
	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer, reactor);
	thread_1.activate();

	std::cout << "Implementer run, attends change request... " << std::endl;
	std::cout << "Press 'r' to release Implementer " << std::endl;

	do{
		std::cout << " - ";
		std::cin >> choice;
		if(choice == 'r'){
			implementer.reactor()->end_reactor_event_loop();
			errorCode = oiHandler.removeObjectImpl(&implementer);
			if ( errorCode == ACS_CC_FAILURE ){
				std::cout << "ERROR: Release implementer '"<< implName <<"' for the object '"<< objName <<"' failure " << std::endl;
				return -1;
			} else {
				std::cout << implName <<" no longer implementer for the object " << objName << std::endl;
			}
			exit = true;
		}else {
			std::cout << "Value not correct, retry...\n";
		}
	}while(!exit);

	exit = false;
	std::cout << "\nDigit 'q' to exit " << std::endl;
	do{
		std::cout << " - ";
		std::cin >> choice;
		if(choice == 'q'){
			exit = true;
		}else {
			std::cout << "Value not correct, retry...\n";
		}
	}while(!exit);

	return 0;

}


int main() {

	std::cout<<"*--------------------------------------------------------------------*\n"
			 <<"*                                                                    *\n"
			 <<"*                  IMM Object Implementer DEMO                       *\n"
			 <<"*                                                                    *\n"
			 <<"*--------------------------------------------------------------------*\n"
			 <<endl;

	int value;
	char className[30];
	char implName[30];
	char dn[60];

	value = printMenu();

	switch ( value ){
	case 1:
		std::cout << "Object Implementer for IMM Class " << std::endl;
		std::cout << "Insert the Implementer Name:\n "
				  << " - ";
		std::cin >> implName;

		std::cout << endl;

		std::cout << "Insert the name of IMM class:\n "
				  << " - ";
		std::cin >> className;

		std::cout << endl;

		classImplementer(implName, className);


		break;
	case 2:
		cout << "Object Implementer for IMM Object " << endl;
		std::cout << "Insert the Implementer Name:\n "
				  << " - ";
		std::cin >> implName;

		std::cout << endl;

		std::cout << "Insert the distinguished name of IMM object :\n "
				  << " - ";
		std::cin >> dn;

		std::cout << endl;

		objectImplementer(implName, dn);

		break;
	default:
		cout << "ERROR: incorrect value " << endl;
		return -1;
	}

	return 0;
}
