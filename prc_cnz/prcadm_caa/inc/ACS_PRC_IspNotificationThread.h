/*
 * ACS_PRC_IspNotificationThread.h
 *
 *  Created on: Jan 18, 2011
 *      Author: xlucpet
 */

#ifndef ACS_PRC_IspNotificationThread_H_
#define ACS_PRC_IspNotificationThread_H_

#include "ace/Task.h"
#include <sys/poll.h>
#include <saNtf.h>
#include "ACS_PRC_ispapi.h"
#include "ACS_TRA_Logging.h"
#include "acs_apgcc_oihandler_V2.h"
#include "ACS_PRC_IspAdminOperationImplementer.h"
#include "ACS_PRC_IspAdminOperationImplementerThread.h"

#define MAX_PRC_IMM_TRY 12;
//#define SA_AMF_HA_STATE = 2;

using namespace std;

typedef struct {
	SaUint16T numEventTypes;
	SaUint16T numNotificationObjects;
	SaUint16T numNotifyingObjects;
	SaUint16T numNotificationClassIds;
	SaUint32T numProbableCauses;
	SaUint32T numPerceivedSeverities;
	SaUint32T numTrends;
} saNotificationFilterAllocationParamsT;

class ACS_PRC_IspNotificationThread : public ACE_Task_Base{

public:
	ACS_PRC_IspNotificationThread();
	virtual ~ACS_PRC_IspNotificationThread();

	void stop();

	void start();

	virtual int svc ( void );

	ACS_PRC_IspAdminOperationImplementer* getOI();
	acs_apgcc_oihandler_V2* getOI_Handler();

private:

	void static NotificationCallback(SaNtfSubscriptionIdT subscriptionId, const SaNtfNotificationsT *notification);
	static SaNtfCallbacksT ntfCallbacks;

	static ACS_TRA_Logging* Logging;

};

#endif /* ACS_PRC_IspNotificationThread_H_ */
