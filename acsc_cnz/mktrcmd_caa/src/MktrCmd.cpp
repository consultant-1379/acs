#include "MktrCmd.h"
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <list>
#include <dirent.h>
#include <grp.h>
#include "Mktr.h"

extern const char* mktruserStr;
// Structure representing a subset of the info associated with a running process
struct ProcessStatInfo
{
	int pid;
	char comm[256];
	char state;
	int ppid;
	int pgrd;
	int sessionID;
	int tty_nr;
	int tty_pgrp;
};


static bool is_number(const char *str)
{
	if(str == NULL)
		return false;

	while(isdigit(*str))
		++str;

	return (*str == '\0');
}

/**
 * default constructor
 */
MktrCmd::MktrCmd()
{
	exitCode  = 0;
	errorCode = 0;
	errorMsg = "";
	appErrorCode = MKTRCMD_NO_INFO;
	cmdExecutorPid = 0;
}

/**
 * default destructor
 */
MktrCmd::~MktrCmd()
{
}


/**
 * write an header text in a file of output
 */
bool MktrCmd::writeHeader(FILE **fp, std::string head)
{
	std::string header = "\n#" + head + '\n';
	int iWrited;
	bool status = true;

	iWrited = fputs(header.c_str(), *fp);

	if(iWrited == EOF)
	{
		errorCode = errno;
		status = false;
	}

	return status;
}


bool MktrCmd::execute(const std::string& cmd, const std::string& log, const std::string mode, const std::string& dir, const int timeout, const char *userName, const char *groupName)
{
	bool status = true;
	FILE    *fLog;

	// STEP 1 : open log file
	if((fLog = fopen((char *) log.c_str(), mode.c_str())) == NULL)
	{
		appErrorCode = MKTRCMD_SYSTEM_ERROR;
		errorCode = errno;
		errorMsg = "fopen() error while opening COMMAND LOG file ";
		return false;
	}

	// STEP 2 : write eventually an HEADER line in the log file
	if( (mode == OPEN_ALWAYS)  &&  !(status = writeHeader(&fLog, cmd)) )
	{
		appErrorCode = MKTRCMD_SYSTEM_ERROR;
		errorCode = errno;
		errorMsg = "fputs() error while writing HEADER in COMMAND LOG file ";
	}

	// STEP 3 : force write on log file of unwritten buffered data
	if(status && !(status = !fflush(fLog)))
	{
		appErrorCode = MKTRCMD_SYSTEM_ERROR;
		errorCode = errno;
		errorMsg = "fflush() error on COMMAND LOG file ";
	}

	// STEP 4 : execute command, redirecting standard output and standard error to log file
	status && (status = this->execute_cmd_helper(cmd, dir, fLog->_fileno, fLog->_fileno, timeout, userName, groupName));

	// close output file
	fclose(fLog);

	return status;
}


/**
 * execute a passed command by launching a child process
 */
bool MktrCmd::execute(const std::string& cmd, int timeout)
{
	return execute_cmd_helper(cmd, "", -1, -1, timeout, "", "");
}


bool MktrCmd::execute(const std::string& cmd, FILE * fLog, const std::string& dir,const int timeout, const char *userName, const char *groupName)
{
	bool status = !fLog;

	// STEP 1 : write eventually an HEADER line in the fLog file
	if( !(status = writeHeader(&fLog, cmd)) )
	{
		appErrorCode = MKTRCMD_SYSTEM_ERROR;
		errorCode = errno;
		errorMsg = "fputs() error while writing HEADER  in the specified LOG file ";
	}

	// STEP 2 : force write on fLog file of unwritten buffered data
	if(status && !(status = !fflush(fLog)))
	{
		appErrorCode = MKTRCMD_SYSTEM_ERROR;
		errorCode = errno;
		errorMsg = "fflush() error on the specified LOG file ";
	}

	// STEP 3 : execute command, redirecting standard output and standard error to fLog file
	status && (status = this->execute_cmd_helper(cmd, "", fLog->_fileno, fLog->_fileno, timeout, userName, groupName));

	ACE_UNUSED_ARG(dir);

	return status;
}



bool MktrCmd::execute_cmd_helper(const std::string & cmd, const std::string & dir, int fdOut,  int fdErr, int timeout, const char *userName, const char *groupName)
{
	int retVal = false;
	std::string cmd_to_launch = cmd;

#ifdef __DEBUG_AUX_LOG__
	Mktr * pMktr = Mktr::getMktrCmdExecutor();
	FILE * mktrLogStream = (pMktr ? pMktr->getLogStream() : 0);
	mktrLogStream && fflush(mktrLogStream);
#endif

	// Create the process that will be used to execute the command
	int pid;
	if((pid = fork()) == -1)
	{
		appErrorCode = MKTRCMD_SYSTEM_ERROR;
		errorCode = errno;
		errorMsg = "fork() error";
	}
	else if(pid == 0)
	{
		// the CHILD
		// if requested, change work dir
		if( (dir != "") &&  chdir((char *) dir.c_str())==-1)
			exit(-1);

		// if requested, redirect standard output
		if( (fdOut != -1) && dup2(fdOut, STDOUT_FILENO)==-1 )
			exit(-2);

		// if requested, redirect standard error
		if( (fdErr != -1) && dup2(fdErr, STDERR_FILENO)==-1 )
			exit(-3);

		// cache EUID and EGID
		uid_t my_uid = geteuid();
		gid_t my_gid = getegid();

		// eventually set EGID (with an important exception, noted ahead)
		if(groupName && *groupName)
		{
			errno = 0;
			struct group * groupInfo = getgrnam(groupName);
			if(!groupInfo)
			{
				if(errno == 0)
					std::cout << "MKTR command executor MESSAGE --> unable to set EGID. Group '" << groupName << "' doesn't exist !" << std::endl << std::endl;
				else
					std::cout << "MKTR command executor MESSAGE --> unable to set EGID. Call 'getgrnam(" << groupName << ")' failed ! errno == " << errno << std::endl << std::endl;
			}
			else
			{
				/* special handling (this is the exception we talked about above)
				 * if userName == "*", then:
				 * 1) get the first user in the group <groupName>; let's call it <found-user>;
				 * 2) execute the command using "sudo -u <found-user> <cmd>"
				 *
				 *  Continue normally if <groupName> has no users
				*/
				if(userName && *userName && !strcmp(userName,"*") && groupInfo->gr_mem && *(groupInfo->gr_mem))
				{
					cmd_to_launch = std::string("sudo -u ") + *(groupInfo->gr_mem) + " " + cmd;
				}
				else if(userName && *userName && !strcmp(userName,mktruserStr))
				{
					cmd_to_launch = std::string("sudo -u ") + userName + " " + cmd;
				}
				else
				{
					// set EGID
					if(setegid(groupInfo->gr_gid) == -1)
					{
						std::cout << "MKTR command executor MESSAGE --> unable to set EGID. Call 'setegid(" << groupInfo->gr_gid << ")' failed ! errno == " << errno << std::endl << std::endl;
					}
#ifdef __DEBUG_AUX_LOG__
					else if(mktrLogStream)
					{
						fprintf(mktrLogStream,"\n--> set EGID to '%d' (%s)", groupInfo->gr_gid, groupName);
					}
#endif
				}
			}
		}

		// eventually set EUID
		if(userName && *userName && strcmp(userName,"*"))
		{
			errno = 0;
			struct passwd * userInfo = getpwnam(userName);
			if(!userInfo)
			{
				if(errno == 0)
					std::cout << "MKTR command executor MESSAGE --> unable to set EUID. User '" << userName << "' doesn't exist !" << std::endl << std::endl;
				else
					std::cout << "MKTR command executor MESSAGE --> unable to set EUID. Call 'getpwnam(" << userName << ")' failed ! errno == " << errno << std::endl << std::endl;
			}
			else
			{
				if(seteuid(userInfo->pw_uid) == -1)
				{
					std::cout << "MKTR command executor MESSAGE --> unable to set UID. Call 'setuid(" << userInfo->pw_uid << ")' failed ! errno == " << errno << std::endl << std::endl;
				}
#ifdef __DEBUG_AUX_LOG__
				else if(mktrLogStream)
				{
					fprintf(mktrLogStream,"\n--> set EUID to '%d' (%s)", userInfo->pw_uid, userName);
				}
#endif
			}
		}

#ifdef __DEBUG_AUX_LOG__
		// log the command effectively launched
		if(mktrLogStream)
		{
			fprintf(mktrLogStream, "\n-->>>  going to launch : '%s'\n", cmd_to_launch.c_str());
			fflush(mktrLogStream);
		}
#endif

		// execute the program with command line "cmd"
		if(execlp("sh","sh", "-c", cmd_to_launch.c_str(), (char *) NULL)==-1){
			seteuid(my_uid );
			setegid(my_gid);
			exit(-4);
		}
	}
	else
	{
		// the FATHER
		// let's take note of the PID of the child
		this->cmdExecutorPid = pid;

		// Set ALARM, if requested
		if(timeout > 0)
			Mktr::setAlarm(timeout);

		// wait CHILD termination
		while(1)
		{
			int status;
			errno = 0;
			if((waitpid(pid, &status, 0) != -1) || (errno == EINTR))
			{
				if(Mktr::isAlarmRaised())
				{
					// TIMEOUT expiration (SIGALRM received). Terminate the process tree rooted at CHILD
					if(terminateProcessTree(cmdExecutorPid))
						appErrorCode = MKTRCMD_TIMEOUT;

				}
				else if(errno == EINTR)
				{
					// waitpid() function was interrupted by a signal other than SIGALRM; re-invoke it
					continue;
				}
				else if(WIFSIGNALED(status))
				{
					// CHILD process terminated because of a signal not caught
					appErrorCode = MKTRCMD_SIGNAL_NOT_CAUGTH;
					char err_str[2048]={0};
					snprintf(err_str, 2048,"Child Process (PID == %d) exited because of a signal (%d) not caught", pid, WTERMSIG(status));
					errorMsg = err_str;
				}
				else if (WIFEXITED(status))
				{
					// normal CHILD termination. Check exit code
					if( (exitCode = WEXITSTATUS(status)) == 0)
					{
						// Child process exited normally with exit code 0
						appErrorCode = MKTRCMD_NORMAL_EXIT_0;
						retVal = true;
					}
					else
						appErrorCode = MKTRCMD_NORMAL_EXIT_NOT_0;
				}
				else
				{
					// abnormal CHILD termination
					appErrorCode = MKTRCMD_ABNORMAL_TERM;
				}
			}
			else
			{
				errorCode = errno;
				errorMsg = "waitpid() error";
				appErrorCode = MKTRCMD_SYSTEM_ERROR;
			}

			//clear ALARM, if previously set
			if(timeout > 0)
				Mktr::setAlarm(0);

			break; // exit loop
		}
	}

	return retVal;
}



bool MktrCmd::buildPostOrderPTreeList(int rootPid, std::list<int> & outPidList)
{
	bool rootPIDFound = false;

	// iterate on "/proc" virtual file system to get running processes
	DIR *dp;
	struct dirent *ep;
	dp = opendir("/proc");
	if (dp != NULL)
	{
	   while ((ep=readdir(dp)) != NULL)
	   {
		   if(is_number(ep->d_name))
		   {
			   // ok, we've an entry representing a PROCESS. Let's take info about it
			   char statFilePath[256];
			   sprintf(statFilePath,"/proc/%s/stat",ep->d_name);

			   // open file /proc/<PID>/stat to get process info
			   FILE *fStat;
			   if( (fStat = fopen(statFilePath,"r")) == NULL)
			   {
				   if(errno == ENOENT)
				   {
					   // 'stat' file doesn't exist. This means that the process has terminated in the meanwhile. No problem !
					   continue;
				   }

				   appErrorCode = MKTRCMD_SYSTEM_ERROR;
				   errorCode = errno;
				   errorMsg  += std::string("fopen() error while opening STAT file: ") + statFilePath;

				   closedir(dp);
				   return false;
			   }

			   // read process info and close file
			   ProcessStatInfo procInfo;
			   int nFields = fscanf(fStat,"%d %s %c %d %d %d %d %d", &procInfo.pid, procInfo.comm, &procInfo.state, &procInfo.ppid, &procInfo.pgrd, &procInfo.sessionID, &procInfo.tty_nr, &procInfo.tty_pgrp );
			   fclose(fStat);

			   // check parsing result
			   if(nFields != 8)
			   {
				   char tmpErrMess[128];
				   sprintf(tmpErrMess, "PAY ATTENTION ! fscanf() returned  %d , instead of the expected value (8)", nFields);
				   appErrorCode = MKTRCMD_APPERROR;
				   errorMsg = tmpErrMess;

				   closedir(dp);
				   return false;
			   }

			   // check if current PROCESS entry is a CHILD of rootPID and is not a ZOMBIE
			   if(procInfo.ppid == rootPid  &&  procInfo.state != 'Z')
			   {
				   //invoke function recursively on the CHILD process subtree
				   if(buildPostOrderPTreeList(procInfo.pid, outPidList) == false)
				   {
					   closedir(dp);
					   return false;
				   }
			   }
			   else if(!rootPIDFound && (procInfo.pid == rootPid))
			   {
				   // we found root PID
				   rootPIDFound = true;
			   }
		   }
	   }

	   // close directory
	   closedir(dp);

	   if(rootPIDFound)
	   {
		   // add ourself to output list
		  outPidList.push_back(rootPid);
	   }
	}

	return true;
}


bool MktrCmd::terminateProcessList(const std::list<int> & pidlist)
{
	int ret_val = true;
	int n_EPERM_errors = 0, n_ESRCH_errors = 0, n_other_errors = 0;

	for(std::list<int>::const_iterator it = pidlist.begin(); it !=  pidlist.end(); ++it)
	{
		if(kill(*it,SIGKILL)!=0)
		{
		   if(errno == EPERM)
			   ++n_EPERM_errors;
		   else if(errno == ESRCH)
			   ++n_ESRCH_errors;	 // not really an error. Process was yet terminated !
		   else
			   ++n_other_errors;
		}
	}

	if(n_EPERM_errors || n_other_errors)
	{
		char tmpErrMess[256];
		sprintf(tmpErrMess, "kill() errors occurred during execution of 'MktrCmd::terminate. N_EPERM = %d, N_ESRCH = %d, N_OTHERS = %d", n_EPERM_errors, n_ESRCH_errors, n_other_errors);
		errorMsg = tmpErrMess;
		appErrorCode = MKTRCMD_SYSTEM_ERROR;
		ret_val = false;
	}

	return ret_val;
}


bool MktrCmd::terminateProcessTree(int rootPID)
{
	bool retVal = false;

	// build the list of processes that we have to kill, in the correct order
	std::list<int> pidList;
	if(buildPostOrderPTreeList(rootPID,pidList))
	{
		// terminate the processes in the list
		retVal = terminateProcessList(pidList);
	}

	return retVal;
}


std::string MktrCmd::getLastErrorMsg()
{
   	std::string retErrorMsg = "";

   	switch(appErrorCode)
   	{
   	case MKTRCMD_NO_INFO:
		retErrorMsg = "no info available !";
		break;
   	case MKTRCMD_APPERROR:
		{
			retErrorMsg = "Application (not system) error --> DETAILS:   ";
			retErrorMsg += errorMsg;
		}
   		break;
   	case MKTRCMD_SYSTEM_ERROR:
		{
			char sysErrMess[1024];
   	        snprintf(sysErrMess, 1024, "%s. ERRNO = %lu, ERRDESC = %s", this->errorMsg.c_str(), this->errorCode, strerror(this->errorCode));
   	        retErrorMsg = "Unexpected error --> DETAILS:   ";
   	        retErrorMsg += sysErrMess;
   		}
		break;
   	case MKTRCMD_NORMAL_EXIT_0:
		break;
   	case MKTRCMD_NORMAL_EXIT_NOT_0:
		{
   	        char ecs[16];
   	        sprintf(ecs,"%lu",this->exitCode);
   	        retErrorMsg = "Command exited with:";
   	        retErrorMsg += ecs;
   	    }
		break;
	case MKTRCMD_TIMEOUT:
		retErrorMsg = "Timeout occurred";
		break;
	case MKTRCMD_SIGNAL_NOT_CAUGTH:
		retErrorMsg = "Command terminated because of an unexpected signal--> DETAILS:  ";
		retErrorMsg += errorMsg;
		break;
	case MKTRCMD_ABNORMAL_TERM:
		retErrorMsg = "Command terminated abnormally";
		break;
	default:
		break;
   	}

   	return retErrorMsg;

}

