//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE88C0303B4.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE88C0303B4.cm

//## begin module%4BE88C0303B4.cp preserve=no
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
//## end module%4BE88C0303B4.cp

//## Module: ACS_CS_CPId_GetCPCapacity%4BE88C0303B4; Package body
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetCPCapacity.cpp

//## begin module%4BE88C0303B4.additionalIncludes preserve=no
//## end module%4BE88C0303B4.additionalIncludes

//## begin module%4BE88C0303B4.includes preserve=yes
#include "ACS_CS_API.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_HostNetworkConverter.h"
//## end module%4BE88C0303B4.includes

// ACS_CS_CPId_GetCPCapacity
#include "ACS_CS_CPId_GetCPCapacity.h"
//## begin module%4BE88C0303B4.declarations preserve=no
//## end module%4BE88C0303B4.declarations

//## begin module%4BE88C0303B4.additionalDeclarations preserve=yes
//## end module%4BE88C0303B4.additionalDeclarations


// Class ACS_CS_CPId_GetCPCapacity 

ACS_CS_CPId_GetCPCapacity::ACS_CS_CPId_GetCPCapacity()
  //## begin ACS_CS_CPId_GetCPCapacity::ACS_CS_CPId_GetCPCapacity%4BD9C0730339_const.hasinit preserve=no
      : m_cpId(ACS_CS_NS::ILLEGAL_SYSID)
  //## end ACS_CS_CPId_GetCPCapacity::ACS_CS_CPId_GetCPCapacity%4BD9C0730339_const.hasinit
  //## begin ACS_CS_CPId_GetCPCapacity::ACS_CS_CPId_GetCPCapacity%4BD9C0730339_const.initialization preserve=yes
  //## end ACS_CS_CPId_GetCPCapacity::ACS_CS_CPId_GetCPCapacity%4BD9C0730339_const.initialization
{
  //## begin ACS_CS_CPId_GetCPCapacity::ACS_CS_CPId_GetCPCapacity%4BD9C0730339_const.body preserve=yes
  //## end ACS_CS_CPId_GetCPCapacity::ACS_CS_CPId_GetCPCapacity%4BD9C0730339_const.body
}


ACS_CS_CPId_GetCPCapacity::~ACS_CS_CPId_GetCPCapacity()
{
  //## begin ACS_CS_CPId_GetCPCapacity::~ACS_CS_CPId_GetCPCapacity%4BD9C0730339_dest.body preserve=yes
  //## end ACS_CS_CPId_GetCPCapacity::~ACS_CS_CPId_GetCPCapacity%4BD9C0730339_dest.body
}



//## Other Operations (implementation)
 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetCPCapacity::getPrimitiveType () const
{
  //## begin ACS_CS_CPId_GetCPCapacity::getPrimitiveType%4BD9C78503B5.body preserve=yes
   return ACS_CS_Protocol::Primitive_GetCPCapacity;
  //## end ACS_CS_CPId_GetCPCapacity::getPrimitiveType%4BD9C78503B5.body
}

 int ACS_CS_CPId_GetCPCapacity::setBuffer (const char *buffer, int /*size*/)
{
  //## begin ACS_CS_CPId_GetCPCapacity::setBuffer%4BD9C7880384.body preserve=yes
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
  //## end ACS_CS_CPId_GetCPCapacity::setBuffer%4BD9C7880384.body
}

 int ACS_CS_CPId_GetCPCapacity::getBuffer (char *buffer, int size) const
{
  //## begin ACS_CS_CPId_GetCPCapacity::getBuffer%4BD9C78C01CE.body preserve=yes
   return ACS_CS_HostNetworkConverter::serialize(buffer, size, *this);
  //## end ACS_CS_CPId_GetCPCapacity::getBuffer%4BD9C78C01CE.body
}

 short unsigned ACS_CS_CPId_GetCPCapacity::getLength () const
{
  //## begin ACS_CS_CPId_GetCPCapacity::getLength%4BD9C78F023A.body preserve=yes
   return sizeof(*this);
  //## end ACS_CS_CPId_GetCPCapacity::getLength%4BD9C78F023A.body
}

 ACS_CS_Primitive * ACS_CS_CPId_GetCPCapacity::clone () const
{
  //## begin ACS_CS_CPId_GetCPCapacity::clone%4BD9C7920352.body preserve=yes
   return new ACS_CS_CPId_GetCPCapacity(*this);
  //## end ACS_CS_CPId_GetCPCapacity::clone%4BD9C7920352.body
}

 void ACS_CS_CPId_GetCPCapacity::setCpId (const CPID cpId)
{
  //## begin ACS_CS_CPId_GetCPCapacity::setCpId%4BD9DA72005B.body preserve=yes
   m_cpId = cpId;
  //## end ACS_CS_CPId_GetCPCapacity::setCpId%4BD9DA72005B.body
}

 CPID ACS_CS_CPId_GetCPCapacity::getCpId () const
{
  //## begin ACS_CS_CPId_GetCPCapacity::getCpId%4BD9DA7E0250.body preserve=yes
   return m_cpId;
  //## end ACS_CS_CPId_GetCPCapacity::getCpId%4BD9DA7E0250.body
}

// Additional Declarations
  //## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.declarations preserve=yes
  //## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.declarations

//## begin module%4BE88C0303B4.epilog preserve=yes
//## end module%4BE88C0303B4.epilog
