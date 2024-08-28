/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImObjectCreator.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
  *
  * REVISION HISTORY
  *
  * DATE        USER     DESCRIPTION/TR
  * --------------------------------
  * 2011-00-01  XBJOAXE  Starting from scratch
  *
  ****************************************************************************/

#include "ACS_CS_ImObjectCreator.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_ImVlan.h"
#include "ACS_CS_ImVlanCategory.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImOtherBlade.h"
#include "ACS_CS_ImHardwareMgmt.h"
#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImClusterCp.h"
#include "ACS_CS_ImDualSidedCp.h"
#include "ACS_CS_ImEquipment.h"
#include "ACS_CS_ImCpCluster.h"
#include "ACS_CS_ImApServiceCategory.h"
#include "ACS_CS_ImApBlade.h"
#include "ACS_CS_ImAdvancedConfiguration.h"
#include "ACS_APGCC_Util.H"
#include "acs_prc_api.h"
#include "ACS_CS_ImLogicalMgmt.h"
#include "ACS_CS_ImCpClusterStruct.h"
#include "ACS_CS_ImOmProfile.h"
#include "ACS_CS_ImCcFile.h"
#include "ACS_CS_ImOmProfileManager.h"
#include "ACS_CS_ImCcFileManager.h"
#include "ACS_CS_ImCandidateCcFile.h"
#include "ACS_CS_ImOmProfileStruct.h"
#include "ACS_CS_ImCandidateCcFileStruct.h"
#include "ACS_CS_ImCpProductInfo.h"
#include "ACS_CS_ImCrMgmt.h"
#include "ACS_CS_ImComputeResource.h"
#include "ACS_CS_ImComputeResourceNetwork.h"
#include "ACS_CS_ImIplbCluster.h"

#include "ACS_CS_ImFunctionDistribution.h"
#include "ACS_CS_ImApg.h"
#include "ACS_CS_ImFunction.h"

#include "ACS_CS_ImCrmEquipment.h"
#include "ACS_CS_ImCrmComputeResource.h"
#include "ACS_CS_ImExternalNetwork.cpp"

#include "ACS_CS_ImTransport.h"
#include "ACS_CS_ImHost.h"
#include "ACS_CS_ImInterface.h"
#include "ACS_CS_ImInternalNetwork.cpp"

#include "../ACS_CS_TFTP_Configurator.cpp"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_ImObjectCreator_TRACE);


ACS_CS_ImObjectCreator::ACS_CS_ImObjectCreator() {

}

ACS_CS_ImObjectCreator::~ACS_CS_ImObjectCreator() {
}



ACS_CS_ImBase* ACS_CS_ImObjectCreator::createImBaseObject(const ACS_APGCC_ImmObject &object)
{
	string name = ACS_CS_ImUtils::getClassName(object);
	ACS_CS_ImBase* retObj = NULL;

	if (name.compare(ACS_CS_ImmMapper::CLASS_VLAN) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createVlanObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_VLAN_CATEGORY) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createVlanCategoryObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_SHELF) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createShelfObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_OTHER_BLADE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createOtherBladeObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_HARDWARE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createHardwareMgmtObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createDualSidedCpObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CP_BLADE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCpBladeObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CLUSTER_CP) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createClusterCpObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_EQUIPMENT) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createEquipmentObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CP_CLUSTER) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCpClusterObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_AP_SERVICE_CATEGORY) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createApServiceCategoryObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_AP_SERVICE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createApServiceObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_LOGICAL) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createLogicalMgmtObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_AP_BLADE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createApBladeObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_APG) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createApObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_ADVANCED_CONF) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createAdvancedConfigurationObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCpClusterStructObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_OM_PROFILE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createOmProfileObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CCFILE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCcFileObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_OM_PROFILE_MANAGER) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createOmProfileManagerObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CCFILE_MANAGER) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCcFileManagerObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CANDIDATE_CCFILE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCandidateCcFileObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_OM_PROFILE_STRUCT) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createOmProfileManagerStructObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CANDIDATE_STRUCT) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCandidateCcFileStructObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_FUNCTION_DIST) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createFunctionDistributionObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_APGNODE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createApgNodeObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_FUNCTION) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createFunctionObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CP_PRODUCT_INFO) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCpProductInfoObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_CRMGMT) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCrMgmtObject(object);
	}

	else if (name.compare(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createComputeResourceObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE_NETWORK) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createComputeResourceNetworkObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_IPLB_CLUSTER) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createIplbClusterObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_CRM_EQUIPMENT) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCrmEquipmentObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_CRM_COMPUTE_RESOURCE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createCrmComputeResourceObject(object);
	}

	//TRM
	else if (name.compare(ACS_CS_ImmMapper::CLASS_TRM_TRANSPORT) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createTrmTransportObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_TRM_HOST) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createTrmHostObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_TRM_INTERFACE) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createTrmInterfaceObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_TRM_EXTERNALNETWORK) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createTrmLogicalNetworkObject(object);
	}
	else if (name.compare(ACS_CS_ImmMapper::CLASS_TRM_INTERNALNETWORK) == 0)
	{
		retObj = ACS_CS_ImObjectCreator::createTrmInternalNetworkObject(object);
	}

	return retObj;

}



ACS_CS_ImBase * ACS_CS_ImObjectCreator::createImBaseObject(const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_APGCC_ImmObject immObject;

	if (!createImmObject(immObject, className, parentName, attr))
		return NULL;

	//ACS_CS_ImUtils::printImmObject(immObject);
	ACS_CS_ImBase * imBaseObject = createImBaseObject(immObject);
	//ACS_CS_ImUtils::printImBaseObject(imBaseObject);

	return imBaseObject;
}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createVlanObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImVlan *vlanObj = new ACS_CS_ImVlan();

	if (vlanObj == NULL)
		return NULL;

	vlanObj->rdn = object.objName;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_VLAN_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->vlanId = (char *) it->attrValues[0];
			else
				vlanObj->vlanId = '\0';
		}

		else if (it->attrName.compare("stack") == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->stack = (VlanStackEnum) (*(int *) it->attrValues[0]);
			else
				vlanObj->stack = UNDEF_VLANSTACK;
		}

		else if (it->attrName.compare("pcp") == 0)
		{

			if(it->attrValuesNum > 0)
				vlanObj->pcp = *(int *)it->attrValues[0];
			else
				vlanObj->pcp = 99;
		}

		else if (it->attrName.compare("vlanType") == 0)
		{
			if(it->attrValuesNum > 0)
				vlanObj->vlanType = (VlanTypeEnum) (*(int *) it->attrValues[0]);
			else
				vlanObj->vlanType = UNDEF_VLANTYPE;
		}

		else if (it->attrName.compare("networkAddress") == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->networkAddress = (char *) it->attrValues[0];
			else
				vlanObj->networkAddress = '\0';
		}

		else if (it->attrName.compare("netmask") == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->netmask = (char *) it->attrValues[0];
			else
				vlanObj->netmask = '\0';
		}

		else if (it->attrName.compare("name") == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->name = (char *) it->attrValues[0];
			else
				vlanObj->name = '\0';

		}

		else if (it->attrName.compare("vlanTag") == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->vlanTag = *(uint32_t *) it->attrValues[0];
			else
				vlanObj->vlanTag = 0;
		}

		else if (it->attrName.compare("vNICName") == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->vNICName = (char *) it->attrValues[0];
			else
				vlanObj->vNICName = '\0';
		}

		else if (it->attrName.compare("reserved") == 0)
		{
			if (it->attrValuesNum > 0)
				vlanObj->reserved = *(int *)it->attrValues[0];
			else
				vlanObj->reserved = 0;

		}
	}
	return vlanObj;
}



ACS_CS_ImBase * ACS_CS_ImObjectCreator::createVlanCategoryObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImVlanCategory *vlanCategoryObj = new ACS_CS_ImVlanCategory();

	if (vlanCategoryObj == NULL)
		return NULL;

	vlanCategoryObj->rdn = object.objName;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_VLAN_CATEGORY_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				vlanCategoryObj->vlanCategoryId = (char *) it->attrValues[0];
			else
				vlanCategoryObj->vlanCategoryId = '\0';

		}
	}

	return vlanCategoryObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createShelfObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImShelf *shelfObj = new ACS_CS_ImShelf();

	if (shelfObj == NULL)
		return NULL;

	shelfObj->rdn = object.objName;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_SHELF_ID) == 0)
		{
			if (it->attrValuesNum > 0) {
				shelfObj->axeShelfId = (char *) it->attrValues[0];
				string address = ACS_APGCC::after(shelfObj->axeShelfId,"=");
				shelfObj->address = address;
			}
			else {
				shelfObj->axeShelfId = '\0';
				shelfObj->address = "0.0.0.0";
			}

		}
	}

	return shelfObj;

}



void ACS_CS_ImObjectCreator::initBaseClassParameters(ACS_CS_ImBlade *blade, const ACS_APGCC_ImmObject &object)
{

	//Assign default values
	blade->systemType = UNDEF_SYSTEMTYPE;
	blade->systemNumber = 0;
	blade->side = UNDEF_SIDE;
	blade->sequenceNumber = -1;
	blade->macAddressEthB = '\0';
	blade->macAddressEthA = '\0';
	blade->ipAliasEthB = "0.0.0.0";
	blade->ipAliasEthA = "0.0.0.0";
	blade->ipAddressEthB = '\0';
	blade->ipAddressEthA = '\0';
	blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
	blade->dhcpOption = UNDEF_DHCP;
	blade->aliasNetmaskEthB = "0.0.0.0";
	blade->aliasNetmaskEthA = "0.0.0.0";
	blade->entryId = 0;
	blade->uuid = "";



	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		//Different names for the same attribute in ApBlade and CpBlade...
		if (it->attrName.compare("systemType") == 0 || it->attrName.compare("cpArchitecture") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->systemType = (*(SystemTypeEnum *) it->attrValues[0]);
			else
				blade->systemType = UNDEF_SYSTEMTYPE;

		}

		else if (it->attrName.compare("systemNumber") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->systemNumber = (*(uint16_t *) it->attrValues[0]);
			else
				blade->systemNumber = 0;

		}
		else if (it->attrName.compare("side") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->side = (*(SideEnum *) it->attrValues[0]);
			else
				blade->side = UNDEF_SIDE;
		}

		else if (it->attrName.compare("sequenceNumber") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->sequenceNumber = (*(int32_t *) it->attrValues[0]);
			else
				blade->sequenceNumber = -1;
		}

		else if (it->attrName.compare("macAddressEthB") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->macAddressEthB = (char *) it->attrValues[0];
			else
				blade->macAddressEthB = '\0';
		}
		else if (it->attrName.compare("macAddressEthA") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->macAddressEthA = (char *) it->attrValues[0];
			else
				blade->macAddressEthA = '\0';
		}

		else if (it->attrName.compare("ipAliasEthB") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->ipAliasEthB = (char *) it->attrValues[0];
			else
				blade->ipAliasEthB = "0.0.0.0";
		}

		else if (it->attrName.compare("ipAliasEthA") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->ipAliasEthA = (char *) it->attrValues[0];
			else
				blade->ipAliasEthA = "0.0.0.0";

		}

		else if (it->attrName.compare("ipAddressEthB") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->ipAddressEthB = (char *) it->attrValues[0];
			else
				blade->ipAddressEthB = '\0';

		}

		else if (it->attrName.compare("ipAddressEthA") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->ipAddressEthA = (char *) it->attrValues[0];
			else
				blade->ipAddressEthA = '\0';

		}

		else if (it->attrName.compare("functionalBoardName") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->functionalBoardName = *((FunctionalBoardNameEnum *) it->attrValues[0]);
			else
				blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;

		}

		else if (it->attrName.compare("dhcpOption") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->dhcpOption = *((DhcpEnum *) it->attrValues[0]);
			else
				blade->dhcpOption = UNDEF_DHCP;

		}

		else if (it->attrName.compare("aliasNetmaskEthB") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->aliasNetmaskEthB = (char *) it->attrValues[0];
			else
				blade->aliasNetmaskEthB = "0.0.0.0";

		}

		else if (it->attrName.compare("aliasNetmaskEthA") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->aliasNetmaskEthA = (char *) it->attrValues[0];
			else
				blade->aliasNetmaskEthA = "0.0.0.0";
		}
		else if (it->attrName.compare("entryId") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->entryId = (*(uint16_t *)it->attrValues[0]);
			else
				blade->entryId = 0;
		}
		else if (it->attrName.compare("uuid") == 0)
		{
			if (it->attrValuesNum > 0)
				blade->uuid = (char *) it->attrValues[0];
			else
				blade->uuid = "";
		}

	}
}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createOtherBladeObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImOtherBlade *otherBladeObj = new ACS_CS_ImOtherBlade();

	if (otherBladeObj == NULL)
	{
		return NULL;
	}

	otherBladeObj->rdn = object.objName;

	initBaseClassParameters(otherBladeObj, object);

	otherBladeObj->aptEthInPort = 31;
	otherBladeObj->currentLoadModuleVersion = '\0';
	otherBladeObj->bladeProductNumber = '\0';
	otherBladeObj->swVerType = 2;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_OTHER_BLADE_ID) == 0)
		{
			if (it->attrValuesNum > 0) {
				otherBladeObj->axeOtherBladeId = (char *) it->attrValues[0];
				int slotNumber;
				std::istringstream ss( ACS_APGCC::after(otherBladeObj->axeOtherBladeId,"=") );
				ss >> slotNumber;
				otherBladeObj->slotNumber = slotNumber;
			} else {
				otherBladeObj->axeOtherBladeId = '\0';
				otherBladeObj->slotNumber = 0;
			}
		}

		if (it->attrName.compare("currentLoadModuleVersion") == 0)
		{
			if (it->attrValuesNum > 0)
			{
				otherBladeObj->currentLoadModuleVersion = (char *) it->attrValues[0];

				int apgNumber = 0;
				//SW version attribute is only available on AP1
				if (ACS_CS_Registry::getApgNumber(apgNumber) && apgNumber == 1)
				{
					ACS_CS_ImUtils::getImmAttributeInt(otherBladeObj->currentLoadModuleVersion, "swVerType", otherBladeObj->swVerType);
				}
			}
			else{
				otherBladeObj->currentLoadModuleVersion = '\0';
				otherBladeObj->swVerType = 2;
			}
		}

		else if (it->attrName.compare("bladeProductNumber") == 0)
		{
			if (it->attrValuesNum > 0)
				otherBladeObj->bladeProductNumber = (char *) it->attrValues[0];
			else
				otherBladeObj->bladeProductNumber = '\0';
		}

		else if (it->attrName.compare("aptEhtInPort") == 0)
		{
			if (it->attrValuesNum > 0)
				otherBladeObj->aptEthInPort = (*(uint16_t *)it->attrValues[0]);
			else
				otherBladeObj->aptEthInPort = 31;
		}
	}

	return otherBladeObj;
}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createHardwareMgmtObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImHardwareMgmt *hardwareConfigurationCategoryObj = new ACS_CS_ImHardwareMgmt();

	if (hardwareConfigurationCategoryObj == NULL)
		return NULL;

	hardwareConfigurationCategoryObj->rdn = object.objName;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_HARDWARE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				hardwareConfigurationCategoryObj->axeHardwareMgmtId = (char *) it->attrValues[0];
			else
				hardwareConfigurationCategoryObj->axeHardwareMgmtId = '\0';
		}
		else if (it->attrName.compare("staticIpMap") == 0)
		{
			for (uint32_t j = 0; j < it->attrValuesNum; j++)
			{
				hardwareConfigurationCategoryObj->staticIpMap.insert(reinterpret_cast<char *>(it->attrValues[j]));
			}
		}
	}

	return hardwareConfigurationCategoryObj;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCpBladeObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCpBlade *cpBladeObj = new ACS_CS_ImCpBlade();

	if (cpBladeObj == NULL)
		return NULL;


	cpBladeObj->rdn = object.objName;

	initBaseClassParameters(cpBladeObj, object);


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_BLADE_ID) == 0)
		{
			if (it->attrValuesNum > 0) {
				cpBladeObj->axeCpBladeId = (char *) it->attrValues[0];
				int slotNumber;
				std::istringstream ss( ACS_APGCC::after(cpBladeObj->axeCpBladeId,"=") );
				ss >> slotNumber;
				cpBladeObj->slotNumber = slotNumber;
			}
			else {
				cpBladeObj->axeCpBladeId = '\0';
				cpBladeObj->slotNumber = 0;
			}
		}
	}

	return cpBladeObj;
}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createClusterCpObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImClusterCp *cpObj = new ACS_CS_ImClusterCp();

	if (cpObj == NULL)
		return NULL;

	cpObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CLUSTER_CP_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->axeClusterCpId = (char *) it->attrValues[0];
			else
				cpObj->axeClusterCpId = '\0';
		}

		else if (it->attrName.compare("systemIdentifier") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->systemIdentifier = (*(uint16_t *) it->attrValues[0]);
			else
				cpObj->systemIdentifier = 0;

		}

		else if (it->attrName.compare("cpAlias") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->alias = (char *) it->attrValues[0];
			else
				cpObj->alias = '\0';
		}

		else if (it->attrName.compare("apzSystem") == 0)
		{
			if (it->attrValuesNum > 0)
				//cpObj->apzSystem = (*(uint16_t *) it->attrValues[0]);
				cpObj->apzSystem = (char *) it->attrValues[0];
			else
				cpObj->apzSystem = '\0';
		}

		else if (it->attrName.compare("cpType") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->cpType = (*(uint16_t *) it->attrValues[0]);
			else
				cpObj->cpType = 0;
		}

		else if (it->attrName.compare("cpState") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->cpState = (*(int32_t *) it->attrValues[0]);
			else
				cpObj->cpState = 0;
		}

		else if (it->attrName.compare("applicationId") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->applicationId = (*(int32_t *) it->attrValues[0]);
			else
				cpObj->applicationId = 0;
		}

		else if (it->attrName.compare("apzSubstate") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->apzSubstate = (*(int32_t *) it->attrValues[0]);
			else
				cpObj->apzSubstate = 0;
		}

		else if (it->attrName.compare("aptSubstate") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->aptSubstate = (*(int32_t *) it->attrValues[0]);
			else
				cpObj->aptSubstate = 0;
		}

		else if (it->attrName.compare("stateTransition") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->stateTransition = (*(int32_t *) it->attrValues[0]);
			else
				cpObj->stateTransition = 0;
		}

		else if (it->attrName.compare("blockingInfo") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->blockingInfo = (*(uint16_t *) it->attrValues[0]);
			else
				cpObj->blockingInfo = 0;
		}

		else if (it->attrName.compare("cpCapacity") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->cpCapacity = (*(uint32_t *) it->attrValues[0]);
			else
				cpObj->cpCapacity = 0;
		}

		else if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO) == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->axeCpBladeRefTo = (char *) it->attrValues[0];
			else
				cpObj->axeCpBladeRefTo = '\0';
		}
	}

	return cpObj;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createDualSidedCpObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImDualSidedCp *cpObj = new ACS_CS_ImDualSidedCp();
	if (cpObj == NULL)
		return NULL;

	cpObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_DUAL_SIDED_CP_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->axeDualSidedCpId = (char *) it->attrValues[0];
			else
				cpObj->axeDualSidedCpId = '\0';
		}

		else if (it->attrName.compare("cpAlias") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->alias = (char *) it->attrValues[0];
			else
				cpObj->alias = '\0';
		}

		else if (it->attrName.compare("systemIdentifier") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->systemIdentifier = (*(uint16_t *) it->attrValues[0]);
			else
				cpObj->systemIdentifier = 0;

		}

		else if (it->attrName.compare("apzSystem") == 0)
		{
			if (it->attrValuesNum > 0)
				//cpObj->apzSystem = (*(uint16_t *) it->attrValues[0]);
				cpObj->apzSystem = (char *) it->attrValues[0];
			else
				cpObj->apzSystem = '\0';
		}

		else if (it->attrName.compare("cpType") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->cpType = (*(uint16_t *) it->attrValues[0]);
			else
				cpObj->cpType = 0;
		}

		else if (it->attrName.compare("mauType") == 0)
		{
			if (it->attrValuesNum > 0)
				cpObj->mauType = (*(int32_t *) it->attrValues[0]);
			else
			{
				cpObj->mauType = 1;
			}
		}
		else if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO) == 0)
		{
			for (uint32_t j = 0; j < it->attrValuesNum; j++)
			{
				cpObj->axeCpBladeRefTo.insert(reinterpret_cast<char *>(it->attrValues[j]));
			}
		}
	}

	return cpObj;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createEquipmentObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImEquipment *configurationInfoObj = new ACS_CS_ImEquipment();

	if (configurationInfoObj == NULL)
		return NULL;

	configurationInfoObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_EQUIPMENT_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				configurationInfoObj->axeEquipmentId = (char *) it->attrValues[0];
			else
				configurationInfoObj->axeEquipmentId = '\0';
		}
	}

	int environment = -1;

	if (ACS_CS_ImUtils::getImmAttributeInt(ACS_CS_ImmMapper::RDN_APZFUNCTIONS,ACS_CS_ImmMapper::ATTR_SYSTEM_TYPE,environment)) {
		if (environment == SINGLE_CP_SYSTEM)
			configurationInfoObj->isBladeClusterSystem = false;
		else if (environment == MULTI_CP_SYSTEM)
			configurationInfoObj->isBladeClusterSystem = true;
	}


	return configurationInfoObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createLogicalMgmtObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImLogicalMgmt *lgicalMgmtObj = new ACS_CS_ImLogicalMgmt();

	if (lgicalMgmtObj == NULL)
		return NULL;

	lgicalMgmtObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_LOGICAL_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				lgicalMgmtObj->axeLogicalMgmtId = (char *) it->attrValues[0];
			else
				lgicalMgmtObj->axeLogicalMgmtId = '\0';
		}
	}

	return lgicalMgmtObj;

}



ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCpClusterObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCpCluster *cpClusterObj = new ACS_CS_ImCpCluster();

	if (cpClusterObj == NULL)
		return NULL;

	cpClusterObj->rdn = object.objName;
	cpClusterObj->axeCpClusterId = '\0';
	//   cpClusterObj->omProfile = '\0';
	cpClusterObj->clusterOpMode = UNDEF_CLUSTEROPMODE;
	cpClusterObj->clusterOpModeType = UNDEF_CLUSTEROPMODETYPE;
	cpClusterObj->alarmMaster = 0;
	cpClusterObj->clockMaster = 0;
	//   cpClusterObj->phase = UNDEF_OMPROFILEPHASE;
	cpClusterObj->frontAp = UNDEF_APNAME;
	cpClusterObj->ogClearCode = UNDEF_CLEARCODE;
	cpClusterObj->reportProgress = '\0';
	//   cpClusterObj->activeCcFile = '\0';


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_CLUSTER_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->axeCpClusterId = (char *) it->attrValues[0];
			else
				cpClusterObj->axeCpClusterId = '\0';
		}

		//      else if (it->attrName.compare("omProfile") == 0)
		//      {
		//         if (it->attrValuesNum > 0)
		//        	 cpClusterObj->omProfile = (char *) it->attrValues[0];
		//         else
		//        	 cpClusterObj->omProfile = '\0';
		//
		//      }

		else if (it->attrName.compare("clusterOpMode") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->clusterOpMode = *((ClusterOpModeEnum *) it->attrValues[0]);
			else
				cpClusterObj->clusterOpMode = UNDEF_CLUSTEROPMODE;

		}

		else if (it->attrName.compare("clusterOpModeType") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->clusterOpModeType = *((ClusterOpModeTypeEnum *) it->attrValues[0]);
			else
				cpClusterObj->clusterOpModeType = UNDEF_CLUSTEROPMODETYPE;

		}

		else if (it->attrName.compare("alarmMaster") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->alarmMaster = (*(uint32_t *) it->attrValues[0]);
			else
				cpClusterObj->alarmMaster = 0;

		}

		else if (it->attrName.compare("clockMaster") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->clockMaster = (*(uint32_t *) it->attrValues[0]);
			else
				cpClusterObj->clockMaster = 0;

		}

		//      else if (it->attrName.compare("phase") == 0)
		//      {
		//    	  if (it->attrValuesNum > 0)
		//    		  cpClusterObj->phase = (*(OmProfilePhaseEnum *) it->attrValues[0]);
		//    	  else
		//    		  cpClusterObj->phase = UNDEF_OMPROFILEPHASE;
		//
		//      }

		else if (it->attrName.compare("frontAp") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->frontAp = (*(ApNameEnum *) it->attrValues[0]);
			else
				cpClusterObj->frontAp = UNDEF_APNAME;

		}

		else if (it->attrName.compare("clearOgCode") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->ogClearCode = (*(OgClearCode *) it->attrValues[0]);
			else
				cpClusterObj->ogClearCode = UNDEF_CLEARCODE;

		}

		//      else if (it->attrName.compare("aptProfile") == 0)
		//      {
		//    	  if (it->attrValuesNum > 0)
		//    		  cpClusterObj->aptProfile = ((char *) it->attrValues[0]);
		//    	  else
		//    		  cpClusterObj->aptProfile = '\0';
		//
		//      }
		//
		//      else if (it->attrName.compare("apzProfile") == 0)
		//      {
		//    	  if (it->attrValuesNum > 0)
		//    		  cpClusterObj->apzProfile = ((char *) it->attrValues[0]);
		//    	  else
		//    		  cpClusterObj->apzProfile = '\0';
		//
		//      }

		else if (it->attrName.compare("allBcGroup") == 0)
		{
			for (uint32_t j = 0; j < it->attrValuesNum; j++)
			{
				cpClusterObj->allBcGroup.insert(reinterpret_cast<char *>(it->attrValues[j]));
			}
		}

		else if (it->attrName.compare("operativeGroup") == 0)
		{
			for (uint32_t j = 0; j < it->attrValuesNum; j++)
			{
				cpClusterObj->operativeGroup.insert(reinterpret_cast<char *>(it->attrValues[j]));
			}
		}
		else if (it->attrName.compare("reportProgress") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterObj->reportProgress = ((char *) it->attrValues[0]);
			else
				cpClusterObj->reportProgress = '\0';

		}
		//      else if (it->attrName.compare("activeCcFile") == 0)
		//      {
		//    	  if (it->attrValuesNum > 0)
		//    		  cpClusterObj->activeCcFile = ((char *) it->attrValues[0]);
		//    	  else
		//    		  cpClusterObj->activeCcFile = '\0';
		//
		//      }
	}

	return cpClusterObj;

}



ACS_CS_ImBase * ACS_CS_ImObjectCreator::createApServiceCategoryObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImApServiceCategory *apServiceCategoryObj = new ACS_CS_ImApServiceCategory();

	if (apServiceCategoryObj == NULL)
		return NULL;

	apServiceCategoryObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_AP_SERVICE_CATEGORY_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				apServiceCategoryObj->axeApServiceCategoryId = (char *) it->attrValues[0];
			else
				apServiceCategoryObj->axeApServiceCategoryId = '\0';
		}
	}


	return apServiceCategoryObj;
}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createApServiceObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImApService *apServiceObj = new ACS_CS_ImApService();

	if (apServiceObj == NULL)
		return NULL;

	apServiceObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare("serviceName") == 0)
		{
			if (it->attrValuesNum > 0)
				apServiceObj->serviceName = (char *) it->attrValues[0];
			else
				apServiceObj->serviceName = '\0';
		}

		else if (it->attrName.compare("domainName") == 0)
		{
			if (it->attrValuesNum > 0)
				apServiceObj->domainName = (char *) it->attrValues[0];
			else
				apServiceObj->domainName = '\0';
		}

		else if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_AP_SERVICE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				apServiceObj->apServiceId = (char *) it->attrValues[0];
			else
				apServiceObj->apServiceId = '\0';
		}

		if (it->attrName.compare("apList") == 0)
		{
			for (uint32_t j = 0; j < it->attrValuesNum; j++)
			{
				apServiceObj->apRdns.insert(reinterpret_cast<char *>(it->attrValues[j]));
			}
		}
	}

	return apServiceObj;

}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createApBladeObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImApBlade *apBladeObj = new ACS_CS_ImApBlade();

	if (apBladeObj == NULL)
		return NULL;


	apBladeObj->rdn = object.objName;

	initBaseClassParameters(apBladeObj, object);

	apBladeObj->hwVersion = 0;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_AP_BLADE_ID) == 0)
		{
			if (it->attrValuesNum > 0) {
				apBladeObj->axeApBladeId = (char *) it->attrValues[0];
				int slotNumber;
				std::istringstream ss( ACS_APGCC::after(apBladeObj->axeApBladeId,"=") );
				ss >> slotNumber;
				apBladeObj->slotNumber = slotNumber;
			}
			else {
				apBladeObj->axeApBladeId = '\0';
				apBladeObj->slotNumber = 0;
			}
		}
		else if (it->attrName.compare("hwVersion") == 0)
		{
			if (it->attrValuesNum > 0)
				apBladeObj->hwVersion = *((uint16_t *) it->attrValues[0]);
			else
				apBladeObj->hwVersion = 0;
		}
	}

	return apBladeObj;

}



ACS_CS_ImBase * ACS_CS_ImObjectCreator::createApObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImAp *apObj = new ACS_CS_ImAp();

	if (apObj == NULL)
		return NULL;

	apObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_APG_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				apObj->axeApgId = (char *) it->attrValues[0];
			else
				apObj->axeApgId = '\0';
		}

		else if (it->attrName.compare("entryId") == 0)
		{
			if (it->attrValuesNum > 0)
				apObj->entryId = (*(uint16_t *) it->attrValues[0]);
			else
				apObj->entryId = 0;
		}

		else if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_AP_BLADE_REF_TO) == 0)
		{
			for (uint32_t j = 0; j < it->attrValuesNum; j++)
			{
				apObj->axeApBladeRefTo.insert(reinterpret_cast<char *>(it->attrValues[j]));
			}
		}

	}

	return apObj;

}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createAdvancedConfigurationObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImAdvancedConfiguration *advancedConfigurationObj = new ACS_CS_ImAdvancedConfiguration();

	if (advancedConfigurationObj == NULL)
		return NULL;

	advancedConfigurationObj->rdn = object.objName;

	advancedConfigurationObj->omProfileSupervisionPhase = IDLE;
	advancedConfigurationObj->omProfileSupervisionTimeout = 0;
	advancedConfigurationObj->omProfileChangeTrigger = UNDEF_PROFILECHANGETRIGGER;
	advancedConfigurationObj->phaseSubscriberCountCpNotify = 0;
	advancedConfigurationObj->phaseSubscriberCountApNotify = 0;
	advancedConfigurationObj->phaseSubscriberCountCommit = 0;
	advancedConfigurationObj->phaseSubscriberCountValidate = 0;
	advancedConfigurationObj->startupProfileSupervision = 0;
	advancedConfigurationObj->trafficLeaderCp = 0;
	advancedConfigurationObj->trafficIsolatedCp = 0;
	advancedConfigurationObj->omProfileRequested = 0;
	advancedConfigurationObj->omProfilePhase = IDLE;
	advancedConfigurationObj->omProfileCurrent = 0;
	advancedConfigurationObj->neSubscriberCount = 0;
	advancedConfigurationObj->initiateUpdatePhaseChange = 0;
	advancedConfigurationObj->cancelProfileSupervision = IDLE;
	advancedConfigurationObj->cancelClusterOpModeSupervision = false;
	advancedConfigurationObj->apzProfileRequested = 0;
	advancedConfigurationObj->apzProfileQueued = 0;
	advancedConfigurationObj->apzProfileCurrent = 0;
	advancedConfigurationObj->aptProfileRequested = 0;
	advancedConfigurationObj->aptProfileQueued = 0;
	advancedConfigurationObj->aptProfileCurrent = 0;
	advancedConfigurationObj->alarmMaster = 0;
	advancedConfigurationObj->clockMaster = 0;
	advancedConfigurationObj->isTestEnvironment = false;
	advancedConfigurationObj->axeAdvancedConfigurationId = '\0';
	advancedConfigurationObj->clusterAligned = true;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare("omProfileSupervisionPhase") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->omProfileSupervisionPhase = (*(OmProfilePhaseEnum *) it->attrValues[0]);
			else
				advancedConfigurationObj->omProfileSupervisionPhase = IDLE;
		}

		else if (it->attrName.compare("omProfileSupervisionTimeout") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->omProfileSupervisionTimeout = (*(OmProfilePhaseEnum *) it->attrValues[0]);
			else
				advancedConfigurationObj->omProfileSupervisionTimeout = 0;
		}

		else if (it->attrName.compare("omProfileChangeTrigger") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->omProfileChangeTrigger = (*(ProfileChangeTriggerEnum *) it->attrValues[0]);
			else
				advancedConfigurationObj->omProfileChangeTrigger = UNDEF_PROFILECHANGETRIGGER;

		}

		else if (it->attrName.compare("phaseSubscriberCountCpNotify") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->phaseSubscriberCountCpNotify = (*(unsigned int *) it->attrValues[0]);
			else
				advancedConfigurationObj->phaseSubscriberCountCpNotify = 0;
		}

		else if (it->attrName.compare("phaseSubscriberCountApNotify") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->phaseSubscriberCountApNotify = (*(unsigned int *) it->attrValues[0]);
			else
				advancedConfigurationObj->phaseSubscriberCountApNotify = 0;
		}

		else if (it->attrName.compare("phaseSubscriberCountCommit") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->phaseSubscriberCountCommit = (*(unsigned int *) it->attrValues[0]);
			else
				advancedConfigurationObj->phaseSubscriberCountCommit = 0;
		}

		else if (it->attrName.compare("phaseSubscriberCountValidate") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->phaseSubscriberCountValidate = (*(unsigned int *) it->attrValues[0]);
			else
				advancedConfigurationObj->phaseSubscriberCountValidate = 0;
		}

		else if (it->attrName.compare("startupProfileSupervision") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->startupProfileSupervision = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->startupProfileSupervision = 0;
		}

		else if (it->attrName.compare("trafficLeaderCp") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->trafficLeaderCp = (*(uint16_t *) it->attrValues[0]);
			else
				advancedConfigurationObj->trafficLeaderCp = 0;
		}

		else if (it->attrName.compare("trafficIsolatedCp") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->trafficIsolatedCp = (*(uint16_t *) it->attrValues[0]);
			else
				advancedConfigurationObj->trafficIsolatedCp = 0;
		}

		else if (it->attrName.compare("omProfileRequested") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->omProfileRequested = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->omProfileRequested = 0;
		}

		else if (it->attrName.compare("omProfilePhase") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->omProfilePhase = (*(OmProfilePhaseEnum *) it->attrValues[0]);
			else
				advancedConfigurationObj->omProfilePhase = IDLE;
		}

		else if (it->attrName.compare("omProfileCurrent") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->omProfileCurrent = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->omProfileCurrent = 0;
		}

		else if (it->attrName.compare("neSubscriberCount") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->neSubscriberCount = (*(unsigned int *) it->attrValues[0]);
			else
				advancedConfigurationObj->neSubscriberCount = 0;
		}

		else if (it->attrName.compare("initiateUpdatePhaseChange") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->initiateUpdatePhaseChange = (*(unsigned int *) it->attrValues[0]);
			else
				advancedConfigurationObj->initiateUpdatePhaseChange = 0;
		}

		else if (it->attrName.compare("cancelProfileSupervision") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->cancelProfileSupervision = (*(OmProfilePhaseEnum *) it->attrValues[0]);
			else
				advancedConfigurationObj->cancelProfileSupervision = IDLE;
		}

		else if (it->attrName.compare("cancelClusterOpModeSupervision") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->cancelClusterOpModeSupervision = (*(bool *) it->attrValues[0]);
			else
				advancedConfigurationObj->cancelClusterOpModeSupervision = false;
		}

		else if (it->attrName.compare("apzProfileRequested") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->apzProfileRequested = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->apzProfileRequested = 0;
		}

		else if (it->attrName.compare("apzProfileQueued") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->apzProfileQueued = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->apzProfileQueued = 0;
		}

		else if (it->attrName.compare("apzProfileCurrent") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->apzProfileCurrent = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->apzProfileCurrent = 0;
		}

		else if (it->attrName.compare("aptProfileRequested") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->aptProfileRequested = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->aptProfileRequested = 0;
		}

		else if (it->attrName.compare("aptProfileQueued") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->aptProfileQueued = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->aptProfileQueued = 0;
		}

		else if (it->attrName.compare("aptProfileCurrent") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->aptProfileCurrent = (*(int *) it->attrValues[0]);
			else
				advancedConfigurationObj->aptProfileCurrent = 0;
		}

		else if (it->attrName.compare("alarmMaster") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->alarmMaster = (*(uint32_t *) it->attrValues[0]);
			else
				advancedConfigurationObj->alarmMaster = 0;

		}

		else if (it->attrName.compare("clockMaster") == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->clockMaster = (*(uint32_t *) it->attrValues[0]);
			else
				advancedConfigurationObj->clockMaster = 0;

		}

		else if (it->attrName.compare("isTestEnvironment") == 0)
		{
			if (it->attrValuesNum > 0) {

				advancedConfigurationObj->isTestEnvironment = (*(bool *) it->attrValues[0]);

				//        	int value =  (*(int *) it->attrValues[0]);
				//
				//        	if (value > 0)
				//        		advancedConfigurationObj->isTestEnvironment = true;
				//        	else
				//        		advancedConfigurationObj->isTestEnvironment = false;
				//
				//            if (advancedConfigurationObj->isTestEnvironment)
				//            	cout << "isTestEnvironment = TRUE!!! Value = " << value << endl;
				//            else
				//            	cout << "isTestEnvironment = FALSE!!! Value = " << value << endl;
			}

			else
				advancedConfigurationObj->isTestEnvironment = false;
		}

		else if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_ADVANCED_CONF_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				advancedConfigurationObj->axeAdvancedConfigurationId = (char *) it->attrValues[0];
			else
				advancedConfigurationObj->axeAdvancedConfigurationId = '\0';
		}

		else if (it->attrName.compare("clusterAligned") == 0)
		{
			if (it->attrValuesNum > 0) {

				advancedConfigurationObj->clusterAligned = (*(int *) it->attrValues[0]);
			}
			else
				advancedConfigurationObj->clusterAligned = true;
		}
	}


	return advancedConfigurationObj;

}



bool ACS_CS_ImObjectCreator::createImmObject(ACS_APGCC_ImmObject &immObject,
		const char *className,
		const char* parentName,
		ACS_APGCC_AttrValues **attr)
{
	bool success = true;

	// Go through all the attributes
	for (size_t x = 0; attr[x] != NULL; x++) {
		ACS_APGCC_ImmAttribute attribute;
		attribute.attrName = attr[x]->attrName;
		attribute.attrType = attr[x]->attrType;
		attribute.attrValuesNum = attr[x]->attrValuesNum;

		for (unsigned int y = 0; y < attr[x]->attrValuesNum; y++) {
			attribute.attrValues[y] = attr[x]->attrValues[y];
		}

		attribute.attrValues[attr[x]->attrValuesNum] = 0;
		immObject.attributes.push_back(attribute);

		string name = ACS_CS_ImUtils::getIdAttributeFromClassName(className);

		if(strcasecmp(name.c_str(), attribute.attrName.c_str()) == 0)
		{
			immObject.objName = reinterpret_cast<char *>(attribute.attrValues[0]);
			immObject.objName.append(",");
			immObject.objName.append(parentName);
		}
	}

	return success;
}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCpClusterStructObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCpClusterStruct *cpClusterStructObj = new ACS_CS_ImCpClusterStruct();

	if (cpClusterStructObj == NULL)
		return NULL;

	cpClusterStructObj->rdn = object.objName;
	cpClusterStructObj->axeCpClusterStructId = '\0';
	cpClusterStructObj->reason = '\0';
	cpClusterStructObj->timeOfLastAction = '\0';
	cpClusterStructObj->state = UNDEFINED_STATE;
	cpClusterStructObj->actionId = UNDEFINED_TYPE;
	cpClusterStructObj->result = NOT_AVAILABLE;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->axeCpClusterStructId = (char *) it->attrValues[0];
			else
				cpClusterStructObj->axeCpClusterStructId = '\0';
		}

		else if (it->attrName.compare("reason") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->reason = (char *) it->attrValues[0];
			else
				cpClusterStructObj->reason = '\0';

		}

		else if (it->attrName.compare("timeOfLastAction") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->timeOfLastAction = (char *) it->attrValues[0];
			else
				cpClusterStructObj->timeOfLastAction = '\0';

		}

		else if (it->attrName.compare("state") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->state = *((AsyncActionStateType *) it->attrValues[0]);
			else
				cpClusterStructObj->state = UNDEFINED_STATE;

		}

		else if (it->attrName.compare("actionId") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->actionId = *((AsyncActionType *) it->attrValues[0]);
			else
				cpClusterStructObj->actionId = UNDEFINED_TYPE;

		}

		else if (it->attrName.compare("result") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->result = *((ActionResultType *) it->attrValues[0]);
			else
				cpClusterStructObj->result = NOT_AVAILABLE;

		}

	}

	return cpClusterStructObj;

}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createOmProfileObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImOmProfile *omProfileObj = new ACS_CS_ImOmProfile();

	if (omProfileObj == NULL)
		return NULL;

	omProfileObj->rdn = object.objName;

	omProfileObj->apzProfile = '\0';
	omProfileObj->aptProfile = '\0';
	omProfileObj->state = UNDEF_OMPROFILESTATETYPE;
	//   omProfileObj->currentCcFile = '\0';


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_OM_PROFILE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->omProfileId = (char *) it->attrValues[0];
			else
				omProfileObj->omProfileId = '\0';
		}

		else if (it->attrName.compare("apzProfile") == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->apzProfile = (char *) it->attrValues[0];
			else
				omProfileObj->apzProfile = '\0';

		}

		else if (it->attrName.compare("aptProfile") == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->aptProfile = (char *) it->attrValues[0];
			else
				omProfileObj->aptProfile = '\0';

		}

		else if (it->attrName.compare("state") == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->state = *((OmProfileStateType *) it->attrValues[0]);
			else
				omProfileObj->state = UNDEF_OMPROFILESTATETYPE;

		}

		//      else if (it->attrName.compare("currentCcFile") == 0)
		//      {
		//    	  if (it->attrValuesNum > 0)
		//    		  omProfileObj->currentCcFile = (char *) it->attrValues[0];
		//    	  else
		//    		  omProfileObj->currentCcFile = '\0';
		//
		//      }

	}

	return omProfileObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCcFileObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCcFile *ccFileObj = new ACS_CS_ImCcFile();

	if (ccFileObj == NULL)
		return NULL;

	ccFileObj->rdn = object.objName;

	ccFileObj->state = UNDEF_CCFSTATETYPE;
	ccFileObj->rulesVersion = '\0';

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CCFILE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				ccFileObj->ccFileId = (char *) it->attrValues[0];
			else
				ccFileObj->ccFileId = '\0';
		}

		else if (it->attrName.compare("state") == 0)
		{
			if (it->attrValuesNum > 0)
				ccFileObj->state = *((CcFileStateType *) it->attrValues[0]);
			else
				ccFileObj->state = UNDEF_CCFSTATETYPE;

		}

		else if (it->attrName.compare("rulesVersion") == 0)
		{
			if (it->attrValuesNum > 0)
				ccFileObj->rulesVersion = (char *) it->attrValues[0];
			else
				ccFileObj->rulesVersion = '\0';

		}

	}

	return ccFileObj;

}

ACS_CS_ImBase* ACS_CS_ImObjectCreator::createFunctionDistributionObject(const ACS_APGCC_ImmObject& object)
{
	ACS_CS_ImFunctionDistribution* funcDistObj = new (std::nothrow) ACS_CS_ImFunctionDistribution();

	if( NULL != funcDistObj )
	{
		funcDistObj->rdn = object.objName;


		for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
		{
			if( (it->attrName.compare(ACS_CS_ImmMapper::ATTR_FUNCTION_DIST_ID) == 0)
					&& (it->attrValuesNum > 0) )
			{
				funcDistObj->m_FunctionDistributionMId = reinterpret_cast<char *>( it->attrValues[0]);
			}
		}
	}

	return funcDistObj;
}

ACS_CS_ImBase* ACS_CS_ImObjectCreator::createApgNodeObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImApg* apgNodeObj = new (std::nothrow) ACS_CS_ImApg();

	if( NULL != apgNodeObj )
	{
		apgNodeObj->rdn = object.objName;


		for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
		{
			if( (it->attrName.compare(ACS_CS_ImmMapper::ATTR_APGNODE_ID) == 0)
					&& (it->attrValuesNum > 0) )
			{
				apgNodeObj->m_ApgId = reinterpret_cast<char *>( it->attrValues[0]);
			}
		}
	}

	return apgNodeObj;
}

ACS_CS_ImBase* ACS_CS_ImObjectCreator::createFunctionObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImFunction* functionObj = new (std::nothrow) ACS_CS_ImFunction();

	if( NULL != functionObj )
	{
		functionObj->rdn = object.objName;


		for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
		{
			if( (it->attrName.compare(ACS_CS_ImmMapper::ATTR_FUNCTION_ID) == 0)
					&& (it->attrValuesNum > 0) )
			{
				functionObj->m_FunctionId = reinterpret_cast<char *>( it->attrValues[0]);
				continue;
			}

			if( (it->attrName.compare(FunctionDistribution::descriptionAttribute) == 0)
					&& (it->attrValuesNum > 0) )
			{
				functionObj->m_Description = reinterpret_cast<char *>( it->attrValues[0]);
				continue;
			}

			if( (it->attrName.compare(FunctionDistribution::domainAttribute) == 0)
					&& (it->attrValuesNum > 0) )
			{
				functionObj->m_Domain = reinterpret_cast<char *>( it->attrValues[0]);
				continue;
			}

			if( (it->attrName.compare(FunctionDistribution::funReferenceAttribute) == 0)
					&& (it->attrValuesNum > 0) )
			{
				functionObj->m_FunctionReferenceDn = reinterpret_cast<char *>( it->attrValues[0]);
				continue;
			}
		}
	}

	return functionObj;
}
ACS_CS_ImBase * ACS_CS_ImObjectCreator::createOmProfileManagerObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImOmProfileManager *omProfileObj = new ACS_CS_ImOmProfileManager();

	if (omProfileObj == NULL)
		return NULL;

	omProfileObj->rdn = object.objName;

	omProfileObj->activeCcFile = '\0';
	omProfileObj->omProfile = '\0';
	omProfileObj->reportProgress = '\0';
	omProfileObj->scope = UNDEF_PROFILESCOPETYPE;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_OM_PROFILE_MANAGER_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->omProfileManagerId = (char *) it->attrValues[0];
			else
				omProfileObj->omProfileManagerId = '\0';
		}

		else if (it->attrName.compare("omProfile") == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->omProfile = (char *) it->attrValues[0];
			else
				omProfileObj->omProfile = '\0';

		}

		else if (it->attrName.compare("activeCcFile") == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->activeCcFile = (char *) it->attrValues[0];
			else
				omProfileObj->activeCcFile = '\0';

		}

		else if (it->attrName.compare("reportProgress") == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->reportProgress = ((char *) it->attrValues[0]);
			else
				omProfileObj->reportProgress = '\0';

		}

		else if (it->attrName.compare("scope") == 0)
		{
			if (it->attrValuesNum > 0)
				omProfileObj->scope = *((ProfileScopeType *) it->attrValues[0]);
			else
				omProfileObj->scope = UNDEF_PROFILESCOPETYPE;

		}

	}

	return omProfileObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCcFileManagerObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCcFileManager *ccFileManagerObj = new ACS_CS_ImCcFileManager();

	if (ccFileManagerObj == NULL)
		return NULL;

	ccFileManagerObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CCFILE_MANAGER_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				ccFileManagerObj->ccFileManagerId = (char *) it->attrValues[0];
			else
				ccFileManagerObj->ccFileManagerId = '\0';
		}
	}

	return ccFileManagerObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCandidateCcFileObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCandidateCcFile *candidateCcFileObj = new ACS_CS_ImCandidateCcFile();

	if (candidateCcFileObj == NULL)
		return NULL;

	candidateCcFileObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				candidateCcFileObj->candidateCcFileId = (char *) it->attrValues[0];
			else
				candidateCcFileObj->candidateCcFileId = '\0';
		}

		else if (it->attrName.compare("reportProgress") == 0)
		{
			if (it->attrValuesNum > 0)
				candidateCcFileObj->reportProgress = ((char *) it->attrValues[0]);
			else
				candidateCcFileObj->reportProgress = '\0';

		}
	}

	return candidateCcFileObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createOmProfileManagerStructObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImOmProfileStruct *cpClusterStructObj = new ACS_CS_ImOmProfileStruct();

	if (cpClusterStructObj == NULL)
		return NULL;

	cpClusterStructObj->rdn = object.objName;
	cpClusterStructObj->axeCpClusterStructId = '\0';
	cpClusterStructObj->reason = '\0';
	cpClusterStructObj->timeOfLastAction = '\0';
	cpClusterStructObj->state = UNDEFINED_STATE;
	cpClusterStructObj->actionId = UNDEFINED_TYPE;
	cpClusterStructObj->result = NOT_AVAILABLE;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->axeCpClusterStructId = (char *) it->attrValues[0];
			else
				cpClusterStructObj->axeCpClusterStructId = '\0';
		}

		else if (it->attrName.compare("reason") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->reason = (char *) it->attrValues[0];
			else
				cpClusterStructObj->reason = '\0';

		}

		else if (it->attrName.compare("timeOfLastAction") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->timeOfLastAction = (char *) it->attrValues[0];
			else
				cpClusterStructObj->timeOfLastAction = '\0';

		}

		else if (it->attrName.compare("state") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->state = *((AsyncActionStateType *) it->attrValues[0]);
			else
				cpClusterStructObj->state = UNDEFINED_STATE;

		}

		else if (it->attrName.compare("actionId") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->actionId = *((AsyncActionType *) it->attrValues[0]);
			else
				cpClusterStructObj->actionId = UNDEFINED_TYPE;

		}

		else if (it->attrName.compare("result") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->result = *((ActionResultType *) it->attrValues[0]);
			else
				cpClusterStructObj->result = NOT_AVAILABLE;

		}

	}

	return cpClusterStructObj;

}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCandidateCcFileStructObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCandidateCcFileStruct *cpClusterStructObj = new ACS_CS_ImCandidateCcFileStruct();

	if (cpClusterStructObj == NULL)
		return NULL;

	cpClusterStructObj->rdn = object.objName;
	cpClusterStructObj->axeCpClusterStructId = '\0';
	cpClusterStructObj->reason = '\0';
	cpClusterStructObj->timeOfLastAction = '\0';
	cpClusterStructObj->state = UNDEFINED_STATE;
	cpClusterStructObj->actionId = UNDEFINED_TYPE;
	cpClusterStructObj->result = NOT_AVAILABLE;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->axeCpClusterStructId = (char *) it->attrValues[0];
			else
				cpClusterStructObj->axeCpClusterStructId = '\0';
		}

		else if (it->attrName.compare("reason") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->reason = (char *) it->attrValues[0];
			else
				cpClusterStructObj->reason = '\0';

		}

		else if (it->attrName.compare("timeOfLastAction") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->timeOfLastAction = (char *) it->attrValues[0];
			else
				cpClusterStructObj->timeOfLastAction = '\0';

		}

		else if (it->attrName.compare("state") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->state = *((AsyncActionStateType *) it->attrValues[0]);
			else
				cpClusterStructObj->state = UNDEFINED_STATE;

		}

		else if (it->attrName.compare("actionId") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->actionId = *((AsyncActionType *) it->attrValues[0]);
			else
				cpClusterStructObj->actionId = UNDEFINED_TYPE;

		}

		else if (it->attrName.compare("result") == 0)
		{
			if (it->attrValuesNum > 0)
				cpClusterStructObj->result = *((ActionResultType *) it->attrValues[0]);
			else
				cpClusterStructObj->result = NOT_AVAILABLE;

		}

	}

	return cpClusterStructObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCpProductInfoObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImCpProductInfo* prodInfo = new ACS_CS_ImCpProductInfo();

	if (prodInfo == NULL)
		return NULL;

	prodInfo->rdn = object.objName;
	prodInfo->cpProductInfoId = '\0';
	prodInfo->productName = '\0';
	prodInfo->productRevision = '\0';
	prodInfo->manufacturingDate = '\0';
	prodInfo->productVendor = '\0';
	prodInfo->productNumber = '\0';
	prodInfo->serialNumber = '\0';

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CP_PRODUCT_INFO_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				prodInfo->cpProductInfoId = (char *) it->attrValues[0];
			else
				prodInfo->cpProductInfoId = '\0';
		}
		else if (it->attrName.compare("productName") == 0)
		{
			if (it->attrValuesNum > 0)
				prodInfo->productName = (char *) it->attrValues[0];
			else
				prodInfo->productName = '\0';
		}
		else if (it->attrName.compare("productRevision") == 0)
		{
			if (it->attrValuesNum > 0)
				prodInfo->productRevision = (char *) it->attrValues[0];
			else
				prodInfo->productRevision = '\0';
		}
		else if (it->attrName.compare("manufacturingDate") == 0)
		{
			if (it->attrValuesNum > 0)
				prodInfo->manufacturingDate = (char *) it->attrValues[0];
			else
				prodInfo->manufacturingDate = '\0';
		}
		else if (it->attrName.compare("productVendor") == 0)
		{
			if (it->attrValuesNum > 0)
				prodInfo->productVendor = (char *) it->attrValues[0];
			else
				prodInfo->productVendor = '\0';
		}
		else if (it->attrName.compare("productNumber") == 0)
		{
			if (it->attrValuesNum > 0)
				prodInfo->productNumber = (char *) it->attrValues[0];
			else
				prodInfo->productNumber = '\0';
		}
		else if (it->attrName.compare("serialNumber") == 0)
		{
			if (it->attrValuesNum > 0)
				prodInfo->serialNumber = (char *) it->attrValues[0];
			else
				prodInfo->serialNumber = '\0';
		}

	}

	return prodInfo;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCrMgmtObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCrMgmt *crMgmtObj = new ACS_CS_ImCrMgmt();

	if (crMgmtObj == NULL)
		return NULL;

	crMgmtObj->rdn = object.objName;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CRM_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				crMgmtObj->crMgmtId = (char *) it->attrValues[0];
			else
				crMgmtObj->crMgmtId = '\0';
		}
	}

	return crMgmtObj;
}


ACS_CS_ImBase * ACS_CS_ImObjectCreator::createComputeResourceObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImComputeResource *computeResourceObj = new ACS_CS_ImComputeResource();

	if (computeResourceObj == NULL)
		return NULL;

	computeResourceObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_COMPUTE_RESOURCE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->computeResourceId = (char *) it->attrValues[0];
			else
				computeResourceObj->computeResourceId = "";
		}

		else if (it->attrName.compare("uuid") == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->uuid = ((char *) it->attrValues[0]);
			else
				computeResourceObj->uuid = "";
		}

		else if (it->attrName.compare("crType") == 0)	// HY60399: IPLB SW check moved to ACS_CS_ServiceHandler::Create()
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->crType = *((RoleType *) it->attrValues[0]);
			else
				computeResourceObj->crType = UNDEF_TYPE;
		}

		else if (it->attrName.compare("crRoleLabel") == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->crRoleLabel = ((char *) it->attrValues[0]);
			else
				computeResourceObj->crRoleLabel = "";
		}

		else if (it->attrName.compare("crRoleId") == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->crRoleId = (*(int *) it->attrValues[0]);
			else
				computeResourceObj->crRoleId = ROLE_FREE;
		}

		else if (it->attrName.compare("macAddressEthA") == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->macAddressEthA = ((char *) it->attrValues[0]);
			else
				computeResourceObj->macAddressEthA = "";
		}

		else if (it->attrName.compare("macAddressEthB") == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->macAddressEthB = ((char *) it->attrValues[0]);
			else
				computeResourceObj->macAddressEthB = "";
		}

		else if (it->attrName.compare("ipAddressEthA") == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->ipAddressEthA = ((char *) it->attrValues[0]);
			else
				computeResourceObj->ipAddressEthA = "";
		}

		else if (it->attrName.compare("ipAddressEthB") == 0)
		{
			if (it->attrValuesNum > 0)
				computeResourceObj->ipAddressEthB = ((char *) it->attrValues[0]);
			else
				computeResourceObj->ipAddressEthB = "";
		}

		else if (it->attrName.compare("network") == 0)
		{
			for (uint32_t j = 0; j <  it->attrValuesNum; j++)
			{
				computeResourceObj->networks.insert(reinterpret_cast<char *>(it->attrValues[j]));
			}
		}		
	}

	return computeResourceObj;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createComputeResourceNetworkObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImComputeResourceNetwork *computeResourceNetworkObj = new ACS_CS_ImComputeResourceNetwork();

	if (computeResourceNetworkObj == NULL)
		return NULL;

	computeResourceNetworkObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare("id") == 0)
		{
		  if (it->attrValuesNum > 0)
			computeResourceNetworkObj->networkId = (char *) it->attrValues[0];
		  else
			computeResourceNetworkObj->networkId = "";
		}

		else if (it->attrName.compare("nicName") == 0)
		{
		  if (it->attrValuesNum > 0)
				computeResourceNetworkObj->nicName = (char *) it->attrValues[0];
			else
				computeResourceNetworkObj->nicName = "";
		}

		else if (it->attrName.compare("netName") == 0)
		{
		  if (it->attrValuesNum > 0)
			computeResourceNetworkObj->netName = ((char *) it->attrValues[0]);
		  else
			computeResourceNetworkObj->netName = "";
		}
			
		else if (it->attrName.compare("macAddress") == 0)
		{
		  if (it->attrValuesNum > 0)
			computeResourceNetworkObj->macAddress = ((char *) it->attrValues[0]);
		  else
			computeResourceNetworkObj->macAddress = "";
		}
	}

	return computeResourceNetworkObj;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createIplbClusterObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImIplbCluster *iplbClusterObj = new ACS_CS_ImIplbCluster();

	if (iplbClusterObj == NULL)
		return NULL;

	iplbClusterObj->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_IPLB_CLUSTER_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				iplbClusterObj->iplbClusterId = (char *) it->attrValues[0];
			else
				iplbClusterObj->iplbClusterId = '\0';
		}
	}

	return iplbClusterObj;

}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCrmEquipmentObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCrmEquipment *crmEquipment = new ACS_CS_ImCrmEquipment();

	if (crmEquipment == NULL)
		return NULL;

	crmEquipment->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CRM_EQUIPMENT_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				crmEquipment->equipmentId = (char *) it->attrValues[0];
			else
				crmEquipment->equipmentId = '\0';
		}
	}

	return crmEquipment;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createCrmComputeResourceObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImCrmComputeResource *crmComputeResource = new ACS_CS_ImCrmComputeResource();

	if (crmComputeResource == NULL)
		return NULL;

	crmComputeResource->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				crmComputeResource->computeResourceId = (char *) it->attrValues[0];
			else
				crmComputeResource->computeResourceId = '\0';
		}
		else if (it->attrName.compare("uuid") == 0)
		{
			if (it->attrValuesNum > 0)
				crmComputeResource->uuid = ((char *) it->attrValues[0]);
			else
				crmComputeResource->uuid = "";
		}
		else if (it->attrName.compare("macAddress") == 0)
		{
			crmComputeResource->macAddress.insert((char**) it->attrValues, (char**) it->attrValues + it->attrValuesNum);
		}
	}

	return crmComputeResource;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createTrmTransportObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImTransport *transport = new ACS_CS_ImTransport();

	if (transport == NULL)
		return NULL;

	transport->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{
		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_TRM_TRANSPORT_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				transport->transportMId = (char *) it->attrValues[0];
			else
				transport->transportMId = '\0';
		}
	}

	return transport;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createTrmHostObject(const ACS_APGCC_ImmObject &object)
{

	ACS_CS_ImHost *host = new ACS_CS_ImHost();

	if (host == NULL)
		return NULL;

	host->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_TRM_HOST_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				host->hostId = (char *) it->attrValues[0];
			else
				host->hostId = '\0';
		}
		else if (it->attrName.compare("computeResourceDn") == 0)
		{
			if (it->attrValuesNum > 0)
				host->computeResourceDn = ((char *) it->attrValues[0]);
			else
				host->computeResourceDn = "";
		}
	}

	return host;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createTrmInterfaceObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImInterface *interface = new ACS_CS_ImInterface();

	if (interface == NULL)
		return NULL;

	interface->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_TRM_INTERFACE_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				interface->interfaceId = (char *) it->attrValues[0];
			else
				interface->interfaceId = '\0';
		}
		else if (it->attrName.compare("mac") == 0)
		{
			if (it->attrValuesNum > 0)
				interface->mac = ((char *) it->attrValues[0]);
			else
				interface->mac = "";
		}
		else if (it->attrName.compare("domain") == 0)
		{
			if (it->attrValuesNum > 0)
				interface->domain = *((int32_t *) it->attrValues[0]);
			else
				interface->domain = Network::DOMAIN_NR;
		}
	}

	return interface;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createTrmLogicalNetworkObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImExternalNetwork *network = new ACS_CS_ImExternalNetwork();

	if (network == NULL)
		return NULL;

	network->rdn = object.objName;

	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_TRM_EXTERNALNETWORK_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				network->networkId = (char *) it->attrValues[0];
			else
				network->networkId = '\0';
		}
		else if (it->attrName.compare("description") == 0)
		{
			if (it->attrValuesNum > 0)
				network->description = ((char *) it->attrValues[0]);
			else
				network->description = "";
		}
		else if (it->attrName.compare("name") == 0)
		{
			if (it->attrValuesNum > 0)
				network->name = ((char *) it->attrValues[0]);
			else
				network->name = "";
		}
		else if (it->attrName.compare("restrictedName") == 0)
		{
			if (it->attrValuesNum > 0)
				network->restrictedName = ((char *) it->attrValues[0]);
			else
				network->restrictedName = "";
		}
		else if (it->attrName.compare("adminState") == 0)
		{
			if (it->attrValuesNum > 0)
				network->adminState = *((AdminState *) it->attrValues[0]);
			else
				network->adminState = EMPTY_ADMIN_STATE;
		}
		else if (it->attrName.compare("attachedInterfaces") == 0)
		{
			network->attachedInterfaces.insert((char**) it->attrValues, (char**) it->attrValues + it->attrValuesNum);
		}
		else if (it->attrName.compare("nicName") == 0)
		{
			network->nicName.insert((char**) it->attrValues, (char**) it->attrValues + it->attrValuesNum);
		}
	}

	return network;
}

ACS_CS_ImBase * ACS_CS_ImObjectCreator::createTrmInternalNetworkObject(const ACS_APGCC_ImmObject &object)
{
	ACS_CS_ImInternalNetwork *network = new ACS_CS_ImInternalNetwork();

	if (network == NULL)
		return NULL;

	network->rdn = object.objName;


	for (std::vector<ACS_APGCC_ImmAttribute>::const_iterator it = object.attributes.begin(); object.attributes.end() != it; ++it)
	{

		if (it->attrName.compare(ACS_CS_ImmMapper::ATTR_TRM_INTERNALNETWORK_ID) == 0)
		{
			if (it->attrValuesNum > 0)
				network->networkId = (char *) it->attrValues[0];
			else
				network->networkId = '\0';
		}
		else if (it->attrName.compare("description") == 0)
		{
			if (it->attrValuesNum > 0)
				network->description = ((char *) it->attrValues[0]);
			else
				network->description = "";
		}
		else if (it->attrName.compare("name") == 0)
		{
			if (it->attrValuesNum > 0)
				network->name = ((char *) it->attrValues[0]);
			else
				network->name = "";
		}
		else if (it->attrName.compare("restrictedName") == 0)
		{
			if (it->attrValuesNum > 0)
				network->restrictedName = ((char *) it->attrValues[0]);
			else
				network->restrictedName = "";
		}
		else if (it->attrName.compare("adminState") == 0)
		{
			if (it->attrValuesNum > 0)
				network->adminState = *((AdminState *) it->attrValues[0]);
			else
				network->adminState = EMPTY_ADMIN_STATE;
		}
		else if (it->attrName.compare("attachedInterfaces") == 0)
		{
			network->attachedInterfaces.insert((char**) it->attrValues, (char**) it->attrValues + it->attrValuesNum);
		}
		else if (it->attrName.compare("nicName") == 0)
		{
			network->nicName.insert((char**) it->attrValues, (char**) it->attrValues + it->attrValuesNum);
		}
	}

	return network;
}
