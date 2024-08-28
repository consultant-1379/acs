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
 * @file ACS_CS_ImApServiceCategoryServiceCategory.cpp
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

#include "ACS_CS_ImApServiceCategory.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(ApServiceCategory_serialization, ObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImApServiceCategoryServiceCategory.serialized";

   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImApServiceCategory *obj2 = new ACS_CS_ImApServiceCategory();

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

TEST(ApServiceCategory_operatorcomparison, IfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImApServiceCategory *obj2 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(ApServiceCategory_operatorcomparison, IfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImApServiceCategory *obj2 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   obj2->apServiceCategoryId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(ApServiceCategory_operatorcomparison, IfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImApServiceCategory *obj2 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(ApServiceCategory_operatorcomparison, ApServiceCategoryIfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}




TEST(ApServiceCategory_ApServiceCategoryToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(ApServiceCategory_ApServiceCategoryToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("ApServiceCategory", immObject->getClassName());
}

TEST(ApServiceCategory_ApServiceCategoryToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(ApServiceCategory_ApServiceCategoryToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(ApServiceCategory_ApServiceCategoryToImmObject, immObjectSizeIsOne)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 1);
}

TEST(ApServiceCategory_ApServiceCategoryToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(ApServiceCategory_ApServiceCategoryToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "apServiceCategoryId") == 0);
}

TEST(ApServiceCategory_ApServiceCategoryToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "apServiceCategoryId=1") == 0);
}

TEST(ApServiceCategory_ApServiceCategoryToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImApServiceCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


