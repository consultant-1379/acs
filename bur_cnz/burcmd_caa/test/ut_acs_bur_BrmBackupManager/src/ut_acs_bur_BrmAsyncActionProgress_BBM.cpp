//============================================================================
// Name        : ut_acs_bur_BrmBackup.cpp
// Author      : egiacri,egimarr
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "acs_bur_Define.h"
#include "acs_bur_BrmAsyncActionProgress.h"
#include "ut_acs_bur_BrmBackupManager.h"

using namespace std;
#define INITIAL_ACTION_ID 	99
#define DN_AAP "id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1"

void test_Constructor_Positive_AAP(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_BrmAsyncActionProgress_Constructor_Positive()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: acs_bur_BrmAsyncActionProgress class constructor invoked without error  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	int retValue = ut_bur_AAP.getOpCode();
	if (retValue==INITERROR)
		ASSERT_EQUALM("CUTE FAUILURE:acs_bur_BrmAsyncActionProgress() method failed to init", INITERROR, retValue);
	else if (retValue==GETATTRERROR)
		ASSERT_EQUALM("CUTE FAILURE:acs_bur_BrmAsyncActionProgress() method failed to getAttributes", GETATTRERROR, retValue);
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress() method", NOERROR, retValue);

}

void test_Constructor_Negative_AAP(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_costructor_1()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TEST: acs_bur_BrmAsyncActionProgress class constructor invoked with wrong dn  "<<endl;
	cout << " TARGET TEST ENVIRONMENT: brmBackupManagerId=SYSTEM_DATA,brMId=1 MO" << endl;
	cout << " RESULT: acs_bur_BrmAsyncActionProgress class constructor invoked with error code  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	char nameObject[] = "id=1,brmBackupManagerId=SYSTEM_,brMId=1";
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);

	int retValue = ut_bur_AAP.getOpCode();
	cout << "Error code raised: "<<retValue<<endl;
	ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress() method", GETATTRERROR, retValue );
}

void test_get_Attribute_ActionName(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_ActionName()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute ActionName correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	string retValue_s= ut_bur_AAP.getActionName();
	cout << "Read ActionName attribute: "<<retValue_s<<endl;
	int test_counter=0;
	if (strcmp(retValue_s.c_str(), "DELETE")==0)
		test_counter++;
	else if (strcmp(retValue_s.c_str(), "CREATE")==0)
		test_counter++;
	else if (strcmp(retValue_s.c_str(), "CANCELL")==0)
		test_counter++;
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"actionName",char_value_s)==ACS_CC_SUCCESS && !test_end && test_counter!=0)
	{
		cout << "Read actionName attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getActionName() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getActionName() method", 1 ,test_counter );
}

void test_get_Attribute_AdditionalInfo(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_AdditionalInfo()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute AdditionalInfo correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	string retValue_s= ut_bur_AAP.getAdditionalInfo();
	char retValue_c [512];
	strncpy(retValue_c, retValue_s.c_str(), 512);
	ut_acs_bur_Common::clearCRLF(retValue_c);
	cout << "Read AdditionalInfo attribute: "<<retValue_s<<endl;
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"additionalInfo",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read AdditionalInfo attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getAdditionalInfo() method via immlist command", 0, strcmp(retValue_c,char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getAdditionalInfo() method", 0, strcmp(retValue_s.c_str(), retValue_s.c_str()) );
}

void test_get_Attribute_ProgressInfo(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_ProgressInfo()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute ProgressInfo correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	string retValue_s= ut_bur_AAP.getProgressInfo();
	cout << "Read ProgressInfo attribute: "<<retValue_s<<endl;
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"progressInfo",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read ProgressInfo attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getProgressInfo() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getProgressInfo() method",0 , strcmp(retValue_s.c_str(), retValue_s.c_str()) );
}

void test_get_Attribute_ResultInfo(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_ResultInfo()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute ResultInfo correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	string retValue_s= ut_bur_AAP.getResultInfo();
	char retValue_c [512];
	strncpy(retValue_c, retValue_s.c_str(), 512);
	ut_acs_bur_Common::clearCRLF(retValue_c);
	cout << "Read ResultInfo attribute: "<<retValue_s<<endl;
	char char_value_s[512];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"resultInfo",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read ResultInfo attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getResultInfo() method via immlist command", 0, strcmp(retValue_c,char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getResultInfo() method", 0, strcmp(retValue_s.c_str(), retValue_s.c_str()));
}

void test_get_Attribute_TimeActionStarted(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_TimeActionStarted()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute TimeActionStarted correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	string retValue_s= ut_bur_AAP.getTimeActionStarted();
	cout << "Read TimeActionStarted attribute: "<<retValue_s<<endl;
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"timeActionStarted",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read TimeActionStarted attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getTimeActionStarted() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getTimeActionStarted() method", 0, strcmp(retValue_s.c_str(), retValue_s.c_str()));
}

void test_get_Attribute_TimeActionCompleted(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_TimeActionCompleted()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute TimeActionCompleted correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	string retValue_s= ut_bur_AAP.getTimeActionCompleted();
	cout << "Read TimeActionCompleted attribute: "<<retValue_s<<endl;
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"timeActionCompleted",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read TimeActionCompleted attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getTimeActionCompleted() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getTimeActionCompleted() method", 0, strcmp(retValue_s.c_str(), retValue_s.c_str()));
}

void test_get_Attribute_TimeOfLastStatusUpdate(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_TimeOfLastStatusUpdate()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute TimeOfLastStatusUpdate correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	string retValue_s= ut_bur_AAP.getTimeOfLastStatusUpdate();
	cout << "Read TimeOfLastStatusUpdate attribute: "<<retValue_s<<endl;
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"timeOfLastStatusUpdate",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read TimeOfLastStatusUpdate attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::TimeOfLastStatusUpdate() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getTimeOfLastStatusUpdate() method", 0, strcmp(retValue_s.c_str(), retValue_s.c_str()));
}

void test_get_Attribute_ActionId(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_ActionId()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute ActionId correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	int retValue= ut_bur_AAP.getActionId();
	int test_counter=0;
	if (retValue==CREATEBACKUP)
		test_counter++;
	else if (retValue==DELETEBACKUP)
		test_counter++;
	else if (retValue==CANCELCURRENT)
		test_counter++;
	else if (retValue==INITIAL_ACTION_ID)
		test_counter++;
	cout << "Read ActionId attribute: "<<retValue<<endl;
	char char_value_s[50];
	char ret_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"actionId",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		sprintf(ret_value_s,"%d",retValue);
		cout << "Read ActionId attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getAdditionalId() method via immlist command", 0, strcmp(ret_value_s,char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getActionId() method",1 ,test_counter);
}

void test_get_Attribute_Result(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_Result()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute Result correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	int retValue= ut_bur_AAP.getResult();
	cout << "Read Result attribute: "<<retValue<<endl;
	int test_counter=0;
	if (retValue==SUCCESS)
		test_counter++;
	else if (retValue==FAILURE)
		test_counter++;
	else if (retValue==NOT_AVAILABLE)
		test_counter++;
	char char_value_s[50];
	char ret_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"result",char_value_s)==ACS_CC_SUCCESS && !test_end && test_counter!=0)
	{
		sprintf(ret_value_s,"%d",retValue);
		cout << "Read Result attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getResult() method via immlist command", 0, strcmp(ret_value_s,char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getResult() method",1 ,test_counter);
}

void test_get_Attribute_ProgressPercentage(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_ProgressPercentage()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute ProgressPercentage correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	int retValue= ut_bur_AAP.getProgressPercentage();
	cout << "Read ProgressPercentage attribute: "<<retValue<<endl;
	int test_counter=0;
	if (retValue==0)
		test_counter++;
	else if (retValue==25)
		test_counter++;
	else if (retValue==50)
		test_counter++;
	else if (retValue==75)
		test_counter++;
	else if (retValue==100)
		test_counter++;
	char char_value_s[50];
	char ret_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"progressPercentage",char_value_s)==ACS_CC_SUCCESS && !test_end && test_counter!=0)
	{
		sprintf(ret_value_s,"%d",retValue);
		cout << "Read ProgressPercentage attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getProgressPercentage() method via immlist command", 0, strcmp(ret_value_s,char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getProgressPercentage() method",1 ,test_counter);
}
void test_get_Attribute_State(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_State()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmAsyncActionProgress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: "<<DN_AAP<<" MO" << endl;
	cout << " RESULT: BrmAsyncActionProgress attribute State correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_AAP;
	acs_bur_BrmAsyncActionProgress ut_bur_AAP(nameObject);
	int retValue= ut_bur_AAP.getState();
	cout << "Read State attribute: "<<retValue<<endl;
	int test_counter=0;
	if (retValue==CANCELLING)
		test_counter++;
	else if (retValue==RUNNING)
		test_counter++;
	else if (retValue==FINISHED)
		test_counter++;
	else if (retValue==CANCELLED)
		test_counter++;
	char char_value_s[50];
	char ret_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_AAP,"state",char_value_s)==ACS_CC_SUCCESS && !test_end)
	{
		sprintf(ret_value_s,"%d",retValue);
		cout << "Read state attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of  acs_bur_BrmAsyncActionProgress::getState() method via immlist command", 0, strcmp(ret_value_s,char_value_s ) );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmAsyncActionProgress::getState() method",1 ,test_counter);
}

void runSuite_AAP(){
	cute::suite s;
	//TODO add your test here
	s.push_back(CUTE(test_Constructor_Positive_AAP));
	s.push_back(CUTE(test_Constructor_Negative_AAP));
	s.push_back(CUTE(test_get_Attribute_ActionName));
	s.push_back(CUTE(test_get_Attribute_AdditionalInfo));
	s.push_back(CUTE(test_get_Attribute_ProgressInfo));
	s.push_back(CUTE(test_get_Attribute_ResultInfo));
	s.push_back(CUTE(test_get_Attribute_TimeActionStarted));
	s.push_back(CUTE(test_get_Attribute_TimeActionCompleted));
	s.push_back(CUTE(test_get_Attribute_TimeOfLastStatusUpdate));
	s.push_back(CUTE(test_get_Attribute_ActionId));
	s.push_back(CUTE(test_get_Attribute_Result));
	s.push_back(CUTE(test_get_Attribute_ProgressPercentage));
	s.push_back(CUTE(test_get_Attribute_State));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test suite BrmAsyncActionProgress");

}

int main() {

	cout << "Start Test acs_bur_BrmAsyncActionProgress class" << endl;
	runSuite_AAP();
	cout << "End Test acs_bur_BrmAsyncActionProgress class " << endl;
	return 0;
}


