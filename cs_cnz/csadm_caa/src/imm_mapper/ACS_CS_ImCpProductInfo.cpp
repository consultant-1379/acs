/*
 * ACS_CS_ImCpProductInfo.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: eanform
 */


#include "ACS_CS_ImCpProductInfo.h"
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

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCpProductInfo);

ACS_CS_ImCpProductInfo::ACS_CS_ImCpProductInfo():
cpProductInfoId(""),
productRevision(""),
productName(""),
manufacturingDate(""),
productVendor(""),
productNumber(""),
serialNumber("")
{
   type = CPPRODUCTINFO_T;
}



ACS_CS_ImCpProductInfo::ACS_CS_ImCpProductInfo(const ACS_CS_ImCpProductInfo &other)
{
   *this = other;
}

ACS_CS_ImCpProductInfo::~ACS_CS_ImCpProductInfo()
{}


bool ACS_CS_ImCpProductInfo::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImCpProductInfo::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CP_PRODUCT_INFO_ID.c_str()) == 0)
      {
    	  cpProductInfoId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "productRevision") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  productRevision = (char *) modAttr->attrValues[0];
    	  else
    		  productRevision = '\0';
      }
      else if (strcmp(modAttr->attrName, "productName") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  productName = (char *) modAttr->attrValues[0];
    	  else
    		  productName = '\0';
      }
      else if (strcmp(modAttr->attrName, "manufacturingDate") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  manufacturingDate = (char *) modAttr->attrValues[0];
    	  else
    		  manufacturingDate = '\0';
      }
      else if (strcmp(modAttr->attrName, "productVendor") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  productVendor = (char *) modAttr->attrValues[0];
    	  else
    		  productVendor = '\0';
      }
      else if (strcmp(modAttr->attrName, "productNumber") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  productNumber = (char *) modAttr->attrValues[0];
    	  else
    		  productNumber = '\0';
      }
      else if (strcmp(modAttr->attrName, "serialNumber") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  serialNumber = (char *) modAttr->attrValues[0];
    	  else
    		  serialNumber = '\0';
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImCpProductInfo::clone() const
{
   ACS_CS_ImBase *prodInfo = new ACS_CS_ImCpProductInfo(*this);
   return prodInfo;
}


ACS_CS_ImCpProductInfo & ACS_CS_ImCpProductInfo::operator=(const ACS_CS_ImCpProductInfo &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImCpProductInfo & ACS_CS_ImCpProductInfo::copyObj(const ACS_CS_ImCpProductInfo &object)
{
	action = 				object.action;
	type = 					object.type;
	rdn = 					object.rdn;
	cpProductInfoId =		object.cpProductInfoId;
	productRevision = 		object.productRevision;
	productName = 			object.productName;
	manufacturingDate = 	object.manufacturingDate;
	productVendor = 		object.productVendor;
	productNumber = 		object.productNumber;
	serialNumber = 			object.serialNumber;

   return *this;
}


bool ACS_CS_ImCpProductInfo::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImCpProductInfo & other = dynamic_cast<const ACS_CS_ImCpProductInfo &>(obj);

      if (cpProductInfoId != other.cpProductInfoId)
    	  return false;
      if (productRevision != other.productRevision)
    	  return false;
      if (productName != other.productName)
    	  return false;
      if (manufacturingDate != other.manufacturingDate)
    	  return false;
      if (productVendor != other.productVendor)
    	  return false;
      if (productNumber != other.productNumber)
    	  return false;
      if (serialNumber != other.serialNumber)
    	  return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCpProductInfo::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CP_PRODUCT_INFO.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CP_PRODUCT_INFO_ID.c_str(), cpProductInfoId));

   if (strlen(productRevision.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("productRevision", productRevision));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("productRevision"));

   if (strlen(productName.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("productName", productName));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("productName"));

   if (strlen(manufacturingDate.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("manufacturingDate", manufacturingDate));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("manufacturingDate"));

   if (strlen(productVendor.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("productVendor", productVendor));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("productVendor"));

   if (strlen(productNumber.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("productNumber", productNumber));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("productNumber"));

   if (strlen(serialNumber.c_str()) > 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("serialNumber", serialNumber));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("serialNumber"));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpProductInfo & prodInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(prodInfo);
         ar & prodInfo.cpProductInfoId;
         ar & prodInfo.productRevision;
         ar & prodInfo.productName;
         ar & prodInfo.manufacturingDate;
         ar & prodInfo.productVendor;
         ar & prodInfo.productNumber;
         ar & prodInfo.serialNumber;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCpProductInfo & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCpProductInfo & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCpProductInfo & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCpProductInfo & base, const unsigned int /*version*/);
   }
}
