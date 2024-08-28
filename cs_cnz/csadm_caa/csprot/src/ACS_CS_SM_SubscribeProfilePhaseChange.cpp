

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
#include "ACS_CS_API.h"

// ACS_CS_SM_SubscribeProfilePhaseChange
#include "ACS_CS_SM_SubscribeProfilePhaseChange.h"

ACS_CS_Trace_TDEF(ACS_CS_SetSubscriptionObject_TRACE);


// Class ACS_CS_SM_SubscribeProfilePhaseChange 

ACS_CS_SM_SubscribeProfilePhaseChange::ACS_CS_SM_SubscribeProfilePhaseChange()
  : m_phase(ACS_CS_API_OmProfilePhase::Idle)
{
}

ACS_CS_SM_SubscribeProfilePhaseChange::ACS_CS_SM_SubscribeProfilePhaseChange(const ACS_CS_SM_SubscribeProfilePhaseChange &right)
   : ACS_CS_SM_SubscriptionObject(right),
     m_phase(right.m_phase)
{
}


ACS_CS_SM_SubscribeProfilePhaseChange::~ACS_CS_SM_SubscribeProfilePhaseChange()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_SM_SubscribeProfilePhaseChange::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_SubscribeProfilePhaseChange;
}

 int ACS_CS_SM_SubscribeProfilePhaseChange::setBuffer (const char *buffer, int /*size*/)
{
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
}

 int ACS_CS_SM_SubscribeProfilePhaseChange::getBuffer (char *buffer, int size) const
{
   return ACS_CS_HostNetworkConverter::serialize(buffer,size,*this);
}

 short unsigned ACS_CS_SM_SubscribeProfilePhaseChange::getLength () const
{
   return sizeof(*this);
}

 ACS_CS_Primitive * ACS_CS_SM_SubscribeProfilePhaseChange::clone () const
{
   return new ACS_CS_SM_SubscribeProfilePhaseChange(*this);
}

 ACS_CS_API_OmProfilePhase::PhaseValue ACS_CS_SM_SubscribeProfilePhaseChange::getPhase () const
{
   return m_phase;
}

 void ACS_CS_SM_SubscribeProfilePhaseChange::setPhase (const ACS_CS_API_OmProfilePhase::PhaseValue phase)
{
   m_phase = phase;
}

// Additional Declarations

