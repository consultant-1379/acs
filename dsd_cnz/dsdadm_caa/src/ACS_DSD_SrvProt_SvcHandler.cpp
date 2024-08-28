#include <string.h>
#include "ace/Log_Msg.h"
#include <typeinfo>
#include <memory>

#include "ACS_DSD_SrvProt_Primitive.h"
#include "ACS_DSD_PrimitiveFactory.h"
#include "ACS_DSD_RemoteApNodesManager.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_PrimitiveFactory dsd_primitive_factory;		/// this object is defined in ACS_DSD_main.cpp
extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_RemoteApNodesManager remote_AP_nodes_manager;


ACS_DSD_SrvProt_SvcHandler::ACS_DSD_SrvProt_SvcHandler(ACE_Reactor * reactor)
: ACS_DSD_ServiceHandler(PROTOCOL_SRVSRV, reactor),_last_received_prim_id(-1),_last_received_prim_vers(-1),
  _linked_to_remote_ap_nodes_manager(false), _remote_ap_system_id()
{
	bzero(_remote_node_name, sizeof(_remote_node_name));
};

int ACS_DSD_SrvProt_SvcHandler::recvPrimitive(ACS_DSD_ServicePrimitive * & receivedPrimitive, uint32_t timeout_ms)
{
	// read the DSD primitive using DSD API library
	ACS_DSD_PrimitiveDataHandler<> pdh(_session);
	ssize_t bytes_received;
	if(timeout_ms == 0)
		bytes_received = pdh.recv_primitive();	// no timeout
	else
	{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		bytes_received = pdh.recv_primitive(& timeout);
	}

	// check result
	if(bytes_received <= 0)
	{
		if((bytes_received == 0) || (bytes_received == acs_dsd::ERR_PEER_CLOSED_CONNECTION))
		{
			// peer closed connection
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %d >. Peer closed connection ! bytes_received = %d . Error description : < %s >!"), this->getID(), bytes_received, _session.last_error_text());
			return SRVPROT_SVC_RECEIVE_PEER_CLOSED;
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Error receiving data (with PDH). bytes_received = %d . Error description : < %s >!"), this->getID(), bytes_received, _session.last_error_text());
		}

		// we must anyway terminate this connection
		return SRVPROT_SVC_RECEIVE_ERROR;
	}


	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %d >. (%d) bytes received !"), this->getID(), bytes_received);

	// cache the primitive id and primitive version (useful in case of error, to give such information to other class methods)
	_last_received_prim_id = pdh.primitive_id();
	_last_received_prim_vers = pdh.primitive_version();

	// create the ACS_DSD_ServicePrimitive object and fill it with primitive data received from the peer
	ACS_DSD_ServicePrimitive * pPrimObj = 0;
	ACS_DSD_PrimitiveFactory::PF_Const  op_res = dsd_primitive_factory.build_primitive(PROTOCOL_SRVSRV, pdh.primitive_version(),pdh.primitive_id(), pPrimObj);
	if(op_res == ACS_DSD_PrimitiveFactory::PF_BUILD_PRIMITIVE_OK)
	{
		// create DSD primitive skeleton
		SrvProt_Primitive * p_SrvProt_primitive = dynamic_cast<SrvProt_Primitive *>(pPrimObj);
		if(! p_SrvProt_primitive)
		{
			// Very strange !!!  We never should be here ! We received a primitive that doesn't belong to CP-AP protocol
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Received an invalid primitive object (not belonging to CP-AP protocol !"), this->getID());
			delete pPrimObj;										// release memory allocated for the primitive
			return SRVPROT_SVC_INTERNAL_ERROR;						// protocol error !
		}

		// and now complete the DSD primitive construction
		p_SrvProt_primitive->build_from_primitive_data_handler(pdh);

		// return the created primitive object to the caller (using the output parameter <recivedPrimitive>. Remember to release memory when done !
		receivedPrimitive = p_SrvProt_primitive;
	}
	else if(op_res == ACS_DSD_PrimitiveFactory::PF_INVALID_PRIMITIVE)
	{
		// we received an invalid primitive
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Received an invalid primitive ID : < %d > !"), this->getID(), pdh.primitive_id());
		return SRVPROT_SVC_INVALID_PRIMITIVE;
	}
	else if( (op_res == ACS_DSD_PrimitiveFactory::PF_INVALID_VERSION)  || (op_res == ACS_DSD_PrimitiveFactory::PF_PRIMITIVE_NOT_IMPLEMENTED))
	{
		// we receive a valid primitive, but not implemented in the requested version
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Received the primitive < %d >, NOT implemented in the requested version < %d > !"), this->getID(), _last_received_prim_id, _last_received_prim_vers);
		return SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE;
	}
	else /* ALL OTHER CASES */
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. ACS_DSD_PrimitiveFactory::build_primitive() failed. RETCODE is < %d > !"), this->getID(),op_res);
		return SRVPROT_SVC_GENERIC_ERROR; /*DSD Generic Error*/
	}

	return SRVPROT_SVC_OK;  /* ALL RIGTH*/
};


int ACS_DSD_SrvProt_SvcHandler::sendPrimitive(const ACS_DSD_ServicePrimitive * primitiveToSend, uint32_t timeout_ms)
{
	if(!primitiveToSend)
		return SRVPROT_SVC_INVALID_PRIMITIVE;

	// pack the DSD Primitive in a DSD API PrimitiveDataHandler object
	const SrvProt_Primitive * pPrimObj =  dynamic_cast<const SrvProt_Primitive *> (primitiveToSend);
	if(pPrimObj)
	{
		ACS_DSD_PrimitiveDataHandler<> pdh(_session);
		int op_res = pPrimObj->pack_into_primitive_data_handler(pdh);
		if(op_res == 0)
		{
			// send the primitive to the peer
			ssize_t bytes_transferred;
			if(timeout_ms)
			{
				ACE_Time_Value tv(0, 1000 * timeout_ms);
				bytes_transferred = pdh.send_primitive(&tv);
			}
			else
				bytes_transferred = pdh.send_primitive();

			if(bytes_transferred <= 0)
			{
				int errno_save = errno;
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Error sending data (with PDH). bytes_transferred = %d !"), this->getID(), bytes_transferred);
				return (errno_save == ETIME) ? SRVPROT_SVC_SEND_TIMEOUT: SRVPROT_SVC_SEND_ERROR;
			}
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %d >. (%d)  bytes sent !"), this->getID(), bytes_transferred);
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. pack_into_primitive_data_handler() error ! retcode is < %d >"), this->getID(), op_res);
			return SRVPROT_SVC_INTERNAL_ERROR;
		}
	}

	return (pPrimObj ? SRVPROT_SVC_OK: SRVPROT_SVC_INVALID_PRIMITIVE);
};


int ACS_DSD_SrvProt_SvcHandler::on_connection_close_hook_func () {
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_SrvProt_SvcHandler::%s: CONN_ID = < %u >. "
			"Preparing to close (SRV-SRV) connection !"),	__func__, getID());

	// If this object is linked to the Remote AP Nodes Manager, remove the item from the collection
	if (_linked_to_remote_ap_nodes_manager) {
		// Remove this connection item from the Remote AP Nodes Manager
		int call_result = remote_AP_nodes_manager.remove_remote_ap_node_connection(_remote_ap_system_id);

		if (call_result) {	//ERROR: removing the item from collection
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_SrvProt_SvcHandler::%s: CONN_ID = < %u >. "
					"Failure removing AP connection for system_id == %d. call_result == %d"),
					__func__, getID(), _remote_ap_system_id, call_result);
		}
		else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_SrvProt_SvcHandler::%s: CONN_ID = < %u >. "
					"Successfully removed AP connection for system_id == %d"), __func__, getID(), _remote_ap_system_id);
			_linked_to_remote_ap_nodes_manager = false;
		}
	}

	return 0;
}

int ACS_DSD_SrvProt_SvcHandler::handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/) {
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_SrvProt_SvcHandler::%s: CONN_ID = < %u >. "
			"Closing (SRV-SRV) connection !"),	__func__, getID());

	// If this object is linked to the Remote AP Nodes Manager, remove the item from the collection
	if (_linked_to_remote_ap_nodes_manager) {
		// Remove this connection item from the Remote AP Nodes Manager
		int call_result = remote_AP_nodes_manager.remove_remote_ap_node_connection(_remote_ap_system_id);

		if (call_result) {	//ERROR: removing the item from collection
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_SrvProt_SvcHandler::%s: CONN_ID = < %u >. "
					"Failure removing AP connection for system_id == %d. call_result == %d"),
					__func__, getID(), _remote_ap_system_id, call_result);
		}
		else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_SrvProt_SvcHandler::%s: CONN_ID = < %u >. "
					"Successfully removed AP connection for system_id == %d"), __func__, getID(), _remote_ap_system_id);
			_linked_to_remote_ap_nodes_manager = false;
		}
	}

	// commit suicide, because this service handler has terminated it's work
	delete this;

	return 0;
}


void ACS_DSD_SrvProt_SvcHandler::set_remote_node_name(char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX])
{
	strncpy(_remote_node_name, node_name,acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);
}
