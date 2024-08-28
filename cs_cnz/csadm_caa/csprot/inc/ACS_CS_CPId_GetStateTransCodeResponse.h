

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


#ifndef ACS_CS_CPId_GetStateTransCodeResponse_h
#define ACS_CS_CPId_GetStateTransCodeResponse_h 1


#include "ACS_CS_API.h"

// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetStateTransCodeResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4A941F8F0068
{

  public:
      ACS_CS_CPId_GetStateTransCodeResponse();

      virtual ~ACS_CS_CPId_GetStateTransCodeResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_API_NS::StateTransition getStateTransCode () const;

      void setStateTransCode (ACS_CS_API_NS::StateTransition newStateTransCode);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetStateTransCodeResponse(const ACS_CS_CPId_GetStateTransCodeResponse &right);

      ACS_CS_CPId_GetStateTransCodeResponse & operator=(const ACS_CS_CPId_GetStateTransCodeResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_API_NS::StateTransition stateTransCode;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetStateTransCodeResponse 



#endif
