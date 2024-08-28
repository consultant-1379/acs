#include <iostream>
#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "ACS_DSD_ConfigurationHelper.h"
#include "ACS_DSD_ImmDataHandler.h"


const IMM_DH_Attrubutes IMM_DH_SRVInfoAttr[IMM_DH_SRVINFO_CLASS_ATTR_MAXNUM+1]= {
		{IMM_DH_SRVINFO_RDN_ATTR_NAME,SRVINFO_RDN_ATTR_TYPE,1},
		{IMM_DH_SRVINFO_NAME_ATTR_NAME,SRVINFO_NAME_ATTR_TYPE,1},
		{IMM_DH_SRVINFO_DOMAIN_ATTR_NAME,SRVINFO_DOMAIN_ATTR_TYPE,1},
		{IMM_DH_SRVINFO_CONNTYPE_ATTR_NAME,SRVINFO_CONN_ATTR_TYPE,1},
		{IMM_DH_SRVINFO_PROCNAME_ATTR_NAME,SRVINFO_PROCNAME_ATTR_TYPE,1},
		{IMM_DH_SRVINFO_PID_ATTR_NAME,SRVINFO_PID_ATTR_TYPE,1},
		{IMM_DH_SRVINFO_VISIB_ATTR_NAME,SRVINFO_VISIB_ATTR_TYPE,1},
		{IMM_DH_SRVINFO_INET_ADDR_ATTR_NAME,SRVINFO_INETADDR_ATTR_TYPE,acs_dsd::CONFIG_NETWORKS_SUPPORTED},
		{IMM_DH_SRVINFO_UNIX_ADDR_ATTR_NAME,SRVINFO_UNIXADDR_ATTR_TYPE,acs_dsd::CONFIG_NETWORKS_SUPPORTED}
};

const IMM_DH_Attrubutes IMM_DH_SRTNodeAttr[IMM_DH_SRT_CLASS_ATTR_MAXNUM+1]= {
		{IMM_DH_SRTNODE_RDN_ATTR_NAME,SRTNODE_RDN_ATTR_TYPE,1},
		{IMM_DH_SRTNODE_NODE_ATTR_NAME,SRTNODE_NODE_ATTR_TYPE,1},
		{IMM_DH_SRTNODE_SIDE_ATTR_NAME,SRTNODE_SIDE_ATTR_TYPE,1},
		{IMM_DH_SRTNODE_STATE_ATTR_NAME,SRTNODE_STATE_ATTR_TYPE,1}
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_ImmDataHandler

__CLASS_NAME__ ::__CLASS_NAME__(ACS_DSD_ImmConnectionHandler *ImmConnHandler)
{
	int i;
	//strcpy(_attributes.node,"");
	*(_attributes.node) = 0;
	//strcpy(_attributes.proc_name,"");
	*(_attributes.proc_name) = 0;
	_attributes.side = acs_dsd::NODE_SIDE_UNDEFINED;
	_attributes.conn_type = 0;
	_attributes.pid = 0;
	_attributes.visibility = 0;      	//default for visibility is Global scope

	for(i=0;i< IMM_DH_SRVINFO_CLASS_ATTR_MAXNUM;i++)
	{
		_attributes.SRVInfo_attr_flag[i]=0;
	}
	for(i=0;i< IMM_DH_SRT_CLASS_ATTR_MAXNUM;i++)
		{
			_attributes.SRTNode_attr_flag[i]=0;
		}
	for (i=0;i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
	{
		strcpy(_attributes.tcp_addresses[i],"");
	}
	_attributes.tcp_addr_num = 0;
	for (i=0;i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
	{
		strcpy(_attributes.unix_addresses[i],"");
	}
	_attributes.unix_addr_num = 0;
	_service_node_info = IMM_DH_LOCAL_NODE;
	_ImmCHobj=ImmConnHandler;
	_omHandlerInitialized=false;
	strcpy(_last_error_text,"");
	memset(_attributes.SRTNode_rdn,'\0', sizeof(_attributes.SRTNode_rdn));
	memset(_attributes.SRVInfo_rdn,'\0', sizeof(_attributes.SRVInfo_rdn));
	memset(_attributes.domain, '\0', sizeof(_attributes.domain));
	memset(_attributes.name, '\0', sizeof(_attributes.name));
	_attributes.state = acs_dsd::NODE_STATE_UNDEFINED;
}

int __CLASS_NAME__ ::omHandler_init()
{
	ACS_CC_ReturnType imm_call_result;

	if ((imm_call_result = _om_handler.Init())!= ACS_CC_SUCCESS){
			snprintf(_last_error_text,ERROR_TEXT_SIZE,
								"error in fetchRegisteredServicesList: OmHandler::Init()' failed: return code == %d", imm_call_result);
			return acs_dsd_imm::ERR_IMM_OM_INIT;
		}
	_omHandlerInitialized=true;
	return acs_dsd_imm::NO_ERROR;
}

void __CLASS_NAME__ ::set_tcp_addresses (const char **value, uint16_t attr_num)
{
	for(int i=0;i < attr_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
	{
		strncpy(_attributes.tcp_addresses[i], value[i],acs_dsd::CONFIG_NETWORK_NAME_SIZE_MAX+8);
	}
	_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_inetaddr_attr]=1;
	_attributes.tcp_addr_num = attr_num;
}

void __CLASS_NAME__ ::set_unix_addresses (const char **value, uint16_t attr_num)
{
	for(int i=0;i < attr_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
	{
		strncpy(_attributes.unix_addresses[i], value[i],acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
	}
	_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_unixaddr_attr]=1;
	_attributes.unix_addr_num = attr_num;
}

void __CLASS_NAME__ ::get_tcp_addresses (char *value[acs_dsd::CONFIG_NETWORKS_SUPPORTED], uint16_t &attr_num) const
{
	for(int i=0;i<_attributes.tcp_addr_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
	{
		strncpy(value[i],_attributes.tcp_addresses[i],acs_dsd::CONFIG_NETWORK_NAME_SIZE_MAX+8);
	}
	attr_num = _attributes.tcp_addr_num ;
}

void __CLASS_NAME__ ::get_inet_addresses (ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED], uint16_t &attr_num) const
{
	attr_num = 0;

	for(int i=0; i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++){
		if(_attributes.tcp_addresses[i][0]){
			inet_addresses[i].set(_attributes.tcp_addresses[i]);
			attr_num++;
		}
	}
}

void __CLASS_NAME__ ::get_unix_addresses (char (*value)[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX], uint16_t &attr_num) const
{
	for(int i=0;i<_attributes.unix_addr_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
	{
		strncpy(value[i],_attributes.unix_addresses[i],acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX);
	}
	attr_num =_attributes.unix_addr_num;
}

void __CLASS_NAME__ ::reset_attributes(void)
{
	strcpy(_attributes.node,"");
	strcpy(_attributes.proc_name,"");
	strcpy(_attributes.name,"");
	strcpy(_attributes.domain,"");
	_attributes.side = acs_dsd::NODE_SIDE_UNDEFINED; _attributes.conn_type = 0;
	_attributes.pid= 0; _attributes.visibility = IMM_DH_GLOBAL_SCOPE;

	for(int i=0;i< IMM_DH_SRVINFO_CLASS_ATTR_MAXNUM;i++) _attributes.SRVInfo_attr_flag[i]=0;

	for(int i=0;i< IMM_DH_SRT_CLASS_ATTR_MAXNUM;i++) _attributes.SRTNode_attr_flag[i]=0;

	for (int i=0;i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++){
		strcpy(_attributes.tcp_addresses[i],""); strcpy(_attributes.unix_addresses[i],"");
	}
	_attributes.tcp_addr_num = 0; _attributes.unix_addr_num = 0;
}

/* This method instances an object of the class above defined and provide values for all attribute*/
int __CLASS_NAME__ ::addNodeInfo(const char *nodeName)
{
	int ret_code=0;
	ACS_CC_ReturnType imm_call_result;

	if(!nodeName || !*nodeName ){
		strncpy(_last_error_text,"error in addNodeInfo: empty parameters ", ERROR_TEXT_SIZE);
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	char rdnValue[ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX]={0};
	void* valueAttr[IMM_DH_SRTNODE_ATTRVALUE_MAXNUM*2];

	//char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};
	const char * imm_dnName = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dnName)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	//snprintf(rdnValue,acs_dsd::CONFIG_IMM_RDN_SIZE_MAX,"%s=%s", ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME,nodeName);
	snprintf(rdnValue,ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX,"%s=%s", ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME,nodeName);

	strncpy(_attributes.SRTNode_rdn,rdnValue,ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX);
	_attrList.clear();

    acs_dsd_imm::DSD_IMM_Srt_Attribute srt_rdn_attrib = acs_dsd_imm::SRT_rdn_attr;

    for(int i=srt_rdn_attrib; i<=acs_dsd_imm::SRT_state_attr; i++){
    	setServiceTableAttribute((acs_dsd_imm::DSD_IMM_Srt_Attribute)i,&valueAttr[i]);
    }
	imm_call_result = _ImmCHobj->createRuntimeObj(ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME, imm_dnName, _attrList);

	if(imm_call_result != ACS_CC_SUCCESS){
		int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
				"addNodeInfo-createRuntimeObj failed. Return code == %d. APGCC internal last error is '%d'", imm_call_result, apgcc_internal_last_error);
		ret_code = -1;

		// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
		if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
			_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
	}
	return ret_code;
}

/* This method deletes an object of the class SRVInfo*/
int __CLASS_NAME__ ::deleteServiceInfo(const char *nodeName, const char *serviceName, const char *domain)
{
	// check parameters
	if(!nodeName || !serviceName || !domain || !*nodeName || !*serviceName || !*domain){
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"error in unregisterServiceInfo: empty parameters ");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	int returnCode=0;

	char rdnValue[ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX]={0};
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	/*The RDN of the class to be defined*/
	snprintf(rdnValue,ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX,"%s=%s@%s",ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME,serviceName,domain);
	/*The DN name of the class to be defined*/
	snprintf(imm_dnName,acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,"%s,%s=%s,%s", rdnValue, ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME,nodeName,
			imm_dsd_root_dn);

	returnCode = deleteServiceInfo(imm_dnName);
	return returnCode;
}

int __CLASS_NAME__ ::deleteServiceInfo(const char *dnName)
{
	int ret_code=0;
	ACS_CC_ReturnType imm_call_result;

	imm_call_result = _ImmCHobj->deleteRuntimeObj(dnName);

	if(imm_call_result != ACS_CC_SUCCESS){
		int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
		snprintf(_last_error_text,ERROR_TEXT_SIZE, "deleteSRVInfoNodeObj:deleteRuntimeObj(%s)failed, retcode == %d. APGCC internal last error is '%d'", dnName, imm_call_result, apgcc_internal_last_error);
		ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_DELETE;

		// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
		if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
			_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
	}
	return ret_code;
}


/* This method deletes all the objects of the class SRVInfo having
 * as root object the SRTNode having the given name. */
int __CLASS_NAME__::deleteAllServicesInfo(const char *node_name)
{
	// Check parameters
	if(!node_name || !*node_name)
	{
		strncpy(_last_error_text, "error in 'deleteAllServicesInfo': empty parameters ", ERROR_TEXT_SIZE);
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	int return_code = acs_dsd_imm::NO_ERROR;
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};
	const char * imm_dsd_root_dn = 0;

	// Search the parent name of the class to be removed
	int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn);
	if ( call_result )
	{
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: "
			"while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	if( !_omHandlerInitialized && (return_code = omHandler_init())!= acs_dsd_imm::NO_ERROR )
	{
		snprintf(_last_error_text, ERROR_TEXT_SIZE,
				"deleteAllServicesInfo() - omHandler_init() failed: return code == %d", return_code);
		return return_code;
	}

	// Make up the dn to reach the SRTNodeinstance object
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s", node_name, imm_dsd_root_dn);

	// Get all the children objects of the given SRTNode
	std::vector<std::string> services_dnList;
	ACS_CC_ReturnType imm_call_result = _om_handler.getChildren(imm_dnName, ACS_APGCC_SUBLEVEL, &services_dnList);

	if ( imm_call_result == ACS_CC_SUCCESS )
	{
		for ( unsigned int i=0; i < services_dnList.size(); i++ )
		{
			imm_call_result = _ImmCHobj->deleteRuntimeObj(services_dnList[i].c_str());
			if ( imm_call_result != ACS_CC_SUCCESS ) {
				int apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
				snprintf(_last_error_text,ERROR_TEXT_SIZE,
						"deleteAllServicesInfo:deleteRuntimeObj(%s)failed, retcode == %d. APGCC internal last error is '%d'",
						services_dnList[i].c_str(), imm_call_result, apgcc_internal_last_error);
				return_code = acs_dsd_imm::ERR_IMM_RUNTIME_DELETE;

				// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
				if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
					_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
			}
		}
	}
	else
	{
		snprintf(_last_error_text, ERROR_TEXT_SIZE,
						"Error in 'deleteAllServicesInfo': getChildren(%s) failed: return code == %d",
						imm_dnName, imm_call_result);
		return_code = acs_dsd_imm::ERR_IMM_GET_CHILDREN;
	}

	return return_code;
}


/* This method deletes an object of the class SRTNode*/
int __CLASS_NAME__ ::removeNodeInfo(const char *nodeName)
{
	// check parameters
	if(!nodeName || !*nodeName){
		strncpy(_last_error_text,"error in removeNodeInfo: empty parameters ", ERROR_TEXT_SIZE);
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}
	int ret_code=0;
	ACS_CC_ReturnType imm_call_result;
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	//Make up the dn to reach the SRVInfo instance object
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
		nodeName, imm_dsd_root_dn);

	if( !_omHandlerInitialized && (ret_code = omHandler_init())!= acs_dsd_imm::NO_ERROR )
	{
		snprintf(_last_error_text, ERROR_TEXT_SIZE,
				"deleteAllServicesInfo() - omHandler_init() failed: return code == %d", ret_code);
		return ret_code;
	}

	/*The RDN of the class to be defined*/
	std::vector<std::string> services_dnList;

	if ((imm_call_result = _om_handler.getChildren(imm_dnName,ACS_APGCC_SUBLEVEL,&services_dnList)) == ACS_CC_SUCCESS)
	{
		// delete all service node info created under SrtNode
		for(unsigned int i=0; i<services_dnList.size(); i++){
			if ((imm_call_result = _ImmCHobj->deleteRuntimeObj(services_dnList[i].c_str()))!= ACS_CC_SUCCESS){
				int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
				snprintf(_last_error_text,ERROR_TEXT_SIZE,
									"error in removeNodeInfo: deleteRuntimeObj(%s)' failed: return code == %d. APGCC internal last error is '%d'",services_dnList[i].c_str(), imm_call_result, apgcc_internal_last_error);
				ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_DELETE;

				// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
				if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
					_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
			}
		}
	}
	if (ret_code != acs_dsd_imm::ERR_IMM_RUNTIME_DELETE){
		// Now delete SRTnode
		if ((imm_call_result = _ImmCHobj->deleteRuntimeObj(imm_dnName))!= ACS_CC_SUCCESS){
			int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
			snprintf(_last_error_text,ERROR_TEXT_SIZE,
											"error in removeNodeInfo: deleteRuntimeObj(%s)' failed: return code == %d. APGCC internal last error is '%d'", imm_dnName, imm_call_result, apgcc_internal_last_error);
			ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_DELETE;

			// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
			if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
				_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
		}
	}
	return ret_code;
}


/* This method modify the attributes of an object of the class SRVInfo*/
int __CLASS_NAME__ ::modifyServiceInfo(const char *nodeName,const char *serviceName,const char *domain)
{
	 // check parameters
	if(!nodeName || !serviceName || !domain || !*nodeName || !*serviceName || !*domain){
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"error in modifyServiceInfo: parameters empty");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}
	int ret_code=acs_dsd_imm::NO_ERROR;
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	/*The DN name of the class to be defined*/
	snprintf(imm_dnName,acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
		ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
		serviceName, domain, nodeName, imm_dsd_root_dn);

	if(_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_procname_attr])
	{
		ret_code = modifyAttribute(imm_dnName, acs_dsd_imm::SRV_procname_attr);
	}
	if(_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_conntype_attr] && !ret_code)
	{
		ret_code = modifyAttribute(imm_dnName, acs_dsd_imm::SRV_conntype_attr);
	}
	if(_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_pid_attr] && !ret_code)
	{
		ret_code = modifyAttribute(imm_dnName, acs_dsd_imm::SRV_pid_attr);
	}
	if(_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_inetaddr_attr] && !ret_code)
	{
		ret_code = modifyAttribute(imm_dnName, acs_dsd_imm::SRV_inetaddr_attr);
	}
	if(_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_unixaddr_attr] && !ret_code)
	{
		ret_code = modifyAttribute(imm_dnName, acs_dsd_imm::SRV_unixaddr_attr);
	}
	if(_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_visibility_attr] && !ret_code)
	{
		ret_code = modifyAttribute(imm_dnName, acs_dsd_imm::SRV_visibility_attr);
	}

	reset_attributes();
	return ret_code;
}

/* This method deletes an object of the class SRVInfo*/
int __CLASS_NAME__ ::modifyNodeInfo(const char *nodeName, uint16_t n_state)
{
	// check parameters
	if(!nodeName|| !*nodeName ){
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"error in modifyNodeInfo: parameters empty");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	int ret=0;
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	/*The DN name of the class to be defined*/
	snprintf(imm_dnName,acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
		ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s", nodeName, imm_dsd_root_dn);

	_attributes.state= (uint32_t)n_state;
	ret = modifyAttribute(imm_dnName,acs_dsd_imm::SRT_state_attr);
	//ret = modifyAttribute(imm_dnName,acs_dsd_imm::SRT_node_attr);
	return ret;
}

/* This method modify the attributes of an object of the class SRVInfo*/
int __CLASS_NAME__ ::addServiceInfo(const char *nodeName, const char *serviceName, const char *domain)
{

	 // check parameters
	if(!nodeName || !serviceName || !domain || !*nodeName || !*serviceName || !*domain){
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"error in addServiceInfo: parameters empty");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}
	ACS_CC_ReturnType imm_call_result;
	int ret_code=acs_dsd_imm::NO_ERROR;
	char rdnValue[ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX]={0};
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};
	void* valueAttr[IMM_DH_SRVINFO_ATTRVALUE_MAXNUM];

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	/*The DN name of the class to be defined*/
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
		ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s", nodeName, imm_dsd_root_dn);

	/*The RDN of the class to be defined*/
	snprintf(rdnValue,ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX,"%s=%s@%s",ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME,serviceName,domain);
	_attrList.clear();
	strncpy(_attributes.SRVInfo_rdn,rdnValue,ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX);
	strncpy(_attributes.name,serviceName,acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);
	strncpy(_attributes.domain,domain,acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);

	acs_dsd_imm::DSD_IMM_Srv_Attribute srv_rdn_attrib = acs_dsd_imm::SRV_rdn_attr;
	int  attrib_value_num, shift_index = 0;
	for(int i=srv_rdn_attrib; i<=acs_dsd_imm::SRV_unixaddr_attr; i++){
		attrib_value_num = setServiceInfoAttribute((acs_dsd_imm::DSD_IMM_Srv_Attribute)i, &valueAttr[i+shift_index]); // SRV_inetaddr_attr and SRV_unix_attr are multivalue
	    shift_index = attrib_value_num-1;
	}
	imm_call_result = _ImmCHobj->createRuntimeObj(ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME, imm_dnName, _attrList);

	if(imm_call_result != ACS_CC_SUCCESS){
			int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
			snprintf(_last_error_text,ERROR_TEXT_SIZE,"addServiceInfo(%s):createRuntimeObj failed returnCode =%d. APGCC internal last error is '%d'",imm_dnName, imm_call_result, apgcc_internal_last_error);
			ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_CREATE;

			// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
			if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
				_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
	}
	return ret_code;
}

void __CLASS_NAME__ ::setServiceTableAttribute(acs_dsd_imm::DSD_IMM_Srt_Attribute attrib, void **valueAttr)
{
	ACS_CC_ValuesDefinitionType srt_attribute;
	bool attrib_found =true;

	switch(attrib){
		case acs_dsd_imm::SRT_rdn_attr:
			valueAttr[0]=reinterpret_cast<void*>(_attributes.SRTNode_rdn);
		break;
		case acs_dsd_imm::SRT_node_attr :
					valueAttr[0]=reinterpret_cast<void*>(_attributes.node);
		break;
		case acs_dsd_imm::SRT_side_attr :
					valueAttr[0]=reinterpret_cast<void*>(&(_attributes.side));
		break;
		case acs_dsd_imm::SRT_state_attr :
			valueAttr[0]=reinterpret_cast<void*>(&(_attributes.state));
		break;
		default:
			attrib_found = false;
		break;
	}
    if(attrib_found){
    	srt_attribute.attrName = const_cast<char*>(IMM_DH_SRTNodeAttr[attrib].attr_name);
    	srt_attribute.attrType = (ACS_CC_AttrValueType) IMM_DH_SRTNodeAttr[attrib].attr_type;
    	srt_attribute.attrValuesNum = IMM_DH_SRTNodeAttr[attrib].attr_num;
    	srt_attribute.attrValues = valueAttr;

    	_attrList.push_back(srt_attribute);
    }
}

int  __CLASS_NAME__ ::setServiceInfoAttribute(acs_dsd_imm::DSD_IMM_Srv_Attribute attrib, void **valueAttr)
{
	ACS_CC_ValuesDefinitionType srv_attribute;
	uint16_t attr_value_num =1;
	bool attrib_found = true;

	switch(attrib){
		case acs_dsd_imm::SRV_rdn_attr:
			valueAttr[0]=reinterpret_cast<void*>(_attributes.SRVInfo_rdn);
		break;
		case acs_dsd_imm::SRV_name_attr :
			valueAttr[0]=reinterpret_cast<void*>(_attributes.name);
		break;
		case acs_dsd_imm::SRV_domain_attr :
			valueAttr[0]=reinterpret_cast<void*>(_attributes.domain);
		break;
		case acs_dsd_imm::SRV_procname_attr :
			valueAttr[0]=reinterpret_cast<void*>(_attributes.proc_name);
		break;
		case acs_dsd_imm::SRV_conntype_attr:
			valueAttr[0]=reinterpret_cast<void*>(&(_attributes.conn_type));
		break;
		case acs_dsd_imm::SRV_pid_attr:
			valueAttr[0]=reinterpret_cast<void*>(&(_attributes.pid));
		break;
		case acs_dsd_imm::SRV_visibility_attr:
					valueAttr[0]=reinterpret_cast<void*>(&(_attributes.visibility));
		break;
		case acs_dsd_imm::SRV_inetaddr_attr :
			attr_value_num = _attributes.tcp_addr_num;
			for(int i=0;i<attr_value_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++){
				valueAttr[i]= reinterpret_cast<void*>(_attributes.tcp_addresses[i]);
						//p_mvalue[1]= reinterpret_cast<void*>(tcp_address2);
			}
		break;
		case acs_dsd_imm::SRV_unixaddr_attr:
			attr_value_num = _attributes.unix_addr_num;
			for(int i=0;i<attr_value_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++){
				valueAttr[i]= reinterpret_cast<void*>(_attributes.unix_addresses[i]);
							//p_mvalue[1]= reinterpret_cast<void*>(tcp_address2);
			}
		break;
		default:
			attrib_found = false;
		break;
	}

    if(attrib_found){
    	srv_attribute.attrName = const_cast<char*>(IMM_DH_SRVInfoAttr[attrib].attr_name);
    	srv_attribute.attrType = (ACS_CC_AttrValueType) IMM_DH_SRVInfoAttr[attrib].attr_type;
    	srv_attribute.attrValuesNum = attr_value_num;
    	srv_attribute.attrValues = valueAttr;

    	_attrList.push_back(srv_attribute);
    }
    return attr_value_num;
}

int  __CLASS_NAME__ ::modifyAttribute(const char *imm_dnName, acs_dsd_imm::DSD_IMM_Srv_Attribute attrib)
{
	ACS_CC_ReturnType imm_call_result;
	ACS_CC_ImmParameter param2modify;
	int ret_code=acs_dsd_imm::NO_ERROR;
	uint16_t attr_value_num =1;
	bool attrib_found = true;
	void* value[acs_dsd::CONFIG_NETWORKS_SUPPORTED];

	switch(attrib)
	{
		case acs_dsd_imm::SRV_conntype_attr :
			value[0]=reinterpret_cast<void*>(&(_attributes.conn_type));
			break;
		case acs_dsd_imm::SRV_procname_attr :
			value[0]=reinterpret_cast<void*>(_attributes.proc_name);
			break;
		case acs_dsd_imm::SRV_pid_attr:
			value[0]=reinterpret_cast<void*>(&(_attributes.pid));
			break;
		case acs_dsd_imm::SRV_visibility_attr:
			value[0]=reinterpret_cast<void*>(&(_attributes.visibility));
			break;
		case acs_dsd_imm::SRV_inetaddr_attr :
			attr_value_num = _attributes.tcp_addr_num;
			for(int i=0;i<attr_value_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
				value[i]= reinterpret_cast<void*>(_attributes.tcp_addresses[i]);
			break;
		case acs_dsd_imm::SRV_unixaddr_attr:
			attr_value_num = _attributes.unix_addr_num;
			for(int i=0;i<attr_value_num && i<acs_dsd::CONFIG_NETWORKS_SUPPORTED;i++)
				value[i]= reinterpret_cast<void*>(_attributes.unix_addresses[i]);
			break;
		default:
			attrib_found = false;
			break;
	}
	if(attrib_found){
		param2modify.attrName = const_cast<char*>(IMM_DH_SRVInfoAttr[attrib].attr_name);
		param2modify.attrType = (ACS_CC_AttrValueType) IMM_DH_SRVInfoAttr[attrib].attr_type;
		param2modify.attrValuesNum = attr_value_num;
		param2modify.attrValues = value;

		imm_call_result = _ImmCHobj->modifyRuntimeObj(imm_dnName, &param2modify);

		if(imm_call_result != ACS_CC_SUCCESS){
			int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
			snprintf(_last_error_text,ERROR_TEXT_SIZE,"error in modifyAttribute: modifyRuntimeObj failed retcode == %d. APGCC internal last error is '%d'",imm_call_result, apgcc_internal_last_error);
			ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_MODIFY;

			// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
			if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
				_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
		}
	}
	return ret_code;
}

int __CLASS_NAME__ ::modifyAttribute(const char *imm_dnName,acs_dsd_imm::DSD_IMM_Srt_Attribute attrib)
{
	ACS_CC_ReturnType imm_call_result;
	ACS_CC_ImmParameter param2modify;
	int ret_code=acs_dsd_imm::NO_ERROR;
	bool attrib_found = true;
	void* valueAttr[1];

	switch(attrib)
	{
		case acs_dsd_imm::SRT_state_attr:
			valueAttr[0]=reinterpret_cast<void*>(&(_attributes.state));
		break;

		case acs_dsd_imm::SRT_node_attr:
			valueAttr[0]=reinterpret_cast<void*>(_attributes.node);
		break;

		default:
			attrib_found = false;
		break;
	}
	if(attrib_found){
		param2modify.attrName = const_cast<char*>(IMM_DH_SRTNodeAttr[attrib].attr_name);
		param2modify.attrType = (ACS_CC_AttrValueType) IMM_DH_SRTNodeAttr[attrib].attr_type;
		param2modify.attrValuesNum = IMM_DH_SRTNodeAttr[attrib].attr_num;
		param2modify.attrValues = valueAttr;

		imm_call_result = _ImmCHobj->modifyRuntimeObj(imm_dnName, &param2modify);

		if(imm_call_result != ACS_CC_SUCCESS){
			int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
			snprintf(_last_error_text,ERROR_TEXT_SIZE,"error in modifyAttribute: modifyRuntimeObj failed retcode == %d. APGCC internal last error is '%d'",imm_call_result, apgcc_internal_last_error);
			ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_MODIFY;

			// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
			if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
				_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
		}
	}
	return ret_code;
}

int  __CLASS_NAME__ ::fetch_ServiceInfo(const char *nodeName,const char *serviceName,const char *domain, int check_running)
{
	int ret_code = acs_dsd_imm::NO_ERROR;
	// check parameters
	if(!nodeName || !serviceName || !domain || !*nodeName || !*serviceName || !*domain){
		snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in fetch_ServiceInfo: parameters empty");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}
	if(check_running){
		ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t p_local_node;
		if(ACS_DSD_ConfigurationHelper::get_my_ap_node(p_local_node) < 0) {
			snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in fetch_ServiceInfo: cannot obtain local AP node info");
			return acs_dsd_imm::ERR_GENERIC_FAILURE;
		}
		if(strcmp(p_local_node->node_name, nodeName)) {
			snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in fetch_ServiceInfo: parameter 'check_running' can be 1 only for local AP node");
			return acs_dsd_imm::ERR_INPUT_PARAMETER_INVALID;
		}
	}

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	if(!_omHandlerInitialized && (ret_code= omHandler_init())!= acs_dsd_imm::NO_ERROR){
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_ServiceInfo(%s)-ImmDHobj.init() failed: return code == %d", nodeName, ret_code);
		return ret_code;
	}

	ACS_CC_ReturnType imm_call_result;
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

		//Make up the dn to reach the SRVInfo instance object
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
		ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
		serviceName, domain, nodeName, imm_dsd_root_dn);

	// try to fetch the obj from IMM
	ACS_APGCC_ImmObject imm_object;
	imm_object.objName =  imm_dnName;

	if ((imm_call_result = _om_handler.getObject(&imm_object)) == ACS_CC_SUCCESS)
	{
		unsigned attributes_num = imm_object.attributes.size();
		//  reset all attributes
		reset_attributes();

		//Search for  attributes
		for (unsigned i = 0; i < attributes_num; ++i)
		{
			const char * attr_name = imm_object.attributes[i].attrName.c_str();
			unsigned values_count = imm_object.attributes[i].attrValuesNum;

			if (!strcmp(attr_name,IMM_DH_SRVINFO_NAME_ATTR_NAME)) {
				if (values_count && (reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0])))
					set_serviceName(reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0]));
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRVINFO_DOMAIN_ATTR_NAME)){
				if (values_count && (reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0])))
					set_domainName(reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0]));
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRVINFO_CONNTYPE_ATTR_NAME)){
				if (values_count && (reinterpret_cast<uint32_t *>(imm_object.attributes[i].attrValues[0])))
					set_conn_type(*reinterpret_cast<uint32_t *>(imm_object.attributes[i].attrValues[0]));
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRVINFO_PROCNAME_ATTR_NAME)) {
				if (values_count && (reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0])))
					set_proc_name(reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0]));
				else {
					ret_code = acs_dsd_imm::ERR_IMM_EMPTY_VALUESNUM_ATTR;
					break;
				}
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRVINFO_PID_ATTR_NAME)){
				if (values_count && (reinterpret_cast<int32_t *>(imm_object.attributes[i].attrValues[0])))
					set_pid(*reinterpret_cast<int32_t *>(imm_object.attributes[i].attrValues[0]));
				else {
					ret_code = acs_dsd_imm::ERR_IMM_EMPTY_VALUESNUM_ATTR;
					break;
				}
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRVINFO_VISIB_ATTR_NAME)){
				if (values_count && (reinterpret_cast<uint32_t *>(imm_object.attributes[i].attrValues[0])))
					set_visibility(*reinterpret_cast<uint32_t *>(imm_object.attributes[i].attrValues[0]));
				else {
					ret_code = acs_dsd_imm::ERR_IMM_EMPTY_VALUESNUM_ATTR;
					break;
				}
				continue;
			}
			int addr_unix_type =-1;
			int addr_inet_type = strcmp(attr_name, IMM_DH_SRVINFO_INET_ADDR_ATTR_NAME);
			if(addr_inet_type) addr_unix_type = strcmp(attr_name, IMM_DH_SRVINFO_UNIX_ADDR_ATTR_NAME);
			if (!addr_inet_type || !addr_unix_type){
				char ** values = reinterpret_cast<char **>(imm_object.attributes[i].attrValues);
				if (values_count && values && *values) {
					if (!addr_inet_type) set_tcp_addresses((const char **)values, values_count);
					if (!addr_unix_type) set_unix_addresses((const char **)values, values_count);
				}
			}
		}

		// take into account "check_running" parameter
		if(check_running && !ACS_DSD_ConfigurationHelper::process_running(_attributes.proc_name,_attributes.pid)) {
			snprintf(_last_error_text, ERROR_TEXT_SIZE,
					"error in %s: the retrieved object '%s' is INVALID (service NOT running on local AP node)",  __func__, imm_dnName);
			ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
		}
	}
	else{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"OmHandler::getObject(...' failed to retrieve the object '%s': return code == %d", imm_dnName, imm_call_result);
		ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
	}

	return ret_code;
}

int  __CLASS_NAME__ ::fetchRegisteredServicesList(const char *nodeName, std::vector<std::string> &reg_serviceList)
{
	 // check parameters
	if(!nodeName || !*nodeName){
		snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in fetchRegisteredServicesList: parameter empty");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	ACS_CC_ReturnType imm_call_result;
	int ret_code=acs_dsd_imm::NO_ERROR;
	if(!_omHandlerInitialized && (ret_code= omHandler_init())!= acs_dsd_imm::NO_ERROR){
			snprintf(_last_error_text,ERROR_TEXT_SIZE,"unregisterOldServicesfromIMM(%s)-ImmDHobj.init() failed: return code == %d",
																				nodeName, ret_code);
			return ret_code;
	}

	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

	//Make up the dn to reach the SRTNode instance object
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s", nodeName, imm_dsd_root_dn);

	// try to fetch the obj from IMM
	std::vector<std::string> services_dnList;

	if ((imm_call_result = _om_handler.getChildren(imm_dnName,ACS_APGCC_SUBLEVEL,&services_dnList)) != ACS_CC_SUCCESS)
	{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,
									"error in fetchRegisteredServicesList: om_handler.getChildren' failed: return code == %d", imm_call_result);
		ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
	}
	else
		// getChildren gave the service list
		reg_serviceList=services_dnList;

	return ret_code;
}

int  __CLASS_NAME__ ::fetch_serviceProcName_ProcId(const char *dn_service)
{
	ACS_CC_ReturnType imm_call_result;
	int ret_code = acs_dsd_imm::NO_ERROR;

	if(!_omHandlerInitialized && (ret_code= omHandler_init())!= acs_dsd_imm::NO_ERROR){
			snprintf(_last_error_text,ERROR_TEXT_SIZE,"unregisterOldServicesfromIMM(%s)-ImmDHobj.init() failed: return code == %d",
											dn_service, ret_code);
			return ret_code;
	}
	// try to fetch the obj from IMM
	ACS_APGCC_ImmObject imm_object;

	imm_object.objName =  dn_service;

	if ((imm_call_result = _om_handler.getObject(&imm_object)) == ACS_CC_SUCCESS){
		unsigned attributes_num = imm_object.attributes.size();

		//Search for  attributes
		int  attr_2_fetch = 0;

		for (unsigned j = 0; j < attributes_num && attr_2_fetch < 2; ++j){
			const char * attr_name = imm_object.attributes[j].attrName.c_str();
			unsigned values_count = imm_object.attributes[j].attrValuesNum;

			if (!strcmp(attr_name,IMM_DH_SRVINFO_PROCNAME_ATTR_NAME)){
				char * value = reinterpret_cast<char *>(imm_object.attributes[j].attrValues[0]);
				if (values_count && value) {
					strncpy(_attributes.proc_name,value,acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX);
					attr_2_fetch++;
				} else snprintf(_last_error_text,ERROR_TEXT_SIZE,"unregisterOldServicesfromIMM: "IMM_DH_SRVINFO_PROCNAME_ATTR_NAME" attribute is empty");
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRVINFO_PID_ATTR_NAME)){
				int32_t * pid_ptr = reinterpret_cast<int32_t *>(imm_object.attributes[j].attrValues[0]);
				if (values_count && pid_ptr) {
					_attributes.pid = *pid_ptr;
					attr_2_fetch++;
				} else snprintf(_last_error_text,ERROR_TEXT_SIZE,"unregisterOldServicesfromIMM: "IMM_DH_SRVINFO_PID_ATTR_NAME" attribute is empty");
				continue;
			}
		}
		if (attr_2_fetch != 2){
			snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_serviceProcName_ProcId(%s)ATTRIBUTES NOT FOUND!!!", dn_service);
			ret_code = acs_dsd_imm::ERR_IMM_OM_ATTR_NOT_FOUND;
		}
	}
	else{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_serviceProcName_ProcId(%s)-OmHandler::getObject(...' failed: return code == %d",
																	dn_service, imm_call_result);
		ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
	}

	return ret_code;
}

int __CLASS_NAME__ ::fetch_serv_addresses_fromIMM(const char *service_name, const char *domain, const char *local_node_name,const char *partner_node_name, int check_running)
{
	const char FX_NAME[] = "fetch_serv_addresses_fromIMM: ";
	ACS_CC_ReturnType imm_call_result;
	int ret_code = acs_dsd_imm::NO_ERROR;

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	if(!_omHandlerInitialized && (ret_code= omHandler_init())!= acs_dsd_imm::NO_ERROR){
			snprintf(_last_error_text,ERROR_TEXT_SIZE,"%s - ImmDHobj.init() failed: return code == %d", FX_NAME, ret_code);
			return ret_code;
	}

	 // check parameters
	if(!local_node_name || !service_name || !domain || !*local_node_name || !*service_name || !*domain) {
		snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in %s parameters empty",FX_NAME);
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	_service_node_info = IMM_DH_LOCAL_NODE;
	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

		//Make up the dn to reach the SRVInfo instance object
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
		ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
		service_name, domain, local_node_name, imm_dsd_root_dn);

	// try to fetch the obj from IMM from <SRTNode=local_node_name>
	ACS_APGCC_ImmObject imm_object;
	imm_object.objName =  imm_dnName;

	if ((imm_call_result = _om_handler.getObject(&imm_object)) != ACS_CC_SUCCESS &&
													(partner_node_name && *partner_node_name)){
		snprintf(_last_error_text,ERROR_TEXT_SIZE,
			"error in %s: OmHandler::getObject(...' failed to retrieve the object '%s': return code == %d, try to retrieve the obj on the partner node...",  FX_NAME, imm_dnName, imm_call_result);

		snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX,
			ACS_DSD_CONFIG_IMM_SERVICE_INFO_CLASS_NAME"=%s@%s,"ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s",
			service_name, domain, partner_node_name, imm_dsd_root_dn);

		_service_node_info = IMM_DH_PARTNER_NODE;
		// try to fetch the obj from IMM from  <SRTNode=partner_node_name>
		imm_object.objName = imm_dnName;

		imm_call_result = _om_handler.getObject(&imm_object);
	}

	if (imm_call_result == ACS_CC_SUCCESS){
		//  reset all attributes
		reset_attributes();
		unsigned attributes_num = imm_object.attributes.size();
		int attr_found=0;
		//Search for  attributes
		for (unsigned i = 0; i < attributes_num && attr_found < 6; ++i){
			const char * attr_name = imm_object.attributes[i].attrName.c_str();
			unsigned values_count = imm_object.attributes[i].attrValuesNum;

			if (!strcmp(attr_name,IMM_DH_SRVINFO_PROCNAME_ATTR_NAME)){
				char * value = reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && value) {
					strncpy(_attributes.proc_name,value,acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX);
					++attr_found;
				} else snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_serv_addresses_fromIMM: "IMM_DH_SRVINFO_PROCNAME_ATTR_NAME" attribute is empty");
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRVINFO_PID_ATTR_NAME)){
				int32_t * pid_ptr = reinterpret_cast<int32_t *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && pid_ptr) {
					_attributes.pid = *pid_ptr;
					++attr_found;
				} else snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_serv_addresses_fromIMM: "IMM_DH_SRVINFO_PID_ATTR_NAME" attribute is empty");
				continue;
			}
			if(!strcmp(attr_name,IMM_DH_SRVINFO_VISIB_ATTR_NAME)){
				uint32_t * vis_ptr = reinterpret_cast<uint32_t *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && vis_ptr) {
					set_visibility(*vis_ptr);
					attr_found++;
				} else {
					snprintf(_last_error_text,ERROR_TEXT_SIZE, "warning in %s: OmHandler::getObject(...' null value for attribute <%s>", FX_NAME, attr_name);
					return acs_dsd_imm::ERR_IMM_EMPTY_VALUESNUM_ATTR;
				}
			}
			bool addr_unix_type = false;
			bool addr_inet_type = strcmp(attr_name, IMM_DH_SRVINFO_INET_ADDR_ATTR_NAME)? false: true;
			if(!addr_inet_type) addr_unix_type = strcmp(attr_name, IMM_DH_SRVINFO_UNIX_ADDR_ATTR_NAME)? false :true;
			if(!strcmp(attr_name,IMM_DH_SRVINFO_CONNTYPE_ATTR_NAME)){
				uint32_t * conn_type_ptr = reinterpret_cast<uint32_t *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && conn_type_ptr) {
					set_conn_type(*conn_type_ptr);
					attr_found++;
				} else {
					snprintf(_last_error_text,ERROR_TEXT_SIZE, "warning in %s: OmHandler::getObject(...' null value for attribute <%s>", FX_NAME, attr_name);
					return acs_dsd_imm::ERR_IMM_EMPTY_VALUESNUM_ATTR;
				}
			}
			if (addr_inet_type || addr_unix_type) {
				char ** values = reinterpret_cast<char **>(imm_object.attributes[i].attrValues);
				if (values_count && values && *values) {
					attr_found++;
					if(addr_inet_type) set_tcp_addresses((const char **)values, values_count);
					if(addr_unix_type) set_unix_addresses((const char **)values, values_count);
				} else snprintf(_last_error_text,ERROR_TEXT_SIZE, "warning in %s: OmHandler::getObject(...' null value for attribute <%s>", FX_NAME, attr_name);
			}
		}  // end of for

		if (attr_found < 5){  // it should not occur!! at least one address type must be found (inet or unix)
			snprintf(_last_error_text,ERROR_TEXT_SIZE,
									"error in %s: OmHandler::getObject(...' failed to retrieve addresses attributes", FX_NAME);
			ret_code = acs_dsd_imm::ERR_IMM_OM_ATTR_NOT_FOUND;
		}

		// if "check_running" flag argument is 1, and the found service was registered on local AP node, we must check if it's still running
		if(check_running && (_service_node_info == IMM_DH_LOCAL_NODE) && !ACS_DSD_ConfigurationHelper::process_running(_attributes.proc_name, _attributes.pid)) {
			snprintf(_last_error_text,ERROR_TEXT_SIZE,
					"error in %s: the retrieved object '%s' is INVALID (service NOT running on local AP node)",  FX_NAME, imm_dnName);
			ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
		}
	}
	else{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,
			"error in %s: OmHandler::getObject(...' failed to retrieve the object '%s': return code == %d",  FX_NAME, imm_dnName, imm_call_result);
		ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
	}
	return ret_code;
}


int  __CLASS_NAME__ ::fetch_SRTNodes_list(std::vector<std::string> & node_name_list)
{
	const char * imm_dsd_root_dn = 0;
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	ACS_CC_ReturnType imm_call_result;
	int ret_code=acs_dsd_imm::NO_ERROR;
	if(!_omHandlerInitialized && (ret_code= omHandler_init())!= acs_dsd_imm::NO_ERROR){
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_SRTNodes_list() -ImmDHobj.init() failed: return code == %d", ret_code);
		return ret_code;
	}

	// try to fetch the SRTNode objects from IMM
	ACS_APGCC_ImmObject imm_object;
	if ((imm_call_result = _om_handler.getChildren(imm_dsd_root_dn, ACS_APGCC_SUBLEVEL, & node_name_list)) != ACS_CC_SUCCESS)
	{
		snprintf(_last_error_text,ERROR_TEXT_SIZE, "error in fetch_SRTNodes_list: om_handler.getChildren' failed: return code == %d", imm_call_result);
		ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
	}

	return ret_code;
}


int __CLASS_NAME__ ::removeNodeInfoByDN(const char *node_dn /*distinguished name*/)
{
	int ret_code=0;
	ACS_CC_ReturnType imm_call_result;

	if(!node_dn || !*node_dn)
	{
		snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in removeNodeInfoByDN(): invalid DN parameter");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	if ( !_omHandlerInitialized && (ret_code = omHandler_init()) != acs_dsd_imm::NO_ERROR )
	{
			snprintf(_last_error_text, ERROR_TEXT_SIZE,
					"fetch_SRTNodes_list() - omHandler_init() failed: return code == %d", ret_code);
			return ret_code;
	}

	// Get the list of SRVinfo objects that are children of the given node
	std::vector<std::string> services_dnList;
	if ((imm_call_result = _om_handler.getChildren(node_dn, ACS_APGCC_SUBLEVEL, & services_dnList)) == ACS_CC_SUCCESS)
	{
		// delete all service node info created under SrtNode
		for(unsigned int i=0; i<services_dnList.size(); i++){
			if ((imm_call_result = _ImmCHobj->deleteRuntimeObj(services_dnList[i].c_str()))!= ACS_CC_SUCCESS){
				int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
				snprintf(_last_error_text,ERROR_TEXT_SIZE,
									"error in removeNodeInfo: deleteRuntimeObj(%s)' failed: return code == %d. APGCC internal last error is '%d'",services_dnList[i].c_str(), imm_call_result, apgcc_internal_last_error);
				ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_DELETE;

				// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
				if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
					_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
			}
		}
	}
	if (ret_code != acs_dsd_imm::ERR_IMM_RUNTIME_DELETE){
		// Now delete SRTnode
		if ((imm_call_result = _ImmCHobj->deleteRuntimeObj(node_dn))!= ACS_CC_SUCCESS){
			int  apgcc_internal_last_error = _ImmCHobj->getInternalLastError();
			snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in removeNodeInfo: deleteRuntimeObj(%s)' failed: return code == %d. APGCC internal last error is '%d'", node_dn, imm_call_result, apgcc_internal_last_error);
			ret_code = acs_dsd_imm::ERR_IMM_RUNTIME_DELETE;

			// if a critical error has occurred, let's invalidate the ACS_DSD_ImmConnectionHandler object used to perform the operation
			if(ACS_DSD_ImmConnectionHandler::is_critical_error(apgcc_internal_last_error))
				_ImmCHobj->setImmConnState(ACS_DSD_ImmConnectionHandler::BAD);
		}
	}
	return ret_code;
}

int __CLASS_NAME__ ::fetch_NodeInfoByDN(const char * node_dn)
{
	int ret_code = acs_dsd_imm::NO_ERROR;

	if(!node_dn || !*node_dn)
	{
		snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in fetch_NodeInfoByDN(): invalid DN parameter");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	const char * imm_dsd_root_dn = 0;

	//Searching the parent name of the class to be searched
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn))
	{
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	if(!_omHandlerInitialized && (ret_code = omHandler_init())!= acs_dsd_imm::NO_ERROR)
	{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_NodeInfoByDN() - ImmDHobj.init() failed: return code == %d", ret_code);
		return ret_code;
	}

	// try to fetch the obj from IMM
	ACS_CC_ReturnType imm_call_result;
	ACS_APGCC_ImmObject imm_object;
	imm_object.objName =  node_dn;

	if ((imm_call_result = _om_handler.getObject(&imm_object)) == ACS_CC_SUCCESS)
	{
		unsigned attributes_num = imm_object.attributes.size();
		//  reset all attributes
		reset_attributes();

		//Search for  attributes
		for (unsigned i = 0; i < attributes_num; ++i)
		{
			const char * attr_name = imm_object.attributes[i].attrName.c_str();
			unsigned values_count = imm_object.attributes[i].attrValuesNum;

			if (!strcmp(attr_name,IMM_DH_SRTNODE_NODE_ATTR_NAME)){
				if (values_count && (reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0])))
					set_node(reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0]));
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRTNODE_SIDE_ATTR_NAME)){
				imm_side_t * side_ptr = reinterpret_cast<imm_side_t *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && side_ptr) set_side(*side_ptr);
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRTNODE_STATE_ATTR_NAME)){
				imm_state_t * state_ptr = reinterpret_cast<imm_state_t *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && state_ptr) set_state(*state_ptr);
				continue;
			}
		}
	}
	else
	{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"OmHandler::getObject(...' failed to retrieve the object '%s': return code == %d", node_dn, imm_call_result);
		ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
	}

	return ret_code;
}


int __CLASS_NAME__ ::fetch_NodeInfo(const char * node_name)
{
	int ret_code = acs_dsd_imm::NO_ERROR;

	if(!node_name || !*node_name)
	{
		snprintf(_last_error_text, ERROR_TEXT_SIZE, "error in fetch_NodeInfo(): invalid node_name parameter");
		return acs_dsd_imm::ERR_INPUT_PARAMETER_EMPTY;
	}

	const char * imm_dsd_root_dn = 0;

	//Searching the parent name of the class to be searched
	if (int call_result = ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn))
	{
		snprintf(_last_error_text, ACS_DSD_ARRAY_SIZE(_last_error_text),
			"Call to 'ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(...' failed: while searching for DSD root parameter subtree in IMM: return code == %d",
			call_result);
		return acs_dsd_imm::ERR_IMM_OM_GET;
	}

	if(!_omHandlerInitialized && (ret_code = omHandler_init())!= acs_dsd_imm::NO_ERROR)
	{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"fetch_NodeInfo() - ImmDHobj.init() failed: return code == %d", ret_code);
		return ret_code;
	}

	char imm_dnName[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX]={0};

	//Make up the dn to reach the SRTNode instance object
	snprintf(imm_dnName, acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX, ACS_DSD_CONFIG_IMM_REGISTRATION_TABLE_CLASS_NAME"=%s,%s", node_name, imm_dsd_root_dn);

	// try to fetch the instance object from IMM
	ACS_CC_ReturnType imm_call_result;
	ACS_APGCC_ImmObject imm_object;
	imm_object.objName = imm_dnName;

	if ((imm_call_result = _om_handler.getObject(&imm_object)) == ACS_CC_SUCCESS)
	{
		unsigned attributes_num = imm_object.attributes.size();
		//  reset all attributes
		reset_attributes();

		//Search for  attributes
		for (unsigned i = 0; i < attributes_num; ++i)
		{
			const char * attr_name = imm_object.attributes[i].attrName.c_str();
			unsigned values_count = imm_object.attributes[i].attrValuesNum;

			if (!strcmp(attr_name,IMM_DH_SRTNODE_NODE_ATTR_NAME)){
				if (values_count && (reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0])))
					set_node(reinterpret_cast<char *>(imm_object.attributes[i].attrValues[0]));
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRTNODE_SIDE_ATTR_NAME)){
				imm_side_t * side_ptr = reinterpret_cast<imm_side_t *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && side_ptr) set_side(*side_ptr);
				continue;
			}
			if (!strcmp(attr_name,IMM_DH_SRTNODE_STATE_ATTR_NAME)){
				imm_state_t * state_ptr = reinterpret_cast<imm_state_t *>(imm_object.attributes[i].attrValues[0]);
				if (values_count && state_ptr) set_state(*state_ptr);
				continue;
			}
		}
	}
	else
	{
		snprintf(_last_error_text,ERROR_TEXT_SIZE,"OmHandler::getObject(...' failed to retrieve the object '%s': return code == %d", imm_dnName, imm_call_result);
		ret_code = acs_dsd_imm::ERR_IMM_OM_GET;
	}

	return ret_code;
}
