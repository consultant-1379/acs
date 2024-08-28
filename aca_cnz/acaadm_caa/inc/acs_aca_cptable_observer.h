#ifndef HEADER_GUARD_CLASS__ACS_ACA_CPTable_Observer
#define HEADER_GUARD_CLASS__ACS_ACA_CPTable_Observer ACS_ACA_CPTable_Observer

/** @file acs_aca_cptable_observer.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-01-27
 *
 *	COPYRIGHT Ericsson AB, 2014
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
 *	| R-001 | 2014-01-27 | xcasale      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_CS_API.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_ACA_CPTable_Observer

/** @class ACS_ACA_CPTable_Observer acs_aca_cptable_observer.h
 *	@brief ACS_ACA_CPTable_Observer class
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-01-27
 *
 *	ACS_ACA_CPTable_Observer <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACS_CS_API_CpTableObserver {

	//==============//
	// Enumerations //
	//==============//
public:
	typedef enum ObserverStateConstants {
		OBSERVER_STATE_STOPPED = 0,
		OBSERVER_STATE_STARTED = 1
	} observer_state_t;

	enum CPTableObserverErrorConstants {
			ERROR_NO_ERROR = 0,
			ERROR_ALREADY_CONNECTED,
			ERROR_ALREADY_DISCONNECTED,
			ERROR_CS_GET_INSTANCE,
			ERROR_CS_SUBSCRIPTION_FAILED,
			ERROR_CS_UNSUBSCRIPTION_FAILED
	};

	enum CPTableOperations {
		BLADE_ADD,
		BLADE_REMOVE,
		BLADE_CHANGE
	};

	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__ () : _state(OBSERVER_STATE_STOPPED), _sync_obj() {};

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
	virtual void update (const ACS_CS_API_CpTableChange & observee);
	int start (ACS_CS_API_SubscriptionMgr * subscription_manager = 0);
	int stop (ACS_CS_API_SubscriptionMgr * subscription_manager = 0);

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	observer_state_t _state;
	ACE_Recursive_Thread_Mutex _sync_obj;
};

#endif // HEADER_GUARD_CLASS__ACS_ACA_CPTable_Observer
