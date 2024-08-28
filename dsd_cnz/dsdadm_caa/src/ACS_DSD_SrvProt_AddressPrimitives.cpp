#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_SrvProt_AddressPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


/* AppProt_address_request methods */
extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACE_Recursive_Thread_Mutex imm_conn_mutex;
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_StartupSessionsGroup * startup_session_group;


SrvProt_address_request::SrvProt_address_request()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_ADDRESS_REQUEST;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	_system_id = 0;
	_node_state = acs_dsd::NODE_STATE_UNDEFINED;
}

std::string SrvProt_address_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"SYSTEM_ID = %u,"
				"NODE_STATE = %d",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain,
				_system_id,
				_node_state);

	return mess;
}

int SrvProt_address_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int SrvProt_address_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int SrvProt_address_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int SrvProt_address_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}

int SrvProt_address_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_ADDRESS_REQUEST, SRVSRV_PROT_V1, _serv_name, _serv_domain, _system_id, _node_state);
}

int SrvProt_address_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version,_serv_name,_serv_domain, &_system_id, &_node_state);
}

int SrvProt_address_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_address_request::process";
#endif

	int returnCode = ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: - applName = %s, domain = %s, systemId = %d, node_state = %d"), FX_NAME, _serv_name,_serv_domain,_system_id, _node_state);

	// If an address-request primitive of SRV-SRV protocol is received,
	// the system has a Multi-AP configuration
	startup_session_group->system_configuration(ACS_DSD_StartupSessionsGroup::SYS_CONF_MULTI_AP);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s:: Set 'system_configuration' status to MULTI-AP!"), FX_NAME);

	std::string error_descr;
	ACS_DSD_Node local_node;
	int op_res = ACS_DSD_Utils::get_node_info(local_node,error_descr);
	if(op_res<0){
		_process_error_descr = " ACS_DSD_Utils::get_node_info() failed!";
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Utils::get_node_info() failure! return code is < %d > - description is < %s > ! Exiting with code < -2 >"), op_res, error_descr.c_str());
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}
	ACS_DSD_Node partner_node;
	op_res = ACS_DSD_Utils::get_partner_node_info(partner_node,error_descr);
	if(op_res<0){
		_process_error_descr = " ACS_DSD_Utils::get_partner_node_info() failed!";
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Utils::get_partner_node_info() failure! return code is < %d > - description is < %s > ! Exiting with code < -2 >"), op_res, error_descr.c_str());
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	// Check the node name with the given state
	char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};
	int check_running = 0;
	if ( local_node.node_state == _node_state )
	{
		strncpy(node_name, local_node.node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);
		check_running = 1;
	}
	else if ( partner_node.node_state == _node_state )
		strncpy(node_name, partner_node.node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);
	else
	{
		// ERROR: Bad node state provided!
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN,
				ACS_DSD_TEXT("%s: The node_state parameter provided (value %d) is different from the local node_state "
				"(value %d) and from the partner node_state (value %d)"),
				FX_NAME, _node_state, local_node.node_state, partner_node.node_state);

		// Set the error code and send the reply primitive
		SrvProt_address_inet_response * p_resp_prim = new (std::nothrow) SrvProt_address_inet_response();
		if ( !p_resp_prim )
		{
			_process_error_descr = "Memory not available to allocate the primitive object";
			return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
		}

		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
		response_primitive = p_resp_prim;
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	// try to fetch service addresses from IMM
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immData(immConnHandlerObj_ptr);
	int call_result = immData.fetch_ServiceInfo(node_name, _serv_name, _serv_domain, check_running);

	if(call_result == acs_dsd_imm::ERR_IMM_OM_INIT) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("SrvProt_address_request  < %s@%s > Failed!!  Error descr: %s"), _serv_name, _serv_domain, immData.last_error_text());
		_process_error_descr = std::string("IMM failure: ") + immData.last_error_text();
		return ACS_DSD_PRIM_PROCESS_IMM_ERROR;
	}
	uint8_t serv_visib = IMM_DH_LOCAL_SCOPE;

	if(call_result == acs_dsd_imm::NO_ERROR && (serv_visib = immData.get_visibility())== IMM_DH_GLOBAL_SCOPE)
	{
		uint16_t conn_type;
		immData.get_conn_type(conn_type);
		acs_dsd::NodeStateConstants node_state;
		if( immData.get_service_node_info()== IMM_DH_LOCAL_NODE) node_state = local_node.node_state;
		else node_state = partner_node.node_state;

		switch(conn_type){
			case acs_dsd::CONNECTION_TYPE_INET:
			{
				// allocate a new primitive object
				SrvProt_address_inet_response  *p_resp_prim = new (std::nothrow) SrvProt_address_inet_response();
				if(!p_resp_prim){
					_process_error_descr = "Memory not avaliable to allocate the primitive object";
					return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
				}
				uint16_t conn_num =0;
				ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
				immData.get_inet_addresses(inet_addresses, conn_num);

				ACE_INET_Addr tmp_inet_addr[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
				int i,j;
				for( i=0,j=0; i<conn_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++){
					if(inet_addresses[i].get_ip_address()){
						tmp_inet_addr[j].set(inet_addresses[i]); j++;
					}
				}
				uint16_t valid_conn_num =j;
				if(valid_conn_num){
					p_resp_prim->set_ip_address_1(tmp_inet_addr[0].get_ip_address());
					p_resp_prim->set_port_1(tmp_inet_addr[0].get_port_number());

					if (valid_conn_num > 1){
						p_resp_prim->set_ip_address_2(tmp_inet_addr[1].get_ip_address());
						p_resp_prim->set_port_2(tmp_inet_addr[1].get_port_number());
					}
					p_resp_prim->set_node_state(node_state);
					p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("SrvProt_address_request  < %s@%s > successful!!"), _serv_name, _serv_domain);
				}
				else{
					p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("SrvProt_address_request  < %s@%s > failed: ADDRESSES EMPTY!"), _serv_name, _serv_domain);
				}
				response_primitive = p_resp_prim;
			}
			break;
			case acs_dsd::CONNECTION_TYPE_UNIX:
			{
				// allocate a new primitive object
				SrvProt_address_unix_response  *p_resp_prim = new (std::nothrow) SrvProt_address_unix_response();
				if(!p_resp_prim){
					_process_error_descr = "Memory not available to allocate the primitive object";
					return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
				}
				uint16_t conn_num =0;
				char unix_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX] = {{0}};
				immData.get_unix_addresses(unix_addresses, conn_num);

				char tmp_unix_addr[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX] = {{0}};
				int i,j;
				for( i=0,j=0;i<conn_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++){
					if(*unix_addresses[i]){
						strncpy(tmp_unix_addr[j], unix_addresses[i], acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
						j++;
					}
				}
				uint16_t valid_conn_num =j;
				if(valid_conn_num){
					p_resp_prim->set_unix_sock_path1(tmp_unix_addr[0]);
					if (valid_conn_num > 1)
						p_resp_prim->set_unix_sock_path2(tmp_unix_addr[1]);
					p_resp_prim->set_node_state(node_state);
					p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("SrvProt_address_request < %s@%s > successful!!"), _serv_name, _serv_domain);
				}
				else{
					p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE);
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("SrvProt_address_request < %s@%s > failed: Invalid conn_type retrieved from IMM <conn_type=%d>!"), _serv_name, _serv_domain, conn_type);
				}
				response_primitive = p_resp_prim;
			}
			break;
			default:
			{
				//Bad connection type retrieved from IMM
				SrvProt_address_unix_response  *p_resp_prim = new (std::nothrow) SrvProt_address_unix_response();
				if(!p_resp_prim){
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("SrvProt_address_request  < %s@%s > Failed!! error in allocate primitive object"),_serv_name,_serv_domain);
						_process_error_descr = "Memory not available to allocate the primitive object";
						return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
				}
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE);
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("SrvProt_address_request < %s,%s,%d > Failed!! Bad connection type retrieved from IMM <conn_type=%d>"),_serv_name,_serv_domain,_system_id,conn_type);
				response_primitive = p_resp_prim;
			}
			break;
		}
	}
	else {
		SrvProt_address_unix_response  *p_resp_prim = new (std::nothrow) SrvProt_address_unix_response();
		if(!p_resp_prim){
			_process_error_descr = "Memory not available to allocate the primitive object";
			return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
		}
		if (serv_visib!= IMM_DH_GLOBAL_SCOPE) { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("SrvProt_address_request  < %s@%s > visibility has not global scope <serv_visib=%u>"), _serv_name, _serv_domain, serv_visib); }
		else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("SrvProt_address_request  < %s@%s > Failed! Error descr: %s"), _serv_name, _serv_domain, immData.last_error_text()); }

		if(call_result == acs_dsd_imm::ERR_IMM_OM_GET || serv_visib !=IMM_DH_GLOBAL_SCOPE)
			p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED);
		else
			p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
		response_primitive = p_resp_prim;
	}
	return returnCode;
}


/**********************************************************************************************************************/


/* SrvProt_addressINET_response methods */

SrvProt_address_inet_response::SrvProt_address_inet_response()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_ADDRESS_INET_REPLY;
	_error = 0;
	_ip1 = 0;
	_ip2 = 0;
	_port1 = 0;
	_port2 = 0;
	_node_state = acs_dsd::NODE_STATE_UNDEFINED;
}

std::string SrvProt_address_inet_response::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u,"
				"IP1 = %u,"
				"PORT1 = %u,"
				"IP2 = %u,"
				"PORT2 = %u,"
				"NodeState = %u",
				_primitive_id,
				_protocol_version,
				_error,
				_ip1,
				_port1,
				_ip2,
				_port2,
				_node_state);

	return mess;
}

int SrvProt_address_inet_response::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_ADDRESS_INET_REPLY, SRVSRV_PROT_V1, _error, _ip1, _ip2, _port1, _port2, _node_state);
}

int SrvProt_address_inet_response::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, _error, _ip1, _ip2, _port1, _port2);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, &_ip1, &_ip2, &_port1, &_port2, &_node_state);
}

int SrvProt_address_inet_response::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}


/**********************************************************************************************************************/


/* SrvProt_addressUNIX_response methods */

SrvProt_address_unix_response::SrvProt_address_unix_response()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_ADDRESS_UNIX_REPLY;
	_error = 0;
	memset(_unix_sock_path1,'\0',sizeof(_unix_sock_path1));
	memset(_unix_sock_path2,'\0',sizeof(_unix_sock_path2));
	_node_state = acs_dsd::NODE_STATE_UNDEFINED;
}

std::string SrvProt_address_unix_response::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"ERROR = %u,"
				"UNIX_SOCKET_NAME_1 = %s,"
				"UNIX_SOCKET_NAME_1 = %s,"
				"NodeState = %u",
				_primitive_id,
				_protocol_version,
				_error,
				_unix_sock_path1,
				_unix_sock_path2,
				_node_state);

	return mess;
}

int SrvProt_address_unix_response::get_unix_sock_path1(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path1, buff, buffsize);
}

int SrvProt_address_unix_response::set_unix_sock_path1(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path1, str, strlen(str));
}

int SrvProt_address_unix_response::get_unix_sock_path2(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path2, buff, buffsize);
}

int SrvProt_address_unix_response::set_unix_sock_path2(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path2, str, strlen(str));
}


int SrvProt_address_unix_response::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_ADDRESS_UNIX_REPLY, SRVSRV_PROT_V1, _error, _unix_sock_path1, _unix_sock_path2,_node_state);
}

int SrvProt_address_unix_response::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, &_unix_sock_path1, &_unix_sock_path2,&_node_state);
}

int SrvProt_address_unix_response::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}

