#ifndef ACS_DSD_SERVICEACCEPTOR_H_
#define ACS_DSD_SERVICEACCEPTOR_H_

/** @file ACS_DSD_ServiceAcceptor.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-10
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2010-11-10 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <errno.h>
#include <string.h>
#include <vector>
#include "ace/Event_Handler.h"
#include "ace/Reactor.h"

#include "ACS_DSD_Server.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Utils.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

class ACS_DSD_ServiceAcceptorTask;

/// global objects defined in ACS_DSD_Main.cpp
extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_ServiceAcceptorTask * p_serviceAcceptorTask;

/// This enumeration represents the SAP (Service Access Point) types
enum SAP_Type {ACS_DSD_UNIX_SAP, ACS_DSD_INET_SAP};

/** @class ACS_DSD_ServiceAcceptor ACS_DSD_ServiceAcceptor.h
 *	@brief Used to expose a DSD service on a Unix domain socket or on an Internet socket.
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-10
 *	@version 0.1
 *
 *	This template class is used to expose a DSD service on a Unix domain socket  or on an Internet socket. When a client connection is accepted,
 *	an instance of the template parameter class <ACS_DSD_ServiceHandler> is created to handle the connection. There are three client types:
 *		- CP/BC processes
 *		- AP internal processes
 *		- remote AP processes
 *
 */
template <typename SERVICE_HANDLER, SAP_Type saptype = ACS_DSD_UNIX_SAP>
class ACS_DSD_ServiceAcceptor : public ACE_Event_Handler
{
public:
	/// Constants used as return codes
	enum ACS_DSD_ServiceAcceptor_constants
	{
		ACS_DSD_SAC_SUCCESS = 0,
		ACS_DSD_SAC_INVALID_REACTOR = -1,
		ACS_DSD_SAC_INVALID_SAP = -2,
		ACS_DSD_SAC_ACCEPTOR_GET_HANDLES_ERROR = -3,
		ACS_DSD_SAC_REGISTER_HANDLE_ERROR = -4,
		ACS_DSD_SAC_ACCEPTOR_OPEN_ERROR = -5,
		ACS_DSD_SAC_REMOVE_HANDLERS_ERROR = -6,
		ACS_DSD_SAC_ACCEPTOR_CLOSE_ERROR = -7
	};

	/// Service Acceptor states
	enum ServiceAcceptor_State
	{
		SAC_STATE_CLOSED	=	0,
		SAC_STATE_OPEN		=	1
	};

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_DSD_ServiceAcceptor default constructor
	 *
	 *	ACS_DSD_ServiceAcceptor default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	ACS_DSD_ServiceAcceptor()
	: _object_name("anonymous"), _state(SAC_STATE_CLOSED), _reactor(0),_service_handler_reactor(0)
	{
		memset(_sap_id, 0, sizeof(_sap_id));
	};

	/** @brief ACS_DSD_ServiceAcceptor constructor
	 *
	 *	ACS_DSD_ServiceAcceptor constructor detailed description
	 *
	 *	@param[in] reactor reactor on which we register to be alerted (invoking the "handle_input" callback method) when a client connects
	 *  @param[in] sap_id  service access point (tcp port or unix socket path) on which we wait for connections
	 *	@remarks Remarks
	 */
	ACS_DSD_ServiceAcceptor(ACE_Reactor * reactor, const char *sap_id = 0, std::string object_name = "anonymous")
	: _object_name(object_name), _state(SAC_STATE_CLOSED), _reactor(reactor),_service_handler_reactor(0)
	{
		memset(_sap_id, 0, sizeof(_sap_id));
		if(sap_id) strncpy(_sap_id, sap_id, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
	};

	//==============//
	// Destructor   //
	//==============//

	/// ACS_DSD_ServiceAcceptor Destructor
	virtual ~ACS_DSD_ServiceAcceptor()
	{
		//remove_handlers_from_reactor();
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : destroying object  !"), _object_name.c_str());
		_acceptor.close();
	}

public:
	/** @brief open method
	 *
	 *	Initializes the object listening on the passed SAP and registering with the REACTOR
	 *
	 *	@param[in] sap_id  service access point (tcp port or unix socket path) on which we wait for connections
	 *	@remarks Remarks
	 */
	int open(const char * sap_id)
	{
		if(_state == SAC_STATE_OPEN)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor ( %s ) : open() terminated with success. Object was already open !"), _object_name.c_str());
			return ACS_DSD_SAC_SUCCESS;	 // object is already open
		}

		if(!_reactor)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor ( %s ) : <_reactor == 0> !"), _object_name.c_str());
			return ACS_DSD_SAC_INVALID_REACTOR;  // cannot proceed without a reactor
		}

		// set the SAP to listen on for connections
		if(sap_id)
			strncpy(_sap_id, sap_id, acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : invalid SAP < NULL >!"), _object_name.c_str());
			return 	ACS_DSD_SAC_INVALID_SAP; // invalid SAP
		}

		// if the SAP is a UNIX domain socket already existing, remove it before opening
		if(saptype == ACS_DSD_UNIX_SAP)
		{
			if(unlink(sap_id) == 0)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : Unix Socket < %s >  successfully removed !"), _object_name.c_str(), _sap_id);
			}
			else if(errno != ENOENT)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : error removing unix socket < %s >!"), _object_name.c_str(), _sap_id);
			}
		}

		// start listening for connections
		acs_dsd::ServiceModeConstants service_mode = ( (saptype == ACS_DSD_INET_SAP) ? acs_dsd::SERVICE_MODE_INET_SOCKET_PRIVATE : acs_dsd::SERVICE_MODE_UNIX_SOCKET_PRIVATE );
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : Start listening for connections on SAP < %s > !"), _object_name.c_str(), _sap_id);
		if(const int call_result = _acceptor.open(service_mode, _sap_id))
		{
			_last_error_descr = _acceptor.last_error_text();
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : ACS_DSD_Server::open() failed ! Description:  < %s > !"), _object_name.c_str(), _acceptor.last_error_text());
			_acceptor.close();
			memset(_sap_id, 0, sizeof(_sap_id));
			return ACS_DSD_SAC_ACCEPTOR_OPEN_ERROR;
		}

		// for UNIX domain socket SAPs, grant write access to others (in order to allow connections from no-root and no-privileged processes)
		if((saptype == ACS_DSD_UNIX_SAP) && (chmod(sap_id,0777) < 0))
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : Failure while setting file permission (0777) for file < %s > ! errno == %s !"), _object_name.c_str(), sap_id, errno);

		// get socket handles associated to internal acceptor
		int handle_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1;
		acs_dsd::HANDLE sock_handles [acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1];
		if(_acceptor.get_handles(sock_handles, handle_count) < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : ACS_DSD_Server::get_handles() failed ! Description:  < %s > !"), _object_name.c_str(), _acceptor.last_error_text());
			_acceptor.close();
			memset(_sap_id, 0, sizeof(_sap_id));
			return ACS_DSD_SAC_ACCEPTOR_GET_HANDLES_ERROR;
		}

		// now register <this> object with reactor, as event handler for accept events on the socket handles associated to acceptor
 		for(int i=0; i<handle_count; ++i)
 		{
 			if(_reactor->register_handler(sock_handles[i],this, ACE_Event_Handler::ACCEPT_MASK) < 0)
 			{
 				// try to rollback canceling previous successfully registrations
 				for(int j = 0; j < i; ++j)
 					_reactor->remove_handler(sock_handles[j], ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL );

 				// update internal list of registered socket handles
 				_registered_socket_handles.clear();

 				// close acceptor and clear internal SAP ID
 				_acceptor.close();
 				memset(_sap_id, 0, sizeof(_sap_id));

 				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : Error registering <this> with reactor for ACCEPT_MASK for handle < %u > !"), _object_name.c_str(),  sock_handles[i]);
 				return ACS_DSD_SAC_REGISTER_HANDLE_ERROR;
 			}

 			// update internal list of registered socket handles
 			_registered_socket_handles.push_back(sock_handles[i]);
 		}

 		_state = SAC_STATE_OPEN;

 		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : open() terminated with success"), _object_name.c_str());
 		return ACS_DSD_SAC_SUCCESS;	// ALL RIGTH
	}


	/** @brief set_reactor method
	 *
	 *	set the reactor used to dispatch the object instance when a client connection is ready to be accepted
	 *
	 *	@param[in] new_reactor Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_reactor(ACE_Reactor * new_reactor) { _reactor = new_reactor; }


	/** @brief set_service_handler_reactor method
	 *
	 *	set_service_handler_reactor method detailed description
	 *
	 *	@param[in] serv_handler_reactor Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_service_handler_reactor(ACE_Reactor * serv_handler_reactor) { _service_handler_reactor = serv_handler_reactor; }


	/** @brief get_state method
	 *
	 *	Return the state of ServiceAcceptor (i.e., if it's OPEN or CLOSED)
	 *
	 *	@return The state of the ServiceAcceptor
	 *	@remarks Remarks
	 */
	inline ServiceAcceptor_State get_state() { return _state;}


	/**	@brief handle_input method
	 *
	 *	Callback method invoked by the Reactor (_reactor attribute) when a client connection is ready to be accepted.
	 *	Accepts the connection and creates an object of type SERVICE_HANDLER to handle it.
	 *
	 *	@param[in] fd handle of the socket on which connection is waiting to be accepted
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int handle_input (ACE_HANDLE /*fd*/ = ACE_INVALID_HANDLE)
	{
		// Notify the global ServiceAcceptorrTask object that this thread is handling a work item
		Thread_Work_Notifyer work_notifyer(p_serviceAcceptorTask);

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : connection ready to be accepted on SAP < %s >!"), _object_name.c_str(), _sap_id);

		// create an object for handling the new service connection
		SERVICE_HANDLER * servHandler =  new (std::nothrow) SERVICE_HANDLER(_service_handler_reactor);
		if(!servHandler)
			return -1;	// no memory available on heap. Stop handling events.

		auto_ptr<SERVICE_HANDLER> p(servHandler);

		// accept the connection
		if(_acceptor.accept(servHandler->peer()) < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : ACS_DSD_Server::accept() failed ! Description:  < %s >"),_object_name.c_str(), _acceptor.last_error_text());
			return 0; 	// an error occurred accepting connections, may be a temporary failure. Let's continue work
		}

		// log the connection
		if(saptype ==  ACS_DSD_INET_SAP)
		{
			char ip_addr_str[INET_ADDRSTRLEN] = {0};
			ACS_DSD_Utils::get_ip_addr_str(servHandler->peer().get_remote_ip4_address(), ip_addr_str, INET_ADDRSTRLEN);
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : connection established (with host < %s >). CONN_ID = < %u >, Service Handler Address == < %p > !"), _object_name.c_str(), ip_addr_str, servHandler->getID(), servHandler);
		}
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : connection established. CONN_ID = < %u >, Service Handler Address == < %p > !"), _object_name.c_str(), servHandler->getID(), servHandler);

		// cache ID of the service handler before calling 'open()'
		uint32_t servHandlerID = servHandler->getID();

		// activate the handler for handling service requests (register it with the service handler reactor)
		if(servHandler->open() < 0)
		{
			// an error occurred during registration phase ! May be a temporary failure. Continue the work
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : ServiceHandler open() failure ! The connection CONN_ID = < %u > will be closed !"), _object_name.c_str(), servHandlerID);
			return 0;
		}

		// the object pointed by <servHandler> must continue to live after the end of the function
		p.release();

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : Service Handler for CONN_ID = < %u > successfully opened. "),_object_name.c_str(), servHandlerID);
		return 0;
	}

	/**	@brief handle_close method
	 *
	 *	Called when a <handle_*()> method returns -1 or when the <remove_handler> method is called on the Reactor.
	 *	The close_mask indicates which event has triggered the <handle_close> method callback on a particular handle.
	 *
	 *	@param[in] handle Description
	 *	@param[in] close_mask Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int  handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/)
	{
		// Notify the global ServiceAcceptorrTask object that this thread is handling a work item
		Thread_Work_Notifyer work_notifyer(p_serviceAcceptorTask);
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s): removing event handlers from reactor for SAP < %s > ..."), _object_name.c_str(), _sap_id);
		int call_result = remove_handlers_from_reactor();
		if(call_result == 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s): event handlers for SAP < %s > successfully removed from reactor !"), _object_name.c_str(), _sap_id);
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s): some errors occurred while removing from reactor event handlers for SAP < %s > ! Call 'remove_handlers_from_reactor()' returned %d"), _object_name.c_str(), _sap_id, call_result);

		return call_result;
	};

	/** @brief close method
	 *
	 *	close method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int close()
	{
		if(_state == SAC_STATE_CLOSED)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor ( %s ) : close() terminated with success. Object was already closed !"), _object_name.c_str());
			return ACS_DSD_SAC_SUCCESS;
		}

		int call_result = remove_handlers_from_reactor();
		if(call_result < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : close() failure ! Call 'remove_handlers_from_reactor()' returned %d"), _object_name.c_str(), call_result);
			return ACS_DSD_SAC_REMOVE_HANDLERS_ERROR;
		}

		call_result = _acceptor.close();
		if(call_result < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s) : close() failure ! Call '_acceptor.close()' returned %d"), _object_name.c_str(), call_result);
			return ACS_DSD_SAC_ACCEPTOR_CLOSE_ERROR;
		}

		_state = SAC_STATE_CLOSED;

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor ( %s ) : close() terminated with success."), _object_name.c_str());
		return ACS_DSD_SAC_SUCCESS;

		//return (remove_handlers_from_reactor() < 0) ? -1 : _acceptor.close();
	}


	/// Stop listening for connections
	int stop_listening(int low_level = 0)
	{
		int ret_val = 0;
		if(low_level)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s): shutdown sockets associated with SAP < %s >"), _object_name.c_str(), _sap_id);

			for(std::vector<acs_dsd::HANDLE>::iterator it = _registered_socket_handles.begin(); it != _registered_socket_handles.end(); ++it)
			{
				int call_result = ::shutdown(*it, SHUT_RDWR);
				if(call_result != 0)
				{
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s): Call 'shutdown()' failed on socket handle '%d' ! errno == %d"), _object_name.c_str(), *it, errno);
					ret_val = -1;
				}
			}
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s): stop listening for connections on SAP < %s >. LOW_LEVEL == %d"), _object_name.c_str(), _sap_id, low_level);

			int call_result = _acceptor.close();
			if(call_result != acs_dsd::ERR_NO_ERRORS)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ServiceAcceptor (%s): Call 'ACS_DSD_Server::close()' returned %d ! Details:  %s"), _object_name.c_str(), call_result, _acceptor.last_error_text());
				ret_val = -1;
			}
		}

		return ret_val;
	}


	/// Shutdown of sockets associated with SAP '_sap_id'
	int shutdown_sockets() { return stop_listening(1); }


	/// Returns the description of last error occurred
	std::string last_error_descr()
	{
		return _last_error_descr;
	};


	/// Returns the name assigned to the object instance
	std::string get_name() { return _object_name; }

private:

	/**	@brief remove_handlers_from_reactor method
	 *
	 *	remove_handlers_from_reactor method description.
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int remove_handlers_from_reactor()
	{
		int ret_code = 0;

		if(!_reactor) return 0;		// nothing to do

		/*
		// we need the socket handles manipulated by the ACS_DSD_Server object (<this->_acceptor>)
		int handle_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1;
		acs_dsd::HANDLE sock_handles [acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1];
		if(_acceptor.get_handles(sock_handles, handle_count) < 0)
			return -1;	// an error occurred while getting handles
		 */

		int handle_count = _registered_socket_handles.size();

		// now remove <this> as event handler for the socket handles
		for(int i=0; i < handle_count; ++i)
		{
			acs_dsd::HANDLE hdl = _registered_socket_handles.at(i);
			if((0 == _reactor->handler(hdl, ACE_Event_Handler::ACCEPT_MASK)) &&  (_reactor->remove_handler(hdl, ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL) < 0))
			{
				// an error occurred during remove_handler. Go ahead !
				ret_code = -1;
			}
		}

		return ret_code;
	}

private:
	//==============//
	//   Fields   	//
	//==============//
	std::string _object_name;									/// Name of the object
	ServiceAcceptor_State _state;								/// Object state
	ACS_DSD_Server _acceptor;									/// DSDAPI Server object used to accept connections
	char _sap_id[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX + 1];	/// SAP (Service Access Point) on which the object listens for connections
	std::vector<acs_dsd::HANDLE> _registered_socket_handles;	/// List of handles associated to the DSDAPI Server object and successfully registered with '_reactor'
	ACE_Reactor * _reactor;										/// Reactor used to dispatch the object instance when a client connection is ready to accept
	ACE_Reactor * _service_handler_reactor; 					/// Reactor to be passes to the SERVICE_HANDLER class constructor when initializing a new SERVICE_HANDLER for handling a new client connection
	std::string _last_error_descr; 								/// Last error occurred during a method execution
};

#endif /* ACS_DSD_SERVICEACCEPTOR_H_ */
