#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_Logger_v2.h"
#include "ACS_DSD_CSHwcTableChangeObserver.h"
#include "ACS_DSD_SrvProt_HwcChange_Primitives.h"

extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_CSHwcTableChangeObserver configuration_change_observer;

SrvProt_HwcChange_notify_v1_request::SrvProt_HwcChange_notify_v1_request () {
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_HWC_CHANGE_NOTIFY;
	_fbn = ACS_DSD_CSHwcTableChangeObserver::FBN_UNKNOWN;
	_op_type = ACS_DSD_CSHwcTableChangeObserver::OP_UNSPECIFIED;
	_system_type = ACS_DSD_CSHwcTableChangeObserver::CS_SYSTEM_TYPE_UNKNOWN;
	_system_id = acs_dsd::SYSTEM_ID_UNKNOWN;
	_side = acs_dsd::NODE_SIDE_UNDEFINED;
}

std::string SrvProt_HwcChange_notify_v1_request::to_text () const {
	char primitive_text[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN] = {0};

	::snprintf(primitive_text, ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u, VERSION = %u, FBN = %u, OP_TYPE = %u, SYSTEM_TYPE = %u, SYSTEM_ID = %u, SIDE = %d",
				_primitive_id, _protocol_version, _fbn, _op_type, _system_type, _system_id, _side);

	return primitive_text;
}

int SrvProt_HwcChange_notify_v1_request::pack_into_primitive_data_handler (ACS_DSD_PrimitiveDataHandler<> & pdh) const {
	return pdh.make_primitive(SRVSRV_HWC_CHANGE_NOTIFY, SRVSRV_PROT_V1, _fbn, _op_type, _system_type, _system_id, _side);
}

int SrvProt_HwcChange_notify_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh) {
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_fbn, &_op_type, &_system_type, &_system_id, &_side);
}

int SrvProt_HwcChange_notify_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler * /*service_handler*/) const {
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_HwcChange_notify_v1_request::process()";
#endif
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, "%s: Received a board change notification! - fbn = %u, op_type = %d, "
			"system_type = %u, system_id = %d, side = %d", FX_NAME, _fbn, _op_type, _system_type, _system_id, _side);

	uint8_t primitive_error_code = 0;
	int call_result = 0;

	// Understand the right operation to be executed to handle the board change notification
	switch (_fbn) {
	case ACS_DSD_CSHwcTableChangeObserver::FBN_APUB:
		call_result = configuration_change_observer.handle_apub_board_notification(_op_type, _system_type, _system_id, _side, false);
		break;

	case ACS_DSD_CSHwcTableChangeObserver::FBN_CPUB:
		call_result = configuration_change_observer.handle_cpub_board_notification(_op_type, _system_type, _system_id, _side, false);
		break;

	default:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, "%s: Received a board change notification for a not APUB and CPUB board, "
				"very strange, returning a primitive with error!", FX_NAME);
		primitive_error_code = 1;
	}

	SrvProt_HwcChange_notify_v1_reply * p_reply_prim = new (std::nothrow) SrvProt_HwcChange_notify_v1_reply();
	if (!p_reply_prim) {
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	if (call_result) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s: Call 'handle_XXub_board_notification' failed, call_result = %d"), FX_NAME, call_result);
		primitive_error_code = 1;
	}

	p_reply_prim->set_error_field(primitive_error_code);
	response_primitive = p_reply_prim;
	return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
}

/**********************************************************************************************************************/

SrvProt_HwcChange_notify_v1_reply::SrvProt_HwcChange_notify_v1_reply () {
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_HWC_CHANGE_NOTIFY_REPLY;
	_error = 0;
}

std::string SrvProt_HwcChange_notify_v1_reply::to_text() const {
	char primitive_text[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN] = {0};

	::snprintf(primitive_text, ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
			"PRIMITIVE_ID = %u, VERSION = %u, ERROR = %u", _primitive_id, _protocol_version, _error);

	return primitive_text;
}

int SrvProt_HwcChange_notify_v1_reply::pack_into_primitive_data_handler (ACS_DSD_PrimitiveDataHandler<> & pdh) const {
	return pdh.make_primitive(SRVSRV_HWC_CHANGE_NOTIFY_REPLY, SRVSRV_PROT_V1, _error);
}

int SrvProt_HwcChange_notify_v1_reply::build_from_primitive_data_handler (const ACS_DSD_PrimitiveDataHandler<> & pdh) {
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error);
}

int SrvProt_HwcChange_notify_v1_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const {
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}
