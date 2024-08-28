//*=================================================================== */
/**
  @file acs_alog_activeWorks.cpp

  Class method implementationn for acs_alog_activeWorks type module.

  This module contains the implementation of class declared in
  the acs_alog_activeWorks.h module

  @version 3.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       02/02/2011     xgencol/xgaeerr       Initial Release
  N/A       07/08/2012     xbhakat               Final Release
  N/A       07/09/2015     xsansud               HT92230
  =================================================================== */


#include <acs_alog_activeWorks.h>
#include <acs_alog_Imm_Implementer.h>
#include <acs_alog_utility.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <ace/UNIX_Addr.h>
#include <sys/stat.h>
#include <ace/LSOCK_Acceptor.h>
#include <string>
#include <queue>

#include <unistd.h>
#include <bits/stdc++.h>
#include <lde-logger.h>

using namespace std;

#define PIPE_NAME		"/var/run/ap/ALOG_API_pipe"
#define MAX_ERROR_OPEN_API_PIPE 5
#define MSG_SIZE_FIELD_LENGTH 4
#define MAX_SECONDS_BEFORE_EXIT 20

string acs_alog_activeWorks::dataEvent;
string acs_alog_activeWorks::timeEvent;
int acs_alog_activeWorks::SYSCALLFind;
int acs_alog_activeWorks::EXECVEFind;
int acs_alog_activeWorks::startWrite;
int acs_alog_activeWorks::noWork = 0;
string acs_alog_activeWorks::dataEXVE;
string acs_alog_activeWorks::timeEXVE;
string acs_alog_activeWorks::dataSYCL;
string acs_alog_activeWorks::timeSYCL;
string acs_alog_activeWorks::sid;

ACE_Recursive_Thread_Mutex  acs_alog_activeWorks::_acs_alog_Mutex_auditlog_file;

int acs_alog_activeWorks::stopThReadMain;
int acs_alog_activeWorks::stopThReadApi;
int acs_alog_activeWorks::stopThHandleLogs;
int stopThEventQueue;

multiset<string>typecmdnc;
multiset<string>typecmdmc;

multiset<string>typecmdmpa;
multiset<string>typecmdmp;
multiset<string>typecmdnp;

ExListType			   m_PMMLcmds; // PL
ExListType			   m_PMMLprts; // PL
ExListType             m_PUSRs;    // PL

ACE_Recursive_Thread_Mutex  _acs_alog_PLog_Mutex;
ACE_Recursive_Thread_Mutex  _acs_alog_ZipPlog_Mutex;
extern ACE_Recursive_Thread_Mutex templateFormatPendingMutex;

acs_alog_nodestate *alogNodeState = NULL;
acs_alog_parser eParse;

string  FilePName;

string appended_event_str ="";//used for appending decrypted strings
string event="";//Final decrypted string written and zipped in file.
string event_current="";//used for storing decrypted string while sending a file to protected_logs folder

bool  CurrLogGreaterThanOneMB  = false;
bool  CurrPLogGreaterThanOneMB = false;

extern string  CurrentLogFile;
extern string  CurrentPLogFile;

extern string  CurrentNodeState;
extern string  FilePAudit;
extern char    FileAudit[];
extern char    PathAudit[];
extern char    PathPAudit[];
extern char    PathCLog[];
extern string  protectedPassword;
extern string  TQdn;
extern string  commandLogTQdn;

extern acs_alog_Imm_Implementer  implementer_AL;

extern char   WritePlogError;
extern char   OpenPlogError;
extern char   OpenSysLogError;
extern char   ReadSysLogError;
extern char   OpenAttrFileError;
extern char   ReadAttrFileError;
extern char   OpenPMMLPRTFileError;
extern char   ReadPMMLPRTFileError;
extern char   OpenPMMCMDFileError;
extern char   ReadPMMLCMDFileError;
extern char   OpenPUSRFileError;
extern char   ReadPUSRFileError;
extern char   OpenPLogDirError;
extern char   AttachPLTQerror;


extern int     checkHardLimit (int sizeEvent);
extern int     checkApiPipeInSyslog (void);
extern void    handleLoggingDirSize (const char *dirName, const int dirSize);
extern void    handleCLogDirSize (const char *dirName, const int dirSize);
extern void    handleProtectedLoggingDirSize (const char *dirName, const int dirSize);
extern void    sendOneFileToGOH (string auditDir, string TQ, string fileToSend);
extern void    sendOnePlogFileToGOH (string auditDir, string TQ, string fileToSend);
extern void    sendOneClusterLogFiletoGOH(std::string, std::string, std::string);
extern void    sendClusterLogFiletoGOH(std::string, std::string);
extern void    sendFilesToGOH (string auditDir, string TQ);
extern void    sendPlogFilesToGOH (string auditDir, string TQ);
extern void    stringToUpper (string &str);
extern string  extractMMLprintoutHeader (string exclRec);
extern string  createCLOGfile (void);
extern bool    checkCLOGfileSize (const char *fName, const long fSize);
extern bool    checkIfSYBUPisReceived (const char *fName);
extern void    checkAlogWinFiles (string loggingDir, string tq);
extern void    checkPlogWinFiles (string loggingDir, string tq);
extern void  emptingFileNameVariables (void);

int CheckALOGSysFile( bool check_size_zero , bool if_writable);
void sendCurrentEncryptedPLogFile(void);

string  TransferQueue = "";
string  commandLogTransferQueue = "";
string  PLOGTransferQueue = "";
string  clusterLogsTQFolder = "/data/opt/ap/internal_root/data_transfer/source/cluster_clog/";
int     AlogIsAlive = 0;
int     APtype = 0;            //  AP1 = 1   AP2 = 2
int     PlogIsAlive = 0;	// PLOG
int     CommandAndSessionLogState = 0;
int     CommandAndSessionPLogState = 0;
int     LargeEventDataSkipped = 0;
int     FileNamingFormat = 0;
int     RecForCLog = 0;
bool    MCpSystem = false;
string  CLOGevent;
bool	flagTemplateFormatChangePending = false;
string  strHostname = "";

ACE_thread_t  read_main_thread_id = 0;
ACE_thread_t  read_api_thread_id = 0;
ACE_thread_t  handle_logs_thread_id = 0;

int  PipeAPI  = 0;
int  PipeMain = 0;
//alogFileisActive is a global variable to store status of alogFileAdmState attribute
int alogFileisActive = 1;
//mmlSyslogisActive is a global variable to store status of mmlSyslogAdmState attribute
int mmlSyslogisActive = 0;
//mmlSyslogAvailabilityState is a global variable to store status of mmlSyslogAvailabilityState attribute
int mmlSyslogAvailabilityState = 0;
bool  IsMultiCP  = false;
bool  CmdIDused  = false;
string LastEvent;

string cmdIdString = "";

std::queue<std::string*> event_queue;
ACE_Recursive_Thread_Mutex  _acs_alog_Mutex_event_queue;

void coverPasswords (std::string & data){

	size_t lenData = data.length();
	int  crCounter = 0;

	for (size_t j=0; j<(lenData - 6); j++){
		if ((unsigned char)data[j] == '\n'){          //  the check is executed only on the first lines
			crCounter++;

			if (crCounter >= 5)
				return;
		}

		if (strncasecmp(&data[j], "PSW", 3) == 0  ||  strncasecmp(&data[j], "PWD", 3) == 0){
			j += 3;

			while (data[j] == ' ')
				j++;

			if (data[j] == '=')
				j++;

			while ((j < lenData) && (data[j] != ',') && (data[j] != ';') && ((unsigned char)data[j] != '\n')){
				data[j++] = '*';
			}
		}
	}
}


bool  checkPrintableChar (unsigned char car)
{

	if (car == 0)  return true;
	if (car == 10  ||  car == 11  ||  car == 12  ||  car == 13)  return true;
	if (car >= 32  &&  car <= 126)  return true;

	return false;
}

void  acs_alog_activeWorks::writeTargetLog (string& event)
{

	fstream  targetFile;
	long     posStart, posEnd;

	if (strlen (FileAudit) == 0)  rotateAuditLog ();
	else
		if (CurrLogGreaterThanOneMB == true)
		{
			string  oldLogFile = CurrentLogFile;
			rotateAuditLog ();

			if (CurrentLogFile != oldLogFile)
			{
				CurrLogGreaterThanOneMB = false;
				if (TransferQueue.length() > 0)      								//  A Transfer Queue is configured :  send new file to GOH
					sendOneFileToGOH (PathAudit, TransferQueue, oldLogFile);
			}
		}

	targetFile.open(FileAudit, fstream::out | fstream::app);
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(CurrentLogFile));
	if (! targetFile.is_open())
	{
		if (CurrentNodeState == "active")
		{
			string  s1 = "Error when opening ";
			string  s2 = "CAUSE: Could not open the file : ";
			s1 += CurrentLogFile;  s2 += CurrentLogFile;
			AEHEVENTMESSAGE ("acs_alogmaind", 9952, "EVENT", "AUDIT LOG FAULT", "EVENT", "APZ", s1.c_str(), s2.c_str());
		}
		return;
	}

	int  eventLength = strlen (event.c_str());

	for ( int j=0; j < eventLength; j++)
	{
		if (checkPrintableChar ((unsigned char)event.at(j)) == false)  event.at(j) = '*';
	}

	posStart = targetFile.tellp();

	targetFile << event.c_str() << "\n" << "\n";

	if (targetFile.bad())
	{
		string  s1 = "Error when writing in ";
		string  s2 = "CAUSE: Could not write to file : ";
		s1 += CurrentLogFile;  s2 += CurrentLogFile;
		AEHEVENTMESSAGE ("acs_alogmaind", 9951, "EVENT", "AUDIT LOG FAULT", "EVENT", "APZ", s1.c_str(), s2.c_str());
	}

	targetFile.flush();
	posEnd = targetFile.tellp();
	targetFile.close();

	if (posEnd < (posStart + (long)eventLength + 2))
	{
		if (truncate (FileAudit, posStart) == -1)
			ACSALOGLOG (LOG_LEVEL_FATAL,TEXTERROR("ERROR when truncating the Audit File "));
	}
	else {
		if (posEnd > ONE_MEGA)  CurrLogGreaterThanOneMB = true;
	}

}


ACE_THR_FUNC_RETURN acs_alog_activeWorks::write_file ( void* ){

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("write_file thread Start"));

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_event_queue (_acs_alog_Mutex_event_queue);
		while ( !event_queue.empty() ){
			delete event_queue.front();
			event_queue.pop();
		}
	}

	while ( !stopThEventQueue ){
		if ( !event_queue.empty() ){
			std::string* tmp;

			{
				ACE_Guard<ACE_Recursive_Thread_Mutex> guard_event_queue (_acs_alog_Mutex_event_queue);
				tmp = event_queue.front();
				event_queue.pop();
			}

			coverPasswords(*tmp);

			ACE_Guard<ACE_Recursive_Thread_Mutex> guard_auditlog_file (_acs_alog_Mutex_auditlog_file);

			std::string retWork = workSingleRecord(*tmp);
			setCommandID(tmp);
			if (retWork.compare("NOT_PARSER")){
				bool  isProtectedItem = false;

				if (PlogIsAlive && AlogIsAlive){
					if ((isProtectedItem = checkProtectedItems(retWork)) == true){
						if (CommandAndSessionPLogState){
							retWork = workSinglePlogRecord (*tmp);
							writeTargetPLog (retWork);
						}
						else{
							writeTargetPLog (retWork);
						}
					}
				}

				if (isProtectedItem == false  &&  checkExcl()){
					if(alogFileisActive && AlogIsAlive){
						writeTargetLog (retWork);
					}
					if(mmlSyslogisActive){
						redirectmmltosyslog(retWork);
					}
				}

				if (RecForCLog == 1 && AlogIsAlive){
					writeCLOGfile ();
					RecForCLog = 0;
				}
			}

			delete tmp;
		}
		else
			usleep (500000); //wait 0.5 sec

		{	// TR HW28526 - templateFormat was changed - Empty the filename variables
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard_templateFormatChange (templateFormatPendingMutex);

			if(flagTemplateFormatChangePending == true){
				sendCurrentEncryptedPLogFile();
				emptingFileNameVariables();
			}

		}
	}

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("write_file thread shut down phase"));

	struct timeval begin_time, current_time;
	gettimeofday(&begin_time, NULL);

	while ( !event_queue.empty() ){

		std::string* tmp;

		{
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard_event_queue (_acs_alog_Mutex_event_queue);
			tmp = event_queue.front();
			event_queue.pop();
		}

		coverPasswords(*tmp);

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_auditlog_file (_acs_alog_Mutex_auditlog_file);

		std::string retWork = workSingleRecord(*tmp);

		if (retWork.compare("NOT_PARSER")){
			bool  isProtectedItem = false;

			if (PlogIsAlive && AlogIsAlive){
				if ((isProtectedItem = checkProtectedItems(retWork)) == true){
					if (CommandAndSessionPLogState){
						retWork = workSinglePlogRecord (*tmp);
						writeTargetPLog (retWork);
					}
					else{
						writeTargetPLog (retWork);
					}
				}
			}

			if (isProtectedItem == false  &&  checkExcl()){
				if(alogFileisActive && AlogIsAlive){
					writeTargetLog (retWork);
				}
				if(mmlSyslogisActive){
					redirectmmltosyslog(retWork);
				}
			}

			if (RecForCLog == 1 && AlogIsAlive){
				writeCLOGfile ();
				RecForCLog = 0;
			}
		}

		delete tmp;

		gettimeofday(&current_time, NULL);

		if ( abs(current_time.tv_sec - begin_time.tv_sec) > MAX_SECONDS_BEFORE_EXIT )
			break;

	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_event_queue (_acs_alog_Mutex_event_queue);

	while ( !event_queue.empty() ){
		delete event_queue.front();
		event_queue.pop();
	}

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("write_file thread stopped"));

	return 0;
}

acs_alog_activeWorks::acs_alog_activeWorks()
{
	//noWork = 0;         commentato da PLOG
	HA = 0;
	stopThReadMain = 0;
	stopThReadApi  = 0;
	stopThHandleLogs = 0;
	stopThEventQueue = 0;
	startWrite = 0;
	SYSCALLFind = 0;
	EXECVEFind = 0;
};


acs_alog_activeWorks::acs_alog_activeWorks(string psid)
{
	sid = psid;
	//noWork = 0;         commentato da PLOG
	HA = 0;
	stopThReadMain = 0;
	stopThReadApi  = 0;
	stopThHandleLogs = 0;
	stopThEventQueue = 0;
	startWrite = 0;
	SYSCALLFind = 0;
	EXECVEFind = 0;
};


ACS_ALOG_ReturnType acs_alog_activeWorks::stopThread()
{

	ACSALOGLOG (LOG_LEVEL_INFO,TEXTERROR("Stop all working threads"));

	stopThReadMain   = 1;
	stopThReadApi    = 1;
	stopThHandleLogs = 1;
	stopThEventQueue = 1;

	if (transferqueue_checker_thread_id != 0)
	{
		// if not yet terminated, cancel the thread asynchronously
		ACE_Thread_Manager::instance()->cancel(transferqueue_checker_thread_id, 1);
		ACE_Thread_Manager::instance()->join(transferqueue_checker_thread_id);	// reclaim thread resources
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Transfer Queue Checker thread joined"));
		transferqueue_checker_thread_id = 0;
	}

	if (read_main_thread_id != 0)
	{
		ACE_Thread_Manager::instance()->join(read_main_thread_id);
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("watch_syslog_pipe thread joined"));
		read_main_thread_id = 0;
	}

	if (read_api_thread_id != 0)
	{
		ACE_Thread_Manager::instance()->join(read_api_thread_id);
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("watch_api_pipe thread joined"));
		read_api_thread_id = 0;
	}

	if (handle_logs_thread_id != 0)
	{
		ACE_Thread_Manager::instance()->join(handle_logs_thread_id);
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Handle log thread joined"));
		handle_logs_thread_id = 0;
	}

	if (cease_alarms_thread != 0)
	{
		ACE_Thread_Manager::instance()->join(cease_alarms_thread);
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Cease alarms thread joined"));
		cease_alarms_thread = 0;
	}

	if ( write_file_thread_id != 0 ){
		ACE_Thread_Manager::instance()->join(write_file_thread_id);
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Write file queue thread joined"));
		write_file_thread_id = 0;
	}

	if (! HA)  delete alogNodeState;

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType acs_alog_activeWorks::MultipleCpCheck()
{
	/*
	 * invoke the ACS_CS_API to know if we're executing
	 * on a SingleCP System or on a MultipleCP System
	 */

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::isMultipleCPSystem(MCpSystem);

	if (MCpSystem == true)   ACSALOGLOG (LOG_LEVEL_INFO,TEXTERROR(":: MCpSystem is TRUE"));
	if (MCpSystem == false)  ACSALOGLOG (LOG_LEVEL_INFO,TEXTERROR(":: MCpSystem is FALSE"));

	if(returnValue != ACS_CS_API_NS::Result_Success)
	{
		switch(returnValue)
		{
		case ACS_CS_API_NS::Result_NoAccess:
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(ERR56));
			return ACS_ALOG_error;
		case ACS_CS_API_NS::Result_NoEntry:
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(ERR56));
			return ACS_ALOG_error;
		case ACS_CS_API_NS::Result_NoValue:
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(ERR56));
			return ACS_ALOG_error;
		case ACS_CS_API_NS::Result_Failure:
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(ERR56));
			return ACS_ALOG_error;
		default:
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(ERR56));
			return ACS_ALOG_error;
		}
	}

	return ACS_ALOG_ok;
}

ACS_ALOG_ReturnType acs_alog_activeWorks::alogInit()
{
	acs_alog_cmdHandler  cmdAction;
	char* nodeAddrOfSide;

	if (MultipleCpCheck())
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Single or Multi CP not defined"));
		return ACS_ALOG_error;
	}

	if (readParameter("axeFunctionsId=1","ap"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  ap par"));
		return ACS_ALOG_error;
	}

	if (setLocalHostName())
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed!"));
		return ACS_ALOG_error;
	}

	if (readParameter(AUDITDNNAME,"alogAdmState"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  alogAdmState"));
		return ACS_ALOG_error;
	}
	cmdAction.setAlogStatus(AlogIsAlive);

	if (readParameter(AUDITDNNAME,"transferQueue"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  transferQueue"));
		return ACS_ALOG_error;
	}
	string strToLog = "ALOG Transfer Queue :  TQ-name = ";
	strToLog.append(TransferQueue);
	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(strToLog.c_str()));

	if (readParameter(AUDITDNNAME,"commandLogTransferQueue")){
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  commandLogTransferQueue"));
		return ACS_ALOG_error;
	}

	strToLog = "ALOG Transfer Queue ( CLOG ) : TQ-name = ";
	strToLog.append(commandLogTransferQueue);
	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(strToLog.c_str()));

	// if Transfer Queue is defined, check it and update "transferQueueDn" in IMM.
	// Do this starting a separate thread, because the operation can take much time
	if ((TransferQueue != "") && (commandLogTransferQueue != "") && startTransferQueueChecker())		// start the transfer queue validator thread
	{
		syslog (LOG_ERR, "Start thread 'Transfer Queue Validator' failed");
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Start thread 'Transfer Queue Validator' failed"));
	}

	if (readParameter(AUDITDNNAME,"largeEventDataSkipped"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  largeEventDataSkipped"));
		return ACS_ALOG_error;
	}

	if (readParameter(AUDITDNNAME,"commandAndSessionAdmState"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  commandAndSessionAdmState"));
		return ACS_ALOG_error;
	}

	if (readParameter(AUDITDNNAME,"templateFormat"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  templateFormat"));
		return ACS_ALOG_error;
	}
	/*TR:HY25168 changes start*/
	if(strcmp(acs_alog_activeWorks::local_hostname.c_str(),"SC-2-1")==0)
	{
		nodeAddrOfSide = NODE_A_IP_ADDRESS;
	}
	else if(strcmp(acs_alog_activeWorks::local_hostname.c_str(),"SC-2-2")== 0)
	{
		nodeAddrOfSide = NODE_B_IP_ADDRESS;
	}

	if (readParameter("northBoundId=1,networkConfigurationId=1",nodeAddrOfSide))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  nodeAddrOfSide"));
		return ACS_ALOG_error;
	}
	/*TR:HY25168 changes End*/

	if (cmdAction.getExclCmd())
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed"));
		return ACS_ALOG_error;
	}
	if (readParameter(AUDITDNNAME,"alogFileAdmState"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  alogFileAdmState"));
		return ACS_ALOG_error;
	}

	if (readParameter(AUDITDNNAME,"mmlSyslogAdmState"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  mmlSyslogAdmState"));
		return ACS_ALOG_error;
	}
	if (readParameter(AUDITDNNAME,"mmlSyslogAvailabilityState"))
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed :  mmlSyslogAvailabilityState"));
		return ACS_ALOG_error;
	}

	if (APtype == 2)            //  PLOG is not installed on AP2 node
	{
		PlogIsAlive = 0;
		return ACS_ALOG_ok;
	}

	if(! CheckALOGSysFile(false,false))
	{
		PlogIsAlive = 1;
	}

	/*START - Added to avoid failure of file creation before transition to active status**/
	string alogDir;
	getAlogDataDiskPath(alogDir);
	alogDir += "/ACS_ALOG";
	bool alogDirCreated = false;
	alogDirCreated = createDir(alogDir);
	if (alogDirCreated)
		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("alogDirCreated!!!"));
	/*END -Added */
	switch(FetchInfoATTRFILE())
	{
	case 0:
		break;
	case 1:
		OpenAttrFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9924, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access ATTRFILEP file");
		break;
	case 2:
		ReadAttrFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9925, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when reading a system-file",
				"FCODE: 6\nCAUSE: Could not read ATTRFILEP file");
		break;
	}

	switch(FetchInfoSYS())
	{
	case 0:
		break;
	case 1:
		OpenSysLogError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9929, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access SYS file");
		break;
	case 2:
		ReadSysLogError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9938, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when reading a system-file",
				"FCODE: 6\nCAUSE: Could not read SYS file");
		break;
	}

	switch (FetchInfoPMMLCMD())
	{
	case 0:
		break;
	case 1:
		OpenPMMCMDFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9930, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access PMMLCMD file");
		break;
	case 2:
		ReadPMMLCMDFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9936, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when reading a system-file",
				"FCODE: 6\nCAUSE: Could not read PMMLCMD file");
		break;
	}

	switch (FetchInfoPMMLPRT())
	{
	case 0:
		break;
	case 1:
		OpenPMMLPRTFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9931, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access PMMLPRT file");
		break;
	case 2:
		ReadPMMLPRTFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9937, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when reading a system-file",
				"FCODE: 6\nCAUSE: Could not read PMMLPRT file");
		break;
	}

	switch (FetchInfoPUSR())
	{
	case 0:
		break;
	case 1:
		OpenPUSRFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9939, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when opening a system-file",
				"FCODE: 5\nCAUSE: Could not access PUSR file");
		break;
	case 2:
		ReadPUSRFileError = 1;
		AEHEVENTMESSAGE ("acs_alogmaind", 9940, "A2", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "Error when reading a system-file",
				"FCODE: 6\nCAUSE: Could not read PUSR file");
		break;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType acs_alog_activeWorks::alogWorks()
{

	if (! HA)  alogNodeState = new (std::nothrow) acs_alog_nodestate();

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("::alogWorks Start"));

	if (startWatchSyslogPipe())		// start the audit of events from main pipe
	{
		syslog (LOG_ERR, "Start thread Read Main failed");
		stopThread();
		return ACS_ALOG_error;
	}

	if (startWriteFile())
	{
		syslog (LOG_ERR, "Start thread Write file failed");
		stopThread();
		return ACS_ALOG_error;
	}

	if (startWatchApiPipe())		// start the audit of events from API pipe
	{
		syslog (LOG_ERR, "Start thread Read Api failed");
		stopThread();
		return ACS_ALOG_error;
	}

	if (startHandleLogFiles())		// start the handling of the audit files  (Transfer Queue or not)
	{
		syslog (LOG_ERR, "Start thread Handle Logs failed");
		stopThread();
		return ACS_ALOG_error;
	}

	if(startHandleToCeaseAlarms())
	{
		syslog (LOG_ERR, "Start thread of CeaseAlarmCheck failed");
		stopThread();
		return ACS_ALOG_error;
	}

	if (! HA)
	{
		if (alogNodeState->startWatchForNodeState())		// start watch for node state changes
		{
			syslog (LOG_ERR, "Start thread Watch Node State failed");
			stopThread();
			return ACS_ALOG_error;
		}
	}

	return ACS_ALOG_ok;
}


void  acs_alog_activeWorks::writeCLOGfile (){

	DIR  *handle;
	struct dirent  *entry;

	std::list<string> clogFiles;

	string  fName, fileToSendToGOH;
	bool    newFileCreated = false;

	clogFiles.clear();

	if ((handle = opendir (PathCLog)) == NULL){
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error :  CLOG folder doesn't exist !"));
		return;
	}

	while ((entry = readdir(handle)) != NULL){
		fName = entry->d_name;
		if (fName.substr(0,8).compare("CLogfile") == 0)
			clogFiles.push_back(fName);
	}
	closedir (handle);

	clogFiles.sort();

	if (clogFiles.size() > 0)
		fileToSendToGOH = fName = clogFiles.back();
	else {
		fName = createCLOGfile ();
		newFileCreated = true;
	}

	fName = PathCLog + fName;

	if (checkCLOGfileSize (fName.c_str(), ONE_MEGA) == true){
		fName = createCLOGfile ();
		fName = PathCLog + fName;
		newFileCreated = true;
	}

	if (newFileCreated == false){
		string  currEvent = CLOGevent;

		stringToUpper (currEvent);

		if (currEvent.find("SYBUP") != string::npos){
			fName = createCLOGfile ();
			fName = PathCLog + fName;
			newFileCreated = true;
		}
	}

	if ( newFileCreated && commandLogTransferQueue.length() > 0 ){ // A new CLOG file will be created, send the old one to GOH
		boost::system::error_code copyResult;
		boost::filesystem::copy_file(std::string(PathCLog).append(fileToSendToGOH), std::string(clusterLogsTQFolder).append(fileToSendToGOH), boost::filesystem::copy_option::overwrite_if_exists, copyResult);
		if ( copyResult.value() == boost::system::errc::success )
			sendOneClusterLogFiletoGOH(fileToSendToGOH, commandLogTransferQueue, clusterLogsTQFolder);
		else {
			char strError[512] = {0};
			snprintf(strError,sizeof(strError),"ERROR when coping the CLOG File (%s) - Error %i(%s)",std::string(PathCLog).append(fileToSendToGOH).c_str(),copyResult.value(),copyResult.message().c_str());
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(strError));
		}
	}

	fstream  clogFile;
	clogFile.open (fName.c_str(), fstream::out | fstream::app);

	if (! clogFile.is_open()){
		string  ss1 = "Error when opening ";
		ss1 = ss1 + fName;
		string  ss2 = "CAUSE: Could not open the file ";
		ss2 = ss2 + fName;

		AEHEVENTMESSAGE ("acs_alogmaind", 9954, "EVENT", "AUDIT LOG FAULT", "EVENT", "APZ", ss1.c_str(), ss2.c_str());
		return;
	}

	long  posStart, posEnd;

	posStart = clogFile.tellp();

	size_t eventlength = CLOGevent.length();
	for (size_t j=0; j < eventlength; j++){
		if (checkPrintableChar ((unsigned char)CLOGevent.at(j)) == false)
			CLOGevent.at(j) = '*';
	}

	clogFile << CLOGevent.c_str();

	if (clogFile.bad()){
		string  ss1 = "Error when writing in ";
		ss1 = ss1 + fName;
		string  ss2 = "CAUSE: Could not write to file ";
		ss2 = ss2 + fName;

		AEHEVENTMESSAGE ("acs_alogmaind", 9953, "EVENT", "AUDIT LOG FAULT", "EVENT", "APZ", ss1.c_str(), ss2.c_str());
	}

	clogFile.flush();
	posEnd = clogFile.tellp();
	clogFile.close();

	if (posEnd < (posStart + (long)CLOGevent.length()))
	{
		if (truncate (fName.c_str(), posStart) == -1)
		{
			ACSALOGLOG (LOG_LEVEL_FATAL,TEXTERROR("ERROR when truncating the CLOG File "));
		}
	}
}


void  acs_alog_activeWorks::writeTargetPLog (string& event_new)
{
	//The entire function is changed due to High memory of ALOG in HLR provisioning traffic /*HX86564*/

	if (FilePAudit.length() == 0)
	{
		rotatePAuditLog ();
	}
	else  if (CurrPLogGreaterThanOneMB == true)
	{
		string  oldLogFile = CurrentPLogFile;
		rotatePAuditLog ();
		CurrPLogGreaterThanOneMB = false;
		if (PLOGTransferQueue.length() > 0)      //  A Transfer Queue is configured :  send new files to GOH
		{
			sendOnePlogFileToGOH (PathPAudit, PLOGTransferQueue, oldLogFile);
		}
	}
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogZip (_acs_alog_ZipPlog_Mutex);

	struct stat fileStat;
	string decrypted_str ="";
	string encrypted_str ="";
	event_new += "\n\n";
	encrypted_str = encryptString(event_new);
	fstream  sourceFile;
	long posEnd;

	sourceFile.open (FilePName.c_str(), fstream::out | fstream::app);
	if (!sourceFile.is_open())
	{
		unlink (FilePName.c_str());
		return;
	}
	sourceFile << encrypted_str << "\n";
	if (sourceFile.bad())
	{
		unlink (FilePName.c_str());
		return;
	}
	sourceFile.flush();
	posEnd = sourceFile.tellp();
	sourceFile.close();

	if (posEnd > TWO_MEGA){

		std::ifstream ifs(FilePName.c_str());//change to filepname
		std::string content( (std::istreambuf_iterator<char>(ifs) ),
				(std::istreambuf_iterator<char>()    ) );
		long int str_len = strlen(content.c_str());
		size_t reference = 0;
		size_t found = 0;
		string decrypted_event_sub= "";
		while(reference < str_len)
		{
			found = content.find("\n",reference);
			decrypted_event_sub = content.substr(reference, found-reference);
			event += decryptString(decrypted_event_sub);
			reference = found+1;
		}
		if(reference == str_len)
			unlink (FilePName.c_str());//take care of it after zipping and putting the file in /protected_logs

	}
	else{
		return;
	}
	ACE_stat  statBuff;
	DIR      *handle;
	//int  exitCode;

	string pwdFor7zExtractionInteractive = protectedPassword + "\n";
	string pwdFor7zArchiveInteractive = pwdFor7zExtractionInteractive + pwdFor7zExtractionInteractive;

	if (checkHardLimit (event.length()) == 0)     //  HARD LIMIT check
	{
		ACSALOGLOG (LOG_LEVEL_INFO,TEXTERROR("HARD LIMIT check :  PLOG event not logged !"));
		ACSALOGLOG (LOG_LEVEL_TRACE,TEXTERROR("Mutex is released as HARD LIMIT check :  PLOG event not logged !"));
		return;
	}

	if ((handle = opendir (PathPAudit)) != NULL)
	{
		if (OpenPLogDirError == 1)
		{
			AEHEVENTMESSAGE ("acs_alogmaind", 9935, "CEASING", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "", "");
			OpenPLogDirError = 0;
		}
		closedir (handle);
	}
	else {
		if (OpenPLogDirError == 0)
		{
			AEHEVENTMESSAGE ("acs_alogmaind", 9935, "A2", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "Error when opening the protected log-file directory",
					"FCODE: 17\nCAUSE: Could not open the protected log-file directory");
			OpenPLogDirError = 1;
		}
		return;
	}

	bool commandExecuted = false;
	int retCode_7z_archive = 0;

	fstream  targetFile;
	targetFile.open (FilePName.c_str(), fstream::out | fstream::app);

	if (! targetFile.is_open())
	{
		unlink (FilePName.c_str());

		if (WritePlogError == 0)
		{
			string  s = "FCODE: 15\nCAUSE: Could not write to file : ";
			s += CurrentPLogFile;
			AEHEVENTMESSAGE ("acs_alogmaind", 9933, "A2", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "Error when writing a protected log-file", s.c_str());
			WritePlogError = 1;
		}
		ACSALOGLOG (LOG_LEVEL_FATAL,TEXTERROR("Error opening /tmp PLOG file"));
		return;
	}

	size_t str_len = event.length();

	for (size_t j=0; j < str_len; j++)
	{
		if (checkPrintableChar ((unsigned char)event.at(j)) == false)  event[j] = '*';
	}
	targetFile << event.c_str();
	if (targetFile.bad())
	{
		targetFile.close();

		unlink (FilePName.c_str());

		if (WritePlogError == 0)
		{
			string  s = "FCODE: 15\nCAUSE: Could not write to file : ";
			s += CurrentPLogFile;
			AEHEVENTMESSAGE ("acs_alogmaind", 9933, "A2", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "Error when writing a protected log-file", s.c_str());
			WritePlogError = 1;
		}
		ACSALOGLOG (LOG_LEVEL_FATAL,TEXTERROR("Error writing in /tmp PLOG file"));
		return;
	}

	targetFile.flush();  targetFile.close();

	string  cmdZip = "/usr/bin/7z a -tzip -mx=0 -mem=AES256 " + FilePAudit + " " + FilePName + " -p";

	commandExecuted = false;
	commandExecuted = execute7zCmd(cmdZip, pwdFor7zArchiveInteractive, retCode_7z_archive);         // TR HX49170
	pwdFor7zArchiveInteractive.clear();

	if (!commandExecuted || !WIFEXITED(retCode_7z_archive) || (WEXITSTATUS(retCode_7z_archive) != 0))
	{
		unlink (FilePName.c_str());

		if (WritePlogError == 0)
		{
			string  s = "FCODE: 15\nCAUSE: Could not write to file : ";
			s += CurrentPLogFile;
			AEHEVENTMESSAGE ("acs_alogmaind", 9933, "A2", "AUDIT LOG FAULT",
					"APZ", "acs_alogmaind/Alarm", "Error when writing a protected log-file", s.c_str());
			WritePlogError = 1;
		}
		string  msg = "7z archive command failed :  code = ";
		char  code[10];
		sprintf (code, "   %03d", WEXITSTATUS(retCode_7z_archive));
		msg = msg + code;
		ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(msg));
		return;
	}
	unlink (FilePName.c_str());

	event.clear();

	if (WritePlogError == 1)
	{
		AEHEVENTMESSAGE ("acs_alogmaind", 9933, "CEASING", "AUDIT LOG FAULT",
				"APZ", "acs_alogmaind/Alarm", "", "");
		WritePlogError = 0;
	}

	ACE_OS::stat (FilePAudit.c_str(), &statBuff);
	long  posAudit = statBuff.st_size;

	if (posAudit > ONE_MEGA)
		CurrPLogGreaterThanOneMB = true;
	else
		CurrPLogGreaterThanOneMB = false;

}


ACS_ALOG_ReturnType  acs_alog_activeWorks::startTransferQueueChecker()
{
	const ACE_TCHAR  *thread_name = "TransferQueue_Checker_Thread";

	// launch a new thread, setting its 'cancelability type' to 'THR_CANCEL_ASYNCHRONOUS', in order to be able to cancel the thread asynchronously
	int  call_result = ACE_Thread_Manager::instance()->spawn ( & transferqueue_checker_svc,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED | THR_CANCEL_ASYNCHRONOUS,
			&transferqueue_checker_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);

	if (call_result == -1)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error creating Transfer queue checker thread"));
		return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType  acs_alog_activeWorks::startWatchSyslogPipe()
{

	const ACE_TCHAR  *thread_name = "Read_Main_Thread";

	int  call_result = ACE_Thread_Manager::instance()->spawn (&watch_syslog_pipe,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&read_main_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);
	if (call_result == -1)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error creating Thread read main"));
		return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType  acs_alog_activeWorks::startWatchApiPipe()
{
	const ACE_TCHAR  *thread_name = "Read_Api_Thread";

	int  call_result = ACE_Thread_Manager::instance()->spawn (&watch_api_pipe,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&read_api_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);
	if (call_result == -1)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error creating Thread read api"));
		return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType  acs_alog_activeWorks::startWriteFile()
{
	const ACE_TCHAR  *thread_name = "Write_File_Thread";

	int  call_result = ACE_Thread_Manager::instance()->spawn (&write_file,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&write_file_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);
	if (call_result == -1)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error creating Thread read api"));
		return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType  acs_alog_activeWorks::startHandleToCeaseAlarms()
{

	const ACE_TCHAR  *thread_name = "Cease_Alarm_Thread";

	int  call_result = ACE_Thread_Manager::instance()->spawn (&ceaseCheck,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&cease_alarms_thread,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);

	if (call_result == -1)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error creating Thread write"));
		return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType  acs_alog_activeWorks::startHandleLogFiles()
{

	const ACE_TCHAR  *thread_name = "Handle_LogFiles_Thread";

	int  call_result = ACE_Thread_Manager::instance()->spawn (&handle_log_files,
			(void *)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&handle_logs_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&thread_name);
	if (call_result == -1)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error creating Thread write"));
		return ACS_ALOG_error;
	}

	return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType acs_alog_activeWorks::setLocalHostName()
{
	char  buff_host[16];

	if (gethostname(buff_host,sizeof(buff_host)))
	{
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Host name not found"));
		return ACS_ALOG_error;
	}
	local_hostname = buff_host;

	return ACS_ALOG_ok;
}


ACE_THR_FUNC_RETURN acs_alog_activeWorks::transferqueue_checker_svc(void*)
{
	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Transfer Queue Checker thread Start"));

	while(true)
	{
		if (getTransferQueueDn(TransferQueue) == 1){
			ACS_CC_ImmParameter  attr;
			char  par[] = "transferQueueDn";
			void *attrValuesPtrs[1] = {0};

			attr.attrName = par;
			attr.attrType = ATTR_NAMET;
			attr.attrValuesNum = 1;
			attr.attrValues = attrValuesPtrs;
			attr.attrValues[0] = reinterpret_cast<void*> (const_cast<char*> (TQdn.c_str()));

			int call_result = implementer_AL.modifyRuntimeObj (AUDITDNNAME, &attr);
			char msg[256];
			if(call_result == 0){
				sprintf(msg,"The attribute 'transferQueueDn' of 'AxeAuditLoggingauditLoggingMId=1' object has been successfully set");
				ACSALOGLOG(LOG_LEVEL_DEBUG,TEXTERROR(msg));

				//break; 		// terminate the thread
			}
			else{
				sprintf(msg,"Unable to set the attribute 'transferQueueDn' of 'AxeAuditLoggingauditLoggingMId=1' object ! 'modifyRuntimeObj' returned %d", call_result);
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(msg));
			}
		}
		else{
			char msg[256];
			sprintf(msg,"Unable to get Transfer Queue DN via OHI API!");
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(msg));
		}

		if (getcommandLogTransferQueueDn(commandLogTransferQueue) == 1){
			ACS_CC_ImmParameter  attr;
			char  par[] = "commandLogTransferQueueDn";
			void *attrValuesPtrs[1] = {0};

			attr.attrName = par;
			attr.attrType = ATTR_NAMET;
			attr.attrValuesNum = 1;
			attr.attrValues = attrValuesPtrs;
			attr.attrValues[0] = reinterpret_cast<void*> (const_cast<char*> (commandLogTQdn.c_str()));

			int call_result = implementer_AL.modifyRuntimeObj (AUDITDNNAME, &attr);
			char msg[256];
			if(call_result == 0){
				sprintf(msg,"The attribute 'commandLogTransferQueueDn' of 'AxeAuditLoggingauditLoggingMId=1' object has been successfully set");
				ACSALOGLOG(LOG_LEVEL_DEBUG,TEXTERROR(msg));

				break; 		// terminate the thread
			}
			else{
				sprintf(msg,"Unable to set the attribute 'commandLogTransferQueueDn' of 'AxeAuditLoggingauditLoggingMId=1' object ! 'modifyRuntimeObj' returned %d", call_result);
				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(msg));
			}
		}
		else{
			char msg[256];
			sprintf(msg,"Unable to get Transfer Queue DN via OHI API!");
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(msg));
		}





		sleep(10); // wait somehow and retry
	}

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Transfer Queue Checker thread Stop"));
	return 0;
}


ACE_THR_FUNC_RETURN acs_alog_activeWorks::watch_syslog_pipe(void*)
{
	int     num = 0;
	char    buffer[MAXLEN_SYSLOG];
	char    singleEve[MAXLEN_SYSLOG];
	string  retWork;
	struct timespec  req;
	req.tv_sec  = 0;
	req.tv_nsec = WAITREAD * 1000000L;

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("watch_syslog_pipe thread Start"));

	if ((PipeMain = open(SOURCEMAIN_P, O_RDONLY | O_NONBLOCK)) < 0)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Pipe Main wrong opening"));
		return 0;
	}

	while (! stopThReadMain)
	{
		if ((num = ACE_OS::read(PipeMain, buffer, sizeof(buffer))) < 0)
		{
			//				ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Pipe Main read error"));
			nanosleep (&req, (struct timespec *)NULL);
			continue;
		}

		if (num > 0  &&  num < MAXLEN_SYSLOG  &&  acs_alog_activeWorks::noWork  &&  AlogIsAlive)
		{
			buffer[num] = 0;

			int  j, idx = 0;
			int  lenEve = strlen(buffer);

			while ((lenEve - idx) > 5)
			{
				j = 0;
				while (buffer[idx] != '\n'  &&  idx < lenEve)  singleEve[j++] = buffer[idx++];

				singleEve[j++] = '\n',  idx ++;
				singleEve[j] = '\0';

				ACE_Guard<ACE_Recursive_Thread_Mutex> guard_auditlog_file (_acs_alog_Mutex_auditlog_file);

				retWork = workSingleRecord((string) singleEve);

				if (retWork.compare("NOT_PARSER"))
				{
					checkPasswordInCommand ((char *)retWork.c_str());

					bool  IsProtectedItem = false;
					if (PlogIsAlive)
					{
						IsProtectedItem = checkProtectedCmdItems();       //  TR :  HR37487

						if (IsProtectedItem == true)  writeTargetPLog (retWork);
					}

					if (IsProtectedItem == false  &&  checkExcl())
					{
						if(alogFileisActive){
							writeTargetLog (retWork);
						}
					}
				}
			}
		}

		nanosleep (&req, (struct timespec *)NULL);
	}
	close (PipeMain);
	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("watch_syslog_pipe thread Stop"));
	return 0;
}


ACE_THR_FUNC_RETURN acs_alog_activeWorks::watch_api_pipe(void*){

	int   counter = 0;

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("watch_api_pipe thread Start"));
	bool  isProtectedItem = false;
	::unlink(PIPE_NAME);

	ACE_UNIX_Addr addr(PIPE_NAME);
	ACE_LSOCK_Acceptor Acceptor;
	ACE_LSOCK_Stream stream;

	while ( Acceptor.open(addr) != 0 ) {
		int error = ACE_OS::last_error();

		char str_to_log[1024] = {0};

		if ( counter++ >= MAX_ERROR_OPEN_API_PIPE ){

			snprintf(str_to_log,sizeof(str_to_log),"Pipe Api wrong opening - errno = %d. Thread exit with error",error);
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(str_to_log));

			Acceptor.remove();
			return 0;
		}

		snprintf(str_to_log,sizeof(str_to_log),"Pipe Api wrong opening - errno = %d, retry %d",error,counter+1);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(str_to_log));

		usleep(500000); //wait 0,5 seconds before retry a new open
	}

	while (! stopThReadApi) // infinite loop until the thread has not been stopped
	{
		ACE_Time_Value val(5);

		if ( Acceptor.accept(stream,0,&val,0) != 0 ){

			int error = ACE_OS::last_error();

			stream.close();

			if (( error == EINTR ) || ( error == ETIMEDOUT ) || ( error == ETIME ))
				continue;

			char str_to_log[1024] = {0};
			snprintf(str_to_log,sizeof(str_to_log),"%s - accept failed - errno = %d",__PRETTY_FUNCTION__, error );
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(str_to_log));
			Acceptor.remove();

			usleep(200000); // wait 0.2 second
			Acceptor.open(addr);

			continue;
		}

		uint32_t byte_to_read = 0;

		if ( stream.recv ( &byte_to_read, MSG_SIZE_FIELD_LENGTH ) < MSG_SIZE_FIELD_LENGTH ){
			char str_to_log[1024] = {0};
			snprintf(str_to_log,sizeof(str_to_log),"%s - recv failed errno = %d, skip event",__PRETTY_FUNCTION__,ACE_OS::last_error());
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(str_to_log));

			stream.close();
			continue;
		}

		byte_to_read = ::ntohl(byte_to_read);

		char* buffer_from_lsock = new (std::nothrow) char[byte_to_read+1];

		if (!buffer_from_lsock){
			char str_to_log[1024] = {0};
			snprintf(str_to_log,sizeof(str_to_log),"%s - buffer allocation failed errno = %d, skip event",__PRETTY_FUNCTION__,ACE_OS::last_error());
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(str_to_log));

			stream.close();
			continue;
		}

		int byte_read = stream.recv_n (buffer_from_lsock, byte_to_read);

		if (byte_read > 0  &&  acs_alog_activeWorks::noWork && (AlogIsAlive | mmlSyslogisActive)){
			buffer_from_lsock[byte_read-1] = '\n';
			buffer_from_lsock[byte_read] = 0;
			std::string* tmp_str = new (std::nothrow) std::string(buffer_from_lsock);
			//setCommandID(tmp_str);
			if (!tmp_str){
				char str_to_log[1024] = {0};
				snprintf(str_to_log,sizeof(str_to_log),"%s - string allocation failed errno = %d, skip event",__PRETTY_FUNCTION__,ACE_OS::last_error());
				ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(str_to_log));

				stream.close();	delete [] buffer_from_lsock;
				continue;
			}

			ACE_Guard<ACE_Recursive_Thread_Mutex> guard_event_queue (_acs_alog_Mutex_event_queue);
			event_queue.push( tmp_str );
		}
		stream.close();	delete [] buffer_from_lsock;
	}

	Acceptor.remove();

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("watch_api_pipe thread Stop"));

	return 0;
}


ACE_THR_FUNC_RETURN  acs_alog_activeWorks::ceaseCheck(void*)
{
	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("cease_alarms thread Start"));

	acs_alog_activeWorks  CeaseObj;
	int  ErrorCode;
	int  seconds = 0;

	while (! stopThHandleLogs)
	{
		if (seconds < 20)       			//  every 20 sec the alarms on PLOG files are checked
		{
			sleep (1);       			//  wait only 1 sec to check frequently if the thread has been stopped
			seconds = seconds + 1;
			continue;
		}

		if (OpenSysLogError  ||  ReadSysLogError)
		{
			ErrorCode = CeaseObj.FetchInfoSYS();
			if(ErrorCode == 0)
			{
				AEHEVENTMESSAGE ("acs_alogmaind", 9929, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				AEHEVENTMESSAGE ("acs_alogmaind", 9938, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				OpenSysLogError = 0;
				ReadSysLogError = 0;
			}
		}

		if (OpenAttrFileError  ||  ReadAttrFileError)
		{
			ErrorCode = CeaseObj.FetchInfoATTRFILE();
			if(ErrorCode == 0)
			{
				AEHEVENTMESSAGE ("acs_alogmaind", 9924, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");

				AEHEVENTMESSAGE ("acs_alogmaind", 9925, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");

				OpenAttrFileError = 0;
				ReadAttrFileError = 0;
			}
		}

		if (OpenPMMLPRTFileError  ||  ReadPMMLPRTFileError)
		{
			ErrorCode = CeaseObj.FetchInfoPMMLPRT();
			if(ErrorCode == 0)
			{
				AEHEVENTMESSAGE ("acs_alogmaind", 9931, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				AEHEVENTMESSAGE ("acs_alogmaind", 9937, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				OpenPMMLPRTFileError = 0;
				ReadPMMLPRTFileError = 0;
			}
		}

		if (OpenPMMCMDFileError  ||  ReadPMMLCMDFileError)
		{
			ErrorCode = CeaseObj.FetchInfoPMMLCMD();
			if(ErrorCode == 0)
			{
				AEHEVENTMESSAGE ("acs_alogmaind", 9930, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				AEHEVENTMESSAGE ("acs_alogmaind", 9936, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				OpenPMMCMDFileError = 0;
				ReadPMMLCMDFileError = 0;
			}
		}

		if (OpenPUSRFileError  ||  ReadPUSRFileError)
		{
			ErrorCode = CeaseObj.FetchInfoPUSR();
			if(ErrorCode == 0)
			{
				AEHEVENTMESSAGE ("acs_alogmaind", 9939, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				AEHEVENTMESSAGE ("acs_alogmaind", 9940, "CEASING", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "", "");
				OpenPUSRFileError = 0;
				ReadPUSRFileError = 0;
			}
		}

		if (AttachPLTQerror == 1  &&  PLOGTransferQueue.length() == 0)       //  TR :  HS11854
		{
			AEHEVENTMESSAGE ("acs_alogmaind", 9927, "CEASING",
					"DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
			AttachPLTQerror = 0;
		}
		seconds = 0;
	}

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("cease_alarms thread Stop"));
	return 0;
}


ACE_THR_FUNC_RETURN  acs_alog_activeWorks::handle_log_files(void*)
{

	int  seconds = 150;         //  the first check is done after 30 seconds

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("handle_log_files thread Start"));

	while (! stopThHandleLogs)
	{
		if (seconds < 180)       			//  every 3 min the global Logs size is checked
		{
			sleep (1);       			//  wait only 1 sec to check frequently if the thread has been stopped
			seconds = seconds + 1;
			continue;
		}

		if (acs_alog_activeWorks::noWork  &&  AlogIsAlive  &&  CurrentNodeState == "active"  &&  strlen(PathAudit) > 0)
		{
			//				checkAlogWinFiles (PathAudit, TransferQueue);		for TurkCell issue is not required the Files Renaming

			if (TransferQueue.length() > 0)      //  A Transfer Queue is configured :  send new files to GOH (if any)
			{
				sendFilesToGOH (PathAudit, TransferQueue);
			}
			else {
				handleLoggingDirSize (PathAudit, ONE_GIGA);		//  ALOG checks if the size of the Audit Logging Dir
				//  is larger than 1 GigaBytes and deletes the old files
			}

			if (commandLogTransferQueue.length() > 0){
				ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("sendClusterLogFiletoGOH called"));
				sendClusterLogFiletoGOH(PathCLog, commandLogTransferQueue);
			}

			handleCLogDirSize (PathCLog, HUNDRED_MEGA);		//  ALOG checks if the size of the CLOG Dir
			//  is larger than 100 MegaBytes and deletes the old files
		}

		if (acs_alog_activeWorks::noWork  &&  PlogIsAlive  &&  CurrentNodeState == "active"  &&  strlen(PathPAudit) > 0)
		{
			//				checkPlogWinFiles (PathPAudit, PLOGTransferQueue);		for TurkCell issue is not required the Files Renaming

			if (PLOGTransferQueue.length() > 0)      //  A Transfer Queue is configured :  send new files to GOH (if any)
			{
				sendPlogFilesToGOH (PathPAudit, PLOGTransferQueue);
			}
			else {
				handleProtectedLoggingDirSize (PathPAudit, HUNDRED_MEGA);	//  PLOG checks if the size of the Logging Dir
				//  is larger than 100 MegaBytes and deletes the old files
			}
		}
		seconds = 0;
	}

	ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("handle_log_files thread Stop"));
	return 0;
}


void  acs_alog_activeWorks::checkPasswordInCommand (char *command)
{
	bool  pwdFound = false;
	bool  mmlFound = false;

	int command_lengh = strlen(command);

	for (int j = 0; j < command_lengh - 6; j++){
		if (strncasecmp(&command[j], "MML", 3) == 0)
		{
			mmlFound = true;
			break;
		}
	}
	if (mmlFound == false)  return;

	for (int j=0; j<(int)strlen(command)-4; j++)
	{
		if (strncasecmp(&command[j], "PSW", 3) == 0  ||  strncasecmp(&command[j], "PWD", 3) == 0)
		{
			j = j + 3;

			while (command[j] == ' ')  j ++;
			if (command[j] == '=')
			{
				j ++;  pwdFound = true;
			}

			if (pwdFound == true)
			{
				while (command[j] != ' '  &&  command[j] != ','  &&  command[j] != ';'  &&  command[j] != '\n')
				{
					command[j] = '*';  j ++;
				}
				return;
			}
		}
	}
}


string acs_alog_activeWorks::workSingleRecord(const std::string& buffer)
{

	string  retParse = "NOT_PARSER";
	size_t  found = 0;
	size_t  foundCmd = 0;
	string  noStr = "NO_STRING";

	foundCmd = buffer.find("apcommand.notice:") ;
	if (foundCmd == string::npos)
	{
		eParse.set_BufStrOne (buffer);
		eParse.set_BufStrTwo (noStr);
		retParse = eParse._PARSE_LINE (CommandAndSessionLogState, sid);
	}
	else {
		found = buffer.find("type=SYSCALL");
		if (found != string::npos)
		{
			SYSCALLFind = 0;

			found = buffer.find("tty=(none)");
			if (found == string::npos)
			{
				eParse.set_BufStrTwo (buffer);
				SYSCALLFind = 1;
			}
		}
		else {           // type = EXECVE

			if (SYSCALLFind == 1)  		//  relative SYSCALL already received
			{
				eParse.set_BufStrOne (buffer);
				retParse = eParse._PARSE_LINE (CommandAndSessionLogState, sid);
				SYSCALLFind = 0;
			}
		}
	}

	return retParse;
}


string acs_alog_activeWorks::workSinglePlogRecord(const std::string& buffer)
{

	string  retParse = "NOT_PARSER";
	size_t  found = 0;
	size_t  foundCmd = 0;
	string  noStr = "NO_STRING";

	foundCmd = buffer.find("apcommand.notice:") ;
	if (foundCmd == string::npos)
	{
		eParse.set_BufStrOne (buffer);
		eParse.set_BufStrTwo (noStr);
		retParse = eParse._PARSE_LINE (CommandAndSessionPLogState, sid);
	}
	else {
		found = buffer.find("type=SYSCALL");
		if (found != string::npos)
		{
			SYSCALLFind = 0;

			found = buffer.find("tty=(none)");
			if (found == string::npos)
			{
				eParse.set_BufStrTwo (buffer);
				SYSCALLFind = 1;
			}
		}
		else {           // type = EXECVE

			if (SYSCALLFind == 1)  		//  relative SYSCALL already received
			{
				eParse.set_BufStrOne (buffer);
				retParse = eParse._PARSE_LINE (CommandAndSessionPLogState, sid);
				SYSCALLFind = 0;
			}
		}
	}

	return retParse;
}


ACS_ALOG_ReturnType acs_alog_activeWorks::readParameter(const char* objectName, const char* nameAttr)
{

	ACS_CC_ImmParameter retStatus;
	ACS_CC_ReturnType result;
	const char *nameObject = objectName;
	char name[50];
	char msg[100];
	strcpy(name,nameAttr);

	OmHandler GetPar;

	result = GetPar.Init();
	if ( result != ACS_CC_SUCCESS )
	{
		sprintf(msg,"Error read %s: GetPar.Init() failure",nameAttr);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(msg));
		return ACS_ALOG_error;
	}

	retStatus.attrName = name;

	result = GetPar.getAttribute(nameObject, &retStatus);
	if ( result != ACS_CC_SUCCESS )
	{
		sprintf(msg,"Error read %s: getAttribute() failure",nameAttr);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(msg));
		return ACS_ALOG_error;
	}

	if (!strcmp(nameAttr,"ap"))
	{
		APtype = *(reinterpret_cast<int*>(retStatus.attrValues[0]));

		if (APtype == 1)  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("APtype = AP1"));
		else if (APtype == 2)  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("APtype = AP2"));
		else  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("APtype = BOH"));
	}

	if (!strcmp(nameAttr,"alogAdmState"))
	{
		AlogIsAlive = *(reinterpret_cast<int*>(retStatus.attrValues[0]));
	}

	if (!strcmp(nameAttr,"transferQueue"))
	{
		if (retStatus.attrValuesNum == 0)  TransferQueue = "";
		else							   TransferQueue = reinterpret_cast<char *>(retStatus.attrValues[0]);
	}

	if (!strcmp(nameAttr,"commandLogTransferQueue"))
	{
		if (retStatus.attrValuesNum == 0)  commandLogTransferQueue = "";
		else							   commandLogTransferQueue = reinterpret_cast<char *>(retStatus.attrValues[0]);
	}

	if (!strcmp(nameAttr,"largeEventDataSkipped"))
	{
		LargeEventDataSkipped = *(reinterpret_cast<int*>(retStatus.attrValues[0]));
	}

	if(!strcmp(nameAttr,"commandAndSessionAdmState"))
	{
		CommandAndSessionLogState = *(reinterpret_cast<int*>(retStatus.attrValues[0]));
	}

	if (!strcmp(nameAttr,"templateFormat"))
	{
		FileNamingFormat = *(reinterpret_cast<int*>(retStatus.attrValues[0]));

		if (FileNamingFormat == 0)  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("File Naming Format = audit...."));
		else if (FileNamingFormat == 1)  ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("File Naming Format = LOGFILE-..."));
		else ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("File Naming Format = ERROR "));
	}
	/* TR:HY25168 starts here */
	if (!strcmp(nameAttr,"nodeAIpAddress"))
	{
		if (retStatus.attrValuesNum == 0)
			strHostname = "";
		else
		{                                                       
			strHostname = reinterpret_cast<char *>(retStatus.attrValues[0]);
		}
	}

	if (!strcmp(nameAttr,"nodeBIpAddress"))
	{
		if (retStatus.attrValuesNum == 0)
			strHostname = "";
		else
		{
			strHostname = reinterpret_cast<char *>(retStatus.attrValues[0]);
		}
	}
	if (!strcmp(nameAttr,"alogFileAdmState"))
	{
		alogFileisActive = *(reinterpret_cast<int*>(retStatus.attrValues[0]));
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("alogFileAdmState"));
	}
	if (!strcmp(nameAttr,"mmlSyslogAdmState"))
	{
		mmlSyslogisActive = *(reinterpret_cast<int*>(retStatus.attrValues[0]));
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("mmlSyslogAdmState"));
	}
	if(!strcmp(nameAttr,"mmlSyslogAvailabilityState"))
	{
		mmlSyslogAvailabilityState = *(reinterpret_cast<int*>(retStatus.attrValues[0]));
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("mmlSyslogAvailabilityState"));
	}



	/*TR:HY25168 ends here */


	result = GetPar.Finalize();
	if ( result != ACS_CC_SUCCESS )
	{
		sprintf(msg,"Error read %s: GetPar.Finalize() failure",nameAttr);
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(msg));
		return ACS_ALOG_error;
	}
	return ACS_ALOG_ok;
}


bool  checkSpecificCommand (string event, string cmd)
{

	size_t  pos = event.find(cmd);

	if (pos != string::npos)       //  positive match
	{
		int  len = cmd.length();

		if (pos == 0)					    			//  case :  # command  or  # command pars
		{
			if (event.at(pos+len) == ' '  ||
					event.at(pos+len) == ';')  return true;
		}
		else {											//  case :  # /..../command  or  # /..../command pars
			if (event.at(pos-1) == '/'  &&
					(event.at(pos+len) == ' '  ||  event.at(pos+len) == ';'))  return true;
		}
	}

	return false;
}


ACS_ALOG_ReturnType acs_alog_activeWorks::checkExcl()
{

	if ( typecmdnc.empty() && typecmdmc.empty() && typecmdmpa.empty() && typecmdmp.empty() && typecmdnp.empty() )
		return ACS_ALOG_error;

	size_t  posFinCmd = 0;
	string  exclType, exclCmd, dataMsg;
	dataMsg.reserve(500);

	dataMsg.assign (eParse.get_PartOfMsg());
	exclType.assign (eParse.get_TypeRec());

	if (exclType.compare("mc")  &&  exclType.compare("mpa")  &&
			exclType.compare("mp")  &&  exclType.compare("np")   &&
			exclType.compare("nc")  &&  exclType.compare("com"))      // the last condition was added for TR HR41607
	{
		return ACS_ALOG_error;
	}

	size_t  posFin  = dataMsg.find(";");
	string  command = dataMsg.substr (0, posFin+1);

	stringToUpper (command);

	if (! exclType.compare("com"))  exclType.assign("nc");    	//  added for TR :  HR41607

	if (! exclType.compare("nc"))
	{
		if (checkSpecificCommand (command, "ALOGPCHG"))  return ACS_ALOG_ok;   // the alogpchg and alogpls are not
		if (checkSpecificCommand (command, "ALOGPLS"))   return ACS_ALOG_ok;   // logged in the audit/protected logs
		// (refer to the relative MPs)
	}

	if (! exclType.compare("nc")  &&  typecmdnc.empty())  return ACS_ALOG_error;

	if ( (! exclType.compare("mc")  ||  ! exclType.compare("mpa"))  &&
			(typecmdmc.empty()  &&  typecmdmpa.empty()) )  return ACS_ALOG_error;

	if (! exclType.compare("mp")  &&  typecmdmp.empty())  return ACS_ALOG_error;

	if (! exclType.compare("np")  &&  typecmdnp.empty())  return ACS_ALOG_error;

	if (! exclType.compare("mp"))
	{
		exclCmd = extractMMLprintoutHeader (dataMsg);
	}
	else {
		size_t  pos1;
		//				size_t  xx1 = command.find(255);			//  TR HS2231
		size_t  xx1 = command.find("\n");			//  TR HS2231
		size_t  xx2 = command.find(";");

		if (xx1 == string::npos)  pos1 = xx2;
		else {
			if (xx1 < xx2)  pos1 = xx1;
			else     		pos1 = xx2;
		}

		size_t  pos2 = command.find(" ");

		if (pos2 == string::npos)  posFinCmd = pos1;
		else {
			if (pos1 < pos2)  posFinCmd = pos1;
			else 			  posFinCmd = pos2;
		}

		exclCmd = command.substr (0, posFinCmd);
	}
	stringToUpper (exclCmd);

	if (! exclType.compare("nc"))       //  changed for TR :  HR41607
	{
		multiset<string>::iterator  it;

		for (it=typecmdnc.begin(); it!=typecmdnc.end(); it++)
		{
			string  exclItem = *(it);
			size_t  pos = command.find(exclItem);

			if (pos != string::npos)       //  positive match
			{
				int  len = exclItem.length();

				if (pos == 0)					    			//  case :  # command  or  # command pars
				{
					if (command.at(pos+len) == ' '  ||
							command.at(pos+len) == ';')  return ACS_ALOG_ok;
				}
				else {											//  case :  # /..../command  or  # /..../command pars
					if (command.at(pos-1) == '/'  &&
							(command.at(pos+len) == ' '  ||  command.at(pos+len) == ';'))  return ACS_ALOG_ok;
				}
			}
		}
	}

	else if (! exclType.compare("mc")  ||  ! exclType.compare("mpa"))
	{
		if (exclCmd.length() <= 1)  return ACS_ALOG_ok;       //  only the CR has been received

		std::string  mcCmd, tmp_par;
		std::vector<std::string> pars, mpaCmds;
		size_t pos = 0;
		int nPar = 0;

		if ((pos = exclCmd.find(":")) == std::string::npos) {	// find first parameter separator
			mcCmd = exclCmd;
		}
		else {
			mcCmd = exclCmd.substr(0, pos);
			pos++;

			while (pos < exclCmd.length()) {
				if (exclCmd.at(pos) == ',') {	//  next parameter separator
					nPar++;
					pars.push_back(tmp_par);
					tmp_par.clear();
				}
				else if (exclCmd.at(pos) == '=') {	//  value of a parameter
					nPar++; pos++;
					pars.push_back(tmp_par);
					tmp_par.clear();

					if ((pos = exclCmd.find(',', pos)) == std::string::npos) break;
				}
				else {
					tmp_par += exclCmd.at(pos);
				}

				pos++;
			}

			for (size_t i = 0; i < pars.size(); i++)
				mpaCmds.push_back(mcCmd + " " + pars[i]);     //  this format is established in COM
		}

		if (typecmdmc.count(mcCmd) > 0)  return ACS_ALOG_ok;

		for (int j=0; j< nPar; j++)
			if (typecmdmpa.count(mpaCmds[j]) > 0)  return ACS_ALOG_ok;
	}

	else if (! exclType.compare("mp"))
	{
		multiset<string>::iterator  it;

		for (it=typecmdmp.begin(); it!=typecmdmp.end(); it++)
		{
			string  exclItem = *(it);

			if (exclCmd.find(exclItem.c_str()) != string::npos)  return ACS_ALOG_ok;
		}
	}

	return ACS_ALOG_error;
}


void  acs_alog_activeWorks::setAllPar (string psid)
{
	sid = psid;
	//noWork = 0;         commentato da PLOG
	HA = 0;
	stopThReadMain = 0;
	stopThReadApi  = 0;
	stopThHandleLogs = 0;
	stopThEventQueue = 0;
	startWrite = 0;
	SYSCALLFind = 0;
	EXECVEFind = 0;
}


bool  acs_alog_activeWorks::checkProtectedCmdItems(){

	ExListIterType exListIter;

	size_t  posFinCmd = 0;
	string  exclType, exclCmd, dataMsg;

	exclType.assign (eParse.get_TypeRec());

	if (exclType.compare("nc")  &&  exclType.compare("com"))  return false;

	dataMsg = eParse.get_PartOfMsg();

	posFinCmd = dataMsg.find(";");

	exclCmd = dataMsg.substr (0, posFinCmd);

	stringToUpper (exclCmd);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	for (exListIter=m_PMMLcmds.begin(); exListIter!=m_PMMLcmds.end(); exListIter++){
		if (exclCmd.find((*exListIter).Data1) != string::npos)  return true;
	}

	return false;
}


bool acs_alog_activeWorks::checkProtectedItems(string plogRec){

	ExListIterType exListIter;

	size_t  posIni = 0;
	size_t  posFinType = 0;
	size_t  posFinCmd = 0;

	string  plogType;
	string  plogCmd;
	string plogUser;

	int  i = 0;

	while (i < 12)
	{
		posIni = plogRec.find(";");
		if (posIni == string::npos)
		{
			ACSALOGLOG(LOG_LEVEL_WARN,TEXTERROR("Error In check protected item separator not found"));
			return false;
		}
		plogRec = plogRec.substr(posIni + 1);

		if (i == 2)
		{
			posFinType = plogRec.find(";");
			plogType   = plogRec.substr(0,posFinType);

			if (! plogType.compare("sec")  ||  ! plogType.compare("com")  ||  ! plogType.compare("nc")  ||
					! plogType.compare("cmw")  ||  ! plogType.compare("alp")  ||  ! plogType.compare("apd"))
			{
				return false;
			}
		}

		if (i == 4)
		{
			posFinType = plogRec.find(";");
			plogUser   = plogRec.substr(0,posFinType);

			if ( strlen(plogUser.c_str()) > 0)
			{
				ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

				for (exListIter=m_PUSRs.begin();exListIter!=m_PUSRs.end();exListIter++) {
					if (plogUser.compare((*exListIter).Data1) == 0) {
						return true;
					}
				}
			}
		}

		if (i == 11)
		{
			if (! plogType.compare("mp")  ||  ! plogType.compare("mpi"))
			{
				plogCmd = extractMMLprintoutHeader (plogRec);

				if ( (! plogCmd.compare("COMMAND EXECUTED")) || (! plogCmd.compare("COMMAND NOT EXECUTED")) )
				{
					ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

					for (exListIter=m_PMMLcmds.begin();exListIter!=m_PMMLcmds.end();exListIter++)
						if ( (plogRec.find((*exListIter).Data1)) != string::npos )
						{
							plogCmd += " ";
							plogCmd += (*exListIter).Data1;
						}
				}
			}
			else {
				size_t  pos1;
				//					size_t  xx1 = plogRec.find(255);			//  TR HS2231
				size_t  xx1 = plogRec.find("\n");			//  TR HS2231
				size_t  xx2 = plogRec.find(";");

				if (xx1 == string::npos)  pos1 = xx2;
				else {
					if (xx1 < xx2)  pos1 = xx1;
					else     		pos1 = xx2;
				}

				size_t  pos2 = plogRec.find(" ");

				if (pos2 == string::npos)  posFinCmd = pos1;
				else {
					if (pos1 < pos2)  posFinCmd = pos1;
					else 			  posFinCmd = pos2;
				}

				plogCmd = plogRec.substr(0, posFinCmd);
				if (! plogType.compare("mc")   ||  ! plogType.compare("mci")  ||
						! plogType.compare("mcl")  ||  ! plogType.compare("mli"))
				{
					string  plogCmd2;
					size_t  pos3 = plogCmd.find(":");
					if ( pos3 != string::npos )
					{
						plogCmd2 = plogCmd.substr(0,pos3);
						plogCmd  = plogCmd2;
					}
				}
				stringToUpper (plogCmd);
			}
		}
		i ++;
	}

	if (! plogType.compare("mc")   ||  ! plogType.compare("mci")  ||
			! plogType.compare("mcl")  ||  ! plogType.compare("mli"))
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

		for (exListIter=m_PMMLcmds.begin();exListIter!=m_PMMLcmds.end();exListIter++)

			if (! strcmp((*exListIter).Data1,plogCmd.c_str()))
			{
				return true;
			}
	}

	if (! plogType.compare("mp")  ||  ! plogType.compare("mpi"))
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

		for (exListIter=m_PMMLprts.begin();exListIter!=m_PMMLprts.end();exListIter++) {
			if (plogCmd.find((*exListIter).Data1) != string::npos) {
				return true;
			}
		}
	}

	return false;
}


int acs_alog_activeWorks::FetchInfoSYS()
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	// Need exclusive access
	//	   TCHAR szFileName[128+1];
	//   _tcscpy(szFileName, ACS_ALOG_Path(_T("$ACS_LOGS/ALOG/system/SYS")));
	string szFileName;
	int m_nRetCode = 0;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/SYS";

	if (!checkFile(szFileName.c_str(), false))
	{
		m_nRetCode = -1;
		m_hFile =  ACE_OS::open(szFileName.c_str(),O_CREAT|O_WRONLY);
		if (m_hFile == ACE_INVALID_HANDLE){
			return 1;
		}
		ACE_OS::close(m_hFile);
	}
	else {
		m_hFile =  ACE_OS::open(szFileName.c_str(),O_WRONLY);
		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}
		ACE_OS::close(m_hFile);

		acs_alog_cmdHandler  cmdHandler;
		m_nRetCode = cmdHandler.UpdateProtectedPassword();
	}

	return m_nRetCode;
}


int acs_alog_activeWorks::FetchInfoATTRFILE()
{
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/ATTRFILEP";

	if (!checkFile(szFileName.c_str(), false))
	{
		m_hFile = ACE_OS::open(szFileName.c_str(), O_CREAT | O_WRONLY | O_TRUNC);

		m_AttrFileRecord.status = activee;
		m_AttrFileRecord.FileSize = 100;     // MB
		(*m_AttrFileRecord.FileDest) = '\0';
		m_AttrFileRecord.IdTag = false;

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}
		int dwBytesWritten;
		dwBytesWritten = ACE_OS::write(m_hFile,
				(void*)&m_AttrFileRecord,
				sizeof(ACS_ALOG_AttrFileRecord));

		if (dwBytesWritten == 0)
		{
			ACE_OS::close(m_hFile);
			//m_nRetCode = m_pAlarmHandler->ReportError(esPMMLCMD, etFileWrite);
			//Unlock();
			return cerFileWriteError;
		}

		ACE_OS::close(m_hFile);
		return 0;
	}
	else
	{
		// Check on file locked
		m_hFile = ACE_OS::open(szFileName.c_str(), O_WRONLY);
		if (m_hFile == ACE_INVALID_HANDLE) {
			return 1;
		}
		ACE_OS::close(m_hFile);

		m_hFile = ACE_OS::open(szFileName.c_str(),O_RDONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 2;
		}
		int dwBytesRead;
		dwBytesRead = ACE_OS::read(m_hFile, (void*) (& m_AttrFileRecord),
				sizeof(ACS_ALOG_AttrFileRecord));
		if(dwBytesRead == -1)
		{
			ACE_OS::close(m_hFile);
			return 2;
		}

		if (m_AttrFileRecord.FileDest == '\0')  PLOGTransferQueue ='\0';
		else									PLOGTransferQueue = m_AttrFileRecord.FileDest;

		string  xx = "Read from ATTRFILEP ....  PLOGTransferQueue = ";
		xx = xx + PLOGTransferQueue;
		ACSALOGLOG(LOG_LEVEL_WARN,TEXTERROR(xx.c_str()));

		if (m_AttrFileRecord.IdTag)  CommandAndSessionPLogState =1;
		else						 CommandAndSessionPLogState =0;

		ACE_OS::close(m_hFile);
		return 0;
	}
}


int acs_alog_activeWorks::FetchInfoPMMLCMD()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/PMMLCMD";

	if (!checkFile(szFileName.c_str(), false))
	{
		m_hFile = ACE_OS::open(szFileName.c_str(), O_CREAT | O_WRONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}
		ACE_OS::close(m_hFile);
		return 0;
	}
	else
	{
		// Check on file locked
		m_hFile = ACE_OS::open(szFileName.c_str(), O_WRONLY);
		if (m_hFile == ACE_INVALID_HANDLE) {
			return 1;
		}
		ACE_OS::close(m_hFile);

		ClearExcludedList(m_PMMLcmds);

		m_hFile = ACE_OS::open(szFileName.c_str(),O_RDONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}

		while (true) {
			int dwBytesRead;
			dwBytesRead = ACE_OS::read(m_hFile, (void*) (& m_Exclude),
					sizeof(ACS_ALOG_Exclude));

			if (dwBytesRead == -1)
			{
				ACE_OS::close(m_hFile);
				return 2;
			}

			if ( dwBytesRead == 0)
				break;

			// Adds as a new ACS_ALOG_Exclude object in the exclude list

			m_PMMLcmds.push_back(ACS_ALOG_Exclude(Decode(m_Exclude.Data1)));
		}

		ACE_OS::close(m_hFile);

		// Sort the list
		m_PMMLcmds.sort();

		return 0;
	}
}


int acs_alog_activeWorks::FetchInfoPMMLPRT()
{

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/PMMLPRT";

	if (!checkFile(szFileName.c_str(), false))
	{
		m_hFile = ACE_OS::open(szFileName.c_str(), O_CREAT | O_WRONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}
		ACE_OS::close(m_hFile);
		return 0;
	}
	else
	{
		// Check on file locked
		m_hFile = ACE_OS::open(szFileName.c_str(), O_WRONLY);
		if (m_hFile == ACE_INVALID_HANDLE) {
			return 1;
		}
		close(m_hFile);

		ClearExcludedList(m_PMMLprts);

		m_hFile = ACE_OS::open(szFileName.c_str(),O_RDONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}

		while (true) {
			int dwBytesRead;
			dwBytesRead = ACE_OS::read(m_hFile, (void*) (& m_Exclude),
					sizeof(ACS_ALOG_Exclude));

			if (dwBytesRead == 0)
			{
				break;
			}

			if (dwBytesRead == -1)
			{
				ACE_OS::close(m_hFile);
				return 2 ;
			}
			// Adds as a new ACS_ALOG_Exclude object in the exclude list
			m_PMMLprts.push_back(ACS_ALOG_Exclude(Decode(m_Exclude.Data1)));
		}

		ACE_OS::close(m_hFile);

		// Sort the list
		m_PMMLprts.sort();
		return 0;
	}
}

int acs_alog_activeWorks::FetchInfoPUSR()
{

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_PLogList (_acs_alog_PLog_Mutex);

	ACE_HANDLE m_hFile = NULL;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/PUSR";

	if (!checkFile(szFileName.c_str(), false))
	{
		m_hFile = ACE_OS::open(szFileName.c_str(), O_CREAT | O_WRONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}
		ACE_OS::close(m_hFile);
		return 0;
	}
	else
	{
		// Check on file locked
		m_hFile = ACE_OS::open(szFileName.c_str(), O_WRONLY);
		if (m_hFile == ACE_INVALID_HANDLE) {
			return 1;
		}
		close(m_hFile);

		ClearExcludedList(m_PUSRs);

		m_hFile = ACE_OS::open(szFileName.c_str(),O_RDONLY);

		if (m_hFile == ACE_INVALID_HANDLE)
		{
			return 1;
		}

		while (true) {
			int dwBytesRead;
			dwBytesRead = ACE_OS::read(m_hFile, (void*) (& m_Exclude),
					sizeof(ACS_ALOG_Exclude));

			if (dwBytesRead == 0)
			{
				break;
			}

			if (dwBytesRead == -1)
			{
				ACE_OS::close(m_hFile);
				return 2 ;
			}
			// Adds as a new ACS_ALOG_Exclude object in the exclude list
			m_PUSRs.push_back(ACS_ALOG_Exclude(Decode(m_Exclude.Data1)));
		}

		ACE_OS::close(m_hFile);

		// Sort the list
		m_PUSRs.sort();
		return 0;
	}
}

void acs_alog_activeWorks::ClearExcludedList(std::list<ACS_ALOG_Exclude>& ExItems)
{
	ExItems.clear();
}


int CheckALOGSysFile( bool check_size_zero , bool if_writable)
{
	string szFileName ;
	getAlogDataDiskPath(szFileName);

	szFileName += "/ACS_ALOG/SYS";

	if(if_writable)
	{
		//Do nothing
	}
	ACE_stat statBuff;
	if( ACE_OS::stat(szFileName.c_str(), &statBuff)  != 0 )
	{
		cerr << endl << "File access error" << endl ;
		return 22;

	}
	int retcode = 0;
	if(check_size_zero)
	{
		if(statBuff.st_size != 0)
		{

			cerr << "Password already defined"  << endl;
			retcode = 41;
		}
	}
	else
	{
		if(statBuff.st_size == 0)
		{
			cerr << endl << "Password not defined"  << endl;
			retcode = 42;
		}
	}
	return retcode;
}


enumCmdExecuteResult acs_alog_activeWorks::getPlogTransferQeueAttribute()
{
	ACS_ALOG_AttrFileRecord m_AttrFileRecord;
	ACE_HANDLE m_hFile = ACE_INVALID_HANDLE;
	string szFileName;

	getAlogDataDiskPath(szFileName);
	szFileName += "/ACS_ALOG/ATTRFILEP";

	m_hFile = ACE_OS::open(szFileName.c_str(), O_RDONLY);

	if (m_hFile == ACE_INVALID_HANDLE)
	{
		return cerFileOpenError;
	}

	int dwBytesRead;
	dwBytesRead = ACE_OS::read(m_hFile, (void*) (& m_AttrFileRecord),
			sizeof(ACS_ALOG_AttrFileRecord));

	if (dwBytesRead != sizeof(ACS_ALOG_AttrFileRecord))
	{
		ACE_OS::close(m_hFile);
		return cerFileReadError;
	}
	strcpy(m_AttrFileRecord.FileDest, PLOGTransferQueue.c_str());
	ACE_OS::close(m_hFile);
	return cerSuccess;
}

void sendCurrentEncryptedPLogFile (void){

	if(FilePName.length() > 0){
		std::ifstream ifs(FilePName.c_str());//change to filepname
		std::string content( (std::istreambuf_iterator<char>(ifs) ),
				(std::istreambuf_iterator<char>()    ) ); //Taking the entire encrypted content into a string
		long int str_len = strlen(content.c_str());
		//Every encrypted event is ended with new line "\n" so reading the single event, decrypting it and appending to event string
		size_t reference = 0;//for starting position of the string
		size_t found = 0;//for ending position of the string
		string decrypted_event_sub= "";
		while(reference < str_len)
		{
			found = content.find("\n",reference);
			decrypted_event_sub = content.substr(reference, found-reference);
			event_current += decryptString(decrypted_event_sub);
			reference = found+1;
		}
		if(reference == str_len)
			unlink (FilePName.c_str());//take care of it after zipping and putting the file in /protected_logs
		fstream  targetFile;

		targetFile.open (FilePName.c_str(), fstream::out | fstream::app);

		if (! targetFile.is_open())
		{
			unlink (FilePName.c_str());
			if (WritePlogError == 0)
			{
				string  s = "FCODE: 15\nCAUSE: Could not write to file : ";
				s += CurrentPLogFile;
				AEHEVENTMESSAGE ("acs_alogmaind", 9933, "A2", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "Error when writing a protected log-file", s.c_str());
				WritePlogError = 1;
			}
			ACSALOGLOG (LOG_LEVEL_FATAL,TEXTERROR("Error opening /tmp PLOG file"));
			return;
		}

		str_len = event_current.length();

		for (size_t j=0; j < str_len; j++)
		{
			if (checkPrintableChar ((unsigned char)event_current.at(j)) == false)  event_current[j] = '*';
		}

		targetFile << event_current.c_str();

		if (targetFile.bad())
		{
			targetFile.close();

			unlink (FilePName.c_str());

			if (WritePlogError == 0)
			{
				string  s = "FCODE: 15\nCAUSE: Could not write to file : ";
				s += CurrentPLogFile;
				AEHEVENTMESSAGE ("acs_alogmaind", 9933, "A2", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "Error when writing a protected log-file", s.c_str());
				WritePlogError = 1;
			}
			ACSALOGLOG (LOG_LEVEL_FATAL,TEXTERROR("Error writing in /tmp PLOG file"));
			return;
		}

		targetFile.flush();  targetFile.close();

		string pwdFor7zExtractionInteractive = protectedPassword + "\n";
		string pwdFor7zArchiveInteractive = pwdFor7zExtractionInteractive + pwdFor7zExtractionInteractive;

		string  cmdZip = "/usr/bin/7z a -tzip -mx=0 -mem=AES256 " + FilePAudit + " " + FilePName + " -p";
		bool commandExecuted = false;
		int retCode_7z_archive = 0;
		commandExecuted = execute7zCmd(cmdZip, pwdFor7zArchiveInteractive, retCode_7z_archive);                 // TR HX49170
		pwdFor7zArchiveInteractive.clear();
		if (!commandExecuted || !WIFEXITED(retCode_7z_archive) || (WEXITSTATUS(retCode_7z_archive) != 0))
		{
			unlink (FilePName.c_str());

			if (WritePlogError == 0)
			{
				string  s = "FCODE: 15\nCAUSE: Could not write to file : ";
				s += CurrentPLogFile;
				AEHEVENTMESSAGE ("acs_alogmaind", 9933, "A2", "AUDIT LOG FAULT",
						"APZ", "acs_alogmaind/Alarm", "Error when writing a protected log-file", s.c_str());
				WritePlogError = 1;
			}
			string  msg = "7z archive command failed :  code = ";
			char  code[10];
			sprintf (code, "   %03d", WEXITSTATUS(retCode_7z_archive));
			msg = msg + code;
			ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(msg));
			return;
		}

		unlink (FilePName.c_str());
		event_current.clear();
	}
}
void acs_alog_activeWorks::redirectmmltosyslog(string retWork)
{  

	ACSALOGLOG (LOG_LEVEL_DEBUG,TEXTERROR("Sending MML event to syslog"));
	struct logger_ctl ctl;
	ctl.fd = -1;
	ctl.pid = getProcessid();
	ctl.pri = 110; //facility = 13, pri = 13x8 + LOG_INFO
	ctl.prio_prefix = 0;
	ctl.app_name = "mcs_adhd";
	ctl.unix_socket = "/dev/log";
	ctl.unix_socket_errors = 0;
	ctl.server = NULL;
	ctl.port = NULL;
	ctl.hdr = NULL;
	ctl.msgid ="mml_audit";
	ctl.socket_type = ALL_TYPES;
	ctl.rfc5424_time = 1;
	ctl.rfc5424_tq = 1;
	ctl.rfc5424_host = 1;
	ctl.skip_empty_lines = 1;
	ctl.max_message_size = 2048 * 4;
	ctl.octet_count = 0; //TR IA70359
	fieldRec outputFeilds;
	ElaborateRecord(retWork, outputFeilds);
	outputFeilds.cmdid = cmdIdString;
	ctl.pri = getPriorityLevel(outputFeilds.msg);
	retWork = getOutputPrint(outputFeilds);
	logger_open(&ctl);
	logger_stdin(&ctl, retWork.c_str());
	logger_close(&ctl);

}	

bool acs_alog_activeWorks::isNumeric(const char *s){

	int  l = strlen(s);

	for (int i = 0; i<l; i++)
	{
		if (s[i] < '0'  ||  s[i] > '9')  return false;
	}
	return true;
}

int  acs_alog_activeWorks::ElaborateRecord (string lineInput, fieldRec &outputFields)
{

	size_t  posFin = 0, posStop = 0;
	int     j = 0,  counter = 0;
	bool 	singleLine = false;

	while (j < (int)lineInput.length())					// ------  Checks due to record corruption (Hard Limit)
	{									// ------         and to different format (Win vs Linux)
		if (lineInput.at(j) == ';')  counter ++;		// ------
		j ++;
	}
	if (counter < 13)  return 1;

	int  pos = lineInput.length() - 1;
	while (lineInput.at(pos) == '\n' || lineInput.at(pos) == '\r' || lineInput.at(pos) == ' ')  pos --;

	lineInput = lineInput.substr(0,pos+1); // TR-HT78529

	if (lineInput.at(pos) != ';')  return 1;
	j = 0,  counter = 0;

	while (j < pos)
	{
		if (lineInput.at(j) == '\n')  counter ++;
		j ++;
	}														

	if (counter == 0)  singleLine = true;	

	posFin = lineInput.find(";");
	if (posFin == string::npos)  return 1;

	outputFields.size_record = lineInput.substr(0,posFin);
	posStop = posFin;
	if (posFin >= lineInput.length())  return 1;

	posFin = lineInput.find(";",posStop+1);
	if (posFin == string::npos)  return 1;

	outputFields.data = lineInput.substr(posStop+1,posFin-posStop-1);

	if (outputFields.data.length() > 0)
	{
		counter = 0;
		for (j=0; j<(int)outputFields.data.length(); j++)
		{
			if (outputFields.data.at(j) == '-')  counter ++;
		}
		if (counter != 2)  return 1;

		outputFields.data.erase (outputFields.data.find('-'),1);
		outputFields.data.erase (outputFields.data.find('-'),1);
	}
	posStop = posFin;
	posFin = lineInput.find(";",posStop+1);
	outputFields.time = lineInput.substr(posStop+1, posFin-posStop-1);
	posStop = posFin;

	if (isNumeric(outputFields.time.c_str()) == false)  return 1;

	posFin = lineInput.find(";",posStop+1);
	outputFields.type = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;
	posFin = lineInput.find(";",posStop+1);
	outputFields.prca = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;
	posFin = lineInput.find(";",posStop+1);
	outputFields.user_name = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.device = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.node_number = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.session_id = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.local_host_name = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.remote_host = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	posFin = lineInput.find(";",posStop+1);
	outputFields.size_msg = lineInput.substr(posStop+1,posFin-posStop-1);
	posStop = posFin;

	if (isNumeric(outputFields.size_msg.c_str()) == false)  return 1;

	int  lenData = atoi (outputFields.size_msg.c_str());

	if (singleLine == true)
	{
		outputFields.msg = lineInput.substr(posStop+1, lenData);
		posFin ++;
	}
	else {  //TR-HT78529
		int posFin1 = lineInput.length() - 1;
		while (lineInput.at(posFin1) != ';')   posFin1 --;
		while (lineInput.at(posFin1) != '\n')  posFin1 --;

		outputFields.msg = lineInput.substr(posStop+1, posFin1-posStop);
	}

	CmdIDused = false;

	size_t  lenEve = lineInput.length();
	counter = 0;

	for (j=(posFin+outputFields.msg.length()); j<(int)lenEve; j++)   //TR-HT78529
	{
		if (lineInput.at(j) == ';')  counter ++;
	}

	if (counter == 1)  return 0;		// the event is on a Single CP, without the Cmd ID
	posFin= posFin+outputFields.msg.length()+2;
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cmdidlist += lineInput.at(posFin++);//TR HV19568: cmidlength
	if (posFin == lenEve)  return 1;


	CmdIDused = true;
	counter   = 0;

	for (j=posFin; j<(int)lenEve; j++)
	{
		if (lineInput.at(j) == ';')  counter ++;
	}
	if (counter == 3)  return 0;

	if (isNumeric(outputFields.cmdidlist.c_str()) == false)  return 1;

	int  cmidLenRead = atoi (outputFields.cmdidlist.c_str());

	posFin++;
	outputFields.cmdid = lineInput.substr(posFin,cmidLenRead);//get the substring from posFin for a length cmidLenRead
	posFin+=cmidLenRead;//now posfin is on the ; after the cmdid
	if (counter < 3)  return 0;//moved here
	//TR HV19568:END

	if (outputFields.cmdid.length() == 0)  CmdIDused = false;

	if (outputFields.type.compare("sec") == 0)  return 0;

	posFin ++;
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cpnameGroup += lineInput.at(posFin++);
	if (posFin == lenEve)  return 1;
	posFin ++;
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cpidList += lineInput.at(posFin++);
	if (posFin == lenEve)  return 1;
	posFin ++;
	while (posFin < lenEve  &&  lineInput.at(posFin) != ';')  outputFields.cmdSeqNum += lineInput.at(posFin++);
	if (posFin == lenEve)  return 1;
	return 0;
}
string  acs_alog_activeWorks::getOutputPrint (fieldRec &outputFields)
{
	ACS_CS_API_NetworkElement::isMultipleCPSystem(IsMultiCP);
	string parsedOutput = "interface=mml ";

	parsedOutput += "acct=" + outputFields.user_name + " ";				//  EEMEA\eabuser
	parsedOutput += "ses=" + outputFields.session_id + " ";				//  23

	parsedOutput += "hostname=" + outputFields.local_host_name + " ";		//  UPC8095/osd179
	parsedOutput += "addr=" + outputFields.remote_host + " ";

	parsedOutput += "event-type=" + outputFields.type + " ";//eventTypeConversion(outputFields.type) + " ";
	parsedOutput += "device=" + outputFields.device + " ";
	if(outputFields.prca.length() > 0)
		parsedOutput += "prca=" + outputFields.prca + " ";
	if (outputFields.cmdid.length() > 0)
		parsedOutput += "command-id=" + outputFields.cmdid + " ";


	if (IsMultiCP)
	{
		if(outputFields.cpnameGroup.length() > 0)
			parsedOutput += "cp-name-group=" + outputFields.cpnameGroup + " ";
		if(outputFields.cpidList.length() > 0)
			parsedOutput += "cp-id-list=" + outputFields.cpidList + " ";
		if (outputFields.cmdSeqNum.length() > 0)
			parsedOutput += "csn=" + outputFields.cmdSeqNum + " ";
	}


	string command = "";
	string dataPart = "";
	if(outputFields.msg.find(';') != string::npos){
		int firstSemicolonIndex = outputFields.msg.find(';');
		command = outputFields.msg.substr(0, firstSemicolonIndex);
		std::replace(command.begin(), command.end(), '\n', ' ');
		parsedOutput += "command=";
		parsedOutput += command + " ";
		dataPart = outputFields.msg.substr(firstSemicolonIndex + 1);
	}
	else{
		dataPart = outputFields.msg;
	}
	parsedOutput += "data=";


	replace(dataPart.begin(), dataPart.end(), '\r', '\n');
	while(dataPart.find("\n\n") != string::npos){
		dataPart.replace(dataPart.find("\n\n"), 2, "\n");
	}
	while(dataPart.find('\n') != string::npos){
		dataPart.replace(dataPart.find('\n'), 1, "\\n");
	}
	while(dataPart.find(' ') != string::npos){
		dataPart.replace(dataPart.find(' '), 1, "\\s");
	}

	parsedOutput += dataPart + "\n";

	LastEvent.assign(outputFields.type);
	if (LastEvent.compare("alp") == 0)  parsedOutput += "\n";
	if (LastEvent.compare("mc")  == 0)  parsedOutput += "\n";
	if (LastEvent.compare("sec") == 0)  parsedOutput += "\n";
	if (LastEvent.compare("mmc") == 0)  parsedOutput += "\n";
	if (LastEvent.compare("mp")  == 0)
	{
		parsedOutput += "\n";
		if (outputFields.msg.at(outputFields.msg.length()-1) != '\n')  parsedOutput += "\n";
	}
	return parsedOutput;
}

int acs_alog_activeWorks::getPriorityLevel(string message){
	if(message.find("ALARM CEASING") != string::npos)
		return 109; //NOTICE

	/*
	alarm message syntax is 
	***ALARM alnr |INCREASED TO or DECREASED TO or REPEATED| alcls/alcat "origin" att date time
	*/

	string alarmPrintOut = "";
	int newLinePos = 0;
	int forwardSlashIndex = -1;
	string alarmClass = "";
	if(message.find("*** ALARM") != string::npos){
		newLinePos = message.find("*** ALARM") + 9;
		while(message[newLinePos] != '\n')
			alarmPrintOut += message[newLinePos++];
		syslog(LOG_INFO, "Alarm Print out Line = %s", alarmPrintOut.c_str());
		if(alarmPrintOut.find("/") != string::npos){
			forwardSlashIndex = alarmPrintOut.find("/");
			alarmClass =  alarmPrintOut.substr(forwardSlashIndex - 2, 2);
		}
	}
	syslog(LOG_INFO, "Alarm Class = %s", alarmClass.c_str());
	if(alarmClass == "A1")
		return 105; //ALERT
	else if(alarmClass == "A2")
		return 106; //CRITICAL
	else if(alarmClass == "A3")
		return 107; //ERROR
	else if(alarmClass == "O1")
		return 108; //WARNING
	else if(alarmClass == "O2")
		return 108; //WARNING
	else
		return 110; //INFO
}

int acs_alog_activeWorks::getProcessid(){
	string pidString;
	ifstream fileHandler;
	fileHandler.open("/var/run/apg/mcs_adhd.pid");

	if(fileHandler.is_open()){
		fileHandler >> pidString;
	}
	fileHandler.close();
	stringstream ss(pidString);
	int pid = -1;
	if(ss.str().length() > 0)
		ss >> pid;
	if(pid == -1)
		ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("Unabble to fetch process id of mcs_adhd service"));
	return pid;
}

void acs_alog_activeWorks::setCommandID(string* tmp_str){

	string s = *tmp_str;
	int cmdPos = s.find("cmdid=");
	cmdPos += 6;
	cmdIdString = "";
	while(s[cmdPos] != ','){
		cmdIdString += s[cmdPos];
		cmdPos++;
	}

}
