#include <string.h>
#include <memory>

#include "ACS_DSD_ProtocolsDefs.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ImmConnectionHandler.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_Macros.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_CpProt_StartupPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger dsd_logger;


/* CpProt_startup_v0_request class methods */

CpProt_startup_v0_request::CpProt_startup_v0_request()
{
	_protocol_id = PROTOCOL_CPAP;
	_protocol_version = CPAP_PROTOCOL_VERSION_0;
	_primitive_id = CPAP_STARTUP_REQUEST;
	_AP_mask = 0;
	_CP_EX_mask = 0;
	_CP_SB_mask = 0;
	_APidentity = 0;
}


std::string CpProt_startup_v0_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"AP_mask = %016lu,"
				"CP_EX_mask = %08u,"
				"CP_SB_mask = %08u,"
				"APIdentity = %02u",
				_primitive_id,
				_protocol_version,
				_AP_mask,
				_CP_EX_mask,
				_CP_SB_mask,
				_APidentity);


	return mess;
}

int CpProt_startup_v0_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(CPAP_STARTUP_REQUEST, CPAP_PROTOCOL_VERSION_0, _AP_mask, _CP_EX_mask, _CP_SB_mask, _APidentity);
}

int CpProt_startup_v0_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version,  & _AP_mask, & _CP_EX_mask, & _CP_SB_mask, & _APidentity);
}

int CpProt_startup_v0_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const
{
	// this primitive represents a CP-AP protocol "Start up request" primitive in which an unsupported protocol version has been specified. So we must reply with error code 16
	CpProt_startup_v2_reply   *p_resp_prim = new (std::nothrow) CpProt_startup_v2_reply();
	if(!p_resp_prim)
	{
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	// get info about the remote CP Node
	ACS_DSD_Node remoteCpNodeInfo;
	int op_res = service_handler->peer().get_remote_node(remoteCpNodeInfo);
	if(op_res < 0 ) { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. ACS_DSD_Session::get_remote_node() failure ! error code = < %d >"), service_handler->getID(), op_res); }
	else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Remote CP Node info: SysId = %d, SystemName =%s, SysType=%d"), service_handler->getID(), remoteCpNodeInfo.system_id, remoteCpNodeInfo.system_name, remoteCpNodeInfo.system_type); }

	//  get the two IP addresses associated to the remote CP Node
	uint32_t tmp_ip[2] = {0,0};
	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t cpHostInfo;
	int call_result = ACS_DSD_ConfigurationHelper::get_node_by_node_name(remoteCpNodeInfo.node_name,cpHostInfo);
	if(call_result == 0)
	{
		for(uint8_t i = 0; (i<cpHostInfo->ip4_adresses_count) && (i<2); ++i)
			tmp_ip[i] = ntohl(cpHostInfo->ip4_addresses[i]);
	}
	else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. ACS_DSD_ConfigurationHelper::get_node_by_name() failure ! Return code is < %d >"), service_handler->getID(), call_result); }


	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %u >. APAP Startup request specifies an unsupported protocol version: < %u > !"), service_handler->getID(), _protocol_version);
	p_resp_prim->set_APidentity(this->_APidentity);
	p_resp_prim->set_Error_field(acs_dsd::PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED);
	p_resp_prim->set_Reserved(0);
	p_resp_prim->set_ip_address_1(tmp_ip[0]);
	p_resp_prim->set_ip_address_2(tmp_ip[1]);

	// pass the response primitive to the caller
	response_primitive = p_resp_prim;

	return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
}


/* CpProt_startup_v2_request class methods */

CpProt_startup_v2_request::CpProt_startup_v2_request()
{
	_protocol_id = PROTOCOL_CPAP;
	_protocol_version = CPAP_PROTOCOL_VERSION_2;
	_primitive_id = CPAP_STARTUP_REQUEST;
	_AP_mask = 0;
	_CP_EX_mask = 0;
	_CP_SB_mask = 0;
	_APidentity = 0;
}


std::string CpProt_startup_v2_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"AP_mask = %016lu,"
				"CP_EX_mask = %08u,"
				"CP_SB_mask = %08u,"
				"APIdentity = %02u",
				_primitive_id,
				_protocol_version,
				_AP_mask,
				_CP_EX_mask,
				_CP_SB_mask,
				_APidentity);


	return mess;
}

int CpProt_startup_v2_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(CPAP_STARTUP_REQUEST, CPAP_PROTOCOL_VERSION_2, _AP_mask, _CP_EX_mask, _CP_SB_mask, _APidentity);
}

int CpProt_startup_v2_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version,  & _AP_mask, & _CP_EX_mask, & _CP_SB_mask, & _APidentity);
}

int CpProt_startup_v2_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const
{
	// create the "startup v2 reply" primitive
	CpProt_startup_v2_reply * p_resp_prim = new (std::nothrow) CpProt_startup_v2_reply();
	if(!p_resp_prim)
	{
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	// get some general info about the remote CP node
	ACS_DSD_Node remoteCpNodeInfo;
	int call_result = service_handler->peer().get_remote_node(remoteCpNodeInfo);
	if(call_result < 0 ) { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. ACS_DSD_Session::get_remote_node() failure ! error code = < %d >"), service_handler->getID(), call_result); }
	else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Remote CP Node info: NodeName = %s, SysId = %d, SystemName =%s, SysType=%d"), service_handler->getID(), remoteCpNodeInfo.node_name, remoteCpNodeInfo.system_id, remoteCpNodeInfo.system_name, remoteCpNodeInfo.system_type); }

	// get the two IP addresses associated to the remote CP node
	uint32_t tmp_ip[2] = {0,0};
	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t cpHostInfo;
	call_result = ACS_DSD_ConfigurationHelper::get_node_by_node_name(remoteCpNodeInfo.node_name,cpHostInfo);
	if(call_result == 0)
	{
		for(uint8_t i = 0; (i<cpHostInfo->ip4_adresses_count) && (i<2); ++i)
			tmp_ip[i] = ntohl(cpHostInfo->ip4_addresses[i]);
	}
	else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. ACS_DSD_ConfigurationHelper::get_node_by_name() failure ! Return code is < %d >"), service_handler->getID(), call_result); }

	// begin construction of the "startup v2 reply" primitive
	p_resp_prim->set_APidentity(_APidentity);
	p_resp_prim->set_Reserved(0);
	p_resp_prim->set_ip_address_1(tmp_ip[0]);
	p_resp_prim->set_ip_address_2(tmp_ip[1]);

	// check AP identity
	std::string strAPidentity = ACS_DSD_Utils::get_AP_name(_APidentity);
	if(strAPidentity != dsd_local_node.node_name)
	{
		// check failed. Set response primitive (ERROR_CODE = 1) and return  (note that the response primitive will be sent to the peer by someone else )
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CpProt_startup_v2_request::process() : The AP identity (%s) received from the CP does not correspond to the AP identity (%s) stored in the AP"), strAPidentity.c_str(), dsd_local_node.system_name);
		p_resp_prim->set_Error_field(acs_dsd::PCP_ERROR_CODE_NODE_NOT_VALID);
		response_primitive = p_resp_prim;
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	// the response primitive will be sent to the peer within this method: so take care of memory deallocation
	std::auto_ptr<CpProt_startup_v2_reply> auto_ptr_resp_prim(p_resp_prim);

	// complete and send the "startup v2 reply" primitive
	p_resp_prim->set_Error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"), service_handler->getID(), p_resp_prim->to_text().c_str());
	if(service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0)
	{
		_process_error_descr = "Error sending Startup Reply for CP-AP Protocol primitive";
		return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CpProt_startup_v2_request::process(): CONN_ID = < %u >.  The response primitive has been sent !"), service_handler->getID());

	// get the CP state communicated in the "startup v2 request" primitive
	acs_dsd::NodeStateConstants cp_node_state = acs_dsd::NODE_STATE_UNDEFINED;
	if(this->_CP_EX_mask)
		cp_node_state = acs_dsd::NODE_STATE_EXECUTION;
	else if(this->_CP_SB_mask)
		cp_node_state = acs_dsd::NODE_STATE_STANDBY;

	// request exclusive access to the CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpNodesManager.synch_mutex());

	// check if we have to notify the CP system state to the client applications that registered themselves to receive CP system state notifications.
	// If so, calculate also the CP system state to be notified.
	bool notify_needed = false;
	acs_dsd::CPStateConstants out_cp_system_state = acs_dsd::CP_STATE_UNKNOWN;
	if(remoteCpNodeInfo.system_type == acs_dsd::SYSTEM_TYPE_CP)
		notify_needed = cpNodesManager.notification_needed(remoteCpNodeInfo.node_name, remoteCpNodeInfo.system_id, remoteCpNodeInfo.system_type, remoteCpNodeInfo.node_side, cp_node_state, out_cp_system_state);

	// update the CP nodes collection
	ACS_DSD_CpNodesManager::OP_Result op_res = cpNodesManager.set_cp_conn(remoteCpNodeInfo.node_name, remoteCpNodeInfo.system_id, remoteCpNodeInfo.system_type, remoteCpNodeInfo.node_side, cp_node_state, dynamic_cast<ACS_DSD_CpProt_SvcHandler *>(service_handler));
	if(op_res < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. CpProt_startup_v2_request::process(..) - Call 'ACS_DSD_CpNodesManager::set_cp_node(..)' failed ! call_result == %d"), service_handler->getID(), op_res);
		char error_message[256];
		sprintf(error_message,"ACS_DSD_CpNodesManager::set_cp_node() failed ! call_result == %d", op_res);
		_process_error_descr = error_message;
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	// cache some important info in the Service Handler (such info will be used when closing the connection associated to the Service Handler)
	ACS_DSD_CpProt_SvcHandler *pSrvHdlr = dynamic_cast<ACS_DSD_CpProt_SvcHandler *>(service_handler);
	if(pSrvHdlr)
	{
		pSrvHdlr->set_linked_to_cp_nodes_manager(true);
		pSrvHdlr->set_remote_node_name(remoteCpNodeInfo.node_name);
	}

	// take into account notifications
	if(notify_needed)
	{
		// start notifications
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::notifyCpStateChange() - system_id = %d, cp_state = %d"), remoteCpNodeInfo.system_id, out_cp_system_state);
		cpNodesManager.notifyCpStateChange(remoteCpNodeInfo.system_id, out_cp_system_state);
	}

	return ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT;
}


/* CpProt_startup_v2_reply class methods */

CpProt_startup_v2_reply::CpProt_startup_v2_reply()
{
	_protocol_id = PROTOCOL_CPAP;
	_protocol_version = CPAP_PROTOCOL_VERSION_2;
	_primitive_id = CPAP_STARTUP_REPLY;
	_Error = 0;
	_APidentity = 0;
	_reserved = 0;
	_ip1 = 0;
	_ip2 = 0;
}

std::string CpProt_startup_v2_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u,"
				"APIdentity = %02u,"
				"Reserved = %u,"
				"IP1 = %08X,"
				"IP2 = %08X",
				_primitive_id,
				_protocol_version,
				_Error,
				_APidentity,
				_reserved,
				_ip1,
				_ip2);

	return mess;
}

int CpProt_startup_v2_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<>  & pdh) const
{
	return  pdh.make_primitive(CPAP_STARTUP_REPLY, CPAP_PROTOCOL_VERSION_2, _Error, _APidentity, _reserved, _ip1, _ip2);
}

int  CpProt_startup_v2_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, & _Error, & _APidentity, & _reserved, & _ip1, & _ip2);
}

int CpProt_startup_v2_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}

