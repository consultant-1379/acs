//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE8906D02CD.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE8906D02CD.cm

//## begin module%4BE8906D02CD.cp preserve=no
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
//## end module%4BE8906D02CD.cp

//## Module: ACS_CS_CPId_GetCPCapacityResponse%4BE8906D02CD; Package specification
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetCPCapacityResponse.h

#ifndef ACS_CS_CPId_GetCPCapacityResponse_h
#define ACS_CS_CPId_GetCPCapacityResponse_h 1

//## begin module%4BE8906D02CD.additionalIncludes preserve=no
//## end module%4BE8906D02CD.additionalIncludes

//## begin module%4BE8906D02CD.includes preserve=yes
//## end module%4BE8906D02CD.includes

// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"
//## begin module%4BE8906D02CD.declarations preserve=no
//## end module%4BE8906D02CD.declarations

//## begin module%4BE8906D02CD.additionalDeclarations preserve=yes
//## end module%4BE8906D02CD.additionalDeclarations


//## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.preface preserve=yes
//## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.preface

//## Class: ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110
//## Category: cs_protocol::cpid%45C349AE013A
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class ACS_CS_CPId_GetCPCapacityResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4BD9D6570229
{
  //## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.initialDeclarations preserve=yes
  //## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.initialDeclarations

  public:
    //## Constructors (generated)
      ACS_CS_CPId_GetCPCapacityResponse();

    //## Destructor (generated)
      virtual ~ACS_CS_CPId_GetCPCapacityResponse();


    //## Other Operations (specified)
      //## Operation: getPrimitiveType%4BD9D8940058
      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      //## Operation: setBuffer%4BD9D89A0104
      virtual int setBuffer (const char *buffer, int size);

      //## Operation: getBuffer%4BD9D89D0078
      virtual int getBuffer (char *buffer, int size) const;

      //## Operation: getLength%4BD9D8A00114
      virtual unsigned short getLength () const;

      //## Operation: clone%4BD9D8A501A1
      virtual ACS_CS_Primitive * clone () const;

      //## Operation: getCPCapacity%4BD9DBED01F4
      ACS_CS_API_NS::CpCapacity getCPCapacity () const;

      //## Operation: setCPCapacity%4BD9DBFF0129
      void setCPCapacity (const ACS_CS_API_NS::CpCapacity cpCapacity);

    // Additional Public Declarations
      //## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.public preserve=yes
      //## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.public

  protected:
    // Additional Protected Declarations
      //## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.protected preserve=yes
      //## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.protected

  private:
    //## Assignment Operation (generated)
      ACS_CS_CPId_GetCPCapacityResponse & operator=(const ACS_CS_CPId_GetCPCapacityResponse &right);

    // Additional Private Declarations
      //## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.private preserve=yes
      //## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: m_cpCapacity%4BD9F0E00216
      //## begin ACS_CS_CPId_GetCPCapacityResponse::m_cpCapacity%4BD9F0E00216.attr preserve=no  private:  ACS_CS_API_NS::CpCapacity {U} 0
       ACS_CS_API_NS::CpCapacity m_cpCapacity;
      //## end ACS_CS_CPId_GetCPCapacityResponse::m_cpCapacity%4BD9F0E00216.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.implementation preserve=yes
      //## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.implementation

};

//## begin ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.postscript preserve=yes
//## end ACS_CS_CPId_GetCPCapacityResponse%4BD9D6230110.postscript

// Class ACS_CS_CPId_GetCPCapacityResponse 

//## begin module%4BE8906D02CD.epilog preserve=yes
//## end module%4BE8906D02CD.epilog


#endif
