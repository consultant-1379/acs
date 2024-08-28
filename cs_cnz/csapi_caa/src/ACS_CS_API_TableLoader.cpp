
//	 Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_API_TableLoader.h"

#include <sstream>

#include "ACS_CS_API.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_API_Tracer.h"
#include "ACS_CS_Protocol.h"

#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImIMMReader.h"

ACS_CS_API_TRACER_DEFINE(ACS_CS_API_TableLoader_TRACE);

using namespace ACS_CS_API_NS;

using std::string;

// Class ACS_CS_API_TableLoader 

ACS_CS_API_TableLoader::ACS_CS_API_TableLoader()
{}

ACS_CS_API_TableLoader::~ACS_CS_API_TableLoader()
{}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_TableLoader::loadCPTable (ACS_CS_Table &cpTable)
{
	CS_API_Result resultValue = Result_Success;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(model && immReader && immReader->loadCpObjects(model)){

		cpTable.setTableType(ACS_CS_INTERNAL_API::Table_CP);

		set<const ACS_CS_ImBase *>::iterator clusterCpIt;
		set<const ACS_CS_ImBase *>::iterator dualSidedCpIt;

		set<const ACS_CS_ImBase *> clusterCpObjects;
		set<const ACS_CS_ImBase *> dualSidedCpObjects;

		model->getObjects(clusterCpObjects, CLUSTERCP_T);
		model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

		for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++){

			const ACS_CS_ImBase *base = *clusterCpIt;
			const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

			if (cp){
				ACS_CS_TableEntry entry(cp->systemIdentifier);

				string defaultName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);

				ACS_CS_Attribute attCpName(ACS_CS_Protocol::Attribute_CPId_Name);
				attCpName.setValue(defaultName.c_str(), defaultName.size());
				entry.setValue(attCpName);

				ACS_CS_Attribute attCPId_Alias(ACS_CS_Protocol::Attribute_CPId_Alias);
				attCPId_Alias.setValue(cp->alias.c_str(), cp->alias.size());
				entry.setValue(attCPId_Alias);

				ACS_CS_Attribute attCPId_APZ_System(ACS_CS_Protocol::Attribute_CPId_APZ_System);
				attCPId_APZ_System.setValue(reinterpret_cast<const char*>(&cp->apzSystem), sizeof(cp->apzSystem));
				entry.setValue(attCPId_APZ_System);

				ACS_CS_Attribute attCPId_CP_Type(ACS_CS_Protocol::Attribute_CPId_CP_Type);
				attCPId_CP_Type.setValue(reinterpret_cast<const char*>(&cp->cpType), sizeof(cp->cpType));
				entry.setValue(attCPId_CP_Type);

				ACS_CS_Attribute attCPId_CPState(ACS_CS_Protocol::Attribute_CPId_CPState);
				attCPId_CPState.setValue(reinterpret_cast<const char*>(&cp->cpState), sizeof(cp->cpState));
				entry.setValue(attCPId_CPState);

				ACS_CS_Attribute attCPId_ApplicationId(ACS_CS_Protocol::Attribute_CPId_ApplicationId);
				attCPId_ApplicationId.setValue(reinterpret_cast<const char*>(&cp->applicationId), sizeof(cp->applicationId));
				entry.setValue(attCPId_ApplicationId);

				ACS_CS_Attribute attCPId_ApzSubstate(ACS_CS_Protocol::Attribute_CPId_ApzSubstate);
				attCPId_ApzSubstate.setValue(reinterpret_cast<const char*>(&cp->apzSubstate), sizeof(cp->apzSubstate));
				entry.setValue(attCPId_ApzSubstate);

				ACS_CS_Attribute attCPId_StateTransCode(ACS_CS_Protocol::Attribute_CPId_StateTransCode);
				attCPId_StateTransCode.setValue(reinterpret_cast<const char*>(&cp->stateTransition), sizeof(cp->stateTransition));
				entry.setValue(attCPId_StateTransCode);

//				ACS_CS_Attribute attCPId_StateTransCode(ACS_CS_Protocol::Attribute_CPId_StateTransCode);
//				attCPId_StateTransCode.setValue(reinterpret_cast<const char*>(&cp->stateTransitionCode), sizeof(cp->stateTransitionCode));
//				entry.setValue(attCPId_StateTransCode);

				ACS_CS_Attribute attCPId_AptSubstate(ACS_CS_Protocol::Attribute_CPId_AptSubstate);
				attCPId_AptSubstate.setValue(reinterpret_cast<const char*>(&cp->aptSubstate), sizeof(cp->aptSubstate));
				entry.setValue(attCPId_AptSubstate);

				ACS_CS_Attribute attCPId_BlockingInfo(ACS_CS_Protocol::Attribute_CPId_BlockingInfo);
				attCPId_BlockingInfo.setValue(reinterpret_cast<const char*>(&cp->blockingInfo), sizeof(cp->blockingInfo));
				entry.setValue(attCPId_BlockingInfo);

				ACS_CS_Attribute attCPId_CPCapacity(ACS_CS_Protocol::Attribute_CPId_CPCapacity);
				attCPId_CPCapacity.setValue(reinterpret_cast<const char*>(&cp->cpCapacity), sizeof(cp->cpCapacity));
				entry.setValue(attCPId_CPCapacity);

				cpTable.addEntry(entry);
			}
			else {
				ACS_CS_API_TRACER_MESSAGE(("ACS_CS_TableLoader::loadCPTable()\n"
						"Failed to fetch ACS_CS_ImCp object(s)"));
				resultValue = Result_Failure;
			}
		}
		
		for(dualSidedCpIt = dualSidedCpObjects.begin(); dualSidedCpIt != dualSidedCpObjects.end(); dualSidedCpIt++){

			const ACS_CS_ImBase *base = *dualSidedCpIt;
			const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

			if (cp){
				
				string defaultName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
				
				ACS_CS_TableEntry entry(cp->systemIdentifier);
				ACS_CS_Attribute attCpName(ACS_CS_Protocol::Attribute_CPId_Name);
				attCpName.setValue(defaultName.c_str(), defaultName.size());
				entry.setValue(attCpName);
				
				ACS_CS_Attribute attCPId_MAU_Type(ACS_CS_Protocol::Attribute_CPId_MAU_Type);
				attCPId_MAU_Type.setValue(reinterpret_cast<const char*>(&cp->mauType), sizeof(cp->mauType));
				entry.setValue(attCPId_MAU_Type);
				cpTable.addEntry(entry);

				}
		}

	}

	if (model)
		delete model;
	delete immReader;

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_TableLoader::loadCPGroupTable (stringCPIDSetMap &CPGroupMap)
{
	ACS_CS_API_NameList groupNameList;
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Success;
	ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();

	CS_API_Result res = cpGroupTable->getGroupNames(groupNameList);

	if(res == Result_Success)
	{

		for (unsigned int i = 0; i < groupNameList.size(); i++)
		{
			ACS_CS_API_IdList cpList;
			cpidSet * cpset = new cpidSet();

			ACS_CS_API_Name cpGroupName = groupNameList[i];
			size_t nameLength = cpGroupName.length();

			char* groupNameChar = new char[nameLength];
			string groupNameString = groupNameChar;

			cpGroupName.getName(groupNameChar, nameLength);

			res = cpGroupTable->getGroupMembers(cpGroupName, cpList);

			if(res == Result_Success)
			{
				for (unsigned int i = 0; i < cpList.size(); i++)
				{
					cpset->insert(cpList[i]);
				}
			}
			CPGroupMap.insert(stringCPIDSetMap::value_type(groupNameString, cpset));
			delete []groupNameChar;
		}
	}
	else if (res == Result_NoValue){
		result = ACS_CS_API_NS::Result_NoValue;
	}
	else{
		result = ACS_CS_API_NS::Result_Failure;
	}

	delete cpGroupTable;

	return result;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_TableLoader::loadHWCTable (ACS_CS_Table &HWCTable)
{
	CS_API_Result resultValue = Result_Success;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(model && immReader && immReader->loadBladeObjects(model)){

		HWCTable.setTableType(ACS_CS_INTERNAL_API::Table_HWC);

		set<const ACS_CS_ImBase *>::iterator it;
		std::set<const ACS_CS_ImBase *> cpBladeObjects;
		std::set<const ACS_CS_ImBase *> apBladeObjects;
		std::set<const ACS_CS_ImBase *> otBladeObjects;

		model->getObjects(cpBladeObjects, CPBLADE_T);
		model->getObjects(apBladeObjects, APBLADE_T);
		model->getObjects(otBladeObjects, OTHERBLADE_T);

		for(it = cpBladeObjects.begin(); it != cpBladeObjects.end(); it++){


			const ACS_CS_ImBase *base = *it;
			const ACS_CS_ImCpBlade* blade = dynamic_cast<const ACS_CS_ImCpBlade*>(base);

			if (blade){
				ACS_CS_TableEntry entry(blade->entryId);

				ACS_CS_ImBase * shelfBase = model->getParentOf(blade->rdn);
				const ACS_CS_ImShelf * shelf = dynamic_cast<const ACS_CS_ImShelf*>(shelfBase);

				if(shelf){
					ACS_CS_Attribute attMagazine(ACS_CS_Protocol::Attribute_Magazine);
					attMagazine.setValue(shelf->address.c_str(), shelf->address.size());
					entry.setValue(attMagazine);
				}


				ACS_CS_Attribute attSlot(ACS_CS_Protocol::Attribute_Slot);
				attSlot.setValue(reinterpret_cast<const char*>(&blade->slotNumber), sizeof(blade->slotNumber));
				entry.setValue(attSlot);

				ACS_CS_Attribute attSysType(ACS_CS_Protocol::Attribute_SysType);
				attSysType.setValue(reinterpret_cast<const char*>(&blade->systemType), sizeof(blade->systemType));
				entry.setValue(attSysType);

				ACS_CS_Attribute attSysNo(ACS_CS_Protocol::Attribute_SysNo);
				attSysNo.setValue(reinterpret_cast<const char*>(&blade->systemNumber), sizeof(blade->systemNumber));
				entry.setValue(attSysNo);

				ACS_CS_Attribute attFBN(ACS_CS_Protocol::Attribute_FBN);
				attFBN.setValue(reinterpret_cast<const char*>(&blade->functionalBoardName), sizeof(blade->functionalBoardName));
				entry.setValue(attFBN);

				ACS_CS_Attribute attSide(ACS_CS_Protocol::Attribute_Side);
				attSide.setValue(reinterpret_cast<const char*>(&blade->side), sizeof(blade->side));
				entry.setValue(attSide);

				ACS_CS_Attribute attSeqNo(ACS_CS_Protocol::Attribute_SeqNo);
				attSeqNo.setValue(reinterpret_cast<const char*>(&blade->sequenceNumber), sizeof(blade->sequenceNumber));
				entry.setValue(attSeqNo);

				ACS_CS_Attribute attIP_EthA(ACS_CS_Protocol::Attribute_IP_EthA);
				attIP_EthA.setValue(blade->ipAddressEthA.c_str(), blade->ipAddressEthA.size());
				entry.setValue(attIP_EthA);

				ACS_CS_Attribute attIP_EthB(ACS_CS_Protocol::Attribute_IP_EthB);
				attIP_EthB.setValue(blade->ipAddressEthB.c_str(), blade->ipAddressEthB.size());
				entry.setValue(attIP_EthB);

				ACS_CS_Attribute attAlias_EthA(ACS_CS_Protocol::Attribute_Alias_EthA);
				attAlias_EthA.setValue(blade->ipAliasEthA.c_str(), blade->ipAliasEthA.size());
				entry.setValue(attAlias_EthA);

				ACS_CS_Attribute attAlias_EthB(ACS_CS_Protocol::Attribute_Alias_EthB);
				attAlias_EthB.setValue(blade->ipAliasEthB.c_str(), blade->ipAliasEthB.size());
				entry.setValue(attAlias_EthB);

				ACS_CS_Attribute attNetmask_Alias_EthA(ACS_CS_Protocol::Attribute_Netmask_Alias_EthA);
				attNetmask_Alias_EthA.setValue(blade->aliasNetmaskEthA.c_str(), blade->aliasNetmaskEthA.size());
				entry.setValue(attNetmask_Alias_EthA);

				ACS_CS_Attribute attNetmask_Alias_EthB(ACS_CS_Protocol::Attribute_Netmask_Alias_EthB);
				attNetmask_Alias_EthB.setValue(blade->aliasNetmaskEthB.c_str(), blade->aliasNetmaskEthB.size());
				entry.setValue(attNetmask_Alias_EthB);

				ACS_CS_Attribute attDHCP(ACS_CS_Protocol::Attribute_DHCP);
				attDHCP.setValue(reinterpret_cast<const char*>(&blade->dhcpOption), sizeof(blade->dhcpOption));
				entry.setValue(attDHCP);


				HWCTable.addEntry(entry);
			}
			else {
				ACS_CS_API_TRACER_MESSAGE(("ACS_CS_TableLoader::loadHWCTable()\n"
						"Failed to fetch ACS_CS_ImCpBlade object(s)"));
				resultValue = Result_Failure;
			}
		}

		for(it = apBladeObjects.begin(); it != apBladeObjects.end(); it++){

			const ACS_CS_ImBase *base = *it;
			const ACS_CS_ImApBlade* blade = dynamic_cast<const ACS_CS_ImApBlade*>(base);

			if (blade){
				ACS_CS_TableEntry entry(blade->entryId);

				ACS_CS_ImBase * shelfBase = model->getParentOf(blade->rdn);
				const ACS_CS_ImShelf * shelf = dynamic_cast<const ACS_CS_ImShelf*>(shelfBase);

				if(shelf){
					ACS_CS_Attribute attMagazine(ACS_CS_Protocol::Attribute_Magazine);
					attMagazine.setValue(shelf->address.c_str(), shelf->address.size());
					entry.setValue(attMagazine);
				}

				ACS_CS_Attribute attSlot(ACS_CS_Protocol::Attribute_Slot);
				attSlot.setValue(reinterpret_cast<const char*>(&blade->slotNumber), sizeof(blade->slotNumber));
				entry.setValue(attSlot);

				ACS_CS_Attribute attSysType(ACS_CS_Protocol::Attribute_SysType);
				attSysType.setValue(reinterpret_cast<const char*>(&blade->systemType), sizeof(blade->systemType));
				entry.setValue(attSysType);

				ACS_CS_Attribute attSysNo(ACS_CS_Protocol::Attribute_SysNo);
				attSysNo.setValue(reinterpret_cast<const char*>(&blade->systemNumber), sizeof(blade->systemNumber));
				entry.setValue(attSysNo);

				ACS_CS_Attribute attFBN(ACS_CS_Protocol::Attribute_FBN);
				attFBN.setValue(reinterpret_cast<const char*>(&blade->functionalBoardName), sizeof(blade->functionalBoardName));
				entry.setValue(attFBN);

				ACS_CS_Attribute attSide(ACS_CS_Protocol::Attribute_Side);
				attSide.setValue(reinterpret_cast<const char*>(&blade->side), sizeof(blade->side));
				entry.setValue(attSide);

				ACS_CS_Attribute attSeqNo(ACS_CS_Protocol::Attribute_SeqNo);
				attSeqNo.setValue(reinterpret_cast<const char*>(&blade->sequenceNumber), sizeof(blade->sequenceNumber));
				entry.setValue(attSeqNo);

				ACS_CS_Attribute attIP_EthA(ACS_CS_Protocol::Attribute_IP_EthA);
				attIP_EthA.setValue(blade->ipAddressEthA.c_str(), blade->ipAddressEthA.size());
				entry.setValue(attIP_EthA);

				ACS_CS_Attribute attIP_EthB(ACS_CS_Protocol::Attribute_IP_EthB);
				attIP_EthB.setValue(blade->ipAddressEthB.c_str(), blade->ipAddressEthB.size());
				entry.setValue(attIP_EthB);

				ACS_CS_Attribute attAlias_EthA(ACS_CS_Protocol::Attribute_Alias_EthA);
				attAlias_EthA.setValue(blade->ipAliasEthA.c_str(), blade->ipAliasEthA.size());
				entry.setValue(attAlias_EthA);

				ACS_CS_Attribute attAlias_EthB(ACS_CS_Protocol::Attribute_Alias_EthB);
				attAlias_EthB.setValue(blade->ipAliasEthB.c_str(), blade->ipAliasEthB.size());
				entry.setValue(attAlias_EthB);

				ACS_CS_Attribute attNetmask_Alias_EthA(ACS_CS_Protocol::Attribute_Netmask_Alias_EthA);
				attNetmask_Alias_EthA.setValue(blade->aliasNetmaskEthA.c_str(), blade->aliasNetmaskEthA.size());
				entry.setValue(attNetmask_Alias_EthA);

				ACS_CS_Attribute attNetmask_Alias_EthB(ACS_CS_Protocol::Attribute_Netmask_Alias_EthB);
				attNetmask_Alias_EthB.setValue(blade->aliasNetmaskEthB.c_str(), blade->aliasNetmaskEthB.size());
				entry.setValue(attNetmask_Alias_EthB);

				ACS_CS_Attribute attDHCP(ACS_CS_Protocol::Attribute_DHCP);
				attDHCP.setValue(reinterpret_cast<const char*>(&blade->dhcpOption), sizeof(blade->dhcpOption));
				entry.setValue(attDHCP);


				HWCTable.addEntry(entry);
			}
			else {
				ACS_CS_API_TRACER_MESSAGE(("ACS_CS_TableLoader::loadHWCTable()\n"
						"Failed to fetch ACS_CS_ImApBlade object(s)"));
				resultValue = Result_Failure;
			}
		}


		for(it = otBladeObjects.begin(); it != otBladeObjects.end(); it++){

			const ACS_CS_ImBase *base = *it;
			const ACS_CS_ImOtherBlade* blade = dynamic_cast<const ACS_CS_ImOtherBlade*>(base);

			if (blade){
				ACS_CS_TableEntry entry(blade->entryId);

				ACS_CS_ImBase * shelfBase = model->getParentOf(blade->rdn);
				const ACS_CS_ImShelf * shelf = dynamic_cast<const ACS_CS_ImShelf*>(shelfBase);

				if(shelf){
					ACS_CS_Attribute attMagazine(ACS_CS_Protocol::Attribute_Magazine);
					attMagazine.setValue(shelf->address.c_str(), shelf->address.size());
					entry.setValue(attMagazine);
				}

				ACS_CS_Attribute attSlot(ACS_CS_Protocol::Attribute_Slot);
				attSlot.setValue(reinterpret_cast<const char*>(&blade->slotNumber), sizeof(blade->slotNumber));
				entry.setValue(attSlot);

				ACS_CS_Attribute attSysType(ACS_CS_Protocol::Attribute_SysType);
				attSysType.setValue(reinterpret_cast<const char*>(&blade->systemType), sizeof(blade->systemType));
				entry.setValue(attSysType);

				ACS_CS_Attribute attSysNo(ACS_CS_Protocol::Attribute_SysNo);
				attSysNo.setValue(reinterpret_cast<const char*>(&blade->systemNumber), sizeof(blade->systemNumber));
				entry.setValue(attSysNo);

				ACS_CS_Attribute attFBN(ACS_CS_Protocol::Attribute_FBN);
				attFBN.setValue(reinterpret_cast<const char*>(&blade->functionalBoardName), sizeof(blade->functionalBoardName));
				entry.setValue(attFBN);

				ACS_CS_Attribute attSide(ACS_CS_Protocol::Attribute_Side);
				attSide.setValue(reinterpret_cast<const char*>(&blade->side), sizeof(blade->side));
				entry.setValue(attSide);

				ACS_CS_Attribute attSeqNo(ACS_CS_Protocol::Attribute_SeqNo);
				attSeqNo.setValue(reinterpret_cast<const char*>(&blade->sequenceNumber), sizeof(blade->sequenceNumber));
				entry.setValue(attSeqNo);

				ACS_CS_Attribute attIP_EthA(ACS_CS_Protocol::Attribute_IP_EthA);
				attIP_EthA.setValue(blade->ipAddressEthA.c_str(), blade->ipAddressEthA.size());
				entry.setValue(attIP_EthA);

				ACS_CS_Attribute attIP_EthB(ACS_CS_Protocol::Attribute_IP_EthB);
				attIP_EthB.setValue(blade->ipAddressEthB.c_str(), blade->ipAddressEthB.size());
				entry.setValue(attIP_EthB);

				ACS_CS_Attribute attAlias_EthA(ACS_CS_Protocol::Attribute_Alias_EthA);
				attAlias_EthA.setValue(blade->ipAliasEthA.c_str(), blade->ipAliasEthA.size());
				entry.setValue(attAlias_EthA);

				ACS_CS_Attribute attAlias_EthB(ACS_CS_Protocol::Attribute_Alias_EthB);
				attAlias_EthB.setValue(blade->ipAliasEthB.c_str(), blade->ipAliasEthB.size());
				entry.setValue(attAlias_EthB);

				ACS_CS_Attribute attNetmask_Alias_EthA(ACS_CS_Protocol::Attribute_Netmask_Alias_EthA);
				attNetmask_Alias_EthA.setValue(blade->aliasNetmaskEthA.c_str(), blade->aliasNetmaskEthA.size());
				entry.setValue(attNetmask_Alias_EthA);

				ACS_CS_Attribute attNetmask_Alias_EthB(ACS_CS_Protocol::Attribute_Netmask_Alias_EthB);
				attNetmask_Alias_EthB.setValue(blade->aliasNetmaskEthB.c_str(), blade->aliasNetmaskEthB.size());
				entry.setValue(attNetmask_Alias_EthB);

				ACS_CS_Attribute attDHCP(ACS_CS_Protocol::Attribute_DHCP);
				attDHCP.setValue(reinterpret_cast<const char*>(&blade->dhcpOption), sizeof(blade->dhcpOption));
				entry.setValue(attDHCP);


				HWCTable.addEntry(entry);
			}
			else {
				ACS_CS_API_TRACER_MESSAGE(("ACS_CS_TableLoader::loadHWCTable()\n"
						"Failed to fetch ACS_CS_ImApBlade object(s)"));
				resultValue = Result_Failure;
			}
		}
	}

	if (model)
		delete model;

	if (immReader)
		delete immReader;

	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_TableLoader::loadFunctionDistributionTable (ACS_CS_Table &FDTable)
{
	CS_API_Result resultValue = Result_Success;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(immReader->loadModel(model, ACS_CS_ImmMapper::RDN_APSERVICECATEGORY, ACS_APGCC_SUBLEVEL)){

		FDTable.setTableType(ACS_CS_INTERNAL_API::Table_FD);

		uint16_t index = 0;
		set<const ACS_CS_ImBase *>::iterator it;
		std::set<const ACS_CS_ImBase *> serviceObjects;
		model->getObjects(serviceObjects, APSERVICE_T);

		for(it = serviceObjects.begin(); it != serviceObjects.end(); it++){

			const ACS_CS_ImBase *base = *it;
			const ACS_CS_ImApService* aps = dynamic_cast<const ACS_CS_ImApService*>(base);
			std::stringstream functionName;

			if (aps){
				ACS_CS_TableEntry fdEntry(index++);

				ACS_CS_Attribute attDomainName(ACS_CS_Protocol::Attribute_FD_DomainName);
				attDomainName.setValue(aps->domainName.c_str(), aps->domainName.size());
				fdEntry.setValue(attDomainName);

				ACS_CS_Attribute attServiceName(ACS_CS_Protocol::Attribute_FD_ServiceName);
				attServiceName.setValue(aps->serviceName.c_str(), aps->serviceName.size());
				fdEntry.setValue(attServiceName);

				set<ACS_CS_ImAp *> apList = aps->aps;
				std::set<ACS_CS_ImAp*>::iterator apIt;
				apIt = apList.begin();

				if (apIt != apList.end()){
					ACS_CS_ImAp* ap = *apIt;
					ACS_CS_Attribute attAPIdentifier(ACS_CS_Protocol::Attribute_FD_APIdentifier);
					attAPIdentifier.setValue(reinterpret_cast<char*>(&ap->entryId), sizeof(ap->entryId));
					fdEntry.setValue(attAPIdentifier);
				}

				FDTable.addEntry(fdEntry);
			}
			else {
				ACS_CS_API_TRACER_MESSAGE(("ACS_CS_TableLoader::loadFunctionDistributionTable()\n"
						"Failed to fetch ACS_CS_ImApService object(s)"));
				resultValue = Result_Failure;
			}
		}
	}

	delete model;
	delete immReader;

	return resultValue;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_TableLoader::loadNETable (ACS_CS_Table &NETable)
{

	CS_API_Result result = Result_Failure;
	ACS_CS_Table * tmpTable = ACS_CS_API_NeHandling::loadNeTable();

	if(tmpTable){
		result = Result_Success;
		NETable = *tmpTable;

		delete tmpTable;
	}

	return result;
}


