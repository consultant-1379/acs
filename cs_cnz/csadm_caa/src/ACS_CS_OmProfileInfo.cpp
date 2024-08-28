





// ACS_CS_OmProfileInfo
#include "ACS_CS_OmProfileInfo.h"



// Class ACS_CS_OmProfileInfo 

ACS_CS_OmProfileInfo::ACS_CS_OmProfileInfo()
      : m_omProfileCurrent(0),
        m_omProfileRequested(0),
        m_aptCurrent(0),
        m_aptRequested(0),
        m_apzCurrent(0),
        m_apzRequested(0),
        m_notificationPhase(3),
        m_nProcRegPh0(0),
        m_nProcRegPh1(0),
        m_nProcRegPh2(0)
{
}


ACS_CS_OmProfileInfo::~ACS_CS_OmProfileInfo()
{
}



 void ACS_CS_OmProfileInfo::handleOmProfileNotificationSuccess (unsigned phase)
{
}

 void ACS_CS_OmProfileInfo::handleOmProfileNotificationFailure (unsigned phase)
{
}

 void ACS_CS_OmProfileInfo::setLocalProfiles (unsigned aptProfile, unsigned apzProfile)
{
}

// Additional Declarations

