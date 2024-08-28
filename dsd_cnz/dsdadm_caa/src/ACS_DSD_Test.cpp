#include "ACS_DSD_Test.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_SrvProt_StartupPrimitives.h"
#include "ACS_DSD_SrvProt_ListPrimitives.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"
#include "ACS_DSD_PrimitiveDataHandler.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;


int ACS_DSD_Test::start_activity()
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Starting TASK < %s > ..."), _task_name);

	int call_result = activate(_n_threads);
	if(call_result < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > NOT STARTED ! ACE_Task_Base::activate() returned < %d >"),_task_name, call_result);
		return -1;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > successfully STARTED !"), _task_name);

	return 0;
}


int ACS_DSD_Test::stop_activity(bool wait_termination)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Stopping TASK < %s > ..."), _task_name);

	int call_result = 0;

	if( wait_termination )
	{
		call_result = this->wait();
		if(call_result == -1)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > NOT STOPPED ! Call 'ACE_Task_Base::wait()' returned < %d > ! errno == %d"),_task_name, call_result, errno);
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > successfully  STOPPED !"), _task_name);
	return call_result;
}


int ACS_DSD_Test::svc(void)
{
	int call_result = 0;

	// Test Startup Primitives
	call_result = test_SrvProt_StartupPrimitives();

	// Test List Primitives
	call_result = test_SrvProt_ListPrimitives(0);
	call_result = test_SrvProt_ListPrimitives(1);
	call_result = test_SrvProt_ListPrimitives(2);

	return call_result;
}


int ACS_DSD_Test::test_SrvProt_StartupPrimitives()
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_StartupPrimitives(): Preparing the primitive to send."));

	// Find info about local node
	ACS_DSD_Node node_info;
	std::string error_descr;
	int call_result = ACS_DSD_Utils::get_node_info(node_info,error_descr);
	if( call_result < 0 )
	{
		ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Utils::get_node_info() failure ! return code is < %d > - description is < %s > !"), call_result, error_descr.c_str());
		return -1;
	}

	// Find node names for AP2 cluster
	char nodes_name[acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {{0}};
	int32_t sysId = acs_dsd::CONFIG_AP_ID_SET_START + 2;
	call_result = ACS_DSD_Utils::get_ApNodesName(sysId, nodes_name);
	if (call_result != 0 )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Test::test_SrvProt_StartupPrimitives() - get_ApNodesName():error finding AP node name with systemId=%d,  erroCode = %d"), sysId, call_result);
		return -1;
	}

	for (int idx = 0; idx < acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED; idx++)
	{
		SrvProt_startup_v1_request req_prim;
		req_prim.set_node_name(node_info.node_name);
		req_prim.set_node_state(node_info.node_state);
		req_prim.set_system_id(node_info.system_id);

		ACS_DSD_Client client;
		ACS_DSD_SrvProt_SvcHandler service_handler;
		call_result = client.connect(service_handler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, sysId, nodes_name[idx], DSD_SERVER_CONNECT_TIMEOUT_VALUE);
		if( call_result != 0 )
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_StartupPrimitives() - Can't establish connection to the DSD server on AP having node_name <%s>. CONN_ID = < %u >. Error code = < %d >"), nodes_name[idx], service_handler.getID(), call_result);
			return -1;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_StartupPrimitives() - Established connection to the DSD server on AP having node_name <%s>. CONN_ID = < %u >"), nodes_name[idx], service_handler.getID());

		call_result = service_handler.sendPrimitive(&req_prim, DSD_SERVER_SEND_TIMEOUT_VALUE);
		if( call_result != 0 )
		{
			ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Test::test_SrvProt_StartupPrimitives() - Unable to send the primitive!!! - Error Code = <%d> "), call_result);
			return -1;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_StartupPrimitives() - CONN_ID = < %u >. The STARTUP request primitive (v1) has been sent to AP < %s >. Primitive sent: %s "), service_handler.getID(), nodes_name[idx], req_prim.to_text().c_str());

		ACS_DSD_ServicePrimitive * p_resp_prim = 0;
		call_result = service_handler.recvPrimitive(p_resp_prim, DSD_SERVER_RECV_TIMEOUT_VALUE);
		if( call_result != 0 )
		{
			ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Test::test_SrvProt_StartupPrimitives() - Unable to receive the primitive!!! - Error Code = <%d> "), call_result);
			return -1;
		}

		SrvProt_startup_v1_reply * p_startup_reply = dynamic_cast<SrvProt_startup_v1_reply *>(p_resp_prim);

		if(p_startup_reply->get_error_field() != 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_StartupPrimitives(): Received the primitive < %d > with the error code < %d >. "), p_startup_reply->get_primitive_id(), p_startup_reply->get_error_field());
			return 0;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_StartupPrimitives(): Received correctly the primitive < %d >. Primitive received: %s"), p_startup_reply->get_primitive_id(), p_startup_reply->to_text().c_str());
		service_handler.close();
		delete p_startup_reply;
	}

	return call_result;
}


int ACS_DSD_Test::test_SrvProt_ListPrimitives(int list_scope)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives(): Preparing the primitive to send (scope %d)."), list_scope);

	// Find node names for AP2 cluster
	char nodes_name[acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {{0}};
	int32_t sysId = acs_dsd::CONFIG_AP_ID_SET_START + 2;
	int call_result = ACS_DSD_Utils::get_ApNodesName(sysId, nodes_name);
	if (call_result != 0 )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Test::test_SrvProt_ListPrimitives() - get_ApNodesName():error finding AP node name with systemId=%d,  erroCode = %d"), sysId, call_result);
		return -1;
	}

	// Connect to a node of AP2 cluster
	SrvProt_list_v1_request req_prim;

	if( list_scope == 0 )
		req_prim.set_system_id(acs_dsd::SYSTEM_ID_THIS_NODE);
	else if( list_scope == 1 )
		req_prim.set_system_id(acs_dsd::SYSTEM_ID_PARTNER_NODE);
	else
		req_prim.set_system_id(acs_dsd::SYSTEM_ID_ALL_AP_NODES);
	req_prim.set_node_name("");

	ACS_DSD_Client client;
	ACS_DSD_SrvProt_SvcHandler service_handler;
	call_result = client.connect(service_handler.peer(), ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID, sysId, nodes_name[0], DSD_SERVER_CONNECT_TIMEOUT_VALUE);
	if( call_result != 0 )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives() - Can't establish connection to the DSD server on AP having node_name <%s>. CONN_ID = < %u >. Error code = < %d >"), nodes_name[0], service_handler.getID(), call_result);
		return -1;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives() - Established connection to the DSD server on AP having node_name <%s>. CONN_ID = < %u >"), nodes_name[0], service_handler.getID());

	call_result = service_handler.sendPrimitive(&req_prim, DSD_SERVER_SEND_TIMEOUT_VALUE);
	if( call_result != 0 )
	{
		ACS_DSD_LOG (dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT ("ACS_DSD_Test::test_SrvProt_ListPrimitives() - Unable to send the primitive!!! - Error Code = <%d> "), call_result);
		return -1;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives() - CONN_ID = < %u >. The List request primitive (v1) has been sent to AP < %s >. Primitive sent: %s "), service_handler.getID(), nodes_name[0], req_prim.to_text().c_str());

	int more_to_receive = 1;
	do
	{
		ACS_DSD_ServicePrimitive * p_resp_prim = 0;
		call_result = service_handler.recvPrimitive(p_resp_prim, DSD_SERVER_RECV_TIMEOUT_VALUE);

		switch(call_result)
		{
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_RECEIVE_PEER_CLOSED:
				more_to_receive = 0;
				break;
			case ACS_DSD_SrvProt_SvcHandler::SRVPROT_SVC_OK:
				if( p_resp_prim->get_primitive_id() == SRVSRV_LIST_INET_REPLY )
				{
					SrvProt_list_inet_v1_reply * p_list_inet_reply = dynamic_cast<SrvProt_list_inet_v1_reply *> (p_resp_prim);
					if( p_list_inet_reply->get_error_field() != 0 )
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives(): Received the primitive < %d > with the error code < %d >. "), p_list_inet_reply->get_primitive_id(), p_list_inet_reply->get_error_field());
					else
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives(): Received correctly the primitive < %d >. Primitive received: %s"), p_list_inet_reply->get_primitive_id(), p_list_inet_reply->to_text().c_str());
					delete p_list_inet_reply;
				}
				else if( p_resp_prim->get_primitive_id() == SRVSRV_LIST_UNIX_REPLY )
				{
					SrvProt_list_unix_v1_reply * p_list_unix_reply = dynamic_cast<SrvProt_list_unix_v1_reply *> (p_resp_prim);
					if( p_list_unix_reply->get_error_field() != 0 )
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives(): Received the primitive < %d > with the error code < %d >. "), p_list_unix_reply->get_primitive_id(), p_list_unix_reply->get_error_field());
					else
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives(): Received correctly the primitive < %d >. Primitive received: %s"), p_list_unix_reply->get_primitive_id(), p_list_unix_reply->to_text().c_str());
					delete p_list_unix_reply;
				}
				else
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives(): Received wrong primitive (ID = < %d >)."), p_resp_prim->get_primitive_id());
				break;
			default:
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives() - Error while receiving the list reply primitive. Error code = < %d >"), call_result);
				more_to_receive = 0;
				break;
		}

	}while(more_to_receive);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_Test::test_SrvProt_ListPrimitives(): No more primitives to receive."));
	service_handler.close();

	return call_result;
}
