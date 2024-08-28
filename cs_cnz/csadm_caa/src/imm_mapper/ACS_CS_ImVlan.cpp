/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImVlan.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
  *
  * REVISION HISTORY
  *
  * DATE        USER     DESCRIPTION/TR
  * --------------------------------
  * 2011-08-29  XBJOAXE  Starting from scratch
  *
  ****************************************************************************/

#include "ACS_CS_ImVlan.h"
#include "ACS_CS_ImUtils.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImVlan);

ACS_CS_ImVlan::ACS_CS_ImVlan()
{
	type = VLAN_T;
}


ACS_CS_ImVlan::ACS_CS_ImVlan(const ACS_CS_ImVlan &other)
{
	copyObj(other);
}

ACS_CS_ImVlan::~ACS_CS_ImVlan()
{

}

ACS_CS_ImVlan & ACS_CS_ImVlan::operator=(const ACS_CS_ImVlan &rhs)
{
	copyObj(rhs);
	return *this;
}


bool ACS_CS_ImVlan::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;
	if(!(this->pcp >= 0 && this->pcp < 8))
	{
		return false;
	}
	return true;
}

bool ACS_CS_ImVlan::modify(ACS_APGCC_AttrModification **attrMods)
{

	for (int i = 0; attrMods[i] != NULL; i++ )
	{

		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;


		if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_VLAN_ID.c_str()) == 0)
		{
			vlanId = reinterpret_cast<char *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "networkAddress") == 0)
		{
			networkAddress = reinterpret_cast<char *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "netmask") == 0)
		{
			netmask = reinterpret_cast<char *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "name") == 0)
		{
			name = reinterpret_cast<char *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "stack") == 0)
		{
			stack = *reinterpret_cast<VlanStackEnum *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "pcp") == 0)
		{
			pcp = *reinterpret_cast<int *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "vlanType") == 0)
		{
			vlanType = *reinterpret_cast<VlanTypeEnum *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "vlanTag") == 0)
		{
			vlanTag = *reinterpret_cast<uint32_t * >(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "vNICName") == 0)
		{
			vNICName = reinterpret_cast<char *>(modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "reserved") == 0)
		{
			reserved = *reinterpret_cast<int *>(modAttr->attrValues[0]);
		}
		else              // Bad attributeName
			return false;

	}

	return true;
}

void ACS_CS_ImVlan::copyObj(const ACS_CS_ImVlan &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	vlanId = object.vlanId;
	networkAddress = object.networkAddress;
	netmask = object.netmask;
	name = object.name;
	stack = object.stack;
	pcp = object.pcp;
	vlanType = object.vlanType;
	vlanTag = object.vlanTag;
	vNICName = object.vNICName;
	reserved = object.reserved;
}


ACS_CS_ImBase * ACS_CS_ImVlan::clone() const
{
	ACS_CS_ImBase *vlan = new ACS_CS_ImVlan(*this);
	return vlan;
}

bool ACS_CS_ImVlan::equals(const ACS_CS_ImBase &obj) const
{
	if (!ACS_CS_ImBase::equals(obj))
		return false;

	try {
		const ACS_CS_ImVlan & other = dynamic_cast<const ACS_CS_ImVlan &>(obj);

		if (vlanId != other.vlanId)
			return false;
		if (networkAddress != other.networkAddress)
			return false;
		if (netmask != other.netmask)
			return false;
		if (name != other.name)
			return false;
		if (stack != other.stack)
			return false;
		if(pcp != other.pcp)
			return false;
		if(vlanType != other.vlanType)
			return false;
		if(vlanTag != other.vlanTag)
			return false;
		if(vNICName != other.vNICName)
			return false;
		if(reserved != other.reserved)
			return false;
	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;

}

ACS_CS_ImImmObject * ACS_CS_ImVlan::toImmObject(bool onlyModifiedAttrs)
{
	ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

	immObject->setClassName(ACS_CS_ImmMapper::CLASS_VLAN.c_str());
	immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

	std::vector<ACS_CS_ImValuesDefinitionType> attrList;

	if(!onlyModifiedAttrs)
		attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_VLAN_ID.c_str(), vlanId));
	attrList.push_back(ACS_CS_ImUtils::createStringType("networkAddress", networkAddress));
	attrList.push_back(ACS_CS_ImUtils::createStringType("netmask", netmask));
	attrList.push_back(ACS_CS_ImUtils::createStringType("name", name));
	attrList.push_back(ACS_CS_ImUtils::createIntType("stack", stack));
	attrList.push_back(ACS_CS_ImUtils::createIntType("pcp",pcp));
	attrList.push_back(ACS_CS_ImUtils::createIntType("vlanType",vlanType));
	attrList.push_back(ACS_CS_ImUtils::createIntType("reserved",reserved));
	attrList.push_back(ACS_CS_ImUtils::createUIntType("vlanTag",vlanTag));
	attrList.push_back(ACS_CS_ImUtils::createStringType("vNICName",vNICName));

	immObject->setAttributeList(attrList);


	return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImVlan & vlan, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(vlan);
         ar & vlan.vlanId;
         ar & vlan.networkAddress;
         ar & vlan.netmask;
         ar & vlan.name;
         ar & vlan.stack;
         ar & vlan.pcp;
         ar & vlan.vlanType;
         ar & vlan.reserved;
         ar & vlan.vlanTag;
         ar & vlan.vNICName;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImVlan & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImVlan & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImVlan & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImVlan & base, const unsigned int /*version*/);

   }
}

