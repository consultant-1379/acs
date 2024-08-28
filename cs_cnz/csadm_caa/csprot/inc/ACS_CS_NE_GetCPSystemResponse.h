

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetCPSystemResponse_h
#define ACS_CS_NE_GetCPSystemResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetCPSystemResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45CC3AA80377
{

  public:
      ACS_CS_NE_GetCPSystemResponse();

      ACS_CS_NE_GetCPSystemResponse(const ACS_CS_NE_GetCPSystemResponse &right);

      virtual ~ACS_CS_NE_GetCPSystemResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      ACS_CS_Protocol::CS_CP_System_Identifier getCPSystem () const;

      void setCPSystem (ACS_CS_Protocol::CS_CP_System_Identifier system);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetCPSystemResponse & operator=(const ACS_CS_NE_GetCPSystemResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_Protocol::CS_CP_System_Identifier cpSystem;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetCPSystemResponse 



#endif
