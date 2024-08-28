/*
 * printattributes.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: mann
 */

#include <iostream>

#include "ACS_CS_Internal_Table.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Protocol.h"
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using ACS_CS_Protocol::CS_ProtocolChecker;

void printAttribute(const ACS_CS_Attribute *attribute);

int main (int argc, char **argv) {

	ACS_CS_INTERNAL_API::ACS_CS_Internal_Table * table =
			ACS_CS_INTERNAL_API::ACS_CS_Internal_Table::createTableInstance();

	size_t numOfBoards = 0;
	uint16_t* boardIds;

	if (table->getEntries(ACS_CS_INTERNAL_API::Table_HWC, 0, &boardIds, numOfBoards))
	{
		for (size_t i = 0; i < numOfBoards; i++)
	    {
			uint16_t boardId = boardIds[i];
			size_t totalNumOfAttributes = 0;
			ACS_CS_Attribute **attributes = NULL;

			cout << "**************** BoardId " << boardId << " *******************" << endl;

			if (table->getAttributes(ACS_CS_INTERNAL_API::Table_HWC, boardId, &attributes, totalNumOfAttributes))
			{
				for (size_t j = 0; j < totalNumOfAttributes; j++)
			   {
					cout << "---------------------------" << endl;
					printAttribute(attributes[j]);
			   }
			}
	   }
	}
}

void printAttribute(const ACS_CS_Attribute *attribute) {

	char* tempBuffer = new char[sizeof(long)];

	attribute->getValue(tempBuffer, attribute->getValueLength());

	std::stringstream ss;
	uint16_t tempValue = 0;

	if (attribute && attribute->getValueLength()) {

		int id = attribute->getIdentifier();

		cout << "Attribute: " << id;

		switch(id){
			case 0: cout << " (Attribute_NotSpecified) " << endl; break;
			case 1: cout << " (Attribute_Magazine) " << endl; break;
			case 2:
				tempValue = *(reinterpret_cast<uint16_t*>(tempBuffer));
				ss << tempValue;
				cout << " (Attribute_Slot = " << ss.str() << ")"<< endl;
			break;

			case 3:
				cout << " (Attribute_SysType = ";
				tempValue = *(reinterpret_cast<uint16_t*>(tempBuffer));

				if (tempValue == ACS_CS_Protocol::SysType_BC)
				  cout << "BC";
				else if (tempValue == ACS_CS_Protocol::SysType_CP)
					cout <<  "CP";
				else if (tempValue == ACS_CS_Protocol::SysType_AP)
					cout << "AP";

				cout << ") " << endl;

			break;

			case 4: cout << " (Attribute_SysNo) " << endl; break;
			case 5:
				cout << " (Attribute_FBN = " ;

				tempValue = *(reinterpret_cast<uint16_t*>(tempBuffer));

				if (tempValue == ACS_CS_Protocol::FBN_SCBRP)
					cout << "SCB-RP";
				else if (tempValue == ACS_CS_Protocol::FBN_RPBIS)
					cout << "RPBI-S";
				else if (tempValue == ACS_CS_Protocol::FBN_CPUB)
					cout << "CPUB";
				else if (tempValue == ACS_CS_Protocol::FBN_MAUB)
					cout << "MAUB";
				else if (tempValue == ACS_CS_Protocol::FBN_APUB)
					cout << "APUB";
				else if (tempValue == ACS_CS_Protocol::FBN_Disk)
					cout << "Disk";
				else if (tempValue == ACS_CS_Protocol::FBN_DVD)
					cout << "DVD";
				else if (tempValue == ACS_CS_Protocol::FBN_GEA)
					cout << "GEA";
				else if (tempValue == ACS_CS_Protocol::FBN_GESB)
					cout << "GESB";

				cout << ") " << endl;

				break;
			case 6: cout << " (Attribute_Side) " << endl; break;
			case 7: cout << " (Attribute_SeqNo) " << endl; break;
			case 8: cout << " (Attribute_IP_EthA) " << endl; break;
			case 9: cout << " (Attribute_IP_EthB) " << endl; break;
			case 10: cout << " (Attribute_Alias_EthA) " << endl; break;
			case 11: cout << " (Attribute_Alias_EthB) " << endl; break;
			case 12: cout << " (Attribute_Netmask_Alias_EthA) " << endl; break;
			case 13: cout << " (Attribute_Netmask_Alias_EthB) " << endl; break;
			case 14: cout << " (Attribute_DHCP) " << endl; break;
			case 15: cout << " (Attribute_SysId) " << endl; break;
			case 97: cout << " (Attribute_Sync_Flag) " << endl; break;
			case 98: cout << " (Attribute_MAC_EthA) " << endl; break;
			case 99: cout << " (Attribute_MAC_EthB) " << endl; break;
			default: cout << "(Attribute_unknown)" << endl;

		}

//	      Attribute_Magazine           = 1,
//	      Attribute_Slot               = 2,
//	      Attribute_SysType            = 3,
//	      Attribute_SysNo              = 4,
//	      Attribute_FBN                = 5,
//	      Attribute_Side               = 6,
//	      Attribute_SeqNo              = 7,
//	      Attribute_IP_EthA            = 8,
//	      Attribute_IP_EthB            = 9,
//	      Attribute_Alias_EthA         = 10,
//	      Attribute_Alias_EthB         = 11,
//	      Attribute_Netmask_Alias_EthA = 12,
//	      Attribute_Netmask_Alias_EthB = 13,
//	      Attribute_DHCP               = 14,
//	      Attribute_SysId              = 15,
//	      Attribute_Sync_Flag          = 97,
//	      Attribute_MAC_EthA           = 98, // These are internal attributes not to be used
//	      Attribute_MAC_EthB           = 99, // by external clients. They are used for MAC-


		char * buffer = new char[attribute->getValueLength() + 1];
		attribute->getValue(buffer, attribute->getValueLength());
		buffer[attribute->getValueLength()] = 0;
		string data = CS_ProtocolChecker::binToString(buffer, attribute->getValueLength());
		cout << "Value: " << data << endl;
		delete [] buffer;

	}
}
