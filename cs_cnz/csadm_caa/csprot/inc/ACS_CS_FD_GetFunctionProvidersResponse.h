

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_FD_GetFunctionProvidersResponse_h
#define ACS_CS_FD_GetFunctionProvidersResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_FD_GetFunctionProvidersResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45DDA68D0394
{

  public:
      ACS_CS_FD_GetFunctionProvidersResponse();

      ACS_CS_FD_GetFunctionProvidersResponse(const ACS_CS_FD_GetFunctionProvidersResponse &right);

      virtual ~ACS_CS_FD_GetFunctionProvidersResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      int getResponseArray (unsigned short *array, unsigned short size) const;

      unsigned short getResponseCount () const;

      int setResponseArray (const unsigned short *array, unsigned short size);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_FD_GetFunctionProvidersResponse & operator=(const ACS_CS_FD_GetFunctionProvidersResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       short unsigned *apArray;

       short unsigned apCount;

    // Additional Implementation Declarations

};


// Class ACS_CS_FD_GetFunctionProvidersResponse 



#endif
