#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_AppProt_SvcHandler.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_CpNodesManager.h"
#include "ACS_DSD_Client.h"
#include "ACS_DSD_Utils.h"
#include "ACS_DSD_AppProt_NotificationPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"


/* AppProt_address_v1_request methods */
extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_CpNodesManager cpNodesManager;
extern ACS_DSD_Logger dsd_logger;


AppProt_notification_v1_request::AppProt_notification_v1_request()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
	_primitive_id = SVRAPP_NOTIFY_REQUEST;
	_system_id = 0;
}

std::string AppProt_notification_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"SYSTEM_ID = %d,",
				_primitive_id,
				_protocol_version,
				_system_id);
	return mess;
}

int AppProt_notification_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_NOTIFY_REQUEST, SVRAPP_PROTOCOL_VERSION_1, &_system_id);
}

int AppProt_notification_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_system_id);
}

int AppProt_notification_v1_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler * pServiceHandler) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char FX_NAME[]="AppProt_notification_v1_request::process";
#endif

	int returnCode = ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	int call_result;

	// get effective CP system ID (we need this step to manage special CP system IDs)
	int32_t effective_cp_system_id = cpNodesManager.get_effective_cp_system_id(_system_id);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: - systemId = %d, effective_cp_system_id = %d"), FX_NAME, _system_id, effective_cp_system_id);

	if(effective_cp_system_id == acs_dsd::SYSTEM_ID_UNKNOWN)
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("%s: unable to get effective CP system ID: probably the CS System configuration has not info about CPs"), FX_NAME);

	AppProt_notification_v1_indication  *p_resp_prim = new (std::nothrow) AppProt_notification_v1_indication();
	if(!p_resp_prim){
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	acs_dsd::CPStateConstants cpState = acs_dsd::CP_STATE_UNDEFINED;
	if(effective_cp_system_id != acs_dsd::SYSTEM_ID_UNKNOWN)
		cpNodesManager.get_cp_system_state(effective_cp_system_id, cpState);

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: preparing to send NOTIFICATION_IND with cp state = %d ..."),FX_NAME, cpState);
	p_resp_prim->set_cpState(cpState);
	response_primitive = p_resp_prim;

	if(effective_cp_system_id != acs_dsd::SYSTEM_ID_UNKNOWN)
	{
		if((call_result = cpNodesManager.addNotification(effective_cp_system_id, pServiceHandler))!= ACS_DSD_CpNodesManager::CPNM_OK){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("%s: addNotification for systemId = %d failed retCode= %d!!"),FX_NAME, effective_cp_system_id, call_result);
		}

		// now add the system ID into the list of CpId for which the application required the change status notification.
		ACS_DSD_AppProt_SvcHandler *pAppProt_SvcH = dynamic_cast<ACS_DSD_AppProt_SvcHandler *> (pServiceHandler);
		pAppProt_SvcH->add_to_cpNotified(effective_cp_system_id);
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("%s: CP System ID = %d has been added to '_cpNotifiedList' attribute of service handler (%p)!!"),FX_NAME, effective_cp_system_id, pAppProt_SvcH);
	}

	return returnCode;
}

/* AppProt_addressINET_v1_response methods */

AppProt_notification_v1_indication::AppProt_notification_v1_indication()
{
	_protocol_id = PROTOCOL_SVRAPP;
	_protocol_version = SVRAPP_PROTOCOL_VERSION_1;

	_primitive_id = SVRAPP_NOTIFY_INDICATION;
	_cpState = 0;
}

std::string AppProt_notification_v1_indication::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"STATE = %d",
				_primitive_id,
				_protocol_version,
				_cpState);

	return mess;
}

int AppProt_notification_v1_indication::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(SVRAPP_NOTIFY_INDICATION, SVRAPP_PROTOCOL_VERSION_1,  _cpState);
}

int AppProt_notification_v1_indication::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_cpState);
}

int AppProt_notification_v1_indication::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}
