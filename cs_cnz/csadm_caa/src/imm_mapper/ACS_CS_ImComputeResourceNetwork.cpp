/*
 * ACS_CS_ImComputeResourceNetwork.cpp
 *
 *  Created on: Jan 11, 2017
 *				xmalrao
 */

#include "ACS_CS_ImComputeResourceNetwork.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_NetworkDefinitions.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImComputeResourceNetwork);

ACS_CS_ImComputeResourceNetwork::ACS_CS_ImComputeResourceNetwork()
{
   type = CR_NETWORKSTRUCT_T;
}

ACS_CS_ImComputeResourceNetwork::ACS_CS_ImComputeResourceNetwork(const ACS_CS_ImComputeResourceNetwork *other)
{
	copyObj(other);
}

ACS_CS_ImComputeResourceNetwork::~ACS_CS_ImComputeResourceNetwork()
{}


bool ACS_CS_ImComputeResourceNetwork::isValid(int & errNo, const ACS_CS_ImModel & model)
{
	errNo = 0;

	if (ACS_CS_Registry::hexMACtoDecMAC((macAddress).c_str()) <= 0)
	{
		ACS_CS_ImUtils::setFaultyAttribute(macAddress);
		errNo = TC_INVALIDMAC;
		return false;
	}

	std::set<const ACS_CS_ImBase*> objects;

	model.getObjects(objects, CR_NETWORKSTRUCT_T);

	//std::string parentName = ACS_CS_ImUtils::getParentName(rdn).c_str();

	for (std::set<const ACS_CS_ImBase*>::iterator it = objects.begin(); it != objects.end(); it++)
	{
		//std::size_t foundParent = ((it->rdn).find(parentName));
		//if(foundParent!=std::string::npos)
		//{
			const ACS_CS_ImComputeResourceNetwork* otherComputeResourceNetworks = dynamic_cast<const ACS_CS_ImComputeResourceNetwork*>(*it);
			if (otherComputeResourceNetworks)
			{
				if (networkId.compare(otherComputeResourceNetworks->networkId) != 0)
				{
					if (strcasecmp(macAddress.c_str(),otherComputeResourceNetworks->macAddress.c_str()) == 0) 
					{
						ACS_CS_ImUtils::setFaultyAttribute(macAddress);
						errNo = TC_ATTRALREADYUSED;
						return false;
					}
				}
			}
	  //}
	}
	return true;
}

bool ACS_CS_ImComputeResourceNetwork::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++)
	{
		//ACS_APGCC_AttrModification *atrMod = attrMods[i];

		/*if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;*/

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, "id") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				networkId = (char *) modAttr->attrValues[0];
			else
				networkId = "";
		}
		else if (strcmp(modAttr->attrName, "nicName") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				nicName = (char *) modAttr->attrValues[0];
			else
				nicName = "";
		}
		else if (strcmp(modAttr->attrName, "netName") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				netName = (char *) modAttr->attrValues[0];
			else
				netName = "";
		}
		else if (strcmp(modAttr->attrName, "macAddress") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				macAddress = (char *) modAttr->attrValues[0];
			else
				macAddress = "";
		}
		else
			// Bad attributeName
			return false;
	}

	return true;
}

ACS_CS_ImBase * ACS_CS_ImComputeResourceNetwork::clone() const
{
   ACS_CS_ImBase *network = new ACS_CS_ImComputeResourceNetwork(*this);
   return network;
}


bool ACS_CS_ImComputeResourceNetwork::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
		return false;

	try
	{
		const ACS_CS_ImComputeResourceNetwork & other = dynamic_cast<const ACS_CS_ImComputeResourceNetwork &>(obj);

		if (networkId != other.networkId)
			return false;
		if (nicName != other.nicName)
			return false;
		if (netName != other.netName)
			return false;
		if (macAddress != other.macAddress)
			return false;
	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;
}

void ACS_CS_ImComputeResourceNetwork::copyObj(const ACS_CS_ImComputeResourceNetwork *other)
{
	rdn = other->rdn;
	type = other->type;
	action = other->action;
	networkId = other->networkId;
	nicName = other->nicName;
	netName = other->netName;
	macAddress = other->macAddress;
}

ACS_CS_ImImmObject * ACS_CS_ImComputeResourceNetwork::toImmObject(bool onlyModifiedAttrs)
{
   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE_NETWORK.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType("id", networkId));

   if (!netName.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("netName", netName));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("netName"));

   if (!macAddress.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("macAddress", macAddress));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("macAddress"));

   if (!nicName.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("nicName", nicName));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("nicName"));

   immObject->setAttributeList(attrList);

   return immObject;
}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImComputeResourceNetwork & cp, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(cp);
         ar & cp.networkId;
         ar & cp.nicName;
         ar & cp.netName;
         ar & cp.macAddress;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImComputeResourceNetwork & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImComputeResourceNetwork & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImComputeResourceNetwork & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImComputeResourceNetwork & base, const unsigned int /*version*/);
   }
}
