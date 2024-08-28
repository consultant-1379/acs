#ifndef HEADER_GUARD_CLASS__ACS_DSD_LSocketConnector
#define HEADER_GUARD_CLASS__ACS_DSD_LSocketConnector ACS_DSD_LSocketConnector

/** @file ACS_DSD_LSocketConnector.h
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

#include "ace/LSOCK_Connector.h"
#include "ace/UNIX_Addr.h"

#include "ACS_DSD_ConnectorProvider.h"
#include "ACS_DSD_LSocketStream.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_LSocketConnector

typedef ACS_DSD_ConnectorProvider < ACE_LSOCK_Connector, ACE_UNIX_Addr, ACS_DSD_LSocketStream > __CLASS_NAME__;

#endif // HEADER_GUARD_CLASS__ACS_DSD_LSocketConnector
