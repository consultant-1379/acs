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
 * @file ACS_CC_ValuesDefinitionType_test.cpp
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

#include "ACS_CC_Types.h"
#include <cstring>
#include <iostream>

#include "ACS_CS_ImValuesDefinitionType.h"

using namespace std;

TEST(ValuesDefinitionType_copyConstructor, createsCorrectAttrName)
{
   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("AttrName"));

   double value = 1234;
   void* values[1] = { reinterpret_cast<void *>(&value) };
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   // Assert
   EXPECT_TRUE(strcmp(type2.getAttrName(), type.getAttrName()) == 0);
}


TEST(ValuesDefinitionType_copyConstructor, createsCorrectAttrType)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("AttrName"));

   double value = 1234;
   void* values[1] = { reinterpret_cast<void *>(&value) };
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   // Assert
   EXPECT_TRUE(type2.getAttrType() == type.getAttrType());
}

TEST(ValuesDefinitionType_copyConstructor, createsCorrectAttrValuesNum)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("AttrName"));

   double value = 1234;
   void* values[1] = { reinterpret_cast<void *>(&value) };
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   // Assert
   EXPECT_TRUE(type2.getAttrValuesNum() == type.getAttrValuesNum());
}


TEST(ValuesDefinitionType_copyConstructor, createsCorrectAttrValuesPointersAreNotEqual)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("AttrName"));

   int value = 1234;
   void* values[1] = { reinterpret_cast<void *>(&value) };
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   int *val1 = reinterpret_cast<int *> (type.getAttrValues()[0]);
   int *val2 = reinterpret_cast<int *> (type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(val1 != val2);
}

TEST(ValuesDefinitionType_copyConstructor, createsCorrectAttrValuesAreEqual)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("AttrName"));

   int value = 1234;
   void* values[1] = { reinterpret_cast<void *>(&value) };
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   int *val1 = reinterpret_cast<int *> (type.getAttrValues()[0]);
   int *val2 = reinterpret_cast<int *> (type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(*val1 == *val2);
}

TEST(ValuesDefinitionType_copyConstructor, attrValueIs1234)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("AttrName"));

   int value = 1234;
   void* values[1] = { reinterpret_cast<void *>(&value) };
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   int *val2 = reinterpret_cast<int *> (type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(*val2 == 1234);
}




TEST(ValuesDefinitionType_copyConstructor, createsCorrectStringAttrValuesAreEqual)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("StringValue"));

   string strValue = "String";
   char *pString = const_cast<char *>(strValue.c_str());
   void* values[1] = { reinterpret_cast<void *>(pString)};
   type.setAttrValues(values, 1, ATTR_STRINGT);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   const char *val1 = reinterpret_cast<const char *> (type.getAttrValues()[0]);
   const char *val2 = reinterpret_cast<const char *> (type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(val1, val2) == 0);
}


TEST(ValuesDefinitionType_copyConstructor, stringValueIsStringValue)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("StringValue"));

   string strValue = "String";
   char *pString = const_cast<char *>(strValue.c_str());
   void* values[1] = { reinterpret_cast<void *>(pString)};
   type.setAttrValues(values, 1, ATTR_STRINGT);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   const char *val2 = reinterpret_cast<const char *> (type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(val2, strValue.c_str()) == 0);
}


TEST(ValuesDefinitionType_assignmentOperator, createsCorrectStringAttrValuesAreEqual)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("StringValue"));

   string strValue = "String";
   char *pString = const_cast<char *>(strValue.c_str());
   void* values[1] = { reinterpret_cast<void *>(pString)};
   type.setAttrValues(values, 1, ATTR_STRINGT);

   // Act
   ACS_CS_ImValuesDefinitionType type2 = type;

   const char *val1 = reinterpret_cast<const char *> (type.getAttrValues()[0]);
   const char *val2 = reinterpret_cast<const char *> (type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(val1, val2) == 0);
}


TEST(ValuesDefinitionType_assignmentOperator, createsCorrectStringAttrValuesAreEqual2)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("StringValue"));

   string strValue = "String";
   void* values[1] = { reinterpret_cast<void *>(&strValue)};
   type.setAttrValues(values, 1, ATTR_STRINGT);

   // Act
   ACS_CS_ImValuesDefinitionType type2 = type;

   const char *val1 = reinterpret_cast<const char *>(type.getAttrValues()[0]);
   const char *val2 = reinterpret_cast<const char *>(type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(strcmp(val1, val2) == 0);
}


TEST(ValuesDefinitionType_assignmentOperator, createsCorrectUINTAttrValuesAreEqual2)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("UIntValue"));

   unsigned int uiValue = 4321;
   void* values[1] = { reinterpret_cast<void *>(&uiValue)};
   type.setAttrValues(values, 1, ATTR_UINT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2 = type;

   unsigned int *val1 = reinterpret_cast<unsigned int *>(type.getAttrValues()[0]);
   unsigned int *val2 = reinterpret_cast<unsigned int *>(type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(*val1 == *val2);
}


TEST(ValuesDefinitionType_assignmentOperator, createsCorrectUINTAttrValuesAreEqualEvenIfValueChanges)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("UIntValue"));

   unsigned int uiValue = 4321;
   void* values[1] = { reinterpret_cast<void *>(&uiValue)};
   type.setAttrValues(values, 1, ATTR_UINT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2 = type;
   uiValue = 1234;

   unsigned int *val1 = reinterpret_cast<unsigned int *>(type.getAttrValues()[0]);
   unsigned int *val2 = reinterpret_cast<unsigned int *>(type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(*val1 == *val2);
}



TEST(ValuesDefinitionType_assignmentOperator, createsCorrectINTAttrValuesAreEqual2)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("IntValue"));

   int uiValue = 4321;
   void* values[1] = { reinterpret_cast<void *>(&uiValue)};
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2 = type;

   int *val1 = reinterpret_cast<int *>(type.getAttrValues()[0]);
   int *val2 = reinterpret_cast<int *>(type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(*val1 == *val2);
}

TEST(ValuesDefinitionType_assignmentOperator, createsCorrectINTAttrPointersAreNotEqual)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("IntValue"));

   int uiValue = 4321;
   void* values[1] = { reinterpret_cast<void *>(&uiValue)};
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2 = type;

   int *val1 = reinterpret_cast<int *>(type.getAttrValues()[0]);
   int *val2 = reinterpret_cast<int *>(type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(val1 != val2);
}

TEST(ValuesDefinitionType_assignmentOperator, createsCorrectINTAttrPointersAreNotEqual2)
{

   // Arrange
   ACS_CS_ImValuesDefinitionType type;

   type.setAttrName(const_cast<char *>("IntValue"));

   int uiValue = 4321;
   void* values[1] = { reinterpret_cast<void *>(&uiValue)};
   type.setAttrValues(values, 1, ATTR_INT32T);

   // Act
   ACS_CS_ImValuesDefinitionType type2(type);

   int *val1 = reinterpret_cast<int *>(type.getAttrValues()[0]);
   int *val2 = reinterpret_cast<int *>(type2.getAttrValues()[0]);

   // Assert
   EXPECT_TRUE(val1 != val2);
}
