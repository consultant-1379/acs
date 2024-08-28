#include <memory>

#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"
#include "ACS_DSD_RemoteApNodesManager.h"
#include "ACS_DSD_StartupSessionsGroup.h"
#include "ACS_DSD_SrvProt_StartupPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_RemoteApNodesManager remote_AP_nodes_manager;
extern ACS_DSD_StartupSessionsGroup * startup_session_group;

/*SrvProt_startup_v1_request methods */
SrvProt_startup_v1_request::SrvProt_startup_v1_request()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_STARTUP_REQUEST;
	_system_id = 0;
	_node_state = 0;
	memset(_node_name,'\0',sizeof(_node_name));
}


std::string SrvProt_startup_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"SYSTEM_ID = %u,"
				"NODE_STATE = %u,"
				"NODE_NAME = %s,",
				_primitive_id,
				_protocol_version,
				_system_id,
				_node_state,
				_node_name);

	return mess;
}


int SrvProt_startup_v1_request::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_node_name, buff, buffsize);
}


int SrvProt_startup_v1_request::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}


int SrvProt_startup_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_STARTUP_REQUEST, SRVSRV_PROT_V1, _system_id, _node_state, _node_name);
}


int SrvProt_startup_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_system_id, &_node_state, _node_name);
}


int SrvProt_startup_v1_request::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * service_handler) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="SrvProt_startup_v1_request::process";
#endif

	int returnCode = ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT ("%s: - systemId = %d, state = %d, nodeName = %s"),
			FX_NAME, _system_id, _node_state, _node_name);

	// If a startup-request primitive of SRV-SRV protocol is received,
	// the system has a Multi-AP configuration
	startup_session_group->system_configuration(ACS_DSD_StartupSessionsGroup::SYS_CONF_MULTI_AP);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s:: Set 'system_configuration' status to MULTI-AP!"), FX_NAME);

	// Fetching info regarding the local node to send in the reply primitive
	ACS_DSD_Node node_info;
	std::string error_descr;
	int op_res = ACS_DSD_Utils::get_node_info(node_info,error_descr);
	if( op_res < 0 )
	{
		_process_error_descr = " ACS_DSD_Utils::get_node_info()failed!";
		ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT ("ACS_DSD_Utils::get_node_info() failure ! return code is < %d > - description is < %s > !"),
				op_res, error_descr.c_str());
		return ACS_DSD_PRIM_PROCESS_ERROR;
	}

	// Allocate a new primitive object
	SrvProt_startup_v1_reply *p_resp_prim = new (std::nothrow) SrvProt_startup_v1_reply();
	if( !p_resp_prim )
	{
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	// the response primitive will be sent to the peer within this method: so take care of memory deallocation
	std::auto_ptr<SrvProt_startup_v1_reply> auto_ptr_resp_prim(p_resp_prim);

	// Populate startup reply primitive fields
	p_resp_prim->set_system_id(node_info.system_id);
	p_resp_prim->set_node_state(node_info.node_state);
	p_resp_prim->set_node_name(node_info.node_name);
	p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
			ACS_DSD_TEXT("%s:: CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"),
			FX_NAME, service_handler->getID(), p_resp_prim->to_text().c_str());
	if(service_handler->sendPrimitive(p_resp_prim, DSD_SERVER_SEND_TIMEOUT_VALUE) < 0)
	{
		_process_error_descr = "Error sending Startup Reply for SRV-SRV Protocol primitive";
		return ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
			ACS_DSD_TEXT("%s: CONN_ID = < %u >.  The response primitive has been sent !"),
			FX_NAME, service_handler->getID());

	if ( node_info.node_state == acs_dsd::NODE_STATE_ACTIVE ) {

		ACS_DSD_SrvProt_SvcHandler * srv_prot_svc_hdl = dynamic_cast <ACS_DSD_SrvProt_SvcHandler *> (service_handler);
		if ( ! srv_prot_svc_hdl )
		{
			_process_error_descr = "Error while casting the service handler";
			return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
		}

		// Try to add this connection to collection of Remote AP Nodes Manager collection
		op_res = remote_AP_nodes_manager.add_remote_ap_node_connection(_system_id, srv_prot_svc_hdl);

		if ( op_res != ACS_DSD_RemoteApNodesManager::APNM_ERROR_NO_ERROR ) { //ERROR: Adding the connection to the collection
			if ( op_res == ACS_DSD_RemoteApNodesManager::APNM_ERROR_ITEM_ALREADY_PRESENT ) {
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
						ACS_DSD_TEXT("%s: The always alive connection for the remote AP %d is already present. Nothing to do!"),
						FX_NAME, _system_id);
			} else {
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
						ACS_DSD_TEXT("%s: CONN_ID = < %u >.  Call 'add_remote_ap_node_connection' failed: cannot add this "
						"service handler into my internal Remote-AP-Nodes collection: call_result == %d"),
						FX_NAME, service_handler->getID(), op_res);
			}
			return ACS_DSD_PRIM_PROCESS_OK_CLOSE_RESPONSE_ALREADY_SENT;
		}

		srv_prot_svc_hdl->set_linked_to_remote_ap_nodes_manager(true);
		srv_prot_svc_hdl->set_remote_note_system_id(_system_id);

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s: CONN_ID = < %u >.  Added this service handler into my internal "
					"Remote-AP-Nodes collection"), FX_NAME, service_handler->getID());

	} else {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
				ACS_DSD_TEXT("%s: I'm not the active node, nothing else to do!"), FX_NAME);
	}

	return returnCode;
}


/**********************************************************************************************************************/


/*SrvProt_startup_v1_reply methods */


SrvProt_startup_v1_reply::SrvProt_startup_v1_reply()
{
	_protocol_id = PROTOCOL_SRVSRV;
	_protocol_version = SRVSRV_PROT_V1;
	_primitive_id = SRVSRV_STARTUP_REPLY;
	_error = 0;
	_system_id = 0;
	_node_state = 0;
	memset(_node_name,'\0',sizeof(_node_name));
}


std::string SrvProt_startup_v1_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u, "
				"SYSTEM_ID = %u,"
				"NODE_STATE = %u,"
				"NODE_NAME = %s,",
				_primitive_id,
				_protocol_version,
				_error,
				_system_id,
				_node_state,
				_node_name);

	return mess;
}


int SrvProt_startup_v1_reply::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_node_name, buff, buffsize);
}


int SrvProt_startup_v1_reply::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}


int SrvProt_startup_v1_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SRVSRV_STARTUP_REPLY, SRVSRV_PROT_V1, _error, _system_id, _node_state, _node_name);
}


int SrvProt_startup_v1_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, &_system_id, &_node_state, _node_name);
}


int SrvProt_startup_v1_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}


