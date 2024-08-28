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
 * @file ACS_CS_ImApService.cpp
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

#include "ACS_CS_ImApService.h"
#include "ACS_CS_ImModel.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImApService);

ACS_CS_ImApService::ACS_CS_ImApService()
{
   type = APSERVICE_T;
}

ACS_CS_ImApService::ACS_CS_ImApService(const ACS_CS_ImApService &other)
{
   *this = other;
}

ACS_CS_ImApService::~ACS_CS_ImApService()
{}

bool ACS_CS_ImApService::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
	errNo = 0;

	//Check if this ApService is associated with existing Ap object
//	if(model.getObject(apRdn.c_str()) == NULL)
//	{
//		return false;
//	}

	return true;
}

bool ACS_CS_ImApService::modify(ACS_APGCC_AttrModification **attrMods)
{

	for (int i = 0; attrMods[i] != NULL; i++ )
	{
		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, "serviceName") == 0)
		{
			serviceName = (char *) modAttr->attrValues[0];
		}
		else if (strcmp(modAttr->attrName, "domainName") == 0)
		{
			domainName = (char *) modAttr->attrValues[0];
		}
		else if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_AP_SERVICE_ID.c_str()) == 0)
		{
			apServiceId = (char *) modAttr->attrValues[0];
		}
		else if (strcmp(modAttr->attrName, "apList") == 0)
		{
			apRdns.clear();

			for (uint32_t j = 0; j < modAttr->attrValuesNum; j++)
			{
				apRdns.insert(reinterpret_cast<char *>(modAttr->attrValues[j]));
			}
		}
		else              // Bad attributeName
			return false;
	}

	return true;
}

ACS_CS_ImBase * ACS_CS_ImApService::clone() const
{
   ACS_CS_ImBase *apService = new ACS_CS_ImApService(*this);
   return apService;
}


ACS_CS_ImApService & ACS_CS_ImApService::operator=(const ACS_CS_ImApService &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImApService & ACS_CS_ImApService::copyObj(const ACS_CS_ImApService &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	serviceName = object.serviceName;
	domainName = object.domainName;
	apServiceId = object.apServiceId;
	apRdns = object.apRdns;
	aps.clear();

	return *this;
}

bool ACS_CS_ImApService::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImApService & other = dynamic_cast<const ACS_CS_ImApService &>(obj);

      if (serviceName != other.serviceName)
         return false;
      if (domainName != other.domainName)
         return false;
      if (apServiceId != other.apServiceId)
         return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }

   return true;

}



ACS_CS_ImImmObject * ACS_CS_ImApService::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_AP_SERVICE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;
   attrList.push_back(ACS_CS_ImUtils::createStringType("serviceName", serviceName));
   attrList.push_back(ACS_CS_ImUtils::createStringType("domainName", domainName));
   if(false == onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_AP_SERVICE_ID.c_str(), apServiceId));
   attrList.push_back(ACS_CS_ImUtils::createNameType("apList", apRdns));

   immObject->setAttributeList(attrList);

   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImApService & apService, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(apService);
         ar & apService.serviceName;
         ar & apService.domainName;
         ar & apService.apServiceId;
         ar & apService.apRdns;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImApService & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImApService & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImApService & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImApService & base, const unsigned int /*version*/);

   }
}
