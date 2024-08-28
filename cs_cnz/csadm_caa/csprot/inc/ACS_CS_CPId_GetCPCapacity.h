//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE88BEC02C8.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE88BEC02C8.cm

//## begin module%4BE88BEC02C8.cp preserve=no
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
//## end module%4BE88BEC02C8.cp

//## Module: ACS_CS_CPId_GetCPCapacity%4BE88BEC02C8; Package specification
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetCPCapacity.h

#ifndef ACS_CS_CPId_GetCPCapacity_h
#define ACS_CS_CPId_GetCPCapacity_h 1

//## begin module%4BE88BEC02C8.additionalIncludes preserve=no
//## end module%4BE88BEC02C8.additionalIncludes

//## begin module%4BE88BEC02C8.includes preserve=yes
//## end module%4BE88BEC02C8.includes

// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"
//## begin module%4BE88BEC02C8.declarations preserve=no
//## end module%4BE88BEC02C8.declarations

//## begin module%4BE88BEC02C8.additionalDeclarations preserve=yes
//## end module%4BE88BEC02C8.additionalDeclarations


//## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.preface preserve=yes
//## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.preface

//## Class: ACS_CS_CPId_GetCPCapacity%4BD9C0730339
//## Category: cs_protocol::cpid%45C349AE013A
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class ACS_CS_CPId_GetCPCapacity : public ACS_CS_Primitive  //## Inherits: <unnamed>%4BD9C0940116
{
  //## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.initialDeclarations preserve=yes
  //## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.initialDeclarations

  public:
    //## Constructors (generated)
      ACS_CS_CPId_GetCPCapacity();

    //## Destructor (generated)
      virtual ~ACS_CS_CPId_GetCPCapacity();


    //## Other Operations (specified)
      //## Operation: getPrimitiveType%4BD9C78503B5
      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      //## Operation: setBuffer%4BD9C7880384
      virtual int setBuffer (const char *buffer, int size);

      //## Operation: getBuffer%4BD9C78C01CE
      virtual int getBuffer (char *buffer, int size) const;

      //## Operation: getLength%4BD9C78F023A
      virtual unsigned short getLength () const;

      //## Operation: clone%4BD9C7920352
      virtual ACS_CS_Primitive * clone () const;

      //## Operation: setCpId%4BD9DA72005B
      void setCpId (const CPID cpId);

      //## Operation: getCpId%4BD9DA7E0250
      CPID getCpId () const;

    // Additional Public Declarations
      //## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.public preserve=yes
      //## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.public

  protected:
    // Additional Protected Declarations
      //## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.protected preserve=yes
      //## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.protected

  private:
    //## Assignment Operation (generated)
      ACS_CS_CPId_GetCPCapacity & operator=(const ACS_CS_CPId_GetCPCapacity &right);

    // Additional Private Declarations
      //## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.private preserve=yes
      //## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: m_cpId%4BD9E46C0379
      //## begin ACS_CS_CPId_GetCPCapacity::m_cpId%4BD9E46C0379.attr preserve=no  private:  CPID {U} ACS_CS_NS::ILLEGAL_SYSID
       CPID m_cpId;
      //## end ACS_CS_CPId_GetCPCapacity::m_cpId%4BD9E46C0379.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.implementation preserve=yes
      //## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.implementation

};

//## begin ACS_CS_CPId_GetCPCapacity%4BD9C0730339.postscript preserve=yes
//## end ACS_CS_CPId_GetCPCapacity%4BD9C0730339.postscript

// Class ACS_CS_CPId_GetCPCapacity 

//## begin module%4BE88BEC02C8.epilog preserve=yes
//## end module%4BE88BEC02C8.epilog


#endif
