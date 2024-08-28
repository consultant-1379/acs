//============================================================================
// Name        : ut_acs_bur_BrmBackup.cpp
// Author      : egiacri,egimarr
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
using namespace std;

#include "ut_acs_bur_BrmBackup.h"
#include "ut_acs_bur_Common.h"

int createBrmBackupTestObject(){
	int	errorCode = NOERROR;
	string msgError = string ("");
	char opResultInfo[50];
	/*char answer[5];
	char cYes='y';
	cout << " -> Try to create a new BrmBackup test object " << TEST_BKP << endl;
	cout << endl ;
	cout << MSG_YESNOT ;
	while(FOREVER){
		fgets(answer, 4, stdin);
		cYes = *answer;
		if ((2 == strlen(answer)) && (( 'y' == cYes ) || ( 'Y' == cYes) || ( 'n' == cYes ) || ( 'N' == cYes)))
			break; // Exit from get input
	}
	if (cYes=='y'||cYes=='Y')
	{*/
		if(RTN_FAIL == ut_acs_bur_Common::invokeAction_s(CREATEBACKUP,DN_BRMBACKUPMANAGER,TEST_BKP,&errorCode,&msgError))
		{
			cout << "BrmBackup test object creation failed" << endl;
			return RTN_FAIL;
		}
		if(RTN_FAIL == ut_acs_bur_Common::waitForActionEnd (CREATEBACKUP,TEST_BKP,&errorCode))
		{
			cout << "BrmBackup test object creation failed with error code " << errorCode<<endl;
			if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMAAP,"resultInfo",opResultInfo)!=RTN_FAIL)
				cout << "Result Info :: " <<opResultInfo <<endl;
			return RTN_FAIL;
		}
	//} comment for y/n option
	return RTN_OK;

}

int deleteBrmBackupTestObject(){
	int	errorCode = NOERROR;
	string msgError= string("");
	acs_bur_BrmAsyncActionProgress *async;
	char answer[5];
	char cYes='y';
	cout << " -> Try to delete a new BrmBackup test object " << TEST_BKP << endl;
	cout << endl ;
	/*cout << " -> Confirm you really want to delete BrmBackup test object " << TEST_BKP << endl;
	cout << endl ;
	cout << MSG_YESNOT ;
	while(FOREVER){
		fgets(answer, 4, stdin);
		cYes = *answer;
		if ((2 == strlen(answer)) && (( 'y' == cYes ) || ( 'Y' == cYes) || ( 'n' == cYes ) || ( 'N' == cYes)))
			break; // Exit from get input
	}
    if (cYes=='y'||cYes=='Y')
    {*/
		if(RTN_FAIL != ut_acs_bur_Common::invokeAction_s(DELETEBACKUP,DN_BRMBACKUPMANAGER,TEST_BKP,&errorCode,&msgError))
		{
			async= new acs_bur_BrmAsyncActionProgress((char *)DN_BRMAAP);
			if(async->getResult()==FAILURE && async->getActionId()== DELETEBACKUP)
			{
				cout << "BrmBackup test object delete failed" << endl;
				delete async;
				return RTN_FAIL;

			}
		}
		if(RTN_FAIL == ut_acs_bur_Common::waitForActionEnd(DELETEBACKUP,TEST_BKP,&errorCode))
		{
			cout << "BrmBackup test object delete failed with error code " << errorCode<<endl;
			return RTN_FAIL;
		}
   // } comment for y/n
	return RTN_OK;
}


void test_Constructor_Positive(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_BrmBackup_Costructor_Posistive " << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackup "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_TEST_BRMBACKUP<<" MO" << endl;
	cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	//acs_bur_BrmBackupManager ut_bur("brmBackupManagerId=SYSTEM_DATA,brMId=1");
	char nameObject[] = DN_TEST_BRMBACKUP;
	acs_bur_BrmBackup ut_bur(nameObject);
	int retValue = ut_bur.getOpCode();
	ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup() method", 0, retValue );

}
void test_Constructor_Negative(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_BrmBackupManager_Costructor_Negative()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TEST: acs_bur_BrmBackup class constructor invoked with wrong dn  "<<endl;
	cout << " TARGET TEST ENVIRONMENT: brmBackupManagerId=SYSTEM,brMId=1 MO" << endl;
	cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked with error code  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	acs_bur_BrmBackup ut_bur((char *)"brmBackupId=utBackup,brmBackupManagerId=SYSTEM,brMId=1");
	int retValue = ut_bur.getOpCode();
	cout << "Error code raised: "<<retValue<<endl;
	ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup() method", GETATTRERROR, retValue );

}


void test_get_Attribute_ID(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_ID()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackup "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_TEST_BRMBACKUP<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackup attribute Id correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	//acs_bur_BrmBackupManager ut_bur("brmBackupManagerId=SYSTEM_DATA,brMId=1");
	char nameObject[] = DN_TEST_BRMBACKUP;
	acs_bur_BrmBackup ut_bur(nameObject);
	string retValue_s= ut_bur.getBrmBackupId();
	cout << "Read ID attribute via BrmBackup method: "<<retValue_s<<endl;
	char char_value_s[50];
	char char_value[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_TEST_BRMBACKUP,"brmBackupId",char_value_s)==ACS_CC_SUCCESS && !test_end )

	{
		cout << "Read ID attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getBrmBackupId() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_TEST_BRMBACKUP,"brmBackupManagerId",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read ID attribute 	via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getBrmBackupId() method via apgcc", 0, strcmp(retValue_s.c_str(),char_value ) );
		test_end= true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getBrmBackupId() method", 0, strcmp(retValue_s.c_str(), "brmBackupId=utBrmBackup") );
}



void test_get_Attribute_BkpName(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_BkpName()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackup "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_TEST_BRMBACKUP<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackup class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackup attribute BackupName correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	char nameObject[] = DN_TEST_BRMBACKUP;
	acs_bur_BrmBackup ut_bur(nameObject);
	string retValue_s= ut_bur.getBackupName();
	cout << "Read backupName attribute via BrmBackup method: "<<retValue_s<<endl;
	char char_value[50];
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_TEST_BRMBACKUP,"backupName",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read backupName attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getBackupName() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_TEST_BRMBACKUP,"backupName",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read backupName attribute via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getBackupName() method via apgcc", strcmp(retValue_s.c_str(),char_value ), 0 );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getBackupName() method", strcmp(retValue_s.c_str(), "utBrmBackup"), 0 );

}
void test_get_Attribute_BkpCreationTime(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_BkpCreationTime()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackup "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_TEST_BRMBACKUP<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackup class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackup attribute CreationTime correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	char nameObject[] = DN_TEST_BRMBACKUP;
	acs_bur_BrmBackup ut_bur(nameObject);
	string retValue_s= ut_bur.getCreationTime();
	cout << "Read creationTime attribute via BrmBackup method: "<<retValue_s<<endl;
	char char_value[50];
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_TEST_BRMBACKUP,"creationTime",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read CreationTime attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getCreationTime() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_TEST_BRMBACKUP,"creationTime",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read backupName attribute via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getCreationTime() method via apgcc", strcmp(retValue_s.c_str(),char_value ), 0 );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getCreationTime() method", strcmp(retValue_s.c_str(), "utBrmBackup"), 0 );

}

void test_get_Attribute_BkpStatus(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_BkpStatus()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackup "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_TEST_BRMBACKUP<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackup class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackup attribute Status correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	char nameObject[] = DN_TEST_BRMBACKUP;
	acs_bur_BrmBackup ut_bur(nameObject);
	//string retValue_s= ut_bur.getStatus();
	int retValue_i= ut_bur.getStatus();
	char retValue_s [50];
	sprintf(retValue_s,"%d",retValue_i);
	cout << "Read Status attribute via BrmBackup method: "<<retValue_s<<endl;
	char char_value[50];
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_TEST_BRMBACKUP,"status",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read Status attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getStatus() method via immlist command", 0, strcmp(retValue_s,char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_TEST_BRMBACKUP,"status",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read backupName attribute via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getStatus() method via apgcc", strcmp(retValue_s,char_value ), 0 );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getStatus() method", strcmp(retValue_s, "utBrmBackup"), 0 );

}

void test_get_Attribute_BkpAsyncActionProgress(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_BkpAsyncActionProgress()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackup "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_TEST_BRMBACKUP<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackup attribute Backup AsyncActionProgress correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_TEST_BRMBACKUP;
	acs_bur_BrmBackup ut_bur(nameObject);
	string retValue_s= ut_bur.getAsyncActionProgress();
	cout << "Read asyncActionProgress attribute via BrmBackup method: "<<retValue_s<<endl;
	char char_value[100];
	char char_value_s[100];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_TEST_BRMBACKUP,"asyncActionProgress",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read asyncActionProgress attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getAsyncActionProgress() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_TEST_BRMBACKUP,"asyncActionProgress",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read asyncActionProgress attribute via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getAsyncActionProgress() method via apgcc", strcmp(retValue_s.c_str(),char_value ), 0 );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackup::getAsyncActionProgress() method", strcmp(retValue_s.c_str(), "id=1,brmBackupId=utBrmBackup,brmBackupManagerId=SYSTEM_DATA,brMId=1"), 0 );

}

/*
string  getAsyncActionProgress();
int 	getOpCode();
void 	setOpCode(int code);*/


void runSuite(){
	cute::suite s;
	//TODO add your test here
	if (RTN_FAIL != createBrmBackupTestObject())
	{
	s.push_back(CUTE(test_Constructor_Positive));
	s.push_back(CUTE(test_Constructor_Negative));
	s.push_back(CUTE(test_get_Attribute_ID));
	s.push_back(CUTE(test_get_Attribute_BkpName));
	s.push_back(CUTE(test_get_Attribute_BkpCreationTime));
	s.push_back(CUTE(test_get_Attribute_BkpStatus));
	s.push_back(CUTE(test_get_Attribute_BkpAsyncActionProgress));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test suite BrmBackup");
	deleteBrmBackupTestObject();
	}
}

int main() {

	cout << "Start Test acs_bur_BrmBackup class" << endl;
	runSuite();
	cout << "End Test acs_bur_BrmBackup class " << endl;
	return 0;
}
