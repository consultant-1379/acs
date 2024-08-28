//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4BE88F2302CE.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4BE88F2302CE.cm

//## begin module%4BE88F2302CE.cp preserve=no
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
//## end module%4BE88F2302CE.cp

//## Module: ACS_CS_CPId_GetBlockingInfoResponse%4BE88F2302CE; Package specification
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Source file: c:\ccviews\eandboe_hl78796_2ss\ntacs\cs_cnz\csprot_caa\src\ACS_CS_CPId_GetBlockingInfoResponse.h

#ifndef ACS_CS_CPId_GetBlockingInfoResponse_h
#define ACS_CS_CPId_GetBlockingInfoResponse_h 1

//## begin module%4BE88F2302CE.additionalIncludes preserve=no
//## end module%4BE88F2302CE.additionalIncludes

//## begin module%4BE88F2302CE.includes preserve=yes
//## end module%4BE88F2302CE.includes

// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"
#include "ACS_CS_API_Common_R1.h"
//## begin module%4BE88F2302CE.declarations preserve=no
//## end module%4BE88F2302CE.declarations

//## begin module%4BE88F2302CE.additionalDeclarations preserve=yes
//## end module%4BE88F2302CE.additionalDeclarations


//## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.preface preserve=yes
//## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.preface

//## Class: ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1
//## Category: cs_protocol::cpid%45C349AE013A
//## Subsystem: ACS_CS::csprot_caa::src%44ACBCF50111
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class ACS_CS_CPId_GetBlockingInfoResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4BD9D64D0120
{
  //## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.initialDeclarations preserve=yes
  //## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.initialDeclarations

  public:
    //## Constructors (generated)
      ACS_CS_CPId_GetBlockingInfoResponse();

    //## Destructor (generated)
      virtual ~ACS_CS_CPId_GetBlockingInfoResponse();


    //## Other Operations (specified)
      //## Operation: getPrimitiveType%4BD9D7BD023B
      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      //## Operation: setBuffer%4BD9D7C502E7
      virtual int setBuffer (const char *buffer, int size);

      //## Operation: getBuffer%4BD9D7C80345
      virtual int getBuffer (char *buffer, int size) const;

      //## Operation: getLength%4BD9D7CB0335
      virtual unsigned short getLength () const;

      //## Operation: clone%4BD9D82300F4
      virtual ACS_CS_Primitive * clone () const;

      //## Operation: getBlockingInfo%4BD9D8340067
      ACS_CS_API_NS::BlockingInfo getBlockingInfo () const;

      //## Operation: setBlockingInfo%4BD9D92A01D0
      void setBlockingInfo (const ACS_CS_API_NS::BlockingInfo blockingInfo);

    // Additional Public Declarations
      //## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.public preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.public

  protected:
    // Additional Protected Declarations
      //## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.protected preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.protected

  private:
    //## Assignment Operation (generated)
      ACS_CS_CPId_GetBlockingInfoResponse & operator=(const ACS_CS_CPId_GetBlockingInfoResponse &right);

    // Additional Private Declarations
      //## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.private preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: m_blockingInfo%4BD9F05B0273
      //## begin ACS_CS_CPId_GetBlockingInfoResponse::m_blockingInfo%4BD9F05B0273.attr preserve=no  private:  ACS_CS_API_NS::BlockingInfo {U} 0
       ACS_CS_API_NS::BlockingInfo m_blockingInfo;
      //## end ACS_CS_CPId_GetBlockingInfoResponse::m_blockingInfo%4BD9F05B0273.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.implementation preserve=yes
      //## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.implementation

};

//## begin ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.postscript preserve=yes
//## end ACS_CS_CPId_GetBlockingInfoResponse%4BD9D60200D1.postscript

// Class ACS_CS_CPId_GetBlockingInfoResponse 

//## begin module%4BE88F2302CE.epilog preserve=yes
//## end module%4BE88F2302CE.epilog


#endif
