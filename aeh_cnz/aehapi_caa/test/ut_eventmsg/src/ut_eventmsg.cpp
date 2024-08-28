/*
 * ut_eventmsg.cpp
 *
 *  Created on: May 18, 2011
 *      Author: xfabron
 */

#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "acs_aeh_eventmsg.h"
#include "acs_aeh_communicationhandler.h"


using namespace std;

char * p_priority = const_cast<char*>("notice");
char * p_nodeName = const_cast<char*>("SC-2-1");
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




void print_utility(acs_aeh_eventmsg &message){

	cout<<"Node Status: "<<message.getNodeStatus()<<endl;
	cout<<"User name: "<<message.getUserName()<<endl;
	cout<<"Process name: "<<message.getProcessName()<<endl;
	cout<<"Specific problem: "<<message.getSpecificProblem()<<endl;
	cout<<"Perceived severity: "<<message.getPercSeverity()<<endl;
	cout<<"Probable cause: "<<message.getProbableCause()<<endl;
	cout<<"Object class of reference: "<<message.getObjClassOfReference()<<endl;
	cout<<"Object of reference: "<<message.getObjectOfReference()<<endl;
	cout<<"Problem data: "<<message.getProblemData()<<endl;
	cout<<"Problem text: "<<message.getProblemText()<<endl;
	cout<<"Manual Cease: "<<message.getManualCease()<<endl;

}


void test_constructor_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_constructor_1()" << endl;
	cout << " TEST: test the class constructor" << endl;
	cout << " RESULT: all the values are correctly assigned. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(p_processName, message.getProcessName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Specific problem is correct", p_specificProblem , message.getSpecificProblem());
	ASSERT_EQUALM("CUTE: check if value Perceived severity is correct", strcmp(p_percSeverity, message.getPercSeverity().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Probable cause is correct", strcmp(p_probableCause, message.getProbableCause().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is correct", strcmp(p_objClassOfReference, message.getObjClassOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object of reference is correct", strcmp(p_objectOfReference, message.getObjectOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is correct", strcmp(p_problemData, message.getProblemData().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is correct", strcmp(p_problemText, message.getProblemText().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Manual Cease of reference is correct", p_manualCease , message.getManualCease());

	cout<<endl;

	print_utility(message);

	cout<<endl;

	cout<<message.getEventMessage()<<endl;
	acs_aeh_communicationhandler com;

	cout<<com.open()<<endl;
	cout<<com.sendMsg(message)<<endl;
	cout<<com.close()<<endl;

	acs_aeh_eventmsg message_2(p_processName_2, p_specificProblem_2,
					p_percSeverity_2, p_probableCause_2, p_objClassOfReference_2,
					p_objectOfReference_2, p_problemData_2, p_problemText_2, p_manualCease_2);


	cout<<endl;

	print_utility(message);

	cout<<endl;

	cout<<message_2.getEventMessage()<<endl;


	cout<<com.open()<<endl;
	cout<<com.sendMsg(message_2)<<endl;
	cout<<com.close()<<endl;




}


void test_constructor_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_constructor_2()" << endl;
	cout << " TEST: test the class constructor (as parameter are passed argument null or not initialized)" << endl;
	cout << " RESULT: all the values are correctly assigned. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	char * name = 0;
	char * severity = 0;
	char * probableCause = 0;
	char * classOfReference;
	char * objectOfReference;
	char * problemData;
	char * problemText;

	acs_aeh_eventmsg message(name, p_specificProblem,
				severity, probableCause, classOfReference,
				objectOfReference, problemData, problemText, p_manualCease);


	ASSERT_EQUALM("CUTE: check if value Process name is empty", message.getProcessName().empty() , true);
	ASSERT_EQUALM("CUTE: check if value Perceived severity is empty", message.getPercSeverity().empty() , true);
	ASSERT_EQUALM("CUTE: check if value Probable cause is empty",message.getProbableCause().empty(), true);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is empty", message.getObjClassOfReference().empty() , true);
	ASSERT_EQUALM("CUTE: check if value Object of reference is empty", message.getObjectOfReference().empty(), true);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is empty", message.getProblemData().empty(), true);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is empty", message.getProblemText().empty(), true);

	cout<<endl;
	cout<<"Field of Event message"<<endl;
	print_utility(message);

	cout<<endl;

	cout<<message.getEventMessage()<<endl;
}


void test_constructor_3(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_constructor_3()" << endl;
	cout << " TEST: test the class constructor" << endl;
	cout << " RESULT: all the values are correctly assigned. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message(p_priority, p_nodeName, p_nodeStatus, p_userName,  p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	ASSERT_EQUALM("CUTE: check if value priority is correct", strcmp(p_priority, message.getPriorityMsg().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Node name is correct", strcmp(p_nodeName, message.getNodeName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Node status is correct", strcmp(p_nodeStatus, message.getNodeStatus().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value user name is correct", strcmp(p_userName, message.getUserName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(p_processName, message.getProcessName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Specific problem is correct", p_specificProblem , message.getSpecificProblem());
	ASSERT_EQUALM("CUTE: check if value Perceived severity is correct", strcmp(p_percSeverity, message.getPercSeverity().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Probable cause is correct", strcmp(p_probableCause, message.getProbableCause().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is correct", strcmp(p_objClassOfReference, message.getObjClassOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object of reference is correct", strcmp(p_objectOfReference, message.getObjectOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is correct", strcmp(p_problemData, message.getProblemData().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is correct", strcmp(p_problemText, message.getProblemText().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Manual Cease of reference is correct", p_manualCease , message.getManualCease());

	cout<<endl;

	print_utility(message);

	cout<<endl;

	cout<<message.getEventMessage()<<endl;

}



void test_copyConstructor_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_copyConstructor_1()" << endl;
	cout << " TEST: test the copy constructor" << endl;
	cout << " RESULT: the two object are equals\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message_1(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	acs_aeh_eventmsg message_2(message_1);


	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(message_1.getProcessName().c_str(), message_2.getProcessName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Specific problem is correct", message_1.getSpecificProblem() , message_2.getSpecificProblem());
	ASSERT_EQUALM("CUTE: check if value Perceived severity is correct", strcmp(message_1.getPercSeverity().c_str(), message_2.getPercSeverity().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Probable cause is correct", strcmp(message_1.getProbableCause().c_str(), message_2.getProbableCause().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is correct", strcmp(message_1.getObjClassOfReference().c_str(), message_2.getObjClassOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object of reference is correct", strcmp(message_1.getObjectOfReference().c_str(), message_2.getObjectOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is correct", strcmp(message_1.getProblemData().c_str(), message_2.getProblemData().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is correct", strcmp(message_1.getProblemText().c_str(), message_2.getProblemText().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Manual Cease of reference is correct", message_1.getManualCease() , message_2.getManualCease());

	cout<<endl;
	cout<<"Field of message_1"<<endl;
	print_utility(message_1);

	cout<<endl;

	cout<<"Field of message_2"<<endl;
	print_utility(message_2);

	cout<<endl;

	cout<<message_1.getEventMessage()<<endl;
	cout<<message_2.getEventMessage()<<endl;
}


void test_copyConstructor_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_copyConstructor_2()" << endl;
	cout << " TEST: test the copy constructor" << endl;
	cout << " RESULT: the two object are equals\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message_1(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	acs_aeh_eventmsg message_2(message_1);


	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(message_1.getProcessName().c_str(), message_2.getProcessName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Specific problem is correct", message_1.getSpecificProblem() , message_2.getSpecificProblem());
	ASSERT_EQUALM("CUTE: check if value Perceived severity is correct", strcmp(message_1.getPercSeverity().c_str(), message_2.getPercSeverity().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Probable cause is correct", strcmp(message_1.getProbableCause().c_str(), message_2.getProbableCause().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is correct", strcmp(message_1.getObjClassOfReference().c_str(), message_2.getObjClassOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object of reference is correct", strcmp(message_1.getObjectOfReference().c_str(), message_2.getObjectOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is correct", strcmp(message_1.getProblemData().c_str(), message_2.getProblemData().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is correct", strcmp(message_1.getProblemText().c_str(), message_2.getProblemText().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Manual Cease of reference is correct", message_1.getManualCease() , message_2.getManualCease());

	cout<<endl;
	cout<<"Field of message_1"<<endl;
	print_utility(message_1);

	cout<<endl;

	cout<<message_1.getEventMessage()<<endl;
	cout<<endl;

	cout<<"Field of message_2"<<endl;
	print_utility(message_2);

	cout<<endl;
	cout<<message_2.getEventMessage()<<endl;
	cout<<endl;
}


void test_assignmentOperator(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_assignmentOperator()" << endl;
	cout << " TEST: test the operator assignment " << endl;
	cout << " RESULT: the two object are equals\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message_1(p_processName, p_specificProblem,
								p_percSeverity, p_probableCause, p_objClassOfReference,
								p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	acs_aeh_eventmsg message_2;

	message_2 = message_1;

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(message_1.getProcessName().c_str(), message_2.getProcessName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Specific problem is correct", message_1.getSpecificProblem() , message_2.getSpecificProblem());
	ASSERT_EQUALM("CUTE: check if value Perceived severity is correct", strcmp(message_1.getPercSeverity().c_str(), message_2.getPercSeverity().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Probable cause is correct", strcmp(message_1.getProbableCause().c_str(), message_2.getProbableCause().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is correct", strcmp(message_1.getObjClassOfReference().c_str(), message_2.getObjClassOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object of reference is correct", strcmp(message_1.getObjectOfReference().c_str(), message_2.getObjectOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is correct", strcmp(message_1.getProblemData().c_str(), message_2.getProblemData().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is correct", strcmp(message_1.getProblemText().c_str(), message_2.getProblemText().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Manual Cease of reference is correct", message_1.getManualCease() , message_2.getManualCease());

	cout<<endl;
	cout<<"Field of message_1"<<endl;
	print_utility(message_1);

	cout<<endl;
	cout<<message_1.getEventMessage()<<endl;
	cout<<endl;

	cout<<"Field of message_2"<<endl;
	print_utility(message_2);

	cout<<endl;
	cout<<message_2.getEventMessage()<<endl;
	cout<<endl;


}


void test_setGetMethod_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_setGetMethod_1()" << endl;
	cout << " TEST: assign a values to the field of the message and verifies its correctness \n"
			" through the get methods" << endl;
	cout << " RESULT: all the values are correctly assigned. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message;

	message.setProcessName(p_processName);
	message.setSpecificProblem(p_specificProblem);
	message.setPercSeverity(p_percSeverity);
	message.setProbableCause(p_probableCause);
	message.setObjClassOfReference(p_objClassOfReference);
	message.setObjectOfReference(p_objectOfReference);
	message.setProblemData(p_problemData);
	message.setProblemText(p_problemText);
	message.setManualCease(p_manualCease);

	ASSERT_EQUALM("CUTE: check if value Process name is correct", strcmp(p_processName, message.getProcessName().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Specific problem is correct", p_specificProblem , message.getSpecificProblem());
	ASSERT_EQUALM("CUTE: check if value Perceived severity is correct", strcmp(p_percSeverity, message.getPercSeverity().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Probable cause is correct", strcmp(p_probableCause, message.getProbableCause().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is correct", strcmp(p_objClassOfReference, message.getObjClassOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Object of reference is correct", strcmp(p_objectOfReference, message.getObjectOfReference().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is correct", strcmp(p_problemData, message.getProblemData().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is correct", strcmp(p_problemText, message.getProblemText().c_str()) , 0);
	ASSERT_EQUALM("CUTE: check if value Manual Cease of reference is correct", p_manualCease , message.getManualCease());

	cout<<endl;

	print_utility(message);

	cout<<endl;

}


void test_setGetMethod_2(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_setGetMethod_2()" << endl;
	cout << " TEST: call the set methods passing parameter uninitialized"<< endl;
	cout << " RESULT: the filed of message will be initialized with empty value\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	acs_aeh_eventmsg message;

	char * name;
	char * severity;
	char * probableCause;
	char * classOfReference;
	char * objectOfReference;
	char * problemData;
	char * problemText;


	message.setProcessName(name);
	message.setPercSeverity(severity);
	message.setProbableCause(probableCause);
	message.setObjClassOfReference(classOfReference);
	message.setObjectOfReference(objectOfReference);
	message.setProblemData(problemData);
	message.setProblemText(problemText);


	ASSERT_EQUALM("CUTE: check if value Process name is empty", message.getProcessName().empty() , true);
	ASSERT_EQUALM("CUTE: check if value Perceived severity is empty", message.getPercSeverity().empty() , true);
	ASSERT_EQUALM("CUTE: check if value Probable cause is empty",message.getProbableCause().empty(), true);
	ASSERT_EQUALM("CUTE: check if value Object class of reference is empty", message.getObjClassOfReference().empty() , true);
	ASSERT_EQUALM("CUTE: check if value Object of reference is empty", message.getObjectOfReference().empty(), true);
	ASSERT_EQUALM("CUTE: check if value Problem data of reference is empty", message.getProblemData().empty(), true);
	ASSERT_EQUALM("CUTE: check if value Problem text of reference is empty", message.getProblemText().empty(), true);


	cout<<endl;

	cout<<"Process name: "<<message.getProcessName()<<endl;
	cout<<"Perceived severity: "<<message.getPercSeverity()<<endl;
	cout<<"Probable cause: "<<message.getProbableCause()<<endl;
	cout<<"Object class of reference: "<<message.getObjClassOfReference()<<endl;
	cout<<"Object of reference: "<<message.getObjectOfReference()<<endl;
	cout<<"Problem data: "<<message.getProblemData()<<endl;
	cout<<"Problem text: "<<message.getProblemText()<<endl;

	cout<<endl;

}


void test_ComparisonOperator_1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_ComparisonOperator_1()" << endl;
	cout << " TEST: test the comparison operator"<< endl;
	cout << " RESULT: The two object are equal\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);

	acs_aeh_eventmsg message_2;

	message_2 = message;

	bool res;

	if(message==message_2){
		res = true;
		cout<<"The objects are equal"<<endl;
	}else{
		res = false;
		cout<<"The objects not are equal"<<endl;
	}

	ASSERT_EQUALM("CUTE: check if the two object are equal", res, true);

	cout<<endl;

}

void test_ComparisonOperator_2(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_ComparisonOperator_2()" << endl;
	cout << " TEST: test the comparison operator"<< endl;
	cout << " RESULT: The two objects not are equal\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);


	char * objectOfReference = 0;
	acs_aeh_eventmsg message_2(p_processName, p_specificProblem,
			p_percSeverity, p_probableCause, p_objClassOfReference,
			objectOfReference, p_problemData, p_problemText, p_manualCease);


	bool res;

	if(message==message_2){
		res = true;
		cout<<"The objects are equal"<<endl;
	}else{
		res = false;
		cout<<"The objects not are equal"<<endl;
	}

	ASSERT_EQUALM("CUTE: check if the two object are equal", res, false);

	cout<<endl;

}


void test_ComparisonOperator_3(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_ComparisonOperator_2()" << endl;
	cout << " TEST: test the comparison operator"<< endl;
	cout << " RESULT: The two objects not are equal\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	acs_aeh_eventmsg message(p_processName, p_specificProblem,
							p_percSeverity, p_probableCause, p_objClassOfReference,
							p_objectOfReference, p_problemData, p_problemText, p_manualCease);



	acs_aeh_eventmsg message_2;


	bool res;

	if(message==message_2){
		res = true;
		cout<<"The objects are equal"<<endl;
	}else{
		res = false;
		cout<<"The objects not are equal"<<endl;
	}

	ASSERT_EQUALM("CUTE: check if the two object are equal", res, false);

	cout<<endl;

}


void runSuite(){
	cute::suite s;

	//TODO add your test here
	s.push_back(CUTE(test_constructor_1));
	s.push_back(CUTE(test_constructor_2));
	s.push_back(CUTE(test_constructor_3));
	s.push_back(CUTE(test_copyConstructor_1));
	s.push_back(CUTE(test_assignmentOperator));
	s.push_back(CUTE(test_setGetMethod_1));
	s.push_back(CUTE(test_setGetMethod_2));
	s.push_back(CUTE(test_ComparisonOperator_1));
	s.push_back(CUTE(test_ComparisonOperator_2));
	s.push_back(CUTE(test_ComparisonOperator_3));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test Suite");


}


int main() {


	cout << "Test Suite start " << endl;
	runSuite();
	cout << "Test Suite end" << endl;
	return 0;
}
