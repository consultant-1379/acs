#include <memory>

#include "ACS_DSD_Client.h"
#include "ACS_DSD_CpProt_AddressPrimitives.h"
#include "ACS_DSD_AppProt_QueryExchangeCpContext.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_AppProt_QueryExchangeCpContext

int __CLASS_NAME__::activate (
		ACE_Reactor & reactor,
		ACE_Event_Handler * event_handler,
		ACS_DSD_ServiceHandler * service_handler,
		const char * service_name,
		const char * service_domain,
		int32_t system_id,
		const char * node_name,
		acs_dsd::NodeStateConstants node_state,
		/*ACS_DSD_Node & node_to_connect,*/
		const uint32_t * timeout_ms) {
	_service_handler = service_handler;

	_query_response.system_id(system_id);
	_query_response.node_state(node_state);
	_query_response.error_field(acs_dsd::PCP_ERROR_CODE_HOST_UNREACHABLE); //We suppose a negative result

	int call_result = 0;
	bool send_negative_response = true; //We suppose a negative result
	ACS_DSD_Client connector;

	do {
		call_result = timeout_ms
				? connector.connect(_cp_prot_svc_handler.peer(), ACS_DSD_CONFIG_CPAP_INET_SOCKET_SAP_ID,
						system_id, node_name, *timeout_ms)
				: connector.connect(_cp_prot_svc_handler.peer(), ACS_DSD_CONFIG_CPAP_INET_SOCKET_SAP_ID,
						system_id, node_name);

		if (call_result) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s: CONN_ID = < %u >. Error connecting to CP with systemId=%d, node_name =%s "
							"- erroCode = < %d >. Description : < %s >"), __PRETTY_FUNCTION__,
							_service_handler->getID(), system_id, node_name,
							call_result, connector.last_error_text());
			break;
		}

		CpProt_address_request apap_address_request;
		apap_address_request.set_node_name(node_name);
		apap_address_request.set_service_name(service_name);
		apap_address_request.set_service_domain(service_domain);

		if ((call_result = _cp_prot_svc_handler.sendPrimitive(&apap_address_request, DSD_SERVER_SEND_TIMEOUT_VALUE))) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s: CONN_ID = < %u >. Error sending APAP address request [call_result == %d]: "
							"query for node '%s'"), __PRETTY_FUNCTION__, _cp_prot_svc_handler.getID(), call_result,
							node_name);
			break;
		}

		if (reactor.register_handler(event_handler, ACE_Event_Handler::READ_MASK) < 0) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s: error registering the query exchange event handler onto the internal "
							"query response reactor: query for node '%s'"), __PRETTY_FUNCTION__, node_name);

			_cp_prot_svc_handler.peer().close();
			break;
		}

		send_negative_response = false; //All right
	} while (0);

	if (send_negative_response) {
		if ((call_result = _service_handler->sendPrimitive(&_query_response, DSD_SERVER_SEND_TIMEOUT_VALUE)) < 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s: CONN_ID = < %u >. 'sendPrimitive(...' failed to send response to client "
							"[call_result == %d]: query for node '%s'"), __PRETTY_FUNCTION__, _service_handler->getID(),
							call_result, node_name);
		else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
					ACS_DSD_TEXT("%s: CONN_ID = < %d >. The QUERY RESPONSE primitive has been sent to client. "
							"Primitive data == '%s'"), __PRETTY_FUNCTION__, _service_handler->getID(),
							_query_response.to_text().c_str());
		}

		return -1;
	}

	return 0;
}

int __CLASS_NAME__::handle_input (ACE_HANDLE /*fd*/) {
	int call_result = 0;
	ACS_DSD_ServicePrimitive * response_primitive = 0;

	_query_response.error_field(acs_dsd::PCP_ERROR_CODE_HOST_UNREACHABLE); //We suppose a negative result

	do {
		if ((call_result = _cp_prot_svc_handler.recvPrimitive(response_primitive, DSD_SERVER_RECV_TIMEOUT_VALUE))
				!= ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_OK) { //ERROR
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Error receiving the "
					"APAP address response primitive: 'recvPrimitive(...' failed [call result == %d]"),
					__PRETTY_FUNCTION__, _cp_prot_svc_handler.getID(), call_result);
			break;
		}

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Received primitive. "
				"Primitive data : %s"), __PRETTY_FUNCTION__, _cp_prot_svc_handler.getID(),
				response_primitive->to_text().c_str());

		// take care of memory deallocation
		std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(response_primitive);

		CpProt_address_response * apap_address_response = dynamic_cast<CpProt_address_response *>(response_primitive);

		if (!apap_address_response) { //ERROR: Bad primitive type
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Bad primitive received: "
					"CP replied with the primitive number < %u >"), __PRETTY_FUNCTION__, _cp_prot_svc_handler.getID(),
					response_primitive->get_primitive_id());
			break;
		}

		_query_response.error_field(apap_address_response->get_error_field());
	} while (0);

	_cp_prot_svc_handler.peer().close();

	if ((call_result = _service_handler->sendPrimitive(&_query_response, DSD_SERVER_SEND_TIMEOUT_VALUE)) < 0)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. 'sendPrimitive(...' "
				"failed to send response to client [call_result == %d]"), __PRETTY_FUNCTION__,
				_service_handler->getID(), call_result);
	else {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: CONN_ID = < %d >. The QUERY RESPONSE "
				"primitive has been sent to client. Primitive data == '%s'"), __PRETTY_FUNCTION__,
				_service_handler->getID(), _query_response.to_text().c_str());
	}

	return -1;
}

/*
int __CLASS_NAME__::handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {
	if (__sync_sub_and_fetch(_query_handlers_counter, 1) <= 0) if (ACE_Reactor * my_reactor = reactor()) my_reactor->end_reactor_event_loop();

	return 0;
}
*/
