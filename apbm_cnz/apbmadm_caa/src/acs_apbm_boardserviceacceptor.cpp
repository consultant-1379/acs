#include "acs_apbm_boardserviceacceptor.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_programmacros.h"
#include "acs_apbm_boardsessionhandler.h"
#include <memory>

acs_apbm_boardserviceacceptor::acs_apbm_boardserviceacceptor(acs_apbm_serverworkingset * server_working_set)
: _server_working_set(server_working_set)
{
	_service_acceptor_reactor = 0;
	_session_handler_reactor = 0;
	_sap_type = UNKNOWN_SAP;
	memset(_sap_id,'\0',sizeof(_sap_id));
}


acs_apbm_boardserviceacceptor::acs_apbm_boardserviceacceptor(ACE_Reactor * service_acceptor_reactor, ACE_Reactor * session_handler_reactor, SAP_Type sap_type, const char *sap_id, acs_apbm_serverworkingset * server_working_set)
: _server_working_set(server_working_set)
{
	_service_acceptor_reactor = service_acceptor_reactor;
	_session_handler_reactor = session_handler_reactor;
	_sap_type = sap_type;
	memset(_sap_id,'\0',sizeof(_sap_id));
	if(sap_id)
		strncpy(_sap_id, sap_id, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
}


acs_apbm_boardserviceacceptor::~acs_apbm_boardserviceacceptor()
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Destroying BOARD SERVICE ACCEPTOR object ! (this == %p)", this);
	close();
}


void acs_apbm_boardserviceacceptor::set_sap(acs_apbm_boardserviceacceptor::SAP_Type sap_type, const char *sap_id)
{
	_sap_type = sap_type;
	memset(_sap_id,'\0',sizeof(_sap_id));
	if(sap_id)
		strncpy(_sap_id, sap_id, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
}


acs_apbm_boardserviceacceptor::acs_apbm_boardserviceacceptor_constants acs_apbm_boardserviceacceptor::open(SAP_Type sap_type, const char * sap_id)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO,"Opening BOARD SERVICE ACCEPTOR object [sap_type == %d, sap_id == '%s'] (this == %p) ...", sap_type, sap_id, this);

	// check object state
	if(!_service_acceptor_reactor)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Open failed. Reason: (this->_service_acceptor_reactor == 0) ! (this == %p)", this);
		return ACS_APBM_BSA_INVALID_SERVICE_ACCEPTOR_REACTOR;
	}

	if(!_session_handler_reactor)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Open failed. Reason: (this->_session_acceptor_reactor == 0) ! (this == %p)", this);
		return ACS_APBM_BSA_INVALID_SESSION_HANDLER_REACTOR;
	}

	// check input parameters
	if(sap_type == UNKNOWN_SAP)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Open failed. Reason: invalid sap_type parameter (UNKNOWN_SAP) ! (this == %p)", this);
		return ACS_APBM_BSA_INVALID_SAP_TYPE;
	}

	if(!sap_id || !*sap_id)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Open failed. Reason: (invalid sap_id parameter) ! (this == %p)", this);
		return ACS_APBM_BSA_INVALID_SAP_ID;
	}

	// set the SAP to listen for connections
	_sap_type = sap_type;
	strncpy(_sap_id, sap_id, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);

	// for Unix domain sockets, remove the Unix domain socket before opening it
	if(sap_type == UNIX_SAP)
	{
		errno = 0;
		if(!unlink(_sap_id))
			ACS_APBM_LOG(LOG_LEVEL_INFO, "the Unix domain socket < %s >  has been successfully removed ! (this == %p)", _sap_id, this);
		else if(errno != ENOENT)
			ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "an error occurred trying to remove the Unix domain socket < %s > ! (this == %p)", _sap_id, this);
	}

	// start listening for connections on the specified SAP
	acs_dsd::ServiceModeConstants service_mode = ( (sap_type == INET_SAP) ? acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE : acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE );
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Start listening for connections on SAP < %s > ! (this == %p)", _sap_id, this);
	if(const int call_result = _acceptor.open(service_mode, _sap_id))
	{
		if (call_result < 0)
		{
			// a critical error occurred !
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Open failed. Reason: ACS_DSD_Server::open() failed. Description:  < %s > ! (this == %p)", _acceptor.last_error_text(), this);
			return ACS_APBM_BSA_ACCEPTOR_OPEN_ERROR;
		}

		// call_result > 0;  a not critical error occurred. Go ahead !
		ACS_APBM_LOG(LOG_LEVEL_WARN, "ACS_DSD_Server::open() returned code < %d >  ! Description:  < %s > ! (this == %p)", call_result, _acceptor.last_error_text(), this);
	}

	// get the socket handles manipulated by the ACS_DSD_Server object
	int handle_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1;
	acs_dsd::HANDLE sock_handles [acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1];
	if(_acceptor.get_handles(sock_handles, handle_count) < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Open failed. Reason: ACS_DSD_Server::get_handles() failed. Description:  < %s > ! (this == %p)", _acceptor.last_error_text(), this);
		return ACS_APBM_ACCEPTOR_GET_HANDLES_ERROR;	// an error occurred while getting handles
	}

	// now register with <this->_service_acceptor_reactor> to receive accept events occurring on the handles
	for(int i=0; i<handle_count; ++i)
	{
		if(_service_acceptor_reactor->register_handler(sock_handles[i],this, ACE_Event_Handler::ACCEPT_MASK) < 0)
		{
			// cancel previous registrations
			for(int j = 0; j < i; ++j)
				_service_acceptor_reactor->remove_handler(sock_handles[j], ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL );

			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Open failed. Reason: error registering BOARD SERVICE ACCEPTOR with <this->_service_acceptor_reactor> to receive accept events occurring on handle < %d > ! (this == %u)", sock_handles[i], this);
			return ACS_APBM_REGISTER_HANDLE_ERROR;
		}

		ACS_APBM_LOG(LOG_LEVEL_INFO, "BOARD SERVICE ACCEPTOR object successfully registered with the Reactor (this->_service_acceptor_reactor) to receive accept events occurring on handle < %u > ! (this == %p)", sock_handles[i], this);
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "BOARD SERVICE ACCEPTOR successfully opened ! (this == %p)", this);
	return ACS_APBM_BSA_SUCCESS;
}


int acs_apbm_boardserviceacceptor::close()
{
	int ret_code = 0;
	ACS_APBM_LOG(LOG_LEVEL_INFO,"Closing BOARD SERVICE ACCEPTOR object [sap_type == %d, sap_id == '%s'] (this == %p) ...", _sap_type, _sap_id, this);

	if(!is_open())
		return ret_code;	// the object is yet closed

	// unregister from _service_acceptor_reactor>
	ret_code = remove_handlers_from_reactor();
	if(ret_code < 0)
		ACS_APBM_LOG(LOG_LEVEL_WARN, "failure trying to unregister from (this->_service_acceptor_reactor). Details: (call_result == %d) ! (this == %p)", ret_code, this);

	// stop listening for connections
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Stop listening for connections on SAP < %s > ! (this == %p)", _sap_id, this);
	if(_acceptor.close() < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "ACS_DSD_Server::close() failed. Description:  < %s > ! (this == %p)", _acceptor.last_error_text(), this);
		ret_code = -1;
	}

	return ret_code;
}


int acs_apbm_boardserviceacceptor::handle_input (ACE_HANDLE /*fd*/)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"connection ready to be accepted on SAP < %s > ! (this == %p)",_sap_id, this);

	// create a session handler object to handle the new connection
	acs_apbm_boardsessionhandler * session_handler =  new (std::nothrow) acs_apbm_boardsessionhandler(_session_handler_reactor, _server_working_set);
	if(!session_handler)
		return -1;	// no memory available on heap. Stop handling events.

	std::auto_ptr<acs_apbm_boardsessionhandler> p(session_handler);

	// accept the connection
	if(_acceptor.accept(session_handler->peer()) < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "ACS_DSD_Server::accept() failed. Description:  < %s > ! (this == %p)", _acceptor.last_error_text(), this);
		return 0; 	// an error occurred accepting connections, may be a temporary failure. Let's continue work
	}

	// report the connection in the log file
	if(_sap_type ==  INET_SAP)
	{
		char ip_addr_str[INET_ADDRSTRLEN] = {0};
		acs_apbm_boardserviceacceptor::get_ip_addr_str(session_handler->peer().get_remote_ip4_address(), ip_addr_str, INET_ADDRSTRLEN);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "connection established (with host < %s >). CONN_ID = < %u > ! (this == %p)", ip_addr_str, session_handler->getID(), this);
	}
	else
		ACS_APBM_LOG(LOG_LEVEL_INFO, "connection established. CONN_ID = < %u > ! (this == %p)", session_handler->getID(), this);

	// activate the session handler (register it with <this->_session_handler_reactor>)
	if(session_handler->open() < 0)
	{
		// an error occurred during registration phase ! May be a temporary failure. Continue the work (but the new connection will be closed)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "acs_apbm_sessionhandler::open() failed ! The connection CONN_ID = < %u > will be closed !", session_handler->getID());
		return 0;
	}

	// the object pointet by <session_handler> must continue to live after the end of the function
	p.release();

	return 0;
}


int  acs_apbm_boardserviceacceptor::handle_close (ACE_HANDLE handle, ACE_Reactor_Mask /*close_mask*/)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "handle_close() invoked on BOARD SERVICE ACCEPTOR object. HANDLE == %d ! (this == %p)", handle, this);
	return (close() == 0 ? 0: -1);
}


int acs_apbm_boardserviceacceptor::remove_handlers_from_reactor()
{
	int ret_code = 0;

	// we need the socket handles manipulated by the ACS_DSD_Server object (<this->_acceptor>)
	int handle_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1;
	acs_dsd::HANDLE sock_handles [acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1];
	if(_acceptor.get_handles(sock_handles, handle_count) < 0)
		return -1;	// an error occurred while getting handles

	// now remove <this> as event handler for the socket handles
	for(int i=0; i<handle_count; ++i)
	{
		if(_service_acceptor_reactor->remove_handler(sock_handles[i], ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL) < 0)
			ret_code = -1;   		// an error occurred during remove_handler. Go ahead !
	}

	return ret_code;
}


int acs_apbm_boardserviceacceptor::get_ip_addr_str(uint32_t ip_addr, char *buff, int len)
{
	if(*buff || (len < INET_ADDRSTRLEN))
		return -1;

	in_addr inet_addr;
	inet_addr.s_addr = ip_addr;

	const char *p = inet_ntop(AF_INET, & inet_addr, buff, len);
	return (p ? 0: -1);
}
