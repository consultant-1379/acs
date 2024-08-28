//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************

#ifndef APG_APP1_CLASS_H
#define APG_APP1_CLASS_H

#include "ACS_APGCC_ApplicationManager.h"
#include "unistd.h"
//#include "syslog.h"
#include "ace/Task.h"
#include "ace/OS_NS_poll.h"

#include "ACS_CS_ServiceHandler.h"

class HAClass: public ACS_APGCC_ApplicationManager {

private:

	int readWritePipe[2];
	ACS_APGCC_BOOL Is_terminated;

	ACE_thread_t cs_ha_application_thread_id;

public:
	HAClass(const char* daemon_name, const char* username);
	~HAClass() {};

	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	ACS_APGCC_ReturnType performComponentRemoveJobs(void);
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);
	int wait_ha_application_thread_termination();		// TR HX28206


	ACS_APGCC_ReturnType svc(void);

	ACE_thread_t get_application_thread_handle() { return cs_ha_application_thread_id; }

	ACS_CS_ServiceHandler * csService;
};

#endif /* APG_APP1_CLASS_H */

