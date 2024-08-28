/*=================================================================== */
/**
   @file   acs_ssu_svc_loader.cpp

   @brief cpp file for acs_ssu_svc_loader.cpp

          This module responsible for spawning various functional threads required for SSU functionality.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/06/2010     XSIRKUM        APG43 on Linux.

==================================================================== */
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/ACE.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <sys/types.h>
#include <sys/stat.h>
//! External Interfaces used by SSU
#include <ACS_APGCC_Path.H>
#include <ACS_TRA_Logging.h>
//! Internal Interfaces to SSU
#include "acs_ssu_svc_loader.h"
#include "acs_ssu_types.h"
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_alarm_handler.h"
#include "acs_ssu_folder_quota_mon.h"
#if 0
#include "acs_ssu_command_handler.h"
#endif
#include "acs_ssu_perf_mon_objectimpl.h"
#include "acs_ssu_disk_mon_objectimpl.h"

SSU_Service_Loader * SSU_Service_Loader::m_poServiceHandler = 0;
bool SSU_Service_Loader::m_bInteractive=FALSE;
ACE_Event *SSU_Service_Loader::m_lpEndEvent = 0;

ACE_INT32 s32GrpIdForAll = -1;
ACE_thread_t diskMonThreadId = -1;
ACE_thread_t perfMonThreadId = -1;
ACE_thread_t folderMonThreadId = -1;
ACE_thread_t ssuOIThreadId = -1;

enum SERVICE_STATE
{
	SSU_IN_DEBUG,
	SSU_IN_ACTIVE,
	SSU_IN_PASSIVE
};
SERVICE_STATE ssuServiceStatus;

ACE_Sig_Set signalSet;

daemon_exit_t SSU_Service_Loader::becomeDaemon()
{
	pid_t pid = -1;

	/* reset the file mode mask */
	ACE_OS::umask(0);

	if (m_bInteractive){
		//Running in Stand alone Mode
		goto standalone;
	}
	else{
		//Running as Daemon
	}

	/*!  fork() so the parent can exit, this returns control to the command line or shell invoking your program.
	 * This step is required so that the new process is guaranteed not to be a process group leader.
	 *  The next step, setsid(), fails if you're a process group leader.
	 */
	pid = ACE_OS::fork();


	switch (pid) {
	case -1 :
    {
       // Report error
       ACE_TCHAR szData[256];

       snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
              ACE_TEXT("\n Unable to Fork Process  for Creating Daemon : %m"));

       (void)ACS_SSU_AEH::ReportEvent(1003,
                                   ACE_TEXT("EVENT"),
                                   CAUSE_AP_INTERNAL_FAULT,
                                   ACE_TEXT(""),
                                   szData,
                                   SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);
       ERROR("%s \n",szData);

		return EXIT_ERROR;
    }
	/*!@todo Raise event(1003) for internal error while fork */
	case 0 :
	{

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

		switch (pid) {
		case -1 :
			return EXIT_ERROR;
		/*!@todo Raise event(1003) for internal error while fork */
		case 0 :
			break;
		default :
			return EXIT_OK;
		}

		/*!
		 * (0 == pid) we are the final child
		 */

		/*! sleep for 1 second to give the parent plenty of
		   time to exit */
		ACE_OS::sleep(1);
	}

	   break;
	default :
	   {
		/*! wait for intermediate child */
		ACE_OS::waitpid(pid, (ACE_exitcode *) 0, 0);

		return EXIT_OK;
   	   }
	}

	//! In standalone Service runs in the foreground and is used for debugging
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

	//! @todo Raise event for failure in opening the file

	if (!m_bInteractive) {

		/*! attach file descriptors STDIN_FILENO(0),
		 *  STDOUT_FILENO(1) and STDERR_FILENO(2) to
		 *  /dev/null
		 */
		ACE_OS::dup2(fd0, STDIN_FILENO);
		ACE_OS::dup2(fd0, STDOUT_FILENO);
		ACE_OS::dup2(fd0, STDERR_FILENO);
	}

	//SSU Service Process is running Successfully :)

	return EXIT_DAEMON;
}


SSU_Service_Loader::SSU_Service_Loader()
:config_file_(""),
 pid_file_( SSU_PID_FILE ),
 lockFd_(0),
 m_pAlarmHandler(0),
 m_poReactor( 0 ),
 m_s64ExecuteTimeout(INFINITE),
 thetpReactorImpl(0),
 m_s32TimerId(0),
 m_pPerfMonitor(0),
 m_pDiskMonitor(0),
 m_pFolderQuotaMon(0),
 m_haObj(0)
{
	m_plhEventHandles[0] = (ACE_Event*)NULL;  // Disk Monitor
	m_plhEventHandles[1] = (ACE_Event*)NULL;  // Perf Monitor
	m_plhEventHandles[2] = (ACE_Event*)NULL;  // Folder Monitor

	thetpReactorImpl = new ACE_TP_Reactor();
	m_poReactor = new ACE_Reactor(thetpReactorImpl);
	m_poReactor->open(1);
	omHandlerPtr = new OmHandler ();
}

void SSU_Service_Loader::initService()
{
	m_poServiceHandler = this;
}

SSU_Service_Loader::~SSU_Service_Loader(void)
{
	DEBUG("%s","Entering SSU_Service_Loader::~SSU_Service_Loader");

	m_poServiceHandler  = 0;

	DEBUG("%s","Deleting Disk pointer");
	if(m_pDiskMonitor != 0)
	{
		delete m_pDiskMonitor;
		m_pDiskMonitor = 0;
	}
	DEBUG("%s","Deleting perf pointer");
	if(m_pPerfMonitor != 0)
	{
		delete m_pPerfMonitor;
		m_pPerfMonitor = 0;
	}
	DEBUG("%s","Deleting Folder pointer");
	if(m_pFolderQuotaMon != 0)
	{
		delete m_pFolderQuotaMon;
		m_pFolderQuotaMon = 0;
	}
	DEBUG("%s","Deleting omHandlerPtr");
	if(omHandlerPtr != 0)
	{
		omHandlerPtr->Finalize();
		delete omHandlerPtr;
		omHandlerPtr = 0;
	}

	if( m_pAlarmHandler != NULL )
	{
		delete m_pAlarmHandler;
		m_pAlarmHandler = 0;
	}

	if( m_plhEventHandles[0] != (ACE_Event*)NULL )
	{
		delete m_plhEventHandles[0];
		m_plhEventHandles[0] = 0;
	}

	if( m_plhEventHandles[1] != (ACE_Event*)NULL )
	{
		delete m_plhEventHandles[1];
		m_plhEventHandles[1] = 0;
	}

	if( m_plhEventHandles[2] != (ACE_Event*)NULL )
	{
		delete m_plhEventHandles[2];
		m_plhEventHandles[2] = 0;
	}
	if(m_poReactor != 0)
	{
		m_poReactor->remove_handler(signalSet);
		m_poReactor->close();
		delete m_poReactor;
		m_poReactor = NULL;
	}
	if(thetpReactorImpl != 0)
        {
                delete thetpReactorImpl;
                thetpReactorImpl = NULL;
        }

	DEBUG("%s","Leaving SSU_Service_Loader::~SSU_Service_Loader");
}


void SSU_Service_Loader::printUsageAndDie(const ACE_TCHAR *prog)
{

	DEBUG(
		   "Usage: ssu_monitor_start_script",
		   " -V -d -f <file> ",
		   " -V: Print the version\n",
		   " -d: <time in seconds> No fork - Run as a regular application for particular",
		   "  time for Debug purpose and Use current directory as working directory\n",
		   " -f: <file> Configuration file, default is \"/etc/ssumonitorservice.conf \"\n",
		   prog,
		   0);

	ACE_OS::exit(1);
}


ACE_INT32 SSU_Service_Loader::parseArgs( ACE_UINT32 argc, ACE_TCHAR *argv[] )
{
	ssuServiceStatus = SSU_IN_DEBUG;
	if (!argc)
		return 0;

    //! Write to both console and log file
    ACE_LOG_MSG->set_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);

	ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("Vd:f:"));

	ACE_INT32 c;

	while ((c = get_opt()) != -1) {

		switch (c) {
		case 'V':
			ACE_OS::printf("SSU Monitor Service  Version %s\n", SSU_SERVICE_VER);
			return 1;
		case 'd':
			m_bInteractive = true;
            //! Convert String to unsigned 64 Bit integer
			m_s64ExecuteTimeout = ACE_OS::strtoull(get_opt.opt_arg(),'\0',10);

			//SSU Monitor Service  is in debug mode for  m_s64ExecuteTimeout

			if( m_s64ExecuteTimeout < 0 )
               return -1;
			break;
		case 'f':
			config_file_ = get_opt.opt_arg();

			break;
		default:
			printUsageAndDie(argv[0]);
			break;
		}
	}


	//! Stop writing to Terminal to Output stream
	ACE_LOG_MSG->clr_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);

	return (ACE_INT32)0;
}

int SSU_Service_Loader::active(ACS_SSU_Service* haObj )
{
	DEBUG("%s","Entering SSU_Service_Loader::active()");
	ssuServiceStatus = SSU_IN_ACTIVE;
	m_haObj=haObj;

	// start with active activities.
	if(startSSUFunctionalityinActive() == ACS_CC_FAILURE)
	{
		DEBUG("%s","Failed to start SSU Functionality in ACTIVE mode");
		DEBUG("%s","Exiting SSU_Service_Loader::active()");
		return -1;
	}
	DEBUG("%s","Exiting SSU_Service_Loader::active()");
	return 0;
}

int SSU_Service_Loader::passive(ACS_SSU_Service* haObj )
{
	DEBUG("%s","Entering SSU_Service_Loader::passive()");
	ssuServiceStatus = SSU_IN_PASSIVE;
	m_haObj=haObj;

	// start with passive activities
	if(startSSUFunctionalityinPassive() == ACS_CC_FAILURE)
	{
		DEBUG("%s","Failed to start SSU Functionality in PASSIVE mode");
		DEBUG("%s","Exiting SSU_Service_Loader::active()");
		return -1;
	}
	DEBUG("%s","Exiting SSU_Service_Loader::passive()");
	return 0;
}

void SSU_Service_Loader::stop() 
{
	DEBUG("%s","Entering SSU_Service_Loader::stop()");
	// start shutdown activities.
	shutdown();
	DEBUG("%s","Exiting SSU_Service_Loader::stop()");
}

ACS_CC_ReturnType SSU_Service_Loader::startSSUFunctionalityinActive()
{
	DEBUG("%s","Entering SSU_Service_Loader::startSSUFunctionalityinActive");
	initService();

	if( omHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		DEBUG("%s","Error occurred while initializing APGCC OmHandler.");
		DEBUG("%s","Exiting SSU_Service_Loader::startSSUFunctionalityinActive");
		return ACS_CC_FAILURE;
	}

	if (ACS_SSU_Common::fetchDnOfRootObjFromIMM(omHandlerPtr) == -1)
	{
		DEBUG("%s","Unable to fetch DN of SSU root from IMM.");
		DEBUG("%s","SSU IMM files are not configured properly. Hence exiting from Service ...");
		DEBUG("%s","Exiting SSU_Service_Loader::startSSUFunctionalityinActive");
		return ACS_CC_FAILURE;
	}

	if (!ACS_SSU_Common::GetAllPhaParameters(omHandlerPtr))
	{
		DEBUG("%s","Unable to fetch Initial params of SSU from IMM.");
		DEBUG("%s","SSU IMM files are not configured properly. Hence exiting from Service ...");
		DEBUG("%s","Exiting SSU_Service_Loader::startSSUFunctionalityinActive");
		return ACS_CC_FAILURE;
	}

	ACE_INT32 s32GrpId = setupMonitorThreadsinActive();
	if( s32GrpId != -1 )
	{
		DEBUG("%s", "Threads are spawned in Active mode successfully.");
	}
	else
	{
		ERROR("%s"," Failure in setting up of threads. Exit");
	}

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType SSU_Service_Loader::startSSUFunctionalityinPassive()
{
	DEBUG("%s","Entering SSU_Service_Loader::startSSUFunctionalityinPassive");
	initService();

	if( omHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		ERROR("%s","Error occurred while initializing APGCC OmHandler.");
		DEBUG("%s","Exiting SSU_Service_Loader::startSSUFunctionalityinPassive");
		return ACS_CC_FAILURE;
	}

	if (ACS_SSU_Common::fetchDnOfRootObjFromIMM(omHandlerPtr) == -1)
	{
		ERROR("%s","Unable to fetch DN of SSU root from IMM.");
		DEBUG("%s","SSU IMM files are not configured properly. Hence exiting from Service ...");
		DEBUG("%s","Exiting SSU_Service_Loader::startSSUFunctionalityinPassive");
		return ACS_CC_FAILURE;
	}

	if (!ACS_SSU_Common::GetAllPhaParameters(omHandlerPtr))
	{
		ERROR("%s","Unable to fetch Initial params of SSU from IMM.");
		DEBUG("%s","SSU IMM files are not configured properly. Hence exiting from Service ...");
		DEBUG("%s","Exiting SSU_Service_Loader::startSSUFunctionalityinPassive");
		return ACS_CC_FAILURE;
	}

	ACE_INT32 s32GrpId = setupMonitorThreadsinPassive();
	if(s32GrpId != -1)
	{
		DEBUG("%s", "Threads are spawned in PASSIVE mode successfully.");
	}
	else
	{
		ERROR("%s \n"," Failure in setting up of threads. Exit");
	}

	return ACS_CC_SUCCESS;
}
ACS_CC_ReturnType SSU_Service_Loader::startSSUFunctionalityInteractiveMode()
{
	ACE_INT32 s32GrpId = setupMonitorThreads();
	if( s32GrpId != -1 )
	{
		setupShutdownHandler( m_poReactor );
		//! Start reactor event loop
		m_poReactor->run_reactor_event_loop();
	}
	else
	{
		ERROR("%s \n"," Failure in setting up of threads. Exit");
		return ACS_CC_FAILURE;
	}
	return ACS_CC_SUCCESS;
}

ACE_INT32 SSU_Service_Loader::run_i()
{
	DEBUG(" %s ","In SSU_Service_Loader::run_i");
	if (!this->getLock(this->pid_file_.c_str()))
	{
		DEBUG("Could not lock pidfile: %s ",pid_file_.c_str());
		return EXIT_ERROR;
	}
	//! if m_u64ExecuteTimeout=0 Service runs in interactive mode until terminated
	if( m_bInteractive && ( m_s64ExecuteTimeout != 0 ) )
	{
		m_s32TimerId = setTimeoutForInteractiveMode( m_s64ExecuteTimeout );
	}
	startSSUFunctionalityInteractiveMode();
	//! release the lock
	if ( this->lockFd_ != -1 )
	{
		unlockFd(this->lockFd_);
		close(this->lockFd_);
		this->lockFd_ = -1;
	}
	ACE_OS::unlink (this->pid_file_.c_str());
	DEBUG("%s ","Lock Released and Exiting SSU Monitor Service");
	return EXIT_SUCCESS;
}

ACE_INT32 SSU_Service_Loader::runService()
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
}

ACE_INT32 SSU_Service_Loader::execute (void)
{
	if( omHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		ERROR("%s","Error occurred whil initializing APGCC OmHandler");
		DEBUG("%s","Exiting from SSU_Service_Loader::execute");
		return -1;
	}
	if (ACS_SSU_Common::fetchDnOfRootObjFromIMM(omHandlerPtr) == -1)
	{
		ERROR("%s","Unable to fetch DN of SSU root from IMM");
		DEBUG("%s","SSU IMM files are not configured properly. Hence exiting from Service ...");
		return -1;
	}

	return this->runService();
}

bool SSU_Service_Loader::getLock(const char *lock_file_path)
{

    DEBUG("%s ","In SSU_Service_Loader::getLock() ");
    DEBUG("Input file path: %s",lock_file_path);


	// get the lock and write pid
	if (getProcessLock(this->lockFd_, pid_file_.c_str()))
	{
		DEBUG("%s ","fd locked successfully ");

		return true;
	}

	if (-1 != this->lockFd_) {
		close(this->lockFd_);
		this->lockFd_ = -1;
	}
	DEBUG("%s ","fd lock not successful");
    return false;
}
/*
 * Will attempt to lock the PID file and write the
 * pid into it. This function will return false for
 * all errors. Callee is responsible for closing *fd
 * if (*fd != -1).
 */
bool SSU_Service_Loader::getProcessLock(ACE_HANDLE  & fd, const char *path)
{

	DEBUG("In SSU_Service_Loader::getProcessLock:%s",path);
	std::string pid_file;

	char buf[32] = { '\0' };
	ACE_INT32  p = 0;
	ssize_t w = 0;

	fd = -1;
	if (!path || !strlen(path))
	{
		ERROR("%s ","process locking unsuccessful ");
		return false;
	}

	fd = ACE_OS::open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (fd == -1)
	{
		ERROR("%s ","process locking unsuccessful ");
		return false;
	}

	// lock it
	if (lockFd(fd))
	{
		ERROR("%s ","process locking unsuccessful ");
		return false;
	}

	// we have the lock
	if (ftruncate(fd, 0))
	{
		ERROR("%s ","process locking unsuccessful ");
		return false;
	}

	/* write pid */
	p = snprintf(buf, sizeof(buf), "%d", ACE_OS::getpid());

	//! sizeof(char) is required to write Null character
	w = ACE_OS::write(fd, buf, strlen(buf) + sizeof(char));
	p += sizeof(char);
	if (p != (ACE_INT32)w)
	{
		ERROR("%s ","process locking unsuccessful ");
		return false;
	}

	if (ACE_OS::fsync(fd))
	{
		ERROR("%s ","process locking unsuccessful ");
		return false;
	}


	DEBUG(" %s ","process locked successfully ");

	return true;
}

ACE_INT32 SSU_Service_Loader::lockFd(const ACE_HANDLE fd)
{
	DEBUG("%s ","In SSU_Service_Loader::lockFd()");
        struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	if ( fd == -1 )
	{
		ERROR("%s ","lockFd is -1 ");

		return -1;
	}

	return ACE_OS::fcntl(fd, F_SETLK, reinterpret_cast<long>(&lock));
}


ACE_INT32 SSU_Service_Loader::unlockFd(const ACE_INT32 fd)
{
	DEBUG("In SSU_Service_Loader::unlockFd():%d ",fd);

	struct flock lock;

	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	if (-1 == fd)
	{
		ERROR("%s ","unLockFd is -1 ");
		return -1;
	}

	return ACE_OS::fcntl(fd, F_SETLK, reinterpret_cast<long>(&lock));
}


bool SSU_Service_Loader::setupShutdownHandler( ACE_Reactor *poReactor )
{

	DEBUG("%s "," In SSU_Service_Loader::setupShutdownHandler()");

	//! Register SIGINT
	if( m_bInteractive )
		signalSet.sig_add(SIGINT);

	signalSet.sig_add( SIGTSTP );
	signalSet.sig_add( SIGTERM );
	//! Signal register for USR1 for running alarm
	signalSet.sig_add( SIGUSR1 );

	//! Register the signal_handler method to process all the signals in sigset
	poReactor->register_handler( signalSet, this );

	DEBUG("%s "," ShutdownHandler setup successful");

	return true;
}
int SSU_Service_Loader::shutdown()
{
   DEBUG("shut down trigger for SSU Monitor Service PID: %d",getpid());
   int ret = 0;
   if(m_plhEventHandles[0] != NULL)
   {
	   m_plhEventHandles[0]->signal();
   }

   if(m_plhEventHandles[1] != NULL)
   {
	   m_plhEventHandles[1]->signal();
   }

   if(m_plhEventHandles[2] != NULL)
   {
	   m_plhEventHandles[2]->signal();
   }
	//shutdown triggred is true to stop execution of the new procls 
	m_pAlarmHandler->isShutdownTriggered = true;

	if(m_pAlarmHandler->outstandingPID != -1)
	{
		int sig = 9;
		int status = kill((m_pAlarmHandler->outstandingPID),sig);
		if(!status)
		{
			DEBUG("%s","Alarm handler outstanding job killed");
		}
		else
		{
			DEBUG("%s","Alarm handler outstanding job kill failed");
		}

	}
	if(m_pFolderQuotaMon != 0)
	{
		m_pFolderQuotaMon->stopFolderQuotaMonThread();
		//ACE_Thread_Manager::instance()->join(folderMonThreadId);
		m_pFolderQuotaMon->wait();
		DEBUG("%s","Folder quotas monitor thread is closed successfully");
	}
	if(m_pPerfMonitor != 0)
	{
		ACE_Thread_Manager::instance()->join(perfMonThreadId);
		DEBUG("%s","PerfMonThread is closed successfully");
	}
	if(m_pDiskMonitor != 0)
	{
		ACE_Thread_Manager::instance()->join(diskMonThreadId);
		DEBUG("%s","DiskMonThread thread is closed successfully");
	}

	if (ssuServiceStatus == SSU_IN_DEBUG) // If the service is in DEBUG mode
	{
	  ret = m_poReactor->end_reactor_event_loop();
	}

   DEBUG("shut down of SSU Monitor Service PID: %d successful",getpid());
   return ret;
}

int SSU_Service_Loader::handle_timeout(const ACE_Time_Value &current_time, const void *)
{

	DEBUG("%s","In SSU_Service_Loader::handle_timeout()");
    DEBUG("%ld",current_time.msec());

   if( m_bInteractive )
   {
	   DEBUG(" Timeout For Interactive mode received for SSU PID:%d",getpid());
	   ACE_OS::kill(ACE_OS::getpid(), SIGTERM);
   }

   return 0;
}

int SSU_Service_Loader::handle_signal(ACE_INT32 signum, siginfo_t * siginfo, ucontext_t * ucontext)
{
	DEBUG("Received Signal %d siginfo %d uc_flags %ld",signum,siginfo->si_signo,ucontext->uc_flags);

	if( !m_poServiceHandler )
	{
		ERROR("%s "," m_poServiceHandler is NULL ");
		return 0;
	}

	if(signum == SIGINT && ( m_bInteractive && ( m_s64ExecuteTimeout != 0 )))
	{
		s32ResetTimer(m_s32TimerId);
	}

	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
		//Report event
		ACE_TCHAR szData[256];

		snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("SSU Monitor service terminated as requested by the user"));

		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);

		DEBUG("\n\n%s\n\n",szData);

		m_poServiceHandler->shutdown();
	}
	return 0;
}

ACE_INT32 SSU_Service_Loader::setTimeoutForInteractiveMode( ACE_UINT64  u64TimeOut)
{
	DEBUG("setTimeOutForInteractiveMode %d ",u64TimeOut);

	ACE_Time_Value exptime(u64TimeOut);

	ACE_INT32 s32TimerId = ACE_Reactor::instance()->schedule_timer(this,0,exptime);

	DEBUG("setTimeoutForInteractiveMode: Timer created:%s ",s32TimerId==-1?"Failure":"Successful");
	return s32TimerId;
}

ACE_INT32 SSU_Service_Loader::s32ResetTimer(ACE_INT32 s32Timer)
{
    ACE_ASSERT( s32Timer!=-1 );

	ACE_INT32 s32Result = ACE_Reactor::instance()->cancel_timer(s32Timer);

    DEBUG("SSU_Service_Loader::bResetTimer : %s ",s32Result==1?"SUCCESS":"FAILURE");

    return s32Result;
}

ACE_INT32 SSU_Service_Loader::setupMonitorThreadsinActive()
{
    DEBUG("%s ","Entering SSU_Service_Loader::setupMonitorThreadsinActive");

	m_pAlarmHandler = new ACS_SSU_AlarmHandler();
	if (m_pAlarmHandler==NULL)
	{
		// Report error
		ACE_TCHAR szData[256];
		sprintf(szData,
				ACE_TEXT("Unable to create an object instance of the \"%s\" class due to the following error:\r\n%s"),
				ACE_TEXT("ACS_SSU_AlarmHandler"),
				strerror(errno));

		ERROR("%s",szData);

		// Send event
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		DEBUG("%s ","Exiting SSU_Service_Loader::setupMonitorThreadsinActive");
		return -1;
	}

	m_lpEndEvent = m_plhEventHandles[0] = new ACE_Event(true,FALSE,USYNC_THREAD,"EV_ACS_SSU_TERMINATE_EVENT");

	// Perf Monitor End Event.
	m_plhEventHandles[1] = new ACE_Event(true,FALSE,USYNC_THREAD,"EV_ACS_SSU_PERFMON_TERMINATE_EVENT");
	// Folder Monitor End Event.
	m_plhEventHandles[2] = new ACE_Event(true,FALSE,USYNC_THREAD,"EV_ACS_SSU_FOLDERMON_TERMINATE_EVENT");


	if (m_plhEventHandles[0] == NULL)
	{
		// Report error
		ACE_TCHAR szData[256];
		snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Unable to create event due to the following error:\r\n%s"),
				strerror(errno));

		ERROR("%s",szData);

		// Send event
		(void)ACS_SSU_AEH::ReportEvent(1001,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("CREATE EVENT FAILED IN SSU SYSTEM SUPERVISOR"));

		DEBUG("%s ","Exiting SSU_Service_Loader::setupMonitorThreadsinActive");
		return -1;
	}

	//! Store the Groups Id which is later used to terminate all threads spawned


	 s32GrpIdForAll = ACS_SSU_Common::StartThread(DiskMonThread, (void*)this, ACE_TEXT("DiskMonThread"),&diskMonThreadId);
	 DEBUG("Starting DiskMonThread, Thread Id: %u groupId:%d ",diskMonThreadId,s32GrpIdForAll);
	//! A failure in creation of one thread should shutdown entire SSU

	 if( s32GrpIdForAll != -1 )
	 {
		 ACE_INT32 s32Result;

		 s32Result = ACS_SSU_Common::StartThread(PerfMonThread, (void*)this, ACE_TEXT("PerfMonThread"),&perfMonThreadId);
		 DEBUG("Starting PerfMonThread, Thread Id: %u groupId:%d ",perfMonThreadId,s32Result);

		#if 0
		 ACE_OS::sleep(1);
		 s32Result = ACS_SSU_Common::StartThread(FolderQuotaMonThread, (void*)this, ACE_TEXT("FolderQuotaMon"),&folderMonThreadId);
		 DEBUG("Starting FolderQuotaMonThread, Thread Id: %u groupId:%d ",folderMonThreadId,s32Result);
		#endif
		FolderQuotaMon();
		
	 }

	DEBUG("%s ","Exiting SSU_Service_Loader::setupMonitorThreadsinActive");
	return s32GrpIdForAll;
}

ACE_INT32 SSU_Service_Loader::setupMonitorThreads()
{
    DEBUG("%s ","In SSU_Service_Loader::setupMonitorThreads");
	ACE_INT32 status;
	if(ACS_SSU_Common::CheckIfActiveNode()==1)
	{
		status=setupMonitorThreadsinActive();	
	}
	else
	{
		status=setupMonitorThreadsinPassive();
	}
	return status;
}

ACE_INT32 SSU_Service_Loader::setupMonitorThreadsinPassive()
{
    DEBUG("%s ","Entering SSU_Service_Loader::setupMonitorThreadsinPassive");

	m_pAlarmHandler = new ACS_SSU_AlarmHandler();
	if (m_pAlarmHandler==NULL)
	{
		// Report error
		ACE_TCHAR szData[256];
		sprintf(szData,
				ACE_TEXT("Unable to create an object instance of the \"%s\" class due to the following error:\r\n%s"),
				ACE_TEXT("ACS_SSU_AlarmHandler"),
				strerror(errno));

		ERROR("%s",szData);

		// Send event
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

		DEBUG("%s ","Exiting SSU_Service_Loader::setupMonitorThreadsinPassive");
		return -1;
	}

	m_lpEndEvent = m_plhEventHandles[0] = new ACE_Event(true,FALSE,USYNC_THREAD,"EV_ACS_SSU_TERMINATE_EVENT");

	// Perf Monitor End Event.
	m_plhEventHandles[1] = new ACE_Event(true,FALSE,USYNC_THREAD,"EV_ACS_SSU_PERFMON_TERMINATE_EVENT");

	if (m_plhEventHandles[0] == NULL)
	{
		// Report error
		ACE_TCHAR szData[256];
		snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Unable to create event due to the following error:\r\n%s"),
				strerror(errno));

		ERROR("%s",szData);

		// Send event
		(void)ACS_SSU_AEH::ReportEvent(1001,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szData,
				ACE_TEXT("CREATE EVENT FAILED IN SSU SYSTEM SUPERVISOR"));

		DEBUG("%s ","Exiting SSU_Service_Loader::setupMonitorThreadsinPassive");
		return -1;
	}

	//! Store the Groups Id which is later used to terminate all threads spawned


	 s32GrpIdForAll = ACS_SSU_Common::StartThread(DiskMonThread, (void*)this, ACE_TEXT("DiskMonThread"),&diskMonThreadId);
	 DEBUG("Starting DiskMonThread, ThreadId:%u groupId:%d ",diskMonThreadId,s32GrpIdForAll);
	//! A failure in creation of one thread should shutdown entire SSU

	 if( s32GrpIdForAll != -1 )
	 {
		 ACE_INT32 s32Result;

		 s32Result = ACS_SSU_Common::StartThread(PerfMonThread, (void*)this, ACE_TEXT("PerfMonThread"),&perfMonThreadId);
		 DEBUG("Starting PerfMonThread, ThreadId: %u groupId:%d ",perfMonThreadId,s32GrpIdForAll);
	 }

	DEBUG("%s ","Exiting SSU_Service_Loader::setupMonitorThreadsinPassive");
	return s32GrpIdForAll;
}

/**
 * Setup Disk Monitor Thread
 */
ACE_THR_FUNC_RETURN SSU_Service_Loader::DiskMonThread(void* pvThis)
{
    DEBUG("%s","Spawning DiskMonThread");
   ((SSU_Service_Loader*)pvThis)->DiskMon();
   return 0;
}

ACE_THR_FUNC_RETURN SSU_Service_Loader::PerfMonThread(void* pvThis)
{
	DEBUG("%s","Spawning PerfMonThread");

   ((SSU_Service_Loader*)pvThis)->PerfMon();
   return 0;
}

ACE_THR_FUNC_RETURN SSU_Service_Loader::FolderQuotaMonThread(void* pvThis)
{
	DEBUG("%s","Spawning FolderQuotaMonThread");

   ((SSU_Service_Loader*)pvThis)->FolderQuotaMon();
   DEBUG("%s","Leaving FolderQuotaMonThread");
   return 0;
}

//! Running the DiskMon thread
void SSU_Service_Loader::DiskMon()
{
	DEBUG("%s ","Staring Disk Monitor Thread");
	m_pDiskMonitor = new ACS_SSU_DiskMon(m_plhEventHandles[0], m_pAlarmHandler);
	m_pDiskMonitor->Execute();
}

////! Running the PerfMon thread
void SSU_Service_Loader::PerfMon()
{

   DEBUG("%s ","Staring Performance Monitor Thread");
   m_pPerfMonitor = new ACS_SSU_PerfMon(m_plhEventHandles[1], m_pAlarmHandler);
   m_pPerfMonitor->Execute();
}
//////! Running the FolderQuotaMon thread
void SSU_Service_Loader::FolderQuotaMon()
{

   DEBUG("%s ","Starting Folder Quota Monitor Thread ");

   m_pFolderQuotaMon = new ACS_SSU_FolderQuotaMon(m_plhEventHandles[2], m_pAlarmHandler,omHandlerPtr);
   if (!m_pFolderQuotaMon)
   {
	   // Report error
	   ACE_TCHAR szData[256];
	   sprintf(szData,
			   ACE_TEXT("Unable to create an object instance of the \"%s\" class due to the following error:\r\n%s"),
			   ACE_TEXT("ACS_SSU_FolderQuotaMon"),
			   strerror(errno));

	   ERROR("%s",szData);

	   // Send event
	   (void)ACS_SSU_AEH::ReportEvent(1003,
			   ACE_TEXT("EVENT"),
			   CAUSE_AP_INTERNAL_FAULT,
			   ACE_TEXT(""),
			   szData,
			   ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

	   return;
   }
   m_pFolderQuotaMon->activate();
}

