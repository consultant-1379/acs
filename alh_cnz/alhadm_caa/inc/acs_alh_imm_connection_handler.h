/*
 * acs_alh_immconnectionhandler.h
 *
 *  Created on: Nov 7, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_IMMCONNECTIONHANDLER_H_
#define ACS_ALH_IMMCONNECTIONHANDLER_H_


/** @file acs_alh_imm_connection_handler
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
 *	The acs_alh_imm_connection_handler class is responsible to manage the IMM connection
 *
 *
 * ERROR HANDLING
 * -
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-07 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */


#include "ACS_APGCC_RuntimeOwner_V2.h"
#include "ACS_CC_Types.h"

class acs_alh_imm_connection_handler : public ACS_APGCC_RuntimeOwner_V2 {

public:

	inline acs_alh_imm_connection_handler(){}
	inline ~acs_alh_imm_connection_handler(){}

	/**Implementation of virtual method **/
	inline ACS_CC_ReturnType updateCallback(const char* /*p_objName*/, const char** /*p_attrName*/){ return (ACS_CC_ReturnType)0;}

	inline void adminOperationCallback(ACS_APGCC_OiHandle /*oiHandle*/,
									ACS_APGCC_InvocationType /*invocation*/,
									const char* /*p_objName*/,
									ACS_APGCC_AdminOperationIdType /*operationId*/,
									ACS_APGCC_AdminOperationParamType** /*paramList*/){}
};

#endif /* ACS_ALH_IMMCONNECTIONHANDLER_H_ */
