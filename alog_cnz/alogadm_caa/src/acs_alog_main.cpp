
/*=================================================================== */
/**
  @file acs_alog_main.cpp

  Class method implementationn for acs_alog_main type module.

  This module contains the implementation of class declared in
  the acs_alog_main.h module

  @version 3.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       02/02/2011     xgencol/xgaeerr       Initial Release
  N/A       28/11/2011     xgencol               Final Release
  N/A		11/09/2015	   xsasarv				 HT73351
  =================================================================== */

#include <acs_alog_main.h>
#include <acs_alog_command.h>
#include <acs_aeh_signalhandler.h>
#include <pthread.h>
#include <fstream>
#include <acs_apgcc_oihandler_V3.h>

ACE_Thread_Mutex alog_stop_mutex;                                                // mutex used to stop alog Server
ACE_Condition<ACE_Thread_Mutex> *p_alog_stop_condition = 0;                      // condition used to stop alog Server

acs_apgcc_oihandler_V3 oiHandler;
ACS_ALOG_HA_AppManager *haObj = NULL;
acs_alog_activeWorks *oWorksNHA = NULL;

acs_alog_activeWorks  oWorksObj;

acs_alog_Imm_Implementer implementer_AL;
//acs_alog_Imm_Implementer implementer_CL;
acs_alog_Imm_Implementer implementer_EI;

MyThread_2 th1(&implementer_AL);
//MyThread_2 th2(&implementer_CL);
MyThread_2 th3(&implementer_EI);

pthread_mutex_t  Mutex;

// ... global objects used to represents command-line options
int cflag = 0;
int nohaflag = 0;

int     Failure = 0;

extern int State;

pid_t sid = 0;

string  CurrentLogFile;		//  used to specialize the File Name in the Alarms / Events
string  CurrentPLogFile;
string  CurrentCLogFile;

string  CurrentNodeState;
char    FileAudit[300] = "";

//char    FilePAudit[300] = "";
std::string protectedPassword;
std::string FilePAudit;
char    PathAudit[300] = "";
char    PathPAudit[300] = "";
char    PathCLog[300]  = "";

bool    IsRegImpl = false;

//  ALARM flags :  valued 0 if the alarm is not active, valued 1 if the alarm is active

char   OpenSysLogError= 0;
char   ReadSysLogError= 0;
char   OpenAttrFileError= 0;
char   ReadAttrFileError = 0;
char   OpenPMMLPRTFileError = 0;
char   ReadPMMLPRTFileError = 0;
char   OpenPMMCMDFileError  = 0;
char   ReadPMMLCMDFileError = 0;
char   OpenPUSRFileError  = 0;
char   ReadPUSRFileError = 0;


char   DeleteFilesError    = 0;       //  FCODE =  3
char   AttachTQerror       = 0;       //  FCODE =  7
char   AttachPLTQerror     = 0;		  //  FCODE =  7
char   AttachCLOGTQerror   = 0;		  //  FCODE =  7
char   SendFilesError      = 0;       //  FCODE =  8
char   SendCLOGFilesError  = 0;       //  FCODE =  8
char   TQdefinedError      = 0;       //  FCODE =  9
char   CommandLogTQdefinedError = 0;  //  FCODE =  11 NEW FCODE FOR COMMAND LOG TQ ERROR
char   DelCLogFilesError   = 0;       //  FCODE =  12
char   OpenPlogError       = 0;		  //  FCODE =  14
char   WritePlogError      = 0;		  //  FCODE =  15
char   DelPLogFilesError   = 0;       //  FCODE =  16
char   OpenPLogDirError    = 0;		  //  FCODE =  17
char   SendPLFilesError    = 0;		  //  FCODE =  18
char   ALOGdeactivateError = 0;       //  FCODE = ???

using namespace std;



void nodeStateActions(bool init)
{
	char mess[150];

	if (CurrentNodeState == "active")
	{
                if(init)
		{
			ACE_OS::sprintf(mess,"CURRENT NODE STATE is: %s\n", CurrentNodeState.c_str());
			ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(mess));
		}
        }else if(CurrentNodeState == "standby") {
                if(init) 
		{
			ACE_OS::sprintf(mess,"CURRENT NODE STATE is: %s\n", CurrentNodeState.c_str());
			ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR(mess));
		}
        }else if(CurrentNodeState != "active" && CurrentNodeState != "standby") {
                ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("CURRENT NODE STATE IS WRONG"));
                return;
        }
}


ACS_ALOG_ReturnType setInitialNodeState() {

	int nodeStateVal = 0;
	string nodeState;
	ACS_PRC_API prc;

	try {
		nodeStateVal = prc.askForNodeState(0);
	/* Drop user if there is one, and we were run as root */
	}
	catch(int e)
	{
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("i've tried to retrieve for nodeStateVal...but i'm fail"));
		return ACS_ALOG_error;
	}

	if(nodeStateVal == 1)
		nodeState = "active";
	else if(nodeStateVal == 2)
		nodeState = "standby";
	else if(nodeStateVal != 1 && nodeStateVal != 2)
		exit(0);

	CurrentNodeState = nodeState;

	nodeStateActions(true);
	return ACS_ALOG_ok;
}


int parse_command_line(int argc, char **argv)
 {
   int index;
   int c;
   int noha_opt_cnt = 0;

   static struct option long_options[] =
   {
     /* These options set a flag. */
         {"noha", no_argument, & nohaflag, 1},
         {0, 0, 0, 0}
   };

   /* getopt_long stores the option index here. */
   int option_index = 0;

   while ((c = getopt_long(argc, argv, "c", long_options, & option_index)) != -1)
         switch (c)
         {
           case 0:
                   // we've found a match with a long option, but we mustn't accept abbreviations (e.g. '--noh' instead of '--noha'). So check for this
                   if(strcmp(long_options[option_index].name, argv[optind-1] + 2))
                   {
                           fprintf(stderr,"acs_alogmaind: Unrecognized option '%s'\n",argv[optind-1]);
                           return -1;
                   }

                   /* found --noha option */
                   if(noha_opt_cnt > 0)
                   {
                           fprintf(stderr,"acs_alogmaind: duplicated long option 'noha'\n");
                           return -1;
                   }
                   ++noha_opt_cnt;
                   break;
           case 'c':
                   if(cflag)
                   {
                           fprintf(stderr,"acs_alogmaind: duplicated option 'c'\n");
                           return -1;
                   }
                   cflag = 1;
                 break;
           case '?':
                   return -1;
           default:
                 abort ();
         }

   /*
    * We must reject the command line  "acs_alog --".
    * Note that if "--" command line argument is found, getopt_long() increments <optind> variable and returns -1.
   */
   if((optind > 1) && !strcmp(argv[optind-1],"--"))
   {
           fprintf(stderr,"acs_alogmaind: Unrecognized option '%s'\n",argv[optind-1]);
           return -1;
   }

   if(optind < argc)
   {
           for (index = optind; index < argc; index++)
                   printf ("Incorrect usage: found non-option argument '%s'\n", argv[index]);
           return -1;
   }

   return 0;
}


void clean()
{

		delete  p_alog_stop_condition;
}


void stop_alog_daemon_NoHA()
{

        oWorksNHA->stopThReadMain = oWorksNHA->stopThReadApi = oWorksNHA->stopThHandleLogs = 1;
}


void stop_alog_daemon_HA()
{
        p_alog_stop_condition->signal();
		syslog (LOG_INFO, "The signal() command has been executed");
}


void child_handler(int signum)
{
	if(signum == SIGUSR2)
	{
		ACE_OS::close( STDIN_FILENO );
		ACE_OS::close( STDOUT_FILENO );
		ACE_OS::close( STDERR_FILENO );
	}
}


void parent_handler(int signum)
{
    switch(signum) {
    case SIGALRM: ACE_OS::exit(EXIT_FAILURE); break;
    case SIGUSR1: ACE_OS::exit(EXIT_SUCCESS); break;
    case SIGCHLD: ACE_OS::exit(EXIT_FAILURE); break;
    }
}


ACE_THR_FUNC_RETURN run_alog_daemon_NoHA( void* )
{
	//define the constants
	const char* CWD_DIR = "/";
	const mode_t FILE_MASK = 002;

    pid_t pid, sid, parent;

    /* already a daemon */
    if ( ACE_OS::getppid() == 1 ) return (ACE_THR_FUNC_RETURN) -1;

    /* Implement some locking mechanism */

     // check if there's another alog daemon instance running and, if so, exit function
        int fdlock = open("/var/run/ap/alogfile.lck", O_CREAT | O_WRONLY | O_APPEND, 0664);
        if(fdlock < 1)
                return (ACE_THR_FUNC_RETURN) -2;
        if(flock(fdlock, LOCK_EX | LOCK_NB) < 0)
        {
                if(errno == EWOULDBLOCK)
                {
                        ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("another alog daemon instance running"));
                        return (ACE_THR_FUNC_RETURN) 1;
                }
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("another alog daemon instance running"));
               return (ACE_THR_FUNC_RETURN) -3;
        }

    /* Trap signals that we expect to receive */
    ACE_OS::signal(SIGCHLD,parent_handler);
    ACE_OS::signal(SIGUSR1,parent_handler);
    ACE_OS::signal(SIGALRM,parent_handler);

    /* Fork off the parent process */
    pid = ACE_OS::fork();

    if (pid < 0) {
    	ACE_OS::strerror( (int)errno );
    	if(errno == EAGAIN)
    	{
    		// manage the error
                char trace[400];
    	        ACE_OS::sprintf(trace, "unable to fork daemon: Resource temporarily unavailable, code=%d", errno);
    		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(trace));
    	}
    	else
    	{
			if(errno == ENOMEM)
			{
				// manage the error
				   char trace[400];
				   ACE_OS::sprintf(trace, "unable to fork daemon: Not enough space, code=%d", errno);
				   ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(trace)); 
	                           return (ACE_THR_FUNC_RETURN) 1;

			}
			else{
			  	   char trace[400];
				   ACE_OS::sprintf(trace, "unable to fork daemon, code=%d (%s)",
				   errno, ACE_OS::strerror(errno));
			           ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(trace));             
                        	   return (ACE_THR_FUNC_RETURN) 1;
			}
    	}
    	ACE_OS::exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {

        /* Wait for confirmation from the child via SIGTERM or SIGCHLD, or
           for two seconds to elapse (SIGALRM).  pause() should not return. */
    	ACE_OS::alarm(2);
    	pause();

        ACE_OS::exit(EXIT_FAILURE);
    }

    /* At this point we are executing as the child process */
    parent = ACE_OS::getppid();

    /* Cancel certain signals */
    ACE_OS::signal(SIGCHLD,SIG_DFL); /* A child process dies */
    ACE_OS::signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
    ACE_OS::signal(SIGTTOU,SIG_IGN);
    ACE_OS::signal(SIGTTIN,SIG_IGN);
    ACE_OS::signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
    ACE_OS::signal(SIGTERM,SIG_DFL); /* Die on SIGTERM */

    ACE_OS::signal(SIGUSR2,child_handler);

    /* Change the file mode mask */
    ACE_OS::umask(FILE_MASK);

    /* Create a new SID for the child process */
    sid = ACE_OS::setsid();
    if (sid < 0) {
            char trace[400];
            ACE_OS::sprintf(trace, "unable to create a new session, code %d (%s)",
                        	errno, ACE_OS::strerror(errno));
	    ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(trace));
    	    ACE_OS::exit(EXIT_FAILURE);
    }

        char strsid[50];
        ACE_OS::sprintf(strsid,"%d",sid);
        string parsid = strsid;
        oWorksNHA = new (std::nothrow) acs_alog_activeWorks(parsid);

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((ACE_OS::chdir(CWD_DIR)) < 0) {
            char trace[400];
            ACE_OS::sprintf(trace, "unable to change directory to %s, code %d (%s)",
                   "/", errno, ACE_OS::strerror(errno));
            ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR(trace));
            ACE_OS::exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
#ifdef KDEBUG
    ACE_OS::freopen( "/dev/null", "r", stdin);
    ACE_OS::freopen( "/dev/null", "w", stdout);
    ACE_OS::freopen( "/dev/null", "w", stderr);
#else
    ACE_OS::close( STDIN_FILENO );
    ACE_OS::close( STDOUT_FILENO );
    ACE_OS::close( STDERR_FILENO );
#endif
    /* Tell the parent process that we are A-okay */
    ACE_OS::kill( parent, SIGUSR1 );

	if(setInitialNodeState())
	{
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed"));
	                return (ACE_THR_FUNC_RETURN) 1;
	}

	if(!CurrentNodeState.compare("active"))
	{
		oWorksNHA->setnoWork(1);
		if(oWorksNHA->alogInit())
		{
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Init phase failed"));
			return (ACE_THR_FUNC_RETURN) 1;
		}
		if(registerImplementers())
		{
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Daemon alog failure"));
		}
		if(oWorksNHA->alogWorks())
		{
            		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Daemon start failed"));
            		return (ACE_THR_FUNC_RETURN) 1;
		}
	}
	else
	{
		oWorksNHA->setnoWork(0);
		if(oWorksNHA->alogWorks())
		{
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Daemon start failed"));
			return (ACE_THR_FUNC_RETURN) 1;
		}
	}

    while(1) {

                if(!CurrentNodeState.compare("active"))
                {
                	oWorksNHA->setnoWork(1);
                	sleep(1);
                }
                else {
                        oWorksNHA->setnoWork(0);
                        if(removeImplementer())
                        {
                                ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("remove imlementers failed"));
                                return (ACE_THR_FUNC_RETURN) 1;
                        }
                        sleep(1);
                }
	}

        // delete lock file
        if(unlink("/var/run/ap/alogfile.lck") < 0)
                ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Error deleting ALOG Daemon lock file"));

    	close(fdlock);
	return 0;
}


ACE_THR_FUNC_RETURN run_alog_daemon_HA( void* )
{

//		string message = " -> Init Thread run_alog_daemon_HA \n";
//      ACSALOGTRACE(message);

		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("run_alog_daemon_HA Start"));

        char strsid[50] = {0};
        ACE_OS::sprintf(strsid,"%d",sid);
        string parsid = strsid;

//      oWorksObj = new (std::nothrow)acs_alog_activeWorks(strsid);
        oWorksObj.setAllPar(strsid);
        oWorksObj.setHA(1);

    	// create ACE_Condition object to be used to stop ALOG Daemon
    	p_alog_stop_condition = new (std::nothrow) ACE_Condition<ACE_Thread_Mutex>(alog_stop_mutex);

    	if (! p_alog_stop_condition)
    	{
    		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("run_alog_daemon_HA() - Memory error allocating object"));
//          delete oWorksObj;
			return (ACE_THR_FUNC_RETURN) -1;
    	}

    	if (oWorksObj.alogWorks())
        {
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Daemon start failed"));
                Failure = 1;
        }

        if (pthread_mutex_init (&Mutex, NULL) != 0)
        {
					syslog(LOG_ERR, "run_alog_daemon_HA() - Mutex init failed !");
					ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Mutex init failed !"));
					oWorksObj.stopThread();
//					delete oWorksObj;
					clean();
					return (ACE_THR_FUNC_RETURN) -1;
        }

        ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("run_alog_daemon_HA() ... wait for stop command"));
        p_alog_stop_condition->wait();        	// wait for stop command
        ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("run_alog_daemon_HA() ... stop command arrived"));

        ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("run_alog_daemon_HA() ... stop the threads"));
        oWorksObj.stopThread();
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("run_alog_daemon_HA() ... stop the threads done"));

        pthread_mutex_destroy (&Mutex);

    	if (ALOGdeactivateError == 1)
    	{
				AEHEVENTMESSAGE ("acs_alogmaind", 9900, "CEASING",
								 "AUDIT LOG DEACTIVATED", "APZ", "acs_alogmaind/Alarm", "", "");
    	}

    	if (DeleteFilesError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9910, "CEASING",
							     "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (TQdefinedError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9911, "CEASING",
							     "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (CommandLogTQdefinedError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9901, "CEASING",
							     "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (AttachTQerror == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9912, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (SendFilesError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9913, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (AttachCLOGTQerror == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9915, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (SendCLOGFilesError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9916, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (DelCLogFilesError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9914, "CEASING",
							     "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (DelPLogFilesError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9934, "CEASING",
								 "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (OpenPlogError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9932, "CEASING",
								 "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (WritePlogError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9933, "CEASING",
								 "AUDIT LOG FAULT", "APZ", "acs_alogmaind/Alarm", "", "");
		}

    	if (AttachPLTQerror == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9927, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
		}

    	if (SendPLFilesError == 1)
		{
				AEHEVENTMESSAGE ("acs_alogmaind", 9928, "CEASING",
								 "DATA OUTPUT, AP TRANSMISSION FAULT", "APZ", "acs_alogmain/Alarm", "", "");
		}

//      delete oWorksObj;

        clean();    		// release objects dynamically allocated

        ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("run_alog_daemon_HA End"));

        return 0;
}


ACE_THR_FUNC_RETURN HandleCmd(void*){

	int thread_exit_code = -1;

	do {

		acs_alog_command command;

		thread_exit_code = command.CommandManagement();

		if ( thread_exit_code )
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Protect Logging Thread exits due to error - Thread restarted"));

	} while ( thread_exit_code );

	return 0;
}


ACS_ALOG_ReturnType registerImplementers()
{

		if (IsRegImpl == true)  return ACS_ALOG_ok;

		ACSALOGLOG(LOG_LEVEL_INFO,TEXTERROR("Execution of registerImplementers"));

//		string  message = " -> Start register implementers \n";
//      ACSALOGTRACE(message);

        if (ImplementerAuditLogging())
        {
                ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Register implementers :  error on AuditLogging"));
                return ACS_ALOG_error;
        }

//        if(ImplementerCommandLog())
//        {
//                ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Daemon alog stopped"));
//		return ACS_ALOG_error;
//        }

        if (ImplementerExclusionItem())
        {
                ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Register implementers :  error on ExclusionItem"));
                return ACS_ALOG_error;
        }

//      message = " -> Finish register implementers \n";
//      ACSALOGTRACE(message);

        IsRegImpl = true;
        return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType removeImplementer()
{
	char  nameClass[40] = "AxeAuditLoggingExclusionItem";
        ACS_CC_ReturnType result;

        if (IsRegImpl == false)  return ACS_ALOG_ok;
        
        th1.stop();									//HT73351
       	result = oiHandler.removeObjectImpl(&implementer_AL);
       	if (result == ACS_CC_SUCCESS)
       	{
                ACSALOGLOG(LOG_LEVEL_WARN,TEXTERROR("Implementer on object of class AuditLogging removed"));
       	}
       	else {
                ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Implementer on object of class AuditLogging not removed"));
       	}

       	th3.stop();									//HT73351
        result = oiHandler.removeClassImpl(&implementer_EI, nameClass);
        if (result == ACS_CC_SUCCESS)
        {
                ACSALOGLOG(LOG_LEVEL_WARN,TEXTERROR("Implementer on object of class ExclusionItem removed"));
        }
        else {
                ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Implementer on object of class ExclusionItem not removed"));
        }
      //  th3.stop();

        th1.wait();
       	th3.wait();

        IsRegImpl = false;
        return ACS_ALOG_ok;
}


ACS_ALOG_ReturnType ImplementerAuditLogging()
{

	int max_retry = 30;

	const char *dnObjName = AUDITDNNAME;
	const char *impName = "impAlog";
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	implementer_AL.setObjName(dnObjName);
	implementer_AL.setImpName(impName);
	implementer_AL.setScope(scope);

	while ( oiHandler.addObjectImpl(&implementer_AL) != ACS_CC_SUCCESS ) 
	{
		max_retry --;
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Registered implementer on object of class AuditLogging failed"));
		
		if ( max_retry == 0 )  return ACS_ALOG_error;

		usleep(200000);
	}

	th1.start();
	th1.activate();

	ACSALOGLOG(LOG_LEVEL_WARN,TEXTERROR("Implementer on object of class AuditLogging registered"));

	return ACS_ALOG_ok;
}

ACS_ALOG_ReturnType ImplementerExclusionItem()
{

	int max_retry = 30;

	char nameClass[40] = "AxeAuditLoggingExclusionItem";
	const char *impName = "impAlogExclusionItem";

	implementer_EI.setImpName(impName);

	while (oiHandler.addClassImpl(&implementer_EI, nameClass) != ACS_CC_SUCCESS)
	{
		max_retry --;
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Registered implementer on object of class ExclusionItem failed"));
		if (max_retry == 0)  return ACS_ALOG_error;

		usleep(200000);
	}

	th3.start();
	th3.activate();

	ACSALOGLOG(LOG_LEVEL_WARN,TEXTERROR("Implementer on class ExclusionItem  registered"));

	return ACS_ALOG_ok;
}


int  main (int argc, char **argv)
{
	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

	system ("/bin/rm /tmp/paudit.*");
	system ("/bin/rm /tmp/PLOGFILE_*");

        if (parse_command_line(argc, argv) < 0)
        {
                fprintf(stderr, "USAGE: acs_alogmaind [--noha] \n");
                return  ACS_APGCC_FAILURE;
        }

        /* Create a new SID */
        sid = ACE_OS::setsid();
        if (sid < 0) {
                        syslog(LOG_ERR,"unable to create a new session! Failed - %s ",strerror(errno));
        }

        // if the alog Service has been launched with "--noha" option, we must avoid registration with AMF
        if(nohaflag)
        {
	        syslog(LOG_INFO, "Starting acs_alogmaind service.. ");
                return (long)run_alog_daemon_NoHA(0);
        }

         /*
         *  instantiate the Application Manager object that must be used to integrate alog Service with HA framework.
         *  This instantiation daemonizes alog Service application.
         */
        haObj = new (std::nothrow) ACS_ALOG_HA_AppManager("acs_alogmaind");

        if(!haObj)
        {
                syslog(LOG_ERR, "acs_alogmaind, Application Manager object creation failed !");
                delete haObj;
                return ACS_APGCC_FAILURE;
        }

        syslog(LOG_INFO, "Starting acs_alogmaind service.. ");

        acs_aeh_setSignalExceptionHandler ("acs_alogmaind");

        /**
         * initialize alog daemon towards AMF and start work (it's a blocking call). In this method the following actions are essentially executed:
         *  1- initialization of alog Daemon towards AMF;
         *  2- an event loop is performed, waiting for command events from AMF and invoking appropriate callbacks to handle them. For example,
         *  when AMF assigns ACTIVE state to the  alog service instance, the acs_alog_main::performStateTransitionToActiveJobs() method is invoked
         *  to handle this command event.
         */
        errorCode = haObj->activate();

        if (errorCode == ACS_APGCC_HA_FAILURE)
        {
                syslog(LOG_ERR, "acs_alogmaind, HA Activation Failed!!");
                delete haObj;
                return ACS_APGCC_FAILURE;
        }

        if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
        {
                syslog(LOG_ERR, "acs_alogmaind, HA Application Failed to Gracefully closed!!");
                if (haObj != NULL)  delete haObj;
                return ACS_APGCC_FAILURE;
        }

        if (errorCode == ACS_APGCC_HA_SUCCESS)
        {
                syslog(LOG_INFO, "acs_alogmaind, HA Application Gracefully closing ... waiting for Application Thread termination !");

                if (haObj != NULL)
                {
//						ACE_thread_t  haApplThreadId = haObj->get_application_thread_handle();

/* ----  HP98075  ---- */
//						if (haApplThreadId != 0)
//								ACE_Thread_Manager::instance()->join(haApplThreadId);
//								ACE_Thread::cancel (haApplThreadId);

						delete haObj;
                }
                syslog(LOG_INFO, "acs_alogmaind, HA Application Gracefully closed!!");
                return ACS_APGCC_FAILURE;
        }

        delete haObj;
        return ACS_APGCC_SUCCESS;
}
