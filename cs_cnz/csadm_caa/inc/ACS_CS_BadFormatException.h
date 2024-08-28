

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_BadFormatException_h
#define ACS_CS_BadFormatException_h 1



// ACS_CS_ProtocolException
#include "ACS_CS_ProtocolException.h"







class ACS_CS_BadFormatException : public ACS_CS_ProtocolException
{

  public:
      ACS_CS_BadFormatException();

      ACS_CS_BadFormatException(const ACS_CS_BadFormatException &right);

      virtual ~ACS_CS_BadFormatException();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_BadFormatException & operator=(const ACS_CS_BadFormatException &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_BadFormatException 



#endif
