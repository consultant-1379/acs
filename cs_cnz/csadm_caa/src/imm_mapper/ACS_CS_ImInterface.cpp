/*
 * ACS_CS_ImInterface.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: eanform
 */


#include "ACS_CS_ImInterface.h"
#include "ACS_CS_ImUtils.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImInterface);

ACS_CS_ImInterface::ACS_CS_ImInterface()
:domain(Network::DOMAIN_NR)
{
   type = TRM_INTERFACE_T;
}

ACS_CS_ImInterface::ACS_CS_ImInterface(const ACS_CS_ImInterface &other)
{
	copyObj(other);
}

ACS_CS_ImInterface::~ACS_CS_ImInterface()
{
}

ACS_CS_ImInterface &ACS_CS_ImInterface::operator=(const ACS_CS_ImInterface &rhs)
{
	copyObj(rhs);
	return *this;
}

bool ACS_CS_ImInterface::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

   	return true;
}


bool ACS_CS_ImInterface::modify(ACS_APGCC_AttrModification **attrMods)
{
	for (int i = 0; attrMods[i] != NULL; i++ )
   {
	  ACS_APGCC_AttrModification *atrMod = attrMods[i];

	  if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
		 return false;

	  ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

	  if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_TRM_INTERFACE_ID.c_str()) == 0)
	  {
		  interfaceId = (char *) modAttr->attrValues[0];
	  }
	  else if (strcmp(modAttr->attrName, "mac") == 0)
	  {
		  mac = (char *) modAttr->attrValues[0];
	  }
	  else if (strcmp(modAttr->attrName, "domain") == 0)
	  {
		  if (modAttr->attrValuesNum > 0)
			  domain = (*(int32_t *) modAttr->attrValues[0]);
		  else
			  domain = Network::DOMAIN_NR;
	  }
	  else              // Bad attributeName
		 return false;

   }

	   return true;
}

ACS_CS_ImBase * ACS_CS_ImInterface::clone() const
{
   ACS_CS_ImBase *Im = new ACS_CS_ImInterface(*this);
   return Im;
}

void ACS_CS_ImInterface::copyObj(const ACS_CS_ImInterface &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	interfaceId = object.interfaceId;
	mac = object.mac;
	domain = object.domain;
}


bool ACS_CS_ImInterface::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImInterface & other = dynamic_cast<const ACS_CS_ImInterface &>(obj);

      if (interfaceId != other.interfaceId)
    	  return false;
      if (mac != other.mac)
    	  return false;
      if (domain != other.domain)
    	  return false;

   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImInterface::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_TRM_INTERFACE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_TRM_INTERFACE_ID.c_str(), interfaceId));

   if (!mac.empty())
  	   attrList.push_back(ACS_CS_ImUtils::createStringType("mac", mac));
     else
  	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("mac"));

   if (domain != Network::DOMAIN_NR)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("domain", domain));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("domain"));


   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImInterface & hdwm, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(hdwm);
         ar & hdwm.interfaceId;
         ar & hdwm.mac;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImInterface & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImInterface & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImInterface & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImInterface & base, const unsigned int /*version*/);
   }
}


