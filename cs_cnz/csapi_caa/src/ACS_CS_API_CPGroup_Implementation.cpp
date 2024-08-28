//	*********************************************************
//	 COPYRIGHT Ericsson 2012.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************

#include "ACS_CS_API.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_API_Tracer.h"
#include "ACS_CS_API_CPGroup_Implementation.h"

ACS_CS_API_TRACER_DEFINE(ACS_CS_API_CPGroup_TRACE);
using namespace ACS_CS_API_NS;

ACS_CS_API_CPGroup_Implementation::ACS_CS_API_CPGroup_Implementation()
{
	ACS_CS_API_TRACER_MESSAGE("Entering ACS_CS_API_CPGroup_Implementation() constructor");
	// Hardcoded now that user defined groups are not longer possible
	cpGroupNames.insert("ALLBC");
	cpGroupNames.insert("OPGROUP");
    model = new ACS_CS_ImModel();
    immReader = new ACS_CS_ImIMMReader();

    ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_API_CPGroup_Implementation() constructor");
}

ACS_CS_API_CPGroup_Implementation::~ACS_CS_API_CPGroup_Implementation()
{
	ACS_CS_API_TRACER_MESSAGE("Entering ACS_CS_API_CPGroup_Implementation() destructor");
	if(immReader)
		delete immReader;
	if(model)
		delete model;
	ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_API_CPGroup_Implementation() destructor");
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_CPGroup_Implementation::getGroupMembers (const ACS_CS_API_Name_R1 &name, ACS_CS_API_IdList_R1 &cplist)
{
	 ACS_CS_API_TRACER_MESSAGE("Entering ACS_CS_API_CPGroup_Implementation::getGroupMembers()");
	 CS_API_Result resultValue = Result_Failure;

     // Read group name from name class
	 size_t nameLength = name.length();
     char* groupNameChar = new char[nameLength];
     resultValue = name.getName(groupNameChar, nameLength);

     string groupName = groupNameChar;
     transform(groupName.begin(), groupName.end(), groupName.begin(),(int(*)(int)) toupper);

     delete[] groupNameChar;

     if(resultValue != Result_Success)
     {
    	 (void)cplist.setSize(0);
    	 ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_API_CPGroup_Implementation::getGroupMembers() - 1");
    	 return resultValue;
     }

     // Validate group name
     set<string>::iterator groupNamesIt;
     groupNamesIt = cpGroupNames.find(groupName);

     if(groupNamesIt == cpGroupNames.end()){
    	 ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_API_CPGroup_Implementation::getGroupMembers() - 2");
    	 return Result_NoEntry;
     }

     // Find the CP objects for the requested group
     set<const ACS_CS_ImClusterCp *> cpSet;
     set<const ACS_CS_ImClusterCp *>::iterator cpIt;
     this->getMatchingCpObjects(groupName, cpSet);

     // Include also DualSided CP in group ALL
//     set<const ACS_CS_ImBase *> dsCpSet;
//     set<const ACS_CS_ImBase *>::iterator dsCpIt;

     /*if (groupName.compare("ALL") == 0) {
    	 model->getObjects(dsCpSet, DUALSIDEDCP_T);
     }*/

     int index = 0;
     (void)cplist.setSize(cpSet.size());// + dsCpSet.size());

     resultValue = ACS_CS_API_NS::Result_Success;

     // Populate callers cpList with ClusterCp entries
     for(cpIt = cpSet.begin(); cpIt != cpSet.end(); cpIt++)
     {
    	 const ACS_CS_ImClusterCp * cp = *cpIt;

    	 if(cp)
    	 {
    		 cplist.setValue(cp->systemIdentifier, index++);
    	 }
    	 else
    	 {
    		 resultValue = ACS_CS_API_NS::Result_Failure;
    	 }
     }

     // Populate callers cpList with DualSidedCp entries
//     for(dsCpIt = dsCpSet.begin(); dsCpIt != dsCpSet.end(); dsCpIt++)
//     {
//    	 const ACS_CS_ImDualSidedCp * cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(*dsCpIt);
//
//    	 if(cp)
//    	 {
//    		 cplist.setValue(cp->systemIdentifier, index++);
//    	 }
//    	 else
//    	 {
//    		 resultValue = ACS_CS_API_NS::Result_Failure;
//    	 }
//     }

	 if(resultValue != ACS_CS_API_NS::Result_Success){
		 (void)cplist.setSize(0);
	 }
	 ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_API_CPGroup_Implementation::getGroupMembers() - 3");
	 return resultValue;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_CPGroup_Implementation::getGroupNames (ACS_CS_API_NameList_R1 &nameList)
{
	 ACS_CS_API_TRACER_MESSAGE("Entering ACS_CS_API_CPGroup_Implementation::getGroupNames()");
	 int index = 0;
	 set<string>::iterator it;

	 nameList.setSize(cpGroupNames.size());

	 for(it = cpGroupNames.begin(); it != cpGroupNames.end(); it++)
	 {
		 string name = *it;
		 ACS_CS_API_Name cpGroupName(name.c_str());
		 nameList.setValue(cpGroupName, index++);
	 }

	 ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_API_CPGroup_Implementation::getGroupNames()");
	 return Result_Success;
}

 void ACS_CS_API_CPGroup_Implementation::getMatchingCpObjects(string groupName, set<const ACS_CS_ImClusterCp *> &cpSet)
 {
	 ACS_CS_API_TRACER_MESSAGE("Entering getMatchingCpObjects()");
	 bool ret = false;
	 if(model && immReader)
	 {
		 ACS_CS_API_TRACER_MESSAGE("Before loadModel() - model->size() == %d", (int)model->size());
		 if(model->size() == 0)
		 {
			 ACS_CS_API_TRACER_MESSAGE("MODEL SIZE is zero, loadModel()");
			 ret = immReader->loadModel(model, ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER, ACS_APGCC_SUBTREE);
			 ACS_CS_API_TRACER_MESSAGE("After loadModel() - ret == %d, model->size == %d",ret, (int)model->size());
			 if(!ret)
			 {
				 if(model){
					 ACS_CS_API_TRACER_MESSAGE("getMatchingCpObjects() -> setting model to NULL");
					 delete model;
					 model = NULL;
				 }
				 ACS_CS_API_TRACER_MESSAGE("Leaving getMatchingCpObjects() - 1");
				 return;
			 }
		 }
	 }
	 else
	 {
		 ACS_CS_API_TRACER_MESSAGE("getMatchingCpObjects() -> model == NULL");
		 ACS_CS_API_TRACER_MESSAGE("Leaving getMatchingCpObjects() - 2");
		 return;
	 }
	 ACS_CS_API_TRACER_MESSAGE("getMatchingCpObjects() -> model->size() == %d", (int)model->size());

	 set<const ACS_CS_ImBase *>::iterator cpIt;
	 set<const ACS_CS_ImBase *> cpObjects;
	 set<const ACS_CS_ImBase *> clusterObject;

	 model->getObjects(clusterObject, CPCLUSTER_T);
	 model->getObjects(cpObjects, CLUSTERCP_T);

	 if(clusterObject.size() != 1)
	 {
		 ACS_CS_API_TRACER_MESSAGE("Leaving getMatchingCpObjects() - 3");
		 return;
	 }

	 set<string> cpNames;
	 set<string>::iterator nameIt;
	 const ACS_CS_ImBase *cpClusterBase = *clusterObject.begin();
	 const ACS_CS_ImCpCluster *cpCluster = dynamic_cast<const ACS_CS_ImCpCluster*>(cpClusterBase);

	 if (!cpCluster)
		 return;

	 if(groupName.compare("ALLBC") == 0)
	 {
		 cpNames = cpCluster->allBcGroup;
	 }
	 else if(groupName.compare("OPGROUP") == 0)
	 {
		 cpNames = cpCluster->operativeGroup;
	 }

	 for(nameIt = cpNames.begin(); nameIt != cpNames.end(); nameIt++)
	 {
		 string cpName = *nameIt;
		 transform(cpName.begin(), cpName.end(), cpName.begin(), (int(*)(int))toupper);

		 for(cpIt= cpObjects.begin(); cpIt != cpObjects.end(); cpIt++){

			 const ACS_CS_ImBase *base = *cpIt;
			 const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

			 if(cp)
			 {
				 string defaultCpName = ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn);
				 transform(defaultCpName.begin(), defaultCpName.end(), defaultCpName.begin(), (int(*)(int))toupper);

				 if(defaultCpName.compare(cpName) == 0)
				 {
					 cpSet.insert(cp);
				 }
			 }
		 }
	 }
	 ACS_CS_API_TRACER_MESSAGE("Leaving getMatchingCpObjects() - 4");
 }


