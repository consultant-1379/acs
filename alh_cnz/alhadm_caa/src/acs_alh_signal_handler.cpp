/*
 * acs_alh_signal_handler.cpp
 *
 *  Created on: Dec 5, 2011
 *      Author: efabron
 */


#include "acs_alh_signal_handler.h"
//#include "acs_alh_ha_appmanager.h"
//#include "acs_alh_manager.h"

//extern acs_alh_manager * manager;
//extern acs_alh_ha_appmanager * haObj;	// defined in acs_alh_main.cpp
extern int nohaflag;					// defined in acs_alh_main.cpp
extern void stop_alh_daemon_work();		// defined in acs_alh_main.cpp

acs_alh_signal_handler::acs_alh_signal_handler() : log_(){

}


int acs_alh_signal_handler::handle_signal (int signum, siginfo_t *,ucontext_t *)
{

	log_.write(LOG_LEVEL_DEBUG, "acs_alh_signal_handler : received signal < %u > !", signum);
	switch(signum)
	{
	case SIGPIPE:
		break;
	case SIGINT:
	case SIGTERM:
		if(nohaflag)
			stop_alh_daemon_work();
		else
		{
			log_.write(LOG_LEVEL_DEBUG, "acs_alh_signal_handler : the SIGTERM signal will be ignored ! You must use 'immadm' commands to terminate a ALH Daemon launched without '--noha' option !", signum);
			//kill(ACE_OS::getpid(),SIGUSR2);
			//manager->set_event_reader_thread_failed_flag(true);
		}
		break;
	};
	return 0;
}
