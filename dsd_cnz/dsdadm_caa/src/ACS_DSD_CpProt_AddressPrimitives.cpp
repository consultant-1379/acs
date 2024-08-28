#include <string>
#include <vector>

#include "ACS_CS_API.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_paramhandling.h"

#include "ACS_DSD_Utils.h"
#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_ImmConnectionHandler.h"
#include "ACS_DSD_ImmDataHandler.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ACS_DSD_CpProt_AddressPrimitives.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

namespace {
	const char * const ACS_DSD_IMM_ACA_RECORD_TRANSFER_ROOT_DN 			= "AxeDataRecorddataRecordMId=1";
	const char * const ACS_DSD_IMM_ACA_MESSAGE_STORE_NAME_ATTR_NAME = "messageStoreName";
	const int ACS_DSD_IMM_ACA_MESSAGE_STORE_NAME_ATTR_SIZE					= 1024;
}

extern ACS_DSD_Node dsd_local_node;
extern ACS_DSD_ImmConnectionHandler * immConnHandlerObj_ptr;
extern ACE_Recursive_Thread_Mutex imm_conn_mutex;
extern ACS_DSD_Logger dsd_logger;

/* CpProt_address_v1_request methods */

CpProt_address_request::CpProt_address_request()
{
	_protocol_id = PROTOCOL_CPAP;
	_primitive_id = CPAP_ADDRESS_REQUEST;
	memset(_serv_name,'\0',sizeof(_serv_name));
	memset(_serv_domain,'\0',sizeof(_serv_domain));
	memset(_node_name,'\0',sizeof(_node_name));
}

std::string CpProt_address_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u, "
				"SERVICE_NAME = %s,"
				"SERVICE_DOMAIN = %s,"
				"NODE_NAME = %s,",
				_primitive_id,
				_protocol_version,
				_serv_name,
				_serv_domain,
				_node_name);

	return mess;
}

int CpProt_address_request::get_service_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_name, buff, buffsize);
}

int CpProt_address_request::set_service_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_name, str, strlen(str));
}

int CpProt_address_request::get_service_domain(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_serv_domain, buff, buffsize);
}

int CpProt_address_request::set_service_domain(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_serv_domain, str, strlen(str));
}

int CpProt_address_request::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_node_name, str, strlen(str));
}


int CpProt_address_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(CPAP_ADDRESS_REQUEST, CPAP_PROTOCOL_VERSION_0, _node_name, _serv_domain, _serv_name);
}

int CpProt_address_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, _node_name, _serv_domain, _serv_name);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, _node_name, _serv_domain, _serv_name);
}

int CpProt_address_request::process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const
{
	int ret_code = ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	// allocate a new primitive object
	CpProt_address_response  *p_resp_prim = new (std::nothrow) CpProt_address_response();
	if(!p_resp_prim)
	{
		_process_error_descr = "Memory not available to allocate the primitive object";
		return ACS_DSD_PRIM_PROCESS_MEMORY_ERROR;
	}

	ACS_DSD_Session & session = service_handler->peer();
	ACS_DSD_Node remote_node;
    int32_t remote_system_id=acs_dsd::SYSTEM_ID_UNKNOWN;

	// get the systemId of the calling CP node
	if(session.get_remote_node(remote_node)== 0)
		remote_system_id=remote_node.system_id;

	// check if the service required is a distributable service
	bool FuncDistOK = isFunctionDistributable((char *)_serv_domain, (char *)_serv_name, dsd_local_node.system_id);

	if (!FuncDistOK){
		// the service is not distributed for the calling CP node
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED);
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CpProt_address_request::process():  < %s@%s > service NOT registered. isFunctionDistributable() returns %d!!"), _serv_name, _serv_domain, FuncDistOK );
		response_primitive = p_resp_prim;
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	//access to IMM to fetch the service info
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(imm_conn_mutex);
	ACS_DSD_ImmDataHandler immDHobj(immConnHandlerObj_ptr);

	char partner_node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]= {0};
	ACS_DSD_ConfigurationHelper::HostInfo partner_node;

	if(ACS_DSD_ConfigurationHelper::get_my_ap_partner_node (partner_node)== 0)
		strncpy(partner_node_name, partner_node.node_name, ACS_DSD_ARRAY_SIZE(partner_node_name));

	// try to fetch the address info from IMM
	int op_res = immDHobj.fetch_serv_addresses_fromIMM( (const char *)_serv_name, (const char *) _serv_domain, dsd_local_node.node_name, partner_node_name, 1);

	if(op_res == acs_dsd_imm::ERR_IMM_OM_INIT) {
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CpProt_address_request::process():  failure searching for < %s@%s > service !!  Error descr: %s"), _serv_name, _serv_domain, immDHobj.last_error_text());
		_process_error_descr = std::string("IMM failure: ") + immDHobj.last_error_text();
		response_primitive = p_resp_prim;
	 //	return ACS_DSD_PRIM_PROCESS_IMM_ERROR;
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}
	if(op_res == acs_dsd_imm::ERR_IMM_OM_GET){
		p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED);
		//ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CpProt_address_request  < %s@%s > Failed! Error descr: %s"), _serv_name, _serv_domain, immDHobj.last_error_text());
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CpProt_address_request::process():  The service < %s@%s > "
				"is NOT registered since it wasn't found into IMM."), _serv_name, _serv_domain);
		response_primitive = p_resp_prim;
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}
	if(op_res == acs_dsd_imm::NO_ERROR)
	{
		uint16_t conn_num =0;
		ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
		immDHobj.get_inet_addresses(inet_addresses, conn_num);

		ACE_INET_Addr tmp_inet_addr[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {ACE_INET_Addr()};
		int i,j;
		for( i=0,j=0; i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++){
			if(inet_addresses[i].get_ip_address()){
				tmp_inet_addr[j].set(inet_addresses[i]); j++;
			}
		}
		uint16_t valid_conn_num =j;
		if(valid_conn_num){
			p_resp_prim->set_conn_num(valid_conn_num);

			p_resp_prim->set_ip_address_1(tmp_inet_addr[0].get_ip_address());
			p_resp_prim->set_port_1(tmp_inet_addr[0].get_port_number());

			if (valid_conn_num > 1){
				p_resp_prim->set_ip_address_2(tmp_inet_addr[1].get_ip_address());
				p_resp_prim->set_port_2(tmp_inet_addr[1].get_port_number());
			}
			p_resp_prim->set_conn_type(acs_dsd::CONNECTION_TYPE_INET);
			p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL);
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("CpProt_address_request::process().  < %s@%s > service found  !"), _serv_name, _serv_domain);

		}
		else{
			p_resp_prim->set_error_field(acs_dsd::PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED);
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("CpProt_address_request::process(): failure getting info about < %s@%s > service:  conn_num = %d!"), _serv_name, _serv_domain, valid_conn_num);
		}
	}
	response_primitive = p_resp_prim;
	return ret_code;
}


bool CpProt_address_request::isFunctionDistributable(char * serv_domain, char * serv_name, int32_t system_id) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	static char FX_NAME[] = " isFunctionDistributable()";
#endif

	bool retValue = false;
	std::string message_store_id;
	if (isMessageStore(serv_name, message_store_id)) {
		//ACS_CS_API_FunctionDistribution *funcDist = ACS_CS_API::createFunctionDistributionInstance();
		char fName[512] = {0};
		snprintf(fName, ACS_DSD_ARRAY_SIZE(fName), "%s:%s", serv_domain, message_store_id.c_str());
		retValue = isFunctionDistributed(system_id, fName);
		//ACS_CS_API::deleteFunctionDistributionInstance(funcDist);
	} else {
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("[%s] :the service < %s@%s > is not a message store!"), FX_NAME,_serv_name, _serv_domain);
		retValue = true;
	}
	return retValue;
}


bool CpProt_address_request::isFunctionDistributed(int32_t apid, const char * fName) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	static char FX_NAME[] = "isFunctionDistributed()";
#endif

	bool found = false;

	ACS_CS_API_FunctionDistribution * funcDist = ACS_CS_API::createFunctionDistributionInstance();
	if ( !funcDist )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s: 'createFunctionDistributionInstance' failed! - Null object"), FX_NAME);
		return false;
	}

	ACS_CS_API_IdList ap_list;
	ACS_CS_API_NS::CS_API_Result call_result = funcDist->getFunctionProviders(ACS_CS_API_Name(fName), ap_list);
	if ( call_result != ACS_CS_API_NS::Result_Success )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s: 'getFunctionProviders' failed! - call_result == %d"), FX_NAME, call_result);
		return false;
	}
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s: Got the list of APs where the service <%s> is distributed (ap_list.size == %d)"),
			FX_NAME, fName, ap_list.size());

	for ( unsigned idx = 0; idx < ap_list.size(); idx++ )
	{
		if ( ap_list[idx] == apid )
		{
			found = true;
			break;
		}
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG,
			ACS_DSD_TEXT("%s: The service <%s> was %s on the local AP node!"),
			FX_NAME, fName, (found ? "found" : "not found"));

	ACS_CS_API::deleteFunctionDistributionInstance(funcDist);

	return found;
}

bool CpProt_address_request::isMessageStore(char* msName, std::string & ms_id) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	static char FX_NAME[] = "isMessageStore()";
#endif

	OmHandler om_handler;
	if ( om_handler.Init() )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s: 'om_handler.Init' failed! - Error message = %s"),
				FX_NAME, om_handler.getInternalLastErrorText());
		return false;
	}

	// Set up the dn to reach the MessageStore instances
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_DSD_IMM_ACA_RECORD_TRANSFER_ROOT_DN);

	// Fetch from IMM the message store objects
	std::vector<std::string> message_stores;
	if ( om_handler.getChildren(imm_dnName, ACS_APGCC_SUBLEVEL, &message_stores) )
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
			ACS_DSD_TEXT("%s: 'om_handler.getChildren' failed! - Error message = %s"),
			FX_NAME, om_handler.getInternalLastErrorText());

		om_handler.Finalize();
		return false;
	}

	acs_apgcc_paramhandling par_hdl;
	bool found = false;

	// Check if the given name is a message store name
	for ( std::vector<std::string>::iterator it = message_stores.begin(); it != message_stores.end(); it++ )
	{
		// First, get the Name attribute from IMM and check it with the given name
		char message_store_alias[ACS_DSD_IMM_ACA_MESSAGE_STORE_NAME_ATTR_SIZE] = {0};
		int call_result = par_hdl.getParameter<ACS_DSD_IMM_ACA_MESSAGE_STORE_NAME_ATTR_SIZE>(*it, ACS_DSD_IMM_ACA_MESSAGE_STORE_NAME_ATTR_NAME, message_store_alias);
		if ( call_result )
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR,
				ACS_DSD_TEXT("%s: 'par_hdl.getParameter' failed! - Error message = %s"),
				FX_NAME, par_hdl.getInternalLastErrorText());
			break;
		}

		// Get the message store ID from the dn
		size_t pos_equal = it->find_first_of('=');
		size_t pos_comma = it->find_first_of(',');
		ms_id = it->substr(pos_equal + 1, pos_comma - pos_equal - 1);

		// If the message store alias is equal to the given name, return true
		if ( ! ::strcasecmp(msName, message_store_alias) )
		{
			found = true;
			break;
		}
	}

	om_handler.Finalize();

	return found;
}


/**********************************************************************************************************************/
/* CpProt_address response method */

CpProt_address_response::CpProt_address_response()
{
	_protocol_id = PROTOCOL_CPAP;
	_protocol_version = CPAP_PROTOCOL_VERSION_0;
	_primitive_id = CPAP_ADDRESS_REPLY;
	_error = 0;
	_conn_type =0;
	_conn_num= 0;
	_ip1 = 0;
	_ip2 = 0;
	_port1 = 0;
	_port2 = 0;
	_process_prim_info.data_validation_result = ACS_DSD_PRIM_VALIDATION_OK;;
}

int CpProt_address_response::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(CPAP_ADDRESS_REPLY, CPAP_PROTOCOL_VERSION_0, _error, _conn_type,_conn_num,_ip1,_port1,_ip2,_port2);
}

int CpProt_address_response::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, _error, _conn_type,_conn_num,_ip1,_port1,_ip2,_port2);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, & _error, & _conn_type, & _conn_num, & _ip1, & _port1, & _ip2, & _port2);
}

int CpProt_address_response::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
//#ifdef ACS_DSD_LOGGING_ACTIVE
#ifdef ACS_DSD_HAS_LOGGING
	const char *FX_NAME ="CpProt_address_response:process";
#endif

	int returnCode = ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;

	if( _error!= acs_dsd::PCP_ERROR_CODE_SUCCESSFUL &&
		_error!= acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("[%s]: unexpected error code was replied from CP  Error =%u"),FX_NAME,_error);
		// An  Event Handler must be set
			_process_prim_info.data_validation_result = ACS_DSD_PRIM_BAD_ERROR_PARAMETER;
			return returnCode;
	}
	if( _error == acs_dsd::PCP_ERROR_CODE_SERVICE_NOT_REGISTERED){
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("[%s]: CP replied with Service not registered- Error =%u"),FX_NAME,_error);
			_process_prim_info.data_validation_result = ACS_DSD_PRIM_SERVICE_NOT_REGISTERED;
			return returnCode;
	}
	if(_conn_num == 0){
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("[%s]: unexpected conn_num was replied from CP  NrOfConn =%u"),FX_NAME,_conn_num);
		_process_prim_info.data_validation_result = ACS_DSD_PRIM_BAD_IP_ADDRESSES_COUNT;
		return returnCode;
	}

	_process_prim_info.data_validation_result = ACS_DSD_PRIM_VALIDATION_OK;
	return returnCode;
}

std::string CpProt_address_response::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u,"
				"CONNTYPE = %u,"
				"NROFCONN = %u,"
				"IP1=%u,"
				"PORT1=%u,"
				"IP2=%u,"
				"PORT2=%u",
				_primitive_id,
				_protocol_version,
				_error,
				_conn_type,
				_conn_num,
				_ip1,
				_port1,
				_ip2,
				_port2);

	return mess;
}
