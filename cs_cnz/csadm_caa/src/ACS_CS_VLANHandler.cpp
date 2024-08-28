//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//

extern "C" {
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
}

#include "ACS_CS_EntryCounter.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_BasicResponse.h"
#include "ACS_CS_Trace.h"

#include "ACS_CS_VLAN_GetVLANList.h"
#include "ACS_CS_VLAN_GetVLANListResponse.h"
#include "ACS_CS_VLAN_GetVLANAddress.h"
#include "ACS_CS_VLAN_GetVLANAddressResponse.h"
#include "ACS_CS_VLAN_GetVLANTable.h"
#include "ACS_CS_VLAN_GetVLANTableResponse.h"
#include "ACS_CS_VLAN_GetNetworkTable.h"
#include "ACS_CS_VLAN_GetNetworkTableResponse.h"

#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Registry.h"

#include "ACS_CS_ImModelSaver.h"

#include <sstream>
#include <vector>


// ACS_CS_VLANHandler
#include "ACS_CS_VLANHandler.h"


ACS_CS_Trace_TDEF(ACS_CS_VLANHandler_TRACE);

using std::string;
using std::stringstream;
using std::vector;


static const uint32_t AP_1_NODE_A_BGCI_IP	= 0x00000013;	// 192.254.15.19
static const uint32_t AP_1_NODE_B_BGCI_IP	= 0x00000014;	// 192.254.15.20

static const uint32_t AP_2_NODE_A_BGCI_IP	= 0x00000015;	// 192.254.15.21
static const uint32_t AP_2_NODE_B_BGCI_IP	= 0x00000016;	// 192.254.15.22



// Class ACS_CS_VLANHandler 

ACS_CS_VLANHandler::ACS_CS_VLANHandler()
: logicalClock(0),
  shelfArchitecture(NODE_SCB)
{

	this->logicalClock++;

	ACS_CS_TableHandler::setGlobalLogicalClock(ACS_CS_Protocol::Scope_VLAN,logicalClock);
	// Check the shelf Architecture
	ACS_CS_Registry::getNodeArchitecture(shelfArchitecture);

}


ACS_CS_VLANHandler::~ACS_CS_VLANHandler()
{

	//	if (this->vlanTable)
	//		delete this->vlanTable;
}


int ACS_CS_VLANHandler::handleRequest (ACS_CS_PDU *pdu)
{

	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::handleRequest()\n"
			"Entering method\n"));

	if (pdu == 0)	// Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleRequest()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}

	int error = 0;

	const ACS_CS_HeaderBase * header = pdu->getHeader(); // Get header from PDU

	if (header)
	{
		// Check which primitive that has been received and call handler function
		if (header->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANAddress)
		{
			error = this->handleGetVLANAddress(pdu);
		}
		else if (header->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANList)
		{
			error = this->handleGetVLANList(pdu);
		}
		else if(header->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetVLANTable)
		{
			error = this->handleGetVLANTable(pdu);
		}
		else if(header->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetNetworkTable)
		{
			error = this->handleGetNetworkTable(pdu);
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
					"(%t) ACS_CS_VLANHandler::handleRequest()\n"
					"Error: Unknown primitive, id = %d\n",
					header->getPrimitiveId()));

			error = -1;
		}
	}

	return error;

}

bool ACS_CS_VLANHandler::loadTable ()
{

	// The VLAN table isn't saved to disk.
	// It is re-created on startup every time.

	return true;

}

int ACS_CS_VLANHandler::handleGetVLANAddress (ACS_CS_PDU *pdu)
{

	// Suppress warning for not freeing header and body
	// which we shouldn't since they are passed to the PDU
	//lint --e{429}

	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::handleGetVLANAddress()\n"
			"Entering method\n"));

	if (pdu == 0)	// Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANAddress()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}

	// Get header and body from PDU
	const ACS_CS_Header * header = 0;

	if (pdu && pdu->getHeader())
		header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

	ACS_CS_Protocol::CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	// Get version from incoming package
	if (header)
	{
		version = header->getVersion();
	}

	const ACS_CS_Primitive * primitive = pdu->getPrimitive();
	const ACS_CS_VLAN_GetVLANAddress * getVLANAddress = 0;

	// Check for valid header and body
	if ( (header == 0) || (primitive == 0) )
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANAddress()\n"
				"Error: Invalid header or primitive, header = %d, primitive = %d\n",
				header,
				primitive));

		return -1;
	}

	// Check primitive type and cast to correct type
	if (primitive->getPrimitiveType() == ACS_CS_Protocol::Primitive_GetVLANAddress)
	{
		getVLANAddress = reinterpret_cast<const ACS_CS_VLAN_GetVLANAddress *> (primitive);

		if (getVLANAddress == 0)
		{
			ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
					"(%t) ACS_CS_VLANHandler::handleGetVLANAddress()\n"
					"Error: Cannot cast primitive\n"));

			return -1;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANAddress()\n"
				"Error: Wrong primitive type, type = %d\n",
				primitive->getPrimitiveType()));

		return -1;
	}

	unsigned short requestId = header->getRequestId();			// Get request id
	int error = 0;

	//create the VLAN IP address
	uint32_t vlanIPAddress = 0;

	//retrieve the network IP address for given VLAN
	//vlanVector::iterator vecIt;
	string vlanNameGiven = getVLANAddress->getVLANName();
	string ipAddressString;
	bool getSuccess = getVLANAddressM(getVLANAddress->getBoardId(), vlanNameGiven, ipAddressString);

	struct in_addr convaddr;
	memset(&convaddr, 0, sizeof convaddr);
	int convertSuccess = inet_pton(AF_INET, ipAddressString.c_str(), &convaddr);
	vlanIPAddress = ntohl(convaddr.s_addr);

	if (!getSuccess)
	{
		error = this->createBasicResponse(pdu,						// Create response
				ACS_CS_Protocol::Primitive_GetVLANAddressResponse,
				requestId,
				ACS_CS_Protocol::Result_No_Such_Entry);

		return error;
	}

	if (convertSuccess <= 0)
	{
		error = this->createBasicResponse(pdu,						// Create response
				ACS_CS_Protocol::Primitive_GetVLANAddressResponse,
				requestId,
				ACS_CS_Protocol::Result_Other_Failure);

		return error;
	}

	//create response header and response primitive
	ACS_CS_Header * responseHeader = new ACS_CS_Header();
	ACS_CS_VLAN_GetVLANAddressResponse * responseBody = new ACS_CS_VLAN_GetVLANAddressResponse();

	if ( (responseHeader == 0) || (responseBody == 0) )
	{
		if (responseHeader)
			delete responseHeader;

		if (responseBody)
			delete responseBody;

		return -1;
	}

	responseHeader->setVersion(version);
	responseHeader->setScope(ACS_CS_Protocol::Scope_VLAN);
	responseHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetVLANAddressResponse);
	responseHeader->setRequestId(requestId);

	responseBody->setLogicalClock(this->logicalClock);
	responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
	responseBody->setAddress(vlanIPAddress);

	responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

	pdu->setHeader(responseHeader);
	pdu->setPrimitive(responseBody);

	return 0;

}

int ACS_CS_VLANHandler::handleGetVLANList (ACS_CS_PDU *pdu)
{

	// Suppress warning for not freeing header and body
	// which we shouldn't since they are passed to the PDU
	//lint --e{429}

	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::handleGetVLANList()\n"
			"Entering method\n"));

	if (pdu == 0)	// Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANList()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}
	//	else if (this->vlanTable == 0)	// Check for valid VLAN table
	//	{
	//		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
	//				"(%t) ACS_CS_VLANHandler::handleGetVLANList()\n"
	//				"Error: No table pointer\n"));
	//
	//		return -1;
	//	}

	vlanVector vlanTable;

	if (!populateVLANTable(vlanTable,ACS_CS_Protocol::VLANVersion_0, Network::CP_DOMAIN))
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANList()\n"
				"Error: No table pointer\n"));

		return -1;
	}


	// Get header and body from PDU
	const ACS_CS_Header * header = 0;

	if (pdu && pdu->getHeader())
		header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

	ACS_CS_Protocol::CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
		// Get version from incoming package
	if (header)
	{
		version = header->getVersion();
	}

	const ACS_CS_Primitive * primitive = pdu->getPrimitive();
	const ACS_CS_VLAN_GetVLANList * getVLANList = 0;

	// Check for valid header and body
	if ( (header == 0) || (primitive == 0) )
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANList()\n"
				"Error: Invalid header or primitive, header = %d, primitive = %d\n",
				header,
				primitive));

		return -1;
	}

	// Check primitive type and cast to correct type
	if (primitive->getPrimitiveType() == ACS_CS_Protocol::Primitive_GetVLANList)
	{
		getVLANList = reinterpret_cast<const ACS_CS_VLAN_GetVLANList *> (primitive);

		if (getVLANList == 0)
		{
			ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
					"(%t) ACS_CS_VLANHandler::handleGetVLANList()\n"
					"Error: Cannot cast primitive\n"));

			return -1;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANList()\n"
				"Error: Wrong primitive type, type = %d\n",
				primitive->getPrimitiveType()));

		return -1;
	}

	unsigned short requestId = header->getRequestId();			// Get request id

	ACS_CS_Header * responseHeader = new ACS_CS_Header();
	ACS_CS_VLAN_GetVLANListResponse * responseBody = new ACS_CS_VLAN_GetVLANListResponse();

	if ( (responseHeader == 0) || (responseBody == 0) )
	{
		if (responseHeader)
			delete responseHeader;

		if (responseBody)
			delete responseBody;

		return -1;
	}

	responseHeader->setVersion(version);
	responseHeader->setScope(ACS_CS_Protocol::Scope_VLAN);
	responseHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetVLANListResponse);
	responseHeader->setRequestId(requestId);

	responseBody->setLogicalClock(this->logicalClock);
	responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
	responseBody->setVLANVector(vlanTable);

	responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

	pdu->setHeader(responseHeader);
	pdu->setPrimitive(responseBody);

	return 0;

}

int ACS_CS_VLANHandler::handleGetVLANTable (ACS_CS_PDU *pdu)
{

	// Suppress warning for not freeing header and body
	// which we shouldn't since they are passed to the PDU
	//lint --e{429}

	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::handleGetVLANTable()\n"
			"Entering method\n"));

	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANTable()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}

	// Get header and body from PDU
	const ACS_CS_Header * header = 0;

	if (pdu && pdu->getHeader())
		header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

	ACS_CS_Protocol::CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	// Get version from incoming package
	if (header)
	{
		version = header->getVersion();
	}

	const ACS_CS_Primitive * primitive = pdu->getPrimitive();
	const ACS_CS_VLAN_GetVLANTable * getVLANTable = 0;

	// Check for valid header and body
	if ( (header == 0) || (primitive == 0) )
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANTable()\n"
				"Error: Invalid header or primitive, header = %d, primitive = %d\n",
				header,
				primitive));

		return -1;
	}

	// Check primitive type and cast to correct type
	if (primitive->getPrimitiveType() == ACS_CS_Protocol::Primitive_GetVLANTable)
	{
		getVLANTable = reinterpret_cast<const ACS_CS_VLAN_GetVLANTable *> (primitive);

		if (getVLANTable == 0)
		{
			ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
					"(%t) ACS_CS_VLANHandler::handleGetVLANTable()\n"
					"Error: Cannot cast primitive\n"));

			return -1;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANTable()\n"
				"Error: Wrong primitive type, type = %d\n",
				primitive->getPrimitiveType()));

		return -1;
	}

	unsigned short requestId = header->getRequestId();                      // Get request id

	//retrieve the version from pdu
	unsigned short vlanTableVer =  getVLANTable->getVlanTableVersion();
	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::handleGetVLANTable()\n"
			"getVlanTableVer = %d\n",
			vlanTableVer));


	vlanVector vlanTable;	
	bool return_val;

	return_val = populateVLANTable(vlanTable, vlanTableVer, Network::CP_DOMAIN);

	if (!return_val)
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetVLANTable()\n"
				"Error: No table pointer\n"));

		return -1;


	}
	ACS_CS_Header * responseHeader = new ACS_CS_Header();

	ACS_CS_VLAN_GetVLANTableResponse * responseBody = new ACS_CS_VLAN_GetVLANTableResponse();

	if ( (responseHeader == 0) || (responseBody == 0) )
	{
		if (responseHeader)
			delete responseHeader;

		if (responseBody)
			delete responseBody;

		return -1;
	}

	responseHeader->setVersion(version);
	responseHeader->setScope(ACS_CS_Protocol::Scope_VLAN);
	responseHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetVLANTableResponse);
	responseHeader->setRequestId(requestId);


	responseBody->setLogicalClock(this->logicalClock);
	responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
	responseBody->setTableVersion(vlanTableVer);

	//GEP2_GEP5 to GEP7 HWSWAP
	responseBody->setCPVersion(version);

	responseBody->setVLANVector(vlanTable);
	responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

	pdu->setHeader(responseHeader);
	pdu->setPrimitive(responseBody);

	return 0;
}

int ACS_CS_VLANHandler::handleGetNetworkTable (ACS_CS_PDU *pdu)
{

	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::handleGetNetworkTable()\n"
			"Entering method\n"));

	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetNetworkTable()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}

	// Get header and body from PDU
	const ACS_CS_Header * header = 0;

	if (pdu && pdu->getHeader())
		header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

	ACS_CS_Protocol::CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	// Get version from incoming package
	if (header)
	{
		version = header->getVersion();
	}

	const ACS_CS_Primitive * primitive = pdu->getPrimitive();
	const ACS_CS_VLAN_GetNetworkTable * networkTablePrimitive = 0;

	// Check for valid header and body
	if ( (header == 0) || (primitive == 0) )
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetNetworkTable()\n"
				"Error: Invalid header or primitive, header = %d, primitive = %d\n",
				header,
				primitive));

		return -1;
	}

	// Check primitive type and cast to correct type
	if (primitive->getPrimitiveType() == ACS_CS_Protocol::Primitive_GetNetworkTable)
	{
		networkTablePrimitive = reinterpret_cast<const ACS_CS_VLAN_GetNetworkTable *> (primitive);

		if (networkTablePrimitive == 0)
		{
			ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
					"(%t) ACS_CS_VLANHandler::handleGetNetworkTable()\n"
					"Error: Cannot cast primitive\n"));

			return -1;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetNetworkTable()\n"
				"Error: Wrong primitive type, type = %d\n",
				primitive->getPrimitiveType()));

		return -1;
	}

	unsigned short requestId = header->getRequestId();                      // Get request id

	//retrieve the version from pdu
	unsigned short tableVer =  networkTablePrimitive->getTableVersion();
	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::handleGetNetworkTable()\n"
			"tableVer = %d\n",
			tableVer));

	unsigned short boardId = networkTablePrimitive->getBoardId();


	vlanVector vlanTable;
	bool return_val;

	return_val = populateVLANTable(vlanTable, tableVer, getNetworkDomain(boardId));

	if (!return_val)
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::handleGetNetworkTable()\n"
				"Error: No table pointer\n"));

		return -1;


	}
	ACS_CS_Header * responseHeader = new ACS_CS_Header();

	ACS_CS_VLAN_GetNetworkTableResponse * responseBody = new ACS_CS_VLAN_GetNetworkTableResponse();

	if ((responseHeader == 0) || (responseBody == 0))
	{
		if (responseHeader)
			delete responseHeader;

		if (responseBody)
			delete responseBody;

		return -1;
	}

	responseHeader->setVersion(version);
	responseHeader->setScope(ACS_CS_Protocol::Scope_VLAN);
	responseHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetNetworkTableResponse);
	responseHeader->setRequestId(requestId);

	responseBody->setLogicalClock(this->logicalClock);
	responseBody->setResultCode(ACS_CS_Protocol::Result_Success);
	responseBody->setTableVersion(tableVer);
	responseBody->setBoardId(boardId);

	//GEP2_GEP5 to GEP7 HWSWAP
	responseBody->setCPVersion(version);

	responseBody->setVLANVector(vlanTable);
	responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );

	pdu->setHeader(responseHeader);
	pdu->setPrimitive(responseBody);

	return 0;
}


bool ACS_CS_VLANHandler::extractIPAddress (const std::string &ipAddressString, uint32_t &ipAddress)
{

	//Create a stringstream object initialized to the IP address string
	stringstream str(ipAddressString);

	//Check the length of the IP address string, should be between 7-15 chars
	if (ipAddressString.length() < 7 || ipAddressString.length() > 15)
		return false;

	//Check the number of dots in the stringstream, should be 3
	str.clear();			// return to the beginning
	(void)str.seekg(0);	//      of the stream
	char c;
	unsigned short numDots = 0;
	unsigned int numFigures = 0;

	do
	{
		c = str.peek();   //store the current character

		if (c == EOF)
		{
			if (numFigures == 0) //not a valid IP address
				return false;

			if (numDots != 3)	   //if not 3 dots, invalid IP address
				return false;
		}
		else if (c == '.')
		{
			if (numFigures == 0) //not a valid IP address
				return false;

			numFigures = 0;
			numDots++;
		}
		else if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
				c == '5' || c == '6' || c == '7' || c == '8' || c == '9')
		{
			numFigures++;

			if (numFigures > 3)  //not a valid IP address
				return false;
		}
		else     //illegal character
			return false;

		(void)str.seekg((int)(str.tellg()) + 1);  //move to next character in the stringstream
	} while (c != EOF);

	if (numDots != 3)	// If not 3 dots, faulty format
		return false;

	//Extract the IP address, store it in an unsigned long variable
	str.clear();		   // return to the beginning
	(void)str.seekg(0);  //      of the stream
	string strAddr = "";
	unsigned short byteNum = 0;
	unsigned long value = 0;

	do {
		c = str.get();

		if (c == '.' || c == EOF)
		{
			byteNum++;
			stringstream converter(strAddr);

			strAddr = "";
			unsigned long plug;
			if (! (converter >> plug)) //check if stringstream is possible to convert to unsigned long
			{
				value = 0;
				return false;
			}
			else
			{
				if (byteNum == 1 || byteNum == 2 || byteNum == 3 || byteNum == 4)
				{
					if (plug > 255)   //byte value should be between 0-255
					{
						value = 0;
						return false;
					}
				}

				value = plug | value;

				if (byteNum != 4)		// if not last byte, shift 1 byte to the left
					value = value << 8;
			}
		}
		else
			strAddr = strAddr + c;
	} while (c != EOF);


	//Set the IP address value to the referenced variable
	ipAddress = value;

	return true;

}

bool ACS_CS_VLANHandler::validateIPAddress (uint32_t ipAddress, uint32_t ipNetmask)
{

	vlanVector::iterator vecIt;

	if ((ipAddress & ipNetmask) != ipAddress)
		return false;

	unsigned long newMinValue = ipAddress;
	unsigned long newMaxValue = ipAddress | (~ipNetmask); //bitwise or between IP address and inverted netmask
	//to get the maximum IP value of this new network

	vlanVector vlanTable;

	if (populateVLANTable(vlanTable,ACS_CS_Protocol::VLANVersion_0))
	{
		for (vecIt = vlanTable.begin(); vecIt != vlanTable.end(); ++vecIt)
		{
			unsigned long oldMinValue = vecIt->vlanAddress;
			unsigned long oldMaxValue = vecIt->vlanAddress | (~vecIt->vlanNetmask); //get the maximum IP value for current network

			//check if new address range is in conflict with any of the previous ones
			if (oldMinValue <= newMinValue && newMinValue <= oldMaxValue)
				return false;
			else if (oldMinValue <= newMaxValue && newMaxValue <= oldMaxValue)
				return false;
			else if (newMinValue <= oldMinValue && oldMinValue <= newMaxValue)
				return false;
			else if (newMinValue <= oldMaxValue && oldMaxValue <= newMaxValue)
				return false;
		}

		return true;
	}

	return false;

}

bool ACS_CS_VLANHandler::validateIPNetmask (uint32_t ipNetmask)
{

	unsigned long tmpUL = 0xFF;

	if ((ipNetmask & tmpUL) != 0)    //bitwise and between netmask and 0x000000FF must be 0,
		return false;                 //otherwise a too big netmask has been used

	char binValueStr[33];				         //size 33, "0xFFFFFFFF" + null character
	memset(binValueStr, 0, 33);
	std::stringstream parser;
	parser << ipNetmask;
	if ( !( parser >> binValueStr) )
	{
		// Some fault
	}

	//(void)ultoa(ipNetmask, binValueStr, 2);	//convert the integer value (binary) to string value and store it
	bool boolZero = false;

	for (int i = 0; i < 33; i++)
	{
		char c = binValueStr[i];

		if (c == '0')
			boolZero = true;

		else if (c == '1')
		{
			if (boolZero)
				return false;	
		}
	}

	return true;

}

int ACS_CS_VLANHandler::createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result)
{

	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"(%t) ACS_CS_VLANHandler::createBasicResponse()\n"
			"Entering method\n"));

	// Suppress warning for not freeing header and body
	// which we shouldn't since they are passed to the PDU
	// and freed there
	//lint --e{429}

	if (pdu == 0)	// Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::createBasicResponse()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}

	const ACS_CS_Header * oldHeader = 0;
	if (pdu && pdu->getHeader())
	    	oldHeader = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

	ACS_CS_Protocol::CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
		// Get version from incoming package
	if (oldHeader)
	{
		version = oldHeader->getVersion();
	}

	ACS_CS_Header * header = new ACS_CS_Header();	// Create header

	if (header == 0)
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::createBasicResponse()\n"
				"Error: Invalid header\n"));

		return -1;
	}

	ACS_CS_BasicResponse * body = 0;

	// Create body
	if (type == ACS_CS_Protocol::Primitive_GetVLANAddressResponse)
		body = new ACS_CS_VLAN_GetVLANAddressResponse();
	else if (type == ACS_CS_Protocol::Primitive_GetVLANListResponse)
		body = new ACS_CS_VLAN_GetVLANListResponse();
	//qos start
	else if(type == ACS_CS_Protocol::Primitive_GetVLANTableResponse)
		body = new ACS_CS_VLAN_GetVLANTableResponse();
	//qos end

	if (body == 0)
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"(%t) ACS_CS_VLANHandler::createBasicResponse()\n"
				"Error: Invalid body\n"));

		delete header;
		return -1;
	}


	// Set all header values
	header->setVersion(version);
	header->setScope(ACS_CS_Protocol::Scope_VLAN);
	header->setPrimitive(type);
	header->setRequestId(requestId);

	// Set resultcode
	if (body)
		body->setResultCode(result);

	// Set length
	header->setTotalLength(header->getHeaderLength() + 2);

	// Update PDU
	pdu->setHeader(header);
	pdu->setPrimitive(body);

	return 0;

}



bool ACS_CS_VLANHandler::getVLANAddressM(BoardID boardId, std::string vlanName, std::string &ipAddress)
{
	bool result = true;
	std::string ipAddressLastByte = "";
	std::string ipAddress3Bytes = "";

	//Get last byte of IPAddress of the board

	uint32_t tempIP = 0;

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	if (!model)
		return false;

	std::set<const ACS_CS_ImBase *> objects;
	model->getObjects(objects, APBLADE_T);
	model->getObjects(objects, CPBLADE_T);
	model->getObjects(objects, OTHERBLADE_T);

	std::set<const ACS_CS_ImBase *>::const_iterator it;

	for (it = objects.begin(); it != objects.end(); it++){
		const ACS_CS_ImBlade * blade = reinterpret_cast<const ACS_CS_ImBlade *>(*it);
		if(blade->entryId == boardId){
			in_addr address;
			if (inet_aton(blade->ipAddressEthA.c_str(), &address) != -1){
				tempIP = htonl(address.s_addr);
				break;
			}
			else{
				delete model;
				return false;
			}
		}
	}

	tempIP = tempIP & 0x000000ff;

	//In DMX environment APUB have reserved IP addresses on BGCI-A and BGCI-B Vlans
	if (shelfArchitecture == NODE_DMX &&
			(vlanName.compare(ACS_CS_NS::VLAN_BGCI_A) == 0 || vlanName.compare(ACS_CS_NS::VLAN_BGCI_B) == 0))
	{
		if (tempIP == 1)
		{
			tempIP = AP_1_NODE_A_BGCI_IP & 0x000000ff;
		}
		else if (tempIP == 2)
		{
			tempIP = AP_1_NODE_B_BGCI_IP & 0x000000ff;
		}
		else if (tempIP == 3)
		{
			tempIP = AP_2_NODE_A_BGCI_IP & 0x000000ff;
		}
		else if (tempIP == 4)
		{
			tempIP = AP_2_NODE_B_BGCI_IP & 0x000000ff;
		}
	}


	std::stringstream ss;
	ss << tempIP;
	ss >> ipAddressLastByte;

	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"ACS_CS_Internal_Table::getVLANAddress()\n"
			"ipAddressLastByte = %d", tempIP));

	ACS_CS_INTERNAL_API::VLAN tmpVLAN;
	std::string vlanAddress;

	//Get 3 first bytes of VLAN address
	vlanVector vlanDataVector;
	vlanVector::iterator itVec;

	//if (getVLANList(vlanDataVector))
	if (populateVLANTable(vlanDataVector, ACS_CS_Protocol::VLANVersion_2, Network::DOMAIN_NR))
	{
		for (itVec = vlanDataVector.begin(); itVec != vlanDataVector.end(); itVec++)  //loop through the VLAN vector
		{
			if (itVec->vlanName == vlanName)
			{

				char str[INET_ADDRSTRLEN];
				uint32_t ipaddr = htonl((itVec->vlanNetmask & itVec->vlanAddress) | tempIP);

				if (inet_ntop(AF_INET, &ipaddr, str, INET_ADDRSTRLEN) != NULL)
				{
					ipAddress = str;

					ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
							"ACS_CS_Internal_Table::getVLANAddress()\n"
							"ipAddress = %s", ipAddress.c_str()));
				}
				else // invalid address
				{
					result = false;
				}



//				uint32_t ipaddr = ntohl(itVec->vlanAddress);
//				inet_ntop(AF_INET, &ipaddr, str, INET_ADDRSTRLEN);
//				vlanAddress = str;
//
//				size_t dotLastPos = vlanAddress.find_last_of(".");
//				ipAddress3Bytes = vlanAddress.substr(0, dotLastPos);
//				std::string vlanAddressToValidate = ipAddress3Bytes + "." + ipAddressLastByte;
//
//				//Validate the address
//				int success = 0;
//				struct in_addr convaddr;
//				memset(&convaddr, 0, sizeof convaddr);
//				success = inet_pton(AF_INET, vlanAddressToValidate.c_str(), &convaddr);
//
//				if (success == 1)
//				{
//					ipAddress = vlanAddressToValidate;
//
//					ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
//							"ACS_CS_Internal_Table::getVLANAddress()\n"
//							"ipAddress3Bytes = %s", ipAddress3Bytes.c_str()));
//
//					ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
//							"ACS_CS_Internal_Table::getVLANAddress()\n"
//							"ipAddress = %s", ipAddress.c_str()));
//				}
//				else // invalid address
//				{
//					result = false;
//				}
				break;
			}
		}
	}
	else
	{
		result = false;
	}

	delete model;
	return result;

}


int ACS_CS_VLANHandler::newTableOperationRequest(ACS_CS_ImModelSubset *subset)
{
	ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
			"ACS_CS_VLANHandler::newTableOperationRequest()\n"
			"Entered newTableOperationRequest call"));
	set<const ACS_CS_ImBase *> obj;
	subset->getObjects(obj, VLAN_T);

	if (obj.size() > 0)
	{
		ACS_CS_TRACE((ACS_CS_VLANHandler_TRACE,
				"ACS_CS_VLANHandler::newTableOperationRequest()\n"
				"Incrementing clock \n"));
		this->logicalClock++;
		ACS_CS_TableHandler::setGlobalLogicalClock(ACS_CS_Protocol::Scope_VLAN,logicalClock);
	}

	return 0;
}

Network::Domain ACS_CS_VLANHandler::getNetworkDomain(BoardID boardId)
{
	ACS_CS_DEBUG(("BoardId = %d", boardId))

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
	Network::Domain domain(Network::DOMAIN_NR);


	if (model)
	{
		const ACS_CS_ImBlade * blade =  model->getBlade(boardId);

		ACS_CS_DEBUG(("BoardId = %d, blade = %d", boardId, blade))

		if (blade)
		{
			switch (blade->systemType)
			{
			case SINGLE_SIDED_CP:
				domain = Network::BC_DOMAIN;
				break;
			case DOUBLE_SIDED_CP:
				domain = Network::CP_DOMAIN;
				break;
			case AP:
				domain = Network::AP_DOMAIN;
				break;
			default:
				break;
			}
		}

		delete model;
	}

	return domain;
}



// Additional Declarations

