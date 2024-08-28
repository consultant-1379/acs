#include <string.h>
#include "ace/Log_Msg.h"
#include <typeinfo>
#include <memory>

#include "ACS_DSD_PrimitiveFactory.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_CpProt_UnknownPrimitive.h"
#include "ACS_DSD_Task_Base.h"
#include "ACS_DSD_CpProt_SvcHandler.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_PrimitiveFactory dsd_primitive_factory;   /// this object is defined in ACS_DSD_main.cpp
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_Task_Base * p_serviceHandlerTask;         /// global object defined in ACS_DSD_Main.cpp

ACS_DSD_CpProt_SvcHandler::ACS_DSD_CpProt_SvcHandler(ACE_Reactor * reactor): ACS_DSD_ServiceHandler(PROTOCOL_CPAP, reactor)
{
	_linked_to_cp_nodes_manager = false;
	_last_received_prim_id = -1;
	_last_received_prim_vers = -1;
	bzero(_remote_node_name, sizeof(_remote_node_name));
};

int ACS_DSD_CpProt_SvcHandler::recvPrimitive(ACS_DSD_ServicePrimitive * & receivedPrimitive, uint32_t timeout_ms)
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
			return CPPROT_SVC_RECEIVE_PEER_CLOSED;
		}
		else { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Error receiving data (with PDH). bytes_received = %d . Error description : < %s >!"), this->getID(), bytes_received, _session.last_error_text()); }

		// we must anyway terminate this connection
		return CPPROT_SVC_RECEIVE_ERROR;
	}


	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %d >. (%d) bytes received  !"), this->getID(), bytes_received);

	// cache the primitive id and primitive version (useful in case of error, to give such information to other class methods)
	_last_received_prim_id = pdh.primitive_id();
	_last_received_prim_vers = pdh.primitive_version();

	// create the ACS_DSD_ServicePrimitive object and fill it with primitive data received from the peer
	ACS_DSD_ServicePrimitive * pPrimObj = 0;
	ACS_DSD_PrimitiveFactory::PF_Const  op_res = dsd_primitive_factory.build_primitive(PROTOCOL_CPAP, pdh.primitive_version(),pdh.primitive_id(), pPrimObj);
	if(op_res == ACS_DSD_PrimitiveFactory::PF_BUILD_PRIMITIVE_OK)
	{
		// create DSD primitive skeleton
		CpProt_Primitive * p_CpProt_primitive = dynamic_cast<CpProt_Primitive *>(pPrimObj);
		if(! p_CpProt_primitive)
		{
			// Very strange !!!  We never should be here ! We received a primitive that doesn't belong to CP-AP protocol
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Received an invalid primitive object (not belonging to CP-AP protocol !"), this->getID());
			delete pPrimObj;										// release memory allocated for the primitive
			return CPPROT_SVC_INTERNAL_ERROR;						// protocol error !
		}

		// and now complete the DSD primitive construction
		p_CpProt_primitive->build_from_primitive_data_handler(pdh);

		// return the created primitive object to the caller (using the output parameter <recivedPrimitive>. Remember to release memory when done !
		receivedPrimitive = p_CpProt_primitive;
	}
	else if(op_res == ACS_DSD_PrimitiveFactory::PF_INVALID_PRIMITIVE)
	{
		// we received an invalid primitive
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Received an invalid primitive ID : < %d > !"), this->getID(), pdh.primitive_id());
		return CPPROT_SVC_INVALID_PRIMITIVE;
	}
	else if( (op_res == ACS_DSD_PrimitiveFactory::PF_INVALID_VERSION)  || (op_res == ACS_DSD_PrimitiveFactory::PF_PRIMITIVE_NOT_IMPLEMENTED))
	{
		// we receive a valid primitive, but not implemented in the requested version
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Received the primitive < %d >, NOT implemented in the requested version < %d > !"), this->getID(), _last_received_prim_id, _last_received_prim_vers);
		return CPPROT_SVC_UNIMPLEMENTED_PRIMITIVE;
	}
	else /* ALL OTHER CASES */
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. ACS_DSD_PrimitiveFactory::build_primitive() failed. RETCODE is < %d > !"), this->getID(),op_res);
		return CPPROT_SVC_GENERIC_ERROR; /*DSD Generic Error*/
	}

	return CPPROT_SVC_OK;  /* ALL RIGTH*/
};


int ACS_DSD_CpProt_SvcHandler::sendPrimitive(const ACS_DSD_ServicePrimitive * primitiveToSend, uint32_t timeout_ms)
{
	if(!primitiveToSend)
		return CPPROT_SVC_INVALID_PRIMITIVE;

	// pack the DSD Primitive in a DSD API PrimitiveDataHandler object
	const CpProt_Primitive * pPrimObj =  dynamic_cast<const CpProt_Primitive *> (primitiveToSend);
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
				return (errno_save == ETIME) ? CPPROT_SVC_SEND_TIMEOUT : CPPROT_SVC_SEND_ERROR;
			}
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %d >. (%d)  bytes sent !"), this->getID(), bytes_transferred);
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. pack_into_primitive_data_handler() error ! retcode is < %d >"), this->getID(), op_res);
			return CPPROT_SVC_INTERNAL_ERROR;
		}
	}
	else
		return CPPROT_SVC_INVALID_PRIMITIVE;

	return CPPROT_SVC_OK;
};


int  ACS_DSD_CpProt_SvcHandler::handlePrimitive(const ACS_DSD_ServicePrimitive * primitive, ACS_DSD_ServicePrimitive * & response)
{
	int retval;

	if(!primitive)
		return CPPROT_SVC_HDLPRIM_ERROR;		// very strange !!

	int op_res = primitive->process(response, const_cast<ACS_DSD_CpProt_SvcHandler*>(this));
	if(op_res < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Error processing primitive. Descr  < %s > !"), this->getID(), primitive->process_error_descr().c_str());
		retval = CPPROT_SVC_HDLPRIM_PROCESS_ERROR;
	}
	else if(op_res == ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE)
		retval = CPPROT_SVC_HDLPRIM_OK_WITH_RESP;
	else if(op_res == ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE)
		retval = CPPROT_SVC_HDLPRIM_OK_NO_RESP;
	else if(op_res == ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT)
		retval = CPPROT_SVC_HDLPRIM_OK_RESP_ALREADY_SENT;
	else /* this path should never be reached. To be sure, try to handle this situation */
		retval = (response ? CPPROT_SVC_HDLPRIM_OK_WITH_RESP : CPPROT_SVC_HDLPRIM_OK_NO_RESP);

	return retval;
}


int ACS_DSD_CpProt_SvcHandler::handle_input (ACE_HANDLE /*fd*/)
{
	// notify the global ServiceHandlerTask object that this thread is handling a work item
	Thread_Work_Notifyer work_notifyer(p_serviceHandlerTask);

	ACS_DSD_ServicePrimitive * received_primitive = 0;
	ACS_DSD_ServicePrimitive * response_primitive = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %u >. Data ready to be read."), this->getID());

	// read the DSD primitive sent by the peer and check the result
	int op_res = recvPrimitive(received_primitive, DSD_SERVER_RECV_TIMEOUT_VALUE);
	switch(op_res)
	{
	case CPPROT_SVC_OK:
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Received primitive. Primitive data : %s"), this->getID(), received_primitive->to_text().c_str());
		break;
	case CPPROT_SVC_INVALID_PRIMITIVE:
	case CPPROT_SVC_UNIMPLEMENTED_PRIMITIVE:
		{
			// we must reply with an "Unknown reply" primitive
			CpProt_unknown_reply  unknown_reply_prim;
			unknown_reply_prim.set_Unknown_field(_last_received_prim_id);

			// send the primitive to the peer and exit method
			if(sendPrimitive(&unknown_reply_prim, DSD_SERVER_SEND_TIMEOUT_VALUE)==0)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The 'Unknown reply' primitive has been sent !"), this->getID());
				return 0;
			}
			else
			{
				// error sending Unknown reply primitive. Close the connection !
				on_connection_close_hook_func();
				return -1;
			}
		}
		break;
	default:
		on_connection_close_hook_func();
		return -1;
	}

	// take care of memory deallocation for received primitive
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(received_primitive);

	// let's check if the primitive belongs to the correct protocol
	if((received_primitive->get_protocol_id() != _protocol_id))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. Protocol Error - invalid primitive !"), this->getID());
		on_connection_close_hook_func();
		return -2;
	}

	// process the primitive
	int hdlprim_res = handlePrimitive(received_primitive, response_primitive);
	if(hdlprim_res < 0)
	{
		on_connection_close_hook_func();
		return -3;
	}

	// take care of memory deallocation fro response primitive
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_resp_prim(response_primitive);

	// send the response primitive to the peer, if a response has to be sent and if that response has not already been sent by the handlePrimitive() method
	if(hdlprim_res == CPPROT_SVC_HDLPRIM_OK_WITH_RESP)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"), this->getID(), response_primitive->to_text().c_str());
		if (sendPrimitive(response_primitive, DSD_SERVER_SEND_TIMEOUT_VALUE)==0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >.  The response primitive has been sent !"), this->getID());
		else
		{
			on_connection_close_hook_func();
			return -1;	// error sending response
		}
	}
	else if(hdlprim_res == CPPROT_SVC_HDLPRIM_OK_NO_RESP)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >.  The primitive has been processed ! No response"), this->getID());
	else
	{
		; // the response has been sent by the process() method of the primitive
	}

	return  0;
}


int ACS_DSD_CpProt_SvcHandler::on_connection_close_hook_func()
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Preparing to close (CP-AP) connection !"), this->getID());

	if(_linked_to_cp_nodes_manager)
	{
		// remove the connection from the CPNodesManager
		ACS_DSD_CpNodesManager::OP_Result op_res = cpNodesManager.remove_cp_conn(_remote_node_name, this);
		if((op_res < 0) && (op_res != ACS_DSD_CpNodesManager::CPNM_CONN_NOT_FOUND))
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. Failure removing CP connection. ACS_DSD_CpNodesManager::remove_cp_conn() error. ERROR_CODE  = < %d >"), this->getID(), op_res);
		}
		else
		{
			if(op_res == ACS_DSD_CpNodesManager::CPNM_CONN_NOT_FOUND)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("CONN_ID = < %u >. CP connection NOT found !"), this->getID());

			_linked_to_cp_nodes_manager = false;
		}
	}

	return 0;
}


int ACS_DSD_CpProt_SvcHandler::handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Closing connection !"), this->getID());

	if(_linked_to_cp_nodes_manager)
	{
		// remove the connection from the CPNodesManager
		ACS_DSD_CpNodesManager::OP_Result op_res = cpNodesManager.remove_cp_conn(_remote_node_name, this);
		if(op_res == ACS_DSD_CpNodesManager::CPNM_CONN_NOT_FOUND)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("CONN_ID = < %u >. CP connection NOT found !"), this->getID());
		}
		else if(op_res < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. Failure removing CP connection. ACS_DSD_CpNodesManager::remove_cp_conn() error. ERROR_CODE  = < %d >"), this->getID(), op_res);
		}
	}

	// commit suicide, because this service handler has terminated it's work
	delete this;
	return 0;
}


void ACS_DSD_CpProt_SvcHandler::set_remote_node_name(char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX])
{
	strncpy(_remote_node_name, node_name,acs_dsd::CONFIG_NODE_NAME_SIZE_MAX);
}
