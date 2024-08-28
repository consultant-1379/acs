

//	© Copyright Ericsson AB 2014. All rights reserved.


#ifndef ACS_CS_CPId_GetMAUTypeResponse_h
#define ACS_CS_CPId_GetMAUTypeResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"


class ACS_CS_CPId_GetMAUTypeResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4669526701D3
{

  public:
      ACS_CS_CPId_GetMAUTypeResponse();

      ACS_CS_CPId_GetMAUTypeResponse(const ACS_CS_CPId_GetMAUTypeResponse &right);

      virtual ~ACS_CS_CPId_GetMAUTypeResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      ACS_CS_Protocol::CS_MAU_Type_Identifier getMAUType () const;

      void setMAUType (ACS_CS_Protocol::CS_MAU_Type_Identifier type);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetMAUTypeResponse & operator=(const ACS_CS_CPId_GetMAUTypeResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       ACS_CS_Protocol::CS_MAU_Type_Identifier  mauType;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetMAUTypeResponse 



#endif
