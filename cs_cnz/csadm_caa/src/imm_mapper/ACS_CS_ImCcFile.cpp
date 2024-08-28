/*
 * ACS_CS_ImCcFile.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: eanform
 */


#include "ACS_CS_ImCcFile.h"
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


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCcFile);

ACS_CS_ImCcFile::ACS_CS_ImCcFile():ccFileId(""),state(NEW_CCF),rulesVersion("")
{
   type = CCFILE_T;
}

ACS_CS_ImCcFile::ACS_CS_ImCcFile(const ACS_CS_ImCcFile &other)
{
   *this = other;
}

ACS_CS_ImCcFile::~ACS_CS_ImCcFile()
{}


bool ACS_CS_ImCcFile::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImCcFile::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CCFILE_ID.c_str()) == 0)
      {
         ccFileId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "state") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  state = (*(CcFileStateType *) modAttr->attrValues[0]);
    	  else
    		  state = UNDEF_CCFSTATETYPE;
      }
      else if (strcmp(modAttr->attrName, "rulesVersion") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  rulesVersion = (char *) modAttr->attrValues[0];
    	  else
    		  rulesVersion = '\0';
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImCcFile::clone() const
{
   ACS_CS_ImBase *bladeClusterInfo = new ACS_CS_ImCcFile(*this);
   return bladeClusterInfo;
}


ACS_CS_ImCcFile & ACS_CS_ImCcFile::operator=(const ACS_CS_ImCcFile &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImCcFile & ACS_CS_ImCcFile::copyObj(const ACS_CS_ImCcFile &object)
{
	action = 				object.action;
	type = 					object.type;
	rdn = 					object.rdn;
	ccFileId =		 		object.ccFileId;
	state = 				object.state;
	rulesVersion = 			object.rulesVersion;

   return *this;
}


bool ACS_CS_ImCcFile::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImCcFile & other = dynamic_cast<const ACS_CS_ImCcFile &>(obj);

      if (ccFileId != other.ccFileId)
    	  return false;
      if (rulesVersion != other.rulesVersion)
    	  return false;
      if (state != other.state)
    	  return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCcFile::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CCFILE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CCFILE_ID.c_str(), ccFileId));

   if (state != UNDEF_CCFSTATETYPE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("state", state));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("state"));

   if (strlen(rulesVersion.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("rulesVersion", rulesVersion));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("rulesVersion"));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCcFile & bladeClusterInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(bladeClusterInfo);
         ar & bladeClusterInfo.ccFileId;
         ar & bladeClusterInfo.rulesVersion;
         ar & bladeClusterInfo.state;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCcFile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCcFile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCcFile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCcFile & base, const unsigned int /*version*/);
   }
}
