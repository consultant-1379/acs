#include <memory>
#include "ace/Log_Msg.h"

#include "ACS_DSD_ServicePrimitive.h"
#include "ACS_DSD_Task_Base.h"
#include "ACS_DSD_ServiceHandler.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Task_Base * p_serviceHandlerTask; /// global object defined in ACS_DSD_Main.cpp
extern ACS_DSD_Logger	dsd_logger;

uint32_t ACS_DSD_ServiceHandler::_next_available_ID = 1;
ACE_Thread_Mutex ACS_DSD_ServiceHandler::_svcHandler_mutex;

ACS_DSD_ServiceHandler::ACS_DSD_ServiceHandler(ACS_DSD_ProtocolID prot_ID, ACE_Reactor * reactor)
{
	_protocol_id = prot_ID;
	_reactor = reactor;
	_object_ID =  ACS_DSD_ServiceHandler::sync_get_ID();
	_notification_to_service_handler_task = true;
}


ACE_HANDLE ACS_DSD_ServiceHandler::get_handle (void) const
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %u >. getting session handle ...  "), _object_ID);

	int handle_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1;
	acs_dsd::HANDLE handles [acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1];
	if(_session.get_handles(handles, handle_count) < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. Call 'ACS_DSD_Session::get_handles()' failed ! last_error_text == '%s' "), _object_ID, _session.last_error_text());
		return -1;	// an error occurred while getting handles
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %u >. ... got session handle (%d) "), _object_ID, handles[0]);
	return handles[0];	// the DSD API Session object has always one I/O handle !
}


int ACS_DSD_ServiceHandler::open()
{
	if(!_reactor)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceHandler (%p):  <this->_reactor == 0> !"), this);
		return ACS_DSD_SHDLC_INVALID_REACTOR;  // object is in a bad state
	}

	// register object as event handler for "data ready" events on the I/O handle that is associated to the DSD API Session object
	if(_reactor->register_handler(this, ACE_Event_Handler::READ_MASK) < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceHandler (%p):  error registering with reactor for READ events !"), this);
		return ACS_DSD_SHDLC_REGISTER_HANDLE_ERROR;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %u >. Service Handler (%p) successfully registered with the Service Handler Reactor"), _object_ID, this);
	return ACS_DSD_SHDLC_SUCCESS;	// ALL RIGTH
}


int ACS_DSD_ServiceHandler::close()
{
	if(_reactor)
	{
		int op_res = _reactor->remove_handler(this, ACE_Event_Handler::READ_MASK);
		if(!op_res)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceHandler (%p): unregistered from reactor"), this);
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceHandler: error occurred trying to unregister from reactor. Error code is < %d >"), op_res);
			return  ACS_DSD_SHDLC_UNREGISTER_HANDLE_ERROR;
		}

	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceHandler (%p): ACS_DSD_ServiceHandler::close() terminated with success"), this);
	return ACS_DSD_SHDLC_SUCCESS;
}



int ACS_DSD_ServiceHandler::handle_input (ACE_HANDLE /*fd*/)
{
	// If the "Notify Work to ServiceHandlerTask" feature is enabled for this service handler,
	// we notify the global ServiceHandlerTask object that this thread is handling a work item
	Thread_Work_Notifyer work_notifyer(notification_to_service_handler_task() ? p_serviceHandlerTask: 0);

	ACS_DSD_ServicePrimitive * received_primitive = 0;
	ACS_DSD_ServicePrimitive * response_primitive = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("CONN_ID = < %u >. Data ready to be read."), this->getID());

	// read the primitive sent by the peer
	if(recvPrimitive(received_primitive, DSD_SERVER_RECV_TIMEOUT_VALUE)< 0)
	{
		// an error occurred ! Connection has to be closed.
		on_connection_close_hook_func();
		return -1;
	}

	// be sure that memory allocated on the heap for the received_primitive be released on exit function
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_recv_prim(received_primitive);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Received primitive.  Primitive data : %s"), this->getID(), received_primitive->to_text().c_str());

	// check if the primitive belongs to the correct protocol
	if(received_primitive->get_protocol_id() != _protocol_id)
	{
		// a protocol error occurred ! Connection has to be closed.
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. Protocol Error - invalid primitive !"), this->getID(), received_primitive->to_text().c_str());
		on_connection_close_hook_func();
		return -1;
	}

	// process the primitive
	int ret_code;
	if((ret_code= handlePrimitive(received_primitive, response_primitive)) < 0)
	{
		// an error occurred while handling primitive. Connection has to be closed.
		on_connection_close_hook_func();
		return -1;
	}

	// be sure that memory (eventually) allocated on the heap for the response primitive be released on exit function
	std::auto_ptr<ACS_DSD_ServicePrimitive> auto_ptr_resp_prim(response_primitive);

	// check the result of primitive processing and send a response, if requested by the protocol
	switch(ret_code)
	{
		case ACS_DSD_PROT_SHDLR_OK_NO_RESP:
		case ACS_DSD_PROT_SHDLR_OK_CLOSE_NO_RESP:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed ! No response"), this->getID());
			break;
		case ACS_DSD_PROT_SHLDR_OK_RESP_ALREADY_SENT:
		case ACS_DSD_PROT_SHLDR_OK_CLOSE_RESP_ALREADY_SENT:
			break;
		case ACS_DSD_PROT_SHDLR_OK_WITH_RESP:
		case ACS_DSD_PROT_SHDLR_OK_CLOSE_WITH_RESP:
			if(response_primitive)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The primitive has been processed producing response primitive: %s"), this->getID(), response_primitive->to_text().c_str());
				if (sendPrimitive(response_primitive, DSD_SERVER_SEND_TIMEOUT_VALUE)==0) {
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. The response primitive has been sent !"), this->getID());
				} else {
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. Error in sending response primitive: <%s> !"), this->getID(), (this->_session).last_error_text());
					on_connection_close_hook_func();
					return -1; // close connection
				}
			}
			else{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. Cannot send reply: The pointer to response primitive is NULL"), this->getID());
				on_connection_close_hook_func();
				return -1;	// very strange ! This shouldn't occur, close connection !
			}
			break;

		default:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >. handlePrimitive() returned unexpected code <%d>"), this->getID(),ret_code );
			on_connection_close_hook_func();
			return -1;	// very strange ! This shouldn't occur, close connection !!
		break;
	}

	// check if we have to close connection
	if( (ret_code == ACS_DSD_PROT_SHDLR_OK_CLOSE_NO_RESP) || (ret_code == ACS_DSD_PROT_SHDLR_OK_CLOSE_WITH_RESP) || (ret_code == ACS_DSD_PROT_SHLDR_OK_CLOSE_RESP_ALREADY_SENT))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Protocol requires end of connection. DSD Server is going to end connection  !"), this->getID());
		on_connection_close_hook_func();
		return -1;	// an explicit 'connection close' has been requested by the protocol : "handlePrimive() method told us to close connection  !
	}

	// connection has to be kept alive
	return 0;
}


int  ACS_DSD_ServiceHandler::handle_close(ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CONN_ID = < %u >. Closing connection !"), this->getID());
	delete this;	// commit suicide, because this service handler has terminated it's work
	return 0;
}


int  ACS_DSD_ServiceHandler::handlePrimitive(const ACS_DSD_ServicePrimitive * primitive, ACS_DSD_ServicePrimitive * & response)
{
	if(!primitive)
		return -1;		// very strange !!

	int op_res = primitive->process(response, const_cast<ACS_DSD_ServiceHandler*>(this));
	if(op_res < 0){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u >.  Error processing primitive. Descr:  < %s >"), this->getID(), primitive->process_error_descr().c_str());
		return ACS_DSD_PROT_SHDLR_ERROR;
	}

	switch(op_res)
	{
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE:
			return ACS_DSD_PROT_SHDLR_OK_WITH_RESP;
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE:
			return ACS_DSD_PROT_SHDLR_OK_NO_RESP;
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT:
			return ACS_DSD_PROT_SHLDR_OK_RESP_ALREADY_SENT;
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_CLOSE_WITH_RESPONSE:
			return ACS_DSD_PROT_SHDLR_OK_CLOSE_WITH_RESP;
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_CLOSE_NO_RESPONSE:
			return ACS_DSD_PROT_SHDLR_OK_CLOSE_NO_RESP;
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_OK_CLOSE_RESPONSE_ALREADY_SENT:
			return ACS_DSD_PROT_SHLDR_OK_CLOSE_RESP_ALREADY_SENT;
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_ERROR:
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_MEMORY_ERROR:
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_IMM_ERROR:
		case ACS_DSD_ServicePrimitive::ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR:
			return ACS_DSD_PROT_SHDLR_ERROR;
		default:
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CONN_ID = < %u > processing primitive return code unexpected! return code = < %d >"), this->getID(), op_res);
		break;
	}
	return ACS_DSD_PROT_SHDLR_ERROR;
}

uint32_t ACS_DSD_ServiceHandler::sync_get_ID()
{
	ACE_Guard<ACE_Thread_Mutex> guard(ACS_DSD_ServiceHandler::_svcHandler_mutex);
	return _next_available_ID++;
}
