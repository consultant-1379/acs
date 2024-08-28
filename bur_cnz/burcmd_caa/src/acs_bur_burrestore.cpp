/*
 * acs_bur_burrestore.cpp
 *
 *  Modified on: 2 Apr 2014
 *      Author: xquydao
 *  Created on: 1 Dec 2011
 *      Author: egimarr
 */
#include "acs_bur_Define.h"
#include "acs_bur_burrestore.h"
#include "acs_bur_BrmAsyncActionProgress.h"
#include "acs_bur_BrmBackupManager.h"
#include "acs_bur_BrmBackup.h"
#include "ACS_TRA_trace.h"
#include "acs_bur_trace.h"
#include <ace/ACE.h>
#include "ACS_APGCC_CommonLib.h"
#include "ACS_CS_API.h"
#include "acs_prc_api.h"
#include "acs_bur_util.h"

int acs_bur_burrestore::error_message_handler()
{
	int code = getOpCode();
	char msg[256];
	TRACE(m_restore_trace, "%s %d","Error_message_handler() with code",code);
	switch (code){

		case NOERROR :
			// OK Successful Execution command
			return 0;

		case FORCEDQUIT:
			// OK Forced to quit forced exit (n)
			return 0;

		case GENERALFAULT :
			sprintf(msg, MSG_GENERAL,m_errorMessage.c_str());
			cout << msg << endl;
			return 1;

		case SYNTAXERR:
			// KO Incorrect Usage print usage
			cout << MSG_INCUSA << endl;
			print_usage();
			return 2;

		case OPNOTACEPT :
			// KO BRF not available or Busy
			cout << MSG_SRVBUSY << endl;
			return 3;

		case DONTEXIST:
			sprintf(msg,MSG_BKPARCNOTEX,m_errorMessage.c_str());
			cout <<  msg << endl;
			return 4;

		case CHGCUROP:
			// KO restore aborted
			cout << MSG_SYSTEMRESTORE_ABORT << endl;
			return 5;

		case PATHDOESNOTSEXIST:
		case SETUIDERR:
		case POPENERR:
		case INVALIDIMAGE:
		case IMPORTFAILS:
		case INITERROR:
		case GETATTRERROR:
		case FINALIZEERROR:
			// Internal program fault
			cout << MSG_INTFAULT << m_errorMessage.c_str() << endl;
			return 6;

		case BACKUPFAIL :
			cout << MSG_SYSTEMRESTORE_FAIL << endl;
			return 7;

		case INVOPTION :
			// KO Invalid option
			cout << MSG_ILLOPT << m_errorMessage.c_str() << endl;
			return 8;

		case PARINVNAME:
		case PATHNOTCOR:
		case EXTNAMERR:
			cout << MSG_BKPNOTVALID << endl; // "<archivename> is not a valid backup file name"
			return 9;

		case NOSPACELEFT:
			cout << MSG_NOSPACERST << endl;
			return 10;

		case OPEINPROG:
			cout << MSG_CONFLICT << endl;
			return 11;

		case ALREADYEXIST :
			cout << MSG_RSTALREXIST<< endl;
			return 12;

		case UNKNOWARCTYPE:
			cout << MSG_UNKNOWFRT << endl;
			return 13;

		case NOTBRFBKP:
			cout << MSG_INVBKPTYP << endl;
			return 14;

		case INVOKEERR:
		case BRFCONFLICT:
			cout << MSG_BRF_CONFLICT << endl;
			return 15;
		case INVALIDPWD:
			cout << endl << MSG_RESTOREINVALIDPWD << endl;
			return 16;
		case INVALIDOPT:
			cout << MSG_INVALIDOPT << endl;
			return 17;
		case MISSINGOPT:
			cout << MSG_MISSINGOPT << endl;
			return 18;

	}
	return 0;
}


acs_bur_burrestore::acs_bur_burrestore(int argc , char* argv[] ): argc_ (argc), argv_ (argv)
{
	m_restore_trace = new ACS_TRA_trace("acs_bur_burrestore");
	TRACE(m_restore_trace, "%s","acs_bur_burrestore::Constructor()");
	m_burLog = new ACS_TRA_Logging();
	m_burLog->Open("BUR");
	setOpCode(NOERROR);

	// Initialize private attributes
	m_dnBrmBackup = "";
	m_dnBrmBackupManager = "";	
	m_archive_filename = "";
	m_backupCreationTime = "";
	m_label = "";
	m_backupName = "";
	m_backupPasswd = "";
	m_errorMessage = "";
	m_only_import = false;
	m_isSecured = false;
}

void acs_bur_burrestore::print_usage()
{
	cout << "Usage:" << endl;
	cout << " burrestore -o archive_filename [-p] [-f]" << endl;
	cout << " burrestore -a archive_filename [-f]" << endl;
}

//
// This method return 0 if command is correctly parsering
// 					  1 if command is correctly parsering but no was digit without -f option
//					  -1   command is incorrect
int acs_bur_burrestore::parse(int argc_ ,char ** argv_,string *archiveFileName,string *backupPasswd,bool *onlyImport, bool *isSecuredBackup)
{
	int num_o = 0;
	int num_f = 0;
	int num_a = 0;
	int num_p = 0;
	int num_filename = 0;

	TRACE(m_restore_trace, "%s","Enter in parse()");
	*archiveFileName = "";
	*backupPasswd = "";
	for (int n = 1; n < argc_; n++){
	    if (strcmp(argv_[n],"-o")==0){
	    	num_o++;
	    	TRACE(m_restore_trace, "%s", "parse() found -o option");
	    }
	    else if (strcmp(argv_[n],"-f")==0) {
	    	num_f++;
	    	TRACE(m_restore_trace, "%s", "parse() found -f option ");
	    }else if (strcmp(argv_[n],"-p")==0) {
	    	num_p++;
	    	TRACE(m_restore_trace, "%s", "parse() found -p option ");
	    }else if (strcmp(argv_[n],"-a")==0) {
	    	num_a++;
	    	TRACE(m_restore_trace, "%s", "parse() found -a option ");
	    	if ((argc_ == (n+1)) ){
	    		TRACE(m_restore_trace, "%s", "parse() -a without <backupname> ");
	    		setOpCode(SYNTAXERR);
	    		return RTN_FAIL;
	    	}
	    	else{
	    		if(argv_[n+1][0] == '-'){
	    			TRACE(m_restore_trace,"%s","parse() -a with an option instead of backupname");
	    		    setOpCode(SYNTAXERR);
	    		    return RTN_FAIL;
	    		}
	    	}
	    }else if (argv_[n][0] == '-'){
	    	setOpCode(INVOPTION, string( argv_[n] ));
	    	m_burLog->Write("parse(): option - without letter ",LOG_LEVEL_DEBUG);
	    	return RTN_FAIL;
	    }else if (num_filename == 1){
			m_burLog->Write("parse(): duplicate <parameter> ",LOG_LEVEL_DEBUG);
	    	setOpCode(SYNTAXERR);
	    	return RTN_FAIL;
	    }else {
	    	num_filename++;
	    	*archiveFileName = argv_[n];
	    }

	    if ( (num_f > 1)||(num_o > 1)||(num_a > 1) ||(num_p > 1) ||(num_filename > 1) ) {
	    	setOpCode(SYNTAXERR);
			m_burLog->Write("parse(): duplicate option",LOG_LEVEL_DEBUG);
	    	return RTN_FAIL;
	    }
	}

	// option -o -a together
    if ( ((1 == num_o) || (1 == num_p))  && (1 == num_a) ){
		m_burLog->Write("parse(): -o/-p with -a options not are valid",LOG_LEVEL_DEBUG);
    	setOpCode(SYNTAXERR);
    	return RTN_FAIL;
    }

    // option -o and -a not present
    if ( ( 0 == num_o)  && ( 0 == num_a) ){
 		m_burLog->Write("parse(): -o or -a option must be present",LOG_LEVEL_DEBUG);
     	setOpCode(SYNTAXERR);
     	return RTN_FAIL;
     }

    // -o option must be with filename
    if ( (1 == num_o) && ( 0 == num_filename) ){
		m_burLog->Write("parse(): -o without filename not is valid",LOG_LEVEL_DEBUG);
    	setOpCode(SYNTAXERR);
    	return RTN_FAIL;
    }

	// command with filename without -a
    if( ( 0 == num_filename ) && (1 == num_a) ){
    	setOpCode(SYNTAXERR);
		m_burLog->Write("parse(): <filename> without -a option not is valid",LOG_LEVEL_DEBUG);
		return RTN_FAIL;
	}

    // command with -f without -o OR without -a
    if ( (1 == num_f) && ( 0 == num_a) && ( 0 == num_o) ){
    	setOpCode(SYNTAXERR);
    	m_burLog->Write("parse(): -f without -o OR without -a is not valid",LOG_LEVEL_DEBUG);
		return RTN_FAIL;
	}

	if(num_o == 1)
	{
		string backupName = "/data/opt/ap/internal_root/backup_restore/" + *archiveFileName;
		FILE *testFileToImport;
		testFileToImport = fopen(backupName.c_str(), "r");
		// test if file exist on system file
		if (NULL!=testFileToImport)
    		{
        		fclose(testFileToImport);
        		// OK FILE Exist
			string command = "tar -tvf " + backupName + " | awk -F/ '{ if($NF != \"\") print $NF }'";
			string backupList = acs_bur_util::exec(command.c_str());
			bool secureBackup = false;
			if( backupList.find("config.tar.gz.enc") != string::npos )
			{
				secureBackup = true;
			}
			if( secureBackup && (num_p == 0) )
			{
				setOpCode(MISSINGOPT);
			        m_burLog->Write("parse(): -o without -p for secured backup is not valid",LOG_LEVEL_DEBUG);
	                	return RTN_FAIL;
			}
			if( !secureBackup && (num_p == 1))
			{
				setOpCode(INVALIDOPT);
			        m_burLog->Write("parse(): -o with -p for regular backup is not valid",LOG_LEVEL_DEBUG);
	                	return RTN_FAIL;
			}
    		}
		else
		{
			setOpCode(DONTEXIST,*archiveFileName);
                        m_burLog->Write("parse(): Backup archive does not exist",LOG_LEVEL_DEBUG);
                        return RTN_FAIL;
		}
	}


 	TRACE(m_restore_trace, "%s %s", "parse(): m_label ",m_label.c_str());

 	bool answer = true;

	if(0 == num_f){
		std::cout << MSG_EXEBREST << std::endl;
		if(1 == num_o){
			if(1 == num_p)
			{
				std::cout << "burrestore -o -p" << std::endl;
			}
			else
			{
				std::cout << "burrestore -o" << std::endl;
			}
		}
		else{
			cout << "burrestore -a "<< (*archiveFileName).c_str() << std::endl;
		}

		string question = "";

		answer = acs_bur_util::affirm(question);
	}

	if( !answer ){
		TRACE(m_restore_trace, "%s", "parse(): Quit by command, no execution.");
		setOpCode(FORCEDQUIT);
	  	m_burLog->Write("parse(): quitting by burrestore [n] key pressed ",LOG_LEVEL_DEBUG);
		return RTN_FAIL; //
	}

	*onlyImport = false;
        *isSecuredBackup = false;

	if(1 == num_a){
		*onlyImport = true;
	}

	if(RTN_FAIL==getLabel(*archiveFileName,&m_label))
	{
		TRACE(m_restore_trace, "%s ", "parse(): <imagename> is not a valid name");
		m_burLog->Write("parse(): <imagename> is not a valid name",LOG_LEVEL_DEBUG);
		return RTN_FAIL;
	}
	if(1 == num_p){
                *isSecuredBackup = true;
		cout << "Password\03: " << flush;
		string backupPass = acs_bur_util::getPassword();
		*backupPasswd = backupPass; 
	}
	cout << endl ;
	return RTN_OK;
}

int acs_bur_burrestore::getLabel(const string pathFileName,string *label)
{
	TRACE(m_restore_trace, "%s %s", "getLabel starting passing ",pathFileName.c_str());
	string path_search = "/"; // BACKUPRESTOREPATH;
	string substring = "";

	string::size_type found = pathFileName.find(path_search);
	if (string::npos != found){
    	// founded path "/"
    	substring = pathFileName.substr(found+path_search.length());
    	TRACE(m_restore_trace, "getLabel() %s %s", "found / path , subpath: ",substring.c_str());
       	setOpCode(PATHNOTCOR,pathFileName);
    	m_burLog->Write("getLabel(): Path not correct ",LOG_LEVEL_DEBUG);
        return RTN_FAIL;
    }
    else{
    	substring = pathFileName;
    	TRACE(m_restore_trace, "getLabel() %s ", "/ path not found");
    }

    string::size_type foundExtention = substring.find(".");
    if ( string::npos != foundExtention){
    	TRACE(m_restore_trace, " %s ", " found extension file ");
     	setOpCode(EXTNAMERR,pathFileName);
	  	m_burLog->Write("getLabel(): File extension name error ",LOG_LEVEL_DEBUG);
    	return RTN_FAIL;
    }

    *label = substring;

    TRACE(m_restore_trace, " %s ", "getLabel() RTN_OK");
	return RTN_OK;
}


int acs_bur_burrestore::getOpCode()
{
	return opCode;
}

void acs_bur_burrestore::setOpCode(int code)
{
	opCode = code;
	m_errorMessage = "";
}
void acs_bur_burrestore::setOpCode(int code,string message)
{
	m_errorMessage = message;
	opCode = code;
}


acs_bur_burrestore::~acs_bur_burrestore()
{
	TRACE(m_restore_trace, "%s ", "acs_bur_burrestore::~acs_bur_burrestore() ");
    if (NULL != m_restore_trace)
    {
    	delete m_restore_trace;
    }
    if (NULL != m_burLog)
    {
        delete m_burLog;
    }
}

//
// This method open tar file and get from config.metadata file the backup name
// input parameter tarFileName is a full path system file name
// output parameter backupName is the name of backup
//
int acs_bur_burrestore::getBackupNameFromTarFile(string tarFileName,string *backupName)
{
	// get folder name from tar file
	char cmdShell[256];
	char precmd[80];
	string tarFolder;
	string outputBuffer;
	string softwareMetadata;
	string configMetadata;
	int result;
	string workPath="/cluster/home/";

	// Formatting command shell join tar file name
	sprintf(cmdShell,"tar tvf %s | gawk  '{ print $6 }' | gawk -F'/' '{print $1}' | tail -1",tarFileName.c_str());

	// trace command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() get folder command formatting : ",cmdShell);

	result = acs_bur_util::do_command((const char *)cmdShell,&outputBuffer);
	// execute command
	if(RTN_OK != result)
    {
		if(SETUIDERR==result)
			setOpCode(result,"setuid fail()");
		else
			setOpCode(result,"popen fail()");
		// log message of error
		m_burLog->Write("acs_bur_burrestore::getBackupNameFromTarFile error do_command() getting folder name from tar image backup file",LOG_LEVEL_ERROR);
		return RTN_FAIL;
    }
	// trace output buffer command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() output buffer : ",outputBuffer.c_str());

	tarFolder = outputBuffer;

	sprintf(precmd,"cd /cluster/home/tmp");
	// make a temporany folder using follow script
	//  if [ ! -d '/cluster/home/$username/tmp' ]; then mkdir '/cluster/home/$username/tmp' ; fi
	sprintf(cmdShell,"if [ ! -d '/cluster/home/tmp' ]; then mkdir '/cluster/home/tmp'; fi");

	// trace command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() mkdir '/cluster/home/%s/tmp ... command formatting : ",cmdShell);

	// execute command
	result = acs_bur_util::do_command((const char *)cmdShell,&outputBuffer);
	if(RTN_OK != result)
    {
		if(SETUIDERR==result)
			setOpCode(result,"setuid fail()");
		else
			setOpCode(result,"popen fail()");

		m_burLog->Write("acs_bur_burrestore::getBackupNameFromTarFile error do_command() getting backup name from config.metadata",LOG_LEVEL_ERROR);
    	return RTN_FAIL;
    }

	// Formatting command shell join folder tar file
	sprintf(cmdShell,"%s ; tar xpvf %s %s/config.metadata  > /dev/null 2>&1",precmd,tarFileName.c_str(),tarFolder.c_str());

	// trace command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() tar xpvf ... command formatting : ",cmdShell);

	// execute command
	result = acs_bur_util::do_command((const char *)cmdShell,&outputBuffer);

	if(RTN_OK != result)
    {
		if(SETUIDERR==result)
			setOpCode(result,"setuid fail()");
		else
			setOpCode(result,"popen fail()");

		m_burLog->Write("acs_bur_burrestore::getBackupNameFromTarFile error do_command() getting backup name from config.metadata",LOG_LEVEL_ERROR);
    	return RTN_FAIL;
    }

	// Formatting grep backupName $FOLDER/config.metadata | gawk '{ print $2 }'
	sprintf(cmdShell,"%s ; grep backupName %s/config.metadata | gawk '{ print $2 }'",precmd,tarFolder.c_str());

	// trace command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() grep backup ... command formatting : ",cmdShell);

	// execute command
	result = acs_bur_util::do_command((const char *)cmdShell,&outputBuffer);

	if(RTN_OK != result)
    {
		if(SETUIDERR==result)
			setOpCode(result,"setuid fail()");
		else
			setOpCode(result,"popen fail()");

		m_burLog->Write("acs_bur_burrestore::getBackupNameFromTarFile error do_command() grep backup ...",LOG_LEVEL_ERROR);

    	return RTN_FAIL;
    }

	// trace output buffer command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() output buffer : ",outputBuffer.c_str());

	*backupName = outputBuffer;

	configMetadata = outputBuffer;

	// Formatting command shell join folder tar file
	sprintf(cmdShell,"%s ; tar xpvf %s %s/software.metadata  > /dev/null 2>&1",precmd,tarFileName.c_str(),tarFolder.c_str());

	// trace command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() tar xpvf ... command formatting : ",cmdShell);

	// execute command
	result = acs_bur_util::do_command((const char *)cmdShell,&outputBuffer);

	if(RTN_OK != result)
    {
		if(SETUIDERR==result)
			setOpCode(result,"setuid fail()");
		else
			setOpCode(result,"popen fail()");

		m_burLog->Write("acs_bur_burrestore::getBackupNameFromTarFile error do_command() getting backup name from config.metadata",LOG_LEVEL_ERROR);
    	return RTN_FAIL;
    }

	// Formatting grep backupName $FOLDER/software.metadata | gawk '{ print $2 }'
	sprintf(cmdShell,"%s ; grep backupName %s/software.metadata | gawk '{ print $2 }'",precmd,tarFolder.c_str());

	// trace command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() grep backup ... command formatting : ",cmdShell);

	// execute command
	result = acs_bur_util::do_command((const char *)cmdShell,&outputBuffer);

	if(RTN_OK != result)
    {
		if(SETUIDERR==result)
			setOpCode(result,"setuid fail()");
		else
			setOpCode(result,"popen fail()");

		m_burLog->Write("acs_bur_burrestore::getBackupNameFromTarFile error do_command() grep backup ...",LOG_LEVEL_ERROR);

    	return RTN_FAIL;
    }

	// trace output buffer command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() output buffer : ",outputBuffer.c_str());

	softwareMetadata = outputBuffer;

	// Clear temp folder

	// Formatting command shell join folder tar file
	sprintf(cmdShell,"%s ; cd .. ; rm -r tmp ",precmd);

	// trace command
	TRACE(m_restore_trace,"%s %s","getBackupNameFromTarFile() remove tmp folder command formatting : ",cmdShell);

	// execute command
	result = acs_bur_util::do_command((const char *)cmdShell,&outputBuffer);

	if(RTN_OK != result)
    {
		if(SETUIDERR==result)
			setOpCode(result,"setuid fail()");
		else
			setOpCode(result,"popen fail()");

		m_burLog->Write("acs_bur_burrestore::getBackupNameFromTarFile error do_command() remove folder",LOG_LEVEL_ERROR);

    	return RTN_FAIL;
    }

	// trace output buffer command

	TRACE(m_restore_trace,"getBackupNameFromTarFile() softwareMetadata : %s configMetadata : %s",softwareMetadata.c_str(),configMetadata.c_str());

	if(softwareMetadata!=configMetadata){
		setOpCode(NOTBRFBKP);
		return RTN_FAIL;
	}

	// return ok
	return RTN_OK;
}


int acs_bur_burrestore::import_backup(string backupFileName,string *backupName)
{
	// After import digit command: "lde-brf print import -t system"
	// will return last label
	//
	// lde-brf import -l <label> -f <complete file path name> -t user
	// result can be : "specified type config missing" error 2
	//				   "not found" error 99
	//                 "Unknow archive type" errore su file non di tipo tar - error 2
	//				   "is not a BRF backup" error 99


	//
	// Syntax lde-brf import -t backuptype -f pathfilename
	//
	string defaultBackupRestorePath = "";
	char msg[256];
	ACS_APGCC_DNFPath_ReturnTypeT ret;
	char p_Path[256];
	memset((void *)p_Path,0,(size_t)sizeof(p_Path));

	string p_FileMFuncName = "backupRestore";
	int p_Len = 0;

	char importcmd[] = IMPORTCMD;
	string importCmd = string(importcmd);

	ACS_APGCC_CommonLib cmLib;
	p_Len = (int)sizeof(p_Path)-1;
	ACS_PRC_API objPRC;
	//To prevent errors due to lde-brf changes on the passive node the command is rejected immediately

	if (objPRC.askForNodeState()==2){
		setOpCode(PATHDOESNOTSEXIST,"import fails no such file or directory");
		m_burLog->Write("acs_bur_burrestore::import_backup no such file or directory on PASSIVE node",LOG_LEVEL_ERROR);
		return(RTN_FAIL);
	}

	// get path file name from system file
	ret = cmLib.GetFileMPath(p_FileMFuncName.c_str(),p_Path,p_Len);

	if (ret != ACS_APGCC_DNFPATH_SUCCESS){
		sprintf(msg,"%s , error code %d","import_backup:: Error calling GetFileMPath()",ret);
		setOpCode(EXPFAIL,msg);
		m_burLog->Write(msg,LOG_LEVEL_ERROR);
		return(RTN_FAIL);
	}

	acs_bur_util::clearCRLF(p_Path);

	defaultBackupRestorePath = string(p_Path);
    FILE *testFileToImport;

	// Verify if path exist (ACTIVE NODE only see)
    //
	TRACE(m_restore_trace,"%s %s","import_backup() test path name if is ACTIVE NODE: ",defaultBackupRestorePath.c_str());
    testFileToImport = fopen(defaultBackupRestorePath.c_str(), "r");
    // test if path exists on system file (only for ACTIVE NODE)
    if (NULL!=testFileToImport)
    {
        fclose(testFileToImport);
        // PATH Exists Continue .....
    }
    else
    {
		setOpCode(PATHDOESNOTSEXIST,"import fails");
		m_burLog->Write("acs_bur_burrestore::import_backup test path error probably PASSIVE node",LOG_LEVEL_ERROR);
		// KO PATH does't exist
		return RTN_FAIL;
    }


//string fileNametoExport = string(defaultBackupRestorePath + "/" + m_label + string(DEF_EXTENTION));
    string fileNametoExport = string(defaultBackupRestorePath + "/" + m_label );

    TRACE(m_restore_trace,"%s %s","import_backup() test file exist name : ",fileNametoExport.c_str());

    testFileToImport = fopen(fileNametoExport.c_str(), "r");
    // test if file exist on system file
    if (NULL!=testFileToImport)
    {
        fclose(testFileToImport);
        // OK FILE Exist
    }
    else
    {
		setOpCode(DONTEXIST,backupFileName);
		m_burLog->Write("acs_bur_burrestore::import_backup test file error before launch export",LOG_LEVEL_ERROR);
		// KO File does't exist
		return RTN_FAIL;
    }

    // define command string
    // tar -vf to validate a tar file
    // if not valid response is  "tar: This does not look like a tar archive"
    //
    char cmdString[256];
    sprintf(cmdString,"%s %s %s",TARFILEVALIDATE,fileNametoExport.c_str(),MSG_NOSTDERR);
    TRACE(m_restore_trace,"%s %s","import_backup() check valid tar file : ",cmdString);

	string outBuffer = "";
    int result = acs_bur_util::do_command(cmdString,&outBuffer);
    TRACE(m_restore_trace,"%s %s","import_backup() check valid tar file output response : ",outBuffer.c_str());

	if (string::npos != outBuffer.find(TARFILENOTVALID_STRING)){
		setOpCode(UNKNOWARCTYPE,TARFILENOTVALID_STRING);
		m_burLog->Write("acs_bur_burrestore::import_backup error file archive not is a tar file ",LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}

    string backupTarName;

    // get backup name from backup image tar file
    if( RTN_FAIL == acs_bur_burrestore::getBackupNameFromTarFile(fileNametoExport,&backupTarName) )
    	return RTN_FAIL;

    *backupName = backupTarName;
    TRACE(m_restore_trace,"%s %s","import_backup() backupTarName : ",backupTarName.c_str());

    // Check if backup name exists
    int errorCode;
    string errorMessage;
    string dnBrmBackup;

    //
    // Checks if backup name exists
    //
    if( RTN_OK == acs_bur_util::getBBInstance(backupTarName,&dnBrmBackup,&errorCode,&errorMessage) )
    {
    	if(true == m_only_import)
    	{
    		setOpCode(ALREADYEXIST,"file already exists");
    		m_burLog->Write("acs_bur_burrestore::import_backup error file archive already exists ",LOG_LEVEL_ERROR);
    		return RTN_FAIL;
    	}

    	// Exit backup already exist but don't make import
        if (false == m_only_import)
        	cout << endl << MSG_RSTSTARTR << endl;

    	cout << MSG_IMPORTSTART << endl;
    	cout << MSG_IMPORTCOMPLETED << endl << endl;

    	return RTN_OK;
    }
    //
    // getBBInstance method has returned RTN_FAIL it means that backup is not present, we can go on
    //
    setOpCode(NOERROR,"");

    if (false == m_only_import)
    	cout << endl << MSG_RSTSTARTR << endl;

    importCmd = importCmd + defaultBackupRestorePath + "/" + m_label +MSG_NOSTDERR ;
    //importCmd = importCmd + defaultBackupRestorePath + "/" + m_label + string(DEF_EXTENTION)+MSG_NOSTDERR ;

	TRACE(m_restore_trace,"%s %s","acs_bur_burrestore::import_backup() launch: ",importCmd.c_str());

	cout << MSG_IMPORTSTART << endl;

	result = acs_bur_util::do_command(importCmd.c_str(),&outBuffer);

	TRACE(m_restore_trace,"%s %s","acs_bur_burrestore::import_backup() lde-brf output: ",outBuffer.c_str());

	if (RTN_OK != result){
		if(SETUIDERR==result)
			setOpCode(result,"setuid() fail");
		else
			setOpCode(result,"popen() fail");
		m_burLog->Write("acs_bur_burrestore::import_backup error do_command() IMPORTCMD ",LOG_LEVEL_ERROR);
		return(RTN_FAIL);
	}

	// 99 errorcode
	if (string::npos != outBuffer.find("already exists")){
		setOpCode(ALREADYEXIST,"file already exists");
		m_burLog->Write("acs_bur_burrestore::import_backup error file archive already exists ",LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}
	//failed, backup corrupt
	//before last version "is not a BRF backup"
	if (string::npos != outBuffer.find("failed, backup corrupt")){
		setOpCode(NOTBRFBKP,"failed, backup corrupt");
		m_burLog->Write("acs_bur_burrestore::import_backup failed, backup corrupt ",LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}

	// 2 erorcode
	if (string::npos != outBuffer.find("Unknown archive type")){
			setOpCode(UNKNOWARCTYPE,"Unknown archive type");
			m_burLog->Write("acs_bur_burrestore::import_backup error Unknow archive type ",LOG_LEVEL_ERROR);
			return RTN_FAIL;
	}

	// 2 code
	if (string::npos != outBuffer.find("not found")){
			setOpCode(DONTEXIST,backupFileName);
			m_burLog->Write("acs_bur_burrestore::import_backup error not found",LOG_LEVEL_ERROR);
			return RTN_FAIL;
	}
	// 2 code
	if (string::npos != outBuffer.find("in progress")){
			setOpCode(OPEINPROG,"operation in progress");
			m_burLog->Write("acs_bur_burrestore::import_backup operation in progress",LOG_LEVEL_ERROR);
			return RTN_FAIL;
	}
	// 1 code
	//Defect fixing 045
//	if (string::npos != outBuffer.find("No space left")){
//			setOpCode(NOSPACELEFT,"No space left on Device");
//			m_burLog->Write("acs_bur_burrestore::import_backup error No space left on Device ",LOG_LEVEL_ERROR);
//			return RTN_FAIL;
//	}
	// 1 code
	if (string::npos != outBuffer.find("failed")){
				setOpCode(NOSPACELEFT,"No space left on Device");
				m_burLog->Write("acs_bur_burrestore::import_backup error No space left on Device ",LOG_LEVEL_ERROR);
				return RTN_FAIL;
		}
	int watchDogCounter = 40*SLEEPDELAY; // more of 1 minute
	string message;

	// Waiting until backup will be imported in IMM or quit for fail

	while(FOREVER){
		// wait
		sleep(SLEEPDELAY);
		TRACE(m_restore_trace,"%s %d","import_backup() watchdog counter : ",watchDogCounter);

		// find instance of BrmBackup from BrmBackupManager and Brm parent instance
		if(RTN_OK == acs_bur_util::getBBInstance(backupTarName,&m_dnBrmBackup,&errorCode,&message))
		{
			//backup was been imported exit from while loop
			m_burLog->Write("acs_bur_burrestore::import_backup() Found backup in IMM ",LOG_LEVEL_DEBUG);
			TRACE(m_restore_trace,"%s ","import_backup() Found backup in IMM ");
			break;
		}

		// INVALIDIMAGE means that DN is not created in case of other type of error exit with specific error code
		/*if(INVALIDIMAGE!=errorCode){
			m_burLog->Write("acs_bur_burrestore::execute error getBBInstance()",LOG_LEVEL_ERROR);
			// set message error
			setOpCode(errorCode,message);
			return(RTN_FAIL);
		}*/

		// decrement watch dog counter
		watchDogCounter--;

		if(0==watchDogCounter){
			// timeout expired and backup isn't been imported in IMM
			m_burLog->Write("acs_bur_burrestore::execute error getBBInstance()",LOG_LEVEL_ERROR);
			// set message error
			setOpCode(errorCode,message);
			return(RTN_FAIL);
		}
	}

	// Insert printout on import successful

	cout << MSG_IMPORTCOMPLETED << endl << endl;

	// end printout
	return RTN_OK;

}

int  acs_bur_burrestore::print_archive_info(string backupFileName, string *backupCreationTime)
{
	int errorCode;
	string message = "";
	string dnBrmBackup = "";
	if (acs_bur_util::getBrmBackupDN(&dnBrmBackup, backupFileName, &errorCode, &message) == RTN_FAIL)
	{
		m_burLog->Write("print_archive_info: failed to retrieve DN of BrmBackup",LOG_LEVEL_DEBUG);
		setOpCode(errorCode, message);
		return RTN_FAIL;
	}
	
	TRACE(m_restore_trace,"%s %s","print_archive_info() DN for BrmBackup is : ",dnBrmBackup.c_str());

	acs_bur_BrmBackup *backup = new acs_bur_BrmBackup((char *)(dnBrmBackup).c_str());

	if (backup->getOpCode() != NOERROR)
	{
		setOpCode(backup->getOpCode(),backup->getOpMessage());
		m_burLog->Write("acs_bur_burrestore::print_archive_info error acs_bur_BrmBackup()",LOG_LEVEL_ERROR);
		delete backup;
		return RTN_FAIL;
	}

	*backupCreationTime = backup->getCreationTime();
	delete backup;



	cout << MSG_IMPBKPINF << endl << endl;
	cout << "Imagename: " << backupFileName.c_str() << endl;
	cout << "Creation date: " << (*backupCreationTime).c_str() << endl;
	cout << endl;
	cout << MSG_BKPEXPOK << endl;
	return RTN_OK;
}

int acs_bur_burrestore::execute()
{
	TRACE(m_restore_trace, "%s", "acs_bur_burrestore::execute() ");
	bool loopForever = true;
	int errorCode = NOERROR;
	string message = "";
	try {

		do {

			if(RTN_FAIL == parse(argc_ ,argv_, &m_archive_filename,&m_backupPasswd,&m_only_import,&m_isSecured))
				break;

			// import backup file calling
			if(RTN_FAIL == import_backup(m_archive_filename,&m_backupName))
				break;

			// find instance of BrmBackup from BrmBackupManager and Brm parent instance
			if(RTN_FAIL == acs_bur_util::getBBInstance(m_backupName,&m_dnBrmBackup,&errorCode,&message))
			{
				m_burLog->Write("acs_bur_burrestore::execute error getBBInstance()",LOG_LEVEL_ERROR);
				setOpCode(errorCode,message);
				break;
			}

			if(RTN_FAIL == print_archive_info(m_backupName,&m_backupCreationTime))
				break;

			if(false == m_only_import)
			{

				// invoke createBackup action with retry mechanism TR HQ71675
				m_burLog->Write("acs_bur_burrestore:: Invoking Restorebackup Action",LOG_LEVEL_ERROR);
				int retry = 0;
				int MAXRETRY = 3; 
				while(retry<MAXRETRY)
				{
					if(RTN_OK == acs_bur_util::invokeAction(RESTOREBACKUP,m_dnBrmBackup,m_backupName,&errorCode,&message,m_isSecured,m_backupPasswd,"",true))
					{
						m_burLog->Write("acs_bur_burrestore::restore invokeAction() success ",LOG_LEVEL_DEBUG);
						break;
					}
					else
					{
						m_burLog->Write("acs_bur_burrestore::execute error on restore invokeAction() retrying",LOG_LEVEL_ERROR);
						retry++;
						sleep(1);
					}
				}

				if (retry>=MAXRETRY)
				{
					m_burLog->Write("acs_bur_burrestore::execute error invokeAction()",LOG_LEVEL_ERROR);
					setOpCode(errorCode,message);
					break;
				} 

				/*if(RTN_FAIL == acs_bur_util::invokeAction(RESTOREBACKUP,m_dnBrmBackup,m_backupName,&errorCode,&message))
				{
					m_burLog->Write("acs_bur_burrestore::execute error invokeAction()",LOG_LEVEL_ERROR);
					setOpCode(errorCode,message);
					break;
				}*/

				// waiting until create operation was finished or cancelled and state is SUCCESS or FAILED
				if(RTN_FAIL == acs_bur_util::waitForTeminate(RESTOREBACKUP,m_dnBrmBackup,&m_backupCreationTime,m_backupName,&errorCode,&message))
				{
					m_burLog->Write("acs_bur_burrestore::execute error waitForTeminate()",LOG_LEVEL_ERROR);
					setOpCode(errorCode,message);
					break;
				}

			}

			loopForever = false;

		}while(true == loopForever);
	}// end try
	catch (exception& e)
	{
		m_burLog->Write("acs_bur_burrestore::execute an exception was raised",LOG_LEVEL_ERROR);
		setOpCode(GENERALFAULT,e.what());
	}

	return error_message_handler();
}
