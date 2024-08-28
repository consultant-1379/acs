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
 * @file ACS_CS_ImEquipment.cpp
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

#include "ACS_CS_ImEquipment.h"
#include "ACS_CS_ImModel.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImEquipment);

ACS_CS_ImEquipment::ACS_CS_ImEquipment()
{
   type = EQUIPMENT_T;

}

ACS_CS_ImEquipment::ACS_CS_ImEquipment(const ACS_CS_ImEquipment &other)
{
   *this = other;
}

ACS_CS_ImEquipment::~ACS_CS_ImEquipment()
{}

bool ACS_CS_ImEquipment::isValid(int &errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;

   return true;
}


bool ACS_CS_ImEquipment::modify(ACS_APGCC_AttrModification **attrMods)
{

   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_EQUIPMENT_ID.c_str()) == 0)
      {
         axeEquipmentId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "isBladeClusterSystem") == 0)
      {
         isBladeClusterSystem = (*(bool *) modAttr->attrValues[0]);
      }
      else              // Bad attributeNameACS_CS_ImEquipment &operator=(const ACS_CS_ImEquipment &rhs);
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImEquipment::clone() const
{
   ACS_CS_ImBase *axeEquipment = new ACS_CS_ImEquipment(*this);
   return axeEquipment;
}


ACS_CS_ImEquipment &ACS_CS_ImEquipment::operator=(const ACS_CS_ImEquipment &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImEquipment &ACS_CS_ImEquipment::copyObj(const ACS_CS_ImEquipment &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	axeEquipmentId = object.axeEquipmentId;
	isBladeClusterSystem = object.isBladeClusterSystem;

	return *this;
}

bool ACS_CS_ImEquipment::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImEquipment & other = dynamic_cast<const ACS_CS_ImEquipment &>(obj);

      if (axeEquipmentId != other.axeEquipmentId)
         return false;
//      if (isBladeClusterSystem != other.isBladeClusterSystem)
//         return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }

   return true;

}



ACS_CS_ImImmObject * ACS_CS_ImEquipment::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_EQUIPMENT.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;
   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_EQUIPMENT_ID.c_str(), axeEquipmentId));
   //attrList.push_back(ACS_CS_ImUtils::createIntType("isBladeClusterSystem", isBladeClusterSystem));

   immObject->setAttributeList(attrList);


   return immObject;

}


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImEquipment & imeq, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(imeq);
         ar & imeq.axeEquipmentId;
         ar & imeq.isBladeClusterSystem;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImEquipment & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImEquipment & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImEquipment & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImEquipment & base, const unsigned int /*version*/);
   }
}
