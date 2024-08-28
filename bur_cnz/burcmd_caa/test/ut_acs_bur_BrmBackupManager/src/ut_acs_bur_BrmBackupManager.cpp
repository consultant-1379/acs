//============================================================================
// Name        : ut_acs_bur_BrmBackup.cpp
// Author      : egiacri,egimarr
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "ut_acs_bur_BrmBackupManager.h"


//char char_value[50];
//test_getObjectAttribute("provaConfObj1,safApp=safImmService","ATTRIBUTE_7",char_value);



void test_Constructor_Positive(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_BrmBackupManager_Costructor_Posistive " << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_BRMBACKUPMANAGER<<" MO" << endl;
	cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	//acs_bur_BrmBackupManager ut_bur("brmBackupManagerId=SYSTEM_DATA,brMId=1");
	char nameObject[] = DN_BRMBACKUPMANAGER;
	acs_bur_BrmBackupManager ut_bur(nameObject);
	int retValue = ut_bur.getOpCode();
	ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager() method", retValue , 0);

}
void test_Constructor_Negative(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_BrmBackupManager_Costructor_Negative()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TEST: acs_bur_BrmBackupManager class constructor invoked with wrong dn  "<<endl;
	cout << " TARGET TEST ENVIRONMENT: brmBackupManagerId=SYSTEM,brMId=1 MO" << endl;
	cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked with error code  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	acs_bur_BrmBackupManager ut_bur((char *)"brmBackupManagerId=SYSTEM,brMId=1");
	int retValue = ut_bur.getOpCode();
	cout << "Error code raised: "<<retValue<<endl;
	ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager() method", retValue , GETATTRERROR);

}
void test_get_Attribute_ID(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_ID()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_BRMBACKUPMANAGER<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackupManager attribute Id correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	//acs_bur_BrmBackupManager ut_bur("brmBackupManagerId=SYSTEM_DATA,brMId=1");
	char nameObject[] = DN_BRMBACKUPMANAGER;
	acs_bur_BrmBackupManager ut_bur(nameObject);
	string retValue_s= ut_bur.getBrmBackupManagerId();
	cout << "Read ID attribute via BrmBackupManager method: "<<retValue_s<<endl;
	char char_value_s[50];
	char char_value[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMBACKUPMANAGER,"brmBackupManagerId",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read ID attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBrmBackupManagerId() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_BRMBACKUPMANAGER,"brmBackupManagerId",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read ID attribute 	via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBrmBackupManagerId() method via apgcc", strcmp(retValue_s.c_str(),char_value ), 0 );
		test_end= true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBrmBackupManagerId() method", strcmp(retValue_s.c_str(), "brmBackupManagerId=SYSTEM_DATA"), 0 );

}


void test_get_Attribute_BkpType(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_BkpType()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_BRMBACKUPMANAGER<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackupManager attribute BackupType correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	char nameObject[] = DN_BRMBACKUPMANAGER;
	acs_bur_BrmBackupManager ut_bur(nameObject);
	string retValue_s= ut_bur.getBackupType();
	cout << "Read backupType attribute via BrmBackupManager method: "<<retValue_s<<endl;
	char char_value[50];
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMBACKUPMANAGER,"backupType",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read backupType attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBackupType() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_BRMBACKUPMANAGER,"backupType",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read backupType attribute via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBackupType() method via apgcc", strcmp(retValue_s.c_str(),char_value ), 0 );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBackupType() method", strcmp(retValue_s.c_str(), "BRM_SYSTEM_DATA"), 0 );

}


void test_get_Attribute_BkpDomain(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_BkpDomain()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_BRMBACKUPMANAGER<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackupManager attribute BackupDomain correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	char nameObject[] = DN_BRMBACKUPMANAGER;
	acs_bur_BrmBackupManager ut_bur(nameObject);
	string retValue_s= ut_bur.getBackupDomain();
	cout << "Read backupDomain attribute via BrmBackupManager method: "<<retValue_s<<endl;
	char char_value[50];
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMBACKUPMANAGER,"backupDomain",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read backupDomain attribute 	via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBackupDomain() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_BRMBACKUPMANAGER,"backupDomain",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read backupDomain attribute via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBackupDomain() method via apgcc", strcmp(retValue_s.c_str(),char_value ), 0 );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getBackupDomain() method", strcmp(retValue_s.c_str(), "BRM_SYSTEM_DATA"), 0 );

}

void test_get_Attribute_AsyncActionProgress(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_get_Attribute_AsyncActionProgress()" << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TARGET TEST ENVIRONMENT:"<<DN_BRMBACKUPMANAGER<<" MO" << endl;
	//cout << " RESULT: acs_bur_BrmBackupManager class constructor invoked without error  \n" << endl;
	cout << " RESULT: BrmBackupManager attribute Backup AsyncActionProgress correctly read   \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	char nameObject[] = DN_BRMBACKUPMANAGER;
	acs_bur_BrmBackupManager ut_bur(nameObject);
	string retValue_s= ut_bur.getAsyncActionProgress();
	cout << "Read asyncActionProgress attribute via BrmBackupManager method: "<<retValue_s<<endl;
	char char_value[50];
	char char_value_s[50];
	bool test_end=false;
	if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMBACKUPMANAGER,"asyncActionProgress",char_value_s)==ACS_CC_SUCCESS && !test_end )
	{
		cout << "Read asyncActionProgress attribute via immlist command: "<<char_value_s<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getAsyncActionProgress() method via immlist command", 0, strcmp(retValue_s.c_str(),char_value_s ) );
		test_end = true;
	}
	else if(ut_acs_bur_Common::test_getObjectAttribute(DN_BRMBACKUPMANAGER,"asyncActionProgress",char_value)==ACS_CC_SUCCESS && !test_end)
	{
		cout << "Read asyncActionProgress attribute via APGCC: "<<char_value<<endl;
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getAsyncActionProgress() method via apgcc", strcmp(retValue_s.c_str(),char_value ), 0 );
		test_end = true;
	}
	else
		ASSERT_EQUALM("CUTE: check return value of acs_bur_BrmBackupManager::getAsyncActionProgress() method", strcmp(retValue_s.c_str(), "id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1"), 0 );

}



void runSuite(){
	cute::suite s;
	//TODO add your test here

	s.push_back(CUTE(test_Constructor_Positive));
	s.push_back(CUTE(test_Constructor_Negative));
	s.push_back(CUTE(test_get_Attribute_ID));
	//s.push_back(CUTE(test_get_Attribute_ID_Neg));
	s.push_back(CUTE(test_get_Attribute_BkpDomain));
	//s.push_back(CUTE(test_get_Attribute_BkpDomain_Neg));
	s.push_back(CUTE(test_get_Attribute_BkpType));
	//s.push_back(CUTE(test_get_Attribute_BkpType_Neg));
	s.push_back(CUTE(test_get_Attribute_AsyncActionProgress));
	//s.push_back(CUTE(test_get_Attribute_AsyncActionProgress_Neg));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test suite BrmBackupManager");

}

int main() {

	cout << "Start Test acs_bur_BrmBackupManager class" << endl;
	runSuite();
	cout << "End Test acs_bur_BrmBackupManager class " << endl;
	return 0;
}


