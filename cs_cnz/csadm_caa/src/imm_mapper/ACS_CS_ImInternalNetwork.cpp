/*
 * ACS_CS_ImInternalNetwork.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: eanform
 */


#include "ACS_CS_ImInternalNetwork.h"
#include "ACS_CS_NetworkDefinitions.h"

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
#include "../../inc/imm_mapper/ACS_CS_ImExternalNetwork.h"


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImInternalNetwork);

ACS_CS_ImInternalNetwork::ACS_CS_ImInternalNetwork()
:adminState(EMPTY_ADMIN_STATE)
{
   type = TRM_INTERNALNETWORK_T;
}

ACS_CS_ImInternalNetwork::ACS_CS_ImInternalNetwork(const ACS_CS_ImInternalNetwork *other)
{
	copyObj(other);
}

ACS_CS_ImInternalNetwork::~ACS_CS_ImInternalNetwork()
{}


bool ACS_CS_ImInternalNetwork::isValid(int & errNo, const ACS_CS_ImModel & model)
{
	errNo = 0;

	//Verify if name is restricted
	if (!restrictedName.empty() && restrictedName.compare(name) != 0)
	{
		errNo = TC_RESTRICTEDOP;
		return false;
	}

	std::set<const ACS_CS_ImBase *> networks;
	model.getObjects(networks, TRM_EXTERNALNETWORK_T);
	model.getObjects(networks, TRM_INTERNALNETWORK_T);

	for (std::set<const ACS_CS_ImBase *>::iterator it = networks.begin(); networks.end() != it; ++it)
	{
		std::string otherName;

		if (TRM_EXTERNALNETWORK_T == (*it)->type)
		{
			const ACS_CS_ImExternalNetwork * network = dynamic_cast<const ACS_CS_ImExternalNetwork *>(*it);

			if (network)
			{
				otherName = network->name;
			}
		}
		else if (TRM_INTERNALNETWORK_T == (*it)->type)
		{
			const ACS_CS_ImInternalNetwork * network = dynamic_cast<const ACS_CS_ImInternalNetwork *>(*it);

			if (network && network->rdn.compare(rdn) != 0)
			{
				otherName = network->name;
			}
		}

		if (otherName.compare(name) == 0)
		{
			ACS_CS_ImUtils::setFaultyAttribute(name);
			errNo = TC_ATTRALREADYUSED;
			return false;
		}

	}

	return true;
}

bool ACS_CS_ImInternalNetwork::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++)
	{
		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_TRM_INTERNALNETWORK_ID.c_str()) == 0)
		{
			networkId = (char *) modAttr->attrValues[0];
		}
		else if (strcmp(modAttr->attrName, "name") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				name = (char *) modAttr->attrValues[0];
			else
				name = "";
		}
		else if (strcmp(modAttr->attrName, "restrictedName") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				restrictedName = (char *) modAttr->attrValues[0];
			else
				restrictedName = "";
		}
		else if (strcmp(modAttr->attrName, "attachedInterfaces") == 0)
		{
			attachedInterfaces.clear();
			for (unsigned int j = 0; j < modAttr->attrValuesNum; ++j)
			{
				attachedInterfaces.insert((char *) modAttr->attrValues[j]);
			}
		}
		else if (strcmp(modAttr->attrName, "adminState") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				adminState = (*(AdminState *) modAttr->attrValues[0]);
			else
				adminState = EMPTY_ADMIN_STATE;
		}
		else if (strcmp(modAttr->attrName, "nicName") == 0)
		{
			nicName.clear();
			for (unsigned int j = 0; j < modAttr->attrValuesNum; ++j)
			{
				nicName.insert((char *) modAttr->attrValues[j]);
			}
		}
		else if (strcmp(modAttr->attrName, "description") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				description = (char *) modAttr->attrValues[0];
			else
				description = "";
		}
		else
			// Bad attributeName
			return false;
	}

	return true;
}

ACS_CS_ImBase * ACS_CS_ImInternalNetwork::clone() const
{
   ACS_CS_ImBase *cp = new ACS_CS_ImInternalNetwork(*this);
   return cp;
}


bool ACS_CS_ImInternalNetwork::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
		return false;

	try
	{
		const ACS_CS_ImInternalNetwork & other = dynamic_cast<const ACS_CS_ImInternalNetwork &>(obj);

		if (networkId != other.networkId)
			return false;
		if (name != other.name)
			return false;
		if (restrictedName != other.restrictedName)
			return false;
		if (attachedInterfaces != other.attachedInterfaces)
			return false;
		if (adminState != other.adminState)
			return false;
		if (nicName != other.nicName)
			return false;
		if (description != other.description)
			return false;
	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;
}

void ACS_CS_ImInternalNetwork::copyObj(const ACS_CS_ImInternalNetwork *other)
{
	rdn = other->rdn;
	type = other->type;
	action = other->action;
	networkId = other->networkId;
	name = other->name;
	restrictedName = other->restrictedName;
	attachedInterfaces = other->attachedInterfaces;
	adminState = other->adminState;
	nicName = other->nicName;
	description = other->description;
}
ACS_CS_ImImmObject * ACS_CS_ImInternalNetwork::toImmObject(bool onlyModifiedAttrs)
{
   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_TRM_INTERNALNETWORK.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_TRM_INTERNALNETWORK_ID.c_str(), networkId));

   if (!name.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("name", name));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("name"));

   if (!restrictedName.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("restrictedName", restrictedName));


   if (!attachedInterfaces.empty())
	   attrList.push_back(ACS_CS_ImUtils::createNameType("attachedInterfaces", attachedInterfaces));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyNameType("attachedInterfaces"));

   if (adminState != EMPTY_ADMIN_STATE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("adminState", adminState));

   if (!nicName.empty())
	   attrList.push_back(ACS_CS_ImUtils::createMultipleStringType("nicName", nicName));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("nicName"));

   if (!description.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("description", description));

   immObject->setAttributeList(attrList);

   return immObject;
}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImInternalNetwork & ln, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(ln);
         ar & ln.networkId;
         ar & ln.name;
         ar & ln.restrictedName;
         ar & ln.attachedInterfaces;
         ar & ln.adminState;
         ar & ln.nicName;
         ar & ln.description;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImInternalNetwork & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImInternalNetwork & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImInternalNetwork & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImInternalNetwork & base, const unsigned int /*version*/);
   }
}



