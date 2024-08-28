#ifndef HEADER_GUARD_CLASS__ACS_DSD_StartupSessionsGroup
#define HEADER_GUARD_CLASS__ACS_DSD_StartupSessionsGroup ACS_DSD_StartupSessionsGroup

/** @file ACS_DSD_StartupSessionsGroup.h
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

#include <stdint.h>

#include <vector>
#include <map>

#include "ace/Reactor.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_StartupSessionHandler.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_StartupSessionsGroup


/** @class ACS_DSD_StartupSessionsGroup ACS_DSD_StartupSessionsGroup.h
 *	@brief ACS_DSD_StartupSessionsGroup class
 *	@author xnicmut (Nicola Muto)
 *	@date 2013-04-30
 *
 *	ACS_DSD_StartupSessionsGroup <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACE_Event_Handler {
	//=========//
	// Friends //
	//=========//
friend class ACS_DSD_StartupSessionHandler;


	//==============//
	// Enumerations //
	//==============//
public:
	enum StartupSessionsGroupStateConstants {
		SSG_STATE_DISCONNECTED = 0,
		SSG_STATE_SOME_CONNECTED,
		SSG_STATE_ALL_CONNECTED
	};

	enum StartupSessionsGroupErrorConstants {
		SSG_ERROR_NO_ERROR = 0,
		SSG_ERROR_ALREADY_CONNECTED,
		SSG_ERROR_ALREADY_CLOSED,
		SSG_ERROR_GET_AP_NODES,
		SSG_ERROR_NO_MULTI_AP_SYSTEM,
		SSG_ERROR_SESSION_NOT_FOUND,
		SSG_ERROR_SESSION_IS_NULL,
		SSG_ERROR_SESSION_CLOSE,
		SSG_ERROR_NO_REACTORS,
		SSG_ERROR_REACTOR_REGISTER_HANDLER,
		SSG_ERROR_REACTOR_REMOVE_HANDLER,
		SSG_ERROR_LOAD_CONFIGURATION_FAILED
	};

	enum SystemConfigurationConstants {
		SYS_CONF_UNKNOWN		= -1,
		SYS_CONF_SINGLE_AP	=  0,
		SYS_CONF_MULTI_AP		=  1
		//SYS_CONF_MULTI_AP_STRONG	=  2			// Unused for now.
	};

	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_StartupSessionsGroup constructor
	 */
	inline explicit __CLASS_NAME__ (ACE_Reactor * reactor = 0)
	: _state(SSG_STATE_DISCONNECTED), _other_aps_count(-1), _other_ap_objects(),
	  _reactor(reactor), _mutex(), _system_configuration(SYS_CONF_UNKNOWN)
	{}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_StartupSessionsGroup Destructor
	 */
	inline virtual ~__CLASS_NAME__ () { close(_reactor ? 1 : 0); }


	//==================//
	// Fields Accessors //
	//==================//
public:
	inline StartupSessionsGroupStateConstants state () const { return _state; }

	inline int other_aps_count () const { return _other_aps_count; }

	inline ACE_Reactor * reactor () const { return _reactor; }
	inline void reactor (ACE_Reactor * new_value) { _reactor = new_value; }

	inline int system_configuration () { return __sync_fetch_and_or(&_system_configuration, 0); }
	inline void system_configuration ( SystemConfigurationConstants system_configuration ) {
		__sync_lock_test_and_set(&_system_configuration, system_configuration);
	}

	//=============================//
	// ACE_Event_Handler Interface //
	//=============================//
public:
	virtual int handle_timeout (const ACE_Time_Value & current_time, const void * act = 0);


	//===========//
	// Functions //
	//===========//
public:
	int connect ();

	int close (int remove_from_reactor = 1);

	int close_session (int32_t system_id, int remove_from_reactor = 1);

	int delete_session (int32_t system_id, int remove_from_reactor = 1);

private:

	int close_session_item (ACS_DSD_StartupSessionHandler * ssh, int remove_from_reactor = 1);
	int delete_session_item (ACS_DSD_StartupSessionHandler * ssh, int remove_from_reactor = 1);

	int register_session_to_reactor (ACS_DSD_StartupSessionHandler * ssh);
	int remove_session_from_reactor (ACS_DSD_StartupSessionHandler * ssh);

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	StartupSessionsGroupStateConstants _state;

	int _other_aps_count;

	std::map <
		int32_t,
		std::pair< std::vector<ACS_DSD_ConfigurationHelper::HostInfo>, ACS_DSD_StartupSessionHandler *>
	> _other_ap_objects;

	ACE_Reactor * _reactor;

	ACE_Recursive_Thread_Mutex _mutex;

	int _system_configuration;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_StartupSessionsGroup
