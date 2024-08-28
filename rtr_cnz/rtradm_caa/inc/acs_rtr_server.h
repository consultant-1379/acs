//********************************************************
//
//	COPYRIGHT Ericsson AB 2014.
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson AB.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson AB or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//********************************************************

#ifndef _ACS_RTR_SERVER_H_
#define _ACS_RTR_SERVER_H_

#include "acs_rtr_errorsupportbase.h"
#include "acs_rtr_global.h"
#include "acs_rtr_lib.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_DSD_Server.h"

#include <ace/Task.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <string>
#include <list>
#include <vector>


class ACS_RTR_Manager;
class ACS_RTR_DefinitionStore;
class RTR_Events;
class RTR_statistics;
class ACS_RTR_ImmHandler;
struct CpStatisticsInfo;
struct StatisticsInfo;

class OmHandler;

class ACS_RTR_Server : public ACS_RTR_ErrorSupportBase , public ACE_Task_Base
{
 public:

	static const int RTRSRV_NO_ERROR = 0; //NO error
	static const int RTRSRV_ERR_NEW_DSD = -1;
	static const int RTRSRV_ERR_CREATE_STOP_EVENT = -2;
	static const int RTRSRV_ERR_OPEN_DSD = -3;
	static const int RTRSRV_ERR_PUBLISH_DSD = -4;
	static const int RTRSRV_ERR_GETHANDLES_DSD = -5;
	static const int RTRSRV_ERR_NEW_SYS_CONFIGURATION = -6;
	static const int RTRSRV_ERR_SYS_CONFIGURATION = -7;
	static const int RTRSRV_ERR_CREATE_EVENT_HANDLER = -8;
	static const int RTRSRV_ERR_NEW_DSD_SESSION = -9;
	static const int RTRSRV_ERR_DSD_ACCEPT = -10;
	static const int RTRSRV_ERR_DSD_RECV = -11;
	static const int RTRSRV_ERR_WAIT_FAILED = -12;
	static const int RTRSRV_ERR_NEW_MGR = -13;
	static const int RTRSRV_ERR_START_MGR = -14;
	static const int RTRSRV_ERR_SEND_DSD = -15;
	static const int RTRSRV_ERR_NEW_STAT = -16;
	static const int RTRSRV_ERR_INIT_STAT = -17;
	static const int RTRSRV_ERR_LOAD_DEF_FILE = -18;
	static const int RTRSRV_ERR_CHECK_DATADISK = -19;
	static const int RTRSRV_START_PROTOCOL_TIMEOUT = 5; //15 seconds
	static const int RTRSRV_GET_CONFIGURATION_TIMEOUT = 500;
	static const int RTRSRV_WAIT_DSDEVENT_TIMEOUT = 10;  // 10 sec
	static const int RTRSRV_WAIT_FINALIZE_ALL_MGR = 20000;
	static const int RTRSRV_MAX_RETRIES_ON_ERROR = 10;

	//==============
	// Constructor
	//==============
	ACS_RTR_Server();

	//==============
	// Destructor
	//==============
	virtual ~ACS_RTR_Server();

	//==============
	// Methods
	//==============
	virtual int open(void *args=0);

	bool addNewManager(const char* msName, const RTRMS_Parameters* parameters, const std::string& jobBaseDN);

	bool removeManager(const std::string& messageStoreName);

	bool isJobDefined(const std::string& messageStoreName);

	bool isJobDefined(const std::string& messageStoreName, const std::string& transferQueue);

	bool getManagerParameters(const std::string& messageStoreName, RTRMS_Parameters* currentSet);

	void setManagerParameters(const std::string& messageStoreName, RTRMS_Parameters* currentSet);

	void forceFileEnd(const std::string& messageStoreName);

	bool getCpStatistics(const std::string& messageStoreName, CpStatisticsInfo* cpStatisticsInfo, short cpId = DEFAULT_CPID);

	bool getMessageStoreStatistics(const std::string& messageStoreName, StatisticsInfo* msStatisticsInfo);

	bool stop();

	bool isAlive() const { return (thr_count() > 0); };

	bool fxIsRTRManager(const char* msname, const char* tq = 0);

	inline static RTR_statistics* getProcessStatistics() { return _rtrStat; }

	bool startImmHandler();

 private:

	virtual int svc();

	bool fxInitialize(ACE_INT32 waitTimeout);

	bool fxStartProtocol(ACE_INT32 waitTimeout);

	bool fxStopProtocol();

	bool fxRecovery();

	bool loadFileJobsFromIMM(OmHandler* objManager);

	bool loadBlockJobsFromIMM(OmHandler* objManager);

	bool fxWorking(int waitTimeoutSec, unsigned maxRetries);

	bool fxDsdProcessing();

	void fxProcessRequest(ACS_DSD_Session* session, const char* request);

	void fxProcessRTRStartService(ACS_DSD_Session* session, const char* request);

	void fxProcessRTRListStatistics(ACS_DSD_Session* session, const char* request);

	void fxProcessRTRChangeService(ACS_DSD_Session* session, const char* request);

	void fxProcessRTRForcedFileEnd(ACS_DSD_Session* session, const char* request);

	void fxProcessRTRStopService(ACS_DSD_Session* session, const char* request);

	void fxProcessUnknown(ACS_DSD_Session* session, const char* request);

	bool sendResponse(ACS_DSD_Session* session, const char* code);

	void defParPrintingFormat(char* str,int& len,const RTRMS_Parameters& defPar);

	bool sendMessage(ACS_DSD_Session* ses, respCode code, unsigned int len, const char* data); 

	void parPrintingFormat(char* str,int& len,const RTRMS_Parameters* par,const char* msname, bool stat,const char* site);

	void BC_parPrintingFormat(char* str,int& len,const RTRMS_Parameters* par,const char* msname,bool stat,const char* ap);

	bool fxStopManagers();

	void fxGarbageCollector();

	bool fxRestartManager(const std::string& oldMSName, const std::string& newMSName);

	ACS_RTR_Manager* fxGetRTRManager(const char* msname);

	ACS_RTR_Manager* getRTRManager(const std::string& messageStoreName);

	bool checkDatadisk();

	bool fxTQExists(RTROutputType outputType, const char* fileTqName,respCode& retVal);

	bool createRTRJob(const char *parentName, const int outputType, const char *transferQueue,std::string &errorText);

	bool modifyRTRJob(char *parentName, int* options, const rtr_ch_req_t* request, int OutputType,std::string &errorText);

	bool getParentName(char *parentName, const char* msName);

	int fxCreateRTRStatistics(int startMode);

	void waitBeforeRetry(int waitTimeOutSec);

	//==============
	// Attributes
	//==============
  private:

	ACE_HANDLE m_stopEvent;

	ACS_DSD_Server* m_DSDServer;

	std::vector<acs_dsd::HANDLE> m_dsdHandles;


	static RTR_statistics* _rtrStat;

	bool _stopRequested;

	std::list<ACS_RTR_Manager*> _managerList;

	ACS_RTR_ImmHandler* m_ImmHandler;

	/**
	   @brief      m_MsgQueueHandle
			  Message queue communication handle
	*/
	ACE_HANDLE m_MsgQueueHandle;

	/*
	 * @brief _managerCriticalSection
	 * 		  Mutex to protect the Manager deletion.
	 */

	ACE_Recursive_Thread_Mutex _managerCriticalSection;
};

#endif //_ACS_RTR_SERVER_H_
