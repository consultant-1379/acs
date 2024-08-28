#ifndef HEADER_GUARD_CLASS__acs_apbm_trapsubscriptionmanager
#define HEADER_GUARD_CLASS__acs_apbm_trapsubscriptionmanager acs_apbm_trapsubscriptionmanager

/** @file acs_apbm_subscriptionmanager.h
 *	@brief
 *	@author xassore
 *	@date 2011-09-28
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
 *	| R-001 | 2011-09-28 | xassore      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <string>
#include <vector>
#include <list>
#include <map>
#include <ace/Recursive_Thread_Mutex.h>
#include "acs_apbm_snmpconstants.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_trapsubscriptionmanager


class acs_apbm_boardsessionhandler;

#define BIT_1_MASK		0x00000001
#define IS_BIT_1(x)	(x & BIT_1_MASK)
#define ACS_APBM_MAX_NUM_SLOT 29


// This class is used to keep information about a trap
class trapdata
{
public:
	inline trapdata(){trapType=0;message_len=0;strcpy(message, "");}
	inline ~trapdata(){}
	trapdata & operator= (const trapdata & from_trapdata);

	int trapType;
	size_t message_len;
	char message[acs_apbm_snmp::TRAP_MESSAGE_LEN];
	std::vector<int> values;
};


class __CLASS_NAME__
{
public:
	enum acs_apbm_trapsubscrition_constants
		{
			TRAPSUB_SUCCESS = 0,
			TRAPSUB_SUBSCRIBER_ALREADY_PRESENT = 1,
			TRAPSUB_GEN_ERROR  = -1,
			TRAPSUB_SUBSCRIBER_ADD_ERR = -2,
			TRAPSUB_BAD_PARAMETER_VALUE = -3
		};

public:
	/** acs_apbm_trapsubscriptionmanager constructor */
	inline __CLASS_NAME__ () {}

	/** acs_apbm_trapsubscriptionmanager Destructor */
	inline ~__CLASS_NAME__ () {}

	typedef std::list<acs_apbm_boardsessionhandler *> TrapSubscriberList;

	/// subscribe a 'client session' to receive all traps concerning the AP boards whose slots are specified in the bitmap <slotmask>
	int addSubscriber(int32_t slotmask, acs_apbm_boardsessionhandler *pSessionHandler);

	// cancel a subscription
	int removeSubscriber(acs_apbm_boardsessionhandler *pSessionHandler);

	// notify to the object a trap involving the AP board in slot <slotId>
	int notifyTrapInfo(long slotId, trapdata &trapMsg);

private:
	/// object used to make the class thread-safe
		ACE_Recursive_Thread_Mutex _trapSubscription_mutex;
		TrapSubscriberList _boardtrapSubscriberList[ACS_APBM_MAX_NUM_SLOT];
};


#endif /* ACS_APBM_SUBSCRIBER_H_ */
