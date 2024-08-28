/*=================================================================== */
/**
   @file   acs_ssu_svc_loader.h

   @brief Header file for SSU Loader module.

          This module contains all the declarations useful to
          specify the SSU_Service_Loader class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/06/2010     XSIRKUM        APG43 on Linux.

==================================================================== */
#ifndef _ACS_SSU_SVC_LOADER_H_
#define _ACS_SSU_SVC_LOADER_H_


#include <iostream>
#include <map>
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Task.h>
#include <ace/Task_T.h>
#include <ace/OS.h>
#include <ace/Event_Handler.h>
#include <acs_apgcc_omhandler.h>
#include <ace/Monotonic_Time_Policy.h>
#include <acs_apgcc_objectimplementereventhandler_V2.h>
#include "acs_ssu_perf_mon.h"
#include "acs_ssu_disk_mon.h"
//#include "acs_ssu_perf_mon_objectimpl.h"
//#include "acs_ssu_disk_mon_objectimpl.h"

//! For CUTE to access private methods, ACS_SSU_CUTE_TEST is defined only in Cute Make file
#ifdef ACS_SSU_CUTE_TEST
   #include <Cute_SSU_Monitor_Service.h>
#endif

#define NO_OF_HANDLES      3

class ACS_SSU_AlarmHandler;
class ACS_SSU_FolderQuotaMon;
class ACS_SSU_CommandHandler;
class ACE_Reactor;
class ACE_TP_Reactor;
class ACS_SSU_DiskMon_ObjectImpl;
class ACS_SSU_PerfMon_ObjectImpl;

typedef enum {
	EXIT_DAEMON = 0, /* we are the daemon                  */
	EXIT_OK = 1,	 /* caller must exit with EXIT_SUCCESS */
	EXIT_ERROR = 2	 /* caller must exit with EXIT_FAILURE */
} daemon_exit_t;

class ACS_SSU_Service;

/*!
* @class SSU_Service_Loader
*
* @brief Implementation class for SSU  Monitor Service
*/

class SSU_Service_Loader: /*public ACE_Event_Handler*/ public ACE_Task<ACE_SYNCH,ACE_Monotonic_Time_Policy>
{

public:
	/*!
	*Constructor for SSU Service Loader class, Parameters Mutex and ACE_Condition
	*are used for sending the Terminate Signal to the service on SIGTERM or Ctrl-C
	*/
	SSU_Service_Loader();
	/** @brief SSU_Service_Loader Destructor
	*
	*	SSU_Service_Loader Destructor of class
	*
	*	@remarks Remarks
	*/
	virtual ~SSU_Service_Loader(void);
	/*!
	* @brief Parse arguments passed
	* @param argc Number of arguments
	* @param argv Array of character pointer to arguments
	* @return < 0 Exit with Failure
	* @return > 0 Exit with Success
	* @return =0 Run Daemon
	*/
	ACE_INT32 parseArgs(ACE_UINT32 argc, ACE_TCHAR *argv[]);
	/**
	* @brief	runService
	* @param    None
	* @return   int
	*/
	int runService(void);
	/**
	* @brief	printUsageAndDie
	* @param    const ACE_TCHAR*
	* @return   void
	*/
	void printUsageAndDie(const ACE_TCHAR *prog);
	/**
	* @brief	execute
	* @param    None
	* @return   ACE_INT32
	*/
	virtual ACE_INT32 execute (void);
	/**
	* @brief	initService - Initialize Any member variables
	* @param    None
	* @return   void
	*/
	void initService();
	/**
	* @brief Timeout handler for terminating interactive mode
	* @param current_time Time that our event handler was selected for dispatching
	* @param arg  Pointer passed at the time of registration
	* @return
	*/
	int handle_timeout(const ACE_Time_Value &current_time, const void * arg);
	/**
	* Signal handler function
	* @brief This function handles interrupt and terminate signals.
	*        If terminate signal encounters, then an event will be raised regarding the SSU monitor service termination.
	*
	*/
	int handle_signal(int signum, siginfo_t * siginfo=0, ucontext_t * ucontext=0);
	/*!
	* @brief This function is thread call back function for Disk Monitoring thread
	*        after it is spawned
	* @param pvThis Pointer to Service Loader
	* @return ACE Thread function return Value, zero returned by default
	*/
	static ACE_THR_FUNC_RETURN DiskMonThread(void* pvThis);
	/*!
	* @brief This function is thread call back function for performance monitoring
	*        after it is spawned
	* @param pvThis Pointer to Service Loader
	* @return ACE Thread function return Value
	*/
	static ACE_THR_FUNC_RETURN PerfMonThread(void* pvThis);
	/*!
	*
	* @param pvThis
	* @return
	*/
	static ACE_THR_FUNC_RETURN FolderQuotaMonThread(void* pvThis);
	/**
	* Shuts down SSU Monitor Service
	*/
	int shutdown();
	/**
	* @brief	startSSUFunctionalityinActive - start SSU functionality in ACTIVE mode.
	* @return   ACS_CC_ReturnType
	*/
	ACS_CC_ReturnType startSSUFunctionalityinActive();
	/**
	* @brief	startSSUFunctionalityinPassive - start SSU functionality in PASSIVE mode.
	* @return   ACS_CC_ReturnType
	*/
	ACS_CC_ReturnType startSSUFunctionalityinPassive();
	/**
	* @brief	startSSUFunctionalityInteractiveMode - start SSU functionality in DEBUG mode.
	* @return   ACS_CC_ReturnType
	*/
	ACS_CC_ReturnType startSSUFunctionalityInteractiveMode();
	/**
	* @brief	active - starts SSU functionality in ACTIVE mode.
	* @param  Pointer to SSU HA class
	* @return   int
	*/
	int active(ACS_SSU_Service*);
	/**
	* @brief	passive - starts SSU functionality in PASSIVE mode.
	* @param  Pointer to SSU HA class
	* @return   int
	*/
	int passive(ACS_SSU_Service*);
	/**
	* @brief	stop - stops SSU functionality.
	* @param    None
	* @return   None
	*/
	void stop();
#if 0
	/**
	* @brief	getAppThreadId - gives SSU application thread ID.
	* @param    None
	* @return   None
	*/
	ACE_thread_t getAppThreadId()
	{
		return applicationThreadId;
	}
#endif
	/*=================================================================== */
	/** @brief   m_bInteractive.
	*/
	/*=================================================================== */
	static bool m_bInteractive;

private:

	/**
	* Main service running function
	* @return Returns Exit code of Service
	*/
	ACE_INT32 run_i(void);
	/**
	* Config file for SSU Monitor Service
	* @todo To be later checked whether this needs to be removed
	*/
	std::string config_file_;
	/**
	* pidfile for storing Service Pid, this is used for debug purpose
	*/
	std::string pid_file_;
	/**
	* Get the lock on pid_file used for running only one instance of the service
	*/
	bool  getLock(const char *lock_file_path);
	/**
	* @brief	getProcessLock
	* @param    ACE_HANDLE
	* @param    const char *
	* @return   bool
	*/
	bool getProcessLock( ACE_HANDLE & fd, const char *path);
	/**
	* @brief	lockFd
	* @param    ACE_HANDLE
	* @return   ACE_INT32
	*/
	ACE_INT32 lockFd(const ACE_HANDLE fd);
	/**
	* @brief	unlockFd
	* @param    ACE_HANDLE
	* @return   ACE_INT32
	*/
	ACE_INT32 unlockFd(const ACE_INT32 fd);
	/**
	* @brief	setupMonitorThreads - Sets and starts Monitoring threads
	* @param    None
	* @return   ACE_INT32
	*/
	ACE_INT32 setupMonitorThreads();
	/**
	* @brief	setupMonitorThreadsinActive - Sets and starts Monitoring threads in ACTIVE node
	* @param    None
	* @return   ACE_INT32
	*/
	ACE_INT32 setupMonitorThreadsinActive();
	/**
	* @brief	setupMonitorThreadsinPassive - Sets and starts Monitoring threads in PASSIVE node
	* @param    None
	* @return   ACE_INT32
	*/
	ACE_INT32 setupMonitorThreadsinPassive();
	/**
	* @brief	setupShutdownHandler - Registers signal handler for as call back for handling signals
	* @param    pointer to ACE_Reactor
	* @return   bool
	*/
	bool setupShutdownHandler( ACE_Reactor *poReactor );
	/**
	* @brief	DiskMon - Disk Monitoring Object is created and its execution loop is invoked in this function
	* @return   None
	*/
	void DiskMon();
	/**
	* @brief	PerfMon - Performance Monitoring Object is created and its execution loop is invoked in this function
	* @return   None
	*/
	void PerfMon();
	/**
	* @brief	FolderQuotaMon - Folder Monitoring Object is created and its execution loop is invoked in this function
	* @return   None
	*/
	void FolderQuotaMon();
	/**
	* Daemonize Function
	*
	* @return Success or Failure
	*/
	daemon_exit_t becomeDaemon();
	/**
	* Sets Timeout for interactive mode
	* @param u64TimeOut
	* @return TimerId
	*/
	ACE_INT32 setTimeoutForInteractiveMode( ACE_UINT64  u64TimeOut);
    /**
     * @brief s32ResetTimer
     * @param ACE_INT32
     * @return ACE_INT32
     */
	ACE_INT32 s32ResetTimer(ACE_INT32 s32Timer);
	/**
	@brief          applicationThreadId
	*/
	//! Descriptor to pid file
	/**
	@brief          lockFd_
	*/
	ACE_HANDLE lockFd_;
	/**
	@brief          m_poServiceHandler
	*/
	static SSU_Service_Loader *m_poServiceHandler;
	/**
	@brief          m_pAlarmHandler
	*/
	ACS_SSU_AlarmHandler * m_pAlarmHandler;
	/*=================================================================== */
	/** @brief   m_poReactor.
	*/
	/*=================================================================== */
	ACE_Reactor *m_poReactor;
	/**
	@brief          m_s64ExecuteTimeout
	*/
	ACE_INT64 m_s64ExecuteTimeout;
	/**
	@brief          thetpReactorImpl
	*/
	ACE_TP_Reactor *thetpReactorImpl;
	/*=================================================================== */
	/** @brief   m_plhEventHandles.
	*/
	/*=================================================================== */
	ACE_Event *m_plhEventHandles[NO_OF_HANDLES];
	/*=================================================================== */
	/** @brief   m_lpEndEvent.
	*/
	/*=================================================================== */
	static ACE_Event *m_lpEndEvent;
	/*=================================================================== */
	/** @brief   m_s32TimerId.
	*/
	/*=================================================================== */
	ACE_INT32 m_s32TimerId;
	/*=================================================================== */
	/** @brief   m_pPerfMonitor.
	*/
	/*=================================================================== */
	ACS_SSU_PerfMon *m_pPerfMonitor;
	/*=================================================================== */
	/** @brief   m_pDiskMonitor.
	*/
	/*=================================================================== */
	ACS_SSU_DiskMon *m_pDiskMonitor;
	/*=================================================================== */
	/** @brief   m_pFolderQuotaMon.
	*/
	/*=================================================================== */
	ACS_SSU_FolderQuotaMon * m_pFolderQuotaMon;
	/*=================================================================== */
	/** @brief   omHandlerPtr.
	*/
	/*=================================================================== */
	OmHandler* omHandlerPtr;
    /**
    @brief          m_haObj
    */
    ACS_SSU_Service* m_haObj;

//! Only defined for CUTE
#ifdef ACS_SSU_CUTE_TEST
    friend class Cute_SSU_Monitor_Service;
#endif

};

#endif /* _ACS_SSU_SVC_LOADER_H_ */
