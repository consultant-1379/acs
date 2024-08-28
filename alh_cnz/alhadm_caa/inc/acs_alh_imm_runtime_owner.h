/*
 * acs_alh_imm_util.h
 *
 *  Created on: Nov 8, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_IMM_RUNTIME_OWNER_H_
#define ACS_ALH_IMM_RUNTIME_OWNER_H_


/** @file acs_alh_imm_runtime_owner
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2011-11-04
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 * DESCRIPTION
 *	The acs_alh_imm_runtime_owner class is responsible to handle the imm connection OI side
 *
 * ERROR HANDLING
 * -
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-04 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */


#include "acs_alh_common.h"
#include "acs_alh_util.h"
#include "acs_alh_log.h"
#include "acs_alh_error.h"
#include "acs_alh_imm_connection_handler.h"

//========================================================================================
//	Class declarations for acs_alh_imm_runtime_owner
//========================================================================================
class acs_alh_imm_runtime_owner {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief Default constructor
	 *
	 *	Constructor of class
	 *
	 *	@remarks
	 *
	 */
	acs_alh_imm_runtime_owner(acs_alh_imm_connection_handler *immConnHandler);


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	~acs_alh_imm_runtime_owner();



	//===========//
	// Functions //
	//===========//

	/** @brief
	 *	Initializes alhd as Runtime Owner of AlarmInfo Object
	 *
	 *	@return int: error code
	 *
	 *	@remarks	-
	 */
	int init_IMM(bool cleanImmStructure = false);



	// finalize DSD Server's IMM components
	/** @brief
	 *	Finalize alhd as Runtime Owner of AlarmInfo Object
	 *
	 *	@return int: error code
	 *
	 *	@remarks	-
	 */
	int finalize_IMM();

private:

	acs_alh_util util_;

	acs_alh_log log_;

	acs_alh_imm_connection_handler *immConnHandlerObj_;
};



#endif /* ACS_ALH_IMM_RUNTIME_OWNER_H_ */
