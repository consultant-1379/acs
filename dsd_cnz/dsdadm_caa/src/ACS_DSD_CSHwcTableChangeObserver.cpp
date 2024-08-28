#include "ace/Guard_T.h"

#include "ACS_DSD_Client.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_Logger_v2.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_SrvProt_HwcChange_Primitives.h"
#include "ACS_DSD_CSHwcTableChangeObserver.h"

extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_StartupSessionsGroup * startup_session_group;
extern ACS_DSD_CpNodesManager cpNodesManager;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_CSHwcTableChangeObserver

void __CLASS_NAME__::update (const ACS_CS_API_HWCTableChange & observee) {
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s::%s: CS HWC table change notification received!"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

	for (int i = 0; i < observee.dataSize; i++) {
		const ACS_CS_API_HWCTableData_R1 & hwc_item = observee.hwcData[i];
		int call_result = 0;

		switch (hwc_item.fbn) {
		case FBN_APUB:
			call_result = handle_apub_board_notification(hwc_item);
			break;

		case FBN_CPUB:
			call_result = handle_cpub_board_notification(hwc_item);
			break;

		default:	// FBN != APUB && FBN != CPUB
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Received a notification for a "
					"NOT APUB and a NOT CPUB board, nothing to do!"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		}

		if (call_result) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Call 'handle_XXub_board_notification' failed, call_result = %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
		}
	}
}

int __CLASS_NAME__::start (ACS_CS_API_SubscriptionMgr * subscription_manager)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if ( _state == OBSERVER_STATE_STARTED )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN,
				ACS_DSD_TEXT ("%s::%s: Observer already started!"),	ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		return TCO_ERROR_NO_ERROR;
	}

	// Getting the subscription manager if not provided into the subscription_manager parameter
	subscription_manager || (subscription_manager = ACS_CS_API_SubscriptionMgr::getInstance());

	if ( !subscription_manager )
	{
		// ERROR: getting the subscription manager from CS API
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT ("%s::%s: Call 'getInstance' failed: cannot get the subscription manager from CS API"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		return TCO_ERROR_CS_GET_INSTANCE;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
			ACS_DSD_TEXT ("%s::%s: Subscribing DSD server to the CS HWC table change notification interface..."),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

	ACS_CS_API_NS::CS_API_Result cs_call_result = subscription_manager->subscribeHWCTableChanges(*this);

	if ( cs_call_result != ACS_CS_API_NS::Result_Success )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT ("%s::%s: Call 'subscribeHWCTableChanges' failed: cs_call_result == %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, cs_call_result);
		return TCO_ERROR_CS_ERROR;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
			ACS_DSD_TEXT ("%s::%s: DSD server successfully subscribed to the CS HWC table change notification interface"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

	_state = OBSERVER_STATE_STARTED;

	return TCO_ERROR_NO_ERROR;
}


int __CLASS_NAME__::stop (ACS_CS_API_SubscriptionMgr * subscription_manager)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if ( _state == OBSERVER_STATE_STOPPED )
		return TCO_ERROR_NO_ERROR;

	// Getting the subscription manager if not provided into the subscription_manager parameter
	subscription_manager || (subscription_manager = ACS_CS_API_SubscriptionMgr::getInstance());

	if ( !subscription_manager )
	{
		// ERROR: getting the subscription manager from CS API
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT ("%s::%s: Call 'getInstance' failed: cannot get the subscription manager from CS API"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		return TCO_ERROR_CS_GET_INSTANCE;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
			ACS_DSD_TEXT ("%s::%s: Unsubscribing DSD server to the CS HWC table change notification interface..."),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

	ACS_CS_API_NS::CS_API_Result cs_call_result = subscription_manager->unsubscribeHWCTableChanges(*this);

	if ( cs_call_result != ACS_CS_API_NS::Result_Success )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT ("%s::%s: Call 'unsubscribeHWCTableChanges' failed: cs_call_result == %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, cs_call_result);
		return TCO_ERROR_CS_ERROR;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
			ACS_DSD_TEXT ("%s::%s: DSD server successfully unsubscribed to the CS HWC table change notification interface"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

	_state = OBSERVER_STATE_STOPPED;

	return TCO_ERROR_NO_ERROR;
}

int __CLASS_NAME__::handle_apub_board_notification (const ACS_CS_API_HWCTableData_R1 & hwc_item) {
	int op_type = hwc_item.operationType;
	uint16_t system_type = hwc_item.sysType;
	uint16_t system_id = (acs_dsd::CONFIG_AP_ID_SET_START + hwc_item.sysNo);
	int8_t side = hwc_item.side;

	return handle_apub_board_notification(op_type, system_type, system_id, side, true);
}

int __CLASS_NAME__::handle_apub_board_notification (uint8_t op_type, uint16_t system_type, int32_t system_id, int8_t side, bool send_notification_to_passive) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_sync_object);

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Received a notification regarding a APUB board, "
			"op_type = %d, system_type = %u, system_id = %u"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
			op_type, system_type, system_id);

	ACS_DSD_ConfigurationHelper::force_configuration_reload();
	startup_session_group->close(1);
	startup_session_group->system_configuration(ACS_DSD_StartupSessionsGroup::SYS_CONF_UNKNOWN);

	if (send_notification_to_passive)
		return send_board_notification_to_passive_node(FBN_APUB, op_type, system_type, system_id, side);

	return TCO_ERROR_NO_ERROR;
}

int __CLASS_NAME__::handle_cpub_board_notification (const ACS_CS_API_HWCTableData_R1 & hwc_item) {
	int op_type = hwc_item.operationType;
	uint16_t system_type = hwc_item.sysType;
	uint16_t system_id = (system_type == CS_SYSTEM_TYPE_BC) ? hwc_item.seqNo : (hwc_item.sysNo + acs_dsd::CONFIG_CP_ID_SET_START);
	int8_t side = (system_type == CS_SYSTEM_TYPE_BC) ? static_cast<int8_t>(acs_dsd::NODE_SIDE_UNDEFINED): hwc_item.side;

	return handle_cpub_board_notification(op_type, system_type, system_id, side, true);
}

int __CLASS_NAME__::handle_cpub_board_notification (uint8_t op_type, uint16_t system_type, int32_t system_id, int8_t side, bool send_notification_to_passive) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_sync_object);
	bool new_master_to_be_chosen = false;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Received a notification regarding a CPUB board, "
			"op_type = %d, system_type = %u, system_id = %u"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
			op_type, system_type, system_id);

	// Understand the type of notification received, in order to execute the correct operation
	switch (op_type) {

	// When a new BC/CP is added to the configuration, no other operations are needed.
	case OP_ADD:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: The CP with system_id = %u has been added!"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);

		// If the new CP could be the alarm/clock master CP, a new master must be chosen.
		if ((system_type == CS_SYSTEM_TYPE_CP) && (system_id < cpNodesManager.get_alarm_master_cp_system_id()) && (system_id < cpNodesManager.get_clock_master_cp_system_id())) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: The CP with system_id = %u should be the alarm/clock "
					"master!"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);

			new_master_to_be_chosen = true;
		}
		break;

		// When a BC/CP is deleted from the configuration, close all the session for the deleted BC/CP.
		// If the alarm/clock master CP is deleted, a new alarm/clock master CP must be chosen.
	case OP_DELETE: {
		int call_result = 0;
		char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};
		ACS_DSD_ConfigurationHelper::system_id_to_system_name(system_id, node_name, ACS_DSD_ARRAY_SIZE(node_name));

		if (system_type == CS_SYSTEM_TYPE_CP) {
			char tmp[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};
			::strncpy(tmp, node_name, ::strlen(node_name));
			::snprintf(node_name, ACS_DSD_ARRAY_SIZE(node_name), "%s%c", tmp, (side == acs_dsd::NODE_SIDE_A) ? 'A' : 'B');
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: The CP '%s' has been deleted, removing it "
				"from CP Nodes Manager collection!"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name);

		// Close all the sessions for the given BC/CP
		if ((call_result = cpNodesManager.close_cp_conn(node_name))) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s::%s: Call 'close_cp_conn()' failed, probably the "
					"CP wasn't connected, call_result = %d"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
		}

		// If the alarm/clock master CP has been deleted, a new master must be chosen.
		if ((system_type == CS_SYSTEM_TYPE_CP) && (system_id == cpNodesManager.get_alarm_master_cp_system_id()) && (system_id == cpNodesManager.get_clock_master_cp_system_id())) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: The CP '%s' (system_id = %u) was the alarm/clock "
					"master, a new master must be chosen!"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name, system_id);

			new_master_to_be_chosen = true;
		}
		break;
	}

	case OP_CHANGE:
	default:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Nothing to do for CP with system_id = %u (op_type = %d)!"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id, op_type);
		break;
	}

	// For each HWC table change notification regarding a CPUB board, reset the loaded configuration.
	ACS_DSD_ConfigurationHelper::force_configuration_reload();

	int call_result = 0;
	if ((new_master_to_be_chosen) && (call_result = cpNodesManager.choose_master_cp())) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Call 'choose_master_cp()' failed, call_result = %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
		return TCO_ERROR_MASTER_CP_SET_FAILED;
	}

	if (send_notification_to_passive)
		return send_board_notification_to_passive_node(FBN_CPUB, op_type, system_type, system_id, side);

	return TCO_ERROR_NO_ERROR;
}

int __CLASS_NAME__::send_board_notification_to_passive_node (uint16_t fbn, uint8_t op_type, uint16_t system_type, int32_t system_id, int8_t side) {
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Sending a primitive to notify the passive node about "
			"the HWC Table change - fbn = %u, op_type = %u, system_type = %u, system_id = %u"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, fbn, op_type, system_type, system_id);

	int call_result = 0;
	ACS_DSD_Node node_info;
	std::string error_descr;

	// FIRST: get the node name of the partner (PASSIVE) node
	if ((call_result = ACS_DSD_Utils::get_partner_node_info(node_info, error_descr))) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Call 'ACS_DSD_Utils::get_node_info()' failed! - "
				"call_result = %d, error_message = '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result, error_descr.c_str());
		return TCO_ERROR_GET_PARTNER_NODE_FAILED;
	}

	ACS_DSD_Client client;
	ACS_DSD_SrvProt_SvcHandler service_handler;

	// SECOND: connect to the partner (PASSIVE) node to send the primitive
	if ((call_result = client.connect(service_handler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, node_info.system_id, node_info.node_name, DSD_SERVER_CONNECT_TIMEOUT_VALUE))) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Call 'connect' failed! - call_result = %d, error_code = %d, error_message = '%s'"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result, client.last_error(), client.last_error_text());
		return TCO_ERROR_CONNECT_FAILED;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Established connection to the DSD server on AP "
			"having node_name <%s>. CONN_ID = <%u>"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_info.node_name, service_handler.getID());

	// THIRD: prepare and send the primitive to the partner (PASSIVE) node
	SrvProt_HwcChange_notify_v1_request req_primitive;
	req_primitive.set_fbn(fbn);
	req_primitive.set_op_type(op_type);
	req_primitive.set_system_type(system_type);
	req_primitive.set_system_id(system_id);
	req_primitive.set_side(side);

	if ((call_result = service_handler.sendPrimitive(&req_primitive, DSD_SERVER_SEND_TIMEOUT_VALUE))) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Unable to send the primitive! call_result = %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
		return TCO_ERROR_SEND_FAILED;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: CONN_ID = <%u>. The HWC CHANGE NOTIFY primitive (v1) "
			"has been sent to AP <%s>. Primitive sent: '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
			service_handler.getID(), node_info.node_name, req_primitive.to_text().c_str());

	// FOURTH: receive the response primitive from the partner (PASSSIVE) node and check the error code
	ACS_DSD_ServicePrimitive * p_resp_prim = 0;
	if ((call_result = service_handler.recvPrimitive(p_resp_prim, DSD_SERVER_RECV_TIMEOUT_VALUE))) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Unable to receive the primitive! call_result = %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
		return TCO_ERROR_RECV_FAILED;
	}

	SrvProt_HwcChange_notify_v1_reply * p_hwc_change_reply = dynamic_cast<SrvProt_HwcChange_notify_v1_reply *> (p_resp_prim);
	if (p_hwc_change_reply->get_error_field() != 0) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s::%s: Received the primitive <%d> with the error code <%d>"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, p_hwc_change_reply->get_primitive_id(), p_hwc_change_reply->get_error_field());
		return TCO_ERROR_PRIMITIVE_WITH_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Received correctly the primitive <%d>. Primitive received: '%s'"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, p_hwc_change_reply->get_primitive_id(), p_hwc_change_reply->to_text().c_str());

	service_handler.close();
	delete p_hwc_change_reply;
	return TCO_ERROR_NO_ERROR;
}
