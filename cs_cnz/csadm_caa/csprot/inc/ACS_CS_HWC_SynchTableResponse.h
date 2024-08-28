

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_HWC_SynchTableResponse_h
#define ACS_CS_HWC_SynchTableResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_HWC_SynchTableResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45C707ED030E
{

  public:
      ACS_CS_HWC_SynchTableResponse();

      ACS_CS_HWC_SynchTableResponse(const ACS_CS_HWC_SynchTableResponse &right);

      virtual ~ACS_CS_HWC_SynchTableResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_HWC_SynchTableResponse & operator=(const ACS_CS_HWC_SynchTableResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_HWC_SynchTableResponse 



#endif
