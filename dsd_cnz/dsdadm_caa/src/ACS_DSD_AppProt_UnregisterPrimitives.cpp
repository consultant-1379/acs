#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ImmConnectionHandler.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_Macros.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_SrvProt_NotificationPrimitives.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"
#include "ACS_DSD_AppProt_UnregisterPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACE_Recursive_Thread_Mutex imm_conn_mutex;
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_Logger dsd_logger;

/* AppProt_unregister_v1_request methods */

AppProt_unregister_v1_request::AppProt_unregister_v1_request()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_UNREGISTER_REQUEST;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
}

std::string AppProt_unregister_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain);

	return mess;
}

int AppProt_unregister_v1_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int AppProt_unregister_v1_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int AppProt_unregister_v1_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int AppProt_unregister_v1_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}

int AppProt_unregister_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_UNREGISTER_RESPONSE, SVRAPP_PROTOCOL_VERSION_1, _serv_name, _serv_domain);
}

int AppProt_unregister_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, _serv_name, _serv_domain);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, _serv_name, _serv_domain);
}

int AppProt_unregister_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const
{
	// allocate the primitive object for the response
	AppProt_unregister_v1_response   *p_resp_prim = new (std::nothrow) AppProt_unregister_v1_response();
	if(!p_resp_prim)
	{
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	//  access to IMM to unregister the service
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immDHobj(immConnHandlerObj_ptr);

	// try to delete the IMM object for the service
	int op_res = immDHobj.deleteServiceInfo(dsd_local_node.node_name, (char *) _serv_name, (char *) _serv_domain);
	if(op_res != 0)
	{
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED);
		response_primitive = p_resp_prim;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Service <%s@%s> is NOT registered!  Error descr: %s"), _serv_name, _serv_domain, immDHobj.last_error_text());
		_process_error_descr = std::string("IMM failure: ") + immDHobj.last_error_text();
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	// release the mutex for IMM access control
	guard.release();

	// the IMM object has been successfully deleted ! Send response primitive to the client
	std::auto_ptr<AppProt_unregister_v1_response> auto_ptr_resp_prim(p_resp_prim);
	p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Service < %s@%s > successfully unregistered !"), _serv_name, _serv_domain);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"), service_handler->getID(), p_resp_prim->to_text().c_str());
	if(service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0)
	{
		_process_error_descr = "Error sending 'unregister response' of AppProt protocol";
		return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("AppProt_unregister_v1_request::process(): CONN_ID = < %u >.  The response primitive has been sent !"), service_handler->getID());

	// now send unregistration notify to a DSD server of all clusters in the system
	notify_to_all_AP();

	return ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT;
}


int AppProt_unregister_v1_request::notify_to_AP(int32_t ap_system_id) const
{
	ACS_DSD_Client cli;
	ACS_DSD_SrvProt_SvcHandler srvProtHandler;
	int resultCode;

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[] = "AppProt_unregister_v1_request::notify_to_AP";
#endif

	ACS_DSD_ServicePrimitive *recv_primitive = 0;
	SrvProt_notify_unregister_v1_reply  *p_SrvProt_reply;
	uint8_t primitive_rcvd;
	uint8_t response_code;

	char ap_node_name[acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]= {{0}};

	resultCode= ACS_DSD_Utils::get_ApNodesName(ap_system_id, ap_node_name);
	if (resultCode == acs_dsd::ERR_NODE_NOT_FOUND){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: get_ApNodesName():error finding AP node names with systemId=%d,  erroCode = %d"), FX_NAME,ap_system_id, resultCode);
		return NOTIFICATION_GENERIC_ERROR;
	}
	for(int i=0; i<acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED; i++){
		if ((resultCode = cli.connect(srvProtHandler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, ap_system_id, ap_node_name[i], DSD_SERVER_CONNECT_TIMEOUT_VALUE))){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: error connecting to AP=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, ap_system_id, ap_node_name[i], resultCode, cli.last_error_text());
			// error connecting to DSD server running on ap_node_name,
			// try with DSD server running on the other ap_node of the cluster
			continue;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: Established connection to the AP node <%s>. CONN ID = < %u >"), FX_NAME, ap_node_name[i], srvProtHandler.getID());

		// AP name found, and the connection with DSD server is established!!!
		// Now send  unregister notify primitive to DSD server

		SrvProt_notify_unregister_v1_request notify_unregister_prim;
		notify_unregister_prim.set_service_domain((char *)_serv_domain);
		notify_unregister_prim.set_service_name((char *)_serv_name);
		notify_unregister_prim.set_node_name(dsd_local_node.node_name);

		if (!srvProtHandler.sendPrimitive(& notify_unregister_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. The unregistration notify primitive (prim 60) has been sent to remote DSD !"),FX_NAME, srvProtHandler.getID());
		else{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. Failed to send unregistration notify primitive (prim 60) to remote DSD !"), FX_NAME, srvProtHandler.getID());
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
				if(primitive_rcvd != acs_dsd::PCP_DSDDSD_NOTIFY_UNREGISTER_REPLY_ID ){
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: unregistration notify (prim 60) error: DSD server replied with the primitive number %d instead of the primitive number 61"), FX_NAME, primitive_rcvd);
					return NOTIFICATION_PROTOCOL_ERROR;
				}
				p_SrvProt_reply = dynamic_cast<SrvProt_notify_unregister_v1_reply *>(recv_primitive);
				response_code = p_SrvProt_reply->get_error_field();
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s: remote DSD server replied with response code < %d >"), FX_NAME, response_code);
				if(response_code != acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED)
					return NOTIFICATION_OK;
			}
			break;
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_INVALID_PRIMITIVE:
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: remote DSD Server replied with an invalid or not implemented primitive"),FX_NAME);
				return NOTIFICATION_PROTOCOL_ERROR;
			}
			default:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 60) error: ACS_DSD_SrvProt_SvcHandler::recvPrimitive() invocation returned < %d >"),FX_NAME, resultCode);
			return NOTIFICATION_RESP_RECEIVE_ERROR;
		}
	}
	return NOTIFICATION_OK;
}

/*
int AppProt_unregister_v1_request::test_notify_to_AP() const
{
	ACS_DSD_Client cli;
	ACS_DSD_SrvProt_SvcHandler srvProtHandler;
	int resultCode;

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[] = " AppProt_unregister_v1_request::test_notify_to_AP()";
#endif

	ACS_DSD_ServicePrimitive *recv_primitive = 0;
	SrvProt_notify_unregister_v1_reply  *p_SrvProt_reply;
	uint8_t primitive_rcvd;
	uint8_t response_code;

	if ((resultCode = cli.connect(srvProtHandler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, dsd_local_node.system_id, dsd_local_node.node_name))){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: error connecting to AP=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, dsd_local_node.system_id, dsd_local_node.node_name, resultCode, cli.last_error_text());
		// error connecting to DSD server running on ap_node_name,
		return NOTIFICATION_CONN_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Established connection to the AP node <%s>. CONN ID = < %u >"), FX_NAME, dsd_local_node.node_name, srvProtHandler.getID());

	// AP name found, and the connection with DSD server is established!!!
	// Now send  register notify primitive to DSD server

	SrvProt_notify_unregister_v1_request notify_unregister_prim;

	notify_unregister_prim.set_service_domain((char *)_serv_domain);
	notify_unregister_prim.set_service_name((char *)_serv_name);
	notify_unregister_prim.set_node_name("AP2A");

	if (!srvProtHandler.sendPrimitive(&notify_unregister_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. The unregistration notify primitive (prim 60) has been sent to DSD !"),FX_NAME, srvProtHandler.getID());
	else{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. Failed to send unregistration notify primitive (prim 60) to DSD !"), FX_NAME, srvProtHandler.getID());
		// error sending primitive to DSD server running on ap_node_name,
		return NOTIFICATION_REQ_SEND_ERROR;
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
		if(primitive_rcvd != acs_dsd::PCP_DSDDSD_NOTIFY_UNREGISTER_REPLY_ID ){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 58) error: DSD server replied with the primitive number %d instead of the primitive number 61"), FX_NAME, primitive_rcvd);
			return NOTIFICATION_PROTOCOL_ERROR;
		}
		p_SrvProt_reply = dynamic_cast<SrvProt_notify_unregister_v1_reply *>(recv_primitive);
		response_code = p_SrvProt_reply->get_error_field();
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: DSD server replied with response code < %d >"),FX_NAME,response_code);
		if(response_code != acs_dsd::PCP_ERROR_CODE_OPERATION_NOT_ALLOWED)
			return NOTIFICATION_OK;
		break;
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_INVALID_PRIMITIVE:
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: DSD replied with an invalid or not implemented primitive"),FX_NAME);
			return NOTIFICATION_PROTOCOL_ERROR;
		}
		default:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: registration notify (prim 60) error: DSD server reply error %d"),FX_NAME, resultCode);
		return NOTIFICATION_RESP_RECEIVE_ERROR;
	}
	return NOTIFICATION_OK;
}
*/

/* AppProt_unregister_v1_response methods */

AppProt_unregister_v1_response::AppProt_unregister_v1_response()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_UNREGISTER_RESPONSE;
	_error = 0;
}

std::string AppProt_unregister_v1_response::to_text() const
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

int AppProt_unregister_v1_response::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<>  & pdh) const
{
	return  pdh.make_primitive(SVRAPP_UNREGISTER_RESPONSE, SVRAPP_PROTOCOL_VERSION_1, _error);
}

int  AppProt_unregister_v1_response::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, &_error);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error);
}

int AppProt_unregister_v1_response::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}
