#include <time.h>
#include <errno.h>
#include "ace/Time_Value_T.h"

#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ImmCpNodesUpdater.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;
extern ACE_Recursive_Thread_Mutex imm_shared_objects_conn_mutex;
extern int is_imm_shared_objects_controller;


ACS_DSD_ImmCpNodesUpdater::ACS_DSD_ImmCpNodesUpdater(ACS_DSD_CpNodesManager * cp_nodes_manager)
	: _cp_nodes_manager(cp_nodes_manager), _bStopped(false), _condition_attributes(), _thread_mutex(),
	  _stop_working_condition(_thread_mutex, _condition_attributes)
{}


int ACS_DSD_ImmCpNodesUpdater::svc (void)
{
	int n_serv_added = 0;
	int n_serv_modified = 0;
	int n_serv_removed = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ImmCpNodesUpdater started !"));

	while(!_bStopped)
	{
		// we must go on with the iteration only if we are the controller of IMM shared objects
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_shared_objects_conn_mutex);
		if(is_imm_shared_objects_controller)
		{
			// release control of IMM shared objects
			guard.release();

			// get the list of the connected CP nodes whose 'service list' has to be refreshed
			NodeNamesList cp_node_names_list;
			get_cp_nodes_to_update(cp_node_names_list);

			// iterate on the  list
			for(NodeNamesList::iterator it = cp_node_names_list.begin(); it != cp_node_names_list.end(); ++it)
			{
				const char * cp_node_name = (*it).c_str();
				n_serv_added = 0;
				n_serv_modified = 0;
				n_serv_removed = 0;
				ServiceInfoList serv_info_list;
				if(!_cp_nodes_manager->get_cp_node_services(cp_node_name, serv_info_list))
				{
					ACS_DSD_CpNodesManager::OP_Result op_res = _cp_nodes_manager->update_IMM_cp_node(cp_node_name, serv_info_list, n_serv_added, n_serv_modified, n_serv_removed);
					if(op_res == ACS_DSD_CpNodesManager::CPNM_OK)
						update_cp_node_info(cp_node_name, n_serv_added, n_serv_modified, n_serv_removed);
				}
			}
		}
		else
			// release control of IMM shared objects
			guard.release();

		if(!_bStopped) {
			ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
			timeout = timeout.now() + ACE_Time_Value(IMM_CP_NODES_UPDATER_INTERVAL);

			// Sleep for IMM_CP_NODES_UPDATER_INTERVAL seconds or exit immediately if the stop condition
			// is signalled
			errno = 0;
			if (_stop_working_condition.wait(&timeout) == -1) {
				// Timeout expired: thread should continue to work
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ImmCpNodesUpdater: sleep timeout expired: thread continue to work: errno == %d"), errno);
			} else {
				// Stop condition signalled: thread will terminate immediately
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ImmCpNodesUpdater: stop condition signalled: thread will terminate immediately"));
			}
		}
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("ACS_DSD_ImmCpNodesUpdater stopped !"));
	return 0;
}


int ACS_DSD_ImmCpNodesUpdater::start_activity()
{
	_bStopped = false;
	return this->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1);
}

int ACS_DSD_ImmCpNodesUpdater::stop_activity(bool wait_termination)
{
	_bStopped = true;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_ImmCpNodesUpdater: signalling the stop condition to stop the thread"));

	_stop_working_condition.signal();

	int call_result = 0;
	if(wait_termination)
		call_result = wait();

	return call_result;
}

void ACS_DSD_ImmCpNodesUpdater::get_cp_nodes_to_update(NodeNamesList & nodes_to_be_updated)
{
	// obtain exclusive access to the internal CP nodes collection of CpNodesManager object
	 ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_cp_nodes_manager->_update_info_list_mutex);

	// iterate on the CpNodes collection
	ACS_DSD_CpNodesManager::CpNodesIMMUpdateInfoCollection  & collection = _cp_nodes_manager->_cp_node_imm_update_info_collection;
	for(ACS_DSD_CpNodesManager::CpNodesIMMUpdateInfoCollection::iterator it = collection.begin(); it != collection.end(); ++it)
	{
		CpNode_imm_update_info & cpNodeImmUpdateInfo = *((*it).second);

		if(cpNodeImmUpdateInfo.next_scheduled_update_time.now() >= cpNodeImmUpdateInfo.next_scheduled_update_time)
		{
			nodes_to_be_updated.push_back(cpNodeImmUpdateInfo.node_name);
		}
	}
}

int ACS_DSD_ImmCpNodesUpdater::update_cp_node_info(const char * node_name, int n_serv_added, int /*n_serv_modified*/, int n_serv_removed)
{
	// obtain exclusive access to the internal CP nodes collection of CpNodesManager object
	 ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_cp_nodes_manager->_update_info_list_mutex);

	// iterate on the CpNodes collection
	ACS_DSD_CpNodesManager::CpNodesIMMUpdateInfoCollection  & collection = _cp_nodes_manager->_cp_node_imm_update_info_collection;
	for(ACS_DSD_CpNodesManager::CpNodesIMMUpdateInfoCollection::iterator it = collection.begin(); it != collection.end(); ++it)
	{
		CpNode_imm_update_info & cpNodeImmUpdateInfo = *((*it).second);
		if(!strcmp(cpNodeImmUpdateInfo.node_name, node_name))
		{
			// set persistent state counter
			if(!n_serv_added && !n_serv_removed)
				++cpNodeImmUpdateInfo.n_persist_state_updates;	// no service added or removed for the CP Node. Increment counter
			else
				cpNodeImmUpdateInfo.n_persist_state_updates=0;	// service list associated to the CP Node changed. Reset counter

			// set next update time
			time_t secs_sched = MIN_IMM_CP_NODE_UPDATE_INTERVAL * (1 + cpNodeImmUpdateInfo.n_persist_state_updates);
			if (secs_sched > MAX_IMM_CP_NODE_UPDATE_INTERVAL) secs_sched = MAX_IMM_CP_NODE_UPDATE_INTERVAL;
			cpNodeImmUpdateInfo.next_scheduled_update_time = cpNodeImmUpdateInfo.next_scheduled_update_time.now() + ACE_Time_Value(secs_sched);

			// Ok, CP Node info updated !
			return 0;
		}
	}

	// CP node not found. May be that the relative connection has been removed in the meanwhile
	return 1;
}
