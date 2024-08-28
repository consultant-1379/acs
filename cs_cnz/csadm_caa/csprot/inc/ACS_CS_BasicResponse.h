

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_BasicResponse_h
#define ACS_CS_BasicResponse_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_BasicResponse : public ACS_CS_Primitive  //## Inherits: <unnamed>%45C70B8903CA
{

  public:
      ACS_CS_BasicResponse();

      virtual ~ACS_CS_BasicResponse();


      virtual int setBuffer (const char *buffer, int size);

      virtual int getBuffer (char *buffer, int size) const;

      virtual unsigned short getLength () const;

      virtual void setResultCode (ACS_CS_Protocol::CS_Result_Code result);

      virtual ACS_CS_Protocol::CS_Result_Code getResultCode () const;

    // Additional Public Declarations

  protected:
      ACS_CS_BasicResponse(const ACS_CS_BasicResponse &right);

    // Data Members for Class Attributes

       ACS_CS_Protocol::CS_Result_Code resultCode;

    // Additional Protected Declarations

  private:
      ACS_CS_BasicResponse & operator=(const ACS_CS_BasicResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_BasicResponse 



#endif
