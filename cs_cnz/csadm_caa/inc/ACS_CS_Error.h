

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_Error_h
#define ACS_CS_Error_h 1



// ACS_CS_Throwable
#include "ACS_CS_Throwable.h"







class ACS_CS_Error : public ACS_CS_Throwable  //## Inherits: <unnamed>%45AB7F5800C9
{

  public:
      ACS_CS_Error();

      ACS_CS_Error(const ACS_CS_Error &right);

      virtual ~ACS_CS_Error() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_Error & operator=(const ACS_CS_Error &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_Error 



#endif
