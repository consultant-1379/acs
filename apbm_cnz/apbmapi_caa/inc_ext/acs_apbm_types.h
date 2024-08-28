#ifndef HEADER_GUARD_FILE__acs_apbm_types
#define HEADER_GUARD_FILE__acs_apbm_types acs_apbm_types.h

/** @file acs_apbm_types.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-06-28
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
 *	| R-001 | 2011-06-28 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

namespace acs_apbm {
#ifndef ACS_APBM_TRAP_HANDLE_DEFINED
	typedef int trap_handle_t;
#define ACS_APBM_TRAP_HANDLE_DEFINED 1
#endif

	typedef enum BoardPresenceReturnCode {
		BOARD_PRESENCE_ERROR	    			= -1,
		BOARD_PRESENCE_BOARD_PRESENT 			=  0,
		BOARD_PRESENCE_BOARD_NOT_CONFIGURED		=  1,
		BOARD_PRESENCE_BOARD_NOT_AVAILABLE     	=  2
	} board_presence_rc_t ;

	typedef enum BoardStatusReturnCode {
		BOARD_STATUS_BOARD_NOT_CONFIGURED	= -2,
		BOARD_STATUS_ERROR		    		= -1,
		BOARD_STATUS_BOARD_WORKING 			=  0,
		BOARD_STATUS_BOARD_FAULTY			=  1,
		BOARD_STATUS_BOARD_BLOCKED     		=  2,
		BOARD_STATUS_BOARD_MISSING    		=  3,
	} board_status_rc_t ;

	typedef enum BoardNameConstants {
		BOARD_NAME_DVD = 0,
		BOARD_NAME_GEA = 1
	} board_name_t;

	typedef enum ReportedBoardStatusConstants {
		REPORTED_BOARD_STATUS_FAULTY = -1,
		REPORTED_BOARD_STATUS_WORKING = 0

	} reported_board_status_t;

}

#endif // HEADER_GUARD_FILE__acs_apbm_types
