/*
 * acs_alh_log.h
 *
 *  Created on: Nov 04, 2011
 *      Author: xgiopap
 */

#ifndef ACS_ALH_LOG_H_
#define ACS_ALH_LOG_H_

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
 *	| 0.1    | 2011-11-04 | xgiopap      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_TRA_Logging.h"

class acs_alh_log {
public:

	/** @brief acs_alh_log default constructor
	 *
	 *	The default constructor of the class.
	 *
	 *	@remarks Remarks
	 */
	acs_alh_log();


	/** @brief ~acs_alh_log default destructor
	 *
	 *	The default destructor of the class.
	 *
	 *	@remarks Remarks
	 */
	~acs_alh_log();


	//===========//
	// Functions //
	//===========//

	/** @brief log method
	 *
	 *	trace provides logging utilities
	 *
	 *	@param[in]
	 *
	 *	@return int:	error code.
	 *
	 *	@remarks Remarks
	 */
	int write(ACS_TRA_LogLevel Log_Level, const char* Log_Message, ...);

private:

	ACS_TRA_Logging log_obj_;
};

#endif /* ACS_ALH_LOG_H_ */
