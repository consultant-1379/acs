/*
 * ACS_CS_NetworkHandler.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: estevol
 */

#include "ACS_CS_NetworkHandler.h"
#include "ACS_CS_NetworkList.h"
#include "ACS_CS_NetworkDefinitions.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTransport.h"
#include "ACS_CS_ImHost.h"
#include "ACS_CS_ImInternalNetwork.h"
#include "ACS_CS_ImInterface.h"


#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"

#include <boost/thread/lock_guard.hpp>
#include <boost/type_traits/conditional.hpp>
#include <sstream>
#include "ACS_CS_ImExternalNetwork.h"

ACS_CS_Trace_TDEF(ACS_CS_NetworkHandler_TRACE);

ACS_CS_NetworkHandler* ACS_CS_NetworkHandler::m_instance = 0;

oiCcbAugmentHandlesStruct::oiCcbAugmentHandlesStruct()
{
	this->oiHandle=0;
	this->ccbId=0;
	this->attr=0;
	//this->ccbHandleVal=0;
	//this->adminOwnwrHandlerVal=0;
	this->base=0;
}
void oiCcbAugmentHandlesStruct::setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
		ACS_APGCC_CcbHandle &ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->attr=attr;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->base=base;
}

void ACS_CS_NetworkTransaction::storeAllNetworks(Network::Type type)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	for (ACS_CS_NetworkList::iterator it = networks[type].begin(); networks[type].end() != it; ++it)
	{
		storeNetwork(it->first, type);
	}
}

void ACS_CS_NetworkTransaction::storeNetworkAndHost(int index, Network::Type type)
{

	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	if (Network::INTERNAL_NETWORK == type)
	{
		saveNetwork<ACS_CS_ImInternalNetwork>(index, type, true);
	}
	else
	{
		saveNetwork<ACS_CS_ImExternalNetwork>(index, type, true);
	}
}


void ACS_CS_NetworkTransaction::storeNetwork(int index, Network::Type type)
{

	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	if (Network::INTERNAL_NETWORK == type)
	{
		saveNetwork<ACS_CS_ImInternalNetwork>(index, type);
	}
	else
	{
		saveNetwork<ACS_CS_ImExternalNetwork>(index, type);
	}
}

template<class ImNetwork_t>  bool ACS_CS_NetworkTransaction::detachHostInterfaceFromNetwork(const std::string hostname, int index, Network::Type type)
{
	bool result = true;
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_NetworkTransaction::detachHostInterfaceFromNetwork() - Entering"));
	ImNetwork_t * imNetwork;
	std::string networkRdn;
	std::stringstream networkIndex;
	networkIndex << index;
	if (Network::EXTERNAL_NETWORK == type)
		networkRdn = ACS_CS_ImmMapper::ATTR_TRM_EXTERNALNETWORK_ID + "=" +  networkIndex.str();
	else
		networkRdn = ACS_CS_ImmMapper::ATTR_TRM_INTERNALNETWORK_ID + "=" + networkIndex.str();

	networkRdn += "," +  ACS_CS_ImmMapper::RDN_TRM_TRANSPORT;

	//TODO: JAY: should be fetched from local Repository
	const ACS_CS_ImModel* subset = ACS_CS_ImRepository::instance()->getSubset(ccbAugmentHandles.ccbId); //ACS_CS_ImRepository::instance()->getModelCopy();
	ACS_CS_ImBase* networkObj = (subset != NULL)? subset->getObject(networkRdn.c_str()) : 0;

	if(!networkObj) {
		const ACS_CS_ImModel* currentModel = ACS_CS_ImRepository::instance()->getModel();
		networkObj = (currentModel != NULL)? currentModel->getObject(networkRdn.c_str()) : 0;
	}

	if(networkObj) {
		imNetwork = dynamic_cast<ImNetwork_t*>(networkObj);
		//ACS_CS_ImInternalNetwork* tempNet = dynamic_cast<ACS_CS_ImInternalNetwork*>(networkObj);

		std::set<string> attachedInterfaces = imNetwork->attachedInterfaces;
		std::set<string>::iterator interfacesIterator = attachedInterfaces.begin();
		for(; interfacesIterator!=attachedInterfaces.end();interfacesIterator++) {
			std::string interfaceDn = *interfacesIterator;
			if(interfaceDn.find(hostname) != std::string::npos) {
				break;
			}
		}
		attachedInterfaces.erase(interfacesIterator);

		ACS_CS_ImValuesDefinitionType attr = ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_TRM_ATTACHEDINTERFACES.c_str(), attachedInterfaces);

		ACS_CC_ImmParameter attachedInterfacesParam; //systemIdentifier

		attachedInterfacesParam.attrName = attr.getAttrName();
		attachedInterfacesParam.attrType = attr.getAttrType();
		attachedInterfacesParam.attrValuesNum = attr.getAttrValuesNum();
		attachedInterfacesParam.attrValues = attr.getAttrValues();

		if ( ACS_CC_SUCCESS != modifyObjectAugmentCcb(ccbAugmentHandles.ccbHandleVal, ccbAugmentHandles.adminOwnwrHandlerVal, networkRdn.c_str(), &attachedInterfacesParam))
		{
			ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED Augment attachedInterfaces attr modify in networkDn == %s", ccbAugmentHandles.ccbId, networkRdn.c_str()));
			return false;
		}
		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - SUCCESS Augment attachedInterfaces attr modify in networkDn == %s", ccbAugmentHandles.ccbId, networkRdn.c_str()));
		ACS_APGCC_AttrModification *attrMods[2];
		ACS_APGCC_AttrValues attrVal;
		attrVal.attrName = attr.getAttrName();
		attrVal.attrType = attr.getAttrType();
		attrVal.attrValuesNum = attr.getAttrValuesNum();
		attrVal.attrValues = attr.getAttrValues();

		attrMods[0] = new ACS_APGCC_AttrModification;
		attrMods[0]->modType=ACS_APGCC_ATTR_VALUES_REPLACE;
		attrMods[0]->modAttr = attrVal;

		attrMods[1] = NULL;
		ACS_CS_ImRepository::instance()->modifyObject(ccbAugmentHandles.ccbId, networkRdn.c_str(), attrMods);
	}
	else {
		result = false;
		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "Unable to find obj [%s] in model", networkRdn.c_str()));
	}
	//	ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createNameType(const char* attrName, const std::set<string> &strings)
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "ACS_CS_NetworkTransaction::detachHostInterfaceFromNetwork() - Exiting"));
	return true;
}

template<class ImNetwork_t>  void ACS_CS_NetworkTransaction::saveNetwork(int index, Network::Type type, bool createHost)
{

	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	if (NULL == model->getObject(ACS_CS_ImmMapper::RDN_TRM_TRANSPORT.c_str()))
	{
		ACS_CS_ImTransport* transportM = new ACS_CS_ImTransport();

		transportM->transportMId = ACS_CS_ImmMapper::RDN_TRM_TRANSPORT;
		transportM->rdn = transportM->transportMId;
		transportM->action = ACS_CS_ImBase::CREATE;

		model->addObject(transportM);
	}

	ImNetwork_t * imNetwork = new ImNetwork_t();

	std::stringstream networkIndex;
	networkIndex << index;
	if (Network::EXTERNAL_NETWORK == type)
	{
		imNetwork->networkId = ACS_CS_ImmMapper::ATTR_TRM_EXTERNALNETWORK_ID + "=" +  networkIndex.str();
	}
	else
	{
		imNetwork->networkId = ACS_CS_ImmMapper::ATTR_TRM_INTERNALNETWORK_ID + "=" + networkIndex.str();
	}

	imNetwork->rdn = imNetwork->networkId + "," +  ACS_CS_ImmMapper::RDN_TRM_TRANSPORT;
	imNetwork->name = networks[type].getName(index);
	imNetwork->description = getNetworkDescription(imNetwork->name);

	for (uint32_t j = 0; j < Network::DOMAIN_NR; ++j)
	{
		std::string nicName = networks[type].getNicName(index, static_cast<Network::Domain>(j));
		if (!nicName.empty())
		{
			std::stringstream nicNameEntry;
			nicNameEntry << j << "_" << nicName;
			imNetwork->nicName.insert(nicNameEntry.str());
		}
	}
	if (isRestrictedName(imNetwork->name))
	{
		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "is restricted network name %s",imNetwork->name.c_str()));
		imNetwork->restrictedName = imNetwork->name;
	}

	imNetwork->action = ACS_CS_ImBase::CREATE;

	std::set<ACS_CS_Interface> interfaces = networks[type].getInterfaces(index);

	for (std::set<ACS_CS_Interface>::iterator it = interfaces.begin(); interfaces.end() != it; ++it)
	{
		std::string hostId = ACS_CS_ImmMapper::ATTR_TRM_HOST_ID + "=" + it->getHostName();
		std::string hostDn = hostId + "," +  ACS_CS_ImmMapper::RDN_TRM_TRANSPORT;

		if (createHost && NULL == model->getObject(hostDn.c_str()))
		{
			ACS_CS_ImHost* host = new ACS_CS_ImHost();
			host->hostId = hostId;
			host->rdn = hostDn;
			host->computeResourceDn = ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID + "=" + it->getHostName() + "," +  ACS_CS_ImmMapper::RDN_CRM_EQUIPMENT;
			host->action = ACS_CS_ImBase::CREATE;

			model->addObject(host);
		}

		ACS_CS_ImInterface* imInterface = new ACS_CS_ImInterface();

		imInterface->interfaceId = ACS_CS_ImmMapper::ATTR_TRM_INTERFACE_ID + "=" + networks[type].getNicName(index, it->getDomain());


		imInterface->rdn = imInterface->interfaceId + "," + hostDn;
		imInterface->mac = it->getMac();
		imInterface->domain = it->getDomain();
		imInterface->action = ACS_CS_ImBase::CREATE;

		imNetwork->attachedInterfaces.insert(imInterface->rdn);
		model->addObject(imInterface);
	}

	model->addObject(imNetwork);

}

void ACS_CS_NetworkTransaction::deleteHost(const std::string & hostName)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR,__PRETTY_FUNCTION__));

	ACS_CS_ImHost* host = new ACS_CS_ImHost();
	host->hostId = ACS_CS_ImmMapper::ATTR_TRM_HOST_ID + "=" + hostName;
	host->rdn = host->hostId + "," +  ACS_CS_ImmMapper::RDN_TRM_TRANSPORT;

	host->action = ACS_CS_ImBase::DELETE;

	model->addObject(host);

}

std::string ACS_CS_NetworkTransaction::getNetworkDescription(const std::string& name)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	std::string description;

	Network::NetDescriptionMap_t::const_iterator it = Network::STATIC_NET_DESCRIPTIONS_MAP.find(name);
	if( it != Network::STATIC_NET_DESCRIPTIONS_MAP.end())
	{
		description = it->second;
	}

	return description;
}

bool ACS_CS_NetworkTransaction::isRestrictedName(const std::string& name)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	if( Network::STATIC_NET_DESCRIPTIONS_MAP.find(name) != Network::STATIC_NET_DESCRIPTIONS_MAP.end())
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

ACS_CS_NetworkHandler::ACS_CS_NetworkHandler()
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	memset(m_nicNameCounter, 0, sizeof(m_nicNameCounter));

//	for (uint32_t j = 0; j < Network::DOMAIN_NR; ++j)
//	{
//		ACS_CS_DEBUG(("Domain %d, counter %d", j, m_nicNameCounter[j]));
//	}

	rebuildNetworkList<ACS_CS_ImInternalNetwork>(Network::INTERNAL_NETWORK);
	rebuildNetworkList<ACS_CS_ImExternalNetwork>(Network::EXTERNAL_NETWORK);
}

ACS_CS_NetworkHandler* ACS_CS_NetworkHandler::getInstance ()
{
	ACS_CS_DEBUG((__PRETTY_FUNCTION__));

	if (m_instance == 0)
	{
		m_instance = new ACS_CS_NetworkHandler();
	}

	return m_instance;
}

void ACS_CS_NetworkHandler::finalize()
{
	ACS_CS_DEBUG((__PRETTY_FUNCTION__));

	if (m_instance)
	{
		delete m_instance;
		m_instance = 0;
	}
}
void ACS_CS_NetworkHandler::startDeletionTransaction(oiCcbAugmentHandlesStruct ccbAugmentHandles, ACS_CS_ImModel* model)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR,__PRETTY_FUNCTION__));

	ACS_CS_NetworkTransaction transaction;

	transaction.model = model;

	for (uint32_t i = 0; i < Network::TYPE_NR; ++i)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_networkMutex[i]);
		transaction.networks[i] = m_networks[i];
	}
	transaction.ccbAugmentHandles = ccbAugmentHandles;
	transaction.augmentHandlesAvailable = true;
	m_deletion_transactions.insert(std::pair<ACS_APGCC_CcbId, ACS_CS_NetworkTransaction>(ccbAugmentHandles.ccbId, transaction));
}

void ACS_CS_NetworkHandler::startTransaction(std::string id, ACS_CS_ImModel* model)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	ACS_CS_NetworkTransaction transaction;

	transaction.model = model;

	for (uint32_t i = 0; i < Network::TYPE_NR; ++i)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_networkMutex[i]);
		transaction.networks[i] = m_networks[i];
	}
	m_transactions.insert(std::pair<std::string, ACS_CS_NetworkTransaction>(id, transaction));
}

void ACS_CS_NetworkHandler::commitDeletionTransaction(ACS_APGCC_CcbId ccbId)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR,__PRETTY_FUNCTION__));

	std::map<ACS_APGCC_CcbId, ACS_CS_NetworkTransaction>::iterator it = m_deletion_transactions.find(ccbId);

	if (m_deletion_transactions.end() != it)
	{
		for (uint32_t i = 0; i < Network::TYPE_NR; ++i)
		{
			boost::lock_guard<boost::recursive_mutex> guard(m_networkMutex[i]);
			m_networks[i] = it->second.networks[i];
		}
		m_deletion_transactions.erase(it);
	}
}

void ACS_CS_NetworkHandler::commitTransaction(std::string id)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	std::map<std::string, ACS_CS_NetworkTransaction>::iterator it = m_transactions.find(id);

	if (m_transactions.end() != it)
	{
		for (uint32_t i = 0; i < Network::TYPE_NR; ++i)
		{
			boost::lock_guard<boost::recursive_mutex> guard(m_networkMutex[i]);
			m_networks[i] = it->second.networks[i];
		}

		m_transactions.erase(it);
	}
}

void ACS_CS_NetworkHandler::abortDeletionTransaction(ACS_APGCC_CcbId ccbId)
{
	ACS_CS_TRACE((ACS_CS_NetworkHandler_TRACE, "abortDeletionTransaction(id:%d)\n", ccbId));
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "abortDeletionTransaction(ccbId = %d)", ccbId));

	std::map<ACS_APGCC_CcbId, ACS_CS_NetworkTransaction>::iterator it = m_deletion_transactions.find(ccbId);

	if (m_deletion_transactions.end() != it)
	{
		m_deletion_transactions.erase(it);
	}
}

void ACS_CS_NetworkHandler::abortTransaction(std::string id)
{
	ACS_CS_TRACE((ACS_CS_NetworkHandler_TRACE, "abortTransaction(id:%s)\n", id.c_str()));
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "abortTransaction(id:%s)\n", id.c_str()));

	std::map<std::string, ACS_CS_NetworkTransaction>::iterator it = m_transactions.find(id);

	if (m_transactions.end() != it)
	{
		m_transactions.erase(it);
	}
}

bool ACS_CS_NetworkHandler::addHost(const std::set<ACS_CS_Interface> & interfaces, const std::string& transactionId)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	bool result = true;
	for (std::set<ACS_CS_Interface>::iterator it = interfaces.begin(); it != interfaces.end() && result; ++it)
	{
		result = addNetworkAndInterface(it->getNetworkName(), *it, transactionId);
	}

	return result;
}

bool ACS_CS_NetworkHandler::handleComputeResourceDeletion(const ACS_CS_ComputeResourceRole& computeResource, ACS_APGCC_CcbId ccbId)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR,__PRETTY_FUNCTION__));
	std::string hostName = computeResource.roleLabel;
	std::map<ACS_APGCC_CcbId, ACS_CS_NetworkTransaction>::iterator it = m_deletion_transactions.find(ccbId);

	if (m_deletion_transactions.end() == it)
	{
		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED m_deletion_transactions has no element with ccbId[%d]", ccbId, ccbId));
		return false;
	}

	ACS_CS_NetworkTransaction & transaction = it->second;

	if(transaction.augmentHandlesAvailable) {
		ACS_CC_ReturnType modResult;

		std::set<int> modifiedNetworkIndexSet;
		transaction.networks[Network::INTERNAL_NETWORK].removeInterfacesByHostName(hostName,modifiedNetworkIndexSet);

		std::set<int>::iterator modifiedNetIterator = modifiedNetworkIndexSet.begin();
		for(;modifiedNetIterator != modifiedNetworkIndexSet.end(); modifiedNetIterator++){
			/*
			 * get CS network obj
			 * build attachedInterfaces attr obj
			 * call modifyObjectaugment
			 */
			ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] -  Calling transaction.modifyNetwork() INTERNAL == %d,%d", transaction.ccbAugmentHandles.ccbId,*modifiedNetIterator,Network::INTERNAL_NETWORK));
			if(!transaction.detachHostInterfaceFromNetwork<ACS_CS_ImInternalNetwork>(hostName, *modifiedNetIterator,Network::INTERNAL_NETWORK)) {
				ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] -  FAILED transaction.modifyNetwork() INTERNAL == %d,%d", transaction.ccbAugmentHandles.ccbId,*modifiedNetIterator,Network::INTERNAL_NETWORK));
				return false;
			}

		}
		modifiedNetworkIndexSet.clear();


		transaction.networks[Network::EXTERNAL_NETWORK].removeInterfacesByHostName(hostName,modifiedNetworkIndexSet);

		modifiedNetIterator = modifiedNetworkIndexSet.begin();
		for(;modifiedNetIterator != modifiedNetworkIndexSet.end(); modifiedNetIterator++){
			/*
			 * get CS network obj
			 * build attachedInterfaces attr obj
			 * call modifyObjectaugment
			 */
			ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] -  Called transaction.modifyNetwork() EXTERNAL == %d,%d", transaction.ccbAugmentHandles.ccbId, *modifiedNetIterator,Network::EXTERNAL_NETWORK));
			if(!transaction.detachHostInterfaceFromNetwork<ACS_CS_ImExternalNetwork>(hostName, *modifiedNetIterator,Network::EXTERNAL_NETWORK)){
				ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] -  FAILED transaction.modifyNetwork() EXTERNAL == %d,%d", transaction.ccbAugmentHandles.ccbId,*modifiedNetIterator,Network::EXTERNAL_NETWORK));
				return false;
			}
		}
		modifiedNetworkIndexSet.clear();


		//Augment delete Host=<role>
		std::string host_rdn = ACS_CS_ImmMapper::ATTR_TRM_HOST_ID + "=" + hostName + "," +  ACS_CS_ImmMapper::RDN_TRM_TRANSPORT;

		ACS_CS_ImModel* currentModel = ACS_CS_ImRepository::instance()->getModel();
		ACS_CS_ImBase* hostImBase;
		if(currentModel)
			hostImBase = currentModel->getObject(host_rdn.c_str());

		if(hostImBase) {
			std::set<const ACS_CS_ImBase *> interfaceChildObjs;
			currentModel->getChildren(hostImBase,interfaceChildObjs);

			std::set<const ACS_CS_ImBase *>::iterator interfaceIterator = interfaceChildObjs.begin();
			for(; interfaceIterator != interfaceChildObjs.end(); interfaceIterator++) {
				const ACS_CS_ImBase* ifObj = dynamic_cast<const ACS_CS_ImBase*>(*interfaceIterator);
				std::string inferfaceRdn = ifObj->rdn;
				modResult = deleteObjectAugmentCcb(transaction.ccbAugmentHandles.ccbHandleVal, transaction.ccbAugmentHandles.adminOwnwrHandlerVal, inferfaceRdn.c_str());

				if (modResult != ACS_CC_SUCCESS) {
					ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s", transaction.ccbAugmentHandles.ccbId, inferfaceRdn.c_str()));
					ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s -> errCode[%d], errText[%s]", transaction.ccbAugmentHandles.ccbId, inferfaceRdn.c_str(), \
							getInternalLastError(),getInternalLastErrorText()));
					return false;
				}
				ACS_CS_ImRepository::instance()->deleteObject(transaction.ccbAugmentHandles.ccbId, inferfaceRdn.c_str());
			}
		}

		modResult = deleteObjectAugmentCcb(transaction.ccbAugmentHandles.ccbHandleVal, transaction.ccbAugmentHandles.adminOwnwrHandlerVal, host_rdn.c_str());

		if (modResult != ACS_CC_SUCCESS) {
			ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s", transaction.ccbAugmentHandles.ccbId, host_rdn.c_str()));
			ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - FAILED deleteObjectAugmentCcb() == %s -> errCode[%d], errText[%s]", transaction.ccbAugmentHandles.ccbId, host_rdn.c_str(), \
					getInternalLastError(),getInternalLastErrorText()));
			return false;
		}
		ACS_CS_ImRepository::instance()->deleteObject(transaction.ccbAugmentHandles.ccbId, host_rdn.c_str());
		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "CcbId[%d] - SUCCESS deleteObjectAugmentCcb() == %s", transaction.ccbAugmentHandles.ccbId, host_rdn.c_str()));
	}
	else {
		return false;
	}
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_ERROR, "Exiting"));
	return true;
}

bool ACS_CS_NetworkHandler::deleteHost(const std::string & hostName, const std::string& transactionId)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	bool result = true;

	std::map<std::string, ACS_CS_NetworkTransaction>::iterator it = m_transactions.find(transactionId);

	if (m_transactions.end() == it)
	{
		return false;
	}

	ACS_CS_NetworkTransaction & transaction = it->second;

	for (uint32_t i = 0; i < Network::TYPE_NR; ++i)
	{
		transaction.networks[i].removeInterfacesByHostName(hostName);

		transaction.storeAllNetworks(static_cast<Network::Type>(i));
		transaction.deleteHost(hostName);
	}

	return result;
}

bool ACS_CS_NetworkHandler::addNetwork(const std::string & name, Network::Domain domain, const std::string& transactionId)
{

	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	std::map<std::string, ACS_CS_NetworkTransaction>::iterator it = m_transactions.find(transactionId);

	if (m_transactions.end() == it)
	{
		return false;
	}

	ACS_CS_NetworkTransaction & transaction = it->second;

	Network::Type type = getNetworkType(name);

	int index = transaction.networks[type].findIndexByName(name);

	if (index < 0)
	{
		//Network doesn't exist
		std::string nicNames[Network::DOMAIN_NR];
		nicNames[domain] = generateNicName(name, domain);
		index = transaction.networks[type].add(name, nicNames);
	}

	transaction.storeNetworkAndHost(index, type);

	return true;
}

bool ACS_CS_NetworkHandler::addNetworkAndInterface(const std::string & name, const ACS_CS_Interface & interface, const std::string& transactionId)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO,
            "addNetworkAndInterface(name:%s host:%s, mac:%s, net:%s, nic:%s, domain:%d)\n",
            name.c_str(), interface.getHostName().c_str(),
            interface.getMac().c_str(), interface.getNetworkName().c_str(),
            interface.getNicName().c_str(), interface.getDomain()));

	std::map<std::string, ACS_CS_NetworkTransaction>::iterator it = m_transactions.find(transactionId);

	if (m_transactions.end() == it)
	{
		return false;
	}

	ACS_CS_NetworkTransaction & transaction = it->second;

	Network::Type type = getNetworkType(name);

	ACS_CS_NetworkList & transactionNetworks = transaction.networks[type];

	ACS_CS_Interface interfaceCopy(interface);

	int index = Network::NO_INDEX;

	std::string nicName(interface.getNicName());
	Network::Domain domain(interface.getDomain());

	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG, "NicName received at deployment = %s",nicName.c_str()));
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO,
		"NetworkType:%d, domain:%d, NicName received at deployment = %s\n", type, domain, nicName.c_str()));

	std::string nicNames[Network::DOMAIN_NR];

	//Search by NetName first. If not found search by NIC name
	if ( (index = transactionNetworks.findIndexByName(name)) == Network::NO_INDEX )
	{
	    ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO,
			            "%s network not find \n",name.c_str()));

		index = transactionNetworks.findIndexByNicName(nicName, domain);
		if (index != Network::NO_INDEX)
		{

    	    ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO,
						            "%s nic name present  \n", nicName.c_str()));


			if (transactionNetworks.getName(index) != name)
			{
				ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO,
					"existing netName:%s has nicName:%s but doesnot match given netName:%s, must create new\n",
					transactionNetworks.getName(index).get().c_str(), name.c_str()));
				index = Network::NO_INDEX;
			}
		}
	}

	std::copy(transactionNetworks.getNicNames(index), transactionNetworks.getNicNames(index) + Network::DOMAIN_NR, nicNames);


	//There's no valid nicName yet for this network and domain: generate a new one or take the one received from the transaction (if available)
	if (nicNames[domain].empty())
	{

		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO,
							"There's no valid nicName yet for this network and domain \n"));

		nicNames[domain] = (nicName.empty())? generateNicName(name, domain): nicName;

		Network::NetNicMap_t::const_iterator it = Network::STATIC_NIC_NAMES_MAP.find(name);
		if (Network::STATIC_NIC_NAMES_MAP.end() != it)
		{
			  ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO,
											"nicName %s match static value \n",it->second.c_str()));

				if (it->second != nicNames[domain])
				{

					ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO, "correcting wrong NIC name used:%s should be:%s, net:%s\n",
						nicNames[domain].c_str(), it->second.c_str(), name.c_str()));
					nicNames[domain] = it->second.c_str();
				}
		}

		interfaceCopy.setNicName(nicNames[domain]);
	}

	std::set<ACS_CS_Interface> interfaces(&interfaceCopy, &interfaceCopy +1 );

	index = transactionNetworks.add(index, name, nicNames, interfaces);

	transaction.storeNetworkAndHost(index, type);

	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_INFO, "addNetworkAndInterface()-END, net:%s type:%d index=%d nic:%s\n",
			name.c_str(), type, index, nicNames[domain].c_str()));

	return true;
}

bool ACS_CS_NetworkHandler::changeNetworkName(int index, Network::Type type, const std::string& name)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	boost::lock_guard<boost::recursive_mutex> guard(m_networkMutex[type]);
	return m_networks[type].changeName(index, name);
}


Network::Type ACS_CS_NetworkHandler::getNetworkType(const std::string& name)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	Network::Type networkType = Network::EXTERNAL_NETWORK;
	if (Network::INTERNAL_NETWORK_NAMES_SET.find(name) != Network::INTERNAL_NETWORK_NAMES_SET.end())
	{
		networkType = Network::INTERNAL_NETWORK;
	}

	// for data integrity, use existing value already in system MO data
	for (int i = Network::INTERNAL_NETWORK; i < Network::TYPE_NR; i++)
	{
		for (ACS_CS_NetworkList::iterator it = m_networks[i].begin(); it != m_networks[i].end(); it++)
		{
			if (it->second.getName().compare(name) == 0)
			{
				if (networkType != i)
				{
					ACS_CS_TRACE((ACS_CS_NetworkHandler_TRACE, "%s network type mis-matched in-used:%d, predefined:%d\n",
						it->second.getName().c_str(), i, networkType));
				}
				return Network::Type(i);
			}
		}
	}

	return networkType;
}

std::string ACS_CS_NetworkHandler::generateNicName(const std::string& name, Network::Domain domain)
{

	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	Network::NetNicMap_t::const_iterator it = Network::STATIC_NIC_NAMES_MAP.find(name);

	if (Network::STATIC_NIC_NAMES_MAP.end() != it)
	{
		return it->second;
	}

	std::string out;
	bool alreadyUsed = false;
	do
	{

		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG, "Domain %d, Counter before increment %d", domain, m_nicNameCounter[domain]));

		std::stringstream nicName;
		nicName << Network::VNIC_PREFIX << ++(m_nicNameCounter[domain]);

		out = nicName.str();

		ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG, "Domain %d, Counter %d VNIC name %s", domain, m_nicNameCounter[domain], out.c_str()));

		alreadyUsed = false;
		for (int type = 0; type < Network::TYPE_NR && !alreadyUsed; ++type)
		{
			alreadyUsed = m_networks[type].hasNicName(out, domain);
		}

	} while (alreadyUsed);

	return out;
}

template<class ImNetwork_t> void ACS_CS_NetworkHandler::rebuildNetworkList(Network::Type type)
{
	ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG,__PRETTY_FUNCTION__));

	ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();

	std::set<const ACS_CS_ImBase*> imNetworks[Network::TYPE_NR];

	if (model)
	{
		model->getObjects(imNetworks[type], (type == Network::INTERNAL_NETWORK)? TRM_INTERNALNETWORK_T: TRM_EXTERNALNETWORK_T);

		for (std::set<const ACS_CS_ImBase*>::iterator it = imNetworks[type].begin(); imNetworks[type].end() != it; ++it)
		{
			const ImNetwork_t * imNetwork = dynamic_cast<const ImNetwork_t *>(*it);

			std::set<ACS_CS_Interface> interfaces;

			for (std::set<std::string>::iterator if_dn = imNetwork->attachedInterfaces.begin(); imNetwork->attachedInterfaces.end() != if_dn; ++if_dn)
			{
				const ACS_CS_ImInterface * imInterface = dynamic_cast<const ACS_CS_ImInterface * >(model->getObject(if_dn->c_str()));

				std::string hostname = ACS_CS_ImUtils::getIdValueFromRdn(ACS_CS_ImUtils::getParentName(imInterface->rdn));
				std::string nicName = ACS_CS_ImUtils::getIdValueFromRdn(imInterface->rdn);

				interfaces.insert(ACS_CS_Interface(hostname, imInterface->mac, imNetwork->name, static_cast<Network::Domain>(imInterface->domain), nicName));
			}

			//Add to network list
			int index = atoi(ACS_CS_ImUtils::getIdValueFromRdn(imNetwork->rdn.c_str()).c_str());

			std::string nicNames[Network::DOMAIN_NR];
			for (std::set<std::string>::iterator it = imNetwork->nicName.begin(); imNetwork->nicName.end() != it; ++it)
			{
				std::string nicDomainStr, nicName;
				ACS_CS_NS::ACS_CS_Util::splitString(nicDomainStr, nicName, *it, "_");

				ACS_CS_FTRACE((ACS_CS_NetworkHandler_TRACE, LOG_LEVEL_DEBUG, "Reload : Domain %s, nicName %s", nicDomainStr.c_str(), nicName.c_str()));
				ACS_CS_TRACE((ACS_CS_NetworkHandler_TRACE, "Reload Domain:%s nic:%s net:%s index:%d type:%d\n", 
						nicDomainStr.c_str(), nicName.c_str(), imNetwork->name.c_str(), index, type));

				int nicDomain = atoi(nicDomainStr.c_str());

				if (nicDomain >= 0 && nicDomain < Network::DOMAIN_NR)
				{
					nicNames[nicDomain] = nicName;
				}
			}

//			for(uint32_t i = 0; i < Network::DOMAIN_NR; ++i)
//			{
//				ACS_CS_DEBUG(("Network %s reloaded NicNames[%d] = %s", imNetwork->name.c_str(), i, nicNames[i].c_str()));
//			}

			m_networks[type].add(index, imNetwork->name, nicNames, interfaces);
		}

		delete model;
	}
}
