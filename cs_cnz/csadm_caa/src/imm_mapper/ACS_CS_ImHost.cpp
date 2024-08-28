/*
 * ACS_CS_ImHost.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: eanform
 */

#include "ACS_CS_ImHost.h"
#include "ACS_CS_ImUtils.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImHost);

ACS_CS_ImHost::ACS_CS_ImHost()
{
   type = TRM_HOST_T;
}

ACS_CS_ImHost::ACS_CS_ImHost(const ACS_CS_ImHost &other)
{
	copyObj(other);
}

ACS_CS_ImHost::~ACS_CS_ImHost()
{
}

ACS_CS_ImHost &ACS_CS_ImHost::operator=(const ACS_CS_ImHost &rhs)
{
	copyObj(rhs);
	return *this;
}

bool ACS_CS_ImHost::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

   	return true;
}


bool ACS_CS_ImHost::modify(ACS_APGCC_AttrModification **attrMods)
{
	for (int i = 0; attrMods[i] != NULL; i++ )
   {
	  ACS_APGCC_AttrModification *atrMod = attrMods[i];

	  if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
		 return false;

	  ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

	  if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_TRM_HOST_ID.c_str()) == 0)
	  {
		  hostId = (char *) modAttr->attrValues[0];
	  }
	  else if (strcmp(modAttr->attrName, "computeResourceDn") == 0)
	  {
		  computeResourceDn = (char *) modAttr->attrValues[0];
	  }
	  else              // Bad attributeName
		 return false;

   }

	   return true;
}

ACS_CS_ImBase * ACS_CS_ImHost::clone() const
{
   ACS_CS_ImBase *ImHost = new ACS_CS_ImHost(*this);
   return ImHost;
}

void ACS_CS_ImHost::copyObj(const ACS_CS_ImHost &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	hostId = object.hostId;
	computeResourceDn = object.computeResourceDn;
}


bool ACS_CS_ImHost::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImHost & other = dynamic_cast<const ACS_CS_ImHost &>(obj);

      if (hostId != other.hostId)
    	  return false;
      if (computeResourceDn != other.computeResourceDn)
    	  return false;

   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImHost::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_TRM_HOST.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_TRM_HOST_ID.c_str(), hostId));

   if (!computeResourceDn.empty())
  	   attrList.push_back(ACS_CS_ImUtils::createNameType("computeResourceDn", computeResourceDn));
     else
  	   attrList.push_back(ACS_CS_ImUtils::createEmptyNameType("computeResourceDn"));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImHost & hdwm, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(hdwm);
         ar & hdwm.hostId;
         ar & hdwm.computeResourceDn;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImHost & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImHost & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImHost & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImHost & base, const unsigned int /*version*/);
   }
}
