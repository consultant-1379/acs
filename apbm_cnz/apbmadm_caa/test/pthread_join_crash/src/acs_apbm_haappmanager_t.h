#ifndef HEADER_GUARD_CLASS__acs_apbm_haappmanager
#define HEADER_GUARD_CLASS__acs_apbm_haappmanager acs_apbm_haappmanager


#include "ACS_APGCC_ApplicationManager.h"

#include "acs_apbm_serverwork_t.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_haappmanager


class __CLASS_NAME__ : public ACS_APGCC_ApplicationManager {
	//==============//
	// Constructors //
	//==============//
public:
	inline explicit __CLASS_NAME__ (acs_apbm_serverwork * srv_work)
	: ACS_APGCC_ApplicationManager(), _srv_work_ptr(srv_work) {}

	inline __CLASS_NAME__ (acs_apbm_serverwork * srv_work, const char * daemon_name)
	: ACS_APGCC_ApplicationManager(daemon_name), _srv_work_ptr(srv_work) {}

	inline __CLASS_NAME__ (acs_apbm_serverwork * srv_work, const char * daemon_name, const char * username)
	: ACS_APGCC_ApplicationManager(daemon_name, username), _srv_work_ptr(srv_work) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	/*
	 * BEGIN: ACS_APGCC_ApplicationManager interface implementation
	 */
	virtual ACS_APGCC_ReturnType performStateTransitionToActiveJobs (ACS_APGCC_AMF_HA_StateT state);
	virtual ACS_APGCC_ReturnType performStateTransitionToPassiveJobs (ACS_APGCC_AMF_HA_StateT state);
	virtual ACS_APGCC_ReturnType performStateTransitionToQueisingJobs (ACS_APGCC_AMF_HA_StateT state);
	virtual ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs (ACS_APGCC_AMF_HA_StateT state);
	virtual ACS_APGCC_ReturnType performComponentHealthCheck ();
	virtual ACS_APGCC_ReturnType performComponentTerminateJobs ();
	virtual ACS_APGCC_ReturnType performComponentRemoveJobs ();
	virtual ACS_APGCC_ReturnType performApplicationShutdownJobs ();
	/*
	 * END: ACS_APGCC_ApplicationManager interface implementation
	 */

private:
	void send_stop_work_operation_to_exit_program ();
//	void send_start_work_operation_by_node_state ();


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
public:
	acs_apbm_serverwork * _srv_work_ptr;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_haappmanager
