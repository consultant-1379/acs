#ifndef ACS_APBM_IPMIAPI_COMMON_H
#define  ACS_APBM_IPMIAPI_COMMON_H

/** @file acs_apbm_ipmiapi_common.h
 *	@brief
 *	@author
 *	@date 2012-03-06
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
 *	| R-001 | 2012-03-06 |   		 	| Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#define IPMIAPI_MUTEX_NAME  "/var/run/acs_apbm_ipmiapi_mutex"
#define IPMIAPI_DEFAULT_DEVICE_NAME "/dev/eri_ipmi"
#define IPMIAPI_DEFAULT_EXTERN_DEVICE_NAME "/dev/ttyUSB0"
#define IPMIAPI_DEVICE_NAME_PREFIX  "/dev/"
#define IPMIAPI_DEVICE_NAME_MAX_LEN  256
#define IPMIAPI_QUERY_RETRY_NUMBER  20
#define IPMIAPI_OP_RETRY_NUMBER  	3
#define IPMIAPI_QUERY_EPOLL_TIMEOUT  400
#define IPMIAPI_MUTEX_ACQUIRE_TIMEOUT_MS 5000
#define MAX_L2_READ_RETRIES 10

namespace acs_apbm_ipmiapi_ns {

enum ipmiapistate_t {
	IPMIAPI_NOT_INITIALIZED = 0,
	IPMIAPI_INITIALIZED = 1,
};

enum communication_modes_t {
	UNKNOWN_MODE,
	DRIVER_MODE,
	SERIAL_MODE
};

/** @brief APBM IPMI API error constants.
 *
 *	ErrorConstants enumeration detailed description
 */
enum ErrorConstants {
	ERR_NO_ERRORS				=	0,
	ERR_GENERIC_ERROR			=	-1,
	ERR_OP_NOT_SUPPORTED		=	-2,
	ERR_OP_NOT_EXECUTED			=	-3,
	ERR_NODE_BUSY				=	-4,
	ERR_API_ALREADY_INITIALIZED	=	-5,
	ERR_INVALID_DEVICE_NAME		=	-6,
	ERR_OPEN_DEVICE_FAILURE		=	-7,
	ERR_EPOLL_CREATE_FAILURE	=	-8,
	ERR_EPOLL_CTL_FAILURE		=	-9,
	ERR_API_NOT_INITIALIZED		=	-10,
	ERR_NO_SYSTEM_RESOURCE		=	-11,
	ERR_INVALID_PARAMETER		=	-12,
	ERR_QUERY_EPOLL_ERROR		=	-13,
	ERR_QUERY_EPOLL_TIMEOUT		=	-14,
	ERR_QUERY_READ_ERROR		=	-15,
	ERR_QUERY_RESPONSE_ERROR	=	-16,
	ERR_QUERY_UNKNOWN_RESPONSE	=	-17,
	ERR_QUERY_READ_TIMEOUT		=	-18,
	ERR_QUERY_WRITE_ERROR		=	-19,
	ERR_INIT_LEVEL2_ERROR		=	-20,
	ERR_WRITE_LEVEL2_ERROR		=	-21,
	ERR_READ_LEVEL2_ERROR		=	-22
};

}	// END OF NAMESPACE

#endif
