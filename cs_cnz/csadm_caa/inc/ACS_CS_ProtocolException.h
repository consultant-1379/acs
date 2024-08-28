

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_ProtocolException_h
#define ACS_CS_ProtocolException_h 1



// ACS_CS_Exception
#include "ACS_CS_Exception.h"







class ACS_CS_ProtocolException : public ACS_CS_Exception  //## Inherits: <unnamed>%45AB7F4602CC
{

  public:
      ACS_CS_ProtocolException();

      ACS_CS_ProtocolException(const ACS_CS_ProtocolException &right);

      virtual ~ACS_CS_ProtocolException() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_ProtocolException & operator=(const ACS_CS_ProtocolException &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_ProtocolException 



#endif
