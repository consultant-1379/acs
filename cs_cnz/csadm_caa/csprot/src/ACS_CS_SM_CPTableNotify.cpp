

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

// ACS_CS_SM_CPTableNotify
#include "ACS_CS_SM_CPTableNotify.h"

ACS_CS_Trace_TDEF(ACS_CS_CPNotify_TRACE);


// Class ACS_CS_SM_CPTableNotify 

ACS_CS_SM_CPTableNotify::ACS_CS_SM_CPTableNotify()
{
}

ACS_CS_SM_CPTableNotify::ACS_CS_SM_CPTableNotify(const ACS_CS_SM_CPTableNotify &right)
   : ACS_CS_SM_NotifyObject(right),
     m_cpTableChange(right.m_cpTableChange)
{
}


ACS_CS_SM_CPTableNotify::~ACS_CS_SM_CPTableNotify()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_SM_CPTableNotify::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_NotifyCpidTableChange;
}

 int ACS_CS_SM_CPTableNotify::setBuffer (const char *buffer, int /*size*/)
{
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
}

 int ACS_CS_SM_CPTableNotify::getBuffer (char *buffer, int size) const
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

 ACS_CS_Primitive * ACS_CS_SM_CPTableNotify::clone () const
{
   return new ACS_CS_SM_CPTableNotify(*this);
}

 void ACS_CS_SM_CPTableNotify::getCpTableChange (ACS_CS_CpTableChange &cpTableChange) const
{
   cpTableChange = m_cpTableChange;
}

 void ACS_CS_SM_CPTableNotify::setCpTableChange (const ACS_CS_CpTableChange &cpTableChange)
{
   m_cpTableChange = cpTableChange;
   m_length = 0;
   this->getLength();  // Call getLength to initialize buffer.
}

// Additional Declarations

