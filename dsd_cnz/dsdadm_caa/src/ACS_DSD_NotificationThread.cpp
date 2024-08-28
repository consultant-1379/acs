#include <assert.h>
#include <poll.h>
#include <saAmf.h>
#include <saNtf.h>
#include <saAis.h>
#include <saClm.h>
#include <string>
#include "ACS_DSD_Imm_functions.h"
#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_RemoteApNodesManager.h"
#include "ACS_DSD_NotificationThread.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


extern ACS_DSD_Logger dsd_logger;
extern bool dsd_is_stopping;
extern ACS_DSD_StartupSessionsGroup * startup_session_group;
extern ACS_DSD_RemoteApNodesManager remote_AP_nodes_manager;
static SaNtfHandleT ntfHandle;
static SaSelectionObjectT selObj;
static char ap_node_hostname[256] = {0};
static char ap_node_id[8] = {0};
static char ap_other_node_id[8] = {0};
extern bool is_swm_2_0;
/* Release code, major version, minor version */
static SaVersionT version = { 'A', 0x01, 0x01 };

static struct s_filters_T {
	 int all;
	 int alarm;
	 int obj_cr_del;
	 int att_ch;
	 int st_ch;
	 int sec_al;
} used_filters = {1,1,1,1,1,1};


static const char *sa_amf_ha_state_list[] = {
	 "",
	 "SA_AMF_HA_ACTIVE",
	 "SA_AMF_HA_STANDBY",
	 "SA_AMF_HA_QUIESCED",
	 "SA_AMF_HA_QUIESCING"
};

static char *error_output(SaAisErrorT result)
{
	 static char error_result[256];

	 sprintf(error_result, "error: %u", result);
	 return (error_result);
}

SaNtfCallbacksT ACS_DSD_NotificationThread::ntfCallbacks = {
	 saNtfNotificationCallback,
	 saNtfNotificationDiscardedCallback
};


bool ACS_DSD_NotificationThread::_notifications_handling_enabled = false;


void ACS_DSD_NotificationThread::saNtfNotificationCallback(SaNtfSubscriptionIdT /*subscriptionId*/, const SaNtfNotificationsT *notification)
{
	 SaNtfNotificationHandleT notificationHandle;
	 const SaNtfNotificationHeaderT *notificationHeader;
	 char notificationObject[SA_MAX_NAME_LENGTH + 1];
	 char additional_info[SA_MAX_NAME_LENGTH + 1]= {0};
	 char tmpObject_1[SA_MAX_NAME_LENGTH + 1] = {0};
	 char tmpObject_2[SA_MAX_NAME_LENGTH + 1] = {0};
	 SaNtfStateChangeT *saNtfStateChange = 0;
	std::string NodeStateSi;
        std::string NodeStateSu;
	 if(!notification)
	 {	 // should not occur, but protect us against this anomaly
		 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_NotificationThread: 'saNtfNotificationCallback()' invoked with the parameter 'notification' equal to 0 !"));
		 return;
	 }

	 if(_notifications_handling_enabled && !dsd_is_stopping)
	 {
		 switch (notification->notificationType) {
			 case SA_NTF_TYPE_STATE_CHANGE:
			 {
				 notificationHandle = notification->notification.stateChangeNotification.notificationHandle;
				 notificationHeader = &notification->notification.stateChangeNotification.notificationHeader;
				 strncpy(notificationObject, (char *)notificationHeader->notificationObject->value, notificationHeader->notificationObject->length);
				 notificationObject[notificationHeader->notificationObject->length] = '\0';

				 // Get Additional Info associated to the notification
				 if ( notificationHeader->additionalInfo != NULL ) {
					if ( notificationHeader->additionalInfo[0].infoType == SA_NTF_VALUE_LDAP_NAME) {
						SaNameT *dataPtr;
						SaUint16T dataSize;
						SaAisErrorT rc;

						rc = saNtfPtrValGet( notificationHandle, &notificationHeader->additionalInfo[0].infoValue, (void **)&dataPtr, &dataSize );

						if (rc == SA_AIS_OK){
							strncpy( additional_info, (char*)dataPtr->value, dataPtr->length);
						}
					}
				 }

				 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread: state change notification ! notificationObject = \"%s\", additionalInfo = \"%s\""), notificationObject, additional_info);

				 // in exceptional situations, could get a 'SA_NTF_TYPE_STATE_CHANGE' notification with zero state changes.
				 // We must handle this case.
				 if(notification->notification.stateChangeNotification.numStateChanges == 0)
				 {
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_NotificationThread: received state change notification with zero state changes !"));
					break;
				 }

				 // Reload AP node ID and AP partner node ID if not yet read
				 if (!(*ap_node_id)) {
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread : reading the AP node ID."));
						get_ap_node_id(ap_node_id, ACS_DSD_ARRAY_SIZE(ap_node_id));
				 }
				 if (!(*ap_other_node_id)) {
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread : reading my partner AP node ID."));
						get_partner_ap_node_id(ap_other_node_id, ACS_DSD_ARRAY_SIZE(ap_other_node_id));
				 }
			        if ( is_swm_2_0 == true )
				{
				  NodeStateSu="safSu=SC-%s,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service";
				  NodeStateSi="safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service";
			        }
			        else{
				 NodeStateSu= "safSu=%s,safSg=2N,safApp=ERIC-APG";
				 NodeStateSi="safSi=AGENT,safApp=ERIC-APG";
			        }

				 // check if the STATE_CHANGE is of type "SA_AMF_HA_STATE" and if the notification is related to AP nodes
				 if (*ap_node_id) sprintf(tmpObject_1, NodeStateSu.c_str(), ap_node_id);
				 if (*ap_other_node_id) sprintf(tmpObject_2, NodeStateSu.c_str(), ap_other_node_id);
				 bool cond_1 = !strcmp(tmpObject_1, notificationObject);
				 bool cond_2 = !strcmp(tmpObject_2, notificationObject);
				 saNtfStateChange = & notification->notification.stateChangeNotification.changedStates[0];

				 if(saNtfStateChange && (saNtfStateChange->stateId == SA_AMF_HA_STATE) && (cond_1 || cond_2) && strstr(additional_info, NodeStateSi.c_str()) != NULL)
				 {
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
							ACS_DSD_TEXT("ACS_DSD_NotificationThread: Detected HA state transition for AP Node '%s'. New State: < %s > "),
							cond_1 ? ap_node_id : ap_other_node_id,  sa_amf_ha_state_list[saNtfStateChange->newState]);

					// reset flags used to handle pending IMM operations (take/release control)
					reset_IMM_op_pending_flags();

					if(cond_1 && (saNtfStateChange->newState == SA_AMF_HA_ACTIVE) && !dsd_is_stopping)	//Fix for TR HY35813
					{
						// this AP node has become ACTIVE
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Trying to update local node state in IMM ..."));
						int call_result = update_local_node_state_in_IMM();
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT(" ... ended attempt to update local node state in IMM. Result == < %d > "), call_result);

						// we must take control of IMM shared objects
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Trying to take control of IMM SHARED objects ..."));
						int nRetry = N_RETRY_IMM_REGISTER;
						int op_res = take_control_of_IMM_shared_objects(nRetry);
						if(op_res < 0)
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Failure while trying to take control of IMM SHARED objects. Return code is < %d >"), op_res);
						else if(op_res == 1)
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("This DSD Server was yet the controller of IMM SHARED objects. NRETRY = < %d >"), nRetry);
						else
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("This DSD Server is now the controller of IMM SHARED objects. NRETRY = < %d >"), nRetry);
					}
					else if( (cond_2 && (saNtfStateChange->newState == SA_AMF_HA_ACTIVE) && !dsd_is_stopping) ||  /* the other AP Node has become ACTIVE */
							 (cond_1 &&
								 (
								   (saNtfStateChange->newState == SA_AMF_HA_STANDBY)   || /* this AP Node has become PASSIVE */
							       (saNtfStateChange->newState == SA_AMF_HA_QUIESCED)  ||   /* this AP Node entered QUIESCED state */
							       (saNtfStateChange->newState == SA_AMF_HA_QUIESCING) /* this AP Node entered QUIESCING state */
							     ) && !dsd_is_stopping	//Fix for TR HY31357
						     )
						   )
					{
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Trying to update local node state in IMM ..."));
						int call_result = update_local_node_state_in_IMM();
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT(" ... ended attempt to update local node state in IMM. Result == < %d > "), call_result);

						// we must release control of IMM shared objects
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Trying to release control of IMM SHARED objects ..."));
						int nRetry = N_RETRY_IMM_FINALIZE;
						int op_res = release_control_of_IMM_shared_objects(nRetry);
						if(op_res < 0)
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Failure while trying to release control of IMM SHARED objects. Return code is < %d >"), op_res);
						else if(op_res == 1)
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("This DSD Server was NOT the controller of IMM SHARED objects. NRETRY = < %d >"), nRetry);
						else
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("DSD Server has released control of IMM SHARED objects. NRETRY = < %d >"), nRetry);

						if ( startup_session_group ) {
							// We must close all the kept-alive connections previously opened
							op_res = startup_session_group->close(1);
							if ( op_res && (op_res != ACS_DSD_StartupSessionsGroup::SSG_ERROR_ALREADY_CLOSED) )
								ACS_DSD_LOG(dsd_logger,	LOG_LEVEL_ERROR,
										ACS_DSD_TEXT("Call 'startup_session_group->close' failed. Return code is < %d >"), op_res);
							else
								ACS_DSD_LOG(dsd_logger,
										LOG_LEVEL_DEBUG, ACS_DSD_TEXT("StartupSessionGroup correctly closed."));
						}

						// We must delete all the objects used to keep information about the startup-sessions with remote DSD servers opened
						remote_AP_nodes_manager.remove_all_ap_nodes_connections();
					}
				 }
			 }
			 break;

		 case SA_NTF_TYPE_ALARM:
		 case SA_NTF_TYPE_OBJECT_CREATE_DELETE:
		 case SA_NTF_TYPE_ATTRIBUTE_CHANGE:
		 case SA_NTF_TYPE_SECURITY_ALARM:
			 break;
		 default:
			 break;
		 }
	 }
	// else
	//	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread: state change notification while DSD is stopping. Ignore it"));


	 switch (notification->notificationType) {
	 case SA_NTF_TYPE_ALARM:
			 saNtfNotificationFree(notification->notification.alarmNotification.notificationHandle);
			 break;
	 case SA_NTF_TYPE_SECURITY_ALARM:
			 saNtfNotificationFree(notification->notification.securityAlarmNotification.notificationHandle);
			 break;
	 case SA_NTF_TYPE_STATE_CHANGE:
			 saNtfNotificationFree(notification->notification.stateChangeNotification.notificationHandle);
			 break;
	 case SA_NTF_TYPE_OBJECT_CREATE_DELETE:
			 saNtfNotificationFree(notification->notification.objectCreateDeleteNotification.notificationHandle);
			 break;
	 case SA_NTF_TYPE_ATTRIBUTE_CHANGE:
			 saNtfNotificationFree(notification->notification.attributeChangeNotification.notificationHandle);
			 break;

	 default:
			 assert(0);
	 }
}

void ACS_DSD_NotificationThread::saNtfNotificationDiscardedCallback(SaNtfSubscriptionIdT subscriptionId,
											SaNtfNotificationTypeT notificationType,
											SaUint32T numberDiscarded,
											const SaNtfIdentifierT */*discardedNotificationIdentifiers*/)
{


	 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			   ACS_DSD_TEXT("Discarded callback function  notificationType: %d\n \
			   subscriptionId  : %u \n \
			   numberDiscarded : %u"), (int)notificationType, (unsigned int)subscriptionId, (unsigned int)numberDiscarded);
}

static SaAisErrorT waitForNotifications(SaNtfHandleT myHandle, int selectionObject, int timeout_ms)
{
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
					 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread: in function waitForNotifications() poll FAILED ! %s "), strerror(errno) );
					 return SA_AIS_ERR_BAD_OPERATION;
			 }

			 if (rv == 0) {
					 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread: in function waitForNotifications() poll timeout !"));
					 return SA_AIS_OK;
			 }

			 do {
					 error = saNtfDispatch(myHandle, SA_DISPATCH_ALL);
					 if (SA_AIS_ERR_TRY_AGAIN == error)
							 sleep(1);
			 } while (SA_AIS_ERR_TRY_AGAIN == error);

			 if (error != SA_AIS_OK)
				 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread: in function waitForNotifications() saNtfDispatch Error %d"), error);
	 }

	 return error;
}


static void freeNtfFilter(SaNtfNotificationFilterHandleT *fh_ptr)
{
	 SaAisErrorT errorCode = SA_AIS_OK;
	 if (*fh_ptr) {
			 errorCode = saNtfNotificationFilterFree(*fh_ptr);
			 if (SA_AIS_OK != errorCode) {
					 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread:  saNtfNotificationFilterFree() failed ! Error code is < %d >"),errorCode );
			 }
	 }
}

/* Subscribe */
static SaAisErrorT subscribeForNotifications(const saNotificationFilterAllocationParamsT * notificationFilterAllocationParams, SaNtfSubscriptionIdT subscriptionId)
{
	 SaAisErrorT errorCode = SA_AIS_OK;
	 SaNtfAlarmNotificationFilterT myAlarmFilter;
	 SaNtfAttributeChangeNotificationFilterT attChFilter;
	 SaNtfStateChangeNotificationFilterT stChFilter;
	 SaNtfObjectCreateDeleteNotificationFilterT objCrDelFilter;
	 SaNtfSecurityAlarmNotificationFilterT secAlarmFilter;

	 SaNtfNotificationTypeFilterHandlesT notificationFilterHandles;
	 memset(&notificationFilterHandles, 0, sizeof notificationFilterHandles);

	 if (used_filters.all || used_filters.alarm) {
			 errorCode = saNtfAlarmNotificationFilterAllocate(ntfHandle,
															  &myAlarmFilter,
															  notificationFilterAllocationParams->numEventTypes,
															  notificationFilterAllocationParams->numNotificationObjects,
															  notificationFilterAllocationParams->numNotifyingObjects,
															  notificationFilterAllocationParams->numNotificationClassIds,
															  notificationFilterAllocationParams->numProbableCauses,
															  notificationFilterAllocationParams->numPerceivedSeverities,
															  notificationFilterAllocationParams->numTrends);

			 if (errorCode != SA_AIS_OK) {
				 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("saNtfAlarmNotificationFilterAllocate failed - %s"), error_output(errorCode));
				 return errorCode;
			 }
			 notificationFilterHandles.alarmFilterHandle = myAlarmFilter.notificationFilterHandle;
	 }

	 if (used_filters.all || used_filters.att_ch) {
			 errorCode = saNtfAttributeChangeNotificationFilterAllocate(ntfHandle,
					 &attChFilter,
					 notificationFilterAllocationParams->numEventTypes,
					 notificationFilterAllocationParams->numNotificationObjects,
					 notificationFilterAllocationParams->numNotifyingObjects,
					 notificationFilterAllocationParams->numNotificationClassIds,
					 0);

			 if (errorCode != SA_AIS_OK) {
					 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("saNtfAttributeChangeNotificationFilterAllocate failed - %s"), error_output(errorCode));
					 return errorCode;
			 }
			 notificationFilterHandles.attributeChangeFilterHandle = attChFilter.notificationFilterHandle;
	 }

	 if (used_filters.all || used_filters.obj_cr_del) {
			 errorCode = saNtfObjectCreateDeleteNotificationFilterAllocate(ntfHandle,
					 &objCrDelFilter,
					 notificationFilterAllocationParams->numEventTypes,
					 notificationFilterAllocationParams->numNotificationObjects,
					 notificationFilterAllocationParams->numNotifyingObjects,
					 notificationFilterAllocationParams->numNotificationClassIds,
					 0);

			 if (errorCode != SA_AIS_OK) {
					 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("saNtfObjectCreateDeleteNotificationFilterAllocate failed - %s"), error_output(errorCode));
					 return errorCode;
			 }
			 notificationFilterHandles.objectCreateDeleteFilterHandle = objCrDelFilter.notificationFilterHandle;
	 }

	 if (used_filters.all || used_filters.st_ch) {
			 errorCode = saNtfStateChangeNotificationFilterAllocate(ntfHandle,
					 &stChFilter,
					 notificationFilterAllocationParams->numEventTypes,
					 notificationFilterAllocationParams->numNotificationObjects,
					 notificationFilterAllocationParams->numNotifyingObjects,
					 notificationFilterAllocationParams->numNotificationClassIds,
					 0,
					 0);
			 if (errorCode != SA_AIS_OK) {
					 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("saNtfStateChangeNotificationFilterAllocate failed - %s"), error_output(errorCode));
					 return errorCode;
			 }
			 notificationFilterHandles.stateChangeFilterHandle = stChFilter.notificationFilterHandle;
	 }

	 if (used_filters.all || used_filters.sec_al) {
			 errorCode = saNtfSecurityAlarmNotificationFilterAllocate(ntfHandle,
					 &secAlarmFilter,
					 notificationFilterAllocationParams->numEventTypes,
					 notificationFilterAllocationParams->numNotificationObjects,
					 notificationFilterAllocationParams->numNotifyingObjects,
					 notificationFilterAllocationParams->numNotificationClassIds,
					 0,0,0,0,0);
			 if (errorCode != SA_AIS_OK) {
					 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("saNtfSecurityAlarmNotificationFilterAllocate failed - %s"), error_output(errorCode));
					 return errorCode;
			 }
			 notificationFilterHandles.securityAlarmFilterHandle = secAlarmFilter.notificationFilterHandle;
	 }


	 errorCode = saNtfNotificationSubscribe(&notificationFilterHandles, subscriptionId);
	 if (SA_AIS_OK != errorCode) {
			 ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("saNtfNotificationSubscribe failed - %s"), error_output(errorCode));
			 return errorCode;
	 }
	 freeNtfFilter (&notificationFilterHandles.alarmFilterHandle);
	 freeNtfFilter (&notificationFilterHandles.attributeChangeFilterHandle);
	 freeNtfFilter (&notificationFilterHandles.objectCreateDeleteFilterHandle);
	 freeNtfFilter (&notificationFilterHandles.stateChangeFilterHandle);
	 freeNtfFilter (&notificationFilterHandles.securityAlarmFilterHandle);

	 return errorCode;
}


int ACS_DSD_NotificationThread::start_activity()
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_NotificationThread : NTF notifications handling ENABLED."));
	_notifications_handling_enabled = true;

	int ret_val = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1);
	if(!ret_val) {
		ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
		timeout = timeout.now() + ACE_Time_Value(3);

		_ntf_subscription_completed.wait(&timeout);	 // wait for NFT subscription completed, to avoid losing notifications
	}

	return ret_val;
}

int ACS_DSD_NotificationThread::stop_activity()
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_NotificationThread : NTF notifications handling DISABLED."));
	_notifications_handling_enabled = false;
	return 0;
}


int ACS_DSD_NotificationThread::svc()
{
	int timeout = -1;
	SaAisErrorT error;
	saNotificationFilterAllocationParamsT notificationFilterAllocationParams = {0,0,0,0,0,0,0};
	SaNtfSubscriptionIdT subscriptionId = 1;
//	int nParsedFields = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread started !"));

	// get the ap node 'hostname'
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread : reading the AP hostname."));
	get_ap_hostname(ap_node_hostname, ACS_DSD_ARRAY_SIZE(ap_node_hostname));

	// get 'id' of the AP node
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread : reading the AP node ID."));
	get_ap_node_id(ap_node_id, ACS_DSD_ARRAY_SIZE(ap_node_id));

	// get 'id' of the other AP Node
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread : reading my partner AP node ID."));
	get_partner_ap_node_id(ap_other_node_id, ACS_DSD_ARRAY_SIZE(ap_other_node_id));

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread : HOSTNAME = <%s>, HOSTID = <%s> !"),
			(*ap_node_hostname ? ap_node_hostname : "[UNKNOWN]"), (*ap_node_id ? ap_node_id : "[UNKNOWN]"));

	// connect to CoreMW Ntf service and register callbacks
	while ((error = saNtfInitialize(&ntfHandle, &ntfCallbacks, &version)) != SA_AIS_OK) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread : saNtfInitialize failed - %s : retrying"),
				error_output(error));
		::sleep(1);
	}

	while ((error = saNtfSelectionObjectGet(ntfHandle, &selObj)) != SA_AIS_OK) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread : saNtfSelectionObjectGet failed - %s : retrying"),
				error_output(error));
		::sleep(1);
	}

	while ((error = subscribeForNotifications(&notificationFilterAllocationParams, subscriptionId)) != SA_AIS_OK) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread : subscribeForNotifications failed - %s : retrying"),
				error_output(error));
		::sleep(1);
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread : start waiting for notifications !"));

	// notify to "start_activity()" method the completion of subscription procedure
	_ntf_subscription_completed.signal();

	error = waitForNotifications(ntfHandle, selObj, timeout);
	if (SA_AIS_OK != error) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread : waitForNotifications failed - %s"), error_output(error));
	}

	error = saNtfNotificationUnsubscribe(subscriptionId);
	if (SA_AIS_OK != error) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread : saNtfNotificationUnsubscribe failed - %s"), error_output(error));
	}

	error = saNtfFinalize(ntfHandle);
	if (SA_AIS_OK != error) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_NotificationThread : saNtfFinalize failed - %s"), error_output(error));
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread is ending !"));

	return error;
}

int ACS_DSD_NotificationThread::get_ap_hostname (char * buffer, size_t size) {
	int return_code = read_string_from_file(AP_NODE_HOSTNAME_FILEPATH, buffer, size);

	if (return_code) {
		*buffer = '\0';
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("ACS_DSD_NotificationThread :  Error trying to read the ap 'hostname' from the file '%s'!"),
				AP_NODE_HOSTNAME_FILEPATH);
	}

	return return_code;
}

int ACS_DSD_NotificationThread::get_ap_node_id (char * buffer, size_t size) {
	int return_code = read_string_from_file(AP_NODE_ID_FILEPATH, buffer, size);

	if (return_code) {
		*buffer = '\0';
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("ACS_DSD_NotificationThread :  Error trying to read the ap 'ID' from the file '%s'!"),
				AP_NODE_ID_FILEPATH);
	}

	return return_code;
}

int ACS_DSD_NotificationThread::get_partner_ap_node_id (char * buffer, size_t size) {
	int return_code = read_string_from_file(AP_OTHER_NODE_ID_FILEPATH, buffer, size);

	if (return_code) {
		*buffer = '\0';
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("ACS_DSD_NotificationThread :  Error trying to read my partner ap 'ID' from the file '%s'!"),
				AP_OTHER_NODE_ID_FILEPATH);
	}

	return return_code;
}

int ACS_DSD_NotificationThread::read_string_from_file (const char * filename, char * buffer, size_t size) {
	FILE * file = ::fopen(filename, "r");

	if(!file) { // ERROR: opening the file
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_NotificationThread :  Error trying to open file '%s'!"), filename);
		return -1;
	}

	char read_format [32];
	::snprintf(read_format, ACS_DSD_ARRAY_SIZE(read_format), "%%%zus", size - 1);
	read_format[ACS_DSD_ARRAY_SIZE(read_format) - 1] = '\0';

	int nParsedFields = ::fscanf(file, read_format, buffer);

	::fclose(file);

	if(nParsedFields != 1) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("ACS_DSD_NotificationThread :  Error trying to read a string from file '%s'. fscanf() returned < %d > !"),
				filename, nParsedFields);
		return -1;
	}

	return 0;
}
