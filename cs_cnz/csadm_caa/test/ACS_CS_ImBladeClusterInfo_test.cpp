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
 * @file ACS_CS_ImBladeClusterInfo.cpp
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

#include "ACS_CS_ImBladeClusterInfo.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImCp.h"

#include "ACS_CS_ImUtils.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(BladeClusterInfo_serialization, BladeClusterInfoObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImBladeClusterInfo.serialized";

   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImBladeClusterInfo *obj2 = new ACS_CS_ImBladeClusterInfo();

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


TEST(BladeClusterInfo_operatorcomparison, ifTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImBladeClusterInfo *obj2 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(BladeClusterInfo_operatorcomparison, ifTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImBladeClusterInfo *obj2 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   obj2->clockMaster=1;

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(BladeClusterInfo_operatorcomparison, ifTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImBladeClusterInfo *obj2 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(BladeClusterInfo_operatorcomparison, ifTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}











TEST(BladeClusterInfo_BladeClusterInfoToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(BladeClusterInfo_BladeClusterInfoToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("BladeClusterInfo", immObject->getClassName());
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, immObjectSizeIsNine)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 9);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "bladeClusterInfoId") == 0);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_STREQ("bladeClusterInfoId=1234", apName);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(BladeClusterInfo_BladeClusterInfoToImmObject, secondAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "alarmMaster") == 0);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, secondAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   uint16_t *iPtr= reinterpret_cast<uint16_t *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_EQ(1, *iPtr);
}

TEST(BladeClusterInfo_BladeClusterInfoToImmObject, secondAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImBladeClusterInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_UINT32T);
}



