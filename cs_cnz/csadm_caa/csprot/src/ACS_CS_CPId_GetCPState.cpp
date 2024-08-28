

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

// ACS_CS_CPId_GetCPState
#include "ACS_CS_CPId_GetCPState.h"



// Class ACS_CS_CPId_GetCPState 

ACS_CS_CPId_GetCPState::ACS_CS_CPId_GetCPState()
      : cpId(ACS_CS_NS::ILLEGAL_SYSID)
{
}

ACS_CS_CPId_GetCPState::ACS_CS_CPId_GetCPState(const ACS_CS_CPId_GetCPState &right)
	: ACS_CS_Primitive(right)
{
   cpId = right.cpId;
}


ACS_CS_CPId_GetCPState::~ACS_CS_CPId_GetCPState()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPState::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetCPState;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPState::clone () const
{
   return new ACS_CS_CPId_GetCPState(*this);
}

 CPID ACS_CS_CPId_GetCPState::getCpId () const
{
   return cpId;
}

 void ACS_CS_CPId_GetCPState::setCpId (CPID newCpId)
{
   cpId = newCpId;
}

 int ACS_CS_CPId_GetCPState::setBuffer (const char *buffer, int size)
{

   if(size != sizeof(CPID))
   {
      return -1;
   }

   cpId = static_cast<CPID>(ntohs(*(reinterpret_cast<const CPID*>(buffer))));

   return(0);

}

 int ACS_CS_CPId_GetCPState::getBuffer (char *buffer, int size) const
{

   if(size != sizeof(CPID))
   {
      return -1;
   }

   *(reinterpret_cast<CPID*>(buffer)) = htons(static_cast<const CPID>(cpId));

   return(0);

}

 short unsigned ACS_CS_CPId_GetCPState::getLength () const
{
   return sizeof(CPID);
}

// Additional Declarations

