#include "Mktr.h"
#include "MktrCommonDefs.h"
#include "MktrCmd.h"
#include "MktrCmdCsFiller.h"
#include "MktrDateFormat.h"
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <sstream>
#include <sys/prctl.h>
#include <sys/capability.h>

Mktr* Mktr::instance = NULL;
bool Mktr::alarm_raised = false;
#define TAR_CMD_FORMAT						" --format=gnu --rmt-command=/usr/libexec/rmt"

Mktr::Mktr(const std::string& path, const std::string& zfile)
{
	isAvailForWork = true;
	not_available_reason = "no error";
	stopped = false;
    mktrHome = path;
    mktrArchiveFileName = zfile + ".tar";
    mktrArchiveFilePathName = path + "/" + mktrArchiveFileName;
    bVerboseOption = false;

    std::string hostname;
    std::stringstream ssTmp;

    getHostname(hostname);

    printStreamOut("", true);

    ssTmp.str("");
    ssTmp << "Started collecting trouble information for " << hostname << ":" << std::endl;
    printStreamOut(ssTmp.str(), true);

    //Create MKTR log file
    mktrLogFile = path+"/acs_mktr.log";
    if((mktrLog = fopen(mktrLogFile.c_str() ,"w")) == NULL)
    {
    	// critical error !
     	isAvailForWork = false;
     	not_available_reason  = "Unable to create MKTR LOG file";
        ssTmp.str("");
        ssTmp << "Error: cannot create Mktr LOG file "<< mktrLogFile.c_str() << ". ERRNO = " << errno << std::endl;
        printStreamOut(ssTmp.str(), true);
        return;
    }

    // get APG Info
    if(apgInfo.load() != 0)
    {
    	fprintf(this->mktrLog, "Mktr::Mktr() ---> Cannot obtain APG Info ! Details: %s\n", apgInfo.getErrorDescr().c_str());
    	// Fix for TR HR38509 and HR63241 : avoid exiting when some APG info is not available
    	/*
    	isAvailForWork = false;
    	not_available_reason  = "Cannot obtain APG Info";
    	return;
    	 */
    }

    // Log APG Type
	if(apgInfo.apgType() == MktrCommonDefs::APG_APG43L)
    {
		fprintf(this->mktrLog,"\nRunning on APG43L\n");
    	folderSwitch = "";
    }
    else
    {
    	fprintf(this->mktrLog, "\nRunning on UNKNOWN node\n");
    	// Fix for TR HR38509 and HR63241 : avoid exiting when APG type is not available
    	/*
    	isAvailForWork = false;
     	not_available_reason  = "Unknown APG type";
    	return;
    	*/
    }

    // Log Node Status
    int nodeStatus = apgInfo.nodeStatus();
    if(nodeStatus == MktrCommonDefs::NODE_PASSIVE)
    	fprintf(mktrLog, "Running on passive node\n");
    else if(nodeStatus == MktrCommonDefs::NODE_ACTIVE)
    	fprintf(mktrLog, "Running on active node\n");
    else
    	fprintf(mktrLog, "Running on a node whose state is 'undefined'\n");

    // Log APG Shelf architecture type
    fprintf(mktrLog, "APG shelf architecture type : '%s'\n", apgShelfArchitectureType_descr(apgInfo.apgShelfArchitectureType()));

    // Log APZ Type
    fprintf(mktrLog, "APZ type: '%s'\n", apzType_descr(apgInfo.apzType()));
	
    // create the new MKTR tar file (overwriting the old MKTR tar file, if it exists)
    std::string createArchiveCmd = "tar cfT  " + mktrArchiveFilePathName + " /dev/null" + TAR_CMD_FORMAT;
    MktrCmd cmd;
    if(cmd.execute(createArchiveCmd)==false)
    {
    	// critical error !
    	fprintf(this->mktrLog,"Failed to create MKTR tar file \"%s\"\n", this->mktrArchiveFilePathName.c_str());
        ssTmp.str("");
        ssTmp << "Failed to create MKTR tar file \""<<mktrArchiveFilePathName<<"\""<<std::endl;
        printStreamOut(ssTmp.str(), true);
    	isAvailForWork = false;
    	not_available_reason = "Cannot create MKTR tar file";
    }

    fprintf(this->mktrLog, "\n");
}


Mktr::~Mktr()
{
    // close MKTR log file
	fclose(mktrLog);
}

// Init capability and propagates it to childs of this process
void Mktr::initCapability()
{
	cap_t cap;
	cap_value_t cap_list[1];
	// Initialization
	cap = NULL;
	cap_list[0] = CAP_SYS_RESOURCE;
	// Set inheritance capability propagation	
	if(prctl(PR_CAPBSET_DROP, CAP_SYS_RESOURCE) == 0 ){
			// allocates a capability state in working storage
			cap = cap_get_proc();
			// Check error
			if(cap != NULL){
				// Remove "cap_list" capability
				if ((cap_set_flag(cap, CAP_EFFECTIVE, 1, cap_list, CAP_CLEAR) == -1) || 
						(cap_set_flag(cap, CAP_INHERITABLE, 1, cap_list, CAP_CLEAR) == -1) ||
						(cap_set_flag(cap, CAP_PERMITTED, 1, cap_list, CAP_CLEAR) == -1)){
					// Error to remove "cap_list" capability
					fprintf(this->mktrLog, "Mktr::initCapability() - Error to remove capability flag.\n");
				}else{
					// Change capability
					if (cap_set_proc(cap) == 0){
						// Setting of capability ok
						fprintf(this->mktrLog, "Mktr::initCapability() - Capability set correctly.\n");
					}else{
						// handle error
						fprintf(this->mktrLog, "Mktr::initCapability() - Error to change capability.\n");
					}
				}
				// Release capability
				if(cap_free(cap) == -1){
					// handle error
					fprintf(this->mktrLog, "Mktr::initCapability() - Error to free capability.\n");
				}
			}else{
				// Error to allocates a capability
				fprintf(this->mktrLog, "Mktr::initCapability() - Error to allocates a capability.\n");
			}
	}else{
		// Error to set inheritance propagation
		fprintf(this->mktrLog, "Mktr::initCapability() - Error to set capability inheritance propagation.\n");
	}
}

int Mktr::archieve(const std::string& logFile, const bool isFile)
{
	int ret_val = 0;

	// sleep for 100 milliseconds
	usleep(100000);

	// STEP 1 - we need to split "logFile" parameter in two parts:
	// 1) <logFileDir> ---> directory containing "logFile" if "logFile" doesn't end with "/", or "logFile" string otherwise;
	// 2) <logFileName> ---> filename part of "logFile if "logFile" doesn't end with "/", or "." otherwise.
	// Example: if logFile == "/home/mydir/myfile", then <logFileDir> = "/home/mydir/" and <logFileName> = "myfile"
	std::string logFileName;
	std::string logFileDir;
	size_t ind = logFile.find_last_of("/");
	if(ind != std::string::npos)
	{
		if(ind == logFile.size() - 1)
		{
			// logFile terminates with "/" character.
			logFileName = logFile;
			logFileDir = ".";
		}
		else
		{
			logFileName = logFile.substr(ind+1);
			logFileDir = logFile.substr(0,ind);
		}
	}
	else
	{
		logFileName = logFile;
		logFileDir = ".";
	}

	// STEP 2 - build the command line.
    // We have to add the file "logFile" to the local Mktr single-node archive using TAR utility
	// This command  will be executed in two steps:
	// 1) move to the directory <logFileDir>;
	// 2) add the file <logFileName> to the TAR archive.
	std::string archCmdLine;
    if(isFile)
	{
		archCmdLine = "tar -C " + logFileDir + " -rf " + this->mktrArchiveFilePathName + " " + logFileName + TAR_CMD_FORMAT;
	}
	else
	{
		MktrFile file(logFile);
		if(file.isDir())
		{
			// "logFile" is a directory
			archCmdLine = "tar -C " + logFileDir + " -rf " + this->mktrArchiveFilePathName + " " + logFileName + folderSwitch
			+ TAR_CMD_FORMAT;
		}
		else //Files alike type
		{
			// "logFile" is an expression containing wildcard characters
			archCmdLine = "tar -C " + logFileDir + " -rf " + this->mktrArchiveFilePathName + " " + logFileName + TAR_CMD_FORMAT;
		}
	}


    fprintf(mktrLog,"Adding to archive: \"%s\"",archCmdLine.c_str());

	// STEP 3 - Execute the TAR command, redirecting standard output and standard error to MKTR log file.
	// Before launching TAR command, a line is written in the MKTR log file, having following format:
	// "#<commandline>\n" , where <commandline> is the TAR command to be launched
	fflush(mktrLog);
    MktrCmd cmd;
    if(!cmd.execute(archCmdLine, this->mktrLog,"",0))
    {
    	fprintf(mktrLog,", Failed!(%s)", cmd.getLastErrorMsg().c_str());
    	if((cmd.getCommandExecutionResult()== MktrCmd::MKTRCMD_NORMAL_EXIT_NOT_0) && (cmd.getCommandExecutorProcessExitCode() == 1))
			ret_val = 1;
    	else
    		ret_val = -1;
    }

    fprintf(mktrLog,"\n");
    return ret_val;
}


int Mktr::addToArchieve(const std::string& logFile, const std::string& tmpArchFile, const bool isFile)
{
	int ret_val = 0;

	usleep(100000);

	// Avoid that TAR utility archives files with their pathnames
	std::string logFileName;
	std::string logFileDir;
	size_t ind = logFile.find_last_of("/");
	if(ind != std::string::npos)
	{
		if(ind == logFile.size() - 1)
		{
			logFileName = logFile;
			logFileDir = ".";
		}
		else
		{
			logFileName = logFile.substr(ind+1);
			logFileDir = logFile.substr(0,ind);
		}
	}
	else
	{
		logFileName = logFile;
		logFileDir = ".";
	}

	std::string archCmdLine;
	if(isFile)
	{
		archCmdLine = "cd " + logFileDir + ";" + "tar -czf " + tmpArchFile + " " + logFileName + TAR_CMD_FORMAT;
	}
	else
	{
		MktrFile file(logFile);
		if(file.isDir())
		{
			archCmdLine = "cd " + logFileDir + ";" + "tar -czf " + tmpArchFile + " " + logFileName + folderSwitch + TAR_CMD_FORMAT;
		}
		else //Files alike type
		{
			archCmdLine = "cd " + logFileDir + ";" + "tar -czf " + tmpArchFile + " " + logFileName + TAR_CMD_FORMAT;
		}
	}

    fprintf(mktrLog,"Adding to archive: \"%s\"",archCmdLine.c_str());
    fflush(mktrLog);
	MktrCmd cmd;
	if(!cmd.execute(archCmdLine, this->mktrLog, "",0))
	{
		fprintf(mktrLog,", Failed!(%s)\n", cmd.getLastErrorMsg().c_str());
		ret_val = (cmd.getCommandExecutorProcessExitCode() == 1 ? 1 : -1);
	}

	return ret_val;
}



void Mktr::executeCmd(const MktrCommonDefs::Command & cmd)
{
	/*
	 * call the method replaceHostName to replace
	 * the tag <hostname1> and/or <hostname2>
	 * with the hostname retrieved from the STP
	 */
	std::stringstream ssTmp;
	std::string mycmdLine;
	mycmdLine = cmd.cmdLine;
	replaceHostName(mycmdLine);

	// check if we can execute the command
	if(stopped || !isSuitableToRun(!cmd.apply_strong_match, cmd.apType, cmd.apgType, cmd.apgShelfArchitectureType, cmd.apzType, cmd.nodeType, cmd.apgHwGepType))
    {
		// MKTR application has been stopped  or the command is not executable on this node in the current state
		if(cmd.logFile != NULL)
		{
			fprintf(mktrLog, "Skipping command: \"%s\", Log file:%s, timeout:%u\n\n", mycmdLine.c_str(), cmd.logFile, cmd.timeout);
		}
		else
		{
			fprintf(mktrLog, "Skipping command: \"%s\", timeout:%u\n\n", mycmdLine.c_str(), cmd.timeout);
		}

		return;
    }

	// trace the command we're going to execute
	if(cmd.logFile != NULL)
	{
		fprintf(mktrLog, "Executing the command: \"%s\", Log file:%s, timeout:%u\n", mycmdLine.c_str(), cmd.logFile, cmd.timeout);
	}
	else
	{
		fprintf(mktrLog, "Executing the command: \"%s\", timeout:%u\n", mycmdLine.c_str(), cmd.timeout);
	}

	// Clear the old file if exists
	if(cmd.logFile != NULL)
	{
		MktrFile oldLogFile(this->mktrHome + "/" + cmd.logFile);
		if(oldLogFile.exists())
		{
			if(!oldLogFile.remove())
			{
				fprintf(mktrLog,"Failed to remove old file \"%s\"\n",oldLogFile.name().c_str());
			}
		}
	}

	//Execute the command.
	// There are two types of commands:
	// 1 - commands containing only fixed parameters (without placeholders) --->  cmd.fType ==  MktrCommonDefs::CMD_FILLER_NONE
	// 2 - commands with placeholders										--->  cmd.fType ==  MktrCommonDefs::CMD_FILLER_CS
	switch(cmd.fType)
	{
		case MktrCommonDefs::CMD_FILLER_NONE:
		{
			// we're going to execute a command with fixed parameters (CASE 1)
			MktrCmd cmdExecutor;

			std::string timeMsg = "";
			if(cmd.timeout >= 3600)
			{
				timeMsg = " (This may take few hours)";
			}
			else if(cmd.timeout >= 60)
			{
				timeMsg = " (This may take few minutes)";
			}

			fprintf(mktrLog, "Fetching output from \"%s\"%s: ", cmd.title, timeMsg.c_str());
			ssTmp.str("");
			ssTmp << std::endl << "Fetching output from \""<< cmd.title <<"\"" << timeMsg <<": ";
			printStreamOut(ssTmp.str(), false, false);
			std::cout.flush();
			if(bVerboseOption)
				ssTmp.str("");  // if the verbose option is ON, we must reset the string stream to avoid duplication of "Fetching output from ..." message

			if(cmd.logFile != NULL)
			{
				// the output of the command will be redirected to the file <cmd.logFile>
				if(!cmdExecutor.execute(mycmdLine, mktrHome+"/"+cmd.logFile, CREATE_ALWAYS, "", cmd.timeout, cmd.userName, cmd.grpName))
				{
					fprintf(mktrLog,"Failed! (%s)", cmdExecutor.getLastErrorMsg().c_str());
					ssTmp << "Failed! ("<<cmdExecutor.getLastErrorMsg()<<")";
					printStreamOut(ssTmp.str(), true);
				}
				else
					printStreamOut("", false);

				fprintf(mktrLog,"\n");

				// archive the command output file
				archieve(mktrHome+"/"+cmd.logFile);

				// remove the command output file
				MktrFile ftd(mktrHome+"/"+cmd.logFile);
				if(!ftd.remove())
				{
					fprintf(mktrLog,"Failed to remove \"%s\"\n", ftd.name().c_str());
				}
			}
			else
			{
				// the output of the command will be redirected to Mktr application log file
				fflush(mktrLog);
				if(!cmdExecutor.execute(mycmdLine, this->mktrLog,"", cmd.timeout))
				{
					fprintf(mktrLog,"Failed! (%s)", cmdExecutor.getLastErrorMsg().c_str());
					ssTmp << "Failed! ("<<cmdExecutor.getLastErrorMsg()<<")";
					printStreamOut(ssTmp.str(), true);
				}
				else
					printStreamOut("", false);

				fprintf(mktrLog,"\n");
			}

			std::cout.flush();
			break;
		}

		case MktrCommonDefs::CMD_FILLER_CS:
		{
			// we're going to execute a command with placeholders (CASE 2)
			MktrCmd cmdExecutor;
			std::string timeMsg = "";
			int failed_cmd_count = 0;

			if(cmd.timeout >= 3600)
			{
				timeMsg = " (This may take few hours)";
			}
			else if(cmd.timeout >= 60)
			{
				timeMsg = " (This may take few minutes)";
			}

			fprintf(mktrLog,"Fetching output from \"%s\"%s: ",cmd.title, timeMsg.c_str());
			ssTmp.str("");
			ssTmp << std::endl << "Fetching output from \"" << cmd.title << "\"" << timeMsg << ": ";
			printStreamOut(ssTmp.str(), false, false);
			std::cout.flush();
			if(bVerboseOption)
				ssTmp.str("");  // if the verbose option is ON, we must reset the string stream to avoid duplication of "Fetching output from ..." message

			if(cmd.logFile != NULL)
			{
				// build the command list to launch, substituting placeholders in command line
				MktrCmdCsFiller csFiller(mycmdLine);
				std::list<std::string> cmdList = csFiller.combinations();
				int list_size = cmdList.size();
				const char * lfMode = CREATE_ALWAYS;

				if (list_size > 1) {
					lfMode = OPEN_ALWAYS;
				}

				// execute commands in the list
				for (std::list<std::string>::iterator cmdIt = cmdList.begin(); cmdIt != cmdList.end(); cmdIt++) {
					std::string cmd1 = *cmdIt;
					fprintf(mktrLog, "\n --->>> executing command \"%s\": ", cmd1.c_str());

					if (!cmdExecutor.execute(cmd1, mktrHome+"/"+cmd.logFile, lfMode, "", cmd.timeout, cmd.userName, cmd.grpName)) {
						fprintf(mktrLog,"Failed! (%s)", cmdExecutor.getLastErrorMsg().c_str());
						failed_cmd_count++;
					}

					printStreamOut(".", false, false);
					std::cout.flush();
				}

				if (failed_cmd_count > 0) {	// ERROR: some commands are failed, understand how many of them is failed
					if (failed_cmd_count < list_size)
						ssTmp << "Warning! (" << failed_cmd_count << " of " << list_size << " commands are failed)";
					else
						ssTmp << "Failed! (All commands are failed)";

					printStreamOut(ssTmp.str(), true);
				}
				else
					printStreamOut("", false);

				fprintf(mktrLog,"\n");

				// archive the command output file
				archieve(mktrHome+"/"+cmd.logFile);

				// delete the command output file
				MktrFile ftd(mktrHome+"/"+cmd.logFile);
				if(!ftd.remove())
				{
					fprintf(mktrLog,"Failed to remove \"%s\" %d\n",ftd.name().c_str(), errno);
				}
			}
			else
			{
				MktrCmdCsFiller csFiller(mycmdLine);
				std::list<std::string> cmdList = csFiller.combinations();
				int list_size = cmdList.size();

				// execute commands
				fflush(mktrLog);
				for (std::list<std::string>::iterator cmdIt = cmdList.begin(); cmdIt != cmdList.end(); cmdIt++) {
					std::string cmd1 = *cmdIt;
					fprintf(mktrLog, "\n --->>> executing command \"%s\": ", cmd1.c_str());

					if (!cmdExecutor.execute(cmd1, this->mktrLog,"", cmd.timeout)) {
						fprintf(mktrLog, "Failed! (%s)", cmdExecutor.getLastErrorMsg().c_str());
						failed_cmd_count++;
					}

					printStreamOut(".", false, false);
					std::cout.flush();
				}

				if (failed_cmd_count > 0) {	// ERROR: some commands are failed, understand how many of them is failed
					if (failed_cmd_count < list_size)
						ssTmp << "Warning! (" << failed_cmd_count << " of " << list_size << " commands are failed)";
					else
						ssTmp << "Failed! (All commands are failed)";

					printStreamOut(ssTmp.str(), true);
				}
				else
					printStreamOut("", false);

				fprintf(mktrLog,"\n");

//				std::cout<<std::endl;
				std::cout.flush();
				break;
			}
		}
		default:
		{
			break;
		}
	}
}


void Mktr::executeClt(const MktrCommonDefs::Collector& clt)
{
	std::stringstream ssTmp;

	if(stopped || !isSuitableToRun(!clt.apply_strong_match, clt.apType, clt.apgType, clt.apgShelfArchitectureType, clt.apzType, clt.nodeType))
    {
		// MKTR application has been stopped  or the command is not executable on this node in the current state
		if(clt.logFile != NULL)
		{
			fprintf(mktrLog, "Skipping command: \"%s\", Log file:%s, timeout:%u\n", clt.cmdLine, clt.logFile, clt.timeout);
		}
		else
		{
			fprintf(mktrLog, "Skipping command: \"%s\", timeout:%u\n", clt.cmdLine, clt.timeout);
		}

		return;
    }

    if(clt.logFile != NULL)
    {
    	fprintf(mktrLog, "Executing the command: \"%s\", Log file:%s, timeout:%u\n", clt.cmdLine, clt.logFile, clt.timeout);
    }
    else
    {
    	fprintf(mktrLog, "Executing the command: \"%s\", timeout:%u\n", clt.cmdLine, clt.timeout);
    }

    //Clear the old file if exists
    if(clt.logFile != NULL)
    {
        std::string logFilePath = mktrHome+"/";
        logFilePath += clt.logFile;
        MktrFile oldLogFile(logFilePath);
        if(oldLogFile.exists())
        {
            if(!oldLogFile.remove())
            {
            	fprintf(mktrLog,"Failed to remove old file \"%s\"\n", oldLogFile.name().c_str());
            }
        }
    }

	MktrFile pwd(clt.directory);
	if(pwd.exists())
	{
		pwd.remove();
	}

	if(!pwd.createDir())
	{
		fprintf(mktrLog,"Failed! (%s)", pwd.getLastErrorMsg().c_str());
		//std::cout<<"Failed! ("<<pwd.getLastErrorMsg()<<")";
		ssTmp.str("");
		ssTmp << "Failed! ("<<pwd.getLastErrorMsg()<<")";
		printStreamOut(ssTmp.str(), true);
	}
	else
	{
		//Execute the command
		switch(clt.fType)
		{
			case MktrCommonDefs::CMD_FILLER_NONE:
			{
				MktrCmd cmdExecutor;
				std::string timeMsg = "";
				if(clt.timeout >= 3600)
				{
					timeMsg = " (This may take few hours)";
				}
				else if(clt.timeout >= 60)
				{
					timeMsg = " (This may take few minutes)";
				}

//				std::cout<<std::endl<<"Fetching output from \""<<clt.title<<"\""<<timeMsg<<": ";

				fprintf(mktrLog, "Fetching output from \"%s\"%s: ",clt.title, timeMsg.c_str());
				fflush(mktrLog);
				ssTmp.str("");
				ssTmp << "Fetching output from \"" << clt.title << "\"" << timeMsg << ": ";
				printStreamOut(ssTmp.str(), false, false);
				if(bVerboseOption)
					ssTmp.str("");  // if the verbose option is ON, we must reset the string stream to avoid duplication of "Fetching output from ..." message

				if(clt.logFile != NULL)
				{
					if(!cmdExecutor.execute(clt.cmdLine, this->mktrLog, clt.directory, clt.timeout))
					{
						fprintf(mktrLog,"Failed! (%s)", cmdExecutor.getLastErrorMsg().c_str());
						ssTmp << "Failed! ("<<cmdExecutor.getLastErrorMsg()<<")";
						printStreamOut(ssTmp.str(), true);
						//std::cout<<"Failed! ("<<cmdExecutor.getLastErrorMsg()<<")"<<std::endl;
					}
					else
						printStreamOut("", false);

					addToArchieve(clt.directory, mktrHome+"/"+clt.logFile, false);
					archieve(mktrHome+"/"+clt.logFile);
					MktrFile ftd2(mktrHome+"/"+clt.logFile);
					if(!ftd2.remove())
					{
						fprintf(mktrLog,"Failed to remove \"%s\" %s\n", ftd2.name().c_str(), ftd2.getLastErrorMsg().c_str());
					}
				}
				else
				{
					if(!cmdExecutor.execute(clt.cmdLine, this->mktrLog, clt.directory, clt.timeout))
					{
						fprintf(mktrLog,"Failed! (%s)", cmdExecutor.getLastErrorMsg().c_str());
						ssTmp << "Failed! (" << cmdExecutor.getLastErrorMsg() << ")";
						printStreamOut(ssTmp.str(), true);
					}
					else
						printStreamOut("", false);

					archieve(clt.directory);
				}

//				std::cout<<std::endl;
				std::cout.flush();
				break;
			}
			default:
			{
				break;
			}
		}
		if(!pwd.remove())
		{
			fprintf(mktrLog,"Failed to remove \"%s\"\n", pwd.name().c_str());
		}

	}

}



void Mktr::fetchFile(const MktrCommonDefs::File& file)
{
	std::stringstream ssTmp;

	// Before fetching file, we have to verify two preconditions:
	// 1) the MKTR application has NOT been stopped;
	// 2) the file can be fetched on this node at this moment.
	if(stopped || !isSuitableToRun(!file.apply_strong_match, file.apType, file.apgType, file.apgShelfArchitectureType, file.apzType, file.nodeType))
    {
		return;
    }

    bool status = false;

//    std::cout<<std::endl<<"Fetching \""<<file.title<<"\": ";
//    std::cout.flush();
    ssTmp.str("");
	ssTmp << std::endl << "Fetching \""<<file.title<<"\": ";
	printStreamOut(ssTmp.str(),false, false);
	std::cout.flush();
	if(bVerboseOption)
		ssTmp.str("");  // if the verbose option is ON, we must reset the string stream to avoid duplication of "Fetching ..." message

    /*
     * call the method replaceHostName to replace
     * the tag <hostname1> and/or <hostname2>
     * with the hostname retrieved from the STP
     */
    std::string mySrcFile = file.srcFile;
    replaceHostName(mySrcFile);

    /* as above, for the destination file, but only if this is not NULL */
    std::string myDstFile;
    if(file.dstFile)
    {
    	myDstFile = file.dstFile;
    	replaceHostName(myDstFile);
    }

    fprintf(mktrLog,"Fetching \"%s\": ", file.title);
    if(file.dstFile == NULL)
    {
    	fprintf(mktrLog,"Fetching file: %s\n", mySrcFile.c_str());
    }
    else
    {
    	fprintf(mktrLog,"Fetching file(s): %s to : %s\n", mySrcFile.c_str(), myDstFile.c_str());
    }

    MktrFile srcFile(mySrcFile);

    // we distinguish two cases:
    // 1) file.srcFile is a FILE or a DIRECTORY
    // 2) file.srcFile is an expression containing wildcards (for example *.*)
    bool no_files_to_fetch = false;
    if(srcFile.exists())
    {
    	// CASE 1
    	if(file.dstFile == NULL)
        {
    		status = fetchFile(srcFile, NULL, no_files_to_fetch);
        }
        else
        {
            std::string dest = this->mktrHome + "/" + myDstFile;
            status = fetchFile(srcFile, dest.c_str(), no_files_to_fetch);
        }
    }
    else
    {
        // CASE 2
    	std::list<MktrFile> filesAlike = MktrFile::list(mySrcFile);
        if(filesAlike.size() > 0)
        {
            if(file.dstFile == NULL)
            {
				// add the files matching the expression "file.srcFile" DIRECTLY to the MKTR Archive
            	status = (archieve(mySrcFile, false) >= 0);
            	//status = true;
            }
            else
            {
            	// create, in the MKTR output directory,  a temporary zipped archive containing
            	// the files matching the expression "file.srcFile"
                int call_result = addToArchieve(mySrcFile, this->mktrHome + "/" + myDstFile, false);

                // add the temporary zipped archive to the MKTR Archive
                if(call_result >= 0)
                	call_result = archieve(mktrHome + "/" + myDstFile);

				status = (call_result >= 0);

                // delete the temporary zipped archive
                MktrFile ftd(mktrHome + "/" + myDstFile);
                if(!ftd.remove())
                {
                	fprintf(mktrLog,"Failed to remove \"%s\"\n", ftd.name().c_str());
                }
                //status = true;
            }
        }
        else
        {
        	no_files_to_fetch = true;
        }
    }

    if(!status)
    {
    	std::string strMess = (no_files_to_fetch ? "No File(s) Found": "Failed! (Impossible to fetch File(s))");
    	ssTmp << strMess;
    	printStreamOut(ssTmp.str(), true);
        //std::cout<< strMess;
        fprintf(mktrLog," - %s \"%s\")\n\n", strMess.c_str(), mySrcFile.c_str());
    }
    else
    	printStreamOut("", false);

    //std::cout<<std::endl;
    std::cout.flush();
    fflush(mktrLog);
}

bool Mktr::fetchFile(MktrFile& srcFile, const char* dest, bool & no_files_to_fetch)
{
	bool status = false;

	no_files_to_fetch = false;

	if(dest == NULL)
	{
		if(srcFile.exists())
		{
			if(srcFile.isFile())
			{
				// add the file srcFile.name() to the MKTR Archive
				status = (archieve(srcFile.name()) >= 0);
			}
			else if(srcFile.isDir())
			{
				std::list<MktrFile> contents = srcFile.list();
				if(contents.size() > 0)
				{
					// add the directory srcFile.name() to the MKTR Archive
					status = (archieve(srcFile.name(), false) >= 0);
				}
				else
				{
					no_files_to_fetch = true;
				}
			}
		}
		else
		{
			no_files_to_fetch = true;
		}
	}
	else
	{
		if(srcFile.exists())
		{
			if(srcFile.isFile())
			{
				// copy the file "srcFile.name()" to the destination file "dest"
				MktrFile* dstFile = srcFile.copyTo(dest);
				if(dstFile != NULL)
				{
					// add destination file to the MKTR Archive
					status = (archieve(dest) >= 0);

					// remove destination file
					MktrFile ftd(dest);
					if(!ftd.remove())
					{
						fprintf(mktrLog,"Failed to remove \"%s\"\n",ftd.name().c_str());
					}

					// release memory
					delete dstFile;

					//status = true;
				}
			}
			else if(srcFile.isDir())
			{
				std::list<MktrFile> contents = srcFile.list();
				if(contents.size() > 0)
				{
	            	// create a temporary zipped archive named "dest" containing the directory "file.srcFile"
					status = (addToArchieve(srcFile.name(), dest, false) >= 0);

					// add "dest" temporary zipped archive to the MKTR Archive
					status && (status = (archieve(dest) >= 0));
					printStreamOut("\n\n", true);
					// remove "dest" temporary zipped archive
					MktrFile ftd(dest);
					if(!ftd.remove())
					{
						fprintf(mktrLog,"Failed to remove \"%s\"\n",ftd.name().c_str());
					}
					//status = true;
				}
				else
				{
					no_files_to_fetch = true;
				}
			}
		}
		else
		{
			no_files_to_fetch = true;
		}
	}
    return status;
}

void Mktr::stop(int signum)
{
    fprintf(mktrLog,"Interrupt occurred (signum == '%d') ! stopping mktr\n", signum);
    fflush(mktrLog);
	stopped = true;
}

bool Mktr::isSuitableToRun(bool match_ok_when_info_not_available, const unsigned int apType, const unsigned int apgType, const unsigned int apgShelfArchitectureType, const unsigned int apzType, const MktrCommonDefs::NodeType nodeType, const unsigned int gepType)
{
	bool isApSuitable = false;
    bool isApgSuitable = false;
    bool isApgShelfArchSuitable = false;
    bool isApzSuitable = false;
	bool isNodeSuitable = false;
	bool isGepSuitable = false;

	int my_apType = apgInfo.apType();
	int my_apgType = apgInfo.apgType();
	int my_apgShelfArchitectureType = apgInfo.apgShelfArchitectureType();
	int my_apzType = apgInfo.apzType();
	int my_nodeState = apgInfo.nodeStatus();
	int my_gepType = apgInfo.gepType();

	if(((apType & my_apType) > 0) || (apType && !my_apType && match_ok_when_info_not_available) )
	{
		isApSuitable = true;
	}

	if(((apgType & my_apgType) > 0) || (apgType && !my_apgType && match_ok_when_info_not_available))
    {
		isApgSuitable = true;
    }

	if(((apgShelfArchitectureType & my_apgShelfArchitectureType) > 0) ||
		(apgShelfArchitectureType && !my_apgShelfArchitectureType && match_ok_when_info_not_available))
	{
		isApgShelfArchSuitable = true;
	}

	if(((apzType & my_apzType) > 0) || (apzType && !my_apzType && match_ok_when_info_not_available))
	{
		isApzSuitable = true;
	}

	if( ((nodeType & my_nodeState) > 0) || (nodeType && !my_nodeState && match_ok_when_info_not_available))
	{
		isNodeSuitable = true;
    }

	if (((gepType & my_gepType) > 0) || (gepType && !my_gepType && match_ok_when_info_not_available))
		isGepSuitable = true;

	return (isApgSuitable && isNodeSuitable && isApSuitable && isApgShelfArchSuitable && isApzSuitable && isGepSuitable);
}


void Mktr::fetchFileLogs(const MktrCommonDefs::File * fileTable)
{
    if(fileTable != NULL)
    {
        //Collect all the files
        int fInd=0;
        while(true)
        {
            if(fileTable[fInd].apgType == MktrCommonDefs::APG_NONE)
            {
                break;
            }
            else
            {
                fetchFile(fileTable[fInd]);
            }
            fInd++;
        }
    }
}


void Mktr::fetchCmdLogs(const MktrCommonDefs::Command* cmdTable)
{
    if(cmdTable != NULL)
    {
        //Collect all the command logs
        int cInd = 0;
        while(true)
        {
            if(cmdTable[cInd].apgType == MktrCommonDefs::APG_NONE)
            {
                break;
            }
            else
            {
                executeCmd(cmdTable[cInd]);
            }
            cInd++;
        }
    }
}

void Mktr::fetchCltLogs(const MktrCommonDefs::Collector* cltTable)
{
    if(cltTable != NULL)
    {
        //Collect all the command logs
        int cInd = 0;
        while(true)
        {
            if(cltTable[cInd].apgType == MktrCommonDefs::APG_NONE)
            {
                break;
            }
            else
            {
                executeClt(cltTable[cInd]);
            }
            cInd++;
        }
    }
}

void Mktr::fetchCoreDumpAtTime(const MktrDate& date, bool bTime)
{
	if(stopped)
		return;	// MKTR was stopped due to a TERMINATION SIGNAL (e.g. user sent Ctrl-C keystroke)

	MktrCommonDefs::File file;
	file.apType = MktrCommonDefs::AP_ALL;
	file.apgType = MktrCommonDefs::APG_ALL;
	file.nodeType = MktrCommonDefs::NODE_BOTH;
	file.apgShelfArchitectureType = MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL;
	file.apzType = MktrCommonDefs::APZ_ALL;
	file.dstFile = NULL;

	//Collect core dumps
	std::list<MktrFile> coreFiles = MktrFile::list("/cluster/dumps/*.core");
	std::list<MktrFile>::iterator cfit;
	MktrDate cbDate = date;
	MktrDate ceDate = date;
	cbDate.moveBackword(10);
	ceDate.moveForward(10);

	for(cfit = coreFiles.begin(); cfit != coreFiles.end(); cfit++)
	{
		MktrFile cf =*cfit;
		std::string fn = cf.name();

		if(fn.find(".core") != std::string::npos)
		{
			//MktrDate ct = cf.creationTime();
			MktrDate ct = cf.lastWriteTime();
			if((ct >= cbDate &&  ct <= ceDate)|| (bTime == false))
			{
				std::string ttl = "Core Dump ("+fn+")";
				file.title = (char*)ttl.c_str();
				file.srcFile = (char*)fn.c_str();
				fetchFile(file);
			}
		}
	}
}

void Mktr::fetchMemoryDumpAtTime(const MktrDate& date, bool bTime)
{
	if(stopped)
		return;	// MKTR was stopped due to a TERMINATION SIGNAL (e.g. user sent Ctrl-C keystroke)

	MktrCommonDefs::File file;
	file.apType = MktrCommonDefs::AP_ALL;
	file.apgType = MktrCommonDefs::APG_ALL;
	file.nodeType = MktrCommonDefs::NODE_BOTH;
	file.apgShelfArchitectureType = MktrCommonDefs::APG_SHELF_ARCHITECTURE_ALL;
	file.apzType = MktrCommonDefs::APZ_ALL;
	file.dstFile = NULL;

	//Collect core dumps
	std::list<MktrFile> coreFiles = MktrFile::list("/var/log/core/core*");
	std::list<MktrFile>::iterator cfit;
	MktrDate cbDate = date;
	MktrDate ceDate = date;
	cbDate.moveBackword(10);
	ceDate.moveForward(10);

	for(cfit = coreFiles.begin(); cfit != coreFiles.end(); cfit++)
	{
		MktrFile cf =*cfit;
		std::string fn = cf.name();

		if(fn.find("core") != std::string::npos)
		{
			//MktrDate ct = cf.creationTime();
			MktrDate ct = cf.lastWriteTime();
			if((ct >= cbDate &&  ct <= ceDate) || (bTime == false))
			{
				std::string ttl = "Memory dump ("+fn+")";
				file.title = (char*)ttl.c_str();
				file.srcFile = (char*)fn.c_str();
				fetchFile(file);
			}
		}
	}
}

Mktr * Mktr::createMktrCmdExecutor(const std::string& path, const std::string& file)
{
	if (Mktr::instance == NULL)
		Mktr::instance = new (std::nothrow) Mktr(path, file);

	return Mktr::instance;
}

void Mktr::releaseMktrCmdExecutor()
{
	if (Mktr::instance != NULL)
		delete Mktr::instance;

	Mktr::instance = NULL;
}

int Mktr::createOutZippedArchive()
{
	int retVal = 0;
	std::string outFilePathName = this->mktrArchiveFilePathName + ".gz";
	std::string zipCmd = "gzip -c " + this->mktrArchiveFilePathName + " > " + outFilePathName;

	for(int i = 0; i < 3; ++i)
	{
		MktrCmd cmd;
		if(cmd.execute(zipCmd)==false)
		{
			/*
			std::cout << "Warning : failed attempt " << i+1 << " of " << 10 << " to produce MKTR output file \""<<outFilePathName.c_str()<<"\". Error Description : "<<  cmd.getLastErrorMsg() <<  std::endl;;
			std::cout.flush();
			*/
			retVal = -1;
			::sleep(1);
		}
		else
		{
			retVal = 0;
			break;
		}
	}

	// in case of failure, try to destroy the archive file built so far
	if(retVal == -1)
	{
		MktrFile ftr(outFilePathName);
		ftr.remove();
	}

	// std::cout << ">>>>  Produced MKTR output file <" << outFilePathName.c_str() << ">" << std::endl;

	return retVal;
}


void Mktr::addLogFileToArchiveAndDeleteIt()
{
	// Write last unwritten data
	fflush(this->mktrLog);

	// add MKTR Log file to MKTR TAR archive, taking ONLY the file name
	std::string logFileName;
	size_t ind = this->mktrLogFile.find_last_of("/");
	if(ind != std::string::npos)
		logFileName = this->mktrLogFile.substr(ind+1);
	else
		logFileName = this->mktrLogFile;

	std::string archCmdLine = "tar -C " + this->mktrHome + " -rf " + this->mktrArchiveFilePathName+ " "+ logFileName + TAR_CMD_FORMAT;
	MktrCmd cmd;
	if(!cmd.execute(archCmdLine))
	{
		std::cout<<std::endl<<"Failed to add \""<<this->mktrLogFile<<"\". Details: "<< cmd.getLastErrorMsg() << std::endl;
	}

	// delete MKTR Log file
	MktrFile ftd(this->mktrLogFile);
	if(!ftd.remove())
	{
		std::cout<<std::endl<<"Failed to remove \""<<this->mktrLogFile<<"\""<<std::endl;
	}
}

void Mktr::setAlarm(int nSeconds)
{
	Mktr::alarm_raised = false;
	alarm(nSeconds);
}

int Mktr::replaceHostName(std::string& srcfile)
{
	std::string tmp(srcfile);
	ifstream infile;
	std::string data;

	if(srcfile.find("<hostname1>") != std::string::npos)
	{
		// Open the file to read this hostname
		infile.open("/etc/cluster/nodes/this/hostname");

		if (infile.fail())
			return 1;

		// Read the value
		getline(infile, data);

		//close the file
		infile.close();

		// replace the host name
		tmp.replace(srcfile.find("<hostname1>"), 11, data);
		srcfile = tmp;

		return 0;
	}
	else if(srcfile.find("<hostname2>") != std::string::npos)
	{
		// Open the file to read this hostname
		infile.open("/etc/cluster/nodes/peer/hostname");

		if (infile.fail())
			return 1;

		// Read the value
		getline(infile, data);

		//close the file
		infile.close();

		tmp.replace(srcfile.find("<hostname2>"), 11, data);
		srcfile = tmp;

		return 0;
	}
	else
		return 0;
}

int Mktr::runOnOtherNode(std::string commandLine){
	int retVal = 0;
	ifstream ifs;
	std::string hostname;
	std::string sshCommandString;

	ifs.open("/etc/cluster/nodes/peer/hostname");

	if (ifs.good())
		getline(ifs, hostname);
	else
		retVal = -1;

	ifs.close();

	//sshCommandString = "ssh root@" + hostname + " mktr -o" ;
	sshCommandString = "ssh -t -q root@" + hostname + " " + commandLine + " -o" ;

	MktrCmd cmd;
	if(cmd.execute(sshCommandString,-1))
	{
		retVal = 0;
	}
	else
		retVal = -1;

	return retVal;
}

void Mktr::getHostname(std::string &outputHostname){
	char hostName[1024];
	size_t len = 1024;
	if(ACE_OS::hostname(hostName, len) < 0)
	{
		hostName[0] = '\0';
	}

	outputHostname = "";
	for(int i=0; i<1024 && hostName[i] != '\0'; i++)
	{
		outputHostname  += hostName[i];
	}
}

int Mktr::getPartnerHostName(std::string & outputPartnerHostname)
{
	int retVal = 0;

	ifstream ifs;
	std::string partnerHostname;

	ifs.open("/etc/cluster/nodes/peer/hostname");
	if(ifs.good())
	{
		getline(ifs, partnerHostname);
		if(ifs.fail())
		{
			retVal = -1;
		}
	}
	else
		retVal = -1;

	ifs.close();

	if(retVal == 0)
		outputPartnerHostname = partnerHostname;

	return retVal;
}

void Mktr::setVerbose(bool bVerbose)
{

	bVerboseOption = bVerbose;

}

void Mktr::printStreamOut(const std::string& strFetching, bool bError, bool bPrintNewline)
{
	if((bError == true) || (bVerboseOption == true)){
		std::cout << strFetching.c_str();
		if(bPrintNewline)
			std::cout <<std::endl;
	}
}
