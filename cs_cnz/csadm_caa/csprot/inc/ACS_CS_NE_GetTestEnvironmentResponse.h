

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetTestEnvironmentResponse_h
#define ACS_CS_NE_GetTestEnvironmentResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetTestEnvironmentResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45DC5AD00252
{

  public:
      ACS_CS_NE_GetTestEnvironmentResponse();

      ACS_CS_NE_GetTestEnvironmentResponse(const ACS_CS_NE_GetTestEnvironmentResponse &right);

      virtual ~ACS_CS_NE_GetTestEnvironmentResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      ACS_CS_Protocol::CS_TestEnvironment_Identifier getTestEnvironment () const;

      void setTestEnvironment (ACS_CS_Protocol::CS_TestEnvironment_Identifier identifier);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetTestEnvironmentResponse & operator=(const ACS_CS_NE_GetTestEnvironmentResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_Protocol::CS_TestEnvironment_Identifier testIdentifier;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetTestEnvironmentResponse 



#endif
