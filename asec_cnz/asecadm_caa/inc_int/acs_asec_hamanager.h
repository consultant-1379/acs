#ifndef ACS_ASEC_HAMANAGER_H
#define ACS_ASEC_HAMANAGER_H

#include <iostream>
#include "unistd.h"
#include "syslog.h"
#include "ACS_APGCC_ApplicationManager.h"
#include "ACS_APGCC_AmfTypes.h"
#include "acs_asec_adm.h"
#include "ACS_TRA_Logging.h"

using namespace std; 

class ACS_ASEC_Adm;
class asecHAClass:public ACS_APGCC_ApplicationManager {

   private:

	ACS_APGCC_ReturnType activateApp();
	ACS_APGCC_ReturnType passifyApp();
	ACS_APGCC_ReturnType shutdownApp();

	ACS_ASEC_Adm* m_asecObj; 
	//ACE_UINT32 passiveToActive;

   public:
	asecHAClass(const char* daemon_name, const char* user);
	~asecHAClass();
	bool active;
	ACE_UINT32 passiveToActive;
	ACS_TRA_Logging log;
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);
}; 

#endif /* ACS_ASEC_HAMANAGER_H */
