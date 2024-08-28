#ifndef HEADER_GUARD_CLASS__acs_apbm_haappmanager
#define HEADER_GUARD_CLASS__acs_apbm_haappmanager acs_apbm_haappmanager

/** @file acs_apbm_haappmanager.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-11-28
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
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2011-11-28 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ACS_APGCC_ApplicationManager.h>


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_haappmanager


/** @class acs_apbm_haappmanager acs_apbm_haappmanager.h
 *	@brief acs_apbm_haappmanager class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-11-28
 *
 *	acs_apbm_haappmanager <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACS_APGCC_ApplicationManager {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_haappmanager constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	: ACS_APGCC_ApplicationManager(), _server_working_set(server_working_set)
	{}

	inline __CLASS_NAME__ (const char * daemon_name, acs_apbm_serverworkingset * server_working_set)
	: ACS_APGCC_ApplicationManager(daemon_name), _server_working_set(server_working_set)
	{}

	inline __CLASS_NAME__ (const char * daemon_name, const char * username, acs_apbm_serverworkingset * server_working_set)
	: ACS_APGCC_ApplicationManager(daemon_name, username), _server_working_set(server_working_set)
	{}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_haappmanager Destructor
	 */
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
	void send_start_work_operation_by_node_state ();
	void send_stop_work_operation_to_exit_program ();
	void send_start_work_quiesced_operation ();


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
public:
	acs_apbm_serverworkingset * _server_working_set;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_haappmanager
