
/*=================================================================== */
/**
   @file   acs_alog_activeWorks.h

   @brief Header file for acs_alog_activeWorks type module.

          This module contains all the declarations useful to
          specify the class.

   @version 2.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
   N/A       07/08/2012     xbhakat               Final Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef acs_alog_activeWorks_H_
#define acs_alog_activeWorks_H_


/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
//Include file for external library.
#include <iostream>
#include <set>
#include <fstream>
#include <string.h>
#include <time.h>
#include <vector>
#include <libaudit.h>
#include <fcntl.h>
#include <list>
#include <unistd.h>
#include <syslog.h>

#include <ace/Condition_T.h>
#include <ace/Task.h>
#include <ace/ACE.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/Event.h>
#include <ACS_TRA_trace.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_CS_API.h>

#include <acs_alog_parser.h>
#include <acs_alog_nodestate.h>
#include <acs_alog_cmdHandler.h>
#include <acs_alog_utility.h>
#include <acs_alog_fileDef.h>


using namespace std;

struct fieldRec
{
	string size_record;
	string data;
	string time;
	string type;
	string prca;
	string user_name;
	string device;
	string node_number;
	string session_id;
	string local_host_name;
	string remote_host;
	string size_msg;
	string msg;
	string cmdidlist;
	string cmdid;
	string cpnameGroup;
	string cpidList;
	string cmdSeqNum;
};



class acs_alog_activeWorks{

private:

	int HA;
	static int noWork;
	static string sid;

	ACE_thread_t  read_main_thread_id;
	ACE_thread_t  read_api_thread_id;
	ACE_thread_t  handle_logs_thread_id;
	ACE_thread_t  cease_alarms_thread;
	ACE_thread_t  transferqueue_checker_thread_id;
	ACE_thread_t  write_file_thread_id;

	static ACE_THR_FUNC_RETURN  transferqueue_checker_svc(void*);
	static ACE_THR_FUNC_RETURN  watch_syslog_pipe(void*);
	static ACE_THR_FUNC_RETURN  watch_api_pipe(void*);
	static ACE_THR_FUNC_RETURN  ceaseCheck(void*);
	static ACE_THR_FUNC_RETURN  handle_log_files(void*);
	static ACE_THR_FUNC_RETURN  write_file(void*);

	static void  writeTargetLog (std::string& event);
	static void  writeTargetPLog (std::string& event);
   	static void  writeCLOGfile (void);
	static string workSingleRecord(const std::string& buf);
	static string workSinglePlogRecord(const std::string& buf);
	static void serialRecord(string buffer);
	static void checkPasswordInCommand(char *command);
	static ACS_ALOG_ReturnType checkExcl();
	ACS_ALOG_ReturnType MultipleCpCheck();
	void ClearExcludedList(std::list<ACS_ALOG_Exclude>& ExItems);
	static bool checkProtectedItems(string plogRec);
	static bool checkProtectedCmdItems(void);

	static void redirectmmltosyslog(string str);
	static int getProcessid();
	static bool isNumeric(const char *s);
	static int  ElaborateRecord (string lineInput, fieldRec &outputFields);
	static string  getOutputPrint (fieldRec &outputFields);
	static int getPriorityLevel(string eventType);
	static void setCommandID(string* tmp_str);


	ACS_ALOG_Exclude        m_Exclude;
	ACS_ALOG_AttrFileRecord m_AttrFileRecord;

public:

	static ACE_Recursive_Thread_Mutex  _acs_alog_Mutex_auditlog_file;

	static int stopThReadMain;
	static int stopThReadApi;
	static int stopThHandleLogs;

	static string dataEXVE;
	static string timeEXVE;
	static string dataSYCL;
	static string timeSYCL;

	static int SYSCALLFind;
	static int EXECVEFind;

	static string dataEvent;
	static string timeEvent;

	static vector<string>eventDataUnit;

	static int startWrite;
	static string local_hostname;

	int  getnoWork() { return acs_alog_activeWorks::noWork; };

	void  setHA(int value) { HA = value; };
	void  setnoWork(int value) { acs_alog_activeWorks::noWork = value; };
	void  setAllPar (string psid);

	ACS_ALOG_ReturnType setLocalHostName(void);
	ACS_ALOG_ReturnType readParameter(const char* objectName, const char* nameAttr);
	ACS_ALOG_ReturnType stopThread(void);

	acs_alog_activeWorks();
	acs_alog_activeWorks(string psid);

	~acs_alog_activeWorks(){};

	ACS_ALOG_ReturnType alogInit(void);
	ACS_ALOG_ReturnType alogWorks(void);
	int FetchInfoSYS();
	int FetchInfoPMMLCMD();
	int FetchInfoPMMLPRT();
	int FetchInfoATTRFILE();
	int FetchInfoPUSR();

	ACS_ALOG_ReturnType  startTransferQueueChecker(void);
	ACS_ALOG_ReturnType  startWatchNodeState(void);
	ACS_ALOG_ReturnType  startWatchSyslogPipe(void);
	ACS_ALOG_ReturnType  startWatchApiPipe(void);
	ACS_ALOG_ReturnType  startHandleLogFiles(void);
	ACS_ALOG_ReturnType  startHandleToCeaseAlarms(void);
	ACS_ALOG_ReturnType  startWriteFile(void);
	enumCmdExecuteResult getPlogTransferQeueAttribute();
};

#endif /* acs_alog_activeWorks_H_ */

