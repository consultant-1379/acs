#ifndef HEADER_GUARD_CLASS__acs_apbm_snmptrapdsobserver
#define HEADER_GUARD_CLASS__acs_apbm_snmptrapdsobserver acs_apbm_snmptrapdsobserver

/** @file acs_apbm_snmptrapdsobserver.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-02-14
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
 *	| R-001 | 2012-02-14 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ace/Recursive_Thread_Mutex.h>

#include <ACS_TRAPDS_API.h>


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_snmptrapdsobserver


/** @class acs_apbm_snmptrapdsobserver acs_apbm_snmptrapdsobserver.h
 *	@brief acs_apbm_snmptrapdsobserver class
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-02-14
 *
 *	acs_apbm_snmptrapdsobserver <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACS_TRAPDS_API {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_snmptrapdsobserver constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	: _server_working_set(server_working_set), _subscribed(0), _sync_object() {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_snmptrapdsobserver Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	// BEGIN: ACS_TRAPDS_API Interface
	virtual void handleTrap (ACS_TRAPDS_StructVariable var);
	// END: ACS_TRAPDS_API Interface

	int subscribe (std::string serviceName);
	int unsubscribe ();

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
	uint8_t _subscribed;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_snmptrapdsobserver
