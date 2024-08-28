#include "ace/Guard_T.h"

#include "acs_aca_logger.h"
#include "acs_aca_msd_service.h"
#include "acs_aca_configuration_helper.h"
#include "acs_aca_cptable_observer.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_ACA_CPTable_Observer

int __CLASS_NAME__::start (ACS_CS_API_SubscriptionMgr * subscription_manager) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_obj);

	if (_state == OBSERVER_STATE_STARTED) {	// ERROR: The observer is already connected
		ACS_ACA_LOG(LOG_LEVEL_WARN, "The CpTable observer was already started!");
		return ERROR_ALREADY_CONNECTED;
	}

	subscription_manager || (subscription_manager = ACS_CS_API_SubscriptionMgr::getInstance());
	if (!subscription_manager) {	// ERROR: Failed to get the subscription manager from CS API
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACS_CS_API_SubscriptionMgr::getInstance()' failed: "
				"cannot get the subscription manager from CS API!");
		return ERROR_CS_GET_INSTANCE;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Subscribing ACA server to CS CP table change notification interface...");

	ACS_CS_API_NS::CS_API_Result cs_call_result = subscription_manager->subscribeCpTableChanges(*this);

	if (cs_call_result != ACS_CS_API_NS::Result_Success) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'subscribeCpTableChanges()' failed: cs_call_result == %d", cs_call_result);
		return ERROR_CS_SUBSCRIPTION_FAILED;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "ACA server successfully subscribed to CS CP table change notification interface!");
	_state = OBSERVER_STATE_STARTED;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ERROR_NO_ERROR;
}

int __CLASS_NAME__::stop (ACS_CS_API_SubscriptionMgr * subscription_manager) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_obj);

	if (_state == OBSERVER_STATE_STOPPED) {	// ERROR: The observer is already disconnected
		ACS_ACA_LOG(LOG_LEVEL_WARN, "The CpTable observer was already stopped!");
		return ERROR_ALREADY_DISCONNECTED;
	}

	subscription_manager || (subscription_manager = ACS_CS_API_SubscriptionMgr::getInstance());
	if (!subscription_manager) {	// ERROR: Failed to get the subscription manager from CS API
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACS_CS_API_SubscriptionMgr::getInstance()' failed: "
				"cannot get the subscription manager from CS API!");
		return ERROR_CS_GET_INSTANCE;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Unsubscribing ACA server from CS CP table change notification interface...");

	ACS_CS_API_NS::CS_API_Result cs_call_result = subscription_manager->unsubscribeCpTableChanges(*this);

	if (cs_call_result != ACS_CS_API_NS::Result_Success) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'unsubscribeCpTableChanges()' failed: cs_call_result == %d", cs_call_result);
		return ERROR_CS_UNSUBSCRIPTION_FAILED;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "ACA server successfully unsubscribed from CS CP table change notification interface!");
	_state = OBSERVER_STATE_STOPPED;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ERROR_NO_ERROR;
}

void __CLASS_NAME__::update (const ACS_CS_API_CpTableChange & observee) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int number_of_notifications = observee.dataSize;
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Received %d CP Table change notifications!", number_of_notifications);

	for (int i = 0; i < number_of_notifications; i++) {
		ACS_CS_API_CpTableData cp_data = observee.cpData[i];
		unsigned cp_id = cp_data.cpId;
		ACS_CS_API_TableChangeOperation::OpType op_type = cp_data.operationType;

		ACS_ACA_LOG(LOG_LEVEL_INFO, "Received a notification for the operation %d for the CP having system id = %u", op_type, cp_id);

		switch (op_type) {
		case ACS_CS_API_TableChangeOperation::Add:
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Notifying the presence of the new BC (with system id = %u) to the main thread...", cp_id);
			ACS_MSD_Service::notify_cptable_change(cp_id, BLADE_ADD);
			break;

		case ACS_CS_API_TableChangeOperation::Delete:
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Notifying the deletion of the BC (with system id = %u) to the main thread...", cp_id);
			ACS_MSD_Service::notify_cptable_change(cp_id, BLADE_REMOVE);
			break;

		case ACS_CS_API_TableChangeOperation::Change:
		case ACS_CS_API_TableChangeOperation::Unspecified:
		default:
			ACS_ACA_LOG(LOG_LEVEL_WARN, "The operation type received for the CP having system id = %u is %d, "
					"i'll ignore this notification!", cp_id, op_type);
			break;
		}
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Update callback received, forcing the configuration reload!");
	ACS_ACA_ConfigurationHelper::force_configuration_reload();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}
