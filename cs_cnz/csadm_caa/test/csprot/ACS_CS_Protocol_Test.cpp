#include <iostream>
#include <string>
#include <iomanip>
#include <map>

#include <stdlib.h>
#include <getopt.h>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"

#include "ACS_CS_Protocol.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_Primitive.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_HWC_GetBoardList.h"
#include "ACS_CS_HWC_GetBoardListResponse.h"
#include "ACS_CS_HWC_GetValue.h"
#include "ACS_CS_HWC_GetValueResponse.h"

#include "ACS_CS_VLAN_GetVLANAddress.h"
#include "ACS_CS_VLAN_GetVLANAddressResponse.h"
#include "ACS_CS_VLAN_GetVLANList.h"
#include "ACS_CS_VLAN_GetVLANListResponse.h"
#include "ACS_CS_VLAN_GetVLANTable.h"
#include "ACS_CS_VLAN_GetVLANTableResponse.h"
#include "ACS_CS_VLAN_GetNetworkTable.h"
#include "ACS_CS_VLAN_GetNetworkTableResponse.h"

#include "ACS_CS_GLK_GetGlobalLogicalClock.h"
#include "ACS_CS_GLK_GetGlobalLogicalClockResponse.h"

#include "ACS_CS_MODD_Change.h"
#include "ACS_CS_MODD_ChangeResponse.h"
#include "ACS_CS_MODD_Revert.h"
#include "ACS_CS_MODD_RevertResponse.h"
#include "ACS_CS_MODD_Header.h"
#include "ACS_CS_CPId_GetMAUType.h"
#include "ACS_CS_CPId_GetMAUTypeResponse.h"

#include "ACS_CS_Registry.h"

using namespace ACS_CS_Protocol;
using std::endl;
using std::cout;

void testGetMauType(unsigned short cpId, int version=ACS_CS_Protocol::Latest_Version);
void testParser();
int sendData();
void getPduExample1(char * buffer, int size);
void printPduData(ACS_CS_PDU &pdu);
void testHwcTable();
void testVlanTable(std::string mag, uint16_t slot);

void testGetVlanTable(int ver);
void testGclk();
void testGeNetworkTable(int ver, std::string mag, uint16_t slot);


void testMODDChange(std::string mac, std::string boot, int timeout);
void testMODDRevert(std::string mac);

int sendRecvData(ACS_CS_PDU *pdu);
int sendRecvData(ACS_CS_PDU *pdu, ACS_CS_PDU& response, bool isModd = false);

void parse(char* buf, int size);
void parse(char* buffer, int size, ACS_CS_PDU& pdu, bool isModd = false);

int normalize(char r);
void hexCharToBinChar(unsigned char *input,char *out, int size);
int getRealMac(std::string ss,char *out, int size);

ACS_CS_Protocol::CS_Result_Code getValue(int boardId, ACS_CS_Protocol::CS_Attribute_Identifier attrId, char* &buf);
ACS_CS_Protocol::CS_Result_Code getValue(int boardId, ACS_CS_Protocol::CS_Attribute_Identifier attrId, char* &buf, int &buf_size);

void getVLANList();
void usage(const char *prgName);
int architecture=0;



#define APZVLANLS 1
#define MODDCHANGE 0
#define MODDREVERT 0
#define GETMAUTYPE 0
#define GETUUID 0

void usage(const char *prgName) {
	std::cout << "Usage: " << prgName << " [[-m magazine] [-s slot] [-n version]] [-l] [-g] [-t version]" << std::endl;
	std::cout << "version - 0 : native APG " << std::endl;
	std::cout << "version - 1 : QoS Handling " << std::endl;
	std::cout << "version - 2 : vAPG " << std::endl;
	std::cout << std::endl;
}


int main (int argc, char **argv) {

	//    testHwcTable();

#if APZVLANLS
	uint16_t slot;
	int vlanVer;
	char magazine[16] = {'\0'};

	int opt= 0;
	int mag = 0, slt = 0, vlanls = 0, glclk = 0, vlantable=0, networktable = 0;

	//Specifying the options to parse for CS Protocol test tool
	static struct option long_options[] = {
			{"magazine",      required_argument,       0,  'm' },
			{"slot", required_argument,       0,  's' },
			{"vlist",   no_argument, 0,  'l' },
			{"glclock",   no_argument, 0,  'g' },
			{"vtablelist",   required_argument, 0,  't' },
			{"networktable",   required_argument, 0,  'n' },
			{0,           0,                 0,  0   }
	};

	int long_index =0;
	while ((opt = getopt_long(argc, argv,"m:s:lt:gn:",
			long_options, &long_index )) != -1) {
		switch (opt) {
		case 'm' : 
			mag = 1;
			memcpy(magazine,optarg,16);
			break;
		case 's' : 
			slt = 1;
			slot = atoi(optarg);
			break;
		case 'l' : 
			vlanls = 1; 
			break;
		case 't' : 
			vlantable = 1; 
			vlanVer = atoi(optarg);
			break;
		case 'n' :
			networktable = 1;
			vlanVer = atoi(optarg);
			break;
		case 'g' : 
			glclk = 1; 
			break;
		default:  
			break;
		}
	}

	ACS_CS_Registry::getNodeArchitecture(architecture);
	if ( (mag == 0 && slt == 1) || (mag == 1 && slt == 0)) {
		usage(argv[0]);
		return 1;
	}
	else if (mag == 1 && slt == 1 && networktable == 0){
		testVlanTable(magazine, slot);
		return 0;
	}
	else if (vlanls == 1){
		getVLANList();
		return 0;
	}
	else if (glclk == 1){
		testGclk();
		return 0;
	}
	else if (vlantable == 1){
		testGetVlanTable(vlanVer);
		return 0;
	}
	else if (mag == 1 && slt == 1 && networktable == 1){
		testGeNetworkTable(vlanVer, magazine, slot);
		return 0;
	}
	else {
		usage(argv[0]);
		return 1;
	}
#elif MODDCHANGE
	char mac[16] = {0};
	char bootFile[256] = {0};
	int timeout = 0;

	if (argc == 4)
	{
		memcpy(mac, argv[1],16);
		memcpy(bootFile, argv[2],256);
		timeout = atoi(argv[3]);
	}
	else if (argc == 3)
	{
		memcpy(mac, argv[1],16);
		memcpy(bootFile, argv[2],256);
	}
	else
	{
		std::cout << "Usage: " << argv[0] << " [MAC Address] [boot FileName] [timeout = 0]" << std::endl;
		return 1;
	}

	testMODDChange(mac, bootFile, timeout);
#elif MODDREVERT
	char mac[16] = {0};

	if (argc == 2)
	{
		memcpy(mac, argv[1],16);
	}
	else
	{
		std::cout << "Usage: " << argv[0] << " [MAC Address]" << std::endl;
		return 1;
	}

	testMODDRevert(mac);
#elif GETMAUTYPE
	unsigned short cpid = 0;
	unsigned int version = 0;
	if (argc == 2)
	{
		cpid= atoi(argv[1]);

	}
	else if (argc == 3)
	{VLAN
		cpid= atoi(argv[1]);
		version = atoi(argv[2]);

	}
	else
	{
		std::cout << "Usage: " << argv[0] << " CPID" << std::endl;
		return 1;
	}
	(version)? testGetMauType(cpid, version): testGetMauType(cpid);
#elif GETUUID
	testHwcTable();
#endif

	return 0;
}

int sendData() {

	char getRequest[] = "GET /index.htm HTTP/1.1\r\n\r\n";

	cout << getRequest << endl;

	ACE_INET_Addr srvr ("127.0.0.1:80");
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;

	if (-1 == connector.connect (peer, srvr)) {
		cout << "Could not connect" << endl;
		ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "connect"), 1);
	}

	int bc;
	char buf[64];

	peer.send_n (getRequest, strlen(getRequest));
	bc = peer.recv (buf, sizeof(buf));
	write (1, buf, bc);
	peer.close ();

	return 0;
}

void testParser()
{

	int size = 22;
	char buffer[size];
	getPduExample1(buffer, size);

	ACS_CS_Parser parser(ACS_CS_Protocol::Protocol_CS);
	parser.newData(buffer, size);

	try {
		parser.parse();
	} catch (ACS_CS_Exception e) {
		cout << "Exception caught: " << e.description() << endl;
	}

	ACS_CS_PDU pdu;

	if (parser.getPDU(pdu)) {
		//cout << "PDU parsed" << endl;
		printPduData(pdu);
	} else {
		cout << "No PDU" << endl;
	}
}

void parse(char* buffer, int size)
{
	ACS_CS_Parser parser(ACS_CS_Protocol::Protocol_CS);
	parser.newData(buffer, size);

	try {
		parser.parse();
	} catch (ACS_CS_Exception e) {
		cout << "Exception caught: " << e.description() << endl;
	}

	ACS_CS_PDU pdu;

	if (parser.getPDU(pdu)) {
		//cout << "PDU parsed" << endl;
		printPduData(pdu);
	} else {
		cout << "No PDU" << endl;
	}
}

void parse(char* buffer, int size, ACS_CS_PDU& pdu, bool isModd)
{
	ACS_CS_Parser* parser=0;
	if (isModd)
	{
		parser = new ACS_CS_Parser(ACS_CS_Protocol::Protocol_MODD);
		parser->newData(buffer, size);
	}
	else
	{
		parser = new ACS_CS_Parser(ACS_CS_Protocol::Protocol_CS);
		parser->newData(buffer, size);
	}

	try {
		parser->parse();
	} catch (ACS_CS_Exception e) {
		cout << "Exception caught: " << e.description() << endl;
	}


	if (parser->getPDU(pdu)) {
		//cout << "PDU parsed" << endl;
		//printPduData(pdu); 
	} else {
		cout << "No PDU" << endl;
	}

	delete parser;
}

void printPduData(ACS_CS_PDU &pdu) {

	cout << "\n########################## PDU RECEIVED ###########################" << std::endl;
	cout << "Header" << endl;
	cout << "###################################################################" << std::endl;
	cout << "Version: " << pdu.getHeader()->getVersion() << endl;
	cout << "Length: " << pdu.getLength() << endl;
	cout << "Scope: " << pdu.getHeader()->getScope() << endl;
	cout << "Primitive id: " << pdu.getHeader()->getPrimitiveId() << endl;
	//	cout << "Request id: " << pdu.getHeader()->getRequestId() << endl;
	cout << "###################################################################" << std::endl;
	cout << "Body" << endl;
	cout << "###################################################################" << std::endl;

	int bodyLength = pdu.getPrimitive()->getLength();
	char * body;

	if (bodyLength > 0) {
		body = new char[bodyLength];
		pdu.getPrimitive()->getBuffer(body, bodyLength);
	}

	for (int i = 0; i < bodyLength; i++) {

		cout << i << " " << ACS_CS_Protocol::CS_ProtocolChecker::binToString(body + i, 1) << endl;
	}


	if(pdu.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANAddressResponse)
	{
		cout << "###################################################################" << std::endl;
		const ACS_CS_VLAN_GetVLANAddressResponse* response = dynamic_cast<const ACS_CS_VLAN_GetVLANAddressResponse*>(pdu.getPrimitive());

		if (response)
		{
			char str[INET_ADDRSTRLEN];
			int addr = htonl(response->getAddress());

			std::string netAddress = inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
			cout << "Primitive type: GetVLANAddressResponse" << std::endl;
			cout << "Vlan  Address: " << netAddress << std::endl;
		}
	}
	cout << "########################### END OF PDU ############################\n\n" << std::endl;
}

void getPduExample1(char * buffer, int size) {

	if (size != 22)
		return;

	// Header
	buffer[0] = 0x0;
	buffer[1] = 0x2;
	buffer[2] = 0x0;
	buffer[3] = 0x16;
	buffer[4] = 0x0;
	buffer[5] = 0x1;
	buffer[6] = 0x0;
	buffer[7] = 0xC;
	buffer[8] = 0x12;
	buffer[9] = 0x34;
	buffer[10] = 0x0;
	buffer[11] = 0x0;

	// Body
	buffer[12] = 0x0;
	buffer[13] = 0x1;
	buffer[14] = 0x0;
	buffer[15] = 0x8;
	buffer[16] = 0x0;
	buffer[17] = 0x4;
	buffer[18] = 0xC0;
	buffer[19] = 0xA8;
	buffer[20] = 0xA9;
	buffer[21] = 0x3C;
}

void testHwcTable()
{

	std::cout << "----------------------------------" << std::endl;
	std::cout << "DBG: " << __FUNCTION__ << std::endl;
	unsigned short systemNum = 1;

	ACS_CS_Attribute* newAttribute = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_SysNo);

	newAttribute->setValue(reinterpret_cast<const char *>(&systemNum), sizeof(systemNum));

	const ACS_CS_Attribute **attArray = new const ACS_CS_Attribute *[1];

	attArray[0] = newAttribute;

	ACS_CS_HWC_GetBoardList* pduGetBoardList = new ACS_CS_HWC_GetBoardList();

	//pduGetBoardList->setAttributeArray(attArray, 1);

	unsigned int pduSize = ACS_CS_Protocol::CS_Header_Size + pduGetBoardList->getLength(); //set total PDU size

	ACS_CS_Header* pduHeader = new ACS_CS_Header();

	//set header values
	pduHeader->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
	pduHeader->setTotalLength(pduSize);
	pduHeader->setScope(ACS_CS_Protocol::Scope_HWCTable);
	pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetBoardList);
	pduHeader->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);


	ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader, pduGetBoardList);
	ACS_CS_PDU response;

	if (sendRecvData(pdu, response) != 0)
		return;

	if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetBoardListResponse)
	{
		const ACS_CS_HWC_GetBoardListResponse* list = dynamic_cast<const ACS_CS_HWC_GetBoardListResponse*>(response.getPrimitive());

		if (list)
		{
			int length = list->getResponseCount();

			std::cout << "list length = " << length << std::endl;

			for (int it = 0; it < length; it++)
			{
				int boardId = list->getResponse(it);
				uint16_t slot = 0;
				uint32_t magazine = 0;
				char shelfAddr[INET_ADDRSTRLEN];
				char * uuid = 0;

				char* buf = 0;

				ACS_CS_Protocol::CS_Result_Code res = getValue(boardId, ACS_CS_Protocol::Attribute_Slot, buf);


				if ( res == ACS_CS_Protocol::Result_Success)
				{
					slot = static_cast<uint16_t>(*buf);
					delete(buf);
				}


				int mag_size = 0;
				res = getValue(boardId, ACS_CS_Protocol::Attribute_Magazine, buf, mag_size);

				if ( res == ACS_CS_Protocol::Result_Success)
				{



					//magazine = static_cast<uint32_t>(*buf);
					inet_ntop(AF_INET, buf, shelfAddr, sizeof(shelfAddr));

					delete(buf);
					//std::cout << "Slot = " << slot << "\t Magazine = " << tmpSide << std::endl;
				}

				int uuid_size = 0;
				res = getValue(boardId, ACS_CS_Protocol::Attribute_Blade_Uuid, buf, uuid_size);

				if ( res == ACS_CS_Protocol::Result_Success)
				{
					uuid = new char[uuid_size + 1];
					strncpy(uuid,buf,uuid_size);
					delete(buf);
					//std::cout << "Slot = " << slot << "\t Magazine = " << tmpSide << std::endl;
				}

				std::cout << "BoardId = " << boardId << ", Mag = " << shelfAddr << " (size = " << mag_size << ")"
						<< ", Slot = " << slot << ", UUID = " << uuid << " (size = " << uuid_size << ")" << std::endl;

				delete(uuid);


				/*
			  ACS_CS_HWC_GetValue* pduGetValue =  new ACS_CS_HWC_GetValue();

			  pduGetValue->setAttributeId(ACS_CS_Protocol::Attribute_Side);
			  pduGetValue->setBoardId(boardId);

			  unsigned int pduSizeVal = ACS_CS_Protocol::CS_Header_Size + pduGetValue->getLength(); //set total PDU size

			  ACS_CS_Header* pduHeaderVal = new ACS_CS_Header();

			  //set header values
			  pduHeaderVal->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
			  pduHeaderVal->setTotalLength(pduSizeVal);
			  pduHeaderVal->setScope(ACS_CS_Protocol::Scope_HWCTable);
			  pduHeaderVal->setPrimitive(ACS_CS_Protocol::Primitive_GetValue);
			  pduHeaderVal->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);


			  ACS_CS_PDU *pduVal = new ACS_CS_PDU(pduHeaderVal, pduGetValue);
			  ACS_CS_PDU responseVal;

			  if (sendRecvData(pduVal, responseVal) != 0)
			  {
				  std::cout << "Receive Error!!" << std::endl;
				  return;
			  }

			  if(responseVal.getHeader() &&
					  responseVal.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetValueResponse)
			   {
				  const ACS_CS_HWC_GetValueResponse* value = dynamic_cast<const ACS_CS_HWC_GetValueResponse*>(responseVal.getPrimitive());

				  if (value)
				  {
					  if (value->getResultCode() == ACS_CS_Protocol::Result_No_Value)
					  {
						  std::cout << "board ID = " << boardId << "\t Side = NO_VALUE" << std::endl;
					  }
					  else if (value->getResultCode() == ACS_CS_Protocol::Result_Success)
					  {
						  int size = value->getResponseLength();

						  char *buf = new char[size +1];
						  memset(buf,0,size+1);

						  if (value->getResponse(buf, size) == 0)
						  {
							  uint16_t tmpSide = static_cast<uint16_t>(*buf);
							  std::cout << "board ID = " << boardId << "\t Side = " << tmpSide << std::endl;
						  }
						  delete(buf);
					  }
				  }
			   }

			  delete(pduGetValue);
				 */

			}
			//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ List Size: " << list->getResponseCount() << std::endl;
			//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Returned Board Id: " << boardId << std::endl;
		}
	}
	// SEND

}

ACS_CS_Protocol::CS_Result_Code getValue(int boardId, ACS_CS_Protocol::CS_Attribute_Identifier attrId, char* &buf)
{
	ACS_CS_Protocol::CS_Result_Code result = ACS_CS_Protocol::Result_Other_Failure;

	ACS_CS_HWC_GetValue* pduGetValue =  new ACS_CS_HWC_GetValue();

	pduGetValue->setAttributeId(attrId);
	pduGetValue->setBoardId(boardId);

	unsigned int pduSizeVal = ACS_CS_Protocol::CS_Header_Size + pduGetValue->getLength(); //set total PDU size

	ACS_CS_Header* pduHeaderVal = new ACS_CS_Header();

	//set header values
	pduHeaderVal->setVersion(ACS_CS_Protocol::Version_3); //fills the header with data ###_2
	pduHeaderVal->setTotalLength(pduSizeVal);
	pduHeaderVal->setScope(ACS_CS_Protocol::Scope_HWCTable);
	pduHeaderVal->setPrimitive(ACS_CS_Protocol::Primitive_GetValue);
	pduHeaderVal->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);


	ACS_CS_PDU *pduVal = new ACS_CS_PDU(pduHeaderVal, pduGetValue);
	ACS_CS_PDU responseVal;

	if (sendRecvData(pduVal, responseVal) != 0)
	{
		std::cout << "Receive Error!!" << std::endl;
		return result;
	}

	if(responseVal.getHeader() &&
			responseVal.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetValueResponse)
	{
		const ACS_CS_HWC_GetValueResponse* value = dynamic_cast<const ACS_CS_HWC_GetValueResponse*>(responseVal.getPrimitive());

		if (value)
		{
			result = value->getResultCode();

			if (result == ACS_CS_Protocol::Result_Success)
			{
				int size = value->getResponseLength();

				buf = new char[size +1];
				memset(buf,0,size+1);
				value->getResponse(buf, size);

			}
		}
	}

	delete(pduGetValue);

	return result;
}

ACS_CS_Protocol::CS_Result_Code getValue(int boardId, ACS_CS_Protocol::CS_Attribute_Identifier attrId, char* &buf, int &buf_size)
{
	ACS_CS_Protocol::CS_Result_Code result = ACS_CS_Protocol::Result_Other_Failure;

	ACS_CS_HWC_GetValue* pduGetValue =  new ACS_CS_HWC_GetValue();

	pduGetValue->setAttributeId(attrId);
	pduGetValue->setBoardId(boardId);

	unsigned int pduSizeVal = ACS_CS_Protocol::CS_Header_Size + pduGetValue->getLength(); //set total PDU size

	ACS_CS_Header* pduHeaderVal = new ACS_CS_Header();

	//set header values
	pduHeaderVal->setVersion(ACS_CS_Protocol::Version_3); //fills the header with data ###_2
	pduHeaderVal->setTotalLength(pduSizeVal);
	pduHeaderVal->setScope(ACS_CS_Protocol::Scope_HWCTable);
	pduHeaderVal->setPrimitive(ACS_CS_Protocol::Primitive_GetValue);
	pduHeaderVal->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);


	ACS_CS_PDU *pduVal = new ACS_CS_PDU(pduHeaderVal, pduGetValue);
	ACS_CS_PDU responseVal;

	if (sendRecvData(pduVal, responseVal) != 0)
	{
		std::cout << "Receive Error!!" << std::endl;
		return result;
	}

	if(responseVal.getHeader() &&
			responseVal.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetValueResponse)
	{
		const ACS_CS_HWC_GetValueResponse* value = dynamic_cast<const ACS_CS_HWC_GetValueResponse*>(responseVal.getPrimitive());

		if (value)
		{
			result = value->getResultCode();

			if (result == ACS_CS_Protocol::Result_Success)
			{
				int size = value->getResponseLength();

				buf_size = size;
				buf = new char[size +1];
				memset(buf,0,size+1);
				value->getResponse(buf, size);

			}
		}
	}

	delete(pduGetValue);

	return result;
}

void testVlanTable(std::string mag_input, uint16_t slot_input)
{

	//	uint16_t slot_input = 3;
	//	std::string mag_input = "1.2.0.4";
	int boardId = 0;
	uint32_t uint_mag = 0;
	struct in_addr convaddr;
	memset(&convaddr, 0, sizeof convaddr);
	inet_pton(AF_INET,mag_input.c_str(),&convaddr);
	uint_mag = convaddr.s_addr;

	//uint_mag = htonl(uint_mag);
	//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Converted Magazine: " << uint_mag << std::endl;

	ACS_CS_Attribute* magAttribute = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Magazine);
	ACS_CS_Attribute* slotAttribute = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Slot);
	uint32_t magazine = uint_mag;
	uint16_t slot = slot_input;

	magAttribute->setValue(reinterpret_cast<const char *>(&magazine), sizeof(magazine));
	slotAttribute->setValue(reinterpret_cast<const char *>(&slot), sizeof(slot));

	const ACS_CS_Attribute **attArray = new const ACS_CS_Attribute *[2];

	attArray[0] = magAttribute;
	attArray[1] = slotAttribute;

	{
		ACS_CS_HWC_GetBoardList* pduGetBoardList = new ACS_CS_HWC_GetBoardList();

		pduGetBoardList->setAttributeArray(attArray, 2);

		unsigned int pduSize = ACS_CS_Protocol::CS_Header_Size + pduGetBoardList->getLength(); //set total PDU size

		ACS_CS_Header* pduHeader = new ACS_CS_Header();

		//set header values
		pduHeader->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
		pduHeader->setTotalLength(pduSize);
		pduHeader->setScope(ACS_CS_Protocol::Scope_HWCTable);
		pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetBoardList);
		pduHeader->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);


		ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader, pduGetBoardList);
		ACS_CS_PDU response;
		if (sendRecvData(pdu, response) != 0)
			return;

		if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetBoardListResponse)
		{
			const ACS_CS_HWC_GetBoardListResponse* list = dynamic_cast<const ACS_CS_HWC_GetBoardListResponse*>(response.getPrimitive());

			if (list)
			{
				boardId = list->getResponse(0);
				//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ List Size: " << list->getResponseCount() << std::endl;
			}
		}
	}

	//Get VLAN List
	ACS_CS_VLAN_GetVLANList* pduGetVlanList = new ACS_CS_VLAN_GetVLANList();
	unsigned int pduSize = ACS_CS_Protocol::CS_Header_Size + pduGetVlanList->getLength(); //set total PDU size
	//std::cout<<"pduGetVlanList->getLength() = "<<pduGetVlanList->getLength()<<std::endl;

	ACS_CS_Header* pduHeader = new ACS_CS_Header();

	//set header values
	pduHeader->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
	pduHeader->setTotalLength(pduSize);
	pduHeader->setScope(ACS_CS_Protocol::Scope_VLAN);
	pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetVLANList);
	pduHeader->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);

	ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader, pduGetVlanList);

	ACS_CS_PDU response;
	if (sendRecvData(pdu, response) != 0)
		return;
	std::vector<ACS_CS_VLAN_DATA> vlanVector;

	if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANListResponse)
	{
		const ACS_CS_VLAN_GetVLANListResponse* list = dynamic_cast<const ACS_CS_VLAN_GetVLANListResponse*>(response.getPrimitive());

		if (list)
		{
			list->getVLANVector(vlanVector);
		}
	}
	std::set<std::string> vlanSet;

	for(unsigned i = 0; i < vlanVector.size(); i++)
	{
		vlanSet.insert(vlanVector.at(i).vlanName);
	}
	std::cout << "VLAN ADDRESSES FOR BOARD ON MAGAZINE " << mag_input << " SLOT " <<  slot_input << "\n" << std::endl;
	std::cout <<setiosflags(ios::left)<<std::setw(15)<< "NAME";
	std::cout <<setiosflags(ios::left)<<std::setw(19)<<"ADDRESS";
	std::cout<< std::endl;

	std::set<std::string>::iterator it;
	for(it = vlanSet.begin(); it != vlanSet.end(); it++)
	{
		ACS_CS_VLAN_GetVLANAddress* pduGetVlanAddress = new ACS_CS_VLAN_GetVLANAddress();

		pduGetVlanAddress->setBoardId(boardId);
		pduGetVlanAddress->setVLANName(*it);

		//std::cout << "Getting Address for board on Shelf " << mag_input << " Slot " << slot_input <<  " on Vlan: " << *it << std::endl ;

		unsigned int pduSize = ACS_CS_Protocol::CS_Header_Size + pduGetVlanAddress->getLength(); //set total PDU size

		ACS_CS_Header* pduHeader = new ACS_CS_Header();

		//set header values
		pduHeader->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
		pduHeader->setTotalLength(pduSize);
		pduHeader->setScope(ACS_CS_Protocol::Scope_VLAN);
		pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetVLANAddress);
		pduHeader->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);

		ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader, pduGetVlanAddress);
		ACS_CS_PDU response;

		if (sendRecvData(pdu, response) != 0)
			return;

		if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANAddressResponse)
		{
			const ACS_CS_VLAN_GetVLANAddressResponse* vlanAddr = dynamic_cast<const ACS_CS_VLAN_GetVLANAddressResponse*>(response.getPrimitive());

			if (vlanAddr)
			{
				char str[INET_ADDRSTRLEN];
				int addr = htonl(vlanAddr->getAddress());

				std::string netAddress = inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
				//					cout << "Primitive type: GetVLANAddressResponse" << std::endl;
				//					cout << "Vlan  Address: " << netAddress << std::endl;

				size_t dotLastPos = netAddress.find_last_of(".");
				std::string ipAddressLastByte = netAddress.substr(dotLastPos, std::string::npos);


				if (ipAddressLastByte.compare(".0") == 0)
					netAddress = "-";

				std::cout <<setiosflags(ios::left)<<std::setw(15)<< *it;
				std::cout <<setiosflags(ios::left)<<std::setw(19)<< netAddress;
				std::cout<< std::endl;


			}
		}
	}
	std::cout<< std::endl;


}

void testMODDChange(std::string macString, std::string boot, int timeout)
{
	ACS_CS_MODD_Change* change = new ACS_CS_MODD_Change();

	char mac[6] = {0};//{'0x00','0x15','0x60','0x04', '0x33', '0xA8'};

	uint16_t tm = (uint16_t) timeout;

	//CORRECT MAC
	//std::string macString("0015600433A8");

	//FAKE MAC
	//std::string macString("0015600433F8");

	getRealMac(macString, mac, 6);

	//change->setBootImage("/fw/cpsb_upgrade/CXC1060166_");
	//change->setBootImage("/fw/cpsb_upgrade/CXC1060166_R13A.efi");
	change->setBootImage(boot.c_str());
	change->setMacAddress(mac,6);
	change->setTimeout(tm);

	int pduSize = change->getLength(); //set total PDU size

	//cout << "PDU Size = " << pduSize;

	ACS_CS_MODD_Header* pduHeader = new ACS_CS_MODD_Header();

	//set header values
	pduHeader->setVersion(ACS_CS_Protocol::Version_1); //fills the header with data
	pduHeader->setTotalLength(pduSize);
	pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_MODD_Change);


	ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader,change);

	//	pdu->setHeader(pduHeader);
	//	pdu->setPrimitive(change);



	ACS_CS_PDU response;
	if (sendRecvData(pdu, response, true) != 0)
		return;


	if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_MODD_ChangeResponse)
	{
		const ACS_CS_MODD_ChangeResponse* resp = dynamic_cast<const ACS_CS_MODD_ChangeResponse*>(response.getPrimitive());

		if (resp)
		{
			std::string descr = resp->getDescription();
			ACS_CS_Protocol::MODD_Result_Code rc = resp->getResultCode();
			printf("\nResponse Received: Result %d, %s\n", rc, descr.c_str());
		}
	}
}

void testMODDRevert(std::string macString)
{
	ACS_CS_MODD_Revert* revert = new ACS_CS_MODD_Revert();

	char mac[6] = {0};//{'0x00','0x15','0x60','0x04', '0x33', '0xA8'};


	//CORRECT MAC
	//std::string macString("0015600433A8");

	//FAKE MAC
	//std::string macString("0015600433F8");

	getRealMac(macString, mac, 6);

	//change->setBootImage("/fw/cpsb_upgrade/CXC1060166_");
	//change->setBootImage("/fw/cpsb_upgrade/CXC1060166_R13A.efi");
	//change->setBootImage(boot.c_str());
	revert->setMacAddress(mac,6);

	int pduSize = revert->getLength(); //set total PDU size

	//cout << "PDU Size = " << pduSize;

	ACS_CS_MODD_Header* pduHeader = new ACS_CS_MODD_Header();

	//set header values
	pduHeader->setVersion(ACS_CS_Protocol::Version_1); //fills the header with data
	pduHeader->setTotalLength(pduSize);
	pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_MODD_Revert);


	ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader,revert);

	ACS_CS_PDU response;
	if (sendRecvData(pdu, response, true) != 0)
		return;


	if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_MODD_RevertResponse)
	{
		const ACS_CS_MODD_RevertResponse* resp = dynamic_cast<const ACS_CS_MODD_RevertResponse*>(response.getPrimitive());

		if (resp)
		{
			std::string descr = resp->getDescription();
			ACS_CS_Protocol::MODD_Result_Code rc = resp->getResultCode();
			printf("\nResponse Received: Result %d, %s\n", rc, descr.c_str());
		}
	}
}

int sendRecvData(ACS_CS_PDU *pdu) {

	//std::cout << "DBG: " << __FUNCTION__ << std::endl;
	// Connect

	//ACE_INET_Addr srvr_1 ("127.0.0.1:14015");

	ACE_INET_Addr srvr_1 ("192.168.169.1:14015");
	ACE_INET_Addr srvr_2 ("192.168.169.2:14015");
	ACE_INET_Addr srvr_3 ("192.168.169.1:14015");
	ACE_INET_Addr srvr_4 ("192.168.169.2:14015");

	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;

	if (-1 == connector.connect (peer, srvr_1)) {
		if (-1 == connector.connect (peer, srvr_2)) {
			if (-1 == connector.connect (peer, srvr_3)) {
				if (-1 == connector.connect (peer, srvr_4)) {
					cout << "Could not connect to CS Server port 14015" << endl;
					ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "connect"), 1);
					return -1;
				}
			}
		}
	}


	//Send data
	{
		const int pdusize = pdu->getLength();
		char * buffer = new char[pdusize];
		(void) pdu->getBuffer(buffer, pdusize); // Copy pdu data into the buffer

		ssize_t sentBytes = peer.send_n (buffer, pdusize);

		//std::cout << "DBG: sentBytes == " << sentBytes << std::endl;
		delete [] buffer;
	}

	//Receive data
	{
		char buf[65536] = {0};
		ssize_t recvdBytes = peer.recv (buf, sizeof(buf));
		//std::cout << "DBG: recvdBytes == " << recvdBytes << std::endl;
		parse(buf, recvdBytes);
	}


	peer.close ();

	return 0;
}

int sendRecvData(ACS_CS_PDU *pdu, ACS_CS_PDU& response, bool isModd) {

	//std::cout << "DBG: " << __FUNCTION__ << std::endl;
	// Connect
	//	ACE_INET_Addr srvr ("127.0.0.1:14015");
	//	ACE_SOCK_Connector connector;
	//	ACE_SOCK_Stream peer;
	//
	//	if (-1 == connector.connect (peer, srvr)) {
	//		cout << "Could not connect to 127.0.0.1:14015" << endl;
	//		ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "connect"), 1);
	//                return -1;
	//	}
	std::string port;

	if (isModd)
		port = "6798";
	else
		port = "14015";

	char srv_name1[32] = {0};
	char srv_name2[32] = {0};
	char srv_name3[32] = {0};
	char srv_name4[32] = {0};

	sprintf(srv_name1,"192.168.169.1:%s",port.c_str());
	sprintf(srv_name2,"192.168.169.2:%s",port.c_str());
	sprintf(srv_name3,"192.168.170.1:%s",port.c_str());
	sprintf(srv_name4,"192.168.170.2:%s",port.c_str());


	ACE_INET_Addr srvr_1 (srv_name1);
	ACE_INET_Addr srvr_2 (srv_name2);
	ACE_INET_Addr srvr_3 (srv_name3);
	ACE_INET_Addr srvr_4 (srv_name4);

	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;

	if (-1 == connector.connect (peer, srvr_1)) {
		if (-1 == connector.connect (peer, srvr_2)) {
			if (-1 == connector.connect (peer, srvr_3)) {
				if (-1 == connector.connect (peer, srvr_4)) {
					cout << "Could not connect to CS Server port "<< port << endl;
					ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "connect"), 1);
					return -1;
				}
			}
		}
	}

	//Send data
	{
		int pdusize;

		pdusize = pdu->getLength();

		char * buffer = new char[pdusize];
		(void) pdu->getBuffer(buffer, pdusize); // Copy pdu data into the buffer

		ssize_t sentBytes = peer.send_n (buffer, pdusize);

		//std::cout << "DBG: sentBytes == " << sentBytes << std::endl;
		delete [] buffer;
	}

	//Receive data
	{
		char buf[65536] = {0};
		ssize_t recvdBytes = peer.recv (buf, sizeof(buf));
		//std::cout << "DBG: recvdBytes == " << recvdBytes << std::endl;
		parse(buf, recvdBytes, response, isModd);
	}


	peer.close ();

	return 0;
}


int normalize(char r)
{

	switch(r)
	{
	case 'F':
	case 'f':
		return 15;
	case 'E':
	case 'e':
		return 14;
	case 'D':
	case 'd':
		return 13;
	case 'C':
	case 'c':
		return 12;
	case 'B':
	case 'b':
		return 11;
	case 'A':
	case 'a':
		return 10;
	default:
		return r-48;
	}


}

void hexCharToBinChar(unsigned char *input,char *out, int size)
{

	unsigned char *output=(unsigned char*)malloc(size);
	memset(output,0,size);

	size_t t=0;
	int count=0;
	unsigned int ris=0;
	int index=0;


	for(t=0;t<strlen((char*)input);t++)
	{

		if (count == 0) {
			ris += 16 * normalize(input[t]);
			count++;
		} else {
			ris += (unsigned int) normalize(input[t]);
			output[index++] = ris;
			//printf("ris=%d\n", ris);
			count = 0;
			ris = 0;
		}
	}

	for(int t=0;t<index;t++)
	{
		out[t]=output[t];
	}

	free(output);

}


int getRealMac(std::string ss,char *out, int size)
{

	char * str=new char[size*2];
	memset(str, 0, size*2);

	sprintf(str, "%s", const_cast<char*> (ss.c_str()));

	unsigned char *a = new unsigned char[size*2];
	memset(a, 0, size*2);

	for (int t = 0; t < size*2; t++) {
		a[t] = (unsigned char) (str[t]);
	}

	hexCharToBinChar(a, out, size);

	delete [] str;
	delete [] a;

	return 0;
}


void testGetMauType(unsigned short cpid, int version)
{
	ACS_CS_CPId_GetMAUType* pduGetMauType =  new ACS_CS_CPId_GetMAUType();
	unsigned short mauType;

	pduGetMauType->setCPId(cpid);

	unsigned int pduSizeVal = ACS_CS_Protocol::CS_Header_Size + pduGetMauType->getLength(); //set total PDU size

	ACS_CS_Header* pduHeaderVal = new ACS_CS_Header();
	//set header values
	pduHeaderVal->setVersion((ACS_CS_Protocol::CS_Version_Identifier) version); //fills the header with data
	pduHeaderVal->setTotalLength(pduSizeVal);
	pduHeaderVal->setScope(ACS_CS_Protocol::Scope_CPIdTable);
	pduHeaderVal->setPrimitive(ACS_CS_Protocol::Primitive_GetMAUType);
	pduHeaderVal->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);

	ACS_CS_PDU *pduVal = new ACS_CS_PDU(pduHeaderVal, pduGetMauType);
	ACS_CS_PDU responseVal;

	if (sendRecvData(pduVal, responseVal) != 0)
	{
		std::cout << "Receive Error!!" << std::endl;
		return;
	}

	if(responseVal.getHeader() &&
			responseVal.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetMAUTypeResponse)
	{
		std::cout << "Response Header Version " << responseVal.getHeader()->getVersion() << std::endl;

		const ACS_CS_CPId_GetMAUTypeResponse* value = dynamic_cast<const ACS_CS_CPId_GetMAUTypeResponse*>(responseVal.getPrimitive());
		if (value)
		{
			if (value->getResultCode() == ACS_CS_Protocol::Result_No_Such_Entry)
			{
				std::cout << "There is no entry for CPID " << cpid <<  std::endl;
			}
			else if (value->getResultCode() == ACS_CS_Protocol::Result_Success)
			{
				mauType = value->getMAUType();
				// unsigned short mauType = static_cast<unsigned short>(*buf);
				std::string str="";
				if(mauType==1)
					str="MAUB";
				else if(mauType==2)
					str="MAUS";
				std::cout << "MAU Type for CPID = " << cpid << "\t is MAUType = " << str.c_str() << std::endl;

			}
		}
	}

}			  

void getVLANList()
{
	ACS_CS_VLAN_GetVLANList* pduGetVlanList = new ACS_CS_VLAN_GetVLANList();

	unsigned int pduSize = ACS_CS_Protocol::CS_Header_Size + pduGetVlanList->getLength(); //set total PDU size

	ACS_CS_Header* pduHeader = new ACS_CS_Header();

	//set header values
	pduHeader->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
	pduHeader->setTotalLength(pduSize);
	pduHeader->setScope(ACS_CS_Protocol::Scope_VLAN);
	pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetVLANList);
	pduHeader->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);

	ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader, pduGetVlanList);

	ACS_CS_PDU response;
	if (sendRecvData(pdu, response) != 0)
		return;
	std::vector<ACS_CS_VLAN_DATA> vlanVector;

	if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANListResponse)
	{
		const ACS_CS_VLAN_GetVLANListResponse* list = dynamic_cast<const ACS_CS_VLAN_GetVLANListResponse*>(response.getPrimitive());

		if (list)
		{
			list->getVLANVector(vlanVector);
		}
	}

	typedef std::map<std::string, ACS_CS_VLAN_DATA> vlanListMap;
	typedef std::pair<std::string, ACS_CS_VLAN_DATA> vlanListPair;
	vlanListMap vlanlist;
	for(unsigned i = 0; i < vlanVector.size(); i++)
	{
		vlanlist.insert(vlanListPair(vlanVector.at(i).vlanName,vlanVector.at(i)));
	}

	std::cout << "VLAN TABLE\n" << std::endl;
	std::cout <<setiosflags(ios::left)<<std::setw(15)<< "NAME";
	std::cout <<setiosflags(ios::left)<<std::setw(19)<<"ADDRESS";
	std::cout <<setiosflags(ios::left)<<std::setw(19)<<"NETMASK";
	std::cout <<setiosflags(ios::left)<<std::setw(7)<<"STACK";
	std::cout<< std::endl;

	vlanListMap::iterator iter;

	for( iter=vlanlist.begin(); iter!=vlanlist.end(); ++iter)
	{
		char tempValueStr[INET_ADDRSTRLEN]={'\0'};
		uint32_t address = htonl(iter->second.vlanAddress);
		uint32_t netmask = htonl(iter->second.vlanNetmask);

		std::string vlanAddress = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);
		std::string vlanNetmask = inet_ntop(AF_INET, &netmask, tempValueStr, INET_ADDRSTRLEN);

		std::string vlanStack("");

		if (iter->second.vlanStack == ACS_CS_Protocol::Stack_KIP)
		{
			vlanStack = "KIP";
		} else if (iter->second.vlanStack == ACS_CS_Protocol::Stack_TIP)
		{
			vlanStack = "TIP";
		}

		std::cout <<setiosflags(ios::left)<<std::setw(15)<< iter->first;
		std::cout <<setiosflags(ios::left)<<std::setw(19)<< vlanAddress;
		std::cout <<setiosflags(ios::left)<<std::setw(19) << vlanNetmask;
		std::cout <<setiosflags(ios::left)<<std::setw(7)<< vlanStack;
		std::cout << std::endl;

	}
	std::cout << std::endl;
}

void testGclk()
{
	ACS_CS_GLK_GetGlobalLogicalClock* pduGetGclk = new ACS_CS_GLK_GetGlobalLogicalClock();
	unsigned int pduSize = ACS_CS_Protocol::CS_Header_Size + pduGetGclk->getLength(); //set total PDU size
	ACS_CS_Header* pduHeader = new ACS_CS_Header();
	//set header values
	pduHeader->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
	pduHeader->setTotalLength(pduSize);
	pduHeader->setScope(ACS_CS_Protocol::Scope_GlobalLogicalClock);
	pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetGlobalLogicalClock);
	pduHeader->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);
	ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader, pduGetGclk);
	ACS_CS_PDU response;
	int gclk;
	if (sendRecvData(pdu, response) != 0)
		return;


	std::vector<ACS_CS_GLC_DATA> glcVector;
	if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetGlobalLogicalClockResponse)
	{
		const  ACS_CS_GLK_GetGlobalLogicalClockResponse* gclkRes =dynamic_cast< const ACS_CS_GLK_GetGlobalLogicalClockResponse*>(response.getPrimitive());
		ACS_CS_GLK_GetGlobalLogicalClockResponse* cgclkRes=const_cast<ACS_CS_GLK_GetGlobalLogicalClockResponse*>(gclkRes);
		cgclkRes->getGLCVector(glcVector);

	}


	for(unsigned i = 0; i < glcVector.size(); i++)
	{
		std::cout << "attrScopeIdentifier : " <<glcVector.at(i).attrScopeIdentifier;
		std::cout << "       attrLogicalClock : " <<glcVector.at(i).attrLogicalClock<<endl;
	}


	std::cout<< std::endl;
}

void testGetVlanTable(int ver)
{

	ACS_CS_VLAN_GetVLANTable* pduGetVlanTable = new ACS_CS_VLAN_GetVLANTable();

	pduGetVlanTable->setVlanTableVersion(ver);
	//ACS_CS_VLAN_GetVLANTableResponse::setTableVersion(ver);

	unsigned int pduSizeq = ACS_CS_Protocol::CS_Header_Size + pduGetVlanTable->getLength(); //set total PDU size

	ACS_CS_Header* pduHeaderq = new ACS_CS_Header();

	//set header values
	pduHeaderq->setVersion(ACS_CS_Protocol::Version_3); //fills the header with data
	pduHeaderq->setTotalLength(pduSizeq);
	pduHeaderq->setScope(ACS_CS_Protocol::Scope_VLAN);
	pduHeaderq->setPrimitive(ACS_CS_Protocol::Primitive_GetVLANTable);
	pduHeaderq->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);

	ACS_CS_PDU *pduq = new ACS_CS_PDU(pduHeaderq, pduGetVlanTable);

	ACS_CS_PDU responseq;
	if (sendRecvData(pduq, responseq) != 0)
	{
		std::cout<<"sendRecvData is non zero, so returning"<<std::endl;
		return;
	}
	std::vector<ACS_CS_VLAN_DATA> vlanVector;
	unsigned short myPcp;
	unsigned short myType;

	if(responseq.getHeader() && responseq.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANTableResponse)
	{
		const ACS_CS_VLAN_GetVLANTableResponse* listq = dynamic_cast<const ACS_CS_VLAN_GetVLANTableResponse*>(responseq.getPrimitive());

		if (listq)
		{
			listq->getVLANVector(vlanVector);
		}
	}

	typedef std::map<std::string, ACS_CS_VLAN_DATA> vlanListMap;
	typedef std::pair<std::string, ACS_CS_VLAN_DATA> vlanListPair;
	vlanListMap vlanlist;
	for(unsigned i = 0; i < vlanVector.size(); i++)
	{
		vlanlist.insert(vlanListPair(vlanVector.at(i).vlanName,vlanVector.at(i)));
	}

	std::cout << "VLAN TABLE\n" << std::endl;
	std::cout <<setiosflags(ios::left)<<std::setw(15)<< "NAME";
	std::cout <<setiosflags(ios::left)<<std::setw(19)<<"ADDRESS";
	std::cout <<setiosflags(ios::left)<<std::setw(19)<<"NETMASK";
	std::cout <<setiosflags(ios::left)<<std::setw(7)<<"STACK";
	if(ver == 1 || ver == 2)
	{
		std::cout <<setiosflags(ios::left)<<std::setw(7)<<"PCP";
		std::cout <<setiosflags(ios::left)<<std::setw(12)<<"VLANTYPE";
	}
	if(ver == 2)
	{
		std::cout <<setiosflags(ios::left)<<std::setw(12)<<"VLANTAG";
		std::cout <<setiosflags(ios::left)<<std::setw(12)<<"vNICNAME";
	}
	std::cout<< std::endl;

	vlanListMap::iterator iter;

	for( iter=vlanlist.begin(); iter!=vlanlist.end(); ++iter)
	{
		char tempValueStr[INET_ADDRSTRLEN]={'\0'};
		uint32_t address = htonl(iter->second.vlanAddress);
		uint32_t netmask = htonl(iter->second.vlanNetmask);

		std::string vlanAddress = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);
		std::string vlanNetmask = inet_ntop(AF_INET, &netmask, tempValueStr, INET_ADDRSTRLEN);

		std::string vlanStack("");

		if (iter->second.vlanStack == ACS_CS_Protocol::Stack_KIP)
		{
			vlanStack = "KIP";
		} else if (iter->second.vlanStack == ACS_CS_Protocol::Stack_TIP)
		{
			vlanStack = "TIP";
		}

		std::cout <<setiosflags(ios::left)<<std::setw(15)<< iter->first;
		std::cout <<setiosflags(ios::left)<<std::setw(19)<< vlanAddress;
		std::cout <<setiosflags(ios::left)<<std::setw(19) << vlanNetmask;
		std::cout <<setiosflags(ios::left)<<std::setw(7)<< vlanStack;
		if(ver == 1 || ver == 2)
		{
			std::cout <<setiosflags(ios::left)<<std::setw(7)<<iter->second.pcp;
			std::cout <<setiosflags(ios::left)<<std::setw(12)<<iter->second.vlanType;
		}
		if(ver == 2) // Version 2 fro vAPG
		{
			std::cout <<setiosflags(ios::left)<<std::setw(12)<<iter->second.vlanTag;
			std::cout <<setiosflags(ios::left)<<std::setw(12)<<iter->second.vNICName;
		}

		std::cout << std::endl;

	}
	std::cout << std::endl;
}

void testGeNetworkTable(int ver, std::string mag_input, uint16_t slot_input)
{

	//	uint16_t slot_input = 3;
	//	std::string mag_input = "1.2.0.4";
	int boardId = 0;
	uint32_t uint_mag = 0;
	struct in_addr convaddr;
	memset(&convaddr, 0, sizeof convaddr);
	inet_pton(AF_INET,mag_input.c_str(),&convaddr);
	uint_mag = convaddr.s_addr;

	//uint_mag = htonl(uint_mag);
	//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Converted Magazine: " << uint_mag << std::endl;

	ACS_CS_Attribute* magAttribute = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Magazine);
	ACS_CS_Attribute* slotAttribute = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Slot);
	uint32_t magazine = uint_mag;
	uint16_t slot = slot_input;

	magAttribute->setValue(reinterpret_cast<const char *>(&magazine), sizeof(magazine));
	slotAttribute->setValue(reinterpret_cast<const char *>(&slot), sizeof(slot));

	const ACS_CS_Attribute **attArray = new const ACS_CS_Attribute *[2];

	attArray[0] = magAttribute;
	attArray[1] = slotAttribute;

	{
		ACS_CS_HWC_GetBoardList* pduGetBoardList = new ACS_CS_HWC_GetBoardList();

		pduGetBoardList->setAttributeArray(attArray, 2);

		unsigned int pduSize = ACS_CS_Protocol::CS_Header_Size + pduGetBoardList->getLength(); //set total PDU size

		ACS_CS_Header* pduHeader = new ACS_CS_Header();

		//set header values
		pduHeader->setVersion(ACS_CS_Protocol::Version_2); //fills the header with data
		pduHeader->setTotalLength(pduSize);
		pduHeader->setScope(ACS_CS_Protocol::Scope_HWCTable);
		pduHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetBoardList);
		pduHeader->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);


		ACS_CS_PDU *pdu = new ACS_CS_PDU(pduHeader, pduGetBoardList);
		ACS_CS_PDU response;
		if (sendRecvData(pdu, response) != 0)
			return;

		if(response.getHeader() && response.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetBoardListResponse)
		{
			const ACS_CS_HWC_GetBoardListResponse* list = dynamic_cast<const ACS_CS_HWC_GetBoardListResponse*>(response.getPrimitive());

			if (list)
			{
				boardId = list->getResponse(0);
				//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ List Size: " << list->getResponseCount() << std::endl;
			}
		}
	}


	ACS_CS_VLAN_GetNetworkTable* pduGetNetworkTable = new ACS_CS_VLAN_GetNetworkTable();

	pduGetNetworkTable->setTableVersion(ver);
	pduGetNetworkTable->setBoardId(boardId);
	//ACS_CS_VLAN_GetVLANTableResponse::setTableVersion(ver);

	unsigned int pduSizeq = ACS_CS_Protocol::CS_Header_Size + pduGetNetworkTable->getLength(); //set total PDU size

	ACS_CS_Header* pduHeaderq = new ACS_CS_Header();

	//set header values
	pduHeaderq->setVersion(ACS_CS_Protocol::Latest_Version); //fills the header with data
	pduHeaderq->setTotalLength(pduSizeq);
	pduHeaderq->setScope(ACS_CS_Protocol::Scope_VLAN);
	pduHeaderq->setPrimitive(ACS_CS_Protocol::Primitive_GetNetworkTable);
	pduHeaderq->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);

	ACS_CS_PDU *pduq = new ACS_CS_PDU(pduHeaderq, pduGetNetworkTable);

	ACS_CS_PDU responseq;
	if (sendRecvData(pduq, responseq) != 0)
	{
		std::cout<<"sendRecvData is non zero, so returning"<<std::endl;
		return;
	}
	std::vector<ACS_CS_VLAN_DATA> vlanVector;
	unsigned short myPcp;
	unsigned short myType;

	if(responseq.getHeader() && responseq.getHeader()->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetNetworkTableResponse)
	{
		const ACS_CS_VLAN_GetNetworkTableResponse* listq = dynamic_cast<const ACS_CS_VLAN_GetNetworkTableResponse*>(responseq.getPrimitive());

		if (listq)
		{
			listq->getVLANVector(vlanVector);
		}
	}

	typedef std::map<std::string, ACS_CS_VLAN_DATA> vlanListMap;
	typedef std::pair<std::string, ACS_CS_VLAN_DATA> vlanListPair;
	vlanListMap vlanlist;
	for(unsigned i = 0; i < vlanVector.size(); i++)
	{
		vlanlist.insert(vlanListPair(vlanVector.at(i).vlanName,vlanVector.at(i)));
	}

	std::cout << "VLAN TABLE\n" << std::endl;
	std::cout <<setiosflags(ios::left)<<std::setw(15)<< "NAME";
	std::cout <<setiosflags(ios::left)<<std::setw(19)<<"ADDRESS";
	std::cout <<setiosflags(ios::left)<<std::setw(19)<<"NETMASK";
	std::cout <<setiosflags(ios::left)<<std::setw(7)<<"STACK";
	if(ver == 1 || ver == 2)
	{
		std::cout <<setiosflags(ios::left)<<std::setw(7)<<"PCP";
		std::cout <<setiosflags(ios::left)<<std::setw(12)<<"VLANTYPE";
	}
	if(ver == 2)
	{
		std::cout <<setiosflags(ios::left)<<std::setw(12)<<"VLANTAG";
		std::cout <<setiosflags(ios::left)<<std::setw(12)<<"vNICNAME";
	}
	std::cout<< std::endl;

	vlanListMap::iterator iter;

	for( iter=vlanlist.begin(); iter!=vlanlist.end(); ++iter)
	{
		char tempValueStr[INET_ADDRSTRLEN]={'\0'};
		uint32_t address = htonl(iter->second.vlanAddress);
		uint32_t netmask = htonl(iter->second.vlanNetmask);

		std::string vlanAddress = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);
		std::string vlanNetmask = inet_ntop(AF_INET, &netmask, tempValueStr, INET_ADDRSTRLEN);

		std::string vlanStack("");

		if (iter->second.vlanStack == ACS_CS_Protocol::Stack_KIP)
		{
			vlanStack = "KIP";
		} else if (iter->second.vlanStack == ACS_CS_Protocol::Stack_TIP)
		{
			vlanStack = "TIP";
		}

		std::cout <<setiosflags(ios::left)<<std::setw(15)<< iter->first;
		std::cout <<setiosflags(ios::left)<<std::setw(19)<< vlanAddress;
		std::cout <<setiosflags(ios::left)<<std::setw(19) << vlanNetmask;
		std::cout <<setiosflags(ios::left)<<std::setw(7)<< vlanStack;
		if(ver == 1 || ver == 2)
		{
			std::cout <<setiosflags(ios::left)<<std::setw(7)<<iter->second.pcp;
			std::cout <<setiosflags(ios::left)<<std::setw(12)<<iter->second.vlanType;
		}
		if(ver == 2) // Version 2 fro vAPG
		{
			std::cout <<setiosflags(ios::left)<<std::setw(12)<<iter->second.vlanTag;
			std::cout <<setiosflags(ios::left)<<std::setw(12)<<iter->second.vNICName;
		}

		std::cout << std::endl;

	}
	std::cout << std::endl;
}
