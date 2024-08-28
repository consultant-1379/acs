

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetAPZSystemResponse_h
#define ACS_CS_CPId_GetAPZSystemResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetAPZSystemResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4669525E0146
{

  public:
      ACS_CS_CPId_GetAPZSystemResponse();

      ACS_CS_CPId_GetAPZSystemResponse(const ACS_CS_CPId_GetAPZSystemResponse &right);

      virtual ~ACS_CS_CPId_GetAPZSystemResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      unsigned short getAPZSystem () const;

      void setAPZSystem (unsigned short system);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetAPZSystemResponse & operator=(const ACS_CS_CPId_GetAPZSystemResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       short unsigned apzSystem;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetAPZSystemResponse 



#endif
