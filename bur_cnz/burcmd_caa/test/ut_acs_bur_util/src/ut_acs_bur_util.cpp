//============================================================================
// Name        : ut_acs_bur_util.cpp
// Author      : egimarr
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "ut_acs_bur_util.h"


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





void test_do_command(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_do_command " << endl;
	cout << " TEST: Verify that a shell command is called and result was received successful " << endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive case result of echo TEST_DO_COMMAND string was received \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	string dn = "";
	string resultMessage = "";

	int testResult = acs_bur_util::do_command("echo TEST_DO_COMMAND",&resultMessage);

	if (RTN_OK==testResult){
		if(0==strcmp(resultMessage.c_str(),"TEST_DO_COMMAND"))
			testResult = RTN_OK;
		else
			testResult = RTN_FAIL;
	}

	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::getBBMInstance ", RTN_OK , testResult  );
}
void test_getBBMInstance(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_getBBMInstance " << endl;
	cout << " TEST: BrmBackupManager object with DN equal to " << DN_BRMBACKUPMANAGER << " is present in IMM and ready to use"<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive cases object is present in IMM  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	string dn = "";
	string errorMessage = "";
	int errorCode = 0;

	int testResult = acs_bur_util::getBBMInstance(&dn,&errorCode,&errorMessage);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::getBBMInstance ", RTN_OK , testResult  );
}

void test_getBBMInstance1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_getBBMInstance1 " << endl;
	cout << " TEST: BrmBackupManager object with DN equal to " << DN_BRMBACKUPMANAGER << " is present in IMM but not ready because an action was in progress"<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case return FAILURE  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// launch a create backup vis immadm -o brmBackupManager will start a create backup
	int resultSystem = system("immadm -o 0 -p name:SA_STRING_T:Backup-From-Imm brmBackupManagerId=SYSTEM_DATA,brMId=1");

	// Wait 1 second for start action
	sleep(1);
	cout << "1 second delay elapsed.... continue" <<endl;
	string dn = "";
	string errorMessage = "";
	int errorCode = 0;

	int testResult = acs_bur_util::getBBMInstance(&dn,&errorCode,&errorMessage);

	// Cancel action in progress
	resultSystem = system("immadm -o 2 -p name:SA_STRING_T:Backup-From-Imm brmBackupManagerId=SYSTEM_DATA,brMId=1");

	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::getBBMInstance1 ", RTN_FAIL , testResult  );
}

void test_getBBInstance(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_getBBInstance " << endl;
	cout << " TARGET TEST ENVIRONMENT: Distinguish Name of a BrmBackup has been assumed from the created test environment function" << endl;
	cout << " RESULT: Positive cases a BrmBackup object is present in IMM  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	string dn = "";
	string errorMessage = "";
	int errorCode = 0;
	string archiveFileName;
	//int resultSystem;

	//char envCmd[100];

	// get last backup name
	//sprintf(envCmd,"immfind -c BrmBackup  | gawk -F'=|,' '{  print $2 }' | tail -1");

	//string outBuffer="";
	//int resultCmd = acs_bur_util::do_command(envCmd,&outBuffer);

	//archiveFileName = outBuffer;
	archiveFileName = TEST_BKP;

	cout << "LAST BACKUP IS "<< archiveFileName.c_str() << endl;

	int testResult = acs_bur_util::getBBInstance(archiveFileName,&dn,&errorCode,&errorMessage);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::getBBInstance ", RTN_OK , testResult  );
}
void test_getBBInstance1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_getBBInstance1 " << endl;
	cout << " TEST: BrmBackup object with a specific is not present in IMM and fails"<<endl;
	cout << " TARGET TEST ENVIRONMENT: we gived a BrmBackup Name not exiting" << endl;
	cout << " RESULT: Negative case a BrmBackup object is non present in IMM  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	string dn = "";
	string errorMessage = "";
	int errorCode = 0;
	string archiveFileName;

	archiveFileName = string("BACKUPNAME_FAKE");

	int testResult = acs_bur_util::getBBInstance(archiveFileName,&dn,&errorCode,&errorMessage);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::getBBInstance ", RTN_FAIL , testResult  );
}
void test_invokeAction(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_invokeAction " << endl;
	cout << " TEST: Invokes createBackup action on BrmBackupManager object with a correct DN"<<endl;
	cout << " TARGET TEST ENVIRONMENT:  " << endl;
	cout << " RESULT: Positive case createAction was accepted  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	string dn = string(DN_BRMBACKUPMANAGER);
	string backupName = string("BACKUPUNITTEST");
	int errorCode = 0;
	string errorMessage = "";
	int testResult = acs_bur_util::invokeAction( CREATEBACKUP,dn, backupName,&errorCode,&errorMessage);
	sleep(1);
	// cancel action
	system("immadm -o 2 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1");
	sleep(1);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::test_invokeAction ", RTN_OK , testResult  );
}

void test_invokeAction1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_invokeAction1 " << endl;
	cout << " TEST: Invokes createBackup action on BrmBackupManager object with a wrong DN "<<endl;
	cout << " TARGET TEST ENVIRONMENT:  " << endl;
	cout << " RESULT: Negative case createAction was not accepted and INVOKEERR code is produced.  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	string dn = string("brmBackupManagerId=FAKE_DATA,brMId=1");
	string backupName = string("BACKUPUNITTEST");
	int errorCode = 0;
	string errorMessage = "";
	int resultTest = acs_bur_util::invokeAction( CREATEBACKUP,dn, backupName,&errorCode,&errorMessage);
	if (resultTest != RTN_FAIL)
		errorCode = 0;
	sleep(1);
	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::test_invokeAction1 ", INVOKEERR , errorCode  );
}

void test_invokeAction2(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_invokeAction2 " << endl;
	cout << " TEST: Invokes createBackup action on BrmBackupManager object with an backup action in progress "<<endl;
	cout << " TARGET TEST ENVIRONMENT: Create backup with immadm command interface before start test" << endl;
	cout << " RESULT: Negative case createAction was not accepted and OPNOTACEPT code is produced.  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// Create action for test
	system("immadm -o 0 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1");
	// Wait 1 second for start action
	sleep(1);
	cout << "1 second delay elapsed for start createAction invoked via IMM command interface continue" <<endl;

	string dn = string(DN_BRMBACKUPMANAGER);
	string backupName = string("BACKUPUNITTEST");
	int errorCode = 0;
	string errorMessage = "";
	int resultTest = acs_bur_util::invokeAction( CREATEBACKUP,dn, backupName,&errorCode,&errorMessage);
	if (resultTest != RTN_FAIL)
		errorCode = 0;

	// Cancel action for test
	system("immadm -o 2 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1");
	// Wait 1 second for start action
	sleep(1);
	cout << "1 second delay elapsed for start cancelCurrentAction invoked via IMM command interface continue" <<endl;

	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::test_invokeAction2 ", OPNOTACEPT , errorCode  );
}

void test_waitForTerminate(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_waitForTerminate " << endl;
	cout << " TEST: Wait until a create action will be terminate successful "<<endl;
	cout << " TARGET TEST ENVIRONMENT: Starting a create backup " << endl;
	cout << " RESULT: Positive case returns RTN_OK " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	// Create action for test
	system("immadm -o 0 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1");
	// Wait 1 second for start action
	cout << "waits 1 second for start createAction invoked via IMM command interface continue" <<endl;
	sleep(1);

	//
	string dn = string(DN_BRMBACKUPMANAGER);
	string backupCreationTime = string("");
	string backupName = string("BACKUPUNITTEST");
	int errorCode = 0;
	string errorMessage = string("");
	int testResult = acs_bur_util::waitForTeminate(CREATEBACKUP, dn,&backupCreationTime,backupName,&errorCode, &errorMessage);
	if(RTN_OK==testResult){
		cout << "Deleting BACKUPUNITTEST in progress ...." << endl;
		system("immadm -o 0 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1");
		sleep(3);
	}
	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::test_waitForTerminate ", RTN_OK , testResult  );

}

void test_waitForTerminate1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_waitForTerminate1 " << endl;
	cout << " TEST:  Create action not is started and ends with error code becouse backup name not exists "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case returns RTN_FAIL " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	//
	string dn = string(DN_BRMBACKUPMANAGER);
	string backupCreationTime = string("");
	string backupName = string("BACKUPUNITTEST1");
	int errorCode = 0;
	string errorMessage = string("");
	int testResult = acs_bur_util::waitForTeminate(CREATEBACKUP, dn,&backupCreationTime,backupName,&errorCode, &errorMessage);
	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::test_waitForTerminate1 ", RTN_FAIL , testResult  );

}
//bool acs_bur_util::validFileName(string name)
void test_validFileName(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_validFileName " << endl;
	cout << " TEST: Backup name contain alfabetic , numeric , '_' , '-' characters is accepted "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive case returns TRUE " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	string backupName = string("ABC123_-");
	bool testResult = acs_bur_util::validFileName(backupName);
	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::test_validFileName ", true , testResult  );

}
void test_validFileName1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_validFileName1 " << endl;
	cout << " TEST: Backup name contain extra characters like '$' and '%' is not accepted "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case returns FALSE " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	string backupName = string("ABC%123$.$$$");
	bool testResult = acs_bur_util::validFileName(backupName);
	ASSERT_EQUALM("CUTE: check return value of acs_bur_util::test_validFileName1 ", false , testResult  );

}
void runSuite(){
	cute::suite s;
	//TODO add your test here
	if (RTN_FAIL != createBrmBackupTestObject()){
		s.push_back(CUTE(test_do_command));
		s.push_back(CUTE(test_getBBMInstance));
		s.push_back(CUTE(test_getBBMInstance1));
		s.push_back(CUTE(test_getBBInstance));
		s.push_back(CUTE(test_getBBInstance1));
		//
		s.push_back(CUTE(test_invokeAction));
		s.push_back(CUTE(test_invokeAction1));
		s.push_back(CUTE(test_invokeAction2));
		//
		s.push_back(CUTE(test_waitForTerminate));
		s.push_back(CUTE(test_waitForTerminate1));
		//
		s.push_back(CUTE(test_validFileName));
		s.push_back(CUTE(test_validFileName1));
		cute::ide_listener lis;
		cute::makeRunner(lis)(s, "Test suite for acs_bur_util class");
		deleteBrmBackupTestObject();
	}

}

int main() {

	cout << "Start Test acs_bur_util class" << endl;
	runSuite();
	cout << "End Test acs_bur_util class " << endl;
	return 0;
}


