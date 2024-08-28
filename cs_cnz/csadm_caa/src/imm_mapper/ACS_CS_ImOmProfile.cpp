/*
 * ACS_CS_ImOmProfile.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: eanform
 */

#include "ACS_CS_ImOmProfile.h"
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

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImOmProfile);

ACS_CS_ImOmProfile::ACS_CS_ImOmProfile():omProfileId(), state(UNDEFINED_OMP), apzProfile(""), aptProfile("")//, currentCcFile("")
{
   type = OMPROFILE_T;
}

ACS_CS_ImOmProfile::ACS_CS_ImOmProfile(const ACS_CS_ImOmProfile &other)
{
   *this = other;
}

ACS_CS_ImOmProfile::~ACS_CS_ImOmProfile()
{}


bool ACS_CS_ImOmProfile::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImOmProfile::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_OM_PROFILE_ID.c_str()) == 0)
      {
         omProfileId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "state") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  state = (*(OmProfileStateType *) modAttr->attrValues[0]);
    	  else
    		  state = UNDEF_OMPROFILESTATETYPE;
      }
      else if (strcmp(modAttr->attrName, "apzProfile") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  apzProfile = (char *) modAttr->attrValues[0];
    	  else
    		  apzProfile = '\0';
      }
      else if (strcmp(modAttr->attrName, "aptProfile") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  aptProfile =  (char *) modAttr->attrValues[0];
    	  else
    		  aptProfile = '\0';
      }
//      else if (strcmp(modAttr->attrName, "currentCcFile") == 0)
//      {
//    	  if (modAttr->attrValuesNum > 0)
//    		  currentCcFile = ((char *) modAttr->attrValues[0]);
//    	  else
//    		  aptProfile = '\0';
//      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImOmProfile::clone() const
{
   ACS_CS_ImBase *bladeClusterInfo = new ACS_CS_ImOmProfile(*this);
   return bladeClusterInfo;
}


ACS_CS_ImOmProfile & ACS_CS_ImOmProfile::operator=(const ACS_CS_ImOmProfile &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImOmProfile & ACS_CS_ImOmProfile::copyObj(const ACS_CS_ImOmProfile &object)
{
	action = 				object.action;
	type = 					object.type;
	rdn = 					object.rdn;
	omProfileId =		 	object.omProfileId;
	apzProfile = 			object.apzProfile;
	state = 				object.state;
	aptProfile = 			object.aptProfile;
//	currentCcFile = 		object.currentCcFile;

   return *this;
}


bool ACS_CS_ImOmProfile::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImOmProfile & other = dynamic_cast<const ACS_CS_ImOmProfile &>(obj);

      if (omProfileId != other.omProfileId)
    	  return false;
      if (apzProfile != other.apzProfile)
    	  return false;
      if (state != other.state)
    	  return false;
      if (aptProfile != other.aptProfile)
    	  return false;
//      if (currentCcFile != other.currentCcFile)
//    	  return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImOmProfile::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_OM_PROFILE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_OM_PROFILE_ID.c_str(), omProfileId));

   if (state != UNDEF_OMPROFILESTATETYPE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("state", state));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("state"));

   if (strlen(apzProfile.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("apzProfile", apzProfile));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("apzProfile"));

   if (strlen(aptProfile.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("aptProfile", aptProfile));
   else
   	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("aptProfile"));

//   if (strlen(currentCcFile.c_str()) > 0)
//	   attrList.push_back(ACS_CS_ImUtils::createStringType("currentCcFile", currentCcFile));
//   else
//	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("currentCcFile"));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImOmProfile & bladeClusterInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(bladeClusterInfo);
         ar & bladeClusterInfo.omProfileId;
         ar & bladeClusterInfo.apzProfile;
         ar & bladeClusterInfo.state;
         ar & bladeClusterInfo.aptProfile;
//         ar & bladeClusterInfo.currentCcFile;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImOmProfile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImOmProfile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImOmProfile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImOmProfile & base, const unsigned int /*version*/);
   }
}
