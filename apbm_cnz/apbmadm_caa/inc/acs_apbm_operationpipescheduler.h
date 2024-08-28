#ifndef HEADER_GUARD_CLASS__acs_apbm_operationpipescheduler
#define HEADER_GUARD_CLASS__acs_apbm_operationpipescheduler acs_apbm_operationpipescheduler

/** @file acs_apbm_operationpipescheduler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-01-18
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
 *	| R-001 | 2012-01-18 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <time.h>

#include <map>
#include <set>
#include <vector>

#include <ace/Event_Handler.h>
#include <ace/Recursive_Thread_Mutex.h>

#include "acs_apbm_programconstants.h"


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_operationpipescheduler


/** @class acs_apbm_operationpipescheduler acs_apbm_operationpipescheduler.h
 *	@brief acs_apbm_operationpipescheduler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-01-18
 *
 *	acs_apbm_operationpipescheduler <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACE_Event_Handler {
	//==========//
	// Typedefs //
	//==========//
public:
	typedef uint32_t tick_t;


	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_operationpipescheduler constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	: ACE_Event_Handler(), _server_working_set(server_working_set), _reactor(0), _free_operation_vectors(),
	  _tick_operations(), _tick_counter(0), _sync_object() {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_operationpipescheduler Destructor
	 */
	inline virtual ~__CLASS_NAME__ () { stop(); }


	//===========//
	// Functions //
	//===========//
public:

	// BEGIN: ACE_Event_Handler interface //
	virtual int handle_timeout (const ACE_Time_Value & current_time, const void * act = 0);

	virtual int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	// END: ACE_Event_Handler interface //

//	int start (ACE_Reactor * reactor, time_t delay, time_t interval);
	int start (ACE_Reactor * reactor, unsigned delay_ms, unsigned interval_ms);

	int stop ();

	int schedule_operation (acs_apbm::ServerWorkOperationCodeConstants op, tick_t ticks_to_wait);

	void get_current_tick_counter(tick_t &currentTicksValue);

private:
	void free_objects ();


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
	ACE_Reactor * _reactor;

	typedef std::vector<acs_apbm::ServerWorkOperationCodeConstants> operation_vector_t;
	typedef operation_vector_t * operation_vector_pointer_t;
	typedef std::map<tick_t, operation_vector_pointer_t> tick_operations_map_t;
	typedef std::vector<operation_vector_pointer_t> operation_vectors_t;

	operation_vectors_t _free_operation_vectors;
	tick_operations_map_t _tick_operations;

	tick_t _tick_counter;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_operationpipescheduler
