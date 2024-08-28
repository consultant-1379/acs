//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_Exception_h
#define ACS_CS_Exception_h 1


// ACS_CS_Throwable
#include "ACS_CS_Throwable.h"


class ACS_CS_Exception : public ACS_CS_Throwable
{

  public:
      ACS_CS_Exception();

      ACS_CS_Exception(const ACS_CS_Exception &right);

      virtual ~ACS_CS_Exception() throw();

  protected:

  private:
      ACS_CS_Exception & operator=(const ACS_CS_Exception &right);


  private: //## implementation

};


// Class ACS_CS_Exception 



#endif
