#ifndef HEADER_GUARD_CLASS__acs_apbm_sanotifiereventhandler
#define HEADER_GUARD_CLASS__acs_apbm_sanotifiereventhandler acs_apbm_sanotifiereventhandler

/** @file acs_apbm_sanotifiereventhandler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-12-02
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
 *	| R-001 | 2011-12-02 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <ace/Event_Handler.h>

//#include "saAis.h"
//#include "saAmf.h"
#include <saNtf.h>
//#include "saClm.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_sanotifiereventhandler


/** @class acs_apbm_sanotifiereventhandler acs_apbm_sanotifiereventhandler.h
 *	@brief acs_apbm_sanotifiereventhandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-12-02
 *
 *	acs_apbm_sanotifiereventhandler <PUT DESCRIPTION>
 */
class __CLASS_NAME__ : public ACE_Event_Handler {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_sanotifiereventhandler constructor
	 */
	inline __CLASS_NAME__ () : ACE_Event_Handler(), _handle(ACE_INVALID_HANDLE), _ntf_handle(0) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_sanotifiereventhandler Destructor
	 */
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	/*
	 * BEGIN: ACE_Event_Handler interface implementation
	 */
	inline virtual ACE_HANDLE get_handle () const { return _handle; }

	inline virtual void set_handle (ACE_HANDLE handle) { _handle = handle; }

	virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

	virtual int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	/*
	 * END: ACE_Event_Handler interface implementation
	 */

	inline void set_ntf_handle (SaNtfHandleT handle) { _ntf_handle = handle; }

	inline void set_handles (ACE_HANDLE handle, SaNtfHandleT ntf_handle) { set_handle(handle); set_ntf_handle(ntf_handle); }


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	ACE_HANDLE _handle;
	SaNtfHandleT _ntf_handle;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_sanotifiereventhandler
