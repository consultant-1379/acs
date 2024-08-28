#include <memory>

#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_SrvProt_ListPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


/* Srv_Prot_list_v1_request methods */
extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACE_Recursive_Thread_Mutex imm_conn_mutex;
extern ACS_DSD_Logger dsd_logger;

SrvProt_list_v1_request::SrvProt_list_v1_request()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_LIST_REQUEST;
	_system_id = 0;
	memset(_node_name,'\0',sizeof(_node_name));
}


std::string SrvProt_list_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"SYSTEM_ID = %u,"
				"NODE_NAME = %s,",
				_primitive_id,
				_protocol_version,
				_system_id,
				_node_name);

	return mess;
}


int SrvProt_list_v1_request::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_node_name, buff, buffsize);
}


int SrvProt_list_v1_request::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}


int SrvProt_list_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_LIST_REQUEST, SRVSRV_PROT_V1, _system_id, _node_name);
}


int SrvProt_list_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_system_id, _node_name);
}


int SrvProt_list_v1_request::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * service_handler) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_list_v1_request::process";
#endif

	int returnCode = ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT;
	int number_of_nodes = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s: - systemId = %d, nodeName = %s"), FX_NAME, _system_id, _node_name);
	char nodeNames[acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED][acs_dsd::PCP_FIELD_SIZE_NODE_NAME] = {{0}};

	// Fetch info regarding the local node
	ACS_DSD_Node local_node_info;
	std::string error_descr;
	int op_res = ACS_DSD_Utils::get_node_info(local_node_info, error_descr);
	if( op_res < 0 )
	{
		_process_error_descr = " ACS_DSD_Utils::get_node_info()failed!";
		ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Utils::get_node_info() failure ! return code is < %d > - description is < %s > !"), op_res, error_descr.c_str());
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	// If the _system_id field has one of these values, fetch the local node name
	if( (_system_id == acs_dsd::SYSTEM_ID_THIS_NODE) || (_system_id == acs_dsd::SYSTEM_ID_ALL_AP_NODES) )
	{
		number_of_nodes++;
		strncpy(nodeNames[number_of_nodes - 1], local_node_info.node_name, acs_dsd::PCP_FIELD_SIZE_NODE_NAME);
	}

	// Else if the _system_id field has one of these other values, fetch the partner node name
	if( (_system_id == acs_dsd::SYSTEM_ID_PARTNER_NODE) || (_system_id == acs_dsd::SYSTEM_ID_ALL_AP_NODES) )
	{
		number_of_nodes++;
		ACS_DSD_Node partner_node_info;
		op_res = ACS_DSD_Utils::get_partner_node_info(partner_node_info, error_descr);
		if( op_res < 0 )
		{
			_process_error_descr = " ACS_DSD_Utils::get_partner_node_info() failed!";
			ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Utils::get_partner_node_info() failure ! return code is < %d > - description is < %s > !"), op_res, error_descr.c_str());
			return ACS_DSD_PRIM_PROCESS_ERROR;
		}
		strncpy(nodeNames[number_of_nodes - 1], partner_node_info.node_name, acs_dsd::PCP_FIELD_SIZE_NODE_NAME);
	}

	// Else fetch the node name from the primitive field
	if( _system_id == acs_dsd::SYSTEM_ID_UNKNOWN )
	{
		number_of_nodes = 1;
		strncpy(nodeNames[0], reinterpret_cast<const char *> (_node_name), acs_dsd::PCP_FIELD_SIZE_NODE_NAME);
	}

	// After fetching the node(s) name(s), fetch the info regarding the registered services on each of it, and forward the information to the caller DSD server
	for( int idx = 0; idx < number_of_nodes; idx++ )
	{
		if( !strcmp(nodeNames[idx], local_node_info.node_name) )
			returnCode = getServicesOnLocalNode(service_handler, nodeNames[idx]);
		else
			returnCode = getServicesOnPartnerNode(service_handler, nodeNames[idx]);
	}

	return returnCode;
}


int SrvProt_list_v1_request::getServicesOnLocalNode(ACS_DSD_ServiceHandler * service_handler, char * nodeName) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_list_v1_request::getServicesOnLocalNode";
#endif

	// Fetch from IMM the list of registered services on the given node
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immDHobj(immConnHandlerObj_ptr);
	std::vector<std::string> reg_services_rdns;
	int op_res = immDHobj.fetchRegisteredServicesList(nodeName, reg_services_rdns);
	if( op_res != acs_dsd_imm::NO_ERROR )
	{
		_process_error_descr = "ACS_DSD_ImmDataHandler::fetchRegisteredServicesList() failed!";
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: IMM error getting services for AP Node < %s >. Error code = < %d >"), FX_NAME, nodeName, op_res);
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}
	guard.release();

	// Send a response for each registered service found on the given node
	for(std::vector<std::string>::iterator it = reg_services_rdns.begin(); it != reg_services_rdns.end(); ++it)
	{
		// Parse the current rdn to get service name and service domain
		std::string servName = it->substr(it->find_first_of("=")+1, it->find_first_of("@") -1 - it->find_first_of("="));
		std::string servDomain = it->substr(it->find_first_of("@")+1, it->find_first_of(",") -1 - it->find_first_of("@"));

		// Fetch from IMM the info regarding the current service
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_2(imm_conn_mutex);
		const int check_running = 1;
		op_res = immDHobj.fetch_ServiceInfo(nodeName, servName.c_str(), servDomain.c_str(), check_running);
		if( op_res != acs_dsd_imm::NO_ERROR )
		{
			_process_error_descr = "ACS_DSD_ImmDataHandler::fetch_ServiceInfo() failed!";
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Error while fetching the SRVInfo object from IMM for service < %s@%s > on node < %s >!"),FX_NAME, servName.c_str(), servDomain.c_str(), nodeName);
			return ACS_DSD_PRIM_PROCESS_ERROR;
		}

		// Get connection type from service info in order to send the correct reply primitive
		uint16_t conn_type;
		immDHobj.get_conn_type(conn_type);

		if( conn_type == acs_dsd::CONNECTION_TYPE_INET )
		{
			SrvProt_list_inet_v1_reply * p_resp_prim = new (std::nothrow) SrvProt_list_inet_v1_reply();
			if( !p_resp_prim )
			{
				_process_error_descr = "Memory not available to allocate the primitive object";
				return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
			}

			// Take care of object deallocation
			std::auto_ptr<SrvProt_list_inet_v1_reply> auto_ptr_resp_prim(p_resp_prim);

			// Set node name, service name and service domain fields
			p_resp_prim->set_node_name(nodeName);
			p_resp_prim->set_service_name(servName.c_str());
			p_resp_prim->set_service_domain(servDomain.c_str());

			// Set process name, PID and visibility fields
			char procName[acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX] = {0};
			immDHobj.get_proc_name(procName);
			p_resp_prim->set_process_name(procName);
			int32_t pid;
			immDHobj.get_pid(pid);
			p_resp_prim->set_pid(pid);
			p_resp_prim->set_visibility(immDHobj.get_visibility());

			// Set IP addresses and ports fields
			uint16_t n_conn;
			ACE_INET_Addr inet_addr[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
			immDHobj.get_inet_addresses(inet_addr, n_conn);
			int valid_inet_addr = 0;
			for( int i = 0; i < n_conn && i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; i++ )
			{
				if( inet_addr[i].get_ip_address() )
				{
					if( !valid_inet_addr )
					{
						p_resp_prim->set_ip_address_1(inet_addr[i].get_ip_address());
						p_resp_prim->set_port_1(inet_addr[i].get_port_number());
					}
					else
					{
						p_resp_prim->set_ip_address_2(inet_addr[i].get_ip_address());
						p_resp_prim->set_port_2(inet_addr[i].get_port_number());
					}
					valid_inet_addr++;
				}
			}

			// Set Unix address field
			char unix_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX] = {{0}};
			immDHobj.get_unix_addresses(unix_addresses, n_conn);
			int valid_unix_addr = 0;
			for( int i = 0; i < n_conn && i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; i++ )
			{
				if( *unix_addresses[i] )
				{
					p_resp_prim->set_unix_path(unix_addresses[i]);
					valid_unix_addr++;
				}
			}

			// Set error field
			if( !valid_inet_addr || !valid_unix_addr )
			{
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_LIST_REPLY_FAILED);
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Error while preparing a list (inet) reply primitive (prim 63) on node < %s >!"),FX_NAME, nodeName);
			}
			else
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);

			// Send the list inet reply previously prepared
			if( !service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) )
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. The list reply (inet) primitive (prim 63) for the service < %s@%s > has been sent by the DSD on node < %s >!"),FX_NAME, service_handler->getID(), servName.c_str(), servDomain.c_str(), nodeName);
			else
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Failed to send list reply (inet) primitive (prim 63) for the service < %s@%s > by the DSD on node < %s >!"),FX_NAME, service_handler->getID(), servName.c_str(), servDomain.c_str(), nodeName);
				//return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
				// TODO: check behavior in this case: (skip this primitive or return an error and stop?)
				//       Probabilmente sara` inutile continuare cone le successive response, quindi si potrebbe
				//       terminare qui l'esecuzione ed uscire subito.
			}
		}
		else if( conn_type == acs_dsd::CONNECTION_TYPE_UNIX )
		{
			SrvProt_list_unix_v1_reply * p_resp_prim = new (std::nothrow) SrvProt_list_unix_v1_reply();
			if( !p_resp_prim )
			{
				_process_error_descr = "Memory not available to allocate the primitive object";
				return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
			}

			// Take care of object deallocation
			std::auto_ptr<SrvProt_list_unix_v1_reply> auto_ptr_resp_prim(p_resp_prim);

			// Set node name, service name and service domain fields
			p_resp_prim->set_node_name(nodeName);
			p_resp_prim->set_service_name(servName.c_str());
			p_resp_prim->set_service_domain(servDomain.c_str());

			// Set process name, PID and visibility fields
			char procName[acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX] = {0};
			immDHobj.get_proc_name(procName);
			p_resp_prim->set_process_name(procName);
			int32_t pid;
			immDHobj.get_pid(pid);
			p_resp_prim->set_pid(pid);
			p_resp_prim->set_visibility(immDHobj.get_visibility());

			// Set Unix addresses fields
			uint16_t n_conn;
			char unix_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX] = {{0}};
			immDHobj.get_unix_addresses(unix_addresses, n_conn);
			int valid_unix_addr = 0;
			for( int i = 0; i < n_conn && i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; i++ )
			{
				if( *unix_addresses[i] )
				{
					if( !valid_unix_addr )
						p_resp_prim->set_unix_sock_path1(unix_addresses[i]);
					else
						p_resp_prim->set_unix_sock_path2(unix_addresses[i]);
					valid_unix_addr++;
				}
			}

			// Set error field
			if( !valid_unix_addr )
			{
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_LIST_REPLY_FAILED);
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Error while preparing a list (unix) reply primitive (prim 64) on node < %s >!"),FX_NAME, nodeName);
			}
			else
				p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);

			// Send the list unix reply previously prepared
			if( !service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) )
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. The list reply (unix) primitive (prim 64) for the service < %s@%s > has been sent by the DSD on node < %s >!"),FX_NAME, service_handler->getID(), servName.c_str(), servDomain.c_str(), nodeName);
			else
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Failed to send list reply (unix) primitive (prim 64) for the service < %s@%s > by the DSD on node < %s >!"),FX_NAME, service_handler->getID(), servName.c_str(), servDomain.c_str(), nodeName);
				//return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
				// TODO: check behavior in this case: (skip this primitive or return an error and stop
				//       CFR il commento precedente.
			}
		}
		// The current rdn is regarding a service with a different connection type, discard it
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. The current service has a connection type different from INET or UNIX, i'll discard it! "),FX_NAME, nodeName);
	}

	// Close the connection after send the last reply
	return ACS_DSD_PRIM_PROCESS_OK_CLOSE_RESPONSE_ALREADY_SENT;
}


int SrvProt_list_v1_request::getServicesOnPartnerNode(ACS_DSD_ServiceHandler * service_handler, char * nodeName) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_list_v1_request::getServicesOnPartnerNode";
#endif

	int call_result;
	ACS_DSD_Client client;
	ACS_DSD_SrvProt_SvcHandler service_handler_2;

	// Connect to the DSD server on the partner node to forward to it the list request
	call_result = client.connect(service_handler_2.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, acs_dsd::SYSTEM_ID_PARTNER_NODE, nodeName, DSD_SERVER_CONNECT_TIMEOUT_VALUE);
	if( call_result != acs_dsd::ERR_NO_ERRORS )
	{
		_process_error_descr = "ACS_DSD_Client::connect() failed!";
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Can't establish connection to the DSD server on AP having node_name <%s>. CONN ID = < %u >. Error code = < %d >"), FX_NAME, nodeName, service_handler_2.getID(), call_result);
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Established connection to the DSD server on AP having node_name <%s>. CONN ID = < %u >"), FX_NAME, nodeName, service_handler_2.getID());

	// Prepare the primitive to forward
	SrvProt_list_v1_request list_primitive;
	list_primitive._system_id = acs_dsd::SYSTEM_ID_THIS_NODE;
	list_primitive.set_node_name("");

	// Forward the list primitive to the DSD server on the partner node
	call_result = service_handler_2.sendPrimitive(&list_primitive, DSD_SERVER_SEND_TIMEOUT_VALUE);
	if (call_result)
	{
		_process_error_descr = "ACS_DSD_SrvProt_SvcHandler::sendPrimitive() failed!";
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Failed to send list request primitive (prim 62) to DSD server on node < %s >! - Error code = < %d > "),FX_NAME, service_handler_2.getID(), nodeName, call_result);
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. The list request primitive (prim 62) has been sent to DSD server on node < %s >!"),FX_NAME, service_handler_2.getID(), nodeName);

	// Receive the replies from the DSD server on the partner node and forward them to the caller DSD server
	while(1)
	{
		ACS_DSD_ServicePrimitive * p_resp_prim = 0;
		call_result = service_handler_2.recvPrimitive(p_resp_prim, DSD_SERVER_RECV_TIMEOUT_VALUE);

		// If a valid primitive has been received from the DSD server on the partner node, forward it to the caller DSD server
		if( call_result == ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK )
		{
			// Take care of object deallocation
			std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_resp_prim(p_resp_prim);

			int call_result_2 = service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE);		// TODO: check what is the correct behavior when this send fails
			if( !call_result_2 )
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. The list reply primitive (prim 63/64) has been correctly forwarded!"),FX_NAME, service_handler->getID());
			else
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Failed to forward the list reply primitive (prim 63/64)! - Error Code = < %d > "),FX_NAME, service_handler->getID(), call_result_2);
		}
		// If the connection has been closed by the DSD server on the partner node, no more primitives will be forwarded to the caller DSD server
		else if( call_result == ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_RECEIVE_PEER_CLOSED )
			break;
		else			// TODO: check the correct behavior in this case (error and exit, or only log?)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("%s:  CONN_ID = < %u >. Failed to receive a list reply primitive (prim 63/64) from the DSD server on node < %s >!"),FX_NAME, service_handler_2.getID(), nodeName);
	}

	return ACS_DSD_PRIM_PROCESS_OK_CLOSE_RESPONSE_ALREADY_SENT;
}


/**********************************************************************************************************************/


/* SrvProt_list_inet_v1_reply methods */

SrvProt_list_inet_v1_reply::SrvProt_list_inet_v1_reply()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_LIST_INET_REPLY;
	_error = 0;
	memset(_node_name,'\0',sizeof(_node_name));
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	memset(_process_name,'\0',sizeof(_process_name));
	_pid = 0;
	_visibility = 0;
	_ip1 = 0;
	_ip2 = 0;
	_port1 = 0;
	_port2 = 0;
	memset(_unix_sock_path,'\0',sizeof(_unix_sock_path));
}


std::string SrvProt_list_inet_v1_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"ERROR = %u,"
			  "NODE_NAME = %s,"
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
				_error,
				_node_name,
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


int SrvProt_list_inet_v1_reply::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_node_name, buff, buffsize);
}


int SrvProt_list_inet_v1_reply::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}


int SrvProt_list_inet_v1_reply::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}


int SrvProt_list_inet_v1_reply::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}


int SrvProt_list_inet_v1_reply::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}


int SrvProt_list_inet_v1_reply::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}


int SrvProt_list_inet_v1_reply::get_process_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_process_name, buff, buffsize);
}


int SrvProt_list_inet_v1_reply::set_process_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_process_name, str, strlen(str));
}


int SrvProt_list_inet_v1_reply::get_unix_path(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path, buff, buffsize);
}


int SrvProt_list_inet_v1_reply::set_unix_path(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path, str, strlen(str));
}


int SrvProt_list_inet_v1_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return pdh.make_primitive(SRVSRV_LIST_INET_REPLY, SRVSRV_PROT_V1, _error, _node_name, _serv_name, _serv_domain, _process_name, _pid, _visibility, _ip1, _port1, _ip2, _port2, _unix_sock_path);
}


int SrvProt_list_inet_v1_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, _node_name, _serv_name, _serv_domain, _process_name, &_pid, &_visibility, &_ip1, &_port1, &_ip2, &_port2, _unix_sock_path);
}


int SrvProt_list_inet_v1_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}


/**********************************************************************************************************************/


/* SrvProt_list_unix_v1_reply methods */

SrvProt_list_unix_v1_reply::SrvProt_list_unix_v1_reply()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_LIST_UNIX_REPLY;
	_error = 0;
	memset(_node_name,'\0',sizeof(_node_name));
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	memset(_process_name,'\0',sizeof(_process_name));
	_pid = 0;
	_visibility = 0;
	memset(_unix_sock_path1,'\0',sizeof(_unix_sock_path1));
	memset(_unix_sock_path2,'\0',sizeof(_unix_sock_path2));
}


std::string SrvProt_list_unix_v1_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"ERROR = %u,"
				"NODE_NAME = %s,"
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"PROCESS_NAME = %s,"
				"PID = %u,"
				"VISIBILITY = %u,"
				"UNIX_SOCKET_NAME1= %s"
				"UNIX_SOCKET_NAME2= %s",
				_primitive_id,
				_protocol_version,
				_error,
				_node_name,
				_serv_name,
				_serv_domain,
				_process_name,
				_pid,
				_visibility,
				_unix_sock_path1,
				_unix_sock_path2);

	return mess;
}


int SrvProt_list_unix_v1_reply::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_node_name, buff, buffsize);
}


int SrvProt_list_unix_v1_reply::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}


int SrvProt_list_unix_v1_reply::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}


int SrvProt_list_unix_v1_reply::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}


int SrvProt_list_unix_v1_reply::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}


int SrvProt_list_unix_v1_reply::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}


int SrvProt_list_unix_v1_reply::get_process_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_process_name, buff, buffsize);
}


int SrvProt_list_unix_v1_reply::set_process_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_process_name, str, strlen(str));
}


int SrvProt_list_unix_v1_reply::get_unix_sock_path1(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path1, buff, buffsize);
}


int SrvProt_list_unix_v1_reply::set_unix_sock_path1(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path1, str, strlen(str));
}


int SrvProt_list_unix_v1_reply::get_unix_sock_path2(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_unix_sock_path2, buff, buffsize);
}


int SrvProt_list_unix_v1_reply::set_unix_sock_path2(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_unix_sock_path2, str, strlen(str));
}


int SrvProt_list_unix_v1_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return pdh.make_primitive(SRVSRV_LIST_UNIX_REPLY, SRVSRV_PROT_V1, _error, _node_name, _serv_name, _serv_domain, _process_name, _pid, _visibility, _unix_sock_path1, _unix_sock_path2);
}


int SrvProt_list_unix_v1_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, _node_name, _serv_name, _serv_domain, _process_name, &_pid, &_visibility, _unix_sock_path1, _unix_sock_path2);
}


int SrvProt_list_unix_v1_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}
