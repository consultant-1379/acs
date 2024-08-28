

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPNameResponse_h
#define ACS_CS_CPId_GetCPNameResponse_h 1



#include <string>


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetCPNameResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45C86C450291
{

  public:
      ACS_CS_CPId_GetCPNameResponse();

      ACS_CS_CPId_GetCPNameResponse(const ACS_CS_CPId_GetCPNameResponse &right);

      virtual ~ACS_CS_CPId_GetCPNameResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      std::string getName () const;

      void setName (std::string name);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetCPNameResponse & operator=(const ACS_CS_CPId_GetCPNameResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       std::string nameStr;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPNameResponse 



#endif
