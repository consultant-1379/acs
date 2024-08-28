#ifndef HEADER_GUARD_FILE__ACS_DSD_Macros
#define HEADER_GUARD_FILE__ACS_DSD_Macros ACS_DSD_Macros.h

/** @file ACS_DSD_Macros.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-08-12
 *	@version 1.0.0
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
 *	| P0.9.1 | 2010-08-12 | xnicmut      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+========+============+==============+=====================================+
 */

#include <string.h>

#include "ACS_DSD_MacrosConstants.h"
#include "ACS_DSD_MacrosConfig.h"
#include "ACS_DSD_MacrosUtils.h"

namespace acs_dsd {
#	ifndef ACS_DSD_HANDLE_DEFINED
		typedef int HANDLE;
		typedef HANDLE SOCKET_HANDLE;
#		define ACS_DSD_HANDLE_DEFINED 1
#	endif
}

#define ACS_DSD_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

struct ACS_DSD_Node {
	inline ACS_DSD_Node()
	: system_id(acs_dsd::SYSTEM_ID_UNKNOWN), system_name(), system_type(acs_dsd::SYSTEM_TYPE_UNKNOWN), node_state(acs_dsd::NODE_STATE_UNDEFINED),
	  node_name(), node_side(acs_dsd::NODE_SIDE_UNDEFINED) {}

	inline void reset () {
		system_id = acs_dsd::SYSTEM_ID_UNKNOWN;
		*system_name = 0;
		system_type = acs_dsd::SYSTEM_TYPE_UNKNOWN;
		node_state = acs_dsd::NODE_STATE_UNDEFINED;
		*node_name = 0;
		node_side = acs_dsd::NODE_SIDE_UNDEFINED;
	}

	inline void set (int32_t system_id_, const char * system_name_, acs_dsd::SystemTypeConstants system_type_, acs_dsd::NodeStateConstants node_state_, const char * node_name_, acs_dsd::NodeSideConstants node_side_) {
		system_id = system_id_;
		system_name_
				? (memccpy(system_name, system_name_, 0, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX), system_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX - 1] = 0)
				: (*system_name = 0);
		system_type = system_type_;
		node_state = node_state_;
		node_name_
				? (memccpy(node_name, node_name_, 0, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX), node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX - 1] = 0)
				: (*node_name = 0);
		node_side = node_side_;
	}

	int32_t system_id;
	char system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX];
	acs_dsd::SystemTypeConstants system_type;
	acs_dsd::NodeStateConstants node_state;
	char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX];
	acs_dsd::NodeSideConstants node_side;
};

#endif // HEADER_GUARD_FILE__ACS_DSD_Macros
