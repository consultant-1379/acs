#ifndef HEADER_GUARD_CLASS__acs_apbm_switchboardinfo
#define HEADER_GUARD_CLASS__acs_apbm_switchboardinfo acs_apbm_switchboardinfo

/** @file acs_apbm_switchboardinfo.h
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
 *      |       | 2014-06-17 | xsunach	    | TR HS54933                          |
 *      +=======+============+==============+=====================================+
 */


#include <stdint.h>

#include <acs_apbm_programconstants.h>
#include "acs_apbm_snmpconstants.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_switchboardinfo


/** @class acs_apbm_switchboardinfo acs_apbm_switchboardinfo.h
 *	@brief acs_apbm_switchboardinfo class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-10-05
 *
 *	acs_apbm_switchboardinfo <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_switchboardinfo constructor
	 */
	inline __CLASS_NAME__ ()
	: slot_position(acs_apbm::BOARD_SLOT_UNDEFINED), magazine(acs_apbm::BOARD_MAGAZINE_UNDEFINED),
	  ipna_str(), ipnb_str(), state(-1), neighbour_state(-1), snmp_link_down_time(0),trap_subscription_state(false) /*, coldstart_cnt(0)*/ {
		*ipna_str = 0;
		*ipnb_str = 0;
		*ipaliasa_str = 0;
		*ipaliasb_str = 0;
	}

private:
	// Use the default copy constructor
	//__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_switchboardinfo Destructor
	 */
	inline ~__CLASS_NAME__ () {}

public:
	//===========//
	// Functions //
	//===========//



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

	char ipna_str [16];
	char ipnb_str [16];
	char ipaliasa_str [16];
	char ipaliasb_str [16];

//	acs_apbm_snmp::switch_board_shelf_mgr_state_t state;
//	acs_apbm_snmp::switch_board_neighbour_state_t neighbour_state;
	int state;
	int neighbour_state;

//	uint8_t  coldstart_cnt; // Removed because not used anymore

	// store the time of detected link down
	int64_t snmp_link_down_time;

	bool trap_subscription_state;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_switchboardinfo
