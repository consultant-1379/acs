#include <typeinfo>
#include "ace/Log_Msg.h"

#include "ACS_DSD_PrimitiveFactory.h"
#include "ACS_DSD_AppProt_Primitive.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_AppProt_SvcHandler.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


extern ACS_DSD_PrimitiveFactory dsd_primitive_factory;		/// this object is defined in ACS_DSD_main.cpp
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger dsd_logger;

ACS_DSD_AppProt_SvcHandler::ACS_DSD_AppProt_SvcHandler(ACE_Reactor * reactor): ACS_DSD_ServiceHandler(PROTOCOL_SVRAPP, reactor)
{
};

int ACS_DSD_AppProt_SvcHandler::recvPrimitive(ACS_DSD_ServicePrimitive * & receivedPrimitive, uint32_t timeout_ms)
{
	// Read the DSD primitive using DSD API
	ACS_DSD_PrimitiveDataHandler<> pdh(_session);
	ssize_t bytes_received;

	if(timeout_ms == 0)
		bytes_received = pdh.recv_primitive();	// no timeout
	else{
		ACE_Time_Value timeout(0, 1000 * timeout_ms);
		bytes_received = pdh.recv_primitive(& timeout);
	}

	if(bytes_received <= 0)
	{
		// check if peer closed the connection or another error occurred
		if((bytes_received == 0) || (bytes_received == acs_dsd::ERR_PEER_CLOSED_CONNECTION)){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %d >. Peer closed connection !"), this->getID());
			return APPPROT_SVC_RECEIVE_PEER_CLOSED;
		}
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Error receiving data (with PDH). bytes_received = %d !"), this->getID(), bytes_received);

		// we must anyway terminate this connection
		return APPPROT_SVC_RECEIVE_ERROR;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %d >. (%d) bytes received  !"), this->getID(), bytes_received);

	// create the ACS_DSD_ServicePrimitive object and fill it with primitive data received from the peer
	ACS_DSD_ServicePrimitive * pPrimObj = 0;
	ACS_DSD_PrimitiveFactory::PF_Const  op_res = dsd_primitive_factory.build_primitive(PROTOCOL_SVRAPP, pdh.primitive_version(),pdh.primitive_id(), pPrimObj);
	if(op_res==0)
	{
		// create DSD primitive skeleton
		AppProt_Primitive * p_AppProt_primitive = dynamic_cast<AppProt_Primitive *>(pPrimObj);
		if(! p_AppProt_primitive)
		{
			// We received a primitive that doesn't belong to SVR-APP protocol
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. Received an invalid primitive object (not belonging to SERVICE_APPLICATION protocol !"), this->getID());
			delete pPrimObj;	// release memory allocated for the primitive
			return APPPROT_SVC_INTERNAL_ERROR;			// protocol error !
		}

		// and now complete the DSD primitive construction
		p_AppProt_primitive->build_from_primitive_data_handler(pdh);

		// return the created primitive object to the caller (using the output parameter <recivedPrimitive>. Remember to release memory when done !
		receivedPrimitive = p_AppProt_primitive;
	}

	if(!pPrimObj)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. ACS_DSD_PrimitiveFactory::build_primitive() failed  error = < %d >!"), this->getID(),op_res);
		return APPPROT_SVC_GENERIC_ERROR; /*DSD Generic Error*/
	}

	return APPPROT_SVC_OK;  /* ALL RIGTH*/
};


int ACS_DSD_AppProt_SvcHandler::sendPrimitive(const ACS_DSD_ServicePrimitive * primitiveToSend, uint32_t timeout_ms)
{
	if(!primitiveToSend)
		return APPPROT_SVC_INVALID_PRIMITIVE;

	// pack the DSD Primitive in a DSD API PrimitiveDataHandler object
	const AppProt_Primitive * pPrimObj =  dynamic_cast<const AppProt_Primitive *> (primitiveToSend);
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
				return (errno_save == ETIME) ? APPPROT_SVC_SEND_TIMEOUT: APPPROT_SVC_SEND_ERROR;
			}
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %d >. (%d) bytes sent !"), this->getID(), bytes_transferred);
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %d >. pack_into_primitive_data_handler() error ! retcode is < %d >"), this->getID(), op_res);
			return APPPROT_SVC_INTERNAL_ERROR;
		}
	}
	else
		return APPPROT_SVC_INVALID_PRIMITIVE;

	return APPPROT_SVC_OK;
};


int ACS_DSD_AppProt_SvcHandler::on_connection_close_hook_func()
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Preparing to close (SVR-APP) connection ..."), this->getID());

	// remove notification subscription from cpNodesManager if any
	std::vector<uint32_t>::iterator it;
	for(it = _cpNotifiedList.begin(); it !=_cpNotifiedList.end(); it++)
		cpNodesManager.removeNotification(*it, this);

	_cpNotifiedList.clear();

	return 0;
}


int  ACS_DSD_AppProt_SvcHandler::handle_close(ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Closing connection !"), this->getID());

	// remove notification subscription from cpNodesManager if any
	std::vector<uint32_t>::iterator it;
	for(it = _cpNotifiedList.begin(); it !=_cpNotifiedList.end(); it++)
		cpNodesManager.removeNotification(*it, this);

	delete this;	// commit suicide, because this service handler has terminated it's work
	return 0;
}
