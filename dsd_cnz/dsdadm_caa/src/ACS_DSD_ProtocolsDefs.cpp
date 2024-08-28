#include "ACS_DSD_ProtocolsDefs.h"

// Element <i> of this array is 1 if and only if the expression : < i + CPAP_MIN_PRIMITIVE_ID > is a valid primitive
int cp_ap_valid_primitive_id[] = {	1,/*CPAP_UNKNOWN -> 0 */
									1,/*CPAP_ADDRESS_REQUEST -> 1 */
									1,/*CPAP_ADDRESS_REPLY -> 2 */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									0,/* NOT ASSIGNED */
									1,/*CPAP_LISTAPP_REQUEST -> 18 */
									1,/*CPAP_LISTAPP_REPLY -> 19 */
									1,/*STARTUP REQUEST -> 20 */
									1 /*STARTUP REPLY -> 21 */ };


// Element <i> of this array is 1 if and only if the expression : < i + SVRAPP_MIN_PRIMITIVE_ID > is a valid primitive
int svr_app_valid_primitive_id[] = { 1, /*SVRAPP_REGISTER_UNIX_REQUEST -> 31 */
									 1, /*SVRAPP_REGISTER_UNIX_REQUEST-> 32 */
									 1, /*SVRAPP_REGISTER_RESPONSE -> 33 */
									 1, /*SVRAPP_UNREGISTER_REQUEST -> 34 */
									 1, /*SVRAPP_UNREGISTER_RESPONSE -> 35 */
									 1, /*SVRAPP_ADDR_REQUEST --> 36 */
									 1, /*SVRAPP_ADDR_INET_RESPONSE --> 37 */
									 1, /*SVRAPP_ADDR_UNIX_RESPONSE --> 38 */
									 1, /*SVRAPP_QUERY_REQUEST -_>39 */
									 1, /*SVRAPP_QUERY_RESPONSE --> 40*/
									 1, /*SVRAPP_LIST_REQUEST --> 41*/
									 1, /*SVRAPP_LIST_INET_RESPONSE --> 42*/
									 1, /*SVRAPP_LIST_UNIX_RESPONSE --> 43*/
									 1, /*SVRAPP_NOTIFY_REQUEST --> 44*/
									 1, /*SVRAPP_NOTIFY_INDICATION --> 45*/
									 1, /*SVRAPP_NODE_REQUEST --> 46 */
									 1 /*SVRAPP_NODE_RESPONSE --> 47*/};


// Element <i> of this array is 1 if and only if the expression : < i + SVRSVR_MIN_PRIMITIVE_ID > is a valid primitive
int srv_srv_valid_primitive_id[] = { 1, /*SvRSRV_ADDRESS_REQUEST ---> 51 */
									 1, /*SvRSRV_ADDRESS_INET_REPLY --> 52 */
									 1, /*SvRSRV_ADDRESS_UNIX_REPLY --> 53 */
									 1, /*SvRSRV_NODE_INFO_REQUEST -->  54 */
									 1, /*SvRSRV_NODE_INFO_REPLY  -->   55 */
									 1, /*SvRSRV_QUERY_REQUEST --> 56  */
									 1, /*SvRSRV_QUERY_REPLY  -->   57 */
									 1, /*SVRSVR_NOTIFY_PUBLISH --> 58 */
									 1, /*SvRSRV_NOTIFY_PUBLISH_REPLY --> 59 */
									 1, /*SvRSRV_NOTIFY_UNREGISTER  --> 60 */
									 1, /*SvRSRV_NOTIFY_UNREGISTER_REPLY  --> 61 */
									 1, /*SRVSRV_LIST_REQUEST --> 62 */
									 1, /*SRVSRV_LIST_INET_REPLY --> 63 */
									 1, /*SRVSRV_LIST_INET_REPLY --> 64 */
									 1, /*SRVSRV_STARTUP_REQUEST --> 65 */
									 1, /*SRVSRV_STARTUP_REPLY --> 66 */
									 1, /*SRVSRV_HWC_CHANGE_NOTIFY --> 67 */
									 1  /*SRVSRV_HWC_CHANGE_NOTIFY_REPLY --> 68 */};



ACS_DSD_ProtocolInfo CPAP_prot_info =
{
	// CP-AP Protocol Info
	PROTOCOL_CPAP, 					/* protocol ID */
	CPAP_PROTOCOL_VERSION_MAX,  	/* maximum protocol version */
	CPAP_MIN_PRIMITIVE_ID, 			/* minimum primitive ID of the protocol */
	CPAP_MAX_PRIMITIVE_ID, 			/* maximum primitive ID of the protocol */
	cp_ap_valid_primitive_id
};

ACS_DSD_ProtocolInfo SVRAPP_prot_info =
{
	// SERVICE-APPLICATION Protocol Info
	PROTOCOL_SVRAPP, 				/* protocol ID */
	SVRAPP_PROTOCOL_VERSION_MAX,  	/* maximum protocol version */
	SVRAPP_MIN_PRIMITIVE_ID, 		/* minimum primitive ID of the protocol */
	SVRAPP_MAX_PRIMITIVE_ID, 		/* maximum primitive ID of the protocol */
	svr_app_valid_primitive_id
};

ACS_DSD_ProtocolInfo SRVSRV_prot_info =
{
	// SERVICE-SERVICE Protocol Info
	PROTOCOL_SRVSRV, 				/* protocol ID */
	SRVSRV_PROT_VERS_MAX,  			/* maximum protocol version */
	SRVSRV_MIN_PRIMITIVE_ID, 		/* minimum primitive ID of the protocol */
	SRVSRV_MAX_PRIMITIVE_ID, 		/* maximum primitive ID of the protocol */
	srv_srv_valid_primitive_id
};

