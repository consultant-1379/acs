/*
 * ut_aeh_evreport.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: xfabron
 */


#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <errno.h>


#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "acs_aeh_evreport.h"

using namespace std;


std::string p_processName("ut_evreport_test");
long p_specificProblem = 1402;
std::string p_percSeverity("EVENT");
std::string p_probableCause("File Open Error in AP");
std::string p_objClassOfReference("APZ");
std::string p_objectOfReference("DISC");
std::string p_problemData("problem opening for write");
std::string p_problemText("VOLUME  \tLIM\nDISC/1 \t95 ");
bool p_manualCease = false;



int restartSyslogNG(){

	FILE *pidSyslogFile_p = popen("pgrep -x syslog-ng", "r");

	if(!pidSyslogFile_p){
		return -1;
	}

	char buffer[16];
	char *line_p = fgets(buffer, sizeof(buffer), pidSyslogFile_p);
	pclose(pidSyslogFile_p);



	std::string pidSyslog(line_p);
	std::string restartSyslog("kill -HUP ");
	restartSyslog.append(pidSyslog);

	system(restartSyslog.c_str());

	return 0;

}


void test_copyConstructor_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_copyConstructor_1()" << endl;
	cout << " TEST: test the copy constructor of class acs_aeh_evreport" << endl;
	cout << " RESULT: the two event are reported in log file. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;

	p_processName.append(":");
	std::stringstream out;
	out<<getpid();
	p_processName.append(out.str());

	char *percSeverityO1 = const_cast<char*>("O1");
	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityO1, p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	acs_aeh_evreport event_2(event);
	ret = event_2.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityO1, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


}


void test_assignmentOperator_1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_assignmentOperator_1" << endl;
	cout << " TEST: test the assignment operator of class acs_aeh_evreport" << endl;
	cout << " RESULT: the two event are reported in log file. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;
	acs_aeh_evreport event_2;


	char *percSeverityO2 = const_cast<char*>("O2");
	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityO2, p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	event_2 = event;
	ret = event_2.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityO2, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);
}


void test_sendEventMessage_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMEssage_1()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the event is reported in log file. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

}


void test_sendEventMessage_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMEssage_2()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method return with error ACS_AEH_error code ACS_AEH_syntaxError \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;


	char *percSeverityWRONG = const_cast<char*>("WRONG SEVERITY");

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityWRONG, p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

	ASSERTM("CUTE: check return value of getError", event.getError() == ACS_AEH_syntaxError);

	cout<<event.getErrorText()<<endl;

}


void test_sendEventMessage_3(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMEssage_3()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method return with error ACS_AEH_error code ACS_AEH_eventDeliveryFailure \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;

	system("rm /var/run/ap/aehfifo");
	sleep(1);

	ret = event.sendEventMessage("pippo", p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());


	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

	ASSERTM("CUTE: check return value of getError", event.getError() == ACS_AEH_eventDeliveryFailure);
//	sleep(1);
//	system("mkfifo /var/run/ap/aehfifo");
//
//	sleep(1);
//
//	restartSyslogNG();

}


void test_sendEventMessage_4(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMEssage_4()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the event with severity A1 is reported in log file. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;

	char *percSeverityA1 = const_cast<char*>("A1");

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityA1, p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

}


void test_sendEventMessage_5(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMEssage_5()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method return with error ACS_AEH_error code ACS_AEH_syntaxError \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;


	char *percSeverityWRONG = const_cast<char*>("WRONG SEVERITY");

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityWRONG, p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

	ASSERTM("CUTE: check return value of getError", event.getError() == ACS_AEH_syntaxError);

}


void test_sendEventMessage_6(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMEssage_6()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method return with error ACS_AEH_error code ACS_AEH_eventDeliveryFailure \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;

	char *percSeverityA2 = const_cast<char*>("A2");

//	system("rm /var/run/ap/aehfifo");


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityA2, p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);


	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

	ASSERTM("CUTE: check return value of getError", event.getError() == ACS_AEH_eventDeliveryFailure);

	system("mkfifo /var/run/ap/aehfifo");

	sleep(1);

	restartSyslogNG();

}

void test_sendEventMessage_7(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMessage_7()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method send a event for each possible value of severity field \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;

	char *percSeverityPR = const_cast<char*>("PR");

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, ACS_AEH_PERCEIVED_SEVERITY_EVENT, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, ACS_AEH_PERCEIVED_SEVERITY_O1, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, ACS_AEH_PERCEIVED_SEVERITY_O2, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, ACS_AEH_PERCEIVED_SEVERITY_A1, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, ACS_AEH_PERCEIVED_SEVERITY_A2, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, ACS_AEH_PERCEIVED_SEVERITY_A3, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, ACS_AEH_PERCEIVED_SEVERITY_CEASING, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, percSeverityPR, p_probableCause.c_str(),
				p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

}


void test_sendEventMessage_8(){

	cout << " -----------------------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMessage_8()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method send a event for each possible value of object class of reference  field \n" << endl;
	cout << " ---------------------------------------------------------------------------------------------- " << endl;

	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;


	char *objClassOfReferenceWRG = const_cast<char*>("WRG");


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			ACS_AEH_OBJ_CLASS_REFERENZE_APZ, p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			ACS_AEH_OBJ_CLASS_REFERENZE_APT, p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			ACS_AEH_OBJ_CLASS_REFERENZE_PWR, p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			ACS_AEH_OBJ_CLASS_REFERENZE_EXT, p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			ACS_AEH_OBJ_CLASS_REFERENZE_EVENT, p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			objClassOfReferenceWRG, p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

}


void test_sendEventMessage_9(){


	cout << " -----------------------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMessage_9()" << endl;
	cout << " TEST: test the method sendEventMessage condition errors" << endl;
	cout << " RESULT: the method send a event without specify the process name field \n" << endl;
	cout << " ---------------------------------------------------------------------------------------------- " << endl;


	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;


	char* processName = 0;


	ret = event.sendEventMessage(processName, p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str(), true);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

	cout<<event.getErrorText()<<endl;

}


void test_sendEventMessage_10(){


	cout << " -----------------------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMessage_10()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method send a event with the minimum information possible \n" << endl;
	cout << " ---------------------------------------------------------------------------------------------- " << endl;


	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;



	char* probableCause = 0;
	char* objectOfReference = 0;
	char* problemData = 0;
	char* problemText = 0;

	fstream fileId;
	std::string line;

	fileId.open("/etc/cluster/nodes/this/id", ios::out | ios::in);
	getline(fileId, line);
	fileId.close();

	fileId.open("/etc/cluster/nodes/this/id", ios::out);
	cout<<"Result: "<<fileId.write("3", 1)<<endl;
	fileId.close();

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), probableCause,
			p_objClassOfReference.c_str(), objectOfReference, problemData, problemText, false);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	fileId.open("/etc/cluster/nodes/this/id", ios::out);
	fileId<<line;
	fileId.close();

}



void test_sendEventMessage_11(){


	cout << " -----------------------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMessage_11()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the event is sent only if not already sent before 30 seconds\n" << endl;
	cout << " ---------------------------------------------------------------------------------------------- " << endl;


	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	long specificProblem = 12345;
	ret = event.sendEventMessage(p_processName.c_str(), specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	sleep(16);

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	sleep(16);

	char* problemText = 0;
	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), problemText);

}


void test_sendEventMessage_12(){


	cout << " -----------------------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMessage_12()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method send a event with non ascii character \n" << endl;
	cout << " ---------------------------------------------------------------------------------------------- " << endl;


	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;

	const char* probableCause = "\x01\x02\x03\x04";
	char* objectOfReference = 0;
	char* problemData = 0;
	char* problemText = 0;


	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), probableCause,
			p_objClassOfReference.c_str(), objectOfReference, problemData, problemText, false);


	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

	cout<<event.getErrorText()<<endl;

	probableCause = 0;
	char* problemText_2 = const_cast<char* >("£");
	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), probableCause,
				p_objClassOfReference.c_str(), objectOfReference, problemData, problemText_2, false);

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_error);

	cout<<event.getErrorText()<<endl;

}


void test_sendEventMessage_13(){


	cout << " -----------------------------------------------------------------------------------------------" << endl;
	cout << " test_sendEventMessage_13()" << endl;
	cout << " TEST: test the method sendEventMessage" << endl;
	cout << " RESULT: the method send a event and simulate the node passive \n" << endl;
	cout << " ---------------------------------------------------------------------------------------------- " << endl;


	ACS_AEH_ReturnType ret;
	acs_aeh_evreport event;


	fstream fileId;
	std::string line;
	std::string line_2("2");

	fileId.open("/etc/cluster/nodes/this/id", ios::out | ios::in);
	getline(fileId, line);
	fileId.close();

	fileId.open("/etc/cluster/nodes/this/id", ios::out);
	fileId<<line_2;
	fileId.close();

	ret = event.sendEventMessage(p_processName.c_str(), p_specificProblem, p_percSeverity.c_str(), p_probableCause.c_str(),
			p_objClassOfReference.c_str(), p_objectOfReference.c_str(), p_problemData.c_str(), p_problemText.c_str());

	ASSERTM("CUTE: check return value of sendEventMessage", ret == ACS_AEH_ok);

	fileId.open("/etc/cluster/nodes/this/id", ios::out);
	fileId<<line;
	fileId.close();


}


void runSuite(){
	cute::suite s;

	s.push_back(CUTE(test_copyConstructor_1));
	s.push_back(CUTE(test_assignmentOperator_1));
	s.push_back(CUTE(test_sendEventMessage_1));
	s.push_back(CUTE(test_sendEventMessage_2));
	s.push_back(CUTE(test_sendEventMessage_4));
	s.push_back(CUTE(test_sendEventMessage_5));
	s.push_back(CUTE(test_sendEventMessage_7));
	s.push_back(CUTE(test_sendEventMessage_8));
	s.push_back(CUTE(test_sendEventMessage_9));
	s.push_back(CUTE(test_sendEventMessage_10));
	s.push_back(CUTE(test_sendEventMessage_11));
	s.push_back(CUTE(test_sendEventMessage_12));
	s.push_back(CUTE(test_sendEventMessage_13));
	s.push_back(CUTE(test_sendEventMessage_3));
	s.push_back(CUTE(test_sendEventMessage_6));


	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "thi_is_errorimpl_Test");


}





int main() {


	cout << "Start Test acs_aeh_evreport class" << endl;

	runSuite();

//	system("rm /var/run/ap/aehfifo");
//	sleep(1);
//	system("mkfifo /var/run/ap/aehfifo");
//	sleep(1);
//	restartSyslogNG();

//	int * p = reinterpret_cast<int *>(0xBAAD);
//	printf("p == %p\n", p);
//	printf("*p == %d\n", *p);

	cout << "End Test acs_aeh_evreport class " << endl;

	return 0;
}
