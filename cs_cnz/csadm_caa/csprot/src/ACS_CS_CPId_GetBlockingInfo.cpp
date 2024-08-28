//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE8899E0212.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE8899E0212.cm

//## begin module%4BE8899E0212.cp preserve=no
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
//## end module%4BE8899E0212.cp

//## Module: ACS_CS_CPId_GetBlockingInfo%4BE8899E0212; Package body
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetBlockingInfo.cpp

//## begin module%4BE8899E0212.additionalIncludes preserve=no
//## end module%4BE8899E0212.additionalIncludes

//## begin module%4BE8899E0212.includes preserve=yes
#include "ACS_CS_API.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_HostNetworkConverter.h"
//## end module%4BE8899E0212.includes

// ACS_CS_CPId_GetBlockingInfo
#include "ACS_CS_CPId_GetBlockingInfo.h"
//## begin module%4BE8899E0212.declarations preserve=no
//## end module%4BE8899E0212.declarations

//## begin module%4BE8899E0212.additionalDeclarations preserve=yes
//## end module%4BE8899E0212.additionalDeclarations


// Class ACS_CS_CPId_GetBlockingInfo 

ACS_CS_CPId_GetBlockingInfo::ACS_CS_CPId_GetBlockingInfo()
  //## begin ACS_CS_CPId_GetBlockingInfo::ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_const.hasinit preserve=no
      : m_cpId(ACS_CS_NS::ILLEGAL_SYSID)
  //## end ACS_CS_CPId_GetBlockingInfo::ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_const.hasinit
  //## begin ACS_CS_CPId_GetBlockingInfo::ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_const.initialization preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfo::ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_const.initialization
{
  //## begin ACS_CS_CPId_GetBlockingInfo::ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_const.body preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfo::ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_const.body
}


ACS_CS_CPId_GetBlockingInfo::~ACS_CS_CPId_GetBlockingInfo()
{
  //## begin ACS_CS_CPId_GetBlockingInfo::~ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_dest.body preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfo::~ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0_dest.body
}



//## Other Operations (implementation)
 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_CPId_GetBlockingInfo::getPrimitiveType () const
{
  //## begin ACS_CS_CPId_GetBlockingInfo::getPrimitiveType%4BD9C6D0026A.body preserve=yes
   return ACS_CS_Protocol::Primitive_GetBlockingInfo;
  //## end ACS_CS_CPId_GetBlockingInfo::getPrimitiveType%4BD9C6D0026A.body
}

 int ACS_CS_CPId_GetBlockingInfo::setBuffer (const char *buffer, int /*size*/)
{
  //## begin ACS_CS_CPId_GetBlockingInfo::setBuffer%4BD9C6DC00F4.body preserve=yes
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
  //## end ACS_CS_CPId_GetBlockingInfo::setBuffer%4BD9C6DC00F4.body
}

 int ACS_CS_CPId_GetBlockingInfo::getBuffer (char *buffer, int size) const
{
  //## begin ACS_CS_CPId_GetBlockingInfo::getBuffer%4BD9C6E20090.body preserve=yes
   return ACS_CS_HostNetworkConverter::serialize(buffer, size, *this);
  //## end ACS_CS_CPId_GetBlockingInfo::getBuffer%4BD9C6E20090.body
}

 short unsigned ACS_CS_CPId_GetBlockingInfo::getLength () const
{
  //## begin ACS_CS_CPId_GetBlockingInfo::getLength%4BD9C6E80182.body preserve=yes
   return sizeof(*this);
  //## end ACS_CS_CPId_GetBlockingInfo::getLength%4BD9C6E80182.body
}

 ACS_CS_Primitive * ACS_CS_CPId_GetBlockingInfo::clone () const
{
  //## begin ACS_CS_CPId_GetBlockingInfo::clone%4BD9C6EE01BA.body preserve=yes
   return new ACS_CS_CPId_GetBlockingInfo(*this);
  //## end ACS_CS_CPId_GetBlockingInfo::clone%4BD9C6EE01BA.body
}

 void ACS_CS_CPId_GetBlockingInfo::setCpId (const CPID cpId)
{
  //## begin ACS_CS_CPId_GetBlockingInfo::setCpId%4BD9D97E0367.body preserve=yes
   m_cpId = cpId;
  //## end ACS_CS_CPId_GetBlockingInfo::setCpId%4BD9D97E0367.body
}

 CPID ACS_CS_CPId_GetBlockingInfo::getCpId () const
{
  //## begin ACS_CS_CPId_GetBlockingInfo::getCpId%4BD9D9900116.body preserve=yes
   return m_cpId;
  //## end ACS_CS_CPId_GetBlockingInfo::getCpId%4BD9D9900116.body
}

// Additional Declarations
  //## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.declarations preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.declarations

//## begin module%4BE8899E0212.epilog preserve=yes
//## end module%4BE8899E0212.epilog
