//  *********************************************************
//   COPYRIGHT Ericsson 2009-2018.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2009-2018.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2009-2018 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

// ACS_CS_HWCHandler
#include "ACS_CS_HWCHandler.h"

#include <sstream>
#include <string.h>
#include <map>
#include <vector>
#include <iostream>
#include <set>

#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Common_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_TableSearch.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_EntryCounter.h"
#include "ACS_CS_Primitive.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_HWC_GetValue.h"
#include "ACS_CS_HWC_GetValueResponse.h"
#include "ACS_CS_HWC_GetBoardList.h"
#include "ACS_CS_HWC_GetBoardListResponse.h"
#include "ACS_CS_HWC_SynchTable.h"
#include "ACS_CS_HWC_SynchTableResponse.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_FTPAccessHandler.h"
ACS_CS_Trace_TDEF(ACS_CS_HWCHandler_TRACE);

using namespace ACS_CS_NS;
using namespace ACS_CS_API_NS;
using namespace ACS_CS_Protocol;
using std::ostringstream;

// Class ACS_CS_HWCHandler 

ACS_CS_HWCHandler::ACS_CS_HWCHandler()
      : table(0),
        lock(0),
        shelfArchitecture(NODE_UNDEFINED),
	serviceHandler(0),
        agentInstance(0)
{
    table = new ACS_CS_Table();
    lock = new ACS_CS_ReaderWriterLock();

    agentInstance = ACS_CS_SubscriptionAgent::getInstance();
	
    ACS_CS_TableHandler::setGlobalLogicalClock(ACS_CS_Protocol::Scope_HWCTable,logicalClock);

    // Check the shelf Architecture
    ACS_CS_Registry::getNodeArchitecture(shelfArchitecture);
}


ACS_CS_HWCHandler::~ACS_CS_HWCHandler()
{
	if (table)
		delete table;

	if (lock)
		delete lock;
}

int ACS_CS_HWCHandler::handleRequest (ACS_CS_PDU *pdu)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::handleRequest()\n"
        "Entering method\n"));

    if (pdu == 0)   // Check for valid PDU
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleRequest()\n"
            "Error: Invalid PDU\n"));

        return -1;
    }

    int error = 0;

    const ACS_CS_HeaderBase * header = pdu->getHeader(); // Get header from PDU

    if (header)
    {
        // Check which primitive that has been received and call handler function
        if (header->getPrimitiveId() == Primitive_GetValue)
        {
        	{ //trace
        		char msg[256] = {0};
        		snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] Primitive_GetValue", __FILE__, __FUNCTION__, __LINE__);
        		ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
        		std::cout << "DBG: " << msg << std::endl;
        	}
            lock->start_reading();
            error = handleGetValue(pdu);
            lock->stop_reading();
        }
        else if (header->getPrimitiveId() == Primitive_GetBoardList)
        {
	
        	{ //trace
        		char msg[256] = {0};
        		snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] Primitive_GetBoardList", __FILE__, __FUNCTION__, __LINE__);
        		ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
        		//std::cout << "DBG: " << msg << std::endl;
        	}
            lock->start_reading();
            error = handleGetBoardList(pdu);
            lock->stop_reading();
        }
        else if (header->getPrimitiveId() == Primitive_SynchTable)
        {
        	{ //trace
        		char msg[256] = {0};
        		snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] Primitive_SynchTable: calling lock->start_writing()", __FILE__, __FUNCTION__, __LINE__);
        		ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
        		std::cout << "DBG: " << msg << std::endl;
        	}

            lock->start_writing();
            error = handleSynchTable(pdu);

            { //trace
            	char msg[256] = {0};
            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] Primitive_SynchTable: calling lock->stop_writing()", __FILE__, __FUNCTION__, __LINE__);
            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
            	std::cout << "DBG: " << msg << std::endl;
            }

            lock->stop_writing();
        }
        else
        {
            { //trace
            	char msg[256] = {0};
            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] Error: Unknown primitive, id = %d", __FILE__, __FUNCTION__, __LINE__, header->getPrimitiveId());
            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
            	std::cout << "DBG: " << msg << std::endl;
            }

            error = -1;
        }
    }

    return error;
}

int ACS_CS_HWCHandler::newTableOperationRequest(ACS_CS_ImModelSubset *subset)
{
    // Handle HWCTable operations made to the table for the subscriptions
    handleHWCTableSubscription(subset);

    return 0;
}

bool ACS_CS_HWCHandler::loadTable ()
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::loadTable()\n"
        "Entering method\n"));

    if ( ! table )
        return false;

    ACS_CS_API_TableLoader *tableLoader = new ACS_CS_API_TableLoader();

    table->clearTable();

    if (tableLoader->loadHWCTable(*table) == ACS_CS_API_NS::Result_Success)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                "(%t) ACS_CS_HWCHandler::loadTable()\n"
                "Loaded %d table entries", table->getSize()));

        delete tableLoader;
        return true;
    }

    if (tableLoader) {
        delete tableLoader;
    }

    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
       "(%t) ACS_CS_HWCHandler::loadTable()\n"
       "Error: Failed to load the HWC table\n"));

    return false;
}


void ACS_CS_HWCHandler::updateMAC (ACS_CS_DHCP_Info &client)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::updateMAC()\n"
        "Entering method\n" ));

    if (table == NULL)
        return;

    ACS_CS_TableSearch searchObject;        // Create object that will hold search criteria
    ACS_CS_Attribute * macAttr = 0;

    // Get network id and create MAC attribute
    if (client.getNetwork() == ACS_CS_Protocol::Eth_A)
    {
        macAttr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_MAC_EthA);
    }
    else if (client.getNetwork() == ACS_CS_Protocol::Eth_B)
    {
        macAttr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_MAC_EthB);
    }
    else
    {
        return;
    }

    // Get MAC address
    int macLength = client.getMacLength();

    if (macLength < 1)
    {
        delete macAttr;
        return;
    }

    char * mac = new char[macLength];
    client.getMAC( reinterpret_cast<char *>(mac), macLength);
    if (macAttr->setValue(mac, macLength) < 0)
    {
        delete macAttr;
        delete [] mac;
        return;
    }

    // Get mag, slot and fbn for board
    uint16_t fbn = client.getFBN();
    uint32_t mag = client.getMagAddress();
    uint16_t slot = client.getSlot();

    ACS_CS_Attribute fbnAttr(ACS_CS_Protocol::Attribute_FBN);
    ACS_CS_Attribute magAttr(ACS_CS_Protocol::Attribute_Magazine);
    ACS_CS_Attribute slotAttr(ACS_CS_Protocol::Attribute_Slot);

    (void) fbnAttr.setValue( (reinterpret_cast<const char *> (&fbn) ), sizeof(fbn) );
    (void) magAttr.setValue( (reinterpret_cast<const char *> (&mag) ), sizeof(mag) );
    (void) slotAttr.setValue( (reinterpret_cast<const char *> (&slot) ), sizeof(slot) );

    const ACS_CS_Attribute ** array = new const ACS_CS_Attribute *[3];

    array[0] = &fbnAttr;
    array[1] = &slotAttr;
    array[2] = &magAttr;

    // Set attributes in search object
    if (searchObject.setAttributeArray(array, 3) < 0)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::updateMAC()\n"
            "Error: Cannot call searchObject.setAttributeArray(array, 3)\n"));

        delete macAttr;
        delete [] array;
        delete [] mac;
        return;
    }

    int error = 0;

	// Search the table for the board with the specified mag, slot and fbn
	error = table->search(searchObject);

	if (error >= 0)
	{
		// Only one board with this combination
		if (searchObject.getEntryCount() == 1)
		{
			uint16_t boardId;

			// Get that board
			(void) searchObject.getEntryList(&boardId, 1);

			// Set its MAC
			(void) table->setValue(boardId, *macAttr);

		}
		else    // More than one board with this combination (error)
		{
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
				"(%t) ACS_CS_HWCHandler::updateMAC()\n"
				"Error: Duplicate boards found in table\n"));

			ACS_CS_EVENT(Event_ProgrammingError, ACS_CS_EventReporter::Severity_Event,
				"Duplicate boards in the table",
				"Duplicate boards with the same mag, slot and fbn.",
				"");
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
			"(%t) ACS_CS_HWCHandler::updateMAC()\n"
			"Error: Failed to search table\n"));

		ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
			"Search HWC Table",
			"Unable to search the HWC table",
			"");
	}

    delete macAttr;
    delete [] array;
    delete [] mac;

}


int ACS_CS_HWCHandler::handleGetValue (ACS_CS_PDU *pdu)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::handleGetValue()\n"
        "Entering method\n"));

    if (pdu == 0)   // Check for valid PDU
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetValue()\n"
            "Error: Invalid PDU\n"));

        return -1;
    }
    else if (table == 0)    // Check for valid table
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetValue()\n"
            "Error: No table pointer\n"));

        return -1;
    }
    // Get header and body from PDU
    const ACS_CS_Header * header = 0;

    if (pdu && pdu->getHeader())
       header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

    const ACS_CS_Primitive * primitive = pdu->getPrimitive();
    const ACS_CS_HWC_GetValue * getValue = 0;

    // Check for valid header and body
    if ( (header == 0) || (primitive == 0) )
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetValue()\n"
            "Error: Invalid header or primitive, header = %d, primitive = %d\n",
            header,
            primitive));

        return -1;
    }

    // Check primitive type and cast to correct type
    if (primitive->getPrimitiveType() == Primitive_GetValue)
    {
        getValue = reinterpret_cast<const ACS_CS_HWC_GetValue *> (primitive);

        if (getValue == 0)
        {
            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                "(%t) ACS_CS_HWCHandler::handleGetValue()\n"
                "Error: Cannot cast primitive\n"));

            return -1;
        }
    }
    else
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetValue()\n"
            "Error: Wrong primitive type, type = %d\n",
            primitive->getPrimitiveType()));

        return -1;
    }

    int error = 0;
    uint16_t requestId = header->getRequestId();          // Save request id
    uint16_t boardId = getValue->getBoardId();            // Get board id
    CS_Attribute_Identifier attributeId = getValue->getAttributeId();   // Get attribte id

    //**********************************************************************************

    int size = 0;
    char * buffer = 0;
    in_addr address;
    memset(&address, 0, sizeof(address));
    bool boardFound = false;
    ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
    const ACS_CS_ImBlade *blade = 0;

    if (!model)
    {
    	return createGetValueResponse(pdu, requestId, ACS_CS_Protocol::Result_Busy);
    }

    std::set<const ACS_CS_ImBase *> objects;
    model->getObjects(objects, BLADE_T);

    std::set<const ACS_CS_ImBase *>::const_iterator it;

    for (it = objects.begin(); it != objects.end(); it++){

    	blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);

    	if(blade && blade->entryId == boardId){
    		boardFound = true;
    		break;
    	}
    }

    if(!boardFound)
    {
    	delete model;
    	return createGetValueResponse(pdu, requestId, ACS_CS_Protocol::Result_No_Such_Entry);
    }

    switch (attributeId) {

	case Attribute_Magazine:
	{
		//Attribute Length: 4 Bytes
		uint32_t magazine = 0;
		ACS_CS_ImBase *parent = model->getParentOf(blade->rdn);
		ACS_CS_ImShelf *shelf = dynamic_cast<ACS_CS_ImShelf *>(parent);
		if (shelf)
		{
			in_addr address;
			if (inet_aton(shelf->address.c_str(), &address) != 0)
			{
				magazine = address.s_addr;
				buffer = new char[sizeof(uint32_t)];
				memcpy(buffer, &magazine, sizeof(uint32_t));
				size = sizeof(uint32_t);
			}

			//trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_Magazine: attrId == %d, value = %s, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, shelf->address.c_str(), size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_Magazine: attrId == %d, value = %s, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, shelf->address.c_str(), size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;
	}

	case Attribute_Slot:
		//Attribute Length: 2 Bytes
		size = sizeof(uint16_t);
		buffer = new char[sizeof(uint16_t)];
		memcpy(buffer, &blade->slotNumber, sizeof(uint16_t));

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_Slot: attrId == %d, value = %u, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->slotNumber, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;

	case Attribute_SysType:
		//Attribute Length: 2 Bytes
		if (blade->systemType != UNDEF_SYSTEMTYPE)
		{
			size = sizeof(uint16_t);
			buffer = new char[sizeof(uint16_t)];
			uint16_t tmpSysType = static_cast<uint16_t>(blade->systemType);
			memcpy(buffer, &tmpSysType, sizeof(uint16_t));
		}

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_SysType: attrId == %d, value = %d, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->systemType, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;

	case Attribute_SysNo:
		//Attribute Length: 2 Bytes
		if (blade->systemNumber != 0) //Zero stands for undefined System Number
		{
			size = sizeof(uint16_t);
			buffer = new char[sizeof(uint16_t)];
			uint16_t tmpSysNo = static_cast<uint16_t>(blade->systemNumber);
			memcpy(buffer, &tmpSysNo, sizeof(uint16_t));
		}

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_SysNo: attrId == %d, value = %u, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->systemNumber, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;

	case Attribute_FBN:
		//Attribute Length: 2 Bytes
		if (blade->functionalBoardName != UNDEF_FUNCTIONALBOARDNAME)
		{
			size = sizeof(uint16_t);
			buffer = new char[sizeof(uint16_t)];
			uint16_t tmpFbn = static_cast<uint16_t>(blade->functionalBoardName);
			memcpy(buffer, &tmpFbn, sizeof(uint16_t));
		}

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_FBN: attrId == %d, value = %u, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->functionalBoardName, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;

	case Attribute_Side:
		//Attribute Length: 2 Bytes
		if (blade->side != UNDEF_SIDE)
		{
			size = sizeof(uint16_t);
			buffer = new char[sizeof(uint16_t)];
			uint16_t tmpSide = static_cast<uint16_t>(blade->side);
			memcpy(buffer, &tmpSide, sizeof(uint16_t));
		}

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_Side: attrId == %d, value = %u, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->side, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;

	case Attribute_SeqNo:
		//Attribute Length: 2 Bytes
		if (blade->sequenceNumber != -1)
		{
			size = sizeof(uint16_t);
			buffer = new char[sizeof(uint16_t)];
			uint16_t tmpSeqNo = static_cast<uint16_t>(blade->sequenceNumber);
			memcpy(buffer, &tmpSeqNo, sizeof(uint16_t));
		}

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_SeqNo: attrId == %d, value = %d, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->sequenceNumber, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;

	case Attribute_IP_EthA:
		//Attribute Length: 4 Bytes
		if (inet_aton(blade->ipAddressEthA.c_str(), &address) != 0)
		{
			size = sizeof(uint32_t);
			buffer = new char[sizeof(uint32_t)];
			uint32_t ip = htonl(address.s_addr);
			memcpy(buffer, &ip, sizeof(uint32_t));
			{ //trace
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1,
						"[%s::%s@%d] Request for Attribute_IP_EthA: attrId == %d, value = %s (0x%X), size = %d",
						__FILE__, __FUNCTION__, __LINE__, attributeId, blade->ipAddressEthA.c_str(), ip, size);
				ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
				std::cout << "DBG: " << msg << std::endl;
			}
		}
		break;

	case Attribute_IP_EthB:
		//Attribute Length: 4 Bytes
		if (inet_aton(blade->ipAddressEthB.c_str(), &address) != 0)
		{
			size = sizeof(uint32_t);
			buffer = new char[sizeof(uint32_t)];
			uint32_t ip = htonl(address.s_addr);
			memcpy(buffer, &ip, sizeof(uint32_t));
			{ //trace
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1,
						"[%s::%s@%d] Request for Attribute_IP_EthB: attrId == %d, value = %s (0x%X), size = %d",
						__FILE__, __FUNCTION__, __LINE__, attributeId, blade->ipAddressEthA.c_str(), ip, size);
				ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
				std::cout << "DBG: " << msg << std::endl;
			}
		}
		break;

	case Attribute_Alias_EthA:
		//Attribute Length: 4 Bytes
		if (inet_aton(blade->ipAliasEthA.c_str(), &address) != 0)
		{
			size = sizeof(uint32_t);
			buffer = new char[sizeof(uint32_t)];
			uint32_t ip = htonl(address.s_addr);
			memcpy(buffer, &ip, sizeof(uint32_t));
			{ //trace
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1,
						"[%s::%s@%d] Request for Attribute_Alias_EthA: attrId == %d, value = %s (0x%X), size = %d",
						__FILE__, __FUNCTION__, __LINE__, attributeId, blade->ipAliasEthA.c_str(), ip, size);
				ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
				std::cout << "DBG: " << msg << std::endl;
			}
		}
		break;

	case Attribute_Alias_EthB:
		//Attribute Length: 4 Bytes
		if (inet_aton(blade->ipAliasEthB.c_str(), &address) != 0)
		{
			size = sizeof(uint32_t);
			buffer = new char[sizeof(uint32_t)];
			uint32_t ip = htonl(address.s_addr);
			memcpy(buffer, &ip, sizeof(uint32_t));
			{ //trace
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1,
						"[%s::%s@%d] Request for Attribute_Alias_EthB: attrId == %d, value = %s (0x%X), size = %d",
						__FILE__, __FUNCTION__, __LINE__, attributeId, blade->ipAliasEthB.c_str(), ip, size);
				ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "%s", msg));
				std::cout << "DBG: " << msg << std::endl;
			}
		}
		break;

	case Attribute_Netmask_Alias_EthA:
		//Attribute Length: 4 Bytes
		if (inet_aton(blade->aliasNetmaskEthA.c_str(), &address) != 0)
		{
			size = sizeof(uint32_t);
			buffer = new char[sizeof(uint32_t)];
			uint32_t ip = htonl(address.s_addr);
			memcpy(buffer, &ip, sizeof(uint32_t));
			{ //trace
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1,
						"[%s::%s@%d] Request for Attribute_Netmask_Alias_EthA: attrId == %d, value = %s (0x%X), size = %d",
						__FILE__, __FUNCTION__, __LINE__, attributeId, blade->aliasNetmaskEthA.c_str(), ip, size);
				ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
				std::cout << "DBG: " << msg << std::endl;
			}
		}
		break;

	case Attribute_Netmask_Alias_EthB:
		//Attribute Length: 4 Bytes
		if (inet_aton(blade->aliasNetmaskEthB.c_str(), &address) != 0)
		{
			size = sizeof(uint32_t);
			buffer = new char[sizeof(uint32_t)];
			uint32_t ip = htonl(address.s_addr);
			memcpy(buffer, &ip, sizeof(uint32_t));
			{ //trace
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1,
						"[%s::%s@%d] Request for Attribute_Netmask_Alias_EthB: attrId == %d, value = %s (0x%X), size = %d",
						__FILE__, __FUNCTION__, __LINE__, attributeId, blade->aliasNetmaskEthB.c_str(), ip, size);
				ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
				std::cout << "DBG: " << msg << std::endl;
			}
		}
		break;

	case Attribute_DHCP:
		//Attribute Length: 2 Bytes
		if (blade->dhcpOption != UNDEF_DHCP)
		{
			size = sizeof(uint16_t);
			buffer = new char[sizeof(uint16_t)];
			uint16_t tmpDhcp = static_cast<uint16_t>(blade->dhcpOption);
			memcpy(buffer, &tmpDhcp, sizeof(uint16_t));
		}
		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_DHCP: attrId == %d, value = %d, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->dhcpOption, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "%s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;

	case Attribute_SysId:
	{
		//Attribute Length: 2 Bytes
		uint16_t sysId;

		if(blade->systemType == SINGLE_SIDED_CP)
		{
			sysId = blade->systemType + blade->sequenceNumber;
		}
		else if(blade->systemType == DOUBLE_SIDED_CP || blade->systemType == AP)
		{
			sysId = blade->systemType + blade->systemNumber;
		}
		else
		{
			sysId = 0;
		}

		size = sizeof(uint16_t);
		buffer = new char[sizeof(uint16_t)];
		memcpy(buffer, &sysId, sizeof(uint16_t));

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_SysId: attrId == %d, value = %u, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, sysId, size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}

		break;
	}

	case Attribute_Current_Load_Module_Version:
	{
		const ACS_CS_ImOtherBlade *otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade *>(blade);
		if (otherBlade)
		{
			if ( (size = otherBlade->currentLoadModuleVersion.size()) > 0)
			{
				buffer = new char[size];
				strncpy(buffer, otherBlade->currentLoadModuleVersion.c_str(), size);
			}

			//trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_Current_Load_Module_Version: attrId == %d, value = %s, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, otherBlade->currentLoadModuleVersion.c_str(), size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}		
		break;
	}

	case Attribute_Blade_Product_Number: {
		const ACS_CS_ImOtherBlade *otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade *>(blade);
		if (otherBlade)
		{
			if ( (size = otherBlade->bladeProductNumber.size()) > 0)
			{
				buffer = new char[size];
				strncpy(buffer, otherBlade->bladeProductNumber.c_str(), size);
			}

			//trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_Blade_Product_Number: attrId == %d, value = %s, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, otherBlade->bladeProductNumber.c_str(), size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;
	}

	case Attribute_Blade_Uuid:
	{
		if ( (size = blade->uuid.size()) > 0)
		{
			buffer = new char[size];
			strncpy(buffer, blade->uuid.c_str(), size);
		}

		{ //trace
			char msg[256] = {0};
			snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_Blade_Uuid: attrId == %d, value = %s, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, blade->uuid.c_str(), size);
			ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "(%t) %s", msg));
			std::cout << "DBG: " << msg << std::endl;
		}
		break;
	}

	case Attribute_MAC_EthA:
	case Attribute_MAC_EthB:
	// MAC_EthA and MAC_EthB are internal attributes not to be used by external clients.
	// They are used to for MAC-addresses for the boards when the HWC table is saved to disk.
	{ //trace
		char msg[256] = {0};
		snprintf(msg, sizeof(msg) - 1,
				"[%s::%s@%d] ERROR: Request for MAC Address attribute: attrId == %d",
				__FILE__, __FUNCTION__, __LINE__, attributeId);
		ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		std::cout << "DBG: " << msg << std::endl;
		break;
	}	
	case Attribute_SwVer_Type:
	{
		if(blade->functionalBoardName == CMXB)
		{
			const ACS_CS_ImOtherBlade *otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade *>(blade);
			if(otherBlade)
			{
				uint16_t swVerType;
				swVerType = otherBlade->swVerType;
				size = sizeof(uint16_t);
				buffer = new char[sizeof(uint16_t)];
				memcpy(buffer, &swVerType, sizeof(uint16_t));

				//trace
				char msg[256] = {0};
				snprintf(msg, sizeof(msg) - 1,
					"[%s::%s@%d] Request for Attribute_SwVer_Type: attrId == %d, value = %d, size = %d",
					__FILE__, __FUNCTION__, __LINE__, attributeId, swVerType, size);
				ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "%s", msg));
				std::cout << "DBG: " << msg << std::endl;
			}
		}
		break;
	}
	default:
		error = createGetValueResponse(pdu, requestId,
				ACS_CS_Protocol::Result_No_Value);
		delete model;
		return error;
	}

    if(buffer != 0)
    {
    	error = createGetValueResponse(pdu,             // Send response
    			requestId,
    			ACS_CS_Protocol::Result_Success,
    			logicalClock,
    			buffer,
    			size);
    	delete [] buffer;

    }
    else
    {
    	error = createGetValueResponse(pdu, requestId, ACS_CS_Protocol::Result_No_Value);
    	{ //trace
    		char msg[256] = {0};
    		snprintf(msg, sizeof(msg) - 1,
    				"[%s::%s@%d] Request for Unknown Attribute: attrId == %d, answer = Result_No_Value",
    				__FILE__, __FUNCTION__, __LINE__, attributeId);
    		ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
    		std::cout << "DBG: " << msg << std::endl;
    	}
    }


    //***********************************************************************

//    ACS_CS_TableEntry board(ILLEGAL_TABLE_ENTRY);
//
//    try
//    {
//        board = table->getEntry(boardId);   // Get board from table
//    }
//    catch (ACS_CS_TimeoutException ex)  // If timeout from table, send busy response
//    {
//        error = createBasicResponse(pdu,
//            ACS_CS_Protocol::Primitive_GetValueResponse,
//            requestId,
//            ACS_CS_Protocol::Result_Busy);
//
//        return error;
//    }
//
//    ACS_CS_Attribute attribute;
//
//    if (board.getId() == boardId)                               // If board was in table
//    {
//        attribute = board.getValue(attributeId);                // Get attribute
//
//        if (attribute.getIdentifier() != Attribute_NotSpecified)                // If board had attribute
//        {
//            int size = attribute.getValueLength();              // Get value length
//
//            if (size > 0)                                       // If there is a value to return
//            {
//                char * buffer = new char[size];                 // Allocate memory for value
//
//                if (buffer)                                     // Check for successful new
//                {
//                    if (attribute.getValue(buffer, size) > 0)   // Get value
//                    {
//
//                        error = createGetValueResponse(pdu,             // Send response
//                            requestId,
//                            ACS_CS_Protocol::Result_Success,
//                            table->getLogicalClock(),
//                            buffer,
//                            size);
//
//                    }
//                    else    // Some logic error
//                    {
//                        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
//                            "(%t) ACS_CS_HWCHandler::handleGetValue()\n"
//                            "Error: Cannot call attribute->getValue(buffer, size)\n"));
//
//                        delete [] buffer;
//                        return -1;
//                    }
//                }
//                else    // Unsuccessful new
//                {
//                    return -1;
//                }
//
//                delete [] buffer;
//            }
//            else    // No value for this attribute
//            {
//                error = createGetValueResponse(pdu, requestId, ACS_CS_Protocol::Result_No_Value);
//            }
//        }
//        else    // No attribute of this kind for this board
//        {
//            error = createGetValueResponse(pdu, requestId, ACS_CS_Protocol::Result_No_Value);
//        }
//    }
//    else    // No board with this board id
//    {
//        error = createGetValueResponse(pdu, requestId, ACS_CS_Protocol::Result_No_Such_Entry);
//    }

    delete model;

    return error;
}

int ACS_CS_HWCHandler::handleGetBoardList (ACS_CS_PDU *pdu)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
        "Entering method\n"));

    if (pdu == 0)   // Check for valid PDU
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
            "Error: Invalid PDU\n"));

        return -1;
    }
    else if (table == 0)    // Check for valid table
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
            "Error: No table pointer\n"));

        return -1;
    }

    // Get header and body from PDU
    const ACS_CS_Header * header = 0;

    if (pdu && pdu->getHeader())
       header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

    const ACS_CS_Primitive * primitive = pdu->getPrimitive();
    const ACS_CS_HWC_GetBoardList * getBoardList = 0;

    // Check for valid header and body
    if ( (header == 0) || (primitive == 0) )
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
            "Error: Invalid header or primitive, header = %d, primitive = %d\n",
            header,
            primitive));

        return -1;
    }

    // Check primitive type and cast to correct type
    if (primitive->getPrimitiveType() == Primitive_GetBoardList)
    {
        getBoardList = reinterpret_cast<const ACS_CS_HWC_GetBoardList *> (primitive);

        if (getBoardList == 0)
        {
            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
                "Error: Cannot cast primitive\n"));

            return -1;
        }
    }
    else
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
            "Error: Wrong primitive type, type = %d\n",
            primitive->getPrimitiveType()));

        return -1;
    }

    uint16_t requestId = header->getRequestId();              // Save request id
    uint16_t attributeCount = getBoardList->getAttributeCount();  // Get no of attributes

//    ACS_CS_API_HWC * hwc =  ACS_CS_API::createHWCInstance();

    ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

    if(!model){
    	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                    "ACS_CS_HWCHandler::handleGetBoardList()\n"
                    "ERROR: ACS_CS_API::createHWCInstance() failed\n"));

    	return createBasicResponse(pdu,
    	            ACS_CS_Protocol::Primitive_GetBoardListResponse,
    	            requestId,
    	            ACS_CS_Protocol::Result_Busy);
    }

    ACS_CS_API_BoardSearch_Implementation *boardSearch = new ACS_CS_API_BoardSearch_Implementation();

    if(!boardSearch){
    	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                    "ACS_CS_HWCHandler::handleGetBoardList()\n"
                    "ERROR: ACS_CS_API_BoardSearch::createBoardSearchInstance() failed\n"));

    	return createBasicResponse(pdu,
    	            ACS_CS_Protocol::Primitive_GetBoardListResponse,
    	            requestId,
    	            ACS_CS_Protocol::Result_Busy);
    }

    int error = 0;
    uint32_t ip = 0;
    uint16_t shortAttr = 0;
    std::string strAttr;

    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                "ACS_CS_HWCHandler::handleGetBoardList()\n"
                "BoardSearch: attributeCount %d\n",
                attributeCount));

    if (attributeCount > 0)
    {
        // Get all attributes from primitive
        for (int i = 0; i < attributeCount; i++)
        {
        	const ACS_CS_Attribute * attr = getBoardList->getAttribute(i);

            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                        "ACS_CS_HWCHandler::handleGetBoardList()\n"
                        "BoardSearch: search criteria %d\n",
                        attr->getIdentifier()));

        	switch(attr->getIdentifier())
        	{
				case Attribute_Magazine:
					ip = getIpAddress(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setMagazine(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, ip);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setMagazine(ip);
					break;

				case Attribute_Slot:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setSlot(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setSlot(shortAttr);
					break;

				case Attribute_SysType:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setSysType(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setSysType(shortAttr);
					break;

				case Attribute_SysNo:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setSysNo(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setSysNo(shortAttr);
					break;

				case Attribute_FBN:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setFBN(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setFBN(shortAttr);
					break;

				case Attribute_Side:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setSide(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setSide(shortAttr);
					break;

				case Attribute_SeqNo:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setSeqNo(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setSeqNo(shortAttr);
					break;

				case Attribute_IP_EthA:
					ip = getIpAddress(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setIPEthA(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, ip);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setIPEthA(ip);
					break;

				case Attribute_IP_EthB:
					ip = getIpAddress(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setIPEthB(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, ip);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "%s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setIPEthB(ip);
					break;

				case Attribute_Alias_EthA:
					ip = getIpAddress(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setAliasEthA(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, ip);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setAliasEthA(ip);
					break;

				case Attribute_Alias_EthB:
					ip = getIpAddress(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setAliasEthB(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, ip);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setAliasEthB(ip);
					break;

				case Attribute_Netmask_Alias_EthA:
					ip = getIpAddress(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setAliasNetmaskEthA(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, ip);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "%s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setAliasNetmaskEthA(ip);
					break;

				case Attribute_Netmask_Alias_EthB:
					ip = getIpAddress(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setAliasNetmaskEthB(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, ip);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setAliasNetmaskEthB(ip);
					break;

				case Attribute_DHCP:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setDhcpMethod(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setDhcpMethod(shortAttr);
					break;

				case Attribute_SysId:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setSysId(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setSysId(shortAttr);
					break;
				case Attribute_SwVer_Type:
					shortAttr = getShortAttr(attr);
		            { //trace
		            	char msg[256] = {0};
		            	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setSwVerType(%u)",
		            			__FILE__, __FUNCTION__, __LINE__, shortAttr);
		            	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "%s", msg));
		            	std::cout << "DBG: " << msg << std::endl;
		            }
					boardSearch->setSwVerType(shortAttr);
					break;
				case Attribute_Blade_Uuid:
					strAttr = getStringAttr(attr);
                            { //trace
                                char msg[256] = {0};
                                snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: setUuid(%s)",
                                                __FILE__, __FUNCTION__, __LINE__, strAttr.c_str());
                                ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
                                std::cout << "DBG: " << msg << std::endl;
                            }
                                        boardSearch->setUuid(strAttr);
                                        break;
	

				default:
					ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
						"ACS_CS_HWCHandler::handleGetBoardList()\n"
						"Unsupported search attribute"));
					{ //trace
						char msg[256] = {0};
						snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] BoardSearch: Unsupported attribute",
								__FILE__, __FUNCTION__, __LINE__);
						ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
						std::cout << "DBG: " << msg << std::endl;
					}
					break;
        	}
        }
    }
//
//        // Set attributes in search object
//        if (searchObject.setAttributeArray(array, attributeCount) < 0)
//        {
//            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
//                "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
//                "Error: Cannot call searchObject.setAttributeArray(array, attributeCount)\n"));
//
//            delete [] array;
//            return -1;
//        }
//
//        delete [] array;
//    }
//
//    try
//    {
//        error = table->search(searchObject);            // Pass search object to table
//
//        if (error < 0)
//        {
//            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
//                "(%t) ACS_CS_HWCHandler::handleGetBoardList()\n"
//                "Error: Cannot search table\n"));
//
//            return -1;
//        }
//    }
//    catch (ACS_CS_TimeoutException ex)          // If timeout from table, send busy response
//    {
//        error = createBasicResponse(pdu,
//            ACS_CS_Protocol::Primitive_GetBoardListResponse,
//            requestId,
//            ACS_CS_Protocol::Result_Busy);
//
//        return error;
//    }
//

    vector<uint16_t> ids;

    std::set<const ACS_CS_ImBase *> objects;
	model->getObjects(objects, BLADE_T);

	std::set<const ACS_CS_ImBase *>::const_iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		if(boardSearch->match(*it))
		{
			const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);

			if(blade)
			{
				ids.push_back(blade->entryId);
			}
		}
	}

	int hits = ids.size();
	uint16_t *boardList = NULL;

	if(hits)
	{
		boardList = new uint16_t[hits];
		memcpy(boardList, ids.data(), sizeof(uint16_t) * hits);
	}

//	for(size_t i = 0; i < hits; ++i)
//	{
//		boardList[i] = ids.at(i);
//	}



    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
    		"ACS_CS_HWCHandler::handleGetBoardList()\n"
    		"BoardSearch: search result count: %d\n",
    		hits));

    // Send response
    error = createGetBoardListResponse(pdu,
        requestId,
        ACS_CS_Protocol::Result_Success,
        logicalClock,
        boardList,
        hits);

    { //trace
    	char msg[256] = {0};
    	snprintf(msg, sizeof(msg) - 1, "[%s::%s@%d] Created GetBoardListResponse with logicalClock == %u",
    			__FILE__, __FUNCTION__, __LINE__, logicalClock);
    	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, " %s", msg));
    	//std::cout << "DBG: " << msg << std::endl;
    }

    if(boardList)
        delete [] boardList;

	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        			"ACS_CS_HWCHandler::handleGetBoardList()\n"
        			"return  %d\n",
        			error));

	delete model;
	delete boardSearch;
    return error;
}



int ACS_CS_HWCHandler::handleSynchTable (ACS_CS_PDU *pdu)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::handleSynchTable()\n"
        "Entering method\n"));

    // Not implemented
    //return -1;


    if (pdu == 0)   // Check for valid PDU
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleSynchTable()\n"
            "Error: Invalid PDU\n"));

        return -1;
    }
    else if (table == 0)    // Check for valid table
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleSynchTable()\n"
            "Error: No table pointer\n"));

        return -1;
    }

    // Get header and body from PDU
    const ACS_CS_Header * header = 0;

    if (pdu && pdu->getHeader())
       header = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

    const ACS_CS_Primitive * primitive = pdu->getPrimitive();
    const ACS_CS_HWC_SynchTable * synchTable = 0;

    // Check for valid header and body
    if ( (header == 0) || (primitive == 0) )
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleSynchTable()\n"
            "Error: Invalid header or primitive, header = %d, primitive = %d\n",
            header,
            primitive));

        return -1;
    }

    // Check primitive type and cast to correct type
    if (primitive->getPrimitiveType() == Primitive_SynchTable)
    {
        synchTable = reinterpret_cast<const ACS_CS_HWC_SynchTable *> (primitive);

        if (synchTable == 0)
        {
            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                "(%t) ACS_CS_HWCHandler::handleSynchTable()\n"
                "Error: Cannot cast primitive\n"));

            return -1;
        }
    }
    else
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::handleSynchTable()\n"
            "Error: Wrong primitive type, type = %d\n",
            primitive->getPrimitiveType()));

        return -1;
    }

    uint16_t requestId = header->getRequestId();          // Get request id
    int error = -1;

    if (this->table->getSize() == 0)    // Nothing to sync, respond at once
    {
        error = createBasicResponse(pdu,                        // Create response
            ACS_CS_Protocol::Primitive_SynchTableResponse,
            requestId,
            ACS_CS_Protocol::Result_Success);

        return error;
    }

    // Vector to hold all clients to synch
    vector<ACS_CS_DHCP_Info> syncVector;

//    ACS_CS_TableSearch searchObj;
//    (void) this->table->search(searchObj);
//
//    if (searchObj.getEntryCount() > 0)
//    {
//        uint16_t * boardList = new uint16_t[searchObj.getEntryCount()];
//
//        if (boardList)
//            (void) searchObj.getEntryList(boardList, searchObj.getEntryCount());
//
//        for (int i = 0; i < searchObj.getEntryCount(); i++)
//        {
//            ACS_CS_TableEntry board(ILLEGAL_TABLE_ENTRY);
//
//            if (boardList)
//                board = table->getEntry(boardList[i]);
//
//            ACS_CS_DHCP_Info reservationEthA = this->getDhcpInfo(Eth_A, board);
//            ACS_CS_DHCP_Info reservationEthB = this->getDhcpInfo(Eth_B, board);
//
//            if (reservationEthA.getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)
//                syncVector.push_back(reservationEthA);
//
//            if (reservationEthB.getDHCPMethod() == ACS_CS_Protocol::DHCP_Normal)
//                syncVector.push_back(reservationEthB);
//        }
//
//        delete [] boardList;
//    }

    ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

        if(!model){
        	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                        "ACS_CS_HWCHandler::handleSynchTable()\n"
                        "ERROR: ACS_CS_API::createHWCInstance() failed\n"));

        	return createBasicResponse(pdu,
        	            ACS_CS_Protocol::Primitive_SynchTableResponse,
        	            requestId,
        	            ACS_CS_Protocol::Result_Busy);
        }



    ACS_CS_API_BoardSearch_Implementation *boardSearch = new ACS_CS_API_BoardSearch_Implementation();

    if(!boardSearch){
    	ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
    			"ACS_CS_HWCHandler::handleSynchTable()\n"
    			"ERROR: ACS_CS_API_BoardSearch::createBoardSearchInstance() failed\n"));

    	delete model;

    	return createBasicResponse(pdu,
    			ACS_CS_Protocol::Primitive_SynchTableResponse,
    			requestId,
    			ACS_CS_Protocol::Result_Busy);
    }

    boardSearch->setDhcpMethod(ACS_CS_Protocol::DHCP_Normal);

    std::set<const ACS_CS_ImBase *> objects;
    model->getObjects(objects, BLADE_T);

    std::set<const ACS_CS_ImBase *>::const_iterator it;

    for (it = objects.begin(); it != objects.end(); it++)
    {
    	if(boardSearch->match(*it))
    	{
    		const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);

    		if(blade)
    		{
    			ACS_CS_DHCP_Info reservationEthA,reservationAliasEthA;
    			ACS_CS_DHCP_Info reservationEthB,reservationAliasEthB;
				ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE;
				if(blade->functionalBoardName==SMXB)
					switchType = ACS_CS_Protocol::CONTROL_SWITCH;

    			if (serviceHandler) {					
    				serviceHandler->createDHCPInfo(reservationEthA, blade, ACS_CS_Protocol::Eth_A,switchType);
					serviceHandler->createDHCPInfo(reservationEthB, blade, ACS_CS_Protocol::Eth_B,switchType);
				if(blade->functionalBoardName==SMXB){
					serviceHandler->createDHCPInfo(reservationAliasEthA, blade, ACS_CS_Protocol::Eth_A, ACS_CS_Protocol::TRANSPORT_SWITCH);
					serviceHandler->createDHCPInfo(reservationAliasEthB, blade, ACS_CS_Protocol::Eth_B, ACS_CS_Protocol::TRANSPORT_SWITCH);
				}
    			}

    			syncVector.push_back(reservationEthA);
    			syncVector.push_back(reservationEthB);
				if(blade->functionalBoardName==SMXB){
					syncVector.push_back(reservationAliasEthA);
					syncVector.push_back(reservationAliasEthB);
				}
    		}
    	}
    }

    delete model;
    delete boardSearch;

    if (syncVector.size() == 0)
    {
    	error = createBasicResponse(pdu,                        // Create response
			ACS_CS_Protocol::Primitive_SynchTableResponse,
			requestId,
			ACS_CS_Protocol::Result_Success);

		return error;
    }

    // Get synch data
    bool syncSuccess = false;

    // Convert to pointers (needed by SNMP handler)
    vector<ACS_CS_DHCP_Info*> ptrVector;

    for (size_t i = 0; i < syncVector.size(); i++)
    {
    	ptrVector.push_back(&syncVector[i]);
    }

    if (serviceHandler)
    {
       ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "------------------------  BJAX ------------------------ serviceHandler != NULL\n"));
       syncSuccess = serviceHandler->syncClients(ptrVector);
    }
    else
       ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE, "------------------------  BJAX ------------------------ serviceHandler == NULL\n"));

    if (syncSuccess)    // Synch data received
    {
        vector<ACS_CS_DHCP_Info*>::iterator it;
        bool commitSuccess = false;

//        ACS_CS_Table * tmpTable = new ACS_CS_Table(*table);
//        bool tableUpdated = false;
//
//        // Loop through all boards
//        for (it = ptrVector.begin(); it != ptrVector.end(); ++it)
//        {
//            this->updateMAC(**it);    // Set the new MAC addresses
//        }
//
//        if (table->save(ACS_CS_INTERNAL_API::Table_HWC) == false)  // If save failed
//		{
//			delete table;           // Revert to old table
//			table = tmpTable;
//		}
//		else
//		{
//			delete tmpTable;
//			tableUpdated = true;
//		}

        //TODO: Check if we should store something before this point
        bool tableUpdated = true;

		if (tableUpdated)
		{
			// Commit changes in DHCP
			if (serviceHandler) {
				commitSuccess = serviceHandler->commitSync();
			}
		}

        if (commitSuccess == true)
        {
            error = createBasicResponse(pdu,                        // Create response
                ACS_CS_Protocol::Primitive_SynchTableResponse,
                requestId,
                ACS_CS_Protocol::Result_Success);
        }
        else if (tableUpdated == false)
		{
			error = createBasicResponse(pdu,                        // Create response
				ACS_CS_Protocol::Primitive_SynchTableResponse,
				requestId,
				ACS_CS_Protocol::Result_Other_Failure);
		}
        else
        {
            error = createBasicResponse(pdu,                        // Create response
                ACS_CS_Protocol::Primitive_SynchTableResponse,
                requestId,
                ACS_CS_Protocol::Result_Busy);
        }
    }
    else
    {
        error = createBasicResponse(pdu,                        // Create response
            ACS_CS_Protocol::Primitive_SynchTableResponse,
            requestId,
            ACS_CS_Protocol::Result_Other_Failure);
    }

    return error;

}

void ACS_CS_HWCHandler::handleHWCTableSubscription(ACS_CS_ImModelSubset *subset)
{
	set<const ACS_CS_ImBase *> blades;
	subset->getObjects(blades, CPBLADE_T);
	subset->getObjects(blades, APBLADE_T);
	subset->getObjects(blades, OTHERBLADE_T);

	ACS_CS_HWCChange change;

	set<const ACS_CS_ImBase *>::const_iterator it;
	for(it = blades.begin(); it != blades.end(); it++)
	{
		change.hwcData.push_back(getHWCTableData(dynamic_cast<const ACS_CS_ImBlade *>(*it)));
	}
	if(change.hwcData.size() > 0)
	{
		agentInstance->handleHWCUpdates(change);
		logicalClock++;
		ACS_CS_TableHandler::setGlobalLogicalClock(ACS_CS_Protocol::Scope_HWCTable,logicalClock);
	}
}

ACS_CS_HWCData ACS_CS_HWCHandler::getHWCTableData(const ACS_CS_ImBlade *blade) const
{
	if(NULL == blade)
		return ACS_CS_HWCData();


	ACS_CS_HWCData hwcData;

	switch(blade->action)
	{
	case ACS_CS_ImBlade::CREATE:
		if(blade->functionalBoardName == CPUB || blade->functionalBoardName == ::MAUB || blade->functionalBoardName == APUB || (shelfArchitecture == NODE_VIRTUALIZED && blade->functionalBoardName == IPLB))
		{
			ACS_CS_FTPAccessHandler::instance()->enableBladeForFTP(blade);
		}
		hwcData.operationType = ACS_CS_API_TableChangeOperation::Add;
		break;

	case ACS_CS_ImBlade::MODIFY:
		hwcData.operationType = ACS_CS_API_TableChangeOperation::Change;
		break;

	case ACS_CS_ImBlade::DELETE:
		if(blade->functionalBoardName == CPUB || blade->functionalBoardName == ::MAUB || blade->functionalBoardName == APUB || (shelfArchitecture == NODE_VIRTUALIZED && blade->functionalBoardName == IPLB))
		{
			ACS_CS_FTPAccessHandler::instance()->disableBladeForFTP(blade);
		}
		hwcData.operationType = ACS_CS_API_TableChangeOperation::Delete;
		break;

	default:
		hwcData.operationType = ACS_CS_API_TableChangeOperation::Unspecified;
		break;
	}

	hwcData.slot = blade->slotNumber;
	hwcData.sysType = static_cast<uint16_t>(blade->systemType);
	hwcData.sysNo = blade->systemNumber;
	hwcData.fbn = static_cast<uint16_t>(blade->functionalBoardName);
	hwcData.side = static_cast<uint16_t>(blade->side);
	hwcData.seqNo = blade->sequenceNumber;
	hwcData.ipEthA = getIpAddress(blade->ipAddressEthA);
	hwcData.ipEthB = getIpAddress(blade->ipAddressEthB);
	hwcData.aliasEthA = getIpAddress(blade->ipAliasEthA);
	hwcData.aliasEthB = getIpAddress(blade->ipAliasEthB);
	hwcData.netmaskAliasEthA = getIpAddress(blade->aliasNetmaskEthA);
	hwcData.netmaskAliasEthB = getIpAddress(blade->aliasNetmaskEthB);
	hwcData.dhcpMethod = static_cast<uint16_t>(blade->dhcpOption);
	
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();

	const ACS_CS_ImShelf *shelf = dynamic_cast<const ACS_CS_ImShelf *>(model->getParentOf(blade->rdn));

	if(shelf)
	{
		hwcData.magazine = getIpAddress(shelf->address);
	} else {
		hwcData.magazine = ACS_CS_API_HWCTableData_R1::UNSPECIFIED_MAGAZINE;
	}

	hwcData.sysId = ACS_CS_API_HWCTableData_R1::UNSPECIFIED_SYSID;
	if(CPBLADE_T == blade->type)
	{
		const ACS_CS_ImCpBlade *cpBlade = dynamic_cast<const ACS_CS_ImCpBlade *>(blade);
		if(cpBlade)
		{
			if (cpBlade->systemType == DOUBLE_SIDED_CP) {

				set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;
				set<const ACS_CS_ImBase *> dualSidedCpObjects;
				model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

				bool cpRefFound = false;

				for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end() && cpRefFound == false; dualSidedCpIt++)
				{
					const ACS_CS_ImBase *base = *dualSidedCpIt;
					const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

					if (cp)
					{
						set<string> cpBladeRefTo = cp->axeCpBladeRefTo;
						set<string>::iterator refIt;

						for (refIt = cpBladeRefTo.begin(); refIt != cpBladeRefTo.end() && cpRefFound == false; refIt++) {
							string ref = *refIt;
							if (ref.compare(cpBlade->rdn.c_str()) == 0) {
								hwcData.sysId = cp->systemIdentifier;
								cpRefFound = true;
								break;
							}

						}
					}
				}

			} else if (cpBlade->systemType == SINGLE_SIDED_CP){
				set<const ACS_CS_ImBase *>::iterator clusterCpIt;
				set<const ACS_CS_ImBase *> clusterCpObjects;
				model->getObjects(clusterCpObjects, CLUSTERCP_T);


				for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++) {

					const ACS_CS_ImBase *base = *clusterCpIt;
					const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

					if (cp) {
						string cpBladeRefTo = cp->axeCpBladeRefTo;
						if (cpBladeRefTo.compare(cpBlade->rdn.c_str()) == 0) {
							hwcData.sysId = cp->systemIdentifier;
							break;
						}

					}
				}

			}
		}
	}

	return hwcData;
}

uint32_t ACS_CS_HWCHandler::getIpAddress(const std::string &address)
{
	uint32_t intAddress = ACS_CS_API_HWCTableData_R1::UNSPECIFIED_ADDRESS;
	if(address != "")
	{
		inet_pton(AF_INET, address.c_str(), &intAddress);
		if(intAddress)
		{
			intAddress = ntohl(intAddress);
		}
	}
	return intAddress;
}

int ACS_CS_HWCHandler::createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::createBasicResponse()\n"
        "Entering method\n"));

    // Suppress warning for not freeing header and body
    // which we shouldn't since they are passed to the PDU
    // and freed there
    //lint --e{429}

    if (pdu == 0)   // Check for valid PDU
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createBasicResponse()\n"
            "Error: Invalid PDU\n"));

        return -1;
    }

    ACS_CS_Header * header = new ACS_CS_Header();   // Create header

    if (header == 0)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createBasicResponse()\n"
            "Error: Invalid header\n"));

        return -1;
    }

    const ACS_CS_Header * oldHeader = 0;

    if (pdu && pdu->getHeader())
    	oldHeader = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

    CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

    // Get version from incoming package
    if (oldHeader)
    {
        version = oldHeader->getVersion();
    }

    ACS_CS_BasicResponse * body = 0;

    // Create body
    if (type == ACS_CS_Protocol::Primitive_SynchTableResponse)
        body = new ACS_CS_HWC_SynchTableResponse();
    else if (type == ACS_CS_Protocol::Primitive_GetBoardListResponse)
    {
        // This function can only be used for error responses for this primitive
        if (result == ACS_CS_Protocol::Result_Success)
        {
            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                "(%t) ACS_CS_HWCHandler::createBasicResponse()\n"
                "Error: Wrong function call, type = %d, result = %d\n",
                type,
                result));

            delete header;

            return -1;
        }

        body = new ACS_CS_HWC_GetBoardListResponse();
    }
    else if (type == ACS_CS_Protocol::Primitive_GetValueResponse)
    {
        // This function can only be used fo error responses for this primitive
        if (result == ACS_CS_Protocol::Result_Success)
        {
            ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
                "(%t) ACS_CS_HWCHandler::createBasicResponse()\n"
                "Error: Wrong function call, type = %d, result = %d\n",
                type,
                result));

            delete header;

            return -1;
        }

        body = new ACS_CS_HWC_GetValueResponse();
    }

    if (body == 0)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createBasicResponse()\n"
            "Error: Invalid body\n"));

        delete header;
        return -1;
    }

    // Set all header values
    header->setVersion(version);
    header->setScope(Scope_HWCTable);
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

int ACS_CS_HWCHandler::createGetBoardListResponse (ACS_CS_PDU *pdu, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result, uint16_t logicalClock, uint16_t *boardList, int size)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::createGetBoardListResponse()\n"
        "Entering method\n"));

    // Suppress warning for not freeing header and body
    // which we shouldn't since they are passed to the PDU
    //lint --e{429}

    if (pdu == 0)   // Check for valid PDU
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createGetBoardListResponse()\n"
            "Error: Invalid PDU\n"));

        return -1;
    }

    ACS_CS_Header * header = new ACS_CS_Header();   // Create header

    if (header == 0)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createGetBoardListResponse()\n"
            "Error: Invalid header\n"));

        return -1;
    }

    const ACS_CS_Header * oldHeader = 0;

    if (pdu && pdu->getHeader())
    	oldHeader = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

    CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

    // Get version from incoming package
    if (oldHeader)
    {
        version = oldHeader->getVersion();
    }

    header->setVersion(version);                            // and set all values
    header->setScope(Scope_HWCTable);
    header->setPrimitive(Primitive_GetBoardListResponse);
    header->setRequestId(requestId);

    ACS_CS_HWC_GetBoardListResponse * body = new ACS_CS_HWC_GetBoardListResponse(); // Create body

    if (body == 0)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createGetBoardListResponse()\n"
            "Error: Invalid body\n"));

        delete header;
        return -1;
    }

    body->setResultCode(result);                                // set result

    if (boardList)                                          // If there is a list to return
    {
        header->setTotalLength(header->getHeaderLength() + 6 + (size * 2) ); // set values
        body->setLogicalClock(logicalClock);
        (void) body->setResponseArray(boardList, size);
    }
    else                                                    // No list to return
    {
        if (result == ACS_CS_Protocol::Result_Success)
        {
            header->setTotalLength(header->getHeaderLength() + 6);
            body->setLogicalClock(logicalClock);
        }
        else
        {
            header->setTotalLength(header->getHeaderLength() + 2);
        }
    }

    // Update PDU
    pdu->setHeader(header);
    pdu->setPrimitive(body);

    return 0;
}

int ACS_CS_HWCHandler::createGetValueResponse (ACS_CS_PDU *pdu, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result, uint16_t logicalClock, const char  *response, int length)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::createGetValueResponse()\n"
        "Entering method\n"));

    // Suppress warning for not freeing header and body
    // which we shouldn't since they are passed to the PDU
    //lint --e{429}

    if (pdu == 0)   // Check for valid PDU
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createGetValueResponse()\n"
            "Error: Invalid PDU\n"));

        return -1;
    }

    ACS_CS_Header * header = new ACS_CS_Header();   // Create header

    if (header == 0)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createGetValueResponse()\n"
            "Error: Invalid header\n"));

        return -1;
    }

    const ACS_CS_Header * oldHeader = 0;

    if (pdu && pdu->getHeader())
    	oldHeader = dynamic_cast<const ACS_CS_Header *>(pdu->getHeader()); // Get header from PDU

    CS_Version_Identifier version = ACS_CS_Protocol::Latest_Version;

    // Get version from incoming package
    if (oldHeader)
    {
        version = oldHeader->getVersion();
    }

    header->setVersion(version);                            // and set all values
    header->setScope(Scope_HWCTable);
    header->setPrimitive(Primitive_GetValueResponse);
    header->setRequestId(requestId);

    ACS_CS_HWC_GetValueResponse * body = new ACS_CS_HWC_GetValueResponse();     // Create body

    if (body == 0)
    {
        ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
            "(%t) ACS_CS_HWCHandler::createGetValueResponse()\n"
            "Error: Invalid body\n"));

        delete header;
        return -1;
    }

    body->setResultCode(result);                            // set result


    if (response)                                           // If there is a response to return
    {
        header->setTotalLength(header->getHeaderLength() + 6 + length);   // set values
        body->setLogicalClock(logicalClock);
        (void) body->setResponse(response, length);
    }
    else                                                    // No response to return
    {
        header->setTotalLength(header->getHeaderLength() + 2);
    }

    // Update PDU
    pdu->setHeader(header);
    pdu->setPrimitive(body);

    return 0;
}


ACS_CS_DHCP_Info ACS_CS_HWCHandler::getDhcpInfo (ACS_CS_Protocol::CS_Network_Identifier network, ACS_CS_TableEntry &board)
{
    ACS_CS_TRACE((ACS_CS_HWCHandler_TRACE,
        "(%t) ACS_CS_HWCHandler::getDhcpInfo()\n"
        "Entering method\n" ));

    ACS_CS_DHCP_Info client;
    ACS_CS_Protocol::CS_Attribute_Identifier macIdentifier =
        ACS_CS_Protocol::Attribute_NotSpecified;

    if (network == ACS_CS_Protocol::Eth_A)
    {
        macIdentifier = ACS_CS_Protocol::Attribute_MAC_EthA;
    }
    else if (network == ACS_CS_Protocol::Eth_B)
    {
        macIdentifier = ACS_CS_Protocol::Attribute_MAC_EthB;
    }

    // Attribute pointers
    ACS_CS_Attribute magazine = board.getValue(ACS_CS_Protocol::Attribute_Magazine);
    ACS_CS_Attribute slot = board.getValue(ACS_CS_Protocol::Attribute_Slot);
    ACS_CS_Attribute fbn = board.getValue(ACS_CS_Protocol::Attribute_FBN);
    ACS_CS_Attribute side = board.getValue(ACS_CS_Protocol::Attribute_Side);
    ACS_CS_Attribute macAtt = board.getValue(macIdentifier);
    ACS_CS_Attribute dhcpAtt = board.getValue(ACS_CS_Protocol::Attribute_DHCP);
    ACS_CS_Attribute ipEthA = board.getValue(ACS_CS_Protocol::Attribute_IP_EthA);
    ACS_CS_Attribute ipEthB = board.getValue(ACS_CS_Protocol::Attribute_IP_EthB);
    ACS_CS_Attribute aliasEthA = board.getValue(ACS_CS_Protocol::Attribute_Alias_EthA);
    ACS_CS_Attribute aliasEthB = board.getValue(ACS_CS_Protocol::Attribute_Alias_EthB);
    ACS_CS_Attribute sysType = board.getValue(ACS_CS_Protocol::Attribute_SysType);

    if ( (magazine.getIdentifier() == Attribute_NotSpecified)
        || (slot.getIdentifier() == Attribute_NotSpecified)
        || (fbn.getIdentifier() == Attribute_NotSpecified)
        ||
        (dhcpAtt.getIdentifier() == Attribute_NotSpecified)
        || (ipEthA.getIdentifier() == Attribute_NotSpecified)
        || (ipEthB.getIdentifier() == Attribute_NotSpecified))  // Check for valid pointers
        return client;
    else if (magazine.getValueLength() != 4)            // Check value length
        return client;
    else if (slot.getValueLength() != 2)
        return client;
    else if (fbn.getValueLength() != 2)
        return client;
    else if (dhcpAtt.getValueLength() != 2)
        return client;

    uint16_t sysId = ACS_CS_EntryCounter::getSysId(&board);

    if (sysId != ILLEGAL_SYSID)
    {
        client.setSysId(sysId);
    }

    // Get system type
    if (sysType.getIdentifier() != Attribute_NotSpecified)
    {
        char sysBuff[2];
        uint16_t * tmpPtr = 0;
        uint16_t tmpSysType = 0;

        if (sysType.getValueLength() == 2)
        {
            (void) sysType.getValue(sysBuff, 2);
            tmpPtr = reinterpret_cast<uint16_t *> (sysBuff);
            tmpSysType = *tmpPtr;

            if ( (tmpSysType == ACS_CS_Protocol::SysType_BC) ||
                (tmpSysType == ACS_CS_Protocol::SysType_CP) )
            {
                client.setSystemType(tmpSysType);
            }
        }
    }

    // Create assignment struct
    IPAssignment assignment;
    assignment.iPEthA = 0;
    assignment.iPEthB = 0;
    assignment.aliasEthA = 0;
    assignment.aliasEthB = 0;
	

    // Get IP addresses

    if (network == ACS_CS_Protocol::Eth_A)
    {
        (void) ipEthA.getValue(reinterpret_cast<char *>(&assignment.iPEthA), 4);

        client.setScope(ACS_CS_NS::DHCP_Scope_EthA);
        client.setNetwork(ACS_CS_Protocol::Eth_A);
        client.setIP(assignment.iPEthA);
		
		(void) aliasEthA.getValue(reinterpret_cast<char *>(&assignment.aliasEthA), 4);

		client.setScope(ACS_CS_NS::DHCP_Scope_EthA);
		client.setNetwork(ACS_CS_Protocol::Eth_A);
		client.setIP(assignment.aliasEthA);
				
    }
    else if (network == ACS_CS_Protocol::Eth_B)
    {
        (void) ipEthB.getValue(reinterpret_cast<char *>(&assignment.iPEthB), 4);

        client.setScope(ACS_CS_NS::DHCP_Scope_EthB);
        client.setNetwork(ACS_CS_Protocol::Eth_B);
        client.setIP(assignment.iPEthB);
				
		(void) aliasEthB.getValue(reinterpret_cast<char *>(&assignment.aliasEthB), 4);

		client.setScope(ACS_CS_NS::DHCP_Scope_EthB);
		client.setNetwork(ACS_CS_Protocol::Eth_B);
		client.setIP(assignment.aliasEthB);
    }

    char magAddress[4];                         // Buffer to hold magazine address
    char slotAddress[2];                        // Buffer to hold slot position
    char fbnArr[2];                             // Buffer to hold FBN
    char dhcpValue[2];                          // Buffer to hold DHCP method
    char * macArr = 0;                          // Buffer to hold MAC
    int macLength = 0;                          // Length of MAC buffer
    ostringstream os;                           // Stream to build up the hardware identifier
    ACS_CS_Protocol::CS_Side_Identifier sideId = ACS_CS_Protocol::Side_NotSpecified;

    // Get values from the different attribute objects
    (void) magazine.getValue(magAddress, 4);
    (void) slot.getValue(slotAddress, 2);
    (void) fbn.getValue(fbnArr, 2);
    (void) dhcpAtt.getValue(dhcpValue, 2);

    // Get MAC A
    if (macAtt.getIdentifier() != Attribute_NotSpecified)
    {
        macLength = macAtt.getValueLength();

        if (macLength > 0)
        {
            macArr = new char[macLength];
            (void) macAtt.getValue(macArr, macLength);
        }
    }

    // Get side
    if (side.getIdentifier() != Attribute_NotSpecified)
    {
        if (side.getValueLength() == 2)
        {
            char sideArr[2];
            (void) side.getValue(sideArr, 2);
            uint16_t *sideValue = reinterpret_cast<uint16_t *> (sideArr);

            if (CS_ProtocolChecker::checkSide(*sideValue))
                sideId = static_cast<CS_Side_Identifier> (*sideValue);
        }
    }

    // Cast values
    uint32_t magNo = *( reinterpret_cast<uint32_t *>(magAddress) );
    uint16_t slotNo = *( reinterpret_cast<uint16_t *>(slotAddress) );
    uint16_t tempFbn = *( reinterpret_cast<uint16_t *>(fbnArr) );
    uint16_t tempDhcp = *(reinterpret_cast<uint16_t *>(dhcpValue) );
    ACS_CS_Protocol::CS_FBN_Identifier fbnId = ACS_CS_Protocol::FBN_NotSpecified;

    if (ACS_CS_Protocol::CS_ProtocolChecker::checkFBN(tempFbn) )
        fbnId = static_cast<ACS_CS_Protocol::CS_FBN_Identifier> (tempFbn);

    ACS_CS_Protocol::CS_DHCP_Method_Identifier dhcpMethod =
        ACS_CS_Protocol::DHCP_NotSpecified;

    // Check for valid DHCP attribute and cast to correct type
    if ( ACS_CS_Protocol::CS_ProtocolChecker::checkDHCP(tempDhcp) )
        dhcpMethod = static_cast<ACS_CS_Protocol::CS_DHCP_Method_Identifier>(tempDhcp);

    // Update DHCP info for ethA

    if (fbnId == ACS_CS_Protocol::FBN_SCBRP)
        client.setSCBRP(true);
    else
        client.setSCBRP(false);

    if (macArr)
    {
        client.setMAC(reinterpret_cast<char *>(macArr), macLength);
        client.setPersistent(true);
    }

    client.setDHCPMethod(dhcpMethod);
    client.setMagAddress(magNo);
    client.setSlot(slotNo);
    client.setFBN(fbnId);
    client.setSide(sideId);
    client.setComment("In use. Do not remove.");

    // Calculate hardware identifier
	client.calcOption61Id(fbnId);

    if (macArr)
        delete [] macArr;

    return client;
}

void ACS_CS_HWCHandler::setServiceHandler(ACS_CS_ServiceHandler* serviceHandler)
{
	this->serviceHandler = serviceHandler;
}

uint32_t ACS_CS_HWCHandler::getIpAddress(const ACS_CS_Attribute *attribute)
{

	int addrLength = attribute->getValueLength();
	char* addrChar = new char[addrLength + 1];
	memset(addrChar, 0, (addrLength + 1));
	attribute->getValue(addrChar, addrLength);
	uint32_t tempValue = *(reinterpret_cast<uint32_t*>(addrChar));
	delete [] addrChar;

	return tempValue;
}

uint16_t ACS_CS_HWCHandler::getShortAttr(const ACS_CS_Attribute *attribute)
{
	uint16_t val = 0;

	if (attribute->getValueLength() == sizeof(val))
	{
		attribute->getValue(reinterpret_cast<char *>(&val), sizeof(val));
	}

	return val;
}

string ACS_CS_HWCHandler::getStringAttr(const ACS_CS_Attribute *attribute)
{
	string addr = "";

	int addrLength = attribute->getValueLength();	//get length of attribute value
	char* addrChar = new char[addrLength + 1];		//create char array to store attribute value
	memset(addrChar, 0, (addrLength + 1));			//initialize aliasChar
	attribute->getValue(addrChar, addrLength);

	addr = addrChar;
	delete [] addrChar;

	return addr;
}

