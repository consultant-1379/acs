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
 * @file ACS_CS_ImImmObject.h
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

#ifndef ACS_CS_IMIMMOBJECT_H_
#define ACS_CS_IMIMMOBJECT_H_

#include "ACS_CS_ImValuesDefinitionType.h"
#include <vector>

class ACS_CS_ImImmObject {
public:
   ACS_CS_ImImmObject();
   virtual ~ACS_CS_ImImmObject();

   void setClassName(const char *className);
   void setParentName(const char *parentName);
   void setAttributeList(std::vector<ACS_CS_ImValuesDefinitionType> &attributes);

   const char *getClassName() const;
   const char *getParentName() const;
   std::vector<ACS_CS_ImValuesDefinitionType> getAttributes();


private:

   const char* p_className;
   const char* p_parentName;
   std::vector<ACS_CS_ImValuesDefinitionType> p_attrValuesList;

};

#endif /* ACS_CS_IMIMMOBJECT_H_ */
