/*
 * ACS_CS_RoleAssignment.cpp
 *
 *  Created on: Mar 31, 2015
 *      Author: eanform,estevol
 */

#include "ACS_CS_RoleAssignment.h"
#include "ACS_CS_NetworkHandler.h"
#include "ACS_CS_MethodHandler.h"

#define ROLE_ARRAY_NORMALIZATION_FACTOR 1000
#define IMM_MAX_RETRY 5U
#define IMM_RETRY_DELAY_MS	300U

ACS_CS_Trace_TDEF(ACS_CS_RoleAssignment_TRACE);

void ACS_CS_RoleTableUpdate::setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal)
{
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->attr=attr;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->base=base;

	augmentHandlesAvailable = true;
}

bool ACS_CS_RoleTableUpdate::removeRolesAugment()
{
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_DEBUG, "Entering"));
	//Check the list of removed roles first and release them.
	for (ComputeResourceMap::iterator it = removeComputeResourceMap->begin(); it != removeComputeResourceMap->end(); ++it)
	{
		ComputeResourcePair removeComputeResourcePair = *it;
		ACS_CS_ComputeResourceRole removeComputeResourceRole = removeComputeResourcePair.second;

		if (removeComputeResourceRole.hasValidRole())
		{
			ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Found ComputeResource %s with role %d to release",
					removeComputeResourceRole.id.c_str(),
					removeComputeResourceRole.roleAssignment));


			ACS_CS_RoleAssignment* roleAssignment = ACS_CS_RoleAssignment::createRoleAssignment(removeComputeResourceRole.type, removeComputeResourceRole.roleAssignment);
			if(augmentHandlesAvailable) {
				roleAssignment->setAugmentCcbHandles(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
				if(!roleAssignment->releaseRoleAugment(removeComputeResourceRole))
				{
					ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Failed to release role %d assigned to ComputeResource %s", roleAssignment->getAssignment(), removeComputeResourceRole.id.c_str()));
					delete roleAssignment;
					return false;
				}
			}
//			else {
//				if(!roleAssignment->releaseRole(removeComputeResourceRole))
//				{
//					ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Failed to release role %d assigned to ComputeResource %s", roleAssignment->getAssignment(), removeComputeResourceRole.id.c_str()));
//				}
//			}

			delete roleAssignment;
		}
	}
	return true;
}

void ACS_CS_RoleTableUpdate::updateRoles()
{
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_DEBUG, "Entering updateRoles()"));
//	//Check the list of removed roles first and release them.
//	for (ComputeResourceMap::iterator it = removeComputeResourceMap->begin(); it != removeComputeResourceMap->end(); ++it)
//	{
//		ComputeResourcePair removeComputeResourcePair = *it;
//		ACS_CS_ComputeResourceRole removeComputeResourceRole = removeComputeResourcePair.second;
//
//		if (removeComputeResourceRole.hasValidRole())
//		{
//			ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Found ComputeResource %s with role %d to release",
//					removeComputeResourceRole.id.c_str(),
//					removeComputeResourceRole.roleAssignment));
//
//
//			ACS_CS_RoleAssignment* roleAssignment = ACS_CS_RoleAssignment::createRoleAssignment(removeComputeResourceRole.type, removeComputeResourceRole.roleAssignment);
//			if(augmentHandlesAvailable) {
//				roleAssignment->setAugmentCcbHandles(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
//				if(!roleAssignment->releaseRoleAugment(removeComputeResourceRole))
//				{
//					ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Failed to release role %d assigned to ComputeResource %s", roleAssignment->getAssignment(), removeComputeResourceRole.id.c_str()));
//				}
//			}
//			else {
//				if(!roleAssignment->releaseRole(removeComputeResourceRole))
//				{
//					ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Failed to release role %d assigned to ComputeResource %s", roleAssignment->getAssignment(), removeComputeResourceRole.id.c_str()));
//				}
//			}
//
//			delete roleAssignment;
//		}
//	}

	// Now check for any compute resource that has been modified.
	for (ComputeResourceMap::iterator it = modifyComputeResourceMap->begin(); it != modifyComputeResourceMap->end(); ++it)
	{
//		ComputeResourcePair modifyComputeResourcePair = ;
		ACS_CS_ComputeResourceRole modifyComputeResourceRole = it->second;

		if (modifyComputeResourceRole.hasValidRole())
		{
			ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Found ComputeResource %s with role %d to modify. Role assignment state: %d",
					modifyComputeResourceRole.id.c_str(),
					modifyComputeResourceRole.roleAssignment,
					modifyComputeResourceRole.roleAssigmentState));

			ACS_CS_RoleAssignment* roleAssignment = ACS_CS_RoleAssignment::createRoleAssignment(modifyComputeResourceRole.type, modifyComputeResourceRole.roleAssignment);

			//Put computeResource in maintenance and then release it
			if (roleAssignment->setMaintenanceRole(modifyComputeResourceRole) )
			{
				if (roleAssignment->releaseRole(modifyComputeResourceRole) )
				{
					//Assign the role again in order to refresh compute resource info
					roleAssignment->assignRole(modifyComputeResourceRole);
				}
			}

			delete roleAssignment;
		}
	}

	// Now check for any compute resource without an assigned role or with a requested role or with a revoked role.
	for (ComputeResourceMap::iterator it = computeResourceMap->begin(); it != computeResourceMap->end(); ++it)
	{
		ComputeResourcePair computeResourcePair = *it;
		ACS_CS_ComputeResourceRole computeResourceRole = computeResourcePair.second;

		// Compute Resource is free: assign a new role if available.
		if (computeResourceRole.hasRoleFree())
		{
			ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Found ComputeResource %s with no role assigned. Role Assignment state: %d", computeResourceRole.id.c_str(), computeResourceRole.roleAssigmentState));

			ACS_CS_RoleAssignment* roleAssignment = getRoleAssignment(computeResourceRole);

			ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Assigning role %d to ComputeResource %s", roleAssignment->getAssignment(), computeResourceRole.id.c_str()));

			computeResourceRole.roleAssignment = roleAssignment->getAssignment();
			computeResourceRole.roleLabel = roleAssignment->getAssignmentLabel();

			if (roleAssignment->assignRole(computeResourceRole))
			{
				it->second.roleAssignment = roleAssignment->getAssignment();
				it->second.roleLabel = roleAssignment->getAssignmentLabel();
				it->second.roleAssigmentState = ROLE_ASSIGNED;

				ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Assigned role %d to ComputeResource %s. Role Assignment state: %d",
						it->second.roleAssignment, it->second.roleLabel.c_str(), it->second.roleAssigmentState));
			}
			else
			{
				ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Assignment of role %d to ComputeResource %s FAILED!", roleAssignment->getAssignment(), computeResourceRole.id.c_str()));
			}

			delete roleAssignment;
		}
		// Compute Resource have a Role "requested". Check if that role is available and assign it.
		else if (computeResourceRole.hasValidRole() && !computeResourceRole.isAssigned())
		{
			ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Found ComputeResource %s with role requested %d. Role Assignment state: %d",
					computeResourceRole.id.c_str(), computeResourceRole.roleAssignment, computeResourceRole.roleAssigmentState));

			ACS_CS_RoleAssignment* roleAssignment = ACS_CS_RoleAssignment::createRoleAssignment(computeResourceRole.type, computeResourceRole.roleAssignment);

			if (isRoleAvailable(roleAssignment, getOtherComputeResourceOfSameType(computeResourceRole)))
			{
				ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Requested role %d is free. Assigning it to ComputeResource %s", roleAssignment->getAssignment(), computeResourceRole.id.c_str()));

				computeResourceRole.roleAssignment = roleAssignment->getAssignment();
				computeResourceRole.roleLabel = roleAssignment->getAssignmentLabel();

				if (roleAssignment->assignRole(computeResourceRole))
				{
					it->second.roleAssignment = roleAssignment->getAssignment();
					it->second.roleLabel = roleAssignment->getAssignmentLabel();
					it->second.roleAssigmentState = ROLE_ASSIGNED;

					ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Assigned role %d to ComputeResource %s. Role Assignment state: %d",
							it->second.roleAssignment, it->second.roleLabel.c_str(), it->second.roleAssigmentState));
				}
				else
				{
					ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Assignment of role %d to ComputeResource %s FAILED!", roleAssignment->getAssignment(), computeResourceRole.id.c_str()));
				}
			}
			//else... Role requested is not available. Shall we delete the computeResource or assign it another role?

			delete roleAssignment;
		}
		// Compute Resource's role has been revoked. Clean up the label if needed. (Role has already been released).
		else if ((computeResourceRole.hasRoleRevoked() && computeResourceRole.hasRoleLabel()))
		{
			ACS_CS_RoleAssignment* roleAssignment = ACS_CS_RoleAssignment::createRoleAssignment(computeResourceRole.type, computeResourceRole.roleAssignment);

			roleAssignment->setRevokedRole(computeResourceRole);

			delete roleAssignment;
		}
	}

	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_DEBUG, "Leaving updateRoles()."));
}


ACS_CS_RoleAssignment* ACS_CS_RoleTableUpdate::getRoleAssignment(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CS_RoleAssignment* staticRole = 0;
	return ((staticRole = getStaticRoleAssignment(computeResource)))? staticRole: getAvailableRoleAssignment(computeResource.type);
}

ACS_CS_RoleAssignment* ACS_CS_RoleTableUpdate::getStaticRoleAssignment(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CS_RoleAssignment* assignment = 0;
	if (!computeResource.ipAddressEthA.empty())
	{
		int ipLastByte = atoi(ACS_APGCC::afterLast(computeResource.ipAddressEthA,"."));

			int index = ipLastByte - STATIC_ROLES_IP_OFFSET;

			if (index >= 0 && index < STATIC_ROLES_SIZE)
			{
				assignment = ACS_CS_RoleAssignment::createRoleAssignment(computeResource.type, StaticRoles[index]);

				if (!isRoleAvailable(assignment, getOtherComputeResourceOfSameType(computeResource)))
				{
					delete assignment;
					assignment = 0;
				}
			}
	}

	return assignment;
}

ACS_CS_RoleAssignment* ACS_CS_RoleTableUpdate::getAvailableRoleAssignment(RoleType type)
{
	ComputeResourceMap computeResourcePerType = getComputeResourcePerType(type);

	ACS_CS_RoleAssignment* iterator = ACS_CS_RoleAssignment::createRoleAssignment(type);

	do
	{
		++(*iterator);

		if (isRoleAvailable(iterator, computeResourcePerType))
			return iterator;

	}while (iterator->getAssignment() != ROLE_EXCEEDED);

	return iterator;
}



ComputeResourceMap ACS_CS_RoleTableUpdate::getComputeResourcePerType(RoleType roleTypeFilter)
{
	ComputeResourceMap computeResourcePerType;
	for (ComputeResourceMap::iterator it = computeResourceMap->begin(); it != computeResourceMap->end(); ++it)
	{
		ComputeResourcePair computeResourcePair = *it;
		ACS_CS_ComputeResourceRole computeResourceRole = computeResourcePair.second;

		if (roleTypeFilter == computeResourceRole.type)
		{
			computeResourcePerType.insert(computeResourcePair);
		}
	}

	return computeResourcePerType;
}

ComputeResourceMap ACS_CS_RoleTableUpdate::getOtherComputeResourceOfSameType(const ACS_CS_ComputeResourceRole& computeResource)
{
	ComputeResourceMap computeResourcePerType;
	for (ComputeResourceMap::iterator it = computeResourceMap->begin(); it != computeResourceMap->end(); ++it)
	{
		ComputeResourcePair computeResourcePair = *it;
		ACS_CS_ComputeResourceRole otherComputeResourceRole = computeResourcePair.second;

		if (computeResource.type == otherComputeResourceRole.type &&
				computeResource != otherComputeResourceRole)
		{
			computeResourcePerType.insert(computeResourcePair);
		}
	}

	return computeResourcePerType;
}

int ACS_CS_RoleTableUpdate::getMaxRoleId(RoleType type)
{
	//TODO: Not used anymore
	ComputeResourceMap computeResourcePerType = getComputeResourcePerType(type);
	int maxRoleAssigned = ROLE_FREE;

	for (ComputeResourceMap::iterator it = computeResourcePerType.begin(); it != computeResourcePerType.end(); ++it)
	{
		ComputeResourcePair computeResourcePair = *it;
		ACS_CS_ComputeResourceRole computeResourceRole = computeResourcePair.second;

		if (computeResourceRole.roleAssignment > maxRoleAssigned )
		{
			maxRoleAssigned = computeResourceRole.roleAssignment;
		}
	}

	return maxRoleAssigned;
}


bool ACS_CS_RoleTableUpdate::isRoleAvailable(ACS_CS_RoleAssignment* roleAssignment, ComputeResourceMap computeResourcePerType)
{
	for (ComputeResourceMap::iterator it = computeResourcePerType.begin(); it != computeResourcePerType.end(); ++it)
	{
		ComputeResourcePair computeResourcePair = *it;

		ACS_CS_ComputeResourceRole computeResourceRole = computeResourcePair.second;

		if (roleAssignment->getAssignment() == computeResourceRole.roleAssignment &&
				roleAssignment->getAssignmentLabel().compare(computeResourceRole.roleLabel) == 0)
			return false;
	}

	return true;
}

/////////////////////////// ACS_CS_RoleAssignment ///////////////////////////

ACS_CS_RoleAssignment::ACS_CS_RoleAssignment(RoleType inputType): roleType(inputType)
{
    assignment = ROLE_FREE;
    if (roleType >= 0)
        properties = PropertyArray[roleType/ROLE_ARRAY_NORMALIZATION_FACTOR];
}

ACS_CS_RoleAssignment* ACS_CS_RoleAssignment::createRoleAssignment(RoleType type)
{
	ACS_CS_RoleAssignment* roleAssignment = 0;

    switch (type) {
        case BC_TYPE:
            roleAssignment = new ACS_CS_BcRoleAssignment();
            break;
        case CP_TYPE:
            roleAssignment = new ACS_CS_CpRoleAssignment();
            break;
        case AP_TYPE:
            roleAssignment = new ACS_CS_ApRoleAssignment();
            break;
        case IPLB_TYPE:
            roleAssignment = new ACS_CS_IplbRoleAssignment();
            break;
        default:
            roleAssignment = new ACS_CS_NullRoleAssignment();
            break;
    }


    return roleAssignment;
}

ACS_CS_RoleAssignment* ACS_CS_RoleAssignment::createRoleAssignment(RoleType type, int inputAssignment)
{
	ACS_CS_RoleAssignment* roleAssignment = createRoleAssignment(type);

    roleAssignment->setAssignment(inputAssignment);

    return roleAssignment;
}


int ACS_CS_RoleAssignment::increase()
{
	if (assignment == properties.upperThreshold || assignment == ROLE_EXCEEDED)
	{
		assignment = ROLE_EXCEEDED;
	}
	else if (assignment == ROLE_FREE)
	{
		assignment = properties.lowerThreshold;
	}
	else
	{
		nextRole();
	}

	return assignment;
}

ACS_CS_ImBase* ACS_CS_RoleAssignment::getShelf()
{

	ACS_CS_ImShelf *shelf = new ACS_CS_ImShelf();

	std::string shelfAddress = getShelfAddress();
	std::string shelfId = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelfAddress;

	shelf->address = shelfAddress;
	shelf->axeShelfId = shelfId;
	shelf->rdn = shelfId + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	return shelf;
}

ACS_CS_ImBase* ACS_CS_RoleAssignment::getCrmEquipment()
{

	ACS_CS_ImCrmEquipment *crmEquipment = new ACS_CS_ImCrmEquipment();

	crmEquipment->equipmentId = ACS_CS_ImmMapper::RDN_CRM_EQUIPMENT;
	crmEquipment->rdn = ACS_CS_ImmMapper::RDN_CRM_EQUIPMENT;

	return crmEquipment;
}

std::string ACS_CS_RoleAssignment::getShelfAddress()
{
	int firstPlug 	= roleType/1000;
	int secondPlug 	= (roleType > 0)? getMagicNo(): 0;
	int thirdPlug 	= 1;
	int fourthPlug 	= getMagicNo()/24;

	char shelfAddress[32] = {0};
	sprintf(shelfAddress,"%d.%d.%d.%d", firstPlug, secondPlug, thirdPlug, fourthPlug);

	return shelfAddress;
}

std::string ACS_CS_RoleAssignment::getAssignmentLabel()
{
	//return (getTypeLabel() + ACS_APGCC::itoa(getMagicNo()) + getSideLabel());

	return ACS_CS_ImUtils::getRoleLabel(roleType, getSide(), getMagicNo());
}

//std::string ACS_CS_RoleAssignment::getSideLabel()
//{
//	int side = getSide();
//
//	switch (side)
//	{
//	case A:
//		return "A";
//	case B:
//		return "B";
//	default:
//		return "";
//	}
//}

bool ACS_CS_RoleAssignment::assignRole(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	std::string transactionName = "ADD_" + computeResource.roleLabel;

	if (assignment != ROLE_FREE && assignment != ROLE_EXCEEDED && assignment != ROLE_REVOKED)
	{
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Creating Blade object for roleId %d, macA %s, macB %s",
				assignment, computeResource.macAddressEthA.c_str(), computeResource.macAddressEthB.c_str()));

		ACS_CS_ImModel tempModel;


		//Hardware Mgmt Model
		ACS_CS_ImBase* shelfObj = getShelf();
		shelfObj->action = ACS_CS_ImBase::CREATE;
		tempModel.addObject(shelfObj);

		ACS_CS_ImBase* bladeObj = getBlade(computeResource);
		bladeObj->action = ACS_CS_ImBase::CREATE;
		tempModel.addObject(bladeObj);

		//CrMgmt Model
		ACS_CS_ImBase* computeResourceObj = getComputeResource(computeResource);
		computeResourceObj->action = ACS_CS_ImBase::MODIFY;
		tempModel.addObject(computeResourceObj);

		//ECIM Equipment Model
		ACS_CS_ImBase* crmEquipment = getCrmEquipment();
		crmEquipment->action = ACS_CS_ImBase::CREATE;
		tempModel.addObject(crmEquipment);

		ACS_CS_ImBase* crmComputeResourceObj = getCrmComputeResource(computeResource);
		crmComputeResourceObj->action = ACS_CS_ImBase::CREATE;
		tempModel.addObject(crmComputeResourceObj);

		//TransportM Model
		ACS_CS_NetworkHandler::getInstance()->startTransaction(transactionName, &tempModel);

		std::set<ACS_CS_Interface> interfaces;
		for(std::set<ACS_CS_ComputeResourceNetwork>::iterator it = computeResource.networks.begin(); computeResource.networks.end() != it; ++it)
		{
			ACS_CS_Interface interface(computeResource.roleLabel, it->mac, it->name, getNetworkDomain(), it->nicName);
			interfaces.insert(interface);
		}

		ACS_CS_NetworkHandler::getInstance()->addHost(interfaces, transactionName);

		for (uint16_t retry = 1; retry <= IMM_MAX_RETRY && ACS_CC_FAILURE == result; ++retry)
		{
			//Save
			ACS_CS_ImModelSaver saver(&tempModel);
			result = saver.save(transactionName, REGISTERED_OI);

			if (ACS_CC_SUCCESS != result)
			{
				usleep(retry * IMM_RETRY_DELAY_MS * 1000U);
			}
		}

		if (ACS_CC_SUCCESS == result)
		{
			ACS_CS_NetworkHandler::getInstance()->commitTransaction(transactionName);
		}
		else
		{
			ACS_CS_NetworkHandler::getInstance()->abortTransaction(transactionName);
		}
	}

	return ACS_CC_SUCCESS == result;
}

bool ACS_CS_RoleAssignment::releaseRoleAugment(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - base-rdn == %s - Delete Blade object for roleId %d, macA %s, macB %s",
			ccbId, base->rdn.c_str(),
			assignment, computeResource.macAddressEthA.c_str(), computeResource.macAddressEthB.c_str()));
	std::string transactionName = "DEL_" + computeResource.roleLabel;
	ACS_CS_ImBase* bladeObj = getBlade();
	if(!bladeObj) {
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - getBlade() returned NULL", ccbId));
		return false;
	}

	bladeObj->action = ACS_CS_ImBase::DELETE;

	ACS_CS_ImBase* crmComputeResourceObj = getCrmComputeResource(computeResource);
	if(!crmComputeResourceObj) {
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - getCrmComputeResource() returned NULL", ccbId));
		return false;
	}

	crmComputeResourceObj->action = ACS_CS_ImBase::DELETE;

	// XpBlade deletion - augment
	ACS_CC_ReturnType modResult;
	modResult = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, bladeObj->rdn.c_str());

	if (modResult != ACS_CC_SUCCESS) {
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s", ccbId, bladeObj->rdn.c_str()));
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s -> errCode[%d], errText[%s]", ccbId, bladeObj->rdn.c_str(), \
				getInternalLastError(),getInternalLastErrorText()));
		return false;
	}
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - SUCCESS deleteObjectAugmentCcb() == %s", ccbId, bladeObj->rdn.c_str()));

	// XpBlade deletion - ACS_CS_ImRepository
	ACS_CS_ImRepository::instance()->deleteObject(ccbId, bladeObj->rdn.c_str());

	// Trigger Exisitng Augmentation Functionality related to deletion of XpBlade MO
	// This existing functionaltiy augments changes in CpCluster, LogicalMgmt, etc related to XpBlade MO
	// This also handles updating ACS_CS_ImRepository, so just make use of this MethodHandler functionality
	ACS_CS_MethodHandler methodHandler;
	methodHandler.setObject(oiHandle,ccbId,ccbHandleVal,adminOwnwrHandlerVal,"","",0,bladeObj,true);

	if(methodHandler.remove()) {
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED methodHandler.remove() == %s", ccbId, bladeObj->rdn.c_str()));
		return false;
	}

	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - SUCCESS methodHandler.remove() == %s", ccbId, bladeObj->rdn.c_str()));

	// Equipment->ComputeResource deletion - augment
	modResult = deleteObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, crmComputeResourceObj->rdn.c_str());

	if (modResult != ACS_CC_SUCCESS) {
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s", ccbId, crmComputeResourceObj->rdn.c_str()));
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s -> errCode[%d], errText[%s]", ccbId, crmComputeResourceObj->rdn.c_str(), \
				getInternalLastError(),getInternalLastErrorText()));
		return false;
	}
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - SUCCESS deleteObjectAugmentCcb() == %s", ccbId, crmComputeResourceObj->rdn.c_str()));


	// Equipment->ComputeResource deletion - ACS_CS_ImRepository
	ACS_CS_ImRepository::instance()->deleteObject(ccbId, crmComputeResourceObj->rdn.c_str());

	//TransportM
//	ACS_CS_NetworkHandler::getInstance()->startTransaction(transactionName, ccbId, &tempModel);
//	ACS_CS_NetworkHandler::getInstance()->deleteHost(computeResource.roleLabel, transactionName);

	oiCcbAugmentHandlesStruct ccbAugmentHandles;
	ccbAugmentHandles.setAugmentCcbHandles(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	ACS_CS_ImModel tempModel;
	ACS_CS_NetworkHandler::getInstance()->startDeletionTransaction(ccbAugmentHandles, &tempModel);
	if(!ACS_CS_NetworkHandler::getInstance()->handleComputeResourceDeletion(computeResource, ccbId)) {
		ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED handling deletion & update of TransportM during deletion of ComputeResource(%s) == %s", ccbId, crmComputeResourceObj->rdn.c_str()));
		return false;
	}

	return true;
}

bool ACS_CS_RoleAssignment::releaseRole(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CC_ReturnType result = ACS_CC_FAILURE;
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "- SHOULD NOT  BE CALLED -"));
	std::string transactionName = "DEL_" + computeResource.roleLabel;

	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Delete Blade object for roleId %d, macA %s, macB %s",
			assignment, computeResource.macAddressEthA.c_str(), computeResource.macAddressEthB.c_str()));

	ACS_CS_ImModel tempModel;

	ACS_CS_ImBase* bladeObj = getBlade();
	bladeObj->action = ACS_CS_ImBase::DELETE;

	ACS_CS_ImBase* crmComputeResourceObj = getCrmComputeResource(computeResource);
	crmComputeResourceObj->action = ACS_CS_ImBase::DELETE;


	tempModel.addObject(bladeObj);
	tempModel.addObject(crmComputeResourceObj);

	//TransportM Model
	ACS_CS_NetworkHandler::getInstance()->startTransaction(transactionName, &tempModel);

	ACS_CS_NetworkHandler::getInstance()->deleteHost(computeResource.roleLabel, transactionName);

	for (uint16_t retry = 1; retry <= IMM_MAX_RETRY && ACS_CC_FAILURE == result; ++retry)
	{
		//Save
		ACS_CS_ImModelSaver saver(&tempModel);
		result = saver.save(transactionName, REGISTERED_OI);

		if (ACS_CC_SUCCESS != result)
		{
			usleep(retry * IMM_RETRY_DELAY_MS * 1000U);
		}
	}

	if (ACS_CC_SUCCESS == result)
	{
		ACS_CS_NetworkHandler::getInstance()->commitTransaction(transactionName);
	}
	else
	{
		ACS_CS_NetworkHandler::getInstance()->abortTransaction(transactionName);
	}

	return ACS_CC_SUCCESS == result;

}

void ACS_CS_RoleAssignment::setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal)
{
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->attr=attr;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->base=base;

	augmentHandlesAvailable = true;
}

bool ACS_CS_RoleAssignment::setRevokedRole(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	//NOTE: revokeRole is only meant to remove roleId and roleLabel attributes. The role has already been released.
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Revoking Role %s to compute resource %s",
			computeResource.roleLabel.c_str(), computeResource.id.c_str()));

	ACS_CS_ImModel tempModel;

	ACS_CS_ComputeResourceRole computeResourceCopy(computeResource);

	computeResourceCopy.roleAssignment = ROLE_REVOKED;
	computeResourceCopy.roleLabel = "";

	ACS_CS_ImBase* computeResourceBaseObj = getComputeResource(computeResourceCopy);

	computeResourceBaseObj->action = ACS_CS_ImBase::MODIFY;

	tempModel.addObject(computeResourceBaseObj);

	for (uint16_t retry = 1; retry <= IMM_MAX_RETRY && ACS_CC_FAILURE == result; ++retry)
	{
		//Save
		ACS_CS_ImModelSaver saver(&tempModel);
		result = saver.save(computeResourceCopy.id, REGISTERED_OI);

		if (ACS_CC_SUCCESS != result)
		{
			usleep(retry * IMM_RETRY_DELAY_MS * 1000U);
		}
	}

	return ACS_CC_SUCCESS == result;
}

bool ACS_CS_RoleAssignment::setMaintenanceRole(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CS_FTRACE((ACS_CS_RoleAssignment_TRACE, LOG_LEVEL_ERROR, "Putting Compute resource %s in maintenance mode",
			computeResource.id.c_str()));

	ACS_CC_ReturnType result = ACS_CC_FAILURE;
	ACS_CS_ImModel tempModel;

	ACS_CS_ComputeResourceRole computeResourceCopy(computeResource);

	computeResourceCopy.roleAssignment = ROLE_MAINTENANCE;
	computeResourceCopy.roleLabel = "";

	ACS_CS_ImBase* computeResourceBaseObj = getComputeResource(computeResourceCopy);

	computeResourceBaseObj->action = ACS_CS_ImBase::MODIFY;

	tempModel.addObject(computeResourceBaseObj);

	for (uint16_t retry = 1; retry <= IMM_MAX_RETRY && ACS_CC_FAILURE == result; ++retry)
	{
		//Save
		ACS_CS_ImModelSaver saver(&tempModel);
		result = saver.save(computeResourceCopy.id, REGISTERED_OI);

		if (ACS_CC_SUCCESS != result)
		{
			usleep(retry * IMM_RETRY_DELAY_MS * 1000U);
		}
	}

	return ACS_CC_SUCCESS == result;
}

ACS_CS_ImBase* ACS_CS_RoleAssignment::getComputeResource(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CS_ImBase* baseObj = computeResource.getImObject();

	return baseObj;
}

ACS_CS_ImBase* ACS_CS_RoleAssignment::getCrmComputeResource(const ACS_CS_ComputeResourceRole& computeResource)
{
	ACS_CS_ImCrmComputeResource* crmComputeResource = new ACS_CS_ImCrmComputeResource();

	crmComputeResource->computeResourceId = ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID + "=" + computeResource.roleLabel;
	crmComputeResource->rdn = crmComputeResource->computeResourceId + "," +  ACS_CS_ImmMapper::RDN_CRM_EQUIPMENT;
	crmComputeResource->uuid = computeResource.uuid;

	for(std::set<ACS_CS_ComputeResourceNetwork>::iterator it = computeResource.networks.begin(); computeResource.networks.end() != it; ++it)
	{
		crmComputeResource->macAddress.insert(it->mac);
	}


	return crmComputeResource;
}

bool ACS_CS_RoleAssignment::isValid (int roleAssignment)
{
    if(roleAssignment == ROLE_EXCEEDED || roleAssignment == ROLE_REVOKED)
        return true;

    ACS_CS_RoleAssignment* firstRole = createRoleAssignment(this->roleType);

    bool valid = false;

    do
    {
        if (firstRole->getAssignment() == roleAssignment) {
            valid = true;
        }

    } while ((++(*firstRole)).getAssignment() != ROLE_EXCEEDED && !valid);

    delete firstRole;
    return valid;
}
/////////////////////////// ACS_CS_CpRoleAssignment ///////////////////////////

void ACS_CS_CpRoleAssignment::nextRole()
{

	if (0 == (assignment % 2))
	{
		assignment += 9;
	}
	else
	{
		assignment++;
	}

}

ACS_CS_ImBase* ACS_CS_CpRoleAssignment::getBlade(const ACS_CS_ComputeResourceRole computeResourceRole)
{
	std::string shelfAddress = getShelfAddress();
	std::string shelfId = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelfAddress;
	std::string shelfDn = shelfId + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	int slot = getSlot();
	int systemNumber = getMagicNo();
	int side = getSide();
	std::string bladeId = ACS_CS_ImmMapper::ATTR_CP_BLADE_ID + "=" + ACS_APGCC::itoa(slot);

	ACS_CS_ImCpBlade* cpBlade = new ACS_CS_ImCpBlade();

	cpBlade->axeCpBladeId = bladeId;
	cpBlade->rdn = bladeId + "," + shelfDn;
	cpBlade->systemNumber = systemNumber;
	cpBlade->slotNumber = slot;
	cpBlade->side = (SideEnum) side;
	cpBlade->functionalBoardName = CPUB;
	cpBlade->systemType = (SystemTypeEnum) roleType;
	cpBlade->sequenceNumber = -1;
	cpBlade->macAddressEthA = computeResourceRole.macAddressEthA;
	cpBlade->macAddressEthB = computeResourceRole.macAddressEthB;
	cpBlade->ipAddressEthA = computeResourceRole.ipAddressEthA;
	cpBlade->ipAddressEthB = computeResourceRole.ipAddressEthB;
	cpBlade->uuid = computeResourceRole.uuid;

	cpBlade->entryId = 0;
	cpBlade->dhcpOption = UNDEF_DHCP;

	return cpBlade;

}

/////////////////////////// ACS_CS_BcRoleAssignment ///////////////////////////

void ACS_CS_BcRoleAssignment::nextRole()
{
	assignment += 10;
}

ACS_CS_ImBase* ACS_CS_BcRoleAssignment::getBlade(const ACS_CS_ComputeResourceRole computeResourceRole)
{

	std::string shelfAddress = getShelfAddress();
	std::string shelfId = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelfAddress;
	std::string shelfDn = shelfId + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	int slot = getSlot();
	int sequenceNumber = getMagicNo();
	std::string bladeId = ACS_CS_ImmMapper::ATTR_CP_BLADE_ID + "=" + ACS_APGCC::itoa(slot);

	ACS_CS_ImCpBlade* cpBlade = new ACS_CS_ImCpBlade();

	cpBlade->axeCpBladeId = bladeId;
	cpBlade->rdn = bladeId + "," + shelfDn;
	cpBlade->sequenceNumber = sequenceNumber;
	cpBlade->slotNumber = slot;
	cpBlade->side = UNDEF_SIDE;
	cpBlade->functionalBoardName = CPUB;
	cpBlade->systemType = (SystemTypeEnum) roleType;
	cpBlade->systemNumber = 1;
	cpBlade->macAddressEthA = computeResourceRole.macAddressEthA;
	cpBlade->macAddressEthB = computeResourceRole.macAddressEthB;
	cpBlade->ipAddressEthA = computeResourceRole.ipAddressEthA;
	cpBlade->ipAddressEthB = computeResourceRole.ipAddressEthB;
	cpBlade->uuid = computeResourceRole.uuid;

	cpBlade->entryId = 0;
	cpBlade->dhcpOption = UNDEF_DHCP;

	return cpBlade;
}




/////////////////////////// ACS_CS_IplbRoleAssignment ///////////////////////////

void ACS_CS_IplbRoleAssignment::nextRole()
{
	if (0 == (assignment % 2))
	{
		 assignment += 9;
	}
	else
	{
		assignment++;
	}
}

ACS_CS_ImBase* ACS_CS_IplbRoleAssignment::getBlade(const ACS_CS_ComputeResourceRole computeResourceRole)
{
	std::string shelfAddress = getShelfAddress();
	std::string shelfId = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelfAddress;
	std::string shelfDn = shelfId + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	int slot = getSlot();
	int systemNumber = getMagicNo();
	int side = getSide();
	std::string bladeId = ACS_CS_ImmMapper::ATTR_OTHER_BLADE_ID + "=" + ACS_APGCC::itoa(slot);

	ACS_CS_ImOtherBlade* otherBlade = new ACS_CS_ImOtherBlade();

	otherBlade->axeOtherBladeId = bladeId;
	otherBlade->rdn = bladeId + "," + shelfDn;
	otherBlade->systemNumber = systemNumber;
	otherBlade->slotNumber = slot;
	otherBlade->side = (SideEnum) side;
	otherBlade->functionalBoardName = IPLB;
	otherBlade->systemType = (SystemTypeEnum) roleType;
	otherBlade->sequenceNumber = -1;
	otherBlade->macAddressEthA = computeResourceRole.macAddressEthA;
	otherBlade->macAddressEthB = computeResourceRole.macAddressEthB;
	otherBlade->ipAddressEthA = computeResourceRole.ipAddressEthA;
	otherBlade->ipAddressEthB = computeResourceRole.ipAddressEthB;
	otherBlade->uuid = computeResourceRole.uuid;

	otherBlade->entryId = 0;
	otherBlade->dhcpOption = UNDEF_DHCP;

	return otherBlade;

}

/////////////////////////// ACS_CS_ApRoleAssignment ///////////////////////////

void ACS_CS_ApRoleAssignment::nextRole()
{

	if (0 == (assignment % 2))
	{
		assignment += 9;
	}
	else
	{
		assignment++;
	}

}

ACS_CS_ImBase* ACS_CS_ApRoleAssignment::getBlade(const ACS_CS_ComputeResourceRole computeResourceRole)
{
	std::string shelfAddress = getShelfAddress();
	std::string shelfId = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelfAddress;
	std::string shelfDn = shelfId + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	int slot = getSlot();
	int systemNumber = getMagicNo();
	int side = getSide();
	std::string bladeId = ACS_CS_ImmMapper::ATTR_AP_BLADE_ID + "=" + ACS_APGCC::itoa(slot);

	ACS_CS_ImApBlade* apBlade = new ACS_CS_ImApBlade();

	apBlade->axeApBladeId = bladeId;
	apBlade->rdn = bladeId + "," + shelfDn;
	apBlade->systemNumber = systemNumber;
	apBlade->slotNumber = slot;
	apBlade->side = (SideEnum) side;
	apBlade->functionalBoardName = APUB;
	apBlade->systemType = (SystemTypeEnum) roleType;
	apBlade->sequenceNumber = -1;
	apBlade->macAddressEthA = computeResourceRole.macAddressEthA;
	apBlade->macAddressEthB = computeResourceRole.macAddressEthB;
	apBlade->ipAddressEthA = computeResourceRole.ipAddressEthA;
	apBlade->ipAddressEthB = computeResourceRole.ipAddressEthB;
	apBlade->uuid = computeResourceRole.uuid;

	apBlade->entryId = 0;
	apBlade->dhcpOption = UNDEF_DHCP;

	return apBlade;

}

/////////////////////////// ACS_CS_NullRoleAssignment ///////////////////////////

void ACS_CS_NullRoleAssignment::nextRole()
{

}

ACS_CS_ImBase* ACS_CS_NullRoleAssignment::getBlade(const ACS_CS_ComputeResourceRole /*computeResourceRole*/)
{
	return 0; //TODO
}
