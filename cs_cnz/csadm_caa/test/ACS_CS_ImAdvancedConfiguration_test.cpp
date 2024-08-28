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
 * @file ACS_CS_ImAdvancedConfiguration_test.cpp
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

#include "ACS_CS_ImAdvancedConfiguration.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImImmObject.h"
#include "ACS_CS_ImUtils.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "ACS_CS_ImDefaultObjectCreator.h"


TEST(AdvancedConfiguration_serialization, AdvancedConfigurationObjectsAreSerializedCorrectly)
{
   ACS_CS_ImAdvancedConfiguration *ac1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImAdvancedConfiguration *ac2 = new ACS_CS_ImAdvancedConfiguration();

   ofstream out("ACS_CS_ImAdvancedConfiguration.serialized");

   if (!out)
      FAIL();

   {
      boost::archive::text_oarchive oa(out);
      oa << *ac1;
   }

   ifstream in("ACS_CS_ImAdvancedConfiguration.serialized");
   if (!in)
      FAIL();

   {
      boost::archive::text_iarchive ia(in);
      ia >> *ac2;
   }

   EXPECT_TRUE(*ac2 == *ac2);

}



TEST(AdvancedConfiguration_operatorcomparison, AdvancedConfigurationIfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImAdvancedConfiguration *obj2 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(AdvancedConfiguration_operatorcomparison, AdvancedConfigurationIfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImAdvancedConfiguration *obj2 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   obj2->advancedConfigurationId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(AdvancedConfiguration_operatorcomparison, AdvancedConfigurationIfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImAdvancedConfiguration *obj2 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(AdvancedConfiguration_operatorcomparison, AdvancedConfigurationIfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(AdvancedConfiguration_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(AdvancedConfiguration_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("AdvancedConfiguration", immObject->getClassName());
}

TEST(AdvancedConfiguration_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   string parentName = ACS_CS_ImUtils::getParentName(obj1).c_str();


   // Assert
   EXPECT_TRUE(parentName.compare(immObject->getParentName()) == 0);
}


TEST(AdvancedConfiguration_ToImmObject, immObjectSizeIs25)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject->getAttributes().size() == 25);
}

TEST(AdvancedConfiguration_ToImmObject, firstAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(AdvancedConfiguration_ToImmObject, firstAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(strcmp(attr.getAttrName(), "omProfileSupervisionPhase") == 0);
}

TEST(AdvancedConfiguration_ToImmObject, firstAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   int *intvalue = reinterpret_cast<int *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_EQ(UNDEF_OMPROFILEPHASE, *intvalue);
}

TEST(AdvancedConfiguration_ToImmObject, firstAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImAdvancedConfiguration *obj1 = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[0];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_INT32T);
}






