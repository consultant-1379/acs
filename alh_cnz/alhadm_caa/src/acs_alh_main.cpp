/*
 * acs_alh_main.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: efabron
 */


#include "ace/Condition_T.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <getopt.h>
#include <sys/file.h>

#include "ace/Signal.h"
#include "ace/Sig_Handler.h"

#include <iostream>
#include "acs_alh_eventmgr.h"
#include "acs_alh_ahtrecord.h"
#include "acs_alh_util.h"
#include "acs_alh_ahtmgr.h"
#include "acs_alh_signal_handler.h"


#include "acs_alh_imm_runtime_owner.h"
#include "acs_alh_imm_data_handler.h"
#include "acs_alh_ha_appmanager.h"


#include "acs_alh_manager.h"
#include "acs_alh_cpnotifier.h"
#include "acs_alh_log.h"

using namespace std;

static int siglist[] = {SIGINT, SIGTERM, SIGPIPE };

// Global objects
acs_alh_ha_appmanager *haObj = 0;									// used to integrate ALH Daemon with HA framework
acs_alh_imm_connection_handler *immConnHandlerObj = 0;
acs_alh_manager *manager;

ACE_Thread_Mutex alh_stop_mutex;									// mutex used to stop ALH Daemon
ACE_Condition<ACE_Thread_Mutex> *p_alh_stop_condition = 0;			// condition used to stop ALH Daemon

ACE_Thread_Mutex alh_manager_stop_mutex;
ACE_Condition<ACE_Thread_Mutex> *p_alh_stop_manager_condition = 0;

bool alh_is_stopping = false;										// indicates if the ALH Daemon has entered the "stopping" phase
// ... global objects used to represents command-line options
int cflag = 0;
int nohaflag = 0;

ACE_THR_FUNC_RETURN run_alh_daemon_work(void *);
void stop_alh_daemon_work();
int parse_command_line(int argc, char **argv);
void cleanup();


int main(int argc, char **argv)
{
	ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

	// parse command line
	if(parse_command_line(argc, argv) < 0)
	{
		fprintf(stderr, "USAGE: acs_alhd [-c] [--noha] \n");
		return  2;
	}


	// if the alhd has been launched with "--noha" option, we must avoid registration with AMF
	if(nohaflag)
		return (long)run_alh_daemon_work(0);


	if(cflag)
		printf("alh_d invocated with option [-c]\n");


	/**
	 *  instantiate the Application Manager object that must be used to integrate ALH Server with HA framework.
	 *  This instantiation daemonizes ALH Server application.
	 */
	haObj = new (std::nothrow) acs_alh_ha_appmanager(ACS_ALH_DAEMON_NAME);
	if(!haObj)
	{
		syslog(LOG_ERR, "acs_alhd, Application Manager object creation failed !");
		delete haObj;
		return ACS_APGCC_FAILURE;
	}

	syslog(LOG_INFO, "Starting acs_alhd service.. ");

	/**
	 * initialize ALH daemon towards AMF and start work (it's a blocking call). In this method the following actions are essentially executed:
	 * 	1- initialization of ALH Daemon towards AMF;
	 *  2- an event loop is performed, waiting for command events from AMF and invoking appropriate callbacks to handle them. For example,
	 *  when AMF assigns ACTIVE state to the ALH Server service instance, the acs_alh_ha_appmanager::performStateTransitionToActiveJobs() method is invoked
	 *  to handle this command event.
	 */
	errorCode = haObj->activate();

	if (errorCode == ACS_APGCC_HA_FAILURE)
	{
		syslog(LOG_ERR, "acs_alhd, HA Activation Failed!!");
		delete haObj;
		return ACS_APGCC_FAILURE;
	}

	if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
	{
		syslog(LOG_ERR, "acs_alhd, HA Application Failed to Gracefully closed!!");
		delete haObj;
		return ACS_APGCC_FAILURE;
	}

	if (errorCode == ACS_APGCC_HA_SUCCESS)
	{
		syslog(LOG_INFO, "acs_alhd, HA Application Gracefully closing ... waiting for Application Thread termination !");

		// wait for HA Application Thread termination
		ACE_Thread_Manager::instance()->join(haObj->get_application_thread_handle());

		syslog(LOG_INFO, "acs_alhd, HA Application Gracefully closed!!");
		delete haObj;
		return ACS_APGCC_FAILURE;
	}

	delete haObj;
	return ACS_APGCC_SUCCESS;
}

/**
 * This function represents the application logic of ALH Daemon. If the ALH Daemon is launched by AMF, this function is executed by the ALH Worker Thread,
 * that is in turn instantiated by the HA Application Thread when AMF assigns for the first time ACTIVE or PASSIVE state to the ALH daemon application.
 */
ACE_THR_FUNC_RETURN run_alh_daemon_work(void *){

	int exit_code = 0;

	// check if there's another acs_alhd instance running and, if so, exit function

	acs_alh_log log;

	int fdlock = open(ALHD_LOCKFILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0664);

	if(fdlock < 0)
	{
		exit_code = -2;
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);		// notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	if(flock(fdlock, LOCK_EX | LOCK_NB) < 0)
	{
		if(errno == EWOULDBLOCK)
		{
			fprintf(stderr, "another acs_alhd instance running\n");
			exit_code = 1;
		}
		else
			exit_code = -3;

		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);		// notify worker thread termination to the HA Application thread

		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	// create ACE_Condition object to be used to stop ALH Daemon
	p_alh_stop_condition = new (std::nothrow) ACE_Condition<ACE_Thread_Mutex>(alh_stop_mutex);
	if(!p_alh_stop_condition)
	{
		exit_code = -1;
		log.write(LOG_LEVEL_ERROR, "Memory error allocating object : ALH STOP CONDITION ! Exiting with code < %d >", exit_code);
		//cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	log.write(LOG_LEVEL_WARN, "p_alh_stop_condition created successfully");
	// create ACE_Condition object to be used to stop ALH manager Thread
	p_alh_stop_manager_condition = new (std::nothrow) ACE_Condition<ACE_Thread_Mutex>(alh_manager_stop_mutex);
	if(!p_alh_stop_manager_condition)
	{
		exit_code = -1;
		log.write(LOG_LEVEL_ERROR, "Memory error allocating object : ALH STOP CONDITION ! Exiting with code < %d >", exit_code);
		//cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	log.write(LOG_LEVEL_WARN, "p_alh_stop_manager_condition created successfully");

	immConnHandlerObj = new (std::nothrow) acs_alh_imm_connection_handler;
	if(!immConnHandlerObj)
	{
		exit_code = -1;
		log.write(LOG_LEVEL_ERROR, "Memory error allocating object : acs_alh_imm_connection_handler! Exiting with code < %d >", exit_code);
		//cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	log.write(LOG_LEVEL_WARN, "ALH Daemon started (PID = %d)", ACE_OS::getpid());

	acs_alh_imm_runtime_owner OI(immConnHandlerObj);

	manager = new (std::nothrow) acs_alh_manager(immConnHandlerObj, p_alh_stop_manager_condition);
	if(!manager)
	{
		exit_code = -1;
		log.write(LOG_LEVEL_ERROR, "Memory error allocating object : ALH STOP CONDITION ! Exiting with code < %d >", exit_code);
		//cleanup();
		if(!nohaflag)
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
		return (ACE_THR_FUNC_RETURN) exit_code;
	}

	log.write(LOG_LEVEL_WARN, "acs_alh_manager created successfully");

	ACE_Sig_Handler ace_sigdispatcher;

	acs_alh_signal_handler alhSignalHandler;

	int numsigs = sizeof ( siglist ) / sizeof(int);
	for ( int i=0 ; i < numsigs ; ++i )
		ace_sigdispatcher.register_handler(siglist[i], &alhSignalHandler);

	//Check if the node is rebooted
	bool reboot = manager->isApReboot();

	if(cflag)
		reboot=cflag;

	int init_ret_code=0;
	// register with IMM and create an AlarmListNode object for the AP node we're running on
	do
	{
		init_ret_code = OI.init_IMM(reboot);
		if(init_ret_code < 0)
		{
			sleep(1);
			OI.finalize_IMM();
			log.write(LOG_LEVEL_ERROR, "init_IMM() failed ! ret_code < %d >", init_ret_code);
		}
	}while((init_ret_code != 0) && (!alh_is_stopping));

	if(alh_is_stopping)
	{
		if(unlink(ALHD_LOCKFILE_PATH) < 0)
		 log.write(LOG_LEVEL_DEBUG, "Error deleting ALH Daemon lock file < %s >. Error description: < %s >", ALHD_LOCKFILE_PATH, strerror(errno));

		sleep(1);

		if(OI.finalize_IMM()< 0)
		 log.write(LOG_LEVEL_ERROR, "finalize_IMM() failed1 !");

		cleanup();

		if(!nohaflag) {
			haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread
			//haObj->componentReportError ( ACS_APGCC_COMPONENT_RESTART ); // force AMF to restar the application
		}
		log.write(LOG_LEVEL_ERROR, "init_IMM() failed ! Exiting with code < %d >", exit_code);
        return (ACE_THR_FUNC_RETURN) exit_code;
	}

	log.write(LOG_LEVEL_WARN, "IMM initialization OK");

	manager->storeTimestampInformation();

	manager->start_activity();

	// wait for stop command

	if ( alh_stop_mutex.acquire() == -1 )
		log.write(LOG_LEVEL_ERROR, "alh_stop_mutex acquire failed !");

	while ( !alh_is_stopping ){

		log.write(LOG_LEVEL_ERROR, "Waiting for signal on condition p_alh_stop_condition");

		if(p_alh_stop_condition->wait() == -1)
			log.write(LOG_LEVEL_ERROR, "p_alh_stop_condition wait failed !");
	}

	if ( alh_stop_mutex.release() == -1 )
		log.write(LOG_LEVEL_ERROR, "alh_stop_mutex released failed !");

	manager->stop_activity();

	if ( manager->is_working() ){
		log.write(LOG_LEVEL_TRACE, "Waiting for ALH_Manager_Thread termination");
		syslog(LOG_INFO, "Waiting for ALH_Manager_Thread termination");
		p_alh_stop_manager_condition->wait();
		sleep(1); //ATTEND termination of manager thread
		log.write(LOG_LEVEL_TRACE, "ALH_Manager_Thread terminated");
		syslog(LOG_INFO, "ALH_Manager_Thread terminated");
	}

	if(OI.finalize_IMM()< 0)
		log.write(LOG_LEVEL_ERROR, "finalize_IMM() failed !");

	cleanup();

	log.write(LOG_LEVEL_WARN, "IMM finalization OK and cleanup phase executed");

	if(unlink(ALHD_LOCKFILE_PATH) < 0)
		log.write(LOG_LEVEL_DEBUG, "Error deleting ALH Daemon lock file < %s >. Error description: < %s >", ALHD_LOCKFILE_PATH, strerror(errno));

	log.write(LOG_LEVEL_TRACE, "ALH Daemon exiting");
	syslog(LOG_INFO, "ALH Daemon exiting");

	if(!nohaflag)
		haObj->notify_worker_thread_termination(exit_code);  // notify worker thread termination to the HA Application thread

	return (ACE_THR_FUNC_RETURN) exit_code;
}



void stop_alh_daemon_work(){

	acs_alh_log log;

	log.write(LOG_LEVEL_TRACE, "stop_alh_daemon_work - Start");

	if ( alh_stop_mutex.acquire() == -1 )
		syslog(LOG_INFO, "stop_alh_daemon_work - alh_stop_mutex acquire failed !");

	log.write(LOG_LEVEL_TRACE, "stop_alh_daemon_work - alh_stop_mutex acquired");

	alh_is_stopping = true;

	if ( alh_stop_mutex.release() == -1 )
		syslog(LOG_INFO, "stop_alh_daemon_work - alh_stop_mutex release failed !");

	log.write(LOG_LEVEL_TRACE, "stop_alh_daemon_work - alh_stop_mutex released");

	if ( p_alh_stop_condition ){
		syslog(LOG_INFO, "stop_alh_daemon_work - signal sends to p_alh_stop_condition");
		log.write(LOG_LEVEL_TRACE, "stop_alh_daemon_work - signal sends to p_alh_stop_condition");
		p_alh_stop_condition->signal();
	}
	else {
		syslog(LOG_INFO, "stop_alh_daemon_work - p_alh_stop_condition is NULL");
		log.write(LOG_LEVEL_TRACE, "stop_alh_daemon_work - p_alh_stop_condition is NULL");
	}
	log.write(LOG_LEVEL_TRACE, "alh_is_stopping is %d",alh_is_stopping);
	log.write(LOG_LEVEL_TRACE, "stop_alh_daemon_work - End");
}


void cleanup()
{
	// cleanup. The order is important !
	if(manager)
	{
	   delete manager;
	   manager = NULL;
	}
	if(immConnHandlerObj)
	{
	   delete immConnHandlerObj;
	   immConnHandlerObj=NULL;
	}
	if(p_alh_stop_condition)
	{
	   delete p_alh_stop_condition;
	   p_alh_stop_condition = NULL;
	}
	if(p_alh_stop_manager_condition)
	{
	   delete p_alh_stop_manager_condition;
	   p_alh_stop_manager_condition = NULL;
	}
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
			   fprintf(stderr,"acs_alhd: Unrecognized option '%s'\n",argv[optind-1]);
			   return -1;
		   }

		   /* found --noha option */
		   if(noha_opt_cnt > 0)
		   {
			   fprintf(stderr,"acs_alhd: duplicated long option 'noha'\n");
			   return -1;
		   }
		   ++noha_opt_cnt;
		   break;
	   case 'c':
		   if(cflag)
		   {
			   fprintf(stderr,"acs_alhd: duplicated option 'c'\n");
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
    * We must reject the command line  "acs_alhd --".
    * Note that if "--" command line argument is found, getopt_long() increments <optind> variable and returns -1.
   */
   if((optind > 1) && !strcmp(argv[optind-1],"--"))
   {
	   fprintf(stderr,"acs_alhd: Unrecognized option '%s'\n",argv[optind-1]);
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
