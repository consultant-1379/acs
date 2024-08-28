/*=================================================================== */
/**
  @file acs_alog_utility.cpp

  Class method implementation for acs_alog_utility type module.

  This module contains the implementation of class declared in
  the acs_alog_utility.h module

  @version 2.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       02/02/2011     xgencol/xgaeerr       Initial Release
  N/A       28/11/2011     xgencol               Final Release
  N/A       07/06/2019     xmanpas               As part of ANSSI MMF4 changed permissions of folder /data/opt/ap/internal_root/audit_logs/ to 755
  =================================================================== */

#include <acs_alog_utility.h>
#include <acs_alog_types.h>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <errno.h>
#include <sys/stat.h>
#include <sys/quota.h>
#include <mntent.h>
#include <fstream>
#include <iostream>
#define QUOTABLOCK_BITS  10
#define QUOTABLOCK_SIZE  (1 << QUOTABLOCK_BITS)

/* Conversion routines from and to quota blocks */
#define toqb(x)  (((x) + QUOTABLOCK_SIZE - 1) >> QUOTABLOCK_BITS)

bool		   DirCreated;
char           buf[BUFSIZ];
struct mntent  Mnt;
struct stat    InfoPathAudit;

char  AbsAuditPath[300];				//  contain the /data path for audit_logs or for protected_logs

extern string  CurrentLogFile;
extern string  CurrentPLogFile;

extern string  CurrentNodeState;
extern string  FilePName;
extern char    FileAudit[];
extern char    PathAudit[];
extern char    PathPAudit[];
extern char    PathCLog[];
extern string  TQdn;
extern string  commandLogTQdn;
extern int     APtype;          //  AP1 = 1   AP2 = 2
extern bool    MCpSystem;
extern string  FilePAudit;
extern string  clusterLogsTQFolder;

extern char   TQdefinedError;
extern char   CommandLogTQdefinedError;
extern char   DeleteFilesError;
extern char   DelCLogFilesError;
extern char   DelPLogFilesError;
extern char   SendFilesError;
extern char   SendPLFilesError;
extern char   SendCLOGFilesError;
extern char   AttachTQerror;
extern char   AttachPLTQerror;
extern char   AttachCLOGTQerror;

extern bool   CurrLogGreaterThanOneMB;
extern bool   CurrPLogGreaterThanOneMB;
extern bool flagTemplateFormatChangePending;
extern int    FileNamingFormat;

const char LastCLogFileSent_Name[] = "LastCLogFileSent.txt";
const char MissingCLogFileSent_Name[] = "MissingCLogFileSent.txt";
const char DefaultCLOGFileName[] = "CLogfile-1970-01-01-0000";

void  stringToUpper (string &str);
void  stringToLower (string &str);
string  getLastWrittenLogFile (DIR *handle);
string  getLastWrittenPLogFile (DIR *handle);

ACE_Recursive_Thread_Mutex CLOG_Filehandling_Sync;
ACE_Recursive_Thread_Mutex templateFormatPendingMutex;

using namespace std;

void  getDateTimeFromFname (const char *fName, char fdate[])			//  return in 'fdate' the Time Stamp in the format  YYYYMMDDhhmm
{

		string  fName_str(fName);
		size_t  found = fName_str.find_first_of("_.-");
		string  fdate_str, tempstr;

		tempstr = fName_str.substr(found+1);

		while ((found = tempstr.find_first_of("_-.")) != string::npos  &&  found < tempstr.length()-1)
		{
				fdate_str.append (tempstr.c_str(),found);
				tempstr = tempstr.substr (found+1);
		}

		if ((tempstr.compare("zip") || tempstr.compare("7z")) != 0)  {
			fdate_str.append(tempstr.c_str());
			strncpy (fdate, fdate_str.c_str(), FILES_TIMESTAMP_LEN);
		}


}

bool  compareFilename (string fName1, string fName2)   				//  return TRUE if fName1 Time Stamp is less than fName2 one
{

		char  fdate1[FILES_TIMESTAMP_LEN+1]= {0}, fdate2[FILES_TIMESTAMP_LEN+1]= {0};

		getDateTimeFromFname (fName1.c_str(), fdate1);
		getDateTimeFromFname (fName2.c_str(), fdate2);

		int  ret;
		if ((ret = strcmp (fdate1, fdate2)) != 0)  return (ret < 0);

		struct stat  status1, status2;						//  When the Time Stamps are equal, the last modification times are evaluated
		string       pathAbs;								//
															//  This situation is verified when two of the files audit.... and LOGFILE-...
		pathAbs.assign (AbsAuditPath);						//  have the same year, the same month, the same day, the same hour and the same minute
		pathAbs = pathAbs + "/" + fName1;
		stat (pathAbs.c_str(), &status1);

		pathAbs.assign (AbsAuditPath);
		pathAbs = pathAbs + "/" + fName2;
		stat (pathAbs.c_str(), &status2);

		return (status1.st_mtim.tv_sec < status2.st_mtim.tv_sec) ? true : false;
}

int  getTransferQueueDn (string TQ)
{
	int   ret = 0;
	unsigned int  errorCode = AES_OHI_ERRORUNKNOWN;
	AES_OHI_ExtFileHandler2  *extFileH;

	extFileH = new AES_OHI_ExtFileHandler2 ("ACS", "ALOG");

	errorCode = extFileH->attach();
	if (errorCode == AES_OHI_NOERRORCODE)
	{
			errorCode = extFileH->fileTransferQueueDefined (TQ.c_str(), TQdn);

			if (errorCode == AES_OHI_NOERRORCODE)
				ret = 1;

			extFileH->detach();
	}

	delete  extFileH;
	return  ret;
}

int  getcommandLogTransferQueueDn (string commandLogTQ)
{
	int   ret = 0;
	unsigned int  errorCode = AES_OHI_ERRORUNKNOWN;
	AES_OHI_ExtFileHandler2  *extFileH;

	extFileH = new AES_OHI_ExtFileHandler2 ("ACS", "ALOG");

	errorCode = extFileH->attach();
	if (errorCode == AES_OHI_NOERRORCODE)
	{
			errorCode = extFileH->fileTransferQueueDefined (commandLogTQ.c_str(), commandLogTQdn);

			if (errorCode == AES_OHI_NOERRORCODE)
				ret = 1;

			extFileH->detach();
	}

	delete  extFileH;
	return  ret;
}

int  validateTransferQueue (string TQ, bool ClusteLog){

	char  fileAlrm[200] = {0};
	int   counter = 0, ret = 0;
	unsigned int  errorCode = AES_OHI_ERRORUNKNOWN;
	AES_OHI_ExtFileHandler2  *extFileH;

	strcpy (fileAlrm, PathAudit);

	if ( ClusteLog )
		strcat (fileAlrm, "/ClogTQdefined.txt");
	else
		strcat (fileAlrm, "/TQdefined.txt");

	bool TQalreadyDefined = false;
	bool CLOG_TQalreadyDefined = false;

	FILE  *fp = ::fopen(fileAlrm, "r");
	if (fp != NULL){
		::fclose (fp);
		if ( ClusteLog )
			CLOG_TQalreadyDefined = true;
		else
			TQalreadyDefined = true;
	}

	if (TQ.size() == 0){
		if (TQdefinedError == 0  &&  TQalreadyDefined == true){
				AEHEVENTMESSAGE ("acs_alogmaind", 9911, "A2", "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm",
								 "Error. No transfer queue defined for audit log-files", "FCODE 9\nTransfer Queue not defined");
				TQdefinedError = 1;
		}
		else if (CommandLogTQdefinedError == 0  &&  CLOG_TQalreadyDefined == true){
				AEHEVENTMESSAGE ("acs_alogmaind", 9901, "A2", "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm",
								 "Error. No transfer queue defined for cluster command log-files", "FCODE 11\nTransfer Queue not defined");
				CommandLogTQdefinedError = 1;
		}

		return 1;
	}

	string strToLog = "Transfer Queue verify :  TQ_name = ";
	strToLog.append(TQ);
	ACSALOGLOG (LOG_LEVEL_INFO, TEXTERROR(strToLog));

	extFileH = new AES_OHI_ExtFileHandler2 ("ACS", "ALOG");

	while (counter < 5  &&  errorCode != AES_OHI_NOERRORCODE){
		errorCode = extFileH->attach();
		if (errorCode != AES_OHI_NOERRORCODE)
			sleep (1);

		counter ++;
	}

	if (errorCode == AES_OHI_NOERRORCODE){

		if (ClusteLog)
			errorCode = extFileH->fileTransferQueueDefined (TQ.c_str(), commandLogTQdn);
		else
			errorCode = extFileH->fileTransferQueueDefined (TQ.c_str(), TQdn);

		if (errorCode == AES_OHI_NOERRORCODE){
			if (TQdefinedError == 1){
				AEHEVENTMESSAGE ("acs_alogmaind", 9911, "CEASING", "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
				TQdefinedError = 0;
			}
			else if (CommandLogTQdefinedError == 1){
				AEHEVENTMESSAGE ("acs_alogmaind", 9901, "CEASING", "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
				CommandLogTQdefinedError = 0;
			}

			if ( TQalreadyDefined == false ){
				FILE  *fp = ::fopen(fileAlrm, "w");
				if (fp != NULL){
					::fputs ("OK\n", fp);
					::fflush (fp);
					::fclose (fp);
				}
			}
			ret = 1;
		}
		else {
			if (TQdefinedError == 0  &&  TQalreadyDefined == true){
					AEHEVENTMESSAGE ("acs_alogmaind", 9911, "A2", "AUDIT LOG FAULT",
									 "APZ", "acs_alogmaind/Alarm", "Error. No transfer queue defined for audit log-files", "FCODE 9\nTransfer Queue not defined");
					TQdefinedError = 1;
			}
			if (CommandLogTQdefinedError == 0  &&  CLOG_TQalreadyDefined == true){
					AEHEVENTMESSAGE ("acs_alogmaind", 9901, "A2", "AUDIT LOG FAULT",
									 "APZ", "acs_alogmaind/Alarm", "Error. No transfer queue defined for cluster command log-files", "FCODE 11\nTransfer Queue not defined");
					TQdefinedError = 1;
			}

			ACSALOGLOG (LOG_LEVEL_ERROR, TEXTERROR("Transfer Queue verify :  fileTransferQueueDefined error !"));
		}

		extFileH->detach();
	}
	else {
			ACSALOGLOG (LOG_LEVEL_ERROR, TEXTERROR("Transfer Queue verify :  attach error !"));
	}

	delete  extFileH;
	return  ret;
}

void  sendOneFileToGOH (string auditDir, string TQ, string fileToSend)
{

	AES_OHI_FileHandler  *fileHandler;
	unsigned int  errorCode;

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", TQ.c_str(), "Transfer Queue handling", auditDir.c_str());

	if (fileHandler == NULL){
		AEHEVENTMESSAGE ("acs_alogmaind", 9950, "EVENT", "The Transfer Queue is unavailable",
						 "EVENT", "APZ", "Null Pointer to the Transfer Queue",
						 "The Transfer Queue is unavailable");
		return;
	}

	errorCode = fileHandler->attach();

	if (errorCode == AES_OHI_NOERRORCODE){
		if (AttachTQerror == 1){
			AEHEVENTMESSAGE ("acs_alogmaind", 9912, "CEASING", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
			AttachTQerror = 0;
		}

		AES_OHI_Filestates  status;
		bool  filesTransferred = false;
		bool  errorInTransfer  = false;

		string  xxx ("Send to GOH ... File = " + fileToSend);

		char  error[10];

		errorCode = fileHandler->getTransferState (fileToSend.c_str(), status);

		if (status == AES_OHI_FSREADY){

			filesTransferred = true;
			errorCode = fileHandler->send (fileToSend.c_str());

					if (errorCode != AES_OHI_NOERRORCODE   &&
						errorCode != AES_OHI_FILENOTFOUND  &&
						errorCode != AES_OHI_SENDITEMEXIST) 	errorInTransfer = true;

sprintf (error, "   %03d", errorCode);
xxx.append(error);  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(xxx.c_str()));
			}

			if (filesTransferred == true  &&  errorInTransfer == true)
			{
					if (SendFilesError == 0)
					{
							char  textMsg[200];
							strcpy (textMsg, "AUDIT LOG, FAULT CODE 8\n-\n");
							strcat (textMsg, TQ.c_str());
							strcat (textMsg, "\n-\n-\n");

							AEHEVENTMESSAGE ("acs_alogmaind", 9913, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
										 	 "APZ", "acs_alogmaind/Alarm", "Error when reporting a log file to file transfer server", textMsg);
							SendFilesError = 1;
					}
			}

			if (filesTransferred == true  &&  errorInTransfer == false)
			{
					if (SendFilesError == 1)
					{
							AEHEVENTMESSAGE ("acs_alogmaind", 9913, "CEASING",
										 	 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
							SendFilesError = 0;
					}
			}

			errorCode = fileHandler->detach();
			if (errorCode != AES_OHI_NOERRORCODE)  syslog (LOG_ERR, "Transfer Queue send to GOH :  detach error [%d]", errorCode);
	}
	else {
			if (AttachTQerror == 0)
			{
					char  textMsg[200];
					strcpy (textMsg, "AUDIT LOG, FAULT CODE 7\n-\n");
					strcat (textMsg, TQ.c_str());
					strcat (textMsg, "\n-\n-\n");

					AEHEVENTMESSAGE ("acs_alogmaind", 9912, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
									 "APZ", "acs_alogmaind/Alarm", "Error when connecting to file transfer server for log transfer queue", textMsg);
					AttachTQerror = 1;
			}
	}

	delete  fileHandler;
}

void  sendOnePlogFileToGOH (string auditDir, string TQ, string fileToSend)
{

	AES_OHI_FileHandler  *fileHandler;
	unsigned int  errorCode;

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", TQ.c_str(), "Transfer Queue handling", auditDir.c_str());

	if (fileHandler == NULL)
	{
		AEHEVENTMESSAGE ("acs_alogmaind", 9950, "EVENT", "The Transfer Queue is unavailable",
						 "EVENT", "APZ", "Null Pointer to the Transfer Queue", "The Transfer Queue is unavailable");
		return;
	}

	errorCode = fileHandler->attach();
	if (errorCode == AES_OHI_NOERRORCODE)
	{
			if (AttachPLTQerror == 1)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9927, "CEASING",
								 	 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
					AttachPLTQerror = 0;
			}

			AES_OHI_Filestates  status;
			bool  filesTransferred = false;
			bool  errorInTransfer  = false;

string  xxx;
char  error[10];
xxx = "Send to GOH ... File = ";
xxx.append(fileToSend.c_str());

			errorCode = fileHandler->getTransferState (fileToSend.c_str(), status);

			if (status == AES_OHI_FSREADY)
			{
					filesTransferred = true;

					errorCode = fileHandler->send (fileToSend.c_str());

					if (errorCode != AES_OHI_NOERRORCODE   &&
						errorCode != AES_OHI_FILENOTFOUND  &&
						errorCode != AES_OHI_SENDITEMEXIST)    errorInTransfer = true;

sprintf (error, "   %03d", errorCode);
xxx.append(error);  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(xxx.c_str()));
			}

			if (filesTransferred == true  &&  errorInTransfer == true)
			{
				if (SendPLFilesError == 0)
				{
						AEHEVENTMESSAGE ("acs_alogmaind", 9928, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
									 	 "APZ", "acs_alogmain/Alarm", "Error when reporting a protected log file to file transfer server", "FCODE 18\nError in transferring PL files to the TQ");
						SendPLFilesError = 1;
				}
			}

			if (filesTransferred == true  &&  errorInTransfer == false)
			{
				if (SendPLFilesError == 1)
				{
						AEHEVENTMESSAGE ("acs_alogmaind", 9928, "CEASING",
									 	 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
						SendPLFilesError = 0;
				}
			}

			errorCode = fileHandler->detach();
			if (errorCode != AES_OHI_NOERRORCODE)  syslog (LOG_ERR, "Transfer Queue send to GOH :  detach error [%d]", errorCode);
	}
	else {
			if (AttachPLTQerror == 0)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9927, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
									 "APZ", "acs_alogmain/Alarm", "Error when connecting to file transfer server for protected log transfer queue", "FCODE 7\nError when connecting to file transfer server for protected log transfer queue");
					AttachPLTQerror = 1;
			}
	}

	delete  fileHandler;
}

void  sendFilesToGOH (string auditDir, string TQ)
{

	DIR  *handle;
	struct dirent  *entry;

	std::list<string> auditFiles;
	std::list<std::string>::iterator  files;

	string  fName, fileToSend;
	AES_OHI_FileHandler  *fileHandler;
	unsigned int  errorCode;

	auditFiles.clear();

	if ((handle = opendir (auditDir.c_str())) == NULL)  return;

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;

			if (fName.substr(0,8).compare("Logfile-") == 0)  auditFiles.push_back(fName);
			else if (fName.substr(0,6).compare("audit.") == 0  &&  fName.compare(CurrentLogFile) != 0)  auditFiles.push_back(fName);
			else if (fName.substr(0,8).compare("LOGFILE-") == 0  &&  fName.compare(CurrentLogFile) != 0)  auditFiles.push_back(fName);
	}
	closedir (handle);

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", TQ.c_str(), "Transfer Queue handling", auditDir.c_str());

	if (fileHandler == NULL)
	{
				AEHEVENTMESSAGE ("acs_alogmaind", 9950, "EVENT", "The Transfer Queue is unavailable",
								 "EVENT", "APZ", "Null Pointer to the Transfer Queue",
								 "The Transfer Queue is unavailable");
				return;
	}

	errorCode = fileHandler->attach();

	if (errorCode == AES_OHI_NOERRORCODE)
	{
		if (AttachTQerror == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9912, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
				AttachTQerror = 0;
		}

    	AES_OHI_Filestates  status;
string  xxx;
char  error[10];
		bool  filesTransferred = false;
		bool  errorInTransfer  = false;

		for (files = auditFiles.begin(); files != auditFiles.end(); files++)
		{
				fileToSend = *(files);

xxx = "Send to GOH ... File = ";
xxx.append(fileToSend.c_str());

				errorCode = fileHandler->getTransferState (fileToSend.c_str(), status);

				if (status == AES_OHI_FSREADY)
				{
//  the fileHandler->isConnected() API should check the connection verify :  but it returns
//  always true, also when the COM connectionVerify command printouts false
						filesTransferred = true;
						errorCode = fileHandler->send (fileToSend.c_str());

						if (errorCode != AES_OHI_NOERRORCODE   &&
							errorCode != AES_OHI_FILENOTFOUND  &&
							errorCode != AES_OHI_SENDITEMEXIST) 	errorInTransfer = true;

sprintf (error, "   %03d", errorCode);
xxx.append(error);  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(xxx.c_str()));
				}
		}

		if (filesTransferred == true  &&  errorInTransfer == true)
		{
				if (SendFilesError == 0)
				{
						char  textMsg[200];
						strcpy (textMsg, "AUDIT LOG, FAULT CODE 8\n-\n");
						strcat (textMsg, TQ.c_str());
						strcat (textMsg, "\n-\n-\n");

						AEHEVENTMESSAGE ("acs_alogmaind", 9913, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
										 "APZ", "acs_alogmaind/Alarm", "Error when reporting a log file to file transfer server", textMsg);
						SendFilesError = 1;
				}
		}

		if (filesTransferred == true  &&  errorInTransfer == false)
		{
				if (SendFilesError == 1)
				{
						AEHEVENTMESSAGE ("acs_alogmaind", 9913, "CEASING",
										 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
						SendFilesError = 0;
				}
		}

		errorCode = fileHandler->detach();
		if (errorCode != AES_OHI_NOERRORCODE)  syslog (LOG_ERR, "Transfer Queue send to GOH :  detach error [%d]", errorCode);
	}
	else {
			if (AttachTQerror == 0)
			{
					char  textMsg[200];
					strcpy (textMsg, "AUDIT LOG, FAULT CODE 7\n-\n");
					strcat (textMsg, TQ.c_str());
					strcat (textMsg, "\n-\n-\n");

					AEHEVENTMESSAGE ("acs_alogmaind", 9912, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
									 "APZ", "acs_alogmaind/Alarm", "Error when connecting to file transfer server for log transfer queue", textMsg);
					AttachTQerror = 1;
			}
	}

	delete  fileHandler;
}

void  sendPlogFilesToGOH (string auditDir, string TQ)
{

	DIR  *handle;
	struct dirent  *entry;

	std::list<string> pauditFiles;
	std::list<std::string>::iterator  files;

	string  fName, fileToSend;
	AES_OHI_FileHandler  *fileHandler;
	unsigned int  errorCode;

	pauditFiles.clear();

	if ((handle = opendir (auditDir.c_str())) == NULL)  return;

	while ((entry = readdir(handle)) != NULL)
	{
		fName = entry->d_name;

		if (fName.substr(0,9).compare("PLogfile_") == 0)  pauditFiles.push_back(fName);
		else if (fName.substr(0,7).compare("paudit.") == 0  &&  fName.compare(CurrentPLogFile) != 0)  pauditFiles.push_back(fName);
		else if (fName.substr(0,9).compare("PLOGFILE_") == 0  &&  fName.compare(CurrentPLogFile) != 0)  pauditFiles.push_back(fName);
	}
	closedir (handle);

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", TQ.c_str(), "Transfer Queue handling", auditDir.c_str());

	if (fileHandler == NULL)
	{
		AEHEVENTMESSAGE ("acs_alogmaind", 9950, "EVENT", "The Transfer Queue is unavailable",
						 "EVENT", "APZ", "Null Pointer to the Transfer Queue", "The Transfer Queue is unavailable");
		return;
	}

	errorCode = fileHandler->attach();
	if (errorCode == AES_OHI_NOERRORCODE)
	{
		if (AttachPLTQerror == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9927, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
				AttachPLTQerror = 0;
		}

string  xxx;
char  error[10];

		AES_OHI_Filestates  status;
		bool  filesTransferred = false;
		bool  errorInTransfer  = false;

		for (files = pauditFiles.begin(); files != pauditFiles.end(); files++)
		{
			fileToSend = *(files);

xxx = "Send to GOH ... File = ";
xxx.append(fileToSend.c_str());

			errorCode = fileHandler->getTransferState (fileToSend.c_str(), status);

			if (status == AES_OHI_FSREADY)
			{
					filesTransferred = true;

					errorCode = fileHandler->send (fileToSend.c_str());

					if (errorCode != AES_OHI_NOERRORCODE   &&
						errorCode != AES_OHI_FILENOTFOUND  &&
						errorCode != AES_OHI_SENDITEMEXIST)    errorInTransfer = true;

sprintf (error, "   %03d", errorCode);
xxx.append(error);  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(xxx.c_str()));
			}
		}

		if (filesTransferred == true  &&  errorInTransfer == true)
		{
			if (SendPLFilesError == 0)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9928, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
									 "APZ", "acs_alogmain/Alarm", "Error when reporting a protected log file to file transfer server", "FCODE 18\nError in transferring PL files to the TQ");
					SendPLFilesError = 1;
			}
		}

		if (filesTransferred == true  &&  errorInTransfer == false)
		{
			if (SendPLFilesError == 1)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9928, "CEASING",
									 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
					SendPLFilesError = 0;
			}
		}

		errorCode = fileHandler->detach();
		if (errorCode != AES_OHI_NOERRORCODE)  syslog (LOG_ERR, "Transfer Queue send to GOH :  detach error [%d]", errorCode);
	}
	else {
			if (AttachPLTQerror == 0)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9927, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT",
									 "APZ", "acs_alogmain/Alarm", "Error when connecting to file transfer server for protected log transfer queue", "FCODE 7\nError when connecting to file transfer server for protected log transfer queue");
					AttachPLTQerror = 1;
			}
	}

	delete  fileHandler;
}

void sendOneClusterLogFiletoGOH (std::string fileToSendToGOH, std::string commandLogTransferQueue, std::string commandLogTransferQueueFolder){

	AES_OHI_FileHandler *fileHandler;
	unsigned int returnCode;
	char strToLog[1024] = {0};

	FILE *fd_LastCLogFileSent = NULL;

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", commandLogTransferQueue.c_str(), "Transfer Queue handling", commandLogTransferQueueFolder.c_str());

	if (fileHandler == NULL){
		AEHEVENTMESSAGE ("acs_alogmaind", 9950, "EVENT", "The Transfer Queue is unavailable", "EVENT", "APZ", "Null Pointer to the Transfer Queue", "The Transfer Queue is unavailable");
		boost::filesystem::remove(std::string(commandLogTransferQueueFolder.append("/").append(fileToSendToGOH)));
		return;
	}

	char LastCLogFileSent[32] = {0};
	char LastCLogFileSentPath[512] = {0};
	snprintf(LastCLogFileSentPath,sizeof(LastCLogFileSentPath),"%s%s", PathCLog, LastCLogFileSent_Name);

	snprintf(strToLog,sizeof(strToLog),"sendOneClusterLogFiletoGOH - File : %s - TransferQueue : %s - TransferQueueFolder : %s", fileToSendToGOH.c_str(), commandLogTransferQueue.c_str(), commandLogTransferQueueFolder.c_str());
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

	returnCode = fileHandler->attach();

	if (returnCode == AES_OHI_NOERRORCODE){
		if (AttachCLOGTQerror == 1){
			AEHEVENTMESSAGE ("acs_alogmaind", 9915, "CEASING", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
			AttachCLOGTQerror = 0;
		}

		AES_OHI_Filestates  status;
		bool  filesTransferred = false;
		bool  errorInTransfer  = false;

		returnCode = fileHandler->getTransferState (fileToSendToGOH.c_str(), status);

		if (status == AES_OHI_FSREADY){

			filesTransferred = true;
			returnCode = fileHandler->send (fileToSendToGOH.c_str());

			ACE_Guard<ACE_Recursive_Thread_Mutex> guard_clog_file (CLOG_Filehandling_Sync);

			if (returnCode != AES_OHI_NOERRORCODE && returnCode != AES_OHI_FILENOTFOUND && returnCode != AES_OHI_SENDITEMEXIST){
				errorInTransfer = true;
				memset(strToLog,0,sizeof(strToLog));
				snprintf(strToLog,sizeof(strToLog),"Fail to send the file %s to GOH - Return code %i",fileToSendToGOH.c_str(),returnCode);
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
			}
			else {
				memset(strToLog,0,sizeof(strToLog));
				snprintf(strToLog,sizeof(strToLog),"The file %s has been successfully sent to GOH",fileToSendToGOH.c_str());
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

				snprintf(LastCLogFileSent,sizeof(LastCLogFileSent),"%s",fileToSendToGOH.c_str());

				if ( (fd_LastCLogFileSent = fopen(LastCLogFileSentPath,"w")) != 0){
					if ( fprintf(fd_LastCLogFileSent,"%s\n",LastCLogFileSent) == strlen(LastCLogFileSent)+1){
							memset(strToLog,0,sizeof(strToLog));
							snprintf(strToLog,sizeof(strToLog),"sendOneClusterLogFiletoGOH - The last CLOG file sent to GOH has been stored correctly");
							ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
					}
					else {
							memset(strToLog,0,sizeof(strToLog));
							snprintf(strToLog,sizeof(strToLog),"sendOneClusterLogFiletoGOH - Fail to store the last CLOG file sent to GOH");
							ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
					}
					fclose(fd_LastCLogFileSent);
				}
				else {
					memset(strToLog,0,sizeof(strToLog));
					snprintf(strToLog,sizeof(strToLog),"sendOneClusterLogFiletoGOH - Error when opening the file %s for writing", LastCLogFileSentPath);
					ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
				}
			}
		}

		if (filesTransferred == true  &&  errorInTransfer == true){ // TQ OK but error in transfer
			if (SendCLOGFilesError == 0){
				char  textMsg[200];
				strcpy (textMsg, "AUDIT LOG, FAULT CODE 8\n-\n");
				strcat (textMsg, commandLogTransferQueue.c_str());
				strcat (textMsg, "\n-\n-\n");

				AEHEVENTMESSAGE ("acs_alogmaind", 9916, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "Error when reporting a log file to file transfer server", textMsg);
				SendCLOGFilesError = 1;
			}
		}

		if (filesTransferred == true  &&  errorInTransfer == false){ // TQ OK and NO error in transfer
			if (SendCLOGFilesError == 1){ // Cease the alarm is raised
				AEHEVENTMESSAGE ("acs_alogmaind", 9916, "CEASING", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
				SendCLOGFilesError = 0;
			}
		}

		returnCode = fileHandler->detach();

		if (returnCode != AES_OHI_NOERRORCODE){
			memset(strToLog,0,sizeof(strToLog));
			snprintf(strToLog,sizeof(strToLog),"Transfer Queue send to GOH :  detach error [%d]",returnCode);
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
		}
	}
	else {
		memset(strToLog,0,sizeof(strToLog));
		snprintf(strToLog,sizeof(strToLog),"Transfer Queue send to GOH :  attach error [%d]",returnCode);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

		if (AttachCLOGTQerror == 0){
			char  textMsg[512] = {0};
			snprintf(textMsg,sizeof(textMsg),"AUDIT LOG, FAULT CODE 7\n-\n%s\n-\n-\n",commandLogTransferQueue.c_str());
			AEHEVENTMESSAGE ("acs_alogmaind", 9915, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "Error when connecting to file transfer server for log transfer queue", textMsg);
			AttachCLOGTQerror = 1;
		}
	}

	if (AttachCLOGTQerror || SendCLOGFilesError)
		boost::filesystem::remove(std::string(commandLogTransferQueueFolder.append("/").append(fileToSendToGOH)));

	delete  fileHandler;
}

void sendClusterLogFiletoGOH (std::string clusterLogsFolder, std::string commandLogTransferQueue){

	DIR  *handle;
	struct dirent *entry;

	FILE *fd_LastCLogFileSent, *fd_MissingCLogFileSent;

	char strToLog[1024] = {0};

	std::list<string> ClusterCLogFiles;
	std::list<string> ClusterCLogFiles_Not_Sent;
	std::list<std::string>::iterator files;

	std::string  fName, fileToSend;
	char CLogFileSent[32] = {0};
	char LastCLogFileSent[32] = {0};
	AES_OHI_FileHandler  *fileHandler;
	unsigned int returnCode;

	char LastCLogFileSentPath[512] = {0};
	char MissingCLogFileSentPath[512] = {0};

	snprintf(LastCLogFileSentPath,sizeof(LastCLogFileSentPath),"%s%s", PathCLog, LastCLogFileSent_Name);
	snprintf(MissingCLogFileSentPath,sizeof(MissingCLogFileSentPath),"%s%s", PathCLog, MissingCLogFileSent_Name);

	memset(strToLog,0,sizeof(strToLog));

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_clog_file (CLOG_Filehandling_Sync);

	if ( (fd_LastCLogFileSent = fopen(LastCLogFileSentPath,"r")) != 0){

		fgets(LastCLogFileSent, sizeof(LastCLogFileSent), fd_LastCLogFileSent);
		LastCLogFileSent[strlen(LastCLogFileSent)-1] = 0;
		fclose(fd_LastCLogFileSent);

		if ( strlen(LastCLogFileSent) != strlen(DefaultCLOGFileName))
			snprintf(LastCLogFileSent,sizeof(LastCLogFileSent),"%s",DefaultCLOGFileName);

		snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - The last CLOG file sent to GOH is : %s",LastCLogFileSent);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
	}
	else {
		snprintf(LastCLogFileSent,sizeof(LastCLogFileSent),"%s",DefaultCLOGFileName);
		snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - Error when opening the file %s for reading - Using default file name %s", LastCLogFileSentPath, LastCLogFileSent);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
	}

	if ( (fd_MissingCLogFileSent = fopen(MissingCLogFileSentPath,"r")) != 0){

		while ( fgets(CLogFileSent, sizeof(CLogFileSent), fd_MissingCLogFileSent) != NULL ){
			CLogFileSent[strlen(CLogFileSent)-1] = 0;
			ClusterCLogFiles_Not_Sent.push_back(CLogFileSent);
			memset(CLogFileSent,0,sizeof(CLogFileSent));
		}

		fclose(fd_MissingCLogFileSent);

		snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - The CLOG files to be sent to GOH are : ");
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

		for (files = ClusterCLogFiles_Not_Sent.begin(); files != ClusterCLogFiles_Not_Sent.end(); files++){
			snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - %s", (*files).c_str());
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
		}
	}
	else {
		snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - Error when opening the file %s for reading", MissingCLogFileSentPath);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
	}

	if ((handle = opendir (clusterLogsFolder.c_str())) == NULL)
		return;

	while ((entry = readdir(handle)) != NULL){
		if ( strcmp(entry->d_name,".") &&
			 strcmp(entry->d_name,"..") &&
			 strcmp(entry->d_name,"ClogFolderCreated.txt") &&
			 strcmp(entry->d_name,LastCLogFileSent_Name) &&
			 strcmp(entry->d_name,MissingCLogFileSent_Name))
			ClusterCLogFiles.push_back(entry->d_name);
	}

	ClusterCLogFiles.sort(compareFilename);

	if ( !ClusterCLogFiles.empty())
		ClusterCLogFiles.pop_back();

	closedir (handle);

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", commandLogTransferQueue.c_str(), "Transfer Queue handling", clusterLogsTQFolder.c_str());

	if (fileHandler == NULL){
		AEHEVENTMESSAGE ("acs_alogmaind", 9950, "EVENT", "The Transfer Queue is unavailable", "EVENT", "APZ", "Null Pointer to the Transfer Queue", "The Transfer Queue is unavailable");
		return;
	}

	memset(strToLog,0,sizeof(strToLog));
	snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - TransferQueue : %s - TransferQueueFolder : %s", commandLogTransferQueue.c_str(), clusterLogsFolder.c_str());
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

	returnCode = fileHandler->attach();

	if (returnCode == AES_OHI_NOERRORCODE){
		if (AttachCLOGTQerror == 1){
			AEHEVENTMESSAGE ("acs_alogmaind", 9915, "CEASING", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
			AttachCLOGTQerror = 0;
		}

    	AES_OHI_Filestates status;

    	bool filesTransferred = false;
		bool errorInTransfer  = false;

		for (files = ClusterCLogFiles.begin(); files != ClusterCLogFiles.end(); files++){

			memset(strToLog,0,sizeof(strToLog));
			snprintf(strToLog,sizeof(strToLog),"Verify if the file %s has been already sent to GOH ",(*(files)).c_str());
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

			if ( strcmp((*(files)).c_str(), LastCLogFileSent) <= 0 ){

				if (std::find(ClusterCLogFiles_Not_Sent.begin(), ClusterCLogFiles_Not_Sent.end(), *files) != ClusterCLogFiles_Not_Sent.end()){
					memset(strToLog,0,sizeof(strToLog));
					snprintf(strToLog,sizeof(strToLog),"The file %s has not been sent to GOH due to error, try again",(*(files)).c_str());
					ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
				}
				else {
					memset(strToLog,0,sizeof(strToLog));
					snprintf(strToLog,sizeof(strToLog),"The file %s has been already sent to GOH - SKIP",(*(files)).c_str());
					ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
					continue;
				}
			}

			returnCode = fileHandler->getTransferState ((*(files)).c_str(), status);

			if (status == AES_OHI_FSREADY){

				memset(strToLog,0,sizeof(strToLog));
				snprintf(strToLog,sizeof(strToLog),"Try to send the file %s to GOH ",(*(files)).c_str());
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

				boost::system::error_code copyResult;
		    	boost::filesystem::copy_file(std::string(PathCLog).append(*(files)), std::string(clusterLogsTQFolder).append(*(files)), boost::filesystem::copy_option::overwrite_if_exists, copyResult);
		    	if ( copyResult.value() == boost::system::errc::success ){
					filesTransferred = true;
					returnCode = fileHandler->send ((*(files)).c_str());

					if (returnCode != AES_OHI_NOERRORCODE && returnCode != AES_OHI_FILENOTFOUND && returnCode != AES_OHI_SENDITEMEXIST){
						errorInTransfer = true;
						memset(strToLog,0,sizeof(strToLog));
						snprintf(strToLog,sizeof(strToLog),"Fail to send the file %s to GOH - Return code %i",(*(files)).c_str(),returnCode);
						ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
						ClusterCLogFiles_Not_Sent.push_back(*files);
					}
					else {
						memset(strToLog,0,sizeof(strToLog));
						snprintf(strToLog,sizeof(strToLog),"The file %s has been successfully sent to GOH",(*(files)).c_str());
						ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

						snprintf(LastCLogFileSent,sizeof(LastCLogFileSent),"%s",(*(files)).c_str());
						ClusterCLogFiles_Not_Sent.remove(*files);
					}
		    	}
		    	else {
		    		memset(strToLog,0,sizeof(strToLog));
		    		snprintf(strToLog,sizeof(strToLog),"ERROR when coping the CLOG File (%s) - Error %i(%s)",std::string(PathCLog).append(*(files)).c_str(),copyResult.value(),copyResult.message().c_str());
		    		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(strToLog));
		    		ClusterCLogFiles_Not_Sent.push_back(*files);
		    	}
			}
			else if ( status == AES_OHI_FSDELETE){
	    		memset(strToLog,0,sizeof(strToLog));
	    		snprintf(strToLog,sizeof(strToLog),"The file (%s) is in deleating state - SKIP",std::string(PathCLog).append(*(files)).c_str());
	    		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(strToLog));
			}
			else {
				memset(strToLog,0,sizeof(strToLog));
				snprintf(strToLog,sizeof(strToLog),"Error, the getTransferState(%s) returns %i",(*(files)).c_str(),status);
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
				ClusterCLogFiles_Not_Sent.push_back(*files);
			}
		}

		if (filesTransferred == true  &&  errorInTransfer == true){ // TQ OK but error in transfer
			if (SendCLOGFilesError == 0){
				char  textMsg[200];
				strcpy (textMsg, "AUDIT LOG, FAULT CODE 8\n-\n");
				strcat (textMsg, commandLogTransferQueue.c_str());
				strcat (textMsg, "\n-\n-\n");

				AEHEVENTMESSAGE ("acs_alogmaind", 9916, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "Error when reporting a log file to file transfer server", textMsg);
				SendCLOGFilesError = 1;
			}
		}

		if (filesTransferred == true  &&  errorInTransfer == false){ // TQ OK and NO error in transfer
			if (SendCLOGFilesError == 1){ // Cease the alarm is raised
				AEHEVENTMESSAGE ("acs_alogmaind", 9916, "CEASING", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
				SendCLOGFilesError = 0;
			}
		}

		returnCode = fileHandler->detach();

		if (returnCode != AES_OHI_NOERRORCODE){
			memset(strToLog,0,sizeof(strToLog));
			snprintf(strToLog,sizeof(strToLog),"Transfer Queue send to GOH :  detach error [%d]",returnCode);
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
		}

		if ( (fd_LastCLogFileSent = fopen(LastCLogFileSentPath,"w")) != 0){
			if ( fprintf(fd_LastCLogFileSent,"%s\n",LastCLogFileSent) == strlen(LastCLogFileSent)+1){
				memset(strToLog,0,sizeof(strToLog));
				snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - The last CLOG file sent to GOH has been stored correctly");
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
			}
			else {
				memset(strToLog,0,sizeof(strToLog));
				snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - Fail to store the last CLOG file sent to GOH");
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
			}
			fclose(fd_LastCLogFileSent);
		}
		else {
			memset(strToLog,0,sizeof(strToLog));
			snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - Error when opening the file %s for writing", LastCLogFileSentPath);
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
		}

		if ( (fd_MissingCLogFileSent = fopen(MissingCLogFileSentPath,"w")) != 0){
			for (files = ClusterCLogFiles_Not_Sent.begin(); files != ClusterCLogFiles_Not_Sent.end(); files++){
				if (strcmp((*files).c_str(), LastCLogFileSent) <= 0)
					fprintf(fd_MissingCLogFileSent,"%s\n",(*files).c_str());
			}
			fclose(fd_MissingCLogFileSent);
		}
		else {
			memset(strToLog,0,sizeof(strToLog));
			snprintf(strToLog,sizeof(strToLog),"sendClusterLogFiletoGOH - Error when opening the file %s for writing", MissingCLogFileSentPath);
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));
		}
	}
	else { // Error in TQ attach

		memset(strToLog,0,sizeof(strToLog));
		snprintf(strToLog,sizeof(strToLog),"Transfer Queue send to GOH :  attach error [%d]",returnCode);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(strToLog));

		if (AttachCLOGTQerror == 0){
			char  textMsg[200];
			strcpy (textMsg, "AUDIT LOG, FAULT CODE 7\n-\n");
			strcat (textMsg, commandLogTransferQueue.c_str());
			strcat (textMsg, "\n-\n-\n");

			AEHEVENTMESSAGE ("acs_alogmaind", 9915, "A2", "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "Error when connecting to file transfer server for log transfer queue", textMsg);
			AttachCLOGTQerror = 1;
		}
	}

	delete  fileHandler;
}

void  checkAlogWinFiles (string loggingDir, string tq)
{

	DIR  *handle;
	struct dirent  *entry;

	std::list<string> winFiles;
	std::list<string> linuxFiles;
	std::list<std::string>::iterator  it, files;

	string  fName, fileToCheck;
	AES_OHI_FileHandler  *fileHandler;
	unsigned int  errorCode;

	if (tq.length() == 0)  return;

	linuxFiles.clear();  winFiles.clear();

	if ((handle = opendir (loggingDir.c_str())) == NULL)  return;

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;

			if (fName.substr(0,8).compare("Logfile-") == 0)  winFiles.push_back(fName);
			if (fName.substr(0,6).compare("audit.") == 0)    linuxFiles.push_back(fName);
	}
	closedir (handle);

	if (winFiles.empty())  return;

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", tq.c_str(), "Transfer Queue handling", loggingDir.c_str());

	if (fileHandler == NULL)  return;

	if ((errorCode = fileHandler->attach()) != AES_OHI_NOERRORCODE)
	{
			delete  fileHandler;
			return;
	}

   	AES_OHI_Filestates  status;

   	for (files = winFiles.begin(); files != winFiles.end(); files++)
	{
			fileToCheck = *(files);

			errorCode = fileHandler->getTransferState (fileToCheck.c_str(), status);

			if (status == AES_OHI_FSREADY)    //  the window file is not stored in the TQ
			{
					string  newFile = "audit.";
					newFile = newFile + fileToCheck.substr(8,4);
					newFile = newFile + fileToCheck.substr(13,2);
					newFile = newFile + fileToCheck.substr(16,2);
					newFile = newFile + "-";
					newFile = newFile + fileToCheck.substr(19,4);
					newFile = newFile + "00";

					it = std::find (linuxFiles.begin(), linuxFiles.end(), newFile);
					if (it != linuxFiles.end())  newFile.at(20) = '1';

					char  file1[300], file2[300];

					strcpy (file1, loggingDir.c_str());    strcpy (file2, loggingDir.c_str());
					strcat (file1, "/");				   strcat (file2, "/");
					strcat (file1, fileToCheck.c_str());   strcat (file2, newFile.c_str());
					rename (file1, file2);
			}
			else {
					char  fileName[300];

					strcpy (fileName, loggingDir.c_str());
					strcat (fileName, "/");
					strcat (fileName, fileToCheck.c_str());
					unlink (fileName);
			}
	}

   	errorCode = fileHandler->detach();
	delete  fileHandler;
}

void  checkPlogWinFiles (string loggingDir, string tq)
{

	DIR  *handle;
	struct dirent  *entry;

	std::list<string> winFiles;
	std::list<string> linuxFiles;
	std::list<std::string>::iterator  it, files;

	string  fName, fileToCheck;
	AES_OHI_FileHandler  *fileHandler;
	unsigned int  errorCode;

	if (tq.length() == 0)  return;

	linuxFiles.clear();  winFiles.clear();

	if ((handle = opendir (loggingDir.c_str())) == NULL)  return;

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;

			if (fName.substr(0,9).compare("PLogfile_") == 0)  winFiles.push_back(fName);
			if (fName.substr(0,7).compare("paudit.") == 0)    linuxFiles.push_back(fName);
	}
	closedir (handle);

	if (winFiles.empty())  return;

	fileHandler = new AES_OHI_FileHandler ("ACS", "ALOG", tq.c_str(), "Transfer Queue handling", loggingDir.c_str());

	if (fileHandler == NULL)  return;

	if ((errorCode = fileHandler->attach()) != AES_OHI_NOERRORCODE)
	{
			delete  fileHandler;
			return;
	}

   	AES_OHI_Filestates  status;

   	for (files = winFiles.begin(); files != winFiles.end(); files++)
	{
			fileToCheck = *(files);

			errorCode = fileHandler->getTransferState (fileToCheck.c_str(), status);

			if (status == AES_OHI_FSREADY)    //  the window file is not stored in the TQ
			{
					string  newFile = "paudit.";
					newFile = newFile + fileToCheck.substr(9,4);
					newFile = newFile + fileToCheck.substr(14,2);
					newFile = newFile + fileToCheck.substr(17,2);
					newFile = newFile + "-";
					newFile = newFile + fileToCheck.substr(20,4);
					newFile = newFile + "00.zip";

					it = std::find (linuxFiles.begin(), linuxFiles.end(), newFile);
					if (it != linuxFiles.end())  newFile.at(21) = '1';

					char  file1[300], file2[300];

					strcpy (file1, loggingDir.c_str());    strcpy (file2, loggingDir.c_str());
					strcat (file1, "/");				   strcat (file2, "/");
					strcat (file1, fileToCheck.c_str());   strcat (file2, newFile.c_str());
					rename (file1, file2);
			}
			else {
					char  fileName[300];

					strcpy (fileName, loggingDir.c_str());
					strcat (fileName, "/");
					strcat (fileName, fileToCheck.c_str());
					unlink (fileName);
			}
	}

   	errorCode = fileHandler->detach();
	delete  fileHandler;
}

struct mntent  *mountpoint(char *filename, struct mntent *mnt, char *buf, size_t buflen)
{

    struct stat  s;
    FILE        *fp;
    dev_t        dev;

    if (stat(filename, &s) != 0)  return NULL;

    dev = s.st_dev;

    if ((fp = setmntent("/proc/mounts", "r")) == NULL)  return NULL;

    while (getmntent_r(fp, mnt, buf, buflen)) 
    {
        if (stat(mnt->mnt_dir, &s) != 0)  continue;

        if (s.st_dev == dev) 
        {
				endmntent(fp);
				return mnt;
        }
    }

    endmntent(fp);
    return NULL;
}

int  checkHardLimit (int sizeEvent)
{

	struct dqblk  strctQuota;

	quotactl (QCMD(Q_GETQUOTA, GRPQUOTA), Mnt.mnt_fsname, InfoPathAudit.st_gid, (char*)&strctQuota);

	if (strctQuota.dqb_bhardlimit < 10)        //  Temporary patch
	{
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("QUOTACTL :  CHECK OK"));
			return 1;
	}

	unsigned int  currBlock = toqb (strctQuota.dqb_curspace + sizeEvent);

	if (currBlock > strctQuota.dqb_bhardlimit)  return 0;
	return 1;
}

ACS_APGCC_ReturnType  setWorkingPath (void)
{

	char  rootPath[15];
	int   lenPath = 300;
	strcpy (rootPath, "auditLog");
	std::string alogDir;
	ACS_APGCC_CommonLib  apgccLib;

	if (apgccLib.GetFileMPath (rootPath, PathAudit, lenPath) != ACS_APGCC_DNFPATH_SUCCESS)
	{
			syslog(LOG_ERR, "acs_alogmaind, PATH AUDIT definition failed !");
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("setWorkingPath ...  PATH AUDIT definition failed  "));
			return ACS_APGCC_FAILURE;
	}

	char  linuxCmd[500];

	strcpy (linuxCmd, "/bin/chmod 775 ");
	strcat (linuxCmd, PathAudit);
	system (linuxCmd);

	strcpy (FileAudit, PathAudit);

	DIR    *handle;
	string  fileInUse;
	int     counter = 0;

	do {																//  TR HT12517 solution :
			if ((handle = opendir (FileAudit)) == NULL)  sleep (1);		//  synchronization with the /data mounting
			counter ++;
	}
	while (handle == NULL  &&  counter < 3);

	if (handle != NULL)
	{
		fileInUse = getLastWrittenLogFile (handle);

		if (fileInUse.length() > 0)
		{
				strcat (FileAudit, "/");  strcat (FileAudit, fileInUse.c_str());
				CurrentLogFile = fileInUse;

				fstream  targetFile;
				long     posAudit;

				targetFile.open (FileAudit, fstream::out | fstream::app);
				posAudit = targetFile.tellp();
				targetFile.close();

				if (posAudit >= ONE_MEGA)  CurrLogGreaterThanOneMB = true;
		}
		else {
				ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("NO FILES IN .../audit_logs FOR THE CURRENT FORMAT  "));
				strcpy (FileAudit, "");
		}
	}
	else {
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("THE FOLDER .../audit_logs DOESN'T EXIST  "));
			strcpy (FileAudit, "");
	}

	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("QUOTA CHECK INITIALIZATION ..... "));
	memset(buf,0,BUFSIZ);
	
	mountpoint (PathAudit, &Mnt, buf, BUFSIZ);      //  Error check omitted
	stat (PathAudit, &InfoPathAudit);               //  Error check omitted

	if (APtype == 2)  return ACS_APGCC_SUCCESS;       //  on AP2 PLOG and CLOG are not provided

	if (CurrentNodeState == "active"  &&  MCpSystem == true)		//  check for a RESTORE
	{
			strcpy (PathCLog, PathAudit);
			strcat (PathCLog, "/command_logs/");
			if (! checkFile (PathCLog, true)){
				ACSALOGLOG (LOG_LEVEL_ERROR, TEXTERROR("Not able to create CLOG directory"));
			}

			if (! checkFile (clusterLogsTQFolder.c_str(), true)){
				ACSALOGLOG (LOG_LEVEL_ERROR, TEXTERROR("Not able to create CLOG TQ directory"));
			}

			if (DirCreated == true){
				string  clogFolderCreated;

				clogFolderCreated.assign(PathCLog);
				clogFolderCreated += "ClogFolderCreated.txt";

				FILE  *fp = ::fopen (clogFolderCreated.c_str(), "w");

				if (fp != NULL){
					::fputs ("OK\n", fp);
					::fflush (fp);
					::fclose (fp);
				}
			}
			else {								//  ISSUE 9  solution
				string  clearFile;

				getClearDataDiskPath (clearFile);
				clearFile += "/AlogRestore.txt";

				FILE  *fp = ::fopen (clearFile.c_str(), "w");

				if (fp != NULL){
					::fputs ("OK\n", fp);
					::fflush (fp);
					::fclose (fp);
				}
			}

			string  clogFolderCreated;			//  check for a RESTORE

			clogFolderCreated.assign(PathCLog);
			clogFolderCreated += "ClogFolderCreated.txt";

			FILE  *fpClog = ::fopen (clogFolderCreated.c_str(), "r");

			if (fpClog != NULL)
			{
					::fclose (fpClog);

					string  clearFile;						//  a RESTORE could be required
															//  the CLOG restore is executed only if the command_logs
															//  folder has been newly created at the last start up
					getClearDataDiskPath (clearFile);
					clearFile += "/AlogRestore.txt";

					FILE  *fp = ::fopen (clearFile.c_str(), "r");

					if (fp != NULL)
					{
							::fclose (fp);
					}
					else {											//  a RESTORE has been required
							unlink (clogFolderCreated.c_str());

							string  backupPath;

							getAlogDataDiskPath (backupPath);
							backupPath += "/ACS_ALOG/command_logs/CLogfile* ";

							char  linuxCmd[500];

							ACSALOGLOG (LOG_LEVEL_INFO, TEXTERROR("Restoring the Backup CLOG files"));
							strcpy (linuxCmd, "/bin/cp -u ");
							strcat (linuxCmd, backupPath.c_str());
							strcat (linuxCmd, PathCLog);
							system (linuxCmd);

							FILE  *fp = ::fopen (clearFile.c_str(), "w");		//  create the file for the next restore

							if (fp != NULL)
							{
									::fputs ("OK\n", fp);
									::fflush (fp);
									::fclose (fp);
							}
					}
			}
	}

	strcpy (PathPAudit, PathAudit);
	strcat (PathPAudit,"/protected_logs");
	std::string PlogEncryptedFilePath("/data/acs/data/alog");//HX86564
	std::string PlogPath(PathPAudit);			//  Check if PLOG directory is present or not
	if (! checkFile(PlogPath.c_str(),true))
	{
			ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Not able to create PLOG directory"));
	}
	if (! checkFile(PlogEncryptedFilePath.c_str(),true))//HX86564
	{
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Not able to create PLOG directory"));
	}
	//FilePAudit = PathPAudit;

	if ((handle = opendir (PlogEncryptedFilePath.c_str())) != NULL)//HX86564
	{
		fileInUse = getLastWrittenPLogFile (handle);

		if (fileInUse.length() > 0)
		{
            FilePName  ="/data/acs/data/alog/" + fileInUse;//HX86564
            FilePAudit = PathPAudit;
            FilePAudit = FilePAudit + "/" + fileInUse + ".7z";
            CurrentPLogFile = fileInUse + ".7z";
		}
		else {
				ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("NO FILES IN .../data/acs/data/alog/ directory"));
				FilePName= "";
				FilePAudit = "";
		}
	}
	else {
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR("THE FOLDER .../protected_logs DOESN'T EXIST  "));
			FilePName= "";
			FilePAudit = "";
	}

	getAlogDataDiskPath(alogDir);
	alogDir += "/ACS_ALOG";
	createDir(alogDir);

	return ACS_APGCC_SUCCESS;
}

void  ACSALOGLOG (ACS_TRA_LogLevel levelErr, string errorText)
{

	acs_alog_errorHandler  Log;
	Log.InternalLogging(errorText,levelErr);
}

void  ACSALOGTRACE (string message)
{
	message = "---> " + message + "!";

	acs_alog_errorHandler  Log;
	Log.TRAEventString(message);
}

string  TEXTERROR (string errText)
{
	string retString;
	retString = " ---> " + errText + "!"; 
	return retString;
}

void  AEHEVENTMESSAGE (const char * processName, long int specificProblem, const char * Severity,
					   const char * probCause, const char * classReference, const char * objOfReference,
					   const char * problemData, const char * problemText )
{

	acs_alog_errorHandler  Log;
	Log.AEHEventMessage (processName, specificProblem, Severity, probCause,
						 classReference, objOfReference, problemData, problemText);
}

void  rotateAuditLog ()
{

	char  suffix[30], suffixMin[30];

	time_t  t;
	time(&t);
	struct tm  *lt = localtime(&t);

	if (FileNamingFormat == LINUX_FORMAT)		//  Format :  audit.yyyymmdd-hhmmss
	{
			sprintf (suffixMin, "%04d%02d%02d-%02d%02d", lt->tm_year + 1900, lt->tm_mon +1, lt->tm_mday, lt->tm_hour, lt->tm_min);

			if (CurrentLogFile.length() != 21  ||  CurrentLogFile.substr(6,13).compare(suffixMin) != 0)
			{
					sprintf (suffix, "%04d%02d%02d-%02d%02d%02d", lt->tm_year + 1900, lt->tm_mon +1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

					strcpy (FileAudit, PathAudit);
					strcat (FileAudit, "/audit.");  strcat (FileAudit, suffix);

					CurrentLogFile = "audit.";  CurrentLogFile += suffix;
			}
	}
	else {		//  WINDOWS_FORMAT :	LOGFILE-yyyy-mm-dd-hhmm

			sprintf (suffix, "%04d-%02d-%02d-%02d%02d", lt->tm_year + 1900, lt->tm_mon +1, lt->tm_mday, lt->tm_hour, lt->tm_min);

			if (CurrentLogFile.length() != 23  ||  CurrentLogFile.substr(8,15).compare(suffix) != 0)
			{
					strcpy (FileAudit, PathAudit);
					strcat (FileAudit, "/LOGFILE-");  strcat (FileAudit, suffix);

					CurrentLogFile = "LOGFILE-";  CurrentLogFile += suffix;
			}
	}
}

void  rotatePAuditLog ()
{
	char  suffix[30], suffixMin[30];

	time_t  t;
	time(&t);
	struct tm  *lt = localtime(&t);

	if (FileNamingFormat == LINUX_FORMAT)		//  Format :  paudit.yyyymmdd-hhmmss.zip
	{
			sprintf (suffixMin, "%04d%02d%02d-%02d%02d", lt->tm_year + 1900, lt->tm_mon +1, lt->tm_mday, lt->tm_hour, lt->tm_min);

			if (CurrentPLogFile.length() != 26  || CurrentPLogFile.substr(7,13).compare(suffixMin) != 0)
			{
					sprintf (suffix, "%04d%02d%02d-%02d%02d%02d", lt->tm_year + 1900, lt->tm_mon +1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

					FilePAudit = PathPAudit;
					FilePAudit = FilePAudit + "/paudit." + suffix + ".7z";
					FilePName  = "/data/acs/data/alog/paudit.";  FilePName = FilePName + suffix;//HX86564
					CurrentPLogFile = "paudit.";  CurrentPLogFile += suffix;  CurrentPLogFile += ".7z";
			}
	}
	else {			//  WINDOWS FORMAT :	PLOGFILE_yyyy_mm_dd_hhmm.zip

			sprintf (suffix, "%04d_%02d_%02d_%02d%02d", lt->tm_year + 1900, lt->tm_mon +1, lt->tm_mday, lt->tm_hour, lt->tm_min);

			if (CurrentPLogFile.length() != 28  || CurrentPLogFile.substr(9,15).compare(suffix) != 0)
			{
					FilePAudit = PathPAudit;
					FilePAudit = FilePAudit + "/PLOGFILE_" + suffix + ".7z";
					FilePName  = "/data/acs/data/alog/PLOGFILE_";  FilePName = FilePName + suffix;//HX86564
					CurrentPLogFile = "PLOGFILE_";  CurrentPLogFile += suffix;  CurrentPLogFile += ".7z";
			}
	}
}

void  emptingFileNameVariables (void)
{
	ACSALOGLOG (LOG_LEVEL_TRACE, TEXTERROR("flagTemplateFormatChangePending is true - Emptying the filename variables"));

	FileAudit[0] = 0;    CurrentLogFile  = "";
	FilePAudit   = "";   CurrentPLogFile = "";//handled in writeTargetPLog

	// TR HW28526 - reset the flagTemplateFormatChangePending flag as filename variables are now emptied
	ACSALOGLOG (LOG_LEVEL_TRACE, TEXTERROR("flagTemplateFormatChangePending is reset to false"));
	flagTemplateFormatChangePending = false;
}

string  createCLOGfile ()
{

	char    suffix[30];
	string  clogFile = "CLogfile-";

	time_t  t;
	time(&t);
	struct tm  *lt = localtime(&t);

	sprintf (suffix, "%04d-%02d-%02d-%02d%02d",
					 lt->tm_year + 1900, lt->tm_mon +1, lt->tm_mday, lt->tm_hour, lt->tm_min);

	clogFile = clogFile + suffix;

	return  clogFile;
}

bool  checkCLOGfileSize (const char *fName, const long fSize)
{

		fstream  clogFile;
		long  posCLOG = 0;

		clogFile.open (fName, fstream::out | fstream::app);
		if (clogFile.is_open())
		{
				posCLOG = clogFile.tellp();
				clogFile.close();
		}

        if (posCLOG > fSize)  return true;

		return false;
}

bool  checkIfSYBUPisReceived (const char *fName)
{
		string    line;
		ifstream  file(fName);
		long      posCLOG;

		if (! file.is_open ())  return false;

		file.seekg(0, file.end);
		posCLOG = file.tellg();
		if (posCLOG > 500)  file.seekg(posCLOG - 500);		//  Look for SYBUP only in the bottom of the current CLOG file
		else				file.seekg(0, file.beg);

		while (file.good())
		{
				getline (file, line);

				if (line.length() > 0)
				{
						stringToUpper (line);

						if (line.find("SYBUP") != string::npos)
						{
								file.close();
								return true;
						}
				}
		}
		file.close();

		return false;
}

string  getLastWrittenLogFile (DIR  *handle)
{

	struct dirent  *entry;
	string  fName;
	std::list<string> loggingFiles;

	loggingFiles.clear();

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;
			if (fName.substr(0,6).compare("audit.") == 0  ||  fName.substr(0,8).compare("LOGFILE-") == 0)  loggingFiles.push_back(fName);
	}
	closedir (handle);

	strcpy (AbsAuditPath, PathAudit);			//  AbsAuditPath is used in the compareFilename() method
	loggingFiles.sort (compareFilename);

	if (! loggingFiles.empty())
	{
			fName.assign (loggingFiles.back());   			//  take the most recent Logging File

			if (FileNamingFormat == LINUX_FORMAT)
			{
					if (fName.substr(0,6).compare("audit.") == 0)  return (fName);
			}

			if (FileNamingFormat == WINDOWS_FORMAT)
			{
					if (fName.substr(0,8).compare("LOGFILE-") == 0)  return (fName);
			}
	}
	return  "";
}

string  getLastWrittenPLogFile (DIR  *handle)
{

	struct dirent  *entry;//START OF HX86564
	string  fName;

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;
			if (fName.substr(0,7).compare("paudit.") == 0  ||  fName.substr(0,9).compare("PLOGFILE_") == 0)
			{
				 closedir (handle);
				 return (fName);// returning the last written PLOG encrypted file name
			}
	}
	closedir (handle);
	return  "";
}

void  handleLoggingDirSize (const char *dirName, const int dirSize)
{

	DIR  *handle;
	struct dirent  *entry;
	struct stat  fileStatus;

	std::list<string> auditFiles;
	string  fName, pathAbs;
	long    totalSize = 0;

	if ((handle = opendir (dirName)) == NULL)  return;

	auditFiles.clear();

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;
			if (fName.substr(0,8).compare("Logfile-") == 0  ||  fName.substr(0,6).compare("audit.") == 0  ||  fName.substr(0,8).compare("LOGFILE-") == 0)
			{
					pathAbs.assign(dirName);
					pathAbs = pathAbs + "/" + fName;

					stat (pathAbs.c_str(), &fileStatus);

					totalSize = totalSize + fileStatus.st_size;

					auditFiles.push_back (fName);
			}
	}
	closedir (handle);

	strcpy (AbsAuditPath, dirName);			//  AbsAuditPath is used in the compareFilename() method
	auditFiles.sort (compareFilename);

	while (totalSize > (long)dirSize)
	{
			fName = auditFiles.front();			//  take the oldest Logging File
			auditFiles.pop_front();				//  delete the oldest Logging File

			pathAbs.assign(dirName);
			pathAbs = pathAbs + "/" + fName;

			stat (pathAbs.c_str(), &fileStatus);

			if (unlink (pathAbs.c_str()) != 0)
			{
					if (DeleteFilesError == 0)
					{
							AEHEVENTMESSAGE ("acs_alogmaind", 9910, "A2", "AUDIT LOG FAULT",
								 	 	 	 "APZ", "acs_alogmaind/Alarm", "Error when deleting a log-file", "FCODE 3\nImpossible to delete a Log File to reduce the Log Folder size");
							DeleteFilesError = 1;
					}
					return;
			}

			totalSize = totalSize - fileStatus.st_size;			//  decrement the DIR size of the deleted file size

			if (DeleteFilesError == 1)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9910, "CEASING", "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
					DeleteFilesError = 0;
			}
	}
}

void  handleCLogDirSize (const char *dirName, const int dirSize)
{

	DIR  *handle;
	struct dirent  *entry;
	struct stat     fileStatus;

	std::list<string>  clogFiles;
	string  fName, pathAbs;
	long    totalSize = 0;

	clogFiles.clear();

	if ((handle = opendir (dirName)) == NULL)  return;

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;

			if (fName.substr(0,9).compare("CLogfile-") == 0)
			{
					pathAbs.assign(dirName);
					pathAbs = pathAbs + "/" + fName;

					stat (pathAbs.c_str(), &fileStatus);

					totalSize = totalSize + fileStatus.st_size;
					clogFiles.push_back(fName);
			}
	}
	closedir (handle);

	clogFiles.sort();

	while (totalSize > (long)dirSize)
	{
			fName = clogFiles.front();
			pathAbs.assign(dirName);
			pathAbs = pathAbs + "/" + fName;

			stat (pathAbs.c_str(), &fileStatus);

			if (unlink (pathAbs.c_str()) != 0)
			{
					if (DelCLogFilesError == 0)
					{
							AEHEVENTMESSAGE ("acs_alogmaind", 9914, "A2", "AUDIT LOG FAULT",
									 	 	 "APZ", "acs_alogmaind/Alarm", "Error when deleting a command log-file", "FCODE 12\nImpossible to delete a CLog File to reduce the CLOG Folder size");
							DelCLogFilesError = 1;
					}
					return;
			}

			totalSize = totalSize - fileStatus.st_size;

			if (DelCLogFilesError == 1)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9914, "CEASING", "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
					DelCLogFilesError = 0;
			}
			clogFiles.pop_front();
	}
}

void  handleProtectedLoggingDirSize (const char *dirName, const int dirSize)
{

	DIR  *handle;
	struct dirent  *entry;
	struct stat  fileStatus;

	std::list<string> pauditFiles;
	string  fName, pathAbs;
	long    totalSize = 0;

	if ((handle = opendir (dirName)) == NULL)  return;

	pauditFiles.clear();

	while ((entry = readdir(handle)) != NULL)
	{
			fName = entry->d_name;
			if (fName.substr(0,9).compare("PLogfile_") == 0  ||  fName.substr(0,7).compare("paudit.") == 0  ||  fName.substr(0,9).compare("PLOGFILE_") == 0)
			{
					pathAbs.assign(dirName);
					pathAbs = pathAbs + "/" + fName;

					stat (pathAbs.c_str(), &fileStatus);

					totalSize = totalSize + fileStatus.st_size;

					pauditFiles.push_back(fName);
			}
	}
	closedir (handle);

	strcpy (AbsAuditPath, dirName);			//  AbsAuditPath is used in the compareFilename() method
	pauditFiles.sort (compareFilename);

	while (totalSize > (long)dirSize)
	{
			fName = pauditFiles.front();		//  take the oldest Logging File
			pauditFiles.pop_front();			//  delete the oldest Logging File

			pathAbs.assign(dirName);
			pathAbs = pathAbs + "/" + fName;

			stat (pathAbs.c_str(), &fileStatus);

			if (unlink (pathAbs.c_str()) != 0)
			{
					if (DelPLogFilesError == 0)
					{
							AEHEVENTMESSAGE ("acs_alogmaind", 9934, "A2", "AUDIT LOG FAULT",
											"APZ", "acs_alogmaind/Alarm", "Error when deleting a protected log-file", "FCODE: 16\nImpossible to delete a Protected Log File to reduce the Log Folder size");
							DelPLogFilesError = 1;
					}
					return;
			}

			totalSize = totalSize - fileStatus.st_size;			//  decrement the DIR size of the deleted file size

			if (DelPLogFilesError == 1)
			{
					AEHEVENTMESSAGE ("acs_alogmaind", 9934, "CEASING", "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
					DelPLogFilesError = 0;
			}
	}
}

int  checkApiPipeInSyslog (void)
{

	int  ret = 0;
	ifstream   file ("/etc/syslog-ng/syslog-ng.conf");

	if (file.is_open())
	{
		string  line;

		while (file.good())
		{
			getline (file, line);
			if (line.find ("s_acs_alog_pipe") != string::npos)  ret = 1;
		}
		file.close();
	}

	return ret;
}

string  extractMMLprintoutHeader (string exclRec)
{

		string  header = "";
		int  posI = 0, posF = 0, exclRec_length = exclRec.length();

		for (int j=0; j < exclRec_length; j++)
		{
				if ((unsigned char)exclRec.at(j) == '\n')
				{
						if (posI == 0)  posI = j + 1;
						else
						{
								posF = j;
								break;
						}
				}
		}
		if (posF > 0)  header = exclRec.substr(posI, posF - posI);

		return  header;
}

void  stringToUpper (string &str)
{
		int string_length = str.length();
		for (int j=0; j<string_length; j++)  str[j] = toupper(str[j]);
}

void  stringToLower (string &str)
{
		int string_length = str.length();
		for (int j=0; j<string_length; j++)  str[j] = tolower(str[j]);
}

string  parseMPAexclItem (string item)
{
	int  j = 0, nItems = 0;
	string  cmdItems[10], res = "";

	while (j < (int)item.length())
	{
			cmdItems[nItems] = "";

			while (j < (int)item.length()  &&  item.at(j) == ' ')  j ++;

			if (j == (int)item.length())  break;

			while (j < (int)item.length()  &&  item.at(j) != ' ')
			{
					cmdItems[nItems] = cmdItems[nItems] + item.at(j);
					j ++;
			}
			if (cmdItems[nItems].compare("") != 0)  nItems ++;
	}

	if (nItems == 2)      //  Format MPA :   " Command  Parameter "
	{
			res = cmdItems[0];
			res.append(" ");  res.append(cmdItems[1]);
	}

	return res;
}

bool createDir(string& Dir)
{
	bool dirCreated = false;
	struct stat statBuff;
	int result = -1;
	if( ::stat(Dir.c_str(), &statBuff)  != 0 )
	{
			result = ACE_OS::mkdir(Dir.c_str());
	}
	else {
			result = 0;
	}

	if(result == EEXIST || result == 0)
	{
		dirCreated = true;
	}

	return dirCreated;
}

bool checkFile(const char* lpszFile, const bool bIsDir)
{
	// Checks if a file or directory exist (even devices)

	DirCreated = false;

	if (bIsDir)
	{
		struct stat statBuff;

		if (::stat(lpszFile, &statBuff) != 0)
		{
				if (ACE_OS::mkdir(lpszFile) != 0  &&  errno != EEXIST)  return false;
				else													DirCreated = true;
		}
		else if (!S_ISDIR(statBuff.st_mode))
		{
				unlink(lpszFile);
				if (ACE_OS::mkdir(lpszFile) != 0  &&  errno != EEXIST)  return false;
		}

		return true;
	}

	bool dirCreated = false;
	struct stat statBuff;
	int result = -1;



	result = ACE_OS::open(lpszFile, O_CREAT);


	if (result == ACE_INVALID_HANDLE)
	{
		return false;
	}

	chmod (lpszFile, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	ACE_OS::close(result);



	result = ACE_OS::open(lpszFile, O_RDONLY|O_WRONLY);


	if (result == ACE_INVALID_HANDLE)
	{
		return false;
	}
	if (errno == EACCES)
	{
		ACE_OS::close(result);
		return false;
	}
	ACE_OS::close(result);

	return true;
}

bool  getAlogDataDiskPath(string& szFolderName)
{
	std::string szFileName = "/usr/share/pso/storage-paths/config";
	string line;
	string path;
	ifstream file(szFileName.c_str());
	if (file.is_open()) {
		while (file.good()) {
			getline(file, line);
			szFolderName += line;
		}
		file.close();
	}
	else {
		return false;
	}

	return true;
}

bool  getClearDataDiskPath (string& szFolderName)
{
	string  szFileName = "/usr/share/pso/storage-paths/clear";
	string  line;
	string  path;

	ifstream file(szFileName.c_str());
	if (file.is_open()) {
		while (file.good()) {
			getline(file, line);
			szFolderName += line;
		}
		file.close();
	}
	else  return false;

	return true;
}

ACE_TCHAR* Encode(ACE_TCHAR* lpszData)
{
	static ACE_TCHAR szData[500];

	int nLen = strlen(lpszData);
	strncpy(szData, lpszData, nLen);
	szData[nLen] = ('\0');

	for (int n = 0; n < nLen; n++)
		szData[n] ^= 255;

	return szData;
}

ACE_TCHAR* Decode(ACE_TCHAR* lpszData)
{
	return Encode(lpszData);
}

bool runCommand(const string command, string& output) 
{
	FILE *fp;
	char readLine[10000];
	output = "";

	/* Open the command for reading. */
	fp = popen(command.c_str(), "r");
	if (fp == 0) {
		return false;
	}

	/* Read the output a line at a time and store it. */
	while (fgets(readLine, sizeof(readLine) - 1, fp) != 0) {

		size_t newbuflen = strlen(readLine);

		if ( (readLine[newbuflen - 1] == '\r') || (readLine[newbuflen - 1] == '\n') ) {
			readLine[newbuflen - 1] = '\0';
		}

		if ( (readLine[newbuflen - 2] == '\r') || (readLine[newbuflen - 2] == '\n') ) {
			readLine[newbuflen - 2] = '\0';
		}

		output += readLine;
	}

	pclose(fp);

	return true;
}

bool  readFromAlarms (const char *info)
{

	char  fileAlarms[100], myRow[50];

	strcpy (fileAlarms, PathAudit);
	strcat (fileAlarms, "/Alarms");

	FILE  *fp = ::fopen(fileAlarms, "r");

	if (fp != NULL)
	{
		while (fgets (myRow, 50, fp) != NULL)
		{
			if (strcmp (info, myRow) == 0)  return true;
		}
		::fclose (fp);
	}
	return false;
}

void  writeOnAlarms (const char *info)
{

	char  fileAlarms[100];

	if (readFromAlarms (info) == true)  return;

	strcpy (fileAlarms, PathAudit);
	strcat (fileAlarms, "/Alarms");

	FILE  *fp = ::fopen(fileAlarms, "a");

	if (fp != NULL)
	{
		::fputs (info, fp);
		::fflush (fp);
		::fclose (fp);
	}
}

void  deleteFromAlarms (const char *info)
{

	char  fileAlarms[100], fileBkp[100], myRow[50];

	strcpy (fileAlarms, PathAudit);  strcat (fileAlarms, "/Alarms");
	strcpy (fileBkp, PathAudit);  strcat (fileBkp, "/Backup");

	FILE  *fpAl = ::fopen(fileAlarms, "r");
	FILE  *fp   = ::fopen(fileBkp, "w");

	if (fpAl != NULL)
	{
		while (fgets (myRow, 50, fpAl) != NULL)
		{
			if (strcmp (info, myRow) != 0)
			{
				::fputs (info, fp);
				::fflush (fp);
			}
		}

		::fclose (fp);
		::fclose(fpAl);
	}

	rename (fileBkp, fileAlarms);
}

// TR HX49170 - Hide 7z password from 'ps' by providing the password interactively to 7z child process
bool execute7zCmd(string command, string plogPassword, int &subprocStatus)
{
	pid_t pid = 0;
	int outpipefd[2];
	string logStr = "";
	char logChar[10];

	int pipeRes = pipe(outpipefd);
	if(pipeRes == -1)
	{
		logStr = "acs_alog_utility::execute7zCmd() - pipe(outpipefd) returned -1! Pipe creation failed - errno = ";
		sprintf(logChar,"%d", errno);
		logStr += logChar;
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(logStr));
		return false;
	}

	pid = fork();
	if (pid == 0)
	{
		// Child
		close(outpipefd[1]);
		if(dup2(outpipefd[0], STDIN_FILENO) == -1)
		{
			close(outpipefd[0]);
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("acs_alog_utility::execute7zCmd() - dup2() failed in child process!"));
			exit(-1);
		}
		close(outpipefd[0]);
		execl("/bin/sh", "sh", "-c", command.c_str(), NULL);
		exit(1);
	}
	else if (pid == -1)
	{
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("acs_alog_activeWorks::execute7zCmd() - fork() failed!"));
		close(outpipefd[0]);
		close(outpipefd[1]);
		return false;
	}

	close(outpipefd[0]);
	write(outpipefd[1], plogPassword.c_str(), plogPassword.length());
	close(outpipefd[1]);

	ACE_OS::waitpid(pid, &subprocStatus, 0);

	return true;
}

std::string decryptString(const std::string &strPwdCrypt) //HX86564
{
    const size_t log_buffer_size = 1024;
    char log_buffer[log_buffer_size] = {0};
    char * plaintext = NULL;
    std::string ciphertext = strPwdCrypt;
    std::string strPwdDecrypt("");
    SecCryptoStatus decryptStatus;

    if(!strPwdCrypt.empty())
    {
        decryptStatus = sec_crypto_decrypt_ecimpassword_legacy(&plaintext, ciphertext.c_str());
        if(decryptStatus != SEC_CRYPTO_OK )
        {
            ::snprintf(log_buffer, log_buffer_size, "Decryption failed");
            //log.Write(log_buffer, LOG_LEVEL_ERROR);
        }else{
            strPwdDecrypt = plaintext;
        }
        if(plaintext !=0 )
            free(plaintext);
    }

    return strPwdDecrypt;
}

std::string encryptString(const std::string& inputString) //HX86564
{
    const size_t log_buffer_size = 1024;
    char log_buffer[log_buffer_size] = {0};
    char * plaintext = NULL;
    std::string ciphertext = inputString;
    std::string strEncrypt("");
    SecCryptoStatus encryptStatus;

    if(!inputString.empty())
    {
        encryptStatus = sec_crypto_encrypt_ecimpassword_legacy(&plaintext, ciphertext.c_str());
        if(encryptStatus != SEC_CRYPTO_OK )
        {
            ::snprintf(log_buffer, log_buffer_size, "Encryption failed");
            //log.Write(log_buffer, LOG_LEVEL_ERROR);
        }else{
            strEncrypt = plaintext;
        }
        if(plaintext !=0 )
            free(plaintext);
    }

    return strEncrypt;
}
