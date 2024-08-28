

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPIdResponse_h
#define ACS_CS_CPId_GetCPIdResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetCPIdResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45C86C4101B6
{

  public:
      ACS_CS_CPId_GetCPIdResponse();

      ACS_CS_CPId_GetCPIdResponse(const ACS_CS_CPId_GetCPIdResponse &right);

      virtual ~ACS_CS_CPId_GetCPIdResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setCPId (unsigned short identifier);

      unsigned short getCPId () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetCPIdResponse & operator=(const ACS_CS_CPId_GetCPIdResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned cpId;

       short unsigned logicalClock;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPIdResponse 



#endif
