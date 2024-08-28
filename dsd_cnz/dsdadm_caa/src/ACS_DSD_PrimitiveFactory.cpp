#include "ace/Log_Msg.h"

#include "ACS_DSD_AppProt_RegisterPrimitives.h"
#include "ACS_DSD_AppProt_UnregisterPrimitives.h"
#include "ACS_DSD_AppProt_AddressPrimitives.h"
#include "ACS_DSD_AppProt_NotificationPrimitives.h"
#include "ACS_DSD_AppProt_QueryPrimitives.h"
#include "ACS_DSD_SrvProt_AddressPrimitives.h"
#include "ACS_DSD_SrvProt_ListPrimitives.h"
#include "ACS_DSD_SrvProt_NotificationPrimitives.h"
#include "ACS_DSD_SrvProt_QueryPrimitives.h"
#include "ACS_DSD_SrvProt_StartupPrimitives.h"
#include "ACS_DSD_SrvProt_HwcChange_Primitives.h"
#include "ACS_DSD_CpProt_StartupPrimitives.h"
#include "ACS_DSD_CpProt_AddressPrimitives.h"
#include "ACS_DSD_CpProt_UnknownPrimitive.h"
#include "ACS_DSD_CpProt_ListApplPrimitives.h"
#include "ACS_DSD_PrimitiveFactory.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


extern ACS_DSD_Logger dsd_logger;

ACS_DSD_PrimitiveFactory::ACS_DSD_PrimitiveFactory()
{
	// initialize primitives table
	for(unsigned int i = 0; i <= CPAP_PROTOCOL_VERSION_MAX; ++i)
		for(unsigned int j = 0; j < num_CPAP_primitives; ++j)
			this->_tbl_CPAP[i][j] = 0;

	for(unsigned int i = 0; i <= SVRAPP_PROTOCOL_VERSION_MAX; ++i)
			for(unsigned int j = 0; j < num_SVRAPP_primitives; ++j)
				this->_tbl_SVRAPP[i][j] = 0;

	for(unsigned int i = 0; i <= SRVSRV_PROT_VERS_MAX; ++i)
			for(unsigned int j = 0; j < num_SRVSRV_primitives; ++j)
				this->_tbl_SRVSRV[i][j] = 0;

	// register primitives for CP-AP protocol
	this->register_CPAP_primitives();

	// register primitives for SVR-APP protocol
	this->register_AppProt_primitives();

	// register primitives for SRV-SRV protocol
	this->register_SRVSRV_primitives();
}


int ACS_DSD_PrimitiveFactory::register_primitive(ACS_DSD_ProtocolID prot_id, uint prot_version, uint primitive_id, CreatePrimitiveFunc create_func)
{
	int retVal = 0;

	// get the internal table position where the <create_func> function has to be stored
	CreatePrimitiveFunc * func_addr = 0;
	PF_Const res = get_make_primitive_func_address(prot_id, prot_version, primitive_id, func_addr, false);

	// update the internal table
	if(res == PF_PRIMITIVE_NOT_IMPLEMENTED)
		*func_addr = create_func;
	else if(res == PF_PRIMITIVE_IS_IMPLEMENTED)
		retVal = -1;	// primitive is yet registered !
	else
		retVal = -2;	// passed arguments are NOT valid

	return retVal;
}


ACS_DSD_PrimitiveFactory::PF_Const ACS_DSD_PrimitiveFactory::build_primitive(ACS_DSD_ProtocolID prot_id, unsigned int prot_version, unsigned int primitive_id, ACS_DSD_ServicePrimitive * & p_prim)
{
	//ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("ACS_DSD_PrimitiveFactory:  build_primitive() called ! PROTOCOL_ID = %u, VERSION = %u, PRIMITIVE_ID = %u !"), prot_id, prot_version, primitive_id);

	// get the address of the "create primitive" function
	CreatePrimitiveFunc * func_addr = 0;
	PF_Const res = get_make_primitive_func_address(prot_id, prot_version, primitive_id, func_addr, true);
	if(res != PF_PRIMITIVE_IS_IMPLEMENTED)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_PrimitiveFactory:  get_make_primitive_func_address() failure ! return_code is  <%d> !"), res);
		return res;
	}

	// invoke the "create primitive" function to create the primitive object
	ACS_DSD_ServicePrimitive *pCreatedPrim =  (*func_addr)();	// PAY ATTENTIOn ! The returned object is allocated on the heap using <new>
	if(!pCreatedPrim)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("ACS_DSD_PrimitiveFactory:  make_primitive function failure"));
		return PF_MEMORY_ERROR;
	}

	// return the created object in the output parameter
	p_prim = pCreatedPrim;
	return PF_BUILD_PRIMITIVE_OK;
}


ACS_DSD_PrimitiveFactory::PF_Const ACS_DSD_PrimitiveFactory::check(ACS_DSD_ProtocolID prot_id, uint prot_version, uint primitive_id)
{
	// get info about the passed protocol
	const ACS_DSD_ProtocolInfo * p_prot_info = 0;
	switch(prot_id)
	{
	case PROTOCOL_CPAP:
		p_prot_info = & CPAP_prot_info;
		break;
	case PROTOCOL_SVRAPP:
		p_prot_info = & SVRAPP_prot_info;
		break;
	case PROTOCOL_SRVSRV:
		p_prot_info = & SRVSRV_prot_info;
	default:
		break;
	}

	// check if protocol id is valid
	if(!p_prot_info)
		return PF_UNKNOWN_PROTOCOL;	// invalid protocol

	// check if primitive ID is valid (with respect to the protocol ID)
	if(primitive_id < p_prot_info->minPrimitiveID || primitive_id > p_prot_info->maxPrimitiveID ||  (! p_prot_info->valid_primitive_id[primitive_id - p_prot_info->minPrimitiveID ]))
		return PF_INVALID_PRIMITIVE;	// invalid primitive id

	// check if protocol version is in the valid range (with respect to the protocol ID)
	if(prot_version > p_prot_info->num_of_versions)
		return PF_INVALID_VERSION;	// invalid version

	return PF_CHECK_OK;
}


ACS_DSD_PrimitiveFactory::PF_Const  ACS_DSD_PrimitiveFactory::get_make_primitive_func_address(ACS_DSD_ProtocolID prot_id, uint prot_version, uint primitive_id, CreatePrimitiveFunc * & make_prim_func_addr, bool serch_default_on_unsupported_version)
{
	int prot_version_to_be_searched = prot_version;

	// check passed data to detect inconsistencies
	PF_Const check_res = check(prot_id, prot_version, primitive_id);
	if(check_res != PF_CHECK_OK)
	{
		// we must handle in a special way the primitive "STARTUP REQUEST" of the CP-AP Protocol. In particular, If <prot_version> is invalid and
		// <serch_default_on_unsupported_version>  is true, then we must try to return the default primitive implementation, if any
		if( (check_res == PF_INVALID_VERSION) && serch_default_on_unsupported_version && (prot_id == PROTOCOL_CPAP) && (primitive_id == CPAP_STARTUP_REQUEST))
			prot_version_to_be_searched = CPAP_PROTOCOL_VERSION_0;
		else
			return check_res;	// passed data are inconsistent.
	}

	// search make_primitive() function for the specified primitive and the specified protocol version
	switch(prot_id)
	{
	case PROTOCOL_CPAP:
		make_prim_func_addr = & (_tbl_CPAP[prot_version_to_be_searched][primitive_id - CPAP_prot_info.minPrimitiveID]);
		// special handling for CP-AP Startup Request primitive having unsupported version. Note that the version
		// may be valid (check_res == PF_CHECK_OK) but non implemented
		if( (*make_prim_func_addr == 0)  && serch_default_on_unsupported_version && (primitive_id == CPAP_STARTUP_REQUEST))
			make_prim_func_addr = & (_tbl_CPAP[CPAP_PROTOCOL_VERSION_0][primitive_id - CPAP_prot_info.minPrimitiveID]);
		break;
	case PROTOCOL_SVRAPP:
		make_prim_func_addr = & (_tbl_SVRAPP[prot_version_to_be_searched][primitive_id - SVRAPP_prot_info.minPrimitiveID]);
		break;
	case PROTOCOL_SRVSRV:
		make_prim_func_addr = & (_tbl_SRVSRV[prot_version_to_be_searched][primitive_id - SRVSRV_prot_info.minPrimitiveID]);
		break;
	default:
		break;
	}

	return (*make_prim_func_addr) ? PF_PRIMITIVE_IS_IMPLEMENTED: PF_PRIMITIVE_NOT_IMPLEMENTED;
}



int ACS_DSD_PrimitiveFactory::register_AppProt_primitives()
{
	// REGISTER request, INET socket service, v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_REGISTER_INET_REQUEST, (CreatePrimitiveFunc) & AppProt_registerINET_v1_request::make_primitive);

	// REGISTER request, UNIX socket service, v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_REGISTER_UNIX_REQUEST, (CreatePrimitiveFunc) & AppProt_registerUNIX_v1_request::make_primitive);

	// REGISTER response, v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_REGISTER_RESPONSE, (CreatePrimitiveFunc) & AppProt_register_v1_response::make_primitive);

	// UNREGISTER request v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_UNREGISTER_REQUEST, (CreatePrimitiveFunc) & AppProt_unregister_v1_request::make_primitive);

	// UNREGISTER response v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_UNREGISTER_RESPONSE, (CreatePrimitiveFunc) & AppProt_unregister_v1_response::make_primitive);

	// ADDRESS request v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_ADDR_REQUEST, (CreatePrimitiveFunc) & AppProt_address_v1_request::make_primitive);

	// ADDRESS response, INET socket service v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_ADDR_INET_RESPONSE, (CreatePrimitiveFunc) & AppProt_addressINET_v1_response::make_primitive);

	// ADDRESS response, UNIX socket service v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_ADDR_UNIX_RESPONSE, (CreatePrimitiveFunc) & AppProt_addressUNIX_v1_response::make_primitive);

	// QUERY request v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_QUERY_REQUEST, (CreatePrimitiveFunc) & AppProt_query_v1_request::make_primitive);

	// QUERY response v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_QUERY_RESPONSE, (CreatePrimitiveFunc) & AppProt_query_v1_response::make_primitive);

	// Notification request v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_NOTIFY_REQUEST, (CreatePrimitiveFunc) & AppProt_notification_v1_request::make_primitive);

	// Notification indication v1
	this->register_primitive(PROTOCOL_SVRAPP, SVRAPP_PROTOCOL_VERSION_1, SVRAPP_NOTIFY_INDICATION, (CreatePrimitiveFunc) & AppProt_notification_v1_indication::make_primitive);

	return 0;
}

int ACS_DSD_PrimitiveFactory::register_CPAP_primitives()
{
	// Address request
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_0, CPAP_ADDRESS_REQUEST, (CreatePrimitiveFunc) & CpProt_address_request::make_primitive);

	// Address reply
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_0, CPAP_ADDRESS_REPLY, (CreatePrimitiveFunc) & CpProt_address_response::make_primitive);

	// Start up request v0  ( for unsupported protocol versions )
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_0, CPAP_STARTUP_REQUEST, (CreatePrimitiveFunc) & CpProt_startup_v0_request::make_primitive);

	// Start up request v2
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_2, CPAP_STARTUP_REQUEST, (CreatePrimitiveFunc) & CpProt_startup_v2_request::make_primitive);

	// Start up reply v2
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_2, CPAP_STARTUP_REPLY, (CreatePrimitiveFunc) & CpProt_startup_v2_reply::make_primitive);

	// Unknown reply v0
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_0, CPAP_UNKNOWN_REPLY, (CreatePrimitiveFunc) & CpProt_unknown_reply::make_primitive);

	// List application request v1
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_1, CPAP_LISTAPP_REQUEST, (CreatePrimitiveFunc) & CpProt_listAppl_v1_request::make_primitive );

	// List application reply v1
	this->register_primitive(PROTOCOL_CPAP, CPAP_PROTOCOL_VERSION_1, CPAP_LISTAPP_REPLY, (CreatePrimitiveFunc) & CpProt_listAppl_v1_reply::make_primitive );

	return 0;
}

int ACS_DSD_PrimitiveFactory::register_SRVSRV_primitives()
{
	// Address request
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_ADDRESS_REQUEST, (CreatePrimitiveFunc) & SrvProt_address_request::make_primitive);

		// Address inet reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_ADDRESS_INET_REPLY, (CreatePrimitiveFunc) & SrvProt_address_inet_response::make_primitive);

		// Address unix reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_ADDRESS_UNIX_REPLY, (CreatePrimitiveFunc) & SrvProt_address_unix_response::make_primitive);

		 //Query request
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_QUERY_REQUEST, (CreatePrimitiveFunc) & SrvProt_query_v1_request::make_primitive);

		//query reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_QUERY_REPLY, (CreatePrimitiveFunc) & SrvProt_query_v1_response::make_primitive );

		// Notify publish
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_NOTIFY_PUBLISH, (CreatePrimitiveFunc) & SrvProt_notify_publish_v1_request::make_primitive );

		// Notify publish reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_NOTIFY_PUBLISH_REPLY, (CreatePrimitiveFunc) & SrvProt_notify_publish_v1_reply::make_primitive);

		// Notify unregister
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_NOTIFY_UNREGISTER, (CreatePrimitiveFunc) & SrvProt_notify_unregister_v1_request::make_primitive );

		//Notify unregister reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_NOTIFY_UNREGISTER_REPLY, (CreatePrimitiveFunc) & SrvProt_notify_unregister_v1_reply::make_primitive );

		// List request
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_LIST_REQUEST, (CreatePrimitiveFunc) & SrvProt_list_v1_request::make_primitive );

		// List inet reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_LIST_INET_REPLY, (CreatePrimitiveFunc) & SrvProt_list_inet_v1_reply::make_primitive );

		// List unix reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_LIST_UNIX_REPLY, (CreatePrimitiveFunc) & SrvProt_list_unix_v1_reply::make_primitive );

		// Startup request
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_STARTUP_REQUEST, (CreatePrimitiveFunc) & SrvProt_startup_v1_request::make_primitive );

		// Startup reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_STARTUP_REPLY, (CreatePrimitiveFunc) & SrvProt_startup_v1_reply::make_primitive );

		// HWC Change Notify request
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_HWC_CHANGE_NOTIFY, (CreatePrimitiveFunc) & SrvProt_HwcChange_notify_v1_request::make_primitive );

		// HWC Change Notify reply
		this->register_primitive(PROTOCOL_SRVSRV, SRVSRV_PROT_V1, SRVSRV_HWC_CHANGE_NOTIFY_REPLY, (CreatePrimitiveFunc) & SrvProt_HwcChange_notify_v1_reply::make_primitive );

		return 0;
}
