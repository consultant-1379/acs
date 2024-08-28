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
 * @file ACS_CS_ImValuesDefinitionType.cpp
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

#include "ACS_CS_ImValuesDefinitionType.h"
#include <iostream>
#include <cstring>

using namespace std;

ACS_CS_ImValuesDefinitionType::ACS_CS_ImValuesDefinitionType() {}


ACS_CS_ImValuesDefinitionType::ACS_CS_ImValuesDefinitionType(const ACS_CS_ImValuesDefinitionType &other)
{
   copyObj(other);
}

ACS_CS_ImValuesDefinitionType::~ACS_CS_ImValuesDefinitionType()
{
   delete[] attrName;

   for (unsigned int i = 0; i < attrValuesNum; i++ )
   {
      switch (attrType)
      {
         case ATTR_INT32T :
            delete (int32_t *) attrValues[i];
            break;
         case ATTR_UINT32T :
            delete (uint32_t *) attrValues[i];
            break;
         case ATTR_NAMET :
            delete[] (char *) attrValues[i];
            break;
         case ATTR_STRINGT :
            delete[] (char *) attrValues[i];
            break;
         default:
            break;
      }
   }

   delete[] attrValues;
}



void ACS_CS_ImValuesDefinitionType::setAttrName(char *attrName)
{
   this->attrName = new char[strlen(attrName) + 1];
   strcpy(this->attrName, attrName);
}


void ACS_CS_ImValuesDefinitionType::setAttrValues(void** values, unsigned int num, ACS_CC_AttrValueType type)
{
   this->attrType = type;
   this->attrValuesNum = num;

   this->attrValues = new void*[num];

   for (unsigned int i = 0; i < num; i++)
   {
      if (type == ATTR_STRINGT)
      {
         char *originalArray = reinterpret_cast<char *>(values[i]);
         char *array = new char[strlen(originalArray) + 1];
         strcpy(array, originalArray);
         attrValues[i] = array;
      }

      else if (type == ATTR_NAMET)
      {
         char *originalArray = reinterpret_cast<char *>(values[i]);
         char *array = new char[strlen(originalArray) + 1];
         strcpy(array, originalArray);
         attrValues[i] = array;
      }

      else if (type == ATTR_INT32T)
      {
         int *pVal = reinterpret_cast<int *>(values[i]);
         int *pNewInt = new int(*pVal);
         attrValues[i] = pNewInt;
      }

      else if (type == ATTR_UINT32T)
      {
         unsigned int *pVal = reinterpret_cast<unsigned int *>(values[i]);
         unsigned int *pNewUInt = new unsigned int(*pVal);
         attrValues[i] = pNewUInt;
      }
   }
}


char * ACS_CS_ImValuesDefinitionType::getAttrName()
{
   return attrName;
}

ACS_CC_AttrValueType ACS_CS_ImValuesDefinitionType::getAttrType()
{
   return attrType;
}

unsigned int ACS_CS_ImValuesDefinitionType::getAttrValuesNum()
{
   return attrValuesNum;

}

void** ACS_CS_ImValuesDefinitionType::getAttrValues()
{
   return attrValues;
}


ACS_CS_ImValuesDefinitionType & ACS_CS_ImValuesDefinitionType::operator=(ACS_CS_ImValuesDefinitionType const &other)
{
   copyObj(other);
   return *this;

}

ACS_CS_ImValuesDefinitionType & ACS_CS_ImValuesDefinitionType::copyObj(ACS_CS_ImValuesDefinitionType const &obj)
{
   this->setAttrName(obj.attrName);
   this->setAttrValues(obj.attrValues, obj.attrValuesNum, obj.attrType);
   return *this;
}



