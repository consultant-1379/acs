

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



#include "ACS_CS_HostNetworkConverter.h"
#include "ACS_CS_API.h"

// ACS_CS_SM_ProfilePhaseNotify
#include "ACS_CS_SM_ProfilePhaseNotify.h"



// Class ACS_CS_SM_ProfilePhaseNotify 

ACS_CS_SM_ProfilePhaseNotify::ACS_CS_SM_ProfilePhaseNotify()
// API object omProfileChange is defined as struct on purpose without default values...
{
   m_omProfileChange.omProfileCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
   m_omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_omProfileChange.apzCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
   m_omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_omProfileChange.aptCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
   m_omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;
   m_omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;
   m_omProfileChange.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_omProfileChange.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
}

ACS_CS_SM_ProfilePhaseNotify::ACS_CS_SM_ProfilePhaseNotify(const ACS_CS_SM_ProfilePhaseNotify &right)
   : ACS_CS_SM_NotifyObject(right),
     m_omProfileChange(right.m_omProfileChange)
{
}


ACS_CS_SM_ProfilePhaseNotify::~ACS_CS_SM_ProfilePhaseNotify()
{
}



 void ACS_CS_SM_ProfilePhaseNotify::getOmProfileChange (ACS_CS_API_OmProfileChange &omChange) const
{
   omChange = m_omProfileChange;
}

 void ACS_CS_SM_ProfilePhaseNotify::setOmProfileChange (const ACS_CS_API_OmProfileChange &omProfileChange)
{
   m_omProfileChange = omProfileChange;
   m_length = 0;
   this->getLength();
}

 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_SM_ProfilePhaseNotify::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_NotifyProfilePhaseChange;
}

 int ACS_CS_SM_ProfilePhaseNotify::setBuffer (const char *buffer, int /*size*/)
{
   return ACS_CS_HostNetworkConverter::deserialize(buffer,*this);
}

 int ACS_CS_SM_ProfilePhaseNotify::getBuffer (char *buffer, int size) const
{
   int rc = ACS_CS_HostNetworkConverter::serialize(buffer,size,*this);
   if( rc == -1)
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

 ACS_CS_Primitive * ACS_CS_SM_ProfilePhaseNotify::clone () const
{
   return new ACS_CS_SM_ProfilePhaseNotify(*this);
}

// Additional Declarations


