#include <string.h>
#include <utility>
#include <set>
#include <memory>
#include "ACS_CC_Types.h"

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_MacrosConstants.h"
#include "ACS_DSD_MacrosConfig.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_CpProt_SvcHandler.h"
#include "ACS_DSD_CpProt_ListApplPrimitives.h"
#include "ACS_DSD_AppProt_SvcHandler.h"
#include "ACS_DSD_AppProt_NotificationPrimitives.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_CpNodesManager.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACS_DSD_ImmConnectionHandler * imm_shared_objects_conn_ptr;
extern ACE_Recursive_Thread_Mutex  imm_shared_objects_conn_mutex;
extern int is_imm_shared_objects_controller;
extern ACS_DSD_Logger dsd_logger;


CpNodeItem::CpNodeItem()
{
	memset(node_name,'\0',sizeof(node_name));
	system_id = acs_dsd::SYSTEM_ID_UNKNOWN;
	system_type = acs_dsd::SYSTEM_TYPE_UNKNOWN;
	node_side = acs_dsd::NODE_SIDE_UNDEFINED;
	node_state = acs_dsd::NODE_STATE_UNDEFINED;
	svc_handlers[0] = 0;
	svc_handlers[1] = 0;
}


CpNodeItem::CpNodeItem(const CpNodeItem & rhs)
{
	memcpy(this->node_name,rhs.node_name,sizeof(node_name));
	this->system_id = rhs.system_id;
	this->system_type = rhs.system_type;
	this->node_side = rhs.node_side;
	this->node_state = rhs.node_state;
	this->svc_handlers[0] = rhs.svc_handlers[0];
	this->svc_handlers[1] = rhs.svc_handlers[1];
}

CpNodeItem::CpNodeItem(const char *node_name, int32_t system_id, acs_dsd::SystemTypeConstants system_type, acs_dsd::NodeSideConstants node_side, acs_dsd::NodeStateConstants node_state)
{
	strncpy(this->node_name, node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);
	this->system_id = system_id;
	this->system_type = system_type;
	this->node_side = node_side;
	this->node_state = node_state;
	svc_handlers[0] = 0;
	svc_handlers[1] = 0;
}


CpNodeItem & CpNodeItem::operator=(const CpNodeItem & rhs)
{
	memcpy(this->node_name,rhs.node_name,sizeof(node_name));
	this->system_id = rhs.system_id;
	this->system_type = rhs.system_type;
	this->node_side = rhs.node_side;
	this->node_state = rhs.node_state;
	this->svc_handlers[0] = rhs.svc_handlers[0];
	this->svc_handlers[1] = rhs.svc_handlers[1];

	return *this;
}


CpNode_imm_update_info::CpNode_imm_update_info(const char * cp_node_name)
: n_persist_state_updates(0), /*last_update_time(),*/ next_scheduled_update_time()
{
	strncpy(node_name, cp_node_name, ACS_DSD_ARRAY_SIZE(node_name));
	node_name[ACS_DSD_ARRAY_SIZE(node_name) -1] = 0;
}


ACS_DSD_CpNodesManager::ACS_DSD_CpNodesManager()
{
	_alarm_master_sys_id = acs_dsd::SYSTEM_ID_UNKNOWN;
	_clock_master_sys_id = acs_dsd::SYSTEM_ID_UNKNOWN;
	_alarm_master_CS_notified = false;
	_clock_master_CS_notified = false;
}


ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::get_cp_node_services(const char *cp_node_name, ServiceInfoList & service_list )
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char * FX_NAME = "ACS_DSD_CpNodesManager::get_cp_node_services()"; // for logging purposes
#endif

	if( (!cp_node_name) || !(*cp_node_name))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: invalid node name !"), FX_NAME);
		return CPNM_INVALID_NODE_NAME;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::get_cp_node_services() invoked. CP node == < %s > !"), cp_node_name);

	int call_result;
	ACS_DSD_Client client;
	ACS_DSD_CpProt_SvcHandler cpProtHandler;

	// we need the CP system id. Get it from System configuration (CS)
	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t hostInfo;
	if((call_result = ACS_DSD_ConfigurationHelper::get_node_by_node_name(cp_node_name, hostInfo)) < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: ACS_DSD_ConfigurationHelper::get_node_by_node_name() invocation returned < %d > !"), FX_NAME, call_result);
		return 	CPNM_NODE_NOT_FOUND;
	}

    // try to connect to the CP Node
	call_result = client.connect(cpProtHandler.peer(), ACS_DSD_CONFIG_CPAP_INET_SOCKET_SAP_ID, hostInfo->system_id, cp_node_name, DSD_SERVER_CONNECT_TIMEOUT_VALUE);
	if(call_result != 0)
	{
		if(call_result < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: error connecting to CP with systemId=%d, node_name =%s - erroCode = < %d >. Description : < %s >"), FX_NAME, hostInfo->system_id, cp_node_name, call_result, client.last_error_text());
			return CPNM_CONN_TO_CP_FAILURE;
		}
		else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: WARNING connecting to CP with systemId=%d, node_name =%s - erroCode = < %d >. Description : < %s >"), FX_NAME, hostInfo->system_id, cp_node_name, call_result, client.last_error_text()); }
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Established connection to CP having node_name <%s>. CONN ID = < %u >"), FX_NAME, cp_node_name, cpProtHandler.getID());


	// build the LIST APPLICATION request primitive and send it to the connected CP Node
	CpProt_listAppl_v1_request listApplReq_prim;
	listApplReq_prim.set_node_name(cp_node_name);

	if (!(call_result = cpProtHandler.sendPrimitive(& listApplReq_prim, DSD_SERVER_SEND_TIMEOUT_VALUE)))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: CONN_ID = < %u >. The LIST APPLICATION request primitive (v1) has been sent to CP. RetCode = < %d > !"), FX_NAME, cpProtHandler.getID(), call_result);
	else
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >.Failed to send LIST APPLICATION request primitive (v1) to CP. RetCode = < %d > !"), FX_NAME, cpProtHandler.getID(), call_result);
		return CPNM_SEND_LISTAPPL_FAILURE;	// error sending primitive
	}

	// wait for LIST APPLICATION reply primitives
	while(1)
	{
		// receive next LIST APPLICATION reply primitive
		ACS_DSD_ServicePrimitive *recv_primitive = 0;
		call_result = cpProtHandler.recvPrimitive(recv_primitive, DSD_SERVER_RECV_TIMEOUT_VALUE);
		switch(call_result)
		{
			case ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_OK:
				uint8_t prim_id;
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Received primitive.  Primitive data : %s"),FX_NAME, cpProtHandler.getID(), recv_primitive->to_text().c_str());
				if( (prim_id = recv_primitive->get_primitive_id()) != acs_dsd::PCP_CPAP_LIST_APPLICATION_REPLY_ID)
				{
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. LIST APPLICATION request primitive (v1) error: CP node replied with the primitive number < %d >"), FX_NAME, cpProtHandler.getID(), prim_id);
					delete recv_primitive;
					return CPNM_RECV_LISTAPPL_INVALID_REPLY;
				}
			break;

			case ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_RECEIVE_TIMEOUT:
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. TImeout occurred while waiting for primitive"),FX_NAME, cpProtHandler.getID());
				return CPNM_RECV_LISTAPPL_TIMEOUT;

			case ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_INVALID_PRIMITIVE:
			case ACS_DSD_CpProt_SvcHandler::CPPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. CP replied with an invalid or not implemented primitive"), FX_NAME, cpProtHandler.getID());
				return CPNM_RECV_LISTAPPL_INVALID_REPLY;

			default:
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. LIST APPLICATION primitive error: CP node reply error < %d >"),FX_NAME, cpProtHandler.getID(), call_result);
				return CPNM_RECV_LISTAPPL_FAILURE;
		}
		// take care of memory deallocation
		std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(recv_primitive);

		CpProt_listAppl_v1_reply *p_CpProt_listReply = dynamic_cast<CpProt_listAppl_v1_reply*>(recv_primitive);

		// check error code and, if != 0, discard the primitive
		if(p_CpProt_listReply->get_Error_field() != acs_dsd::PCP_ERROR_CODE_SUCCESSFUL)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: CONN_ID = < %u >. discarding LIST APPLICATION REPLY primitive having Error field < %u >"),FX_NAME, cpProtHandler.getID(), p_CpProt_listReply->get_Error_field());
			continue;
		}

		// get service name and service domain
		char serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME] = {0};
		char serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME] = {0};
		p_CpProt_listReply->get_service_name(serv_name,sizeof(serv_name));
		p_CpProt_listReply->get_service_domain(serv_domain,sizeof(serv_domain));

		// check if it's the last reply primitive
		if(!(*serv_name) && !(*serv_domain))
		{
			// this is the last reply. Exit loop
			break;
		}

		// insert service info into the output list
		try
		{
			ServiceInfo serv_info(serv_name,
								  serv_domain,
								  p_CpProt_listReply->get_conn_type(),
								  p_CpProt_listReply->get_num_of_conn(),
								  p_CpProt_listReply->get_ip1(),
								  p_CpProt_listReply->get_port1(),
								  p_CpProt_listReply->get_ip2(),
								  p_CpProt_listReply->get_port2());

			service_list.push_back(serv_info);

			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Added service <%s@%s> into output variable <service_list> "), FX_NAME, cpProtHandler.getID(), serv_name, serv_domain);
		}
		catch(std::bad_alloc &ba)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: CONN_ID = < %u >. Memory failure while adding a new service entry into output variable <service_list>. Details: < %s >"), FX_NAME, cpProtHandler.getID(), ba.what());
			return CPNM_MEMORY_FAILURE;
		}

	}

	return CPNM_OK;
}



ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::set_cp_conn(const char * cp_node_name, int32_t cp_system_id, acs_dsd::SystemTypeConstants cp_system_type, acs_dsd::NodeSideConstants cp_node_side, acs_dsd::NodeStateConstants cp_node_state, ACS_DSD_CpProt_SvcHandler * cp_prot_handler)
{
	if( (!cp_node_name) || !(*cp_node_name))
		return CPNM_INVALID_NODE_NAME;

	// let's start creating or updating the IMM entry related to the CP Node <cp_nome_name> ( in particular, we'll set state attribute).
	// Anyway, this operation must be performed only if the DSD Server is currently the controller of IMM shared objects
	imm_shared_objects_conn_mutex.acquire();
	if(is_imm_shared_objects_controller)
	{
		ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);

		// the IMM CP node could be already existing ... so try first an update
		if(!immDHobj.modifyNodeInfo(cp_node_name, cp_node_state))
		{
			// update success !
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::set_cp_conn(): the existing SRTNode for CP Node < %s > has been successfully UPDATED!"), cp_node_name);
		}
		else
		{
			// Update failed ! This means normally that the node doesn't exist. Try to create it
			immDHobj.set_state(cp_node_state);
			immDHobj.set_side(cp_node_side);
			char cp_system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX] = {0};
			ACS_DSD_ConfigurationHelper::system_id_to_system_name(cp_system_id, cp_system_name,acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX);
			immDHobj.set_node(cp_system_name);
			int create_node_res = immDHobj.addNodeInfo(cp_node_name);
			if(create_node_res < 0)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::set_cp_conn():  failed SRTNode creation for CP Node < %s >. Return code is < %d >"), cp_node_name, create_node_res);
				imm_shared_objects_conn_mutex.release();
				return CPNM_IMM_ERROR;
			}

			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::set_cp_conn():  SRTNode for CP Node < %s > successfully created."), cp_node_name);
		}
	}
	// release exclusive access to the IMM connection dedicated to the IMM shared objects
	imm_shared_objects_conn_mutex.release();

	// obtain exclusive access to the internal CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_mutex);

	// search the CP node in the internal collection
	CpNodesCollection::iterator it = _connected_cp_nodes.find(cp_node_name);
	if(it == _connected_cp_nodes.end())
	{
		// the node is NOT  present in the collection; insert it
		CpNodeItem * pNewCpNodeItem = new (std::nothrow) CpNodeItem (cp_node_name, cp_system_id, cp_system_type, cp_node_side, cp_node_state);
		if(!pNewCpNodeItem)
			return CPNM_MEMORY_FAILURE;
		pNewCpNodeItem->svc_handlers[0] = cp_prot_handler;
		CpNodesCollection::value_type newCpNodesCollectionEntry(cp_node_name, pNewCpNodeItem);
		std::pair<CpNodesCollection::iterator,bool> ret_val_1 = _connected_cp_nodes.insert(newCpNodesCollectionEntry);
		if(ret_val_1.second == false)
			return CPNM_NODE_ADD_SYNC_ERR;	// very strange ! Exclusive access to internal collection doesn't work !

		// add a new entry for the CP Node in the collection used to update IMM
		CpNode_imm_update_info * pNewCpNodeImmUpdateInfo =  new (std::nothrow) CpNode_imm_update_info(cp_node_name);
		if(!pNewCpNodeImmUpdateInfo)
			return CPNM_MEMORY_FAILURE;

		// get exclusive access to <_cp_node_imm_update_info_collection>
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->_update_info_list_mutex);
		CpNodesIMMUpdateInfoCollection::value_type newMapEntry(cp_node_name, pNewCpNodeImmUpdateInfo);
		std::pair<CpNodesIMMUpdateInfoCollection::iterator,bool> ret_val_2 = _cp_node_imm_update_info_collection.insert(newMapEntry);
		if(ret_val_2.second == false)
			return CPNM_IMM_UPDATE_INFO_ENTRY_ALREADY_EXISTS;	// very strange ! the element was already existing
	}
	else
	{
		// the CP node is already present in the internal collection. We must decide where to store the current connection handler (<cp_prot_handler>)
		// There can be two different connection handlers associated to the CP Node, but their remote IP addresses must be different. Let's check.
		CpNodeItem * pCpItem = (*it).second;
		int idx = -1;
		uint32_t ip_current_conn = cp_prot_handler->peer().get_remote_ip4_address();
		uint32_t ip_conn_0 = (pCpItem->svc_handlers[0] ? pCpItem->svc_handlers[0]->peer().get_remote_ip4_address() : 0);
		uint32_t ip_conn_1 = (pCpItem->svc_handlers[1] ? pCpItem->svc_handlers[1]->peer().get_remote_ip4_address() : 0);
		if((pCpItem->svc_handlers[0] == 0) && (pCpItem->svc_handlers[1] == 0))
			idx = 0;  // no connection handler is associated to this CP node.
		else if((pCpItem->svc_handlers[0] == 0) && (ip_conn_1 != ip_current_conn))
			idx = 0;  // exactly one connection handler found for this CP node in position <1>; anyway, the connection handler <cp_prot_handler> uses a different IP address.
		else if((pCpItem->svc_handlers[1] == 0) && (ip_conn_0 != ip_current_conn))
			idx = 1; // exactly one connection handler registered for this CP node in position <0>; anyway, the  connection handler <cp_prot_handler> uses a different IP address.
		else if(ip_conn_0 == ip_current_conn)
			idx = 0; // found a connection handler for this CP node in position <0>. using the same IP address of the <cp_prot_handler> connection handler. Replace it
		else if(ip_conn_1 == ip_current_conn)
			idx = 1; // found a connection handler for this CP node in position <1>, using the same IP address of the <cp_prot_handler> connection handler. Replace it

		if(idx == -1)
			return CPNM_NODE_ADD_GENERIC_ERR;

		// check if the current connection handler  must replace an existing connection handler ...
		if( (pCpItem->svc_handlers[idx] != 0) && (pCpItem->svc_handlers[idx]->getID() != cp_prot_handler->getID()))
			pCpItem->svc_handlers[idx]->peer().close(); // stop the old connection handler

		// store the current connection handler
		pCpItem->svc_handlers[idx] = cp_prot_handler;
	}

	// check if the CP Node belongs to a CP System that must be set as Clock and Alarm master
	if((ACS_DSD_ConfigurationHelper::get_cp_id_min() > 0)  &&  is_good_master(cp_system_id))
	{
		// we must set the CP System as Clock and Alarm master
		if(cp_system_id == _alarm_master_sys_id)
		{
			if(!_alarm_master_CS_notified) { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("The alarm master is already known but not been set. Trying of set cp with system id < %d > as alarm master in CS"), cp_system_id); }
			else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("The alarm master is already set. The actual cp with system id < %d > is the alarm master"), cp_system_id); }

			if(!_clock_master_CS_notified) { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("The clock master is already known but not been set. Trying of set cp with system id < %d > as clock master in CS"), cp_system_id); }
			else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("The clock master is already set. The actual cp with system id < %d > is the clock master"), cp_system_id); }
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Setting CP with system id < %d > as alarm and clock master"), cp_system_id);
			_alarm_master_sys_id = cp_system_id;
			_clock_master_sys_id = cp_system_id;

			// The Alarm and Clock master CP is changed: it's needed to set it into CS
			_alarm_master_CS_notified = false;
			_clock_master_CS_notified = false;
		}

		// now let's take care of setting Alarm anc Clock master into CS ...
		if(!_alarm_master_CS_notified)
		{
			int op_res = ACS_DSD_ConfigurationHelper::set_alarm_master_in_cs(cp_system_id);
			if(op_res <  0)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Failure setting CP < %d > as Alarm Master into CS. ERROR_CODE  = < %d >"), _alarm_master_sys_id, op_res);
			else
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CP < %d > set as Alarm Master into CS ! ERROR_CODE = < %d >"), _alarm_master_sys_id, op_res);
				_alarm_master_CS_notified = true;
			}
		}

		// If not already done, try to set Clock Master into System configuration, using CS API
		if(!_clock_master_CS_notified)
		{
			int op_res = ACS_DSD_ConfigurationHelper::set_clock_master_in_cs(cp_system_id);
			if(op_res <  0)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Failure setting CP < %d > as Clock Master into CS. ERROR_CODE  = < %d >"), _clock_master_sys_id, op_res);
			else
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CP < %d > set as Clock Master into CS ! ERROR_CODE = < %d >"), _clock_master_sys_id, op_res);
				_clock_master_CS_notified = true;
			}
		}
	}

	return CPNM_OK;
}

ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::remove_cp_conn(const char * cp_node_name, ACS_DSD_CpProt_SvcHandler * cp_prot_handler)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::remove_cp_conn():  CP Node Name = < %s > ;  cp_prot_handler ID = < %d >!"), cp_node_name, cp_prot_handler->getID());

	if( (!cp_node_name) || !(*cp_node_name))
		return CPNM_INVALID_NODE_NAME;

	bool stateChanged = false;

	// obtain exclusive access to the internal CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_mutex);

	// search the CP node in the internal collection
	CpNodesCollection::iterator it = _connected_cp_nodes.find(cp_node_name);
	if(it == _connected_cp_nodes.end())
		return CPNM_NODE_NOT_FOUND;// the node is NOT  present in the collection

	// now find the connection
	CpNodeItem * pCpItem = (*it).second;
	int idx = -1;
	if(pCpItem->svc_handlers[0] && (pCpItem->svc_handlers[0]->getID() == cp_prot_handler->getID()))
		idx = 0;
	else if(pCpItem->svc_handlers[1] && (pCpItem->svc_handlers[1]->getID() == cp_prot_handler->getID()))
		idx = 1;

	if(idx == -1)
		return CPNM_CONN_NOT_FOUND;		// connection not found

	// remove the connection
	pCpItem->svc_handlers[idx] =  0;

	// cache the CP system id because the object <*pCpItem> could be deleted in the following
	int32_t cp_system_id = pCpItem->system_id;

	// if the removed connection was the last connection, remove the CP node, delete IMM entry for CP node, and start notification
	if(!(pCpItem->svc_handlers[0]) && !(pCpItem->svc_handlers[1]))
	{
		delete pCpItem;
		_connected_cp_nodes.erase(it);

		// delete the IMM entry related to the CP Node, but only if this DSD Server is currently the controller of imm shared objects
		imm_shared_objects_conn_mutex.acquire();
		if(is_imm_shared_objects_controller)
		{
			// ok, this DSD Server is the controller of imm shared objects
			ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);
			int op_res = immDHobj.removeNodeInfo(cp_node_name);
			if(!op_res) { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::remove_cp_conn(): the SRTNode for CP Node < %s > has been successfully DELETED!"), cp_node_name); }
			else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::remove_cp_conn():  Error deleting SRTNode for CP Node < %s >. The method ACS_DSD_ImmDataHandler::removeNodeInfo() returned < %d >"), cp_node_name, op_res); }
		}
		imm_shared_objects_conn_mutex.release();

		// let's remove CP Node entry from  the collection used to update IMM
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_update_info_list_mutex);
		CpNodesIMMUpdateInfoCollection::iterator it = _cp_node_imm_update_info_collection.find(cp_node_name);
		if(it == _cp_node_imm_update_info_collection.end())
			return CPNM_IMM_UPDATE_INFO_ENTRY_NOT_FOUND;

		delete (*it).second;
		_cp_node_imm_update_info_collection.erase(it);

		stateChanged = true;
	}

	if(stateChanged)
	{
		// get CP State
		acs_dsd::CPStateConstants cp_system_state;
		OP_Result op_res = this->get_cp_system_state(cp_system_id, cp_system_state);
		if(op_res == CPNM_OK)
		{
			 // Start Notification !!!!
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("calling ACS_DSD_CpNodesManager::notifyCpStateChange() - system_id = %d, cp_state = %d -"),cp_system_id, cp_system_state);
			notifyCpStateChange(cp_system_id,cp_system_state);
		}
		else
			return CPNM_GETSTATE_ERROR;
	}

	return CPNM_OK;
}

ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::close_cp_conn(const char * cp_node_name) {
	if(!cp_node_name)
		return CPNM_INVALID_NODE_NAME;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("ACS_DSD_CpNodesManager::close_cp_conn(): CP Node Name = < %s >!"), cp_node_name);

	// obtain exclusive access to the internal CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_mutex);

	// search the CP node in the internal collection
	CpNodesCollection::iterator it = _connected_cp_nodes.find(cp_node_name);
	if(it == _connected_cp_nodes.end())
		return CPNM_NODE_NOT_FOUND;

	CpNodeItem * pCpItem = it->second;

	// Close each connection towards the given CP
	for (int i = 0; i < 2; i++)
		if (pCpItem->svc_handlers[i])
			pCpItem->svc_handlers[i]->peer().close();

	return CPNM_OK;
}

ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::get_cp_system_state(int32_t cp_system_id, acs_dsd::CPStateConstants & cp_system_state)
{
	// we need exclusive access to the internal CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_mutex);

	// take into account the special system ids
	int32_t effective_cp_system_id = get_effective_cp_system_id(cp_system_id);

	// iterate on the internal CP nodes collection to get the state of the  CP nodes belonging to the CP System having system id:  <cp_system_id>
	acs_dsd::NodeStateConstants cp_node_1_state = acs_dsd::NODE_STATE_UNDEFINED;
	acs_dsd::NodeStateConstants cp_node_2_state = acs_dsd::NODE_STATE_UNDEFINED;
	bool cp_node_1_found = false;
	bool cp_node_2_found = false;
	for(CpNodesCollection::iterator it = _connected_cp_nodes.begin(); it != _connected_cp_nodes.end(); ++it)
	{
		CpNodeItem * pCpNodeItem = (*it).second;
		if( pCpNodeItem->system_id == effective_cp_system_id)
		{
			if((pCpNodeItem->system_type == acs_dsd::SYSTEM_TYPE_BC) || (pCpNodeItem->node_side == acs_dsd::NODE_SIDE_A))
			{
				cp_node_1_state = pCpNodeItem->node_state;
				cp_node_1_found = true;
			}
			else if(pCpNodeItem->node_side == acs_dsd::NODE_SIDE_B)
			{
				cp_node_2_state = pCpNodeItem->node_state;
				cp_node_2_found = true;
			}

			if( (pCpNodeItem->system_type == acs_dsd::SYSTEM_TYPE_BC) || (cp_node_1_found && cp_node_2_found) )
				break;
		}
	}

	// calculate the CP system state
	if((cp_node_1_state == acs_dsd::NODE_STATE_EXECUTION) && (cp_node_2_state == acs_dsd::NODE_STATE_STANDBY))
		cp_system_state = acs_dsd::CP_STATE_SEPARATED;
	else if((cp_node_1_state == acs_dsd::NODE_STATE_EXECUTION) && (!cp_node_2_found))
		cp_system_state = acs_dsd::CP_STATE_NORMAL;
	else if((cp_node_1_state == acs_dsd::NODE_STATE_STANDBY) && (cp_node_2_state == acs_dsd::NODE_STATE_EXECUTION))
		cp_system_state = acs_dsd::CP_STATE_SEPARATED;
	else if((cp_node_1_state == acs_dsd::NODE_STATE_STANDBY) && (!cp_node_2_found))
		cp_system_state = acs_dsd::CP_STATE_SEPARATED;
	else if((!cp_node_1_found) && (cp_node_2_state == acs_dsd::NODE_STATE_EXECUTION))
		cp_system_state = acs_dsd::CP_STATE_NORMAL;
	else if((!cp_node_1_found) && (cp_node_2_state == acs_dsd::NODE_STATE_STANDBY))
		cp_system_state = acs_dsd::CP_STATE_SEPARATED;
	else
		cp_system_state = acs_dsd::CP_STATE_UNDEFINED;

	return CPNM_OK;
}


ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::get_cp_node_state(const char * cp_node_name, acs_dsd::NodeStateConstants & cp_node_state)
{
	if( (!cp_node_name) || !(*cp_node_name))
		return CPNM_INVALID_NODE_NAME;

	CpNodesCollection::iterator it = _connected_cp_nodes.find(cp_node_name);
	if(it == _connected_cp_nodes.end())
		return CPNM_NODE_NOT_FOUND;

	CpNodeItem * pCpItem = (*it).second;
	cp_node_state = pCpItem->node_state;
	return CPNM_OK;
}


bool ACS_DSD_CpNodesManager::is_good_master(int system_id)
{
	return (system_id == ACS_DSD_ConfigurationHelper::get_cp_id_min());
}


int32_t ACS_DSD_CpNodesManager::get_alarm_master_cp_system_id()
{
	// obtain exclusive access to the internal CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_mutex);

	if(_alarm_master_sys_id != acs_dsd::SYSTEM_ID_UNKNOWN)
		return _alarm_master_sys_id;
	else
		// if DSD Server doesn't know the Alarm master CP, we return the minimum CP System id of the System configuration (CS)
		return ACS_DSD_ConfigurationHelper::get_cp_id_min();
}

int32_t ACS_DSD_CpNodesManager::get_clock_master_cp_system_id()
{
	// obtain exclusive access to the internal CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_mutex);

	if(_clock_master_sys_id != acs_dsd::SYSTEM_ID_UNKNOWN)
		return _clock_master_sys_id;
	else
		// if DSD Server doesn't know the Clock master CP, we return the minimum CP System id of the System configuration (CS)
		return ACS_DSD_ConfigurationHelper::get_cp_id_min();
}


ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::find_cp_node_by_systemId_and_state(int32_t cp_system_id, acs_dsd::NodeStateConstants cp_node_state,  char found_cp_node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX])
{
	// obtain exclusive access to the internal CP nodes collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_mutex);

	// take into account the special system ids
	int32_t effective_cp_system_id = get_effective_cp_system_id(cp_system_id);

	for(CpNodesCollection::iterator it = _connected_cp_nodes.begin(); it != _connected_cp_nodes.end(); ++it)
	{
		CpNodeItem * pCpNodeitem = (*it).second;
		if( (pCpNodeitem->system_id == effective_cp_system_id) && (pCpNodeitem->node_state == cp_node_state) )
		{
			strcpy(found_cp_node_name, pCpNodeitem->node_name);
			return CPNM_OK;
		}
	}

	return CPNM_NODE_NOT_FOUND;
}


bool ACS_DSD_CpNodesManager::notification_needed(const char * cp_node_name, int32_t cp_system_id, acs_dsd::SystemTypeConstants cp_system_type, acs_dsd::NodeSideConstants cp_node_side, acs_dsd::NodeStateConstants cp_node_state, acs_dsd::CPStateConstants & out_cp_system_state )
{
	/*
	 * According to the design base:
	 * 1 - for the blade CPs, the notification is never needed;
	 * 2 - for the double side CPs, the notification is needed if and only if the state of the CP node  <cp_node_name> changed. In this case
	 *     the state of the CP system is calculated and returned in the output parameter <out_cp_system_state>.
	*/
	if(cp_system_type == acs_dsd::SYSTEM_TYPE_BC)
		return false; // No notification needed for blade CPs

	// if we are here the CP node belongs to a double side CP.

	// get the state of the CP node
	acs_dsd::NodeStateConstants old_cp_node_state;
	OP_Result op_res = get_cp_node_state(cp_node_name, old_cp_node_state);
	if((op_res != CPNM_OK) && (op_res != CPNM_NODE_NOT_FOUND))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::notification_needed() - Call 'ACS_DSD_CpNodesManager::get_cp_node_state()' failed ! call_result == %d "), op_res);
		return false;	// Notification doesn't make sense in this situation!
	}

	// if the CP node is present in the internal CP nodes collection, we need to know if the CP node state has changed
	if( (op_res == CPNM_OK) && (old_cp_node_state == cp_node_state) )
		return false;  // the CP node exists in the internal collection (it has already sent a STARTUP primitive) and didn't change its state. Notification not needed !

	// if we are here notification is needed. Now we must calculate the state of the CP system so that we can communicate such state in the notification.
	// Such state depends from the other side of the CP. Get info about such side.
	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t cpOtherSideHostInfo;
	int call_result = ACS_DSD_ConfigurationHelper::get_partner_node_by_side(cp_system_id, cp_node_side, cpOtherSideHostInfo);
	if(call_result < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::notification_needed() - Call 'ACS_DSD_ConfigurationHelper::get_partner_node_by_side()' failed ! call_result == %d"), call_result);
		return false;  // we can't determine CP system state, so it's better to avoid notification
	}
	else
	{
		// get the state of the other CP side
		acs_dsd::NodeStateConstants other_side_cp_node_state;
		OP_Result op_res = get_cp_node_state(cpOtherSideHostInfo->node_name, other_side_cp_node_state);
		if(op_res == CPNM_OK)
			out_cp_system_state = acs_dsd::CP_STATE_SEPARATED;	// both sides of the CP sent STARTUP primitive. CP system state is SEPARATED
		else if(op_res == CPNM_NODE_NOT_FOUND)
		{
			// no STARTUP primitive arrived from the other CP side. The CP system state is decided by the CP node state
			if(cp_node_state == acs_dsd::NODE_STATE_EXECUTION)
				out_cp_system_state = acs_dsd::CP_STATE_NORMAL;
			else
				out_cp_system_state = acs_dsd::CP_STATE_SEPARATED;
		}
		else
		{	// an error occurred getting the state of the other CP side !
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::notification_needed() - Call 'ACS_DSD_CpNodesManager::get_cp_node_state()' for the other CP side failed ! call_result == %d "), op_res);
			return false;  // no notification !
		}
	}

	return true;

}

ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::update_IMM_cp_node(const char * cp_node_name, const ServiceInfoList & service_list, int32_t & n_serv_added, int32_t & n_serv_modified, int32_t & n_serv_removed)
{
	if(!cp_node_name || !*cp_node_name)
		return CPNM_INVALID_NODE_NAME;

	n_serv_added = 0;
	n_serv_modified = 0;
	n_serv_removed = 0;

	ACS_DSD_CpNodesManager::OP_Result ret_val = CPNM_OK;

//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char * FX_NAME = "ACS_DSD_CpNodesManager::update_IMM_cp_node()"; // for logging purposes
#endif

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Updating SRTNode for CP Node: --- CP_NODE.name = %s !"), FX_NAME, cp_node_name);

	const char * dsd_imm_root_dn = 0;

	if (ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(dsd_imm_root_dn)) { //ERROR
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM"), FX_NAME);
		return CPNM_IMM_DSD_ROOT_SEARCH_ERROR;
	}

	//  obtain exclusive access to IMM shared objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_shared_objects_conn_mutex);

	// check if we are the controller of IMM shared objects
	if(! is_imm_shared_objects_controller)
		return CPNM_IMM_OP_NOT_PERMITTED;

	ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);

	// STEP 1: get from IMM the list of services currently registered under the SRTNode associated to the CP Node having name <cp_node_name> (if existing)
	std::vector<std::string> rdn_vec;
	int op_res = immDHobj.fetchRegisteredServicesList(cp_node_name, rdn_vec);
	if(op_res < 0)
	{
		if(op_res == acs_dsd_imm::ERR_IMM_OM_GET)
		{
			// the IMM entry for CP Node doesn't exist
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: the IMM SRTNode entry for CP Node < %s > was NOT found !"),FX_NAME, cp_node_name);
			return CPNM_NODE_NOT_FOUND;
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: IMM error getting services under CP Node %s. Error code = < %d >"), FX_NAME, cp_node_name, op_res);
			return CPNM_IMM_ERROR;
		}
	}

	// build an helper temporary set
	std::set<std::string> current_cp_services_rdn(rdn_vec.begin(), rdn_vec.end());

	// STEP 2: iterate on the <services_list> argument to add or modify the IMM entries for the passed services
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};
	const char *inet_addresses[2];
	ACE_INET_Addr ip_addr[2];
	char addr_str[2][32] = {{0},{0}};
	for(ServiceInfoList::const_iterator it = service_list.begin(); it != service_list.end(); ++it)
	{
		// get the next service item in the list
		const ServiceInfo & servInfoItem = *it;

		// build the IMM DN path for the service item
		snprintf(imm_dnName,
				 acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
				 ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
				 servInfoItem.serv_name,
				 servInfoItem.serv_domain,
			 	 cp_node_name, dsd_imm_root_dn);

		// transform the ip addresses and ports from integer format to text format
		if(servInfoItem.NrOfConn > 0)
		{
			ip_addr[0].set(servInfoItem.port1, servInfoItem.ip1);
			ip_addr[0].addr_to_string(addr_str[0], 32);
			inet_addresses[0] = addr_str[0];
		}
		if(servInfoItem.NrOfConn > 1)
		{
			ip_addr[1].set(servInfoItem.port2, servInfoItem.ip2);
			ip_addr[1].addr_to_string(addr_str[1], 32);
			inet_addresses[1] = addr_str[1];
		}

		// prepare the IMM object to be added or modified
		if(servInfoItem.NrOfConn > 0)
			immDHobj.set_tcp_addresses(inet_addresses, servInfoItem.NrOfConn == 1 ? 1 : 2);

		immDHobj.set_conn_type(acs_dsd::CONNECTION_TYPE_OCP);

		// now search the RDN among the services currently registered in IMM
		if(current_cp_services_rdn.count(imm_dnName)>0)
		{
			// the service is already registered in IMM. Modify it's node entry in IMM
			int op_res = immDHobj.modifyServiceInfo(cp_node_name, servInfoItem.serv_name, servInfoItem.serv_domain);
			if(!op_res)
			{   // modify succeeded !
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Service  < %s@%s > successfully registered (overwriting a previous registration) !"), FX_NAME, servInfoItem.serv_name, servInfoItem.serv_domain);
			}
			else
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Service  < %s@%s > NOT registered !  Error description: %s"), FX_NAME, servInfoItem.serv_name, servInfoItem.serv_domain, immDHobj.last_error_text());
				ret_val = CPNM_IMM_PARTIAL_UPDATE;
			}

			// update counter of modified services
			++n_serv_modified;

			// remove the service from the helper SET
			current_cp_services_rdn.erase(imm_dnName);
		}
		else
		{
			// the service is not registered in IMM. Add a new entry for it
			int op_res = immDHobj.addServiceInfo(cp_node_name, servInfoItem.serv_name, servInfoItem.serv_domain);
			if(!op_res)
			{   // creation succeeded !
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Service  < %s@%s > successfully registered !"), FX_NAME, servInfoItem.serv_name, servInfoItem.serv_domain);
			}
			else
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Service  < %s@%s > NOT registered !  Error description: %s"), FX_NAME, servInfoItem.serv_name, servInfoItem.serv_domain, immDHobj.last_error_text());
				ret_val = CPNM_IMM_PARTIAL_UPDATE;
			}

			// update counter of added services
			++n_serv_added;
		}
	}

	// STEP 3: now we must delete the current services not present in the new service list
	for(std::set<std::string>::iterator it = current_cp_services_rdn.begin(); it != current_cp_services_rdn.end(); ++it)
	{
		int op_res = immDHobj.deleteServiceInfo((*it).c_str());
		if(!op_res)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Service having RDN = < %s > successfully deleted fromm IMM !"), FX_NAME, (*it).c_str());
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Service having having RDN = < %s > NOT deleted from IMM !  Error description: %s"),FX_NAME, (*it).c_str(), immDHobj.last_error_text());
			ret_val = CPNM_IMM_PARTIAL_UPDATE;
		}

		// update counter of removed services
		++n_serv_removed;
	}

	return CPNM_OK;
}


int32_t ACS_DSD_CpNodesManager::get_effective_cp_system_id(int32_t cp_system_id)
{
	if(cp_system_id == acs_dsd::SYSTEM_ID_CP_ALARM_MASTER)
		return get_alarm_master_cp_system_id();
	else if(cp_system_id == acs_dsd::SYSTEM_ID_CP_CLOCK_MASTER)
		return get_clock_master_cp_system_id();
	else
		return cp_system_id;
}

ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::choose_master_cp () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_mutex);

	if (_alarm_master_sys_id != _clock_master_sys_id) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::choose_master_cp(): "
				"WARNING: _alarm_master_sys_id = %d, _clock_master_sys_id = %d!"), _alarm_master_sys_id, _clock_master_sys_id);
	}

	int32_t master_id = _alarm_master_sys_id = _clock_master_sys_id = acs_dsd::SYSTEM_ID_UNKNOWN;
	_alarm_master_CS_notified = _clock_master_CS_notified = false;

	// Try to find a new master, searching for the min CP id different from the actual master
	for (CpNodesCollection::iterator it = _connected_cp_nodes.begin(); it != _connected_cp_nodes.end(); it++) {
		CpNodeItem * node = it->second;

		if (is_good_master(node->system_id)) {
			master_id = node->system_id;
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT(
					"ACS_DSD_CpNodesManager::choose_master_cp(): New master CP found, system_id = %d!"), master_id);
			break;
		}
	}

	if (master_id == acs_dsd::SYSTEM_ID_UNKNOWN) {
		master_id = ACS_DSD_ConfigurationHelper::get_cp_id_min();
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::choose_master_cp(): "
				"No candidate to be alarm/clock master was found in the CP Nodes Manager collection, using the "
				"expected value (%d)!"), master_id);
	}

	int call_result = 0;
	_alarm_master_sys_id = _clock_master_sys_id = master_id;

	// Set the new value as alarm master in CS
	if ((call_result = ACS_DSD_ConfigurationHelper::set_alarm_master_in_cs(master_id))) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Failure setting CP < %d > as Alarm Master into CS. "
				"ERROR_CODE = < %d >"), _alarm_master_sys_id, call_result);
	}
	else {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CP < %d > set as Alarm Master into CS! "
				"ERROR_CODE = < %d >"), _alarm_master_sys_id, call_result);
		_alarm_master_CS_notified = true;
	}

	// Set the new value as clock master in CS
	if ((call_result = ACS_DSD_ConfigurationHelper::set_clock_master_in_cs(master_id))) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Failure setting CP < %d > as Clock Master into CS. "
				"ERROR_CODE = < %d >"), _clock_master_sys_id, call_result);
	}
	else {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CP < %d > set as Clock Master into CS! "
				"ERROR_CODE = < %d >"), _clock_master_sys_id, call_result);
		_clock_master_CS_notified = true;
	}

	return CPNM_OK;
}

//******************************************************************************
//      addNotification
//******************************************************************************
int ACS_DSD_CpNodesManager::addNotification(int32_t sysId, ACS_DSD_ServiceHandler *pServiceHandler)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_notifReq_mutex);
	NotifRequestList::iterator itNotifReq;

	if((itNotifReq =_received_notifRequest.find(sysId)) != _received_notifRequest.end()){
		// a nofication request for cpId already exist, so it needs only add the ServiceHAndler into the associated list
		(itNotifReq->second).push_back(pServiceHandler);
		(itNotifReq->second).unique();
	}
	else {
		// add a new entry in the map
		std::list<ACS_DSD_ServiceHandler *> list_elem;
		list_elem.push_back(pServiceHandler);
		NotifRequestList::value_type newMapEntry(sysId, list_elem);
		std::pair<NotifRequestList::iterator,bool> retvalue = _received_notifRequest.insert(newMapEntry);
		if(retvalue.second == false) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::addNotification() - failure. Service Handler Address == %p, CP System ID == %d"), pServiceHandler, sysId);
			return CPNM_NOTIFICATION_ADD_ERR;	// very strange ! Exclusive access to internal collection doesn't work !

		}
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("service handler (%p) subscribed for CP state notifications regarding CP system having ID == %d"), pServiceHandler, sysId);
	return CPNM_OK;
}

//******************************************************************************
//      removeNotification
//******************************************************************************
int ACS_DSD_CpNodesManager::removeNotification(int32_t sysId, ACS_DSD_ServiceHandler *pServiceHandler)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_notifReq_mutex);
	NotifRequestList::iterator itNotifReq = _received_notifRequest.find(sysId);

	if(itNotifReq == _received_notifRequest.end()){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::removeNotification() - subscription NOT found. (service handler == %p, CP System ID == %d)"), pServiceHandler, sysId);
		return CPNM_OK;
	}
	std::list<ACS_DSD_ServiceHandler *> & servHandler_list = itNotifReq->second;
	for (std::list<ACS_DSD_ServiceHandler *>::iterator it= servHandler_list.begin(); it!=servHandler_list.end(); it++){
		if((*it)== pServiceHandler){
			servHandler_list.erase(it);
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("canceled subscription of service handler (%p) for CP state notifications regarding CP System having ID == %d !"), pServiceHandler, sysId);
			break;
		}
	}
	if(servHandler_list.size() == 0){
		_received_notifRequest.erase(itNotifReq);
	}
	return CPNM_OK;
}

//******************************************************************************
//      removeNotification
//******************************************************************************
int ACS_DSD_CpNodesManager::notifyCpStateChange(int32_t sysId, uint8_t cp_state)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_notifReq_mutex);
	NotifRequestList::iterator itNotifReq = _received_notifRequest.find(sysId);

	if(itNotifReq == _received_notifRequest.end()){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::notifyCpStateChange() - there is no client subscribed for notifications for CP ID = %d"), sysId);
		return CPNM_OK;
	}
	int call_result;
	std::list<ACS_DSD_ServiceHandler *> servHandler_list = itNotifReq->second;
	for (std::list<ACS_DSD_ServiceHandler *>::iterator it=servHandler_list.begin(); it!=servHandler_list.end(); it++){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::NotifyCpStateChange() - send Notification Indication for CP ID = %d to Service Handler %p!"), sysId, *it);
		if((call_result = sendNotificationRsp(*it, cp_state)) < 0){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_CpNodesManager::NotifyCpStateChange() - error in 'sendNotificationRsp' call_result == %d"), call_result);
		}
	}
	return CPNM_OK;
}

int ACS_DSD_CpNodesManager::sendNotificationRsp(ACS_DSD_ServiceHandler *pServiceHandler, uint8_t cp_state)
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]= "ACS_DSD_CpNodesManager::sendNotificationRsp";
#endif

	AppProt_notification_v1_indication  resp_prim;

	resp_prim.set_cpState(cp_state);
	if (!pServiceHandler->sendPrimitive(&resp_prim, DSD_SERVER_SEND_NOTIFY_INDICATION_TIMEOUT_VALUE))
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. The notification indication primitive has been sent!"),FX_NAME, pServiceHandler->getID());
	else{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s:  CONN_ID = < %u >. Failed to send notification indication primitive!"), FX_NAME, pServiceHandler->getID());
		return CPNM_SEND_NOTIFICATIONIND_FAILURE;	// error sending primitive
	}
	return CPNM_OK;
}

ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::get_nodes (
		acs_dsd::SystemTypeConstants system_type,
		ACS_DSD_Node * nodes,
		size_t & size) const {
	size_t items_count = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	for (CpNodesCollection::const_iterator it = _connected_cp_nodes.begin(); it != _connected_cp_nodes.end(); (it++->second->system_type ^ system_type) || ++items_count) ;

	if (items_count > size) { size = items_count; return CPNM_NOT_ENOUGH_SPACE; }

	if ((size = items_count))
		for (CpNodesCollection::const_iterator it = _connected_cp_nodes.begin(); it != _connected_cp_nodes.end(); ++it) {
			if (it->second->system_type ^ system_type) continue;
			memccpy(nodes->node_name, it->second->node_name, 0, ACS_DSD_ARRAY_SIZE(nodes->node_name));
			nodes->node_name[ACS_DSD_ARRAY_SIZE(nodes->node_name) - 1] = 0;
			nodes->node_side = it->second->node_side;
			nodes->node_state = it->second->node_state;
			nodes->system_id = it->second->system_id;
			nodes->system_name[0] = 0;
			nodes->system_type = it->second->system_type;
			++nodes;
		}

	return CPNM_OK;
}


ACS_DSD_CpNodesManager::OP_Result ACS_DSD_CpNodesManager::synch_IMM(int32_t & n_nodes_added, int32_t & n_nodes_removed)
{
	ACS_DSD_CpNodesManager::OP_Result ret_val = CPNM_OK;

	n_nodes_added = 0;
	n_nodes_removed = 0;

	// Get exclusive access to internal collection
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	//#ifdef ACS_DSD_LOGGING_ACTIVE
	#ifdef ACS_DSD_HAS_LOGGING
		const char * FX_NAME = "ACS_DSD_CpNodesManager::synch_IMM()"; // for logging purposes
	#endif

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Synchronizing IMM with ACS_DSD_CpNodesManager object !"), FX_NAME);

	// search IMM DSD root object
	const char * dsd_imm_root_dn = 0;
	if (ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(dsd_imm_root_dn)) { //ERROR
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM"), FX_NAME);
		return CPNM_IMM_DSD_ROOT_SEARCH_ERROR;
	}

	//  get exclusive access to IMM shared objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> imm_guard(imm_shared_objects_conn_mutex);

	// check if we are the controller of IMM shared objects
	if(! is_imm_shared_objects_controller)
		return CPNM_IMM_OP_NOT_PERMITTED;

	ACS_DSD_ImmDataHandler immDHobj(imm_shared_objects_conn_ptr);

	// STEP 1: get from IMM the list of the SRTNode objects (that are children of the IMM DSD root object)
	std::vector<std::string> rdn_vec;
	int op_res = immDHobj.fetch_SRTNodes_list(rdn_vec);
	if(op_res < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: IMM error getting SRTNode objects. Error code = < %d >"), FX_NAME, op_res); //Fix for TR IA26821
		return CPNM_IMM_ERROR;
	}

	// build an helper temporary set
	std::set<std::string> current_SRTNodes_dn(rdn_vec.begin(), rdn_vec.end());

	// STEP 2: iterate on the internal collection
	for (CpNodesCollection::const_iterator it = _connected_cp_nodes.begin(); it != _connected_cp_nodes.end(); ++it)
	{
		const CpNodeItem * cpNodeItem = (*it).second;

		// build DN associated to the current CP node item
		char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};
		snprintf(imm_dnName,
				 acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
				 ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
				 cpNodeItem->node_name,
				 dsd_imm_root_dn);

		// now search the DN in the temporary set ...
		if(current_SRTNodes_dn.count(imm_dnName)>0)
		{
			// the current CP node is already represented in IMM. We must only remove the DN from the helper SET
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: there's already an SRTNode in IMM for CP Node <%s>"), FX_NAME, cpNodeItem->node_name);
			current_SRTNodes_dn.erase(imm_dnName);
		}
		else
		{
			// we have to create a new SRTNode to represent the current CP node
			immDHobj.set_state(cpNodeItem->node_state);
			immDHobj.set_side(cpNodeItem->node_side);
			char cp_system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX] = {0};
			ACS_DSD_ConfigurationHelper::system_id_to_system_name(cpNodeItem->system_id, cp_system_name, acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX);
			immDHobj.set_node(cp_system_name);

			// sometimes IMM is temporary unavailable for a while, so try more times ...
			bool srtNode_created = false;
			for(int i = 0; i < IMM_MAX_TEMP_FAILURES; ++i)
			{
				int create_node_res = immDHobj.addNodeInfo(cpNodeItem->node_name);
				if(create_node_res < 0)
				{
					int imm_internal_error_code = imm_shared_objects_conn_ptr->getInternalLastError();
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: failed SRTNode creation for CP Node < %s >. Return code is < %d >, IMM error code == %d"), FX_NAME, cpNodeItem->node_name, create_node_res, imm_internal_error_code);
				}
				else
				{
					// OK, the IMM SRTNode has been created
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: SRTNode for CP Node < %s > successfully created."), FX_NAME, cpNodeItem->node_name);

					// update counter of added nodes
					++n_nodes_added;

					// set operation success and exit for loop
					srtNode_created = true;
					break;
				}

				// wait a second before retry
				ACE_OS::sleep(1);
			}

			if(!srtNode_created)
				return CPNM_IMM_ERROR;
		}
	}

	// STEP 3: now we must delete all SRTNodes representing CP nodes that aren't present in the internal collection
	for(std::set<std::string>::iterator it = current_SRTNodes_dn.begin(); it != current_SRTNodes_dn.end(); ++it)
	{
		const char * node_dn = (*it).c_str();

		// we need to get the node name
		char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX] = {0};

		// let's perform a check to be sure that the DN is valid
		if(!strstr(node_dn, ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME))
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: the DN <%s> doesn't identify an SRTNode object ! (check 1)"), FX_NAME, node_dn);
			continue;
		}

		// Ok, now search for the characters "=" and ",", that delimit the node name
		const char *pS = 0;
		const char *pE = 0;
		pS = strchr(node_dn,'=');
		pE = strchr(node_dn,',');
		int name_len = pE - pS - 1;
		//ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT(" pS == %p, pE == %p, NAME_LEN = %d"), pS, pE, name_len);
		if(!pS || !pE ||  (name_len <= 0) || (name_len > acs_dsd::CONFIG_NODE_NAME_SIZE_MAX - 1 ))
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: the DN <%s> doesn't identify a valid SRTNode object ! (check 2). < pS == %p, pE == %p, NAME_LEN = %d >"), FX_NAME, node_dn, pS, pE, name_len);
			continue;
		}

		// finally, get the node name
		strncpy(node_name, pS + 1, name_len);

		// get the system type (BC, CP, or AP)
		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t node_info_ptr;
		if(ACS_DSD_ConfigurationHelper::get_node_by_node_name(node_name, node_info_ptr) < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: the node name <%s>  is invalid !"),FX_NAME, node_name);
			continue;
		}
		acs_dsd::SystemTypeConstants system_type = ACS_DSD_ConfigurationHelper::system_id_to_system_type(node_info_ptr->system_id);

		// if the node isn't a BC or a CP, move forward
		if( (system_type != acs_dsd::SYSTEM_TYPE_BC)   && (system_type != acs_dsd::SYSTEM_TYPE_CP) )
			continue;

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: Found (in IMM) a CP SRTNode object not present in the CP internal collection ! NODE_NAME == <%s>"),FX_NAME, node_name);

		// the node is a BC node or a CP node. Delete it !
		int op_res = immDHobj.removeNodeInfoByDN((*it).c_str());
		if(!op_res)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: SRTNode having DN = <%s> successfully deleted from IMM !"), FX_NAME, (*it).c_str());
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: SRTNode having having DN = <%s> NOT deleted from IMM !  Error description: %s"),FX_NAME, (*it).c_str(), immDHobj.last_error_text());
			ret_val = CPNM_IMM_PARTIAL_UPDATE;
		}

		// update counter of removed services
		++n_nodes_removed;
	}

	return ret_val;
}

