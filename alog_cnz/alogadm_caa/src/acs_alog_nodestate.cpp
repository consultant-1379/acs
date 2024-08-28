/*=================================================================== */
   /**
   @file acs_alog_nodestate.cpp

   Class method implementationn for acs_alog_nodestate type module.

   This module contains the implementation of class declared in
   the acs_alog_nodestate.h module

   @version 2.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       02/02/2011     xgencol/xgaeerr       Initial Release
   N/A       28/11/2011     xgencol               Final Release
  =================================================================== */

#include "acs_alog_nodestate.h"

using namespace std;

SaNtfHandleT acs_alog_nodestate::ntfHandle;
SaSelectionObjectT acs_alog_nodestate::selObj;
string acs_alog_activeWorks::local_hostname;

extern void nodeStateActions(bool init);

extern string  CurrentNodeState;

SaNtfCallbacksT ntfCallbacks = { acs_alog_nodestate::saNtfNotificationCallback, NULL };

const char *sa_amf_ha_state_list[] = {
	"",
    "SA_AMF_HA_ACTIVE",
    "SA_AMF_HA_STANDBY",
    "SA_AMF_HA_QUIESCED",
    "SA_AMF_HA_QUIESCING"
};

/* Release code, major version, minor version */
SaVersionT version = { 'A', 0x01, 0x01 };

char* acs_alog_nodestate::error_output(SaAisErrorT result)
{
	static char error_result[256];
	ACE_OS::sprintf(error_result, "error: %u", result);
	ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
	return (error_result);
}

void acs_alog_nodestate::saNtfNotificationCallback(SaNtfSubscriptionIdT, const SaNtfNotificationsT *notification)
{
	SaInt32T i;
	const SaNtfNotificationHeaderT *notificationHeader;
	SaNtfStateChangeT* input;
	string notificationContent,currentState,hostName;

	notificationHeader = &notification->notification.stateChangeNotification.notificationHeader;
	notificationContent = (char *)notificationHeader->notificationObject->value;

	/* Changed states */
	for (i = 0; i < notification->notification.stateChangeNotification.numStateChanges; i++) {

		input = &notification->notification.stateChangeNotification.changedStates[i];

		if(input->stateId == SA_AMF_HA_STATE)
			currentState = sa_amf_ha_state_list[input->newState];
	}

	if(currentState == "SA_AMF_HA_ACTIVE" || currentState == "SA_AMF_HA_STANDBY") {

		if(notificationContent.find("SC-1") != string::npos) {
			hostName = "SC-2-1";
		}else if(notificationContent.find("SC-2") != string::npos) {
			hostName = "SC-2-2";
		}

		if(currentState.find("ACTIVE") != string::npos) {
			currentState = "active";
		}else if(currentState.find("STANDBY") != string::npos) {
			currentState = "standby";
		}

		if(hostName == acs_alog_activeWorks::local_hostname) {

			hostName += "(this ex " + CurrentNodeState + ")";

			if (currentState != CurrentNodeState)
			{
					CurrentNodeState = currentState;
					nodeStateActions(false);
			}
		}
		cout << "NODE: " << hostName << " -> " << currentState << endl;

	}
	saNtfNotificationFree(notification->notification.stateChangeNotification.notificationHandle);
}


SaAisErrorT acs_alog_nodestate::waitForNotifications(SaNtfHandleT myHandle, int selectionObject, int timeout_ms)
{
	static char error_result[256];
	SaAisErrorT error;

	int rv;
	struct pollfd fds[1];

	fds[0].fd = (int)selectionObject;
	fds[0].events = POLLIN;

	for (;;) {
		rv = poll(fds, 1, timeout_ms);

		if (rv == -1) {
			if (errno == EINTR)
				continue;
			ACE_OS::sprintf(error_result, "poll FAILED: %s", strerror(errno));
                	ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
			return SA_AIS_ERR_BAD_OPERATION;
		}

		if (rv == 0) {
			ACE_OS::sprintf(error_result, "poll timeout");
	                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
			return SA_AIS_OK;
		}

		do {
			error = saNtfDispatch(myHandle, SA_DISPATCH_ALL);
			if (SA_AIS_ERR_TRY_AGAIN == error)
				sleep(1);
		} while (SA_AIS_ERR_TRY_AGAIN == error);

		if (error != SA_AIS_OK)
		{
			ACE_OS::sprintf(error_result, "saNtfDispatch Error %d\n", error);
			ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		}
	}

	return error;
}


SaAisErrorT acs_alog_nodestate::subscribeForNotifications()
{
        static char error_result[256];

	SaAisErrorT errorCode = SA_AIS_OK;
	SaNtfStateChangeNotificationFilterT stChFilter;

	SaNtfNotificationTypeFilterHandlesT notificationFilterHandles;
	memset(&notificationFilterHandles, 0, sizeof notificationFilterHandles);

	errorCode = saNtfStateChangeNotificationFilterAllocate(ntfHandle,&stChFilter,0,0,0,0,0,0);
	if (errorCode != SA_AIS_OK) {
		ACE_OS::sprintf(error_result, "saNtfStateChangeNotificationFilterAllocate failed - %s", error_output(errorCode));
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		return errorCode;
	}

	notificationFilterHandles.stateChangeFilterHandle = stChFilter.notificationFilterHandle;

	errorCode = saNtfNotificationSubscribe(&notificationFilterHandles, 1);
	if (SA_AIS_OK != errorCode) {
		ACE_OS::sprintf(error_result,"saNtfNotificationSubscribe failed - %s", error_output(errorCode));
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		return errorCode;
	}

	errorCode = saNtfNotificationFilterFree(notificationFilterHandles.stateChangeFilterHandle);
	if (SA_AIS_OK != errorCode) {
		ACE_OS::sprintf(error_result,"saNtfNotificationFilterFree failed - %s", error_output(errorCode));
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		exit(EXIT_FAILURE);
	}

	return errorCode;
}

int acs_alog_nodestate::startWatchForNodeState() {
        /* spawn thread */
        const ACE_TCHAR* thread_name = "Watch_State_Thread";
        int call_result = ACE_Thread_Manager::instance()->spawn(&watchForNodeState,
                                                                        (void *)this ,
                                                                        THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
                                                                        & alog_watch_state_thread_id,
                                                                        0,
                                                                        ACE_DEFAULT_THREAD_PRIORITY,
                                                                        -1,
                                                                        0,
                                                                        ACE_DEFAULT_THREAD_STACKSIZE,
                                                                        &thread_name);
        if (call_result == -1)
        {
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR("Error creating watch state Thread"));
                return ACS_ALOG_error;
        }

        return ACS_ALOG_ok;
}

void* acs_alog_nodestate::watchForNodeState(void*) {

        static char error_result[256];

	SaAisErrorT error;
	int timeout = -1;	/* block indefintively in poll */
	SaNtfSubscriptionIdT subscriptionId = 1;

	error = saNtfInitialize(&ntfHandle, &ntfCallbacks, &version);
	if (SA_AIS_OK != error) {
		ACE_OS::sprintf(error_result, "saNtfInitialize failed %s", error_output(error));
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		exit(EXIT_FAILURE);
	}

	error = saNtfSelectionObjectGet(ntfHandle, &selObj);
	if (SA_AIS_OK != error) {
		ACE_OS::sprintf(error_result, "saNtfSelectionObjectGet failed %s", error_output(error));
		ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		exit(EXIT_FAILURE);
	}

	error = subscribeForNotifications();
	if (SA_AIS_OK != error) {
		ACE_OS::sprintf(error_result, "subscribeForNotifications failed %s", error_output(error));
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		exit(EXIT_FAILURE);
	}

	error = waitForNotifications(ntfHandle, selObj, timeout);
	if (SA_AIS_OK != error) {
		ACE_OS::sprintf(error_result, "subscribeForNotifications failed %s", error_output(error));
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		exit(EXIT_FAILURE);
	}

	error = saNtfNotificationUnsubscribe(subscriptionId);
	if (SA_AIS_OK != error) {
		ACE_OS::sprintf(error_result, "waitForNotifications failed %s", error_output(error));
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		exit(EXIT_FAILURE);
	}

	error = saNtfFinalize(ntfHandle);
	if (SA_AIS_OK != error) {
		ACE_OS::printf(error_result, "saNtfFinalize failed %s", error_output(error));
                ACSALOGLOG(LOG_LEVEL_FATAL,TEXTERROR(error_result));
		exit(EXIT_FAILURE);
	}

	 pthread_exit(0);
}
