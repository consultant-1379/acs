/*
 * ACS_CS_ComputeResourceHandler.cpp
 *
 *  Created on: Mar 31, 2015
 *      Author: eanform
 */

#include "ACS_CS_ComputeResourceHandler.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_RoleAssignment.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_NetworkHandler.h"

#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "ACS_CS_ImSerialization.h"


ACS_CS_Trace_TDEF(ACS_CS_ComputeResourceHandler_TRACE);

ACS_CS_ComputeResourceRole ACS_CS_ComputeResourceRole::nullObject;

ACS_CS_CrMgmtHandler::ACS_CS_CrMgmtHandler(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId, ACS_APGCC_AttrValues** attr, ACS_CS_ImBase* base,
		ACS_APGCC_CcbHandle& ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle& adminOwnwrHandlerVal,
		ACS_CS_ComputeResourceHandler* p_computeResourceHandler) {
	ACS_CS_FTRACE((ACS_CS_ComputeResourceHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->attr=attr;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->base=base;
	this->computeResourceHandler=p_computeResourceHandler;
}

ACS_CS_CrMgmtHandler::~ACS_CS_CrMgmtHandler()
{

}

int ACS_CS_CrMgmtHandler::create()
{
	return 0;
}

int ACS_CS_CrMgmtHandler::remove()
{
	int result = 0;
	ACS_CS_FTRACE((ACS_CS_ComputeResourceHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	const ACS_CS_ImModelSubset* subset = \
			dynamic_cast<const ACS_CS_ImModelSubset*> (ACS_CS_ImRepository::instance()->getSubset(ccbId));
	ACS_CS_ImModelSubset * crSubset;

	if(subset)
		crSubset = new ACS_CS_ImModelSubset(*subset);

	if(crSubset) {
		this->computeResourceHandler->setAugmentCcbHandles(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
		if(!this->computeResourceHandler->handleComputeResourceDeletion(crSubset))
			result = 1; // failure
		delete crSubset;
	}
	else
		result = 1; // failure

	this->computeResourceHandler->clearAugmentCcbHandles();
	return result;
}

int ACS_CS_CrMgmtHandler::modify()
{
	return 0;
}

void ACS_CS_ComputeResourceHandler::setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		ACS_APGCC_AttrValues **attr, ACS_CS_ImBase *base,
		ACS_APGCC_CcbHandle &ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal)
{
	ACS_CS_FTRACE((ACS_CS_ComputeResourceHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->attr=attr;
	this->base=base;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;

	augmentHandlesAvailable = true;
}

void  ACS_CS_ComputeResourceHandler::clearAugmentCcbHandles()
{
	ACS_CS_FTRACE((ACS_CS_ComputeResourceHandler_TRACE, LOG_LEVEL_DEBUG,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	this->oiHandle=0;
	this->ccbId=0;
	this->attr=0;
	this->base=0;
	this->ccbHandleVal=0;
	this->adminOwnwrHandlerVal=0;

	augmentHandlesAvailable = false;
}


ACS_CS_ComputeResourceHandler::ACS_CS_ComputeResourceHandler()
{
	augmentHandlesAvailable = false;

	reloadTableAtStartup();

	//create cs folder on data disk
	if (ACS_APGCC::create_directories(ACS_CS_NS::ACS_DATA_PATH.c_str(), ACCESSPERMS) == -1)
	{
		ACS_CS_FTRACE((ACS_CS_ComputeResourceHandler_TRACE, LOG_LEVEL_ERROR,  "(%t) [%s@%d] Failed to create the folder %s.\n", __FUNCTION__, __LINE__, ACS_CS_NS::ACS_DATA_PATH.c_str()));
	}
}

ACS_CS_ComputeResourceHandler::~ACS_CS_ComputeResourceHandler()
{

}

int ACS_CS_ComputeResourceHandler::handleRequest (ACS_CS_PDU */*pdu*/)
{
	// TODO CS Protocol not implemented for ComputeResourceTable
	return 0;
}

int ACS_CS_ComputeResourceHandler::newTableOperationRequest(ACS_CS_ImModelSubset *subset)
{
	ACS_CS_DEBUG(("Entering - before mutex"));
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);
	ACS_CS_DEBUG(("Entering - after mutex acquired"));
	if(augmentHandlesAvailable)
		ACS_CS_FTRACE((ACS_CS_ComputeResourceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));

	ACS_CS_ImModel * modelCopy = ACS_CS_ImRepository::instance()->getModelCopy();

	std::set <ComputeResourceSubtree_t> computeResourceSubtrees = getComputeResourceSubtrees(subset, modelCopy);

	if (computeResourceSubtrees.size() > 0)
	{
		storeComputeResourcesOnDisk();
	}

	if (updateTable(computeResourceSubtrees))
	{
		removeComputeResourceMap.clear();
		updateRoleAssignment();
	}

	handleComputeResourceTableSubscription (computeResourceSubtrees);

	delete modelCopy;

	return 0;
}

void ACS_CS_ComputeResourceHandler::applyNetworkHandlerDeletionTransaction(ACS_APGCC_CcbId ccbId) {
	ACS_CS_NetworkHandler::getInstance()->commitDeletionTransaction(ccbId);
}

void ACS_CS_ComputeResourceHandler::abortNetworkHandlerDeletionTransaction(ACS_APGCC_CcbId ccbId) {
	ACS_CS_NetworkHandler::getInstance()->abortDeletionTransaction(ccbId);
}


bool ACS_CS_ComputeResourceHandler::handleComputeResourceDeletion(ACS_CS_ImModelSubset *subset)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);
	if(augmentHandlesAvailable)
		ACS_CS_FTRACE((ACS_CS_ComputeResourceHandler_TRACE, LOG_LEVEL_ERROR,  "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));

	ACS_CS_ImModel * modelCopy = ACS_CS_ImRepository::instance()->getModelCopy();

	std::set <ComputeResourceSubtree_t> computeResourceSubtrees = getComputeResourceSubtrees(subset, modelCopy);


	int ret = false;
	if (updateTable(computeResourceSubtrees))
	{
		ret = releaseRoleAssignmentAugment();
	}

	handleComputeResourceTableSubscription (computeResourceSubtrees);

	if(modelCopy)
		delete modelCopy;

	return ret;
}

bool ACS_CS_ComputeResourceHandler::loadTable ()
{
	//populate compute resources map at start-up phase

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);
	computeResourceMap.clear();

	ACS_CS_ImModel* modelCopy = ACS_CS_ImRepository::instance()->getModelCopy();
	std::set<ComputeResourceSubtree_t> computeResourceSubtrees = getComputeResourceSubtrees(modelCopy);

	for (std::set <ComputeResourceSubtree_t>::iterator it = computeResourceSubtrees.begin(); it != computeResourceSubtrees.end(); ++it)
	{
		insertComputeResource(it->first, it->second);
	}

	delete(modelCopy);

	return true;
}

std::set<const ACS_CS_ImBase *> ACS_CS_ComputeResourceHandler::getComputeResourceObjects(const ACS_CS_ImModel* model)
{
	std::set <const ACS_CS_ImBase *> outputObjects;

	if (model)
	{
		model->getObjects(outputObjects, COMPUTERESOURCE_T);
	}

	return outputObjects;
}

std::set<ComputeResourceSubtree_t> ACS_CS_ComputeResourceHandler::getComputeResourceSubtrees(const ACS_CS_ImModel* subset, const ACS_CS_ImModel* wholeModel)
{
	ACS_CS_DEBUG(("CcbId[%d] - Entering"));
	std::set <ComputeResourceSubtree_t> outputSubtrees;

	if (subset)
	{
		std::set <const ACS_CS_ImBase *> computeResObjs;
		subset->getObjects(computeResObjs, COMPUTERESOURCE_T);

		for (std::set <const ACS_CS_ImBase *>::iterator it = computeResObjs.begin(); it != computeResObjs.end(); ++it)
		{
			ACS_CS_DEBUG(("CcbId[] - ComputeResource MOs present in subset!"));
			std::set <const ACS_CS_ImBase *> networkObjects;

			const ACS_CS_ImComputeResource* computeRes = dynamic_cast<const ACS_CS_ImComputeResource*>(*it);
			if (computeRes)
			{
				ACS_CS_DEBUG(("CcbId[] - ComputeResource MO == %s", computeRes->rdn.c_str()));
				for (std::set<std::string>::iterator dnIt = computeRes->networks.begin(); computeRes->networks.end() != dnIt; ++dnIt)
				{
					string netDn = *dnIt;
					//Look in the subset first....
					ACS_CS_DEBUG(("CcbId[] - ComputeResource MO == %s; network dn == %s", computeRes->rdn.c_str(),netDn.c_str()));
					ACS_CS_DEBUG(("CcbId[] - before getObject() - 1"));
					const ACS_CS_ImBase * networkBase = subset->getObject(netDn.c_str());
					ACS_CS_DEBUG(("CcbId[] - after getObject() - 1"));
					//...If not found look in the entire model
					if (!networkBase && wholeModel)
					{
						ACS_CS_DEBUG(("CcbId[] - before getObject() - 2"));
						networkBase = wholeModel->getObject(netDn.c_str());
						ACS_CS_DEBUG(("CcbId[] - after getObject() - 2"));
					}

					if (networkBase)
					{
						networkObjects.insert(networkBase);
					}
				}
			}

			outputSubtrees.insert(ComputeResourceSubtree_t(*it, networkObjects));
			ACS_CS_DEBUG(("CcbId[] - ADDING ComputeResource MO == %s", computeRes->rdn.c_str()));
		}
	}
	ACS_CS_DEBUG(("CcbId[] - Leaving"));
	return outputSubtrees;
}

void ACS_CS_ComputeResourceHandler::handleComputeResourceTableSubscription (std::set<ComputeResourceSubtree_t> /*computeResourceObjects*/)
{
	//TODO: Notification not implemented for ComputeResourceTable
}

ACS_CS_ComputeResourceRole ACS_CS_ComputeResourceHandler::getComputeResourceRole(const ACS_CS_ImBase* computeResourceBaseObject, std::set <const ACS_CS_ImBase *> networkObjects)
{
	ACS_CS_ComputeResourceRole outputComputeResourceRole(ACS_CS_ComputeResourceRole::nullObject);

	const ACS_CS_ImComputeResource* computeResourceObject = dynamic_cast<const ACS_CS_ImComputeResource*> (computeResourceBaseObject);

	if (computeResourceObject)
	{
		outputComputeResourceRole.id = ACS_CS_ImUtils::getIdValueFromRdn(computeResourceObject->computeResourceId);
		outputComputeResourceRole.uuid = computeResourceObject->uuid;
		outputComputeResourceRole.type = (RoleType) computeResourceObject->crType;
		outputComputeResourceRole.roleLabel = computeResourceObject->crRoleLabel;
		outputComputeResourceRole.roleAssignment = computeResourceObject->crRoleId;
		outputComputeResourceRole.macAddressEthA = computeResourceObject->macAddressEthA;
		outputComputeResourceRole.macAddressEthB = computeResourceObject->macAddressEthB;
		outputComputeResourceRole.ipAddressEthA = computeResourceObject->ipAddressEthA;
		outputComputeResourceRole.ipAddressEthB = computeResourceObject->ipAddressEthB;

		for (std::set <const ACS_CS_ImBase *>::iterator networkIt = networkObjects.begin(); networkObjects.end() != networkIt; ++networkIt)
		{
			const ACS_CS_ImComputeResourceNetwork *imNetwork = dynamic_cast<const ACS_CS_ImComputeResourceNetwork *>(*networkIt);
			if (imNetwork)
			{
				ACS_CS_ComputeResourceNetwork network;

				network.dn = imNetwork->rdn;
				network.nicName = imNetwork->nicName;
				network.name = imNetwork->netName;
				network.mac = imNetwork->macAddress;

				outputComputeResourceRole.networks.insert(network);
			}

		}

	}

	return outputComputeResourceRole;
}

std::string ACS_CS_ComputeResourceHandler::getComputeResourceKey(const ACS_CS_ImBase* computeResourceBaseObject)
{
	std::string outputId;

	const ACS_CS_ImComputeResource* computeResourceObject = dynamic_cast<const ACS_CS_ImComputeResource*> (computeResourceBaseObject);

	if (computeResourceObject)
	{
		outputId = computeResourceObject->uuid;
	}

	return outputId;
}

void ACS_CS_ComputeResourceHandler::insertComputeResource(const ACS_CS_ImBase* computeResourceBaseObject, std::set <const ACS_CS_ImBase *> networkObjects)
{
	//add compute resource in the map

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);

	ACS_CS_ComputeResourceRole computeResourceRole = getComputeResourceRole(computeResourceBaseObject, networkObjects);
	std::string computeResourceKey = getComputeResourceKey(computeResourceBaseObject);

	if (computeResourceRole != ACS_CS_ComputeResourceRole::nullObject && !computeResourceKey.empty())
	{
		computeResourceMap.insert(ComputeResourcePair(computeResourceKey, computeResourceRole));
	}
}

void ACS_CS_ComputeResourceHandler::removeComputeResource(const ACS_CS_ImBase* computeResourceBaseObject)
{
	//remove compute resource from the map

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);
	ACS_CS_ComputeResourceRole computeResourceRole = getComputeResourceRole(computeResourceBaseObject);
	std::string computeResourceKey = getComputeResourceKey(computeResourceBaseObject);
	ComputeResourceMap::iterator computeResourceToRemove = computeResourceMap.find(computeResourceKey);

	if (computeResourceRole != ACS_CS_ComputeResourceRole::nullObject && !computeResourceKey.empty()
			&& computeResourceToRemove != computeResourceMap.end())  //don't add it to removeMap if it is already deleted from computeResourceMap
	{
		removeComputeResourceMap.insert(ComputeResourcePair(computeResourceKey, computeResourceRole));
	}

	if (computeResourceToRemove != computeResourceMap.end())
	{
		computeResourceMap.erase(computeResourceToRemove);
	}
}

void ACS_CS_ComputeResourceHandler::modifyComputeResource(const ACS_CS_ImBase* computeResourceBaseObject, std::set <const ACS_CS_ImBase *> networkObjects)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);

	ACS_CS_ComputeResourceRole modifiedComputeResourceRole = getComputeResourceRole(computeResourceBaseObject, networkObjects);
	std::string computeResourceKey = getComputeResourceKey(computeResourceBaseObject);

	if (modifiedComputeResourceRole != ACS_CS_ComputeResourceRole::nullObject && !computeResourceKey.empty())
	{
		ComputeResourceMap::iterator computeResourceToModify = computeResourceMap.find(computeResourceKey);

		//Compute Resource object already exists in the list. Remove the old one from the list.
		if (computeResourceToModify != computeResourceMap.end())
		{
			modifiedComputeResourceRole.roleAssigmentState = computeResourceToModify->second.roleAssigmentState;

			if ( (modifiedComputeResourceRole.hasRoleRevoked() || modifiedComputeResourceRole.hasRoleFree() )
					&& computeResourceToModify->second.hasValidRole())
			{
				//Modify operation is revoking role. This means that the role shall not be added to the list of modified ones, but
				// shall only be removed. We should also set assignmentState to UNASSIGNED for modified object
				removeComputeResourceMap.insert(ComputeResourcePair(computeResourceKey, computeResourceToModify->second));
				modifiedComputeResourceRole.roleAssigmentState = ROLE_UNASSIGNED;
			}
			else if (modifiedComputeResourceRole.macAddressEthA != computeResourceToModify->second.macAddressEthA ||
					modifiedComputeResourceRole.macAddressEthB != computeResourceToModify->second.macAddressEthB ||
					modifiedComputeResourceRole.ipAddressEthA != computeResourceToModify->second.ipAddressEthA ||
					modifiedComputeResourceRole.ipAddressEthB != computeResourceToModify->second.ipAddressEthB ||
					modifiedComputeResourceRole.networks != computeResourceToModify->second.networks )
			{
				//Something's changed on ComputeResource object. Add it to the list of modified ones.
				modifyComputeResourceMap.insert(ComputeResourcePair(computeResourceKey, modifiedComputeResourceRole));
			}
			//else... modify didn't require an IMM update. Just refresh the map, nothing to change on IMM

			computeResourceMap.erase(computeResourceToModify);
		}

		// Replace old object with new one.
		computeResourceMap.insert(ComputeResourcePair(computeResourceKey, modifiedComputeResourceRole));
	}
}

bool ACS_CS_ComputeResourceHandler::updateTable(std::set<ComputeResourceSubtree_t> computeResourceSubtrees)
{
	bool tableUpdated = false;

	for (std::set <ComputeResourceSubtree_t>::iterator it = computeResourceSubtrees.begin(); it != computeResourceSubtrees.end(); ++it)
	{
		switch(it->first->action)
		{
		case ACS_CS_ImBase::CREATE:
			insertComputeResource(it->first, it->second);
			tableUpdated = true;
			break;
		case ACS_CS_ImBase::DELETE:
			removeComputeResource(it->first);
			tableUpdated = true;
			break;
		case ACS_CS_ImBase::MODIFY:
			modifyComputeResource(it->first, it->second);
			tableUpdated = true;
			break;
		default:
			break;
		}
	}

	return tableUpdated;
}

void ACS_CS_ComputeResourceHandler::updateRoleAssignment()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);

	ACS_CS_RoleTableUpdate roleUpdate(&computeResourceMap, &removeComputeResourceMap, &modifyComputeResourceMap);

	if(augmentHandlesAvailable)
		roleUpdate.setAugmentCcbHandles(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);

	roleUpdate.updateRoles();

	removeComputeResourceMap.clear();
	modifyComputeResourceMap.clear();
}

bool ACS_CS_ComputeResourceHandler::releaseRoleAssignmentAugment()
{
	bool ret = false;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);

	ACS_CS_RoleTableUpdate roleUpdate(&computeResourceMap, &removeComputeResourceMap, &modifyComputeResourceMap);

	if(augmentHandlesAvailable)
		roleUpdate.setAugmentCcbHandles(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);

	ret = roleUpdate.removeRolesAugment();

	removeComputeResourceMap.clear();
	modifyComputeResourceMap.clear();
	return ret;
}

void ACS_CS_ComputeResourceHandler::reloadTableAtStartup()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(computeResourceMapMutex);

	loadTable();
	updateRoleAssignment();
}

void ACS_CS_ComputeResourceHandler::storeComputeResourcesOnDisk ()
{
	ACS_CS_ImModel *currentModel = ACS_CS_ImRepository::instance()->getModelCopy();

	if (currentModel)
	{
		ACS_CS_ImModel *tmpModel = getComputeResourcesModel(currentModel);

		if (tmpModel)
		{
			//ComputeResource MOCs are stored on data disk in /data/acs/data/cs/ComputeResource file.
			try
			{
				std::string fileName = ACS_CS_NS::COMPUTE_RESOURCE_FILE;
				std::ofstream ofs(fileName.c_str());
				boost::archive::binary_oarchive ar(ofs);
				ar << * tmpModel;
			}
			catch (boost::archive::archive_exception& e)
			{
				ACS_CS_TRACE((ACS_CS_ComputeResourceHandler_TRACE,"storeComputeResourcesOnDisk() Serialization exception occurred: %s.", e.what()));
			}
			catch (boost::exception& )
			{
				ACS_CS_TRACE((ACS_CS_ComputeResourceHandler_TRACE,"storeComputeResourcesOnDisk() Serialization exception occurred."));
			}

			delete tmpModel;
		}

		delete currentModel;
	}
}

void ACS_CS_ComputeResourceHandler::restoreComputeResourcesFromDisk ()
{
	ACS_CS_ImModel * model = new ACS_CS_ImModel();

	//Load from ComputeResource file
	try
	{
		std::string ComputeResource = ACS_CS_NS::COMPUTE_RESOURCE_FILE;
		std::ifstream file(ComputeResource.c_str());
		boost::archive::binary_iarchive ia(file);

		//check achive version compatibility
		if (ia.get_library_version() < BOOST_ARCHIVE_VERSION)
		{
			ACS_CS_TRACE((ACS_CS_ComputeResourceHandler_TRACE, "restoreComputeResourcesFromDisk() Boost archive version NOT SUPPORTED!!!."));

			//remove data file
			std::remove(ACS_CS_NS::COMPUTE_RESOURCE_FILE.c_str());

			delete model;
			return;
		}

		ia >> *model;
	}
	catch (boost::archive::archive_exception& e)
	{
		ACS_CS_TRACE((ACS_CS_ComputeResourceHandler_TRACE,"restoreComputeResourcesFromDisk() Serialization exception occurred: %s.", e.what()));
	}
	catch (boost::exception& )
	{
		ACS_CS_TRACE((ACS_CS_ComputeResourceHandler_TRACE,"restoreComputeResourcesFromDisk() Serialization exception occurred."));
	}

	if (model)
	{
		const ACS_CS_ImModel *currentModel = ACS_CS_ImRepository::instance()->getModelCopy();

		if (currentModel)
		{
			ACS_CS_ImModel *currentComputeResourceModel = getComputeResourcesModel(currentModel);

			if (currentComputeResourceModel)
			{
				ACS_CS_ImModel *diffModel = currentComputeResourceModel->findDifferences(model);
				if (diffModel && diffModel->size() > 0)
				{
					ACS_CS_ImModelSaver saver(diffModel);
					ACS_CC_ReturnType saved = saver.save("Restore-ComputeResource");
					if(ACS_CC_FAILURE == saved)
					{
						ACS_CS_TRACE((ACS_CS_ComputeResourceHandler_TRACE,"restoreComputeResourcesFromDisk() Could not restore Quorum Data"));
					}

					delete diffModel;
				}

				delete currentComputeResourceModel;
			}

			delete currentModel;
		}

		delete model;
	}
}

ACS_CS_ImModel * ACS_CS_ComputeResourceHandler::getComputeResourcesModel(const ACS_CS_ImModel* model)
{
	ACS_CS_ImModel *currentComputeResourceModel = new ACS_CS_ImModel();

	if (currentComputeResourceModel)
	{
		std::set <const ACS_CS_ImBase *> computeResourceObjects = getComputeResourceObjects(model);
		for (std::set <const ACS_CS_ImBase *>::iterator it = computeResourceObjects.begin(); it != computeResourceObjects.end(); ++it)
		{
			ACS_CS_ImBase* addObj = (*it)->clone();
			if(addObj)
			{
				currentComputeResourceModel->addObject(addObj);
			}
		}

		return currentComputeResourceModel;
	}

	return 0;

}
