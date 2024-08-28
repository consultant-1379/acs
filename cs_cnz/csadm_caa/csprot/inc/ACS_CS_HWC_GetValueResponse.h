

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_HWC_GetValueResponse_h
#define ACS_CS_HWC_GetValueResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_HWC_GetValueResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45C707F701A7
{

  public:
      ACS_CS_HWC_GetValueResponse();

      ACS_CS_HWC_GetValueResponse(const ACS_CS_HWC_GetValueResponse &right);

      virtual ~ACS_CS_HWC_GetValueResponse();


      unsigned short getResponseLength () const;

      int getResponse (char *buffer, int size) const;

      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int getBuffer (char *buffer, int size) const;

      int setBuffer (const char *buffer, int size);

      int setResponse (const char *buffer, int size);

      void setLogicalClock (unsigned short clockValue);

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_HWC_GetValueResponse & operator=(const ACS_CS_HWC_GetValueResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       char *response;

       short unsigned responseLength;

       short unsigned logicalClock;

    // Additional Implementation Declarations
	   struct getValueResponseBuffer
		{
			unsigned short resultCode;
			unsigned short logicalClock;
			unsigned short responseLength;
		};
};


// Class ACS_CS_HWC_GetValueResponse 



#endif
