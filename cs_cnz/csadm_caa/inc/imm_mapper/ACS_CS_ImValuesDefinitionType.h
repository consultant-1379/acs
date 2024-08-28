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
 * @file ACS_CS_ImValuesDefinitionType.h
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

#ifndef ACS_CS_IMVALUESDEFINITIONTYPE_H_
#define ACS_CS_IMVALUESDEFINITIONTYPE_H_

#include <stdint.h>
#include "ACS_CC_Types.h"

class ACS_CS_ImValuesDefinitionType : public ACS_CC_ValuesDefinitionType {
public:
   ACS_CS_ImValuesDefinitionType();
   ACS_CS_ImValuesDefinitionType(const ACS_CS_ImValuesDefinitionType &other);
   virtual ~ACS_CS_ImValuesDefinitionType();

   void setAttrName(char *attrName);
   void setAttrValues(void** values, unsigned int num, ACS_CC_AttrValueType type);

   char * getAttrName();
   ACS_CC_AttrValueType getAttrType();
   unsigned int getAttrValuesNum();
   void** getAttrValues();

   ACS_CS_ImValuesDefinitionType & operator=(ACS_CS_ImValuesDefinitionType const &other);


private:

   ACS_CS_ImValuesDefinitionType & copyObj(ACS_CS_ImValuesDefinitionType const &obj);

};

#endif /* ACS_CS_IMVALUESDEFINITIONTYPE_H_ */
