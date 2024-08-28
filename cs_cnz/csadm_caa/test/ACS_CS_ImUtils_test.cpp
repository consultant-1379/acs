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


#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImUtils.h"

#include <iostream>
#include <cstring>
#include <set>
#include <vector>

using namespace std;


TEST(Utils_createStringType, createsCorrectAttributeName)
{
   // Arrange
   string stringVal = "StringVal";

   // Act
   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createStringType("attrName", stringVal);

   // Assert
   EXPECT_TRUE(strcmp(type.getAttrName(), "attrName") == 0);
}


TEST(Utils_createStringType, createsCorrectType)
{
   // Arrange
   string stringVal = "StringVal";

   // Act
   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createStringType("attrName", stringVal);

   // Assert
   EXPECT_TRUE(type.getAttrType() == ATTR_STRINGT);
}

TEST(Utils_createStringType, createsCorrectNumberOfValues)
{
   // Arrange
   string stringVal = "StringVal";

   // Act
   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createStringType("attrName", stringVal);

   // Assert
   EXPECT_TRUE(type.getAttrValuesNum() == 1);
}


TEST(Utils_createStringType, createsCorrectValues)
{
   // Arrange
   string stringVal = "StringVal";

   // Act
   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createStringType("attrName", stringVal);

   void** attrs = type.getAttrValues();
   char *pString = reinterpret_cast<char *>(attrs[0]);

   // Assert
   EXPECT_TRUE(strcmp(pString, stringVal.c_str()) == 0);
}



TEST(Utils_createStringType, createsCorrectValuesOfStringCopy)
{
   // Arrange
   string stringVal = "StringVal";

   // Act
   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createStringType("attrName", stringVal);
   ACS_CS_ImValuesDefinitionType type2(type);

   void **attrs = type2.getAttrValues();
   char *pString = reinterpret_cast<char *>(attrs[0]);

   // Assert
   EXPECT_TRUE(strcmp(pString, stringVal.c_str()) == 0);
}


TEST(Utils_createUIntType, createsCorrectValuesOfUIntCopy)
{
   // Arrange
   unsigned int uintVal = 1234;

   // Act
   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createUIntType("attrName", uintVal);
   ACS_CS_ImValuesDefinitionType type2(type);

   void **attrs = type2.getAttrValues();
   unsigned int *pUI = reinterpret_cast<unsigned int *>(attrs[0]);

   // Assert
   EXPECT_TRUE(*pUI == uintVal);
}



TEST(Utils_createUIntType, createsCorrectValuesOfIntCopy)
{
   // Arrange
   int intVal = 1234;

   // Act
   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createIntType("attrName", intVal);
   ACS_CS_ImValuesDefinitionType type2(type);

   void **attrs = type2.getAttrValues();
   int *pI = reinterpret_cast<int *>(attrs[0]);

   // Assert
   EXPECT_TRUE(*pI == intVal);
}




TEST(Utils_createNameType, createsCorrectValuesOfNameCopy)
{
   // Arrange
   string str1 = "str1";
   string str2 = "str2";
   string str3 = "str3";

   std::set<string> strings;
   strings.insert(str1);
   strings.insert(str2);
   strings.insert(str3);


   ACS_CS_ImValuesDefinitionType type = ACS_CS_ImUtils::createNameType("names", strings);
   ACS_CS_ImValuesDefinitionType type2(type);

   // Act

   void **attrs = type2.getAttrValues();
   const char *pstr1 = reinterpret_cast<const char *>(attrs[0]);
   const char *pstr2 = reinterpret_cast<const char *>(attrs[1]);
   const char *pstr3 = reinterpret_cast<const char *>(attrs[2]);

   // Assert
   EXPECT_STREQ(pstr1, str1.c_str());
   EXPECT_STREQ(pstr2, str2.c_str());
   EXPECT_STREQ(pstr3, str3.c_str());
}


