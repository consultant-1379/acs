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
 * @file ACS_CS_ImApService_test.cpp
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

#include <iostream>
#include <fstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImApCategory.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImUtils.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(ApService_serialization, ApServiceObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImApService.serialized";

   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImApService *obj2 = new ACS_CS_ImApService();

   ofstream out(fileName);

   if (!out)
      FAIL();

   {
      boost::archive::text_oarchive oa(out);
      oa << *obj1;
   }

   ifstream in(fileName);
   if (!in)
      FAIL();

   {
      boost::archive::text_iarchive ia(in);
      ia >> *obj2;
   }

   EXPECT_TRUE(*obj1 == *obj2);

}

TEST(ApService_operatorcomparison, ApServiceIfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImApService *obj2 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(ApService_operatorcomparison, ApServiceIfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImApService *obj2 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   obj2->apServiceId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(ApService_operatorcomparison, ApServiceIfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImApService *obj2 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(ApService_operatorcomparison, ApServiceIfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(ApService_ApServiceToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(ApService_ApServiceToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("ApService", immObject->getClassName());
}

TEST(ApService_ApServiceToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(ApService_ApServiceToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(ApService_ApServiceToImmObject, immObjectSizeIsFour)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 4);
}

TEST(ApService_ApServiceToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(ApService_ApServiceToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "serviceName") == 0);
}

TEST(ApService_ApServiceToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "APSERVICE") == 0);
}

TEST(ApService_ApServiceToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(ApService_ApServiceToImmObject, secondAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "domainName") == 0);
}

TEST(ApService_ApServiceToImmObject, secondAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   const char *str = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(str, "ROCKERS") == 0);
}

TEST(ApService_ApServiceToImmObject, secondAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImApService *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}





