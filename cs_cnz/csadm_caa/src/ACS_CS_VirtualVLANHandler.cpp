/*
 * ACS_CS_VirtualVLANHandler.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: estevol
 */

#include "ACS_CS_VirtualVLANHandler.h"

#include "../inc/imm_mapper/ACS_CS_ImExternalNetwork.h"
#include "ACS_CS_ImInternalNetwork.h"
#include "ACS_CS_NetworkHandler.h"

#include "ACS_CS_Protocol.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_BasicResponse.h"

#include "ACS_CS_VLAN_GetVLANList.h"
#include "ACS_CS_VLAN_GetVLANListResponse.h"
#include "ACS_CS_VLAN_GetVLANAddress.h"
#include "ACS_CS_VLAN_GetVLANAddressResponse.h"
#include "ACS_CS_VLAN_GetVLANTable.h"
#include "ACS_CS_VLAN_GetVLANTableResponse.h"

#include "ACS_CS_Common_Util.h"

#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"

ACS_CS_Trace_TDEF(ACS_CS_VirtualVLANHandler_TRACE);

ACS_CS_VirtualVLANHandler::ACS_CS_VirtualVLANHandler():
		ACS_CS_VLANHandler()
{

}

ACS_CS_VirtualVLANHandler::~ACS_CS_VirtualVLANHandler()
{

}

int ACS_CS_VirtualVLANHandler::newTableOperationRequest(ACS_CS_ImModelSubset *subset)
{
	ACS_CS_TRACE((ACS_CS_VirtualVLANHandler_TRACE, "ACS_CS_VirtualVLANHandler::newTableOperationRequest()\n"
			"Entered newTableOperationRequest call"));

	set<const ACS_CS_ImBase *> vlanObj;

	set<const ACS_CS_ImBase *> networkObj;

	subset->getObjects(vlanObj, VLAN_T);
	subset->getObjects(networkObj, TRM_EXTERNALNETWORK_T);
	subset->getObjects(networkObj, TRM_INTERNALNETWORK_T);

	for (std::set<const ACS_CS_ImBase *>::iterator it = networkObj.begin(); networkObj.end() != it; ++it)
	{
		if ((*it)->action == ACS_CS_ImBase::MODIFY)
		{
			if ((*it)->type == TRM_EXTERNALNETWORK_T)
			{
				changeName<ACS_CS_ImExternalNetwork>(*it, Network::EXTERNAL_NETWORK);
			}
			else
			{
				changeName<ACS_CS_ImInternalNetwork>(*it, Network::INTERNAL_NETWORK);
			}
		}
	}

	if (vlanObj.size() > 0 || networkObj.size() > 0)
	{
		ACS_CS_TRACE((ACS_CS_VirtualVLANHandler_TRACE, "ACS_CS_VirtualVLANHandler::newTableOperationRequest()\n"
				"Incrementing clock \n"));
		logicalClock++;
		ACS_CS_TableHandler::setGlobalLogicalClock(ACS_CS_Protocol::Scope_VLAN, logicalClock);
	}

	return 0;
}


//int ACS_CS_VirtualVLANHandler::handleRequest (ACS_CS_PDU *pdu)
//{
//	ACS_CS_TRACE((ACS_CS_VirtualVLANHandler_TRACE, "(%t) ACS_CS_VirtualVLANHandler::handleRequest()\n" "Entering method\n"));
//
//	if (pdu == 0)	// Check for valid PDU
//	{
//		ACS_CS_TRACE((ACS_CS_VirtualVLANHandler_TRACE, "(%t) ACS_CS_VirtualVLANHandler::handleRequest()\n" "Error: Invalid PDU\n"));
//
//		return -1;
//	}
//
//	int error = 0;
//
//	const ACS_CS_HeaderBase * header = pdu->getHeader(); // Get header from PDU
//
//	if (header)
//	{
//		// Check which primitive that has been received and call handler function
//		switch(header->getPrimitiveId())
//		{
//		case ACS_CS_Protocol::Primitive_GetVLANAddress:
//			error = handleGetVLANAddress(pdu);
//			break;
//		case ACS_CS_Protocol::Primitive_GetVLANList:
//			error = handleGetVLANList(pdu);
//			break;
//		case ACS_CS_Protocol::Primitive_GetVLANTable:
//			error = handleGetVLANTable(pdu);
//			break;
//		default:
//			ACS_CS_TRACE((ACS_CS_VirtualVLANHandler_TRACE, "(%t) ACS_CS_VirtualVLANHandler::handleRequest()\n" "Error: Unknown primitive, id = %d\n", header->getPrimitiveId()));
//			error = -1;
//			break;
//		}
//	}
//
//	return error;
//
//}


bool ACS_CS_VirtualVLANHandler::loadTable()

{
	// The VLAN table isn't saved to disk.
	// It is re-created on startup every time.

	return true;
}


template<class ImNetwork_t> void ACS_CS_VirtualVLANHandler::changeName(const ACS_CS_ImBase* baseObj, Network::Type type)
{
	const ImNetwork_t * imNetwork = dynamic_cast<const ImNetwork_t *>(baseObj);
	if (imNetwork)
	{
		int index = atoi(ACS_CS_ImUtils::getIdValueFromRdn(imNetwork->networkId).c_str());

		ACS_CS_NetworkHandler::getInstance()->changeNetworkName(index, type, imNetwork->name);
	}
}


bool ACS_CS_VirtualVLANHandler::populateVLANTable(vlanVector& vlanTable, int tableVersion, Network::Domain domain)
{
	bool result = true;

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();

	if (model)
	{
		std::set<const ACS_CS_ImBase * > networkObjs;
		std::set<const ACS_CS_ImBase * > vlanObjs;

		model->getObjects(networkObjs, TRM_INTERNALNETWORK_T);

		if (ACS_CS_Protocol::VLANVersion_0 != tableVersion)
		{
			//VLAN Table version 0 includes only internal networks
			model->getObjects(networkObjs, TRM_EXTERNALNETWORK_T);
		}

		for (std::set<const ACS_CS_ImBase *>::iterator it = networkObjs.begin(); networkObjs.end() != it; ++it)
		{
			if (TRM_EXTERNALNETWORK_T == (*it)->type)
			{
				insert<ACS_CS_ImExternalNetwork>(vlanTable, *it, Network::EXTERNAL_NETWORK, domain, model);
			}
			else
			{
				insert<ACS_CS_ImInternalNetwork>(vlanTable, *it, Network::INTERNAL_NETWORK, domain, model);
			}
		}

		delete(model);
	}

	return result;
}


template<class ImNetwork_t> void ACS_CS_VirtualVLANHandler::insert(vlanVector & vlanTable, const ACS_CS_ImBase * baseObj, Network::Type type, Network::Domain domain, ACS_CS_ImModel * model)
{
	const ImNetwork_t * imNetwork = dynamic_cast<const ImNetwork_t *>(baseObj);
	const ACS_CS_ImVlan * imLegacyVlan = getLegacyVlanObj(imNetwork->name, model);

	if (isValidEntry<ImNetwork_t>(imNetwork, imLegacyVlan))
	{
		ACS_CS_VLAN_DATA vlanData;
		memset(&vlanData, 0, sizeof(vlanData));

		//Fill name
		strncpy(vlanData.vlanName, imNetwork->name.c_str(), imNetwork->name.length());

		if (imLegacyVlan)
		{
			// Convert VLAN address
			struct in_addr convaddr;
			memset(&convaddr, 0, sizeof convaddr);
			inet_pton(AF_INET, imLegacyVlan->networkAddress.c_str(), &convaddr);
			vlanData.vlanAddress = ntohl(convaddr.s_addr);


			// Convert VLAN netmask
			memset(&convaddr, 0, sizeof convaddr);
			inet_pton(AF_INET,  imLegacyVlan->netmask.c_str(), &convaddr);
			vlanData.vlanNetmask = ntohl(convaddr.s_addr);


			// Convert VLAN stack
			switch(imLegacyVlan->stack)
			{
			case KIP:
				vlanData.vlanStack = ACS_CS_Protocol::Stack_KIP;
				break;
			case TIP:
				vlanData.vlanStack = ACS_CS_Protocol::Stack_TIP;
				break;
			default:
				vlanData.vlanStack = ACS_CS_Protocol::Stack_Undefined;
				break;
			}

			// Convert VLAN type
			switch (imLegacyVlan->vlanType)
			{
			case APZ:
				vlanData.vlanType = ACS_CS_Protocol::Type_APZ;
				break;
			case APT:
				vlanData.vlanType = ACS_CS_Protocol::Type_APT;
				break;
			default:
				vlanData.vlanType = ACS_CS_Protocol::Type_Undefined;
				break;
			}

			vlanData.pcp = imLegacyVlan->pcp;
			vlanData.vlanTag = imLegacyVlan->vlanTag;
		}
		else
		{
			if(!strcmp(vlanData.vlanName,"INT-SIG")) // HW14669 Fix
			{
				vlanData.vlanType = ACS_CS_Protocol::Type_APT;
			}
			else 
			{
				vlanData.vlanType = (Network::INTERNAL_NETWORK == type)? ACS_CS_Protocol::Type_APZ: ACS_CS_Protocol::Type_APT;
			}

			vlanData.vlanStack = (ACS_CS_Protocol::Type_APT == vlanData.vlanType)? ACS_CS_Protocol::Stack_Undefined: ACS_CS_Protocol::Stack_KIP;
			vlanData.pcp = ACS_CS_Protocol::DEFAULT_PCP;
			vlanData.vlanTag = ACS_CS_Protocol::DEFAULT_VLAN_TAG;
		}

		std::string nicNames[Network::DOMAIN_NR];
		for (std::set<std::string>::iterator it = imNetwork->nicName.begin(); imNetwork->nicName.end() != it; ++it)
		{
			std::string nicDomainStr, nicName;
			ACS_CS_NS::ACS_CS_Util::splitString(nicDomainStr, nicName, *it, "_");

			int nicDomain = atoi(nicDomainStr.c_str());

			if (nicDomain >= 0 && nicDomain < Network::DOMAIN_NR)
			{
				nicNames[nicDomain] = nicName;
			}

		}

		if (Network::DOMAIN_NR == domain)
		{
			//No domain provided, hence add to the list without nicName
			vlanTable.push_back(vlanData);
		}
		else if (domain < Network::DOMAIN_NR && !nicNames[domain].empty())
		{
			strncpy(vlanData.vNICName, nicNames[domain].c_str(), nicNames[domain].length());
			vlanTable.push_back(vlanData);
		}

	}

}

const ACS_CS_ImVlan* ACS_CS_VirtualVLANHandler::getLegacyVlanObj(const std::string & name, ACS_CS_ImModel * model)
{
	const ACS_CS_ImVlan * imVlan = 0;

	if (model)
	{
		std::string vlanDn(ACS_CS_ImmMapper::ATTR_VLAN_ID + "=" + name + "," + ACS_CS_ImmMapper::RDN_VLANCATEGORY);

		imVlan = dynamic_cast<const ACS_CS_ImVlan *>(model->getObject(vlanDn.c_str()));
	}

	return imVlan;
}

template<class ImNetwork_t> bool ACS_CS_VirtualVLANHandler::isValidEntry(const ImNetwork_t * imNetwork, const ACS_CS_ImVlan * imLegacyVlan)
{

	if (!imNetwork || UNLOCKED != imNetwork->adminState)
	{
		return false;
	}

	if (imNetwork->name.length() > ACS_CS_NS::MAX_VLAN_NAME_LENGTH)
	{
		return false;
	}

	for (std::set<std::string>::iterator it = imNetwork->nicName.begin(); imNetwork->nicName.end() != it; ++it)
	{
		if (it->length() > ACS_CS_NS::MAX_VNIC_NAME_LENGTH)
		{
			return false;
		}
	}

	if (imLegacyVlan)
	{
		if (!ACS_CS_Common_Util::isIpAddress(imLegacyVlan->networkAddress) || !ACS_CS_Common_Util::isIpAddress(imLegacyVlan->netmask))
		{
			return false;
		}

		switch(imLegacyVlan->stack)
		{
		case KIP:
		case TIP:
			break;
		default:
			if (imLegacyVlan->vlanType != APT)
				return false;
			break;
		}

		switch (imLegacyVlan->vlanType)
		{
		case APZ:
		case APT:
			break;
		default:
			return false;
		}

		if ( imLegacyVlan->pcp < ACS_CS_Protocol::MIN_PCP   || imLegacyVlan->pcp > ACS_CS_Protocol::MAX_PCP )
		{
			return false;
		}

		if ( imLegacyVlan->vlanTag > ACS_CS_Protocol::MAX_VLAN_TAG )
		{
			return false;
		}
	}

	return true;

}
