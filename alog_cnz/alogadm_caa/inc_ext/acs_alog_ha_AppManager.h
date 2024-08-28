/** @file ACS_ALOG_HA_AppManager.h
 *	@brief
 *	@author xgencol (Gennaro Colantuono)
 *	@date 2011-08-02
 *	@version 1.0
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
 *	| 0.1    | 2011-08-02 | xgencol      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#ifndef ACS_ALOG_HA_APPMANAGER_H_

#define ACS_ALOG_HA_APPMANAGER_H_

#include <ACS_APGCC_ApplicationManager.h>
#include <ACS_APGCC_CommonLib.h>
#include <syslog.h>
#include <acs_alog_main.h>
//Start BRFC in ALOG
#include <acs_alog_brfc_interface.h>
//End BRFC in ALOG
/** @class ACS_ALOG_HA_AppManager ACS_ALOG_HA_AppManager.h
 *	@brief ACS_ALOG_HA_AppManager class
 *	@author xgencol (Gennaro Colantuono)
 *	@date 2011-08-02
 *	@version 0.1
 *
 *  ACS_ALOG_HA_AppManager class detailed description
 *
 */
class ACS_ALOG_HA_AppManager: public ACS_APGCC_ApplicationManager
{
   private:
	int readWritePipe[2];
//	ACS_APGCC_BOOL Is_terminated;
	ACE_thread_t alog_worker_thread_id;
	ACE_thread_t alog_command_thread_id;
//	ACE_thread_t alog_ha_application_thread_id;


   public:
	ACS_ALOG_HA_AppManager(const char* daemon_name);
	~ACS_ALOG_HA_AppManager(){};

	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

//	ACS_APGCC_ReturnType svc(void);
//	ACE_thread_t get_application_thread_handle() { return alog_ha_application_thread_id; }

  	int   createThreads();
  	void  stopActivities();
  	void  createBrfcThread();
  	int   createParticipant(int * apgcc_error_code_p = 0, char * error_message_buf = 0, size_t error_message_buf_size = 0);
  	int   deleteParticipant(void);
		
	private:
		void initCapability();
};

#endif

