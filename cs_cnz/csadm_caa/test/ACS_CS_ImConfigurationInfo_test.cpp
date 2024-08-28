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
 * @file ACS_CS_ImConfigurationInfo.cpp
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

#include "ACS_CS_ImConfigurationInfo.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImDefaultObjectCreator.h"

#include "ACS_CS_ImUtils.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TEST(ConfigurationInfo_serialization, ConfigurationInfoObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImConfigurationInfo.serialized";

   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImConfigurationInfo *obj2 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

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


//-----------------------------------------------------------------------------
TEST(ConfigurationInfo_cloneTest, imConfigurationInfo)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *info = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   info->isBladeClusterSystem = true;

   // Act
   ACS_CS_ImConfigurationInfo *newInfo = dynamic_cast<ACS_CS_ImConfigurationInfo*>(info->clone());

   // Assert
   EXPECT_NE(info, newInfo) << "Should not be same reference";
   EXPECT_TRUE(newInfo->isBladeClusterSystem == true);

   delete newInfo;
   delete info;
}

TEST(ConfigurationInfo_operatorcomparison, ConfigurationInfoIfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImConfigurationInfo *obj2 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(ConfigurationInfo_operatorcomparison, ConfigurationInfoIfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImConfigurationInfo *obj2 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   obj2->configurationInfoId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(ConfigurationInfo_operatorcomparison, ConfigurationInfoIfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImConfigurationInfo *obj2 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(ConfigurationInfo_operatorcomparison, ConfigurationInfoIfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(ConfigurationInfo_ConfigurationInfoToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(ConfigurationInfo_ConfigurationInfoToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("ConfigurationInfo", immObject->getClassName());
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, immObjectSizeIsTwo)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_EQ(2, immObject->getAttributes().size());
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "configurationInfoId") == 0);
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   const char *pchar = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_STREQ("configurationInfoId=1", pchar);
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(ConfigurationInfo_ConfigurationInfoToImmObject, secondAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "isBladeClusterSystem") == 0);
}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, secondAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   bool *boolVal = reinterpret_cast<bool *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_FALSE(*boolVal);

}

TEST(ConfigurationInfo_ConfigurationInfoToImmObject, secondAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImConfigurationInfo *obj1 = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[1];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_INT32T);
}












