#include "ACS_CS_ImCrmEquipment.h"
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

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCrmEquipment);

ACS_CS_ImCrmEquipment::ACS_CS_ImCrmEquipment()
{
   type = CRM_EQUIPMENT_T;
}

ACS_CS_ImCrmEquipment::ACS_CS_ImCrmEquipment(const ACS_CS_ImCrmEquipment &other)
{
   *this = other;
}

ACS_CS_ImCrmEquipment::~ACS_CS_ImCrmEquipment()
{}

bool ACS_CS_ImCrmEquipment::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

	return true;
}

bool ACS_CS_ImCrmEquipment::modify(ACS_APGCC_AttrModification **attrMods)
{


   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CRM_EQUIPMENT_ID.c_str()) == 0)
      {
         equipmentId = (char *) modAttr->attrValues[0];
      }
      else              // Bad attributeName
        return false;
   }

   return true;
}

ACS_CS_ImBase * ACS_CS_ImCrmEquipment::clone() const
{
   ACS_CS_ImBase *crmEquipment = new ACS_CS_ImCrmEquipment(*this);
   return crmEquipment;
}


ACS_CS_ImCrmEquipment &ACS_CS_ImCrmEquipment::operator=(const ACS_CS_ImCrmEquipment &rhs)
{

   copyObj(rhs);
   return *this;
}



ACS_CS_ImCrmEquipment &ACS_CS_ImCrmEquipment::copyObj(const ACS_CS_ImCrmEquipment &object)
{

	action = object.action;
	type = object.type;
	rdn = object.rdn;
	equipmentId = object.equipmentId;

   return *this;
}


bool ACS_CS_ImCrmEquipment::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
       return false;

   try {
      const ACS_CS_ImCrmEquipment & other = dynamic_cast<const ACS_CS_ImCrmEquipment &>(obj);

      if (equipmentId != other.equipmentId)
         return false;

   }
   catch (const std::bad_cast &e)
   {
      std::cerr << e.what() << std::endl;
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCrmEquipment::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CRM_EQUIPMENT.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CRM_EQUIPMENT_ID.c_str(), equipmentId));

   immObject->setAttributeList(attrList);

   return immObject;

}


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCrmEquipment & crmEquipment, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(crmEquipment);
         ar & crmEquipment.equipmentId;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCrmEquipment & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCrmEquipment & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCrmEquipment & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCrmEquipment & base, const unsigned int /*version*/);
   }
}
