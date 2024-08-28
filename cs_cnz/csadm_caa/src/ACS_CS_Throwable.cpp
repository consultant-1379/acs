//	Copyright Ericsson AB 2007. All rights reserved.

// ACS_CS_Throwable
#include "ACS_CS_Throwable.h"


// Class ACS_CS_Throwable 

ACS_CS_Throwable::ACS_CS_Throwable()
{
}

ACS_CS_Throwable::ACS_CS_Throwable(const ACS_CS_Throwable &right)
  : std::exception()
{

   this->text = right.text;

}


ACS_CS_Throwable::~ACS_CS_Throwable() throw()
{
}



 std::string ACS_CS_Throwable::description ()
{
   return text;
}

 void ACS_CS_Throwable::setDescription (std::string message)
{
   text = message;
}

