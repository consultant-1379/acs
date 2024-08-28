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
 * @file ACS_CS_ImCpBlade_test.cpp
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

#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImCp.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImModel.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>


TEST(CpBlade_serialization, ObjectsAreSerializedCorrectly)
{
   const char * fileName = "ACS_CS_ImCpBlade.serialized";

   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImCpBlade *obj2 = new ACS_CS_ImCpBlade();

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


TEST(CpBlade_operatorcomparison, IfTwoObjectsAreEqualReturnTrue)
{
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImCpBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   EXPECT_TRUE(*obj1 == *obj2);
}

TEST(CpBlade_operatorcomparison, IfTwoObjectsAreNotEqualReturnFalse)
{
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImCpBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   obj2->cpBladeId="Weird string";

   EXPECT_TRUE(*obj1 != *obj2);
}


TEST(CpBlade_operatorcomparison, IfTwoObjectsEqualReturnTrueOneObjectIsBaseObject)
{
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImCpBlade *obj2 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImBase *pObj2 = obj2;

   EXPECT_TRUE(*obj1 == *pObj2);
}

TEST(CpBlade_operatorcomparison, IfTwoObjectsAreNotSameTypeReturnFalse)
{
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   ACS_CS_ImBase *obj2 = ACS_CS_ImDefaultObjectCreator::createCpObject();

   EXPECT_TRUE(*obj1 != *obj2);
}

TEST(validation, CpBladeIfSystemNumberGreaterThan999ReturnFalse)
{
	ACS_CS_ImModel model;
	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	int error;
	bool result;
	blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
	blade->entryId = 1;

	blade->systemNumber = 1000;
	result = blade->isValid(error, model);

	EXPECT_FALSE(result);
}

TEST(validation, CpBladeIfSingleCpAndSequenceNumberLessThan64ReturnTrue)
{
	ACS_CS_ImModel model;
	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	ACS_CS_ImCp *cp = new ACS_CS_ImCp();
	cp->cpId="cpId=1";
	cp->rdn="cpId=1,cpCategoryId=1,configurationInfoId=1";
	cp->defaultName = "BC42";

	int error;
	bool result;
	blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
	blade->entryId = 1;
	blade->systemType = SINGLE_SIDED_CP;
	blade->cpRdn="cpId=1,cpCategoryId=1,configurationInfoId=1";
	blade->sequenceNumber = 42;
	model.addObject(cp);
	model.addObject(blade);
	result = blade->isValid(error, model);

	EXPECT_TRUE(result);
}


TEST(validation, CpBladeIfSystemNumberLargerThan64ReturnFalse)
{
   ACS_CS_ImModel model;
   ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
   ACS_CS_ImCp *cp = new ACS_CS_ImCp();
   cp->cpId="cpId=1";
   cp->rdn="cpId=1,cpCategoryId=1,configurationInfoId=1";
   cp->defaultName = "BC67";

   int error;
   bool result;
   blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
   blade->entryId = 1;
   blade->cpRdn="cpId=1,cpCategoryId=1,configurationInfoId=1";

   blade->systemNumber = 67;
   model.addObject(cp);
   model.addObject(blade);
   result = blade->isValid(error, model);

   EXPECT_FALSE(result);
}


TEST(validation, CpBladeIfAliasAddressHasWrongFormatReturnFalse)
{
	ACS_CS_ImModel model;
	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	int error;
	bool result;
	blade->systemNumber = 999;
	blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
	blade->entryId = 1;

	blade->ipAliasEthA = "1.289.25.2";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->ipAliasEthA = "1.255.25.2.3";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->ipAliasEthA = "1.255.25.2.";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->ipAliasEthA = ".255.25.2";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->ipAliasEthA = "";

	blade->ipAliasEthB = "1.289.25.2";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->ipAliasEthB = "1.255.25.2.3";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->ipAliasEthB = "1.255.25.2.";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->ipAliasEthB = ".255.25.2";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);
}

TEST(validation, CpBladeIfAliasAddressHasCorrectFormatReturnTrue)
{
	ACS_CS_ImModel model;
   ACS_CS_ImCp *cp = new ACS_CS_ImCp();
   cp->cpId="cpId=1";
   cp->rdn="cpId=1,cpCategoryId=1,configurationInfoId=1";
   cp->defaultName = "BC42";

   ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
   blade->cpRdn="cpId=1,cpCategoryId=1,configurationInfoId=1";

	int error;
	bool result;
	blade->systemNumber = 999;
	blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
	blade->entryId = 1;

	blade->ipAliasEthA = "1.255.25.2";
   model.addObject(blade);
   model.addObject(cp);

	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->ipAliasEthB = "1.255.25.2";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);
}

TEST(validation, CpBladeIfNetmaskHasWrongFormatReturnFalse)
{
	ACS_CS_ImModel model;
	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
	int error;
	bool result;
	blade->systemNumber = 999;
	blade->functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
	blade->entryId = 1;

	blade->aliasNetmaskEthA = "1.289.25.2";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthA = "255.255.255.8";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthA = "255.255.255.0.0";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthA = "255.255.255.0.";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthA = ".255.255.255.0";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthA = "0.255.255.255";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthA = "";

	blade->aliasNetmaskEthB = "1.289.25.2";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthB = "255.255.255.8";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthB = "255.255.255.0.0";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthB = "255.255.255.0.";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthB = ".255.255.255.0";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);

	blade->aliasNetmaskEthB = "0.255.255.255";
	result = blade->isValid(error, model);
	EXPECT_FALSE(result);
}

TEST(validation, CpBladeIfNetmaskHasCorrectFormatReturnTrue)
{
   int error;
   bool result;

	ACS_CS_ImModel model;
   ACS_CS_ImCp *cp = new ACS_CS_ImCp();
   cp->cpId="cpId=1";
   cp->rdn="cpId=1,cpCategoryId=1,configurationInfoId=1";
   cp->defaultName = "BC42";

	ACS_CS_ImCpBlade *blade = new ACS_CS_ImCpBlade();
   blade->cpRdn="cpId=1,cpCategoryId=1,configurationInfoId=1";

   blade->systemType = SINGLE_SIDED_CP;
   blade->cpRdn="cpId=1,cpCategoryId=1,configurationInfoId=1";
   blade->sequenceNumber = 42;
	blade->entryId = 1;

	blade->aliasNetmaskEthA = "255.0.0.0";
	model.addObject(cp);
   model.addObject(blade);

	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->aliasNetmaskEthA = "255.255.0.0";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->aliasNetmaskEthA = "255.255.255.0";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->aliasNetmaskEthA = "255.255.255.255";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->aliasNetmaskEthB = "255.0.0.0";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->aliasNetmaskEthB = "255.255.0.0";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->aliasNetmaskEthB = "255.255.255.0";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);

	blade->aliasNetmaskEthB = "255.255.255.255";
	result = blade->isValid(error, model);
	EXPECT_TRUE(result);
}
TEST(CpBlade_ToImmObject, createsImmObject)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(immObject != NULL);
}


TEST(CpBlade_ToImmObject, immObjectClassNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_STREQ("CpBlade", immObject->getClassName());
}

TEST(CpBlade_ToImmObject, immObjectParentNameIsCorrect)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_TRUE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) == 0);
}

TEST(CpBlade_ToImmObject, immObjectInvertedParentNameIsNotCorrect)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_FALSE(strcmp(immObject->getParentName(), ACS_CS_ImUtils::getParentName(obj1).c_str()) != 0);
}

TEST(CpBlade_ToImmObject, immObjectSizeIsEighteen)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();

   // Assert
   EXPECT_EQ(18, immObject->getAttributes().size());
}

TEST(CpBlade_ToImmObject, fourteenthAttributeIsCorrectNumber)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[13];

   // Assert
   EXPECT_TRUE(attr.getAttrValuesNum() == 1);
}

TEST(CpBlade_ToImmObject, seventeenthAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];

   // Assert
   EXPECT_STREQ("cpBladeId", attr.getAttrName());
}

TEST(CpBlade_ToImmObject, seveneenthhAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];


   ACS_CC_ValuesDefinitionType baseType = dynamic_cast<ACS_CC_ValuesDefinitionType &> (attr);
   ACS_CS_ImUtils::printValuesDefinitionType(baseType);


   const char * apName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_STREQ("cpBladeId=1", apName);
}

TEST(CpBlade_ToImmObject, fourteenthAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[13];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}


TEST(CpBlade_ToImmObject, eighteenthAttributeIsCorrecAttributeName)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[17];

   // Assert
   EXPECT_STREQ("cp", attr.getAttrName());
}

TEST(CpBlade_ToImmObject, eighteenthAttributeIsCorrecAttributeValues)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[17];

   const char * cpName = reinterpret_cast<const char *>(attr.getAttrValues()[0]);

   // Assert
   EXPECT_STREQ(obj1->cpRdn.c_str(), cpName);

   delete obj1;
   delete immObject;
}

TEST(CpBlade_ToImmObject, seventeenthAttributeIsCorrecAttributeType)
{
   // Arrange
   ACS_CS_ImCpBlade *obj1 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   // Act
   ACS_CS_ImImmObject *immObject = obj1->toImmObject();
   ACS_CS_ImValuesDefinitionType attr = immObject->getAttributes()[16];

   // Assert
   EXPECT_TRUE(attr.getAttrType() == ATTR_STRINGT);
}












