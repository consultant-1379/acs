

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_SystemException_h
#define ACS_CS_SystemException_h 1



// ACS_CS_Exception
#include "ACS_CS_Exception.h"







class ACS_CS_SystemException : public ACS_CS_Exception  //## Inherits: <unnamed>%45AB7F4F00D8
{

  public:
      ACS_CS_SystemException();

      ACS_CS_SystemException(const ACS_CS_SystemException &right);

      virtual ~ACS_CS_SystemException() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_SystemException & operator=(const ACS_CS_SystemException &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_SystemException 



#endif
