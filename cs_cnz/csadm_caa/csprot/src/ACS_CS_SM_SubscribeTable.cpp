

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

// ACS_CS_SM_SubscribeTable
#include "ACS_CS_SM_SubscribeTable.h"

ACS_CS_Trace_TDEF(ACS_CS_SetSubscriptionTable_TRACE);


// Class ACS_CS_SM_SubscribeTable 

ACS_CS_SM_SubscribeTable::ACS_CS_SM_SubscribeTable()
   : m_tableScope(ACS_CS_Protocol::Scope_NotSpecified)
{
}

ACS_CS_SM_SubscribeTable::ACS_CS_SM_SubscribeTable(const ACS_CS_SM_SubscribeTable &right)
   : ACS_CS_SM_SubscriptionObject(right),
     m_tableScope(right.m_tableScope)
{
}


ACS_CS_SM_SubscribeTable::~ACS_CS_SM_SubscribeTable()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_SM_SubscribeTable::getPrimitiveType () const
{
	return ACS_CS_Protocol::Primitive_SubscribeTable;
}

 int ACS_CS_SM_SubscribeTable::setBuffer (const char *buffer, int /*size*/)
{
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
}

 int ACS_CS_SM_SubscribeTable::getBuffer (char *buffer, int size) const
{
   return ACS_CS_HostNetworkConverter::serialize(buffer,size,*this);
}

 short unsigned ACS_CS_SM_SubscribeTable::getLength () const
{
   return sizeof(*this);
}

 ACS_CS_Primitive * ACS_CS_SM_SubscribeTable::clone () const
{
   return new ACS_CS_SM_SubscribeTable(*this);
}

 ACS_CS_Protocol::CS_Scope_Identifier ACS_CS_SM_SubscribeTable::getTableScope () const
{
   return m_tableScope;
}

 void ACS_CS_SM_SubscribeTable::setTableScope (ACS_CS_Protocol::CS_Scope_Identifier tableScope)
{
   m_tableScope = tableScope;
}

// Additional Declarations

