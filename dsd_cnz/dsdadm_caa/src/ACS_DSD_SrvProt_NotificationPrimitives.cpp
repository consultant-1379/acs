#include <string.h>

#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_DSD_ProtocolsDefs.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ImmConnectionHandler.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_Macros.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"
#include "ACS_DSD_AppProt_RegisterPrimitives.h"
#include "ACS_DSD_AppProt_UnregisterPrimitives.h"
#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_SrvProt_NotificationPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_ImmConnectionHandler * imm_shared_objects_conn_ptr;
extern ACE_Recursive_Thread_Mutex  imm_shared_objects_conn_mutex;
extern int is_imm_shared_objects_controller;
extern ACS_DSD_Logger	dsd_logger;
extern ACS_DSD_StartupSessionsGroup * startup_session_group;

/* SvrProt_notify_publish_v1_request methods */
SrvProt_notify_publish_v1_request::SrvProt_notify_publish_v1_request()
{
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_NOTIFY_PUBLISH;
	_pid = 0;
	_visibility = 0;
	_ip1 = 0;
	_ip2 = 0;
	_port1 = 0;
	_port2 = 0;
	_conn_type = acs_dsd::CONNECTION_TYPE_UNKNOWN;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	memset(_process_name,'\0',sizeof(_process_name));
	memset(_unix_sock_path, '\0',sizeof(_unix_sock_path));
	memset(_node_name,'\0',sizeof(_node_name));
}


std::string SrvProt_notify_publish_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"PROCESS_NAME = %s,"
			    "NODE_NAME = %s,"
				"PID = %u,"
				"VISIBILITY = %u,"
				"CONN_TYPE = %u,"
				"IP1 = %u,"
				"PORT1 = %u,"
				"IP2 = %u,"
				"PORT2 = %u,"
				"UNIX_SOCK_PATH = %s",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain,
				_process_name,
				_node_name,
				_pid,
				_visibility,
				_conn_type,
				_ip1,
				_port1,
				_ip2,
				_port2,
				_unix_sock_path);


	return mess;
}

int SrvProt_notify_publish_v1_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int SrvProt_notify_publish_v1_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int SrvProt_notify_publish_v1_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int SrvProt_notify_publish_v1_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}

int SrvProt_notify_publish_v1_request::get_process_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_process_name, buff, buffsize);
}

int SrvProt_notify_publish_v1_request::set_process_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_process_name, str, strlen(str));
}

int SrvProt_notify_publish_v1_request::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_node_name, buff, buffsize);
}

int SrvProt_notify_publish_v1_request::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}

int SrvProt_notify_publish_v1_request::get_unix_path(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path, buff, buffsize);
}

int SrvProt_notify_publish_v1_request::set_unix_path(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path,str, strlen(str));
}

int SrvProt_notify_publish_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_NOTIFY_PUBLISH, SRVSRV_PROT_V1, _serv_name, _serv_domain, _process_name, _node_name,_pid, _visibility, _conn_type,_ip1, _port1, _ip2, _port2, _unix_sock_path);
}

int SrvProt_notify_publish_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version,_serv_name,_serv_domain,_process_name,_node_name, & _pid, & _visibility, & _conn_type, & _ip1, & _port1, & _ip2,  & _port2, _unix_sock_path);
}


int SrvProt_notify_publish_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	// If a notify_publish-request primitive of SRV-SRV protocol is received,
	// the system has a Multi-AP configuration
	startup_session_group->system_configuration(ACS_DSD_StartupSessionsGroup::SYS_CONF_MULTI_AP);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("SrvProt_notify_publish_v1_request::process:: Set 'system_configuration' status to MULTI-AP!"));

	// allocate the primitive object to be used as response to the notify publish request
	SrvProt_notify_publish_v1_reply *p_resp_prim = new (std::nothrow) SrvProt_notify_publish_v1_reply();
	if(!p_resp_prim){
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	//  obtain exclusive access to IMM shared objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_shared_objects_conn_mutex);

	// check if we are the controller of IMM shared objects
	if(! is_imm_shared_objects_controller){
	 	p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED);
		response_primitive = p_resp_prim;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Cannot process 'notify publish' primitives on this node ! - this DSD server is not the controller of IMM shared objects"));
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);

	// fist of all, if the SRTNode for the AP remote node doesn't exist, we must create it
	if(immDHobj.fetch_NodeInfo(_node_name) != acs_dsd_imm::NO_ERROR){
		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t hostInfo;
		int call_result;
		if((call_result = ACS_DSD_ConfigurationHelper::get_node_by_node_name(_node_name, hostInfo)) < 0) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: ACS_DSD_ConfigurationHelper::get_node_by_node_name() invocation returned < %d > !"), __func__, call_result);
			_process_error_descr = "cannot obtain info about the remote AP node";
			delete p_resp_prim;
			return 	ACS_DSD_PRIM_PROCESS_ERROR;
		}

		immDHobj.set_state(acs_dsd::NODE_STATE_UNDEFINED);
		immDHobj.set_side(hostInfo->side);
		immDHobj.set_node(hostInfo->system_name);

		int create_node_res = immDHobj.addNodeInfo(_node_name);
		if(create_node_res < 0) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("failed SRTNode creation for remote AP Node < %s >. Return code of 'ACS_DSD_ImmDataHandler::addNodeInfo()' is < %d >"), _node_name, create_node_res);
			_process_error_descr = std::string("cannot create an IMM SRTNode object instance for the remote AP node < ") + hostInfo->node_name + " >";
			delete p_resp_prim;
			return 	ACS_DSD_PRIM_PROCESS_ERROR;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("SRTNode for AP Node < %s > successfully created."), _node_name);
	}

	// now we must add to IMM an SRVInfo object instance representing the published service ...

	// convert ip addresses and ports from integer format to text format
	const char *inet_addresses[2]; const char *unix_addresses[1];
	ACE_INET_Addr ip1_addr; ACE_INET_Addr ip2_addr;
	ip1_addr.set(_port1,_ip1); ip2_addr.set(_port2, _ip2);
	char addr_1_str[32] = {0}; char addr_2_str[32] = {0};
	ip1_addr.addr_to_string(addr_1_str, 32); ip2_addr.addr_to_string(addr_2_str, 32);
	inet_addresses[0] = addr_1_str; inet_addresses[1] = addr_2_str;
	unix_addresses[0] = (const char *) _unix_sock_path;
	immDHobj.set_proc_name( (char *) _process_name);
	immDHobj.set_pid(_pid);
	immDHobj.set_visibility(_visibility);
	immDHobj.set_conn_type(_conn_type);
	if(_conn_type == acs_dsd::CONNECTION_TYPE_INET) immDHobj.set_tcp_addresses(inet_addresses,2);
	else immDHobj.set_unix_addresses(unix_addresses,1);

	// try to create the SRVInfo object
	int op_res = immDHobj.addServiceInfo((char *)_node_name, (char *)_serv_name, (char *) _serv_domain);
	if(op_res == 0){
		// creation succeeded !
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
		response_primitive = p_resp_prim;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("successfully registered, in the local IMM repository, the service <%s@%s> published on the remote AP node '%s' !"), _serv_name, _serv_domain, _node_name);
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	// IMM object creation failed. May be that the object was yet present in IMM; so try a "modify" object operation
	op_res = immDHobj.modifyServiceInfo((char *)_node_name, (char *) _serv_name, (char *) _serv_domain);
	if(op_res==0){
		// modify succeeded !
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
		response_primitive = p_resp_prim;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("successfully registered, in the local IMM repository, the service <%s@%s> published on the remote AP node '%s' (overwriting previous registration)!"), _serv_name, _serv_domain, _node_name);
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	// IMM failure !
	p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_REGISTRATION_FAILURE);
	response_primitive = p_resp_prim;
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("IMM Error while processing 'notify publish' primitive !  Error descr: %s"), immDHobj.last_error_text());
	_process_error_descr = std::string("IMM failure: ") + immDHobj.last_error_text();
	return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
}


/* SrvProt_notify_publish_v1_reply methods */

SrvProt_notify_publish_v1_reply::SrvProt_notify_publish_v1_reply()
{
	_primitive_id = SRVSRV_NOTIFY_PUBLISH_REPLY;
	_error = 0;
}

std::string SrvProt_notify_publish_v1_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u",
				_primitive_id,
				_protocol_version,
				_error);

	return mess;
}

int SrvProt_notify_publish_v1_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<>  & pdh) const
{
	return  pdh.make_primitive(SRVSRV_NOTIFY_PUBLISH_REPLY, SRVSRV_PROT_V1, _error);
}

int  SrvProt_notify_publish_v1_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error);
}

int SrvProt_notify_publish_v1_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}


/* SrvProt_notify_unregister_v1_request methods */

SrvProt_notify_unregister_v1_request::SrvProt_notify_unregister_v1_request()
{
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_NOTIFY_UNREGISTER;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	memset(_node_name,'\0',sizeof(_node_name));
}

std::string SrvProt_notify_unregister_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"NODE NAME = %s",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain,
				_node_name);

	return mess;
}

int SrvProt_notify_unregister_v1_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int SrvProt_notify_unregister_v1_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int SrvProt_notify_unregister_v1_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int SrvProt_notify_unregister_v1_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}

int SrvProt_notify_unregister_v1_request::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_node_name, buff, buffsize);
}

int SrvProt_notify_unregister_v1_request::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}

int SrvProt_notify_unregister_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_NOTIFY_UNREGISTER, SRVSRV_PROT_V1, _serv_name, _serv_domain, _node_name);
}

int SrvProt_notify_unregister_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, _serv_name, _serv_domain, _node_name);
}

int SrvProt_notify_unregister_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	// If a notify_unregister-request primitive of SRV-SRV protocol is received,
	// the system has a Multi-AP configuration
	startup_session_group->system_configuration(ACS_DSD_StartupSessionsGroup::SYS_CONF_MULTI_AP);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("Prot_notify_unregister_v1_request::process:: Set 'system_configuration' status to MULTI-AP!"));

	// allocate the primitive object to be used as response to the notify unregister request
	SrvProt_notify_unregister_v1_reply  *p_resp_prim = new (std::nothrow) SrvProt_notify_unregister_v1_reply();
	if(!p_resp_prim){
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	//  obtain exclusive access to IMM shared objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_shared_objects_conn_mutex);

	// check if we are the controller of IMM shared objects
	if(! is_imm_shared_objects_controller){
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED);
		response_primitive = p_resp_prim;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Cannot process 'notify unregistration' primitives on this node - DSD server is not the controller of IMM shared objects!"));
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);
	// try to delete the IMM object for the service
	int op_res = immDHobj.deleteServiceInfo((char *)_node_name, (char *) _serv_name, (char *) _serv_domain);
	if(op_res == 0)
	{
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
		response_primitive = p_resp_prim;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("successfully deleted, from local IMM repository, the IMM entry regarding the service < %s@%s > registered on AP node '%s'   "), _serv_name, _serv_domain, _node_name);
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_UNREGISTER_FAILURE);
	response_primitive = p_resp_prim;
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("IMM Error while processing 'notify unregister' primitive !  Error descr: %s"), immDHobj.last_error_text());
	return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
}


/* SrvProt_notify_unregister_v1_reply methods */

SrvProt_notify_unregister_v1_reply::SrvProt_notify_unregister_v1_reply()
{
	_primitive_id = SRVSRV_NOTIFY_UNREGISTER_REPLY;
	_protocol_version = SRVSRV_PROT_V1;
	_error = 0;
}

std::string SrvProt_notify_unregister_v1_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u",
				_primitive_id,
				_protocol_version,
				_error);

	return mess;
}

int SrvProt_notify_unregister_v1_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<>  & pdh) const
{
	return  pdh.make_primitive(SRVSRV_NOTIFY_UNREGISTER_REPLY, SRVSRV_PROT_V1, _error);
}

int  SrvProt_notify_unregister_v1_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error);
}

int SrvProt_notify_unregister_v1_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}


