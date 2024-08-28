

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************



#include "ACS_CS_Util.h"

// ACS_CS_CPId_GetApplicationId
#include "ACS_CS_CPId_GetApplicationId.h"



// Class ACS_CS_CPId_GetApplicationId 

ACS_CS_CPId_GetApplicationId::ACS_CS_CPId_GetApplicationId()
      : cpId(ACS_CS_NS::ILLEGAL_SYSID)
{
}

ACS_CS_CPId_GetApplicationId::ACS_CS_CPId_GetApplicationId(const ACS_CS_CPId_GetApplicationId &right)
	: ACS_CS_Primitive(right)
{
   cpId = right.cpId;
}


ACS_CS_CPId_GetApplicationId::~ACS_CS_CPId_GetApplicationId()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetApplicationId::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetApplicationId;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetApplicationId::clone () const
{
   return new ACS_CS_CPId_GetApplicationId(*this);
}

 CPID ACS_CS_CPId_GetApplicationId::getCpId () const
{
   return cpId;
}

 void ACS_CS_CPId_GetApplicationId::setCpId (CPID newCpId)
{
   cpId = newCpId;
}

 int ACS_CS_CPId_GetApplicationId::setBuffer (const char *buffer, int size)
{

   if(size != sizeof(CPID))
   {
      return -1;
   }

   cpId = static_cast<CPID>(ntohs(*(reinterpret_cast<const CPID*>(buffer))));

   return(0);

}

 int ACS_CS_CPId_GetApplicationId::getBuffer (char *buffer, int size) const
{

   if(size != sizeof(CPID))
   {
      return -1;
   }

   *(reinterpret_cast<CPID*>(buffer)) = htons(static_cast<const CPID>(cpId));

   return(0);

}

 short unsigned ACS_CS_CPId_GetApplicationId::getLength () const
{
   return sizeof(CPID);
}

// Additional Declarations

