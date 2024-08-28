//============================================================================
// Name        : testMulipleClassImplementer.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_OiHandler.h"

#include "MyImplementer.h"
#include "MyImplementer_2.h"

#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"

#include "ace/Task.h"
#include <sys/poll.h>
#include "string.h"

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

using namespace std;

/*---------------------------------------------------------------------------------------------------------------------------*/
/*  GLOBAL VARIABLES  */
/*---------------------------------------------------------------------------------------------------------------------------*/
string nameClass_1("Test_config_1");
string nameClass_2("Test_config_2");
string nameClass_3("Test_config_3");


/*---------------------------------------------------------------------------------------------------------------------------*/
/*  METHOD DEFINITION  */
/*---------------------------------------------------------------------------------------------------------------------------*/
void createClassTest(OmHandler omHandler, const char *p_className);
void deleteClassTest(OmHandler omHandler, const char *p_className);
void createObjectTest(OmHandler omHandler, const char* p_className, const char *p_objName);
void deleteObjectTest(OmHandler omHandler, const char *p_objName);
void modifyObjectTest(OmHandler omHandler, const char *p_objName);


/*---------------------------------------------------------------------------------------------------------------------------*/
/*                   THREAD FOR DISPATCH CALLBACK USING REACTOR (ACS_APGCC_ObjectImplementer_EventHandler)                   */
/*---------------------------------------------------------------------------------------------------------------------------*/
class MyThread : public ACE_Task_Base {
public:

	MyThread(MyImplementer *p_user1, ACE_Reactor *p_reactor ){

		implementer = p_user1;
		reactor = p_reactor;

	}

	~MyThread(){}

	int svc(void){

		implementer->dispatch(reactor, ACS_APGCC_DISPATCH_ALL);

		reactor->run_reactor_event_loop();

		return 0;
	}
private:

	MyImplementer *implementer;
	ACE_Reactor *reactor;

};


/*---------------------------------------------------------------------------------------------------------------------------*/
/*                    THREAD FOR DISPATCH CALLBACK USING POLL (ACS_APGCC_ObjectImplementerInterface)                         */
/*---------------------------------------------------------------------------------------------------------------------------*/
class MyThread_2 : public ACE_Task_Base {
public :

	MyThread_2(MyImplementer_2 *p_user1){

		implementer = p_user1;
		isStop = false;

	}

	~MyThread_2(){}

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
				cout<<"FD THREAD: "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);
			}
		}

		return 0;
	}

private:
	MyImplementer_2 *implementer;
	bool isStop;

};


void test_MultipleClassImpl_1(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addMultipleClassImpl(...) and  ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a addMultipleClassImpl(...) and removeMultipleClassImpl(...) method to register and remove \n"
			" a implementer for more classes" << endl;
	cout << " RESULT: The process is registered as implementer of three classes and then is unregistered " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	MyImplementer implementer_1("impName_1");

	result = oiHandler.addMultipleClassImpl(&implementer_1, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;

	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor_2 = new ACE_Reactor(tp_reactor_impl) ;

	reactor_2->open(1);

	MyThread thread_2(&implementer_1, reactor_2);
	thread_2.activate();

	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"CREATE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objNameCreate_1 = "provaConf_1";
	createObjectTest(omHandler, nameClass_1.c_str(), objNameCreate_1);

	cout<<endl;

	const char *objNameCreate_2 = "provaConf_2";
	createObjectTest(omHandler, nameClass_2.c_str(), objNameCreate_2);

	cout<<endl;

	const char *objNameCreate_3 = "provaConf_3";
	createObjectTest(omHandler, nameClass_3.c_str(), objNameCreate_3);

	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"DELETE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objNameDelete_1 = "provaConf_1,safApp=safImmService";
	const char *objNameDelete_2 = "provaConf_2,safApp=safImmService";
	const char *objNameDelete_3 = "provaConf_3,safApp=safImmService";

	deleteObjectTest(omHandler, objNameDelete_1 );
	deleteObjectTest(omHandler, objNameDelete_2 );
	deleteObjectTest(omHandler, objNameDelete_3 );

	result = oiHandler.removeMultipleClassImpl(&implementer_1, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);

	implementer_1.reactor()->end_reactor_event_loop();

	result = omHandler.Finalize();

	delete reactor_2;
	delete tp_reactor_impl;

}


void test_MultipleClassImpl_2(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addMultipleClassImpl(...) and  ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a addMultipleClassImpl(...) and removeMultipleClassImpl(...) methods to register and remove \n"
			" a implementer for more classes" << endl;
	cout << " RESULT: The addMultipleClassImpl and  removeMultipleClassImpl failed because an implementer is already\n"
			" register on one of class with different name " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");
	std::string implName_2("impName_2");

	MyImplementer implementer_1(implName_1.c_str());
	MyImplementer implementer_2(implName_2.c_str());

	result = oiHandler.addClassImpl(&implementer_1, nameClass_3.c_str());
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer_1 registered for the class" << nameClass_3 << " whit name " << implName_1 <<endl;
	}else{
		cout << " -> ERROR: Impossible to register the implementer "<< implName_1 <<"for the class" << nameClass_3 <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addClassImpl ", result == ACS_CC_SUCCESS);

	result = oiHandler.addMultipleClassImpl(&implementer_2, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_FAILURE);

	cout<<endl;

	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor_2 = new ACE_Reactor(tp_reactor_impl) ;

	reactor_2->open(1);

	MyThread thread_2(&implementer_1, reactor_2);
	thread_2.activate();


	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"CREATE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objNameCreate_1 = "provaConf_1";
	createObjectTest(omHandler, nameClass_1.c_str(), objNameCreate_1);

	cout<<endl;

	const char *objNameCreate_2 = "provaConf_2";
	createObjectTest(omHandler, nameClass_2.c_str(), objNameCreate_2);

	cout<<endl;

	const char *objNameCreate_3 = "provaConf_3";
	createObjectTest(omHandler, nameClass_3.c_str(), objNameCreate_3);

	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"DELETE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objNameDelete_1 = "provaConf_1,safApp=safImmService";
	const char *objNameDelete_2 = "provaConf_2,safApp=safImmService";
	const char *objNameDelete_3 = "provaConf_3,safApp=safImmService";

	deleteObjectTest(omHandler, objNameDelete_1 );
	deleteObjectTest(omHandler, objNameDelete_2 );
	deleteObjectTest(omHandler, objNameDelete_3 );

	result = oiHandler.removeClassImpl(&implementer_1, nameClass_3.c_str());
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer "<< implName_1 <<" removed for the class " << nameClass_3 << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" << implName_1 <<" for the class " << nameClass_3 << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);

	result = oiHandler.removeMultipleClassImpl(&implementer_2, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_FAILURE);

	implementer_1.reactor()->end_reactor_event_loop();

	result = omHandler.Finalize();

	delete reactor_2;
	delete tp_reactor_impl;

}


void test_MultipleClassImpl_3(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addMultipleClassImpl(...) and  ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a addMultipleClassImpl(...) and removeMultipleClassImpl(...) methods to register and remove \n"
			" a implementer for more classes" << endl;
	cout << " RESULT: The addMultipleClassImpl and  removeMultipleClassImpl failed because an implementer is already\n"
			" register on one of class with the same name " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");
	std::string implName_2("impName_1");

	MyImplementer implementer_1(implName_1.c_str());
	MyImplementer implementer_2(implName_1.c_str());

	result = oiHandler.addClassImpl(&implementer_1, nameClass_3.c_str());
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer_1 registered for the class" << nameClass_3 << " whit name " << implName_1 <<endl;
	}else{
		cout << " -> ERROR: Impossible to register the implementer "<< implName_1 <<"for the class" << nameClass_3 <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addClassImpl ", result == ACS_CC_SUCCESS);

	result = oiHandler.addMultipleClassImpl(&implementer_2, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_FAILURE);

	cout<<endl;

	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor_2 = new ACE_Reactor(tp_reactor_impl) ;

	reactor_2->open(1);

	MyThread thread_2(&implementer_1, reactor_2);
	thread_2.activate();


	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"CREATE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objNameCreate_1 = "provaConf_1";
	createObjectTest(omHandler, nameClass_1.c_str(), objNameCreate_1);

	cout<<endl;

	const char *objNameCreate_2 = "provaConf_2";
	createObjectTest(omHandler, nameClass_2.c_str(), objNameCreate_2);

	cout<<endl;

	const char *objNameCreate_3 = "provaConf_3";
	createObjectTest(omHandler, nameClass_3.c_str(), objNameCreate_3);

	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"DELETE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objNameDelete_1 = "provaConf_1,safApp=safImmService";
	const char *objNameDelete_2 = "provaConf_2,safApp=safImmService";
	const char *objNameDelete_3 = "provaConf_3,safApp=safImmService";

	deleteObjectTest(omHandler, objNameDelete_1 );
	deleteObjectTest(omHandler, objNameDelete_2 );
	deleteObjectTest(omHandler, objNameDelete_3 );

	result = oiHandler.removeClassImpl(&implementer_1, nameClass_3.c_str());
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer "<< implName_1 <<" removed for the class " << nameClass_3 << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" << implName_1 <<" for the class " << nameClass_3 << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);

	result = oiHandler.removeMultipleClassImpl(&implementer_2, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_FAILURE);

	implementer_1.reactor()->end_reactor_event_loop();

	result = omHandler.Finalize();

	delete reactor_2;
	delete tp_reactor_impl;

}


void test_MultipleClassImpl_4(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addMultipleClassImpl(...) and  ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a addMultipleClassImpl(...) and removeMultipleClassImpl(...) methods to register and remove \n"
			" a implementer for more classes" << endl;
	cout << " RESULT: The addMultipleClassImpl and  removeMultipleClassImpl failed because an implementer is already\n"
			" register on one of class with the same name " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");

	MyImplementer_2 implementer(implName_1.c_str());

	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/*Start the thread to dispatch callback*/
	MyThread_2 th(&implementer);
	th.activate();

	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"CREATE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objNameCreate_1 = "provaConf_1";
	createObjectTest(omHandler, nameClass_1.c_str(), objNameCreate_1);

	cout<<endl;

	const char *objNameCreate_2 = "provaConf_2";
	createObjectTest(omHandler, nameClass_2.c_str(), objNameCreate_2);

	cout<<endl;

	const char *objNameCreate_3 = "provaConf_3";
	createObjectTest(omHandler, nameClass_3.c_str(), objNameCreate_3);

	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"MODIFY OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	const char *objName_1 = "provaConf_1,safApp=safImmService";
	const char *objName_2 = "provaConf_2,safApp=safImmService";
	const char *objName_3 = "provaConf_3,safApp=safImmService";

	modifyObjectTest(omHandler, objName_1);
	modifyObjectTest(omHandler, objName_2);
	modifyObjectTest(omHandler, objName_3);

	cout<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"DELETE OBJECT"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;



	deleteObjectTest(omHandler, objName_1 );
	deleteObjectTest(omHandler, objName_2 );
	deleteObjectTest(omHandler, objName_3 );

	th.stop();

	sleep(1);

	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);


	result = omHandler.Finalize();
}


void test_addMultipleClassImpl_1(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addMultipleClassImpl(...)" << endl;
	cout << " TEST: call a addMultipleClassImpl(...) method to register a implementer for more classes" << endl;
	cout << " RESULT: addMultipleClassImpl failed because the pointer to ACS_APGCC_ObjectImplementer is NULL " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");

	MyImplementer_2 implementer(implName_1.c_str());

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	result = oiHandler.addMultipleClassImpl(NULL, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_FAILURE);


	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;


	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);

	result = omHandler.Finalize();
}


void test_addMultipleClassImpl_2(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addMultipleClassImpl(...)" << endl;
	cout << " TEST: call a addMultipleClassImpl(...) method to register a implementer for more classes" << endl;
	cout << " RESULT: addMultipleClassImpl failed because a list of class passed as parameter at the function is empty " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");

	MyImplementer_2 implementer(implName_1.c_str());

	vector<string> classNames;

	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_FAILURE);


	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;


	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);

	result = omHandler.Finalize();
}


void test_removeMultipleClassImpl_1(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a removeMultipleClassImpl(...) method to unregister a implementer for more classes" << endl;
	cout << " RESULT: RemoveMultipleClassImpl failed because a list of class passed as parameter at the function is empty " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");

	MyImplementer_2 implementer(implName_1.c_str());

	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;

	classNames.clear();

	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_FAILURE);

	classNames.push_back("pippo");
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);


	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_FAILURE);

	classNames.clear();

	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);

	result = omHandler.Finalize();
}


void test_removeMultipleClassImpl_2(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a removeMultipleClassImpl(...) method to unregister a implementer for more classes" << endl;
	cout << " RESULT: RemoveMultipleClassImpl failed because the pointer to ACS_APGCC_ObjectImplementer is NULL " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");

	MyImplementer_2 implementer(implName_1.c_str());

	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;

	result = oiHandler.removeMultipleClassImpl(NULL, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_FAILURE);

	cout<<endl;

	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);


	result = omHandler.Finalize();
}


void test_removeMultipleClassImpl_3(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a removeMultipleClassImpl(...) method to unregister a implementer for more classes" << endl;
	cout << " RESULT: RemoveMultipleClassImpl failed because a list of class passed as parameter at the function is empty " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");

	MyImplementer_2 implementer(implName_1.c_str());

	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;

	classNames.clear();

	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_FAILURE);

	cout<<endl;

	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);

	result = omHandler.Finalize();
}


void test_removeMultipleClassImpl_4(){

	cout << " -----------------------------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::removeMultipleClassImpl(...)" << endl;
	cout << " TEST: call a removeMultipleClassImpl(...) method to unregister a implementer for more classes" << endl;
	cout << " RESULT: RemoveMultipleClassImpl failed because a list of class passed as parameter is empty wrong " << endl;
	cout << " --------------------------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	vector<string> classNames;
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	ACS_APGCC_OiHandler oiHandler;

	std::string implName_1("impName_1");

	MyImplementer_2 implementer(implName_1.c_str());

	result = oiHandler.addMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered for the class:" << endl;
		for (int i=0; i<(int)classNames.size(); i++){
			cout<<" "<<i+1<<" "<<classNames[i]<<endl;
		}
	}else{
		cout << " -> ERROR: Impossible to register the implementer for the classes" << endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::addMultipleClassImpl ", result == ACS_CC_SUCCESS);

	cout<<endl;

	classNames.clear();

	classNames.push_back("pippo");
	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);


	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_FAILURE);

	classNames.clear();

	cout<<endl;

	classNames.push_back(nameClass_1);
	classNames.push_back(nameClass_2);
	classNames.push_back(nameClass_3);

	result = oiHandler.removeMultipleClassImpl(&implementer, classNames);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}else{
		cout << "ERROR: Impossible to remove the Implementer" <<endl;
	}

	ASSERTM("ACS_APGCC_OiHandler::removeMultipleClassImpl ", result == ACS_CC_SUCCESS);

	result = omHandler.Finalize();
}




//-------------------------------------------------------------------------------------------------------------------------
// Functions implementation
//-------------------------------------------------------------------------------------------------------------------------
void createClassTest(OmHandler omHandler, const char *p_className){

	cout << " -> Try to create a new class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char*>("ATTRIBUTE_1"), ATTR_FLOATT, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char*>("ATTRIBUTE_2"),ATTR_INT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("ATTRIBUTE_3"),ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {const_cast<char*>("ATTRIBUTE_4"),ATTR_DOUBLET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {const_cast<char*>("ATTRIBUTE_5"),ATTR_UINT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {const_cast<char*>("ATTRIBUTE_6"),ATTR_UINT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeNAME = {const_cast<char*>("ATTRIBUTE_7"), ATTR_NAMET, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char*>("ATTRIBUTE_8"),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char*>("ATTR_TIME"), ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeUnI32);
	classAttributes.push_back(attributeI64);
	classAttributes.push_back(attributeUnI64);
	classAttributes.push_back(attributeTIME);
	classAttributes.push_back(attributeNAME);
	classAttributes.push_back(attributeFLOAT);
	classAttributes.push_back(attributeDOUBLE);
	classAttributes.push_back(attributeSTRING);

	result = omHandler.defineClass( const_cast<char *>(p_className),categoryClassToBeDefined,classAttributes);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Class " << p_className << " created" << endl;
		cout << endl;
	}else{
		cout << " -> Create class  " << p_className << " failure" << endl;
		cout << " -> ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << " -> ERROR MESSAGE: "<< omHandler.getInternalLastErrorText() << endl;
	}

}



//------------------------------------------------------------------------------------------------
// Create object test
//------------------------------------------------------------------------------------------------
void createObjectTest(OmHandler omHandler, const char* p_className, const char *p_objName){



	cout << endl ;

	ACS_CC_ReturnType result;

	//char nameClass[20] = "Test_config";
	char* nomeParent = const_cast<char *>("safApp=safImmService");

	cout << " -> Try to create the object " << p_objName <<","<< nomeParent <<" instance of class "<< p_className <<endl;

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;

	/*Fill the rdn Attribute */
	char attrdn[]= "RDN_Attribute";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_NAMET;
	attrRdn.attrValuesNum = 1;
	char* rdnValue = const_cast<char *>(p_objName);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attrRdn.attrValues = value;


	/*Fill the ATTRIBUTE_1 */
	char name_attFloat[]="ATTRIBUTE_1";
	attr2.attrName = name_attFloat;
	attr2.attrType = ATTR_FLOATT;
	attr2.attrValuesNum = 1;
	float floatValue = 222.9;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValue)};
	attr2.attrValues = valueFloat;


	/*Fill the ATTRIBUTE_6 */
	char name_attUint32[]="ATTRIBUTE_6";
	attr3.attrName = name_attUint32;
	attr3.attrType = ATTR_UINT32T;
	attr3.attrValuesNum = 1;
	unsigned int intValueU32 = 123;
	void* valueInt[1]={ reinterpret_cast<void*>(&intValueU32)};
	attr3.attrValues = valueInt;


	char name_attrSaString[]= "ATTRIBUTE_8";
	attr4.attrName = name_attrSaString;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 1;
	char* stringValue = const_cast<char*>("pippo");
	void* valueStr[1]={ reinterpret_cast<void*>(stringValue) };
	attr4.attrValues = valueStr;

	/**/
	AttrList.push_back(attrRdn);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);

	result = omHandler.createObject(p_className, nomeParent, AttrList );

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << "," << nomeParent << " created" << endl;
		cout << endl;
	}else{
		cout << " -> Create object  " << p_objName << "," << nomeParent << " failure" << endl;
		cout << " -> ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << " -> ERROR MESSAGE: "<< omHandler.getInternalLastErrorText() << endl;
	}

}


//-------------------------------------------------------------------------------------------------
//  Delete object test
//-------------------------------------------------------------------------------------------------
void deleteObjectTest(OmHandler omHandler, const char *p_objName){

	cout << " -> Try to delete a object " << p_objName <<endl;
	cout << endl;

	ACS_CC_ReturnType result;

	result = omHandler.deleteObject(const_cast<char*>(p_objName));

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << " deleted" << endl;
		cout << endl;
	}else{
		cout << " -> Delete object  " << p_objName << " failure" << endl;
		cout << " -> ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << " -> ERROR MESSAGE: "<< omHandler.getInternalLastErrorText() << endl;
	}

}


//-----------------------------------------------------------------------------------------------------
// Delete class Test
//-----------------------------------------------------------------------------------------------------
void deleteClassTest(OmHandler omHandler, const char *p_className){

	cout << " -> Try to delete a class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;


	result = omHandler.removeClass(const_cast<char *>(p_className));

	if( result == ACS_CC_SUCCESS ){
		cout<< " -> Class " << p_className << " deleted." << endl;
		cout<<endl;
	}else{
		cout << " -> Delete class  " << p_className << " failure" << endl;
		cout << " -> ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << " -> ERROR MESSAGE: "<< omHandler.getInternalLastErrorText() << endl;
	}
}



//-----------------------------------------------------------------------------------------------------
// Modify Attribute class Test
//-----------------------------------------------------------------------------------------------------
void modifyObjectTest(OmHandler omHandler, const char *p_objName){


	/*Modify ATTRIBUTE_1 */
	float valueFlo = 1.3;
	char name_attFloat[] = "ATTRIBUTE_1";

	ACS_CC_ImmParameter par;
	par.attrName = name_attFloat;
	par.attrType = ATTR_FLOATT;
	par.attrValuesNum = 1;
	void* valueFloat[1] = { reinterpret_cast<void*>(&valueFlo) };
	par.attrValues = valueFloat;



	/*Modify ATTRIBUTE_6 */
	char name_attUint32[]="ATTRIBUTE_6";
	unsigned int intValueU32 = 456;

	ACS_CC_ImmParameter par_2;
	par_2.attrName = name_attUint32;
	par_2.attrType = ATTR_UINT32T;
	par_2.attrValuesNum = 1;
	void* valueInt[1]={ reinterpret_cast<void*>(&intValueU32)};
	par_2.attrValues = valueInt;


	/*Modify ATTRIBUTE_8 */
	char name_attrSaString[]= "ATTRIBUTE_8";
	char* stringValue = const_cast<char*>("modify_pippo");

	ACS_CC_ImmParameter par_3;
	par_3.attrName = name_attrSaString;
	par_3.attrType = ATTR_STRINGT;
	par_3.attrValuesNum = 1;
	void* valueStr[1] = { reinterpret_cast<void*>(stringValue) };
	par_3.attrValues = valueStr;


	cout<<"NELLA MODIFY OBJECT"<<endl;

	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTRIBUTE_1 )", omHandler.modifyAttribute(p_objName, &par)==ACS_CC_SUCCESS);
	cout<<"MODIFICHE 1 EFFETTUATE"<<endl;

	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTRIBUTE_8 )", omHandler.modifyAttribute(p_objName, &par_2)==ACS_CC_SUCCESS);
	cout<<"MODIFICHE 2 EFFETTUATE"<<endl;

	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTRIBUTE_7 )", omHandler.modifyAttribute(p_objName, &par_3)==ACS_CC_SUCCESS);

	cout<<"MODIFICHE 3 EFFETTUATE"<<endl;

}



void runSuite(){
	cute::suite s;

	s.push_back(CUTE(test_MultipleClassImpl_1));
	s.push_back(CUTE(test_MultipleClassImpl_2));
	s.push_back(CUTE(test_MultipleClassImpl_3));
	s.push_back(CUTE(test_MultipleClassImpl_4));

	s.push_back(CUTE(test_addMultipleClassImpl_1));
	s.push_back(CUTE(test_addMultipleClassImpl_2));

	s.push_back(CUTE(test_removeMultipleClassImpl_1));
	s.push_back(CUTE(test_removeMultipleClassImpl_2));
	s.push_back(CUTE(test_removeMultipleClassImpl_3));
	s.push_back(CUTE(test_removeMultipleClassImpl_4));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}


int main() {

	cout << "!!!Multiple Class Implementer CUTE Test START!!!" << endl; // prints !!!Hello World!!!

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	/**create a class **/
	createClassTest(omHandler, nameClass_1.c_str());

	createClassTest(omHandler, nameClass_2.c_str());

	createClassTest(omHandler, nameClass_3.c_str());

	runSuite();

	/**delete a class **/
	deleteClassTest(omHandler, nameClass_1.c_str());

	deleteClassTest(omHandler, nameClass_2.c_str());

	deleteClassTest(omHandler, nameClass_3.c_str());

	result = omHandler.Finalize();


	cout << "!!!Multiple Class Implementer CUTE Test END!!!" << endl;

	return 0;
}

