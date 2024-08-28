/*
 * ACS_CS_BladeHandler.cpp
 *
 *  Created on: 07/mag/2012
 *      Author: renato
 */

#include "ACS_CS_BladeHandler.h"
#include "ACS_CS_ApHandler.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImEntryIdHelper.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_TFTP_Configurator.h"

ACS_CS_BladeHandler::ACS_CS_BladeHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
		ACS_APGCC_CcbHandle &ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal)
{
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->className=0;
	this->parentName=0;
	this->attr=attr;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->base=base;
}


ACS_CS_BladeHandler::~ACS_CS_BladeHandler() {

	std::cout<<"DISTRUTTORE ACS_CS_BladeHandler\n";
	// TODO Auto-generated destructor stub
}


int ACS_CS_BladeHandler::create()
{

	int result=0;

	if (base->type == OTHERBLADE_T)
		result = otherBladeCreate();
	else if (base->type == CPBLADE_T)
		result = cpBladeCreate();
	else if (base->type == APBLADE_T)
		result = apBladeCreate();



	return result;

}

int ACS_CS_BladeHandler::modify()
{

	return 0;
}

int ACS_CS_BladeHandler::remove()
{
	ACS_CS_DEBUG(("CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	if (base->type == OTHERBLADE_T)
	{
		ACS_CS_ImOtherBlade *blade = dynamic_cast<ACS_CS_ImOtherBlade *> (base);
		if (blade && (blade->functionalBoardName == IPLB) && (ACS_CS_Protocol::CS_ProtocolChecker::checkIfAP1()==false))
		{
			return TC_NOTALLOWED_AP2;
		}
	}

	int architecture = NODE_UNDEFINED;
	ACS_CS_Registry::getNodeArchitecture(architecture);

	if (NODE_VIRTUALIZED == architecture)
	{
		ACS_CS_ImBlade *blade = dynamic_cast<ACS_CS_ImBlade *> (base);
		if (blade)
		{
			//The manual deleting from HardwareMgmt fragment is not allowed in Virtualized environment
			//Deleting is rejected due to the corresponding between a ComputeResource with a valid role assignement.
			if (checkComputeResourceBlade(blade->uuid))
				return TC_NOTALLOWED;
		}
	}

	return 0;
}

int ACS_CS_BladeHandler::otherBladeCreate()
{       
	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;

	int shelfArchitecture;
	ACS_CS_Registry::getNodeArchitecture(shelfArchitecture);

	ACS_CS_ImOtherBlade *blade = dynamic_cast<ACS_CS_ImOtherBlade *> (base);

	if (!blade)
	{
		return 1;
	}

	//Get a copy of all model
	ACS_CS_ImModel model(*ACS_CS_ImRepository::instance()->getModel());

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);

	//Apply current subset to model copy
	if (subset)
		model.applySubset(subset);
        if ((blade->functionalBoardName == IPLB) && (ACS_CS_Protocol::CS_ProtocolChecker::checkIfAP1()==false))
	{
	 return TC_NOTALLOWED_AP2;
	}
	if (blade->functionalBoardName == SCB_RP || blade->functionalBoardName == GESB) {
//		std::string etha(""), ethb("");

		bool ip_assigned = true;

		if ((blade->ipAddressEthA.empty() || blade->ipAddressEthA.compare("0.0.0.0") == 0) ||
				(blade->ipAddressEthB.empty() || blade->ipAddressEthB.compare("0.0.0.0") == 0))
		{
			ACS_CS_ImIPAssignmentHelper helper;
			ip_assigned = helper.assignIp(&model, blade);
		}

		if (!ip_assigned)
		{
			printf("Unable to assign IP address!\n");
			retVal = ACS_CC_FAILURE;
		}
		else
		{
			int eId = 0;
			eId = ACS_CS_ImEntryIdHelper::getNewEntryId();
			blade->entryId=eId;

			blade->dhcpOption=DHCP_CLIENT;

			ACS_CC_ImmParameter ipAddressEthA;
			ACS_CC_ImmParameter ipAddressEthB;
			ACS_CC_ImmParameter entryIdParam;
			ACS_CC_ImmParameter dhcpOptionParam;

			char attrnameid[]="entryId";
			void *values[] = {reinterpret_cast<void *>(&eId)};

			char attrnameDhcp[]="dhcpOption";
			int dhcpInt =blade->dhcpOption;
			void *dhcpValue[] = {reinterpret_cast<void *>(&dhcpInt)};

			dhcpOptionParam.attrName = attrnameDhcp;
			dhcpOptionParam.attrType = ATTR_INT32T;
			dhcpOptionParam.attrValuesNum = 1;
			dhcpOptionParam.attrValues = dhcpValue;

			entryIdParam.attrName = attrnameid;
			entryIdParam.attrType = ATTR_UINT32T;
			entryIdParam.attrValuesNum = 1;
			entryIdParam.attrValues = values;

			ipAddressEthA.attrName = const_cast<char*>("ipAddressEthA");
			ipAddressEthA.attrType = ATTR_STRINGT;
			ipAddressEthA.attrValuesNum = 1;
			void *valueRDN3a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthA.c_str())) };
			ipAddressEthA.attrValues = valueRDN3a;

			ipAddressEthB.attrName = const_cast<char*>("ipAddressEthB");
			ipAddressEthB.attrType = ATTR_STRINGT;
			ipAddressEthB.attrValuesNum = 1;
			void *valueRDN4a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthB.c_str()))};
			ipAddressEthB.attrValues = valueRDN4a;

			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(), &ipAddressEthA);

			if (retVal != ACS_CC_SUCCESS) {

				cout<< "============================================================================== modifyObjectAugmentCcb::ipAddressEthA ERROR"<< endl;
			}
			else
			{
				cout<< "============================================================================== modifyObjectAugmentCcb::ipAddressEthA::ACS_CC_SUCCESS "<< endl;

				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&ipAddressEthB);

				if (retVal != ACS_CC_SUCCESS)
				{
					cout<< "============================================================================== modifyObjectAugmentCcb::ipAddressEthB  ERROR"<< endl;

				}
				else
				{
					retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&entryIdParam);

					if (retVal != ACS_CC_SUCCESS)
					{
						cout<< "============================================================================== modifyObjectAugmentCcb::entryIdParam  ERROR"<< endl;
					}
					else
					{
						cout<< "============================================================================== modifyObjectAugmentCcb::entryIdParam:ACS_CC_SUCCESS "<< endl;

						retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&dhcpOptionParam);

						if (retVal != ACS_CC_SUCCESS)
						{
							cout<< "============================================================================== modifyObjectAugmentCcb::dhcpOptionParam  ERROR"<< endl;
						}
						else
						{
							cout<< "============================================================================== modifyObjectAugmentCcb::dhcpOptionParam:ACS_CC_SUCCESS "<< endl;
							//Add board IP address entry to static IP table (if needed)
							int ret = storeStaticIPToImm(blade,&model);
							if (ret != ACS_CC_SUCCESS)
								return ACS_CC_FAILURE;
							if (ret != ACS_CC_SUCCESS)
                                                                return ACS_CC_FAILURE;
							cout<< "============================================================================== ACS_CS_BladeHandler::storeStaticIPImm:ACS_CC_SUCCESS "<< endl;

						}
					}

				}
			}
			if (retVal != ACS_CC_SUCCESS)
			{
				return retVal;
			}

		}

	}
	else if ((blade->functionalBoardName == SCXB) || (blade->functionalBoardName == EPB1)
			|| (blade->functionalBoardName == EVOET) || (blade->functionalBoardName == CMXB) || (blade->functionalBoardName == SMXB))//|| (blade->functionalBoardName == IPTB)
	{

		bool ip_assigned = true;

		if ((blade->ipAddressEthA.empty() || blade->ipAddressEthA.compare("0.0.0.0") == 0) ||
				(blade->ipAddressEthB.empty() || blade->ipAddressEthB.compare("0.0.0.0") == 0))
		{
			ACS_CS_ImIPAssignmentHelper helper;
			ip_assigned = helper.assignIp(&model, blade);
		}
		

		if (!ip_assigned)
		{
			printf("Unable to assign IP address!\n");
			retVal = ACS_CC_FAILURE;
		}
		else
		{
			std::string defautPackageDN;

			if (!fetchDefaultSwPackageDn(defautPackageDN,blade->functionalBoardName))
			{
				int eId = 0;
				eId = ACS_CS_ImEntryIdHelper::getNewEntryId();

				blade->currentLoadModuleVersion = defautPackageDN;
				blade->entryId=eId;
				blade->dhcpOption=DHCP_CLIENT;
				blade->swVerType = 2; //default value

				ACS_CS_ImUtils::getImmAttributeInt(blade->currentLoadModuleVersion, "swVerType", blade->swVerType);

				printf("defautPackageDN=%s\n", defautPackageDN.c_str());

				ACS_CC_ImmParameter ipAddressEthA;
				ACS_CC_ImmParameter ipAddressEthB;
				ACS_CC_ImmParameter currentLoadName;
				ACS_CC_ImmParameter entryIdParam;
				ACS_CC_ImmParameter dhcpOptionParam;
				ACS_CC_ImmParameter ipAliasEthA;
				ACS_CC_ImmParameter ipAliasEthB;

				char attrnameid[] = "entryId";
				void *values[] = {reinterpret_cast<void *>(&eId)};

				char attrnameDhcp[]="dhcpOption";

				int dhcpInt = blade->dhcpOption;
				void *dhcpValue[] = {reinterpret_cast<void *>(&dhcpInt)};

				dhcpOptionParam.attrName = attrnameDhcp;
				dhcpOptionParam.attrType = ATTR_INT32T;
				dhcpOptionParam.attrValuesNum = 1;
				dhcpOptionParam.attrValues = dhcpValue;

				entryIdParam.attrName = attrnameid;
				entryIdParam.attrType = ATTR_UINT32T;
				entryIdParam.attrValuesNum = 1;
				entryIdParam.attrValues = values;

				ipAddressEthA.attrName = const_cast<char*>("ipAddressEthA");
				ipAddressEthA.attrType = ATTR_STRINGT;
				ipAddressEthA.attrValuesNum = 1;
				void *valueRDN3a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthA.c_str())) };
				ipAddressEthA.attrValues = valueRDN3a;

				ipAddressEthB.attrName = const_cast<char*>("ipAddressEthB");
				ipAddressEthB.attrType = ATTR_STRINGT;
				ipAddressEthB.attrValuesNum = 1;
				void *valueRDN4a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthB.c_str()))};
				ipAddressEthB.attrValues = valueRDN4a;
				
				currentLoadName.attrName =const_cast<char*>("currentLoadModuleVersion");
				currentLoadName.attrType = ATTR_NAMET;
				currentLoadName.attrValuesNum = 1;
				void *valueRDN5a[1] ={reinterpret_cast<void*>(const_cast<char*>(defautPackageDN.c_str()))};

				currentLoadName.attrValues = valueRDN5a;
				
				if(blade->functionalBoardName == SMXB)
				{							
					ipAliasEthA.attrName = const_cast<char*>("ipAliasEthA");
					ipAliasEthA.attrType = ATTR_STRINGT;
					ipAliasEthA.attrValuesNum = 1;
					void *valueRDN6a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAliasEthA.c_str())) };
					ipAliasEthA.attrValues = valueRDN6a;

					ipAliasEthB.attrName = const_cast<char*>("ipAliasEthB");
					ipAliasEthB.attrType = ATTR_STRINGT;
					ipAliasEthB.attrValuesNum = 1;
					void *valueRDN7a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAliasEthB.c_str()))};
					ipAliasEthB.attrValues = valueRDN7a;

					retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(), &ipAliasEthA);

					if (retVal != ACS_CC_SUCCESS)
						return retVal;
						
					retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(), &ipAliasEthB);

					if (retVal != ACS_CC_SUCCESS)
						return retVal;
				}

				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(), &ipAddressEthA);

				if (retVal != ACS_CC_SUCCESS)
					return retVal;

				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&ipAddressEthB);

				if (retVal != ACS_CC_SUCCESS)
					return retVal;

				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&currentLoadName);

				if (retVal != ACS_CC_SUCCESS)
					return retVal;
				
				
				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&entryIdParam);

				if (retVal != ACS_CC_SUCCESS)
					return retVal;
				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&dhcpOptionParam);

				if (retVal != ACS_CC_SUCCESS)
					return retVal;

				//Add board IP address entry to static IP table (if needed)
				int ret = storeStaticIPToImm(blade,&model);

				if (ret != ACS_CC_SUCCESS)
                                        return ACS_CC_FAILURE;

				if (ret != ACS_CC_SUCCESS)
                                        return ACS_CC_FAILURE;

			}
			else
			{

				cout << "ERROR: NO DEFAULT SOFTWARE INSTALLED " << endl;
				ACS_CS_ImUtils::setFaultyFbn(blade->functionalBoardName);
				return TC_NODEFAULTSOFTWARE;
			}
		}

	}
	else if(blade->functionalBoardName == IPTB || blade->functionalBoardName == IPLB)
	{
		//Assign IP Address
		bool ip_assigned = true;

		if ((blade->ipAddressEthA.empty() || blade->ipAddressEthA.compare("0.0.0.0") == 0) ||
				(blade->ipAddressEthB.empty() || blade->ipAddressEthB.compare("0.0.0.0") == 0))
		{
			ACS_CS_ImIPAssignmentHelper helper;
			ip_assigned = helper.assignIp(&model, blade);
		}

		if (!ip_assigned)
		{
			//Error
			char trace[256] = {0};
			snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] [ERROR] ACS_CS_ImIPAssignmentHelper::assignIp", __FILE__, __FUNCTION__, __LINE__);
			std::cout << trace << std::endl;
			retVal = ACS_CC_FAILURE;
		}
		else
		{

			//Trace
			{
				char trace[256] = {0};
				snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] creating IPTB blade %s",
						__FILE__, __FUNCTION__, __LINE__, blade->ipAddressEthA.c_str());
				std::cout << trace << std::endl;
			}

			std::string defautPackageDN;
			if (shelfArchitecture != NODE_VIRTUALIZED && (fetchDefaultSwPackageDn(defautPackageDN,blade->functionalBoardName) != 0))
			{
				char trace[256] = {0};
				snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] Cannot defautPackage for IPT Board",
						__FILE__, __FUNCTION__, __LINE__);
				std::cout << trace << std::endl;
				ACS_CS_ImUtils::setFaultyFbn(blade->functionalBoardName);
				return TC_NODEFAULTSOFTWARE;

			}
			else
			{
				//Success
				//IP address fetched
				int eId = ACS_CS_ImEntryIdHelper::getNewEntryId();

				blade->entryId = eId;

				if (shelfArchitecture != NODE_VIRTUALIZED)
				{
					blade->currentLoadModuleVersion = defautPackageDN;
				}
				blade->dhcpOption = DHCP_NORMAL;

				//Define IMM parameter: entry Id
				ACS_CC_ImmParameter entryIdParam;
				char attrnameid[]="entryId";
				void *values[] = {reinterpret_cast<void *>(&eId)};
				entryIdParam.attrName = attrnameid; // ?????? const_cast<char*>("entryId");
				entryIdParam.attrType = ATTR_UINT32T;
				entryIdParam.attrValuesNum = 1;
				entryIdParam.attrValues = values;
				{
					//Trace
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] (IPT) Define IMM parameter: entryId==%d"
							, __FILE__, __FUNCTION__, __LINE__, eId);
					std::cout << trace << std::endl;
				}

				//Add entryIdParam to transaction
				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&entryIdParam);
				if (retVal != ACS_CC_SUCCESS)
				{
					//Error
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] [ERROR] ACS_CS_ImIPAssignmentHelper::assignIp", __FILE__, __FUNCTION__, __LINE__);
					std::cout << trace << std::endl;
					return retVal;
				}

				//Define IMM parameter: IPNA Address
				ACS_CC_ImmParameter ipAddressEthA;
				ipAddressEthA.attrName = const_cast<char*>("ipAddressEthA");
				ipAddressEthA.attrType = ATTR_STRINGT;
				ipAddressEthA.attrValuesNum = 1;
				void *valueRDN3a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthA.c_str())) };
				ipAddressEthA.attrValues = valueRDN3a;
				{
					//Trace
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] (IPT) Define IMM parameter: ipAddressEthA==%s"
							, __FILE__, __FUNCTION__, __LINE__, blade->ipAddressEthA.c_str());
					std::cout << trace << std::endl;
				}

				//Add ipAddressEthA to transaction
				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(), &ipAddressEthA);
				if (retVal != ACS_CC_SUCCESS)
				{
					//Error
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] [ERROR] modifyObjectAugmentCcb (ipAddressEthA)", __FILE__, __FUNCTION__, __LINE__);
					std::cout << trace << std::endl;
					return retVal;
				}


				//Define IMM parameter: IPNB Address
				ACS_CC_ImmParameter ipAddressEthB;
				ipAddressEthB.attrName = const_cast<char*>("ipAddressEthB");
				ipAddressEthB.attrType = ATTR_STRINGT;
				ipAddressEthB.attrValuesNum = 1;
				void *valueRDN4a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthB.c_str()))};
				ipAddressEthB.attrValues = valueRDN4a;
				{
					//Trace
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] (IPT) Define IMM parameter: ipAddressEthB==%s"
							, __FILE__, __FUNCTION__, __LINE__, blade->ipAddressEthB.c_str());
					std::cout << trace << std::endl;
				}

				//Add ipAddressEthB to transaction
				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&ipAddressEthB);
				if (retVal != ACS_CC_SUCCESS)
				{
					//Error
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] [ERROR] modifyObjectAugmentCcb (ipAddressEthB)", __FILE__, __FUNCTION__, __LINE__);
					std::cout << trace << std::endl;
					return retVal;
				}

				//Define IMM parameter: DHCP Reservation type
				ACS_CC_ImmParameter dhcpOptionParam;
				char attrnameDhcp[]="dhcpOption";
				int dhcpInt =blade->dhcpOption;
				void *dhcpValue[] = {reinterpret_cast<void *>(&dhcpInt)};
				dhcpOptionParam.attrName = attrnameDhcp;
				dhcpOptionParam.attrType = ATTR_INT32T;
				dhcpOptionParam.attrValuesNum = 1;
				dhcpOptionParam.attrValues = dhcpValue;

				{
					//Trace
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] (IPT) Define IMM parameter: dhcpOption==%d"
							, __FILE__, __FUNCTION__, __LINE__, dhcpInt);
					std::cout << trace << std::endl;
				}

				//Add dhcpOptionParam to transaction
				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&dhcpOptionParam);
				if (retVal != ACS_CC_SUCCESS)
				{
					//Error
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] [ERROR] modifyObjectAugmentCcb (dhcpOptionParam)", __FILE__, __FUNCTION__, __LINE__);
					std::cout << trace << std::endl;
					return retVal;
				}

				//Define IMM parameter: currentLoadName
				ACS_CC_ImmParameter currentLoadName;
				currentLoadName.attrName =const_cast<char*>("currentLoadModuleVersion");
				currentLoadName.attrType = ATTR_NAMET;
				currentLoadName.attrValuesNum = 1;
				void *valueRDN5a[1] ={reinterpret_cast<void*>(const_cast<char*>(defautPackageDN.c_str()))};
				currentLoadName.attrValues = valueRDN5a;
				{
					//Trace
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] (IPT) Define IMM parameter: currentLoadModuleVersion==%s"
							, __FILE__, __FUNCTION__, __LINE__, defautPackageDN.c_str());
					std::cout << trace << std::endl;
				}

				//Add currentLoadName to transaction
				retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&currentLoadName);
				if (retVal != ACS_CC_SUCCESS)
				{
					//Error
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] [ERROR] modifyObjectAugmentCcb (currentLoadName)", __FILE__, __FUNCTION__, __LINE__);
					std::cout << trace << std::endl;
					return retVal;
				}

				//Add Board IP address entry to static IP table (if needed)
				int ret = storeStaticIPToImm(blade,&model);

				if (ret != ACS_CC_SUCCESS)
					return ACS_CC_FAILURE;


				//Trace
				{
					char trace[256] = {0};
					snprintf(trace, sizeof(trace) - 1, "[%s::%s@%d] IPTB blade created :-)",
							__FILE__, __FUNCTION__, __LINE__);
					std::cout << trace << std::endl;
				}
			}
		}
	}
	else
	{
		if(retVal == ACS_CC_SUCCESS){
			std::cout << "CREATION SUCCESS:\n";
		}
		else{
			std::cout << "CREATION FAILURE: COULD NOT ADD NECESSARY INFORMATION TO OBJECT (internal error)\n";
		}
		return retVal;
	}

	return retVal;

}

int ACS_CS_BladeHandler::cpBladeCreate()
{

	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;

	ACS_CS_ImCpBlade *blade = dynamic_cast<ACS_CS_ImCpBlade *> (base);

	if (!blade)
	{
		return 1;
	}

	//Get a copy of all model
	ACS_CS_ImModel model(*ACS_CS_ImRepository::instance()->getModel());

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);

	//Apply current subset to model copy
	if (subset)
		model.applySubset(subset);


	if(blade->functionalBoardName == CPUB || blade->functionalBoardName == MAUB || blade->functionalBoardName == RPBI_S)
	{
		int apzType =  ACS_CS_Registry::getAPZType();

		bool ip_assigned = true;

		if ((blade->ipAddressEthA.empty() || blade->ipAddressEthA.compare("0.0.0.0") == 0) ||
				(blade->ipAddressEthB.empty() || blade->ipAddressEthB.compare("0.0.0.0") == 0))
		{
			//For ApzType < 4 we should not assign IP address for CPUB and MAUB boards
			if ( apzType == 4 || blade->functionalBoardName == RPBI_S) {
				ACS_CS_ImIPAssignmentHelper helper;
				ip_assigned = helper.assignIp(&model, blade);
			}
		}

		if (!ip_assigned)
		{
			printf("NO BLADE to set!\n");
			retVal = ACS_CC_FAILURE;
		}
		else
		{
			std::string defautPackageDN;

			int eId = 0;
			eId = ACS_CS_ImEntryIdHelper::getNewEntryId();

			blade->entryId = eId;

			//Check that an Ip address hasn't been assigned yet (old configurations)
			//if (blade->ipAddressEthA.size() == 0 || blade->ipAddressEthA.compare("0.0.0.0") == 0 || blade->ipAddressEthB.size() == 0 || blade->ipAddressEthB.compare("0.0.0.0") == 0) {

			//Assign the correct DHCP option
			assignDHCPOption(blade, apzType);

			ACS_CC_ImmParameter ipAddressEthA;
			ACS_CC_ImmParameter ipAddressEthB;
			ACS_CC_ImmParameter entryIdParam;
			ACS_CC_ImmParameter dhcpOptionParam;

			char attrnameid[]="entryId";
			void *values[] = {reinterpret_cast<void *>(&eId)};

			char attrnameDhcp[]="dhcpOption";
			int dhcpInt =blade->dhcpOption;
			void *dhcpValue[] = {reinterpret_cast<void *>(&dhcpInt)};

			dhcpOptionParam.attrName = attrnameDhcp;
			dhcpOptionParam.attrType = ATTR_INT32T;
			dhcpOptionParam.attrValuesNum = 1;
			dhcpOptionParam.attrValues = dhcpValue;

			entryIdParam.attrName = attrnameid;
			entryIdParam.attrType = ATTR_UINT32T;
			entryIdParam.attrValuesNum = 1;
			entryIdParam.attrValues = values;

			ipAddressEthA.attrName = const_cast<char*>("ipAddressEthA");
			ipAddressEthA.attrType = ATTR_STRINGT;
			ipAddressEthA.attrValuesNum = 1;
			void *valueRDN3a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthA.c_str())) };
			ipAddressEthA.attrValues = valueRDN3a;
			
			ipAddressEthB.attrName = const_cast<char*>("ipAddressEthB");
			ipAddressEthB.attrType = ATTR_STRINGT;
			ipAddressEthB.attrValuesNum = 1;
			void *valueRDN4a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthB.c_str()))};
			ipAddressEthB.attrValues = valueRDN4a;


			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(), &ipAddressEthA);
			if (retVal != ACS_CC_SUCCESS)
				return retVal;
			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&ipAddressEthB);
			if (retVal != ACS_CC_SUCCESS)
				return retVal;
			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&entryIdParam);
			if (retVal != ACS_CC_SUCCESS)
				return retVal;
			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&dhcpOptionParam);
			if (retVal != ACS_CC_SUCCESS)
				return retVal;

			//Add CP board IP address entry to static IP table (if needed)
			retVal = storeStaticIPToImm(blade,&model);
			if (retVal != ACS_CC_SUCCESS)
				return ACS_CC_FAILURE;

		}
	}
	return retVal;
}

int ACS_CS_BladeHandler::apBladeCreate()
{

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
	if (!model)
		return 1;

	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;

	ACS_CS_ImApBlade *blade = dynamic_cast<ACS_CS_ImApBlade *> (base);

	if (!blade)
	{
		return 1;
	}

	if(blade->functionalBoardName == DVD || blade->functionalBoardName == DISK || blade->functionalBoardName == GEA) {
		ACS_CC_ImmParameter entryIdParam;

		int eId = 0;
		eId = ACS_CS_ImEntryIdHelper::getNewEntryId();
		blade->entryId=eId;

		char attrnameid[]="entryId";
		void *values[] = {reinterpret_cast<void *>(&eId)};

		entryIdParam.attrName = attrnameid;
		entryIdParam.attrType = ATTR_UINT32T;
		entryIdParam.attrValuesNum = 1;
		entryIdParam.attrValues = values;

		retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&entryIdParam);

		if (retVal != ACS_CC_SUCCESS)
			return retVal;
	}
	else if(blade->functionalBoardName == APUB)
	{

		bool ip_assigned = true;

		if ((blade->ipAddressEthA.empty() || blade->ipAddressEthA.compare("0.0.0.0") == 0) ||
				(blade->ipAddressEthB.empty() || blade->ipAddressEthB.compare("0.0.0.0") == 0))
		{
			ACS_CS_ImIPAssignmentHelper helper;
			ip_assigned = helper.assignIp(model, dynamic_cast<ACS_CS_ImBlade *>(base));
		}

		if (!ip_assigned)
		{
			printf("NO BLADE to set!\n");
			retVal = ACS_CC_FAILURE;
		}
		else
		{

			int eId = 0;
			eId = ACS_CS_ImEntryIdHelper::getNewEntryId();
			blade->entryId=eId;

			ACS_CC_ImmParameter ipAddressEthA;
			ACS_CC_ImmParameter ipAddressEthB;
			ACS_CC_ImmParameter entryIdParam;

			char attrnameid[]="entryId";
			void *values[] = {reinterpret_cast<void *>(&eId)};

			entryIdParam.attrName = attrnameid;
			entryIdParam.attrType = ATTR_UINT32T;
			entryIdParam.attrValuesNum = 1;
			entryIdParam.attrValues = values;

			ipAddressEthA.attrName = const_cast<char*>("ipAddressEthA");
			ipAddressEthA.attrType = ATTR_STRINGT;
			ipAddressEthA.attrValuesNum = 1;
			void *valueRDN3a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthA.c_str())) };
			ipAddressEthA.attrValues = valueRDN3a;

			ipAddressEthB.attrName = const_cast<char*>("ipAddressEthB");
			ipAddressEthB.attrType = ATTR_STRINGT;
			ipAddressEthB.attrValuesNum = 1;
			void *valueRDN4a[1] ={reinterpret_cast<void*>(const_cast<char*>(blade->ipAddressEthB.c_str()))};
			ipAddressEthB.attrValues = valueRDN4a;

			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(), &ipAddressEthA);
			if (retVal != ACS_CC_SUCCESS)
				return retVal;
			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&ipAddressEthB);
			if (retVal != ACS_CC_SUCCESS)
				return retVal;
			retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&entryIdParam);
			if (retVal != ACS_CC_SUCCESS)
				return retVal;

			//Add HW Version attribute
			int hwVersion = UNDEF_HWV;

			ACS_CS_Registry::getApgHwVersion(hwVersion);

				if (hwVersion != UNDEF_HWV) {
					blade->hwVersion = hwVersion;

					char attrnameHwVer[]="hwVersion";
					int hwVerInt = blade->hwVersion;
					void *hwVerValue[] = {reinterpret_cast<void *>(&hwVerInt)};

					ACS_CC_ImmParameter hwVerParam;

					hwVerParam.attrName = attrnameHwVer;
					hwVerParam.attrType = ATTR_INT32T;
					hwVerParam.attrValuesNum = 1;
					hwVerParam.attrValues = hwVerValue;

					retVal = modifyObjectAugmentCcb(ccbHandleVal,adminOwnwrHandlerVal, blade->rdn.c_str(),&hwVerParam);
					if (retVal != ACS_CC_SUCCESS)
						return retVal;

				}
		}
	}
	return retVal;
}


//ACS_CC_ReturnType ACS_CS_BladeHandler::figureOutEthaEthbAddress(std::string &etha,std::string &ethb,ACS_APGCC_CcbId ccbId)
//{
//
//	ACS_CC_ReturnType retVal = ACS_CC_SUCCESS;
//
//	ACS_CS_ImModel *all_model =const_cast<ACS_CS_ImModel *> (ACS_CS_ImRepository::instance()->getModel());
//
//	std::string ethaM(""), ethbM("");
//
//	ACS_CS_ImIPAssignmentHelper helper;
//
//	getMaxAddressFromPreviousBoard(ethaM,ethbM,ccbId);
//
//	uint32_t ipa = 0,ipb=0,ipa1 = 0,ipb1=0;
//
//	inet_pton(AF_INET, ethaM.c_str(), &ipa1);
//	inet_pton(AF_INET, ethbM.c_str(), &ipb1);
//
//	if (!helper.assignIp(all_model, etha, ethb))
//	{
//		retVal = ACS_CC_FAILURE;
//	}
//	else
//	{
//		inet_pton(AF_INET, etha.c_str(), &ipa);
//		inet_pton(AF_INET, ethb.c_str(), &ipb);
//
//		ipa = ntohl(ipa);
//		ipb = ntohl(ipb);
//
//		ipa1 = ntohl(ipa1);
//		ipb1 = ntohl(ipb1);
//
//
//		if(ipa1>=ipa)
//		{
//			while(!checkIpaddress(all_model,++ipa1)) ;
//			ipa=ipa1;
//		}
//
//		if(ipb1>=ipb)
//		{
//			while(!checkIpaddress(all_model,++ipb1)) ;
//			ipb=ipb1;
//		}
//
//		char str[INET_ADDRSTRLEN];
//		ipa = htonl(ipa);
//		inet_ntop(AF_INET, &(ipa), str, INET_ADDRSTRLEN);
//		etha=std::string(str);
//
//		ipb = htonl(ipb);
//		inet_ntop(AF_INET, &(ipb), str, INET_ADDRSTRLEN);
//		ethb=std::string(str);
//
//		//		printf("etha=%s\n",etha.c_str());
//		//		printf("ethb=%s\n",ethb.c_str());
//	}
//
//	return retVal;
//}
//
//
//
//int ACS_CS_BladeHandler::getMaxAddressFromPreviousBoard(std::string &ethaM,std::string &ethbM,ACS_APGCC_CcbId ccbId )
//{
//	ACS_CS_ImModel *model =const_cast<ACS_CS_ImModel *> (ACS_CS_ImRepository::instance()->getSubset(ccbId));
//
//	std::set<const ACS_CS_ImBase *> blades;
//
//	//	model->getObjects(blades, APBLADE_T);
//	//model->getObjects(blades, CPBLADE_T);
//	//model->getObjects(blades, OTHERBLADE_T);
//	if (model)
//		model->getObjects(blades, BLADE_T);
//
//	uint32_t ipmaxA_form_ccbid = 0;
//	uint32_t ipmaxB_form_ccbid = 0;
//
//
//	if (blades.size() > 0)
//	{
//		// Loop through returned blades
//		std::set<const ACS_CS_ImBase *>::const_iterator it;
//
//		for(it = blades.begin(); it != blades.end(); it++)
//		{
//
//			uint32_t ipa = 0;
//			uint32_t ipb = 0;
//
//			ACS_CS_ImBase *imBaseObj =const_cast<ACS_CS_ImBase *> (*it);
//
//			ACS_CS_ImBlade *b = dynamic_cast<ACS_CS_ImBlade *> (imBaseObj);
//
//			if(NULL == b)
//				break;
//
//			// convert it to an uint32_t
//
//			if(strcmp(b->ipAddressEthA.c_str(),"-")==0)
//				b->ipAddressEthA="0.0.0.0";
//
//			if(strcmp(b->ipAddressEthB.c_str(),"-")==0)
//				b->ipAddressEthB="0.0.0.0";
//
//			inet_pton(AF_INET, b->ipAddressEthA.c_str(), &ipa);
//			ipa=ntohl(ipa);
//
//			if(ipa>=ipmaxA_form_ccbid)
//			{
//				ipmaxA_form_ccbid=ipa;
//
//			}
//
//
//			inet_pton(AF_INET, b->ipAddressEthB.c_str(), &ipb);
//			ipb=ntohl(ipb);
//
//			if(ipb>=ipmaxB_form_ccbid)
//			{
//				ipmaxB_form_ccbid=ipb;
//				//ipmaxB_form_ccbid = ntohl(ipmaxB_form_ccbid);
//			}
//
//
//		}
//	}
//
//	//helper.assignIp(model,ethaM,ethbM);
//
//	char stra[INET_ADDRSTRLEN];
//	ipmaxA_form_ccbid = htonl(ipmaxA_form_ccbid);
//	inet_ntop(AF_INET, &(ipmaxA_form_ccbid), stra, INET_ADDRSTRLEN);
//	ethaM=std::string(stra);
//
//	ipmaxB_form_ccbid = htonl(ipmaxB_form_ccbid);
//	inet_ntop(AF_INET, &(ipmaxB_form_ccbid), stra, INET_ADDRSTRLEN);
//	ethbM=std::string(stra);
//
////	printf("ethaM=%s\n",ethaM.c_str());
////	printf("ethbM=%s\n",ethbM.c_str());
//
//	return 0;
//
//}







int ACS_CS_BladeHandler::fetchDefaulInfo(std::string &outProductId,std::string &outPath,int boardtype)
{

	int retValue = 1;
	ACS_CC_ReturnType result;

	std::vector<std::string> swRepos;
	std::string sw_dn("");

	OmHandler omHandler;

	result = omHandler.Init();

	if (result != ACS_CC_SUCCESS)
	{
		//printf(" ACS_CS_DHCP_ConfiguratorImpl::fetchDefaulInfo ERROR!\n");
		return retValue;
	}


	//printf(" ACS_CS_DHCP_ConfiguratorImpl::fetchDefaulInfo getting data\n");


	std::string classSwName;

	if((boardtype==SCXB) || (boardtype==SMXB) || (boardtype==IPTB)) // || (boardtype==CMXB))
	{
		classSwName = ACS_CS_ImmMapper::CLASS_AP_SW_PACKAGE;

	}
	else if(boardtype==EPB1) // || (boardtype==EVOET))
	{
		classSwName = ACS_CS_ImmMapper::CLASS_CP_SW_PACKAGE;

	}

	result = omHandler.getClassInstances(classSwName.c_str(),swRepos);

	if (result != ACS_CC_SUCCESS)
	{
		//printf(" ACS_CS_DHCP_ConfiguratorImpl::fetchCurrentLoadModuleInfo\n");

		result = omHandler.Finalize();

		return retValue;
	}

	result = omHandler.Finalize();

	int defaultP=0;
	std::string productIdentity("");
	std::string loadModuleFileName("");

	retValue=ACS_CC_SUCCESS;

	for (unsigned j=0; j<swRepos.size(); j++)
	{
		cout<<" Instances "<< j+1 <<": "<<swRepos[j].c_str()<<endl;
		sw_dn.clear();
		sw_dn = swRepos[j].c_str();

		defaultP = 0;

		if (ACS_CS_ImUtils::getImmAttributeInt(sw_dn,"isDefaultPackage",defaultP))
		{

			if(defaultP == 0)
			{
				continue;
			}
			productIdentity.clear();

			if (ACS_CS_ImUtils::getImmAttributeString(sw_dn,"productIdentity",productIdentity))
			{
				loadModuleFileName.clear();

				if (ACS_CS_ImUtils::getImmAttributeString(sw_dn,"loadModuleFileName",loadModuleFileName))
				{
					retValue = 1;
				}
				else break;
			}
			else break;
		}
	}


	if((retValue == 0) || (swRepos.size()==0))
	{
		return retValue;
	}


	outProductId=productIdentity;

	outPath=loadModuleFileName;


	return retValue;
}




int ACS_CS_BladeHandler::fetchDefaultSwPackageDn(std::string &dn,int boardtype)
{

	int retValue = 1;

	//dn="apManagedSwPackageId=CXP9019918_R2D,bladeSwManagementId=1";

	ACS_CC_ReturnType result;

	std::vector<std::string> swRepos;
	std::string sw_dn("");

	OmHandler omHandler;

	result = omHandler.Init();

	if (result != ACS_CC_SUCCESS)
	{
		//printf(" ACS_CS_DHCP_ConfiguratorImpl::fetchDefaulInfo ERROR!\n");
		return retValue;
	}


	//printf(" ACS_CS_DHCP_ConfiguratorImpl::fetchDefaulInfo getting data\n");


	std::string classSwName;

	if((boardtype==SCXB) || (boardtype==IPTB) || (boardtype==CMXB) || (boardtype==IPLB) || (boardtype==SMXB))
	{
		classSwName = ACS_CS_ImmMapper::CLASS_AP_SW_PACKAGE;

	}
	else if((boardtype == EPB1) || (boardtype == EVOET))
	{
		classSwName = ACS_CS_ImmMapper::CLASS_CP_SW_PACKAGE;

	}

	result = omHandler.getClassInstances(classSwName.c_str(),swRepos);

	if (result != ACS_CC_SUCCESS)
	{
		//printf(" ACS_CS_DHCP_ConfiguratorImpl::fetchCurrentLoadModuleInfo\n");

		result = omHandler.Finalize();

		return retValue;
	}

	result = omHandler.Finalize();

	bool found = false;

	std::string productIdentity("");
	std::string loadModuleFileName("");

	retValue=ACS_CC_SUCCESS;

	dn="";

	for (unsigned j=0; j<swRepos.size(); j++)
	{
		cout<<" Instances "<< j+1 <<": "<<swRepos[j].c_str()<<endl;
		sw_dn.clear();
		sw_dn = swRepos[j].c_str();

		int defaultP=0;
		int faultyP=0;

		int fbnTmp = 0;
		if (ACS_CS_ImUtils::getImmAttributeInt(sw_dn,"functionalBoardName",fbnTmp))
		{
			if (fbnTmp == boardtype)
			{
				if (ACS_CS_ImUtils::getImmAttributeInt(sw_dn,"isDefaultPackage",defaultP))
				{
					if(ACS_CS_ImUtils::getImmAttributeInt(sw_dn,"packageStatus",faultyP))
					{
						if (defaultP && !faultyP)
						{
							found = true;
							dn=sw_dn;
							break;
						}
					}
				}
			}
		}
	}

	if(!found)
	{
		retValue=1;
	}

	return retValue;
}



bool ACS_CS_BladeHandler::checkIpaddress(ACS_CS_ImModel *model,uint32_t iptobeChecked)
{
	bool fl=true;

	std::set<const ACS_CS_ImBase *> blades;

	//model->getObjects(blades, OTHERBLADE_T);
	//	model->getObjects(blades, APBLADE_T);
	//model->getObjects(blades, CPBLADE_T);
	model->getObjects(blades, BLADE_T);


	// Check if any were returned
	if (blades.size() > 0)
	{
		// Loop through returned blades
		std::set<const ACS_CS_ImBase *>::const_iterator it;

		for(it = blades.begin(); it != blades.end(); it++)
		{

			uint32_t ipa = 0;
			uint32_t ipb = 0;

			ACS_CS_ImBase *imBaseObj =const_cast<ACS_CS_ImBase *> (*it);

			ACS_CS_ImBlade *b = dynamic_cast<ACS_CS_ImBlade *> (imBaseObj);

			if(NULL == b)
				continue;

			// convert it to an uint32_t

			inet_pton(AF_INET, b->ipAddressEthA.c_str(), &ipa);

			ipa = ntohl(ipa);

			if(iptobeChecked==ipa)
			{
				fl=false;
				break;
			}


			inet_pton(AF_INET, b->ipAddressEthB.c_str(), &ipb);

			ipb = ntohl(ipb);

			if(iptobeChecked==ipb)
			{
				fl=false;
				break;
			}


		}


	}
	else
		fl=true;

	return fl;

}

void ACS_CS_BladeHandler::assignDHCPOption(ACS_CS_ImBlade* blade, int apzType)
{

	if (blade->type == CPBLADE_T)
	{
		int nodeArch = 0;
		ACS_CS_Registry::getNodeArchitecture(nodeArch);

		//In DMX Configuration even Single Sided CP shall have a NORMAL DHCP reservation
		if (nodeArch == NODE_DMX || nodeArch == NODE_VIRTUALIZED) {
			if (blade->functionalBoardName == CPUB)
			{
				blade->dhcpOption=DHCP_NORMAL;
			}
			else
			{
				blade->dhcpOption=DHCP_CLIENT;
			}
		}
		//For ApzType < 4 we should not perform DHCP reservations for CPUB and MAUB boards
		else if (apzType == 4 || blade->functionalBoardName == RPBI_S)
		{
			if (blade->systemType == DOUBLE_SIDED_CP && blade->functionalBoardName == CPUB)
			{
				//Only Double Sided CP have a NORMAL DHCP reservation
				blade->dhcpOption=DHCP_NORMAL;
			}
			else
			{
				//Otherwise (RPBI_S or Single Sided CP) DHCP option is CLIENT
				blade->dhcpOption=DHCP_CLIENT;
			}
		}
		else //CPUB or MAUB with apzType < 4 => No DHCP Reservation
		{
			blade->dhcpOption=DHCP_NONE;
		}
	}
	else //NOT A CP BLADE
	{
		blade->dhcpOption=DHCP_CLIENT;
	}
}

ACS_CC_ReturnType ACS_CS_BladeHandler::storeStaticIPToImm(ACS_CS_ImBlade* blade, ACS_CS_ImModel *model)
{
	ACS_CC_ReturnType retVal= ACS_CC_SUCCESS;

	int isDHCP_ON=ACS_CS_Registry::getDHCPOnOffFlag();

	//Inhibit In ECS-VIRTUALIZED environment
	if (!isDHCP_ON){
		return retVal;
	}

	ACS_CS_ImHardwareMgmt* hw = dynamic_cast<ACS_CS_ImHardwareMgmt*>(model->getObject(ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));

	if (!hw)
		return ACS_CC_FAILURE;

	//Get Static IP table from model
	std::set<string> ipMap = hw->staticIpMap;
	uint32_t size = ipMap.size();

	string shelfDn = ACS_CS_ImUtils::getParentName(blade->rdn);

	ACS_CS_ImShelf* shelf = dynamic_cast<ACS_CS_ImShelf*> (model->getObject(shelfDn.c_str()));
	if (!shelf)
		return ACS_CC_FAILURE;

	string staticIpEntry;
	ACS_CS_ImUtils::createStaticIpEntry(shelf->address, blade->slotNumber, blade->ipAddressEthA, blade->ipAddressEthB, staticIpEntry );
	//Try to insert entry in map
	if(ipMap.find(staticIpEntry) == ipMap.end())
		ipMap.insert(staticIpEntry);
	if(blade->functionalBoardName == SMXB)
	{
	//inserting IpAliasA,IpAlliasB in static map with slot offset  entry (slot+SLOT_OFFSET)
		staticIpEntry ="";
		uint16_t slotOffset=blade->slotNumber + ACS_CS_NS::SLOT_OFFSET;
		ACS_CS_ImUtils::createStaticIpEntry(shelf->address, slotOffset, blade->ipAliasEthA, blade->ipAliasEthB, staticIpEntry);
		//Try to insert entry in map
		if(ipMap.find(staticIpEntry) == ipMap.end())
			ipMap.insert(staticIpEntry);
	}
		

	//Update the map only if its size is changed
	if (ipMap.size() > size)
	{
		//Update IMM transaction

		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createMultipleStringType("staticIpMap", ipMap);

		ACS_CC_ImmParameter staticIpMap; //systemIdentifier

		staticIpMap.attrName = attr.getAttrName();
		staticIpMap.attrType = attr.getAttrType();
		staticIpMap.attrValuesNum = attr.getAttrValuesNum();
		staticIpMap.attrValues = attr.getAttrValues();

		retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str(), &staticIpMap);

		if (retVal != ACS_CC_SUCCESS)
			return ACS_CC_FAILURE;

		//Update Internal Model transaction
		ACS_APGCC_AttrModification *attrMods[2];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		attrVal.attrValuesNum = attr.getAttrValuesNum();
		attrVal.attrValues = attr.getAttrValues();

		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType=ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = NULL;


		bool res = ACS_CS_ImRepository::instance()->modifyObject(ccbId, ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str(), attrMods);

                delete attrMods[0];

		if (!res)
			return ACS_CC_FAILURE;
	}
	return retVal;
}


bool ACS_CS_BladeHandler::checkComputeResourceBlade(const std::string &uuid)
{
	bool found = false;

	const ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
	if (model)
	{
		std::set<const ACS_CS_ImBase *> computeResources;
		model->getObjects(computeResources, COMPUTERESOURCE_T);


		// Check if any were returned
		if (computeResources.size() > 0)
		{
			// Loop through returned computeResource
			std::set<const ACS_CS_ImBase *>::const_iterator it;
			for(it = computeResources.begin(); !found && it != computeResources.end(); it++)
			{
				const ACS_CS_ImComputeResource *computeResource = dynamic_cast<const ACS_CS_ImComputeResource *>(*it);
				if (computeResource && computeResource->uuid.compare(uuid) == 0)
				{
					//check validity of the role assigned to the ComputeResource
					if (computeResource->crRoleId >= 0)
						found = true;
				}
			}
		}

		delete model;
	}
	return found;

}
