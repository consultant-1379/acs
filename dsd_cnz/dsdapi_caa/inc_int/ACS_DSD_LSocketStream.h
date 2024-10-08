#ifndef HEADER_GUARD_CLASS__ACS_DSD_LSocketStream
#define HEADER_GUARD_CLASS__ACS_DSD_LSocketStream ACS_DSD_LSocketStream

/** @file ACS_DSD_LSocketStream.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-26
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
 *	| P0.9.0 | 2010-07-26 | xnicmut      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2010-08-04 | xnicmut      | Released for sprint 2               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+========+============+==============+=====================================+
 */

#include "ace/LSOCK_Stream.h"

#include "ACS_DSD_IOStreamProvider.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_LSocketStream

typedef ACS_DSD_IOStreamProvider < ACE_LSOCK_Stream > __CLASS_NAME__;

#endif // HEADER_GUARD_CLASS__ACS_DSD_LSocketStream
