#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <ace/Guard_T.h>
#include "acs_apbm_programmacros.h"
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_macros.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_sanotifiermanager.h"
extern const std::string NodeStateSu;
extern const std::string NodeStateSi;
namespace {
	const char * sa_amf_ha_state_list [] = {
		"",
		"SA_AMF_HA_ACTIVE",
		"SA_AMF_HA_STANDBY",
		"SA_AMF_HA_QUIESCED",
		"SA_AMF_HA_QUIESCING"
	};
}


SaVersionT __CLASS_NAME__::_sa_version = { releaseCode: 'A', majorVersion: 0x01, minorVersion: 0x01 };

SaNtfHandleT __CLASS_NAME__::_ntf_handle = 0;

SaSelectionObjectT __CLASS_NAME__::_selection_object = 0;

__CLASS_NAME__::internal_state_t __CLASS_NAME__::_internal_state = __CLASS_NAME__::ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED;

char __CLASS_NAME__::_cluster_node_id [128] = {0};
char __CLASS_NAME__::_cluster_other_node_id [128] = {0};

char __CLASS_NAME__::_ap_node_change_state_su [512] = {0};
char __CLASS_NAME__::_ap_other_node_change_state_su [512] = {0};

SaNtfCallbacksT __CLASS_NAME__::_ntf_callbacks = { notification_callback, notification_discardeed_callback };

SaNtfSubscriptionIdT __CLASS_NAME__::_subscription_id = 0;

acs_apbm_serverworkingset * __CLASS_NAME__::_server_working_set = 0;

ACE_Recursive_Thread_Mutex __CLASS_NAME__::_sync_object;


int __CLASS_NAME__::open (acs_apbm_serverworkingset * server_working_set) {
	if (_internal_state != ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS; // Already open or subscribed

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (_internal_state != ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS; // Already open or subscribed

	if (int const call_result = get_cluster_nodes_id()) { // ERROR: retrieving the cluster node id from the system
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_cluster_nodes_id' failed: cannot get cluster nodes id: call_result == %d", call_result);
		return call_result;
	}

	::snprintf(_ap_node_change_state_su, ACS_APBM_ARRAY_SIZE(_ap_node_change_state_su), NodeStateSu.c_str() , _cluster_node_id);
	::snprintf(_ap_other_node_change_state_su, ACS_APBM_ARRAY_SIZE(_ap_other_node_change_state_su), NodeStateSu.c_str() , _cluster_other_node_id);

	SaAisErrorT sa_call_result = SA_AIS_OK;

	// Try to connect the CoreMW NTF service
	if ((sa_call_result = saNtfInitialize(&_ntf_handle, &_ntf_callbacks, &_sa_version)) != SA_AIS_OK) { // ERROR: Initializing NTF
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfInitialize' failed: cannot initialize NTF framework: sa_call_result == %d", sa_call_result);
		return acs_apbm::ERR_NTF_INIT_FAILURE;
	}

	// Getting the selection object to be use in a poll call
	if ((sa_call_result = saNtfSelectionObjectGet(_ntf_handle, &_selection_object)) != SA_AIS_OK) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'saNtfSelectionObjectGet' failed: cannot get the selection object to interact with NTF framework: sa_call_result == %d",
				sa_call_result);
		saNtfFinalize(_ntf_handle);
		_ntf_handle = _selection_object = 0;
		return acs_apbm::ERR_NTF_SELECTION_OBJECT_GET;
	}

	_server_working_set = server_working_set;

	_internal_state = __CLASS_NAME__::ACS_APBM_SA_NOTIFIER_HANDLER_STATE_OPEN;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::close () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// If subscribed then I should unsubscribe
	if (_internal_state == ACS_APBM_SA_NOTIFIER_HANDLER_STATE_SUBSCRIBED) unsubscribe();

	if (_internal_state == ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED) return acs_apbm::ERR_NO_ERRORS; // Already closed

	SaAisErrorT sa_call_result = saNtfFinalize(_ntf_handle);

	if (sa_call_result != SA_AIS_OK) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfFinalize' failed: cannot close the NTF framework: sa_call_result == %d", sa_call_result);
		return acs_apbm::ERR_NTF_FINALIZE_FAILURE;
	}

	_ntf_handle = _selection_object = 0;
	_internal_state = __CLASS_NAME__::ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::subscribe (active_notification_filter_t active_filter, const notification_filter_allocation_params_t * params) {
	if (_internal_state == ACS_APBM_SA_NOTIFIER_HANDLER_STATE_SUBSCRIBED) return acs_apbm::ERR_NO_ERRORS; // Already subscribed
	if (_internal_state == ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED) return acs_apbm::ERR_NTF_NOTIFIER_NOT_OPEN; // Notifier manager not open

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (_internal_state == ACS_APBM_SA_NOTIFIER_HANDLER_STATE_SUBSCRIBED) return acs_apbm::ERR_NO_ERRORS; // Already subscribed
	if (_internal_state == ACS_APBM_SA_NOTIFIER_HANDLER_STATE_CLOSED) return acs_apbm::ERR_NTF_NOTIFIER_NOT_OPEN; // Notifier manager not open

	SaAisErrorT sa_call_result = SA_AIS_OK;
	SaNtfNotificationTypeFilterHandlesT notification_filter_handles;
	::memset(&notification_filter_handles, 0, sizeof(notification_filter_handles));

	// Allocate an 'alarm' notification filter if requested in the active_fiilter parameter
	if (active_filter.all || active_filter.alarm) {
		SaNtfAlarmNotificationFilterT alarm_notification_filter;
		::memset(&alarm_notification_filter, 0, sizeof(alarm_notification_filter));

		sa_call_result = saNtfAlarmNotificationFilterAllocate(_ntf_handle, &alarm_notification_filter,
				params->numEventTypes, params->numNotificationObjects, params->numNotifyingObjects, params->numNotificationClassIds,
				params->numProbableCauses, params->numPerceivedSeverities, params->numTrends);

		if (sa_call_result == SA_AIS_OK) // OK: filter handle allocated
			notification_filter_handles.alarmFilterHandle = alarm_notification_filter.notificationFilterHandle;
		else // ERROR: filter handle not allocated
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfAlarmNotificationFilterAllocate' failed: sa_call_result == %d", sa_call_result);
	}

	// Allocate a 'state change' notification filter if requested in the active_fiilter parameter
	if ((sa_call_result == SA_AIS_OK) && (active_filter.all || active_filter.state_change)) {
		SaNtfStateChangeNotificationFilterT state_change_notification_filter;
		::memset(&state_change_notification_filter, 0, sizeof(state_change_notification_filter));

		sa_call_result = saNtfStateChangeNotificationFilterAllocate(_ntf_handle, &state_change_notification_filter,
				params->numEventTypes, params->numNotificationObjects, params->numNotifyingObjects, params->numNotificationClassIds,
				params->numSourceIndicators, params->numChangedStates);

		if (sa_call_result == SA_AIS_OK) // OK: filter handle allocated
			notification_filter_handles.stateChangeFilterHandle = state_change_notification_filter.notificationFilterHandle;
		else // ERROR: filter handle not allocated
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfStateChangeNotificationFilterAllocate' failed: sa_call_result == %d", sa_call_result);
	}

	// Allocate an 'object create/delete' notification filter if requested in the active_fiilter parameter
	if ((sa_call_result == SA_AIS_OK) && (active_filter.all || active_filter.object_create_delete)) {
		SaNtfObjectCreateDeleteNotificationFilterT object_create_delete_notification_filter;
		::memset(&object_create_delete_notification_filter, 0, sizeof(object_create_delete_notification_filter));

		sa_call_result = saNtfObjectCreateDeleteNotificationFilterAllocate(_ntf_handle, &object_create_delete_notification_filter,
				params->numEventTypes, params->numNotificationObjects, params->numNotifyingObjects, params->numNotificationClassIds,
				params->numSourceIndicators);

		if (sa_call_result == SA_AIS_OK) // OK: filter handle allocated
			notification_filter_handles.objectCreateDeleteFilterHandle = object_create_delete_notification_filter.notificationFilterHandle;
		else // ERROR: filter handle not allocated
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfObjectCreateDeleteNotificationFilterAllocate' failed: sa_call_result == %d", sa_call_result);
	}

	// Allocate an 'attribute change' notification filter if requested in the active_fiilter parameter
	if ((sa_call_result == SA_AIS_OK) && (active_filter.all || active_filter.attribute_change)) {
		SaNtfAttributeChangeNotificationFilterT attribute_change_notification_filter;
		::memset(&attribute_change_notification_filter, 0, sizeof(attribute_change_notification_filter));

		sa_call_result = saNtfAttributeChangeNotificationFilterAllocate(_ntf_handle, &attribute_change_notification_filter,
				params->numEventTypes, params->numNotificationObjects, params->numNotifyingObjects, params->numNotificationClassIds,
				params->numSourceIndicators);

		if (sa_call_result == SA_AIS_OK) // OK: filter handle allocated
			notification_filter_handles.attributeChangeFilterHandle = attribute_change_notification_filter.notificationFilterHandle;
		else // ERROR: filter handle not allocated
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfAttributeChangeNotificationFilterAllocate' failed: sa_call_result == %d", sa_call_result);
	}

	// Allocate an 'security alarm' notification filter if requested in the active_fiilter parameter
	if ((sa_call_result == SA_AIS_OK) && (active_filter.all || active_filter.security_alarm)) {
		SaNtfSecurityAlarmNotificationFilterT security_alarm_notification_filter;
		::memset(&security_alarm_notification_filter, 0, sizeof(security_alarm_notification_filter));

		sa_call_result = saNtfSecurityAlarmNotificationFilterAllocate(_ntf_handle, &security_alarm_notification_filter,
				params->numEventTypes, params->numNotificationObjects, params->numNotifyingObjects, params->numNotificationClassIds,
				params->numProbableCauses, params->numSeverities, params->numSecurityAlarmDetectors, params->numServiceUsers,
				params->numServiceProviders);

		if (sa_call_result == SA_AIS_OK) // OK: filter handle allocated
			notification_filter_handles.securityAlarmFilterHandle = security_alarm_notification_filter.notificationFilterHandle;
		else // ERROR: filter handle not allocated
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfSecurityAlarmNotificationFilterAllocate' failed: sa_call_result == %d", sa_call_result);
	}

	int return_code = acs_apbm::ERR_NO_ERRORS;

	// If all ok I can subscribe to NTF notification events
	if (sa_call_result == SA_AIS_OK) {
		if ((sa_call_result = saNtfNotificationSubscribe(&notification_filter_handles, ++_subscription_id)) == SA_AIS_OK) {
			// OK: process subscribed successfully
			_internal_state = __CLASS_NAME__::ACS_APBM_SA_NOTIFIER_HANDLER_STATE_SUBSCRIBED;

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SA notification manager correctly subscribed to NTF");
		} else { // ERROR
			return_code = acs_apbm::ERR_NTF_SUBSCRIPTION_FAILURE;
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfNotificationSubscribe' failed: cannot subscribe: sa_call_result == %d", sa_call_result);
		}
	} else return_code = acs_apbm::ERR_NTF_FILTER_ALLOCATION_FAILURE;

	// Now I can free all the allocated filter objects
	notification_filter_free(notification_filter_handles.alarmFilterHandle);
	notification_filter_free(notification_filter_handles.stateChangeFilterHandle);
	notification_filter_free(notification_filter_handles.objectCreateDeleteFilterHandle);
	notification_filter_free(notification_filter_handles.attributeChangeFilterHandle);
	notification_filter_free(notification_filter_handles.securityAlarmFilterHandle);

	return return_code;
}

int __CLASS_NAME__::unsubscribe () {
	if (_internal_state != ACS_APBM_SA_NOTIFIER_HANDLER_STATE_SUBSCRIBED) return acs_apbm::ERR_NO_ERRORS; // Already unsubscribed

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (_internal_state != ACS_APBM_SA_NOTIFIER_HANDLER_STATE_SUBSCRIBED) return acs_apbm::ERR_NO_ERRORS; // Already unsubscribed

	SaAisErrorT sa_call_result = SA_AIS_OK;

	if ((sa_call_result = saNtfNotificationUnsubscribe(_subscription_id)) != SA_AIS_OK) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfNotificationUnsubscribe' failed: cannot unsubscribe from NTF event notification: sa_call_result == %d", sa_call_result);
		return acs_apbm::ERR_NTF_UNSUBSCRIPTION_FAILURE;
	}

	_internal_state = __CLASS_NAME__::ACS_APBM_SA_NOTIFIER_HANDLER_STATE_OPEN;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::get_cluster_nodes_id () {
	if (*_cluster_node_id) return acs_apbm::ERR_NO_ERRORS;

	FILE * f = 0;

	errno = 0;
	if (!(f = ::fopen(acs_apbm_programconfiguration::cluster_node_id_filepath, "r"))) { // ERROR: opening the file
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen' failed: cannot read my cluster node id from file '%s'", acs_apbm_programconfiguration::cluster_node_id_filepath);
		return acs_apbm::ERR_OPEN_CALL;
	}

	char cluster_node_id [128] = {0};
	errno = 0;
	int call_result = ::fscanf(f, "%127s", cluster_node_id);
	int errno_save = errno;

	::fclose(f);

	if (call_result != 1) { // Error: reading the info
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'fscanf' failed: cannot read my cluster node id: call_result == %d", call_result);
		return acs_apbm::ERR_READ_CALL;
	}

	::memccpy(_cluster_node_id, cluster_node_id, 0, ACS_APBM_ARRAY_SIZE(_cluster_node_id));
	_cluster_node_id[ACS_APBM_ARRAY_SIZE(_cluster_node_id) - 1] = 0;

	// get other node id
	errno = 0;
        f = 0;
	if (!(f = ::fopen(acs_apbm_programconfiguration::cluster_other_node_id_filepath, "r"))) { // ERROR: opening the file
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen' failed: cannot read other cluster node id from file '%s'", acs_apbm_programconfiguration::cluster_other_node_id_filepath);
		return acs_apbm::ERR_OPEN_CALL;
	}
	char cluster_other_node_id [128] = {0};
	errno = 0;
	call_result = ::fscanf(f, "%127s", cluster_other_node_id);
	errno_save = errno;

	::fclose(f);

	if (call_result != 1) { // Error: reading the info
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'fscanf' failed: cannot read other cluster node id: call_result == %d", call_result);
		return acs_apbm::ERR_READ_CALL;
	}

	::memccpy(_cluster_other_node_id, cluster_other_node_id, 0, ACS_APBM_ARRAY_SIZE(_cluster_other_node_id));
	_cluster_other_node_id[ACS_APBM_ARRAY_SIZE(_cluster_other_node_id) - 1] = 0;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::notification_filter_free (SaNtfNotificationFilterHandleT handle) {
	SaAisErrorT sa_call_result = (handle ? saNtfNotificationFilterFree(handle) : SA_AIS_OK);

	if (sa_call_result != SA_AIS_OK)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'saNtfNotificationFilterFree' failed: freeing the handle 0x%08lX", handle);

	return (sa_call_result == SA_AIS_OK ? acs_apbm::ERR_NO_ERRORS : acs_apbm::ERR_NTF_FILTER_FREE_FAILURE);
}

void __CLASS_NAME__::notification_callback (SaNtfSubscriptionIdT subscription_id, const SaNtfNotificationsT * notification) {
	SaNtfNotificationHandleT notification_handle;
	const SaNtfNotificationHeaderT * notification_header;
	char notification_object [SA_MAX_NAME_LENGTH + 1] = {0};
	char additional_info[SA_MAX_NAME_LENGTH + 1]= {0};
	SaNtfStateChangeT * ntf_state_change = 0;

	if(notification->notification.stateChangeNotification.numStateChanges <= 0){
		ACS_APBM_LOG(LOG_LEVEL_WARN,
											"APBM server has received invalid node-state-change notification. Bad size! It will be ignored.");
			return;
	}

	if (!_server_working_set->main_reactor->reactor_event_loop_done() && notification->notificationType == SA_NTF_TYPE_STATE_CHANGE) {
		// We are interested in the state change notifications only
		notification_handle = notification->notification.stateChangeNotification.notificationHandle;
		notification_header = &notification->notification.stateChangeNotification.notificationHeader;
		::strncpy(notification_object, reinterpret_cast<const char *>(notification_header->notificationObject->value), notification_header->notificationObject->length);
		notification_object[notification_header->notificationObject->length] = '\0';

		// Get Additional Info associated to the notification
		if ( notification_header->additionalInfo != NULL ) {
			if ( notification_header->additionalInfo[0].infoType == SA_NTF_VALUE_LDAP_NAME) {
				SaNameT *dataPtr;
				SaUint16T dataSize;
				SaAisErrorT rc;

				rc = saNtfPtrValGet( notification_handle, &notification_header->additionalInfo[0].infoValue, (void **)&dataPtr, &dataSize );

				if (rc == SA_AIS_OK){
					strncpy( additional_info, (char*)dataPtr->value, dataPtr->length);
				}
			}
		}
	//	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "State change notification detected! notificationObject = %s, additionalInfo =%s", notification_object, additional_info);


		ntf_state_change = & notification->notification.stateChangeNotification.changedStates[0];
		bool is_local_node_notif = !::strcmp(_ap_node_change_state_su, notification_object);
		bool is_other_node_notif = !::strcmp(_ap_other_node_change_state_su, notification_object);


		// check if the STATE_CHANGE is of type "SA_AMF_HA_STATE" and if the notification is related to this AP node
		// or other AP node since sometime local notification is missed
		if( (ntf_state_change->stateId == SA_AMF_HA_STATE) &&
				 (is_local_node_notif || is_other_node_notif) && strstr(additional_info, NodeStateSi.c_str()) != NULL){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "HA state transition detected for AP node %s: subscription_id == %u: new state is '%s'", is_local_node_notif ? _cluster_node_id : _cluster_other_node_id,
					subscription_id, sa_amf_ha_state_list[ntf_state_change->newState]);
			// Check if local AP Node has to change working in active node
			if ((is_local_node_notif && ntf_state_change->newState == SA_AMF_HA_ACTIVE) || // this AP node has become ACTIVE
					(is_other_node_notif &&
						(
							(ntf_state_change->newState == SA_AMF_HA_STANDBY)   || 	/* other AP Node has become PASSIVE */
							(ntf_state_change->newState == SA_AMF_HA_QUIESCED)  ||  /* other AP Node entered QUIESCED state */
							(ntf_state_change->newState == SA_AMF_HA_QUIESCING) 	/* other AP Node entered QUIESCING state */
						)
					)
				) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "State change notification detected! is_local_node_notif == %d is_other_node_notif == %d", is_local_node_notif, is_other_node_notif);

				ACS_APBM_LOG(LOG_LEVEL_FATAL, "APBM CURRENT STATE == %d", _server_working_set->program_state);

				// Check: if we are quiesced we should ignore this notification
				if (_server_working_set->program_state == acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED)
					ACS_APBM_LOG(LOG_LEVEL_WARN,
							"APBM server is in QUIESCED state so this node-state-change notification was ignored");
				else {
					// Order a 'STOP_WORK_TO_CHANGE_STATE_TO_ACTIVE' operation to change working in active node
					if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_ACTIVE))
						ACS_APBM_LOG(LOG_LEVEL_ERROR,
								"Call 'operation_send' failed: cannot order a STOP_WORK_TO_CHANGE_STATE_TO_ACTIVE operation to change working in active node");
					else ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM Server is changing to work in ACTIVE node");
				}
			}
			// Check if local AP Node has to change working in passive node
			else if( (is_other_node_notif && (ntf_state_change->newState == SA_AMF_HA_ACTIVE)) || // other AP node has become ACTIVE
					   (is_local_node_notif &&
							 (
							   (ntf_state_change->newState == SA_AMF_HA_STANDBY) ||		/* this AP Node has become PASSIVE */
							   (ntf_state_change->newState == SA_AMF_HA_QUIESCED)||		/* this AP Node entered QUIESCED state */
							   (ntf_state_change->newState == SA_AMF_HA_QUIESCING)		/* this AP Node entered QUIESCING state */
							 )
						)
				) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "State change notification detected! is_local_node_notif == %d is_other_node_notif == %d", is_local_node_notif, is_other_node_notif);

				ACS_APBM_LOG(LOG_LEVEL_FATAL, "APBM CURRENT STATE == %d", _server_working_set->program_state);

				// Check: if we are quiesced we should ignore this notification
				if (_server_working_set->program_state == acs_apbm::PROGRAM_STATE_RUNNING_QUIESCED)
					ACS_APBM_LOG(LOG_LEVEL_WARN,
							"APBM server is in QUIESCED state so this node-state-change notification was ignored");
				else {
					// Order a 'STOP_WORK_TO_CHANGE_STATE_TO_PASSIVFE' operation to change working in passive node
					if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_STOP_WORK_TO_CHANGE_STATE_TO_PASSIVE))
						ACS_APBM_LOG(LOG_LEVEL_ERROR,
									"Call 'operation_send' failed: cannot order a STOP_WORK_TO_CHANGE_STATE_TO_PASSIVE operation to change working in passive node");
					else  ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM Server is changing to work in PASSIVE node");
				}
			}
		}
	}

	switch (notification->notificationType) {
	case SA_NTF_TYPE_ALARM: saNtfNotificationFree(notification->notification.alarmNotification.notificationHandle); break;
	case SA_NTF_TYPE_SECURITY_ALARM: saNtfNotificationFree(notification->notification.securityAlarmNotification.notificationHandle); break;
	case SA_NTF_TYPE_STATE_CHANGE: saNtfNotificationFree(notification->notification.stateChangeNotification.notificationHandle); break;
	case SA_NTF_TYPE_OBJECT_CREATE_DELETE: saNtfNotificationFree(notification->notification.objectCreateDeleteNotification.notificationHandle); break;
	case SA_NTF_TYPE_ATTRIBUTE_CHANGE: saNtfNotificationFree(notification->notification.attributeChangeNotification.notificationHandle); break;
	default: break;
	}
}

void __CLASS_NAME__::notification_discardeed_callback (
		SaNtfSubscriptionIdT subscription_id,
		SaNtfNotificationTypeT notification_type,
		SaUint32T number_discarded,
		const SaNtfIdentifierT * /*discarded_notification_identifiers*/) {

	// Nothing to do: only logging
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NOTIFICATION-DISCARDEED-CALLBACK called: subscription_id == %u, notification_type == %d, number_discarded == %u",
			subscription_id, notification_type, number_discarded);
}
