
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
//	*********************************************************


#include <map>
#include "ACS_CS_API.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_API_Util_Internal.h"
#include "ACS_CS_Util.h"
// ACS_CS_API_NetworkElement_Implementation
#include "ACS_CS_API_NetworkElement_Implementation.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Protocol.h"
#include <string.h>
#include <stdlib.h>


#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImModelSaver.h"


#include "ACS_CS_ImUtils.h"
#include "acs_apgcc_paramhandling.h"

#include <iostream>
using namespace std;

using std::string;
//using ACS_CS_INTERNAL_API::ACS_CS_Internal_Table;
using ACS_CS_INTERNAL_API::Table_NE;
using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;


// Class ACS_CS_API_NetworkElement_Implementation 
ACS_CS_API_NetworkElement_Implementation::ACS_CS_API_NetworkElement_Implementation()
:model(0),immReader(0),cpCluster(0),advInfo(0)
{

}

ACS_CS_API_NetworkElement_Implementation::ACS_CS_API_NetworkElement_Implementation(bool logicalSubTree)
:cpCluster(0),advInfo(0)
{
	model = new ACS_CS_ImModel();
	immReader = new ACS_CS_ImIMMReader();

	if(model && immReader)
	{
		if (logicalSubTree)
		{
			//immReader->loadModel(model, ACS_CS_ImmMapper::RDN_AXE_LOGICAL_MGMT, ACS_APGCC_SUBTREE);
			immReader->loadCpObjects(model);
		}

		ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);

		if(advBase != 0) {
			advInfo = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

			if (advInfo == 0) //Serious Fault! Free Memory
				delete advBase;
		}

		ACS_CS_ImBase *clusterBase= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());

		if(clusterBase != 0) {
			cpCluster = dynamic_cast<ACS_CS_ImCpCluster*>(clusterBase);

			if (cpCluster == 0) //Serious Fault! Free Memory
				delete clusterBase;
		}
	}
}


ACS_CS_API_NetworkElement_Implementation::~ACS_CS_API_NetworkElement_Implementation()
{
	if(immReader)
	{
		delete immReader;
		immReader = 0;
	}
	if(model)
	{
		delete model;
		model = 0;
	}
	if(advInfo)
	{
		delete advInfo;
		advInfo = 0;
	}
	if(cpCluster)
	{
		delete cpCluster;
		cpCluster = 0;
	}
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::isMultipleCPSystem (bool &multipleCPSystem)
{

   multipleCPSystem = ACS_CS_Registry::isMultipleCPSystem();

   return ACS_CS_API_NS::Result_Success;

}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::isTestEnvironment (bool &testEnvironment)
 {
	 //	 old solution
	 //   testEnvironment = ACS_CS_Registry::isTestEnvironment();

	 if(advInfo != 0)
	 {
		 testEnvironment = advInfo->isTestEnvironment;
		 return ACS_CS_API_NS::Result_Success;
	 }

	 return ACS_CS_API_NS::Result_Failure;

}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getFrontAPG (APID &frontAPG)
 {
	 //	 old solution
	 //  frontAPG = ACS_CS_Registry::getFrontAPG();

	 frontAPG = 2001;
	 if(cpCluster != 0)
	 {
		 frontAPG = 2000 + cpCluster->frontAp;

		 if (frontAPG > ACS_CS_API_HWC_NS::SysType_AP)
			 return ACS_CS_API_NS::Result_Success;
		 else
			 return ACS_CS_API_NS::Result_Failure;

	 }

	 return ACS_CS_API_NS::Result_Success;

 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getNEID (ACS_CS_API_Name_R1 &neid)
 {

	 ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NS::Result_NoAccess; //default return value

//	 char neId[256];
//	 acs_apgcc_paramhandling acsParamHandling;
//	 ACS_CC_ReturnType result = acsParamHandling.getParameter(ACS_CS_ImmMapper::RDN_MANAGEDELEMENT, "networkManagedElementId", neId);

	 string neId;
	 bool result = ACS_CS_ImUtils::getImmAttributeString(ACS_CS_ImmMapper::RDN_MANAGEDELEMENT, "networkManagedElementId", neId);

	 if (result == true){
		 int length = neId.length();

		 if(length > 0){
			 neid.setName(neId.c_str());
			 returnValue = ACS_CS_API_NS::Result_Success;
		 }
		 else
		 {
			 returnValue = ACS_CS_API_NS::Result_NoValue;
		 }
	 }
	 else{

		 result = ACS_CS_ImUtils::getImmAttributeString(ACS_CS_ImmMapper::RDN_MANAGEDELEMENT, "managedElementId", neId);

		 if (result == true){
			 neId = ACS_CS_ImUtils::getIdValueFromRdn(neId);

			 int length = neId.length();

			 if(length > 0){
				 neid.setName(neId.c_str());
				 returnValue = ACS_CS_API_NS::Result_Success;
			 }
			 else
			 {
				 returnValue = ACS_CS_API_NS::Result_NoValue;
			 }
		 }
		 else{
			 returnValue = ACS_CS_API_NS::Result_Failure;
		 }

	 }

	 return returnValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getSingleSidedCPCount (uint32_t &cpCount)
{
	 ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	 if(model == 0)
		 return resultValue;

	 cpCount = 0;
	 set<const ACS_CS_ImBase *> clusterCpObjects;
	 set<const ACS_CS_ImBase *>::iterator clusterCpIt;

	 model->getObjects(clusterCpObjects, CLUSTERCP_T);
	 resultValue = ACS_CS_API_NS::Result_Success;

	 for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++)
	 {
		 const ACS_CS_ImBase *base = *clusterCpIt;
		 const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		 if (cp && cp->systemIdentifier < ACS_CS_API_HWC_NS::SysType_CP)
			 cpCount++;
	 }

	 return resultValue;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getDoubleSidedCPCount (uint32_t &cpCount)
 {

	 ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	 if(model == 0)
		 return resultValue;

	 cpCount = 0;
	 set<const ACS_CS_ImBase *> cpObjects;
	 set<const ACS_CS_ImBase *>::iterator it;

	 model->getObjects(cpObjects, DUALSIDEDCP_T);
	 resultValue = ACS_CS_API_NS::Result_Success;

	 for(it = cpObjects.begin(); it != cpObjects.end(); it++)
	 {
		 const ACS_CS_ImBase *base = *it;
		 const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

		 if (cp && cp->systemIdentifier >= ACS_CS_API_HWC_NS::SysType_CP)
			 cpCount++;
	 }


	 return resultValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getAPGCount (uint32_t &apgCount)
{

   // Suppress warning for not freeing pointer,
   //lint --e{429}

   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

   ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

   if (hwc)
   {
      ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

      if (boardSearch)
      {
         ACS_CS_API_IdList boardList;

         result = hwc->getBoardIds(boardList, boardSearch);  //search for all board ids (wildcard search)

         if (result == ACS_CS_API_NS::Result_Success)
         {
            apgCount = 0;
            std::map<unsigned int, string> apgIDMap;

            for (unsigned int i = 0; i < boardList.size(); i++)   //step through all the boards in the hwc table
            {
               BoardID boardId = boardList[i];
               SysID sysId = 0;

               result = hwc->getSysId(sysId, boardId);    //get the system id of current board

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  if (sysId >= ACS_CS_API_HWC_NS::SysType_AP)     //check if system type is AP
                  {
                     if (apgIDMap.find(sysId) == apgIDMap.end())  //check if sysId isn't added to the MAP
                     {
                        (void)apgIDMap[sysId];     //add sysId to the MAP
                        apgCount++;
                     }
                  }
               }
               else if (result == ACS_CS_API_NS::Result_NoValue)  //no value isn't a fault, the board just lacks a system id
                  result = ACS_CS_API_NS::Result_Success;
               else           //no entry, no access or other failure, not meaningful to continue
                  break;
            }
         }

         ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
      }

      ACS_CS_API::deleteHWCInstance(hwc);
   }

   return result;

}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getAlarmMaster (CPID &alarmMaster)
 {
	 ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NS::Result_NoAccess;

	 if(cpCluster != 0)
	 {
		 alarmMaster = cpCluster->alarmMaster;
		 returnValue = ACS_CS_API_NS::Result_Success;
	 }
	 else if(advInfo != 0)
	 {
		 alarmMaster = advInfo->alarmMaster;
		 returnValue = ACS_CS_API_NS::Result_Success;
	 }

	 return returnValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getClockMaster (CPID &clockMaster)
 {

	 ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NS::Result_NoAccess;

	 if(cpCluster != 0)
	 {
		 clockMaster = cpCluster->clockMaster;
		 returnValue = ACS_CS_API_NS::Result_Success;
	 }
	 else if(advInfo != 0)
	 {
		 clockMaster = advInfo->clockMaster;
		 returnValue = ACS_CS_API_NS::Result_Success;
	 }

	 return returnValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getBSOMIPAddress (uint32_t &addressEthA, uint32_t &addressEthB)
{
   addressEthA = BSOMIPAddressEthA;
   addressEthB = BSOMIPAddressEthB;

   return ACS_CS_API_NS::Result_Success;

}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getDefaultCPName (CPID cpid, ACS_CS_API_Name_R1 &name)
{

   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;
   string defName = "";

   if (ACS_CS_API_Util::getDefaultCPName(cpid, defName))
   {
      (void)name.setName(defName.c_str());

      result = ACS_CS_API_NS::Result_Success;
   }

   return result;

}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getOmProfile (ACS_CS_API_OmProfileChange_R1 &omProfileChange)
 {
	 ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NS::Result_NoAccess;

	 if (advInfo)
	 {
		 omProfileChange.omProfileCurrent = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->omProfileCurrent);
		 omProfileChange.omProfileRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->omProfileRequested);
		 omProfileChange.aptCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->aptProfileCurrent);
		 omProfileChange.aptRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->aptProfileRequested);
		 omProfileChange.aptQueued = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->aptProfileQueued);
		 omProfileChange.apzCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->apzProfileCurrent);
		 omProfileChange.apzRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->apzProfileRequested);
		 omProfileChange.apzQueued    = static_cast<ACS_CS_API_OmProfileChange::Profile> (advInfo->apzProfileQueued);
		 omProfileChange.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue> (advInfo->omProfilePhase);
		 omProfileChange.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue> (advInfo->omProfileChangeTrigger);

		 returnValue = ACS_CS_API_NS::Result_Success;
	 }

	 return returnValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpModeOut)
 {
	 if (cpCluster == NULL)
	 {
		 // Use default value of Normal
		 clusterOpModeOut = ACS_CS_API_ClusterOpMode::Normal;
	 }
	 else
	 {
		 ClusterOpModeEnum com = cpCluster->clusterOpMode;
		 clusterOpModeOut = (ACS_CS_API_ClusterOpMode::Value) com;
	 }

	 return ACS_CS_API_NS::Result_Success;
 }


 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getTrafficIsolated (CPID &trafficIsolatedCpId)
 {
	 ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NS::Result_NoAccess;

	 if(advInfo != 0)
	 {
		 returnValue = ACS_CS_API_NS::Result_Success;
		 trafficIsolatedCpId = advInfo->trafficIsolatedCp;
	 }

	 return returnValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getTrafficLeader (CPID &trafficLeaderCpId)
 {
	 ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NS::Result_NoAccess; //default return value

	 if(advInfo != 0)
	 {
		 returnValue = ACS_CS_API_NS::Result_Success;
		 trafficLeaderCpId = advInfo->trafficLeaderCp;
	 }

	 return returnValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getAPTType (std::string &aptType)
 {
    aptType = ACS_CS_Registry::getAPTType();

    return ACS_CS_API_NS::Result_Success;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getNodeArchitecture(ACS_CS_API_CommonBasedArchitecture::ArchitectureValue &nodeArchitectureOut) {

	 int value;

	 if (ACS_CS_Registry::getNodeArchitecture(value)) {
		 if (value >= ACS_CS_API_CommonBasedArchitecture::SCB && value <= ACS_CS_API_CommonBasedArchitecture::SMX)
		 {
			 nodeArchitectureOut = (ACS_CS_API_CommonBasedArchitecture::ArchitectureValue) value;
			 return ACS_CS_API_NS::Result_Success;
		 }
		 else
			 return ACS_CS_API_NS::Result_Failure;
	 } else
		 return ACS_CS_API_NS::Result_Failure;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getApgNumber(ACS_CS_API_CommonBasedArchitecture::ApgNumber &apgNumberOut) {

 	 int value;

 	 if (ACS_CS_Registry::getApgNumber(value)) {
 		 if (value >= ACS_CS_API_CommonBasedArchitecture::AP1 && value <= ACS_CS_API_CommonBasedArchitecture::AP2)
 		 {
 			 apgNumberOut = (ACS_CS_API_CommonBasedArchitecture::ApgNumber) value;
 			 return ACS_CS_API_NS::Result_Success;
 		 }
 		 else
 			 return ACS_CS_API_NS::Result_Failure;
 	 } else
 		 return ACS_CS_API_NS::Result_Failure;
  }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getBGCIVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2])
 {
	 return (ACS_CS_Registry::getBGCIVlan(subnet, netmask))? ACS_CS_API_NS::Result_Success: ACS_CS_API_NS::Result_Failure;
 }
 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getSOLVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2])
 {
	 return (ACS_CS_Registry::getSOLVlan(subnet, netmask))? ACS_CS_API_NS::Result_Success: ACS_CS_API_NS::Result_Failure;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getDmxcAddress(uint32_t &addressBgciA, uint32_t &addressBgciB)
 {
	 return (ACS_CS_Registry::getDmxcAddress(addressBgciA, addressBgciB))? ACS_CS_API_NS::Result_Success: ACS_CS_API_NS::Result_Failure;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_NetworkElement_Implementation::getApgBgciAddress(uint16_t apgNr, uint16_t side, uint32_t &ipA, uint32_t &ipB)
 {
	 return (ACS_CS_Registry::getApgBgciAddress(apgNr,  side, ipA, ipB))? ACS_CS_API_NS::Result_Success: ACS_CS_API_NS::Result_Failure;
 }
