

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


#ifndef ACS_CS_CPId_GetAptSubstateResponse_h
#define ACS_CS_CPId_GetAptSubstateResponse_h 1


#include "ACS_CS_API.h"

// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetAptSubstateResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4A9416A40264
{

  public:
      ACS_CS_CPId_GetAptSubstateResponse();

      virtual ~ACS_CS_CPId_GetAptSubstateResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_API_NS::AptSubstate getAptSubstate () const;

      void setAptSubstate (ACS_CS_API_NS::AptSubstate newAptSubstate);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetAptSubstateResponse(const ACS_CS_CPId_GetAptSubstateResponse &right);

      ACS_CS_CPId_GetAptSubstateResponse & operator=(const ACS_CS_CPId_GetAptSubstateResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_API_NS::AptSubstate aptSubstate;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetAptSubstateResponse 



#endif
