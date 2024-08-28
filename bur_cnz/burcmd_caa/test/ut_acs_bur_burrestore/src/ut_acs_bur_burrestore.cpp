//============================================================================
// Name        : ut_acs_bur_burbackup.cpp
// Author      : egimarr
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "ut_acs_bur_burrestore.h"


void test_parse(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse " << endl;
	cout << " TEST: Check parse() method with -o and -f parameters in burrestore command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive cases with parameters accepted  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 4;

	char* pArgv[4] = {(char *)"burrestore",(char *) "-o",(char *) "BACKUPFAKE",(char *) "-f"} ;

	acs_bur_burrestore objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::parse() ", true , ((testResult==RTN_OK)&&(errorCode==NOERROR))  );

}

void test_parse1(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse1 " << endl;
	cout << " TEST: Check parse() method with -a <backupname> and -f parameters in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive cases with parameters accepted  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 4;

	char* pArgv[4] = {(char *)"burrestore", (char *)"-a",(char *)"MYBACKUP",(char *)"-f"} ;

	acs_bur_burrestore objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::parse() ", true , ((testResult==RTN_OK)&&(errorCode==NOERROR)) );

}

void test_parse2(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse2 " << endl;
	cout << " TEST: Check parse() method with -f <backupname> in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case with parameters not accepted without -a  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burrestore", (char *)"-f",(char *)"MYBACKUP"} ;

	acs_bur_burrestore objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::parse() ", true , ((testResult==RTN_FAIL)&&(errorCode==SYNTAXERR)) );

}

void test_parse3(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse3 " << endl;
	cout << " TEST: Check parse() method with -a -f in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case with parameter -a without <backfilename>  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burrestore", (char *)"-a",(char *)"-f"} ;

	acs_bur_burrestore objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::parse() ", true , ((testResult==RTN_FAIL)&&(errorCode==SYNTAXERR)) );

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

	acs_bur_burrestore objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int errorCode = objCmd.getOpCode();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::parse() ", true , ((testResult==RTN_FAIL)&&(errorCode==SYNTAXERR)) );

}

void test_parse5(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_parse5 " << endl;
	cout << " TEST: Check parse() method with -g in burbackup command "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative case with parameter -g illegal option produce \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int iArgc = 2;

	char* pArgv[2] = {(char *)"burrestore", (char *)"-g" } ;

	acs_bur_burrestore objCmd(iArgc,pArgv);

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

	char* pArgv[4] = {(char *)"burrestore", (char *)"-a" ,(char *) "MYBACKUP", (char *)"-o"} ;

	acs_bur_burrestore objCmd(iArgc,pArgv);

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

	char* pArgv[4] = {(char *)"burrestore", (char *)"-g" ,(char *) "MYBACKUP", (char *)"-o"} ;

	acs_bur_burrestore objCmd(iArgc,pArgv);

	string backupName = "";
	bool only_export = false;
	int testResult = objCmd.parse(iArgc,pArgv,&backupName,&only_export);
	int exitCode = 0;
	if (RTN_FAIL == testResult)
		exitCode = objCmd.error_message_handler();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::error_message_handler() ", 8 ,exitCode);
}

void test_error_message_handler2(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_error_message_handler2 " << endl;
	cout << " TEST: error_message_handler with exit code 4 Import backaup failed because backup does't exist "<<endl;
	cout << " TARGET TEST ENVIRONMENT:" << endl;
	cout << " RESULT: return exit code 4 and " << MSG_BKPARCNOTEX << " printout message\n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	string backupFileName = string("BACKUP_FAKE");
	int iArgc = 4;

	char* pArgv[4] = {(char *)"burrestore", (char *)"-a" ,(char *) "MYBACKUP", (char *)"-o"} ;
	acs_bur_burrestore objCmd(iArgc,pArgv);
	int testResult;
	string backupName = string("");
	testResult = objCmd.import_backup(backupFileName,&backupName);
	testResult = objCmd.error_message_handler();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::error_message_handler() ", 4 ,testResult );
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

	acs_bur_burrestore objCmd(iArgc,pArgv);

	int testResult = objCmd.execute();

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burbackup::execute() ", RTN_OK , testResult );

}

void test_import_backup(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_import_backup " << endl;
	cout << " TEST: Import backup fail if name is already created."<<endl;
	cout << " TARGET TEST ENVIRONMENT: get from immfind -c command last BrmBackup object created, copy image backup file from <nbi>/backup_restore/ folder in a athor file name" << endl;
	cout << " RESULT: Negative case RTN_FAIL returned  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	// START TEST ENVIRONMENT PREPARATION
	char envCmd[100];

	// get last backup name
	sprintf(envCmd,"immfind -c BrmBackup  | gawk -F'=|,' '{  print $2 }' | tail -1");

	string outBuffer="";
	if (RTN_FAIL == acs_bur_util::do_command(envCmd,&outBuffer))
		return;

	// remove image tar file from <nbi>/backup_restore/ folder
	sprintf(envCmd,"cp /data/opt/ap/nbi/backup_restore/%s.tar /data/opt/ap/nbi/backup_restore/backupfake.tar",outBuffer.c_str());
	outBuffer="";

	if (RTN_FAIL == acs_bur_util::do_command(envCmd,&outBuffer))
		return;

	// END TEST ENVIRONMENT PREPARATION

	int iArgc = 3;

	char* pArgv[3] = {(char *)"burrestore",(char *) "-a",(char *) "-f"} ;

	acs_bur_burrestore objCmd(iArgc,pArgv);
	//
	string backupFileName = "backupfake";
	string backupName = "";
	int testResult = objCmd.import_backup(backupFileName,&backupName);

	// remove image tar file from <nbi>/backup_restore/ folder
	sprintf(envCmd,"rm /data/opt/ap/nbi/backup_restore/backupfake.tar",outBuffer.c_str());
	outBuffer="";

	if (RTN_FAIL == acs_bur_util::do_command(envCmd,&outBuffer))
		return;

	ASSERT_EQUALM("CUTE: check return value of acs_bur_burrestore::execute() ", RTN_FAIL , testResult );

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
	s.push_back(CUTE(test_import_backup));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test suite for acs_bur_burrestore class");

}

int main() {

	cout << "Start Test acs_bur_burrestore class" << endl;
	runSuite();
	cout << "End Test acs_bur_burrestore class " << endl;
	return 0;
}


