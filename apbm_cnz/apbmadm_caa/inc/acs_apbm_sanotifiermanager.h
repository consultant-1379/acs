#ifndef HEADER_GUARD_CLASS__acs_apbm_sanotifiermanager
#define HEADER_GUARD_CLASS__acs_apbm_sanotifiermanager acs_apbm_sanotifiermanager

/** @file acs_apbm_sanotifiermanager.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-12-02
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
 *	| R-001 | 2011-12-02 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ace/Recursive_Thread_Mutex.h>

#include <saAis.h>
#include <saAmf.h>
#include <saNtf.h>
//#include "saClm.h"


class acs_apbm_serverworkingset;


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_sanotifiermanager


/** @class acs_apbm_sanotifiermanager acs_apbm_sanotifiermanager.h
 *	@brief acs_apbm_sanotifiermanager class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-12-02
 *
 *	acs_apbm_sanotifiermanager <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==========//
	// Typedefs //
	//==========//
public:

	typedef enum internal_state_constants {
		ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED = 0,
		ACS_APBM_SA_NOTIFIER_HANDLER_STATE_OPEN = 1,
		ACS_APBM_SA_NOTIFIER_HANDLER_STATE_SUBSCRIBED = 2
	} internal_state_t;

	typedef struct active_notification_filter_t {
		inline active_notification_filter_t ()
		: all(false), state_change(false), object_create_delete(false), attribute_change(false), alarm(false), security_alarm(false) {}

		bool all;
		bool state_change;
		bool object_create_delete;
		bool attribute_change;
		bool alarm;
		bool security_alarm;
	} active_notification_filter_t;

	typedef struct notification_filter_allocation_params_t {
		inline notification_filter_allocation_params_t ()
		: numChangedStates(0), numEventTypes(0), numNotificationObjects(0), numNotificationClassIds(0), numNotifyingObjects(0),
		  numPerceivedSeverities(0), numProbableCauses(0), numSecurityAlarmDetectors(0), numServiceProviders(0), numServiceUsers(0),
		  numSeverities(0), numSourceIndicators(0), numTrends(0)
		{}

		SaUint16T numChangedStates;
		SaUint16T numEventTypes;
		SaUint16T numNotificationObjects;
		SaUint16T numNotificationClassIds;
		SaUint16T numNotifyingObjects;
		SaUint32T numPerceivedSeverities;
		SaUint32T numProbableCauses;
    SaUint16T numSecurityAlarmDetectors;
    SaUint16T numServiceProviders;
    SaUint16T numServiceUsers;
    SaUint16T numSeverities;
		SaUint16T numSourceIndicators;
		SaUint32T numTrends;
	} notification_filter_allocation_params_t;


	//==============//
	// Constructors //
	//==============//
private:
	/** @brief acs_apbm_sanotifiermanager constructor
	 */
	inline __CLASS_NAME__ () {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_sanotifiermanager Destructor
	 */
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	static int open (acs_apbm_serverworkingset * _server_working_set);

	static int close ();

	static int subscribe (active_notification_filter_t active_filter, const notification_filter_allocation_params_t * params);

	static int unsubscribe ();

	static inline int get_selection_handle () { return (_selection_object ? static_cast<int>(_selection_object) : -1); }
	static inline SaNtfHandleT get_ntf_handle () { return _ntf_handle; }

private:
	static int get_cluster_nodes_id ();

	static int notification_filter_free (SaNtfNotificationFilterHandleT handle);

	/*
	 * BEGIN: NTF Interface
	 */
	static void notification_callback (SaNtfSubscriptionIdT subscription_id, const SaNtfNotificationsT * notification);
	static void  notification_discardeed_callback (
			SaNtfSubscriptionIdT subscription_id,
			SaNtfNotificationTypeT notification_type,
			SaUint32T number_discarded,
			const SaNtfIdentifierT * discarded_notification_identifiers);
	/*
	 * END: NTF Interface
	 */


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	static SaVersionT _sa_version;

	static SaNtfHandleT _ntf_handle;
	static SaSelectionObjectT _selection_object;

	static internal_state_t _internal_state;

	static char _cluster_node_id [128];
	static char _cluster_other_node_id [128];

	static char _ap_node_change_state_su [512];
	static char _ap_other_node_change_state_su [512];

	static SaNtfCallbacksT _ntf_callbacks;

	static SaNtfSubscriptionIdT _subscription_id;

	static acs_apbm_serverworkingset * _server_working_set;

	// Access synchronization controllers
	static ACE_Recursive_Thread_Mutex _sync_object;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_sanotifiermanager
