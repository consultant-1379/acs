#ifndef HEADER_GUARD_CLASS__acs_apbm_csboardinfo
#define HEADER_GUARD_CLASS__acs_apbm_csboardinfo acs_apbm_csboardinfo

/** @file acs_apbm_csboardinfo.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-10-05
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
 *	| R-001 | 2011-10-05 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <stdint.h>

#include <acs_apbm_programconstants.h>
#include "acs_apbm_snmpconstants.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_csboardinfo


/** @class acs_apbm_csboardinfo acs_apbm_csboardinfo.h
 *	@brief acs_apbm_csboardinfo class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-10-05
 *
 *	acs_apbm_csboardinfo <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_csboardinfo constructor
	 */
	inline __CLASS_NAME__ ()
	: slot_position(acs_apbm::BOARD_SLOT_UNDEFINED), magazine(acs_apbm::BOARD_MAGAZINE_UNDEFINED), fbn(0) {}

private:
	// Use the default copy constructor
	//__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_csboardinfo Destructor
	 */
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:


	//===========//
	// Operators //
	//===========//
private:
	// Use the default assignment operator
	//__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
public:
	int32_t slot_position;
	uint32_t magazine;
	uint16_t fbn;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_csboardinfo
