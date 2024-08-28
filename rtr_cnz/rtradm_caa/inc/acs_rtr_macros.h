#ifndef HEADER_GUARD_FILE__acs_rtr_macros
#define HEADER_GUARD_FILE__acs_rtr_macros acs_rtr_macros.h

/** @file acs_rtr_macros.h
 *	@brief
 *	@author xharbav (Harika Bavana)
 *	@date 2013-05-10
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
 *	| R-001 | 2013-07-25 | xharbav      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */



#ifndef ACS_RTR_ARRAY_SIZE
#	define ACS_RTR_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif

#endif // HEADER_GUARD_FILE__acs_rtr_macros

