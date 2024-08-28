#ifndef ACS_DSD_PROTOCOLSDEFS_H_
#define ACS_DSD_PROTOCOLSDEFS_H_

/** @file ACS_DSD_ProtocolDefs.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-11
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
 *	| 0.1    | 2010-11-11 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_PrimitiveDataHandler.h"

#define ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN	1024
#define MAX_NUM_OF_PRIMITIVES 256

/// This struct is used to maintain some general DSD Service Protocol Info
typedef struct
{
	int protocolID;						/// Protocol ID
	unsigned int num_of_versions;		/// Number of released protocol versions ( 1, 2 ...., <num_of_versions> }
	unsigned int minPrimitiveID;		/// Minimum primitive ID of the protocol
	unsigned int maxPrimitiveID;		/// Maximum primitive ID of the protocol
	const int *valid_primitive_id;		/// Array used to represent the valid primitive IDs for the protocol
} ACS_DSD_ProtocolInfo;


/// DSD Service Protocol IDs
enum ACS_DSD_ProtocolID
{
	PROTOCOL_UNKNOWN = -1,
	PROTOCOL_CPAP = 1,						/// CP-AP Protocol
	PROTOCOL_SVRAPP = 2,					/// SERVICE - APPLICATION protocol
	PROTOCOL_SRVSRV = 3,					/// SERVICE - SERVICE protocol
	PROTOCOL_MAX_ID = 3,					/// MAX protocol ID. PLEASE PAY ATTENTION TO KEEP THIS VALUE UP TO DATE
};


/* Definitions for the CP-AP Protocol ... */

/// CPAP Protocol supported versions
enum ACS_DSD_CPAP_ProtocolVersion
{
	CPAP_PROTOCOL_VERSION_0 = 0,			//  version 0
	CPAP_PROTOCOL_VERSION_1 = 1,			//  version 1
	CPAP_PROTOCOL_VERSION_2 = 2,			//  version 2
	CPAP_PROTOCOL_VERSION_MAX = 2			//  MAX CP-AP protocol version. PLEASE PAY ATTENTION TO KEEP THIS VALUE UP TO DATE
};

/// CP-AP Protocol Primitives
enum ACS_DSD_CPAP_PrimitiveID
{
	CPAP_MIN_PRIMITIVE_ID = acs_dsd::PCP_CPAP_UNKNOW_REPLY_ID,						// MIN CP-AP protocol primitive ID. PLEASE PAY ATTENTION TO KEEP THIS VALUE UP TO DATE
	CPAP_MAX_PRIMITIVE_ID = acs_dsd::PCP_CPAP_STARTUP_REPLY_ID, 					// MAX CP-AP protocol primitive ID. PLEASE PAY ATTENTION TO KEEP THIS VALUE UP TO DATE
	CPAP_ADDRESS_REQUEST = acs_dsd::PCP_CPAP_ADDRESS_REQUEST_ID,					// Address request
	CPAP_ADDRESS_REPLY  = acs_dsd::PCP_CPAP_ADDRESS_REPLY_ID,						// Address reply
	CPAP_LISTAPP_REQUEST = acs_dsd::PCP_CPAP_LIST_APPLICATION_REQUEST_ID,			// List application request
	CPAP_LISTAPP_REPLY = acs_dsd::PCP_CPAP_LIST_APPLICATION_REPLY_ID,				// List application reply
	CPAP_STARTUP_REQUEST = acs_dsd::PCP_CPAP_STARTUP_REQUEST_ID,					// Start up request
	CPAP_STARTUP_REPLY = acs_dsd::PCP_CPAP_STARTUP_REPLY_ID,						// Start up reply
	CPAP_UNKNOWN_REPLY = acs_dsd::PCP_CPAP_UNKNOW_REPLY_ID							// Unknown reply
};


extern ACS_DSD_ProtocolInfo CPAP_prot_info;


/* Definitions for the SERV-APP Protocol ... */

/// SERVER - APPLICATION Protocol Versions
enum ACS_DSD_SVRAPP_ProtocolVersion
{
	SVRAPP_PROTOCOL_VERSION_1 = 1,				// version 1
	SVRAPP_PROTOCOL_VERSION_MAX = 1,			// MAX SERVICE-APPLICATION protocol primitive ID. PLEASE PAY ATTENTION TO KEEP THIS VALUE UP TO DATE
};

/// SERV-APP protocol primitives
enum ACS_DSD_SVRAPP_PrimitiveID
{
	SVRAPP_MIN_PRIMITIVE_ID =  acs_dsd::PCP_DSDAPI_REGISTRATION_INET_REQUEST_ID,		/* MIN PRIMITIVE ID, Please pay attention to keep it up to date */
	SVRAPP_MAX_PRIMITIVE_ID = acs_dsd::PCP_DSDAPI_NODE_REPLY_ID,						/* MAX PRIMITIVE ID. Please pay attention to keep it up to date*/
	SVRAPP_UNKNOWN = -1,																/* UNKNOWN PRIMITIVE ID */
	SVRAPP_REGISTER_INET_REQUEST = acs_dsd::PCP_DSDAPI_REGISTRATION_INET_REQUEST_ID,	/* Registration request, INET Socket */
	SVRAPP_REGISTER_UNIX_REQUEST = acs_dsd::PCP_DSDAPI_REGISTRATION_UNIX_REQUEST_ID,	/* Registration request, UNIX Socket */
	SVRAPP_REGISTER_RESPONSE = acs_dsd::PCP_DSDAPI_REGISTRATION_REPLY_ID,				/* Registration response */
	SVRAPP_UNREGISTER_REQUEST = acs_dsd::PCP_DSDAPI_UNREGISTRATION_REQUEST_ID,			/* Unregistration request */
	SVRAPP_UNREGISTER_RESPONSE = acs_dsd::PCP_DSDAPI_UNREGISTRATION_REPLY_ID,			/* Unregistratiojn response */
	SVRAPP_ADDR_REQUEST = acs_dsd::PCP_DSDAPI_ADDRESS_REQUEST_ID,						/* Address request */
	SVRAPP_ADDR_INET_RESPONSE = acs_dsd::PCP_DSDAPI_ADDRESS_INET_REPLY_ID,				/* Address response, INET Socket */
	SVRAPP_ADDR_UNIX_RESPONSE = acs_dsd::PCP_DSDAPI_ADDRESS_UNIX_REPLY_ID,				/* Address response, UNIX Socket */
	SVRAPP_QUERY_REQUEST = acs_dsd::PCP_DSDAPI_QUERY_REQUEST_ID,						/* Query request */
	SVRAPP_QUERY_RESPONSE = acs_dsd::PCP_DSDAPI_QUERY_REPLY_ID,							/* Query response */
	SVRAPP_LIST_REQUEST = acs_dsd::PCP_DSDAPI_LIST_REQUEST_ID,							/* List request */
	SVRAPP_LIST_INET_RESPONSE = acs_dsd::PCP_DSDAPI_LIST_INET_REPLY_ID,					/* List response, INET socket */
	SVRAPP_LIST_UNIX_RESPONSE = acs_dsd::PCP_DSDAPI_LIST_UNIX_REPLY_ID,					/* List response, UNIX socket */
	SVRAPP_NOTIFY_REQUEST = acs_dsd::PCP_DSDAPI_NOTIFICATION_REQUEST_ID,				/* Notification request */
	SVRAPP_NOTIFY_INDICATION = acs_dsd::PCP_DSDAPI_NOTIFICATION_INDICATION_ID,			/* Notification indication */
	SVRAPP_NODE_REQUEST = acs_dsd::PCP_DSDAPI_NODE_REQUEST_ID,							/* Node request */
	SVRAPP_NODE_RESPONSE = acs_dsd::PCP_DSDAPI_NODE_REPLY_ID							/* Node response */
};

extern ACS_DSD_ProtocolInfo SVRAPP_prot_info;


/* Definitions for the SERV-SERV Protocol ... */

/// SERVER - SERVER Protocol Versions
enum ACS_DSD_SRVSRV_ProtocolVersion
{
	SRVSRV_PROT_V1 = 1,
	SRVSRV_PROT_VERS_MAX = 1			// Indicates the MAX VERSION ID. Please pay attention to keep this value up to date
};

/// Set of ID of the SERV_SERV protocol primitives
enum ACS_DSD_SRVSRV_PrimitiveID
{
	// TO BE DEFINED
	SRVSRV_MIN_PRIMITIVE_ID = acs_dsd::PCP_DSDDSD_ADDRESS_REQUEST_ID,		/* MIN PRIMITIVE ID. Please pay attention to keep this value up to date */
	SRVSRV_MAX_PRIMITIVE_ID = acs_dsd::PCP_DSDDSD_HWC_CHANGE_NOTIFY_REPLY_ID,	/* MAX PRIMITIVE ID. Please pay attention to keep this value up to date */
	SRVSRV_UNKNOWN = -1,
	SRVSRV_ADDRESS_REQUEST           = acs_dsd::PCP_DSDDSD_ADDRESS_REQUEST_ID,          	/* Address request */
	SRVSRV_ADDRESS_INET_REPLY        = acs_dsd::PCP_DSDDSD_ADDRESS_INET_REPLY_ID,       	/* Address response, INET Socket */
	SRVSRV_ADDRESS_UNIX_REPLY        = acs_dsd::PCP_DSDDSD_ADDRESS_UNIX_REPLY_ID,       	/* Address response, UNIX Socket */
	SRVSRV_NODE_INFO_REQUEST         = acs_dsd::PCP_DSDDSD_NODE_INFO_REQUEST_ID,			/* Node Info request */
	SRVSRV_NODE_INFO_REPLY 	         = acs_dsd::PCP_DSDDSD_NODE_INFO_REPLY_ID,			/* Node Info reply */
	SRVSRV_QUERY_REQUEST             = acs_dsd::PCP_DSDDSD_QUERY_REQUEST_ID,        		/* Query request */
	SRVSRV_QUERY_REPLY               = acs_dsd::PCP_DSDDSD_QUERY_REPLY_ID,          		/* Query response */
	SRVSRV_NOTIFY_PUBLISH   		 		 = acs_dsd::PCP_DSDDSD_NOTIFY_PUBLISH_ID,       		/* Notify Publish request */
	SRVSRV_NOTIFY_PUBLISH_REPLY      = acs_dsd::PCP_DSDDSD_NOTIFY_PUBLISH_REPLY_ID, 		/* Notify Publish reply */
	SRVSRV_NOTIFY_UNREGISTER 		 		 = acs_dsd::PCP_DSDDSD_NOTIFY_UNREGISTER_ID,    		/* Notify Unregister  */
	SRVSRV_NOTIFY_UNREGISTER_REPLY   = acs_dsd::PCP_DSDDSD_NOTIFY_UNREGISTER_REPLY_ID,		/* Notify Unregister reply */
	SRVSRV_LIST_REQUEST				 			 = acs_dsd::PCP_DSDDSD_LIST_REQUEST_ID,					/* List request */
	SRVSRV_LIST_INET_REPLY			 		 = acs_dsd::PCP_DSDDSD_LIST_INET_REPLY_ID,				/* List reply,INET Socket */
	SRVSRV_LIST_UNIX_REPLY			 		 = acs_dsd::PCP_DSDDSD_LIST_UNIX_REPLY_ID,				/* List reply,UNIX Socket */
	SRVSRV_STARTUP_REQUEST			 		 = acs_dsd::PCP_DSDDSD_STARTUP_REQUEST_ID,				/* Startup request */
	SRVSRV_STARTUP_REPLY			 			 = acs_dsd::PCP_DSDDSD_STARTUP_REPLY_ID,					/* Startup reply */
	SRVSRV_HWC_CHANGE_NOTIFY				 = acs_dsd::PCP_DSDDSD_HWC_CHANGE_NOTIFY_ID,		/* HWC Table change notify */
	SRVSRV_HWC_CHANGE_NOTIFY_REPLY	 = acs_dsd::PCP_DSDDSD_HWC_CHANGE_NOTIFY_REPLY_ID		/* HWC Table change notify reply */
};

extern ACS_DSD_ProtocolInfo SRVSRV_prot_info;


#endif /* ACS_DSD_PROTOCOLSDEFS_H_ */
