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
 * @file ACS_CS_ImCpGroup.cpp
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

#include "ACS_CS_ImCpGroup.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(CpGroup_serialization, CpGroupObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImCpGroup.serialized";

   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImCpGroup *obj2 = new ACS_CS_ImCpGroup();

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


TEST(CpGroup_operatorcomparison, IfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImCpGroup *obj2 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(CpGroup_operatorcomparison, IfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImCpGroup *obj2 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   obj2->cpGroupId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(CpGroup_operatorcomparison, IfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImCpGroup *obj2 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(CpGroup_operatorcomparison, IfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(CpGroup_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(CpGroup_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("CpGroup", immObject->getClassName());
}

TEST(CpGroup_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(CpGroup_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(CpGroup_ToImmObject, immObjectSizeIsThree)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 3);
}

TEST(CpGroup_ToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(CpGroup_ToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "cpGroupId") == 0);
}

TEST(CpGroup_ToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "cpGroupId=1") == 0);
}

TEST(CpGroup_ToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(CpGroup_ToImmObject, secondAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "groupName") == 0);
}

TEST(CpGroup_ToImmObject, secondAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   const char *str = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(str, "GROUPNAME") == 0);
}

TEST(CpGroup_ToImmObject, secondAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImCpGroup *obj1 = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}





