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
 * @file ACS_CS_ImApBlade.cpp
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

#include "ACS_CS_ImApBlade.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImValuesDefinitionType.h"

#include <boost/serialization/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

#include <cstring>
#include <vector>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImApBlade);

ACS_CS_ImApBlade::ACS_CS_ImApBlade()
   // : ap(0)
{
   type = APBLADE_T;
}

ACS_CS_ImApBlade::ACS_CS_ImApBlade(const ACS_CS_ImApBlade &other)
   : ACS_CS_ImBlade(other)
{
   *this = other;
}


ACS_CS_ImApBlade::~ACS_CS_ImApBlade()
{}

bool ACS_CS_ImApBlade::isValid(int & errNo, const ACS_CS_ImModel &model)
{
	errNo = 0;

	// Systemtype AP
	if(AP != systemType)
	{
		errNo = TC_FAULTYSYSTEMTYPE;
		return false;
	}

	// Valid FBN
	switch(functionalBoardName)
	{
	case APUB: break; //Valid ApBlade FBN
	case DISK: break; //Valid ApBlade FBN
	case DVD:  break; //Valid ApBlade FBN
	case GEA:  break; //Valid ApBlade FBN
	default:
		errNo = TC_INVALID_FBN;
		return false;
	}

	// Valid slot
	if(slotNumber > 25)
	{
		errNo = TC_SLOTOUTOFRANGE;
		return false;
	}

	// AP1 or AP2
	if(systemNumber != 1 && systemNumber != 2)
	{
		ACS_CS_ImUtils::setFaultyAttribute(systemNumber);
		errNo = TC_SYSTEMNOTOBIG;
		return false;
	}

	// Disks should be assigned to a side
	if(functionalBoardName == DISK && side == UNDEF_SIDE)
	{
		errNo = TC_SIDENOTSPECIFIED;
		return false;
	}

	// APUBs should be assigned to a side
	if(functionalBoardName == APUB && side == UNDEF_SIDE)
	{
		errNo = TC_SIDENOTSPECIFIED;
		return false;
	}

	// No APUB or DISK allowed with same sysNo and side
	if (functionalBoardName == APUB || functionalBoardName == DISK) {
		std::set<const ACS_CS_ImBase *> apBlades;
		std::set<const ACS_CS_ImBase *>::iterator it;

		model.getObjects(apBlades, APBLADE_T);

		for (it = apBlades.begin(); it != apBlades.end(); it++) {
			const ACS_CS_ImApBlade* apBlade = dynamic_cast<const ACS_CS_ImApBlade*>(*it);

			if (apBlade && apBlade->functionalBoardName == this->functionalBoardName && apBlade->rdn != this->rdn) {
				if (apBlade->systemNumber == this->systemNumber && apBlade->side == this->side) {
					errNo = (functionalBoardName == APUB)? TC_APUBALREADYPRESENT: TC_DISKALREADYPRESENT;
					return false;
				}
			}
		}
	}

	// Only GESB boards can be put in a Shelf with address plugs set to 255
	size_t found;
	found = rdn.find("255");
	if (found != string::npos ){
		ACS_CS_ImUtils::setFaultyFbn(functionalBoardName);
		errNo = TC_INCORRECTMAG;
		return false;
	}

	return ACS_CS_ImBlade::isValid(errNo, model);;
}

bool ACS_CS_ImApBlade::modify(ACS_APGCC_AttrModification **attrMods)
{
   bool baseModded = ACS_CS_ImBlade::modify(attrMods);
   bool selfModded = true;

   for (int i = 0; attrMods[i] != NULL; i++)
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_AP_BLADE_ID.c_str()) == 0)
      {
         axeApBladeId = (char *) modAttr->attrValues[0];
      }
//      else if (strcmp(modAttr->attrName, "ap") == 0)
//      {
//         apRdn = (char *) modAttr->attrValues[0];
//      }
      else              // Bad attributeName
         selfModded = false;
   }

   return (baseModded || selfModded);
}

ACS_CS_ImBase * ACS_CS_ImApBlade::ACS_CS_ImApBlade::clone() const
{
   ACS_CS_ImBase *apBlade = new ACS_CS_ImApBlade(*this);
   return apBlade;
}


ACS_CS_ImApBlade & ACS_CS_ImApBlade::operator=(const ACS_CS_ImApBlade &rhs)
{
   ACS_CS_ImBlade::operator=(rhs);
   copyObj(rhs);
   return *this;

}


ACS_CS_ImApBlade & ACS_CS_ImApBlade::copyObj(const ACS_CS_ImApBlade &object)
{
   ACS_CS_ImBlade::copyObj(object);
	axeApBladeId = object.axeApBladeId;
//	apRdn = object.apRdn;
//	ap = NULL;
	return *this;
}


bool ACS_CS_ImApBlade::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBlade::equals(obj))
      return false;

   try {
      const ACS_CS_ImApBlade & other = dynamic_cast<const ACS_CS_ImApBlade &>(obj);

      if (axeApBladeId != other.axeApBladeId)
         return false;
//      if (apRdn != other.apRdn)
//         return false;
      if (hwVersion != other.hwVersion)
    	  return false;

   }
   catch (const std::bad_cast &e)
   {
      return false;
   }

   return true;

}



ACS_CS_ImImmObject * ACS_CS_ImApBlade::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_AP_BLADE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList = ACS_CS_ImBlade::getApBladeAttributes();

   if(false == onlyModifiedAttrs)
	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_AP_BLADE_ID.c_str(), axeApBladeId));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImApBlade & apBlade, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBlade>(apBlade);
         ar & apBlade.axeApBladeId;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImApBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImApBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImApBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImApBlade & base, const unsigned int /*version*/);
   }
}
