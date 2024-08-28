

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


#ifndef ACS_CS_NE_GetOmProfile_h
#define ACS_CS_NE_GetOmProfile_h 1



// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"







class ACS_CS_NE_GetOmProfile : public ACS_CS_NE_BasicGet  //## Inherits: <unnamed>%4A84A0640180
{

  public:
      ACS_CS_NE_GetOmProfile();

      virtual ~ACS_CS_NE_GetOmProfile();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetOmProfile(const ACS_CS_NE_GetOmProfile &right);

      ACS_CS_NE_GetOmProfile & operator=(const ACS_CS_NE_GetOmProfile &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetOmProfile 



#endif
