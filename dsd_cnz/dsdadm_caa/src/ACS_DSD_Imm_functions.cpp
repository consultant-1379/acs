#include "ACS_DSD_ImmConnectionHandler.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_Server.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_Imm_functions.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"
#define MAX_IMM_INIT_RETRY 40

extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;		// defined in ACS_DSD_Main.cpp.  It's the DSD Server connection to IMM service for private objects
extern ACE_Recursive_Thread_Mutex imm_conn_mutex;					// defined in ACS_DSD_Main.cpp.  It's used to sync access to <immConnHandlerObj> global variable
extern ACS_DSD_ImmConnectionHandler * imm_shared_objects_conn_ptr;	// defined in ACS_DSD_Main.cpp.  It's the DSD Server connection to IMM service for shared objects
extern ACE_Recursive_Thread_Mutex imm_shared_objects_conn_mutex;	// defined in ACS_DSD_Main.cpp.  It's used to sync access to <imm_shared_objects_conn> global variable
extern int is_imm_shared_objects_controller;						// defined in ACS_DSD_Main.cpp.  Indicates if this instance is the controller of shared IMM objects
extern ACS_DSD_CpNodesManager cpNodesManager;						// defined in ACS_DSD_Main.cpp.  Used to keep information about the CP nodes currently connected to DSD Server
extern ACE_Recursive_Thread_Mutex imm_manteinance_op_mutex;			// defined in ACS_DSD_Main.cpp.
extern ACS_DSD_Logger dsd_logger;									// defined in ACS_DSD_Main.cpp.
extern bool dsd_is_stopping;										// defined in ACS_DSD_Main.cpp.
extern ACS_DSD_Node dsd_local_node;									// defined in ACS_DSD_Main.cpp.

static bool take_control_operation_pending =  false;
static bool release_control_operation_pending =  false;
static bool imm_shared_conn_restore_pending = false;
static int imm_op_temporary_failures_cnt = 0;						// used by the the function 'check_and_restore_IMM_connections()'


int init_IMM(const ACS_DSD_Node & dsd_local_node, bool clean_imm_entry_for_local_node)
{
	//ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Connecting to OpenSAF IMM Service .... !"));

	// register DSD Server with IMM service for PRIVATE objects handling
	char dsd_imm_private_name[1024];
	snprintf(dsd_imm_private_name,ACS_DSD_ARRAY_SIZE(dsd_imm_private_name), "%s_%s", DSD_SERVER_IMM_PREFIX, dsd_local_node.node_name);
	dsd_imm_private_name[ACS_DSD_ARRAY_SIZE(dsd_imm_private_name) - 1] = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Registering DSD Server with IMM using implementer name < %s > (PRIVATE objects handling) .... "), dsd_imm_private_name);
	ACS_CC_ReturnType imm_init_res = immConnHandlerObj_ptr->init(dsd_imm_private_name);
	if(imm_init_res != ACS_CC_SUCCESS)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("ACS_DSD_ImmConnectionHandler.init() failure ! return code is < %d > - Error descr: '%s'"),
				imm_init_res, immConnHandlerObj_ptr->getInternalLastErrorText());
		return -1;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("DSD Server successfully registered with IMM using implementer name < %s > (PRIVATE objects handling) !"), dsd_imm_private_name);

	// if the node on which DSD Server is running is in ACTIVE state, register DSD Server with IMM service for SHARED objects handling
	if(dsd_local_node.node_state == acs_dsd::NODE_STATE_ACTIVE)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Registering DSD Server with IMM using implementer name < %s > (SHARED objects handling) .... "), DSD_SERVER_IMM_PREFIX);

		// try to take control of IMM SHARED objects, with < nRetry > retries
		int nRetry = N_RETRY_IMM_REGISTER;
		int call_result = take_control_of_IMM_shared_objects(nRetry);
		if( call_result < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Failure trying to take control of IMM SHARED objects ! "
					"Call 'take_control_of_IMM_shared_objects()' failed ! call_result = %d - Error descr: '%s'"),
					call_result, imm_shared_objects_conn_ptr->getInternalLastErrorText());
			return -2;	// we have been unsuccessful registering with IMM for SHARED objects handling
		}
		if(call_result != 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Call 'take_control_of_IMM_shared_objects()' returned '%d' ! "), call_result);

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("DSD Server successfully registered with IMM using implementer name < %s > (SHARED objects handling) - nRetry = < %d > !"), DSD_SERVER_IMM_PREFIX, nRetry);
	}


	ACS_DSD_ImmDataHandler immDataHandler(immConnHandlerObj_ptr);

	// if requested, clean existing IMM entry for local node before proceeding
	if(clean_imm_entry_for_local_node)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("DSD Server invoked with 'c' option: trying to remove IMM subtree (eventually) rooted at SRTNode  < %s >"),dsd_local_node.node_name);
		int remove_node_res = immDataHandler.removeNodeInfo(dsd_local_node.node_name);
		if(!remove_node_res) { ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("the IMM subtree rooted at SRTNode < %s > has been successfully REMOVED!"), dsd_local_node.node_name); }
	}

	// now create or update the IMM SRTNode object for the current node
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Creating or updating SRTNode: --- NODE.name = %s --- NODE.state = %d ..."),dsd_local_node.node_name, dsd_local_node.node_state);
	if(!clean_imm_entry_for_local_node)
	{
		// the node should be already existing ... so try first an update of the (eventually) existing node
		immDataHandler.set_node(dsd_local_node.system_name);
		int modify_node_res = immDataHandler.modifyNodeInfo(const_cast<char *>(dsd_local_node.node_name), dsd_local_node.node_state);
		if(modify_node_res == 0)
		{
			// update success !
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("the existing SRTNode has been successfully UPDATED !"));
			return 0;
		}
	}

	immDataHandler.set_state(dsd_local_node.node_state);
	immDataHandler.set_side((dsd_local_node.node_side));
	immDataHandler.set_node(dsd_local_node.system_name);
	int create_node_res = immDataHandler.addNodeInfo(const_cast<char *>(dsd_local_node.node_name));
	if(create_node_res < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("ACS_DSD_ImmDataHandler::addNodeInfo() failure! return code is < %d > - Error descr: '%d'"),
				create_node_res, immDataHandler.last_error_text());
		return -3;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("SRTNode has been successfully created!"));

	return 0;
}

int  unregisterOldServicesfromIMM(const ACS_DSD_Node & node)
{
	ACS_DSD_ImmDataHandler ImmDHobj(immConnHandlerObj_ptr);
	std::vector<std::string> registered_service_list;
	unsigned int elem_num=0;
	int call_result;
	int n_imm_errors = 0;

	// get list of registered services
	call_result = ImmDHobj.fetchRegisteredServicesList(node.node_name, registered_service_list);
	if(call_result != acs_dsd_imm::NO_ERROR)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("unregisterOldServicesfromIMM(..): "
				"Call 'ACS_DSD_ImmDataHandler::fetchRegisteredServicesList()' failed! call_result ==  %d - Error descr: '%s'"),
				call_result, ImmDHobj.last_error_text());
		return -1;
	}

	// iterate on the list to see if the services are still running
	elem_num = registered_service_list.size();
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Found < %d > services registered in IMM"), elem_num);
	for(unsigned int i=0; i<elem_num; i++)
	{
		if((call_result = ImmDHobj.fetch_serviceProcName_ProcId(registered_service_list[i].c_str())) == acs_dsd_imm::NO_ERROR)
		{
			char proc_name[acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX]={0};
			int32_t proc_pid;
			ImmDHobj.get_proc_name(proc_name);
			ImmDHobj.get_pid(proc_pid);

			// check if the current service is running
			call_result = ACS_DSD_ConfigurationHelper::process_running(proc_name, proc_pid);
			if(call_result == 1)
			{
				// the current service is running. Don't remove its IMM entry
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Service '%s' is running; we don't remove its IMM entry !"),registered_service_list[i].c_str());
				continue;
			}

			if(call_result == 0)
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Service '%s' is NOT running: we're going to remove its IMM entry !"),registered_service_list[i].c_str());
			else
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("unregisterOldServicesfromIMM(..): Call 'ACS_DSD_ConfigurationHelper::process_running(..) failed !  proc_name == %s, proc_pid == %d,  call_result == %d."), proc_name, proc_pid, call_result);

			// Delete service data from IMM
			call_result = ImmDHobj.deleteServiceInfo(registered_service_list[i].c_str());
			if(call_result != acs_dsd_imm::NO_ERROR)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("unregisterOldServicesfromIMM(..): "
						"Call 'ACS_DSD_ImmDataHandler::deleteServiceInfo(%s)' failed! call_result == %d - Error descr: '%s'"),
						registered_service_list[i].c_str(), call_result, ImmDHobj.last_error_text());
				++ n_imm_errors;
			}
			else
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("IMM entry for Service '%s' successfully removed !"),registered_service_list[i].c_str());
		}
		else
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("unregisterOldServicesfromIMM(..): "
					"Call 'ACS_DSD_ImmDataHandler::fetch_serviceProcName_ProcId()' failed! call_result ==  %d - Error descr: '%s'"),
					call_result, ImmDHobj.last_error_text());
			++ n_imm_errors;
		}
	}

	return n_imm_errors;
}
/*
bool  isServiceProcessRunning(const char *process_name,int32_t process_pid)
{
	bool ret_value = true;
	char processFileName[acs_dsd::CONFIG_FILENAME_SIZE_MAX];
	char name[acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX];
	char pad1[20],pad2[20],status;

	snprintf(processFileName,acs_dsd::CONFIG_FILENAME_SIZE_MAX,DSD_PROCESS_STATUSFILE_PATH_PREFIX"%d/"DSD_PROCESS_STATUSFILE_NAME,process_pid);
	FILE *fstream = fopen(processFileName,"r");
	if(fstream != NULL){
		fscanf(fstream,"%s%s%s%c",pad1,name,pad2,&status);
		// the process with pid = reg_service_pid does NOT exist!!
		if(strcmp(name,process_name) || (status == 'Z')){
			ret_value = false;
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("unregisterOldServicesfromIMM--isServiceProcessRunning(%s, %d):service process pid does not exist or the service process state is Zombi!"), process_name,process_pid);
		}
		fclose(fstream);
	}
	else{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("unregisterOldServicesfromIMM--isServiceProcessRunning():fopen(%s) failed with code = %d: service pid doesn't exist"),processFileName,errno);
		ret_value = false;
	}
	return ret_value;
}
*/

int finalize_IMM(const ACS_DSD_Node & /*dsd_local_node*/, bool /*clean_imm_entry_for_local_node*/)
{
	// finalize DSD Server IMM connection used for PRIVATE OBJECTS handling
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_1(imm_conn_mutex);
	int retry = 0;
	ACS_CC_ReturnType finalize_result = ACS_CC_SUCCESS;
	do
	{
		finalize_result = immConnHandlerObj_ptr->finalize();
		if (finalize_result != ACS_CC_SUCCESS) {
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("failure finalizing IMM connection used for PRIVATE OBJECTS handling ! Retrying"));
			//ACE_OS::usleep(500000);
			usleep(500000);
			retry++;
		}
	}while((finalize_result != ACS_CC_SUCCESS) && ( retry < 10 ));		// Fix for TR HY65585
	if(finalize_result != ACS_CC_SUCCESS)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("failure finalizing IMM connection used for PRIVATE OBJECTS handling ! Call ACS_DSD_ImmConnectionHandler::finalize() failed ! Error descr: %s"), immConnHandlerObj_ptr->getInternalLastErrorText());
		return -1;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("IMM connection used for PRIVATE OBJECTS handling successfully finalized !"));
	guard_1.release();

	// (eventually) finalize DSD Server IMM connection used for SHARED OBJECTS handling
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_2(imm_shared_objects_conn_mutex);

	if(!is_imm_shared_objects_controller)
		return 0;	// the DSD Server is NOT the controller of IMM shared objects

	ACS_CC_ReturnType finalize_res = imm_shared_objects_conn_ptr->finalize();
	if(finalize_res != ACS_CC_SUCCESS)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("failure finalizing IMM connection used for SHARED OBJECTS handling ! Call ACS_DSD_ImmConnectionHandler::finalize() failed ! Error descr: %s"), imm_shared_objects_conn_ptr->getInternalLastErrorText());
		return -2;
	}
	is_imm_shared_objects_controller = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("IMM connection used for SHARED OBJECTS handling successfully finalized !"));

/*
	ACS_DSD_ImmDataHandler immDataHandler(immConnHandlerObj_ptr);
	if(clean_imm_entry_for_local_node)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("DSD Server invoked with 'c' option: trying to remove IMM subtree (eventually) rooted at SRTNode having name = %s"),dsd_local_node.node_name);
		int remove_node_res = immDataHandler.removeNodeInfo(dsd_local_node.node_name);
		if(remove_node_res < 0)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_ImmDataHandler::removeNodeInfo() failure  ! return code is < %d >"), remove_node_res);
			return -1;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("the IMM subtree rooted at SRTNode < %s > has been successfully REMOVED!"), dsd_local_node.node_name);
	}
*/
	return 0;
}

int take_control_of_IMM_shared_objects(int & nRetry)
{
	/*
	 * By invoking this function, the calling thread tries to become the controller of IMM shared objects. If such trial has success, the function also performs a
	 *  "synchronization" between the DSD IMM tree and the CP nodes collection contained in the "cpNodesManager" object.
	 * Since there are some ACS_DSD_CpNodesManager methods that read or write in IMM after locking the CP nodes collection, like "set_cp_conn()"
	 * and "remove_cp_conn()", to avoid deadlocks we have to follow the following order :
	 *  1) lock "imm_manteinance_op_mutex"
	 *  2) lock "cpNodesManager" object;
	 *  3) lock "IMM shared connection" object
	*/

	// be sure that the calling thread is the only one executing a manteinance operation on IMM objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_1(imm_manteinance_op_mutex);

	if(dsd_is_stopping) return 3;  // if DSD Server is stopping, we MUST do nothing

	// get exclusive access to CpNodesManager object
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_2(cpNodesManager.synch_mutex());

	// get exclusive access to global object <imm_shared_objects_conn>
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_3(imm_shared_objects_conn_mutex);

	// reset the flag indicating that there's a "take_control_of_IMM_shared_objects()" operation to be performed
	take_control_operation_pending = false;

	if(is_imm_shared_objects_controller)
	{
		nRetry = 1;
		return 1;	// the DSD Server is already the controller of IMM shared objects
	}

	// try to become the controller of IMM SHARED objects
	for(int i = 0; i < nRetry; ++i)
	{
		ACS_CC_ReturnType imm_init_res = imm_shared_objects_conn_ptr->init(DSD_SERVER_IMM_PREFIX);
		if(imm_init_res == ACS_CC_SUCCESS)
		{
			nRetry = i + 1;
			is_imm_shared_objects_controller = 1;
			imm_shared_conn_restore_pending = false;

			// synchronize IMM based on CP Nodes currently connected to DSD Server
			int32_t n_added, n_removed;
			if(cpNodesManager.synch_IMM(n_added, n_removed) < 0)
				return 2;

			return 0;
		}

		// wait for few seconds before retrying ...
		ACE_OS::sleep(N_SECS_IMM_REGISTER_INTERVAL);
	}

	// take note that the operation has failed and must be retried !
	take_control_operation_pending = true;

	return -1;
}

int release_control_of_IMM_shared_objects(int & nRetry)
{
	// be sure that the calling thread is the only one executing a manteinance operation on IMM objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_1(imm_manteinance_op_mutex);

	if(dsd_is_stopping) return 3;  // if DSD Server is stopping, we MUST do nothing

	// get exclusive access to global object <imm_shared_objects_conn>
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_2(imm_shared_objects_conn_mutex);

	// reset the flag indicating that there's a "release_control" operation to be performed
	release_control_operation_pending = false;

	if(!is_imm_shared_objects_controller)
	{
		nRetry = 1;
		return 1; // the DSD Server is not currently the controller of IMM SHARED objects
	}

	for(int i = 0; i < nRetry; ++i)
	{
		ACS_CC_ReturnType finalize_res = imm_shared_objects_conn_ptr->finalize();
		if(finalize_res == ACS_CC_SUCCESS)
		{
			nRetry = i + 1;
			is_imm_shared_objects_controller = 0;
			imm_shared_conn_restore_pending = false;
			return 0;
		}

		// wait for few seconds before retrying ...
		ACE_OS::sleep(N_SECS_IMM_FINALIZE_INTERVAL);
	}

	// take note that the operation has failed and must be retried !
	release_control_operation_pending = true;

	return -1;
}

int reset_IMM_op_pending_flags(bool lock_imm_manteninance_mutex)
{
	if(lock_imm_manteninance_mutex)
		imm_manteinance_op_mutex.acquire();

	take_control_operation_pending = release_control_operation_pending = imm_shared_conn_restore_pending = false;
	imm_op_temporary_failures_cnt = 0;

	if(lock_imm_manteninance_mutex)
		imm_manteinance_op_mutex.release();

	return 0;
}


int check_and_restore_IMM_connections()
{
	// We must be sure that the calling thread is the only one executing a manteinance operation on IMM connection objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_manteinance_op_mutex);
	if(!guard.locked())
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): unable to acquire lock on 'imm_manteinance_op_mutex'!"));
		return -7;
	}

	// First level checks: verify integrity of IMM connection objects
	int first_level_check_result = 0;

	// check the state of IMM private conn object
	first_level_check_result = check_and_restore_imm_conn_object(immConnHandlerObj_ptr, & imm_conn_mutex, true);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("check_and_restore_IMM_connections(): first level check on IMM PRIVATE objects connection returned '%d'!"), first_level_check_result);

	// check the state of IMM shared conn object
	if(is_imm_shared_objects_controller)
	{
		first_level_check_result = check_and_restore_imm_conn_object(imm_shared_objects_conn_ptr, & imm_shared_objects_conn_mutex, true);
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("check_and_restore_IMM_connections(): first level check on IMM SHARED objects connection returned '%d'!"), first_level_check_result);
		if(first_level_check_result == 1 /*the IMM conn object has been recreated*/)
			imm_shared_conn_restore_pending = false;
	}

	// Second level checks:

	// if there's a pending operation (take_control / release_control) on IMM shared connection, try to complete it
	if(take_control_operation_pending)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): found a pending 'take_control_of_IMM_shared_objects' operation: try to complete it !"));
		int nRetry = N_RETRY_IMM_REGISTER;
		int op_res = take_control_of_IMM_shared_objects(nRetry);
		if(op_res == 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'take_control_of_IMM_shared_objects()' operation successfully completed !"));
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'take_control_of_IMM_shared_objects()' operation failed ! op_res == < %d >"), op_res);
	}
	if(release_control_operation_pending)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): found a pending 'release_control_of_IMM_shared_objects' operation: try to complete it !"));
		int nRetry = N_RETRY_IMM_FINALIZE;
		int op_res = release_control_of_IMM_shared_objects(nRetry);
		if(op_res == 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'release_control_of_IMM_shared_objects()' operation successfully completed !"));
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'release_control_of_IMM_shared_objects()' operation failed ! op_res == < %d >"), op_res);
	}

	// in order to test if IMM is up and running, try to update the IMM object instance associated to the local AP node (in particular, the "state" attribute)
	int retval = 0;

	// reset temporary failure counter, but cache the previous value because it might be requested more forward
	int tmp_val = imm_op_temporary_failures_cnt;
	imm_op_temporary_failures_cnt = 0;

	// get local AP node info
	ACS_DSD_Node my_node;
	std::string error_descr;
	if(ACS_DSD_Utils::get_node_info(my_node, error_descr) < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): ACS_DSD_Utils::get_node_info() failed ! Error descr: %s"), error_descr.c_str());
		return -1;
	}

	// test IMM connection used to handle PRIVATE objects
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_1(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immDataHandler(immConnHandlerObj_ptr);
	immDataHandler.set_node(my_node.system_name);
	int imm_op_result = immDataHandler.modifyNodeInfo(const_cast<char *>(my_node.node_name), my_node.node_state);
	if(imm_op_result != ACS_CC_SUCCESS)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): error testing IMM private connection ! imm_op_result == %d, cnt == %d, last_internal_error == %d"), imm_op_result, tmp_val, immConnHandlerObj_ptr->getInternalLastError());

		if((imm_op_result == acs_dsd_imm::ERR_IMM_RUNTIME_MODIFY) && (immConnHandlerObj_ptr->getInternalLastError() == -6/*ACS_APGCC_ERR_IMM_TRY_AGAIN*/))
		{
			// we've got an IMM temporary failure
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): temporary failure on IMM private connection, ! CNT == < %d >"), 1 + tmp_val);
			if((1 + tmp_val) <= IMM_MAX_TEMP_FAILURES)
			{
				// reassign temporary failures counter to the correct value
				imm_op_temporary_failures_cnt = 1 + tmp_val;
				return 4;	// no action. Hope the next check is successful
			}
		}

		// a not temporary error has occurred (or max temporary faults have been reached). We assume that both IMM connections are invalid and try to restore them.
		imm_shared_conn_restore_pending = true;

		// reinitialize the IMM connection used to handle PRIVATE objects
		char dsd_imm_private_name[1024];
		snprintf(dsd_imm_private_name,ACS_DSD_ARRAY_SIZE(dsd_imm_private_name), "%s_%s", DSD_SERVER_IMM_PREFIX, my_node.node_name);
		dsd_imm_private_name[ACS_DSD_ARRAY_SIZE(dsd_imm_private_name) - 1] = 0;
		if((immConnHandlerObj_ptr->finalize() != ACS_CC_SUCCESS) && (immConnHandlerObj_ptr->getInternalLastError() != -9 /* ACS_APGCC_ERR_IMM_BAD_HANDLE */)) //Fix for TR HY11623
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): failure finalizing IMM private connection ! Error descr: %s"), immConnHandlerObj_ptr->getInternalLastErrorText());
			return -2;
		}
		if(immConnHandlerObj_ptr->init(dsd_imm_private_name) != ACS_CC_SUCCESS)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): failure initializing IMM private connection ! Error descr: %s"), immConnHandlerObj_ptr->getInternalLastErrorText());
			return -3;
		}

		// IMM private connection has been successfully reinitialized
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections(): IMM private connection has been reinitialized"));
		immConnHandlerObj_ptr->setImmConnState(ACS_DSD_ImmConnectionHandler::GOOD);
		retval = 1;

		// test again the IMM connection
		immDataHandler.set_node(my_node.system_name);
		imm_op_result = immDataHandler.modifyNodeInfo(const_cast<char *>(my_node.node_name), my_node.node_state);
		if(imm_op_result != ACS_CC_SUCCESS)
		{
			// modify failed. Try node creation (Maybe that the original node has been destroyed by an IMM failure)
			immDataHandler.set_state(my_node.node_state);
			immDataHandler.set_side(my_node.node_side);
			immDataHandler.set_node(my_node.system_name);
			imm_op_result = immDataHandler.addNodeInfo(const_cast<char *>(my_node.node_name));
			if(imm_op_result < 0)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): failure trying to re-create IMM object for local AP node  ! op_result: %d"), imm_op_result);
				return -4;	// nothing to to ! IMM connection is still DOWN
			}
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections(): the IMM object for local AP node has been successfully re-created"));
			retval = 2;
		}
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections(): IMM private connection is working fine !"));

		// release control of IMM connection used to handle PRIVATE objects
		guard_1.release();
	}

	if(imm_shared_conn_restore_pending)
	{
		// reinitialize the IMM connection used to handle SHARED objects
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_2(imm_shared_objects_conn_mutex);
		if(is_imm_shared_objects_controller)
		{
			if((imm_shared_objects_conn_ptr->finalize() != ACS_CC_SUCCESS) && (imm_shared_objects_conn_ptr->getInternalLastError() != -9)) //Fix for TR HY11623
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): failure finalizing IMM shared connection ! Error descr: %s"), imm_shared_objects_conn_ptr->getInternalLastErrorText());
				return -5;
			}

			if( imm_shared_objects_conn_ptr->init(DSD_SERVER_IMM_PREFIX) != ACS_CC_SUCCESS)
			{
				ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections(): failure initializing IMM shared connection ! Error descr: %s"), imm_shared_objects_conn_ptr->getInternalLastErrorText());
				return -6;
			}

			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections(): IMM shared connection has been reinitialized"));
			if(retval == 0)
				retval = 3;
		}

		imm_shared_conn_restore_pending = false;
	}

	// the DSD Server instance running on ACTIVE node must be the controller of IMM SHARED OBHECTS. Check this condition and eventually try to fix it
	if(!is_imm_shared_objects_controller && (my_node.node_state == acs_dsd::NODE_STATE_ACTIVE))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("check_and_restore_IMM_connections(): NODE is ACTIVE but this DSD Server is not the controller of IMM SHARED OBJECTS. Trying to fix  ... !"));
		int nRetry = 3;
		int op_res = take_control_of_IMM_shared_objects(nRetry);
		if(op_res == 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'take_control_of_IMM_shared_objects()' operation successfully completed !"));
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'take_control_of_IMM_shared_objects()' operation failed ! op_res == < %d >"), op_res);

	}
	else if(is_imm_shared_objects_controller && (my_node.node_state == acs_dsd::NODE_STATE_PASSIVE))
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("check_and_restore_IMM_connections(): NODE is PASSIVE but this DSD Server is still the controller of IMM SHARED OBJECTS. Trying to fix  ... !"));
		int nRetry = 3;
		int op_res = release_control_of_IMM_shared_objects(nRetry);
		if(op_res == 0)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'release_control_of_IMM_shared_objects()' operation successfully completed !"));
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("check_and_restore_IMM_connections():  'release_control_of_IMM_shared_objects()' operation failed ! op_res == < %d >"), op_res);
	}

	return retval;
}



int update_local_node_state_in_IMM(){

	// get local AP node info
	ACS_DSD_Node my_node;
	std::string error_descr;
	if(ACS_DSD_Utils::get_node_info(my_node, error_descr) < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("update_local_node_state_in_IMM(): ACS_DSD_Utils::get_node_info() failed ! Error descr: %s"), error_descr.c_str());
		return -1;
	}
	// update node state in IMM
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immDataHandler(immConnHandlerObj_ptr);
	int imm_op_result = immDataHandler.modifyNodeInfo(const_cast<char *>(my_node.node_name), my_node.node_state);
	if(imm_op_result != ACS_CC_SUCCESS)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ModifyNodeInfo(): error in modifying node state info  imm_op_result == %d, last_internal_error == %d"), imm_op_result, immConnHandlerObj_ptr->getInternalLastError());
	}
	guard.release();

	return imm_op_result;

}


int check_and_restore_imm_conn_object(ACS_DSD_ImmConnectionHandler *& imm_conn_handler_p, ACE_Recursive_Thread_Mutex * imm_conn_mutex_p, bool do_init)
{
	int retval = 0;
	bool is_imm_conn_private = 0;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s(): checking integrity of IMM connection object (%p)"), __func__ , imm_conn_handler_p);

	if(!imm_conn_handler_p)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s(): invalid IMM connection pointer ! imm_conn_handler_p == NULL "), __func__);
		return -1;
	}

	is_imm_conn_private = (imm_conn_handler_p == immConnHandlerObj_ptr);

	if( ! is_imm_conn_private && (imm_conn_handler_p != imm_shared_objects_conn_ptr) )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s(): invalid IMM connection pointer ! It MUST be equal to one of the two global IMM connection objects ( IMM PRIVATE OBJECTS conn or IMM SHARED OBJECTS conn"), __func__);
		return -1;
	}

	if(!imm_conn_mutex_p)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s(): invalid IMM mutex pointer ! imm_conn_mutex_p == NULL "), __func__ );
		return -1;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(*imm_conn_mutex_p);
	if(!guard.locked())
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s(): unable to acquire lock on IMM connection mutex (%p)! imm_conn_handler_p == %p "), __func__ , imm_conn_mutex_p, imm_conn_handler_p);
		return -2;
	}

	if(!imm_conn_handler_p->good())
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s(): IMM connection object (%p) is NOT GOOD. Trying to restore it ... !"), __func__ , imm_conn_handler_p);

		// Step 1: finalize the connection
		if(imm_conn_handler_p->finalize() != ACS_CC_SUCCESS)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s(): failure finalizing IMM connection (%p) ! Error descr: %s"), __func__ , imm_conn_handler_p, imm_conn_handler_p->getInternalLastErrorText());
		else
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s(): IMM connection (%p) successfully finalized (restore case)!"), __func__ , imm_conn_handler_p);

		// Step 2: destroy the IMM object
		imm_conn_handler_p->~ACS_DSD_ImmConnectionHandler();

		// Step3 : recreate the IMM object
		imm_conn_handler_p = new (imm_conn_handler_p) ACS_DSD_ImmConnectionHandler();

		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s(): IMM connection object (%p) successfully recreated ! "), __func__ , imm_conn_handler_p);

		if(do_init)
		{
			// Step 4 : init the IMM object
			char oi_name[1024]={0};
			if(is_imm_conn_private)
				snprintf(oi_name, 1024, "%s_%s", DSD_SERVER_IMM_PREFIX, dsd_local_node.node_name);
			else
				snprintf(oi_name, 1024, "%s", DSD_SERVER_IMM_PREFIX);

			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("%s(): IMM connection object (%p) - registering with IMM using implementer name < %s > .... "), __func__ , imm_conn_handler_p, oi_name);
			//Start for TR HY11623
			int retryCounter=0;
			ACS_CC_ReturnType imm_init_res = ACS_CC_SUCCESS;
			do{
				imm_init_res = imm_conn_handler_p->init(oi_name);
				if(imm_init_res != ACS_CC_SUCCESS)
				{
					ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s() : failure initializing IMM connection (%p) !Error descr: %s"), __func__ , imm_conn_handler_p,imm_conn_handler_p->getInternalLastErrorText());
					retryCounter++;
					usleep(250000);
				}
			}while((imm_init_res != ACS_CC_SUCCESS) && retryCounter<MAX_IMM_INIT_RETRY);

			if(retryCounter==MAX_IMM_INIT_RETRY){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s() : init() failed even after 40 retries."), __func__);
			//End of TR HY11623
			return -3;
			}
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s() : IMM connection object (%p) - successfully registered with IMM."), __func__, imm_conn_handler_p);
		}

		retval = 1;
	}
	else
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s(): IMM connection object (%p) is GOOD "), __func__ , imm_conn_handler_p);

	return retval;
}


