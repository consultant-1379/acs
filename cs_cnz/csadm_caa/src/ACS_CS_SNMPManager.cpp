/*
 * ACS_CS_SNMPManager.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: teiscud
 */

#include "ACS_CS_SNMPManager.h"

std::string oidShelfMgrCtrlSCX =            "1.3.6.1.4.1.193.177.2.2.1.2.1.8";
std::string oidShelfMgrCtrlSCB = 			"1.3.6.1.4.1.193.154.2.1.2.1.1.1.12";

ACS_CS_SNMPManager::ACS_CS_SNMPManager(std::string ipAddress){

	bExit = false;
	bRun = false;
	//trapReceiver = NULL;

	ipAddr = ipAddress;

	std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << std::endl;

	//init_snmp("SnmpManager");
	//if (initialize()) {
	//	std::cout << __FUNCTION__ << "@" << __LINE__ << " Init Failed, thread not started" << std::endl;
	//}
}

ACS_CS_SNMPManager::~ACS_CS_SNMPManager(){

	std::cout << __FUNCTION__ << "@" << __LINE__ << " destroyer Begin " << std::endl;
}

int ACS_CS_SNMPManager::snmpGet(const char* community, const char* peername,const char* usrOID, int type, void* out, unsigned int iBufSize, int strType)
{
	int res;
	snmp_session snmpSession;
	//snmp_session *snmpSessionHandler;
	void * snmpSess;
	char *pchBuffer;
	int *piBuffer;
	int iReqSize;
	int getResult = GETSET_SNMP_OK;
	struct snmp_pdu *snmpPDU;
	struct snmp_pdu *snmpResp;

	struct variable_list *varsList;

	pchBuffer = NULL;
	piBuffer = NULL;
	iReqSize = 0;
	snmp_sess_init( &snmpSession );

//	std::cout << __FUNCTION__ << "@" << __LINE__ <<	" init snmp session ok ... IP: " << peername <<std::endl;

	snmpSession.version = SNMP_VERSION_2c;
	snmpSession.community = (u_char*) strdup(community);
	snmpSession.retries = 3;
	//snmpSession.timeout = 1000000;
	snmpSession.timeout = 200000;
	snmpSession.community_len = strlen((const char *)snmpSession.community);
	snmpSession.peername = strdup(peername);

//	std::cout << __FUNCTION__ << "@" << __LINE__ <<	" init snmp session ok ... IP: " << snmpSession.peername <<std::endl;

	snmpSess = snmp_sess_open(&snmpSession);

	if(snmpSess == NULL)
	{
		std::cout << "DBG: " <<__FUNCTION__ << "@" << __LINE__ <<" ERROR: snmp_open failed - snmp_errno: " << snmp_errno << std::endl;

		free(snmpSession.community);
		free(snmpSession.peername);
		return OPEN_SESS_ERROR;
	}

	//snmpSessionHandler=snmp_sess_session(snmpSess);

	oid snmp_oid[MAX_OID_LEN];
	size_t snmp_oid_len = MAX_OID_LEN;

	if (! read_objid(usrOID, snmp_oid, &snmp_oid_len) )
	{
		std::cout <<"DBG: "<< __FUNCTION__ << "@" << __LINE__ <<" ERROR: Transform OID Failed " <<std::endl;
		snmp_sess_close(snmpSess);
		free(snmpSession.community);
		free(snmpSession.peername);
		return GETSET_SNMP_PARSE_OID_ERROR;
	}

	if ((snmpPDU = snmp_pdu_create(SNMP_MSG_GET)) != NULL)
	{
		snmp_add_null_var(snmpPDU, snmp_oid, snmp_oid_len);
		std::cout <<"DBG: "<< __FUNCTION__ << "@" << __LINE__ <<" Send request ! " <<std::endl;

		usleep(100000);
		res = snmp_sess_synch_response(snmpSess, snmpPDU, &snmpResp);

		switch(res)
		{
			case STAT_SUCCESS:
				std::cout <<"DBG: "<< __FUNCTION__ << "@" << __LINE__ <<" STAT_SUCCESS " << std::endl;
				if((out != NULL) && (iBufSize)){
					if (snmpResp->errstat == SNMP_ERR_NOERROR)
					{
						for (varsList = snmpResp->variables ; varsList ; varsList = varsList->next_variable)
						{
	//						std::cout << __FUNCTION__ << "@" << __LINE__ <<" for .... var len: " << varsList->val_len << std::endl <<" " << varsList->name_loc << std::endl;
	//						int z = varsList->type;
	//						std::cout << __FUNCTION__ << "@" << __LINE__ <<
	//							" type: " << z << std::endl;
							if (varsList->val_len == 0)
							{
								//out = NULL;
								getResult = GETSET_SNMP_OK_LEN_ZERO;
								break;
							}

							// return response as string
							if ((varsList->type == type) && (type == ASN_OCTET_STR))
							{
	//							print_variable(varsList->name, varsList->name_length, varsList);

								switch (strType)
								{

									case ACS_CS_SNMPManager::SNMPGET_STD_STRING:
									{
										//char  Buf[SNMP_MAXBUF];
										pchBuffer = reinterpret_cast<char*>(out);
										//sprintf(Buf, "%s", varsList->val.string);
										iReqSize = snprintf(pchBuffer, iBufSize, "%s", varsList->val.string);
										// Check len of required buffer
										if((unsigned int)iReqSize >= iBufSize){
											// Error. Buffer is too small
											std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: SNMPGET_STD_STRING buffer is too small! " << std::endl;
											getResult = GETSET_SNMP_BUFFERSMALL;
										}
										//out = (void *) Buf;
	//									std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMPGET_STD_STRING " << Buf << std::endl;

										//std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMPGET_STD_STRING " << out << std::endl;
										// //out = (void *) varsList->val.string;
										//out = &varsList->val.string;
										//std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMPGET_STD_STRING " << out << std::endl;
										break;
									}

									case ACS_CS_SNMPManager::SNMPGET_HEX_STRING:
									{
										//char  hexBuf[SNMP_MAXBUF];
										pchBuffer = reinterpret_cast<char*>(out);
										iReqSize = snprint_hexstring( pchBuffer, iBufSize, varsList->val.string, varsList->val_len);
										// Check error
										if(iReqSize < 0){
											// Error. Is the Buffer too small?
											std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: SNMPGET_HEX_STRING can't write in the buffer. Is buffer too small?" << std::endl;
											getResult = GETSET_SNMP_BUFFERSMALL;
										}
										//out = (void *) hexBuf;
	//									std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMPGET_HEX_STRING " << hexBuf << std::endl;
										break;
									}

									case ACS_CS_SNMPManager::SNMPGET_ASCII_STRING:
									{
										pchBuffer = reinterpret_cast<char*>(out);
										iReqSize = snprint_asciistring( pchBuffer, iBufSize, varsList->val.string, varsList->val_len);
										// Check error
										if(iReqSize < 0){
											// Error. Is the Buffer too small?
											std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: SNMPGET_ASCII_STRING can't write in the buffer. Is buffer too small?" << std::endl;
											getResult = GETSET_SNMP_BUFFERSMALL;
										}
	//									std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMPGET_ASCII_STRING " << hexBuf << std::endl;
										break;
									}
								}
								/*
								if (! isHexStr) {
									std::cout << __FUNCTION__ << "@" << __LINE__ << " HEX FALSE " << out << std::endl;
									//out = (void *) varsList->val.string;
									out = &varsList->val.string;
								}
								else {
									std::cout << __FUNCTION__ << "@" << __LINE__ << " HEX TRUE " << out << std::endl;
									char  hexBuf[SNMP_MAXBUF];
									//snprint_hexstring( hexBuf, SNMP_MAXBUF, varsList->val.string, varsList->val_len);

									snprint_asciistring( hexBuf, SNMP_MAXBUF, varsList->val.string, varsList->val_len);
									out = (void *) hexBuf;
								}
								*/

								break;
							}
							else if ((varsList->type == type) && (type == ASN_INTEGER))
							{

								/*
								int i = *varsList->val.integer;

								std::cout << __FUNCTION__ << "@" << __LINE__ <<
										" Val intero !: " << i << std::endl;

								out = (void *) i;
								*/
								if(iBufSize >= sizeof(int)){
									piBuffer = reinterpret_cast<int*>(out);
									*piBuffer = (int)(*(varsList->val.integer));
								}else{
									// Error. Is the Buffer too small?
									std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: ASN_INTEGER buffer is too small!" << std::endl;
									getResult = GETSET_SNMP_BUFFERSMALL;
								}
								//out = (int *) *varsList->val.integer;
								break;
							}
							else
							{
								//todo
							}

						} //for (varsList = snmpResp->variables ; varsList ; varsList = varsList->next_variable)

						//snmp_free_pdu(snmpResp);
					}
					else //if (snmpResp->errstat == SNMP_ERR_NOERROR)
					{
						std::cout << __FUNCTION__ << "@" << __LINE__ << " ERROR: GET-Response Error: "
								<< snmp_errstring(snmpResp->errstat) << std::endl;
						//snmp_free_pdu(snmpResp);
						//out = NULL;
						getResult =  GETSET_SNMP_RESPONSE_ERROR;
					}
			}else{	// if((out != NULL) && (iBufSize))
				// Error buffer
				std::cout << __FUNCTION__ << "@" << __LINE__ <<" ## ERROR: incorrect argument [buffer].  " << std::endl;
				getResult =  GETSET_SNMP_RESPONSE_ERROR;
			}
				break; //EXIT FOR !

			case STAT_TIMEOUT:
				std::cout << __FUNCTION__ << "@" << __LINE__ <<" STAT_TIMEOUT " << std::endl;
				std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: "<< snmp_errstring(snmp_errno) << std::endl;

				//snmp_free_pdu(snmpResp);
				//out = NULL;
				getResult = GETSET_SNMP_STATUS_TIMEOUT;
				break;

			case STAT_ERROR:
				std::cout << __FUNCTION__ << "@" << __LINE__ <<	" STAT_ERROR " << std::endl;

				std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: "	<< snmp_errstring(snmp_errno) << std::endl;

				//snmp_free_pdu(snmpResp);
				//out = NULL;
				getResult = GETSET_SNMP_STATUS_ERROR;
				break;
		}


	}
	else //if ((snmpPDU = snmp_pdu_create(SNMP_MSG_GET)) != NULL)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation: "	<< snmp_errno << std::endl;
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation: "	<< snmp_errstring(snmp_errno) << std::endl;

		getResult = GETSET_SNMP_PDU_CREATE_ERROR;
	}

	std::cout << __FUNCTION__ << "@" << __LINE__ <<	" Close snmp session and exit " << std::endl;

	snmp_free_pdu(snmpResp);
	snmp_sess_close(snmpSess);

	free(snmpSession.community);
	free(snmpSession.peername);
	return getResult;
}


int ACS_CS_SNMPManager::snmpSet(const char* community, const char* peername,const char* usrOID,  char type, const char *value)
{
	/*
		TYPE: one of i, u, t, a, o, s, x, d, b
			i: INTEGER, u: unsigned INTEGER, t: TIMETICKS, a: IPADDRESS
			o: OBJID, s: STRING, x: HEX STRING, d: DECIMAL STRING, b: BITS
			U: unsigned int64, I: signed int64, F: float, D: double
	*/

	snmp_session snmpSession;
	snmp_session *snmpSessionHandler;

	int setResult = GETSET_SNMP_OK;
	struct snmp_pdu *snmpPDU;
	struct snmp_pdu *snmpResp = NULL;
	int res;

	snmp_sess_init( &snmpSession );

	snmpSession.version = SNMP_VERSION_2c;
	snmpSession.community = (u_char*) strdup(community);
	snmpSession.retries = 5;
	snmpSession.timeout = 1000000;
	snmpSession.community_len = strlen((const char *)snmpSession.community);
	snmpSession.peername = strdup(peername);

	std::cout << __FUNCTION__ << "@" << __LINE__ <<" init snmp session ok ... IP: " << snmpSession.peername <<std::endl;

	snmpSessionHandler = snmp_open(&snmpSession);

	if(snmpSessionHandler == NULL)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ <<	" snmp_open failed - snmp_errno: " << snmp_errno << std::endl;
		return OPEN_SESS_ERROR;
	}

	//snmpPDU = snmp_pdu_create(SNMP_MSG_SET);

	oid snmp_oid[MAX_OID_LEN];
	size_t snmp_oid_len = MAX_OID_LEN;

	if (! read_objid(usrOID, snmp_oid, &snmp_oid_len) )
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: "	<< snmp_errstring(snmp_errno) << std::endl;
		snmp_close(snmpSessionHandler);
		return GETSET_SNMP_PARSE_OID_ERROR;
	}

	if ((snmpPDU = snmp_pdu_create(SNMP_MSG_SET)) != NULL)
	{

		if (snmp_add_var(snmpPDU, snmp_oid, snmp_oid_len, type, value) )
		{

			//std::cout << __FUNCTION__ << "@" << __LINE__ << " snmp_add_var error: "	<< snmp_errstring(snmpResp->errstat) << std::endl;
			snmp_close(snmpSessionHandler);
			return GETSET_SNMP_PARSE_OID_ERROR;
		}

		// send pdu...
		res = snmp_synch_response(snmpSessionHandler, snmpPDU, &snmpResp);

		switch(res)
		{
			case STAT_SUCCESS:

				//Check the response
				if (snmpResp->errstat == SNMP_ERR_NOERROR)
				{
					// SET successful done ;) !!
					//snmp_free_pdu(snmpResp);
				}
				else
				{
					std::cout << __FUNCTION__ << "@" << __LINE__ << " SET-Response error: "	<< snmp_errstring(snmpResp->errstat) << std::endl;
					//snmp_free_pdu(snmpResp);
					setResult =  GETSET_SNMP_RESPONSE_ERROR;
				}

				break;

			case STAT_TIMEOUT:
				std::cout << __FUNCTION__ << "@" << __LINE__ <<	" STAT_TIMEOUT " << std::endl;

				std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: "	<< snmp_errstring(snmp_errno) << std::endl;

				setResult = GETSET_SNMP_STATUS_TIMEOUT;
				//snmp_free_pdu(snmpResp);
				break;

			case STAT_ERROR:
				std::cout << __FUNCTION__ << "@" << __LINE__ <<	" STAT_ERROR " << std::endl;

				std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: " << snmp_errstring(snmp_errno) << std::endl;

				//snmp_free_pdu(snmpResp);
				setResult = GETSET_SNMP_STATUS_ERROR;
				break;

		} //switch(res)
	}
	else //if ((snmpPDU = snmp_pdu_create(SNMP_MSG_SET)) != NULL)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation num: "	<< snmp_errno << std::endl;
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation: "	<< snmp_errstring(snmp_errno) << std::endl;
		setResult = GETSET_SNMP_PDU_CREATE_ERROR;
	}

	snmp_free_pdu(snmpResp);
	snmp_close(snmpSessionHandler);
	return setResult;
}


std::string ACS_CS_SNMPManager::getIpAddress(void)
{
	return std::string(ipAddr);
}

void ACS_CS_SNMPManager::setIpAddress(std::string ip)
{
	ipAddr = ip.c_str();
}


bool ACS_CS_SNMPManager::isMaster ()
{
	//oidShelfMgrCtrlSCX
	int res;
	unsigned char out[SNMP_MAXBUF];
	std::string tmpOID("");

	if(ACS_CS_Registry::IsCba() == true){
		tmpOID = oidShelfMgrCtrlSCX + ".0";
	}else{
		return false; //TODO
	}

	//192.168.170.60
	res = snmpGet("PUBLIC", ipAddr.c_str(), tmpOID.c_str(), ASN_OCTET_STR, out, SNMP_MAXBUF, ACS_CS_SNMPManager::SNMPGET_HEX_STRING);

	if (res == GETSET_SNMP_OK)
	{
		static char buffer[128];
		sprintf(buffer, "%s\n", (char *) out);
		std::string shelfMgrCtrlState = std::string((char*) out);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Value: " << shelfMgrCtrlState << std::endl;

		int masterSCB = 0;
		if (shelfMgrCtrlState.length() >= 2)
		{
			std::string tmp = shelfMgrCtrlState.substr(0,2);
			masterSCB = ::atoi(tmp.c_str());
		}
		else
		{
			masterSCB = ::atoi(shelfMgrCtrlState.c_str());
		}

		if (masterSCB == 3) //Active
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " is master." << std::endl;
			return true;
		}
		else
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " is passive." << std::endl;
			return false;
		}
	}
	else
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Get failed on: " << tmpOID.c_str() << std::endl;

	return false;
}

int ACS_CS_SNMPManager::isMasterSCX(std::string ip)
{
	//oidShelfMgrCtrlSCX
	int resp = 0;
	int res;
	unsigned char out[SNMP_MAXBUF];
	std::string tmpOID("");

	if(ACS_CS_Registry::IsCba() == true){
		tmpOID = oidShelfMgrCtrlSCX + ".0";
	}else{
		return false; //TODO
	}

	setIpAddress(ip);

	res = snmpGet("PUBLIC", ipAddr.c_str(), tmpOID.c_str(), ASN_OCTET_STR, out, SNMP_MAXBUF,ACS_CS_SNMPManager::SNMPGET_HEX_STRING);

	if (res == GETSET_SNMP_OK)
	{
		static char buffer[128];
		sprintf(buffer, "%s\n", (char *) out);
		std::string shelfMgrCtrlState = std::string((char*) out);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Value: " << shelfMgrCtrlState << std::endl;

		int masterSCB = 0;
		if (shelfMgrCtrlState.length() >= 2)
		{
			std::string tmp = shelfMgrCtrlState.substr(0,2);
			masterSCB = ::atoi(tmp.c_str());
		}
		else
		{
			masterSCB = ::atoi(shelfMgrCtrlState.c_str());
		}

		if (masterSCB == 3) //Active
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " is master." << std::endl;
			resp = 1;
		}
		else
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " is passive." << std::endl;
			resp = 2;
		}
	}
	else if (res == GETSET_SNMP_STATUS_TIMEOUT)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Get failed on: " << tmpOID.c_str() << std::endl;
		resp = 14;
	}

	return resp;
}

int ACS_CS_SNMPManager::isMasterSCB(std::string ip, int slot)
{
	int resp = 0;
	int res;
	unsigned char out[SNMP_MAXBUF];
	std::string tmpOID("");

	std::stringstream ss;
	ss << slot;

	tmpOID = oidShelfMgrCtrlSCB + "." + ss.str();

	setIpAddress(ip);

	res = snmpGet("PUBLIC", ipAddr.c_str(), tmpOID.c_str(), ASN_INTEGER, out, SNMP_MAXBUF);

	if (res == GETSET_SNMP_OK)
	{
		char buffer[128];
		sprintf(buffer, "%d", *((int*)&out));


		int masterSCB = atoi(buffer);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Value: " << masterSCB << std::endl;

		if (masterSCB == 1) //Active
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " is master." << std::endl;
			resp = 1;
		}
		else
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " is passive." << std::endl;
			resp = 2;
		}
	}
	else if (res == GETSET_SNMP_STATUS_TIMEOUT)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Get failed on: " << tmpOID.c_str() << std::endl;
		resp = 14;
	}

	return resp;
}


bool ACS_CS_SNMPManager::isPassiveDeg ()
{
	//oidShelfMgrCtrlSCX
	int res;
	unsigned char out[SNMP_MAXBUF];
	std::string tmpOID("");
	if(ACS_CS_Registry::IsCba() == true){
		tmpOID = oidShelfMgrCtrlSCX + ".0";
	}else{
		return false; //TODO
	}

	res = snmpGet("PUBLIC", ipAddr.c_str(), tmpOID.c_str(), ASN_OCTET_STR, out, SNMP_MAXBUF, ACS_CS_SNMPManager::SNMPGET_HEX_STRING);

	if (res == GETSET_SNMP_OK)
	{
		static char buffer[128];
		sprintf(buffer, "%s\n", (char *) out);
		std::string shelfMgrCtrlState = std::string((char*) out);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Value: " << shelfMgrCtrlState << std::endl;

		int intStatus = 0;
		if (shelfMgrCtrlState.length() >= 2)
		{
			std::string tmp = shelfMgrCtrlState.substr(0,2);
			intStatus = ::atoi(tmp.c_str());
		}
		else
		{
			intStatus = ::atoi(shelfMgrCtrlState.c_str());
		}

		if (intStatus == 2) //PassiveDegraded
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " PassiveDegraded ." << std::endl;
			return true;
		}
		else
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " ipAddr: " << ipAddr << " SCX OK." << std::endl;
			return false;
		}
	}
	else
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Get failed on: " << tmpOID.c_str() << std::endl;

	return false;
}
