

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************




// ACS_CS_SM_NotifyObject
#include "ACS_CS_SM_NotifyObject.h"

#include <vector>


// Class ACS_CS_SM_NotifyObject 

ACS_CS_SM_NotifyObject::ACS_CS_SM_NotifyObject()
      : m_length(0)
{
}

ACS_CS_SM_NotifyObject::ACS_CS_SM_NotifyObject(const ACS_CS_SM_NotifyObject &right)
   : ACS_CS_Primitive(right),
    m_length(right.m_length)
{
}

ACS_CS_SM_NotifyObject::~ACS_CS_SM_NotifyObject()
{
}

 short unsigned ACS_CS_SM_NotifyObject::getLength () const
{
   if( m_length == 0 )
   {
      static const int maxSize = 1024;
      std::vector<char> buffer(maxSize);
      this->getBuffer(&buffer[0],maxSize);
   }

   return m_length;

}

 void ACS_CS_SM_NotifyObject::setLength (unsigned short length)
{
	m_length = length;
}

// Additional Declarations

