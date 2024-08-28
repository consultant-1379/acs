/*
 * ACS_PRC_NotificationThread.h
 *
 *  Created on: Jan 18, 2011
 *      Author: xlucpet
 */

#ifndef ACS_PRC_NOTIFICATIONTHREAD_H_
#define ACS_PRC_NOTIFICATIONTHREAD_H_

#include "ace/Task.h"
#include "ACS_PRC_RunTimeOwner.h"
#include <sys/poll.h>
#include <saNtf.h>
#include "ACS_PRC_ispapi.h"
#include "ACS_TRA_Logging.h"
#include "acs_prc_runlevel.h"

#define MAX_PRC_IMM_TRY 12;
//#define SA_AMF_HA_STATE = 2;
#define DEFAULT_FLAG 0x0001
	/* #define MAX_NUMBER_OF_STATE_CHANGES 5 */
#define DEFAULT_NUMBER_OF_CHANGED_STATES 4
#define DEFAULT_NUMBER_OF_OBJECT_ATTRIBUTES 2
#define DEFAULT_NUMBER_OF_CHANGED_ATTRIBUTES 2

/* TODO: decide max values ??*/
#define MAX_NUMBER_OF_STATE_CHANGES 1000
#define MAX_NUMBER_OF_CHANGED_ATTRIBUTES 1000
#define MAX_NUMBER_OF_OBJECT_ATTRIBUTES  1000

#define DEFAULT_ADDITIONAL_TEXT "The HA state of the node is changed"
#define DEFAULT_NOTIFICATION_OBJECT "default notification object"
#define DEFAULT_NOTIFYING_OBJECT "Process Control Notification Server"
#define ERICSSON_VENDOR_ID 193

typedef enum {
	SA_NTF_DISABLED,
	SA_NTF_ENABLED,
	SA_NTF_IDLE,
	SA_NTF_ACTIVE
} saNtfStatesT;

typedef enum {
	MY_APP_OPER_STATE = 1,
	MY_APP_USAGE_STATE = 2
} saNtfStateIdT;

typedef struct {
	SaUint16T numEventTypes;
	SaUint16T numNotificationObjects;
	SaUint16T numNotifyingObjects;
	SaUint16T numNotificationClassIds;
	SaUint32T numProbableCauses;
	SaUint32T numPerceivedSeverities;
	SaUint32T numTrends;
} saNotificationFilterAllocationParamsT;

typedef struct {
	SaUint16T numCorrelatedNotifications;
	SaUint16T lengthAdditionalText;
	SaUint16T numAdditionalInfo;
	SaUint16T numSpecificProblems;
	SaUint16T numMonitoredAttributes;
	SaUint16T numProposedRepairActions;
	SaUint16T numStateChanges;
	SaUint16T numObjectAttributes;
	SaUint16T numAttributes;
	SaInt16T variableDataSize;
} saNotificationAllocationParamsT;

typedef SaUint16T saNotificationFlagsT;

typedef struct {
	/* SaNtfSeverityTrendT trend; */
	SaStringT additionalText;
	SaNtfProbableCauseT probableCause;
	SaNtfSeverityT perceivedSeverity;
	SaNtfStateChangeT changedStates[MAX_NUMBER_OF_STATE_CHANGES];
	SaNtfEventTypeT eventType;
	SaNtfEventTypeT alarmEventType;
	SaNtfEventTypeT stateChangeEventType;
	SaNtfEventTypeT objectCreateDeleteEventType;
	SaNtfEventTypeT attributeChangeEventType;
	SaNtfEventTypeT securityAlarmEventType;
	SaNtfNotificationTypeT notificationType;
	SaNameT notificationObject;
	SaNameT notifyingObject;
	SaNtfClassIdT notificationClassId;
	SaTimeT eventTime;
	SaNtfIdentifierT notificationId;
	SaNtfSubscriptionIdT subscriptionId;
	SaNtfSourceIndicatorT stateChangeSourceIndicator;

	/* Object Create Delete Specific */
	SaNtfSourceIndicatorT objectCreateDeleteSourceIndicator;
	SaNtfAttributeT objectAttributes[MAX_NUMBER_OF_OBJECT_ATTRIBUTES];
	SaNtfValueTypeT attributeType;

	/* Attribute Change Specific */
	SaNtfSourceIndicatorT attributeChangeSourceIndicator;
	SaNtfAttributeChangeT changedAttributes[MAX_NUMBER_OF_CHANGED_ATTRIBUTES];

	/* Security Alarm Specific */
	SaNtfProbableCauseT securityAlarmProbableCause;
	SaNtfSeverityT severity;
	SaNtfSecurityAlarmDetectorT securityAlarmDetector;
	SaNtfServiceUserT serviceProvider;
	SaNtfServiceUserT serviceUser;

	SaInt32T timeout;
	SaInt32T burstTimeout;
	unsigned int repeateSends;
} saNotificationParamsT;

class ACS_PRC_NotificationThread : public ACE_Task_Base{

public:
	ACS_PRC_NotificationThread();
	virtual ~ACS_PRC_NotificationThread();

	void set (ACS_PRC_RunTimeOwner* p_Implementer){
		implementer = p_Implementer;
	}

	ACS_PRC_RunTimeOwner* get (){
			return implementer;
	}

	virtual int svc ( void );

	int static getNodeState ();

	void stop();

	void start();

	bool static getSoftwareUpdate (){ return SoftwareUpdate; }

	void static raise_APG_Backup_Missing();
	
	static void setRestoreFlagInRunLevelThread(bool isRestored);

        static bool getRestoreFlag();

private:

	void static NotificationCallback(SaNtfSubscriptionIdT subscriptionId, const SaNtfNotificationsT *notification);

	static SaNtfCallbacksT ntfCallbacks;

	static int NodeState;

	static bool SoftwareUpdate;

	ACS_PRC_RunTimeOwner* implementer;

	static ACS_TRA_Logging* Logging;

	static acs_prc_runlevel* RunLevel;

	static Ispapi* isp_Event;

};

#endif /* ACS_PRC_NOTIFICATIONTHREAD_H_ */
