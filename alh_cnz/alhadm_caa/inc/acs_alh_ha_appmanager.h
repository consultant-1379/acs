/*
 * acs_alh_ha_appmanager.h
 *
 *  Created on: Nov 17, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_HA_APPMANAGER_H_
#define ACS_ALH_HA_APPMANAGER_H_

/** @file acs_alh_ha_appmanager.h
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2012-11-17
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
 *	| 0.1    | 2011-11-17 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */


#include "ACS_APGCC_ApplicationManager.h"
#include "unistd.h"
#include "syslog.h"
#include "ace/Task.h"
#include "ace/OS_NS_poll.h"


/** @class acs_alh_ha_appmanager acs_alh_ha_appmanager.h
 *	@brief acs_alh_ha_appmanager class
 *	@author efabron (Fabio Ronca)
 *	@date 2011-11-17
 *	@version 0.1
 *
 *  acs_alh_ha_appmanager class detailed description
 *
 */
class acs_alh_ha_appmanager : public ACS_APGCC_ApplicationManager
{

public:

	acs_alh_ha_appmanager(const char* daemon_name);
	~acs_alh_ha_appmanager(){};

	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

	ACS_APGCC_ReturnType svc(void);

	ACE_thread_t get_application_thread_handle() { return alh_ha_application_thread_id; }

	void notify_worker_thread_termination(int exit_code) { alh_worker_thread_terminated = true;  alh_worker_thread_exit_code = exit_code; }

	int wait_ha_application_thread_termination(bool nolog = true);

private:
	int readWritePipe[2];
	ACS_APGCC_BOOL Is_terminated;
	ACE_thread_t alh_worker_thread_id;
	ACE_thread_t alh_ha_application_thread_id;
	bool alh_worker_thread_terminated;
	int alh_worker_thread_exit_code;

};


#endif /* ACS_ALH_HA_APPMANAGER_H_ */
