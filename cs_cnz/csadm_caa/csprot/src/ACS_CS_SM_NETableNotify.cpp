

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



#include "ACS_CS_Trace.h"
#include "ACS_CS_API.h"

// ACS_CS_SM_NETableNotify
#include "ACS_CS_SM_NETableNotify.h"

ACS_CS_Trace_TDEF(ACS_CS_NENotify_TRACE);


// Class ACS_CS_SM_NETableNotify 

ACS_CS_SM_NETableNotify::ACS_CS_SM_NETableNotify()
{
}

ACS_CS_SM_NETableNotify::ACS_CS_SM_NETableNotify(const ACS_CS_SM_NETableNotify &right)
   : ACS_CS_SM_NotifyObject(right),
     m_neChange(right.m_neChange)
{
}


ACS_CS_SM_NETableNotify::~ACS_CS_SM_NETableNotify()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_SM_NETableNotify::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_NotifyNeTableChange;
}

 int ACS_CS_SM_NETableNotify::setBuffer (const char *buffer, int /*size*/)
{
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
}

 int ACS_CS_SM_NETableNotify::getBuffer (char *buffer, int size) const
{
   int rc = ACS_CS_HostNetworkConverter::serialize(buffer,size,*this);
   if( rc == -1 )
   {
      m_length = 0;
      return -1;
   }
   else
   {
      m_length = size;
   }

   return 0;
}

 ACS_CS_Primitive * ACS_CS_SM_NETableNotify::clone () const
{
   return new ACS_CS_SM_NETableNotify(*this);
}

 void ACS_CS_SM_NETableNotify::getNeChange (ACS_CS_NetworkElementChange &neChange) const
{
	neChange = m_neChange;
}

 void ACS_CS_SM_NETableNotify::setNeChange (const ACS_CS_NetworkElementChange &neChange)
{
	m_neChange = neChange;
   m_length = 0;
   this->getLength();
}

// Additional Declarations

