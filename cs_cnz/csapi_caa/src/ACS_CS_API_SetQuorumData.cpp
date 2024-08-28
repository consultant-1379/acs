/*
 * @file ACS_CS_API_SetQuorumData.cpp
 * @author xmikhal
 * @date Dec 20, 2010
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */

#include "ACS_CS_Util.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_API_SetQuorumData.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImSerialization.h"
//#include "ACS_CS_CPGroup_Administration.h"

#include "ACS_CS_API_Tracer.h"
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
ACS_CS_API_TRACER_DEFINE(ACS_CS_API_SetQuorum_TRACE);


using namespace ACS_CS_Protocol;



ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_SetQuorumData::setCpData (ACS_CS_ImModel* tmpModel, std::vector<ACS_CS_API_CpData_R1>& cpDataVector)
{
	ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess; //default return value

	ACS_CS_API_TRACER_MESSAGE("entered");


	ACS_CS_ImModel *model = new ACS_CS_ImModel();
	ACS_CS_ImCpCluster *cpCluster = 0;

	if(!model)
	{
		return returnValue;
	}

	if (ACS_CS_API_Util_Internal::isClusterAlignedAfterRestore())
	{
		ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
		if(!immReader)
		{
			delete model;
			return returnValue;
		}

		if(!immReader->loadCpObjects(model)) {
			delete immReader;
			delete model;
			return returnValue;
		}

		cpCluster = dynamic_cast<ACS_CS_ImCpCluster*>(immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER));

		if (!cpCluster)
		{
			delete immReader;
			delete model;
			return returnValue;
		}

		delete immReader;
	}
	else
	{
		//Load from QuorumData file
		try
		{
			std::string QuorumDataFile = ACS_CS_NS::PATH_QUORUM_DATA_FILE;
			std::ifstream file(QuorumDataFile.c_str());
			boost::archive::binary_iarchive ia(file);

			//check achive version compatibility
			if (ia.get_library_version() < BOOST_ARCHIVE_VERSION)
			{
				ACS_CS_API_TRACER_MESSAGE(("setQuorumData() Boost archive version NOT SUPPORTED !!!"));

				//remove quorum data file
				std::remove(ACS_CS_NS::PATH_QUORUM_DATA_FILE.c_str());

				delete model;
				return returnValue;
			}

			ia >> *model;
		}
		catch (boost::archive::archive_exception& e)
		{
			ACS_CS_API_TRACER_MESSAGE("setQuorumData() Serialization exception occurred: %s.", e.what());
		}
		catch (boost::exception& )
		{
			ACS_CS_API_TRACER_MESSAGE("setQuorumData() Serialization exception occurred.");
		}

		cpCluster = dynamic_cast<ACS_CS_ImCpCluster*>(model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str()));

		if (!cpCluster)
		{
			delete model;
			return returnValue;
		}

		cpCluster = dynamic_cast<ACS_CS_ImCpCluster*>(cpCluster->clone());

		if (!cpCluster)
		{
			delete model;
			return returnValue;
		}
	}

	set<const ACS_CS_ImBase *>::iterator clusterCpIt;
	set<const ACS_CS_ImBase *> clusterCpObjects;

	model->getObjects(clusterCpObjects, CLUSTERCP_T);


	CPID *cpidList = new CPID[cpDataVector.size()];
	const unsigned numEntries = cpDataVector.size();

	for (size_t i = 0;i < cpDataVector.size(); i++) {
		cpidList[i] = cpDataVector[i].id;
	}

	if(0 == numEntries)
	{
		// Received a request with empty quorum data, which means
		// the OG is dissolved due to cluster recovery or some other reason.
		// DO NOT return here, because we need to reset the quorum data for all CPs.
		ACS_CS_API_TRACER_MESSAGE("Warning: Reset the quorum data for all CPs due to the empty quorum data received");
	}


	returnValue = ACS_CS_API_SET_NS::Result_Success;

	// Received the quorum data for a couple of CPs.
	// CS should reset the quorum data for those CPs that alinere no longer in the OG,
	// that is, the CPs which don't have an update in the quorum data received.

	// Iterate through the current CP list and check if there is an update for each CP
	for(clusterCpIt = clusterCpObjects.begin(); clusterCpIt != clusterCpObjects.end(); clusterCpIt++) {

		const ACS_CS_ImBase *base = *clusterCpIt;
		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

		if(!cp)
			break;

		ACS_CS_API_NS::CpState state = 0;
		ACS_CS_API_NS::ApplicationId appid = 0;
		ACS_CS_API_NS::ApzSubstate apzss = 0;
		ACS_CS_API_NS::StateTransition tcode = 0;
		ACS_CS_API_NS::AptSubstate aptss = 0;
		ACS_CS_API_NS::BlockingInfo blockingInfo = 0;
		ACS_CS_API_NS::CpCapacity cpCapacity = 0;

		// Check if we have new data for this CP
		bool updateReceived = false;
		bool updateNeeded = false;
		unsigned int x = 0;

		for (; x < numEntries; x++) {
			if (cp->systemIdentifier == cpidList[x]) {
				updateReceived = updateNeeded = true;
				break;
			}
		}

		if (updateReceived)
		{
			// Check if the quorum data received is for a single-sided CP or not,
			// depending on if its CPID is in the range of [0, 999] or not.
			//TODO: Probably we don't need this check any more...
			if (cpidList[x] >= ACS_CS_Protocol::SysType_CP) {
				ACS_CS_API_TRACER_MESSAGE("Quorum Data received for a non single-sided CP (%u). Ignoring the update.",
						cpidList[x]);

				continue;
			}

			ACS_CS_API_CpData_R1 *currentCpData = getCpDataByCpId(cpDataVector, cpidList[x]);

			state = currentCpData->state;
			appid = currentCpData->applicationId;
			apzss = currentCpData->apzSubstate;
			tcode = currentCpData->stateTransition;
			aptss = currentCpData->aptSubstate;
			blockingInfo = currentCpData->blockingInfo;
			cpCapacity = currentCpData->cpCapacity;

		}
		else
		{
			// This CP doesn't have an update coming in from the CP side,
			// which means it is no longer in the OG and we need to
			// check if its quorum data needs to be reset or not.

			if (cp->cpState != state)
				updateNeeded = true;

			// No need to check ApplicationId here, because it should NOT be reset by CS.
			if (!updateNeeded)
			{
				if (cp->apzSubstate != apzss)
					updateNeeded = true;
			}

			if (!updateNeeded)
			{
				if (cp->stateTransition != tcode)
					updateNeeded = true;
			}

			if (!updateNeeded)
			{
				if (cp->aptSubstate != aptss)
					updateNeeded = true;
			}

			if (!updateNeeded)
			{
				if(cp->blockingInfo != blockingInfo)
					updateNeeded = true;
			}

			if (!updateNeeded)
			{
				if(cp->cpCapacity != cpCapacity)
					updateNeeded = true;
			}
		}

		if (updateNeeded)
		{
			ACS_CS_API_TRACER_MESSAGE(
					"Update needed for: CPID: %d, CP state: %d, applicationId: %d, apzSubstate: %d, stateTransition: %d, aptSubstate: %d, blockingInfo: %d, cpCapacity: %d",
					cp->systemIdentifier, state, appid, apzss, tcode, aptss, blockingInfo, cpCapacity );

			ACS_CS_ImClusterCp* modCp = new ACS_CS_ImClusterCp(cp);

			modCp->cpState = (int32_t) state;
			modCp->apzSubstate = (int32_t) apzss;
			modCp->stateTransition = (int32_t) tcode;
			modCp->aptSubstate = (int32_t) aptss;
			modCp->blockingInfo = (uint16_t) blockingInfo;
			modCp->cpCapacity = (uint32_t) cpCapacity;
			modCp->action = ACS_CS_ImBase::MODIFY;

			if (updateReceived) // ApplicationId should only be set when update is received.
				modCp->applicationId = (int32_t) appid;

			// Update the cp group table if needed
			int returnCode = updateCPGroupTable(modCp, tmpModel, cpCluster);

			if (returnCode != 0)
				ACS_CS_API_TRACER_MESSAGE("updateCpGroupTable() returned failure, rc: %d", returnCode);

			tmpModel->addObject(modCp);
		}
	}

	ACS_CS_ImBase* tmp = tmpModel->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());
	if (!tmp)
	{
		delete cpCluster;
	}

	if (model)
		delete model;

	if (cpidList)
		delete[] cpidList;

	return returnValue;

}


ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_SetQuorumData::setTrafficLeader (ACS_CS_ImModel *tmpModel, CPID trafficLeader)
{
	ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess; //default return value

	if (trafficLeader != ACS_CS_NS::UNDEFINED_CPID && !ACS_CS_API_Util::isCPDefined(trafficLeader))
	{
		return ACS_CS_API_SET_NS::Result_Incorrect_CP_Id;
	}

	ACS_CS_ImBase *advBase = 0;

	advBase = tmpModel->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
	bool addToModel = false;

	if (!advBase) {
		ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
		advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
		addToModel = true;
		delete immReader;
	}

	if (!advBase)
		return returnValue;

	ACS_CS_ImAdvancedConfiguration* advInfo = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

	if(advInfo != NULL)
	{
		advInfo->trafficLeaderCp = (uint16_t) trafficLeader;
		advInfo->action = ACS_CS_ImBase::MODIFY;
		returnValue = ACS_CS_API_SET_NS::Result_Success;

		if (addToModel) {
			if (!tmpModel->addObject(advInfo))
				returnValue = ACS_CS_API_SET_NS::Result_Failure;
		}
	} else if (addToModel) {
		delete advBase;
	}


	return returnValue;

}


ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_SetQuorumData::setTrafficIsolated (ACS_CS_ImModel *tmpModel, CPID trafficIsolated)
{
	ACS_CS_API_SET_NS::CS_API_Set_Result returnValue = ACS_CS_API_SET_NS::Result_NoAccess; //default return value

	if (trafficIsolated != ACS_CS_NS::UNDEFINED_CPID && !ACS_CS_API_Util::isCPDefined(trafficIsolated))
	{
		return ACS_CS_API_SET_NS::Result_Incorrect_CP_Id;
	}


	ACS_CS_ImBase *advBase = 0;

	advBase = tmpModel->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
	bool addToModel = false;

	if (!advBase) {
		ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
		advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
		addToModel = true;
		delete immReader;
	}

	if (!advBase)
		return returnValue;

	ACS_CS_ImAdvancedConfiguration* advInfo = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

	if(advInfo != NULL)
	{
		advInfo->trafficIsolatedCp = (uint16_t) trafficIsolated;
		advInfo->action = ACS_CS_ImBase::MODIFY;
		returnValue = ACS_CS_API_SET_NS::Result_Success;

		if (addToModel) {
			if (!tmpModel->addObject(advInfo))
				returnValue = ACS_CS_API_SET_NS::Result_Failure;
		}
	} else if (addToModel) {
		delete advBase;
	}


	return returnValue;
}


bool ACS_CS_API_SetQuorumData::getCpIdList(ACS_CS_API_IdList &cpList)
{
	bool result = false;

	ACS_CS_API_CP *cpApi = ACS_CS_API::createCPInstance();

	if (cpApi) {
		if (cpApi->getCPList(cpList) == ACS_CS_API_NS::Result_Success) {
			result = true;
		}
	}

	if (cpApi)
		ACS_CS_API::deleteCPInstance(cpApi);

	return result;
}


ACS_CS_API_CpData_R1* ACS_CS_API_SetQuorumData::getCpDataByCpId(std::vector<ACS_CS_API_CpData_R1> &cpDataVector, CPID cpId)
{
	for (size_t i = 0; i < cpDataVector.size(); i++) {
		if (cpDataVector[i].id == cpId) {
			return &cpDataVector[i];
		}
	}

	return NULL;
}


int ACS_CS_API_SetQuorumData::updateCPGroupTable (ACS_CS_ImClusterCp *cp, ACS_CS_ImModel* model, ACS_CS_ImCpCluster* intialCpCluster)
{
	ACS_CS_API_TRACER_MESSAGE("entered");

	//TODO: Probably we don't need this check anymore
	if (cp->systemIdentifier >= (SysType_BC + 1000))
	{
		ACS_CS_API_TRACER_MESSAGE("Skipping non-blade CP");
		return 1; // Skip non-blade CP
	}

	//TODO: Probably we don't need to check for missing attributes anymore
	/*
     // CP State attribute
     ACS_CS_Attribute cpStateAtt =
             tableEntry->getValue(ACS_CS_Protocol::Attribute_CPId_CPState);

     ACS_CS_API_NS::CpState cpState;

     if (cpStateAtt.getValueLength() == sizeof(cpState))
     {
        // Get the cp State value
        cpStateAtt.getValue(reinterpret_cast<char *>(&cpState), sizeof(cpState));
     }
     else  // Missing attribute or invalid size.
     {
        // Size of 0 indicates attribute not present in the tableEntry.
        // log failure and continue with the next CP in the list.
        ACS_CS_TRACE((ACS_CS_API_SetQuorum_TRACE,
                "(%t) %s, Missing or invalid cpState entry for CPID %u (size %u). \n",
               __FUNCTION__, tableEntry->getId(), cpStateAtt.getValueLength()));

        return 2;
     }

     // CP ApzSubstate attribute
     ACS_CS_Attribute cpApzSubstateAtt =
             tableEntry->getValue(ACS_CS_Protocol::Attribute_CPId_ApzSubstate);

     ACS_CS_API_NS::ApzSubstate cpApzSubstate;

     if (cpApzSubstateAtt.getValueLength() == sizeof(cpApzSubstate))
     {
        // Get the cp ApzSubstate value
        cpApzSubstateAtt.getValue(reinterpret_cast<char *>(&cpApzSubstate),
                sizeof(cpApzSubstate));
     }
     else  // Missing attribute or invalid size.
     {
        // Size of 0 indicates attribute not present in the tableEntry.
        // log failure and continue with the next CP in the list.
        ACS_CS_TRACE((ACS_CS_API_SetQuorum_TRACE,
                "(%t) %s, Missing or invalid cpApzSubstate entry for CPID %u (size %u). \n",
                __FUNCTION__, tableEntry->getId(), cpApzSubstateAtt.getValueLength()));

        return 3;
     }

     // CP State Transition Code attribute
     ACS_CS_Attribute cpStateTransCodeAtt =
             tableEntry->getValue(ACS_CS_Protocol::Attribute_CPId_StateTransCode);

     ACS_CS_API_NS::StateTransition cpStateTransCode;

     if (cpStateTransCodeAtt.getValueLength() == sizeof(cpStateTransCode))
     {
        // Get the cp State Transition Code value
        cpStateTransCodeAtt.getValue(reinterpret_cast<char *>(&cpStateTransCode),
                sizeof(cpStateTransCode));
     }
     else  // Missing attribute or invalid size.
     {
        // Size of 0 indicates attribute not present in the tableEntry.
        // log failure and continue with the next CP in the list.
        ACS_CS_TRACE((ACS_CS_API_SetQuorum_TRACE,
                "(%t) %s, Missing or invalid cpStateTransCode entry for CPID %u (size %u). \n",
                __FUNCTION__, tableEntry->getId(), cpStateTransCodeAtt.getValueLength()));

        return 4;
     }

     // Application Identity Attribute
     ACS_CS_Attribute applicationIdAtt =
             tableEntry->getValue(ACS_CS_Protocol::Attribute_CPId_ApplicationId);

     ACS_CS_API_NS::ApplicationId applicationId;

     if (applicationIdAtt.getValueLength() == sizeof(applicationId))
     {
        // Get the application Id value
        applicationIdAtt.getValue(reinterpret_cast<char *>(&applicationId),
                sizeof(applicationId));
     }
     else  // Missing attribute or invalid size.
     {
        // Size of 0 indicates attribute not present in the tableEntry.
        // log failure and continue with the next CP in the list.
        ACS_CS_TRACE((ACS_CS_API_SetQuorum_TRACE,
                "(%t) %s, Missing or invalid applicationId entry for CPID %u (size %u). \n",
                __FUNCTION__, tableEntry->getId(), applicationIdAtt.getValueLength()));

        return 5;
     }
	 */


	ACS_CS_API_NS::CpState cpState = cp->cpState;
	ACS_CS_API_NS::ApzSubstate cpApzSubstate = cp->apzSubstate;
	ACS_CS_API_NS::StateTransition cpStateTransCode = cp->stateTransition;
	ACS_CS_API_NS::ApplicationId applicationId = cp->applicationId;



	ACS_CS_API_TRACER_MESSAGE(
			"%s (%u). State=(%u), ApzSubstate=(%u), StateTransCode=(%u), AppliationId=(%u) \n",
			"Received update for CPID", cp->systemIdentifier,
			cpState, cpApzSubstate, cpStateTransCode, applicationId);

	// The following Quorum Data related constants are defined
	// by the Cluster Handler IWD, 1/155 19-ANZ 250 03.
	static const int ApplicationId_Tsc = 1;
	static const int CpState_Active = 3;
	static const int CpState_Intermediate = 4;
	static const int ApzSubstate_TimeSupervision = 1;
	static const int StateTransition_Active_to_Intermediate = 8;


	ACS_CS_ImBase *clusterBase = 0;
	bool addToModel = false;

	ACS_CS_ImCpCluster* cluster = 0;
	clusterBase = model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());

	if (!clusterBase) {

		cluster = intialCpCluster;
		addToModel = true;
	}
	else
	{
		cluster = dynamic_cast<ACS_CS_ImCpCluster*>(clusterBase);
	}

	if (!cluster) {
		return 6; //Some error code?!
	}


	if (ApplicationId_Tsc == applicationId)
	{
		// TSC blades should always be removed from the OperativeGroup.
		//         groupAdministrator->removeCpFromGroup(ACS_CS_NS::CP_GROUP_OPGROUP, tableEntry->getId());

		cluster->operativeGroup.erase(ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn));
		cluster->action = ACS_CS_ImBase::MODIFY;
		if (addToModel)
			model->addObject(cluster);
	}
	else if (CpState_Active == cpState)
	{
		//    	 groupAdministrator->addCpToGroup(ACS_CS_NS::CP_GROUP_OPGROUP, tableEntry->getId());
		cluster->operativeGroup.insert(ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn));
		cluster->action = ACS_CS_ImBase::MODIFY;
		if (addToModel)
			model->addObject(cluster);
	}
	else if (CpState_Intermediate == cpState &&
			ApzSubstate_TimeSupervision == cpApzSubstate)
	{
		// The apzSubstate numerical values take on different meanings depending
		// on the cpState.  TimeSupervision only applies in the Intermediate
		// state, so verify that we are in Intermediate state before checking
		// apzSubstate against time supervision.

		if (StateTransition_Active_to_Intermediate == cpStateTransCode)
		{
			//            groupAdministrator->addCpToGroup(ACS_CS_NS::CP_GROUP_OPGROUP, tableEntry->getId());
			cluster->operativeGroup.insert(ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn));
			cluster->action = ACS_CS_ImBase::MODIFY;
			if (addToModel)
				model->addObject(cluster);
		}
	}
	else
	{
		//         groupAdministrator->removeCpFromGroup(ACS_CS_NS::CP_GROUP_OPGROUP, tableEntry->getId());
		cluster->operativeGroup.erase(ACS_CS_ImUtils::getIdValueFromRdn(cp->rdn));
		cluster->action = ACS_CS_ImBase::MODIFY;
		if (addToModel)
			model->addObject(cluster);
	}

	return 0;
}

