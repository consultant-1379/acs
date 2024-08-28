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
 * @file ACS_CS_ImVlan_test.cpp
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

#include "ACS_CS_ImVlan.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(Vlan_serialization, ObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImVlan.serialized";

   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImVlan *obj2 = new ACS_CS_ImVlan();

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


TEST(Vlan_operatorcomparison, IfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImVlan *obj2 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(Vlan_operatorcomparison, IfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImVlan *obj2 = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   obj2->vlanId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(Vlan_operatorcomparison, IfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImVlan *obj2 = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(Vlan_operatorcomparison, IfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}












TEST(Vlan_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(Vlan_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("Vlan", immObject->getClassName());
}

TEST(Vlan_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(Vlan_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(Vlan_ToImmObject, immObjectSizeIsFive)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 5);
}

TEST(Vlan_ToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(Vlan_ToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "vlanId") == 0);
}

TEST(Vlan_ToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(apName, "vlanId=1") == 0);
}

TEST(Vlan_ToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(Vlan_ToImmObject, secondAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "networkAddress") == 0);
}

TEST(Vlan_ToImmObject, secondAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   const char *str = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(str,  "1.2.0.99") == 0);
}

TEST(Vlan_ToImmObject, secondAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImVlan *obj1 = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}



