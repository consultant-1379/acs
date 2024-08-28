//	Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_Throwable_h
#define ACS_CS_Throwable_h 1


#include <string>


// std\:\:exception
#include "stdexception.h"


class ACS_CS_Throwable : public std::exception  //## Inherits: <unnamed>%45AB7FBC025F
{

  public:
      ACS_CS_Throwable();

      ACS_CS_Throwable(const ACS_CS_Throwable &right);

      virtual ~ACS_CS_Throwable() throw();

      virtual std::string description ();

      virtual void setDescription (std::string message);

  protected:
       std::string text;

  private:
      ACS_CS_Throwable & operator=(const ACS_CS_Throwable &right);


  private: //## implementation

};

#endif
