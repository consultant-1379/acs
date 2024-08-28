/*
 * ACS_CS_ImCcFileManager.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: eanform
 */

#include "ACS_CS_ImCcFileManager.h"
#include "ACS_CS_ImUtils.h"
#include <typeinfo>
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCcFileManager);


ACS_CS_ImCcFileManager::ACS_CS_ImCcFileManager():ccFileManagerId("")
{
   type = CCFILEMANAGER_T;
}

ACS_CS_ImCcFileManager::ACS_CS_ImCcFileManager(const ACS_CS_ImCcFileManager &other)
{
   *this = other;
}

ACS_CS_ImCcFileManager::~ACS_CS_ImCcFileManager()
{}


bool ACS_CS_ImCcFileManager::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImCcFileManager::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CCFILE_MANAGER_ID.c_str()) == 0)
      {
         ccFileManagerId = (char *) modAttr->attrValues[0];
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImCcFileManager::clone() const
{
   ACS_CS_ImBase *bladeClusterInfo = new ACS_CS_ImCcFileManager(*this);
   return bladeClusterInfo;
}


ACS_CS_ImCcFileManager & ACS_CS_ImCcFileManager::operator=(const ACS_CS_ImCcFileManager &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImCcFileManager & ACS_CS_ImCcFileManager::copyObj(const ACS_CS_ImCcFileManager &object)
{
	action = 				object.action;
	type = 					object.type;
	rdn = 					object.rdn;
	ccFileManagerId =		object.ccFileManagerId;

	return *this;
}


bool ACS_CS_ImCcFileManager::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImCcFileManager & other = dynamic_cast<const ACS_CS_ImCcFileManager &>(obj);

      if (ccFileManagerId != other.ccFileManagerId)
    	  return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCcFileManager::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CCFILE_MANAGER.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CCFILE_MANAGER_ID.c_str(), ccFileManagerId));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCcFileManager & bladeClusterInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(bladeClusterInfo);
         ar & bladeClusterInfo.ccFileManagerId;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCcFileManager & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCcFileManager & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCcFileManager & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCcFileManager & base, const unsigned int /*version*/);
   }
}
