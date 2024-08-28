

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPTypeResponse_h
#define ACS_CS_CPId_GetCPTypeResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetCPTypeResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4669526701D3
{

  public:
      ACS_CS_CPId_GetCPTypeResponse();

      ACS_CS_CPId_GetCPTypeResponse(const ACS_CS_CPId_GetCPTypeResponse &right);

      virtual ~ACS_CS_CPId_GetCPTypeResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      unsigned short getCPType () const;

      void setCPType (unsigned short type);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetCPTypeResponse & operator=(const ACS_CS_CPId_GetCPTypeResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       short unsigned cpType;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPTypeResponse 



#endif
