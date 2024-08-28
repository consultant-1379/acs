//============================================================================
// Name        : ut_aeh_error.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "acs_aeh_communicationhandler.h"
#include "acs_aeh_eventmsg.h"

using namespace std;


char * p_nodeStatus = const_cast<char*>("ACTIVE");
char * p_userName = const_cast<char*>("xfabron");
char * p_processName = const_cast<char*>("ut_eventmsg");
long p_specificProblem = 1402;
char * p_percSeverity = const_cast<char*>("A2");
char * p_probableCause = const_cast<char*>("File Open Error in AP");
char * p_objClassOfReference = const_cast<char*>("APZ");
char * p_objectOfReference = const_cast<char*>("DISC");
char * p_problemData = const_cast<char*>("problem opening for write");
char * p_problemText = const_cast<char*>("VOLUME  \tLIM\nDISC/1 \t95 "); ;
bool p_manualCease = true;


char * p_processName_2 = const_cast<char*>("ACS_SSU_Monitor:3612");
long p_specificProblem_2 = 1402;
char * p_percSeverity_2 = const_cast<char*>("EVENT");
char * p_probableCause_2 = const_cast<char*>("AP INTERNAL FAULT");
char * p_objClassOfReference_2 = const_cast<char*>("APZ");
char * p_objectOfReference_2 = const_cast<char*>(" ");
char * p_problemData_2 = const_cast<char*>("Failed to create Folder Quota for directory K:\\MCS "
		"in the File Server Resource Manager due to the following error: A File Server Resource Manager XML "
		"configuration file or import-export file is corrupted.");
char * p_problemText_2 = const_cast<char*>("INTERNAL SSU SYSTEM SUPERVISOR PROBLEM"); ;
bool p_manualCease_2 = false;


char * p_processName_3 = const_cast<char*>("ACS_PRC_ClusterControl:435");
long p_specificProblem_3 = 6022;
char * p_percSeverity_3 = const_cast<char*>("A1");
char * p_probableCause_3 = const_cast<char*>("AP Process STOPPED");
char * p_objClassOfReference_3 = const_cast<char*>("APZ");
char * p_objectOfReference_3 = const_cast<char*>("clusSvc");
char * p_problemData_3 = const_cast<char*>("The AP process has been stopped");
char * p_problemText_3 = const_cast<char*>("RESOURCE GROUP\tPROCESS\nLBB\tclustSvc\nCAUSE\tDATA\tTIME\nProcess death\t20111013\t195511\n"); ;
bool p_manualCease_3 = true;


void test_sendMsg_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendMsg_1()" << endl;
	cout << " TEST: send to the pipe a event message" << endl;
	cout << " RESULT: the event message is correctly stored in the event log \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	acs_aeh_communicationhandler com;
	int retValue = -1;


	retValue = com.open();
	ASSERT_EQUALM("CUTE: check return value of open() method", retValue , 0);

	retValue = com.sendMsg(message);
	ASSERT_EQUALM("CUTE: check return value of sendMsg() method", retValue , 0);

	retValue = com.close();
	ASSERT_EQUALM("CUTE: check return value of close() method", retValue , 0);


}

void test_sendMsg_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendMsg_2()" << endl;
	cout << " TEST: send to the pipe a event message without to call the open method" << endl;
	cout << " RESULT: the event message not is stored in the event log \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	acs_aeh_communicationhandler com;
	int retValue = -100;


	retValue = com.sendMsg(message);
	ASSERT_EQUALM("CUTE: check return value of sendMsg() method", retValue , -1);
	cout<<"ERROR event message not send"<<endl;
	cout<<"ERROR CODE: "<<com.getErrorCode()<<endl;
	cout<<"ERROR TEXT: "<<com.getErrorString()<<endl;


}


void test_close_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_close_1()" << endl;
	cout << " TEST: call the method close without to call the open method" << endl;
	cout << " RESULT: a error code is returned \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	acs_aeh_communicationhandler com;
	int retValue = -100;


	retValue = com.close();
	ASSERT_EQUALM("CUTE: check return value of close() method", retValue, -1);
	cout<<"ERROR event message not send"<<endl;
	cout<<"ERROR CODE: "<<com.getErrorCode()<<endl;
	cout<<"ERROR TEXT: "<<com.getErrorString()<<endl;

}

void test_sendMsg_3(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendMsg_3()" << endl;
	cout << " TEST: call the method sendMsg to write a buffer that exceed the maximum size" << endl;
	cout << " RESULT: a error code is returned \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_communicationhandler com;
	int retValue = -100;

	int size = 9000;
	char problemText_W[size];

	for( int i=0; i<size-1; i++){
		problemText_W[i]='0';
	}

	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, problemText_W, p_manualCease);

	retValue = com.open();
	ASSERT_EQUALM("CUTE: check return value of open() method", retValue , 0);

	retValue = com.sendMsg(message);
	ASSERT_EQUALM("CUTE: check return value of sendMsg() method", retValue , -1);
	cout<<"ERROR event message not send"<<endl;
	cout<<"ERROR CODE: "<<com.getErrorCode()<<endl;
	cout<<"ERROR TEXT: "<<com.getErrorString()<<endl;


	retValue = com.close();
	ASSERT_EQUALM("CUTE: check return value of close() method", retValue, 0);

}

void runSuite(){
	cute::suite s;
	//TODO add your test here

	s.push_back(CUTE(test_sendMsg_1));
	s.push_back(CUTE(test_sendMsg_2));
	s.push_back(CUTE(test_sendMsg_3));
	s.push_back(CUTE(test_close_1));


	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test");


}


int main() {


	cout << "Start Test acs_aeh_communicationhandler class" << endl;
	runSuite();
	cout << "End Test acs_aeh_communicationhandler class " << endl;
	return 0;
}
