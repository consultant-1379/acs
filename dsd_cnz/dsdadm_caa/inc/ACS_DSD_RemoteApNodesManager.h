#ifndef HEADER_GUARD_CLASS__ACS_DSD_RemoteApNodesManager
#define HEADER_GUARD_CLASS__ACS_DSD_RemoteApNodesManager ACS_DSD_RemoteApNodesManager

/** @file ACS_DSD_RemoteApNodesManager.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2013-05-07
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
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2013-05-07 | xcasale      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <map>

#include "ace/Recursive_Thread_Mutex.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_RemoteApNodesManager


/** @class ACS_DSD_RemoteApNodesManager ACS_DSD_RemoteApNodesManager.h
 *	@brief ACS_DSD_RemoteApNodesManager class
 *	@author xcasale (Alessio Cascone)
 *	@date 2013-04-30
 *
 *	ACS_DSD_RemoteApNodesManager <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {

	//==============//
	// Enumerations //
	//==============//
public:
	enum RemoteApNodesManagerErrorConstants {
			APNM_ERROR_NO_ERROR = 0,
			APNM_ERROR_ITEM_ALREADY_PRESENT,
			APNM_ERROR_ITEM_NOT_PRESENT,
			APNM_ERROR_NULL_PARAMETER
	};

	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_RemoteApNodesManager constructor
	 */
	inline __CLASS_NAME__ () : _remote_ap_nodes(), _mutex() {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_RemoteApNodesManager Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	int add_remote_ap_node_connection (int32_t system_id, ACS_DSD_SrvProt_SvcHandler * service_handler);
	int remove_remote_ap_node_connection (int32_t system_id);
	int remove_all_ap_nodes_connections ();


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	std::map <int32_t, ACS_DSD_SrvProt_SvcHandler *> _remote_ap_nodes;
	ACE_Recursive_Thread_Mutex _mutex;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_RemoteApNodesManager
