#ifndef HEADER_GUARD_CLASS__acs_apbm_cshwctablechangeobserver
#define HEADER_GUARD_CLASS__acs_apbm_cshwctablechangeobserver acs_apbm_cshwctablechangeobserver

/** @file acs_apbm_cshwctablechangeobserver.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-02-08
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
 *	| R-001 | 2012-02-08 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ace/Recursive_Thread_Mutex.h>

#include <ACS_CS_API.h>
//#include "ACS_CS_API_Set.h"

//#include "acs_apbm_programconstants.h"


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_cshwctablechangeobserver


/** @class acs_apbm_cshwctablechangeobserver acs_apbm_cshwctablechangeobserver.h
 *	@brief acs_apbm_cshwctablechangeobserver class
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-02-08
 *
 *	acs_apbm_cshwctablechangeobserver <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACS_CS_API_HWCTableObserver {
	//=============//
	// Enumeration //
	//=============//
public:
	typedef enum ObserverStateConstants {
		OBSERVER_STATE_STOPPED = 0,
		OBSERVER_STATE_STARTED = 1
	} observer_state_t;


	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_cshwctablechangeobserver constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	: _server_working_set(server_working_set), _state(OBSERVER_STATE_STOPPED), _sync_object() {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_cshwctablechangeobserver Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	// BEGIN: ACS_CS_API_HWCTableObserver Interface
  virtual void update (const ACS_CS_API_HWCTableChange & observee);
	// END: ACS_CS_API_HWCTableObserver Interface

  int start (ACS_CS_API_SubscriptionMgr * subscription_manager = 0);

  int stop (ACS_CS_API_SubscriptionMgr * subscription_manager = 0);

private:
  const char * hwc_table_change_op_type_str (ACS_CS_API_TableChangeOperation::OpType op_type) const;


  //===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	acs_apbm_serverworkingset * _server_working_set;
	observer_state_t _state;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_cshwctablechangeobserver
