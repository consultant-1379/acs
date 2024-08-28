#include <memory>
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
#include "ACS_DSD_Client.h"
#include "ACS_DSD_SrvProt_NotificationPrimitives.h"
#include "ACS_DSD_AppProt_RegisterPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACE_Recursive_Thread_Mutex imm_conn_mutex;
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_Logger dsd_logger;


/* AppProt_registerINET_v1_request methods */

AppProt_registerINET_v1_request::AppProt_registerINET_v1_request()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_REGISTER_INET_REQUEST;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	memset(_process_name,'\0',sizeof(_process_name));
	memset(_unix_sock_path,'\0',sizeof(_unix_sock_path));
	_pid = 0;
	_visibility = 0;
	_ip1 = 0;
	_ip2 = 0;
	_port1 = 0;
	_port2 = 0;
}


std::string AppProt_registerINET_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"PROCESS_NAME = %s,"
				"PID = %u,"
				"VISIBILITY = %u,"
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
				_pid,
				_visibility,
				_ip1,
				_port1,
				_ip2,
				_port2,
				_unix_sock_path);


	return mess;
}

int AppProt_registerINET_v1_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int AppProt_registerINET_v1_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int AppProt_registerINET_v1_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int AppProt_registerINET_v1_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}

int AppProt_registerINET_v1_request::get_process_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_process_name, buff, buffsize);
}

int AppProt_registerINET_v1_request::set_process_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_process_name, str, strlen(str));
}

int AppProt_registerINET_v1_request::get_unix_path(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path, buff, buffsize);
}

int AppProt_registerINET_v1_request::set_unix_path(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path,str, strlen(str));
}

int AppProt_registerINET_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_REGISTER_INET_REQUEST, SVRAPP_PROTOCOL_VERSION_1, _serv_name, _serv_domain, _process_name, _pid, _visibility, _ip1, _port1, _ip2, _port2, _unix_sock_path);
}

int AppProt_registerINET_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version,_serv_name,_serv_domain,_process_name, & _pid, & _visibility, & _ip1, & _port1, & _ip2,  & _port2, _unix_sock_path);
	return pdh.unpack_primitive(_primitive_id, _protocol_version,_serv_name,_serv_domain,_process_name, & _pid, & _visibility, & _ip1, & _port1, & _ip2,  & _port2, _unix_sock_path);
}


int AppProt_registerINET_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const
{
	// allocate a new primitive object
	AppProt_register_v1_response   *p_resp_prim = new (std::nothrow) AppProt_register_v1_response();
	if(!p_resp_prim){
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	//  access to IMM to register the service
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immDHobj(immConnHandlerObj_ptr);

	// transform ip addresses and ports from integer format to text format
	const char *inet_addresses[2];
	const char *unix_addresses[1];
	ACE_INET_Addr ip1_addr;
	ACE_INET_Addr ip2_addr;
	ip1_addr.set(_port1,_ip1);
	ip2_addr.set(_port2, _ip2);
	char addr_1_str[32] = {0};
	char addr_2_str[32] = {0};
	ip1_addr.addr_to_string(addr_1_str, 32);
	ip2_addr.addr_to_string(addr_2_str, 32);
	inet_addresses[0] = addr_1_str;
	inet_addresses[1] = addr_2_str;
	unix_addresses[0] = (const char *) _unix_sock_path;

	// prepare the IMM object to be created
	immDHobj.set_proc_name( (char *) _process_name);
	immDHobj.set_pid(_pid);
	immDHobj.set_visibility(_visibility);
	immDHobj.set_tcp_addresses(inet_addresses,2);
	immDHobj.set_unix_addresses(unix_addresses,1);
	immDHobj.set_conn_type(acs_dsd::CONNECTION_TYPE_INET);

	// try to create a new object entry in IMM for the service
	int op_res = immDHobj.addServiceInfo(dsd_local_node.node_name, (char *)_serv_name, (char *) _serv_domain);
	if(op_res != 0){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Warning: addServiceInfo(%s@%s) returned < %d > !"), _serv_name, _serv_domain,op_res);
		// IMM object creation failed. May be that the object is already present in IMM; so try a "modify" operation on the object
		op_res = immDHobj.modifyServiceInfo(dsd_local_node.node_name, (char *) _serv_name, (char *) _serv_domain);
		if(op_res!=0){
			// IMM falure !
			p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_REGISTRATION_FAILURE);
			response_primitive = p_resp_prim;
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Service <%s@%s> NOT registered!  Error descr: %s"), _serv_name, _serv_domain, immDHobj.last_error_text());
			_process_error_descr = std::string("IMM failure: ") + immDHobj.last_error_text();
			return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
		}
	}
	// release the mutex for IMM access control
	guard.release();

	// registration successful !
	std::auto_ptr<AppProt_register_v1_response> auto_ptr_resp_prim(p_resp_prim);
	p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Service <%s@%s> successfully registered!"), _serv_name, _serv_domain);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"), service_handler->getID(), p_resp_prim->to_text().c_str());
	if(service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0)
	{
		_process_error_descr = "Error sending register response of AppProt protocol";
		return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("AppProt_registerINET_v1_request::process(): CONN_ID = < %u >.  The response primitive has been sent !"), service_handler->getID());

	// now send registration notify to the other DSD servers in the system
	notify_to_all_AP();

	return ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT;
}


int AppProt_registerINET_v1_request::notify_to_AP(int32_t ap_system_id) const
{
	ACS_DSD_Client cli;
	ACS_DSD_SrvProt_SvcHandler srvProtHandler;
	int resultCode;

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[] = "AppProt_registerINET_v1_request::notify_to_AP";
#endif

	ACS_DSD_ServicePrimitive *recv_primitive = 0;
	SrvProt_notify_publish_v1_reply  *p_SrvProt_reply;
	uint8_t primitive_rcvd;
	uint8_t response_code;

	char ap_node_name[acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]= {{0}};

	resultCode= ACS_DSD_Utils::get_ApNodesName(ap_system_id, ap_node_name);
	if (resultCode == acs_dsd::ERR_NODE_NOT_FOUND){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: get_ApNodesName():error finding AP node names with systemId=%d,  erroCode = %d"), FX_NAME, ap_system_id, resultCode);
		return NOTIFICATION_GENERIC_ERROR;
	}
	for(int i=0; i<acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED; i++){
		if ((resultCode = cli.connect(srvProtHandler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, ap_system_id, ap_node_name[i], DSD_SERVER_CONNECT_TIMEOUT_VALUE))){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: error connecting to AP=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, ap_system_id, ap_node_name[i],resultCode,cli.last_error_text());
			// error connecting to DSD server running on ap_node_name,
			// try with DSD server running on the other ap_node of the cluster
			continue;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: Established connection to the AP node <%s>. CONN ID = < %u >"), FX_NAME, ap_node_name[i], srvProtHandler.getID());

		// AP name found, and the connection with DSD server is established!!!
		// Now send  register notify primitive to DSD server

		SrvProt_notify_publish_v1_request notify_register_prim;

		notify_register_prim.set_service_domain((char *)_serv_domain);
		notify_register_prim.set_service_name((char *)_serv_name);
		notify_register_prim.set_process_name((char *)_process_name);
		notify_register_prim.set_node_name(dsd_local_node.node_name);
		notify_register_prim.set_pid(_pid);
		notify_register_prim.set_visibility(_visibility);
		notify_register_prim.set_conn_type(acs_dsd::CONNECTION_TYPE_INET);
		notify_register_prim.set_ip_address_1(_ip1);
		notify_register_prim.set_ip_address_2(_ip2);
		notify_register_prim.set_port_1(_port1);
		notify_register_prim.set_port_2(_port2);
		notify_register_prim.set_unix_path((char *) _unix_sock_path);

		if (!srvProtHandler.sendPrimitive(&notify_register_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. The registration notify primitive (prim 58) has been sent to remote DSD !"),FX_NAME, srvProtHandler.getID());
		else{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. Failed to send registration notify primitive (prim 58) to remote DSD !"), FX_NAME, srvProtHandler.getID());
			// error sending primitive to DSD server running on ap_node_name,
			// try on DSD server running on the other ap_node of the cluster
			continue;
		}
		// read the reply primitive sent by DSD server and check the result
		resultCode= srvProtHandler.recvPrimitive(recv_primitive,DSD_SERVER_RECV_TIMEOUT_VALUE);

		switch(resultCode)
		{
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK:
			{
				// take care of memory deallocation
				std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(recv_primitive);

				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Received primitive.  Primitive data : %s"),FX_NAME, srvProtHandler.getID(), recv_primitive->to_text().c_str());
				primitive_rcvd = recv_primitive->get_primitive_id();
				if(primitive_rcvd != acs_dsd::PCP_DSDDSD_NOTIFY_PUBLISH_REPLY_ID ){
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: remote DSD server replied with the primitive number %d instead of the primitive number 59"), FX_NAME, primitive_rcvd);
					return NOTIFICATION_PROTOCOL_ERROR;
				}
				p_SrvProt_reply = dynamic_cast<SrvProt_notify_publish_v1_reply *>(recv_primitive);
				response_code = p_SrvProt_reply->get_error_field();
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: remote DSD server replied with response code < %d >"), FX_NAME, response_code);
				if(response_code != acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED)
					return NOTIFICATION_OK;
			}
			break;
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_INVALID_PRIMITIVE:
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: remote DSD Server replied with an invalid or not implemented primitive"), FX_NAME);
				return NOTIFICATION_PROTOCOL_ERROR;
			}
			default:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: ACS_DSD_SrvProt_SvcHandler::recvPrimitive() invocation returned < %d >"),FX_NAME, resultCode);
			return NOTIFICATION_RESP_RECEIVE_ERROR;
		}
	}
	return NOTIFICATION_OK;
}


/*
int AppProt_registerINET_v1_request::test_notify_to_AP() const
{
	ACS_DSD_Client cli;
	ACS_DSD_SrvProt_SvcHandler srvProtHandler;
	int resultCode;

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[] = "AppProt_registerINET_v1_request::test_notify_to_AP()";
#endif

	ACS_DSD_ServicePrimitive *recv_primitive = 0;
	SrvProt_notify_publish_v1_reply  *p_SrvProt_reply;
	uint8_t primitive_rcvd;
	uint8_t response_code;

	if ((resultCode = cli.connect(srvProtHandler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, dsd_local_node.system_id, dsd_local_node.node_name))){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: error connecting to AP=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, dsd_local_node.system_id, dsd_local_node.node_name, resultCode, cli.last_error_text());
		// error connecting to DSD server running on ap_node_name,
		return -1;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Established connection to the AP node <%s>. CONN ID = < %u >"), FX_NAME, dsd_local_node.node_name, srvProtHandler.getID());

	// AP name found, and the connection with DSD server is established!!!
	// Now send  register notify primitive to DSD server

	SrvProt_notify_publish_v1_request notify_register_prim;

	notify_register_prim.set_service_domain((char *)_serv_domain);
	notify_register_prim.set_service_name((char *)_serv_name);
	notify_register_prim.set_process_name((char *)_process_name);
	notify_register_prim.set_node_name("AP2A");
	notify_register_prim.set_pid(_pid);
	notify_register_prim.set_visibility(_visibility);
	notify_register_prim.set_conn_type(acs_dsd::CONNECTION_TYPE_INET);
	notify_register_prim.set_ip_address_1(_ip1);
	notify_register_prim.set_ip_address_2(_ip2);
	notify_register_prim.set_port_1(_port1);
	notify_register_prim.set_port_2(_port2);
	notify_register_prim.set_unix_path((char *) _unix_sock_path);

	if (!srvProtHandler.sendPrimitive(&notify_register_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. The registration notify primitive (prim 58) has been sent to DSD !"),FX_NAME, srvProtHandler.getID());
	else{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. Failed to send registration notify primitive (prim 58) to DSD !"), FX_NAME, srvProtHandler.getID());
		// error sending primitive to DSD server running on ap_node_name,
		return -1;
	}
	// read the reply primitive sent by DSD server and check the result
	resultCode= srvProtHandler.recvPrimitive(recv_primitive,DSD_SERVER_RECV_TIMEOUT_VALUE);
	// take care of memory deallocation
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(recv_primitive);
	switch(resultCode)
	{
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Received primitive.  Primitive data : %s"),FX_NAME, srvProtHandler.getID(), recv_primitive->to_text().c_str());
		primitive_rcvd = recv_primitive->get_primitive_id();
		if(primitive_rcvd != acs_dsd::PCP_DSDDSD_NOTIFY_PUBLISH_REPLY_ID ){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: DSD server replied with the primitive number %d instead of the primitive number 59"), FX_NAME,primitive_rcvd);
			return -1;
		}
		p_SrvProt_reply = dynamic_cast<SrvProt_notify_publish_v1_reply *>(recv_primitive);
		response_code = p_SrvProt_reply->get_error_field();
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: DSD server replied with code %d"), FX_NAME, response_code);
		if(response_code != acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED)
			return 0;
		break;
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_INVALID_PRIMITIVE:
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: DSD replied with an invalid or not implemented primitive"),FX_NAME);
			return -1;
		}
		default:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: DSD server reply error %d"),FX_NAME, resultCode);
		return -1;
	}

	return 0;
}
*/


/* AppProt_registerUNIX_v1_request methods */

AppProt_registerUNIX_v1_request::AppProt_registerUNIX_v1_request()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_REGISTER_UNIX_REQUEST;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	memset(_process_name,'\0',sizeof(_process_name));
	memset(_unix_sock_name,'\0',sizeof(_unix_sock_name));
	_pid = 0;
	_visibility = IMM_DH_GLOBAL_SCOPE;
}


std::string AppProt_registerUNIX_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"PROCESS_NAME = %s,"
				"PID = %u,"
				"VISIBILITY = %u,"
				"UNIX_SOCK_NAME = %s",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain,
				_process_name,
				_pid,
				_visibility,
				_unix_sock_name);

	return mess;
}

int AppProt_registerUNIX_v1_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int AppProt_registerUNIX_v1_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}


int AppProt_registerUNIX_v1_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int AppProt_registerUNIX_v1_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}

int AppProt_registerUNIX_v1_request::get_process_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_process_name, buff, buffsize);
}

int AppProt_registerUNIX_v1_request::set_process_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_process_name, str, strlen(str));
}

int AppProt_registerUNIX_v1_request::get_unix_sock_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_name, buff, buffsize);
}

int AppProt_registerUNIX_v1_request::set_unix_sock_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_name, str, strlen(str));
}

int AppProt_registerUNIX_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_REGISTER_UNIX_REQUEST, SVRAPP_PROTOCOL_VERSION_1, _serv_name, _serv_domain, _process_name, _pid, _visibility, _unix_sock_name);
}

int AppProt_registerUNIX_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version,_serv_name,_serv_domain,_process_name, & _pid, & _visibility, _unix_sock_name);
	return pdh.unpack_primitive(_primitive_id, _protocol_version,_serv_name,_serv_domain,_process_name, & _pid, & _visibility, _unix_sock_name);
}

int AppProt_registerUNIX_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const
{
	// allocate a new primitive object
	AppProt_register_v1_response   *p_resp_prim = new (std::nothrow) AppProt_register_v1_response();
	if(!p_resp_prim){
		_process_error_descr = "Memory not avaliable to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	//  access to IMM to register the service
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immDHobj(immConnHandlerObj_ptr);
	const char *unix_addresses[1];
	unix_addresses[0] = (const char *) this->_unix_sock_name;

	// prepare the IMM object to be created
	immDHobj.set_proc_name( (char *) _process_name);
	immDHobj.set_pid(_pid);
	immDHobj.set_unix_addresses(unix_addresses, 1);
	immDHobj.set_conn_type(acs_dsd::CONNECTION_TYPE_UNIX);

	// try to create a new object in IMM for the service
	int op_res = immDHobj.addServiceInfo(dsd_local_node.node_name, (char *)_serv_name, (char *) _serv_domain);
	if(op_res != 0){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Warning: addServiceInfo(%s@%s) returned < %d > !"), _serv_name, _serv_domain,op_res);
		// IMM object creation failed. May be that the object was yet present in IMM; so try a "modify" object
		op_res = immDHobj.modifyServiceInfo(dsd_local_node.node_name, (char *) _serv_name, (char *) _serv_domain);
		if(op_res !=0)
		{
			p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_REGISTRATION_FAILURE);
			response_primitive = p_resp_prim;
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Service <%s@%s> NOT registered! Error descr: %s"), _serv_name, _serv_domain, immDHobj.last_error_text());
			_process_error_descr = std::string("IMM failure: ") + immDHobj.last_error_text();
			return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
		}
	}
	guard.release();

	// registration succeeded !

	// the response primitive will be sent to the peer within this method: so take care of memory deallocation
	std::auto_ptr<AppProt_register_v1_response> auto_ptr_resp_prim(p_resp_prim);

	p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Service <%s@%s> successfully registered!"), _serv_name, _serv_domain);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"), service_handler->getID(), p_resp_prim->to_text().c_str());
	if(service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard (imm_conn_mutex);
		immDHobj.deleteServiceInfo(dsd_local_node.node_name, (char *)_serv_name, (char *) _serv_domain);
		_process_error_descr = "Error sending register response for DSD-AP Protocol primitive";
		return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("AppProt_registerUNIX_v1_request::process(): CONN_ID = < %u >.  The response primitive has been sent !"), service_handler->getID());

	// now send registration notify to a DSD server of all clusters in the system
	notify_to_all_AP();

	return ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT;
}


int AppProt_registerUNIX_v1_request::notify_to_AP(int32_t ap_system_id) const
{
	ACS_DSD_Client cli;
	ACS_DSD_SrvProt_SvcHandler srvProtHandler;
	int resultCode;

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[] = "AppProt_registerUNIX_v1_request::notify_to_AP";
#endif

	ACS_DSD_ServicePrimitive *recv_primitive = 0;
	SrvProt_notify_publish_v1_reply  *p_SrvProt_reply;
	uint8_t primitive_rcvd;
	uint8_t response_code;

	char ap_node_name[acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]= {{0}};

	resultCode= ACS_DSD_Utils::get_ApNodesName(ap_system_id, ap_node_name);
	if (resultCode == acs_dsd::ERR_NODE_NOT_FOUND){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: get_ApNodesName():error finding AP node names with systemId=%d,  erroCode = %d"), FX_NAME, ap_system_id, resultCode);
		return NOTIFICATION_GENERIC_ERROR;
	}
	for(int i=0; i<acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED; i++){
		if ((resultCode = cli.connect(srvProtHandler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, ap_system_id, ap_node_name[i], DSD_SERVER_CONNECT_TIMEOUT_VALUE))){
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: error connecting to AP=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, ap_system_id, ap_node_name[i], resultCode, cli.last_error_text());
			// error connecting to DSD server running on ap_node_name,
			// try with DSD server running on the other ap_node of the cluster
			continue;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Established connection to the AP node <%s>. CONN ID = < %u >"), FX_NAME, ap_node_name[i], srvProtHandler.getID());

		// the connection with DSD server is established!!!
		// Now send  register notify primitive to DSD server

		SrvProt_notify_publish_v1_request notify_register_prim;

		notify_register_prim.set_service_domain((char *)_serv_domain);
		notify_register_prim.set_service_name((char *)_serv_name);
		notify_register_prim.set_process_name((char *)_process_name);
		notify_register_prim.set_node_name(dsd_local_node.node_name);
		notify_register_prim.set_pid(_pid);
		notify_register_prim.set_visibility(_visibility);
		notify_register_prim.set_conn_type(acs_dsd::CONNECTION_TYPE_UNIX);
		notify_register_prim.set_unix_path((char *)_unix_sock_name);

		if (!srvProtHandler.sendPrimitive(&notify_register_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. The registration notify primitive (prim 58) has been sent to remote DSD !"),FX_NAME, srvProtHandler.getID());
		else{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. Failed to send registration notify primitive (prim 58) to remote DSD !"), FX_NAME, srvProtHandler.getID());
			// error sending primitive to DSD server running on ap_node_name,
			// try on DSD server running on the other ap_node of the cluster
			continue;
		}
		// read the reply primitive sent by DSD server and check the result
		resultCode= srvProtHandler.recvPrimitive(recv_primitive, DSD_SERVER_RECV_TIMEOUT_VALUE);

		switch(resultCode)
		{
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK:
			{
				// take care of memory deallocation
				std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(recv_primitive);

				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Received primitive.  Primitive data : %s"),FX_NAME, srvProtHandler.getID(), recv_primitive->to_text().c_str());
				primitive_rcvd = recv_primitive->get_primitive_id();
				if(primitive_rcvd != acs_dsd::PCP_DSDDSD_NOTIFY_PUBLISH_REPLY_ID ){
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: remote DSD server replied with the primitive number %d instead of the primitive number 59"), FX_NAME, primitive_rcvd);
					return NOTIFICATION_PROTOCOL_ERROR;
				}
				p_SrvProt_reply = dynamic_cast<SrvProt_notify_publish_v1_reply *>(recv_primitive);
				response_code = p_SrvProt_reply->get_error_field();
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: remote DSD server replied with response code < %d >"), FX_NAME, response_code);
				if(response_code != acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED)
					return NOTIFICATION_OK;
			}
			break;
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_INVALID_PRIMITIVE:
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: DSD replied with an invalid or not implemented primitive"),FX_NAME);
				return NOTIFICATION_PROTOCOL_ERROR;
			}

			default:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: ACS_DSD_SrvProt_SvcHandler::recvPrimitive() invocation returned < %d >"),FX_NAME, resultCode);
			return NOTIFICATION_RESP_RECEIVE_ERROR;
		}
	}
	return NOTIFICATION_OK;
}

/*
int AppProt_registerUNIX_v1_request::test_notify_to_AP() const
{
	ACS_DSD_Client cli;
	ACS_DSD_SrvProt_SvcHandler srvProtHandler;
	int resultCode;

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[] = " AppProt_registerUNIX_v1_request::test_notify_to_AP()";
#endif

	ACS_DSD_ServicePrimitive *recv_primitive = 0;
	SrvProt_notify_publish_v1_reply  *p_SrvProt_reply;
	uint8_t primitive_rcvd;
	uint8_t response_code;

	if ((resultCode = cli.connect(srvProtHandler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, dsd_local_node.system_id, dsd_local_node.node_name))){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: error connecting to AP=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, dsd_local_node.system_id, dsd_local_node.node_name, resultCode, cli.last_error_text());
		// error connecting to DSD server running on ap_node_name,
		return -1;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Established connection to the AP node <%s>. CONN ID = < %u >"), FX_NAME, dsd_local_node.node_name, srvProtHandler.getID());

	// AP name found, and the connection with DSD server is established!!!
	// Now send  register notify primitive to DSD server

	SrvProt_notify_publish_v1_request notify_register_prim;

	notify_register_prim.set_service_domain((char *)_serv_domain);
	notify_register_prim.set_service_name((char *)_serv_name);
	notify_register_prim.set_process_name((char *)_process_name);
	notify_register_prim.set_node_name("AP2A");
	notify_register_prim.set_pid(_pid);
	notify_register_prim.set_visibility(_visibility);
	notify_register_prim.set_conn_type(acs_dsd::CONNECTION_TYPE_UNIX);
	notify_register_prim.set_unix_path((char *)_unix_sock_name);

	if (!srvProtHandler.sendPrimitive(&notify_register_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. The registration notify primitive (prim 58) has been sent to DSD !"),FX_NAME, srvProtHandler.getID());
	else{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. Failed to send registration notify primitive (prim 58) to DSD !"), FX_NAME, srvProtHandler.getID());
		// error sending primitive to DSD server running on ap_node_name,
		return -1;
	}
	// read the reply primitive sent by DSD server and check the result
	resultCode= srvProtHandler.recvPrimitive(recv_primitive, DSD_SERVER_RECV_TIMEOUT_VALUE);
	// take care of memory deallocation
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(recv_primitive);
	switch(resultCode)
	{
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Received primitive.  Primitive data : %s"),FX_NAME, srvProtHandler.getID(), recv_primitive->to_text().c_str());
		primitive_rcvd = recv_primitive->get_primitive_id();
		if(primitive_rcvd != acs_dsd::PCP_DSDDSD_NOTIFY_PUBLISH_REPLY_ID ){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: DSD server replied with the primitive number %d instead of the primitive number 59"), FX_NAME,primitive_rcvd);
			return -1;
		}
		p_SrvProt_reply = dynamic_cast<SrvProt_notify_publish_v1_reply *>(recv_primitive);
		response_code = p_SrvProt_reply->get_error_field();
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: DSD server replied with %s"),FX_NAME,ACS_DSD_ProtocolHandler::error_code_description((acs_dsd::PCP_ErrorCodesConstants)response_code));
		if(response_code != acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED)
			return 0;
		break;
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_INVALID_PRIMITIVE:
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: DSD replied with an invalid or not implemented primitive"),FX_NAME);
			return -1;
		}
		default:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: DSD server reply error %d"),FX_NAME, resultCode);
		return -1;
	}

	return 0;
}
*/

/* AppProt_register_v1_response methods */

AppProt_register_v1_response::AppProt_register_v1_response()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_REGISTER_RESPONSE;
	_error = 0;
}

std::string AppProt_register_v1_response::to_text() const
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

int AppProt_register_v1_response::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<>  & pdh) const
{
	return  pdh.make_primitive(SVRAPP_REGISTER_RESPONSE, SVRAPP_PROTOCOL_VERSION_1, _error);
}

int  AppProt_register_v1_response::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error);
}

int AppProt_register_v1_response::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}
