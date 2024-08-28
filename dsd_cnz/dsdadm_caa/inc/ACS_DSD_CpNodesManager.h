#ifndef ACS_DSD_CPNODESMANAGER_H_
#define ACS_DSD_CPNODESMANAGER_H_

/** @file ACS_DSD_CpNodesManager.h
 *	@brief
 *	@author xludesi
 *	@date 2010-12-21
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
 *	| 0.1    | 2010-12-21 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <string>
//#include <time.h>

#include <utility>
#include <map>

#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Time_Value_T.h"
#include "ace/Monotonic_Time_Policy.h"

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_CpProt_SvcHandler.h"
#include "ACS_DSD_MacrosConstants.h"
#include "ACS_DSD_CommonTypes.h"

/// Helper class used to represent general info about a single CP Node
class CpNodeItem
{
public:
	// constructors
	CpNodeItem();
	CpNodeItem(const CpNodeItem & rhs);
	CpNodeItem(const char *node_name, int32_t system_id, acs_dsd::SystemTypeConstants system_type, acs_dsd::NodeSideConstants node_side, acs_dsd::NodeStateConstants node_state);

	// overload of assignment operator
	CpNodeItem & operator=(const CpNodeItem & rhs);

	char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX];						/// name of the CP node (for example: "CP1A")
	int32_t system_id;														/// system id of the CP
	acs_dsd::SystemTypeConstants system_type;								/// system type (BC or CP)
	acs_dsd::NodeSideConstants node_side;									/// side of the CP node
	acs_dsd::NodeStateConstants node_state;									/// state of the CP node  (EXECUTION / STANDBY / UNDEFINED)
	ACS_DSD_CpProt_SvcHandler *svc_handlers[2];								/// connection handlers associated to the CP node (max 2)
};


/// Helper Class used to keep information about IMM updates related to a single CP node
class CpNode_imm_update_info
{
public:
	CpNode_imm_update_info(const char * cp_node_name);

	char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX];		/// CP Node name
	uint32_t n_persist_state_updates;						/// number of consecutive IMM updates occurred without changing the list of services represented

//	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> last_update_time;								/// time of last IMM update
	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> next_scheduled_update_time;						/// time of the next IMM update
};


/** @class ACS_DSD_CpNodesManager ACS_DSD_CpNodesManager.h
 *	@brief Used to manage CP Nodes
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-21
 *	@version 0.1
 *
 *	This class is used to keep information about CP Nodes that sent to DSD Daemon Start up requests
 *
 */
class ACS_DSD_CpNodesManager
{
	friend class ACS_DSD_ImmCpNodesUpdater;

public:
	typedef std::map<std::string,CpNodeItem *> CpNodesCollection;
	typedef std::map<int32_t /* cpID*/, std::list<ACS_DSD_ServiceHandler *> > NotifRequestList;
	typedef std::map<std::string, CpNode_imm_update_info *> CpNodesIMMUpdateInfoCollection;

	enum OP_Result {
		CPNM_OK = 0,
		CPNM_INVALID_NODE_NAME = -1,
		CPNM_NODE_ADD_SYNC_ERR = -2,
		CPNM_NODE_ADD_GENERIC_ERR = -3,
		CPNM_NODE_NOT_FOUND = -4,
		CPNM_CONN_NOT_FOUND = -5,
		CPNM_GETSTATE_ERROR = -6,
		CPNM_CONN_TO_CP_FAILURE = -7,
		CPNM_SEND_LISTAPPL_FAILURE = -8,
		CPNM_RECV_LISTAPPL_INVALID_REPLY = -9,
		CPNM_RECV_LISTAPPL_FAILURE = -10,
		CPNM_RECV_LISTAPPL_TIMEOUT = -11,
		CPNM_MEMORY_FAILURE = -12,
		CPNM_IMM_ERROR = -13,
		CPNM_IMM_PARTIAL_UPDATE = -14,
		CPNM_NOTIFICATION_ADD_ERR = -15,
		CPNM_IMM_UPDATE_INFO_ENTRY_ALREADY_EXISTS = -16,
		CPNM_IMM_UPDATE_INFO_ENTRY_NOT_FOUND = -17,
		CPNM_SEND_NOTIFICATIONIND_FAILURE = -18,
		CPNM_NOT_ENOUGH_SPACE = -19,
		CPNM_IMM_OP_NOT_PERMITTED = -20,
		CPNM_IMM_DSD_ROOT_SEARCH_ERROR = -21,
		CPNM_NEW_MASTER_NOT_FOUND = -22
	};


	/** @brief ACS_DSD_CpNodesManager constructor
	 *
	 *	ACS_DSD_CpNodesManager constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	ACS_DSD_CpNodesManager();


	 /** @brief get_cp_node_services( method
	 *
	 *	get_cp_node_services method detailed description
	 *
	 *	@param[in] cp_node_name Description
	 *	@param[out] service_list Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static OP_Result get_cp_node_services(const char *cp_node_name, ServiceInfoList & service_list);


	 /** @brief  set_cp_conn method
	 *
	 *	 set_cp_conn method detailed description
	 *
	 *	@param[in] cp_node_name Description
	 *	@param[in] cp_system_id Description
	 *	@param[in] cp_system_type Description
	 *	@param[in] cp_node_side Description
	 *	@param[in] cp_node_state Description
	 *	@param[in] cp_prot_handler Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	OP_Result set_cp_conn(const char * cp_node_name, int32_t cp_system_id, acs_dsd::SystemTypeConstants cp_system_type, acs_dsd::NodeSideConstants cp_node_side, acs_dsd::NodeStateConstants cp_node_state, ACS_DSD_CpProt_SvcHandler * cp_prot_handler);


	 /** @brief rem_cp_conn method
	 *
	 *	rem_cp_conn method detailed description
	 *
	 *	@param[in] cp_node_name Description
	 *	@param[in] cp_prot_handler Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	OP_Result remove_cp_conn(const char * cp_node_name, ACS_DSD_CpProt_SvcHandler * cp_prot_handler);


	 /** @brief close_cp_conn method
	 *
	 *	close_cp_conn method detailed description
	 *
	 *	@param[in] cp_node_name Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	OP_Result close_cp_conn(const char * cp_node_name);


	/** @brief get_alarm_master_cp method
	 *
	 *	get_alarm_master_cp method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int32_t get_alarm_master_cp_system_id();

	/** @brief get_clock_master_cp method
	 *
	 *	get_clock_master_cp method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int32_t get_clock_master_cp_system_id();


	 /** @brief find_cp_node_by_systemId_and_state method
	 *
	 *	find_cp_node_by_systemId_and_state detailed description
	 *
	 *	@param[in] cp_system_id Description
	 *	@param[in] cp_node_state Description
	 *	@param[out] found_cp_node_name Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	OP_Result find_cp_node_by_systemId_and_state(int32_t cp_system_id, acs_dsd::NodeStateConstants cp_node_state,  char found_cp_node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]);

	 /** @brief get_cp_node_state method
		 *
		 *	get_cp_node_state method detailed description
		 *
		 *	@param[in] cp_node_name Description
		 *	@param[in] cp_node_state Description
		 *	@return Return Description
		 *	@remarks
	 */

	OP_Result get_cp_node_state(const char * cp_node_name, acs_dsd::NodeStateConstants & cp_node_state);


	 /** @brief get_cp_system_state method
	 *
	 *	get_cp_system_state method detailed description
	 *
	 *	@param[in] cp_system_id Description
	 *	@param[in] cp_system_state Description
	 *	@return Return Description
	 *	@remarks
	 *	algorithm (A --> CP SIDE A;  B --> CP SIDE B)
	 *
	 *	 A --> EX	=>		B = SB				SEPARATED
	 *	 A --> EX	=>		B there isn't		NORMAL
	 *	 A --> SB	=>		B = EX				SEPARATED
	 *	 A --> SB	=>		B there isn't		SEPARATED
	 *	 A --> there isn't	B = EX				NORMAL
	 *	 A --> there isn't 	B = SB				SEPARATED
	 *
	 *	in all other cases --> UNDEFINED
	 */
	OP_Result get_cp_system_state(int32_t cp_system_id, acs_dsd::CPStateConstants & cp_system_state);

	/// returns a reference to the mutex used to obtain exclusive access to the internal CP nodes collection
	ACE_Recursive_Thread_Mutex & synch_mutex() { return _mutex; };


	 /** @brief notification_is_needed method
	 *
	 *	notification_needed method detailed description
	 *
	 *	@param[in] cp_node_name Description
	 *	@param[in] cp_system_id Description
	 *	@param[in] cp_system_type Description
	 *	@param[in] cp_node_side Description
	 *	@param[in] cp_node_state Description
	 *	@param[out] out_cp_system_state Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	bool notification_needed(const char * cp_node_name, int32_t cp_system_id, acs_dsd::SystemTypeConstants cp_system_type, acs_dsd::NodeSideConstants cp_node_side,acs_dsd::NodeStateConstants cp_node_state, acs_dsd::CPStateConstants & out_cp_system_state );

	int addNotification(int32_t sysId, ACS_DSD_ServiceHandler *pServiceHandler);
	int removeNotification(int32_t sysId, ACS_DSD_ServiceHandler *pServiceHandler);
	int sendNotificationRsp(ACS_DSD_ServiceHandler *pServiceHandler, uint8_t cp_state);
	int notifyCpStateChange(int32_t sysId, uint8_t cp_state);


	OP_Result get_nodes (acs_dsd::SystemTypeConstants system_type, ACS_DSD_Node * nodes, size_t & size) const;


	/** @brief synch_IMM method
	 *
	 *	This method is used to synchronize DSD IMM model with the CP Nodes internal collection. This means that, after a successful execution,
	 *	there will be, in IMM, an SRTNode entry for every CP Node that is present in the CP Nodes internal collection.
	 *
	 *	@param[out] n_nodes_added Description
	 *	@param[out] n_nodes_removed Description
	 *	@return Return Description
	 *	@remarks
	 */
	OP_Result synch_IMM(int32_t & n_nodes_added, int32_t & n_nodes_removed);

	/** @brief get_effective_cp_system_id method
	 *
	 *	get the 'effective system id' corresponding to the passed system id (special system id are taken into account)
	 *
	 *	@param[in] system_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int32_t get_effective_cp_system_id(int32_t cp_system_id);

	/** @brief choose_master_cp method
	 *
	 *	Choose the master CP between the connected CPs
	 *
	 *	@param[in] system_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	OP_Result choose_master_cp ();

private:
	/** @brief is_good_master method
	 *
	 *	is_good_master method detailed description
	 *
	 *	@param[in] cp_node_name Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	bool is_good_master(int system_id);


	/** @brief update_IMM_for_cp_node method
	 *
	 *	update_IMM_for_cp_node method detailed description
	 *
	 *	@param[in] cp_node_name Description
	 *	@param[in] service_list Description
	 *	@param[out] n_serv_added Description
	 *	@param[out] n_serv_modified Description
	 *	@param[out] n_serv_removed Description
	 *	@return Return Description
	 *	@remarks
	 */
	OP_Result update_IMM_cp_node(const char * cp_node_name, const ServiceInfoList & service_list, int32_t & n_serv_added, int32_t & n_serv_modified, int32_t & n_serv_removed);


	/// object used to make the class thread-safe
	mutable ACE_Recursive_Thread_Mutex _mutex;
	ACE_Recursive_Thread_Mutex _notifReq_mutex;

	/// CP Nodes connected (association : CP Node name --> CP Node info)
	CpNodesCollection _connected_cp_nodes;

	/// if != -1, indicates the Alarm master CP
	int32_t _alarm_master_sys_id;

	/// indicates if the Alarm master CP has been set into CS
	bool _alarm_master_CS_notified;

	/// if != -1, indicates the Clock master CP
	int32_t _clock_master_sys_id;

	/// indicates if the Clock Master CP has been set into CS
	bool _clock_master_CS_notified;

	// Notification request received (the map holds the association between the cpId and the list of ServiceHandlers
	// that required to be notified the cpId's state change
	NotifRequestList _received_notifRequest;

	ACE_Recursive_Thread_Mutex _update_info_list_mutex;
	CpNodesIMMUpdateInfoCollection _cp_node_imm_update_info_collection;
};

#endif /* ACS_DSD_CPNODESMANAGER_H_ */
