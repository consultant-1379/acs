#include <memory>

#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_SrvProt_QueryPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACE_Recursive_Thread_Mutex imm_conn_mutex;
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger	dsd_logger;
extern ACS_DSD_StartupSessionsGroup * startup_session_group;

/* SrvProt_query_v1_request methods */
std::string SrvProt_query_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain);
	return mess;
}

int SrvProt_query_v1_request::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler *service_handler) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_query_v1_request::process";
#endif

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: - applName = %s, domain = %s"), FX_NAME, _serv_name,_serv_domain);

	// If a query-request primitive of SRV-SRV protocol is received,
	// the system has a Multi-AP configuration
	startup_session_group->system_configuration(ACS_DSD_StartupSessionsGroup::SYS_CONF_MULTI_AP);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s:: Set 'system_configuration' status to MULTI-AP!"), FX_NAME);

	std::string error_descr;
	ACS_DSD_Node local_node;
	int op_res = ACS_DSD_Utils::get_node_info(local_node, error_descr);
	if(op_res<0){
		_process_error_descr = " ACS_DSD_Utils::get_node_info()failed!";
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Utils::get_node_info() failure! return code is < %d > - description is < %s > ! Exiting with code < -2 >"), op_res, error_descr.c_str());
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}
	// check if the service is registered on the local node and reply
	int resultcode = query_to_apnode(local_node, 1, service_handler);

/*
	ACS_DSD_Node partner_node;
	op_res = ACS_DSD_Utils::get_partner_node_info(partner_node, error_descr);
	if(op_res<0){
		_process_error_descr = " ACS_DSD_Utils::get_partner_node_info()failed!";
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Utils::get_partner_node_info() failure! return code is < %d > - description is < %s > ! Exiting with code < -2 >"), op_res, error_descr.c_str());
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}
	// check if the service is registered on the partner node and reply
	resultcode = query_to_apnode(partner_node, 0, service_handler);
*/

	return resultcode;
}

void SrvProt_query_v1_request::service_name (const char * str) {
	::strncpy(_serv_name, str ?: "", ACS_DSD_ARRAY_SIZE(_serv_name));
	_serv_name[ACS_DSD_ARRAY_SIZE(_serv_name) - 1] = 0;
}

void SrvProt_query_v1_request::service_domain (const char * str) {
	::strncpy(_serv_domain, str ?: "", ACS_DSD_ARRAY_SIZE(_serv_domain));
	_serv_domain[ACS_DSD_ARRAY_SIZE(_serv_domain) - 1] = 0;
}

int SrvProt_query_v1_request::query_to_apnode(ACS_DSD_Node ap_node, uint8_t is_local_node, ACS_DSD_ServiceHandler *service_handler) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_query_v1_request::query_to_apnode";
#endif

	// try to fetch service addresses from IMM
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immData(immConnHandlerObj_ptr);
	int call_result = immData.fetch_serv_addresses_fromIMM(_serv_name, _serv_domain, ap_node.node_name, is_local_node);

	if (call_result == acs_dsd_imm::ERR_IMM_OM_INIT) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s<%s>: Failed!!  Error descr: %s"), FX_NAME,
				ap_node.node_name, immData.last_error_text());
		_process_error_descr = std::string("IMM failure: ") + immData.last_error_text();
		return ACS_DSD_PRIM_PROCESS_IMM_ERROR;
	}

	SrvProt_query_v1_response  *p_resp_prim = new (std::nothrow) SrvProt_query_v1_response();
	if (!p_resp_prim) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s<%s@%s>: Failed!!  Error allocating primitive object"),FX_NAME, _serv_name, _serv_domain);
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	p_resp_prim->system_id(ap_node.system_id);
	p_resp_prim->node_state(ap_node.node_state);
	p_resp_prim->node_name(ap_node.node_name);

	// take care of memory deallocation for the response primitive object
	std::auto_ptr<SrvProt_query_v1_response> auto_ptr_resp_prim(p_resp_prim);
	uint8_t serv_visib = IMM_DH_LOCAL_SCOPE ;
	if ((call_result == acs_dsd_imm::NO_ERROR) && ((serv_visib = immData.get_visibility()) == IMM_DH_GLOBAL_SCOPE))
	{
		p_resp_prim->error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s <%s@%s>: successful!!"),FX_NAME, _serv_name, _serv_domain);
	}
	else if (call_result == acs_dsd_imm::ERR_IMM_EMPTY_VALUESNUM_ATTR){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s<%s@%s>: fetch_serv_addresses_fromIMM <%s> returns :%s"), FX_NAME, _serv_name, _serv_domain, ap_node.node_name,immData.last_error_text());
		p_resp_prim->error_field(acs_dsd::PCP_ERROR_CODE_HOST_UNREACHABLE);
	}
	else {
		if (serv_visib!= IMM_DH_GLOBAL_SCOPE) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
					ACS_DSD_TEXT("%s<%s@%s>: visibility has not global scope <serv_visib=%u>"), FX_NAME,
					_serv_name, _serv_domain, serv_visib);
		}
		else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s<%s@%s>: Failed! Error descr: %s"),FX_NAME, _serv_name, _serv_domain, immData.last_error_text()); }

		if(call_result == acs_dsd_imm::ERR_IMM_OM_GET || serv_visib !=IMM_DH_GLOBAL_SCOPE)
			p_resp_prim->error_field(acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED);
		else
			p_resp_prim->error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"),
			service_handler->getID(), p_resp_prim->to_text().c_str());

	if (service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >.  error sending query response primitive!"), FX_NAME, service_handler->getID());
		_process_error_descr = "Error sending query response";
		return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: CONN_ID = < %u >.  The query response primitive has been sent !"),
			FX_NAME, service_handler->getID());

	return ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT;
}



/**********************************************************************************************************************/


std::string SrvProt_query_v1_response::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u,"
				"SYSTEM_ID = %d,"
				"NodeState = %u,"
				"NodeName = %s",
				_primitive_id,
				_protocol_version,
				_error,
				_system_id,
				_node_state,
				_node_name);

	return mess;
}
