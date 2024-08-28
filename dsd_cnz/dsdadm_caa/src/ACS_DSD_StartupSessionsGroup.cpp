#include <string>
#include <algorithm>
#include <new>

#include "ACS_DSD_Utils.h"
#include "ACS_DSD_StartupSessionsGroup.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;
extern ACS_DSD_Node dsd_local_node;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_StartupSessionsGroup

namespace {
	struct create_data_if_not_my_ap {
		inline
		create_data_if_not_my_ap (
				std::map <
					int32_t,
					std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
				> & objects, ACS_DSD_StartupSessionsGroup * ssg)
		: _other_ap_objects(objects), _ssg(ssg) {}

		inline void operator() (std::vector<ACS_DSD_ConfigurationHelper::HostInfo>::reference host_item) {
			if (host_item.system_id != dsd_local_node.system_id) {
				std::pair <
					std::vector<ACS_DSD_ConfigurationHelper::HostInfo>,
					ACS_DSD_StartupSessionHandler *
				> & map_item = _other_ap_objects[host_item.system_id];

				map_item.first.push_back(host_item);
				if (map_item.first.size() == 1) {
					map_item.second = new (std::nothrow) ACS_DSD_StartupSessionHandler(host_item.system_id, _ssg);

					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
							ACS_DSD_TEXT("%s: A new ACS_DSD_StartupSessionHandler object allocated at "
									"%p for system_id == %d"),
							__PRETTY_FUNCTION__, map_item.second, host_item.system_id);
				}
			}
		}
	private:
		std::map <
			int32_t,
			std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
		> & _other_ap_objects;
		ACS_DSD_StartupSessionsGroup * _ssg;
	};
}

int __CLASS_NAME__::handle_timeout (const ACE_Time_Value & /*current_time*/, const void * act) {
	ACS_DSD_Node my_ap_node;
	std::string msg;

	if (ACS_DSD_Utils::get_node_info(my_ap_node, msg)) {
		// ERROR: Getting my node info and state
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT ("%s::%s: Call 'get_node_info' failed: error_text == %s"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, msg.c_str());
		return 0;
	}

	int return_code = 0;

	// Understand the timer that has been expired
	if ( act == ACS_DSD_STARTUP_CONNECTION_CHECK_TIMER_CONTEXT )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT ("%s::%s: Expired Timer: Startup_Connection_Checks"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

		if (my_ap_node.node_state == acs_dsd::NODE_STATE_ACTIVE) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT ("%s::%s: Startup check when node state is ACTIVE: trying to reconnect if not yet"
							" - StartupSessionGroup state == %d, SystemConfiguration == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, _state, system_configuration());

			// Recall the connect that will verify if it's all right
			if (_state == SSG_STATE_ALL_CONNECTED) return 0;

			// If the group state isn't SSG_STATE_ALL_CONNECTED, try
			// to reconnect the group only if the system configuration is
			// not Single-AP
			if ( system_configuration() != SYS_CONF_SINGLE_AP )
			{
				if (const int call_result = connect()) {
					if (call_result == ACS_DSD_StartupSessionsGroup::SSG_ERROR_NO_MULTI_AP_SYSTEM) {
						// WARNING: DSD Server is running on a NO-MULTI-AP system
//					return_code = -1;
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN,
								ACS_DSD_TEXT ("%s::%s: Call 'connect' failed: NO MULTI AP SYSTEM"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
					} else if (call_result && (_state != SSG_STATE_ALL_CONNECTED)) {
						// ERROR: connecting towards my remote DSD Server friends.
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
								ACS_DSD_TEXT ("%s::%s: Call 'connect' failed: call_result == %d"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
					} else {
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
								ACS_DSD_TEXT ("%s::%s: StartupSessionsGroup successfully connected towards all remote DSD "
										"Active Servers: StartupSessionsGroup state == %d"), ACS_DSD_STRINGIZE(__CLASS_NAME__),
										__func__, state());
					}
				}
			}
		} else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT ("%s::%s: Startup check when node state is %s: trying to close all if not yet"
							" - StartupSessionGroup state == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
					(my_ap_node.node_state == acs_dsd::NODE_STATE_UNDEFINED) ? "UNDEFINED" : "PASSIVE", _state);

			if (_state == SSG_STATE_DISCONNECTED) return 0;

			if (const int call_result = close(1)) {
				if ( call_result == ACS_DSD_StartupSessionsGroup::SSG_ERROR_ALREADY_CLOSED )
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
							ACS_DSD_TEXT ("%s::%s: All sessions already closed. Nothing to do!"),
							ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
				else
					// ERROR: closing the connections towards my remote DSD Server friends.
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
							ACS_DSD_TEXT ("%s::%s: Call 'close' failed: call_result == %d"),
							ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
			}
		}
	}
	else if ( act == ACS_DSD_IMM_COPY_POLLING_CONTEXT )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT ("%s::%s: Expired Timer: IMM Copy Polling"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

		if (my_ap_node.node_state == acs_dsd::NODE_STATE_ACTIVE )
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT ("%s::%s: My state is ACTIVE, asking for the list of registered services on remote nodes!"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

			int op_res = 0;
			for (
						std::map <
							int32_t,
							std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
						>::iterator map_iter = _other_ap_objects.begin();
						map_iter != _other_ap_objects.end();
						++map_iter
				)
			{
				ACS_DSD_StartupSessionHandler * ssh = (map_iter->second).second;
				if ( ssh )
				{
					if ( ssh->peer().state() != acs_dsd::SESSION_STATE_DISCONNECTED )
					{
						op_res = ssh->update_IMM_services_info();
						if ( op_res )
						{
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
									ACS_DSD_TEXT ("%s::%s: Call 'update_IMM_services_info' for system_id == %d "
									"failed! op_res == %d"),	ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
									map_iter->first, op_res);
						}
					}
					else
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
								ACS_DSD_TEXT ("%s::%s: The session is disconnected, nothing to do!!!"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
				}
			}
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT ("%s::%s: My state is NOT ACTIVE, nothing to do!"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		}
	} else if ( act == ACS_DSD_STATE_UPDATE_POLLING_CONTEXT ) {

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT ("%s::%s: Expired Timer: Remote AP node state update."), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

		if (my_ap_node.node_state == acs_dsd::NODE_STATE_ACTIVE )	{

			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT ("%s::%s: My state is ACTIVE, checking if some remote node has UNDEFINED state."),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);

			for (	std::map < int32_t,	std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
						>::iterator map_iter = _other_ap_objects.begin();
						map_iter != _other_ap_objects.end();
						++map_iter )	{

				ACS_DSD_StartupSessionHandler * ssh = (map_iter->second).second;
				if ( ssh ) {
					if ( ssh->peer().state() != acs_dsd::SESSION_STATE_DISCONNECTED ) {
						const int call_result = ssh->update_remote_nodes_state();
						if ( call_result ) {
							ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
									ACS_DSD_TEXT ("%s::%s: Call 'update_remote_nodes_state' for system_id == %d "
									"failed! call_result == %d"),	ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__,
									map_iter->first, call_result);
						}
					}
					else
						ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
								ACS_DSD_TEXT ("%s::%s: The session is disconnected, nothing to do!!!"),
								ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
				}
			}
		}
		else {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT ("%s::%s: My state is NOT ACTIVE, nothing to do!"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		}
	}

	return return_code;
}

int __CLASS_NAME__::connect () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	if (_state == SSG_STATE_ALL_CONNECTED) {
		// All startup-session-handler objects have already a
		// communication session opened.
		return SSG_ERROR_ALREADY_CONNECTED;
	}
	// If not yet done create all the internal structures getting info from configuration
	if ( (_other_aps_count <= 0) && (_system_configuration != SYS_CONF_SINGLE_AP) ) {
		std::vector<ACS_DSD_ConfigurationHelper::HostInfo> other_ap_nodes;

		if (const int call_result = ACS_DSD_ConfigurationHelper::get_all_ap_nodes(other_ap_nodes)) {
			// ERROR: Loading the AP nodes from configuration
			return SSG_ERROR_GET_AP_NODES;
		}

		std::for_each (other_ap_nodes.begin(), other_ap_nodes.end(), create_data_if_not_my_ap(_other_ap_objects, this));
		_other_aps_count = static_cast<int>(_other_ap_objects.size());

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: _other_ap_objects.size == %zd"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, _other_ap_objects.size());
	}

	// Is the system at least a dual AP?
	if (_other_aps_count <= 0) { // NO
		if ( _system_configuration == SYS_CONF_UNKNOWN ) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: System configuration was UNKNOWN, i'll set it to SINGLE_AP!!!"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
			system_configuration(SYS_CONF_SINGLE_AP);
			return SSG_ERROR_NO_MULTI_AP_SYSTEM;
		}
		return SSG_ERROR_LOAD_CONFIGURATION_FAILED;
	}

	// If some other APs are found, set the system configuration to a Multi-AP system
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Setting system configuration to MULTI_AP!!!"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
	system_configuration(SYS_CONF_MULTI_AP);

	StartupSessionsGroupStateConstants new_state = SSG_STATE_ALL_CONNECTED; // Positive hypothesis

	// Now we can try to connect to remote DSD servers running on each remote active AP if not
	for (
			std::map <
				int32_t,
				std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
			>::iterator map_iter = _other_ap_objects.begin();
			map_iter != _other_ap_objects.end();
			++map_iter
	) {
		int32_t system_id = map_iter->first;

		ACS_DSD_StartupSessionHandler * ssh = (map_iter->second).second;

		if (!ssh) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
					ACS_DSD_TEXT("%s::%s: StartupSessionHandler is NULL, maybe it was previously deleted: "
							"re-allocating it for system_id == %d..."),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);

			// There are problems on memory allocation. We try to reallocate now a session object
			(map_iter->second).second = ssh = new (std::nothrow) ACS_DSD_StartupSessionHandler(system_id, this);

			if (ssh)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
						ACS_DSD_TEXT("%s::%s: StartupSessionHandler successfully re-allocated for system_id == %d"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);
			else
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
						ACS_DSD_TEXT("%s::%s: StartupSessionHandler re-allocation (new) failed for system_id == %d"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);
		}

		if (ssh) {
			// There is a valid session handler that we can use to connect towards its remote AP
			int call_result = ssh->connection_open((map_iter->second).first);

			if (call_result == ACS_DSD_StartupSessionHandler::SSH_ERROR_ALREADY_CONNECTED) {
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN,
						ACS_DSD_TEXT("%s::%s: StartupSessionHandler (%p) already connected towards AP system %d"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, system_id);
			} else if (call_result) {
				new_state = SSG_STATE_SOME_CONNECTED;
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
						ACS_DSD_TEXT("%s::%s: Call 'ssh->connection_open' failed: StartupSessionHandler (%p) cannot "
								"connect towards AP system %d: call_result == %d"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, system_id, call_result);
			} else {
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
						ACS_DSD_TEXT("%s::%s: StartupSessionHandler (%p) successfully connected towards AP system %d"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, system_id);

				// Here we add this open session into the reactor
				if ((call_result = register_session_to_reactor(ssh))) {
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
							ACS_DSD_TEXT("%s::%s: Call 'register_session_to_reactor' failed: ssh == %p: "
									"system_id == %d: call_result == %d"),
							ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, system_id, call_result);
				}
			}
		} else new_state = SSG_STATE_SOME_CONNECTED;
	}

	_state = new_state;

	return SSG_ERROR_NO_ERROR;
}

int __CLASS_NAME__::close (int remove_from_reactor) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Trying to close all the always-alive connections. StartupSessionGroup.state == %d"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, _state);

	if (_state == SSG_STATE_DISCONNECTED) {
		// All startup-session-handler objects have already been closed.
		return SSG_ERROR_ALREADY_CLOSED;
	}

	int error = 0;			// Positive hypothesis
	int call_result = 0;

	for (
			std::map <
				int32_t,
				std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
			>::iterator map_iter = _other_ap_objects.begin();
			map_iter != _other_ap_objects.end();
			++map_iter
	) {
			int32_t system_id = map_iter->first;
			ACS_DSD_StartupSessionHandler * (& ssh) = (map_iter->second).second;

			if ( !ssh )
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
						ACS_DSD_TEXT("%s::%s: StartupSessionHandler related to system id %d is NULL. Nothing to do!"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);
			}
			else
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
						ACS_DSD_TEXT("%s::%s: Trying to delete the StartupSessionHandler related "
						"to the AP having system_id == %d"),
						ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);

				call_result = delete_session_item(ssh, remove_from_reactor);
				if( call_result )
				{ // ERROR: closing and deleting the session
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
							ACS_DSD_TEXT("%s::%s: Call 'delete_session_item' failed: error deleting the "
							"session for system_id == %d: call_result == %d"),
							ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id, call_result);
					error = 1;
				}
				else
					ssh = 0;
			}
		}

	// If some errors happened during the deleting operation, DON'T erase the internal structures.
	if ( error )
		return call_result;

	// In no errors happened, clear the internal data structures
	_other_ap_objects.clear();
	_other_aps_count = -1;
	_state = SSG_STATE_DISCONNECTED;

	return SSG_ERROR_NO_ERROR;
}

int __CLASS_NAME__::close_session (int32_t system_id, int remove_from_reactor) {
	std::map <
		int32_t,
		std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
	>::iterator map_item = _other_ap_objects.find(system_id);

	if (map_item == _other_ap_objects.end()) return SSG_ERROR_SESSION_NOT_FOUND;

	ACS_DSD_StartupSessionHandler * ssh = (map_item->second).second;

	const int call_result = close_session_item(ssh, remove_from_reactor);

	if (call_result) // ERROR: closing the session
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: Call 'close_session_item' failed: error on closing session "
						"(%p) for system_id == %d: call_result == %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, system_id, call_result);
	else {
		_state = SSG_STATE_SOME_CONNECTED;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: StartupSessionHandler (%p) on system_id == %d correctly closed"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, system_id);
	}

	return call_result;
}

int __CLASS_NAME__::delete_session (int32_t system_id, int remove_from_reactor) {
	std::map <
		int32_t,
		std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
	>::iterator map_item = _other_ap_objects.find(system_id);

	if (map_item == _other_ap_objects.end()) return SSG_ERROR_SESSION_NOT_FOUND;

	ACS_DSD_StartupSessionHandler * (& ssh) = (map_item->second).second;

	const int call_result = delete_session_item(ssh, remove_from_reactor);

	if (call_result) // ERROR: deleting the session
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: Call 'delete_session_item' failed: error on deleting "
						"the session (%p) for system_id == %d: call_result == %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, system_id, call_result);
	else {
		_state = SSG_STATE_SOME_CONNECTED;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
				ACS_DSD_TEXT("%s::%s: StartupSessionHandler on system_id == %d correctly deleted"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);
		ssh = 0;
	}

	return call_result;
}

int __CLASS_NAME__::close_session_item (ACS_DSD_StartupSessionHandler * ssh, int remove_from_reactor)
{
	if( !ssh )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: StartupSessionHandler is NULL!"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		return SSG_ERROR_SESSION_IS_NULL;
	}

	if( remove_from_reactor )
	{
		const int call_result = remove_session_from_reactor(ssh);
		if( call_result )
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Error while removing the "
					"StartupSessionHandler from reactor. call_result == %d"),
					ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
			return call_result;
		}
	}

	return ((ssh->connection_close()) ? SSG_ERROR_SESSION_CLOSE
																				: (_state = SSG_STATE_SOME_CONNECTED, SSG_ERROR_NO_ERROR));
}

int __CLASS_NAME__::delete_session_item (ACS_DSD_StartupSessionHandler * ssh, int remove_from_reactor)
{
	const int call_result = close_session_item(ssh, remove_from_reactor);
	_state = SSG_STATE_SOME_CONNECTED;

	if( call_result )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: Error while closing the session item. I'll not delete this object!."
				" call_result == %d"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, call_result);
		return call_result;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Session item correctly closed. I'm deleting it."),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
	delete ssh;

	return SSG_ERROR_NO_ERROR;
}

int __CLASS_NAME__::register_session_to_reactor (ACS_DSD_StartupSessionHandler * ssh) {
	if (!_reactor) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: No reactors provided"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		return SSG_ERROR_NO_REACTORS;
	}

	// register object as event handler for "data ready" events on the I/O handle that is associated to the DSD API Session object
	if (_reactor->register_handler(ssh, ACE_Event_Handler::READ_MASK) < 0) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: error registering the StartupSessionHandler (%p) with reactor for READ events!"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh);
		return SSG_ERROR_REACTOR_REGISTER_HANDLER;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
			ACS_DSD_TEXT("%s::%s: StartupSessionHandler (%p) registered successfully"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh);

	return SSG_ERROR_NO_ERROR;
}

int __CLASS_NAME__::remove_session_from_reactor (ACS_DSD_StartupSessionHandler * ssh) {
	if (!_reactor) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: No reactors provided"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		return SSG_ERROR_NO_REACTORS;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s::%s: Trying to remove from reactor the StartupSessionHandler %p"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh);

	if (_reactor->remove_handler(ssh, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL)) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s::%s: Call 'remove_handler' failed: ssh == %p, system_id == %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, ssh->system_id());
		return SSG_ERROR_REACTOR_REMOVE_HANDLER;
	} else
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO,
				ACS_DSD_TEXT("%s::%s: StartupSessionHandler (%p) successfully removed from reactor: system_id == %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, ssh, ssh->system_id());

	return SSG_ERROR_NO_ERROR;
}
