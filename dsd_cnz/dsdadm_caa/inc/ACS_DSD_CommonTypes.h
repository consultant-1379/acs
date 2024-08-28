#ifndef ACS_DSD_COMMONTYPES_H_
#define ACS_DSD_COMMONTYPES_H_

/** @file ACS_DSD_CommonTypes.h
 *	@brief
 *	@author xludesi
 *	@date 2010-12-30
 *	@version 0.1
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
 *	| 0.1    | 2010-12-30 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_MacrosConfig.h"
#include <list>

/// Info associated to a registered service
class ServiceInfo
{
public:
	// constructors
	ServiceInfo();
	ServiceInfo(const ServiceInfo & rhs);
	ServiceInfo(char serv_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX],
			    char serv_domain[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX],
			    uint16_t connection_type,
				uint16_t NrOfConn,
				uint32_t ip1,
				uint16_t port1,
				uint32_t ip2,
				uint16_t port2);

	// overload of assignment operator
	ServiceInfo & operator=(const ServiceInfo & rhs);

	char serv_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX];			/// Service name
	char serv_domain[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX];			/// Service domain
	uint16_t connection_type;  											/// Connection type
	uint16_t NrOfConn;													/// Number of connections
	uint32_t ip1;														/// IP-address network 1
	uint16_t port1;														/// Port number 1
	uint32_t ip2;														/// IP-address network 2
	uint16_t port2;														/// Port number 2
};

typedef std::list<ServiceInfo> ServiceInfoList;

#endif /* ACS_DSD_COMMONTYPES_H_ */
