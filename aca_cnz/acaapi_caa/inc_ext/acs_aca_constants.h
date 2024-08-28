#ifndef HEADER_GUARD_FILE__acs_aca_constants
#define HEADER_GUARD_FILE__acs_aca_constants acs_aca_constants.h

/** @file acs_aca_constants.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-03-13
 *
 *	COPYRIGHT Ericsson AB, 2014
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
 *	| R-001 | 2014-03-13 | xcasale      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

namespace aca_rtr_communication {
	const int MESSAGE_QUEUE_DEFAULT_FLAGS = 0;
	const int MESSAGE_QUEUE_MAX_NUM_OF_MESS = 64;
	const int MESSAGE_QUEUE_MAX_MSG_SIZE = 128;
	const int MESSAGE_QUEUE_CURRENT_NUM_OF_MESS = 0;
	const int MESSAGE_QUEUE_DEFAULT_PERM = 0644;
	const int MESSAGE_QUEUE_MSG_DEFAULT_PRIORITY = 0;
	const int MESSAGE_QUEUE_NAME_MAX_SIZE = 128;

	const char * const MESSAGE_QUEUE_PREFIX = "/ACA_RTR_";
	const char * const MS_MESS_QUEUE_NAME_PREFIX = "CHS";
	const char * const MS_NAME_CHANGE_MESS_QUEUE_NAME = "MS_NAME_CHANGE";
	const char * const NEW_CP_CONNECTED_MESSAGE = "MSG_NEW_CP";
	const char * const RECORD_SIZE_CHANGE_MESSAGE = "MSG_RECSIZE_CHANGE=";
	const char * const MS_NAME_CHANGE_SEPARATOR = "^";
}

#endif // HEADER_GUARD_FILE__acs_aca_constants
