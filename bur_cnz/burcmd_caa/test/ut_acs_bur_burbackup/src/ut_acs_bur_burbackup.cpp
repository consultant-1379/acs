//============================================================================
// Name        : ut_acs_bur_burbackup.cpp
// Author      : egiacri,egimarr
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "ut_acs_bur_burbackup.h"

void create_backup_by_imm(){
	// Here start create backup
	cout << "Create backup by immadm command" << endl;

	int resultSystem = -1;
	string bufferOutput = "";

	acs_bur_util::do_command("immadm -o 0 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1",&bufferOutput);

	if( bufferOutput.find("SA_AIS_ERR_BUSY") != string::npos ){
		cout << "administravive operation on create backup can't be called "<< endl;
		return;
	}

	sleep(2);
	// wait until create backup ends

	while(1){
		sleep(2);
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'result ' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if result == SUCCESS
		if(bufferOutput.compare("1")==0){
			break;
		}
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'state' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if state == CANCELLED
		if(bufferOutput.compare("4")==0){
			break;
		}
	}
}
//
void remove_backup_by_imm()
{

	string bufferOutput = "";
	cout << "Deleting BACKUPUNITTEST backup "<<endl;
	// Delete last test unit backup BACKUPUNITTEST
	acs_bur_util::do_command("immadm -o 1 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1",&bufferOutput);

	cout << "Wait 5 second after delete backup "<<endl;

	sleep(5);
	bufferOutput = "0";

	// wait until delete backup ends
	while(1){
		sleep(1);
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'result ' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if result == SUCCESS
		if(bufferOutput.compare("1")==0){
			break;
		}
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'state' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if state == CANCELLED
		if(bufferOutput.compare("4")==0){
			break;
		}
	}


	string defaultBackupRestorePath = "";

	ACS_APGCC_DNFPath_ReturnTypeT ret;
	char p_Path[256];
	memset((void *)p_Path,0,(size_t)sizeof(p_Path));

	string p_FileMFuncName = "backupRestore";
	int p_Len = 0;

	ACS_APGCC_CommonLib cmLib;
	p_Len = (int)sizeof(p_Path)-1;

	ret = cmLib.GetFileMPath(p_FileMFuncName.c_str(),p_Path,p_Len);

	acs_bur_util::clearCRLF(p_Path);

	defaultBackupRestorePath = p_Path;
	char envCmd[256];

	sprintf(envCmd,"rm %s/BACKUPUNITTEST.tar",p_Path);

	acs_bur_util::do_command(envCmd,&bufferOutput);

}
//
void test_parse(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse " << endl;
	cout << " TEST: Check parse() method with -o and -f parameters in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive cases with parameters accepted  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burbackup",(char *) "-o",(char *) "-f"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	cout << " burbackup -o -f" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::parse() ", true , ((testResult==RTN_OK)&&(errorCode==NOERROR))  );

}

void test_parse1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse1 " << endl;
	cout << " TEST: Check parse() method with -a <backupname> and -f parameters in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive cases with parameters accepted  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 4;

	char* pArgv[4] = {(char *)"burbackup", (char *)"-a",(char *)"MYBACKUP",(char *)"-f"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::parse() ", true , ((testResult==RTN_OK)&&(errorCode==NOERROR)) );

}

void test_parse2(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse2 " << endl;
	cout << " TEST: Check parse() method with -f <backupname> in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case with parameters not accepted without -a  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burbackup", (char *)"-f",(char *)"MYBACKUP"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::parse() ", true , ((testResult==RTN_FAIL)&&(errorCode==SYNTAXERR)) );

}

void test_parse3(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse3 " << endl;
	cout << " TEST: Check parse() method with -a -f in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case with parameter -a without <backfilename>  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burbackup", (char *)"-a",(char *)"-f"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::parse() ", true , ((testResult==RTN_FAIL)&&(errorCode==SYNTAXERR)) );

}

void test_parse4(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse4 " << endl;
	cout << " TEST: Check parse() method with -a <backupname> -o in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case with parameter -a and -o together \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 4;

	char* pArgv[4] = {(char *)"burbackup", (char *)"-a" ,(char *) "MYBACKUP", (char *)"-o"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::parse() ", true , ((testResult==RTN_FAIL)&&(errorCode==SYNTAXERR)) );

}

void test_parse5(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse5 " << endl;
	cout << " TEST: Check parse() method with -g in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case with parameter -g illegal option produce \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 2;

	char* pArgv[2] = {(char *)"burbackup", (char *)"-g" } ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::parse() ", true , ((testResult==RTN_FAIL)&&(errorCode==INVOPTION)) );

}

void test_error_message_handler(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler " << endl;
	cout << " TEST: error_message_handler with exit code 2 for Incorrect Usage "<<endl;
	cout << " TARGET TEST ENVIRONMENT: prepare arguments for an erroneous burbackup -o -a command" << endl;
	cout << " RESULT: return exit code equal 2 and Incorrect usage printout message  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	int iArgc = 4;

	char* pArgv[4] = {(char *)"burbackup", (char *)"-a" ,(char *) "MYBACKUP", (char *)"-o"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int exitCode = 0;
	if (RTN_FAIL == testResult)
		exitCode = objCmd.error_message_handler();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ", 2 , exitCode );
}

void test_error_message_handler1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler1 " << endl;
	cout << " TEST: error_message_handler with exit code 8 for Illegal option "<<endl;
	cout << " TARGET TEST ENVIRONMENT: prepare arguments for an erroneous burbackup -g command" << endl;
	cout << " RESULT: return exit code equal 8 and Illegal option printout message\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	int iArgc = 4;

	char* pArgv[4] = {(char *)"burbackup", (char *)"-g" ,(char *) "MYBACKUP", (char *)"-o"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int exitCode = 0;
	if (RTN_FAIL == testResult)
		exitCode = objCmd.error_message_handler();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ", 8 ,exitCode);
}

void test_error_message_handler2(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler2 " << endl;
	cout << " TEST: error_message_handler with exit code 12 Export failed because backup already exists "<<endl;
	cout << " TARGET TEST ENVIRONMENT: get last Backup in IMM on BRM object and create a tar file in <nbi>/backup_restore/ folder" << endl;
	cout << " RESULT: return exit code 12 and " << MSG_ALREADYEXIST << " printout message\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	// START TEST ENVIRONMENT PREPARATION
	char envCmd[100];

	create_backup_by_imm();

	// get last backup name
	sprintf(envCmd,"immfind -c BrmBackup  | gawk -F'=|,' '{  print $2 }' | tail -1");

	string outBuffer="";
	int resultCmd = acs_bur_util::do_command(envCmd,&outBuffer);

	int iArgc = 4;
	char* pArgv[4] = {(char *)"burbackup",(char *) "-a",(char *) "MYBACKUP", (char *) "-f"} ;
	pArgv[2] = (char *)outBuffer.c_str();

	string backupFileName = "BACKUPUNITTEST";

	string defaultBackupRestorePath = "";

	ACS_APGCC_DNFPath_ReturnTypeT ret;
	char p_Path[256];
	memset((void *)p_Path,0,(size_t)sizeof(p_Path));

	string p_FileMFuncName = "backupRestore";
	int p_Len = 0;

	ACS_APGCC_CommonLib cmLib;
	p_Len = (int)sizeof(p_Path)-1;

	ret = cmLib.GetFileMPath(p_FileMFuncName.c_str(),p_Path,p_Len);

	acs_bur_util::clearCRLF(p_Path);

	defaultBackupRestorePath = p_Path;

	sprintf(envCmd,"touch %s/BACKUPUNITTEST.tar",p_Path,outBuffer.c_str());
	resultCmd = acs_bur_util::do_command(envCmd,&outBuffer);

	// END TEST ENVIRONMENT PREPARATION
	acs_bur_burbackup objCmd(iArgc,pArgv);
	int testResult;
	testResult = objCmd.export_backup(backupFileName);
	testResult = objCmd.error_message_handler();

	ASSERT_EQUALM("CUTE: check return value of error_message_handler() ", 12 ,testResult );

	remove_backup_by_imm();
}

void test_error_message_handler3(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler3 " << endl;
	cout << " TEST: error_message_handler with exit code 4 Export fail because backup already exists "<<endl;
	cout << " TARGET TEST ENVIRONMENT: prepare arguments for an erroneous burbackup -g command" << endl;
	cout << " RESULT: return exit code 4 and " << MSG_DONTEXIST << " printout message\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 4;

	char* pArgv[4] = {(char *)"burbackup",(char *) "-a",(char *) "MYBACKUP", (char *) "-f"} ;

	string fakeBackupName = string("FAKEBACKUP");
	acs_bur_burbackup objCmd(iArgc,pArgv);
	int testResult = objCmd.export_backup(fakeBackupName);

	int exitCode = 0;
	if (RTN_FAIL == testResult)
		exitCode = objCmd.error_message_handler();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ", 4 ,exitCode);
}

void test_error_message_handler4(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler4 " << endl;
	cout << " TEST: error_message_handler with exit code 6 Internal program fault : <function>  "<<endl;
	cout << " TARGET TEST ENVIRONMENT: passing wrong backupName to " << endl;
	cout << " RESULT: return exit code 6 and " << MSG_INTFAULT << " printout message\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// Set NEID to APG24$
	int resultSystem = system("immcfg -a networkManagedElementId=APG24$ managedElementId=1");

	if (RTN_FAIL == resultSystem)
		ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ",RTN_OK , RTN_FAIL);

	int iArgc = 4;

	char* pArgv[4] = {(char *)"burbackup",(char *) "-a",(char *) "MYBACKUP", (char *) "-f"} ;

	string fakeBackupName = string("FAKEBACKUP");
	acs_bur_burbackup objCmd(iArgc,pArgv);
	int testResult;
	string networkElementId = "";

	testResult = objCmd.getNEID(&networkElementId);
	int exitCode = 0;

	if (RTN_FAIL == testResult)
		exitCode = objCmd.error_message_handler();

	// Replaces correct NEID
	resultSystem = system("immcfg -a networkManagedElementId=APG242 managedElementId=1");

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ", 6 ,exitCode);
}

void test_error_message_handler5(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler5 " << endl;
	cout << " TEST: error_message_handler with exit code 3 Backup service error "<<endl;
	cout << " TARGET TEST ENVIRONMENT: launch via immadm a create backup " << endl;
	cout << " RESULT: return exit code 3 and " << MSG_SRVBUSY << " printout message\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// launch a create backup vis immadm -o
	int resultSystem = system("immadm -o 0 -p name:SA_STRING_T:Backup-From-Imm brmBackupManagerId=SYSTEM_DATA,brMId=1");
	sleep(2);

	if (RTN_FAIL == resultSystem)
		ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ",RTN_OK , RTN_FAIL);

	int iArgc = 3;
	char* pArgv[3] = {(char *)"burbackup",(char *) "-o", (char *) "-f"} ;
	acs_bur_burbackup objCmd(iArgc,pArgv);

	int testResult;
	testResult = objCmd.execute();

	sleep(2);
	// Cancel action in progress
	system("immadm -o 2 -p name:SA_STRING_T:Backup-From-Imm brmBackupManagerId=SYSTEM_DATA,brMId=1");
	sleep(10);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ", 3 ,testResult);
}

void test_error_message_handler6(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler6 " << endl;
	cout << " TEST: error_message_handler with exit code 11 Export conflict backup "<<endl;
	cout << " TARGET TEST ENVIRONMENT: launch via immadm a create backup " << endl;
	cout << " RESULT: return exit code 11 and " << MSG_EXPCONF << " printout message\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// start a create backup via immadm -o
	int resultSystem;

	char envCmd[100];

	// get last backup name
	sprintf(envCmd,"immfind -c BrmBackup  | gawk -F'=|,' '{  print $2 }' | tail -1");

	string outBuffer="";
	int resultCmd = acs_bur_util::do_command(envCmd,&outBuffer);

	string backupFileName = outBuffer;

	sprintf(envCmd,"rm /data/opt/ap/nbi/backup_restore/%s.tar",outBuffer.c_str());
	resultCmd = acs_bur_util::do_command(envCmd,&outBuffer);

	resultSystem = system("immadm -o 0 -p name:SA_STRING_T:FakeBackup brmBackupManagerId=SYSTEM_DATA,brMId=1");

	if (RTN_FAIL == resultSystem)
		ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ",RTN_OK , RTN_FAIL);

	int iArgc = 4;
	char* pArgv[4] = {(char *)"burbackup",(char *) "-a",(char *)"MYBACKUP", (char *) "-f"} ;
	acs_bur_burbackup objCmd(iArgc,pArgv);

	int testResult;
	testResult = objCmd.export_backup(backupFileName);

	testResult = objCmd.error_message_handler();

	// Cancel action in progress
	resultSystem = system("immadm -o 2 -p name:SA_STRING_T:FakeBackup brmBackupManagerId=SYSTEM_DATA,brMId=1");
	sleep(3);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::error_message_handler() ", 11 ,testResult);
}


void test_execute(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_execute " << endl;
	cout << " TEST: execute methods for a complete backup simulating a burbackup -o -f command"<<endl;
	cout << " TARGET TEST ENVIRONMENT:  " << endl;
	cout << " RESULT: RTN_OK and a complite backup will be create  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	int iArgc = 3;

	char* pArgv[3] = {(char *)"burbackup",(char *) "-o",(char *) "-f"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);

	int testResult = objCmd.execute();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::execute() ", RTN_OK , testResult );

}

void test_export_backup(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_export_backup " << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TARGET TEST ENVIRONMENT: get from immfind -c command last BrmBackup object created, delete image backup file from <nbi>/backup_restore/ folder" << endl;
	cout << " RESULT: export a backup without error  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// START TEST ENVIRONMENT PREPARATION
	create_backup_by_imm();

	string outBuffer="BACKUPUNITTEST";

	// END TEST ENVIRONMENT PREPARATION

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burbackup",(char *) "-a",(char *) "-f"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);
	int testResult = objCmd.export_backup(outBuffer);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::export_backup() ", RTN_OK , testResult );

	remove_backup_by_imm();

}

void test_export_backup_1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_export_backup_1 " << endl;
	cout << " TEST: Create an instance of the class acs_bur_BrmBackupManager "<<endl;
	cout << " TARGET TEST ENVIRONMENT: get from immfind -c command last BrmBackup object created, delete image backup file from <nbi>/backup_restore/ folder" << endl;
	cout << " RESULT: Negative case export a not exist backup \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// Here start create backup
	// create_backup_by_imm();
	//

	string outBuffer="BACKUPNOTEXIT";

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burbackup",(char *) "-a",(char *) "-f"} ;

	acs_bur_burbackup objCmd(iArgc,pArgv);
	int testResult = objCmd.export_backup(outBuffer);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::export_backup_1() ", RTN_FAIL , testResult );

	// remove_backup_by_imm();
}


void test_getNEID(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_getNEID " << endl;
	cout << " TEST: Get Network Element Identification form IMM "<<endl;
	cout << " TARGET TEST ENVIRONMENT: set networkManagedElementId with immcfg in IMM" << endl;
	cout << " RESULT: Return string that setted in MOM  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	int iArgc = 2;
	char* pArgv[2] = {(char *)"burbackup",(char *) "-o" } ;
	acs_bur_burbackup objCmd(iArgc,pArgv);
	string networkElementId = "";

	// Set NEID to APG242
	int resultSystem = system("immcfg -a networkManagedElementId=APG242 managedElementId=1");
	if (resultSystem == -1)
		ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::getNEID() ", RTN_OK , resultSystem  );

	int testResult = objCmd.getNEID(&networkElementId);
	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::getNEID() ", true ,  ((testResult==RTN_OK)&&(networkElementId.compare("APG242")==0)) );

}

void test_getNEID1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_getNEID1 " << endl;
	cout << " TEST: Get a wrong Network Element Identification form IMM "<<endl;
	cout << " TARGET TEST ENVIRONMENT: set networkManagedElementId with APG24$ string by immcfg command in IMM" << endl;
	cout << " RESULT: Return negative case cause returned name with a character not valid \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;
	int iArgc = 2;
	char* pArgv[2] = {(char *)"burbackup",(char *) "-o" } ;
	acs_bur_burbackup objCmd(iArgc,pArgv);
	string networkElementId = "";
	int testResult = 0;

	// Set NEID to APG24$
	int resultSystem = system("immcfg -a networkManagedElementId=APG24$ managedElementId=1");

	if (RTN_FAIL != resultSystem)
		testResult = objCmd.getNEID(&networkElementId);
	// Reset NEID to correct value
	resultSystem = system("immcfg -a networkManagedElementId=APG242 managedElementId=1");

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::getNEID() ", true , ( (testResult==RTN_FAIL) && (EXTALFNUM == objCmd.getOpCode()) )  );

}

void test_makeBackupName(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_makeBackupName " << endl;
	cout << " TEST: Build a backupName following current roules "<<endl;
	cout << " TARGET TEST ENVIRONMENT:  " << endl;
	cout << " RESULT: Return RTN_OK  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// START TEST PREPARATION

	// Set NEID to APG242
	int resultSystem = system("immcfg -a networkManagedElementId=APG242 managedElementId=1");
	if (RTN_FAIL == resultSystem)
		ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::makeBackupName() ", RTN_OK , resultSystem );

	// END TEST PREPARATION


	int iArgc = 3;
	char* pArgv[3] = {(char *)"burbackup",(char *) "-o" , (char *) "-f"} ;
	acs_bur_burbackup objCmd(iArgc,pArgv);
	string networkElementId = "";
	string newBackupName = "";


	int testResult = objCmd.makeBackupName(&newBackupName,&networkElementId);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::makeBackupName() ", RTN_OK , testResult );

}

void test_makeBackupName1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_makeBackupName1 " << endl;
	cout << " TEST: Build a wrong backupName following current roules "<<endl;
	cout << " TARGET TEST ENVIRONMENT: set networkManagedElementId with APG24$ string by immcfg command in IMM " << endl;
	cout << " RESULT: Return RTN_FAIL (Negative case) \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// START TEST PREPARATION

	// Set NEID to APG24$
	int resultSystem = system("immcfg -a networkManagedElementId=APG24$ managedElementId=1");
	if (RTN_FAIL == resultSystem)
		ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::makeBackupName() ", RTN_OK , resultSystem );

	// END TEST PREPARATION

	int iArgc = 3;
	char* pArgv[3] = {(char *)"burbackup",(char *) "-o" , (char *) "-f"} ;
	acs_bur_burbackup objCmd(iArgc,pArgv);

	string networkElementId = "";
	string newBackupName = "";

	int testResult = objCmd.makeBackupName(&newBackupName,&networkElementId);

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::makeBackupName() ", RTN_FAIL , testResult );

}


void runSuite(){
	cute::suite s;
	//TODO add your test here

	s.push_back(CUTE(test_parse));
	s.push_back(CUTE(test_parse1));
	s.push_back(CUTE(test_parse2));
	s.push_back(CUTE(test_parse3));
	s.push_back(CUTE(test_parse4));
	s.push_back(CUTE(test_parse5));

	s.push_back(CUTE(test_error_message_handler));
	s.push_back(CUTE(test_error_message_handler1));
	s.push_back(CUTE(test_error_message_handler2));
	s.push_back(CUTE(test_error_message_handler3));
	s.push_back(CUTE(test_error_message_handler4));
	s.push_back(CUTE(test_error_message_handler5));
	//s.push_back(CUTE(test_error_message_handler6));
	s.push_back(CUTE(test_execute));
	s.push_back(CUTE(test_export_backup));
	s.push_back(CUTE(test_export_backup_1));
	s.push_back(CUTE(test_getNEID1));
	s.push_back(CUTE(test_getNEID));
	s.push_back(CUTE(test_makeBackupName1));
	s.push_back(CUTE(test_makeBackupName));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test suite for acs_bur_burbackup class");

}

int main() {

	cout << "Start Test acs_bur_burbackup class" << endl;
	runSuite();
	cout << "End Test acs_bur_burbackup class " << endl;
	return 0;
}


