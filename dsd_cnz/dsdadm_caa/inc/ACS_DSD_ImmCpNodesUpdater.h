#ifndef ACS_DSD_IMMCPNODESUPDATER_H_
#define ACS_DSD_IMMCPNODESUPDATER_H_

/** @file ACS_DSD_ImmCpNodesUpdater.h
 *	@brief
 *	@author xludesi
 *	@date 2011-01-08
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
 *	| 0.1    | 2011-01-08 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <string>
#include <list>

#include "ace/Condition_Attributes.h"
#include "ace/Monotonic_Time_Policy.h"
#include "ace/Thread_Mutex.h"
#include "ace/Condition_T.h"
#include "ace/Task.h"

#include "ACS_DSD_CpNodesManager.h"

class ACS_DSD_ImmCpNodesUpdater: public ACE_Task_Base
{
public:
	/// Constructor
	ACS_DSD_ImmCpNodesUpdater(ACS_DSD_CpNodesManager * cp_nodes_manager = 0);


	/** @brief svc method
	 *
	 *	svc method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int svc (void);


	/** @brief start_activity method
	 *
	 *	start_activity method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int start_activity();


	/** @brief start_activity method
	 *
	 *	stop_activity method detailed description
	 *
	 *  @param[in] wait_termination Indicates if the method must wait for thread termination
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int stop_activity(bool wait_termination);


	/** @brief set_cp_nodes_manager method
	 *
	 *	set_cp_nodes_manager method detailed description
	 *
	 *  @param[in] cp_nodes_manager pointer to a ACS_DSD_CPNodesManager object
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_cp_nodes_manager(ACS_DSD_CpNodesManager * cp_nodes_manager) { this->_cp_nodes_manager = cp_nodes_manager; }


	/** @brief start_activity method
	 *
	 *	is_working method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
//	bool is_working() { return _is_working; }

private:
	typedef std::list<std::string> NodeNamesList;

	/// obtain the list of the names of the CP node that must be updated
	void get_cp_nodes_to_update(NodeNamesList & nodes_to_be_updated);

	/// update info about a CP node
	int update_cp_node_info(const char * node_name, int n_serv_added, int n_serv_modified, int n_serv_removed);

	ACS_DSD_CpNodesManager * _cp_nodes_manager;
	bool _bStopped;

	ACE_Condition_Attributes_T<ACE_Monotonic_Time_Policy> _condition_attributes;
	ACE_Thread_Mutex _thread_mutex;
	ACE_Condition<ACE_Thread_Mutex> _stop_working_condition;
};

#endif /* ACS_DSD_IMMCPNODESUPDATER_H_ */
