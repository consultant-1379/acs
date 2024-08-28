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
 * @file ACS_CS_ImCpGroupCategory_test.cpp
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

#include "ACS_CS_ImUtils.h"

#include "ACS_CS_ImCpGroupCategory.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(CpGroupCategory_serialization, CpGroupCategoryObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImCpGroupCategory.serialized";

   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImCpGroupCategory *obj2 = new ACS_CS_ImCpGroupCategory();

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

TEST(CpGroupCategory_operatorcomparison, CpGroupCategoryIfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImCpGroupCategory *obj2 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(CpGroupCategory_operatorcomparison, CpGroupCategoryIfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImCpGroupCategory *obj2 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   obj2->cpGroupCategoryId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(CpGroupCategory_operatorcomparison, CpGroupCategoryIfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImCpGroupCategory *obj2 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(CpGroupCategory_operatorcomparison, CpGroupCategoryIfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(CpGroupCategory_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(CpGroupCategory_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("CpGroupCategory", immObject->getClassName());
}

TEST(CpGroupCategory_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(CpGroupCategory_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(CpGroupCategory_ToImmObject, immObjectSizeIsOne)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 1);
}

TEST(CpGroupCategory_ToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(CpGroupCategory_ToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "cpGroupCategoryId") == 0);
}

TEST(CpGroupCategory_ToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "cpGroupCategoryId=1") == 0);
}

TEST(CpGroupCategory_ToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImCpGroupCategory *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}

