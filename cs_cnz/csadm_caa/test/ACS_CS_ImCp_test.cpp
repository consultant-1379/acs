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
 * @file ACS_CS_ImCp_test.cpp
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

#include "ACS_CS_ImCp.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include "ACS_CS_ImUtils.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(Cp_serialization, CpObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImCp.serialized";

   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImCp *obj2 = new ACS_CS_ImCp();

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


TEST(Cp_operatorcomparison, CpIfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImCp *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(Cp_operatorcomparison, CpIfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImCp *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();
   obj2->cpId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(Cp_operatorcomparison, CpIfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImCp *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(Cp_operatorcomparison, CpIfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createApObject();

   EXPECT_TRUE(*obj1 != *obj2);
}





TEST(Cp_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(Cp_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("Cp", immObject->getClassName());
}

TEST(Cp_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(Cp_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(Cp_ToImmObject, immObjectSizeIsThirteen)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 13);
}

TEST(Cp_ToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(Cp_ToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "cpId") == 0);
}

TEST(Cp_ToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "cpId=1") == 0);
}

TEST(Cp_ToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(Cp_ToImmObject, secondAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "systemIdentifier") == 0);
}

TEST(Cp_ToImmObject, secondAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   uint16_t *uint = reinterpret_cast<uint16_t *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(*uint == 234);
}

TEST(Cp_ToImmObject, secondAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImCp *obj1 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_UINT32T);
}








