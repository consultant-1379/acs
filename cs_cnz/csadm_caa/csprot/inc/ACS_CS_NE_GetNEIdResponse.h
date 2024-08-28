

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetNEIdResponse_h
#define ACS_CS_NE_GetNEIdResponse_h 1



#include <string>


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetNEIdResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45CC3AAC0183
{

  public:
      ACS_CS_NE_GetNEIdResponse();

      ACS_CS_NE_GetNEIdResponse(const ACS_CS_NE_GetNEIdResponse &right);

      virtual ~ACS_CS_NE_GetNEIdResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setNEId (std::string neId);

      std::string getNEId () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetNEIdResponse & operator=(const ACS_CS_NE_GetNEIdResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       std::string neStr;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetNEIdResponse 



#endif
