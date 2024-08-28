#include "ace/TP_Reactor.h"

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_ImmConnectionHandler.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_AppProt_SvcHandler.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_Server.h"
#include "ACS_DSD_AppProt_QueryProcessor.h"
#include "ACS_DSD_AppProt_QueryExchangeHandler.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_AppProt_QueryExchangeApContext.h"
#include "ACS_DSD_AppProt_QueryExchangeCpContext.h"
#include "ACS_DSD_AppProt_QueryPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


extern ACE_Recursive_Thread_Mutex imm_conn_mutex;
extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger dsd_logger;

ACE_Sig_Handler dummy_signal_handler;

/*
int AppProt_query_v1_request::get_service_name(char *buff, size_t buffsize) const
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int AppProt_query_v1_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int AppProt_query_v1_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int AppProt_query_v1_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}
*/

std::string AppProt_query_v1_request::to_text() const {
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"QUERY_ORDER = %u,"
				"TIMEOUT = %u",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain,
				_query_order,
				_timeout_ms);

	return mess;
}

int AppProt_query_v1_request::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * service_handler) const {
	int result_code = 0;

	switch (_query_order) {
	case acs_dsd::SYSTEM_TYPE_AP: result_code = process_query_for_ap(service_handler); break;
	case acs_dsd::SYSTEM_TYPE_BC:
	case acs_dsd::SYSTEM_TYPE_CP: result_code = process_query_for_cp_bc(service_handler, static_cast<acs_dsd::SystemTypeConstants>(_query_order)); break;
	default:
		result_code = ACS_DSD_PRIM_PROCESS_ERROR;
		this->_process_error_descr = "Invalid query order field in the query request primitive";
		break;
	}

	return result_code;
}

int AppProt_query_v1_request::process_query_for_ap (ACS_DSD_ServiceHandler * service_handler) const {
	ACS_DSD_Server dummy_server;
	ACS_DSD_Node my_node;

	//Querying my ap node
	if (dummy_server.get_local_node(my_node)) {
		char msg[2048];
		snprintf(msg, ACS_DSD_ARRAY_SIZE(msg),
				"Error querying my AP node: 'ACS_DSD_Server::get_local_node(...' failed: %s",
				dummy_server.last_error_text());
		_process_error_descr = msg;
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	std::vector<ACS_DSD_ConfigurationHelper::HostInfo> all_ap_nodes;

	if (const int call_result = ACS_DSD_ConfigurationHelper::get_all_ap_nodes(all_ap_nodes)) {
		char msg[2048];
		snprintf(msg, ACS_DSD_ARRAY_SIZE(msg),
				"Error getting all AP nodes from configuration helper: 'ACS_DSD_ConfigurationHelper::"
				"get_all_ap_nodes(...' failed: call_result == %d", call_result);
		_process_error_descr = msg;
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	size_t remote_ap_count = 0;
	for (size_t i = 0; i < all_ap_nodes.size(); ++i)
		remote_ap_count += ((all_ap_nodes[i].system_id != my_node.system_id) ? 1 : 0);


	// FIRST: The server reply with info from its AP
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
		ACS_DSD_ImmDataHandler imm_data_handler(immConnHandlerObj_ptr);

		int imm_call_result = imm_data_handler.fetch_ServiceInfo(my_node.node_name, _serv_name, _serv_domain, 1);

		AppProt_query_v1_response query_reply;
		query_reply.system_id(my_node.system_id);
		query_reply.node_state(my_node.node_state);
		query_reply.error_field(imm_call_result ^ acs_dsd_imm::NO_ERROR ? acs_dsd::PCP_ERROR_CODE_HOST_UNREACHABLE : acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Produced response primitive: %s"),service_handler->getID(), query_reply.to_text().c_str());
		if (service_handler->sendPrimitive(&query_reply, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0) {
			char msg[2048];
			snprintf(msg, ACS_DSD_ARRAY_SIZE(msg), "Error sending Query Reply for SERVICE_APPLICATION protocol about <%s> AP node", my_node.node_name);
			_process_error_descr = msg;
			return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("AppProt_query_v1_request::process_query_for_ap(): CONN_ID = < %u >. The response primitive for <%s> AP node has been sent !"), service_handler->getID(), my_node.node_name);

		//Querying my ap partner node
		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t my_partner_node_ptr = 0;
		if (ACS_DSD_ConfigurationHelper::get_my_ap_partner_node(my_partner_node_ptr)) {
			_process_error_descr = "Error querying my AP partner node: 'ACS_DSD_ConfigurationHelper::get_my_ap_partner_node(...' failed!";
			return ACS_DSD_PRIM_PROCESS_ERROR;
		}
		imm_call_result = imm_data_handler.fetch_ServiceInfo(my_partner_node_ptr->node_name, _serv_name, _serv_domain);

		query_reply.system_id(my_partner_node_ptr->system_id);
		switch (my_node.node_state) {
		case acs_dsd::NODE_STATE_ACTIVE: query_reply.node_state(acs_dsd::NODE_STATE_PASSIVE); break;
		case acs_dsd::NODE_STATE_PASSIVE: query_reply.node_state(acs_dsd::NODE_STATE_ACTIVE); break;
		default: query_reply.node_state(acs_dsd::NODE_STATE_UNDEFINED); break;
		}
		query_reply.error_field(imm_call_result ^ acs_dsd_imm::NO_ERROR ? acs_dsd::PCP_ERROR_CODE_HOST_UNREACHABLE : acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Produced response primitive: %s"), service_handler->getID(), query_reply.to_text().c_str());
		if (service_handler->sendPrimitive(&query_reply, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0) {
			char msg[2048];
			snprintf(msg, ACS_DSD_ARRAY_SIZE(msg), "Error sending Query Reply for SERVICE_APPLICATION protocol about <%s> AP node", my_partner_node_ptr->node_name);
			_process_error_descr = msg;
			return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("AppProt_query_v1_request::process_query_for_ap(): CONN_ID = < %u >. The response primitive for <%s> AP node has been sent !"), service_handler->getID(), my_partner_node_ptr->node_name);
	}


	// NEXT: The server reply with info from remote AP nodes
/*********************************************************************************/
	if (remote_ap_count > 0) {
		// There are some remote AP nodes
		int call_result = 0;
		ACE_TP_Reactor exchange_reactor_impl(&dummy_signal_handler);
		ACE_Reactor exchange_reactor(&exchange_reactor_impl);

		//	AppProt_QueryExchangeHandler query_exchange_handlers[nodes_count];
		AppProt_QueryExchangeHandler<AppProt_QueryExchangeApContext> query_exchange_handlers[remote_ap_count];

		volatile int query_handlers_counter = 0;
		int query_handler_index = 0;

		for (size_t i = 0; i < all_ap_nodes.size(); ++i, query_handlers_counter += (call_result ? 0 : 1))
			call_result = (all_ap_nodes[i].system_id != my_node.system_id)
									? query_exchange_handlers[query_handler_index++].activate(exchange_reactor, service_handler,
											_serv_name, _serv_domain, all_ap_nodes[i].system_id, all_ap_nodes[i].node_name,
											acs_dsd::NODE_STATE_UNDEFINED, &query_handlers_counter,
											timeout_ms() == 0xFFFFFFFFU ? 0 : &_timeout_ms)
									: -1;

		if (query_handlers_counter) { //we have some query event handler registered to the reactor
			AppProt_QueryProcessor query_processor(exchange_reactor, timeout_ms());

			query_processor.activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 2);

			/* Here we can ignore the possible active return error because we have always a thread (this)
			 * that can do the work
			 */

			ACE_Time_Value timeout;
			timeout.msec(static_cast<long int>(timeout_ms() == 0xFFFFFFFFU ? 1000 : timeout_ms()));

			if (timeout_ms() == 0xFFFFFFFFU)
				while (query_handlers_counter > 0) {
					ACE_Time_Value tv(timeout);
					exchange_reactor.run_reactor_event_loop(tv);
				}
			else exchange_reactor.run_reactor_event_loop(timeout);

			query_processor.wait();
		}

		exchange_reactor.close();
	}

	return ACS_DSD_PRIM_PROCESS_OK_CLOSE_RESPONSE_ALREADY_SENT;
}

int AppProt_query_v1_request::process_query_for_cp_bc (ACS_DSD_ServiceHandler * service_handler, acs_dsd::SystemTypeConstants system_type) const {
	size_t nodes_count = acs_dsd::CONFIG_CP_NODES_SUPPORTED;
	ACS_DSD_Node nodes[nodes_count];
	ACS_DSD_CpNodesManager::OP_Result op_result = ACS_DSD_CpNodesManager::CPNM_OK;

	if ((op_result = cpNodesManager.get_nodes(system_type, nodes, nodes_count))) {
		char msg[2048];
		snprintf(msg, ACS_DSD_ARRAY_SIZE(msg),
				"In process_query_for_cp_bc call to 'cpNodesManager.get_nodes(...' failed with error code %d: system_type == %d", op_result, system_type);
		_process_error_descr = msg;
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	if (!nodes_count) return ACS_DSD_PRIM_PROCESS_OK_CLOSE_NO_RESPONSE; //Exit closing the session

	int call_result = 0;
	ACE_TP_Reactor exchange_reactor_impl;
	ACE_Reactor exchange_reactor(&exchange_reactor_impl);

//	AppProt_QueryExchangeHandler query_exchange_handlers[nodes_count];
	AppProt_QueryExchangeHandler<AppProt_QueryExchangeCpContext> query_exchange_handlers[nodes_count];

	volatile int query_handlers_counter = 0;

	for (size_t i = 0; i < nodes_count; ++i, query_handlers_counter += (call_result ? 0 : 1))
		call_result = query_exchange_handlers[i].activate(exchange_reactor, service_handler, _serv_name,
				_serv_domain, nodes[i].system_id, nodes[i].node_name, nodes[i].node_state,
				&query_handlers_counter, timeout_ms() == 0xFFFFFFFFU ? 0 : &_timeout_ms);

	if (query_handlers_counter) { //we have some query event handler registered to the reactor
		AppProt_QueryProcessor query_processor(exchange_reactor, timeout_ms());

		query_processor.activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 2);

		/* Here we can ignore the possible active return error because we have always a thread (this)
		 * that can do the work
		 */

		ACE_Time_Value timeout;
		timeout.msec(static_cast<long int>(timeout_ms() == 0xFFFFFFFFU ? 1000 : timeout_ms()));

		if (timeout_ms() == 0xFFFFFFFFU)
			while (query_handlers_counter > 0) {
				ACE_Time_Value tv(timeout);
				exchange_reactor.run_reactor_event_loop(tv);
			}
		else exchange_reactor.run_reactor_event_loop(timeout);

		query_processor.wait();
	}

	exchange_reactor.close();

	//WARNING: it could be needed to use a locking/guard object to synchronize the replies upon the
	//session back to client

	return ACS_DSD_PRIM_PROCESS_OK_CLOSE_NO_RESPONSE;
}

std::string AppProt_query_v1_response::to_text () const {
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u,"
				"SYSTEM ID = %d,"
				"NODE STATE = %u,",
				_primitive_id,
				_protocol_version,
				_error,
				_system_id,
				_node_state);

	return mess;
}
