/*=================================================================== */
   /**
   @file acs_aca_haservice.h

   Header file for ACS_ACA_HAService class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/09/2011     XTANAGG   Initial Revision for APG43L.
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#ifndef ACS_ACA_HAService_H_
#define ACS_ACA_HAService_H_

#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_ApplicationManager.h"
#include "acs_aca_msd_service.h"

/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class ACS_ACA_MSD_HAService : public ACS_APGCC_ApplicationManager {

public:
	//==============//
	// Constructors //
	//==============//
	ACS_ACA_MSD_HAService (const char * daemon_name, const char * username);

	//============//
	// Destructor //
	//============//
	virtual ~ACS_ACA_MSD_HAService ();

	//===========//
	// Functions //
	//===========//
	virtual ACS_APGCC_ReturnType performStateTransitionToActiveJobs (ACS_APGCC_AMF_HA_StateT previousHAState);
	virtual ACS_APGCC_ReturnType performStateTransitionToPassiveJobs (ACS_APGCC_AMF_HA_StateT previousHAState);
	virtual ACS_APGCC_ReturnType performStateTransitionToQueisingJobs (ACS_APGCC_AMF_HA_StateT previousHAState);
	virtual ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs (ACS_APGCC_AMF_HA_StateT previousHAState);
	virtual ACS_APGCC_ReturnType performComponentHealthCheck ();
	virtual ACS_APGCC_ReturnType performComponentTerminateJobs ();
	virtual ACS_APGCC_ReturnType performComponentRemoveJobs ();
	virtual ACS_APGCC_ReturnType performApplicationShutdownJobs ();

private:
	ACS_APGCC_ReturnType activateApp ();
	ACS_APGCC_ReturnType passifyApp ();

	//========//
	// Fields //
	//========//
private:
	ACS_MSD_Service * _root_worker_thread;
};
#endif /* ACS_ACA_HAService_H_ */
