/*
 * ACS_CS_TransportHandler.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: estevol
 */

#include "ACS_CS_TransportHandler.h"

#include "../inc/imm_mapper/ACS_CS_ImExternalNetwork.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImInternalNetwork.h"
#include "ACS_CS_ImInterface.h"
#include "ACS_CS_ImComputeResourceNetwork.h"

#include "ACS_CS_Trace.h"

ACS_CS_Trace_TDEF(ACS_CS_TransportHandler_TRACE);

ACS_CS_TransportHandler::ACS_CS_TransportHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
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

ACS_CS_TransportHandler::~ACS_CS_TransportHandler()
{

}


int ACS_CS_TransportHandler::create()
{
	ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Entering - Current base obj == %s", ccbId, base->rdn.c_str()));
	int result = 0;

	if (TRM_INTERFACE_T == base->type)
	{
		augmentCrmNetworkFromInterface();
	}
	return result;
}

int ACS_CS_TransportHandler::remove()
{
	ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Entering - Current base obj == %s", ccbId, base->rdn.c_str()));
	int result = 0;
	return result;
}

int ACS_CS_TransportHandler::modify()
{
	ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Entering - Current base obj == %s", ccbId, base->rdn.c_str()));
	int result = 0;

	if (TRM_INTERNALNETWORK_T == base->type)
	{
		augmentCrmNetwork<ACS_CS_ImInternalNetwork>(base);
	}
	else if (TRM_EXTERNALNETWORK_T == base->type)
	{
		augmentCrmNetwork<ACS_CS_ImExternalNetwork>(base);
	}
	else if (CR_NETWORKSTRUCT_T == base->type)
	{
		augmentTrmNetwork();
	}
	else if (TRM_INTERFACE_T == base->type)
	{
		augmentCrmNetworkFromInterface();
	}

	return result;
}


template<class ImNetwork_t> void ACS_CS_TransportHandler::augmentCrmNetwork(const ACS_CS_ImBase * object)
{
	ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Entering - Current base obj == %s", ccbId, base->rdn.c_str()));
	const ImNetwork_t* imNetwork = dynamic_cast<const ImNetwork_t*>(object);

	if (!imNetwork)
	{
		return;
	}

	std::set<std::string> crmNetworksDn;
	std::set<std::string> interfaceMacs;
	std::set<const ACS_CS_ImBase*> crmNetworks;

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
	if (model)
	{
		model->getObjects(crmNetworks, CR_NETWORKSTRUCT_T);
		for (std::set<std::string>::iterator it = imNetwork->attachedInterfaces.begin(); imNetwork->attachedInterfaces.end() != it; ++it)
		{
			const ACS_CS_ImInterface * interface = dynamic_cast<const ACS_CS_ImInterface *>(model->getObject(it->c_str()));
			if (interface)
			{
				interfaceMacs.insert(interface->mac);
			}
		}

	}

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
	{
		subset->getObjects(crmNetworks, CR_NETWORKSTRUCT_T);
		for (std::set<std::string>::iterator it = imNetwork->attachedInterfaces.begin(); imNetwork->attachedInterfaces.end() != it; ++it)
		{
			const ACS_CS_ImInterface * interface = dynamic_cast<const ACS_CS_ImInterface *>(subset->getObject(it->c_str()));
			if (interface)
			{
				interfaceMacs.insert(interface->mac);
			}
		}
	}

	for (std::set<const ACS_CS_ImBase*>::iterator it = crmNetworks.begin(); crmNetworks.end() != it; ++it)
	{
		const ACS_CS_ImComputeResourceNetwork * crmNetwork = dynamic_cast<const ACS_CS_ImComputeResourceNetwork * >(*it);

		if (crmNetwork && interfaceMacs.find(crmNetwork->macAddress) != interfaceMacs.end())
		{
			crmNetworksDn.insert((*it)->rdn);
		}
	}


	for (std::set<std::string>::iterator it = crmNetworksDn.begin(); crmNetworksDn.end() != it; ++it)
	{
		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createStringType("netName", imNetwork->name);
		ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Augment netName attr modify in crmNetworksDn == %s", ccbId, (*it).c_str()));
		ACS_CC_ImmParameter netNameAttr;

		netNameAttr.attrName = attr.getAttrName();
		netNameAttr.attrType = attr.getAttrType();
		netNameAttr.attrValuesNum = attr.getAttrValuesNum();
		netNameAttr.attrValues = attr.getAttrValues();


		if ( ACS_CC_SUCCESS != modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, it->c_str(), &netNameAttr))
		{
			ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - FAILED Augment netName attr modify in crmNetworksDn == %s", ccbId,(*it).c_str()));
			return;
		}

		ACS_APGCC_AttrModification *attrMods[2];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		attrVal.attrValuesNum = attr.getAttrValuesNum();
		attrVal.attrValues = attr.getAttrValues();

		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType = ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = NULL;

		ACS_CS_ImRepository::instance()->modifyObject(ccbId, it->c_str(), attrMods);

		delete attrMods[0];

	}
}


void ACS_CS_TransportHandler::augmentTrmNetwork()
{
	ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Entering - Current base obj == %s", ccbId, base->rdn.c_str()));
	const ACS_CS_ImComputeResourceNetwork* crmNetwork = dynamic_cast<const ACS_CS_ImComputeResourceNetwork*>(base);

	if (!crmNetwork)
	{
		return;
	}

	std::set<const ACS_CS_ImBase*> networks;
	std::set<const ACS_CS_ImBase*> interfaces;

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
	if (model)
	{
		model->getObjects(networks, TRM_EXTERNALNETWORK_T);
		model->getObjects(networks, TRM_INTERNALNETWORK_T);
		model->getObjects(interfaces, TRM_INTERFACE_T);
	}

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
	{
		subset->getObjects(networks, TRM_EXTERNALNETWORK_T);
		subset->getObjects(networks, TRM_INTERNALNETWORK_T);
		subset->getObjects(interfaces, TRM_INTERFACE_T);
	}


	//Search the corresponding ImInterface with the same MAC address
	std::string interfaceDn;
	for (std::set<const ACS_CS_ImBase*>::iterator it = interfaces.begin(); interfaces.end() != it && interfaceDn.empty(); ++it)
	{
		const ACS_CS_ImInterface * interface = dynamic_cast<const ACS_CS_ImInterface *>(*it);
		if (interface && interface->mac.compare(crmNetwork->macAddress) == 0)
		{
			interfaceDn = interface->rdn;
		}
	}

	if (interfaceDn.empty())
	{
		return;
	}

	//Search for the network to whom the interface is attached
	std::string networkDn;
	for (std::set<const ACS_CS_ImBase*>::iterator it = networks.begin(); networks.end() != it && networkDn.empty(); ++it)
	{
		if ((*it)->type == TRM_EXTERNALNETWORK_T)
		{
			const ACS_CS_ImExternalNetwork * network = dynamic_cast<const ACS_CS_ImExternalNetwork * >(*it);

			if (network && network->attachedInterfaces.find(interfaceDn) != network->attachedInterfaces.end())
			{
				networkDn = network->rdn;
			}
		}
		else if ((*it)->type == TRM_INTERNALNETWORK_T)
		{
			const ACS_CS_ImInternalNetwork * network = dynamic_cast<const ACS_CS_ImInternalNetwork * >(*it);

			if (network && network->attachedInterfaces.find(interfaceDn) != network->attachedInterfaces.end())
			{
				networkDn = network->rdn;
			}
		}
	}
	if (!networkDn.empty())
	{
		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createStringType("name", crmNetwork->netName);
		ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Entering - Augment name attr modify in networkDn == %s", ccbId, networkDn.c_str()));
		ACS_CC_ImmParameter nameAttr;

		nameAttr.attrName = attr.getAttrName();
		nameAttr.attrType = attr.getAttrType();
		nameAttr.attrValuesNum = attr.getAttrValuesNum();
		nameAttr.attrValues = attr.getAttrValues();


		if ( ACS_CC_SUCCESS != modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, networkDn.c_str(), &nameAttr))
		{
			ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - FAILED Augment name attr modify in networkDn == %s", ccbId, networkDn.c_str()));
			return;
		}

		ACS_APGCC_AttrModification *attrMods[2];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		attrVal.attrValuesNum = attr.getAttrValuesNum();
		attrVal.attrValues = attr.getAttrValues();

		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType = ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = NULL;

		ACS_CS_ImRepository::instance()->modifyObject(ccbId, networkDn.c_str(), attrMods);

		delete attrMods[0];

		if (subset)
		{
			const ACS_CS_ImBase* baseNet = subset->getObject(networkDn.c_str());

			if (baseNet->type == TRM_EXTERNALNETWORK_T)
			{
				augmentCrmNetwork<ACS_CS_ImExternalNetwork>(baseNet);
			}
			else if (baseNet->type == TRM_INTERNALNETWORK_T)
			{
				augmentCrmNetwork<ACS_CS_ImInternalNetwork>(baseNet);
			}
		}

	}
}

void ACS_CS_TransportHandler::augmentCrmNetworkFromInterface()
{
	ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Entering - Current base obj == %s", ccbId, base->rdn.c_str()));

	const ACS_CS_ImInterface * imInterface =  dynamic_cast<const ACS_CS_ImInterface*>(base);

	if (!imInterface)
	{
		return;
	}

	std::set<std::string> crmNetworksDn;
	std::set<const ACS_CS_ImBase*> crmNetworks;

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModel();
	if (model)
	{
		model->getObjects(crmNetworks, CR_NETWORKSTRUCT_T);
	}

	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbId);
	if (subset)
	{
		subset->getObjects(crmNetworks, CR_NETWORKSTRUCT_T);
	}

	for (std::set<const ACS_CS_ImBase*>::iterator it = crmNetworks.begin(); crmNetworks.end() != it; ++it)
	{
		const ACS_CS_ImComputeResourceNetwork * crmNetwork = dynamic_cast<const ACS_CS_ImComputeResourceNetwork * >(*it);

		if (crmNetwork && crmNetwork->macAddress.compare(imInterface->mac) == 0)
		{
			crmNetworksDn.insert((*it)->rdn);
		}
	}


	for (std::set<std::string>::iterator it = crmNetworksDn.begin(); crmNetworksDn.end() != it; ++it)
	{
		ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - Augment nicName attr modify in crmNetworksDn == %s", ccbId, (*it).c_str()));
		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createStringType("nicName", ACS_CS_ImUtils::getIdValueFromRdn(imInterface->interfaceId));

		ACS_CC_ImmParameter nicNameAttr;

		nicNameAttr.attrName = attr.getAttrName();
		nicNameAttr.attrType = attr.getAttrType();
		nicNameAttr.attrValuesNum = attr.getAttrValuesNum();
		nicNameAttr.attrValues = attr.getAttrValues();


		if ( ACS_CC_SUCCESS != modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, it->c_str(), &nicNameAttr))
		{
			ACS_CS_FTRACE((ACS_CS_TransportHandler_TRACE, LOG_LEVEL_DEBUG, "CcbId[%d] - FAILED Augment nicName attr modify in crmNetworksDn == %s", ccbId, (*it).c_str()));
			return;
		}

		ACS_APGCC_AttrModification *attrMods[2];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		attrVal.attrValuesNum = attr.getAttrValuesNum();
		attrVal.attrValues = attr.getAttrValues();

		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType = ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = NULL;

		ACS_CS_ImRepository::instance()->modifyObject(ccbId, it->c_str(), attrMods);

		delete attrMods[0];

	}
}
