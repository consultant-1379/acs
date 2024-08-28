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
 * @file ACS_CS_ImAp_test.cpp
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

#include "ACS_CS_ImAp.h"
#include "ACS_CS_ImTypes.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImUtils.h"
#include <cstring>



TEST(Ap_serialization, ApObjectsAreSerializedCorrectly)
{

   ACS_CS_ImAp *ap1  = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImAp *ap2 = new ACS_CS_ImAp();

   ofstream out("ACS_CS_ImAp.serialized");

   if (!out)
      FAIL();

   {
      boost::archive::text_oarchive oa(out);
      oa << *ap1;
   }

   ifstream in("ACS_CS_ImAp.serialized");
   if (!in)
      FAIL();

   {
      boost::archive::text_iarchive ia(in);
      ia >> *ap2;
   }


   EXPECT_TRUE(*ap1 == *ap2);

}



TEST(Ap_Operatorcomparison, ApIfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImAp *obj2 = ACS_CS_ImDefaultObjectCreator::createApObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(Ap_Operatorcomparison, ApIfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImAp *obj2 = ACS_CS_ImDefaultObjectCreator::createApObject();
   obj2->apId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(Ap_Operatorcomparison, ApIfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImAp *obj2 = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(Ap_Operatorcomparison, ApIfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}

TEST(Ap_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(Ap_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("Ap", immObject->getClassName());
}

TEST(Ap_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(Ap_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(Ap_ToImmObject, immObjectSizeIsTThree)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_EQ(3, immObject->getAttributes().size());
}

TEST(Ap_ToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(Ap_ToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "apId") == 0);
}

TEST(Ap_ToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "apId=1") == 0);
}

TEST(Ap_ToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(Ap_ToImmObject, secondAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "name") == 0);
}

TEST(Ap_ToImmObject, secondAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   ApNameEnum *apNameEnum = reinterpret_cast<ApNameEnum *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(*apNameEnum == AP1);
}

TEST(Ap_ToImmObject, secondAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImAp *obj1 = ACS_CS_ImDefaultObjectCreator::createApObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_INT32T);
}



