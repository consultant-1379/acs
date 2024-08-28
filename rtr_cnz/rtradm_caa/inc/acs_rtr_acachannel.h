/*
 * * @file  acs_rtr_acachannel.h
 *	@brief
 *	Header file for ACS_RTR_ACAChannel class.
 *  This module contains the declaration of the class ACS_RTR_ACAChannel.
 *
 *	COPYRIGHT Ericsson AB, 2014
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2013-01-22 | XHARBAV      | File imported.                      |
 *	+========+============+==============+=====================================+
 *	| 1.1.0  | 2014-03-21 | QVINCON      | Changed.                 		   |
 *	+========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_RTR_ACACHANNEL_H
#define ACS_RTR_ACACHANNEL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */


#include "acs_rtr_lib.h"

#include <ace/Task.h>

#include <string>


/*=====================================================================
                         FORWARD CLASS DECLARATION SECTION
==================================================================== */
class ACS_RTR_Manager;
class MSread;
class ACS_RTR_OutputQueue;
class RTR_Events;
class RTR_statistics;
class RTRfile;
class RTRblock;


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

class ACS_RTR_ACAChannel : public ACE_Task<ACE_MT_SYNCH>
{
	enum
	{ 	// indexes in the HANDLE array
		stop_pos = 0,	// 0 Service shutdown
		rtrrm_pos, 		// 1 rtrrm command issued
		holdTime_pos,	// 2 file/block hold timer expired
		msclient_pos,	// 3 message store ready to read
		rtrfe_pos,		// 4 rtrfe command issued
	};

 public:

	//Error Codes
	static const int RTRACA_NO_ERROR = 0;
	static const int RTRACA_FILE_BUILDING_OK = 1;
	static const int RTRACA_BLOCK_BUILDING_OK = 2;
	static const int RTRACA_REPORTING_OK = 3;
	static const int RTRACA_ERROR = -1;
	static const int RTRACA_MSD_CONNECT_ERROR = -2;
	static const int RTRACA_ERR_DISK_FULL = -3;
	static const int RTRACA_CREATE_BLOCK_IF_ERROR = -4;
	static const int RTRACA_CREATE_FILE_IF_ERROR = -5;

	//Timeout in seconds
	static const int RTRACA_ACA_CONNECTION_ERROR_SLEEP	= 10; // Sleep on ACA connection errors
	static const int RTRACA_BUILDING_ERROR_SLEEP		= 30; // Sleep on Block/File interface errors
	static const int RTRACA_DISK_ERROR_SLEEP 			= 60; // Sleep on disk full
	static const int RTRACA_BUILDING_SLEEP 				= 120; // Safe sleep during normal working
	static const int RTRACA_INITIAL_BUILDING_SLEEP 		= 30; // Safe sleep before first reading loop

	//Retries
	static const int RTRACA_MAX_RETRIES_ON_CONNECT_ERROR = 30;

	/**
  	  @brief           Constructor for ACS_RTR_ACAChannel class.
	 */
	ACS_RTR_ACAChannel(ACS_RTR_Manager *parent,
						const std::string msName,
						const short cpID, 
						RTRMS_Parameters* rtrParams,
						HMACMD5_keys* rtrKeys,
						ACS_RTR_OutputQueue* q,
						RTR_statistics* statptr,
						uint32_t msRecordSize,
						RTR_statistics* msstat = 0
						);

	virtual ~ACS_RTR_ACAChannel();

	virtual int open(ACE_HANDLE& stopEvent, ACE_HANDLE& rmEvent);
	
	/**
   	   @brief   setRtrfeEvent
   	   	   Set the event for file end
	 */
	bool setRtrfeEvent();

	/**
   	   @brief      fxStop
	*/
	inline void fxStop() { _stopRequested = true; };

	/**
   	   @brief      isMsSleeping
   	   @return     bool
	 */
	bool isMsSleeping() const { return _msSleeping;};

	/**
   	   @brief      pfxCommitAcaMessages
                 used within the session/definition
   	   @return     bool
	 */
	bool pfxCommitAcaMessages();

	/**
   	   @brief      pfxCommitAcaMessages
                 usend within the session/definition
   	   @param      numOfMessages
   	   @return     bool
	*/
	bool pfxCommitAcaMessages(unsigned long long numOfMessages);

	/**
   	   @brief      getFSMState
                gets the status
   	   @return     int
	 */
	inline int getFSMState() const  { return _fsmStatus; };

	/**
   	   @brief      getCpID
                gets the Cp Id
	 */
	inline short getCpID() const { return _cpID; } ;

	/**
   	   @brief     getStatistics
                  gets the status
   	   @return     RTR_statistics
	 */
	inline RTR_statistics* getStatistics() { return _statptr; };


	/**
   	   @brief   changeRTRParameters
   	   @param   parameters
   	   @param   newKey
	 */
	void changeRTRParameters(RTRMS_Parameters* parameters, HMACMD5_keys* newKey);

	void search_for_all_files(const std::string& input_folder, std::list<std::string>& lstFiles, const char* file_seach_key);

	bool find_file(const std::string& input_folder, const char * file_seach_key, std::string& path_found);

	/**
   	   @brief      getLocalBlockQueue
   	   @return     ACS_RTR_OutputQueue
	 */
	inline ACS_RTR_OutputQueue* getLocalBlockQueue() { return _cpQueue; };

	bool isRunning() const { return !_stopRequested; };

	/**
		@brief 	This method updates the message store record size
	*/
	void changeMessageStoreRecordSize(const uint32_t& recordSize);

	/**
   	   @brief      ERR
                Error state
 	*/
	static const int ERR				= -1; 

	/**
   	   @brief      IDLE
                Idle state
	*/
	static const int IDLE				= 0;  

	/**
   	   @brief      INIT
                Init state
 	*/
	static const int INIT				= 1;  

	/**
   	   @brief      END
                End state
	*/
	static const int END				= 2;  

	/**
   	   @brief      RESTART_CHECK
                RestartCheck state
 	*/
	static const int RESTART_CHECK		= 3;  

	/**
   	   @brief      RECOVERY
                Recovery state
	*/
	static const int RECOVERY			= 4;  

	/**
   	   @brief      BUILDING
                Building state
	*/
	static const int BUILDING		= 5;  

	/**
   	   @brief      REPORTING
                Reporting state
	*/
	static const int REPORTING		= 6;  


private:

	/**
	   @brief      svc
		Aca Channel worker thread
	 */
	virtual int svc();

	/**
   	   @brief      init
                	initialization procedure
   	   @return     bool
	 */
	bool init();

	/**
   	   @brief      restartCheck
                restart method
 	*/
	void restartCheck();

	/**
   	   @brief      recovery
                restart method
   	   @param      nAcaMessages
	*/
	void recovery(unsigned int nAcaMessages = 0);

	/**
   	   @brief      recoveryBlockDef
                restart method
   	   @param      nAcaMessages
	*/
	void recoveryBlockDef(unsigned int nAcaMessages = 0);

	/**
   	   @brief      recoveryFileDef
                restart method
	*/
	void recoveryFileDef();

	/**
	   @brief      removeFileInDestDir
					restart method
	   @param      searchkey
	   @return     bool
	*/
	bool removeFileInDestDir(const char* searchkey);

	/**
	   @brief      isRTRFileInDestDir
					restart method
	   @param      rtrFile
	   @return     bool
	 */
	bool isRTRFileInDestDir(char* rtrFile = 0);


	/**
	   @brief      retransmitUnreportedFile
					restart method
	   @param      lcf
	   @return     void
	 */
	void retransmitUnreportedFile(unsigned int lcf);

	/**
   	   @brief      verifyNumOfFile
                restart method
   	  @param      rtrFile
  	  @param      lcf
   	  @return     bool
	 */
	bool verifyNumOfFile(const char* rtrFile, unsigned int lcf);

	/**
	   @brief      building
					build methods
	   @return     void
	 */
	void building();

	/**
   	   @brief      fileBuilding
                build methods
   	   @return     void
	*/

	void fileBuilding();

	/**
   	   @brief      blockBuilding
                build methods
   	   @return     void
	*/
	void blockBuilding();

/**
   @brief      waitOnProcessingError
                 skip the infinite loop on disk full and msd connection error

   @param      waitTimeout

   @return     bool
 */
/*=================================================================== */
	bool waitOnProcessingError(ACE_INT32 waitTimeout);
/*=================================================================== */
/**
   @brief      reportFile
                reporting methods

   @param      forcedEnd

   @return     int
 */
/*=================================================================== */
	int reportFile(bool forcedEnd = false);
/*=================================================================== */
/**
   @brief      reportBlock
                reporting methods

   @return     void
 */
/*=================================================================== */
	void reportBlock();
/*=================================================================== */
/**
   @brief      end
                normal termination procedure

   @return     void
 */
/*=================================================================== */
	void end();

	/**
   	   @brief      setFSMState
   	   @return     void
	 */
	inline void setFSMState(int status) {_fsmStatus = status;};

/*=================================================================== */
/**
   @brief      loadDirectoryStructure

   @return     bool
 */
/*=================================================================== */
	bool loadDirectoryStructure();
/*=================================================================== */
/**
   @brief      isREFFileInDestDir

   @return     bool
 */
/*=================================================================== */
	//bool existRefFile();
	bool isREFFileInDestDir();
/*=================================================================== */
/**
   @brief      makeHandles

   @return     bool
 */
/*=================================================================== */
	bool makeHandles();

	/**
   	   @brief      startTimer
   	   @return     bool
	 */
	bool startTimer();

	/**
   	   @brief      cancelTimer
   	   @return     bool
	 */
	bool cancelTimer();

	/**
   	   @brief      resetTimer without change the timer flag
   	   @return     bool
	 */
	bool resetTimer();

	/**
   	   @brief      frep_processACAEvent
                 process ACA event for file reporting
   	   @return     int
	 */
	int frep_processACAEvent();

	/**
   	   @brief      brep_processACAEvent
                process ACA event for block reporting
   	   @param      acaMsgToRecv
   	   @param      recoveryMode
   	   @return     int
 	*/
	int brep_processACAEvent(unsigned int& acaMsgToRecv, bool recoveryMode = false);

	/**
   	   @brief      frep_processTimeout
                 process timeout event for file reporting
   	   @return     int
 	*/
	int frep_processTimeout();

	/**
   	   @brief      brep_processTimeout
                 process timeout event for block reporting
   	   @return     void
 	*/
	void brep_processTimeout();

	/**
   	   @brief      createRTRFileInterface
   	   @return     void
 	*/
	void createRTRFileInterface();

	/**
   	   @brief      createRTRBlockInterface
   	   @return     void
 	*/
    void createRTRBlockInterface(bool startTimer = true);

	/**
   	   @brief      disconnectFromMessageStore
   	   @return     void
 	*/
	void disconnectFromMessageStore();

	/**
   	   @brief      connectToMessageStore
   	   @return     bool
 	*/
	bool connectToMessageStore(unsigned maxRetries = 10);

	/**
   	   @brief      metFileSkipConditions
   	   @return     bool
	*/
	bool metFileSkipConditions(unsigned int bufLen = 0);

	/**
   	   @brief      metBlockSkipConditions
   	   @return     bool
 	*/
	bool metBlockSkipConditions(unsigned int bufLen = 0);

	/**
   	   @brief      restoreACAConnection
   	   @return     bool
 	*/
	void restoreACAConnection();

	/**
   	   @brief     getRecSize
   	   @return     void
 	*/
	void getRecSize (char* jobBasedDn,unsigned short &recSize);

	/**
		   @brief     waitBeforeRetry
		   @param 	waitTimeOutSec timeout in seconds
		   @return     void
	 */
	void waitBeforeRetry(int waitTimeOutSec);

	/**
   	   @brief      msSleeping
                 set _msSleeping true
   	   @return     void
 	*/
	void msSleeping() { _msSleeping = true; };

	/**
   	   @brief      msWorking
                 set _msSleeping false
   	   @return     void
 	*/
	void msWorking() { _msSleeping = false; };

	/**
	   	   @brief	adaptiveDelayOnError
	   	   @param	errorCode
	   	   @return	ACE_UINT32
	 	*/
	ACE_UINT32 adaptiveDelayOnError(int errorCode);

	/**
	   @brief      _msSleeping
	                 flag for aca activity
	*/
	bool _msSleeping;

	/**
   	   @brief      _cpID
                 Execution context data
	 */
	short _cpID;

	/**
   	   @brief      _cpName
                Execution context data
	 */
	std::string _cpName;

	/**
   	   @brief      _msName
                 Execution context data
	 */
	std::string _msName;


	/**
   	   @brief      _msRoot
                 Execution context data
	*/
	std::string _msRoot;

	/**
   	   @brief      _destDir
                 Execution context data
	 */
	std::string _destDir;

	/**
   	   @brief      _repDir
                 Execution context data
	*/
	std::string _repDir;


	/**
   	   @brief      _REFfile
                 Execution context data
	*/
	std::string _REFfile;

	/**
   	   @brief      _isMultipleCpSystem
                 Execution context data
	*/
	bool _isMultipleCpSystem;

	/**
   	   @brief      _recsize
           the Message store record size is read from the ACA Model
	*/
	uint32_t _recsize;

	/**
   	   @brief      m_newRecordSize
           the Message store record size updated
	*/
	uint32_t m_newRecordSize;

	/**
	   @brief      m_recordSizeChanged
	     Flag to indicates a Message store record size change
	*/
	bool m_recordSizeChanged;

	/**
   	   @brief      _timeoutExpired
                 Flags used for ACAChannel state machine
 	 */
	bool _timeoutExpired;

	/**
   	   @brief      _fsmStatus
                Flags used for ACAChannel state machine
 	 */
	int _fsmStatus;

	/**
   	   @brief      _cpQueue
                 local cp queue for block transfer
	 */
	ACS_RTR_OutputQueue* _cpQueue;

	/**
   	   @brief      _messageStoreClient
                 ACA client interface used to read/commit/rollback message stores
	 */
	MSread*	_messageStoreClient;

	/**
   	   @brief      _acaClientHandle
                 ACA client interface used to read/commit/rollback message stores
 	 */
	ACE_HANDLE	_acaClientHandle;

	/**
	   @brief      m_rtrRMEvent
	          To signal a remove of File/Block Job
	 */
	ACE_HANDLE m_rtrRMEvent;

	/**
	   @brief      m_rtrFEEvent
	          To signal a File end
	 */
	ACE_HANDLE m_rtrFEEvent;

	/**
	   @brief      m_stopEvent
	          To signal a service shutdown
	 */
	ACE_HANDLE m_stopEvent;
	/**
	   @brief      m_stopEvent
	          To signal a timer elapsed
	*/
	ACE_HANDLE m_TimerEvent;

	/**
   	   @brief    _fobj
                 _fobj & _bobj are created and passed to OutputQueue within
                 an ACS_RTR_TransmitInfo object.
                 They will be deleted by ACS_RTR_GOHFileReported thread when it will destroy the
                 ACS_RTR_TransmitInfo got from ACS_RTR_OutputQueue, after its processing.
	*/
	RTRfile* _fobj;

	/**
   	   @brief      _bobj
                 _fobj & _bobj are created and passed to OutputQueue within
                 an ACS_RTR_TransmitInfo object.
                 They will be deleted by ACS_RTR_GOHFileReported thread when it will destroy the
                 ACS_RTR_TransmitInfo got from ACS_RTR_OutputQueue, after its processing.
	*/
	RTRblock* _bobj;

	/**
   	   @brief      _rtrParams
                 working rtr parameters
 	*/
	RTRMS_Parameters* _rtrParams;

	/**
   	   @brief      _changedParams
                 changed parameters
 	*/
	RTRMS_Parameters _changedParams;

	/**
   	   @brief      _rtrchIssued
	*/
	bool _rtrchIssued;

	/**
   	   @brief      _outputQueue
                  RTR definition/ACS_RTR_Manager level references
 	*/
	ACS_RTR_OutputQueue* _outputQueue; //shared queue

	/**
   	   @brief      _statptr
                  RTR definition/ACS_RTR_Manager level references
 	*/
	RTR_statistics*	_statptr;

	/**
   	   @brief      _msstat
                  RTR definition/ACS_RTR_Manager level references
 	*/
	RTR_statistics*	_msstat;

	/**
   	   @brief      _parent
	*/
	ACS_RTR_Manager* m_MSManager;

	/**
   	   @brief      _safeBadMsgCounter
 	*/
	int _safeBadMsgCounter;

	/**
   	   @brief      m_acaMutex
	*/
	ACE_Recursive_Thread_Mutex m_acaMutex;

	/**
  	  @brief _stopRequested
	*/
	bool _stopRequested;

	/**
	 	   @brief      m_jobRemoved
	      used for rtrrm
	 */
	bool m_jobRemoved;

    /**
    	   @brief      _RTRkeys
                   Encription keys
  	*/
   	HMACMD5_keys _RTRkeys;

	HMACMD5_keys m_changedKeys;

};

#endif
