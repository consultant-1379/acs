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
 * @file ACS_CS_ImHardwareMgmt.cpp
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

#include "ACS_CS_ImHardwareMgmt.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>


#include "ACS_CS_ImUtils.h"

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImHardwareMgmt);

ACS_CS_ImHardwareMgmt::ACS_CS_ImHardwareMgmt()
{
   type = HARDWAREMGMT_T;
}

ACS_CS_ImHardwareMgmt::ACS_CS_ImHardwareMgmt(const ACS_CS_ImHardwareMgmt &other)
{
	copyObj(other);
}

ACS_CS_ImHardwareMgmt::~ACS_CS_ImHardwareMgmt()
{
}

ACS_CS_ImHardwareMgmt &ACS_CS_ImHardwareMgmt::operator=(const ACS_CS_ImHardwareMgmt &rhs)
{
	copyObj(rhs);
	return *this;
}

bool ACS_CS_ImHardwareMgmt::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

   	return true;
}


bool ACS_CS_ImHardwareMgmt::modify(ACS_APGCC_AttrModification **attrMods)
{
	for (int i = 0; attrMods[i] != NULL; i++ )
   {
	  ACS_APGCC_AttrModification *atrMod = attrMods[i];

	  if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
		 return false;

	  ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

	  string axeHardwareMgmtId;

	  if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_HARDWARE_ID.c_str()) == 0)
	  {
		  axeHardwareMgmtId = (char *) modAttr->attrValues[0];
	  }
	  else if (strcmp(modAttr->attrName, "staticIpMap") == 0)
	  {
		  staticIpMap.clear();
		  for (uint32_t j = 0; j < modAttr->attrValuesNum; j++)
		  {
			  staticIpMap.insert(reinterpret_cast<char *>(modAttr->attrValues[j]));
		  }
	  }
	  else              // Bad attributeName
		 return false;

   }

	   return true;
}

ACS_CS_ImBase * ACS_CS_ImHardwareMgmt::clone() const
{
   ACS_CS_ImBase *ImHardwareMgmt = new ACS_CS_ImHardwareMgmt(*this);
   return ImHardwareMgmt;
}

void ACS_CS_ImHardwareMgmt::copyObj(const ACS_CS_ImHardwareMgmt &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	axeHardwareMgmtId = object.axeHardwareMgmtId;
	staticIpMap = object.staticIpMap;
}


bool ACS_CS_ImHardwareMgmt::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImHardwareMgmt & other = dynamic_cast<const ACS_CS_ImHardwareMgmt &>(obj);

      if (axeHardwareMgmtId != other.axeHardwareMgmtId)
         return false;
      if (staticIpMap != other.staticIpMap)
    	  return false;

   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImHardwareMgmt::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_HARDWARE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_HARDWARE_ID.c_str(), axeHardwareMgmtId));

   attrList.push_back(ACS_CS_ImUtils::createMultipleStringType("staticIpMap", staticIpMap));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImHardwareMgmt & hdwm, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(hdwm);
         ar & hdwm.axeHardwareMgmtId;
         ar & hdwm.staticIpMap;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImHardwareMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImHardwareMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImHardwareMgmt & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImHardwareMgmt & base, const unsigned int /*version*/);
   }
}
