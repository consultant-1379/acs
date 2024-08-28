/*
 * Main.cpp
 *
 *  Created on: Aug 27, 2010
 *      Author: xludesi
 */

#include "ace/Signal.h"
#include "ace/Sig_Handler.h"
#include "acs_prc_api.h"
#include "MktrSignalHandler.h"
#include "ACS_CS_API.h"
#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_paramhandling.h"
#include "MktrInputParams.h"
#include "MktrFile.h"
#include "Mktr.h"
#include "ace/Log_Msg.h"
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <sys/quota.h>
#include <mntent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <sys/wait.h>
#include <string.h>

#define QUOTABLOCK_BITS 10
#define QUOTABLOCK_SIZE (1 << QUOTABLOCK_BITS)
#define ACS_MKTR_FILENAME "/var/log/acs/mktr/bfilename"
#define APOS_USERMGMT_CMD "/opt/ap/apos/bin/usermgmt/usermgmt"

/* Conversion routines from and to quota blocks */
#define toqb(x) (((x) + QUOTABLOCK_SIZE - 1) >> QUOTABLOCK_BITS)

static int siglist[] = {SIGHUP, SIGQUIT, SIGINT, SIGTERM, SIGALRM, SIGABRT, SIGKILL, SIGTSTP, SIGCONT};     // list of the signals that we must catch and handle.
static const char *outputdir = "/var/log/acs/mktr";		 		  		// 'local output directory'; the directory  on system disk where MKTR application saves 'single-node' archives.
char p_finaloutputdir[100] = {0};										// 'final output directory': the directory on data disk where MKTR application (via the process instance running on ACTIVE node ) saves the final output 'double-node' archive ('mktr-<AP Name>-<Timstamp>.tar.gz').
const char * p_mktrFileMPath = "/mktr";
const char * p_FileMFuncName = "supportData";
bool emergency_mode = false;											// true if data disk is not available or node status is not available. In this case, only one single-node archive will be produced
const char* mktruserStr = "mktruser"; //used to collect COM CLI and MML commands
const char *group_comemergency = "com-emergency";
const char *group_tsgroup = "tsgroup";
bool mktruserCreated = false;
bool mktruserUsageDone = false;		// to avoid creating user while handling SIGCONT signal
int nodeState = 0;
enum Mktr_commamd_exit_codes {
	MKTR_EXIT_CODE_OK = 0,									/* SUCCESS */
	MKTR_EXIT_CODE_INTERNAL_ERROR = 1,						/* INTERNAL ERROR */
	MKTR_EXIT_CODE_WRONG_USAGE = 2,							/* WRONG COMMAND USAGE */
	MKTR_EXIT_CODE_INVALID_PARAM_VALUE = 3, 				/* INVALID PARAMETER PASSED IN COMMAND LINE */
	MKTR_EXIT_CODE_PARAM_DUPLICATED	= 4,					/* PARAMETER DUPLICATED IN COMMAND LINE */
	MKTR_EXIT_CODE_GET_SHELF_ARCH_ERROR = 5, 				/* ERROR GETTING SHELF ARCHITECTURE (SCB, SCX, ...) */
	MKTR_EXIT_CODE_NOT_APPLICABLE = 6,						/* COMMAND NOT APPLICABLE */
	MKTR_EXIT_CODE_CREATE_DIR_ERROR = 7,					/* CREATE DIRECTORY ERROR */
	MKTR_EXIT_CODE_CLEAN_DIR_ERROR = 8,						/* ERROR TRYING TO CLEAN A DIRECTORY */
	MKTR_EXIT_CODE_SINGLE_NODE_ARCH_CREATE_ERROR = 9,		/* FAILED CREATION OF SINGLE NODE ARCHIVE */
	MKTR_EXIT_CODE_COPY_TO_PARTNER_NODE_ERROR = 10, 		/* ERROR TRYING TO COPY SINGLE-NODE ARCHIVE TO PARTNER NODE */
	MKTR_EXIT_CODE_ANOTHER_MKTR_INSTANCE_IS_RUNNIG = 11 	/* ANOTHER INSTANCE OF THE COMMAND MKTR IS RUNNING ON THE APG */
	//MKTR_EXIT_CODE_MKTR_NOT_SUPPORTED_IN_CURRENT_SHELF_ARCH = 12 	/* NOT SUPPORTED IN THIS CURRENT SHELF ARCHITECTURE */
};

// forward declarations
void print_usage(const std::string& msg);
int build_mktrLocalOutFileName(std::string &outfilename);
int build_mktrLocalOutFileNamePeer(std::string &outfilename);
int build_mktrFinalOutFileName(std::string &outfilename);
bool cleanMktrOutputDir(const std::string & rootFolderPathName, std::string &outMess, bool leaveOnlyLocalOutArchive);
bool cleanMktrDir(const std::string & dirPath, const std::list<std::string> & exclusionList );
bool check_num_of_mktr_files(const char * dir, uint max_num_files = 2);
int getNodeState();
const char * apg_node_name();
int prepare_input_files_for_cliss_commands(const MktrConfig & mktrConfig);
int prepare_input_file_for_cliss_command(const char * cliss_command_template, const char * cliss_command_input_file_path);
int is_mktr_running();
void remove_lock_files ();
bool checkHardLimit(const std::string &outputFilePath);
struct mntent *mountpoint(const char *filename, struct mntent *mnt, char *buf, size_t buflen);
int deleteMktruser();
int checkMktruser();
int createMktruser();
int applyGroupsToMktruser();
void mktruserMgmtAtStartup();
void mktruserMgmtCleanup();

MktrCommonDefs::ApgShelfArchitectureType apgShelfArchitecture = MktrCommonDefs::APG_SHELF_ARCHITECTURE_NONE;

int main(int argc, char* argv[])
{
//	switch ( is_mktr_running() ){
//
//		case 1 :
//			std::cout << "Another instance of this command is running on the node" << std::endl << std::endl;
//			exit(MKTR_EXIT_CODE_ANOTHER_MKTR_INSTANCE_IS_RUNNIG);
//
//		case 2 :
//
//		case 3 :
//
//		case 4 :
//			std::cout << "Unable to get the host name of the other node" << std::endl;
//			exit(MKTR_EXIT_CODE_ANOTHER_MKTR_INSTANCE_IS_RUNNIG);
//
//	}

	// option 'x' is legal only for SCX Shelf architecture
	int call_result = APGInfo::get_shelf_architecture_type(apgShelfArchitecture);
	if(call_result != 0)
	{
		std::cout << "Unable to get Shelf Architecture" << std::endl << std::endl;
		// Fix for TR HR38509 and HR63241 : avoid exiting when APG Shelf architecture type  is not available
		//exit(MKTR_EXIT_CODE_GET_SHELF_ARCH_ERROR);
	}

	// Parse command line to get input parameters
	MktrInputParams mktrParams;
	int result = mktrParams.parseCommandLine(argc, argv, apgShelfArchitecture);
	if(result != MktrInputParams::MKTRPCL_SUCCESS)
	{
		print_usage(mktrParams.lastParseErrDesc());
		if(result == MktrInputParams::MKTRPCL_INVALID_ARGVALUE)
			exit(MKTR_EXIT_CODE_INVALID_PARAM_VALUE);
		else if(result == MktrInputParams::MKTRPCL_PARAM_DUPLICATED)
			exit(MKTR_EXIT_CODE_PARAM_DUPLICATED);
		else
			exit(MKTR_EXIT_CODE_WRONG_USAGE);
	}

	//CNI_33_6_552 Changes START
	if(!mktrParams.hasOption_o())
	{
		if(is_mktr_running())
		{
			std::cout << "Another instance of this command is running on the node" << std::endl << std::endl;
					exit(MKTR_EXIT_CODE_ANOTHER_MKTR_INSTANCE_IS_RUNNIG);
		}
	}
	//CNI_33_6_552 Changes END

	// install signal handler for "termination signals"
	ACE_Sig_Handler ace_sigdispatcher;
	MktrSignalHandler mktr_sighandler;
	int numsigs = sizeof(siglist) / sizeof(int);
	for(int i=0;i<numsigs; ++i)
		ace_sigdispatcher.register_handler(siglist[i], & mktr_sighandler);
	

	if ((apgShelfArchitecture == MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED) && ((mktrParams.hasOption_d())||(mktrParams.hasOption_x())))
	{
		print_usage(mktrParams.lastParseErrDesc());
		exit(MKTR_EXIT_CODE_WRONG_USAGE);
	}

	if(mktrParams.hasOption_x() || mktrParams.hasOption_d())
	{
		if((apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_SMX) && (mktrParams.hasOption_d()))
		{
			std::cout << "Illegal option in this system configuration" << std::endl << std::endl;
			exit(MKTR_EXIT_CODE_NOT_APPLICABLE);
		}
		if((apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_NONE) && (apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_SCX) && (apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_DMX) && (apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_SMX))
		{			
			//get node architecture. nodeArchitecture=0 -> NOT EVO , nodeArchitecture=1 -> EVO. now exiting since not evo
			std::cout << "Illegal command in this system configuration" << std::endl << std::endl;
			exit(MKTR_EXIT_CODE_NOT_APPLICABLE);
		}


		if(mktrParams.hasOption_d())
		{
			if(!mktrParams.hasOption_x())
			{
				print_usage(mktrParams.lastParseErrDesc());
				exit(MKTR_EXIT_CODE_WRONG_USAGE);
			}
		}
		/*
			 if we are here, one of the following conditions is true:
			 1) Shelf architecure is SCX, or
			 2) Shelf architecture is not known
		 */
	}
	// invoke ACS_CS_API to know if we're executing on a SingleCP System or on a MultipleCP System
	bool isMultCP = false;
	ACS_CS_API_NS::CS_API_Result cs_api_result = ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultCP);

	// get from FileM component the final output directory, i.e the directory where MKTR (via the process instance running on ACTIVE node)
	// will save the final output archive
	ACS_APGCC_CommonLib apgccCommon;
	int len = sizeof(p_finaloutputdir);

//	retcode = apgccCommon.GetFileMPath(p_FileMFuncName, p_finaloutputdir, len);
//
//	if( retcode != ACS_APGCC_DNFPATH_SUCCESS)
//	{
//		std::cout << "Unable to get final output directory path " << std::endl;
//		exit(MKTR_EXIT_CODE_INTERNAL_ERROR);
//	}
//	else
//	{
//		strcat(p_finaloutputdir, p_mktrFileMPath);
//	}

	int apgcc_retry = 5; // try 5 time before assume we are in emergency mode
	int apgcc_retry_interval = 1; // wait 1 second before the next retry

	while ( ( apgcc_retry > 0 ) && ( apgccCommon.GetFileMPath(p_FileMFuncName, p_finaloutputdir, len) != ACS_APGCC_DNFPATH_SUCCESS ) ){
		sleep ( apgcc_retry_interval );
		apgcc_retry--;
	}

	if ( apgcc_retry <= 0 ){
		emergency_mode = true;
		std::cout << "Unable to get final output directory path " << std::endl;
	}
	else {
		strcat(p_finaloutputdir, p_mktrFileMPath);
	}
	// check existence of final output directory. If it doesn't exist, try to create it
	if (strlen(p_finaloutputdir) > 0 ){
		MktrFile mktrFinal(p_finaloutputdir);
		if(!mktrFinal.exists())
		{
			if(!mktrFinal.createDirEx(775))// TR fix IA72764
			{
				//std::cout<<"MKTR final output directory does not exist and failed to create ("<<mktrFinal.getLastErrorMsg()<<")"<<std::endl;
				std::cout<<"Final output directory does not exist and failed to create"<<std::endl;
				emergency_mode = true;
			}
		}
		else
		{
			if(!mktrFinal.isDir())
			{
				std::cout<<"Final output directory does not exist and failed to create (Already exists a non directory entry)"<<std::endl;
				exit(MKTR_EXIT_CODE_CREATE_DIR_ERROR);
			}
		}
	}

	// check existence of  'local output directory'. If it doesn't exist, try to create it
	MktrFile mktrLocal(outputdir);
	if(!mktrLocal.exists())
	{
		if(!mktrLocal.createDirEx())
		{
			//std::cout<<"Local directory does not exist and failed to create ("<<mktrLocal.getLastErrorMsg()<<")"<<std::endl;
			std::cout<<"Local directory does not exist and failed to create"<<std::endl;
			exit(MKTR_EXIT_CODE_CREATE_DIR_ERROR);
		}
	}
	else
	{
		if(!mktrLocal.isDir())
		{
			std::cout<<"Local directory does not exist and failed to create (Already exists a non directory entry)"<<std::endl;
			exit(MKTR_EXIT_CODE_CREATE_DIR_ERROR);
		}
	}

	// build single-node archive file name for this node (without tar.gz extension)
	std::string mktrLocalArchiveName;
	if(build_mktrLocalOutFileName(mktrLocalArchiveName) < 0)
	{
		std::cout << "Unable to get host name" << std::endl;
		exit(MKTR_EXIT_CODE_INTERNAL_ERROR);
	}

	// build single-node archive file name for partner node (without tar.gz extension)
	std::string mktrPartnerNodeArchiveName;
	if(!emergency_mode && (build_mktrLocalOutFileNamePeer(mktrPartnerNodeArchiveName) < 0))
	{
		emergency_mode = true;
	}
	// create the object that will do the work
	Mktr *mktrExecutor = Mktr::createMktrCmdExecutor(mktrLocal.name(), mktrLocalArchiveName);

	//check if the object is available for work
	if(!mktrExecutor || !mktrExecutor->isAvailableForWork())
	{
		if(!mktrExecutor)
			std::cout << "Insufficient memory to execute the command" << std::endl;
		else
			std::cout << "Internal error ( " << mktrExecutor->get_unavailable_reason() << " )" << std::endl;

		Mktr::releaseMktrCmdExecutor();
		exit(MKTR_EXIT_CODE_INTERNAL_ERROR);
	}

	// check and set verbose (option -v)
	mktrExecutor->setVerbose(mktrParams.hasOption_v());
	// Init capability (NOTE: if set capability fail, mktr continue witout hard limit) 
	mktrExecutor->initCapability();
	// get Node state determined when command was launched
	nodeState = mktrExecutor->NodeState();
	if(nodeState == MktrCommonDefs::NODE_NONE)
	{
		// ATTENTION: we don't know if we are running on ACTIVE or PASSIVE node, so enter 'emergency mode' producing only the single-node archive
		// relative to this node
		emergency_mode = true;
	}
	// remove elements from local output directory, excluding some files
	std::list<std::string> exclusionlist;
	exclusionlist.push_back(mktrExecutor->getMktrLogFilePath());									// MKTR log file
	exclusionlist.push_back(std::string(outputdir) + "/" +  mktrLocalArchiveName + ".tar");			// MKTR local tar file
	if( (nodeState == MktrCommonDefs::NODE_ACTIVE) && mktrParams.hasOption_o() )
	{
		std::string str = std::string(outputdir) + "/" +  mktrPartnerNodeArchiveName + ".tar.gz";	// MKTR single-node archive of the partner node
		exclusionlist.push_back(str);
	}

	if(!cleanMktrDir(mktrLocal.name(), exclusionlist))
	{
		std::cout << "Cleanup of local directory failed" << std::endl;
		exit(MKTR_EXIT_CODE_CLEAN_DIR_ERROR);
	}

	// clean final output directory (leaving only 'mktr_*' files), but only if we are not in "emergency mode"
	std::string outMess;
	if(!emergency_mode && !cleanMktrOutputDir(p_finaloutputdir, outMess, false))
	{
		std::cout << "Cleanup of final output directory failed" << std::endl;
		exit(MKTR_EXIT_CODE_CLEAN_DIR_ERROR);
	}

	mktruserMgmtAtStartup();	// TR HY81976

	// get Mktr Command configuration (we could get it from file, but at the moment we use default settings)
	MktrConfig conf;
	conf.buildWithDefaults();

	// Prepare input files for the execution of cliss commands
	prepare_input_files_for_cliss_commands(conf);
	// execute default commands that are independent from  'SingleCP' or 'MultipleCP' system configuration
	mktrExecutor->fetchCmdLogs(conf.commonCmdTable());

	// and now execute default commands that are dependent from system configuration;
	// however, if we don't know what the system configuration is ('singleCp' or 'MultiCp'), we execute both set of commands
	if( (cs_api_result != ACS_CS_API_NS::Result_Success) || isMultCP)
		mktrExecutor->fetchCmdLogs(conf.multiCpCmdTable());

	if( (cs_api_result != ACS_CS_API_NS::Result_Success) || !isMultCP)
		mktrExecutor->fetchCmdLogs(conf.singleCpCmdTable());

	mktruserMgmtCleanup(); // TR HY81976
	mktruserUsageDone = true; // we will no longer use MKTR user, so this flag will skip creation of the user from SIGCONT handling

	//isMultCP ? mktrExecutor->fetchCmdLogs(conf.multiCpCmdTable()): mktrExecutor->fetchCmdLogs(conf.singleCpCmdTable());

	// check if we have to execute "phaprint" command (option '-p')
	//if(mktrParams.hasOption_p())
		//mktrExecutor->fetchCmdLogs(conf.phaCmdTable());

	// fetch default files
	mktrExecutor->fetchFileLogs(conf.commonFileTable());

	// check if we have to include memory dump (option '-d')
	//if(mktrParams.hasOption_d())
		//mktrExecutor->fetchFileLogs(conf.memFileTable());

	// check if we must include "alogfind" output (option '-a')
	if(mktrParams.hasOption_a())
        {
		mktrExecutor->fetchCmdLogs(conf.alogFileTable());
		mktrExecutor->fetchFileLogs(conf.plogFileTable());
        }
	// check if we must include  APOS logs (option '-l')
	if(mktrParams.hasOption_l())
	{
		mktrExecutor->fetchFileLogs(conf.aposFileTable());
		//mktrExecutor->fetchCmdLogs(conf.aposCmdTable());
		//mktrExecutor->fetchCltLogs(conf.lbbCltTable());
	}

	// check if we must include CQRH and MAS logs (option '-b')
	//if(mktrParams.hasOption_b())
	//	mktrExecutor->fetchFileLogs(conf.masFileTable());

	// check if we must include  SCX and CMX logs (option '-x')
	if(mktrParams.hasOption_x())
	{
		if(apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED)
		{
			if(apgShelfArchitecture == MktrCommonDefs::APG_SHELF_ARCHITECTURE_DMX)
			{
				mktrExecutor->fetchCmdLogs(conf.scxCmdTable());
			}
			else //When the architecture is SCX or UNKNOWN then include all SCX and CMX logs
			{
				if(apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_SMX)
				{
					mktrExecutor->fetchFileLogs(conf.scxFileTable());
					mktrExecutor->fetchFileLogs(conf.cmxFileTable());
					mktrExecutor->fetchCmdLogs(conf.scxCmdTable());
				}
				else
				{
					mktrExecutor->fetchCmdLogs(conf.scxCmdTable());
					if(mktrParams.hasOption_d())
						mktrExecutor->fetchCmdLogs(conf.smxCmdTable());
					mktrExecutor->fetchFileLogs(conf.smxFileTable());
				}
			}
		}else{
			mktruserMgmtCleanup(); // TR HY81976
			exit(MKTR_EXIT_CODE_WRONG_USAGE);
		}
	}

	// and finally fetch Logs depending from the TIME param (current time or the value supplied on command line)
	MktrDate dp;
	mktrParams.getDateParam(dp);
	mktrExecutor->fetchCoreDumpAtTime(dp, mktrParams.hasOption_t());

	//fetch the core dump from /var/log/core/
	mktrExecutor->fetchMemoryDumpAtTime(dp, mktrParams.hasOption_t());

	// remove our actions from ACE Signal Dispatcher, and restore default actions
//	for(int j=0;j<numsigs;++j)
//		ace_sigdispatcher.remove_handler(siglist[j]);

	// add MKTR log file to the output archive, and delete it
	mktrExecutor->addLogFileToArchiveAndDeleteIt();

	// now create the MKTR output 'single-node' archive (with extension .tar.gz)
	bool single_node_archive_created = !(mktrExecutor->createOutZippedArchive());

	// if we are running on PASSIVE node and we are not in "emergency mode",
	// then we must copy the single-node archive just created to the local directory of the partner node
	if( single_node_archive_created && (nodeState == MktrCommonDefs::NODE_PASSIVE)  && !emergency_mode)
	{
		std::string partnerHostname;
		if(Mktr::getPartnerHostName(partnerHostname) /*error*/)
		{
			std::cout << "Unable to get the host name of the other node" << std::endl;
			exit(MKTR_EXIT_CODE_COPY_TO_PARTNER_NODE_ERROR);
		}

		// build and execute the "scp" command to move the single node archive from PASSIVE node to ACTIVE node
		MktrCmd cmdExecutor;
		std::string single_node_archive_full_path = std::string(outputdir) + "/" + mktrLocalArchiveName + ".tar.gz";
		std::string scpCommand = std::string("scp ") + single_node_archive_full_path + " " + partnerHostname + ":" + single_node_archive_full_path + " &> /dev/null";
		if(!cmdExecutor.execute(scpCommand))
		{
			std::cout << "Unable to copy 'single node archive' from PASSIVE node to ACTIVE node" <<   std::endl;
			exit(MKTR_EXIT_CODE_COPY_TO_PARTNER_NODE_ERROR);
		}
	}

	// check if we have to launch MKTR on the other node
	int runOnOtherNodeResult = -1;
	if( !emergency_mode && single_node_archive_created &&  !mktrExecutor->isStopped() && !mktrParams.hasOption_o())
	{
		// run MKTR on other NODE (using same options)
		runOnOtherNodeResult = mktrExecutor->runOnOtherNode(mktrParams.getCommandLine());
		if(runOnOtherNodeResult != 0)
		{
			std::cout << "Error in execution on other NODE" <<   std::endl;
		}
	}

	// now, if we're running on ACTIVE node and we are not in emergency mode, we should have, in local output directory,
	// the single-node archives *.tar.gz to be used in order to produce the final output archive.
	std::string finalOutputFileName;
	bool final_output_archive_created = false;
	if (!emergency_mode && single_node_archive_created && (nodeState == MktrCommonDefs::NODE_ACTIVE))		// if we previously failed to produce single-node archive for this node, we stop !
	{
		char cmd_string[4096];
		std::string localOutputFileName = mktrLocalArchiveName + ".tar.gz";
		std::string localOutputFileNamePeer = mktrPartnerNodeArchiveName + ".tar.gz";
		std::string partnerHostname;

		build_mktrFinalOutFileName(finalOutputFileName);
		finalOutputFileName += ".tar.gz";

		// TR HS26505 ---BEGIN
		if(mktrParams.hasOption_o())
	    {
		   const char* temps = finalOutputFileName.c_str();
		   int length = strlen(temps) + 1;

		   std::ofstream ffilen;
		   ffilen.open(ACS_MKTR_FILENAME, std::ios::binary|std::ios::out );
		   if (ffilen.is_open())
		   {
		       char *ptrfileData =  new char[length];
		       if(ptrfileData != 0)
		       {
		          memset(ptrfileData, 0, sizeof( char)*length);
		          sprintf(ptrfileData, temps);
		          std::ifstream::pos_type size  = length;
		          ffilen.write(ptrfileData , size);
		          delete[] ptrfileData;
		          ptrfileData = 0;
		       }
		       ffilen.close();
		   }

		   if(Mktr::getPartnerHostName(partnerHostname) /*error*/)
		   {
		      std::cout << "Unable to get the host name of the other node" << std::endl;
		   }
		   // build and execute the "scp" command to move the build filename from ACTIVE node to PASSIVE  node
		   MktrCmd cmdExecutor1;
		   std::string scpCommand1 = std::string("scp ") + ACS_MKTR_FILENAME + " " + partnerHostname + ":" + outputdir + " &> /dev/null";
		   if(!cmdExecutor1.execute(scpCommand1))
		   {
		      std::cout << "Unable to copy build filename from ACTIVE to PASSIVE  node" <<   std::endl;
		   }
		}
		// TR HS26505 ---END

		// check for existence of *.tar.gz files
		std::string localOutputFilePath = outputdir + std::string("/") + localOutputFileName;
		std::string remotePath =  std::string("/") + "usr" + std::string("/")+ "libexec" + std::string("/") + "rmt";
		std::string localOutputFilePathPeer = outputdir + std::string("/") + localOutputFileNamePeer;
		std::string finalOutputFilePath = p_finaloutputdir + std::string("/") + finalOutputFileName;
		bool b1 = ( ::access(localOutputFilePath.c_str(), F_OK) == 0 );
		bool b2 = ( ::access(localOutputFilePathPeer.c_str(), F_OK) == 0 );
		if(!b1 && !b2)
		{
			std::cout << "Error: no *.tar.gz file found in local output directory '" << outputdir <<  "'"<< std::endl;
		}
		else
		{
			//snprintf(cmd_string, 1024, "cd %s; tar cvzf %s/%s %s %s", clusteroutputdir, p_finaloutputdir, finalOutputFileName.c_str(),localOutputFileName.c_str(), localOutputFileNamePeer.c_str());
			snprintf(cmd_string, 1070, "cd %s; tar czf %s %s %s --format=gnu --rmt-command=%s ", outputdir,  finalOutputFileName.c_str(),
					(b1 ? localOutputFileName.c_str(): ""), (b2 ? localOutputFileNamePeer.c_str(): ""),remotePath.c_str());


			// create MKTR output file (in local output directory)
			MktrCmd cmd;
			if(!cmd.execute(cmd_string))
			{
				std::cout<<std::endl<<"Failed to create Mktr output file \""<< finalOutputFileName<< "\"" << std::endl;
			}
			else
			{
				/*
				 * Check if the final output directory has sufficient space to store the final archive,
				 * that currently is located into the local output directory.
				 * In the other case, if possible, remove some old archive(s).
				 */
				if(checkHardLimit(outputdir + std::string("/") + finalOutputFileName)) {

					// copy mktr output file to final output directory
					snprintf(cmd_string, 1024, "cp -f %s/%s %s 1>/dev/null 2>/dev/null", outputdir, finalOutputFileName.c_str(), finalOutputFilePath.c_str());

					if(!cmd.execute(cmd_string)){
						// Create ostringstream
						ostringstream strMsg;
						// Set message
						strMsg << "Failed to create Mktr final output file \"" << finalOutputFilePath << "\"";
						// Then print failed message
						cout << strMsg.str().c_str() << std::endl;
						// Create remove file command
						snprintf(cmd_string, 1024, "rm %s 1>/dev/null 2>/dev/null", finalOutputFilePath.c_str());
						// Execute "remove file". Note: the error control on execute is not checked because no print out is accepted from costumer.
						if(!cmd.execute(cmd_string)){
							// Then print failed message
							cout << strMsg.str().c_str() << std::endl;
						}
					}else{
						// Success to copy
						final_output_archive_created = true;
					}
					/*
				// copy mktr output file to final output directory
				MktrFile mktrOutputFile(outputdir + std::string("/") + finalOutputFileName);
				MktrFile * pF = mktrOutputFile.copyTo(finalOutputFilePath);
				if(!pF)
				{
					cout << "Failed to create Mktr final output file \"" << finalOutputFilePath << "\"" << "DETAILS: " << mktrOutputFile.getLastErrorMsg() << std::endl;
				}
				else
				{
					final_output_archive_created = true;
				}

				delete pF;
					 */
				}else{
					std::cout<<std::endl<<"Failed to create Mktr output file \""<< finalOutputFileName<< "\"" << std::endl;
				}
			}
		}

		// check final output directory
		check_num_of_mktr_files(p_finaloutputdir, 2);
	}
	// TR HS26505 ---BEGIN
	if(nodeState == MktrCommonDefs::NODE_PASSIVE && !mktrParams.hasOption_o())
	{
	   std::string tempname;
	   std::ifstream ifs(ACS_MKTR_FILENAME, std::ios::binary|std::ios::in);

	   if(std::getline(ifs,tempname)<0)
	   {
	      std::cout <<"error in reading line"<<ACS_MKTR_FILENAME<<std::endl;
	   }
	   if(tempname.size() != 0)
	   {
	     finalOutputFileName = tempname;
	   }
	}
	// TR HS26505 ---END
	// clean local output directory, leaving only (if present) the local single-node output archive
	bool bCleanRes = cleanMktrOutputDir(outputdir, outMess, true);
	if(!bCleanRes)
		std::cout << "WARNING: Error while cleaning Mktr local output directory '" << outputdir << "'. DETAILS: <" << outMess << ">" << std::endl;

	// release mktr object
	Mktr::releaseMktrCmdExecutor();

	// clean final output directory (if we are running on ACTIVE node and we are not in "emergency mode"),
	// leaving only the output files *.tar.gz produced by Mktr
	if(!emergency_mode && single_node_archive_created && (nodeState == MktrCommonDefs::NODE_ACTIVE)
			&& !cleanMktrOutputDir(p_finaloutputdir, outMess, false))
	{
		std::cout << "WARNING: Error while cleaning Mktr final output directory. DETAILS: " << outMess << std::endl;
	//	exit(7);
	}

	// check if we failed to produce single-node output archive
	if(!single_node_archive_created)
	{
		//std::cout << "Unable to produce MKTR single-node output archive: '" << outputdir << "/" << mktrLocalArchiveName << ".tar.gz'" << std::endl;
		std::cout << "Unable to produce MKTR single-node archive" << std::endl;
		mktruserMgmtCleanup(); // TR HY81976
		exit(MKTR_EXIT_CODE_SINGLE_NODE_ARCH_CREATE_ERROR);
	}

	// OK, we at least successfully produced a single-node archive. Give a message to indicate where to find this single-node archive or, hopefully, the final output archive.
	if(!mktrParams.hasOption_o())
	{
		if(emergency_mode  /* emergency mode */ ||
		  ((nodeState != MktrCommonDefs::NODE_ACTIVE) &&  (runOnOtherNodeResult != 0)) /* MKTR launched on PASSIVE node and failed execution on the ACTIVE node */  ||
		  ((nodeState == MktrCommonDefs::NODE_ACTIVE) && !final_output_archive_created) /* MKTR launched on ACTIVE node but final output archive is not available */  )
		{
			std::cout << std::endl << "Done. Find the (single-node archive) file in "<< outputdir << "/" << mktrLocalArchiveName << ".tar.gz" << std::endl;
		}
		else
		{
			acs_apgcc_paramhandling pha;
		//	char tmp_par[50] = {0};
		//	pha.getParameter("AxeNbiFoldersnbiFoldersMId=1", p_FileMFuncName,(char(&)[])tmp_par);
		   char tmp_par[SA_MAX_NAME_LENGTH + 1] = {0};
			 pha.getParameter< SA_MAX_NAME_LENGTH + 1>("AxeNbiFoldersnbiFoldersMId=1", p_FileMFuncName,tmp_par);
			std::cout << std::endl << "Done. Find the file in "<< tmp_par << p_mktrFileMPath << "/" << finalOutputFileName << std::endl;
		}
	}

	//remove_lock_files();
	mktruserMgmtCleanup(); // TR HY81976
	return MKTR_EXIT_CODE_OK;
}


void print_usage(const std::string& msg)
{
	std::cout<<msg<<std::endl;
	int call_result = APGInfo::get_shelf_architecture_type(apgShelfArchitecture);
	if(call_result != 0)
	{
		std::cout << "Unable to get Shelf Architecture" << std::endl << std::endl;
	}

	if(apgShelfArchitecture == MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED){
		std::cout<<"USAGE: mktr [time] [-a] [-l] [-v]"<<std::endl;
	}
	else{
		std::cout<<"USAGE: mktr [time] [-a] [-l] [-x [-d]] [-v]"<<std::endl;
	}
	std::cout<<"time  Time in YYMMDD-HHmm format (<= Current Time)" << std::endl;
	std::cout<<"-a    Include alogfind output and Protected Audit Logs"  << std::endl;
	std::cout<<"-l    Include APOS logs " << std::endl;

	if(apgShelfArchitecture != MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED){
		std::cout<<"-x    Include xcountls output, SCX logs and CMX logs or SMX logs" << std::endl;
		std::cout<<"-d    Include dynamically fetched SMX logs" << std::endl;
	}

	std::cout<<"-v    Print visuals info in verbose form " << std::endl << std::endl;
}

int build_mktrLocalOutFileName(std::string &outfilename)
{

	char hostName[HOST_NAME_MAX + 1]= {0};
	if(ACE_OS::hostname(hostName, HOST_NAME_MAX + 1) < 0)
		return -1;

	// it should be not necessary but ...
	hostName[HOST_NAME_MAX] = 0;

	outfilename = std::string("mktr_") + hostName;

	return 0;
}

int build_mktrLocalOutFileNamePeer(string &outfilename)
{
	ifstream ifs;
	string peerName;

	ifs.open("/etc/cluster/nodes/peer/hostname");

	if (ifs.good())
		getline(ifs, peerName);
	else
		return -1;

	ifs.close();

	outfilename = "mktr_" + peerName;
	return 0;
}


int build_mktrFinalOutFileName(std::string &outfilename)
{
	// get local time
	time_t raw_time;
	ACE_OS::time(&raw_time);

	// convert to broken-time representation
	struct tm bttime;
	if(NULL == ACE_OS::localtime_r(& raw_time, & bttime))
		return -1;

	// build file name using format "mktr_<APG_NodeName>_<Timestamp>"
	char filename[256];
	sprintf (filename,"mktr_%s_%4d-%2.2d-%2.2dT%2.2d-%2.2d-%2.2d",
					   apg_node_name(),
					   (bttime.tm_year)+1900,
					   (bttime.tm_mon)+1,
					   bttime.tm_mday,
					   bttime.tm_hour,
					   bttime.tm_min,
					   bttime.tm_sec);

	outfilename = filename;
	return 0;
}


bool cleanMktrOutputDir(const std::string & rootFolderPathName, std::string &outMess, bool leaveOnlyLocalOutArchive = false)
{
	// proceed to clean the specified directory, excluding local single-node output archive, or "mktr_*" files
	std::string local_out_archive_path;
	std::string mktr_path_prefix;

	if(leaveOnlyLocalOutArchive)
	{
		std::string str_name;
		if(build_mktrLocalOutFileName(str_name) < 0)
		{
			outMess = "Error getting local 'single-node' output archive name";
			return false;
		}

		local_out_archive_path = rootFolderPathName + "/" + str_name  + ".tar.gz";
	}
	else
		mktr_path_prefix = rootFolderPathName + std::string("/mktr_");

	MktrFile obj(rootFolderPathName);
	std::list<MktrFile> rfItems = obj.list();
	for(std::list<MktrFile>::iterator it = rfItems.begin(); it != rfItems.end(); ++it)
	{
		std::string current_filepath = it->name();

		if( (leaveOnlyLocalOutArchive && (current_filepath != local_out_archive_path) ) ||
			(!leaveOnlyLocalOutArchive && (current_filepath.find(mktr_path_prefix) != 0) ))
		{
			if(!(it->remove()))
			{
				outMess = it->getLastErrorMsg();
				return false;
			}
		}
	}

	return true;
}


bool cleanMktrDir(const std::string & dirPath, const std::list<std::string> & exclusionList)
{
	bool retVal = true;

	MktrFile obj(dirPath);
	std::list<MktrFile> rfItems = obj.list();
	for(std::list<MktrFile>::iterator it = rfItems.begin(); it != rfItems.end(); ++it)
	{
		std::string current_filepath = it->name();
		if(std::find(exclusionList.begin(), exclusionList.end(), current_filepath) == exclusionList.end())
		{
			// current file path is not in the exclusion list, so we must remove it !
			if(!(it->remove()))
			{
				retVal = false;
				break;
			}
		}
	}

	return retVal;
}


bool check_num_of_mktr_files(const char * dir,  uint max_num_files)
{
	bool retval = true;

	//std::list<MktrFile> filelist = MktrFile::list(std::string(dir) + "/mktr_*.tar.gz");
	std::list<MktrFile> filelist = MktrFile::list(std::string(dir) + "/mktr_*_[0-9][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9]T[0-2][0-9]-[0-6][0-9]-[0-6][0-9].tar.gz");

	if(filelist.size() <= max_num_files)
		return true;	// no need to clean files

	std::list<MktrFile>::iterator it;
	std::vector<std::string> files_indexes;
	for(it = filelist.begin(); it != filelist.end(); it++)
	{
		std::string filename = it->name();

		// get TIMESTAMP part of current filename: e.g.:  "2012-12-13T15-29-22.tar.gz"
		size_t pos_1  = filename.rfind('_');
		std::string str_timestamp = filename.substr(pos_1 + 1);

		// now get "/mktr_<APName>_"part of current filename : e.g.: "/mktr_TP057_"
		size_t pos_2 = filename.rfind('/', pos_1);
		std::string str_mktr_APname = filename.substr(pos_2, pos_1 - pos_2 + 1);

		// now build the index for current filename :  "<TIMESTAMP>" + "/mktr_<APname>_". E.g.: "2012-12-13T15-29-22.tar.gz/mktr_TP057"
		files_indexes.push_back(str_timestamp + str_mktr_APname);
	}

	// sort files indexes list
	std::sort(files_indexes.begin(), files_indexes.end());

	// delete all exceeding files
	uint n_skipped = 0;
	for(int i = files_indexes.size(); i >= 0; --i)
	{
		if(n_skipped++ <= max_num_files)
			continue;

		// get current file index
		std::string current_file_index = files_indexes.at(i);

		// get "/mktr_<APName>_"part of file index
		size_t pos = current_file_index.rfind('/');
		std::string str_APname = current_file_index.substr(pos);

		// get TIMESTAMP part of file index
		std::string str_timestamp = current_file_index.substr(0, 26);

		// now build the path of the file to be deleted
		std::string filepath_to_delete = std::string(dir) + str_APname + str_timestamp;

		// delete file
		MktrFile ftd(filepath_to_delete);
		if(!ftd.remove())
		{
			std::cout<<std::endl<<"Failed to remove \""<< ftd.name() <<"\""<<std::endl;
			retval = false;
		}
	}

	return retval;
}



int getNodeState()
{
	int nodeState = 0;
	ACS_PRC_API prc;

	try
	{
		nodeState = prc.askForNodeState();
	}
	catch(int e)
	{
		return -1;
	}

	return nodeState;
}


const char * apg_node_name()
{
	static char str_apg_node_name[APG_NODE_NAME_MAX_SIZE] = {0};
	// static const char * p_object_name = "managedElementId=1";
	// static const char * p_attribute_name = "networkManagedElementId";

	if(!*str_apg_node_name)
	{
		ACS_CS_API_Name neid;
		ACS_CS_API_NS::CS_API_Result cs_op_result = ACS_CS_API_NetworkElement::getNEID(neid);
		if(cs_op_result != ACS_CS_API_NS::Result_Success)
		{
	//		std::cout << __func__ << "(): Error : call 'ACS_CS_API_NetworkElement::getNEID()' failed !" << " call_result == <" << cs_op_result << ">" << std::endl;
		}
		else
		{
			size_t len  = APG_NODE_NAME_MAX_SIZE - 1;
			cs_op_result = neid.getName(str_apg_node_name, len);
			if(cs_op_result != ACS_CS_API_NS::Result_Success)
			{
		//		std::cout << __func__ << "(): Error : call 'ACS_CS_API_Name::getname()' failed !" << " call_result == <" << cs_op_result << ">" << std::endl;
			}
		}
	}

	return str_apg_node_name;
}


int prepare_input_file_for_cliss_command(const char * cliss_command_template, const char * cliss_command_input_file_path)
{
	// Check function arguments
	if(!cliss_command_template || !cliss_command_input_file_path)
	{
		std::cout << __func__ << "(): Error : invalid input parameter ! " <<  ((cliss_command_template == NULL) ? "cliss_command_template" : "cliss_command_input_file_path") << " == NULL. " << std::endl;
		return -1;
	}

	// open COM command input file
	FILE * 	stream = fopen(cliss_command_input_file_path, "a");
	if(stream == NULL)
	{
		std::cout << __func__ << "(): Error opening (in write mode) the file  '" << cliss_command_input_file_path << "' ! errno == " << errno <<  ". " << std::endl;
		return -2;
	}

	// build COM command, and append "exit" at the end
	char cliss_cmd[MAX_CLISS_COMMAND_SIZE + 1 + 4 + 1] = {0};  // reserve 1 byte for newline, 4 bytes for "exit", 1 bytes for another newline
	snprintf(cliss_cmd, MAX_CLISS_COMMAND_SIZE, cliss_command_template, apg_node_name());
	strcat(cliss_cmd, "\nexit\n");

	// write COM commands to file
	if(fputs(cliss_cmd, stream) == EOF)
	{
		std::cout << __func__ << "(): Error writing cliss command """ << cliss_cmd << """ to the file  '"<< cliss_command_input_file_path << "' ! errno == " << errno << std::endl;
		fclose(stream);
		return -3;
	}

	fclose(stream);

	// cout << __func__ << "(): prepared CLISS command INPUT file : " << cliss_command_input_file_path << std::endl;

	return 0;
}


int prepare_input_files_for_cliss_commands(const MktrConfig & mktrConfig)
{
	int n_errors = 0;
	for(const MktrCommonDefs::Cliss_command_aux_info * p_cliss_cmd_aux_info = mktrConfig.clissCmdAuxInfoTable(); (p_cliss_cmd_aux_info != NULL) && (p_cliss_cmd_aux_info->cliss_cmd_template != NULL); ++p_cliss_cmd_aux_info)
	{
		prepare_input_file_for_cliss_command(p_cliss_cmd_aux_info->cliss_cmd_template, p_cliss_cmd_aux_info->cliss_cmd_input_file_path) &&  ++n_errors;
	}

	return n_errors;
}
//Commented for CNI_33_6_552

/*int is_mktr_running(){

	// return 0 => No other instances running
	// return 1 => Local lockfile already exist
	// return 2 => Error during local lockfile creation ( internal error )
	// return 3 => Error during remote lockfile creation
	// return 4 => Impossible to retrieve the remote hostname

	int lock_file_fd = -1;

	std::string localHostname;
	std::string partnerHostname;

	Mktr::getHostname(localHostname); //returns void

	if ( Mktr::getPartnerHostName(partnerHostname) ){
		return 4;
	}

	char lockfile_local_node[128] = {0};
	char lockfile_remote_node[128] = {0};

	snprintf(lockfile_local_node, sizeof(lockfile_local_node),"%s/%s",outputdir,localHostname.c_str());
	snprintf(lockfile_remote_node, sizeof(lockfile_remote_node),"%s/%s",outputdir,partnerHostname.c_str());

	char remote_lock_cmd[128] = {0};
	snprintf(remote_lock_cmd, sizeof(remote_lock_cmd),"ssh -t -q root@%s lockfile -1 -r 1 %s",partnerHostname.c_str(), lockfile_remote_node);


	if ((lock_file_fd = ::open(lockfile_local_node, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1 ){

		if ( errno == EEXIST )
			return 1;
		else
			return 2;
	}

	if ( system( remote_lock_cmd ) ){
		return 3;
	}

	return 0;
}*/ //Commented for CNI_33_6_552

//CNI_33_6_552 Changes START
int is_mktr_running()
{
  char*   line = NULL;
  ssize_t read;
  size_t  len =0;
  FILE*   fp;
  bool running_on_currentNode, running_on_partnerNode;
  std::string cmdString = "ps -C mktr";
  fp = popen(cmdString.c_str(), "r");
  if (! fp)  return 0;

  int Num_of_lines =0;
  while ((read = getline(&line, &len, fp)) != -1)
  {
	 Num_of_lines++;
  }

  ::pclose(fp);
  if(Num_of_lines >=4)
  {
    	running_on_currentNode = true;
  }
  else
  {
    	running_on_currentNode = false;
   }
   std::string partnerHostname;
   Mktr::getPartnerHostName(partnerHostname);

   cmdString = "ssh " + partnerHostname + " ps -C mktr";

   fp = popen(cmdString.c_str(), "r");
   if (! fp)  return 0;
   Num_of_lines =0;
   while ((read = getline(&line, &len, fp)) != -1)
   {
      	 Num_of_lines++;
   }
   ::pclose(fp);

   if(Num_of_lines >=2)
   {
     	running_on_partnerNode = true;
   }
   else
   {
     	running_on_partnerNode = false;
   }
   if(running_on_currentNode==true || running_on_partnerNode==true)
    	return 1;
    else
     	return 0;
} //CNI_33_6_552 Changes END

void remove_lock_files (){

	std::string localHostname;
	std::string partnerHostname;

	Mktr::getHostname(localHostname); //returns void

	Mktr::getPartnerHostName(partnerHostname);

	char lockfile_local_node[128] = {0};
	char lockfile_remote_node[128] = {0};

	char remote_lock_cmd[128] = {0};

	snprintf(lockfile_local_node, sizeof(lockfile_local_node),"%s/%s",outputdir,localHostname.c_str());
	snprintf(lockfile_remote_node, sizeof(lockfile_remote_node),"%s/%s",outputdir,partnerHostname.c_str());

	snprintf(remote_lock_cmd, sizeof(remote_lock_cmd),"ssh -t -q root@%s rm %s",partnerHostname.c_str(), lockfile_remote_node);

	remove ( lockfile_local_node );

	system ( remote_lock_cmd );
}


bool checkHardLimit(const std::string &outputFilePath)
{
	bool bRet = false;
	dqblk strctQuota;
	unsigned long sizeOutputFile = 0, sizeOldfile1 = 0, sizeOldfile2 = 0;
	u_int64_t avaiableSpace = 0;
	std::string nameOldFile1, nameOldFile2;

	memset(&strctQuota, 0, sizeof(strctQuota));

	struct stat info;
	stat("/data/opt/ap/internal_root/support_data", &info);  // Error check omitted

	mntent mnt;
	char buf[BUFSIZ];
	memset(buf,0,BUFSIZ);

	mountpoint("/data/opt/ap/internal_root/support_data", &mnt, buf, BUFSIZ);
	int iQ = quotactl(QCMD(Q_GETQUOTA, GRPQUOTA), mnt.mnt_fsname , info.st_gid, (char*)&strctQuota);

	if(iQ == 0){

		avaiableSpace = strctQuota.dqb_bhardlimit - toqb(strctQuota.dqb_curspace);
		struct stat infoFile;
		stat(outputFilePath.c_str(), &infoFile);
		sizeOutputFile = (infoFile.st_size / 1024);


		std::string dir = "/data/opt/ap/internal_root/support_data/mktr";
		std::list<MktrFile> filelist = MktrFile::list(dir + "/mktr_*_[0-9][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9]T[0-2][0-9]-[0-6][0-9]-[0-6][0-9].tar.gz");
		std::list<MktrFile>::iterator it;
		std::vector<std::string> files_indexes;

		for(it = filelist.begin(); it != filelist.end(); it++)
		{
			std::string filename = it->name();
			files_indexes.push_back(filename);
		}

		// sort files indexes list
		std::sort(files_indexes.begin(), files_indexes.end());
		std::vector<std::string>::iterator itName = files_indexes.begin();
		struct stat infoOldFile1,infoOldFile2;


		if(itName != files_indexes.end()){

			nameOldFile1 = *itName;
			stat(nameOldFile1.c_str(), &infoOldFile1);
			sizeOldfile1 = (infoOldFile1.st_size / 1024);
			itName++;
		}

		if(itName != files_indexes.end()){
			nameOldFile2 = *itName;
			stat(nameOldFile2.c_str(), &infoOldFile2);
			sizeOldfile2 = (infoOldFile2.st_size / 1024);
		}

		if(sizeOutputFile > avaiableSpace){
			if(sizeOutputFile > (avaiableSpace + sizeOldfile1)){
				if(sizeOutputFile > (avaiableSpace + sizeOldfile1 + sizeOldfile2)){
					std::cout<<std::endl<<"HardLimit has been exceeded !"<<std::endl;
				}else{
					// delete old file
					MktrFile ftd1(nameOldFile1.c_str());
					MktrFile ftd2(nameOldFile2.c_str());
					if(ftd1.remove() && ftd2.remove()){
						bRet = true;
					}
				}

			}else{
				// delete old file
				MktrFile ftd(nameOldFile1.c_str());
				if(ftd.remove()){
					bRet = true;
				}
			}
		}else{
			bRet = true;
		}
	}

	return bRet;
}

struct mntent *mountpoint(const char *filename, struct mntent *mnt, char *buf, size_t buflen)
{
    struct stat s;
    FILE *      fp;
    dev_t       dev;

    if (stat(filename, &s) != 0) {
        return NULL;
    }

    dev = s.st_dev;

    if ((fp = setmntent("/proc/mounts", "r")) == NULL) {
        return NULL;
    }

    while (getmntent_r(fp, mnt, buf, buflen)) {
        if (stat(mnt->mnt_dir, &s) != 0) {
            continue;
        }

        if (s.st_dev == dev) {
            endmntent(fp);
            return mnt;
        }
    }

    endmntent(fp);

    // Should never reach here.
    errno = EINVAL;
    return NULL;
}



// TR HY81976
/* checkMktruser() -
 * retCode:
 * -1 -> user does not exist
 * 0 -> user exists and all groups are present
 * 1 -> user exists but one or more groups absent
 */
int checkMktruser()
{
	int retCode = -1;
	errno = 0;
	struct passwd * userInfo = getpwnam(mktruserStr);
	if(userInfo == NULL)
		retCode = -1;
	else
	{
		int noOfGroups = 0;
		getgrouplist(userInfo->pw_name, userInfo->pw_gid, NULL, &noOfGroups);
		if(noOfGroups == 0)
			retCode = 1;
		else
		{
			gid_t groupList[noOfGroups];
			getgrouplist(userInfo->pw_name, userInfo->pw_gid, groupList, &noOfGroups);
			bool comEmergencyExists = false, tsgroupExists = false;
			for(int i=0; i < noOfGroups; i++)
			{
				struct group* gr = getgrgid(groupList[i]);
				if(gr != NULL)
				{
					if(!strcmp(gr->gr_name,group_comemergency))
						comEmergencyExists = true;
					else if(!strcmp(gr->gr_name,group_tsgroup))
						tsgroupExists = true;

					if(comEmergencyExists && tsgroupExists)
						break; // stop iterating because we found both groups we are interested in
				}
			}
			if(comEmergencyExists && tsgroupExists)
				retCode = 0;
			else
				retCode = 1;
		}
	}
	return retCode;
}

int createMktruser()
{
	int retCode = -1;
	std::string createUserCmd = (std::string)APOS_USERMGMT_CMD + " user add --shell=/sbin/nologin  --gname=com-emergency" \
			+ " --uname=" + mktruserStr;
	MktrCmd cmdExecutor;
	if(!cmdExecutor.execute(createUserCmd.c_str(),5))
		return retCode;

	if(applyGroupsToMktruser())
		return retCode;

	retCode = 0;
	return retCode;
}

int deleteMktruser()
{
	int retCode = -1;
	std::string deleteUserCmd = (std::string)APOS_USERMGMT_CMD + " user delete" + " --uname=" + mktruserStr;
	MktrCmd cmdExecutor;
	if(!cmdExecutor.execute(deleteUserCmd.c_str(),5))
		return retCode;

	retCode = 0;
	return retCode;
}

int applyGroupsToMktruser()
{
	Mktr* mktrExecInstance = Mktr::getMktrCmdExecutor();
	FILE* mktrLogPtr;
	if(mktrExecInstance != NULL)
		mktrLogPtr = mktrExecInstance->getLogStream();
	int retCode = -1;
	std::string applyGroupsCmd = (std::string) APOS_USERMGMT_CMD + " user modify --appendgroup --secgroups=" + group_tsgroup \
			 + " --uname=" + mktruserStr;
	MktrCmd cmdExecutor;
	if(!cmdExecutor.execute(applyGroupsCmd.c_str(),5))
	{
		(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt - Adding groups to user '%s' FAILED!\n\n",mktruserStr);
		return retCode;
	}
	(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt - Added groups to user '%s'!\n\n",mktruserStr);
	retCode = 0;
	return retCode;
}

void mktruserMgmtCleanup()
{
	if(mktruserCreated)
	{
		Mktr* mktrExecInstance = Mktr::getMktrCmdExecutor();
		FILE* mktrLogPtr;
		if(mktrExecInstance != NULL)
			mktrLogPtr = mktrExecInstance->getLogStream();
		if(deleteMktruser()) {
			(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt Cleanup - User '%s' deletion FAILED!\n\n",mktruserStr);
		}
		else {
			mktruserCreated = false;
			(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt Cleanup - User '%s' deleted!\n\n",mktruserStr);
		}
	}
}

void mktruserMgmtAtStartup()
{
	if(nodeState == MktrCommonDefs::NODE_ACTIVE || emergency_mode)
	{
		Mktr* mktrExecInstance = Mktr::getMktrCmdExecutor();
		FILE* mktrLogPtr;
		if(mktrExecInstance != NULL)
			mktrLogPtr = mktrExecInstance->getLogStream();

		bool errorDetected = false;
		int userStatus = checkMktruser();

		if(userStatus == 0)
		{  //user exists and groups are OK
			(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt Startup - User '%s' already exists!\n\n",mktruserStr);
		}
		else if(userStatus == 1)
		{	//user exists but groups are NOT OK
			(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt Startup - User '%s' already exists but groups not proper. Apply groups.\n",mktruserStr);
			if(applyGroupsToMktruser())
			{
				errorDetected = true;
				(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt Startup - Applying groups to existing user '%s' FAILED!\n\n",mktruserStr);
			}
		}
		else
		{	// user does not exist
			if(createMktruser())
			{
				errorDetected = true;
				(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt Startup - User '%s' creation FAILED!\n\n",mktruserStr);
			}
		}
		if(!errorDetected) {
			mktruserCreated = true;
			(mktrLogPtr != NULL) && fprintf(mktrLogPtr,"MKTR User Mgmt Startup - User '%s' created!\n\n",mktruserStr);
		}
	}
}
