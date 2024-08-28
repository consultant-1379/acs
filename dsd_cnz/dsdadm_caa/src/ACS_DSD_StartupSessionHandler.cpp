#include <memory>
#include <set>

#include "ACS_DSD_Client.h"
#include "ACS_DSD_SrvProt_StartupPrimitives.h"
#include "ACS_DSD_SrvProt_ListPrimitives.h"
#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_StartupSessionHandler.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_ImmConnectionHandler * imm_shared_objects_conn_ptr;
extern ACE_Recursive_Thread_Mutex  imm_shared_objects_conn_mutex;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_StartupSessionHandler

int __CLASS_NAME__::handle_input (ACE_HANDLE /*fd*/) {
	// We are here because the remote DSD server partner is closing
	// the Startup keep-alive connection.
	// We empty the TCP/IP buffer, if needed, and close this session.
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Probably remote DSD server is disconnected. So close this session."),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

	char buffer [2 * 1024]; // 2 Kb buffer

	while (_session.recv(buffer, ACS_DSD_ARRAY_SIZE(buffer)) > 0) ;

	return -1;
}

int __CLASS_NAME__::handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/) {
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Closing and deleting this session."),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

	if (_ssg) _ssg->delete_session(system_id(), 0);

	return 0;
}

int __CLASS_NAME__::connection_open (std::vector<ACS_DSD_ConfigurationHelper::HostInfo> & ap_nodes) {
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Session state == %d - Session error: '%s'"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
			_session.state(), _session.last_error_text());

	if (_session.state() != acs_dsd::SESSION_STATE_DISCONNECTED) {
		// The startup-session-handler object has already a communication session opened.
		return SSH_ERROR_ALREADY_CONNECTED;
	}

	ACS_DSD_SrvProt_SvcHandler * p = this;
	ACS_DSD_SrvProt_SvcHandler temp_handler_to_passive;

	int return_code = SSH_ERROR_NO_ERROR;
	ACS_DSD_Client client;

	// Try to create a new kept-alive connection towards the remote DSD server running onto the active node
	size_t ap_host_index = 0;

	while (ap_host_index < ap_nodes.size()) {
		ACS_DSD_ConfigurationHelper::HostInfo & ap_host = ap_nodes[ap_host_index];
		return_code = SSH_ERROR_NO_ERROR;

		ACS_DSD_Session & session = p->peer(); // Session used by current iteration
		_remote_node_names.push_back(std::string(ap_host.node_name));

		// Before connecting to the remote DSD server, update SRTNode info in IMM, setting the node state to UNDEFINED
		int call_result = update_IMM_node_info(ap_host.node_name, ap_host.system_name,ap_host.system_id, ap_host.side, acs_dsd::NODE_STATE_UNDEFINED);
		if ( call_result ) { //ERROR: Creating/updating SRTNode object in IMM
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Error while setting state to UNDEFINED for remote AP node '%s': call_result == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ap_host.node_name, call_result);
			return_code = call_result;
			ap_host_index++; // Try next host if any.
			continue;
		}

		// Connect to the DSD server on the remote AP cluster
		call_result = client.connect(session, ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID,
				ap_host.system_id, ap_host.node_name, DSD_SERVER_CONNECT_TIMEOUT_VALUE);
		if (call_result != acs_dsd::ERR_NO_ERRORS) {
			// Connection failed, go to the next loop iteration
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Can't establish connection to the DSD server on AP "
							"having node_name <%s>. Error code = < %d >: Error text '%s'"),
							ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ap_host.node_name, call_result, client.last_error_text());
			return_code = SSH_ERROR_CONNECT_FAILED;
			ap_host_index++; // Try next host if any.
			continue;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: Established connection to the DSD server on AP having node_name <%s>."
				" Session state == %d: Session error = '%s'"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ap_host.node_name,
				session.state(), session.last_error_text());

		// Look for the local node info
		ACS_DSD_Node my_node;
		std::string msg;

		if (ACS_DSD_Utils::get_node_info(my_node, msg)) {
			// ERROR: Getting my node info and state
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT ("%s::%s: Call 'get_node_info' failed: error_text == %s"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, msg.c_str());
			session.close();
			return SSH_ERROR_GET_NODE_INFO_FAILED;
		}

		// Prepare a startup request to the remote DSD server to know its state
		SrvProt_startup_v1_request req_prim;
		req_prim.set_node_name(my_node.node_name);
		req_prim.set_node_state(my_node.node_state);
		req_prim.set_system_id(my_node.system_id);

		// Send the primitive
		call_result = p->sendPrimitive(&req_prim, DSD_SERVER_SEND_TIMEOUT_VALUE);
		if (call_result != 0) {
			// Failed to send the primitive to the remote DSD server, go to the next loop iteration
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Failed to send the startup request primitive to the DSD "
							"server on AP having node_name <%s>. Error code = < %d >"),
							ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ap_host.node_name, call_result);
			session.close();
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Session state == %d: Session error: '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, session.state(), session.last_error_text());

			return_code = SSH_ERROR_SEND_PRIMITIVE;
			ap_host_index++; // Try next host if any.
			continue;
		}

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: Startup request primitive sent successfully to the DSD server on AP "
						"having node_name <%s>"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ap_host.node_name);

		// Receive the primitive from the remote DSD server
		ACS_DSD_ServicePrimitive * p_resp_prim = 0;

		call_result = p->recvPrimitive(p_resp_prim, DSD_SERVER_RECV_TIMEOUT_VALUE);
		if (call_result != 0) { // ERROR: receiving the response primitive from remote DSD server
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Unable to receive the primitive!!! - Error code = < %d >"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
			session.close();
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Session state == %d: Session error: '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, session.state(), session.last_error_text());

			return_code = SSH_ERROR_RECV_PRIMITIVE;
			ap_host_index++; // Try next host if any.
			continue;
		}

		// Take care of memory deallocation
		std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_resp_prim(p_resp_prim);

		SrvProt_startup_v1_reply * p_startup_reply = dynamic_cast<SrvProt_startup_v1_reply *>(p_resp_prim);

		if (!p_startup_reply) { // ERROR: Failed cast
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: dynamic_cast<SrvProt_startup_v1_reply *> failed"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
			session.close();
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Session state == %d: Session error: '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
					session.state(), session.last_error_text());

			return_code = SSH_ERROR_CAST_FAILED;
			ap_host_index++; // Try next host if any.
			continue;
		}

		if (p_startup_reply->get_error_field() != 0) { // ERROR: in received response primitive
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Received the primitive < %d > with the error code < %d >. "),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, p_startup_reply->get_primitive_id(),
					p_startup_reply->get_error_field());
			session.close();
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Session state == %d: Session error: '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, session.state(), session.last_error_text());

			return_code = SSH_ERROR_PRIMITIVE_WITH_ERROR;
			ap_host_index++; // Try next host if any.
			continue;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Received correctly the primitive < %d >"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, p_startup_reply->get_primitive_id());

		// Update the SRTNode info into IMM
		uint8_t state = p_startup_reply->get_node_state();
		call_result = update_IMM_node_info(ap_host.node_name, ap_host.system_name,ap_host.system_id, ap_host.side, state);
		if (call_result != SSH_ERROR_NO_ERROR) { // ERROR: updating IMM SRTNode info
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Error while creating/updating IMM objects for remote AP node '%s': "
							"call_result == %d"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ap_host.node_name,
							call_result);
			session.close();
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Session state == %d: Session error: '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, session.state(), session.last_error_text());

			return call_result;
		}

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: IMM objects correctly created/updated for remote AP node '%s'."),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ap_host.node_name);

		if (state == acs_dsd::NODE_STATE_ACTIVE) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: The remote node is ACTIVE."), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
			if (p == this) {
				node_name_active(ap_host.node_name);
				p = &temp_handler_to_passive; // Use the temporary service session handler into next step

				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
						ACS_DSD_TEXT("%s::%s: Keep-alived session correctly established: Session state == %d: "
						"Session error: '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
						session.state(), session.last_error_text());
			} else { // I'm using the temporary session service handler
				session.close(); // so, the session will be closed
				p = this;
//				node_name_passive(ap_host.node_name);

				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN,
						ACS_DSD_TEXT("%s::%s: We are connected using the temporary service session handler: "
						"this is strange: probably remote cluster is switching resources and nodes are switching "
						"states ACTIVE/PASSIVE and viceversa: don't worry about, DSD server will recover at next timer tick"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
			}

			ap_host_index++; // Try next host if any to get information about passive remote node.
			continue;
		} else { // Here we are connected towards a remote AP not ACTIVE, i.e. PASSIVE or UNDEFINED
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: The remote node is NOT ACTIVE"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

			session.close();
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Session state == %d: Session error: '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, session.state(), session.last_error_text());

			if (p != this) p = this;

//			node_name_passive(ap_host.node_name);

			ap_host_index++; // Try next host if any to get information about passive remote node.
			continue;
		}
	}

	if (_session.state() != acs_dsd::SESSION_STATE_DISCONNECTED) {
		// Session connected, so ask for the list of the registered services
		return_code = update_IMM_services_info();
		if ( return_code )
		{
			// If some an error happened during the IMM updating phase, close the session
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Call 'update_IMM_services_info' failed. return_code == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, return_code);
			_session.close();
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Successfully created/updated SRVInfo objects into IMM."),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
			return_code = SSH_ERROR_NO_ERROR;
		}
	}
	else
	{
		node_name_active(0);
		_remote_node_names.clear();
//		node_name_passive(0);

		if (return_code == SSH_ERROR_NO_ERROR) return_code = SSH_ERROR_NOT_CONNECTED;
	}

	return return_code;
}

int __CLASS_NAME__::connection_close () {
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Session state == %d - Session error: '%s'"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
			_session.state(), _session.last_error_text());

	if (_session.state() != acs_dsd::SESSION_STATE_DISCONNECTED) {
		const int call_result = _session.close();

		if (call_result != 0) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Error while closing the session: call_result == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
			return SSH_ERROR_CONNECTION_CLOSE_FAILED;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: Session state == %d - Session error: '%s'"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
				_session.state(), _session.last_error_text());

		node_name_active(0);
		_remote_node_names.clear();
//		node_name_passive(0);

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Session correctly closed."
				" Session State == %d - Session error = %s"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
				_session.state(), _session.last_error_text());
	}

	return SSH_ERROR_NO_ERROR;
}


int __CLASS_NAME__::update_IMM_node_info(const char* node_name, const char* system_name, int32_t system_id, acs_dsd::NodeSideConstants node_side, uint8_t node_state) {

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_shared_objects_conn_mutex);
	ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);

	// Try to update the SRTNode object related to the given node if present
	if ( !immDHobj.modifyNodeInfo(node_name, node_state) )
	{
		// IMM SRTNode object correctly updated
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: The existing SRTNode for AP Node < %s > has been successfully UPDATED! - new_state == %u"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name, node_state);
		return SSH_ERROR_NO_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: The SRTNode object for the AP node "
			"< %s > is NOT present, i'll create it!"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name);

	// Create a new SRTNode object with the given info
	immDHobj.set_node(system_name);
	immDHobj.set_state(node_state);
	immDHobj.set_side(node_side);

	const int call_result = immDHobj.addNodeInfo(node_name);
	if ( call_result != 0 )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Error while creating "
				"the SRTNode object for the AP node < %s > having system_id == %d and state == %u."
				" call_result == %d - Error = '%s'"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name, system_id, node_state,
				call_result, immDHobj.last_error_text());
		return SSH_ERROR_IMM_CREATE_FAILED;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Correctly created the SRTNode "
			"object for the AP node < %s > having system_id == %d and state == %u"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name, system_id, node_state);

	return SSH_ERROR_NO_ERROR;
}


int __CLASS_NAME__::update_IMM_services_info()
{
	int return_code = SSH_ERROR_NO_ERROR;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_shared_objects_conn_mutex);
	ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);

	// Fetch from IMM the list of registered services on both remote nodes
	std::set<std::string> registered_services;
	std::vector<std::string> services_dn;
	int call_result = 0;
	for ( std::vector<std::string>::iterator node_it = _remote_node_names.begin(); node_it != _remote_node_names.end(); node_it++ )
	{
		call_result = immDHobj.fetchRegisteredServicesList(node_it->c_str(), services_dn);
		if ( call_result != 0 )
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Error while retrieving the list of registered services "
					"for the AP node < %s >. Error code = < %d >, Error = '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_it->c_str(),
					call_result, immDHobj.last_error_text());
			return SSH_ERROR_GET_SERVICES_INFO_FAILED;
		}

		for ( std::vector<std::string>::iterator it = services_dn.begin(); it != services_dn.end(); it++ )
			registered_services.insert(*it);

		services_dn.clear();
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Found < %d > services registered on the remote AP cluster!"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, registered_services.size());

	ACS_DSD_Client client;
	ACS_DSD_SrvProt_SvcHandler list_service_handler;

	// Connect to the remote DSD server to ask for the list of registered services
	call_result = client.connect(list_service_handler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID,
			_system_id, node_name_active(), DSD_SERVER_CONNECT_TIMEOUT_VALUE);
	if ( call_result != acs_dsd::ERR_NO_ERRORS )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: Can't establish connection to the DSD server on AP "
				"having node_name <%s>. Error code = < %d >, Error = '%s'"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name_active(), call_result, client.last_error_text());
		return SSH_ERROR_CONNECT_FAILED;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Connection established with the DSD server on AP "
			"having node_name <%s>"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name_active());

	// Prepare and send the list request primitive
	SrvProt_list_v1_request req_prim;
	req_prim.set_system_id(acs_dsd::SYSTEM_ID_ALL_AP_NODES);

	call_result = list_service_handler.sendPrimitive(&req_prim, DSD_SERVER_SEND_TIMEOUT_VALUE);
	if ( call_result != 0 )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: Failed to send the list request primitive to the DSD "
				"server on AP having node_name <%s>. Error code = < %d >"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name_active(), call_result);
		return SSH_ERROR_SEND_PRIMITIVE;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: List request primitive sent to the DSD server on AP "
			"having node_name <%s>"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name_active());

	// Got the DSD IMM root to be used to search if a service is already present into the local IMM
	const char * imm_dsd_root_dn = 0;
	call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn);
	if ( call_result != 0 )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: Call 'get_dsd_imm_root_dn' failed! Error code = %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
		return SSH_ERROR_GET_DSD_IMM_ROOT_FAILED;
	}

	// For each received primitive, create/update the related SRVInfo IMM object
	int more_to_receive = 1;
	do
	{
		ACS_DSD_ServicePrimitive * p_resp_prim = 0;
		call_result = list_service_handler.recvPrimitive(p_resp_prim, DSD_SERVER_RECV_TIMEOUT_VALUE);

		switch ( call_result )
		{
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_RECEIVE_PEER_CLOSED:
				more_to_receive = 0;
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
						ACS_DSD_TEXT("%s::%s: No more primitives to receive from the DSD "
						"server on AP having node_name <%s>"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, node_name_active());
				break;

			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK:
				if ( p_resp_prim->get_primitive_id() == SRVSRV_LIST_INET_REPLY )
				{
					SrvProt_list_inet_v1_reply * p_list_inet_reply = dynamic_cast<SrvProt_list_inet_v1_reply *> (p_resp_prim);
					if ( ! p_list_inet_reply )
					{
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
								ACS_DSD_TEXT("%s::%s: dynamic_cast<SrvProt_list_inet_v1_reply *> failed"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
						return SSH_ERROR_CAST_FAILED;
					}

					// Take care of memory deallocation
					std::auto_ptr<SrvProt_list_inet_v1_reply> auto_ptr_inet_reply(p_list_inet_reply);

					if( p_list_inet_reply->get_error_field() != 0 )
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
								ACS_DSD_TEXT("%s::%s: Received the primitive < %d > with the error code < %d >. "),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
								p_list_inet_reply->get_primitive_id(), p_list_inet_reply->get_error_field());
					else
					{
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
								ACS_DSD_TEXT("%s::%s: Correctly received the primitive < %d >"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
								p_list_inet_reply->get_primitive_id());

						// Set the SRVInfo object fields
						char serv_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX] = {0};
						char serv_domain[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX] = {0};
						char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};
						char process_name[acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX] = {0};

						p_list_inet_reply->get_service_name(serv_name, sizeof(serv_name));
						p_list_inet_reply->get_service_domain(serv_domain, sizeof(serv_domain));
						p_list_inet_reply->get_node_name(node_name, sizeof(node_name));
						p_list_inet_reply->get_process_name(process_name, sizeof(process_name));

						immDHobj.set_conn_type(acs_dsd::CONNECTION_TYPE_INET);
						immDHobj.set_proc_name(process_name);
						immDHobj.set_pid(p_list_inet_reply->get_pid());
						immDHobj.set_visibility(p_list_inet_reply->get_visibility());

						const char *inet_addresses[2];
						ACE_INET_Addr ip1_addr;
						ACE_INET_Addr ip2_addr;
						ip1_addr.set(p_list_inet_reply->get_port_1(), p_list_inet_reply->get_ip_address_1());
						ip2_addr.set(p_list_inet_reply->get_port_2(), p_list_inet_reply->get_ip_address_2());
						char addr_1_str[32] = {0};
						char addr_2_str[32] = {0};
						ip1_addr.addr_to_string(addr_1_str, 32);
						ip2_addr.addr_to_string(addr_2_str, 32);
						inet_addresses[0] = addr_1_str;
						inet_addresses[1] = addr_2_str;
						immDHobj.set_tcp_addresses(inet_addresses, 2);

						char unix_addr[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX] = {0};
						p_list_inet_reply->get_unix_path(unix_addr, sizeof(unix_addr));
						const char *unix_addresses[1];
						unix_addresses[0] = (const char*) unix_addr;
						immDHobj.set_unix_addresses(unix_addresses, 1);

						// Look for the received service into the list of registered services
						char srv_dn[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
						snprintf(srv_dn, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
								ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
								serv_name, serv_domain, node_name, imm_dsd_root_dn);

						std::set<std::string>::iterator item = registered_services.find(std::string (srv_dn));

						if ( item == registered_services.end() )
						{
							// Item not present, insert it
							call_result = immDHobj.addServiceInfo(node_name, serv_name, serv_domain);
							if ( call_result != 0 )
							{
								ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
										ACS_DSD_TEXT("%s::%s: Failed to create the SRVInfo object for the service <%s:%s>. "
										"call_result == %d, Error = '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
										serv_name, serv_domain,	call_result, immDHobj.last_error_text());
								return SSH_ERROR_IMM_CREATE_FAILED;
							}
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
									ACS_DSD_TEXT("%s::%s: SRVInfo object for the service <%s:%s> correctly created."),
									ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, serv_name, serv_domain);
						}
						else
						{
							// Item already present, update it
							call_result = immDHobj.modifyServiceInfo(node_name, serv_name, serv_domain);
							if ( call_result != 0 )
							{
								ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
										ACS_DSD_TEXT("%s::%s: Failed to update the SRVInfo object for the service <%s:%s>. "
										"call_result == %d, Error = '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
										serv_name, serv_domain,	call_result, immDHobj.last_error_text());
								return SSH_ERROR_IMM_UPDATE_FAILED;
							}
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
									ACS_DSD_TEXT("%s::%s: SRVInfo object for the service <%s:%s> correctly updated."),
									ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, serv_name, serv_domain);

							// Remove the object from the list of registered services previously fetched
							registered_services.erase(item);
						}
					}
				}
				else if( p_resp_prim->get_primitive_id() == SRVSRV_LIST_UNIX_REPLY )
				{
					SrvProt_list_unix_v1_reply * p_list_unix_reply = dynamic_cast<SrvProt_list_unix_v1_reply *> (p_resp_prim);
					if ( ! p_list_unix_reply )
					{
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
								ACS_DSD_TEXT("%s::%s: dynamic_cast<SrvProt_list_unix_v1_reply *> failed"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
						return SSH_ERROR_CAST_FAILED;
					}

					// Take care of memory deallocation
					std::auto_ptr<SrvProt_list_unix_v1_reply> auto_ptr_unix_reply(p_list_unix_reply);

					if ( p_list_unix_reply->get_error_field() != 0 )
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
								ACS_DSD_TEXT("%s::%s: Received the primitive < %d > with the error code < %d >."),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
								p_list_unix_reply->get_primitive_id(), p_list_unix_reply->get_error_field());
					else
					{
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
								ACS_DSD_TEXT("%s::%s: Correctly received the primitive < %d >"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
								p_list_unix_reply->get_primitive_id());

						char serv_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX] = {0};
						char serv_domain[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX] = {0};
						char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};
						char process_name[acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX] = {0};

						p_list_unix_reply->get_service_name(serv_name, sizeof(serv_name));
						p_list_unix_reply->get_service_domain(serv_domain, sizeof(serv_domain));
						p_list_unix_reply->get_node_name(node_name, sizeof(node_name));
						p_list_unix_reply->get_process_name(process_name, sizeof(process_name));
						immDHobj.set_conn_type(acs_dsd::CONNECTION_TYPE_UNIX);
						immDHobj.set_proc_name(process_name);
						immDHobj.set_pid(p_list_unix_reply->get_pid());
						immDHobj.set_visibility(p_list_unix_reply->get_visibility());

						char unix_addr[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX] = {0};
						p_list_unix_reply->get_unix_sock_path1(unix_addr, sizeof(unix_addr));
						const char *unix_addresses[1];
						unix_addresses[0] = (const char*) unix_addr;
						immDHobj.set_unix_addresses(unix_addresses, 1);

						// Look for the received service into the list of registered services
						char srv_dn[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
						snprintf(srv_dn, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
								ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
								serv_name, serv_domain, node_name, imm_dsd_root_dn);

						std::set<std::string>::iterator item = registered_services.find(std::string(srv_dn));
						if ( item == registered_services.end() )
						{
							// Item not present, insert it
							call_result = immDHobj.addServiceInfo(node_name, serv_name, serv_domain);
							if ( call_result != 0 )
							{
								ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
										ACS_DSD_TEXT("%s::%s: Failed to create the SRVInfo object for the service <%s:%s>. "
										"call_result == %d, Error = '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
										serv_name, serv_domain,	call_result, immDHobj.last_error_text());
								return SSH_ERROR_IMM_CREATE_FAILED;
							}
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
									ACS_DSD_TEXT("%s::%s: SRVInfo object for the service <%s:%s> correctly created."),
									ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, serv_name, serv_domain);
						}
						else
						{
							// Item already present, update it
							call_result = immDHobj.modifyServiceInfo(node_name, serv_name, serv_domain);
							if ( call_result != 0 )
							{
								ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
										ACS_DSD_TEXT("%s::%s: Failed to update the SRVInfo object for the service <%s:%s>. "
										"call_result == %d, Error = '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
										serv_name, serv_domain,	call_result, immDHobj.last_error_text());
								return SSH_ERROR_IMM_UPDATE_FAILED;
							}
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
									ACS_DSD_TEXT("%s::%s: SRVInfo object for the service <%s:%s> correctly updated."),
									ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, serv_name, serv_domain);

							// Remove the object from the list of registered services previously fetched
							registered_services.erase(item);
						}
					}
				}
				else
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
							ACS_DSD_TEXT("%s::%s: Received wrong primitive (ID = < %d >)"),
							ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, p_resp_prim->get_primitive_id());
				break;
			default:
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
						ACS_DSD_TEXT("%s::%s: Error while receiving the list reply primitive. Error code = < %d >"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
				more_to_receive = 0;
				return_code = SSH_ERROR_UPDATE_SERVICE_LIST_FAILED;
				break;
		}
	}
	while (more_to_receive);

	// After updating the list of the services registered on the remote
	// AP node, delete the services still present into the list
	for ( std::set<std::string>::iterator it = registered_services.begin(); it != registered_services.end(); it++ )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: A list reply primitive for the service <%s> hasn't been received,"
				"delete the related SRVInfo object."), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, it->c_str());

		call_result = immDHobj.deleteServiceInfo(it->c_str());
		if ( call_result != 0 )
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Error while deleting the object <%s>. call_result == %d - error = '%s'"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, it->c_str(), call_result, immDHobj.last_error_text());
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: Object <%s> successfully deleted."),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, it->c_str());
	}

	return return_code;
}

int __CLASS_NAME__::update_remote_nodes_state () {

	for ( std::vector<std::string>::iterator it = _remote_node_names.begin(); it != _remote_node_names.end(); it++ ) {
		const char * remote_node = it->c_str();

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_shared_objects_conn_mutex);
		ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);

		int call_result = immDHobj.fetch_NodeInfo(remote_node);
		if ( call_result ) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Failed to retrieve the SRTNode object for the node <%s> - "
					"call_result == %d - err_descr: '%s'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
					remote_node, call_result, immDHobj.last_error_text());

			return SSH_ERROR_GET_NODE_INFO_FAILED;
		}

		uint16_t remote_node_state;
		immDHobj.get_state(remote_node_state);

		if ( remote_node_state != acs_dsd::NODE_STATE_UNDEFINED ) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: The node %s has NOT undefined state, nothing to do! - node_state == %u"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, remote_node, remote_node_state);
			continue;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: The node %s has undefined state, asking for its state."),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, remote_node);

		ACS_DSD_Node my_ap_node;
		std::string msg;

		if ( ACS_DSD_Utils::get_node_info(my_ap_node, msg) ) { // ERROR: Getting my node info and state
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT ("%s::%s: Call 'get_node_info' failed: error_text == %s"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, msg.c_str());
			return SSH_ERROR_GET_NODE_INFO_FAILED;
		}

		ACS_DSD_Client client;
		ACS_DSD_SrvProt_SvcHandler svc_hdl;

		// Connect to the remote DSD server
		if ( client.connect(svc_hdl.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, _system_id, remote_node, DSD_SERVER_CONNECT_TIMEOUT_VALUE) ) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT ("%s::%s: Call 'client.connect' failed: call_result == %d - error_text == %s"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, client.last_error(), client.last_error_text());
			return SSH_ERROR_CONNECT_FAILED;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT ("%s::%s: Connected to the DSD server on remote node %s"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, remote_node);

		// Prepare a startup request in order to update the remote node state into IMM
		SrvProt_startup_v1_request req_prim;
		req_prim.set_node_name(my_ap_node.node_name);
		req_prim.set_node_state(my_ap_node.node_state);
		req_prim.set_system_id(my_ap_node.system_id);

		call_result = svc_hdl.sendPrimitive(&req_prim, DSD_SERVER_SEND_TIMEOUT_VALUE);
		if ( call_result ) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT ("%s::%s: Call 'svc_hdl.sendPrimitive' failed: call_result == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
			return SSH_ERROR_SEND_PRIMITIVE;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT ("%s::%s: Startup primitive sent to the DSD server on remote node %s"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, remote_node);

		// Receive the startup reply primitive from the remote DSD server
		ACS_DSD_ServicePrimitive * p_resp_prim = 0;

		call_result = svc_hdl.recvPrimitive(p_resp_prim, DSD_SERVER_RECV_TIMEOUT_VALUE);
		if ( call_result ) { // ERROR: receiving the response primitive from remote DSD server
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Unable to receive the primitive!!! - Error code = < %d >"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
			return SSH_ERROR_RECV_PRIMITIVE;
		}

		// Take care of memory deallocation
		std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_resp_prim(p_resp_prim);

		SrvProt_startup_v1_reply * p_startup_reply = dynamic_cast<SrvProt_startup_v1_reply *>(p_resp_prim);

		if ( !p_startup_reply ) { // ERROR: Failed cast
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: dynamic_cast<SrvProt_startup_v1_reply *> failed"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
			return SSH_ERROR_CAST_FAILED;
		}

		if ( p_startup_reply->get_error_field() != 0 ) { // ERROR: in received response primitive
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Received the primitive < %d > with the error code < %d >. "),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, p_startup_reply->get_primitive_id(),	p_startup_reply->get_error_field());
			return SSH_ERROR_PRIMITIVE_WITH_ERROR;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Received correctly the primitive < %d >"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, p_startup_reply->get_primitive_id());

		call_result = immDHobj.modifyNodeInfo(remote_node, p_startup_reply->get_node_state());
		if ( call_result ) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
					ACS_DSD_TEXT("%s::%s: Failed to update the SRTNode object for the node %s! - call_result == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, remote_node, call_result);
			return call_result;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: The SRTNode object for remote node %s successfully updated! - new_state == %u"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, remote_node, p_startup_reply->get_node_state());
	}

	return SSH_ERROR_NO_ERROR;
}
