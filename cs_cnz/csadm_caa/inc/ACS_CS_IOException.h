

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_IOException_h
#define ACS_CS_IOException_h 1



// ACS_CS_Exception
#include "ACS_CS_Exception.h"







class ACS_CS_IOException : public ACS_CS_Exception  //## Inherits: <unnamed>%45AB7F4B005B
{

  public:
      ACS_CS_IOException();

      ACS_CS_IOException(const ACS_CS_IOException &right);

      virtual ~ACS_CS_IOException() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_IOException & operator=(const ACS_CS_IOException &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_IOException 



#endif
