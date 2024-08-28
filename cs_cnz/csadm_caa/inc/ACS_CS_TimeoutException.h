

//	 Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_TimeoutException_h
#define ACS_CS_TimeoutException_h 1



// ACS_CS_Exception
#include "ACS_CS_Exception.h"







class ACS_CS_TimeoutException : public ACS_CS_Exception  //## Inherits: <unnamed>%45AB7F41031B
{

  public:
      ACS_CS_TimeoutException();

      ACS_CS_TimeoutException(const ACS_CS_TimeoutException &right);

      virtual ~ACS_CS_TimeoutException() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_TimeoutException & operator=(const ACS_CS_TimeoutException &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_TimeoutException 



#endif
