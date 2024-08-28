#include "ACS_DSD_CpProt_ListApplPrimitives.h"
#include "ACS_DSD_ServiceHandler.h"

/* CpProt_listAppl_v1_request class methods */

CpProt_listAppl_v1_request::CpProt_listAppl_v1_request()
{
	_protocol_id = PROTOCOL_CPAP;
	_protocol_version = CPAP_PROTOCOL_VERSION_1;
	_primitive_id = CPAP_LISTAPP_REQUEST;
	memset(_NodeName,'\0',sizeof(_NodeName));
}

int CpProt_listAppl_v1_request::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const
{
	return  pdh.make_primitive(CPAP_LISTAPP_REQUEST, CPAP_PROTOCOL_VERSION_1, _NodeName);
}

int CpProt_listAppl_v1_request::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, _NodeName);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, _NodeName);
}

int CpProt_listAppl_v1_request::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
{
	// this primitive can't arrive from CP, because is used only in the direction DSD Daemon --> CP
/*
	// TEST CODE -- Used to simulate a response from a CP
	// send Response 1
	char serv_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX];
	char serv_domain[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX];
	uint8_t error = 0;
	acs_dsd::ConnectionTypeConstants conn_type = acs_dsd::CONNECTION_TYPE_OCP;
	uint16_t nrOfConn;
	uint32_t ip1;
	uint16_t port1;
	uint32_t ip2;
	uint16_t port2;
	memset(serv_name, 0, sizeof(serv_name));
	strcpy(serv_name, "sappo");
	memset(serv_domain, 0, sizeof(serv_name));
	strcpy(serv_domain, "its");
	nrOfConn = 2;
	ACE_INET_Addr ip_addr1(40001, "172.16.67.231");
	ACE_INET_Addr ip_addr2(40002, "172.16.67.231");
	ip1 = ip_addr1.get_ip_address();
	port1 = ip_addr1.get_port_number();
	ip2 = ip_addr2.get_ip_address();
	port2 = ip_addr2.get_port_number();

	CpProt_listAppl_v1_reply *pReply_1 = new CpProt_listAppl_v1_reply();
	pReply_1->_Error = 0;
	strcpy((char *) pReply_1->_ApplService, serv_name);
	strcpy((char *) pReply_1->_Domain, serv_domain);
	pReply_1->_ConnType = acs_dsd::CONNECTION_TYPE_OCP;
	pReply_1->_NrOfConn = 2;
	pReply_1->_ip1 = ip1;
	pReply_1->_port1 = port1;
	pReply_1->_ip2 = ip2;
	pReply_1->_port2 = port2;

	// send the primitive to the receiver
	int op_res = service_handler->sendPrimitive(pReply_1);
	if(op_res < 0)
	{
		printf(" >>> Error sending listAppl Reply 1. Errocode = < %d > ! \n", op_res);
		return -1;
	}
	printf("  >>>  listApply Reply 1 correctly sent. \n");
	delete pReply_1;


	// send Response 2
	memset(serv_name, 0, sizeof(serv_name));
	memset(serv_domain, 0, sizeof(serv_name));
	strcpy(serv_name, "pluto");
	strcpy(serv_domain, "its");
	nrOfConn = 1;
	ACE_INET_Addr ip_addr3(50001,"172.16.67.232");
	ip1 = ip_addr3.get_ip_address();
	port1 = ip_addr3.get_port_number();
	ip2 = 0;
	port2 = 0;

	CpProt_listAppl_v1_reply *pReply_2 = new CpProt_listAppl_v1_reply();
	pReply_2->_Error = 0;
	strcpy((char *) pReply_2->_ApplService, serv_name);
	strcpy((char *) pReply_2->_Domain, serv_domain);
	pReply_2->_ConnType = acs_dsd::CONNECTION_TYPE_OCP;
	pReply_2->_NrOfConn = 1;
	pReply_2->_ip1 = ip1;
	pReply_2->_port1 = port1;
	pReply_2->_ip2 = ip2;
	pReply_2->_port2 = port2;

	// send the primitive to the receiver
	op_res = service_handler->sendPrimitive(pReply_2);
	if(op_res < 0)
	{
		printf(" >>> Error sending listAppl Reply 2. Errocode = < %d > ! \n", op_res);
		return -1;
	}
	printf("  >>>  listApply Reply 2 correctly sent. \n");
	delete pReply_2;


	// send final Response
	memset(serv_name, 0, sizeof(serv_name));
	memset(serv_domain, 0, sizeof(serv_name));
	strcpy(serv_name, "");
	strcpy(serv_domain, "");
	nrOfConn = 1;
	ip1 = 0;
	port1 = 0;
	ip2 = 0;
	port2 = 0;

	CpProt_listAppl_v1_reply *pReply_3 = new CpProt_listAppl_v1_reply();
	pReply_3->_Error = 0;
	strcpy((char *) pReply_3->_ApplService, serv_name);
	strcpy((char *) pReply_3->_Domain, serv_domain);
	pReply_3->_ConnType = acs_dsd::CONNECTION_TYPE_OCP;
	pReply_3->_NrOfConn = 1;
	pReply_3->_ip1 = ip1;
	pReply_3->_port1 = port1;
	pReply_3->_ip2 = ip2;
	pReply_3->_port2 = port2;

	// send the primitive to the receiver
	op_res = service_handler->sendPrimitive(pReply_3);
	if(op_res < 0)
	{
		printf(" >>> Error sending listAppl Reply 3. Errocode = < %d > ! \n", op_res);
		return -1;
	}
	printf("  >>>  listApply Reply 3 correctly sent. \n");
	delete pReply_3;
*/

	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}

std::string CpProt_listAppl_v1_request::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"NodeName = %s",
				_primitive_id,
				_protocol_version,
				_NodeName);

	return mess;
}

int CpProt_listAppl_v1_request::get_node_name(char *buff, size_t buffsize)
{
	return ACS_DSD_Utils::get_primitive_string_field(_NodeName, buff, buffsize);
}

int CpProt_listAppl_v1_request::set_node_name(const char *str)
{
	return ACS_DSD_Utils::set_primitive_string_field(_NodeName, str, strlen(str));
}



/* CpProt_listAppl_v1_reply class methods */

CpProt_listAppl_v1_reply::CpProt_listAppl_v1_reply()
{
	_protocol_id = PROTOCOL_CPAP;
	_protocol_version = CPAP_PROTOCOL_VERSION_1;
	_primitive_id = CPAP_LISTAPP_REPLY;
	_Error = 0;
	memset(_Domain,'\0',sizeof(_Domain));
	memset(_ApplService,'\0',sizeof(_ApplService));
	_ConnType = acs_dsd::CONNECTION_TYPE_UNKNOWN;
	_NrOfConn = 0;
	_ip1 = 0;
	_port1 = 0;
	_ip2 = 0;
	_port2 = 0;
}

std::string CpProt_listAppl_v1_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"ERROR = %u,"
				"DOMAIN = %s,"
				"APPL_SERVICE = %s,"
				"CONN_TYPE = %02u,"
				"NR_OF_CONN = %u,"
				"IP1 = %u,"
				"PORT1 = %u,"
				"IP2 = %u,"
				"PORT2 = %u",
				_primitive_id,
				_protocol_version,
				_Error,
				_Domain,
				_ApplService,
				_ConnType,
				_NrOfConn,
				_ip1,
				_port1,
				_ip2,
				_port2);

	return mess;
}

int CpProt_listAppl_v1_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<>  & pdh) const
{
	return  pdh.make_primitive(CPAP_LISTAPP_REPLY, CPAP_PROTOCOL_VERSION_1, _Error, _Domain, _ApplService, _ConnType, _NrOfConn, _ip1, _port1, _ip2, _port2);
}

int  CpProt_listAppl_v1_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, & _Error, _Domain, _ApplService, & _ConnType, & _NrOfConn, & _ip1, & _port1, & _ip2, & _port2);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, & _Error, _Domain, _ApplService, & _ConnType, & _NrOfConn, & _ip1, & _port1, & _ip2, & _port2);
}

int CpProt_listAppl_v1_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}

