/*
 * ACS_CS_ImCrMgmt.cpp
 *
 *  Created on: Mar 27, 2015
 *      Author: eanform
 */

#include "ACS_CS_ImCrMgmt.h"
#include "ACS_CS_ImUtils.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCrMgmt);

ACS_CS_ImCrMgmt::ACS_CS_ImCrMgmt()
{
   type = CRMGMT_T;
}

ACS_CS_ImCrMgmt::ACS_CS_ImCrMgmt(const ACS_CS_ImCrMgmt &other)
{
	copyObj(other);
}

ACS_CS_ImCrMgmt::~ACS_CS_ImCrMgmt()
{
}

ACS_CS_ImCrMgmt &ACS_CS_ImCrMgmt::operator=(const ACS_CS_ImCrMgmt &rhs)
{
	copyObj(rhs);
	return *this;
}

bool ACS_CS_ImCrMgmt::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

   	return true;
}


bool ACS_CS_ImCrMgmt::modify(ACS_APGCC_AttrModification **attrMods)
{
	for (int i = 0; attrMods[i] != NULL; i++ )
   {
	  ACS_APGCC_AttrModification *atrMod = attrMods[i];

	  if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
		 return false;

	  ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

	  if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CRM_ID.c_str()) == 0)
	  {
		  crMgmtId = (char *) modAttr->attrValues[0];
	  }
	  else              // Bad attributeName
		 return false;

   }

	   return true;
}

ACS_CS_ImBase * ACS_CS_ImCrMgmt::clone() const
{
   ACS_CS_ImBase *ImHardwareMgmt = new ACS_CS_ImCrMgmt(*this);
   return ImHardwareMgmt;
}

void ACS_CS_ImCrMgmt::copyObj(const ACS_CS_ImCrMgmt &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	crMgmtId = object.crMgmtId;
}


bool ACS_CS_ImCrMgmt::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImCrMgmt & other = dynamic_cast<const ACS_CS_ImCrMgmt &>(obj);

      if (crMgmtId != other.crMgmtId)
         return false;

   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCrMgmt::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CRMGMT.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CRM_ID.c_str(), crMgmtId));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCrMgmt & hdwm, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(hdwm);
         ar & hdwm.crMgmtId;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCrMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCrMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCrMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCrMgmt & base, const unsigned int /*version*/);
   }
}
