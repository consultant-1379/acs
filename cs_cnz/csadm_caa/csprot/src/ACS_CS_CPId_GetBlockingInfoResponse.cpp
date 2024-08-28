//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE88F43036C.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE88F43036C.cm

//## begin module%4BE88F43036C.cp preserve=no
//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************
//## end module%4BE88F43036C.cp

//## Module: ACS_CS_CPId_GetBlockingInfoResponse%4BE88F43036C; Package body
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetBlockingInfoResponse.cpp

//## begin module%4BE88F43036C.additionalIncludes preserve=no
//## end module%4BE88F43036C.additionalIncludes

//## begin module%4BE88F43036C.includes preserve=yes
#include "ACS_CS_API.h"
#include "ACS_CS_HostNetworkConverter.h"
//## end module%4BE88F43036C.includes

// ACS_CS_CPId_GetBlockingInfoResponse
#include "ACS_CS_CPId_GetBlockingInfoResponse.h"
//## begin module%4BE88F43036C.declarations preserve=no
//## end module%4BE88F43036C.declarations

//## begin module%4BE88F43036C.additionalDeclarations preserve=yes
//## end module%4BE88F43036C.additionalDeclarations


// Class ACS_CS_CPId_GetBlockingInfoResponse 

ACS_CS_CPId_GetBlockingInfoResponse::ACS_CS_CPId_GetBlockingInfoResponse()
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_const.hasinit preserve=no
      : m_blockingInfo(0)
  //## end ACS_CS_CPId_GetBlockingInfoResponse::ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_const.hasinit
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_const.initialization preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfoResponse::ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_const.initialization
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_const.body preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfoResponse::ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_const.body
}


ACS_CS_CPId_GetBlockingInfoResponse::~ACS_CS_CPId_GetBlockingInfoResponse()
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::~ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_dest.body preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfoResponse::~ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1_dest.body
}



//## Other Operations (implementation)
 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetBlockingInfoResponse::getPrimitiveType () const
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::getPrimitiveType%4BD9D7BD023B.body preserve=yes
   return ACS_CS_Protocol::Primitive_GetBlockingInfoResponse;
  //## end ACS_CS_CPId_GetBlockingInfoResponse::getPrimitiveType%4BD9D7BD023B.body
}

 int ACS_CS_CPId_GetBlockingInfoResponse::setBuffer (const char *buffer, int /*size*/)
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::setBuffer%4BD9D7C502E7.body preserve=yes
   return ACS_CS_HostNetworkConverter::deserialize(buffer, *this);
  //## end ACS_CS_CPId_GetBlockingInfoResponse::setBuffer%4BD9D7C502E7.body
}

 int ACS_CS_CPId_GetBlockingInfoResponse::getBuffer (char *buffer, int size) const
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::getBuffer%4BD9D7C80345.body preserve=yes
   return ACS_CS_HostNetworkConverter::serialize(buffer, size, *this);
  //## end ACS_CS_CPId_GetBlockingInfoResponse::getBuffer%4BD9D7C80345.body
}

 short unsigned ACS_CS_CPId_GetBlockingInfoResponse::getLength () const
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::getLength%4BD9D7CB0335.body preserve=yes
   return sizeof(*this);
  //## end ACS_CS_CPId_GetBlockingInfoResponse::getLength%4BD9D7CB0335.body
}

 ACS_CS_Primitive * ACS_CS_CPId_GetBlockingInfoResponse::clone () const
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::clone%4BD9D82300F4.body preserve=yes
   return new ACS_CS_CPId_GetBlockingInfoResponse(*this);
  //## end ACS_CS_CPId_GetBlockingInfoResponse::clone%4BD9D82300F4.body
}

 ACS_CS_API_NS::BlockingInfo ACS_CS_CPId_GetBlockingInfoResponse::getBlockingInfo () const
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::getBlockingInfo%4BD9D8340067.body preserve=yes
   return m_blockingInfo;
  //## end ACS_CS_CPId_GetBlockingInfoResponse::getBlockingInfo%4BD9D8340067.body
}

 void ACS_CS_CPId_GetBlockingInfoResponse::setBlockingInfo (const ACS_CS_API_NS::BlockingInfo blockingInfo)
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse::setBlockingInfo%4BD9D92A01D0.body preserve=yes
   m_blockingInfo = blockingInfo;
  //## end ACS_CS_CPId_GetBlockingInfoResponse::setBlockingInfo%4BD9D92A01D0.body
}

// Additional Declarations
  //## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.declarations preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.declarations

//## begin module%4BE88F43036C.epilog preserve=yes
//## end module%4BE88F43036C.epilog
