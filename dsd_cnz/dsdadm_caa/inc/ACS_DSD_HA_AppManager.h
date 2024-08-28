#ifndef ACS_DSD_HA_APPMANAGER_H_
#define ACS_DSD_HA_APPMANAGER_H_

/** @file ACS_DSD_HA_AppManager.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2011-05-09
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2010
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
 *	| 0.1    | 2010-05-09 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */


#include "ACS_APGCC_ApplicationManager.h"
#include "unistd.h"
#include "syslog.h"
#include "ace/Task.h"
#include "ace/OS_NS_poll.h"


/** @class ACS_DSD_HA_AppManager ACS_DSD_HA_AppManager.h
 *	@brief ACS_DSD_HA_AppManager class
 *	@author xludesi (Luca De Simone)
 *	@date 2011-05-16
 *	@version 0.1
 *
 *  ACS_DSD_HA_AppManager class detailed description
 *
 */
class ACS_DSD_HA_AppManager: public ACS_APGCC_ApplicationManager
{
	public:
		ACS_DSD_HA_AppManager(const char* daemon_name);
		~ACS_DSD_HA_AppManager(){};

		// callbacks executed when some communication has been received from AMF service
		ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
		ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
		ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
		ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
		ACS_APGCC_ReturnType performComponentHealthCheck(void);
		ACS_APGCC_ReturnType performComponentTerminateJobs(void);
		ACS_APGCC_ReturnType performComponentRemoveJobs (void);
		ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

		// thread function executed by HA Application Thread
		ACS_APGCC_ReturnType svc(void);

		ACE_thread_t get_ha_application_thread_handle() { return dsd_ha_application_thread_id; }
		ACE_thread_t get_worker_thread_handle() { return dsd_worker_thread_id; }

		// communicates to the HA Application Thread that the DSD root worker thread has terminated with the given exit code
		void notify_worker_thread_termination(int exit_code);

		// wait for termination of HA Application Thread identified by'dsd_ha_application_thread_id'
		int wait_ha_application_thread_termination(bool nolog = true);

	private:
		/*
		 * pipe used by 'ACS_DSD_HA_AppManager::perform*()' callbacks to send 'state assignment requests' ('A' for active,
		 * 'P' for passive, 'S for stop ) to the HA Application Thread (i.e the thread executing 'ACS_DSD_HA_AppManager::svc()'
		 */
		int readWritePipe[2];

		// attributes used to remember that the HA Application Thread has already received a request to go in 'stop' state
		ACS_APGCC_BOOL Is_terminated;

		ACE_thread_t dsd_worker_thread_id;			// ID of the root worker thread of DSD daemon (the thread executing 'run_dsd_daemon_work()'
		ACE_thread_t dsd_ha_application_thread_id;	// ID of the HA Application Thread

		bool dsd_worker_thread_terminated;			// used to indicate that root worker thread has terminated
		int dsd_worker_thread_exit_code;			// if 'dsd_worker_thread_terminated' is true, indicates the exit code of the root worker thread
};

#endif

