#ifndef HEADER_GUARD_CLASS__acs_apbm_snmptrapmessagehandler
#define HEADER_GUARD_CLASS__acs_apbm_snmptrapmessagehandler acs_apbm_snmptrapmessagehandler

/** @file acs_apbm_snmptrapmessagehandler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-02-15
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
 *	| R-001 | 2012-02-15 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ACS_TRAPDS_StructVariable.h>

#include <ace/Thread_Mutex.h> //newly added

/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_snmptrapmessagehandler


/** @class acs_apbm_snmptrapmessagehandler acs_apbm_snmptrapmessagehandler.h
 *	@brief acs_apbm_snmptrapmessagehandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-02-15
 *
 *	acs_apbm_snmptrapmessagehandler <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==========//
	// Typedefs //
	//==========//
	typedef std::list<ACS_TRAPDS_varlist> trapds_var_list_t;


	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_snmptrapmessagehandler constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set) : _server_working_set(server_working_set) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_snmptrapmessagehandler Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	int handle_trap (ACS_TRAPDS_StructVariable & var);

	int handle_xshmcShelfMgrCtrl (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end, const char * switch_board_ip) const;

	int handle_BoardPresenceTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end, const char * switch_board_ip) const;

	int handle_SensorStateChangeTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const;

	int handle_BSPBoardPresenceTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const;

        int handle_BSPSensorStateChangeTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const;

        int handle_BSPTemperatureChangeTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const;

        int handle_SELEntryTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const;

	int handle_IpmiUpgradeResultTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end) const;

	int handle_ColdStartTrap(trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end, const char * switch_board_ip) const;

	inline int handle_WarmStartTrap (trapds_var_list_t::iterator & var_it, trapds_var_list_t::iterator & var_list_end, const char * switch_board_ip) const  {
		return  handle_ColdStartTrap(var_it, var_list_end, switch_board_ip);
	}


protected:
	void oid_to_string (char * buf, size_t buf_size, const oid * name, size_t name_size) const;


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	acs_apbm_serverworkingset * _server_working_set;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_snmptrapmessagehandler
