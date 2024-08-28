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
 * @file ACS_CS_ImDefaultObjectCreator.cpp
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
 * 2011-08-29  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImTypes.h"

ACS_CS_ImAdvancedConfiguration * ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject()
{
   ACS_CS_ImAdvancedConfiguration *ac = new ACS_CS_ImAdvancedConfiguration();

   ac->rdn = "advancedConfigurationId=1,configurationInfoId=1";
   ac->type = ADVANCEDCONFIGURATION_T;
   ac->action = ACS_CS_ImBase::CREATE;

   ac->omProfileCurrent = 2;
   ac->phaseSubscriberCountApNotify = 2;
   ac->phaseSubscriberCountCommit = 2;
   ac->phaseSubscriberCountValidate = 2;
   ac->omProfileSupervisionTimeout = 123;
   ac->omProfilePhase = VALIDATE;
   ac->advancedConfigurationId = "advancedConfigurationId=1";
   ac->aptProfileCurrent = 1;
   ac->aptProfileQueued = 2;
   ac->aptProfileRequested = 3;
   ac->apzProfileCurrent = 1;
   ac->apzProfileQueued = 2;
   ac->apzProfileRequested = 3;
   ac->cancelClusterOpModeSupervision = false;
   ac->cancelProfileSupervision = UNDEF_OMPROFILEPHASE;
   ac->initiateUpdatePhaseChange = 0;
   ac->isTestEnvironment = true;
   ac->neSubscriberCount = 1;
   ac->omProfileRequested = 100;
   ac->omProfileChangeTrigger = MANUAL;
   ac->omProfileSupervisionPhase = UNDEF_OMPROFILEPHASE;
   ac->phaseSubscriberCountCpNotify = 4321;
   ac->startupProfileSupervision = 1111;
   ac->trafficIsolatedCp = 12;
   ac->trafficLeaderCp = 21;

   return ac;
}


ACS_CS_ImAp * ACS_CS_ImDefaultObjectCreator::createApObject()
{
   ACS_CS_ImAp * ap = new ACS_CS_ImAp();
   ap->rdn = "apId=1,apCategoryId=1,configurationInfoId=1";
   ap->type = AP_T;
   ap->action = ACS_CS_ImBase::CREATE;
   ap->apId = "apId=1";
   ap->entryId = 1234;
   ap->name = AP1;
   return ap;
}


void ACS_CS_ImDefaultObjectCreator::initBladeObject(ACS_CS_ImBlade * obj)
{
   obj->systemType = SINGLE_SIDED_CP;
   obj->systemNumber = 1;
   obj->slotNumber = 2;
   obj->side = A;
   obj->sequenceNumber = 1;
   obj->macAddressEthA = "1.2.0.1";
   obj->macAddressEthB = "1.2.0.2";
   obj->ipAliasEthA = "2.2.0.1";
   obj->ipAliasEthB = "2.2.0.2";
   obj->ipAddressEthA = "1.2.0.1";
   obj->ipAddressEthB = "1.2.0.2";
   obj->functionalBoardName = SCB_RP;
   obj->dhcpOption = DHCP_NORMAL;
   obj->aliasNetmaskEthA = "4.3.2.1";
   obj->aliasNetmaskEthB = "4.3.2.2";
   obj->entryId = 1;
}


ACS_CS_ImApBlade * ACS_CS_ImDefaultObjectCreator::createApBladeObject()
{
   ACS_CS_ImApBlade * apBlade = new ACS_CS_ImApBlade();

   ACS_CS_ImDefaultObjectCreator::initBladeObject(apBlade);

   apBlade->rdn = "apBladeId=1,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   apBlade->type = APBLADE_T;
   apBlade->action = ACS_CS_ImBase::CREATE;
   apBlade->apBladeId = "apBladeId=1";
   apBlade->apRdn = "apId=1,apCategoryId=1,configurationInfoId=1";
   apBlade->ap = 0;

   return apBlade;

}


ACS_CS_ImApCategory * ACS_CS_ImDefaultObjectCreator::createApCategoryObject()
{
   ACS_CS_ImApCategory * apCategory = new ACS_CS_ImApCategory();
   apCategory->rdn = "apCategoryId=1,configurationInfoId=1";
   apCategory->type = APCATEGORY_T;
   apCategory->action = ACS_CS_ImBase::CREATE;
   apCategory->apCategoryId = "apCategoryId=1";
   return apCategory;
}



ACS_CS_ImApService * ACS_CS_ImDefaultObjectCreator::createApServiceObject()
{
   ACS_CS_ImApService * apService = new ACS_CS_ImApService();
   apService->rdn = "apServiceId=1,configurationInfoId=1";
   apService->type = APSERVICE_T;
   apService->action = ACS_CS_ImBase::CREATE;
   apService->serviceName = "APSERVICE";
   apService->domainName = "ROCKERS";
   apService->apServiceId = "apServiceId=1";
   return apService;
}

ACS_CS_ImApServiceCategory * ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject()
{
   ACS_CS_ImApServiceCategory * apServiceCategory = new ACS_CS_ImApServiceCategory();
   apServiceCategory->rdn = "apServiceCategoryId=1,configurationInfoId=1";
   apServiceCategory->type = APSERVICECATEGORY_T;
   apServiceCategory->action = ACS_CS_ImBase::CREATE;
   apServiceCategory->apServiceCategoryId = "apServiceCategoryId=1";
   return apServiceCategory;
}


ACS_CS_ImBladeClusterInfo * ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject()
{
   ACS_CS_ImBladeClusterInfo * bladeClusterInfo = new ACS_CS_ImBladeClusterInfo();

   bladeClusterInfo->rdn = "bladeClusterInfoId=1234,configurationInfoId=1";
   bladeClusterInfo->type = BLADECLUSTERINFO_T;
   bladeClusterInfo->action = ACS_CS_ImBase::CREATE;
   bladeClusterInfo->bladeClusterInfoId = "bladeClusterInfoId=1234";
   bladeClusterInfo->alarmMaster = 1;
   bladeClusterInfo->clockMaster = 2;
   bladeClusterInfo->omProfile = 100;
   bladeClusterInfo->phase = IDLE;
   bladeClusterInfo->clusterOpMode = NORMAL;
   bladeClusterInfo->clusterOpModeType = UNDEF_CLUSTEROPMODETYPE;
   bladeClusterInfo->frontAp = AP1;
   bladeClusterInfo->ogClearCode = NOT_EMPTY;
   bladeClusterInfo->cpGroupCategory = 0;
   return bladeClusterInfo;
}

ACS_CS_ImConfigurationInfo * ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject()
{
   ACS_CS_ImConfigurationInfo * configurationInfo = new ACS_CS_ImConfigurationInfo();
   configurationInfo->rdn = "configurationInfoId=1";
   configurationInfo->type = CONFIGURATIONINFO_T;
   configurationInfo->action = ACS_CS_ImBase::CREATE;
   configurationInfo->configurationInfoId = "configurationInfoId=1";
   configurationInfo->isBladeClusterSystem = false;


   configurationInfo->advancedConfiguration = 0;
   configurationInfo->bladeClusterInfo = 0;
   configurationInfo->apCategory = 0;
   configurationInfo->hardwareConfigurationCategory = 0;
   configurationInfo->cpCategory = 0;
   configurationInfo->vlanCategory = 0;
   configurationInfo->apServiceCategory = 0;
   return configurationInfo;
}


ACS_CS_ImCp * ACS_CS_ImDefaultObjectCreator::createCpObject()
{
   ACS_CS_ImCp *cp = new ACS_CS_ImCp();
   cp->rdn = "cpId=1,cpCategoryId=1,configurationInfoId=1";
   cp->type = CP_T;
   cp->action = ACS_CS_ImBase::CREATE;
   cp->cpId = "cpId=1";
   cp->systemIdentifier = 234;
   cp->defaultName = "CP1";
   cp->alias = "CP1";
   cp->apzSystem = 2;
   cp->cpType = 2;
   cp->cpState = 2;
   cp->applicationId = 2;
   cp->apzSubstate = 2;
   cp->aptSubstate = 2;
   cp->stateTransition = 2;
   cp->blockingInfo = 2;
   cp->cpCapacity = 2;
   return cp;
}



ACS_CS_ImCpBlade * ACS_CS_ImDefaultObjectCreator::createCpBladeObject()
{
   ACS_CS_ImCpBlade * cpBlade = new ACS_CS_ImCpBlade();
   ACS_CS_ImDefaultObjectCreator::initBladeObject(cpBlade);
   cpBlade->rdn = "cpBladeId=1,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   cpBlade->type = CPBLADE_T;
   cpBlade->action = ACS_CS_ImBase::CREATE;
   cpBlade->cpBladeId = "cpBladeId=1";
   cpBlade->cpRdn = "cpId=1,cpCategoryId=1,configurationInfoId=1";
   cpBlade->cp = 0;
   return cpBlade;

}


ACS_CS_ImCpCategory * ACS_CS_ImDefaultObjectCreator::createCpCategoryObject()
{
   ACS_CS_ImCpCategory *cpCategory = new ACS_CS_ImCpCategory();
   cpCategory->rdn = "cpCategoryId=1,configurationInfoId=1";
   cpCategory->type = CPCATEGORY_T;
   cpCategory->action = ACS_CS_ImBase::CREATE;
   cpCategory->cpCategoryId = "cpCategoryId=1";
   return cpCategory;
}


ACS_CS_ImCpGroup * ACS_CS_ImDefaultObjectCreator::createCpGroupObject()
{
   ACS_CS_ImCpGroup *cpGroup = new ACS_CS_ImCpGroup();
   cpGroup->rdn = "cpGroupId=1,cpGroupCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
   cpGroup->type = CPGROUP_T;
   cpGroup->action = ACS_CS_ImBase::CREATE;
   cpGroup->cpGroupId = "cpGroupId=1";
   cpGroup->groupName = "GROUPNAME";
   return cpGroup;
}


ACS_CS_ImCpGroupCategory * ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject()
{
   ACS_CS_ImCpGroupCategory *cpGroupCategory = new ACS_CS_ImCpGroupCategory();
   cpGroupCategory->rdn = "cpGroupCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
   cpGroupCategory->type = CPGROUPCATEGORY_T;
   cpGroupCategory->action = ACS_CS_ImBase::CREATE;
   cpGroupCategory->cpGroupCategoryId = "cpGroupCategoryId=1";
   return cpGroupCategory;
}


ACS_CS_ImHardwareConfigurationCategory * ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject()
{
   ACS_CS_ImHardwareConfigurationCategory *hardwareConfigurationCategory = new ACS_CS_ImHardwareConfigurationCategory();
   hardwareConfigurationCategory->rdn = "hardwareConfigurationCategoryId=1,configurationInfoId=1";
   hardwareConfigurationCategory->type = HARDWARECONFIGURATIONCATEGORY_T;
   hardwareConfigurationCategory->action = ACS_CS_ImBase::CREATE;
   hardwareConfigurationCategory->hardwareConfigurationCategoryId = "hardwareConfigurationCategoryId=1";
   return hardwareConfigurationCategory;
}

ACS_CS_ImOtherBlade * ACS_CS_ImDefaultObjectCreator::createOtherBladeObject()
{
   ACS_CS_ImOtherBlade * otherBlade = new ACS_CS_ImOtherBlade();
   ACS_CS_ImDefaultObjectCreator::initBladeObject(otherBlade);
   otherBlade->rdn = "otherBladeId=1,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   otherBlade->type = OTHERBLADE_T;
   otherBlade->action = ACS_CS_ImBase::CREATE;
   otherBlade->otherBladeId = "otherBladeId=1";
   otherBlade->currentLoadModuleVersion = "1234";
   otherBlade->bladeProductNumber = "bladeProductNumber";
   return otherBlade;
}


ACS_CS_ImShelf * ACS_CS_ImDefaultObjectCreator::createShelfObject()
{
   ACS_CS_ImShelf *shelf = new ACS_CS_ImShelf();
   shelf->rdn = "shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   shelf->type = SHELF_T;
   shelf->action = ACS_CS_ImBase::CREATE;
   shelf->shelfId = "shelfId=1_2_0_4";
   shelf->address = "1.2.0.4";
   return shelf;
}

ACS_CS_ImShelf * ACS_CS_ImDefaultObjectCreator::createShelfObjectII()
{
   ACS_CS_ImShelf *shelf = new ACS_CS_ImShelf();
   shelf->rdn = "shelfId=1_2_0_6,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   shelf->type = SHELF_T;
   shelf->action = ACS_CS_ImBase::CREATE;
   shelf->shelfId = "shelfId=1_2_0_6";
   shelf->address = "1.2.0.6";
   return shelf;
}


ACS_CS_ImVlan * ACS_CS_ImDefaultObjectCreator::createVlanObject()
{
   ACS_CS_ImVlan *vlan = new ACS_CS_ImVlan();
   vlan->rdn = "vlanId=1,vlanCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
   vlan->type = VLAN_T;
   vlan->action = ACS_CS_ImBase::CREATE;
   vlan->vlanId = "vlanId=1";
   vlan->networkAddress = "1.2.0.99";
   vlan->netmask = "255.255.255.0";
   vlan->name = "Name";
   vlan->stack = KIP;
   return vlan;
}



ACS_CS_ImVlanCategory * ACS_CS_ImDefaultObjectCreator::createVlanCategoryObject()
{
   ACS_CS_ImVlanCategory *vlanCategory = new ACS_CS_ImVlanCategory();
   vlanCategory->rdn = "vlanCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
   vlanCategory->type = VLANCATEGORY_T;
   vlanCategory->action = ACS_CS_ImBase::CREATE;
   vlanCategory->vlanCategoryId = "vlanCategoryId=1";
   return vlanCategory;
}


