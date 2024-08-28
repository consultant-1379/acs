	/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/
/**
*
* @file ACS_XBRM_APBackupHanlder.cpp
*
* @brief
* Main file for creating ap backup
*
* @details
* Parses the arguments passed to the createBackup action and then
* creates and/or exports the backup with dispalying progess information
*
* @author
*
-------------------------------------------------------------------------*/
/*
*
* REVISION HISTORY
*
* DATE            USER         DESCRIPTION/TR
* ----------------------------------------------
* 05/16/2023     XSRAVAN       Initial Release

****************************************************************************/
/* *Modified on: 16 May 2023
 *       Author: xsravan
 */

#include "ACS_XBRM_ApBackupHandler.h"
#include <ace/ACE.h>
#include "ACS_APGCC_CommonLib.h"
#include "ACS_APGCC_CLibTypes.h"
#include "ACS_CS_API.h"
#include "acs_prc_api.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"

#include <stdlib.h>
#include <sys/quota.h>
#include <mntent.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <sys/mount.h>
#include <pthread.h>
#include <cstdio>
#include <iostream>

#define RTN_OK 0
#define RTN_FAIL -1
#define NOERROR 0
#define FORCEDQUIT -1	// Forced quit press not
#define GENERALFAULT -2 // General Fault
#define SPACE_ISSUE -3
#define INTERNALFAULT -4

#define ONGOINGBACKUP -5
#define POPEN_READ 0
#define POPEN_WRITE 1

struct arg_struct
{
	char *tasktartTime;
	char *backupPasswd;
	char *labelName;
	char *transferQueue;
	bool isSecured;
	char *ori_backupName;
	ACS_XBRM_Utilities *utilities;
	char *reportProgressDn;
	char *returnError;
};

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_ApBackupHandler);

using namespace std;
ACS_XBRM_ApBackupHandler::ACS_XBRM_ApBackupHandler(string arg1, string arg2, string arg3, string arg4, string reportProgressDn)
{
	ACS_XBRM_TRACE_FUNCTION;
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered the constrcutor for ACS_XBRM_ApBackupHandler");	
	m_returnError = "";
	m_isSecured = false; // anssi
	m_ori_backupName = "";
	m_tasktartTime = arg1;
	m_backupPasswd = arg2;
	m_labelName = arg3;
	m_transferQueue = arg4;
	this->reportProgressDn = reportProgressDn;
	this->utilities = new ACS_XBRM_Utilities();
}
ACS_XBRM_ApBackupHandler::~ACS_XBRM_ApBackupHandler()
{

	if (utilities != NULL)
	{
		delete utilities;
		utilities = NULL;
	}
}

ACE_INT32 ACS_XBRM_ApBackupHandler::execute()
{
	//ACS_XBRM_TRACE_FUNCTION;
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "execute");

	// Checking if its is a secure or regular backup
	if (m_backupPasswd.length() > 0)
	{
		m_isSecured = true;
		ACS_XBRM_LOG(LOG_LEVEL_INFO, "m_isScecured = %d", m_isSecured);
	}
	else
	{
		m_isSecured = false;
		ACS_XBRM_LOG(LOG_LEVEL_INFO, "m_isScecured = %d", m_isSecured);
	}

	// Creating an args to pass to thread
	arg_struct *args = (arg_struct *)malloc(sizeof(*args));
	args->tasktartTime = strdup(m_tasktartTime.c_str());
	if (m_backupPasswd != (char *)"")
	{
		args->backupPasswd = strdup(ACS_XBRM_Utilities::decryptString(m_backupPasswd).c_str());
	}
	else
	{

		args->backupPasswd = strdup((char *)"");
	}

	if (m_labelName != (char *)"")
	{
		args->labelName = strdup(m_labelName.c_str());
	}
	else
	{
		args->labelName = strdup((char *)"");
	}

	if (m_transferQueue != (char *)"")
	{
		ACS_XBRM_LOG(LOG_LEVEL_INFO, "Starting Thread Creation--%s", m_transferQueue.c_str());
		args->transferQueue = strdup(m_transferQueue.c_str());
	}
	else
	{
		args->transferQueue = strdup((char *)"");
	}

	args->isSecured = m_isSecured;
	args->returnError = strdup((char *)"");
	if (m_ori_backupName != (char *)"")
	{
		args->ori_backupName = strdup(m_ori_backupName.c_str());
	}
	else
	{
		args->ori_backupName = strdup((char *)"");
	}
	args->utilities = this->utilities;
	
	if (this->reportProgressDn != (char *)"")
	{
		args->reportProgressDn = strdup(this->reportProgressDn.c_str());
	}
	else
	{
		args->reportProgressDn = strdup((char *)"");
	}

	
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "Starting Thread Creation");
	pthread_t ptid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int err = pthread_create(&ptid, &attr, &generateBackupandExport, (void *)args);
	if (err != 0)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Thread Creation Failed");
		return RTN_FAIL;
	}
	setPthreadId(ptid);

	ACS_XBRM_LOG(LOG_LEVEL_INFO, "Thread Created");
	void *res;
	pthread_join(ptid, &res);

	if(res == PTHREAD_CANCELED){
		ACS_XBRM_LOG(LOG_LEVEL_WARN, "Pthread cancelled due to shutdown signal");
		return RTN_FAIL;
	}
	
	ACS_XBRM_LOG(LOG_LEVEL_INFO,"BackupName after creation : %s",args->ori_backupName);
	m_ori_backupName = (char*)args->ori_backupName;
	m_returnError = (char*)args->returnError;
	
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "After Thread closed");
	

	if(m_returnError != "")
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed while executing APG Backup creation, Error : %s", m_returnError.c_str());		
		return RTN_FAIL;		
	}
	
		
	return RTN_OK;
}

void *generateBackupandExport(void *arguments)
{

	ACS_XBRM_TRACE_FUNCTION;
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "generateBackup");
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "systemBackupType = %d", systemBackupType);
	bool doforever = true;
	struct arg_struct *args = (struct arg_struct *)arguments;
	string message = "";
	int intVal = 3;
	void *values[1];
	ACS_CC_ImmParameter reportProgress;
	reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str());
	try
	{
		do
		{
			string result = "";
			string brmDir = "/data/opt/ap/internal_root/backup_restore/";
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "m_transferQueue = %s", args->transferQueue);
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "m_labelName = %s", args->labelName);
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "m_tasktartTime = %s", args->tasktartTime);				

			// Once Parameters are read , execute the burbackup command with -i option ( -i is only for internal use)
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "Going to execute burbackup command");
			// burbackup command execution to generate AP backup
			bool toExec = true;
			int count = 0;
			while (toExec)
			{
				// Update the ProgreeReport attr

				string progressInfo = "APG System Backup Started";
				values[0] = reinterpret_cast<void*>(const_cast<char *>(progressInfo.c_str()));
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);
				
				string additionalInfo = "APG System Backup Started";
				values[0] = reinterpret_cast<void*>(const_cast<char *>(additionalInfo.c_str()));
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

				string timeActionStarted = ACS_XBRM_Utilities::getCurrentTime();
				values[0] = reinterpret_cast<void*>(const_cast<char *>(timeActionStarted.c_str()));
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
 
				int ret = do_command(args);
				if (ret != RTN_OK)
				{
					if (ret == ONGOINGBACKUP || ret == INTERNALFAULT)
					{
						count++;
						if (count > 2)
						{
							toExec = false;
							ACS_XBRM_LOG(LOG_LEVEL_ERROR, "APG Backup creation failed after maximum retries.");
							
							intVal = ACS_XBRM_UTILITY::CANCELLED;
							values[0] = reinterpret_cast<void *>(&intVal);
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);
							
							intVal = ACS_XBRM_UTILITY::FAILURE;
							values[0] = reinterpret_cast<void *>(&intVal);
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);



							intVal = 100;
							values[0] = reinterpret_cast<void *>(&intVal);
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);
							
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);
							
							string resultInfo;
							if(ret == ONGOINGBACKUP){
								resultInfo = "APG System Backup Creation Failed. Another backup or restore operation is in progress.";
							}else{
								resultInfo = "APG System Backup Creation Failed. Internal program fault.";
							}
							values[0] = reinterpret_cast<void*>(const_cast<char *>(resultInfo.c_str()));
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);
							
							args->utilities->immHandler.getAttribute(const_cast<char *>(args->reportProgressDn), &reportProgress);
							int len = reportProgress.attrValuesNum + 1;
							void *info[len];
							for (int i = 0; i < len - 1; i++)
								info[i] = reportProgress.attrValues[i];
							info[len - 1] = reinterpret_cast<void *>((char *)"APG System Backup Failed");
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

							string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
							values[0] = reinterpret_cast<void*>(const_cast<char *>(timeActionCompleted.c_str()));
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);
							
							args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
							
							pthread_exit(NULL);
						}else{
							ACS_XBRM_LOG(LOG_LEVEL_WARN,"Backup Creation Failed. Sleeping for 2 min before retry");
							ACE_OS::sleep(120);
						}
					}
					else
					{
						toExec = false;
						ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute burbackup command");
						
						pthread_exit(NULL);
					}
				}
				else
				{
					toExec = false;
				}
			}

			if(systemBackupType == ACS_XBRM_UTILITY::DOMAIN_TYPE){
				intVal = 75;
				values[0] = reinterpret_cast<void *>(&intVal);
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);
			}
			string progressInfo = "APG System Backup Created";
			values[0] = reinterpret_cast<void *>(const_cast<char *>(progressInfo.c_str()));
			args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);
			
			args->utilities->immHandler.getAttribute(const_cast<char *>(args->reportProgressDn), &reportProgress);
			int len = reportProgress.attrValuesNum + 1;
			void *info[len];
			for (int i = 0; i < len - 1; i++)
				info[i] = reportProgress.attrValues[i];
			info[len - 1] = reinterpret_cast<void *>((char *)"APG System Backup Completed");
			args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

			string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
			values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
			
			args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
			
						

			doforever = false;

		} while (doforever);

	} // end try
	catch (exception &e)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Exception was raised.");
		string res = "Internal program fault:";
		checkForErrors(&res, args);
		pthread_exit(NULL);
	}
	return (void *)0;
}

int do_command(void *arguments)
{
	struct arg_struct *args = (struct arg_struct *)arguments;

	int fd1[2];
	int fd2[2], procSTatus = 0;
	pid_t pid;
	string msg = "";
	if (args->isSecured)
	{
		msg = "Creation of Secure APG System Backup :";
	}
	else
	{
		msg = "Creation of APG System Backup : ";
	}

	if ((pipe(fd1) < 0) || (pipe(fd2) < 0))
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "PIPE ERROR");
	}
	if ((pid = fork()) < 0)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "FORK ERROR");
	}
	else if (pid == 0) // CHILD PROCESS
	{
		close(fd1[1]);
		close(fd2[0]);

		if (fd1[0] != STDIN_FILENO)
		{
			if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
			{

				ACS_XBRM_LOG(LOG_LEVEL_ERROR, "dup2 error to stdin");
			}
			close(fd1[0]);
		}

		if (fd2[1] != STDOUT_FILENO)
		{
			if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
			{
				ACS_XBRM_LOG(LOG_LEVEL_ERROR, "dup2 error to stdout");
			}
			close(fd2[1]);
		}
		if (true == args->isSecured)
		{
			const char *s = (args->tasktartTime);
			const char *l = args->labelName;
			execl("/opt/ap/acs/bin/burbackup", "/opt/ap/acs/bin/burbackup", "-r", s, "-p", "-l", l, "-f", static_cast<char *>(NULL));
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "is_Secureed");
		}
		else
		{
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "is_Secured is false");
			const char *s = args->tasktartTime;
			execl("/opt/ap/acs/bin/burbackup", "/opt/ap/acs/bin/burbackup", "-r", s, "-f", static_cast<char *>(NULL));
		}
	}
	else // PARENT PROCESS
	{
		close(fd1[0]);
		close(fd2[1]);
		char *input = (char *)args->backupPasswd;
		int len = 0;
		for (len = 0; input[len] != '\0'; len++)
			;
		write(fd1[1], input, len);
		write(fd1[1], "\n", 1);
		write(fd1[1], input, len);
		write(fd1[1], "\n", 1);

		char buffer[256];
		string result = "";
		while (read(fd2[0], buffer, 255) != 0)
		{
			result = result + buffer;

			int intVal = 0;
			void* values[1];

			if (result.find("75%") != std::string::npos && systemBackupType == ACS_XBRM_UTILITY::DOMAIN_TYPE)
			{
				intVal = 75;
				values[0] = reinterpret_cast<void *>(&intVal);
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

				string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
				values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);

																									 
			}
			else if (result.find("50%") != std::string::npos && systemBackupType == ACS_XBRM_UTILITY::DOMAIN_TYPE)
			{
				intVal = 50;
				values[0] = reinterpret_cast<void *>(&intVal);
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

				string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
				values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
			}
			else if (result.find("25%") != std::string::npos && systemBackupType == ACS_XBRM_UTILITY::DOMAIN_TYPE)
			{
				intVal = 25;
				values[0] = reinterpret_cast<void *>(&intVal);
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

				string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
				values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
				args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
											   
																									 
			}
			memset(buffer, '\0', sizeof(buffer));
		}
		ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", result.c_str());
		ACS_XBRM_LOG(LOG_LEVEL_INFO,"Before child thread wait");
		ACE_OS::waitpid(pid,&procSTatus, 0);
		ACS_XBRM_LOG(LOG_LEVEL_INFO,"After child thread wait");
		int eCode = checkForErrors(&result, args);
		if (eCode != RTN_OK)
		{
			cout << result << endl;
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "burbackup command execution failed");
			close(fd1[0]);
			close(fd1[1]);
			close(fd2[0]);
			close(fd2[1]);

			return eCode;
		}
		if (getBackupName(&result, args) != RTN_OK)
		{
			ACS_XBRM_LOG(LOG_LEVEL_INFO, "Failed while getting the backup name");
			close(fd1[0]);
			close(fd1[1]);
			close(fd2[0]);
			close(fd2[1]);

			return RTN_FAIL;
		}
	}
	close(fd1[0]);
	close(fd1[1]);
	close(fd2[0]);
	close(fd2[1]);
	return RTN_OK;
}

int checkForErrors(string *result, void *arguments)
{
	struct arg_struct *args = (struct arg_struct *)arguments;
	string errStr = "Creation of APG system backup failed: ";
	string error_string = "";
	string result1 = *result;
	bool backupFailed = false;

	string succ_string = "Complete system backup execution completed";
	int pos_start = result1.find(succ_string);
	
	string sec_succ_string = "Complete secured system backup execution completed";
	int pos_start1 = result1.find(sec_succ_string);

	if (pos_start != -1 || pos_start1 != -1)
	{
		ACS_XBRM_LOG(LOG_LEVEL_INFO, "burbackup command execution successfull");
		strcpy(args->returnError, "");
		return RTN_OK;
	}else{
		backupFailed = true;
	}

	error_string = "Backup creation aborted";
	pos_start = result1.find(error_string);
	int intVal = 0;
	void* values[1];
	ACS_CC_ImmParameter reportProgress;
	reportProgress.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str());

	if (pos_start != -1)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "burbackup command failed with %s", error_string.c_str());
		string resultInfo = "APG System Backup Creation Failed. Backup creation aborted.";
		/*
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

		intVal = ACS_XBRM_UTILITY::CANCELLED;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = ACS_XBRM_UTILITY::FAILURE;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = 100;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

		values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);
		args->utilities->immHandler.getAttribute(const_cast<char *>(args->reportProgressDn), &reportProgress);
		int len = reportProgress.attrValuesNum + 1;
		void *info[len];
		for (int i = 0; i < len - 1; i++)
			info[i] = reportProgress.attrValues[i];
		info[len - 1] = reinterpret_cast<void *>((char *)"APG System Backup Failed");
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

		string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
		values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
		*/
		args->returnError = strdup(resultInfo.c_str());
		return INTERNALFAULT;
	}

	error_string = "Backup creation failed";
	pos_start = result1.find(error_string);
	if (pos_start != -1)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "burbackup command failed with %s", error_string.c_str());
		string resultInfo = "APG System Backup Creation Failed.";
		/*
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);
		
		intVal = ACS_XBRM_UTILITY::CANCELLED;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = ACS_XBRM_UTILITY::FAILURE;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = 100;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

		values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->immHandler.getAttribute(const_cast<char *>(args->reportProgressDn), &reportProgress);
		int len = reportProgress.attrValuesNum + 1;
		void *info[len];
		for (int i = 0; i < len - 1; i++)
			info[i] = reportProgress.attrValues[i];
		info[len - 1] = reinterpret_cast<void *>((char *)"APG System Backup Failed");
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

		string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
		values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
		*/
		args->returnError = strdup(resultInfo.c_str());
		return INTERNALFAULT;
	}

	error_string = "Not enough space on data disk";
	pos_start = result1.find(error_string);
	if (pos_start != -1)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "burbackup command failed with %s", error_string.c_str());

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);
		
		intVal = ACS_XBRM_UTILITY::CANCELLED;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = ACS_XBRM_UTILITY::FAILURE;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = 100;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

		string resultInfo = "APG System Backup Creation Failed. Not enough space on data disk.";
		values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->immHandler.getAttribute(const_cast<char *>(args->reportProgressDn), &reportProgress);
		int len = reportProgress.attrValuesNum + 1;
		void *info[len];
		for (int i = 0; i < len - 1; i++)
			info[i] = reportProgress.attrValues[i];
		info[len - 1] = reinterpret_cast<void *>((char *)"APG System Backup Failed");
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

		string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
		values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
		args->returnError = strdup(resultInfo.c_str());
		return SPACE_ISSUE;
	}

	error_string = "Error when executing (general fault)";
	pos_start = result1.find(error_string);
	if (pos_start != -1)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "burbackup command failed with %s", error_string.c_str());
		string resultInfo = "APG System Backup Creation Failed. Error when executing (general fault).";
		/*
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);
		
		intVal = ACS_XBRM_UTILITY::CANCELLED;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = ACS_XBRM_UTILITY::FAILURE;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = 100;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

		values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->immHandler.getAttribute(const_cast<char *>(args->reportProgressDn), &reportProgress);
		int len = reportProgress.attrValuesNum + 1;
		void *info[len];
		for (int i = 0; i < len - 1; i++)
			info[i] = reportProgress.attrValues[i];
		info[len - 1] = reinterpret_cast<void *>((char *)"APG System Backup Failed");
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

		string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
		values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
		*/
		args->returnError = strdup(resultInfo.c_str());
		return INTERNALFAULT;
	}

	error_string = "Command not executed, backup or restore operation ongoing";
	pos_start = result1.find(error_string);
	if (pos_start != -1)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "burbackup command failed with %s", error_string.c_str());
		
		args->returnError = strdup(error_string.c_str());
		return ONGOINGBACKUP;
	}

	error_string = "Internal program fault:";
	pos_start = result1.find(error_string);
	if (pos_start != -1 || backupFailed)
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "burbackup command failed with %s", error_string.c_str());
		string resultInfo = "APG System Backup Creation Failed. Internal program fault.";
		/*
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);
		
		intVal = ACS_XBRM_UTILITY::CANCELLED;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STATE_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = ACS_XBRM_UTILITY::FAILURE;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_NAME.c_str()), ATTR_INT32T, 1, values);

		intVal = 100;
		values[0] = reinterpret_cast<void *>(&intVal);
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_PERCENTAGE_NAME.c_str()), ATTR_UINT32T, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_PROGRESS_INFO_NAME.c_str()), ATTR_STRINGT, 0, 0);

		values[0] = reinterpret_cast<void *>(const_cast<char *>(resultInfo.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESULT_INFO_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->immHandler.getAttribute(const_cast<char *>(args->reportProgressDn), &reportProgress);
		int len = reportProgress.attrValuesNum + 1;
		void *info[len];
		for (int i = 0; i < len - 1; i++)
			info[i] = reportProgress.attrValues[i];
		info[len - 1] = reinterpret_cast<void *>((char *)"APG System Backup Failed");
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_ADDITIONAL_INFO_NAME.c_str()), ATTR_STRINGT, len, info);

		string timeActionCompleted = ACS_XBRM_Utilities::getCurrentTime();
		values[0] = reinterpret_cast<void *>(const_cast<char *>(timeActionCompleted.c_str()));
		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_ACTION_COMPLETED_NAME.c_str()), ATTR_STRINGT, 1, values);

		args->utilities->updateReportProgressAttribute(const_cast<char *>(args->reportProgressDn), const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TIME_OF_LAST_STATUS_UPDATE_NAME.c_str()), ATTR_STRINGT, 1, values);
		*/
		args->returnError = strdup(resultInfo.c_str());
		return INTERNALFAULT;
	}

	strcpy(args->returnError, "");
	return RTN_OK;
}
/*
 *This method extracts the backupname from burbackup command output
 */
int getBackupName(string *result, void *arguments)
{

	ACS_XBRM_TRACE_FUNCTION;
	ACS_XBRM_LOG(LOG_LEVEL_INFO, "getBackupName");
	struct arg_struct *args = (struct arg_struct *)arguments;
	string substr = "Imagename:";
	string result1 = *result;
	cout<<"AP Backup result : "<<result1<<endl;
	int pos_start = result1.find(substr);
	pos_start = pos_start + substr.length() + 1;
	cout<<"pos_start : "<<pos_start<<endl;
	int pos_end = result1.find("Creation");
	cout<<"pos_end : "<<pos_end<<endl;
	string backupName  = result1.substr(pos_start, (pos_end - pos_start));
	ACS_XBRM_LOG(LOG_LEVEL_ERROR, "backupName %s", backupName.c_str());
	size_t i = 0;
	while (i < backupName.length())
	{
		i = backupName.find('\n');
		if (i != std::string::npos)
		{
			backupName.erase(i);
		}
		else
			break;
	}
	args->ori_backupName = strdup(backupName.c_str());
	ACS_XBRM_LOG(LOG_LEVEL_ERROR, "m_ori_backupName %s", args->ori_backupName);
	return RTN_OK;
}
