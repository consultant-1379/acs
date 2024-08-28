

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




// ACS_CS_CPId_GetApplicationIdResponse
#include "ACS_CS_CPId_GetApplicationIdResponse.h"



// Class ACS_CS_CPId_GetApplicationIdResponse 

ACS_CS_CPId_GetApplicationIdResponse::ACS_CS_CPId_GetApplicationIdResponse()
      : applicationId(0)
{
}

ACS_CS_CPId_GetApplicationIdResponse::ACS_CS_CPId_GetApplicationIdResponse(const ACS_CS_CPId_GetApplicationIdResponse &right)
	: ACS_CS_BasicResponse(right)
{
//   resultCode = right.resultCode; //done in superclass' constructor
   applicationId = right.applicationId;
}


ACS_CS_CPId_GetApplicationIdResponse::~ACS_CS_CPId_GetApplicationIdResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetApplicationIdResponse::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetApplicationIdResponse;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetApplicationIdResponse::clone () const
{
   return new ACS_CS_CPId_GetApplicationIdResponse(*this);
}

 int ACS_CS_CPId_GetApplicationIdResponse::setBuffer (const char *buffer, int size)
{
   // deserialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned))
   {
      return -1;
   }

   const char *ptr = buffer;
   resultCode = static_cast<ACS_CS_Protocol::CS_Result_Code>(ntohl(*reinterpret_cast<const unsigned*>(ptr)));

   if(size == sizeof(unsigned) + sizeof(ACS_CS_API_NS::ApplicationId) && resultCode == ACS_CS_Protocol::Result_Success)
   {
      ptr += sizeof(unsigned);
      applicationId = static_cast<ACS_CS_API_NS::ApplicationId>(ntohl(*(reinterpret_cast<const unsigned*>(ptr))));
   }

   return 0;

}

 int ACS_CS_CPId_GetApplicationIdResponse::getBuffer (char *buffer, int size) const
{
   // serialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned) + sizeof(ACS_CS_API_NS::ApplicationId))
   {
      return -1;
   }

   char *ptr = buffer;

   *(reinterpret_cast<unsigned*>(ptr)) = htonl(resultCode);
   ptr += sizeof(unsigned);

   // buffer will be large enough to hold cpState
   // only when the result code is ACS_CS_Protocol::Result_Success.
   if(resultCode == ACS_CS_Protocol::Result_Success)
   {
      *(reinterpret_cast<unsigned*>(ptr)) = htonl(applicationId);
   }

   return 0;

}

 short unsigned ACS_CS_CPId_GetApplicationIdResponse::getLength () const
{
   return sizeof(unsigned) + sizeof(ACS_CS_API_NS::ApplicationId);
}

 ACS_CS_API_NS::ApplicationId ACS_CS_CPId_GetApplicationIdResponse::getApplicationId () const
{
   return applicationId;
}

 void ACS_CS_CPId_GetApplicationIdResponse::setApplicationId (ACS_CS_API_NS::ApplicationId newApplicationId)
{
   applicationId = newApplicationId;
}

// Additional Declarations

