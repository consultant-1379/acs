

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_ParsingException_h
#define ACS_CS_ParsingException_h 1



// ACS_CS_ProtocolException
#include "ACS_CS_ProtocolException.h"







class ACS_CS_ParsingException : public ACS_CS_ProtocolException  //## Inherits: <unnamed>%45AB7F2C0146
{

  public:
      ACS_CS_ParsingException();

      ACS_CS_ParsingException(const ACS_CS_ParsingException &right);

      virtual ~ACS_CS_ParsingException() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_ParsingException & operator=(const ACS_CS_ParsingException &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_ParsingException 



#endif
