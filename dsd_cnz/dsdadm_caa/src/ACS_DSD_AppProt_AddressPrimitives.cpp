#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"
#include "ACS_DSD_CpProt_AddressPrimitives.h"
#include "ACS_DSD_SrvProt_AddressPrimitives.h"
#include "ACS_DSD_AppProt_AddressPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

/* AppProt_address_v1_request methods */
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger dsd_logger;


AppProt_address_v1_request::AppProt_address_v1_request()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_ADDR_REQUEST;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	_system_id = 0;
	_node_state = 0;
}

std::string AppProt_address_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"SYSTEM_ID = %u,"
				"NODE_STATE = %u ",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain,
				_system_id,
				_node_state);

	return mess;
}

int AppProt_address_v1_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int AppProt_address_v1_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int AppProt_address_v1_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int AppProt_address_v1_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}


int AppProt_address_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_ADDR_REQUEST, SVRAPP_PROTOCOL_VERSION_1, _serv_name, _serv_domain, _system_id, _node_state);
}

int AppProt_address_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version,_serv_name,_serv_domain, &_system_id, & _node_state);
}

int AppProt_address_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	/*
	 *  Step 1:  Check if the request is relative to the current AP system.
	 *  Step 1a: (address request for a service on the current AP System)
	 *  	This case shouldn't occur ( the ACS_DSD_Client::Connect() method of the API should handle it. Don't handle for the moment
	 *  Step 1b:  (address request for a service that is on a CP System ) goto step 2
	 *  Step 1c:  (address request for a service that is on another AP System ) goto step 4
	 *  Step 2:  Send an address request to the CP System, using CP-AP Protocol
	 *  Step 3:  receive CP/AP response, process it and goto step 6
	 *  Step 4:  Send an address request to the AP System, using SVR-APP Protocol
	 *  Step 5:  receive AP response and process it
	 *  Step 6: ...
	 */

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="AppProt_address_v1_request::process";
#endif
	int returnCode = ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s: - applName = %s, domain = %s, systemId = %d, state = %d"), FX_NAME, _serv_name,_serv_domain,_system_id,_node_state);

	acs_dsd::NodeStateConstants local_node_state;
	std::string error_descr;
	ACS_DSD_Node node_info;
	int op_res = ACS_DSD_Utils::get_node_info(node_info,error_descr);
	if(op_res<0){
		_process_error_descr = " ACS_DSD_Utils::get_node_info()failed!";
		ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Utils::get_node_info() failure ! return code is < %d > - description is < %s > !"), op_res, error_descr.c_str());
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}
	local_node_state = node_info.node_state;
	acs_dsd::SystemTypeConstants system_type;
	system_type = ACS_DSD_Utils::get_system_type(_system_id);
	switch(system_type){
		case acs_dsd::SYSTEM_TYPE_AP:
		{
			if(dsd_local_node.system_id == _system_id) {
				//This should not occur: the DSD API does not send the address request for local node
				// At the moment return a response reporting request failure!
				AppProt_addressUNIX_v1_response  *p_resp_prim = new (std::nothrow) AppProt_addressUNIX_v1_response();
				if(!p_resp_prim){
					_process_error_descr = "Memory not available to allocate the primitive object";
					return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
				}
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: < %s,%s,%d,%d >: Address Request Failed!!"),FX_NAME,_serv_name,_serv_domain,_system_id,_node_state);
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
				response_primitive = p_resp_prim;
			}
			else{
				 // send Address request to remote AP and prepare the reply primitive
				int call_result = queryAddressestoAP(response_primitive, _node_state);
				// if the reply was not successful, then reply address request failure!!
				if (call_result != ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE){
					AppProt_addressINET_v1_response  *p_resp_prim = new (std::nothrow) AppProt_addressINET_v1_response();
					if(!p_resp_prim){
						_process_error_descr = "Memory not available to allocate the primitive object";
						return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
					}
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: < %s,%s,%d,%d >: Address Request Failed!!"),FX_NAME,_serv_name,_serv_domain,_system_id,_node_state);
					p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
					response_primitive = p_resp_prim;
				}
				// else nothing to do: the reply primitive was already set.
			}
		}
		break;
		case acs_dsd::SYSTEM_TYPE_BC:
		case acs_dsd::SYSTEM_TYPE_CP:
		{
			// allocate a new primitive object
			AppProt_addressINET_v1_response  *p_resp_prim = new (std::nothrow) AppProt_addressINET_v1_response();
			if(!p_resp_prim){
				_process_error_descr = "Memory not available to allocate the primitive object";
				return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
			}
			int32_t cs_systemId = 0;
			if(_system_id == acs_dsd::SYSTEM_ID_CP_CLOCK_MASTER)
				cs_systemId = cpNodesManager.get_clock_master_cp_system_id();
			else if(_system_id == acs_dsd::SYSTEM_ID_CP_ALARM_MASTER)
				cs_systemId = cpNodesManager.get_alarm_master_cp_system_id();
			else
				cs_systemId = _system_id;

			if (cs_systemId == -1){
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_NODE_NOT_VALID);
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: < %s,%s,%d,%d > Failed!! system_id not valid"),FX_NAME,_serv_name,_serv_domain,_system_id,_node_state);
				response_primitive = p_resp_prim;
				return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
			}
			ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
			int call_result = queryAddressestoCP(cs_systemId, inet_addresses);

			if(call_result == ACS_DSD_PRIM_SERVICE_NOT_REGISTERED ){
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("%s: < %s,%s,%d,%d >: CP replied Service Not Registered!!"),FX_NAME, _serv_name, _serv_domain, cs_systemId, _node_state);
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED);
			}
			else if(call_result == ACS_DSD_PRIM_VALIDATION_OK ){
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,ACS_DSD_TEXT ("%s: < %s,%s,%d,%d >: Address Request Reply OK!!"),FX_NAME, _serv_name, _serv_domain, cs_systemId, _node_state);
				p_resp_prim->set_ip_address_1(inet_addresses[0].get_ip_address());
				p_resp_prim->set_port_1(inet_addresses[0].get_port_number());
				p_resp_prim->set_ip_address_2(inet_addresses[1].get_ip_address());
				p_resp_prim->set_port_2(inet_addresses[1].get_port_number());
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
			}
			else {
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: < %s,%s,%d,%d >: CP reply to Address Request Failed!!"),FX_NAME, _serv_name,_serv_domain, cs_systemId,_node_state);
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
			}
			response_primitive = p_resp_prim;
		}
		break;
		default:
		{
			AppProt_addressUNIX_v1_response  *p_resp_prim = new (std::nothrow) AppProt_addressUNIX_v1_response();
			if(!p_resp_prim){
					_process_error_descr = "Memory not available to allocate the primitive object";
					return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
			}
			p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_NODE_NOT_VALID);
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("AppProt_address_request < %s,%s,%d,%d > Failed!! system_type unknown"),_serv_name, _serv_domain, _system_id,_node_state);
			response_primitive = p_resp_prim;
		}
		break;
	}
	//response_primitive = p_resp_prim;
	return returnCode;
}



//****************************************************
//	QueryAddresstoCP
//****************************************************
//query addresses to CP

int AppProt_address_v1_request::queryAddressestoCP(int32_t cp_system_id, ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED]) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="AppProt_address_v1_request::QueryAddressestoCP";
#endif
	int returnCode=0;
	int resultCode;
	ACS_DSD_Client cli;
	ACS_DSD_CpProt_SvcHandler cpProtHandler;

	acs_dsd::NodeStateConstants nStateTmp;

	nStateTmp=(_node_state == 0)? acs_dsd::NODE_STATE_EXECUTION :
				((_node_state ==1 )? acs_dsd::NODE_STATE_STANDBY : acs_dsd::NODE_STATE_UNDEFINED);

	char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};

	ACS_DSD_CpNodesManager::OP_Result op_res = cpNodesManager.find_cp_node_by_systemId_and_state(cp_system_id, nStateTmp, node_name);
	if (op_res != ACS_DSD_CpNodesManager::CPNM_OK){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("%s: unable to find CP node identified by: (systemId = %d, state= %d). call_result = < %d >"), FX_NAME, cp_system_id, nStateTmp, op_res);
		return -1;
	}
	// CP name found, now try to connect to the CP
	resultCode = cli.connect(cpProtHandler.peer(), ACS_DSD_CONFIG_CPAP_INET_SOCKET_SAP_ID, cp_system_id, node_name, DSD_SERVER_CONNECT_TIMEOUT_VALUE);
	if(resultCode < 0){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: error connecting to CP - systemId=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, cp_system_id, node_name, resultCode, cli.last_error_text());
		return -1;
	}
	if (returnCode !=0){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT ("%s: warning connecting to CP - systemId=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME, cp_system_id, node_name,resultCode,cli.last_error_text());
	}
	else
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("%s: established connection with CP - systemId=%d, node_name =%s . CONN_ID = < %u >"), FX_NAME, cp_system_id, node_name, cpProtHandler.getID());

	// prepare the address request (1) primitive to send to the connected CP
	CpProt_address_request addressreq_prim;

	addressreq_prim.set_node_name(node_name);
	addressreq_prim.set_service_domain(reinterpret_cast<const char *>(_serv_domain));
	addressreq_prim.set_service_name(reinterpret_cast<const char *>(_serv_name));

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Sending to CP the following primitive : %s"), FX_NAME, cpProtHandler.getID(), addressreq_prim.to_text().c_str());

	if (!cpProtHandler.sendPrimitive(&addressreq_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. The address request primitive (prim 1) has been sent to CP !"),FX_NAME, cpProtHandler.getID());
	else{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Failed to send address request primitive (prim 1) to CP !"), FX_NAME, cpProtHandler.getID());
		return -1;	// error sending primitive
	}

	ACS_DSD_ServicePrimitive *recv_primitive = 0;

	// read the primitive received from CP and check the result
	resultCode= cpProtHandler.recvPrimitive(recv_primitive, DSD_SERVER_RECV_TIMEOUT_VALUE);

	switch(resultCode)
	{
		case ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_OK:
		{
			uint8_t call_result;
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("%s: CONN_ID = < %u >. Received primitive.  Primitive data : %s"),FX_NAME, cpProtHandler.getID(), recv_primitive->to_text().c_str());
			if((call_result = recv_primitive->get_primitive_id()) != acs_dsd::PCP_CPAP_ADDRESS_REPLY_ID){
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: Address request (prim 1) error: CP node replied with the primitive number %d instead of the primitive number 2"), FX_NAME,call_result);
				delete recv_primitive;
				return -1;
			}
		}
		break;
		case ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_INVALID_PRIMITIVE:
		case ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: CP replied with an invalid or not implemented primitive"),FX_NAME);
			return -1;

		default:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: Address request (prim 1) error: CP node reply error < %u >"),FX_NAME, resultCode);
			return -1;
	}

	// take care of memory deallocation
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(recv_primitive);

	CpProt_address_response *p_CpProt_addrReply = dynamic_cast<CpProt_address_response *>(recv_primitive);
	// process the primitive
	ACS_DSD_ServicePrimitive *reply_prim =0;
	resultCode = p_CpProt_addrReply->process(reply_prim, const_cast<ACS_DSD_CpProt_SvcHandler*>(&cpProtHandler));
	uint8_t validation_result = p_CpProt_addrReply->_process_prim_info.data_validation_result;

	if(!resultCode || validation_result!= ACS_DSD_PRIM_VALIDATION_OK)
	   return (resultCode < 0) ? resultCode : validation_result;
	else{  // CP reply is OK, now get the CP's IP addresses from CS
		ACS_DSD_ConfigurationHelper::HostInfo cp_nodeInfo;
		if((resultCode = ACS_DSD_ConfigurationHelper::get_node_by_node_name(node_name, cp_nodeInfo))!=0){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: get_node_by_node_name (%s) failed! errorcode= %d"),FX_NAME, node_name, resultCode);
			return -1;
		}
		inet_addresses[0].set(htons(p_CpProt_addrReply->get_port_1()),cp_nodeInfo.ip4_addresses[0], 0);
		uint16_t conn_num = p_CpProt_addrReply->get_conn_num();
		if (conn_num >1)
			inet_addresses[1].set(htons(p_CpProt_addrReply->get_port_2()),cp_nodeInfo.ip4_addresses[1], 0);
		else
			inet_addresses[1].set(htons(p_CpProt_addrReply->get_port_1()),cp_nodeInfo.ip4_addresses[1], 0);
	}
	return 0;
}


//****************************************************
//	QueryAddresstoAP
//****************************************************
//query addresses to AP

int AppProt_address_v1_request::queryAddressestoAP(ACS_DSD_ServicePrimitive *& response_primitive, uint8_t remote_node_state) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="AppProt_address_v1_request::queryAddressestoAP";
#endif
	int resultCode;

	ACS_DSD_Client cli;
	ACS_DSD_SrvProt_SvcHandler srvProtHandler;

//	char nodes_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX][acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED]= {""};

	char nodes_name[acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]= {{0}};

	resultCode= ACS_DSD_Utils::get_ApNodesName(_system_id, nodes_name);
	if (resultCode == acs_dsd::ERR_NODE_NOT_FOUND){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: get_ApNodesName():error finding AP node name with systemId=%d,  erroCode = %d"), FX_NAME,_system_id, resultCode);
		return -1;
	}
	for(int i=0; i<acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED; i++){
		if((resultCode = cli.connect(srvProtHandler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, _system_id, nodes_name[i], DSD_SERVER_CONNECT_TIMEOUT_VALUE))){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT ("%s: error connecting to AP=%d, node_name =%s - erroCode = %d <%s>"), FX_NAME,_system_id, nodes_name[i],resultCode,cli.last_error_text());
		}
		else
		{
			// AP name found, and the connection with DSD server is established!!!
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("%s: established connection with AP - AP=%d, node_name =%s . CONN_ID = < %u >"), FX_NAME, _system_id, nodes_name[i], srvProtHandler.getID());
			break;
		}
	}
	if(resultCode){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: error connecting to AP=%d, on both nodes"), FX_NAME,_system_id);
		return -1;
	}
	//send  address request (51) primitive  to DSD server
	SrvProt_address_request addressreq_prim;

	addressreq_prim.set_service_domain((char *)_serv_domain);
	addressreq_prim.set_service_name((char *)_serv_name);
	addressreq_prim.set_system_id(dsd_local_node.system_id);
	addressreq_prim.set_node_state(remote_node_state);

	if (!srvProtHandler.sendPrimitive(&addressreq_prim, DSD_SERVER_SEND_TIMEOUT_VALUE))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. The address request primitive (prim 51) has been sent to DSD !"),FX_NAME, srvProtHandler.getID());
	else{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Failed to send address request primitive (prim 51) to DSD !"), FX_NAME, srvProtHandler.getID());
		return -1;	// error sending primitive
	}
	ACS_DSD_ServicePrimitive *recv_primitive = 0;
	// read the primitive received from DSD and check the result
	resultCode= srvProtHandler.recvPrimitive(recv_primitive,DSD_SERVER_RECV_TIMEOUT_VALUE);
	uint8_t primitive_rcvd;

	switch(resultCode)
	{
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK:
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT ("%s: CONN_ID = < %u >. Received primitive.  Primitive data : %s"),FX_NAME, srvProtHandler.getID(), recv_primitive->to_text().c_str());
			primitive_rcvd = recv_primitive->get_primitive_id();
			if(primitive_rcvd != acs_dsd::PCP_DSDDSD_ADDRESS_INET_REPLY_ID && primitive_rcvd != acs_dsd::PCP_DSDDSD_ADDRESS_UNIX_REPLY_ID){
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: Address request (prim 51) error: DSD server replied with the primitive number %d instead of the primitive number 2"), FX_NAME,primitive_rcvd);
				delete recv_primitive;
				return -1;
			}
		}
		break;
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_INVALID_PRIMITIVE:
		case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: DSD replyed with an invalid or not implemented primitive"),FX_NAME);
			return -1;

		default:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: Address request (prim 51) error:DSD server reply error %u"),FX_NAME, resultCode);
			return -1;
	}

	// take care of memory deallocation
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(recv_primitive);

	if(primitive_rcvd == acs_dsd::PCP_DSDDSD_ADDRESS_INET_REPLY_ID){
		SrvProt_address_inet_response  *p_SrvProt_addrResponse = dynamic_cast<SrvProt_address_inet_response *>(recv_primitive);
		uint8_t response_code = p_SrvProt_addrResponse->get_error_field();

		AppProt_addressINET_v1_response *p_resp_prim = new (std::nothrow) AppProt_addressINET_v1_response();
		if(!p_resp_prim){
			_process_error_descr = "Memory not available to allocate the primitive object";
			return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
		}
		if(response_code == acs_dsd::PCP_ERROR_CODE_SUCCESSFUL){
			p_resp_prim->set_ip_address_1(p_SrvProt_addrResponse->get_ip_address_1());
			p_resp_prim->set_ip_address_2(p_SrvProt_addrResponse->get_ip_address_2());
			p_resp_prim->set_port_1(p_SrvProt_addrResponse->get_port_1());
			p_resp_prim->set_port_2(p_SrvProt_addrResponse->get_port_2());
			response_primitive = p_resp_prim;
		}
		else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: < %s,%s,%d>: DSD replied with error = %d!!"),FX_NAME, _serv_name,_serv_domain,_system_id,response_code);
			p_resp_prim->set_error_field(response_code);
			response_primitive = p_resp_prim;
		}
	}
	else {
		SrvProt_address_unix_response  *p_SrvProt_addrResponse = dynamic_cast<SrvProt_address_unix_response *>(recv_primitive);
		uint8_t response_code = p_SrvProt_addrResponse->get_error_field();

		AppProt_addressUNIX_v1_response *p_resp_prim = new (std::nothrow) AppProt_addressUNIX_v1_response();
		if(response_code == acs_dsd::PCP_ERROR_CODE_SUCCESSFUL){  // DSD reply is OK
			char unix_path1[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX]={0};
			char unix_path2[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX]={0};

			p_SrvProt_addrResponse->get_unix_sock_path1(unix_path1, ACS_DSD_ARRAY_SIZE(unix_path1));
			p_resp_prim->set_unix_sock_path1(unix_path1);
			p_SrvProt_addrResponse->get_unix_sock_path2(unix_path2, ACS_DSD_ARRAY_SIZE(unix_path2));
			p_resp_prim->set_unix_sock_path2(unix_path2);
			response_primitive = p_resp_prim;
		}
		else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s: < %s,%s,%d>: DSD replied with error = %d!!"),FX_NAME, _serv_name,_serv_domain,_system_id,response_code);
			p_resp_prim->set_error_field(response_code);
			response_primitive = p_resp_prim;
		}
	}
	return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
}


/**********************************************************************************************************************/


/* AppProt_addressINET_v1_response methods */

AppProt_addressINET_v1_response::AppProt_addressINET_v1_response()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_ADDR_INET_RESPONSE;
	_error = 0;
	_ip1 = 0;
	_ip2 = 0;
	_port1 = 0;
	_port2 = 0;
}

std::string AppProt_addressINET_v1_response::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u,"
				"IP1 = %u,"
				"PORT1 = %u,"
				"IP2 = %u,"
				"PORT2 = %u",
				_primitive_id,
				_protocol_version,
				_error,
				_ip1,
				_port1,
				_ip2,
				_port2);

	return mess;
}


int AppProt_addressINET_v1_response::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_ADDR_INET_RESPONSE, SVRAPP_PROTOCOL_VERSION_1, _error, _ip1, _port1, _ip2, _port2);
}

int AppProt_addressINET_v1_response::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, &_ip1, &_port1, &_ip2, &_port2);
}

int AppProt_addressINET_v1_response::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}


/**********************************************************************************************************************/


/* AppProt_addressUNIX_v1_response methods */

AppProt_addressUNIX_v1_response::AppProt_addressUNIX_v1_response()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_ADDR_UNIX_RESPONSE;
	_error = 0;
	memset(_unix_sock_path1,'\0',sizeof(_unix_sock_path1));
	memset(_unix_sock_path2,'\0',sizeof(_unix_sock_path2));
}

std::string AppProt_addressUNIX_v1_response::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"ERROR = %u,"
				"UNIX_SOCKET_NAME1 = %s,"
				"UNIX_SOCKET_NAME2 = %s",
				_primitive_id,
				_protocol_version,
				_error,
				_unix_sock_path1,
				_unix_sock_path2);

	return mess;
}

int AppProt_addressUNIX_v1_response::get_unix_sock_path1(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path2, buff, buffsize);
}

int AppProt_addressUNIX_v1_response::set_unix_sock_path1(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path2, str, strlen(str));
}

int AppProt_addressUNIX_v1_response::get_unix_sock_path2(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path2, buff, buffsize);
}

int AppProt_addressUNIX_v1_response::set_unix_sock_path2(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path2, str, strlen(str));
}

int AppProt_addressUNIX_v1_response::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_ADDR_UNIX_RESPONSE, SVRAPP_PROTOCOL_VERSION_1,  _error, _unix_sock_path1, _unix_sock_path2);
}

int AppProt_addressUNIX_v1_response::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, &_unix_sock_path1, &_unix_sock_path2);
}

int AppProt_addressUNIX_v1_response::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}

