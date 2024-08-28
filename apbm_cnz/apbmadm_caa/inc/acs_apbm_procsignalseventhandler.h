#ifndef HEADER_GUARD_CLASS__acs_apbm_procsignalseventhandler
#define HEADER_GUARD_CLASS__acs_apbm_procsignalseventhandler acs_apbm_procsignalseventhandler

/** @file acs_apbm_procsignalseventhandler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-08-05
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
 *	| R-001 | 2011-08-05 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <ace/Event_Handler.h>

/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_procsignalseventhandler

/** @class acs_apbm_procsignalseventhandler acs_apbm_procsignalseventhandler.h
 *	@brief acs_apbm_procsignalseventhandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-08-05
 *
 *	acs_apbm_procsignalseventhandler <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACE_Event_Handler {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_procsignalseventhandler constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	: ACE_Event_Handler(), _server_working_set(server_working_set) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_procsignalseventhandler Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}

	//=================//
	// Field Accessors //
	//=================//
public:

	//===========//
	// Functions //
	//===========//
public:

	// BEGIN: ACE_Event_Handler interface //
  virtual int handle_signal (int signum, siginfo_t * = 0, ucontext_t * = 0);
	// END: ACE_Event_Handler interface //

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

#endif // HEADER_GUARD_CLASS__acs_apbm_procsignalseventhandler
