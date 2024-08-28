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
 * @file ACS_CS_ImOtherBlade_test.cpp
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

#include "ACS_CS_ImOtherBlade.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include "ACS_CS_ImUtils.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(OtherBlade_serialization, ObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImOtherBlade.serialized";

   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImOtherBlade *obj2 = new ACS_CS_ImOtherBlade();

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


TEST(OtherBlade_operatorcomparison, IfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImOtherBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(OtherBlade_operatorcomparison, IfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImOtherBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   obj2->otherBladeId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(OtherBlade_operatorcomparison, IfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImOtherBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(OtherBlade_operatorcomparison, IfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}



TEST(OtherBlade_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(OtherBlade_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("OtherBlade", immObject->getClassName());
}

TEST(OtherBlade_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(OtherBlade_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(OtherBlade_ToImmObject, immObjectSizeIsNineteen)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_EQ(19, immObject->getAttributes().size());
}

TEST(OtherBlade_ToImmObject, nineteenthAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[18];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(OtherBlade_ToImmObject, seventeenthAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];

   // Assert
   EXPECT_STREQ("otherBladeId", attr.getAttrName());
}

TEST(OtherBlade_ToImmObject, seventeenthAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_STREQ("otherBladeId=1", apName);
}

TEST(OtherBlade_ToImmObject, nineteenthAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImOtherBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[18];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


















