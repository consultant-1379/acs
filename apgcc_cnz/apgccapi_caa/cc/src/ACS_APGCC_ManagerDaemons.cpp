#include "ACS_APGCC_ManagerDaemons.h"
#include "ace/OS.h"
static void parent_handler(int signum)
{
    switch(signum) {
    case SIGALRM: ACE_OS::exit(EXIT_FAILURE); break;
    case SIGUSR1: ACE_OS::exit(EXIT_SUCCESS); break;
    case SIGCHLD: ACE_OS::exit(EXIT_FAILURE); break;
    }
}

static void child_handler(int signum){
	if(signum == SIGUSR2)
	{
		ACE_OS::close( STDIN_FILENO );
		ACE_OS::close( STDOUT_FILENO );
		ACE_OS::close( STDERR_FILENO );
	}
}


 void ACS_APGCC_ManagerDaemons::daemonize(const char* user_name, ACS_TRA_trace* log_trace)
{
	//define the constants
	const char* CWD_DIR = "/";
	const mode_t FILE_MASK = 002;

    pid_t pid, sid, parent;

    /* already a daemon */
    if ( ACE_OS::getppid() == 1 ) return;

    /* Implement some locking mechanism */

    /* Drop user if there is one, and we were run as root */
    if(user_name){
    	if ( ACE_OS::getuid() == 0 || ACE_OS::geteuid() == 0 ) {
			struct passwd *pw = ACE_OS::getpwnam(user_name);
			if ( pw ) {
				if (log_trace->ACS_TRA_ON()) {
					char trace[400];
					ACE_OS::sprintf(trace,"setting user to %s", user_name);
					log_trace->ACS_TRA_event(1, trace);
				}
				ACE_OS::setuid( pw->pw_uid );
			}
		}
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
    		if (log_trace->ACS_TRA_ON()) {
    		        char trace[400];
    		        ACE_OS::sprintf(trace, "unable to fork daemon: Resource temporarily unavailable, code=%d", errno);
    		        log_trace->ACS_TRA_event(1, trace);
    		}
    	}
    	else
    	{
			if(errno == ENOMEM)
			{
				// manage the error
				if (log_trace->ACS_TRA_ON())
				{
					   char trace[400];
					   ACE_OS::sprintf(trace, "unable to fork daemon: Not enough space, code=%d", errno);
					   log_trace->ACS_TRA_event(1, trace);
				}
			}else{
				if (log_trace->ACS_TRA_ON())
				{
							char trace[400];
							ACE_OS::sprintf(trace, "unable to fork daemon, code=%d (%s)",
									errno, ACE_OS::strerror(errno));
							log_trace->ACS_TRA_event(1, trace);
				}
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
    	if (log_trace->ACS_TRA_ON()) {
            char trace[400];
            ACE_OS::sprintf(trace, "unable to create a new session, code %d (%s)",
                        	errno, ACE_OS::strerror(errno));
            log_trace->ACS_TRA_event(1, trace);
        }
    	ACE_OS::exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((ACE_OS::chdir(CWD_DIR)) < 0) {
    	if (log_trace->ACS_TRA_ON()) {
            char trace[400];
            ACE_OS::sprintf(trace, "unable to change directory to %s, code %d (%s)",
                   "/", errno, ACE_OS::strerror(errno));
            log_trace->ACS_TRA_event(1, trace);
        }
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
}
