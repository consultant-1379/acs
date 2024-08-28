#ifndef HEADER_GUARD_CLASS__ACS_DSD_StartupSessionHandler
#define HEADER_GUARD_CLASS__ACS_DSD_StartupSessionHandler ACS_DSD_StartupSessionHandler

/** @file ACS_DSD_StartupSessionHandler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2013-04-30
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
 *	| R-001 | 2013-04-30 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <string.h>
#include <stdint.h>

#include <vector>
#include <string>

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"

class ACS_DSD_StartupSessionsGroup;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_StartupSessionHandler


/** @class ACS_DSD_StartupSessionHandler ACS_DSD_StartupSessionHandler.h
 *	@brief ACS_DSD_StartupSessionHandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2013-04-30
 *
 *	ACS_DSD_StartupSessionHandler <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACS_DSD_SrvProt_SvcHandler {
	//==============//
	// Enumerations //
	//==============//
public:
	enum StartupSessionHandlerErrorConstants {
			SSH_ERROR_NO_ERROR = 0,
			SSH_ERROR_ALREADY_CONNECTED,
			SSH_ERROR_IMM_DELETE_FAILED,
			SSH_ERROR_IMM_CREATE_FAILED,
			SSH_ERROR_IMM_UPDATE_FAILED,
			SSH_ERROR_CONNECTION_CLOSE_FAILED,
			SSH_ERROR_CONNECT_FAILED,
			SSH_ERROR_SEND_PRIMITIVE,
			SSH_ERROR_RECV_PRIMITIVE,
			SSH_ERROR_CAST_FAILED,
			SSH_ERROR_PRIMITIVE_WITH_ERROR,
			SSH_ERROR_REMOTE_PARTNER_IS_NOT_ACTIVE,
			SSH_ERROR_REACTOR_REGISTER_FAILED,
			SSH_ERROR_GET_NODE_INFO_FAILED,
			SSH_ERROR_GET_SERVICES_INFO_FAILED,
			SSH_ERROR_GET_DSD_IMM_ROOT_FAILED,
			SSH_ERROR_NOT_CONNECTED,
			SSH_ERROR_UPDATE_SERVICE_LIST_FAILED
	};

	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_StartupSessionHandler constructor
	 */
	inline explicit __CLASS_NAME__ (int32_t system_id, ACS_DSD_StartupSessionsGroup * ssg)
	: ACS_DSD_SrvProt_SvcHandler(), _system_id(system_id), _node_name_active(),
	  _node_name_passive(), _ssg(ssg), _remote_node_names() {
		*_node_name_active = 0;
		*_node_name_passive = 0;
	}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_StartupSessionHandler Destructor
	 */
	inline virtual ~__CLASS_NAME__ () { _remote_node_names.clear(); }


	//=============================//
	// ACE_Event_Handler Interface //
	//=============================//
public:
	virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

	virtual int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);


	//==================================//
	// ACS_DSD_ServiceHandler Interface //
	//==================================//
protected:
	inline virtual int on_connection_close_hook_func () { return 0; }


	//===========//
	// Functions //
	//===========//
public:
	inline int32_t system_id () const { return _system_id; }

	inline const char * node_name_active () const { return _node_name_active; }
	inline void node_name_active (const char * new_value) { set_node_name(_node_name_active, new_value); }

	int connection_open (std::vector<ACS_DSD_ConfigurationHelper::HostInfo> & ap_nodes);

	int connection_close ();

	int update_IMM_services_info();

	int update_remote_nodes_state ();

private:
	int update_IMM_node_info(const char* node_name, const char* system_name, int32_t system_id, acs_dsd::NodeSideConstants node_side, uint8_t node_state);

	inline void set_node_name (char * node_name_field, const char * new_value) {
		new_value ? (::strncpy(node_name_field, new_value, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX),
										node_name_field[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX - 1] = 0)
							: *node_name_field = 0;
	}

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	int32_t _system_id;
	char _node_name_active [acs_dsd::CONFIG_NODE_NAME_SIZE_MAX];
	char _node_name_passive [acs_dsd::CONFIG_NODE_NAME_SIZE_MAX];
	ACS_DSD_StartupSessionsGroup * _ssg;
	std::vector<std::string> _remote_node_names;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_StartupSessionHandler
