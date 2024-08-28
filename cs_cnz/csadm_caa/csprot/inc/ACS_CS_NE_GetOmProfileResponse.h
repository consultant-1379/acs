

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************


#ifndef ACS_CS_NE_GetOmProfileResponse_h
#define ACS_CS_NE_GetOmProfileResponse_h 1


#include "ACS_CS_API.h"

// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetOmProfileResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4A849E9B00CE
{

  public:
      ACS_CS_NE_GetOmProfileResponse();

      virtual ~ACS_CS_NE_GetOmProfileResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      void getOmProfile (ACS_CS_API_OmProfileChange& omProfileChange) const;

      void setOmProfile (const ACS_CS_API_OmProfileChange& omProfileChange);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetOmProfileResponse(const ACS_CS_NE_GetOmProfileResponse &right);

      ACS_CS_NE_GetOmProfileResponse & operator=(const ACS_CS_NE_GetOmProfileResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_API_OmProfileChange m_OmProfileChange;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetOmProfileResponse 



#endif
