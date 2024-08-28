

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



#include "ACS_CS_Protocol.h"

// ACS_CS_CPId_GetAptSubstateResponse
#include "ACS_CS_CPId_GetAptSubstateResponse.h"



// Class ACS_CS_CPId_GetAptSubstateResponse 

ACS_CS_CPId_GetAptSubstateResponse::ACS_CS_CPId_GetAptSubstateResponse()
      : aptSubstate(0)
{
}

ACS_CS_CPId_GetAptSubstateResponse::ACS_CS_CPId_GetAptSubstateResponse(const ACS_CS_CPId_GetAptSubstateResponse &right)
	: ACS_CS_BasicResponse(right)
{
//   resultCode = right.resultCode; //done in superclass' constructor
   aptSubstate = right.aptSubstate;
}


ACS_CS_CPId_GetAptSubstateResponse::~ACS_CS_CPId_GetAptSubstateResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetAptSubstateResponse::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetAptSubstateResponse;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetAptSubstateResponse::clone () const
{
   return new ACS_CS_CPId_GetAptSubstateResponse(*this);
}

 int ACS_CS_CPId_GetAptSubstateResponse::setBuffer (const char *buffer, int size)
{

   // deserialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned))
   {
      return -1;
   }

   const char *ptr = buffer;
   resultCode = static_cast<ACS_CS_Protocol::CS_Result_Code>(ntohl(*reinterpret_cast<const unsigned*>(ptr)));

   if(size == sizeof(unsigned) + sizeof(ACS_CS_API_NS::AptSubstate) && resultCode == ACS_CS_Protocol::Result_Success)
   {
      ptr += sizeof(unsigned);
      aptSubstate = static_cast<ACS_CS_API_NS::AptSubstate>(ntohl(*(reinterpret_cast<const unsigned*>(ptr))));
   }

   return 0;

}

 int ACS_CS_CPId_GetAptSubstateResponse::getBuffer (char *buffer, int size) const
{

   // serialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned) + sizeof(ACS_CS_API_NS::AptSubstate))
   {
      return -1;
   }

   char *ptr = buffer;

   *(reinterpret_cast<unsigned*>(ptr)) = htonl(resultCode);

   // buffer will be large enough to hold aptSubstate
   // only when the result code is ACS_CS_Protocol::Result_Success.
   if(resultCode == ACS_CS_Protocol::Result_Success)
   {
	   ptr += sizeof(unsigned);
      *(reinterpret_cast<unsigned*>(ptr)) = htonl(aptSubstate);
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetAptSubstateResponse::getLength () const
{
   return sizeof(unsigned) + sizeof(ACS_CS_API_NS::AptSubstate);
}

 ACS_CS_API_NS::AptSubstate ACS_CS_CPId_GetAptSubstateResponse::getAptSubstate () const
{
   return aptSubstate;
}

 void ACS_CS_CPId_GetAptSubstateResponse::setAptSubstate (ACS_CS_API_NS::AptSubstate newAptSubstate)
{
   aptSubstate = newAptSubstate;
}

// Additional Declarations

