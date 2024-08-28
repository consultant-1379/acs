/*
 * ACS_CS_NativeVLANHandler.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: estevol
 */

#include "ACS_CS_NativeVLANHandler.h"


#include "ACS_CS_VLAN_GetVLANList.h"
#include "ACS_CS_VLAN_GetVLANListResponse.h"
#include "ACS_CS_VLAN_GetVLANAddress.h"
#include "ACS_CS_VLAN_GetVLANAddressResponse.h"
//qos start
#include "ACS_CS_VLAN_GetVLANTable.h"
#include "ACS_CS_VLAN_GetVLANTableResponse.h"

#include "ACS_CS_Trace.h"

ACS_CS_Trace_TDEF(ACS_CS_NativeVLANHandler_TRACE);

ACS_CS_NativeVLANHandler::ACS_CS_NativeVLANHandler():
		ACS_CS_VLANHandler()
{

}

ACS_CS_NativeVLANHandler::~ACS_CS_NativeVLANHandler()
{

}

bool ACS_CS_NativeVLANHandler::populateVLANTable(vlanVector& vlanTable,int tableVersion, Network::Domain)
{
	bool result = false;

	std::vector<ACS_CS_INTERNAL_API::VLAN> vlanList;
	getVLANList(vlanList);

	std::vector<ACS_CS_INTERNAL_API::VLAN>::iterator it;

	for(it = vlanList.begin(); it != vlanList.end(); ++it)
	{
		ACS_CS_VLAN_DATA vlanData;

		if((tableVersion == ACS_CS_Protocol::VLANVersion_0) && (it->vlanType != ACS_CS_Protocol::Type_APZ ))//vlanData.vlanType !=APZ  )
		{
			ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
					"ACS_CS_NativeVLANHandler::populateVLANTable()\n"
					"skipping loading vlandata as loadAllVlans is false and vlanType is APT or not APZ vlan VlanName =%s",vlanData.vlanName));
			continue;

		}
		result = convertTostruct(vlanData, *it);    //create a struct to hold the VLAN data for each VLAN

		if (result)
		{
			vlanTable.push_back(vlanData);
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
					"ACS_CS_NativeVLANHandler::populateVLANTable()\n"
					"Unable to convert VLAN data"));
			break;
		}
	}

	return result;
}

bool ACS_CS_NativeVLANHandler::getVLANList(std::vector<ACS_CS_INTERNAL_API::VLAN> &vlanList)
{


	ACS_CS_INTERNAL_API::VLAN tmpVLAN;
	//	char str[INET_ADDRSTRLEN];
	//	in_addr_t inNetworkFormat;

	ACS_CS_ImModel *model = 0;
	model = ACS_CS_ImRepository::instance()->getModelCopy();

	// Get children of VlanCategory
	if(model){

		bool invalidEntry = false;
		set<const ACS_CS_ImBase *>::iterator it;
		std::set<const ACS_CS_ImBase *> vlanObjects;
		model->getObjects(vlanObjects, VLAN_T);

		// Loop through CpObjects
		for(it = vlanObjects.begin(); it != vlanObjects.end(); it++)
		{
			int success = 0;
			struct in_addr convaddr;
			const ACS_CS_ImBase *base = *it;
			const ACS_CS_ImVlan* vlan = dynamic_cast<const ACS_CS_ImVlan*>(base);

			if (vlan){

				//////////////////////////////////////////////////////////////////////

				if(vlan->name.length() > 0){
					tmpVLAN.vlanName = vlan->name;
					ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
							"ACS_CS_Internal_Table::getVLANList()\n"
							"VLAN.vlanName = %s", tmpVLAN.vlanName.c_str()));
				}
				else {//invalid name
					invalidEntry = true;
				}

				//////////////////////////////////////////////////////////////////////

				if(!invalidEntry && vlan->networkAddress.length() > 0){

					//Validate the address
					memset(&convaddr, 0, sizeof convaddr);
					success = inet_pton(AF_INET, vlan->networkAddress.c_str(), &convaddr);

					if (success == 1){
						tmpVLAN.vlanAddress = vlan->networkAddress;
						ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
								"ACS_CS_Internal_Table::getVLANList()\n"
								"VLAN.vlanAddress = %s", tmpVLAN.vlanAddress.c_str()));
					}
					else { //invalid address
						invalidEntry = true;
					}
				}
				else { //not set
					invalidEntry = true;
				}

				//////////////////////////////////////////////////////////////////////

				if(!invalidEntry && vlan->netmask.length() > 0){

					//Validate the netmask
					memset(&convaddr, 0, sizeof convaddr);
					success = inet_pton(AF_INET, vlan->netmask.c_str(), &convaddr);
					uint32_t val = ntohl(convaddr.s_addr);
					if ((val & 0xFF) != 0)    //bitwise and between netmask and 0x000000FF must be 0,
						success = 0;

					if (success == 1){
						tmpVLAN.vlanNetmask = vlan->netmask;
						ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
								"ACS_CS_Internal_Table::getVLANList()\n"
								"VLAN.vlanNetmask = %s", tmpVLAN.vlanNetmask.c_str()));
					}
					else { //invalid netmask
						invalidEntry = true;
					}
				}
				else { //not set
					invalidEntry = true;
				}

				//////////////////////////////////////////////////////////////////////

				if(!invalidEntry ){

					switch(vlan->stack){
					case KIP:
						tmpVLAN.vlanStack = "KIP";
						break;
					case TIP:
						tmpVLAN.vlanStack = "TIP";
						break;
					default:
						tmpVLAN.vlanStack = "";


					}

					ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
							"ACS_CS_Internal_Table::getVLANList()\n"
							"VLAN.vlanStack = %s", tmpVLAN.vlanStack.c_str()));

				}

				//////////////////////////////////////////////////////////////////////////
				//qos start
				if((!invalidEntry )&& ( vlan->pcp>=0  && vlan->pcp < 8)){
					tmpVLAN.pcp = vlan->pcp;
					ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
							"ACS_CS_Internal_Table::getVLANList()\n"
							"tmpVLAN.pcp = %d",tmpVLAN.pcp));
				}
				else
				{
					ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
							"ACS_CS_Internal_Table::getVLANList()\n"
							"in pcp else \n"));
					invalidEntry = true;
				}
				///////////////////////////////////////////////////////////////////////////
				if(!invalidEntry && vlan->vlanType != UNDEF_VLANTYPE){
					tmpVLAN.vlanType = vlan->vlanType;
					ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
							"ACS_CS_Internal_Table::getVLANList()\n"
							"VLAN.vlanType = %d",tmpVLAN.vlanType));
				}
				else
				{
					ACS_CS_TRACE((ACS_CS_NativeVLANHandler_TRACE,
							"ACS_CS_Internal_Table::getVLANList()\n"
							"in vlan type else \n"));
					invalidEntry = true;
				}
				//qos end
				///////////////////////////////////////////////////////////////////////////
				if(!invalidEntry && (vlan->vlanTag <= ACS_CS_NS::MAX_VLAN_ID_RANGE)){
					tmpVLAN.vlanTag = vlan->vlanTag;
				}
				else { //invalid vlan tag
					invalidEntry = true;
				}
				if(!invalidEntry && vlan->vNICName.length() > 0){
					tmpVLAN.vNICName = vlan->vNICName;
				}
				else { // invalid vNIC name
					invalidEntry = true;
				}
				if (invalidEntry)
					invalidEntry = false;
				else
					vlanList.push_back(tmpVLAN);
			}
		}
	}

	delete model;
	return true;

}


bool ACS_CS_NativeVLANHandler::convertTostruct(ACS_CS_VLAN_DATA & vlanStruct,
		const ACS_CS_INTERNAL_API::VLAN & vlanParameter)
{
	memset(&vlanStruct, 0, sizeof vlanStruct);

	// Convert VLAN address
	struct in_addr convaddr;
	memset(&convaddr, 0, sizeof convaddr);
	int success = inet_pton(AF_INET, vlanParameter.vlanAddress.c_str(), &convaddr);
	vlanStruct.vlanAddress = ntohl(convaddr.s_addr);

	if (success <= 0)
	{
		return false;
	}

	// Convert VLAN netmask
	memset(&convaddr, 0, sizeof convaddr);
	success = inet_pton(AF_INET, vlanParameter.vlanNetmask.c_str(), &convaddr);
	vlanStruct.vlanNetmask = ntohl(convaddr.s_addr);

	if (success <= 0)
	{
		return false;
	}

	if (vlanParameter.vlanStack == "KIP")
	{
		vlanStruct.vlanStack = ACS_CS_Protocol::Stack_KIP;
	}
	else  if (vlanParameter.vlanStack == "TIP")
	{
		vlanStruct.vlanStack = ACS_CS_Protocol::Stack_TIP;
	}
	else
	{
		vlanStruct.vlanStack = ACS_CS_Protocol::Stack_Undefined;
		if(vlanParameter.vlanType != ACS_CS_Protocol::Type_APT)  //for APT vlans stack is not applicable
		{
			return false;
		}

	}

	if (vlanParameter.vlanName.length() <= ACS_CS_NS::MAX_VLAN_NAME_LENGTH)
	{
		strncpy(vlanStruct.vlanName, vlanParameter.vlanName.c_str(), vlanParameter.vlanName.length());
	}
	else
	{
		return false;
	}
	if(vlanParameter.pcp < 8)
	{
		vlanStruct.pcp = vlanParameter.pcp;
	}
	else
	{
		return false;
	}
	if(vlanParameter.vlanType == ACS_CS_Protocol::Type_APZ
			|| vlanParameter.vlanType == ACS_CS_Protocol::Type_APT)
	{
		vlanStruct.vlanType = vlanParameter.vlanType;
	}
	else
	{
		return false;
	}

	if (vlanParameter.vlanTag <= ACS_CS_NS::MAX_VLAN_ID_RANGE)
	{
		vlanStruct.vlanTag  = vlanParameter.vlanTag;
	}
	else
	{
		return false;
	}
	if (vlanParameter.vNICName.length() <= ACS_CS_NS::MAX_VNIC_NAME_LENGTH)
	{
		strncpy(vlanStruct.vNICName, vlanParameter.vNICName.c_str(), vlanParameter.vNICName.length());
	}
	else
	{
		return false;
	}


	return true;
}
