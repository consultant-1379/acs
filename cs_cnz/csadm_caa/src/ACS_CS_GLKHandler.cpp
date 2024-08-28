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


#include "ACS_CS_GLK_GetGlobalLogicalClock.h"
#include "ACS_CS_GLK_GetGlobalLogicalClockResponse.h"

#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Registry.h"


#include <sstream>
#include <vector>


// ACS_CS_GLKHandler
#include "ACS_CS_GLKHandler.h"


ACS_CS_Trace_TDEF(ACS_CS_GLKHandler_TRACE);

using std::string;
using std::stringstream;
using std::vector;

// Class ACS_CS_GLKHandler 

ACS_CS_GLKHandler::ACS_CS_GLKHandler()
: logicalClock(0)
{

}


ACS_CS_GLKHandler::~ACS_CS_GLKHandler()
{
}


int ACS_CS_GLKHandler::handleRequest (ACS_CS_PDU *pdu)
{

	ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
			"(%t) ACS_CS_GLKHandler::handleRequest()\n"
			"Entering method\n"));

	if (pdu == 0)	// Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::handleRequest()\n"
				"Error: Invalid PDU\n"));

		return -1;
	}

	int error = 0;

	const ACS_CS_HeaderBase * header = pdu->getHeader(); // Get header from PDU

	if (header)
	{
		if (header->getPrimitiveId() == ACS_CS_Protocol::Primitive_GetGlobalLogicalClock)
		{
			error = this->handleGetGlobalLogicalClock(pdu);
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
					"(%t) ACS_CS_GLKHandler::handleRequest()\n"
					"Error: Unknown primitive, id = %d\n",
					header->getPrimitiveId()));

			error = -1;
		}
	}

	return error;

}


int ACS_CS_GLKHandler::handleGetGlobalLogicalClock(ACS_CS_PDU *pdu)
{
	ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
			"(%t) ACS_CS_GLKHandler::handleGetGlobalLogical()\n"
			"Entering method \n"));
	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::handleGetGlobalLogical()\n"
				"Error: Invalid PDU\n"));
		return -1;
	}

	glcVector glcTable;

	if (!getGLCData(glcTable))
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::handleGetGlobalLogical()\n"
				"Error: No table pointer\n"));

		return -1;
	}
	// Get header and body from PDU
	const ACS_CS_Header * header = 0;
	ACS_CS_Protocol::CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	
	if (pdu && pdu->getHeader())
		header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU
	const ACS_CS_Primitive * primitive = pdu->getPrimitive();
	const ACS_CS_GLK_GetGlobalLogicalClock* getGlobalClock = 0;
	// Check for valid header and body
	if ( (header == 0) || (primitive == 0) )
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::handleGetGlobalLogicalClock()\n"
				"Error: Invalid header or primitive, header = %d, primitive = %d\n",
				header,
				primitive));
		return -1;
	}
	
	// Get version from incoming package
	version = header->getVersion();
	
	if (primitive->getPrimitiveType() == ACS_CS_Protocol::Primitive_GetGlobalLogicalClock)
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::handleGetGlobalLogicalClock()\n"
				"primitive is GetGlobalLogicalClock \n"));
		getGlobalClock = reinterpret_cast<const ACS_CS_GLK_GetGlobalLogicalClock *> (primitive);
		if(getGlobalClock == 0)
		{
			ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
					"(%t) ACS_CS_GLKHandler::handleGetGlobalLogicalClock()\n"
					"Error: Cannot cast primitive\n"));
			return -1;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::handleGetGlobalLogicalClock()\n"
				"Error: Wrong primitive type ,type = %d\n",
				primitive->getPrimitiveType()));
		return -1;
	}
	unsigned short requestId = header->getRequestId();

	//create response header and response primitive
	ACS_CS_Header * responseHeader = new ACS_CS_Header();
	ACS_CS_GLK_GetGlobalLogicalClockResponse * responseBody = new ACS_CS_GLK_GetGlobalLogicalClockResponse();
	if ( (responseHeader == 0) || (responseBody == 0) )
	{
		if (responseHeader)
			delete responseHeader;
		if (responseBody)
			delete responseBody;
		return -1;
	}

	ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
			"(%t) ACS_CS_GLKHandler::handleGetGlobalLogicalClock()\n"
			"setting response header\n"));
	responseHeader->setVersion(version);
	responseHeader->setScope(ACS_CS_Protocol::Scope_GlobalLogicalClock);
	responseHeader->setPrimitive(ACS_CS_Protocol::Primitive_GetGlobalLogicalClockResponse);
	responseHeader->setRequestId(requestId);

	responseBody->setGLCVector(glcTable);
	responseBody->setResultCode(ACS_CS_Protocol::Result_Success);

	responseHeader->setTotalLength(responseHeader->getHeaderLength() + responseBody->getLength() );
	ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
			"(%t) ACS_CS_GLKHandler::handleGetGlobalLogicalClock()\n"
			"response header length = %d body length = %d\n",responseHeader->getHeaderLength(),responseBody->getLength()));



	pdu->setHeader(responseHeader);
	pdu->setPrimitive(responseBody);
	ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
			"(%t) ACS_CS_GLKHandler::handleGetGlobalLogicalClock()\n"
			"gclk of respssonse \n"));
	return 0;
}


int ACS_CS_GLKHandler::createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result)
{

	ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
			"(%t) ACS_CS_GLKHandler::createBasicResponse()\n"
			"Entering method\n"));
	if (pdu == 0)   // Check for valid PDU
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::createBasicResponse()\n"
				"Error: Invalid PDU\n"));
		return -1;
	}
	
	ACS_CS_Protocol::CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;
	const ACS_CS_Header * oldHeader = 0;
	
	if (pdu && pdu->getHeader())
    	oldHeader = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU
		
	// Get version from incoming package
	if (oldHeader)
	{
		version = oldHeader->getVersion();
	}
	
	ACS_CS_Header * header = new ACS_CS_Header();   // Create header
	if (header == 0)
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::createBasicResponse()\n"
				"Error: Invalid header\n"));
		return -1;
	}
	ACS_CS_BasicResponse * body = 0;
	// Create body
	if (type == ACS_CS_Protocol::Primitive_GetGlobalLogicalClockResponse)
		body = new ACS_CS_GLK_GetGlobalLogicalClockResponse();
	if (body == 0)
	{
		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"(%t) ACS_CS_GLKHandler::createBasicResponse()\n"
				"Error: Invalid body\n"));
		delete header;
		return -1;
	}

	// Set all header values
	header->setVersion(version);
	header->setScope(ACS_CS_Protocol::Scope_GlobalLogicalClock);
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






bool ACS_CS_GLKHandler::getGLCData(glcVector&  glcInfo)
{
	bool result = false;
	clockMap lcMap=ACS_CS_TableHandler::getGlobalLogicalClockMap();

	for (clockMap::iterator it=lcMap.begin(); it!=lcMap.end(); ++it)
	{
		ACS_CS_GLC_DATA glcData;
		memset(&glcData, 0, sizeof glcData);

		glcData.attrScopeIdentifier=(uint16_t)it->first;
		glcData.attrLogicalClock=it->second;
		glcInfo.push_back(glcData);		

		ACS_CS_TRACE((ACS_CS_GLKHandler_TRACE,
				"ACS_CS_GLKHandler::getGLCData()\n"
				"Pushing data scope=%d logical clock=%d ",glcData.attrScopeIdentifier,glcData.attrLogicalClock));

	}

	result=true;

	return result;
}


bool ACS_CS_GLKHandler::loadTable ()
{

	// The VLAN table isn't saved to disk.
	// It is re-created on startup every time.

	return true;

}

int ACS_CS_GLKHandler::newTableOperationRequest(ACS_CS_ImModelSubset */*subset*/)
{
	return 0;
}

// Additional Declarations


