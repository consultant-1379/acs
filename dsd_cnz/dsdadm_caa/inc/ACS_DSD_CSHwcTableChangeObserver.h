#ifndef HEADER_GUARD_CLASS__ACS_DSD_CSHwcTableChangeObserver
#define HEADER_GUARD_CLASS__ACS_DSD_CSHwcTableChangeObserver ACS_DSD_CSHwcTableChangeObserver

/** @file ACS_DSD_CSHwcTableChangeObserver.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2013-05-27
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
 *	| R-001 | 2013-05-27 | xcasale      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_CS_API.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_CSHwcTableChangeObserver


/** @class ACS_DSD_CSHwcTableChangeObserver ACS_DSD_CSHwcTableChangeObserver.h
 *	@brief ACS_DSD_CSHwcTableChangeObserver class
 *	@author xcasale (Alessio Cascone)
 *	@date 2013-05-27
 *
 *	ACS_DSD_CSHwcTableChangeObserver <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACS_CS_API_HWCTableObserver {

	//==============//
	// Enumerations //
	//==============//
public:
	typedef enum ObserverStateConstants {
		OBSERVER_STATE_STOPPED = 0,
		OBSERVER_STATE_STARTED = 1
	} observer_state_t;

	enum CSHwcTableChangeObserverErrorConstants {
			TCO_ERROR_NO_ERROR = 0,
			TCO_ERROR_CS_GET_INSTANCE,
			TCO_ERROR_CS_ERROR,
			TCO_ERROR_MASTER_CP_SET_FAILED,
			TCO_ERROR_GET_PARTNER_NODE_FAILED,
			TCO_ERROR_CONNECT_FAILED,
			TCO_ERROR_SEND_FAILED,
			TCO_ERROR_RECV_FAILED,
			TCO_ERROR_PRIMITIVE_WITH_ERROR
	};

	enum CSHwcTableChangeObservberFBNConstants {
		FBN_UNKNOWN = -1,
		FBN_CPUB    = ACS_CS_API_HWC_NS::FBN_CPUB,
		FBN_APUB    = ACS_CS_API_HWC_NS::FBN_APUB
	};

	enum CSHwcTableChangeObservberOpTypeConstants {
		OP_UNSPECIFIED = ACS_CS_API_TableChangeOperation::Unspecified,
		OP_ADD         = ACS_CS_API_TableChangeOperation::Add,
		OP_DELETE      = ACS_CS_API_TableChangeOperation::Delete,
		OP_CHANGE      = ACS_CS_API_TableChangeOperation::Change
	};

	enum CSHwcTableChangeObservberCSSystemTypeConstants {
		CS_SYSTEM_TYPE_BC      = ACS_CS_API_HWC_NS::SysType_BC,
		CS_SYSTEM_TYPE_CP      = ACS_CS_API_HWC_NS::SysType_CP,
		CS_SYSTEM_TYPE_AP      = ACS_CS_API_HWC_NS::SysType_AP,
		CS_SYSTEM_TYPE_UNKNOWN
	};


	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_CSHwcTableChangeObserver constructor
	 */
	__CLASS_NAME__ () : _state(OBSERVER_STATE_STOPPED), _sync_object() {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_CSHwcTableChangeObserver Destructor
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
  int handle_apub_board_notification (uint8_t op_type, uint16_t system_type, int32_t system_id, int8_t side, bool send_notification_to_passive);
  int handle_cpub_board_notification (uint8_t op_type, uint16_t system_type, int32_t system_id, int8_t side, bool send_notification_to_passive);

private:
  int handle_apub_board_notification (const ACS_CS_API_HWCTableData_R1 & hwc_item);
  int handle_cpub_board_notification (const ACS_CS_API_HWCTableData_R1 & hwc_item);
  int send_board_notification_to_passive_node (uint16_t fbn, uint8_t op_type, uint16_t system_type, int32_t system_id, int8_t side);


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

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_CSHwcTableChangeObserver
