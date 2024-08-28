
#ifndef APG_APP1_CLASS_H
#define APG_APP1_CLASS_H

#include "ACS_APGCC_ApplicationManager.h"

class myAPGApp1Application : public ACS_APGCC_ApplicationManager 
{

   private:
   public:

	ACS_APGCC_ReturnType myAppInitialize(void); 
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
}; 

#endif /* APG_APP1_CLASS_H */

