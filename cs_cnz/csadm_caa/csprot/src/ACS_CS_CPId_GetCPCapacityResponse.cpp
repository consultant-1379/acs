//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE890820167.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE890820167.cm

//## begin module%4BE890820167.cp preserve=no
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
//## end module%4BE890820167.cp

//## Module: ACS_CS_CPId_GetCPCapacityResponse%4BE890820167; Package body
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetCPCapacityResponse.cpp

//## begin module%4BE890820167.additionalIncludes preserve=no
//## end module%4BE890820167.additionalIncludes

//## begin module%4BE890820167.includes preserve=yes
#include "ACS_CS_API.h"
#include "ACS_CS_HostNetworkConverter.h"
//## end module%4BE890820167.includes

// ACS_CS_CPId_GetCPCapacityResponse
#include "ACS_CS_CPId_GetCPCapacityResponse.h"
//## begin module%4BE890820167.declarations preserve=no
//## end module%4BE890820167.declarations

//## begin module%4BE890820167.additionalDeclarations preserve=yes
//## end module%4BE890820167.additionalDeclarations


// Class ACS_CS_CPId_GetCPCapacityResponse 

ACS_CS_CPId_GetCPCapacityResponse::ACS_CS_CPId_GetCPCapacityResponse()
  //## begin ACS_CS_CPId_GetCPCapacityResponse::ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_const.hasinit preserve=no
      : m_cpCapacity(0)
  //## end ACS_CS_CPId_GetCPCapacityResponse::ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_const.hasinit
  //## begin ACS_CS_CPId_GetCPCapacityResponse::ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_const.initialization preserve=yes
  //## end ACS_CS_CPId_GetCPCapacityResponse::ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_const.initialization
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_const.body preserve=yes
  //## end ACS_CS_CPId_GetCPCapacityResponse::ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_const.body
}


ACS_CS_CPId_GetCPCapacityResponse::~ACS_CS_CPId_GetCPCapacityResponse()
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::~ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_dest.body preserve=yes
  //## end ACS_CS_CPId_GetCPCapacityResponse::~ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110_dest.body
}



//## Other Operations (implementation)
 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPCapacityResponse::getPrimitiveType () const
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::getPrimitiveType%4BD9D8940058.body preserve=yes
   return ACS_CS_Protocol::Primitive_GetCPCapacityResponse;
  //## end ACS_CS_CPId_GetCPCapacityResponse::getPrimitiveType%4BD9D8940058.body
}

 int ACS_CS_CPId_GetCPCapacityResponse::setBuffer (const char *buffer, int /*size*/)
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::setBuffer%4BD9D89A0104.body preserve=yes
   return ACS_CS_HostNetworkConverter::deserialize(buffer, *this);
  //## end ACS_CS_CPId_GetCPCapacityResponse::setBuffer%4BD9D89A0104.body
}

 int ACS_CS_CPId_GetCPCapacityResponse::getBuffer (char *buffer, int size) const
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::getBuffer%4BD9D89D0078.body preserve=yes
   return ACS_CS_HostNetworkConverter::serialize(buffer, size, *this);
  //## end ACS_CS_CPId_GetCPCapacityResponse::getBuffer%4BD9D89D0078.body
}

 short unsigned ACS_CS_CPId_GetCPCapacityResponse::getLength () const
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::getLength%4BD9D8A00114.body preserve=yes
   return sizeof(*this);
  //## end ACS_CS_CPId_GetCPCapacityResponse::getLength%4BD9D8A00114.body
}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPCapacityResponse::clone () const
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::clone%4BD9D8A501A1.body preserve=yes
   return new ACS_CS_CPId_GetCPCapacityResponse(*this);
  //## end ACS_CS_CPId_GetCPCapacityResponse::clone%4BD9D8A501A1.body
}

 ACS_CS_API_NS::CpCapacity ACS_CS_CPId_GetCPCapacityResponse::getCPCapacity () const
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::getCPCapacity%4BD9DBED01F4.body preserve=yes
   return m_cpCapacity;
  //## end ACS_CS_CPId_GetCPCapacityResponse::getCPCapacity%4BD9DBED01F4.body
}

 void ACS_CS_CPId_GetCPCapacityResponse::setCPCapacity (const ACS_CS_API_NS::CpCapacity cpCapacity)
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse::setCPCapacity%4BD9DBFF0129.body preserve=yes
   m_cpCapacity = cpCapacity;
  //## end ACS_CS_CPId_GetCPCapacityResponse::setCPCapacity%4BD9DBFF0129.body
}

// Additional Declarations
  //## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.declarations preserve=yes
  //## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.declarations

//## begin module%4BE890820167.epilog preserve=yes
//## end module%4BE890820167.epilog
