/*
 * ACS_CS_ImCandidateCcFile.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: eanform
 */

#include "ACS_CS_ImCandidateCcFile.h"
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


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCandidateCcFile);


ACS_CS_ImCandidateCcFile::ACS_CS_ImCandidateCcFile():candidateCcFileId("")
{
   type = CANDIDATECCFILE_T;
}

ACS_CS_ImCandidateCcFile::ACS_CS_ImCandidateCcFile(const ACS_CS_ImCandidateCcFile &other)
{
   *this = other;
}

ACS_CS_ImCandidateCcFile::~ACS_CS_ImCandidateCcFile()
{}


bool ACS_CS_ImCandidateCcFile::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImCandidateCcFile::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID.c_str()) == 0)
      {
    	  candidateCcFileId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "reportProgress") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  reportProgress = ((char *) modAttr->attrValues[0]);
    	  else
    		  reportProgress = '\0';
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImCandidateCcFile::clone() const
{
   ACS_CS_ImBase *bladeClusterInfo = new ACS_CS_ImCandidateCcFile(*this);
   return bladeClusterInfo;
}


ACS_CS_ImCandidateCcFile & ACS_CS_ImCandidateCcFile::operator=(const ACS_CS_ImCandidateCcFile &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImCandidateCcFile & ACS_CS_ImCandidateCcFile::copyObj(const ACS_CS_ImCandidateCcFile &object)
{
	action = 				object.action;
	type = 					object.type;
	rdn = 					object.rdn;
	candidateCcFileId =		object.candidateCcFileId;
	reportProgress = 		object.reportProgress;

	return *this;
}


bool ACS_CS_ImCandidateCcFile::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImCandidateCcFile & other = dynamic_cast<const ACS_CS_ImCandidateCcFile &>(obj);

      if (candidateCcFileId != other.candidateCcFileId)
    	  return false;
      if (reportProgress != other.reportProgress)
                return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCandidateCcFile::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CANDIDATE_CCFILE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID.c_str(), candidateCcFileId));

   if (strlen(reportProgress.c_str()) != 0)
	   attrList.push_back(ACS_CS_ImUtils::createNameType("reportProgress", reportProgress));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyNameType("reportProgress"));


   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCandidateCcFile & bladeClusterInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(bladeClusterInfo);
         ar & bladeClusterInfo.candidateCcFileId;
         ar & bladeClusterInfo.reportProgress;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCandidateCcFile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCandidateCcFile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCandidateCcFile & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCandidateCcFile & base, const unsigned int /*version*/);
   }
}
