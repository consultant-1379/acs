///*
// *
// * NAME: dShelfMngrLog.cpp
// *
// * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
// * All rights reserved.
// *
// *  The Copyright to the computer program(s) herein
// *  is the property of Ericsson Telecom AB, Sweden.
// *  The program(s) may be used and/or copied only with
// *  the written permission from Ericsson Telecom AB or in
// *  accordance with the terms and conditions stipulated in the
// *  agreement/contract under which the program(s) have been
// *  supplied.
// *
// * .DESCRIPTION
// *  This is the main program for the dShelfMngrLog command.
// *
// * DOCUMENT NO
// *
// *
// * AUTHOR
// * 	2016-09-26  XSABJHA
// *
// * REVISION
// *
// * CHANGES
// *
// * RELEASE REVISION HISTORY
// *
// * REV NO		DATE		NAME 		DESCRIPTION
//
// */


#include <arpa/inet.h>
#include <ace/Task.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/mib_api.h>
#include <net-snmp/library/mib.h>
#include <ACS_CS_API.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

#define SNMP_RETRY 5
#define SNMP_TIMEOUT 200000
//SNMP return codes
enum {
	GETSET_SNMP_OK,
	GETSET_SNMP_OK_LEN_ZERO,
	OPEN_SESS_INIT_ERROR,
	OPEN_SESS_ERROR,
	GETSET_SNMP_PARSE_OID_ERROR,
	GETSET_SNMP_PDU_CREATE_ERROR,
	GETSET_SNMP_ADD_NULL_VAR,
	GETSET_SNMP_RESPONSE_ERROR,
	GETSET_SNMP_STATUS_ERROR,
	GETSET_SNMP_STATUS_TIMEOUT,
	GETSET_SNMP_BUFFERSMALL
};

//CSAPI return codes
enum {
	EXECUTED    	  = 0,
	INTERNALERR      = 1,
	NO_REPLY_FROM_CS = 7,
	HARDWARE_NOT_PRESENT= 14
};

enum {
	SNMPGET_STD_STRING,
	SNMPGET_HEX_STRING,
	SNMPGET_ASCII_STRING
};

//GEN-LOG-MIB
std::string oidDeltcontfile = "1.3.6.1.4.1.193.177.2.2.4.4.4.0";
std::string oidContFileName = "1.3.6.1.4.1.193.177.2.2.4.4.2.1.2";
std::string oidContFileStatus = "1.3.6.1.4.1.193.177.2.2.4.4.2.1.4";
std::string oidContTransferSrvIP = "1.3.6.1.4.1.193.177.2.2.4.4.1.1.0";
std::string oidContTransferSrvPath  = "1.3.6.1.4.1.193.177.2.2.4.4.1.2.0";
std::string oidContAutomaticTransfer = "1.3.6.1.4.1.193.177.2.2.4.4.1.4.0";
std::string oidContFileEncryption = "1.3.6.1.4.1.193.177.2.2.4.4.1.6.0";
std::string oidCreateContFile = "1.3.6.1.4.1.193.177.2.2.4.4.6.0";


//********************************************************************************************************

int snmpGet(const char* community, const char* peername, const char* usrOID, int type, void* out, unsigned int iBufSize, int strType)
{
	int res;
	snmp_session snmpSession;
	char *pchBuffer = NULL;
	int *piBuffer = NULL;
	int iReqSize;
	int getResult = GETSET_SNMP_OK;
	struct snmp_pdu *snmpPDU = NULL;
	struct snmp_pdu *snmpResp = NULL;
	void *snmpSess = NULL;

	struct variable_list *varsList = NULL;

	pchBuffer = NULL;
	piBuffer = NULL;
	iReqSize = 0;
	snmp_sess_init(&snmpSession);

	snmpSession.version = SNMP_VERSION_2c;
	snmpSession.community = (u_char*) strdup(community);
	snmpSession.retries = SNMP_RETRY;
	snmpSession.timeout = SNMP_TIMEOUT;
	snmpSession.community_len = strlen((const char *) snmpSession.community);
	snmpSession.peername = strdup(peername);

	snmpSess = snmp_sess_open(&snmpSession);

	if (snmpSess == NULL)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << " ERROR: snmp_open failed - snmp_errno: " << snmp_errno << std::endl;
		free(snmpSession.community);
		free(snmpSession.peername);
		return OPEN_SESS_ERROR;
	}

	free(snmpSession.community);
	free(snmpSession.peername);

	oid snmp_oid[MAX_OID_LEN];
	size_t snmp_oid_len = MAX_OID_LEN;

	if (!read_objid(usrOID, snmp_oid, &snmp_oid_len))
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << " ERROR: Transform OID Failed " << std::endl;
		snmp_sess_close(snmpSess);
		return GETSET_SNMP_PARSE_OID_ERROR;
	}

	if ((snmpPDU = snmp_pdu_create(SNMP_MSG_GET)) != NULL)
	{
		snmp_add_null_var(snmpPDU, snmp_oid, snmp_oid_len);
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << " Send request ! " << std::endl;

		res = snmp_sess_synch_response(snmpSess, snmpPDU, &snmpResp);

		switch (res)
		{
		case STAT_SUCCESS:
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << " STAT_SUCCESS " << std::endl;

			if ((out != NULL) && (iBufSize))
			{
				if (snmpResp->errstat == SNMP_ERR_NOERROR)
				{
					for (varsList = snmpResp->variables; varsList; varsList = varsList->next_variable)
					{
						if (varsList->val_len == 0)
						{
							//out = NULL;
							getResult = GETSET_SNMP_OK_LEN_ZERO;
							break;
						}

						// return response as string
						if ((varsList->type == type) && (type == ASN_OCTET_STR))
						{
							cout << "received string" << endl;
							switch (strType)
							{

							case SNMPGET_STD_STRING:
							{
								pchBuffer = reinterpret_cast<char*>(out);

								memset(pchBuffer, 0, iBufSize);
								memcpy(pchBuffer, varsList->val.string, varsList->val_len);

								iReqSize = 0;

								// Check len of required buffer
								if ((unsigned int) iReqSize >= iBufSize)
								{
									// Error. Buffer is too small
									std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: SNMPGET_STD_STRING buffer is too small! " << std::endl;
									getResult = GETSET_SNMP_BUFFERSMALL;
								}
								break;
							}

							case SNMPGET_HEX_STRING:
							{
								pchBuffer = reinterpret_cast<char*>(out);
								iReqSize = snprint_hexstring(pchBuffer, iBufSize, varsList->val.string, varsList->val_len);
								// Check error
								if (iReqSize < 0)
								{
									// Error. Is the Buffer too small?
									std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: SNMPGET_HEX_STRING can't write in the buffer. Is buffer too small?" << std::endl;
									getResult = GETSET_SNMP_BUFFERSMALL;
								}
								break;
							}

							case SNMPGET_ASCII_STRING:
							{
								pchBuffer = reinterpret_cast<char*>(out);
								iReqSize = snprint_asciistring(pchBuffer, iBufSize, varsList->val.string, varsList->val_len);
								// Check error
								if (iReqSize < 0)
								{
									// Error. Is the Buffer too small?
									std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: SNMPGET_ASCII_STRING can't write in the buffer. Is buffer too small?" << std::endl;
									getResult = GETSET_SNMP_BUFFERSMALL;
								}
								break;
							}

							}
							break;
						}
						else if ((varsList->type == type) && (type == ASN_INTEGER))
						{
							if (iBufSize >= sizeof(int))
							{
								piBuffer = reinterpret_cast<int*>(out);
								*piBuffer = (int) (*(varsList->val.integer));
							}
							else
							{
								// Error. Is the Buffer too small?
								std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: ASN_INTEGER buffer is too small!" << std::endl;
								getResult = GETSET_SNMP_BUFFERSMALL;
							}
							//out = (int *) *varsList->val.integer;
							break;
						}
						else if ((varsList->type == type) && (type == ASN_IPADDRESS))
						{
							pchBuffer = reinterpret_cast<char*>(out);
							iReqSize = snprint_ipaddress(pchBuffer, iBufSize, varsList, NULL, NULL, NULL);
							// Check error
							if (iReqSize < 0)
							{
								// Error. Is the Buffer too small?
								std::cout << __FUNCTION__ << "@" << __LINE__ << "### ERROR: SNMPGET_IPADDRESS can't write in the buffer. Is buffer too small?" << std::endl;
								getResult = GETSET_SNMP_BUFFERSMALL;
							}

						}
						else
						{
							//todo
						}

					} //for (varsList = snmpResp->variables ; varsList ; varsList = varsList->next_variable)

				}
				else //if (snmpResp->errstat == SNMP_ERR_NOERROR)
				{
					std::cout << __FUNCTION__ << "@" << __LINE__ << " ERROR: GET-Response Error: " << snmp_errstring(snmpResp->errstat) << std::endl;
					//snmp_free_pdu(snmpResp);
					//out = NULL;
					getResult = GETSET_SNMP_RESPONSE_ERROR;
				}
			}
			else
			{  // if((out != NULL) && (iBufSize))
				// Error buffer
				std::cout << __FUNCTION__ << "@" << __LINE__ << " ## ERROR: incorrect argument [buffer].  " << std::endl;
				getResult = GETSET_SNMP_RESPONSE_ERROR;
			}
			break; //EXIT FOR !

		case STAT_TIMEOUT:
			std::cout << __FUNCTION__ << "@" << __LINE__ << " STAT_TIMEOUT " << std::endl;
			std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: " << snmp_errstring(snmp_errno) << std::endl;

			//snmp_free_pdu(snmpResp);
			//out = NULL;
			getResult = GETSET_SNMP_STATUS_TIMEOUT;
			break;

		case STAT_ERROR:
			std::cout << __FUNCTION__ << "@" << __LINE__ << " STAT_ERROR " << std::endl;

			std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: " << snmp_errstring(snmp_errno) << std::endl;

			//snmp_free_pdu(snmpResp);
			//out = NULL;
			getResult = GETSET_SNMP_STATUS_ERROR;
			break;
		}

	}
	else //if ((snmpPDU = snmp_pdu_create(SNMP_MSG_GET)) != NULL)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation: " << snmp_errno << std::endl;
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation: " << snmp_errstring(snmp_errno) << std::endl;

		getResult = GETSET_SNMP_PDU_CREATE_ERROR;
	}

	std::cout << __FUNCTION__ << "@" << __LINE__ << " Close snmp session and exit " << std::endl;

	snmp_free_pdu(snmpResp);

	//snmp_close(snmpSessionHandler);
	snmp_sess_close(snmpSess);

	return getResult;
}

//*********************************************************************************************************
int snmpSet(const char* community, const char* peername, const char* usrOID, char type, const char *value)
{
	snmp_session snmpSession;
	void *snmpSess = NULL;

	int setResult = GETSET_SNMP_OK;
	struct snmp_pdu *snmpPDU = NULL;
	struct snmp_pdu *snmpResp = NULL;
	int res;

	snmp_sess_init(&snmpSession);

	snmpSession.version = SNMP_VERSION_2c;
	snmpSession.community = (u_char*) strdup(community);
	snmpSession.retries = SNMP_RETRY;
	snmpSession.timeout = SNMP_TIMEOUT;
	snmpSession.community_len = strlen((const char *) snmpSession.community);
	snmpSession.peername = strdup(peername);
	std::cout << __FUNCTION__ << "@" << __LINE__ << " init snmp session ok ... IP: " << snmpSession.peername << std::endl;

	snmpSess = snmp_sess_open(&snmpSession);
	if (snmpSess == NULL)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << " ERROR: snmp_open failed - snmp_errno: " << snmp_errno << std::endl;
		free(snmpSession.community);
		free(snmpSession.peername);
		return OPEN_SESS_ERROR;
	}
	free(snmpSession.community);
	free(snmpSession.peername);

	oid snmp_oid[MAX_OID_LEN];
	size_t snmp_oid_len = MAX_OID_LEN;

	if (!read_objid(usrOID, snmp_oid, &snmp_oid_len))
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: " << snmp_errstring(snmp_errno) << std::endl;
		snmp_sess_close(snmpSess);
		return GETSET_SNMP_PARSE_OID_ERROR;
	}

	if ((snmpPDU = snmp_pdu_create(SNMP_MSG_SET)) != NULL)
	{

		if (snmp_add_var(snmpPDU, snmp_oid, snmp_oid_len, type, value))
		{
			snmp_free_pdu(snmpResp);
			snmp_sess_close(snmpSess);
			return GETSET_SNMP_PARSE_OID_ERROR;

		}
		res = snmp_sess_synch_response(snmpSess, snmpPDU, &snmpResp);

		switch (res)
		{
		case STAT_SUCCESS:

			//Check the response
			if (snmpResp->errstat == SNMP_ERR_NOERROR)
			{
				// SET successful done ;) !!
			}
			else
			{
				std::cout << __FUNCTION__ << "@" << __LINE__ << " SET-Response error: " << snmp_errstring(snmpResp->errstat) << std::endl;
				setResult = GETSET_SNMP_RESPONSE_ERROR;
			}

			break;

		case STAT_TIMEOUT:
			std::cout << __FUNCTION__ << "@" << __LINE__ << " STAT_TIMEOUT " << std::endl;

			std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: " << snmp_errstring(snmp_errno) << std::endl;

			setResult = GETSET_SNMP_STATUS_TIMEOUT;
			break;

		case STAT_ERROR:
			std::cout << __FUNCTION__ << "@" << __LINE__ << " STAT_ERROR " << std::endl;

			std::cout << __FUNCTION__ << "@" << __LINE__ << " Error: " << snmp_errstring(snmp_errno) << std::endl;
			setResult = GETSET_SNMP_STATUS_ERROR;
			break;

		} //switch(res)
	}
	else //if ((snmpPDU = snmp_pdu_create(SNMP_MSG_SET)) != NULL)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation num: " << snmp_errno << std::endl;
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Error during the PDU creation: " << snmp_errstring(snmp_errno) << std::endl;
		setResult = GETSET_SNMP_PDU_CREATE_ERROR;
	}

	snmp_free_pdu(snmpResp);
	snmp_sess_close(snmpSess);
	return setResult;
}
//******************************************************************************

int createDir(std::string dirName)
{
	struct stat dirbuf;
	if (::stat(dirName.c_str(), &dirbuf)!= 0)
	{ //ERROR

		if (int resMk = ::mkdir(dirName.c_str(), 0777))
		{
			std::cout << "Creation failed res: " << resMk << std::endl;
			return -1;
		}
		else
		{
			std::cout << "Dir successfully created " << resMk << std::endl;

			if (int resChmod = ::chmod(dirName.c_str(), 0777))
			{
				std::cout << "resChmod " << resChmod << std::endl;
				return -1;
			}
			else
			{
				std::cout << "Chmod ok" << resChmod << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Already presents: " << std::endl;

		if (int resChmod = ::chmod(dirName.c_str(), 0777))
		{
			std::cout << "resChmod " << resChmod << std::endl;
			return -1;
		}
		else
		{
			std::cout << "Chmod ok" << resChmod << std::endl;
		}
	}
	return 0;
}

int getNode(){

	std::string node_id_path = "/etc/cluster/nodes/this/id";

	fstream nodeFile;
	nodeFile.open(node_id_path.c_str(), ios::in);
	if (!nodeFile) {
		std::cout << "File Not found" << std::endl;
		return -1;
	}
	char val[256];
	char *c = val;
	nodeFile.getline(c, 256);

	nodeFile.close();
	return ::atoi(c);
}

//*****************************************************************************************************************************
int sendSwitchLog(const char * switch_board_ip)
{

	int call_result = 0;

	std::string tftpRoot = "/data/apz/data";
	std::string tftpPathSMX = "/boot/smx/smx_logs/smx_";
	int nodeNum = getNode();
	std::string subnet = std::string(switch_board_ip).substr(0, 12);
	char c[16];
	sprintf(c, "%d", nodeNum);
	std::string clusterIP = subnet + c;


	std::cout << "clusterIP = " << clusterIP.c_str() << std::endl;

	int createRes;
	createRes = createDir(tftpRoot + "/boot/smx");
	std::cout << "createRes = " << createRes << std::endl;

	createRes = createDir(tftpRoot + "/boot/smx/smx_logs");
	std::cout << "createRes = " << createRes << std::endl;

	std::string ipFiltered(switch_board_ip);
	if (ipFiltered.compare("") != 0)
	{
		size_t foundDot;
		foundDot = ipFiltered.find_first_of(".");
		while (foundDot != string::npos)
		{
			ipFiltered[foundDot] = '_';
			foundDot = ipFiltered.find_first_of(".", foundDot + 1);
		}
	}
	std::string logPath = tftpRoot + tftpPathSMX + ipFiltered;

	std::cout << "logPath = " << logPath << std::endl;

	createRes = createDir(logPath);
	std::cout << "createRes = " << createRes << std::endl;

	std::string snmplogPath= tftpPathSMX + ipFiltered;
	std::cout << "snmplogPath = " << snmplogPath << std::endl;

	call_result = 0;
	do
	{
		//oidContTransferSrvIP
		int retryVal = 0;
		do
		{
			call_result = snmpSet("NETMAN", switch_board_ip, oidContTransferSrvIP.c_str(), 's', clusterIP.c_str());
			retryVal++;
		} while (retryVal < 3 && call_result == GETSET_SNMP_STATUS_TIMEOUT);

		if (call_result)
		{ // ERROR: sending the request
			break;
		}

		//ContTransferSrvPath
		retryVal = 0;
		do
		{
			call_result = snmpSet("NETMAN", switch_board_ip, oidContTransferSrvPath.c_str(), 's', snmplogPath.c_str());
			retryVal++;
		} while (retryVal < 3 && call_result == GETSET_SNMP_STATUS_TIMEOUT);

		if (call_result)
		{ // ERROR: sending the request
			break;
		}

		std::string Value = "1";
		//ContFileAutomaticTransfer
		retryVal = 0;
		do
		{
			call_result = snmpSet("NETMAN", switch_board_ip, oidContAutomaticTransfer.c_str(), 'i', Value.c_str());
			retryVal++;
		} while (retryVal < 3 && call_result == GETSET_SNMP_STATUS_TIMEOUT);

		if (call_result)
		{ // ERROR: sending the request
			break;
		}

		//ContFileEncryption
		retryVal = 0;
		do
		{
			call_result = snmpSet("NETMAN", switch_board_ip, oidContFileEncryption.c_str(), 'i', Value.c_str());
			retryVal++;
		} while (retryVal < 3 && call_result == GETSET_SNMP_STATUS_TIMEOUT);

		if (call_result)
		{ 
			break;
		}

		//CreateContFile
		retryVal = 0;
		do
		{
			call_result = snmpSet("NETMAN", switch_board_ip, oidCreateContFile.c_str(), 'i', Value.c_str());
			retryVal++;
		} while (retryVal < 3 && call_result == GETSET_SNMP_STATUS_TIMEOUT);

		if (call_result)
		{ // ERROR: sending the request
			break;
		}

	} while (0);

	return call_result ? -1 : 0;
}

std::string ulongToStringIP (unsigned long ipaddress)
{
	char tempValueStr[16] = {0};
	uint32_t tempValue = ipaddress;
	snprintf(tempValueStr, sizeof(tempValueStr), "%u.%u.%u.%u", (tempValue >> 24) & 0xFF, (tempValue >> 16) & 0xFF, (tempValue >> 8) & 0xFF, tempValue & 0xFF);
	std::string IP = tempValueStr;
	return IP;
}

//*******************************************************************************
int allSMXBoardInfo(std::vector<std::string> & EthAvector, std::vector<std::string> & EthBvector)
{
	int retcode = EXECUTED;

	ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
	ACS_CS_API_BoardSearch * boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
	boardSearch->reset();
	ACS_CS_API_IdList boardList;

	boardSearch->setFBN(ACS_CS_API_HWC_NS::FBN_SMXB);

	ACS_CS_API_NS::CS_API_Result returnValue = hwc->getBoardIds(boardList, boardSearch);

	if (returnValue == ACS_CS_API_NS::Result_Success)
	{
		if (boardList.size() == 0)
		{
			retcode = HARDWARE_NOT_PRESENT;
		}
		else
		{
			// got some SMX board
			for (unsigned int i = 0; i < boardList.size(); i++)
			{
				BoardID boardId = boardList[i];
				uint32_t IP_EthA = 0;
				returnValue = hwc->getIPEthA(IP_EthA, boardId);
				if (returnValue == ACS_CS_API_NS::Result_Success)
				{
					uint32_t IP_EthB = 0;
					returnValue = hwc->getIPEthB(IP_EthB, boardId);
					if (returnValue == ACS_CS_API_NS::Result_Success)
					{
						std::string ipA_String = ulongToStringIP(IP_EthA);
						std::string ipB_String = ulongToStringIP(IP_EthB);

						EthAvector.push_back(ipA_String);
						EthBvector.push_back(ipB_String);

					}
					else
					{
						retcode = NO_REPLY_FROM_CS;
						break;
					}
				}
				else
				{
					//serious fault in CS: No magazine found for the CMXB board

					retcode = NO_REPLY_FROM_CS;
					break;
				}

			}

		}
	}
	else
	{
		retcode = NO_REPLY_FROM_CS;
	}
	if (boardSearch)
		ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
	if (hwc)
		ACS_CS_API::deleteHWCInstance(hwc);
	return retcode;
}

//*****************************************************************************
std::string intToString (int ivalue)
{
	std::stringstream ss_value("");
	ss_value << ivalue; //try to convert operand to value number
	return ss_value.str();
}

//******************************************************************************
int deleteContainerFile(const char * switch_board_ip, int & index)
{
	std::string indexStr("");
	unsigned char out[SNMP_MAXBUF];
	int call_result = 0;
	int result = -1;
	for (int i = 0; i < 10; i++)
	{
		indexStr = intToString(i);
		string oidToSet = oidContFileName + "." + indexStr;

		call_result = snmpGet("PUBLIC", switch_board_ip, oidToSet.c_str(), ASN_OCTET_STR, out, SNMP_MAXBUF, SNMPGET_STD_STRING);
		if (!call_result)
		{
			call_result = snmpSet("NETMAN", switch_board_ip, oidDeltcontfile.c_str(), 's', reinterpret_cast<const char*>(out));
			if (!call_result)
			{
				std::cout << "deletion passed for index" << i << std::endl;
				index = i;
				result = 0;
				break;
			}
		}
		else if (call_result != GETSET_SNMP_OK_LEN_ZERO)
		{
			std::cout << "deleteContaineFile function failed with error===" << call_result << std::endl;
			break;
		}
		else
		{
			index = i;
			std::cout << "No file found in index==" << i << std::endl;
			result = 0;
			break;
		}
	}
	return result;
}
//*****************************************************************************

int chkFileTransfrStatus(const char * switch_ip, int index)
{
	unsigned char out[SNMP_MAXBUF];

	int call_result = 0;
	int result = -1;
	string indexStr("");
	int i = 0;
	indexStr = intToString(index);
	string oidToSet = oidContFileStatus + "." + indexStr;
	do
	{
		call_result = snmpGet("PUBLIC", switch_ip, oidToSet.c_str(), ASN_INTEGER, out, SNMP_MAXBUF, 0);

		if (call_result == GETSET_SNMP_OK)
		{
			int status = *((int*)&out);
			if (status == 2)
			{
				result = 0;
				break;
			}
			else
			{
				sleep(1);
				i++;
			}
		}
		else
		{
			i++;
		}
	} while (i < 30);

	return result;
}
//******************************************************************************
// main process
//******************************************************************************
int main()
{
	std::vector<std::string> EthAvector;
	std::vector<std::string> EthBvector;

	int retcode = EXECUTED;
	retcode = allSMXBoardInfo(EthAvector, EthBvector);
	if (retcode != EXECUTED)
	{
		return retcode;
	}
	int size = EthAvector.size();
	int failcount = 0;
	int call_result = -1;
	int index = 0;

	for (int i = 0; i < size; i++)
	{
		index = 0;
		call_result=-1;
		int ip_index = ::random() & 0x01;
			
		const char * ip_addr [2] = { EthAvector[i].c_str(),EthBvector[i].c_str() };
		for (unsigned int j = 0; (j < 2) && call_result; ++j)
		{
			call_result = deleteContainerFile(ip_addr[ip_index], index);
			if (call_result) {
				ip_index = (ip_index + 1) & 0x01;
			}
			
		}
		if (call_result)
		{
			cout<<"deletion of container file failed for ip"<<EthAvector[i].c_str()<<"and"<<EthBvector[i].c_str()<<endl;
			failcount++;
			continue;
		} else 
			cout<<"deletion of container file successful for ip"<<ip_addr[ip_index]<<endl;
			
		call_result=-1;
		for (unsigned int j = 0; (j < 2) && call_result; ++j)
		{
			call_result = sendSwitchLog(ip_addr[ip_index]);
			if (call_result) {
				ip_index = (ip_index + 1) & 0x01;
			}

		}
		if (call_result)
                {
                        cout<<"Log transfer failed for ip"<<EthAvector[i].c_str()<<"and"<<EthBvector[i].c_str()<<endl;
			failcount++;
                        continue;
                }else 
                        cout<<"Log transfer started for ip"<<ip_addr[ip_index]<<endl;

		call_result=-1;
		for (unsigned int j = 0; (j < 2) && call_result; ++j)
                {
                        call_result = chkFileTransfrStatus(ip_addr[ip_index],index);
			if (call_result) {
				ip_index = (ip_index + 1) & 0x01;
			}

                }
                if (call_result)
                {
                        cout<<"Checking File transfer status failed for ip"<<EthAvector[i].c_str()<<"and"<<EthBvector[i].c_str()<<endl;
                        failcount++;
                }else
                        cout<<"File transfer successful for ip"<<ip_addr[ip_index]<<endl;
	}

	if (failcount == size)
	{
		cout << "log fetching failed for all the board" << std::endl;
		return -1;
	}
	return 0;
}
