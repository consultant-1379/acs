

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_VersionException_h
#define ACS_CS_VersionException_h 1



// ACS_CS_ProtocolException
#include "ACS_CS_ProtocolException.h"







class ACS_CS_VersionException : public ACS_CS_ProtocolException  //## Inherits: <unnamed>%45AB7F31003C
{

  public:
      ACS_CS_VersionException();

      ACS_CS_VersionException(const ACS_CS_VersionException &right);

      virtual ~ACS_CS_VersionException() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_VersionException & operator=(const ACS_CS_VersionException &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_VersionException 



#endif
