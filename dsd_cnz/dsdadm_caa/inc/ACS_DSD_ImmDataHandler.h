#ifndef HEADER_GUARD_CLASS__ACS_DSD_ImmDataHandler 
#define HEADER_GUARD_CLASS__ACS_DSD_ImmDataHandler ACS_DSD_ImmDataHandler

/** @class ACS_DSD_ImmDataHandler ACS_DSD_ImmDataHandler.h
 *	@brief ACS_DSD_ImmDataHandler class
 *	@author 
 *	@date 
 *	@version 
 *
 *	ACS_DSD_ImmDataHandler Class detailed description
 */
#include "ace/INET_Addr.h"
#include "ace/UNIX_Addr.h"
#include "ACS_DSD_MacrosConstants.h"
#include "ACS_DSD_MacrosConfig.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_DSD_ImmConnectionHandler.h"

// defines used to indicate where the service is registered (local node or partner node)
#define IMM_DH_LOCAL_NODE 0
#define IMM_DH_PARTNER_NODE 1

// defines for visibility field
#define IMM_DH_GLOBAL_SCOPE 0
#define IMM_DH_LOCAL_SCOPE  1

#define IMM_DH_ATTR_NAME_MAX_SIZE   128
#define IMM_DH_SRT_CLASS_ATTR_MAXNUM 4	// max num of attribute for IMM SRT class
#define IMM_DH_SRVINFO_CLASS_ATTR_MAXNUM 9  // max num of attribute for IMM Service Info class
#define IMM_DH_ATTR_MAXNUM IMM_DH_SRT_CLASS_ATTR_MAXNUM + IMM_DH_SRVINFO_CLASS_ATTR_MAXNUM
#define IMM_DH_SRVINFO_ATTRVALUE_MAXNUM IMM_DH_SRVINFO_CLASS_ATTR_MAXNUM*2   //there  may be attributes with multi value
#define IMM_DH_SRTNODE_ATTRVALUE_MAXNUM IMM_DH_SRT_CLASS_ATTR_MAXNUM*2


#define SRVINFO_RDN_ATTR_TYPE   	ATTR_STRINGT
#define SRVINFO_NAME_ATTR_TYPE   	ATTR_STRINGT
#define SRVINFO_DOMAIN_ATTR_TYPE	ATTR_STRINGT
#define SRVINFO_UNIXADDR_ATTR_TYPE	ATTR_STRINGT
#define SRVINFO_INETADDR_ATTR_TYPE  ATTR_STRINGT
#define SRVINFO_PROCNAME_ATTR_TYPE  ATTR_STRINGT
#define SRVINFO_CONN_ATTR_TYPE 		ATTR_UINT32T
#define SRVINFO_PID_ATTR_TYPE       ATTR_INT32T
#define SRVINFO_VISIB_ATTR_TYPE     ATTR_INT32T

#define SRTNODE_RDN_ATTR_TYPE  		ATTR_STRINGT
#define SRTNODE_NODE_ATTR_TYPE  	ATTR_STRINGT
#define SRTNODE_STATE_ATTR_TYPE 	ATTR_UINT32T
#define SRTNODE_SIDE_ATTR_TYPE      ATTR_INT32T


// typedef for type attributes in IMM
typedef  uint32_t    imm_state_t;
typedef  int32_t     imm_side_t;
typedef  uint32_t    imm_conn_type_t;
typedef  int32_t     imm_pid_t ;
typedef  int32_t     imm_visib_t;



#ifndef IMM_DH_SRTNODE_RDN_ATTR_NAME
#define IMM_DH_SRTNODE_RDN_ATTR_NAME "srtNodeId"
#endif
#ifndef IMM_DH_SRTNODE_STATE_ATTR_NAME
#define IMM_DH_SRTNODE_STATE_ATTR_NAME ACS_DSD_CONFIG_IMM_STATE_ATTR_NAME
#endif
#ifndef IMM_DH_SRTNODE_SIDE_ATTR_NAME
#define IMM_DH_SRTNODE_SIDE_ATTR_NAME "side"
#endif
#ifndef IMM_DH_SRTNODE_NODE_ATTR_NAME
#define IMM_DH_SRTNODE_NODE_ATTR_NAME "node"
#endif
#ifndef IMM_DH_SRVINFO_RDN_ATTR_NAME
#define IMM_DH_SRVINFO_RDN_ATTR_NAME "srvInfoId"
#endif
#ifndef IMM_DH_SRVINFO_NAME_ATTR_NAME
#define IMM_DH_SRVINFO_NAME_ATTR_NAME "name"
#endif
#ifndef IMM_DH_SRVINFO_DOMAIN_ATTR_NAME
#define IMM_DH_SRVINFO_DOMAIN_ATTR_NAME "domain"
#endif
#ifndef IMM_DH_SRVINFO_CONNTYPE_ATTR_NAME
#define IMM_DH_SRVINFO_CONNTYPE_ATTR_NAME "conn_type"
#endif
#ifndef IMM_DH_SRVINFO_PROCNAME_ATTR_NAME
#define IMM_DH_SRVINFO_PROCNAME_ATTR_NAME "proc_name"
#endif
#ifndef IMM_DH_SRVINFO_PID_ATTR_NAME
#define IMM_DH_SRVINFO_PID_ATTR_NAME "pid"
#endif
#ifndef IMM_DH_SRVINFO_VISIB_ATTR_NAME
#define IMM_DH_SRVINFO_VISIB_ATTR_NAME "visibility"
#endif
#ifndef IMM_DH_SRVINFO_INET_ADDR_ATTR_NAME
#define IMM_DH_SRVINFO_INET_ADDR_ATTR_NAME ACS_DSD_CONFIG_IMM_INET_ADDRESSES_ATTR_NAME
#endif
#ifndef IMM_DH_SRVINFO_UNIX_ADDR_ATTR_NAME
#define IMM_DH_SRVINFO_UNIX_ADDR_ATTR_NAME ACS_DSD_CONFIG_IMM_UNIX_ADDRESSES_ATTR_NAME
#endif


#define ACS_DSD_PROCESS_STATUSFILE_PATH_PREFIX "/proc/"
#define ACS_DSD_PROCESS_STATUS_FILE_NAME "status"

typedef struct ACS_DSD_ImmAttributes{
	  char attr_name[IMM_DH_ATTR_NAME_MAX_SIZE];
	  uint16_t attr_type;
	  uint16_t attr_num;

}IMM_DH_Attrubutes;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_ImmDataHandler

#define ERROR_TEXT_SIZE 512

/// Return codes used by process() method
namespace acs_dsd_imm {
	enum errorCodes
	{
		NO_ERROR 					   = 0,
		ERR_IMM_FAILURE  			   = -1,
		ERR_INPUT_PARAMETER_EMPTY      = -2,
		ERR_IMM_OM_INIT	     		   = -3,		//
		ERR_IMM_OM_GET	     		   = -4,		//
		ERR_IMM_OM_ATTR_NOT_FOUND  	   = -5,
		ERR_IMM_RUNTIME_CREATE         = -6,
		ERR_IMM_RUNTIME_DELETE         = -7,	   //
		ERR_IMM_RUNTIME_MODIFY         = -8,
		ERR_IMM_EMPTY_VALUESNUM_ATTR   = -9,
		ERR_INPUT_PARAMETER_INVALID    = -10,
		ERR_GENERIC_FAILURE            = -11,
		ERR_IMM_GET_CHILDREN					 = -12
	};
	enum DSD_IMM_Srt_Attribute {
		SRT_rdn_attr   = 0,   		//  This must be the first value
		SRT_node_attr  = 1,
		SRT_side_attr  = 2,
		SRT_state_attr = 3   		// This must be the last value
	};
	enum DSD_IMM_Srv_Attribute {
		SRV_rdn_attr= 0,			// This must be the first enum value
		SRV_name_attr = 1,
		SRV_domain_attr = 2,
		SRV_conntype_attr = 3,
		SRV_procname_attr= 4,
		SRV_pid_attr=  5,
		SRV_visibility_attr = 6,
		SRV_inetaddr_attr = 7,
		SRV_unixaddr_attr = 8		 // This must be the last enum value
	};
}
struct DSD_IMM_attributes {
	char SRTNode_rdn[ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX];
	char SRVInfo_rdn[ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX];
	char node[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX];
	char tcp_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_NETWORK_NAME_SIZE_MAX+8];
	char unix_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX];
	char proc_name[acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX];
	char name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX];
	char domain[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX];
	imm_state_t state;
	imm_side_t  side;
	imm_conn_type_t conn_type;
	imm_pid_t pid;
	imm_visib_t visibility;
	uint8_t SRVInfo_attr_flag[IMM_DH_SRVINFO_CLASS_ATTR_MAXNUM];
	uint8_t SRTNode_attr_flag[IMM_DH_SRT_CLASS_ATTR_MAXNUM];
	uint16_t tcp_addr_num;
	uint16_t unix_addr_num;
};

class __CLASS_NAME__ {

public:
	//  constructor
	__CLASS_NAME__(ACS_DSD_ImmConnectionHandler *ImmConnHandler);
	inline ~__CLASS_NAME__(){if(_omHandlerInitialized) {
								_om_handler.Finalize();_omHandlerInitialized=false;}}

	/* SET METHODS */
	inline void set_node (const char *value){strncpy(_attributes.node,value,acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX);
	                                                                        _attributes.SRTNode_attr_flag[acs_dsd_imm::SRT_node_attr]=1;}
	inline void set_proc_name (const char *value){strncpy(_attributes.proc_name, value,acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX);
																					_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_procname_attr]=1;}
	inline void set_side (int16_t side){_attributes.side=(imm_side_t)side; _attributes.SRTNode_attr_flag[acs_dsd_imm::SRT_side_attr]=1;}
	inline void set_conn_type (uint16_t conn_type){_attributes.conn_type= (imm_conn_type_t)conn_type; _attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_conntype_attr]=1;}
	inline void set_pid (uint32_t pid){_attributes.pid=(imm_pid_t)pid; _attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_pid_attr]=1;}
	inline void set_state (uint16_t state){_attributes.state=(imm_state_t)state; _attributes.SRTNode_attr_flag[acs_dsd_imm::SRT_state_attr]=1;}
	inline void set_serviceName (const char *value){strncpy(_attributes.name,value,acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);
																						_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_name_attr]=1;}
	inline void set_domainName (const char *value){strncpy(_attributes.domain,value,acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);
																						_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_domain_attr]=1;}
	inline void set_SRVInfo_rdn (const char *value){strncpy(_attributes.SRVInfo_rdn,value,ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX);
																						_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_rdn_attr]=1;}
	inline void set_SRTNode_rdn (const char *value){strncpy(_attributes.SRTNode_rdn,value,ACS_DSD_CONFIG_IMM_RDN_SIZE_MAX);
																						_attributes.SRTNode_attr_flag[acs_dsd_imm::SRT_rdn_attr]=1;}

	inline void set_visibility(uint8_t visib){ _attributes.visibility = (imm_visib_t)visib;
															_attributes.SRVInfo_attr_flag[acs_dsd_imm::SRV_visibility_attr]=1;}
	/* GET METHODS  */
	inline void get_state (uint16_t &attr) const {attr = _attributes.state;}
	inline void get_node (char *attr)const{strncpy(attr,_attributes.node,acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX);}
	inline void get_proc_name (char *attr) const {strncpy(attr,_attributes.proc_name,acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX);}
	inline void get_side (int16_t &attr)const {attr =_attributes.side;}
	inline void get_conn_type (uint16_t &attr)const {attr=_attributes.conn_type;}
	inline void get_pid (int32_t &attr)const {attr=_attributes.pid;}
	inline void get_serviceName (char *attr)const {strncpy(attr,_attributes.name,acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);}
	inline void get_domainName (char *attr){strncpy(attr,_attributes.domain,acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);}
	void get_unix_addresses (char (*value)[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX], uint16_t &attr_num)const;
	void get_tcp_addresses (char *value[acs_dsd::CONFIG_NETWORKS_SUPPORTED], uint16_t &attr_num)const;
	void get_inet_addresses (ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED], uint16_t &attr_num)const;
	inline uint8_t get_service_node_info()const {return _service_node_info;}
	inline int32_t get_visibility() const {return _attributes.visibility;}
	inline const char *last_error_text () const{ return _last_error_text; }

	void set_unix_addresses (const char **value, uint16_t attr_num);
	void set_tcp_addresses (const char **value, uint16_t attr_num);
	int addNodeInfo(const char *nodeName);
	int addServiceInfo(const char *nodeName,const char *serviceName,const char *domain);
	int removeNodeInfo(const char *nodeName);
	int removeNodeInfoByDN(const char * node_dn /*distinguished name*/);
	int deleteServiceInfo(const char *nodeName, const char *serviceName,const char *domain);
	int deleteServiceInfo(const char *dn /*distinguished name*/);
	int deleteAllServicesInfo(const char *node_name);
	int modifyServiceInfo(const char *nodeName, const char *serviceName, const char *domain);
	int modifyNodeInfo(const char *nodeName, uint16_t n_state);
	int fetch_ServiceInfo(const char *nodeName, const char *serviceName, const char*domain, int check_running = 0);
	int fetch_serv_addresses_fromIMM(const char *service_name, const char *service_domain, const char *local_node_name,const char *partner_node_name, int check_running = 0);
	inline int fetch_serv_addresses_fromIMM(const char *service_name, const char *service_domain, const char *local_node_name, int check_running = 0){
				return fetch_serv_addresses_fromIMM(service_name, service_domain, local_node_name, "", check_running);}
	int fetchRegisteredServicesList(const char *nodeName, std::vector<std::string> &reg_serviceList);
	int fetch_serviceProcName_ProcId(const char *dn_service);

	/**
	 *  Returns a collection. Each item in the collection represents the DN of a single SRTNode object present under
	 *  the root object of the IMM DSD model.
	 */
	int fetch_SRTNodes_list(std::vector<std::string> & node_dn_list);

	// If the "fetch_serv_addresses_from_IMM()" method has been invoked to retrieve from IMM the info about a service,
	// it represents the location of the service found (local node or partner node)
	int service_location() { return _service_node_info; };

	// Get info about the specified SRTNode (storing them in the internal attributes)
	int fetch_NodeInfoByDN(const char * node_dn);
	int fetch_NodeInfo(const char * node_name);

	// public data
	vector<ACS_CC_ValuesDefinitionType> _attrList;

private:
	OmHandler _om_handler;
	struct DSD_IMM_attributes _attributes;
	ACS_DSD_ImmConnectionHandler *_ImmCHobj;

	bool _omHandlerInitialized;
	char _last_error_text[ERROR_TEXT_SIZE];
	uint8_t _service_node_info;  // info about the node (local or partner) where the service info were retrieved
	int omHandler_init();
	int modifyAttribute(const char *imm_dnName, acs_dsd_imm::DSD_IMM_Srv_Attribute attrib);
	int modifyAttribute(const char *imm_dnName,acs_dsd_imm::DSD_IMM_Srt_Attribute attrib);
	int setServiceInfoAttribute(acs_dsd_imm::DSD_IMM_Srv_Attribute attrib, void **pointer);
	void setServiceTableAttribute(acs_dsd_imm::DSD_IMM_Srt_Attribute attrib, void **valueAttr);
	void reset_attributes(void);
};


#endif /* HEADER_GUARD_CLASS__ACS_DSD_ImmDataHandler */
