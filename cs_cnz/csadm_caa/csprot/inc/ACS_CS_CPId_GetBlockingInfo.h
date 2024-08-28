//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE88966003A.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE88966003A.cm

//## begin module%4BE88966003A.cp preserve=no
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
//## end module%4BE88966003A.cp

//## Module: ACS_CS_CPId_GetBlockingInfo%4BE88966003A; Package specification
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetBlockingInfo.h

#ifndef ACS_CS_CPId_GetBlockingInfo_h
#define ACS_CS_CPId_GetBlockingInfo_h 1

//## begin module%4BE88966003A.additionalIncludes preserve=no
//## end module%4BE88966003A.additionalIncludes

//## begin module%4BE88966003A.includes preserve=yes
//## end module%4BE88966003A.includes

// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"
//## begin module%4BE88966003A.declarations preserve=no
//## end module%4BE88966003A.declarations

//## begin module%4BE88966003A.additionalDeclarations preserve=yes
//## end module%4BE88966003A.additionalDeclarations


//## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.preface preserve=yes
//## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.preface

//## Class: ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0
//## Category: cs_protocol::cpid%45C349AE013A
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class ACS_CS_CPId_GetBlockingInfo : public ACS_CS_Primitive  //## Inherits: <unnamed>%4BD9BF290058
{
  //## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.initialDeclarations preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.initialDeclarations

  public:
    //## Constructors (generated)
      ACS_CS_CPId_GetBlockingInfo();

    //## Destructor (generated)
      virtual ~ACS_CS_CPId_GetBlockingInfo();


    //## Other Operations (specified)
      //## Operation: getPrimitiveType%4BD9C6D0026A
      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      //## Operation: setBuffer%4BD9C6DC00F4
      virtual int setBuffer (const char *buffer, int size);

      //## Operation: getBuffer%4BD9C6E20090
      virtual int getBuffer (char *buffer, int size) const;

      //## Operation: getLength%4BD9C6E80182
      virtual unsigned short getLength () const;

      //## Operation: clone%4BD9C6EE01BA
      virtual ACS_CS_Primitive * clone () const;

      //## Operation: setCpId%4BD9D97E0367
      void setCpId (const CPID cpId);

      //## Operation: getCpId%4BD9D9900116
      CPID getCpId () const;

    // Additional Public Declarations
      //## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.public preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.public

  protected:
    // Additional Protected Declarations
      //## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.protected preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.protected

  private:
    //## Assignment Operation (generated)
      ACS_CS_CPId_GetBlockingInfo & operator=(const ACS_CS_CPId_GetBlockingInfo &right);

    // Additional Private Declarations
      //## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.private preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: m_cpId%4BD9E4190136
      //## begin ACS_CS_CPId_GetBlockingInfo::m_cpId%4BD9E4190136.attr preserve=no  private:  CPID {U} ACS_CS_NS::ILLEGAL_SYSID
       CPID m_cpId;
      //## end ACS_CS_CPId_GetBlockingInfo::m_cpId%4BD9E4190136.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.implementation preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.implementation

};

//## begin ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.postscript preserve=yes
//## end ACS_CS_CPId_GetBlockingInfo%4BD9BEFA01B0.postscript

// Class ACS_CS_CPId_GetBlockingInfo 

//## begin module%4BE88966003A.epilog preserve=yes
//## end module%4BE88966003A.epilog


#endif
