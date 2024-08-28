

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




// ACS_CS_CPId_GetApzSubstateResponse
#include "ACS_CS_CPId_GetApzSubstateResponse.h"



// Class ACS_CS_CPId_GetApzSubstateResponse 

ACS_CS_CPId_GetApzSubstateResponse::ACS_CS_CPId_GetApzSubstateResponse()
      : apzSubstate(0)
{
   apzSubstate = static_cast<ACS_CS_API_NS::ApzSubstate>(0);
}

ACS_CS_CPId_GetApzSubstateResponse::ACS_CS_CPId_GetApzSubstateResponse(const ACS_CS_CPId_GetApzSubstateResponse &right)
	: ACS_CS_BasicResponse(right)
{
//   resultCode = right.resultCode; //done in superclass' constructor
   apzSubstate = right.apzSubstate;
}


ACS_CS_CPId_GetApzSubstateResponse::~ACS_CS_CPId_GetApzSubstateResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetApzSubstateResponse::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetApzSubstateResponse;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetApzSubstateResponse::clone () const
{
   return new ACS_CS_CPId_GetApzSubstateResponse(*this);
}

 int ACS_CS_CPId_GetApzSubstateResponse::setBuffer (const char *buffer, int size)
{

   // deserialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned))
   {
      return -1;
   }

   const char *ptr = buffer;
   resultCode = static_cast<ACS_CS_Protocol::CS_Result_Code>(ntohl(*reinterpret_cast<const unsigned*>(ptr)));

   if(size == sizeof(unsigned) + sizeof(ACS_CS_API_NS::ApzSubstate) && resultCode == ACS_CS_Protocol::Result_Success)
   {
      ptr += sizeof(unsigned);
      apzSubstate = static_cast<ACS_CS_API_NS::ApzSubstate>(ntohl(*(reinterpret_cast<const unsigned*>(ptr))));
   }

   return 0;


}

 int ACS_CS_CPId_GetApzSubstateResponse::getBuffer (char *buffer, int size) const
{

   // serialize

   if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned) + sizeof(ACS_CS_API_NS::ApzSubstate))
   {
      return -1;
   }

   char *ptr = buffer;

   *(reinterpret_cast<unsigned*>(ptr)) = htonl(resultCode);

   // buffer will be large enough to hold apzSubstate
   // only when the result code is ACS_CS_Protocol::Result_Success.
   if(resultCode == ACS_CS_Protocol::Result_Success)
   {
      ptr += sizeof(unsigned);
      *(reinterpret_cast<unsigned*>(ptr)) = htonl(apzSubstate);
   }

   // For those new to CS, this returns zero to indicate serialization succeeded,
   // the resultCode is delivered across the socket to the waiting client and does
   // not indicate any status with regard to serialization.
   return 0;


}

 short unsigned ACS_CS_CPId_GetApzSubstateResponse::getLength () const
{
   return sizeof(unsigned) + sizeof(ACS_CS_API_NS::ApzSubstate);
}

 ACS_CS_API_NS::ApzSubstate ACS_CS_CPId_GetApzSubstateResponse::getApzSubstate () const
{
   return apzSubstate;
}

 void ACS_CS_CPId_GetApzSubstateResponse::setApzSubstate (ACS_CS_API_NS::ApzSubstate newApzSubstate)
{
   apzSubstate = newApzSubstate;
}

// Additional Declarations

