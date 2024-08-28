

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
#include "ACS_CS_HostNetworkConverter.h"

// ACS_CS_SM_CPGroupTableNotify
#include "ACS_CS_SM_CPGroupTableNotify.h"

ACS_CS_Trace_TDEF(ACS_CS_CpGroupTableNotify_TRACE);


// Class ACS_CS_SM_CPGroupTableNotify 

ACS_CS_SM_CPGroupTableNotify::ACS_CS_SM_CPGroupTableNotify()
{
}

ACS_CS_SM_CPGroupTableNotify::ACS_CS_SM_CPGroupTableNotify(const ACS_CS_SM_CPGroupTableNotify &right)
   : ACS_CS_SM_NotifyObject(right),
     m_cpGroupChange(right.m_cpGroupChange)
{
}


ACS_CS_SM_CPGroupTableNotify::~ACS_CS_SM_CPGroupTableNotify()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_SM_CPGroupTableNotify::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_NotifyCpGroupTableChange;
}

 int ACS_CS_SM_CPGroupTableNotify::setBuffer (const char *buffer, int /*size*/)
{
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
}

 int ACS_CS_SM_CPGroupTableNotify::getBuffer (char *buffer, int size) const
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

 ACS_CS_Primitive * ACS_CS_SM_CPGroupTableNotify::clone () const
{
   return new ACS_CS_SM_CPGroupTableNotify(*this);
}

 void ACS_CS_SM_CPGroupTableNotify::getCpGroupChange (ACS_CS_CpGroupChange &cpGroupChange) const
{

    cpGroupChange = m_cpGroupChange;

}

 void ACS_CS_SM_CPGroupTableNotify::setCpGroupChange (const ACS_CS_CpGroupChange &cpGroupChange)
{

   m_cpGroupChange = cpGroupChange;
   m_length = 0;
   this->getLength();

}

// Additional Declarations

