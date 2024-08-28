#ifndef ACS_USA_NOTIFICATIONTHREAD_H_
#define ACS_USA_NOTIFICATIONTHREAD_H_

#include "ace/Thread_Mutex.h"
#include <ace/Task_T.h>
#include <ace/OS.h>
#include <saNtf.h>
#include <stdio.h>
#include <acs_usa_global.h>
#include "acs_usa_tratrace.h"
#include "acs_usa_common.h"
//Forwards
class Global;

typedef struct {
	SaUint16T numEventTypes;
	SaUint16T numNotificationObjects;
	SaUint16T numNotifyingObjects;
	SaUint16T numNotificationClassIds;
	SaUint32T numProbableCauses;
	SaUint32T numPerceivedSeverities;
	SaUint32T numTrends;
} saNotificationFilterAllocationParamsT;
class ACS_USA_NtfNotificationThread: public ACE_Task<ACE_SYNCH> 
{
public:
	ACS_USA_NtfNotificationThread();
	virtual ~ACS_USA_NtfNotificationThread();
        int svc();
        int close(u_long);
        int open();
        int close();
private:
	/* BEGIN: NTF implementation */

	static void saNtfNotificationCallback(SaNtfSubscriptionIdT /*subscriptionId*/, const SaNtfNotificationsT * notification);
	void freeNtfFilter(SaNtfNotificationFilterHandleT *fh_ptr);
	SaAisErrorT subscribeForNotifications(const saNotificationFilterAllocationParamsT * notificationFilterAllocationParams, SaNtfSubscriptionIdT subscriptionId);
	SaAisErrorT waitForNotifications(SaNtfHandleT myHandle, int selectionObject, int timeout_ms);

	/* END: NTF implementation */

private:
	/* BEGIN: NTF implementation */
	static SaNtfCallbacksT ntfCallbacks;
	Global *m_global_instance;
	/* END: NTF implementation */
	SaNtfHandleT ntfHandle;
	SaSelectionObjectT selObj;
	static bool thread_sleep;
};

#endif /*ACS_USA_NOTIFICATIONTHREAD_H_*/
