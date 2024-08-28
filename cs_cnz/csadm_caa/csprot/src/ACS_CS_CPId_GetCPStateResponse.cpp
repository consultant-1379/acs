

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




// ACS_CS_CPId_GetCPStateResponse
#include "ACS_CS_CPId_GetCPStateResponse.h"



// Class ACS_CS_CPId_GetCPStateResponse 

ACS_CS_CPId_GetCPStateResponse::ACS_CS_CPId_GetCPStateResponse()
      : cpState(0)
{
}

ACS_CS_CPId_GetCPStateResponse::ACS_CS_CPId_GetCPStateResponse(const ACS_CS_CPId_GetCPStateResponse &right)
	: ACS_CS_BasicResponse(right)
{
//   resultCode = right.resultCode; //done in superclass' constructor
   cpState = right.cpState;
}


ACS_CS_CPId_GetCPStateResponse::~ACS_CS_CPId_GetCPStateResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPStateResponse::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetCPStateResponse;
}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPStateResponse::clone () const
{
   return new ACS_CS_CPId_GetCPStateResponse(*this);
}

 int ACS_CS_CPId_GetCPStateResponse::setBuffer (const char *buffer, int size)
{

   // deserialize

	if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned))
	{
		return -1;
	}

	const char *ptr = buffer;
	resultCode = static_cast<ACS_CS_Protocol::CS_Result_Code>(ntohl(*reinterpret_cast<const unsigned*>(ptr)));

	if(size == sizeof(unsigned) + sizeof(ACS_CS_API_NS::CpState) && resultCode == ACS_CS_Protocol::Result_Success)
	{
		ptr += sizeof(unsigned);
		cpState = static_cast<ACS_CS_API_NS::CpState>(ntohl(*(reinterpret_cast<const unsigned*>(ptr))));
	}

	return 0;

}

 int ACS_CS_CPId_GetCPStateResponse::getBuffer (char *buffer, int size) const
{

   // serialize

	if(buffer == NULL || static_cast<size_t>(size) < sizeof(unsigned) + sizeof(ACS_CS_API_NS::CpState))
	{
		return -1;
	}

	char *ptr = buffer;

	*(reinterpret_cast<unsigned*>(ptr)) = htonl(resultCode);

	// buffer will be large enough to hold cpState
	// only when the result code is ACS_CS_Protocol::Result_Success.
	if(resultCode == ACS_CS_Protocol::Result_Success)
	{
		ptr += sizeof(unsigned);
		*(reinterpret_cast<unsigned*>(ptr)) = htonl(cpState);
	}

	return 0;

}

 short unsigned ACS_CS_CPId_GetCPStateResponse::getLength () const
{
   return sizeof(unsigned) + sizeof(ACS_CS_API_NS::CpState);
}

 ACS_CS_API_NS::CpState ACS_CS_CPId_GetCPStateResponse::getCpState () const
{
   return cpState;
}

 void ACS_CS_CPId_GetCPStateResponse::setCpState (ACS_CS_API_NS::CpState newCpState)
{
   cpState = newCpState;
}

// Additional Declarations

