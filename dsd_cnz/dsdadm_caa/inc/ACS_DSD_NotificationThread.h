#ifndef ACS_DSD_NOTIFICATIONTHREAD_H_
#define ACS_DSD_NTF_NOTIFICATIONTHREAD_H_

#include "ace/Condition_Attributes.h"
#include "ace/Monotonic_Time_Policy.h"
#include "ace/Thread_Mutex.h"
#include "ace/Condition_T.h"
#include "ace/Task.h"

#include <saNtf.h>

typedef struct {
	SaUint16T numEventTypes;
	SaUint16T numNotificationObjects;
	SaUint16T numNotifyingObjects;
	SaUint16T numNotificationClassIds;
	SaUint32T numProbableCauses;
	SaUint32T numPerceivedSeverities;
	SaUint32T numTrends;
} saNotificationFilterAllocationParamsT;

/** @class ACS_DSD_NotificationThread ACS_DSD_NotificationThread.h
 *	@brief ACS_DSD_NotificationThread class
 *	@author xludesi (Luca De Simone)
 *	@date 2011-06-30
 *	@version 0.1
 *
 *	This class is used by DSD Service to get notifications from CoreMW NTF Service (in particular, AP state changes)
 *
 */
class ACS_DSD_NotificationThread: public ACE_Task_Base
{
public:
	inline ACS_DSD_NotificationThread()
	: _condition_attributes(), _ntf_subscription_mutex(), _ntf_subscription_completed(_ntf_subscription_mutex, _condition_attributes){};
	inline virtual ~ACS_DSD_NotificationThread() {};


	/** @brief start_activity method
	 *
	 *	start_activity method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int start_activity();


	/** @brief stop_activity method
	 *
	 *	stop notification thread activity, i.e, after successful execution it no longer reacts to NTF notifications
	 *
	 *	@return Return Description
	 *	@remarks  This method doens't terminate the notification thread, it only suspends NTF notifications handling
	 */
	int stop_activity();

protected:
	/** @brief svc method
	 *
	 *	svc method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int svc (void);

private:
	static int get_ap_hostname (char * buffer, size_t size);
	static int get_ap_node_id (char * buffer, size_t size);
	static int get_partner_ap_node_id (char * buffer, size_t size);

	static int read_string_from_file (const char * filename, char * buffer, size_t size);


	/* BEGIN: NTF implementation */

	static void saNtfNotificationCallback(SaNtfSubscriptionIdT /*subscriptionId*/, const SaNtfNotificationsT * notification);
	static void saNtfNotificationDiscardedCallback(SaNtfSubscriptionIdT subscriptionId,
												SaNtfNotificationTypeT notificationType,
												SaUint32T numberDiscarded,
												const SaNtfIdentifierT */*discardedNotificationIdentifiers*/);

	/* END: NTF implementation */

private:
	/// PAY ATTENTION:  don't change the order of the following two declarations !!! The second attribute depends on the first one, and so
	/// the first attribute must be yet constructed when the costructor for the second attribute is called.
	ACE_Condition_Attributes_T<ACE_Monotonic_Time_Policy> _condition_attributes;
	ACE_Thread_Mutex _ntf_subscription_mutex;
	ACE_Condition<ACE_Thread_Mutex> _ntf_subscription_completed;

	static bool _notifications_handling_enabled;  // true if and only if the NTF notifications handling has to be performed

	/* BEGIN: NTF implementation */
	static SaNtfCallbacksT ntfCallbacks;
	/* END: NTF implementation */
};

#endif
