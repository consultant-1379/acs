//   Read the documentation to learn more about C++ code generator
//   versioning.

//	  %X% %Q% %Z% %W%

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
//	*********************************************************

#include "ACS_CS_Table.h"
#include "ACS_CS_API_UsageLogger.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_API_Util_Internal.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_API_SetClusterOpModeChanged.h"
#include "ACS_CS_API_SetOmProfileNotification.h"
#include "ACS_CS_API_Tracer.h"

#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"

#include "acs_apgcc_omhandler.h"

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImOtherBlade.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImObjectCreator.h"

#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImModelSaver.h"

// ACS_CS_API_Set_Implementation
//#include "ACS_CS_API_Set_R2.h"
//#include "ACS_CS_API_Set.h"
//#include "ACS_CS_API_Set.h"
#include "ACS_CS_API_Set_Implementation.h"

#include "ACS_CS_ImSerialization.h"

#include <boost/archive/binary_oarchive.hpp>


ACS_CS_API_TRACER_DEFINE(ACS_CS_API_Set_TRACE);

using namespace ACS_CS_NS;
using namespace ACS_CS_INTERNAL_API;
using namespace ACS_CS_Protocol;

#include <iostream>
#include <fstream>
using namespace std;


// Class ACS_CS_API_Set_Implementation 

ACS_CS_API_Set_Implementation::ACS_CS_API_Set_Implementation()
{
}


ACS_CS_API_Set_Implementation::~ACS_CS_API_Set_Implementation()
{
}

ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setAlarmMaster (CPID alarmMaster)
{
	ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess;

	if (!ACS_CS_API_Util::isCPDefined(alarmMaster))
	{
		ACS_CS_API_TRACER_MESSAGE("Incorrect CP id supplied, %d", alarmMaster);

		// CP is not defined
		return ACS_CS_API_SET_NS::Result_Incorrect_CP_Id;
	}

	ACS_CS_ImModel *tmpModel = NULL;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *clusterBase= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);
	ACS_CS_ImCpCluster* clusterInfo = dynamic_cast<ACS_CS_ImCpCluster*>(clusterBase);

	ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* advInfo = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

	if(clusterInfo != NULL){
		clusterInfo->alarmMaster = alarmMaster;
		clusterInfo->action = ACS_CS_ImBase::MODIFY;

		tmpModel = new ACS_CS_ImModel();
		tmpModel->addObject(clusterInfo);
	}
	else if(advInfo != NULL)
	{
		advInfo->alarmMaster = alarmMaster;
		advInfo->action = ACS_CS_ImBase::MODIFY;

		tmpModel = new ACS_CS_ImModel();
		tmpModel->addObject(advInfo);
	}

	if(tmpModel == NULL) {
		ACS_CS_API_TRACER_MESSAGE("Could not connect to IMM");
		delete immReader;

		if (clusterBase != NULL)
			delete clusterBase;

		if(advBase != NULL)
			delete advBase;

		return returnValue;
	}

	ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(tmpModel);
	ACS_CC_ReturnType saved = saver->save("AlarmMaster");
	delete saver;

	if(saved == ACS_CC_FAILURE){
		ACS_CS_API_TRACER_MESSAGE("Could not save to IMM");
	}
	else{
		returnValue = ACS_CS_API_SET_NS::Result_Success;
	}

	delete tmpModel;
	delete immReader;

	return returnValue;
}

ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setClockMaster (CPID clockMaster)
{
	ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess;

	if (!ACS_CS_API_Util::isCPDefined(clockMaster))
	{
		ACS_CS_API_TRACER_MESSAGE("Incorrect CP id supplied, %d", clockMaster);

		// CP is not defined
		return ACS_CS_API_SET_NS::Result_Incorrect_CP_Id;
	}

	ACS_CS_ImModel *tmpModel = NULL;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *clusterBase= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);
	ACS_CS_ImCpCluster* clusterInfo = dynamic_cast<ACS_CS_ImCpCluster*>(clusterBase);

	ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* advInfo = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

	if(clusterInfo != NULL){
		clusterInfo->clockMaster = clockMaster;
		clusterInfo->action = ACS_CS_ImBase::MODIFY;

		tmpModel = new ACS_CS_ImModel();
		tmpModel->addObject(clusterInfo);
	}
	else if(advInfo != NULL)
	{
		advInfo->clockMaster = clockMaster;
		advInfo->action = ACS_CS_ImBase::MODIFY;

		tmpModel = new ACS_CS_ImModel();
		tmpModel->addObject(advInfo);
	}

	if(tmpModel == NULL) {
		ACS_CS_API_TRACER_MESSAGE("Could not connect to IMM");
		delete immReader;

		if (clusterBase != NULL)
			delete clusterBase;

		if(advBase != NULL)
			delete advBase;

		return returnValue;
	}

	ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(tmpModel);
	ACS_CC_ReturnType saved = saver->save("ClockMaster");
	delete saver;

	if(saved == ACS_CC_FAILURE){
		ACS_CS_API_TRACER_MESSAGE("Could not save to IMM");
	}
	else{
		returnValue = ACS_CS_API_SET_NS::Result_Success;
	}

	delete tmpModel;
	delete immReader;

	return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setClusterOpModeChanged (ACS_CS_API_RequestedClusterOpMode::RequestedValue clusterOpMode)
{
     ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess; //default return value

     ACS_CS_API_ClusterOpMode::Value clusterOpModeValue;

     // Convert from requestedClusterOpMode to the clusterOpMode enumeration type used internally by CS.
     switch (clusterOpMode)
     {
        case ACS_CS_API_RequestedClusterOpMode::Normal:
           clusterOpModeValue = ACS_CS_API_ClusterOpMode::Normal;
           break;

        case ACS_CS_API_RequestedClusterOpMode::Expert:
           clusterOpModeValue = ACS_CS_API_ClusterOpMode::Expert;
           break;

        default:
        	clusterOpModeValue = ACS_CS_API_ClusterOpMode::Normal;
        	break;
     }

     ACS_CS_API_SetClusterOpModeChanged *clusterMode = new ACS_CS_API_SetClusterOpModeChanged();

     if (clusterMode)
     {
         returnValue = clusterMode->setClusterOpMode(clusterOpModeValue,
        		 ACS_CS_API_NE_NS::CLUSTER_OP_MODE_CHANGED);

         delete clusterMode;
     }

     return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setQuorumData (ACS_CS_API_QuorumData_R1 &quorumData)
{
	 ACS_CS_API_TRACER_MESSAGE("ENTERED!");

	 ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess; //default return value
	 ACS_CS_ImModel *tmpModel = new ACS_CS_ImModel();

	 if (tmpModel)
	 {
		 returnValue = ACS_CS_API_SetQuorumData::setCpData(tmpModel, quorumData.cpData);
		 ACS_CS_API_TRACER_MESSAGE("setCpData return %d", returnValue);

		 // Network Element PDU
		 if (returnValue == ACS_CS_API_SET_NS::Result_Success) {
			 returnValue = ACS_CS_API_SetQuorumData::setTrafficLeader(tmpModel, quorumData.trafficLeaderCpId);
			 ACS_CS_API_TRACER_MESSAGE("setTrafficLeader return %d", returnValue);
		 }

		 // Network Element PDU
		 if (returnValue == ACS_CS_API_SET_NS::Result_Success) {
			 returnValue = ACS_CS_API_SetQuorumData::setTrafficIsolated(tmpModel, quorumData.trafficIsolatedCpId);
			 ACS_CS_API_TRACER_MESSAGE("setTrafficIsolated return %d", returnValue);
		 }

		 if (!ACS_CS_API_Util_Internal::isClusterAlignedAfterRestore())
		 {
			 //AdvancedConfiguration,CpCluster and ClusterCp MOCs are stored on data disk
			 //in /data/acs/data/cs/QuorumData file.
			 try
			 {
				 std::string fileName = ACS_CS_NS::PATH_QUORUM_DATA_FILE;
				 std::ofstream ofs(fileName.c_str());
				 boost::archive::binary_oarchive ar(ofs);
				 ar << * tmpModel;
			 }
			 catch (boost::archive::archive_exception& e)
			 {
				 ACS_CS_API_TRACER_MESSAGE("setQuorumData() Serialization exception occurred: %s.", e.what());
			 }
			 catch (boost::exception& )
			 {
				 ACS_CS_API_TRACER_MESSAGE("setQuorumData() Serialization exception occurred.");
			 }
		 }
		 else
		 {
			 // Commit changes
			 ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(tmpModel);
			 if (saver)
			 {
				 ACS_CC_ReturnType saved = saver->save("QuorumData", REGISTERED_OI);

				 delete saver;
				 delete tmpModel;
				 // Update IMM with new information
				 if(saved == ACS_CC_FAILURE) {
					 returnValue = ACS_CS_API_SET_NS::Result_Failure;
					 ACS_CS_API_TRACER_MESSAGE("saver->save() return FAILURE");
				 }
			 } else {
				 delete tmpModel;
				 returnValue = ACS_CS_API_SET_NS::Result_Failure;
			 }
		 }
	 }

	 if (returnValue == ACS_CS_API_SET_NS::Result_Success) {
		 if (quorumData.automaticQuorumRecovery == true) {
			 returnValue = setAPAProfiles(quorumData.aptProfile, quorumData.apzProfile);
			 ACS_CS_API_TRACER_MESSAGE("setAPAProfiles return %d", returnValue);
		 }
	 }

	 ACS_CS_API_TRACER_MESSAGE("EXIT - return %d", returnValue);
    return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setProfiles (ACS_CS_API_OmProfileChange_R1::Profile omProfile, ACS_CS_API_OmProfileChange_R1::Profile aptProfile, ACS_CS_API_OmProfileChange_R1::Profile apzProfile)
{
     ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_Failure; //default return value

     if (ACS_CS_API_Util::setProfile(omProfile, aptProfile, apzProfile, ACS_CS_API_OmProfileChange::NoChange))
         returnValue = ACS_CS_API_SET_NS::Result_Success;

     return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setOmProfileNotificationStatusSuccess (ACS_CS_API_OmProfilePhase::PhaseValue phase)
{
     ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_Failure; //default return value

     ACS_CS_API_SetOmProfileNotification *profileNotification = new ACS_CS_API_SetOmProfileNotification();

     if (profileNotification)
     {
         returnValue = profileNotification->setOmProfileNotificationStatus(phase, 0);   // Reason 0 indicates success

         delete profileNotification;
     }

     return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setOmProfileNotificationStatusFailure (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_Set_R1::ReasonType reason)
{
     ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_Failure; //default return value

     ACS_CS_API_SetOmProfileNotification *profileNotification = new ACS_CS_API_SetOmProfileNotification();

     if (profileNotification)
     {
         returnValue = profileNotification->setOmProfileNotificationStatus(phase, static_cast<int>(reason));

         delete profileNotification;
     }

     return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setClusterRecovery ()
{
     ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess; //default return value

     returnValue = clearOgGroup(ACS_CS_API_OgChange_R1::ClusterRecovery);

     if (returnValue == ACS_CS_API_SET_NS::Result_Success)
     {
         // Reset traffic leader CP, traffic isolated CP and the quorum data for all CPs
         ACS_CS_API_QuorumData_R1 emptyQuorumData;                              // cpData is empty
         emptyQuorumData.automaticQuorumRecovery = false;                       // Not AQR
         emptyQuorumData.trafficIsolatedCpId = ACS_CS_API_NS::CPID_Unspecified; // Traffic isolated CP unspecified
         emptyQuorumData.trafficLeaderCpId = ACS_CS_API_NS::CPID_Unspecified;   // Traffic leader CP unspecified

         returnValue = setQuorumData(emptyQuorumData);
     }

     return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setAPAProfiles (ACS_CS_API_OmProfileChange_R1::Profile aptProfile, ACS_CS_API_OmProfileChange_R1::Profile apzProfile)
{
//     return setProfiles(ACS_CS_API_OmProfileChange::UnspecifiedProfile, aptProfile, apzProfile);

     ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_Failure; //default return value

     ACS_CS_API_SetOmProfileNotification *profileNotification = new ACS_CS_API_SetOmProfileNotification();

     if (profileNotification)
     {
    	 returnValue = profileNotification->setOmProfileApaNotification(aptProfile, apzProfile);   // Reason 0 indicates success

    	 delete profileNotification;
     }

     return returnValue;
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setOpGroupDissolved ()
{
     return clearOgGroup(ACS_CS_API_OgChange_R1::OtherReason);
}

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::clearOgGroup (ACS_CS_API_OgChange_R1::ACS_CS_API_EmptyOG_ReasonCode reasonCode)
 {
	 ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess;

	 ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	 if(!immReader)
		 return returnValue;

	 ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);
	 ACS_CS_ImCpCluster* cpClusterInfo = dynamic_cast<ACS_CS_ImCpCluster*>(base);

	 if(!cpClusterInfo) {
		 ACS_CS_API_TRACER_MESSAGE("Could not get OPGROUP from IMM");

		 delete immReader;
		 if (base)
			 delete base;
		 return returnValue;
	 }

	 ACS_CC_ReturnType saved = ACS_CC_FAILURE;
	 ACS_CS_ImModel *model = new ACS_CS_ImModel();

	 // Clear Operative Group
	 cpClusterInfo->operativeGroup.clear();

	 // Set reason code
	 switch(reasonCode)
	 {
	 	case ACS_CS_API_OgChange::NotEmpty:
	 		cpClusterInfo->ogClearCode = NOT_EMPTY;
	 		break;
	 	case ACS_CS_API_OgChange::ClusterRecovery:
	 		cpClusterInfo->ogClearCode = CLUSTER_RECOVERY;
	 		break;
	 	case ACS_CS_API_OgChange::OtherReason:
	 		cpClusterInfo->ogClearCode = OTHER_REASON;
	 		break;
	 	default: cpClusterInfo->ogClearCode = UNDEF_CLEARCODE;
	 }

	 cpClusterInfo->action = ACS_CS_ImBase::MODIFY;

	 model->addObject(cpClusterInfo);

	 ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
	 saved = saver->save();
	 delete saver;

	 if(saved == ACS_CC_FAILURE){
		 ACS_CS_API_TRACER_MESSAGE("Could not save empty OPGROUP IMM");
		 returnValue = ACS_CS_API_SET_NS::Result_Failure;
	 }
	 else
	 {
		 ACS_CS_API_TRACER_MESSAGE("Empty OG written on object %s with reason: %d",
				 ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str(), reasonCode);
		 returnValue = ACS_CS_API_SET_NS::Result_Success;
	 }

	 delete model;
	 delete immReader;

	 return returnValue;
 }

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::setSoftwareVersion (std::string softVersion,std::string mag, unsigned short boardId)
 {
	 ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_Failure;

	 ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	 if(!immReader)
		 return returnValue;

	 std::string query;

	 std::stringstream ss;
	 ss << boardId;

	 query= ACS_CS_ImmMapper::ATTR_OTHER_BLADE_ID + "=" + ss.str()+"," + ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" +mag+"," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	 //char *nameObject = const_cast<char*> (query.c_str());

	 ACS_CS_ImBase *base= immReader->getObject(query);

	 delete immReader;

	 if (!base) {
		 return returnValue;
	 }

	 ACS_CS_ImOtherBlade* blade = dynamic_cast<ACS_CS_ImOtherBlade*>(base);

	 if (!blade) {
		 delete base;
		 return returnValue;
	 }

	 std::string loadModRef = "";

	 if (blade->functionalBoardName == SCXB || blade->functionalBoardName == CMXB || blade->functionalBoardName == IPTB || blade->functionalBoardName == IPLB
		||blade->functionalBoardName == SMXB)
		 loadModRef = ACS_CS_ImmMapper::ATTR_AP_SW_PACKAGE_ID + "=" + softVersion + "," + ACS_CS_ImmMapper::RDN_BLADE_SW_MGMT;
	 else if (blade->functionalBoardName == EPB1 || blade->functionalBoardName == EVOET)
		 loadModRef = ACS_CS_ImmMapper::ATTR_CP_SW_PACKAGE_ID + "=" + softVersion + "," + ACS_CS_ImmMapper::RDN_BLADE_SW_MGMT;
	 else {
		 delete base;
		 return returnValue;
	 }

	 delete base;

	 ACS_CC_ReturnType result;

	 OmHandler immHandle;

	 result  = immHandle.Init(REGISTERED_OI);

	 if(result!=ACS_CC_SUCCESS)
		 return returnValue;

	 ACS_APGCC_ImmObject swObj;
	 swObj.objName=loadModRef;


	 if (immHandle.getObject(&swObj) != ACS_CC_SUCCESS) {
		 immHandle.Finalize();
		 return returnValue;
	 }

	 ACS_APGCC_ImmObject currObj;
	 currObj.objName=query;
	 result=immHandle.getObject(&currObj);

	 if(result!=ACS_CC_SUCCESS)
	 {
		 immHandle.Finalize();
	 }
	 else
	 {
		 ACS_CC_ImmParameter currLoadMod;
		 currLoadMod.attrName= const_cast<char*> ("currentLoadModuleVersion");
		 currLoadMod.attrType = ATTR_NAMET;
		 currLoadMod.attrValuesNum = 1;

		 void *valueRDN[1] ={reinterpret_cast<void*> (const_cast<char*> (loadModRef.c_str())) };
		 currLoadMod.attrValues = valueRDN;

		 result = immHandle.modifyAttribute(query.c_str(), &currLoadMod);

		 immHandle.Finalize();

		 if (result != ACS_CC_SUCCESS)
		 {
			 cout << "ERROR CODE: " << immHandle.getInternalLastError() << endl;
			 cout << "ERROR MESSAGE: " << immHandle.getInternalLastErrorText() << endl;

		 }
		 else
		 {
			 returnValue= ACS_CS_API_SET_NS::Result_Success;
		 }

	 }


	 return returnValue;
 }

 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_Implementation::ceaseClusterOpModeAlarm()		// HX28206
 {
	 ACS_CS_API_TRACER_MESSAGE("ceaseClusterOpModeAlarm() - Ceasing Cluster Op Mode EXPERT alarm!");
	 ACS_CS_CEASE(Event_ClusterOpModeExpert, "acs_csd");
	 return ACS_CS_API_SET_NS::Result_Success;
 }
