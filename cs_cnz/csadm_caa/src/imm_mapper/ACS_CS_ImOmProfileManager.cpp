/*
 * ACS_CS_ImOmProfileManager.cpp
 *
 *  Created on: Apr 9, 2013
 *      Author: eanform
 */

#include "ACS_CS_ImOmProfileManager.h"
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

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImOmProfileManager);

ACS_CS_ImOmProfileManager::ACS_CS_ImOmProfileManager()
{
   type = OMPROFILEMANAGER_T;
}

ACS_CS_ImOmProfileManager::ACS_CS_ImOmProfileManager(const ACS_CS_ImOmProfileManager &other)
{
   *this = other;
}

ACS_CS_ImOmProfileManager::~ACS_CS_ImOmProfileManager()
{}


bool ACS_CS_ImOmProfileManager::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImOmProfileManager::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_OM_PROFILE_MANAGER_ID.c_str()) == 0)
      {
    	  omProfileManagerId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "omProfile") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  omProfile = (char *) modAttr->attrValues[0];
    	  else
    		  omProfile = '\0';
      }
      else if (strcmp(modAttr->attrName, "reportProgress") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  reportProgress = ((char *) modAttr->attrValues[0]);
    	  else
    		  reportProgress = '\0';
      }
      else if (strcmp(modAttr->attrName, "activeCcFile") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  activeCcFile = ((char *) modAttr->attrValues[0]);
    	  else
    		  activeCcFile = '\0';
      }
      else if (strcmp(modAttr->attrName, "scope") == 0)
      {
    	  scope = (*(ProfileScopeType *) modAttr->attrValues[0]);
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImOmProfileManager::clone() const
{
   ACS_CS_ImBase *bladeClusterInfo = new ACS_CS_ImOmProfileManager(*this);
   return bladeClusterInfo;
}


ACS_CS_ImOmProfileManager & ACS_CS_ImOmProfileManager::operator=(const ACS_CS_ImOmProfileManager &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImOmProfileManager & ACS_CS_ImOmProfileManager::copyObj(const ACS_CS_ImOmProfileManager &object)
{
	action = 				object.action;
	type = 					object.type;
	rdn = 					object.rdn;
	omProfileManagerId = 	object.omProfileManagerId;
	omProfile = 			object.omProfile;
	reportProgress = 		object.reportProgress;
	activeCcFile =	 		object.activeCcFile;
	scope =					object.scope;

   return *this;
}


bool ACS_CS_ImOmProfileManager::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImOmProfileManager & other = dynamic_cast<const ACS_CS_ImOmProfileManager &>(obj);

      if (omProfileManagerId != other.omProfileManagerId)
    	  return false;
      if (omProfile != other.omProfile)
    	  return false;
      if (reportProgress != other.reportProgress)
          return false;
      if (activeCcFile != other.activeCcFile)
          return false;
      if (scope != other.scope)
              return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImOmProfileManager::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_OM_PROFILE_MANAGER.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;
   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_OM_PROFILE_MANAGER_ID.c_str(), omProfileManagerId));

   if (strlen(omProfile.c_str()) != 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("omProfile", omProfile));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("omProfile"));

   if (strlen(reportProgress.c_str()) != 0)
  	   attrList.push_back(ACS_CS_ImUtils::createNameType("reportProgress", reportProgress));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyNameType("reportProgress"));

   if (strlen(activeCcFile.c_str()) != 0)
     	   attrList.push_back(ACS_CS_ImUtils::createStringType("activeCcFile", activeCcFile));
   else
   	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("activeCcFile"));

   if (scope != UNDEF_PROFILESCOPETYPE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("scope", scope));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("scope"));

   immObject->setAttributeList(attrList);


   return immObject;

}


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImOmProfileManager & bladeClusterInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(bladeClusterInfo);
         ar & bladeClusterInfo.omProfileManagerId;
         ar & bladeClusterInfo.omProfile;
         ar & bladeClusterInfo.activeCcFile;
         ar & bladeClusterInfo.reportProgress;
         ar & bladeClusterInfo.scope;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImOmProfileManager & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImOmProfileManager & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImOmProfileManager & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImOmProfileManager & base, const unsigned int /*version*/);
   }
}
