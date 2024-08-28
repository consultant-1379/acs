#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"

#include "ACS_DSD_RemoteApNodesManager.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_RemoteApNodesManager

int __CLASS_NAME__::add_remote_ap_node_connection (int32_t system_id, ACS_DSD_SrvProt_SvcHandler * service_handler) {
	// Check service_handler parameter
	if (!service_handler) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Received a null 'service_handler' "
				"parameter: system_id = '%d'"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);
		return APNM_ERROR_NULL_PARAMETER;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	// Find if another item with the same key is already present
	std::map<int32_t, ACS_DSD_SrvProt_SvcHandler *>::iterator it = _remote_ap_nodes.find(system_id);
	if (it != _remote_ap_nodes.end()) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Failed to add the element to RemoteAPNodesManager. "
				"An object for the AP system id <%d> is already present"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);
		return APNM_ERROR_ITEM_ALREADY_PRESENT;
	}
	_remote_ap_nodes[system_id] = service_handler;
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Added the service handler with CONN_ID = < %d > into "
			"RemoteApNodesManager collection for the AP node having system_id = %d"),
			ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, service_handler->getID(), system_id);

	return APNM_ERROR_NO_ERROR;
}

int __CLASS_NAME__::remove_remote_ap_node_connection (int32_t system_id) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	// Find if the given key is present before trying to remove it
	std::map<int32_t, ACS_DSD_SrvProt_SvcHandler *>::iterator it = _remote_ap_nodes.find(system_id);
	if (it == _remote_ap_nodes.end()) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s::%s: Failed to remove the element for the system id <%d> "
				"from RemoteAPNodesManager. The element isn't present"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);
		return APNM_ERROR_ITEM_NOT_PRESENT;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Removing from the collection the object for remote "
			"AP system id <%d>"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, system_id);

	_remote_ap_nodes.erase(it);

	return APNM_ERROR_NO_ERROR;
}

int __CLASS_NAME__::remove_all_ap_nodes_connections () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	if (_remote_ap_nodes.empty()) {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: _remote_ap_nodes collection is "
				"already empty, nothing to do!"), ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__);
		return APNM_ERROR_NO_ERROR;
	}

	std::map<int32_t, ACS_DSD_SrvProt_SvcHandler *>::iterator it;
	for (it = _remote_ap_nodes.begin(); it != _remote_ap_nodes.end(); it++)	{
		ACS_DSD_SrvProt_SvcHandler * service_handler = it->second;

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s::%s: Closing the connection "
				"(CONN_ID = < %d >) opened by the AP node having system_id = %d"),
				ACS_DSD_STRINGIZE(__CLASS_NAME__), __func__, service_handler->getID(), it->first);

		service_handler->peer().close();
	}

	return APNM_ERROR_NO_ERROR;
}
