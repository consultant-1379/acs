/**
   @file  acs_rtr_manager.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       24/01/2013   XHARBAV   	Initial Release
   N/A       18/12/2013   QVINCON   	Re-factoring
*/

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_RTR_MANAGER_H_
#define _ACS_RTR_MANAGER_H_

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */

#include "acs_rtr_errorsupportbase.h"
#include "acs_rtr_global.h"
#include "acs_rtr_lib.h"

#include "ace/ACE.h"
#include "ace/Task.h"

#include <string>
#include <list>
#include <map>

/*=====================================================================
                         FORWARD CLASS DECLARATION SECTION
==================================================================== */
class ACS_RTR_ACAChannel;
class ACS_RTR_OutputQueue;
class RTR_Events;
class RTR_statistics;
class RTRMS_Parameters;
class ACS_RTR_GOHReporter;
class ACS_ACA_MessageStoreInterface;

struct CpStatisticsInfo;
struct StatisticsInfo;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      ACS_RTR_Manager
 */
/*=================================================================== */
class ACS_RTR_Manager : public ACS_RTR_ErrorSupportBase, public ACE_Task_Base
{

 public:

  /**
   @brief      RTRMGR_NO_ERROR
  */
  static const int RTRMGR_NO_ERROR = 0; //NO error

	/**
   @brief      RTRMGR_ERR_SEND_DSD
 */
/*=================================================================== */
	//Error constants
	static const int RTRMGR_ERR_SEND_DSD = -1;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_TQ_EXISTS
 */
/*=================================================================== */
	static const int RTRMGR_ERR_TQ_EXISTS = -2;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_WRITE_PARAMETERS
 */
/*=================================================================== */
	static const int RTRMGR_ERR_WRITE_PARAMETERS = -3;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_OUT_QUEUE
 */
/*=================================================================== */
	static const int RTRMGR_ERR_OUT_QUEUE = -4;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_NEW_GOH_THREAD
 */
/*=================================================================== */
	static const int RTRMGR_ERR_NEW_GOH_THREAD = -5;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_START_GOH_THREAD
 */
/*=================================================================== */
	static const int RTRMGR_ERR_START_GOH_THREAD = -6;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_GET_CP_FOR_NAME
 */
/*=================================================================== */
	static const int RTRMGR_ERR_GET_CP_FOR_NAME = -7;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_STATISTICS
 */
/*=================================================================== */
	static const int RTRMGR_ERR_STATISTICS = -8;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_NEW_ACA_CHANNEL
 */
/*=================================================================== */
	static const int RTRMGR_ERR_NEW_ACA_CHANNEL = -9;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_START_ACA_CHANNEL
 */
/*=================================================================== */
	static const int RTRMGR_ERR_START_ACA_CHANNEL = -10;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_MAP_INSERT
 */
/*=================================================================== */
	static const int RTRMGR_ERR_MAP_INSERT = -11;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_WAIT_FAILED
 */
/*=================================================================== */
	static const int RTRMGR_ERR_WAIT_FAILED = -12;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_GET_CPID
 */
/*=================================================================== */
	static const int RTRMGR_ERR_GET_CPID = -13;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_TQ_RESTORED_EXISTS
 */
/*=================================================================== */
	static const int RTRMGR_ERR_TQ_RESTORED_EXISTS = -14;
/*=================================================================== */
/**
   @brief      RTRMGR_ERR_INTERNAL
 */
/*=================================================================== */
	static const int RTRMGR_ERR_INTERNAL = -15;
/*=================================================================== */
/**
   @brief      RTRMGR_WAIT_TIMEOUT
 */
/*=================================================================== */
	
	static const int  RTRMGR_WAIT_TIMEOUT = 60;
/*=================================================================== */
/**
   @brief      RTRMGR_TQ_ERROR_RETRY_TIMEOUT
 */
/*=================================================================== */
	static const int RTRMGR_TQ_ERROR_RETRY_TIMEOUT = 3; //sec

	static const int RTRMGR_STATISTIC_ERROR_RETRY_TIMEOUT = 1; //sec
/*=================================================================== */
/**
   @brief      RTRMGR_GOH_STOP_TIMEOUT
 */
/*=================================================================== */
	static const int RTRMGR_GOH_STOP_TIMEOUT = 2000;
/*=================================================================== */
/**
   @brief      RTRMGR_ACACHANNEL_STOP_TIMEOUT
 */
/*=================================================================== */
	static const int RTRMGR_ACACHANNEL_STOP_TIMEOUT = 2000;
/*=================================================================== */
/**
   @brief      RTRMGR_MAX_RETRIES_ON_ERROR
 */
/*=================================================================== */
	static const int RTRMGR_MAX_RETRIES_ON_ERROR = 5;
/*=================================================================== */
/**
   @brief      RTRMGR_MAX_RETRIES_ON_TQ_ERROR
 */
/*=================================================================== */
	static const int RTRMGR_MAX_RETRIES_ON_TQ_ERROR = 30;

	/**
   	   @brief      Constructor for ACS_RTR_Manager class.
	   @param      msname
	   @param      parameters
	*/
	ACS_RTR_Manager(const char* msname,	const RTRMS_Parameters* parameters);

	virtual ~ACS_RTR_Manager();

	bool isAlive() const { return (thr_count() > 0); };

	/**
	   @brief      fxStop
	   @return     void
	*/
    void fxStop();

    void fxStopOnRename();

	/**
	   @brief      onStop
	   @return     void
	*/
	void onStop();

	/**
	   @brief      getCpStatistics
	   @return     bool
	*/
	bool getCpStatistics(CpStatisticsInfo* cpStatisticsInfo, short cpId);

	/**
	   @brief      getMessageStoreStatistics
	   @return     bool
	*/
	bool getMessageStoreStatistics(StatisticsInfo* msStatisticsInfo);

	/**
   	   @brief      getMessageStore
   	   @return     string
	*/
	inline std::string getMessageStore() const { return _msname; }

	/**
   	   @brief      getTransferQueue
   	   @return     string
	*/
	inline std::string getTransferQueue() const { return _tq; }

   /**
   	   @brief      getStatistics
   	   @return     RTR_statistics
   */

	inline RTR_statistics* getStatistics() const { return _msStat; }

	/**
   	   @brief      getRTRParameters
   	   @return     RTRMS_Parameters
	*/
	inline RTRMS_Parameters * getRTRParameters() const { return _rtrParams; }

	/**
   	   @brief      isRtrrmIssued
   	   @return     bool
	*/
	inline bool isRtrrmIssued() { return _rtrrmIssued; }

	/**
	   @brief      isFileOutput
	   @return     bool
	*/
	inline bool isFileOutput() { return ((_rtrParams->OutputType == FileOutput) ? true : false); }

	/**
   	   @brief      restartNeed
   	   @return     bool
	*/
	bool restartNeed();

	/**
   	   @brief      rtrrmIssue
   	   @return     bool
	*/
	bool rtrrmIssue();

	/**
   	   @brief      rtrfeIssue
   	   @return     bool
	*/
	bool rtrfeIssue();

	/**
   	   @brief      rtrchIssue
   	   @param      changedParams
   	   @return     bool
	*/
	bool rtrchIssue(RTRMS_Parameters* changedParams);

	/**
   	   @brief      pfxCommitAcaMessages
   	   @param      num
   	   @param      cpname
   	   @return     bool
	*/
	bool pfxCommitAcaMessages(unsigned long long num, std::string cpname);

	/**
   	   @brief      checkForNotCommittedAcaMessages
	*/
	bool checkForNotCommittedAcaMessages(std::string &_cpName, unsigned int & nAcaMessages, short cpId = DEFAULT_CPID);

	/**
   	   @brief      getChannelList
   	   @return     ACS_RTR_ACAChannel
	*/
	std::list<ACS_RTR_ACAChannel*> getChannelList();

	/**
   	   @brief      getCPQueue
	   @param      cpId
   	   @return     ACS_RTR_OutputQueue
	*/
	ACS_RTR_OutputQueue* getCPQueue(short cpId);

	/**
   	   @brief      fxTQExists
   	   @return     bool
	*/
	bool fxTQExists();

	/**
	   @brief      fxCreateSharedQueue
	   @return     bool
	*/
    bool fxCreateSharedQueue();

    /**
       @brief      open
       	   Start the worker thread of RTR Manager object
       @return     int
    */
	virtual int open(const ACE_HANDLE& stopHandle);

	/**
	   @brief      isRunning
		   Return the state of the worker thread of RTR Manager
	  @return     bool
	*/
	inline bool isRunning() const { return !_stopManager; };

	/**
	   @brief      setJobBaseDN
		  Sets the DN of the RTR job
	*/
	void setJobBaseDN(const char* jobBaseDN) { m_jobBasedDN.assign(jobBaseDN); };

	/**
	   @brief      setJobBaseDN
		  Gets the DN of the RTR job
	*/
	std::string getJobBaseDN() const { return m_jobBasedDN; };

 private:

	enum Event_Indexes
	{
		Shutdown_Event_Index			= 0, // Service shutdown
		Message_Queue_Event_Index 		= 1, // message queue event used for ACA / RTR communication
		Job_Remove_Event_Index			= 2, // job remove event
		Children_Stop_Event_Index 		= 3, // children shutdown signalled by Server in case of rename
		Number_Of_Events 	= 4
	};

	/**
   	   @brief      fxCreateGohReporter
   	   @return     bool
	*/
	bool fxCreateGohReporter();

	/**
   	   @brief      fxCreateAcaChannels
   	   @param      maxRetries
   	   @return     bool
	*/
	bool fxCreateAcaChannels(int maxRetries = RTRMGR_MAX_RETRIES_ON_ERROR);

	/**
	   @brief      fxCreateChannel
	   @param      cpid
	   @return     bool
	*/
	bool fxCreateChannel(short cpid);

	/**
	   @brief      fxLookForNewChannel
	   @return     bool
	*/
	bool fxLookForNewChannel();

	/**
   	   @brief      fxCheckChilds
   	   @return     bool
	*/
	bool fxCheckChilds();

	/**
   	   @brief      fxCheckGohReporter
   	   @param      maxRetries
   	   @return     bool
	*/
	void fxCheckGohReporter(int maxRetries = RTRMGR_MAX_RETRIES_ON_ERROR);

	/**
   	   @brief      fxCheckACAChannels
   	   @return     bool
	 */
	bool fxCheckACAChannels();

	/**
	   @brief      fxDestroyGOHReporter
	   @param      onstop
	   @return     bool
	 */
	bool fxDestroyGOHReporter(bool onstop = false);

	/**
	   @brief      fxDestroyACAChannels
	   @param      onstop
	   @return     bool
	*/
	bool fxDestroyACAChannels(bool onstop = false);

	/**
	   	@brief 	This method is the working loop of RTR manager thread
	*/
	virtual int svc (void );

	/**
	   	@brief 	This method gets Data Record keys (ipadding/opadding) from IMM
	*/
	bool getDataRecordKeys();

	/**
		@brief 	This method gets MD5 password from IMM
	*/
	bool getHMACMD5Password();

	/**
		@brief 	This method open the message queue
	*/
	bool openMsgQueue();

	/**
		@brief 	This method get the default message store name from the Job DN
	*/
	bool getMessageStoreDefaultName(std::string& messageStoreName);

	/**
		@brief 	This method get the message store record size from IMM
	*/
	bool getMessageStoreRecordSize();

	/**
			@brief 	This method wait on timeout or return on shutdown event
	*/
	void waitBeforeRetry(int waitTimeOutSec, int waitTimeOutMs = 0U);
	
	/**
			@brief 	This method is used to pack 32 Byte HEX to 16 Byte ASCII
	*/
	void packHexString(int ln,char* hexStr,char* pHexStr);

	/**
   	   @brief      _msname
                  message store
	*/
	std::string _msname;

	/**
	   @brief      m_messageStoreRecordSize
				The message store record size
	*/
	uint32_t m_messageStoreRecordSize;

	/**
   	   @brief      _tq
                  transfer queue name
	*/
	std::string _tq;

	/**
   	   @brief      _rtrParams
                  rtr parameters
 	*/
	RTRMS_Parameters* _rtrParams;

	/**
   	   @brief      _acaInterface
                   aca interface
 	*/
	ACS_ACA_MessageStoreInterface* _acaInterface;


	/**
   	   @brief      _eventHandler
	*/
	RTR_Events* _eventHandler;

	/**
   	   @brief      _msStat
     	 in a Multiple CP System: these statistics are valid only at MS level;
	  	 in Single CP System: these statistics are valid for the whole definition.
	*/
	RTR_statistics* _msStat;


	/**
   	   @brief      _gohReporter
	*/
	ACS_RTR_GOHReporter* _gohReporter;

	/**
   	   @brief      _outputQueue
 	*/
	ACS_RTR_OutputQueue* _outputQueue;

	typedef std::map<short, ACS_RTR_ACAChannel*> maptype;
	/**
   	   @brief      _channelMap
                  [key = cpID, value = acaChannel*]
	 */
	maptype _channelMap;

	/**
   	   @brief      m_stopEvent
   	   	   Stop request from the server
 	*/
	ACE_HANDLE m_stopEvent;

	/**
	   @brief      m_ACAChannelStopEvent
	*/
	ACE_HANDLE m_ChildStopEvent;

	/**
	   @brief      m_JobRemoveEvent
			  To signal a remove of File/Block Job
	 */
	ACE_HANDLE m_JobRemoveEvent;

	/**
	   @brief      m_MsgQueueHandle
			  Message queue communication handle
	*/
	ACE_HANDLE m_MsgQueueHandle;


	bool _stopManager;

	/**
	   @brief    _rtrrmIssued
	*/
	bool _rtrrmIssued;

	/**
	   @brief    m_jobBasedDN
	   	   DN of RTR job
	*/
	std::string  m_jobBasedDN;

	/**
	   @brief      m_DataRecordKeys
		   Encription keys
	*/
	HMACMD5_keys m_DataRecordKeys;

	/**
		@brief    _gohReporterCriticalSection
	*/
	ACE_Recursive_Thread_Mutex _gohReporterCriticalSection;

};

#endif //_ACS_RTR_MANAGER_H_
