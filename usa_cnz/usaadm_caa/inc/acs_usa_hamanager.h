#ifndef ACS_USA_HAMANAGER_H
#define ACS_USA_HAMANAGER_H

#include "ACS_APGCC_ApplicationManager.h"
#include "unistd.h"
#include "syslog.h"
#include "acs_usa_adm.h"
#include "ACS_TRA_Logging.h"

class usaHAClass: public ACS_APGCC_ApplicationManager {

   private:

	ACS_USA_Adm *m_usaObj;
	ACS_APGCC_ReturnType activateApp();
	ACS_APGCC_ReturnType passifyApp();

   public:
	usaHAClass(const char* daemon_name, const char* user);
	~usaHAClass();
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

#endif /* ACS_USA_HAMANAGER_H */
