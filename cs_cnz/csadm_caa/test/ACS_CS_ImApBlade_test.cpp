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
 * @file ACS_CS_ImApBlade_test.cpp
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

#include "ACS_CS_ImApBlade.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include "ACS_CS_ImUtils.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(ApBlade_serialization, ObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImApBlade.serialized";

   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImApBlade *obj2 = new ACS_CS_ImApBlade();

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

TEST(ApBlade_operatorcomparison, IfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImApBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   ACS_CS_ImBase *base = obj1;

   EXPECT_TRUE(*obj1 == *obj2);
}


TEST(ApBlade_operatorcomparison, DynamicCastIsNotNull)
{
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImApBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   ACS_CS_ImBase *base = obj1;

   ACS_CS_ImApBlade *olle = dynamic_cast<ACS_CS_ImApBlade *>(base);

   EXPECT_TRUE(olle != NULL);
}


TEST(ApBlade_operatorcomparison, IfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImApBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   obj2->apBladeId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(ApBlade_operatorcomparison, IfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImApBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(ApBlade_operatorcomparison, IfTwoObjectsEqualReturnTrueOneObjectIsBladeObject)
{
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImApBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImBlade *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}



TEST(ApBlade_operatorcomparison, IfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}



TEST(ApBlade_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(ApBlade_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("ApBlade", immObject->getClassName());
}

TEST(ApBlade_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(ApBlade_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(ApBlade_ToImmObject, immObjectSizeIsEighteen)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_EQ(18, immObject->getAttributes().size());
}

TEST(ApBlade_ToImmObject, fiftheenthAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[14];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(ApBlade_ToImmObject, seveneenthAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "apBladeId") == 0);
}

TEST(ApBlade_ToImmObject, seveneenthAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "apBladeId=1") == 0);
}

TEST(ApBlade_ToImmObject, seveneenthAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(ApBlade_ToImmObject, eighteenthAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[17];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "ap") == 0);
}

TEST(ApBlade_ToImmObject, eighteenthAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[17];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "apId=1,apCategoryId=1,configurationInfoId=1") == 0);
}

TEST(ApBlade_ToImmObject, eighteenthAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImApBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[17];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_NAMET);
}










