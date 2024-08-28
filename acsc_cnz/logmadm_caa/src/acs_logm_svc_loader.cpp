/*=================================================================== */
   /**
   @file acs_logm_svc_loader.cpp

   Class method implementation for LOGM module.

   This module contains the implementation of class declared in
   the acs_logm_svc_loader.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/10/2010     XKUSATI  Initial Release
   N/A       17/01/2013     XCSRPAD  Deletion of unwanted CMX log files	
==================================================================== */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/Reactor.h>
#include <ACS_TRA_Logging.h>
#include <acs_logm_svc_loader.h>
#include <acs_logm_logmaint.h>
#include <acs_prc_api.h>
#include <acs_logm_aeh.h>
#include <acs_logm_common.h>


/*=====================================================================
                        VARIABLE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief m_poServiceHandler
 */
/*=================================================================== */
LOGM_Service_Loader 	* LOGM_Service_Loader::m_poServiceHandler = 0;
/*=================================================================== */
/**
   @brief m_lpEndEvent
 */
/*=================================================================== */
ACE_Event 	* LOGM_Service_Loader::m_lpEndEvent = 0;
/*=================================================================== */
/**
   @brief ACS_LOGM_InformationTrace
 */
/*=================================================================== */
ACS_TRA_trace 	* ACS_LOGM_Common::ACS_LOGM_InformationTrace=0;
/*=================================================================== */
/**
   @brief ACS_LOGM_WarningTrace
 */
/*=================================================================== */
ACS_TRA_trace  * ACS_LOGM_Common::ACS_LOGM_WarningTrace=0;
/*=================================================================== */
/**
   @brief ACS_LOGM_ErrorTrace
 */
/*=================================================================== */
ACS_TRA_trace 	* ACS_LOGM_Common::ACS_LOGM_ErrorTrace=0;
/*=================================================================== */
/**
   @brief ACS_LOGM_Logging
 */
/*=================================================================== */
ACS_TRA_Logging 	* ACS_LOGM_Common::ACS_LOGM_Logging=0;
ACE_INT32 s32GrpId = -1;

/*===================================================================
   ROUTINE: becomeDaemon
=================================================================== */
daemon_exit_t LOGM_Service_Loader::becomeDaemon()
{
	pid_t pid = -1;

	ACE_TRACE("become__daemon\n");

	/* reset the file mode mask */
	ACE_OS::umask(0);
	if (m_bInteractive){
		DEBUG("%s","%IRunning LOGM in Stand alone Mode \n\n");
		goto standalone;
	}
	else{
		DEBUG("%s","%IRunning as Daemon \n");
	}

	/*!  fork() so the parent can exit, this returns control to the command line or shell invoking your program.
	 * This step is required so that the new process is guaranteed not to be a process group leader.
	 *  The next step, setsid(), fails if you're a process group leader.
	 */
	pid = ACE_OS::fork();

	switch (pid) {

		case -1 :
		{
			ACE_TCHAR szData[256];
			snprintf(szData,sizeof(szData)/sizeof(szData) - 1,
			ACE_TEXT("\n Unable to Fork Process  for Creating Daemon : %m"));

			// Report error
			(void)ACS_LOGM_AEH::ReportEvent(1003,
                                   ACE_TEXT("EVENT"),
                                   CAUSE_AP_INTERNAL_FAULT,
                                   ACE_TEXT(""),
                                   szData,
                                   LOGM_AEH_TEXT_INT_SUPERVISOR_ERROR);
			return EXIT_ERROR;
		}
	   case 0 :

		   /*!
		    *  setsid() to become a process group and session group leader. Since a controlling terminal is associated
		    *  with a session, and this new session has not yet acquired a controlling terminal our process now has no
		    *  controlling terminal.
		    */

			if ((ACE_OS::setsid()) < 0)
				return EXIT_ERROR;

			/*! Ensure future opens won't allocate controlling TTYs.
			 *  SIGHUP is received when controlling terminal is closed.
			 */

			ACE_OS::signal (SIGHUP, SIG_IGN);

			/*!
			 * 	fork() again so the parent, (the session group leader), can exit. This means that we, as a non-session
			 *  group leader, can never regain a controlling terminal.
			 */

			pid = ACE_OS::fork();
			DEBUG("LOGM child process created having process ID = %d...\n\n",pid);

			switch (pid) {

				case -1 :
					return EXIT_ERROR;
				case 0 :
					break;
				default :
					return EXIT_OK;
				}

			/*!
			 * (0 == pid) we are the final child
			 */

			/*! sleep for 1 second to give the parent plenty of time to exit */
			ACE_OS::sleep(1);

			break;

		default :
			/*! wait for intermediate child */
			ACE_OS::waitpid(pid, (ACE_exitcode *) 0, 0);
			return EXIT_OK;

	}

	standalone:

		/*!
		 *   chdir("/") to ensure that our process doesn't keep any directory in use only for interactive mode
		 */

		if (!m_bInteractive && // change the working directory
				ACE_OS::chdir("/") < 0)
				return EXIT_ERROR;

		ACE_INT32 fd0 = ACE_OS::open("/dev/null", O_RDWR);

		if ( fd0 == -1 )
			return EXIT_ERROR;

		if (!m_bInteractive) {

			/*! attach file descriptors STDIN_FILENO(0),
			 *  STDOUT_FILENO(1) and STDERR_FILENO(2) to
			 *  /dev/null
			 */
			ACE_OS::dup2(fd0, STDIN_FILENO);
			ACE_OS::dup2(fd0, STDOUT_FILENO);
			ACE_OS::dup2(fd0, STDERR_FILENO);

		}

	DEBUG("%s","%IStarting LOGM process...  \n");

	return EXIT_DAEMON;
}//End of becomeDaemon


/*===================================================================
   ROUTINE: LOGM_Service_Loader
=================================================================== */
LOGM_Service_Loader::LOGM_Service_Loader()
	 :config_file_(""),
	 pid_file_( LOGM_PID_FILE ),
	 lockFd_(0),
	 m_bInteractive(false),
	 m_s64ExecuteTimeout(INFINITE)
{
	m_pAlarmHandler = 0;
//	m_SrvParamOIHandler = 0;
//	m_LogParamOIHandler = 0;
	m_plhEventHandles[0] = (ACE_Event*)NULL;
	m_s32TimerId = 0;
}//End of Constructor


/*===================================================================
   ROUTINE: initService
=================================================================== */
void LOGM_Service_Loader::initService()
{
	m_poServiceHandler = this;

}//End of initService

/*===================================================================
   ROUTINE: ~LOGM_Service_Loader
=================================================================== */
LOGM_Service_Loader::~LOGM_Service_Loader(void)
{

#if 0

	if( m_SrvParamOIHandler != 0 )
        {
                delete m_SrvParamOIHandler;
                m_SrvParamOIHandler = 0 ;
        }

        if( m_LogParamOIHandler != 0 )
        {
                delete m_LogParamOIHandler;
                m_LogParamOIHandler = 0;
        }
#endif

	m_poServiceHandler = 0;

	if( m_plhEventHandles[0] != (ACE_Event*)NULL )
	{
	   delete m_plhEventHandles[0];
	   m_plhEventHandles[0] = (ACE_Event*)NULL;
	}
	
}//End of Destructor

/*===================================================================
   ROUTINE: printUsageAndDie
=================================================================== */
void LOGM_Service_Loader::printUsageAndDie(const ACE_TCHAR *prog)
{

	DEBUG( "Usage: LOGM_start_script",
			   " -V -d -f <file> ",
			   " -V: Print the version\n",
			   " -d: <time in seconds> No fork - Run as a regular application for particular",
			   "  time for Debug purpose and Use current directory as working directory\n",
			   " -f: <file> Configuration file, default is \"/etc/logmservice.conf \"\n",
			   prog,
			   0
			   );

	ACE_OS::exit(1);
}//End of printUsageAndDie

/*===================================================================
   ROUTINE: parseArgs
=================================================================== */
ACE_INT32 LOGM_Service_Loader::parseArgs( ACE_UINT32 argc, ACE_TCHAR *argv[] )
{
    if (!argc)
	    return 0;

    //! Write to both console and log file
    ACE_LOG_MSG->set_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);

    ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("Vd:f:"));
    ACE_INT32 c;

    while ((c = get_opt()) != -1) {

	switch (c) {

	    case 'd':
			m_bInteractive = true;
			//! Convert String to unsigned 64 Bit integer
			m_s64ExecuteTimeout = ACE_OS::strtoull(get_opt.opt_arg(),'\0',10);
			DEBUG("LOGM Service is in interactive mode for %d Seconds\n", m_s64ExecuteTimeout);
			break;

	    default:
			printUsageAndDie(argv[0]);
			break;
	    }
    }

    //! Stop writing to Terminal to Output stream
    ACE_LOG_MSG->clr_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);

    return (ACE_INT32)0;
}//End of parseArgs


/*===================================================================
   ROUTINE: run_i
=================================================================== */
ACE_INT32 LOGM_Service_Loader::run_i()
{

	DEBUG(" %s ","In LOGM_Service_Loader function::run_i()....");
	bool testResult;

	if (!this->getLock(this->pid_file_.c_str())) {
		DEBUG("Could not lock pidfile : %s. \nPlease ensure that no other process with name ACS_ACSC_Logmaint is running.\n ",pid_file_.c_str());
		return EXIT_ERROR;
	}
	else
	{
		DEBUG("%s","LOGM Service Process is running Successfully :)  \n");
	}

	//! Register to receive shutdown signals
	testResult = setupShutdownHandler();

	if(testResult)
	{
		DEBUG("%s","Result of shutdown handler :: SUCCESS\n");
	}
	else
	{
		DEBUG("%s","Result of shutdown handler :: FAILED\n");
	}

	//! if m_u64ExecuteTimeout=0 Service runs in interactive mode until terminated
	if( m_bInteractive && ( m_s64ExecuteTimeout != 0 ) )
	{
		m_s32TimerId = setTimeoutForInteractiveMode( m_s64ExecuteTimeout );
	}
	//*************************************************************************************
	// Starting Logmaint thread
	//*************************************************************************************
	ACE_INT32 s32GrpIdAll = setupLogmThread();

	if( s32GrpIdAll != -1 )
	{
	   //! Start reactor event loop

	   ACE_Reactor::instance()->run_reactor_event_loop();
	   
	   DEBUG("%s","Finished reactor event loop, Cancelling Threads.... ");

	   ACE_Thread_Manager::instance()->cancel_grp(s32GrpIdAll);
	   ACE_Thread_Manager::instance()->wait_grp(s32GrpIdAll);
	}
	else
	{
	    s32ResetTimer(m_s32TimerId);
	    ERROR("%s","\n Failed setting Up threads. Exit... \n ");
	}
	


	//! release the lock
	if ( this->lockFd_ != -1 )
	{
	    unlockFd(this->lockFd_);
	    close(this->lockFd_);
	    this->lockFd_ = -1;
	}

	ACE_OS::unlink (this->pid_file_.c_str());
	DEBUG("%s","Lock Released and Exiting LOGM Service");

	return EXIT_SUCCESS;

}//End of run_i


/*===================================================================
   ROUTINE: logmCoreMiddleWare
=================================================================== */
ACE_INT32 LOGM_Service_Loader::logmCoreMiddleWare(void)
{
	DEBUG("%s","Entering logmCoreMiddleWare");

	//*************************************************************************************
	//Starting Logmaint thread
	//*************************************************************************************
	ACE_INT32 s32GrpId = setupLogmThread();

	if( s32GrpId != -1 )
	{
		DEBUG("%s", "LOGM Threads Spawned successfully.");
		#if 0
		//! Start reactor event loop
		//DEBUG("%s","Entering run_reactor_event_loop");
		//ACE_Reactor::instance()->run_reactor_event_loop();

		//DEBUG("%s","Finished reactor event loop, Cancelling Threads.... ");

		//ACE_Thread_Manager::instance()->cancel_grp(s32GrpId);
		//ACE_Thread_Manager::instance()->wait_grp(s32GrpId);
		#endif
	}
	else
	{
		ERROR("%s","\n Failed setting Up threads. Exit... \n ");
	}

	DEBUG("%s","Exiting LOGM Service");
	return s32GrpId;
}//End of logmCoreMiddleWare


/*===================================================================
   ROUTINE: runService
=================================================================== */
ACE_INT32 LOGM_Service_Loader::runService(void)
{
	daemon_exit_t dstat = becomeDaemon();

	switch (dstat) {
		case EXIT_DAEMON :
			break;
		case EXIT_OK :
			exit(EXIT_SUCCESS);
		case EXIT_ERROR :
		default :
			exit(EXIT_FAILURE);
	}

	return this->run_i();
}//End of runService

/*===================================================================
   ROUTINE: execute
=================================================================== */
ACE_INT32 LOGM_Service_Loader::execute (void)
{
	return this->runService();
}//End of execute

/*===================================================================
   ROUTINE: resetLog
=================================================================== */
void LOGM_Service_Loader::resetLog(void)
{
	//!Stop writing log to SYSLOG
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::SYSLOG);
	ofstream *output_file = new ofstream(ACE_TEXT("ACS_LOGM_Service.log"), ios::out);

	if (output_file && (output_file->rdstate() == ios::goodbit))
		ACE_LOG_MSG->msg_ostream(output_file, 1);

	//! Write to both console and log file mentioned in ofstream
	ACE_LOG_MSG->set_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);

}//End of resetLog


/*===================================================================
   ROUTINE: executeDebug
=================================================================== */

void LOGM_Service_Loader::executeDebug(int argc,ACE_TCHAR *argv[])
{
	ACE_INT32 result = 0;
	LOGM_Service_Loader oLOGMService;

	//! Parse arguments
	result = oLOGMService.parseArgs(argc,argv);

	if (result < 0)
		exit(EXIT_FAILURE);  // Error in parsing arguments
	else if (result > 0)
		exit(EXIT_SUCCESS);

	//! Initialize self pointer used later for terminate
	oLOGMService.initService();

	DEBUG("\n(%T) LOGM_Service Version %s initializing...\n\n", LOGM_SERVICE_VER);
	result = oLOGMService.execute();

}//End of executeDebug


/*===================================================================
   ROUTINE: getLock
=================================================================== */
bool LOGM_Service_Loader::getLock(const char *lock_file_path)
{

	DEBUG("%s %s","Locking the pid file to ensure no other ACS_ACSC_Logmaint process runs..",lock_file_path);

	// get the lock and write pid
	if (getProcessLock(this->lockFd_, pid_file_.c_str()))
		return true;

	if (-1 != this->lockFd_) {
		close(this->lockFd_);
		this->lockFd_ = -1;
	}

    return false;
}//End of getLock


/*
 * Will attempt to lock the PID file and write the
 * pid into it. This function will return false for
 * all errors. Callee is responsible for closing *fd
 * if (*fd != -1).
 */
/*===================================================================
   ROUTINE: getProcessLock
=================================================================== */
bool LOGM_Service_Loader::getProcessLock(ACE_HANDLE  & fd, const char *path)
{
	std::string pid_file;

	char buf[32] = { '\0' };
	ACE_INT32  p = 0;
	ssize_t w = 0;

	fd = -1;
	if (!path || !strlen(path))
		return false;

	fd = ACE_OS::open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (fd == -1)
		return false;

	// lock it
	if (lockFd(fd))
		return false;

	// we have the lock
	if (ftruncate(fd, 0))
		return false;

	/* write pid */
	p = snprintf(buf, sizeof(buf), "%d", ACE_OS::getpid());
	w = ACE_OS::write(fd, buf, strlen(buf) + sizeof(char));
	p += sizeof(char);

	if (p != (ACE_INT32)w)
		return false;

	if (ACE_OS::fsync(fd))
		return false;

	DEBUG("%s"," getProcessLock :: successful ");

	return true;
}//End of getProcessLock

/*===================================================================
   ROUTINE: lockFd
=================================================================== */
ACE_INT32 LOGM_Service_Loader::lockFd(const ACE_HANDLE fd)
{
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	if ( fd == -1 )
		return -1;

	return ACE_OS::fcntl(fd, F_SETLK, reinterpret_cast<long>(&lock));
}//End of lockFd

/*===================================================================
   ROUTINE: unlockFd
=================================================================== */
ACE_INT32 LOGM_Service_Loader::unlockFd(const ACE_INT32 fd)
{
	struct flock lock;

	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	if (-1 == fd)
		return -1;

	return ACE_OS::fcntl(fd, F_SETLK, reinterpret_cast<long>(&lock));
}//End of unlockFd

/*===================================================================
   ROUTINE: setupShutdownHandler
=================================================================== */
bool LOGM_Service_Loader::setupShutdownHandler()
{
	DEBUG("%s","Setting up Shutdown handler...");

	ACE_Sig_Set signalSet;

	//! Register SIGINT
	if( m_bInteractive )
		signalSet.sig_add(SIGINT);

	signalSet.sig_add( SIGTSTP );
	signalSet.sig_add( SIGTERM );

	//! Register the signal_handler method to process all the signals in sigset
	ACE_Reactor::instance()->register_handler( signalSet, this );

	return true;
}//End of setupShutdownHandler

/*===================================================================
   ROUTINE: shutdown
=================================================================== */
int LOGM_Service_Loader::shutdown()
{
   DEBUG("%s","Entering Shutdown trigger for LOGM Service...");
   m_plhEventHandles[0]->pulse();
   ACE_Reactor::instance()->end_reactor_event_loop();

#if 0
   ACS_PRC_API prcObj;
   int nodeState = prcObj.askForNodeState();
   if( nodeState == 1 )//Active node
   {
	if( m_SrvParamOIHandler != 0 )
	{
		m_SrvParamOIHandler->shutdown();
	}
	if( m_LogParamOIHandler != 0 )
	{
		m_LogParamOIHandler->shutdown();
	}
   }
#endif
   //ThrExitHandler::cleanup();
   DEBUG("%s","Leaving Shutdown trigger for LOGM Service...");
   return 0;
}//End of shutdown

/*===================================================================
   ROUTINE: shutdownHA
=================================================================== */
int LOGM_Service_Loader::shutdownHA()
{
   DEBUG("%s","Shutdown trigger for LOGM Service...");
   m_plhEventHandles[0]->pulse();

#if 0
   ACS_PRC_API prcObj;
   int nodeState = prcObj.askForNodeState();
   if( nodeState == 1 )//Active node
   {
        if( m_SrvParamOIHandler != 0 )
        {
                m_SrvParamOIHandler->shutdown();
        }
        if( m_LogParamOIHandler != 0 )
        {
               m_LogParamOIHandler->shutdown();
        }
   }

#endif
   //ThrExitHandler::cleanup();
   return 0;
}//End of shutdownHA

/*===================================================================
   ROUTINE: handle_timeout
=================================================================== */
int LOGM_Service_Loader::handle_timeout(const ACE_Time_Value &current_time, const void *)
{
   (void) current_time;
   if( m_bInteractive )
   {
	   DEBUG("%s","Handle Time out for Interactive mode received for LOGM PID: %p",ACE_OS::getpid());
	   ACE_OS::kill(ACE_OS::getpid(), SIGTERM);
   }

   return 0;
}//End of handle_timeout


/*===================================================================
   ROUTINE: handle_signal
=================================================================== */
int LOGM_Service_Loader::handle_signal (int signum, siginfo_t * siginfo, ucontext_t * ucontext)
{
	(void) siginfo;
	(void) ucontext;

	DEBUG("%s","Entering handle_signal()");
	DEBUG("Received Signal %d",signum);

	if( !m_poServiceHandler )
	{
		ERROR("%s "," m_poServiceHandler is NULL ");
		return 0;
	}

	#if 0
	if(signum == SIGINT && ( m_bInteractive && ( m_s64ExecuteTimeout != 0 )))
	{
		DEBUG("Calling s32ResetTimer() with m_s32TimerId = %d",m_s32TimerId);
		s32ResetTimer(m_s32TimerId);
	}
	#endif

	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
	   s32ResetTimer(m_s32TimerId);
	   m_poServiceHandler->shutdown();
	}

	return 0;
}//End of handle_signal


/*===================================================================
   ROUTINE: setTimeoutForInteractiveMode
=================================================================== */
ACE_INT32 LOGM_Service_Loader::setTimeoutForInteractiveMode(ACE_UINT64  u64TimeOut)
{
	DEBUG("Set TimeOut For Interactive Mode : %d seconds",u64TimeOut);

	ACE_Time_Value exptime(u64TimeOut);

	ACE_INT32 s32TimerId = ACE_Reactor::instance()->schedule_timer( this ,
						     0,
						     exptime
					       );

	DEBUG("setTimeoutForInteractiveMode: Timer created :: %s",s32TimerId==-1?"FAILED\n":"SUCCESS\n");
	return s32TimerId;
}//End of setTimeoutForInteractiveMode


/*===================================================================
   ROUTINE: s32ResetTimer
=================================================================== */
ACE_INT32 LOGM_Service_Loader::s32ResetTimer(ACE_INT32 s32Timer)
{
    ACE_ASSERT( s32Timer!=-1 );
    ACE_INT32 s32Result = ACE_Reactor::instance()->cancel_timer(s32Timer);

    DEBUG("\nLOGM_Service_Loader::bResetTimer :: %s",s32Result==1?"SUCCESS\n":"FAILED\n");

    return s32Result;
}//End of s32ResetTimer

/*===================================================================
   ROUTINE: setupLogmThread
=================================================================== */
ACE_INT32 LOGM_Service_Loader::setupLogmThread()
{
    if( ACS_LOGM_Common::fetchDnOfRootObjFromIMM() == -1 )
    {
	    ERROR("%s", "Error occured while fetching the DN of the root object");
	    return s32GrpId;	
    }
	
    m_lpEndEvent = m_plhEventHandles[0] = new ACE_Event(
						     true,
						     FALSE,
	                                             USYNC_THREAD,
	                                             "EV_ACS_LOGM_TERMINATE_EVENT"
				                       );

    if(m_lpEndEvent == 0)
    {
	    ERROR("%s", "Error occured while creating the stop event");
	    return s32GrpId;
    }

    DEBUG("%s","Setting up Logm Threads....\n");
    ACS_PRC_API prcObj;
    ACE_INT32 s32SCXThrdId = -1;
    ACE_INT32 s32CMXThrdId = -1;
    int nodeState = prcObj.askForNodeState();
// Is commented as the IMM callback thread is to be now removed from existing code so that there won;t be provision for user to give the input.

#if 0

    if( nodeState == 1 )//Active node
    {
    	if(setupIMMCallbackThreads(s32GrpId) == ACS_CC_FAILURE)
    	{
	    ERROR("%s", "Error occured while starting IMM OI threads");
	    return s32GrpId;
    	}

    }
	else if( nodeState == 2 )
	{
		DEBUG("%s", "Node State is Passive");
		DEBUG("%s", "IMM Callback Threads Not Spawned on the passive node");
	}
	else
	{
		ERROR("%s", "Node State is Undefined!!");
		return ACS_CC_FAILURE;
	}
#endif     

   	//! Store the Groups Id which is later used to terminate all threads spawned
    ACE_INT32 s32LogMThrdId = ACS_LOGM_Common::StartThread(LOGMServiceThread, (void*)this, ACE_TEXT("LOGMServiceThread"), s32GrpId );


       	if(nodeState == 2)
       	{
       		s32GrpId = s32LogMThrdId;
       	}
    //SCX Thread in Active
    //*************************************************************************************
    // Starting Logmaint thread for SCX
    //*************************************************************************************
//    if (nodeState == 1)
 //   {
    	DEBUG("%s", "Starting LOGM Clean Up Thread for SCX !!");
    	s32SCXThrdId = ACS_LOGM_Common::StartThread(LOGMServiceThreadForSCX, (void*)this, ACE_TEXT("LOGMServiceThreadForSCX"), s32GrpId);
  //  }
	if( s32LogMThrdId != -1 && s32SCXThrdId != -1)
	{
		DEBUG("%s", "LOGM Clean Up Thread successfully spawned!!");
		s32GrpId = s32SCXThrdId;
	}
	else
	{
		ERROR("%s", "Error occured while spawning LOGM Clean up Thread");
	}
//CMX Thread in Active
    //*************************************************************************************
    // Starting Logmaint thread for CMX
    //*************************************************************************************
//    if (nodeState == 1)
 //   {
    	DEBUG("%s", "Starting LOGM Clean Up Thread for CMX !!");
    	s32CMXThrdId = ACS_LOGM_Common::StartThread(LOGMServiceThreadForCMX, (void*)this, ACE_TEXT("LOGMServiceThreadForCMX"), s32GrpId);
  //  }
	if( s32LogMThrdId != -1 && s32CMXThrdId != -1)
	{
		DEBUG("%s", "LOGM Clean Up Thread successfully spawned!!");
		s32GrpId = s32CMXThrdId;
	}
	else
	{
		ERROR("%s", "Error occured while spawning LOGM Clean up Thread");
	}
    return s32GrpId;

}//End of setupLogmThread

/*===================================================================
   Is commented as the IMM callback thread is to be now removed from existing code so that there won;t be provision for user to give the input.
   ROUTINE: setupIMMCallbackThreads
=================================================================== */
#if 0
ACS_CC_ReturnType LOGM_Service_Loader::setupIMMCallbackThreads( ACE_INT32& aThreadGroupId )
{
	DEBUG("%s", "Entering  setupIMMCallbackThreads");

		std::string myLOGMObjName("");
		myLOGMObjName.append(ACS_LOGM_Common::parentObjDNofLOGM);

		std::string myLOGMSrvParamImplName(LOGM_SERVICE_PARAM_IMPL_NAME);

		m_SrvParamOIHandler = new acs_logm_srvparam_oihandler(  myLOGMObjName.c_str(),
									myLOGMSrvParamImplName.c_str(),
									ACS_APGCC_ONE );

		if( m_SrvParamOIHandler == 0 )
		{
			ERROR("%s", "Error occured while allocating memory for m_SrvParamOIHandler.");
                        ERROR("%s", "Leaving setupIMMCallbackThreads");
                        return ACS_CC_FAILURE;
		}

		//Start the OI thread of handling Service Parameters.
		DEBUG("%s", "Spawning Service Param OI Thread");
		if( m_SrvParamOIHandler->setupSrvParamOIThread( m_SrvParamOIHandler, aThreadGroupId )
			==      ACS_CC_FAILURE )
		{
			ERROR("%s", "Error occured while starting Service Param OI Thread.");
			ERROR("%s", "Leaving setupIMMCallbackThreads");
			return ACS_CC_FAILURE;
		}
		DEBUG("%s", "Service Param OI Thread successfully spawned.");
		

		std::string myLogParamClassName(LOGM_LOG_PARAM_CLASS_NAME);
		std::string myLogParamImplName(LOGM_LOG_PARAM_IMPL_NAME);
		
		m_LogParamOIHandler = new acs_logm_logparam_oihandler ( myLogParamClassName.c_str(),
									myLogParamImplName.c_str(),
									ACS_APGCC_ONE );
		if( m_LogParamOIHandler == 0 )
		{
			ERROR("%s", "Error occured while allocating memory for m_LogParamOIHandler.");
                        ERROR("%s", "Leaving setupIMMCallbackThreads");
                        return ACS_CC_FAILURE;
                }

		//Start the thread for log parameters.
		DEBUG("%s", "Spawning Log Param OI Thread");

		if( m_LogParamOIHandler->setupLogParamOIThread( m_LogParamOIHandler, aThreadGroupId )
				==      ACS_CC_FAILURE )
		{
			ERROR("%s", "Error occured while starting LOGM LogParam OI Thread");
			ERROR("%s", "Leaving setupIMMCallbackThreads");
			return ACS_CC_FAILURE;;
		}

		DEBUG("%s", "Log Param OI Thread successfully spawned.");
	DEBUG("%s", "Leaving setupIMMCallbackThreads");
	return ACS_CC_SUCCESS;
}
#endif 
/*===================================================================
   Is commented won't be any state change in noRed.
   ROUTINE: changeLogmFromPassiveToActive
=================================================================== */
#if 0
ACS_CC_ReturnType LOGM_Service_Loader::changeLogmFromPassiveToActive()
{
	//bool s32Result = ACS_CC_FAILURE;
	DEBUG("%s","Entering changeLogmFromPassiveToActive");
	//s32Result = setupIMMCallbackThreads(s32GrpId);

	//  if(  s32Result != ACS_CC_FAILURE )
	 // {
		  // scx thread spawn
		 DEBUG("%s", "Starting LOGM Clean Up Thread for SCX !!");
		 ACS_LOGM_Common::StartThread(LOGMServiceThreadForSCX, (void*)this, ACE_TEXT("LOGMServiceThreadForSCX"), s32GrpId);
	DEBUG("%s", "Starting LOGM Clean Up Thread for CMX !!");
		 ACS_LOGM_Common::StartThread(LOGMServiceThreadForCMX, (void*)this, ACE_TEXT("LOGMServiceThreadForCMX"), s32GrpId);
	//  }
	//  else
	//  {
	//	  ERROR("%s", "Error occured while starting IMM OI threads");
	//	  return ACS_CC_FAILURE;
	 // }

	DEBUG("%s","Exiting changeLogmFromPassiveToActive");
	return ACS_CC_SUCCESS;
}
#endif

/*===================================================================
   ROUTINE: LOGMServiceThread
=================================================================== */
ACE_THR_FUNC_RETURN LOGM_Service_Loader::LOGMServiceThread(void* pvThis)
{
    ((LOGM_Service_Loader*)pvThis)->LOGMCleanupService();
    return 0;
}//End of LOGMServiceThread

/*===================================================================
   ROUTINE: LOGMServiceThreadForSCX
=================================================================== */
ACE_THR_FUNC_RETURN LOGM_Service_Loader::LOGMServiceThreadForSCX(void* pvThis)
{
    ((LOGM_Service_Loader*)pvThis)->LOGMCleanupServiceForSCX();
    return 0;
}//End of LOGMServiceThreadForSCX
/*===================================================================
   ROUTINE: LOGMServiceThreadForCMX
=================================================================== */
ACE_THR_FUNC_RETURN LOGM_Service_Loader::LOGMServiceThreadForCMX(void* pvThis)
{
    ((LOGM_Service_Loader*)pvThis)->LOGMCleanupServiceForCMX();
    return 0;
}//End of LOGMServiceThreadForCMX
/*===================================================================
   ROUTINE: LOGMCleanupService
=================================================================== */
void LOGM_Service_Loader::LOGMCleanupService()
{
    DEBUG("%s","******** Spawning LOGMCleanupService Thread *********");
    ACS_ACSC_Logmaint LOGM_Cleanup(m_plhEventHandles[0]);
    if(m_bInteractive)
    {
	   LOGM_Cleanup.cleanLogFilesInteractive();
    }
    else
    {
	    LOGM_Cleanup.cleanLogFilesCoreMiddleWare();

    }
}//End of LOGMCleanupService

/*===================================================================
   ROUTINE: LOGMCleanupServiceForSCX
=================================================================== */
void LOGM_Service_Loader::LOGMCleanupServiceForSCX()
{
    DEBUG("%s","******** Spawning LOGMCleanupService Thread For SCX *********");
    ACS_ACSC_Logmaint LOGM_Cleanup(m_plhEventHandles[0]);
    if(m_bInteractive)
    {
           LOGM_Cleanup.cleanSCXLogFilesInteractive();
    }
    else
    {
            LOGM_Cleanup.cleanSCXLogFilesCoreMiddleWare();

    }
}//End of LOGMCleanupServiceForSCX
/*===================================================================
   ROUTINE: LOGMCleanupServiceForCMX
=================================================================== */
void LOGM_Service_Loader::LOGMCleanupServiceForCMX()
{
    DEBUG("%s","******** Spawning LOGMCleanupService Thread For CMX *********");
    ACS_ACSC_Logmaint LOGM_Cleanup(m_plhEventHandles[0]);
    if(m_bInteractive)
    {
           LOGM_Cleanup.cleanCMXLogFilesInteractive();
    }
    else
    {
            LOGM_Cleanup.cleanCMXLogFilesCoreMiddleWare();

    }
}//End of LOGMCleanupServiceForCMX

void LOGM_Service_Loader::stop()
{
        DEBUG("%s","Entering SSU_Service_Loader::stop()");
        // start shutdown activities.
        shutdown();
        DEBUG("%s","Exiting SSU_Service_Loader::stop()");
}


