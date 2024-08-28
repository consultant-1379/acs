/*
 * ACS_CS_ImComputeResource.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: eanform
 */

#include "ACS_CS_ImComputeResource.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_Registry.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImComputeResource);

ACS_CS_ImComputeResource::ACS_CS_ImComputeResource()
{
   type = COMPUTERESOURCE_T;
}

ACS_CS_ImComputeResource::ACS_CS_ImComputeResource(const ACS_CS_ImComputeResource *other)
{
	copyObj(other);
}

ACS_CS_ImComputeResource::~ACS_CS_ImComputeResource()
{}


bool ACS_CS_ImComputeResource::isValid(int & errNo, const ACS_CS_ImModel & model)
{
	errNo = 0;
	int shelfArchitecture = NODE_UNDEFINED;
	if (!ACS_CS_Registry::getNodeArchitecture(shelfArchitecture) && (NODE_VIRTUALIZED != shelfArchitecture))
	{
		errNo = TC_NOTALLOWED;
		return false;
	}

	if (ACS_CS_Registry::hexMACtoDecMAC((macAddressEthA).c_str()) <= 0)
	{
		ACS_CS_ImUtils::setFaultyAttribute(macAddressEthA);
		errNo = TC_INVALIDMAC;
		return false;
	}

	if (ACS_CS_Registry::hexMACtoDecMAC((macAddressEthB).c_str()) <= 0)
	{
		ACS_CS_ImUtils::setFaultyAttribute(macAddressEthB);
		errNo = TC_INVALIDMAC;
		return false;
	}

	//Validate IP Addresses
	struct in_addr convaddr;

	if (!ipAddressEthA.empty() && inet_pton(AF_INET, ipAddressEthA.c_str(), &convaddr) != 1)
	{
		ACS_CS_ImUtils::setFaultyAttribute(ipAddressEthA);
		errNo = TC_INVALIDIP;
		return false;
	}

	if (!ipAddressEthB.empty() && inet_pton(AF_INET, ipAddressEthB.c_str(), &convaddr) != 1)
	{
		ACS_CS_ImUtils::setFaultyAttribute(ipAddressEthB);
		errNo = TC_INVALIDIP;
		return false;
	}

	if ( uuid.compare(ACS_CS_ImUtils::getIdValueFromRdn(computeResourceId)) )
	{
		ACS_CS_ImUtils::setFaultyAttribute(uuid);
		errNo = TC_INVALIDUUID;
		return false;
	}

	//MAC addresses and UUID must be unique
	if (strcasecmp(macAddressEthA.c_str(), macAddressEthB.c_str()) == 0)
	{
		ACS_CS_ImUtils::setFaultyAttribute(macAddressEthA);
		errNo = TC_ATTRALREADYUSED;
		return false;
	}

	if (!ipAddressEthA.empty() && ipAddressEthA.compare("0.0.0.0") != 0)
	{
		if (ipAddressEthA.compare(ipAddressEthB) == 0)
		{
			ACS_CS_ImUtils::setFaultyAttribute(ipAddressEthA);
			errNo = TC_ATTRALREADYUSED;
			return false;
		}
	}

	std::set<const ACS_CS_ImBase*> objects;

	model.getObjects(objects, COMPUTERESOURCE_T);

	for (std::set<const ACS_CS_ImBase*>::iterator it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImComputeResource* otherComputeResource = dynamic_cast<const ACS_CS_ImComputeResource*>(*it);
		if (otherComputeResource)
		{
			if (computeResourceId.compare(otherComputeResource->computeResourceId) != 0)
			{
				if (strcasecmp(uuid.c_str(),otherComputeResource->uuid.c_str()) == 0)
				{
					ACS_CS_ImUtils::setFaultyAttribute(uuid);
					errNo = TC_ATTRALREADYUSED;
					return false;
				}

				if (strcasecmp(macAddressEthA.c_str(),otherComputeResource->macAddressEthA.c_str()) == 0 ||
						strcasecmp(macAddressEthA.c_str(),otherComputeResource->macAddressEthB.c_str()) == 0)
				{
					ACS_CS_ImUtils::setFaultyAttribute(macAddressEthA);
					errNo = TC_ATTRALREADYUSED;
					return false;
				}

				if (strcasecmp(macAddressEthB.c_str(),otherComputeResource->macAddressEthA.c_str()) == 0 ||
						strcasecmp(macAddressEthB.c_str(),otherComputeResource->macAddressEthB.c_str()) == 0)
				{
					ACS_CS_ImUtils::setFaultyAttribute(macAddressEthB);
					errNo = TC_ATTRALREADYUSED;
					return false;
				}

				if (!ipAddressEthA.empty() && ipAddressEthA.compare("0.0.0.0") != 0)
				{
					if (ipAddressEthA.compare(otherComputeResource->ipAddressEthA) == 0 ||
							ipAddressEthA.compare(otherComputeResource->ipAddressEthB) == 0)
					{
						ACS_CS_ImUtils::setFaultyAttribute(ipAddressEthA);
						errNo = TC_ATTRALREADYUSED;
						return false;
					}	
				}	

				if (!ipAddressEthB.empty() && ipAddressEthB.compare("0.0.0.0") != 0)
				{
					if (ipAddressEthB.compare(otherComputeResource->ipAddressEthA) == 0 ||
							ipAddressEthB.compare(otherComputeResource->ipAddressEthB) == 0)
					{
						ACS_CS_ImUtils::setFaultyAttribute(ipAddressEthB);
						errNo = TC_ATTRALREADYUSED;
						return false;
					}
				}

				if ((crRoleId == otherComputeResource->crRoleId) || crRoleId >= 640 )
				{
					if(crType == 0)// BC_TYPE = 0
					{
						ACS_CS_ImUtils::setFaultyAttribute(crRoleId);

						if(crRoleId >= 640)
						{
							errNo = TC_INVALIDCRROLEID;
						}
						else
						{
							errNo = TC_ATTRALREADYUSED;
						}
						return false ;
					}
				}

				if (strcasecmp(crRoleLabel.c_str(),otherComputeResource->crRoleLabel.c_str()) == 0)
				{
					ACS_CS_ImUtils::setFaultyAttribute(crRoleLabel);
					errNo = TC_ATTRALREADYUSED;
					return false;
				}
			}
		}

	}

	return true;
}

bool ACS_CS_ImComputeResource::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++)
	{
		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		// Commented to allow multi attribute values 
		/*if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE) 
			return false;*/

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_COMPUTE_RESOURCE_ID.c_str()) == 0)
		{
			computeResourceId = (char *) modAttr->attrValues[0];
		}
		else if (strcmp(modAttr->attrName, "uuid") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				uuid = (char *) modAttr->attrValues[0];
			else
				uuid = "";
		}
		else if (strcmp(modAttr->attrName, "crType") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				crType = (*(int *)  modAttr->attrValues[0]);
			else
				crType = UNDEF_TYPE;
		}
		else if (strcmp(modAttr->attrName, "macAddressEthA") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				macAddressEthA = (char *) modAttr->attrValues[0];
			else
				macAddressEthA = "";
		}
		else if (strcmp(modAttr->attrName, "macAddressEthB") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				macAddressEthB = (char *) modAttr->attrValues[0];
			else
				macAddressEthB = "";
		}
		else if (strcmp(modAttr->attrName, "crRoleLabel") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				crRoleLabel = (char *) modAttr->attrValues[0];
			else
				crRoleLabel = "";
		}
		else if (strcmp(modAttr->attrName, "crRoleId") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				crRoleId = (*(int *)  modAttr->attrValues[0]);
			else
				crRoleId = ROLE_FREE;
		}
		else if (strcmp(modAttr->attrName, "ipAddressEthA") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				ipAddressEthA = (char *) modAttr->attrValues[0];
			else
				ipAddressEthA = "";
		}
		else if (strcmp(modAttr->attrName, "ipAddressEthB") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				ipAddressEthB = (char *) modAttr->attrValues[0];
			else
				ipAddressEthB = "";
		}
		else if (strcmp(modAttr->attrName, "network") == 0)
		{
				char** attrValues = reinterpret_cast<char**>(modAttr->attrValues);

				switch(atrMod->modType)
				{
					case ACS_APGCC_ATTR_VALUES_REPLACE:
							networks.clear();
					case ACS_APGCC_ATTR_VALUES_ADD:
							networks.insert(attrValues, attrValues + modAttr->attrValuesNum);
							break;
					case ACS_APGCC_ATTR_VALUES_DELETE:
							for (uint32_t k = 0; k < modAttr->attrValuesNum; k++)
							{
				 					networks.erase(attrValues[k]);
							}
							break;
				}
		}
		else
			// Bad attributeName
			return false;
	}

	return true;
}

ACS_CS_ImBase * ACS_CS_ImComputeResource::clone() const
{
   ACS_CS_ImBase *cp = new ACS_CS_ImComputeResource(*this);
   return cp;
}


bool ACS_CS_ImComputeResource::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
		return false;

	try
	{
		const ACS_CS_ImComputeResource & other = dynamic_cast<const ACS_CS_ImComputeResource &>(obj);

		if (computeResourceId != other.computeResourceId)
			return false;
		if (uuid != other.uuid)
			return false;
		if (crType != other.crType)
			return false;
		if (macAddressEthA != other.macAddressEthA)
			return false;
		if (macAddressEthB != other.macAddressEthB)
			return false;
		if (crRoleLabel != other.crRoleLabel)
			return false;
		if (crRoleId != other.crRoleId)
			return false;
		if (ipAddressEthA != other.ipAddressEthA)
			return false;
		if (ipAddressEthB != other.ipAddressEthB)
			return false;
		if (networks != other.networks)
			return false;

	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;
}

void ACS_CS_ImComputeResource::copyObj(const ACS_CS_ImComputeResource *other)
{
	rdn = other->rdn;
	type = other->type;
	action = other->action;
	computeResourceId = other->computeResourceId;
	uuid = other->uuid;
	crType = other->crType;
	macAddressEthA = other->macAddressEthA;
	macAddressEthB = other->macAddressEthB;
	crRoleLabel = other->crRoleLabel;
	crRoleId = other->crRoleId;
	ipAddressEthA = other->ipAddressEthA;
	ipAddressEthB = other->ipAddressEthB;
	networks = other->networks;
}
ACS_CS_ImImmObject * ACS_CS_ImComputeResource::toImmObject(bool onlyModifiedAttrs)
{
   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_COMPUTE_RESOURCE_ID.c_str(), computeResourceId));

   if (!uuid.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("uuid", uuid));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("uuid"));

   if (!macAddressEthA.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("macAddressEthA", macAddressEthA));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("macAddressEthA"));

   if (!macAddressEthB.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("macAddressEthB", macAddressEthB));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("macAddressEthB"));

   if (!crRoleLabel.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("crRoleLabel", crRoleLabel));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("crRoleLabel"));

   if (!ipAddressEthA.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("ipAddressEthA", ipAddressEthA));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("ipAddressEthA"));

   if (!ipAddressEthB.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("ipAddressEthB", ipAddressEthB));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("ipAddressEthB"));

   if (!networks.empty())
	    attrList.push_back(ACS_CS_ImUtils::createNameType("network", networks));
   else
	    attrList.push_back(ACS_CS_ImUtils::createEmptyNameType("network"));

   attrList.push_back(ACS_CS_ImUtils::createIntType("crType", crType));
   attrList.push_back(ACS_CS_ImUtils::createIntType("crRoleId", crRoleId));

   immObject->setAttributeList(attrList);

   return immObject;
}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImComputeResource & cp, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(cp);
         ar & cp.computeResourceId;
         ar & cp.uuid;
         ar & cp.crType;
         ar & cp.macAddressEthA;
         ar & cp.macAddressEthB;
         ar & cp.crRoleLabel;
         ar & cp.crRoleId;
         ar & cp.ipAddressEthA;
         ar & cp.ipAddressEthB;
         ar & cp.networks;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImComputeResource & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImComputeResource & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImComputeResource & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImComputeResource & base, const unsigned int /*version*/);
   }
}
