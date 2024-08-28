
/*=================================================================== */
/**
	@file		acs_logm_svc_loader.h

	@brief		Header file for LOGM SVC  module.

			This module contains all the declarations for
			utilities required for service startup.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       21/10/2010     XKUSATI   Initial Release
N/A       17/01/2013     XCSRPAD   Deletion of unwanted CMX log files
==================================================================== */
//******************************************************************************
/*=====================================================================
			DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef _ACS_LOGM_SVC_LOADER_H_
#define _ACS_LOGM_SVC_LOADER_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Monotonic_Time_Policy.h>
#include <ACS_CC_Types.h>
#include <ace/Task.h>
#include <ace/Task_T.h>
#include <ace/OS.h>
//#include <acs_logm_logparam_oihandler.h>
//#include <acs_logm_srvparam_oihandler.h>


//! For CUTE to access private methods, ACS_LOGM_CUTE_TEST is defined only in Cute Make file
#ifdef ACS_LOGM_CUTE_TEST
   #include <Cute_ACS_LOGM_Service.h>
#endif

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief NO_OF_HANDLES
 */
/*=================================================================== */
#define NO_OF_HANDLES      1

/*=====================================================================
			FORWARD DECLARATION SECTION
=======================================================================*/
class ACS_LOGM_AlarmHandler;
//class acs_logm_srvparam_oihandler;
//class acs_logm_logparam_oihandler;
/*=================================================================== */
/**
   @brief daemon_exit_t
 */
/*=================================================================== */
typedef enum {
	EXIT_DAEMON = 0, /* we are the daemon                  */
	EXIT_OK = 1,	 /* caller must exit with EXIT_SUCCESS */
	EXIT_ERROR = 2	 /* caller must exit with EXIT_FAILURE */
} daemon_exit_t;



/*=====================================================================
			CLASS DECLARATION SECTION
=======================================================================*/
/*=====================================================================*/
/**
	@brief		LOGM_Service_Loader

			Implementation class for LOGM Service
 */
/*======================================================================= */

class LOGM_Service_Loader:public ACE_Task<ACE_SYNCH,ACE_Monotonic_Time_Policy>
{

	/*====================================================================
			   PUBLIC DECLARATION SECTION
	====================================================================== */
   public:

	/*=====================================================================
			   CLASS CONSTRUCTORS
	======================================================================= */
	/**

	@brief		Constructor for LOGM_Service_Loader class.
			Constructor for LOGM Service Loader class, Parameters Mutex and ACE_Condition
			are used for sending the Terminate Signal to the service on SIGTERM or Ctrl-C


	 */
	/*===================================================================== */
	LOGM_Service_Loader();

	void stop();

	/*=====================================================================
				CLASS DESTRUCTOR
	======================================================================= */
	/*=================================================================== */
	/**

	@brief		Destructor for LOGM_Service_Loader class.

	 */
	/*=================================================================== */
	virtual ~LOGM_Service_Loader(void);
	/*=================================================================== */
	/*!
	 * @brief 	parseArgs
	 * 		Parse arguments passed
	 *
	 * @param 	argc Number of arguments

	 * @param argv Array of character pointer to arguments
	 *
	 * @return	< 0 Exit with Failure
			> 0 Exit with Success
			= 0 Run Daemon
	 */
	/*=================================================================== */
	ACE_INT32 parseArgs(	ACE_UINT32 argc, 
				ACE_TCHAR* argv[]);

	/*=================================================================== */
	/* @brief 	runService
	 *
	 * return	the status of daemon
			< 0 Exit with Failure
			> 0 Exit with Success
			=0 Run Daemon
	 */
	/*=================================================================== */
	int runService(void);
	/*=================================================================== */
	/* @brief	printUsageAndDie
 	 * 		Prints the statements of Logmaint usage if user passes parameter other than -d.
			Prints the statements of Logmaint usage if user passes parameter other than -d

	   @param 	prog
			Name of program
	 */
	/*=================================================================== */
	void printUsageAndDie(const ACE_TCHAR* prog);
	/*=================================================================== */
	/* @brief 	executeDebug
	 * 	  	Executes Logmaint in debug mode by setting up new Logmaint thread
	 *
	 * @param 	argc Number of arguments
	 *
	 * @param 	argv List of arguments
	 */
	/*=================================================================== */
	void executeDebug(	int argc,
				ACE_TCHAR* argv[]);
	/*=================================================================== */
	/*
	 * @brief  execute
	 * 	   Calls runservice function
	 */
	/*=================================================================== */
	virtual ACE_INT32 execute (void);
	/*=================================================================== */
	/**
	 * @brief  initService
	 * 	   Initialize Any member variables
	 */
	/*=================================================================== */
	void initService();
	/*=================================================================== */
	/**
	 * @brief 	Timeout handler for terminating interactive mode
	 *
	 * @param 	current_time Time that our event handler was selected for dispatching
	 *
	 * @param 	arg  Pointer passed at the time of registration
	 *
	 * @return	int
	 */
	/*=================================================================== */
	int handle_timeout(	const ACE_Time_Value& current_time,
				 const void * arg);
	/*=================================================================== */
	/**
	* @brief 	handle_signal
	*
	* @param 	signum
	*
	* @param 	siginfo
	*
	* @param 	ucontext
	*
	* @return	int
	*/
	/*=================================================================== */
	int handle_signal(	int signum, 
				siginfo_t* siginfo=0, 
				ucontext_t* ucontext=0);
	/*=================================================================== */
	/**
	 * @brief 	getProcessLock
	 * 		Locks the pid file so that no other Logmaint process runs

	 * @param 	fd
	 *
	 * @param 	path

	 * @return	bool
	 */
	/*=================================================================== */
	bool getProcessLock( 	ACE_HANDLE& fd, 		
				const char* path);
	/*=================================================================== */
	/*!
	 * @brief	LOGMServiceThread
	 * 		This function is thread call back function for LogM cleanup thread
	 *          	after it is spawned
	 *
	 * @param 	pvThis 
			Pointer to Service Loader
	 *
	 * @return 	ACE Thread function return Value, zero returned by default
	 */
	/*=================================================================== */
	static ACE_THR_FUNC_RETURN LOGMServiceThread(void* pvThis);
	/*=================================================================== */
        /*!
         * @brief       LOGMServiceThreadForSCX
         *              This function is thread call back function for LogM cleanup thread
         *              after it is spawned
         *
         * @param       pvThis
                        Pointer to Service Loader
         *
         * @return      ACE Thread function return Value, zero returned by default
         */
        /*=================================================================== */
        static ACE_THR_FUNC_RETURN LOGMServiceThreadForSCX(void* pvThis);
	/*=================================================================== */
        /*!
         * @brief       LOGMServiceThreadForCMX
         *              This function is thread call back function for LogM cleanup thread
         *              after it is spawned
         *
         * @param       pvThis
                        Pointer to Service Loader
         *
         * @return      ACE Thread function return Value, zero returned by default
         */
        /*=================================================================== */
        static ACE_THR_FUNC_RETURN LOGMServiceThreadForCMX(void* pvThis);

	/*=================================================================== */
	/*
	 * @brief 	logmCoreMiddleWare
	 * 		Sets up a Logmaint thread and returns the control to Core Middleware
	 *
	 * @return  ACE_INT32
	 */
	/*=================================================================== */
	ACE_INT32 logmCoreMiddleWare(void);
	/*=================================================================== */
	/**
	 * @brief   Shuts down LOGM Service

	 * @return  int
	 */
	/*=================================================================== */
	int shutdown();
	/*=================================================================== */
	/**
	 * @brief   Shuts down LOGM HA Service
	 *
	 * @return  int
	 */
	/*=================================================================== */
	int shutdownHA();

	/*=================================================================== */
	/**
	 * @brief   Starts the threads which runs only on Active node when
	 * 			there is change of nodeState from Active to Passive
	 * @return  ACS_CC_ReturnType
	 */
	/*=================================================================== */

	//ACS_CC_ReturnType changeLogmFromPassiveToActive();
	/*=================================================================== */
	/**
	*  @brief	setupIMMCallbackThreads
	*
	*  @param	aThreadGroupId
	*
	*  @return 	ACS_CC_ReturnType
	*/
	/*=================================================================== */
//	ACS_CC_ReturnType setupIMMCallbackThreads( ACE_INT32 &aThreadGroupId );
	/*=================================================================== */

private:
	/*=================================================================== */
	/**
	 *  @brief 	run_i
	 *  		This function locks the pid file to ensure no other Logmaint process is running
			and calls setupLogmThread function that spawns a thread for Logmaint activity
		    	Main service running function
	 *
	 *  @return ACE_INT32
	 */
	/*=================================================================== */
	ACE_INT32 run_i(void);
	/*=================================================================== */
	/**
	 * @brief	Config file for LOGM Service

	 * @return  	string
	 */
	/*=================================================================== */
	std::string config_file_;
	/*=================================================================== */
	/**
	 * @brief   pidfile for storing Service Pid, this is used for debug purpose
	 *
	 * @return  string
	 */
	/*=================================================================== */
	std::string pid_file_;
	/*=================================================================== */
	/**
	 * @brief   Set the logging options in debug mode
	 *
	 * @return  void
	 */
	/*=================================================================== */
	void resetLog(void);
	/*=================================================================== */
	/**
	 * @brief   Get the lock on pid_file used for running only one instance of the service
	 *
	 * @return  bool
	 */
	/*=================================================================== */
	bool  getLock(const char *lock_file_path);
	/*=================================================================== */
	/**
	 * @brief   lockFd
	 *
	 * @return  ACE_INT32
	 */
	/*=================================================================== */
	ACE_INT32 lockFd(const ACE_HANDLE fd);
	/*=================================================================== */
	/**
	 * @brief   unlockFd
	 *
	 * @return  ACE_INT32
	 */
	/*=================================================================== */
	ACE_INT32 unlockFd(const ACE_INT32 fd);
	/*=================================================================== */
	/**
	 * @brief   Sets up Logmaint thread
	 *
	 * @return  ACE_INT32
	 */
	/*=================================================================== */
	ACE_INT32 setupLogmThread();

	/*=================================================================== */
	/**
	 * @brief	Takes decision to do cleanup activity
			in user interactive fashion or in
			the control of coremiddleware
	 *
	 * @return  	bool
	 */
	/*=================================================================== */
	void LOGMCleanupService();
 	/*=================================================================== */
        /**
         * @brief       Takes decision to do cleanup activity
                        in user interactive fashion or in
                        the control of coremiddleware
         *
         * @return     void 
         */
        /*=================================================================== */
        void LOGMCleanupServiceForSCX();
	/*=================================================================== */
        /**
         * @brief       Takes decision to do cleanup activity
                        in user interactive fashion or in
                        the control of coremiddleware
         *
         * @return     void 
         */
        /*=================================================================== */
        void LOGMCleanupServiceForCMX();

	/*=================================================================== */
	/**
	 * @brief   Registers signal handler for as call back for handling signals
	 *
	 * @return  bool
	 */
	/*=================================================================== */
	bool setupShutdownHandler();
	/*=================================================================== */
	/**
	 * @brief   becomeDaemon
	 *	    This function creates a child process if Logmaint runs in daemon.
		    This function also runs Logmaint as a process if its in debug (standalone) mode
	 *
	 * @return  Success or Failure of daemon
	 */
	/*=================================================================== */
	daemon_exit_t becomeDaemon();
	/*=================================================================== */
	/*!
	 * @brief 	Test PHA Stubs
	 *
	 * @return 	true Success
			false Failure
	 */
	/*=================================================================== */
	bool vTestPHAStubs();
	/*=================================================================== */
	/*!
	 * @brief 	bTestAPGCCEventReporting
	 *
	 * @return 	true Success
			false Failure
	 */
	/*=================================================================== */
	bool bTestAPGCCEventReporting();
	/*=================================================================== */
	/**
	 * @brief 	Sets Timeout for interactive mode
	 *
	 * @param 	u64TimeOut
	 *
	 * @return 	TimerId
	 */
	/*=================================================================== */
	ACE_INT32 setTimeoutForInteractiveMode( ACE_UINT64  u64TimeOut);
	/*=================================================================== */
	/**
	 * @brief 	s32ResetTimer
	 *
	 * @param 	s32Timer
	 *
	 * @return 	ACE_INT32
	 */
	/*=================================================================== */
	ACE_INT32 s32ResetTimer(ACE_INT32 s32Timer);
	/*=================================================================== */
	/**
	 * @brief 	lockFd_
			Descriptor to pid file
	 */
	/*=================================================================== */
	ACE_HANDLE lockFd_;
	/*=================================================================== */
	/**
	 * @brief 	m_poServiceHandler
	 */
	/*=================================================================== */
	static LOGM_Service_Loader *m_poServiceHandler;
	/*=================================================================== */
	/**
	 * @brief 	m_pAlarmHandler
	 */
	/*=================================================================== */
	ACS_LOGM_AlarmHandler * m_pAlarmHandler;
	/*=================================================================== */
	/**
	 * @brief 	m_bInteractive
	 */
	/*=================================================================== */
	bool m_bInteractive;
	/*=================================================================== */
	/**
	 * @brief 	m_s64ExecuteTimeout
	 */
	/*=================================================================== */
	ACE_INT64 m_s64ExecuteTimeout;
	/*=================================================================== */
	/**
	 * @brief 	m_plhEventHandles

	 */
	/*=================================================================== */
	ACE_Event *m_plhEventHandles[NO_OF_HANDLES];
	/*=================================================================== */
	/**
	 * @brief 	m_lpEndEvent

	 */
	/*=================================================================== */
	static ACE_Event *m_lpEndEvent;
	/*=================================================================== */
	/**
	 * @brief 	m_s32TimerId

	 */
	/*=================================================================== */
	ACE_INT32 m_s32TimerId;
	/*=================================================================== */
	
	// Commented as part of new LOGM model for removing OI	
	/*=================================================================== */
	//acs_logm_srvparam_oihandler *m_SrvParamOIHandler;
	/*=================================================================== */

	/*=================================================================== */
	//acs_logm_logparam_oihandler *m_LogParamOIHandler;
	/*=================================================================== */


	//! Only defined for CUTE
	#ifdef ACS_LOGM_CUTE_TEST
		friend class Cute_LOGM_Service;
	#endif

};

#endif /* _ACS_LOGM_SVC_LOADER_H_ */
