#ifndef HEADER_GUARD_CLASS__ACS_DSD_Logger
#define HEADER_GUARD_CLASS__ACS_DSD_Logger ACS_DSD_Logger

/** @file ACS_DSD_Logger.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-11-09
 *	@version 1.0.0
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P1.0.1 | 2010-11-09 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <stdarg.h>

#include "ACS_DSD_Macros.h"

#ifdef ACS_DSD_LOG_USE_LOG4CPLUS
#include "ACS_DSD_Logger4cplus.h"
#else
#include "ACS_DSD_LoggerTra.h"
#endif

#endif // HEADER_GUARD_CLASS__ACS_DSD_Logger
