/*
 * acs_alh_util.h
 *
 *  Created on: Oct 31, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_UTIL_H_
#define ACS_ALH_UTIL_H_

/** @file acs_alh_util
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2011-10-31
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
 *	The acs_alh_util provides method to support other class
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
 *	| 0.1    | 2011-10-31 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_TRA_trace.h"

#define ACS_ALH_TRACE_MESSAGE_SIZE_MAX 4096
#define ACS_ALH_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

class acs_alh_util {
public:

	/** @brief acs_alh_util default constructor
	 *
	 *	The default constructor of the class.
	 *
	 *	@remarks Remarks
	 */
	acs_alh_util(const char* class_name);


	/** @brief ~acs_alh_util default destructor
	 *
	 *	The default destructor of the class.
	 *
	 *	@remarks Remarks
	 */
	~acs_alh_util();


	//===========//
	// Functions //
	//===========//


	/** @brief trace method
	 *
	 *	trace provides trace utilities
	 *
	 *	@param[in]
	 *
	 *	@return int:	error code.
	 *
	 *	@remarks Remarks
	 */
	int trace (const char * format, ...);


	/** @brief get_alh_imm_root_dn method
	 *
	 *	get_alh_imm_root_dn provides the dn of root object of ALH tree
	 *
	 *	@param[out] dn_path:	dn of the root object of ALH tree
	 *	@return int:	error code.
	 *
	 *	@remarks Remarks
	 */
	int get_alh_imm_root_dn(const char * & dn_path);


	/** @brief getlocalNodeName method
	 *
	 *	getlocalNodeName provides the name of local node
	 *
	 *
	 *	@return std::string :	name of local node.
	 *
	 *	@remarks Remarks
	 */
	std::string getlocalNodeName();

private:

	ACS_TRA_trace trace_obj_;
};

#endif /* ACS_ALH_UTIL_H_ */
