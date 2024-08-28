

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




// ACS_CS_CPId_GetStateTransCodeResponse
#include "ACS_CS_CPId_GetStateTransCodeResponse.h"



// Class ACS_CS_CPId_GetStateTransCodeResponse 

ACS_CS_CPId_GetStateTransCodeResponse::ACS_CS_CPId_GetStateTransCodeResponse()
      : stateTransCode(0)
{
}

ACS_CS_CPId_GetStateTransCodeResponse::ACS_CS_CPId_GetStateTransCodeResponse(const ACS_CS_CPId_GetStateTransCodeResponse &right)
	: ACS_CS_BasicResponse(right)
{
//   resultCode = right.resultCode; //done in superclass' constructor
   stateTransCode = right.stateTransCode;
}


ACS_CS_CPId_GetStateTransCodeResponse::~ACS_CS_CPId_GetStateTransCodeResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetStateTransCodeResponse::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetStateTransCodeResponse;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetStateTransCodeResponse::clone () const
{
   return new ACS_CS_CPId_GetStateTransCodeResponse(*this);
}

 int ACS_CS_CPId_GetStateTransCodeResponse::setBuffer (const char *buffer, int size)
{

   // deserialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned))
   {
      return -1;
   }

   const char *ptr = buffer;
   resultCode = static_cast<ACS_CS_Protocol::CS_Result_Code>(ntohl(*reinterpret_cast<const unsigned*>(ptr)));

   if(size == sizeof(unsigned) + sizeof(ACS_CS_API_NS::StateTransition) && resultCode == ACS_CS_Protocol::Result_Success)
   {
      ptr += sizeof(unsigned);
      stateTransCode = static_cast<ACS_CS_API_NS::StateTransition>(ntohl(*(reinterpret_cast<const unsigned*>(ptr))));
   }

   return 0;

}

 int ACS_CS_CPId_GetStateTransCodeResponse::getBuffer (char *buffer, int size) const
{

   // serialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned) + sizeof(ACS_CS_API_NS::StateTransition))
   {
      return -1;
   }

   char *ptr = buffer;

   *(reinterpret_cast<unsigned*>(ptr)) = htonl(resultCode);

   // buffer will be large enough to hold stateTransCode
   // only when the result code is ACS_CS_Protocol::Result_Success.
   if(resultCode == ACS_CS_Protocol::Result_Success)
   {
      ptr += sizeof(unsigned);
      *(reinterpret_cast<unsigned*>(ptr)) = htonl(stateTransCode);
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetStateTransCodeResponse::getLength () const
{
   return sizeof(unsigned) + sizeof(ACS_CS_API_NS::StateTransition);
}

 ACS_CS_API_NS::StateTransition ACS_CS_CPId_GetStateTransCodeResponse::getStateTransCode () const
{
   return stateTransCode;
}

 void ACS_CS_CPId_GetStateTransCodeResponse::setStateTransCode (ACS_CS_API_NS::StateTransition newStateTransCode)
{
   stateTransCode = newStateTransCode;
}

// Additional Declarations

