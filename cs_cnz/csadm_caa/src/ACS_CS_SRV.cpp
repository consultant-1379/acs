#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <limits.h>
#include <syslog.h>
//ACE stuff
#include "ace/Event.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_time.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"


#include "ACS_CS_Event.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_HAClass.h"
#include "ACS_CS_Registry.h"
ACS_CS_Trace_TDEF(ACS_CS_SRV_TRACE);

using namespace std;
//CS
#include "ACS_CS_ServiceHandler.h"

#define RUNNING_DIR	"/tmp"
#define LOCK_FILE	"cssrv.lock"

void signal_handler(int sig)
{
	ACS_CS_TRACE((ACS_CS_SRV_TRACE,
		"ACS_CS_SRV::signal_handler()"));

	switch(sig) {
	case SIGHUP:
		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_WARN, "ACS_CS_SRV::signal_handler() - hangup signal caught"));
		break;
	case SIGTERM:
		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_WARN, "ACS_CS_SRV::signal_handler() - terminate signal caught"));
		exit(0);
		break;
	case SIGCHLD:
		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO, "ACS_CS_SRV::signal_handler() - SIGCHLD caught"));
		break;
	}
}

void daemonize()
{
	ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO, "ACS_CS_SRV::daemonize()"));

	int i,lfp;
	char str[10] = { 0 };

	if(getppid()==1)
		return; /*  already a daemon */

	i=fork();

	if (i<0)
		exit(1); /*fork error */

	if (i>0)
		exit(0); /* parent exits */
	/* child (daemon) continues */

	setsid(); /* obtain a new process group */

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standard I/O */

	umask(027); /* set newly created file permissions */

	chdir(RUNNING_DIR); /* change running directory */

	lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);

	if (lfp<0)
		exit(1); /* can not open */

	if (lockf(lfp,F_TLOCK,0)<0) {
		exit(0); /* can not lock */
	}
	/* first instance continues */
	sprintf(str,"%d\n",getpid());

	write(lfp,str,strlen(str)); /* record pid to lockfile */
	signal(SIGCHLD,signal_handler); /* catch child signal*/
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,signal_handler); /* catch hangup signal */
	signal(SIGTERM,signal_handler); /* catch kill signal */
}


void CreateEvents()
{
	ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO, "ACS_CS_SRV::CreateEvents()"));

    (void) ACS_CS_Event::CreateEvent(true,
            false, ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);


    // Event that is signaled to shutdown the entire service
    (void) ACS_CS_Event::CreateEvent(true,
            false, ACS_CS_NS::EVENT_NAME_SHUTDOWN);
}

void RemoveEvents()
{
	ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO, "ACS_CS_SRV::RemoveEvents()"));

	ACS_CS_EventHandle handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);
	if (handle >= 0)
		(void) ACS_CS_Event::CloseEvent(handle);

    handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);
    if (handle >= 0)
    	(void) ACS_CS_Event::CloseEvent(handle);
}

void shutdown()
{
	ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO, "ACS_CS_SRV::shutdown()"));

	ACS_CS_EventHandle handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);
	if (handle >= 0)
		(void) ACS_CS_Event::SetEvent(handle);

	handle = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);
	if (handle >= 0)
		(void) ACS_CS_Event::SetEvent(handle);
}

bool noHA() {

   bool noHa = false;

   char * envVar = getenv("NO_HA");

   if (envVar != 0 && !strcmp(envVar, "1")) {
      noHa = true;
   }

   return noHa;
}

int main(int argc, char *argv[])
{
	ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO,
			"\n************************************\n**** ACS_CS_SRV::main() - ENTER ****\n************************************"));

	bool noHa = false;
	bool makeDaemon = true;
	int seconds = 0;

	char * envVar = getenv("NO_HA");

	if (envVar != 0 && !strcmp(envVar, "1")) {
	  noHa = true;
	}
	else if((argc == 2 && strcmp(argv[1], "--noha") == 0)){
	   noHa = true;
	}
	else if((argc == 2 && strcmp(argv[1], "--run-foreground") == 0)){
		   noHa = true;
		   makeDaemon = false;
	}
	else if((argc == 3 && strcmp(argv[1], "--run-foreground") == 0)){
		   noHa = true;
		   makeDaemon = false;
		   seconds = atol(argv[2]);
	}
	else if(argc > 1){
		fprintf(stderr, "This application should be started by HA command!\n   USAGE: acs_csd [--noha] \n");
		return  2;
	}

	ACS_CS_Registry::setDebug(!makeDaemon);

	CreateEvents();

	if ( noHa )
	{
		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO, "ACS_CS_SRV::main() - Starting CS WITHOUT HA"));

		if (makeDaemon) {
			daemonize();
		}

		ACS_CS_ServiceHandler theService;
		theService.activate();

		int counter = 0;

		while(seconds == 0 || counter < seconds) {
			sleep(5);
			counter += 5;
		}

		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO, "ACS_CS_SRV::main() - Starting shutdown"));

		shutdown();

		theService.wait(ACS_CS_NS::Daemon_Timeout);

		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO,
				"ACS_CS_SRV::main() - Finished shutdown, exiting\n"
				"###############################################"));

		RemoveEvents();
		return 0;
	}

//	** HA Implementation **


	ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO,	"ACS_CS_SRV::main() - Starting CS service HA way..."));
	syslog(LOG_INFO, "Starting acs_csd service... ");

	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

	HAClass *haObj = new HAClass(ACS_CS_NS::ACS_CS_DAEMON, ACS_CS_NS::ACS_CS_USER);

	errorCode = haObj->activate();

	if (errorCode == ACS_APGCC_HA_FAILURE) {
		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_FATAL,
				"ACS_CS_SRV::main() - HA Activation Failed!!!\n"
				"#############################################"));

		RemoveEvents();
		delete haObj;
		return ACS_APGCC_FAILURE;
	}

	if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE) {
		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_WARN,
				"ACS_CS_SRV::main() - HA Application Failed to Gracefully close!!!\n"
				"#################################################################"));
		RemoveEvents();
		delete haObj;
		return ACS_APGCC_FAILURE;
	}

	if (errorCode == ACS_APGCC_HA_SUCCESS) {

		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO,
				"ACS_CS_SRV::main() - HA Application Gracefully closed!!!\n"
				"########################################################"));

		syslog(LOG_ERR, "CS, HA Application Gracefully closed!!");

		ACE_Thread_Manager::instance()->join(haObj->get_application_thread_handle());

		ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO,
				"ACS_CS_SRV::main() - HA Application thread is finished!!!\n"
				"#########################################################"));

		syslog(LOG_ERR, "CS, HA Application - thread is finished!!");

		RemoveEvents();
		delete haObj;
		return ACS_APGCC_SUCCESS;
	}

	ACS_CS_FTRACE((ACS_CS_SRV_TRACE, LOG_LEVEL_INFO,
			"ACS_CS_SRV::main() - EXIT\n"
			"#########################"));

	RemoveEvents();
	delete haObj;
	return errorCode;

}
