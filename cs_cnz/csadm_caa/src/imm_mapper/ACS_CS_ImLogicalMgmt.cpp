#include "ACS_CS_ImLogicalMgmt.h"


#include "ACS_CS_ImLogicalMgmt.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImValuesDefinitionType.h"
#include <iostream>
#include <cstring>
#include <sstream>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImLogicalMgmt);

ACS_CS_ImLogicalMgmt::ACS_CS_ImLogicalMgmt()
{
   type = LOGICALMGMT_T;
}

ACS_CS_ImLogicalMgmt::ACS_CS_ImLogicalMgmt(const ACS_CS_ImLogicalMgmt &other)
{
   *this = other;
}

ACS_CS_ImLogicalMgmt::~ACS_CS_ImLogicalMgmt()
{}

bool ACS_CS_ImLogicalMgmt::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

	return true;
}

bool ACS_CS_ImLogicalMgmt::modify(ACS_APGCC_AttrModification **attrMods)
{


   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_LOGICAL_ID.c_str()) == 0)
      {
         axeLogicalMgmtId = (char *) modAttr->attrValues[0];
      }
      else              // Bad attributeName
        return false;
   }

   return true;
}

ACS_CS_ImBase * ACS_CS_ImLogicalMgmt::clone() const
{
   ACS_CS_ImBase *LogicalMgmt = new ACS_CS_ImLogicalMgmt(*this);
   return LogicalMgmt;
}


ACS_CS_ImLogicalMgmt &ACS_CS_ImLogicalMgmt::operator=(const ACS_CS_ImLogicalMgmt &rhs)
{

   copyObj(rhs);
   return *this;
}



ACS_CS_ImLogicalMgmt &ACS_CS_ImLogicalMgmt::copyObj(const ACS_CS_ImLogicalMgmt &object)
{

	action = object.action;
	type = object.type;
	rdn = object.rdn;
	axeLogicalMgmtId = object.axeLogicalMgmtId;

   return *this;
}


bool ACS_CS_ImLogicalMgmt::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
       return false;

   try {
      const ACS_CS_ImLogicalMgmt & other = dynamic_cast<const ACS_CS_ImLogicalMgmt &>(obj);

      if (axeLogicalMgmtId != other.axeLogicalMgmtId)
         return false;

   }
   catch (const std::bad_cast &e)
   {
      std::cerr << e.what() << std::endl;
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImLogicalMgmt::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_LOGICAL.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_LOGICAL_ID.c_str(), axeLogicalMgmtId));

   immObject->setAttributeList(attrList);

   return immObject;

}


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImLogicalMgmt & logical, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(logical);
         ar & logical.axeLogicalMgmtId;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImLogicalMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImLogicalMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImLogicalMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImLogicalMgmt & base, const unsigned int /*version*/);
   }
}
