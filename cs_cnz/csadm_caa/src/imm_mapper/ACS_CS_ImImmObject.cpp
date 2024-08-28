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
 * @file ACS_CS_ImImmObject.cpp
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
 * 2011-09-28  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include "ACS_CS_ImImmObject.h"
#include <cstring>
#include <string>

ACS_CS_ImImmObject::ACS_CS_ImImmObject() {

}

ACS_CS_ImImmObject::~ACS_CS_ImImmObject() {
   delete p_className;
   delete p_parentName;
   p_attrValuesList.clear();
}


void ACS_CS_ImImmObject::setClassName(const char *className)
{
   p_className = new char[strlen(className) + 1];
   strcpy(const_cast<char *>(p_className), className);

}

void ACS_CS_ImImmObject::setParentName(const char *parentName)
{
   p_parentName = new char[strlen(parentName) + 1];
   strcpy(const_cast<char *>(p_parentName), parentName);

}
void ACS_CS_ImImmObject::setAttributeList(std::vector<ACS_CS_ImValuesDefinitionType> &attributes)
{
   p_attrValuesList = attributes;
}


const char *ACS_CS_ImImmObject::getClassName() const
{
   return p_className;
}

const char *ACS_CS_ImImmObject::getParentName() const
{
   return p_parentName;
}

std::vector<ACS_CS_ImValuesDefinitionType> ACS_CS_ImImmObject::getAttributes()
{
   return p_attrValuesList;
}

