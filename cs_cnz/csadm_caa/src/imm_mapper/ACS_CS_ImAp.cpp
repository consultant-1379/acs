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
 * @file ACS_CS_ImAp.cpp
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

#include "ACS_CS_ImAp.h"
//#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImUtils.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImAp);


ACS_CS_ImAp::ACS_CS_ImAp()
{
   type = AP_T;
}

ACS_CS_ImAp::ACS_CS_ImAp(const ACS_CS_ImAp &other)
{
	copyObj(other);
}

ACS_CS_ImAp::~ACS_CS_ImAp()
{}


bool ACS_CS_ImAp::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

	string apName = ACS_CS_ImUtils::getIdValueFromRdn(this->rdn);

	if( (apName.compare("AP1") != 0) && (apName.compare("AP2") != 0) ){
		errNo = TC_APNAMEFAULTY;
		return false;
	}

	return true;
}


bool ACS_CS_ImAp::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_APG_ID.c_str()) == 0)
      {
         axeApgId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_AP_BLADE_REF_TO.c_str()) == 0)
      {
			int len = modAttr->attrValuesNum;

			std::set<string>::iterator it=axeApBladeRefTo.begin();

//			for (; it != axeApBladeRefTo.end(); ++it) {
//				axeApBladeRefTo.erase(it);
//			}

			axeApBladeRefTo.clear();

			for (int t = 0; t < len; t++) {
				axeApBladeRefTo.insert(reinterpret_cast<char *> (modAttr->attrValues[t]));
			}
	  }
      else if (strcmp(modAttr->attrName, "entryId") == 0)
      {
         entryId = (*(uint16_t *) modAttr->attrValues[0]);
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImAp::clone() const
{
   ACS_CS_ImBase *ap = new ACS_CS_ImAp(*this);
   return ap;
}

bool ACS_CS_ImAp::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImAp & other = dynamic_cast<const ACS_CS_ImAp &>(obj);

      if (axeApgId != other.axeApgId)
         return false;
      if (axeApBladeRefTo != other.axeApBladeRefTo)
         return false;
      if (entryId != other.entryId)
         return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }

   return true;

}
ACS_CS_ImImmObject * ACS_CS_ImAp::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_APG.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(false == onlyModifiedAttrs)
	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_APG_ID.c_str(), axeApgId));
   attrList.push_back(ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_AP_BLADE_REF_TO.c_str(), axeApBladeRefTo));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("entryId", entryId));

   immObject->setAttributeList(attrList);


   return immObject;

}

void ACS_CS_ImAp::copyObj(const ACS_CS_ImAp &object)
{
	axeApgId = object.axeApgId;
	rdn = object.rdn;
	type = object.type;
	action = object.action;
	entryId = object.entryId;
	axeApBladeRefTo=object.axeApBladeRefTo;

	//std::set<string> elements;
	//elements=getAxeApBladeRefToElements();
	//axeApBladeRefTo=elements;
}


int ACS_CS_ImAp::getAxeApBladeRefToNumberOfElements()
{
	return axeApBladeRefTo.size();
}

void ACS_CS_ImAp::addAxeApBladeRefToElement(std::string apBladeRefTo)
{
	axeApBladeRefTo.insert(apBladeRefTo);
}

std::set<string> ACS_CS_ImAp::getAxeApBladeRefToElements()
{
	return axeApBladeRefTo;
}


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImAp & ap, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(ap);
         ar & ap.axeApgId;
         ar & ap.entryId;
         ar & ap.axeApBladeRefTo;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImAp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImAp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImAp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImAp & base, const unsigned int /*version*/);
   }
}
