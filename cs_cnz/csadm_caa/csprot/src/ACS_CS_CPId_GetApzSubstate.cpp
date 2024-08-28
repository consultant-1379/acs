

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

// ACS_CS_CPId_GetApzSubstate
#include "ACS_CS_CPId_GetApzSubstate.h"



// Class ACS_CS_CPId_GetApzSubstate 

ACS_CS_CPId_GetApzSubstate::ACS_CS_CPId_GetApzSubstate()
      : cpId(ACS_CS_NS::ILLEGAL_SYSID)
{
}

ACS_CS_CPId_GetApzSubstate::ACS_CS_CPId_GetApzSubstate(const ACS_CS_CPId_GetApzSubstate &right)
	: ACS_CS_Primitive(right)
{
   cpId = right.cpId;
}


ACS_CS_CPId_GetApzSubstate::~ACS_CS_CPId_GetApzSubstate()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetApzSubstate::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetApzSubstate;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetApzSubstate::clone () const
{
   return new ACS_CS_CPId_GetApzSubstate(*this);
}

 CPID ACS_CS_CPId_GetApzSubstate::getCpId () const
{
   return cpId;
}

 void ACS_CS_CPId_GetApzSubstate::setCpId (CPID newCpId)
{
   cpId = newCpId;
}

 int ACS_CS_CPId_GetApzSubstate::setBuffer (const char *buffer, int size)
{

   if(size != sizeof(CPID))
   {
      return -1;
   }

   cpId = static_cast<CPID>(ntohs(*(reinterpret_cast<const CPID*>(buffer))));

   return(0);

}

 int ACS_CS_CPId_GetApzSubstate::getBuffer (char *buffer, int size) const
{

   if(size != sizeof(CPID))
   {
      return -1;
   }

   *(reinterpret_cast<CPID*>(buffer)) = htons(static_cast<const CPID>(cpId));

   return(0);

}

 short unsigned ACS_CS_CPId_GetApzSubstate::getLength () const
{
   return sizeof(CPID);
}

// Additional Declarations

