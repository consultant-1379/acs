



#ifndef ACS_CS_OmProfileInfo_h
#define ACS_CS_OmProfileInfo_h 1



// ACS_CS_NEHandler
#include "ACS_CS_NEHandler.h"








class ACS_CS_OmProfileInfo 
{

  public:
      ACS_CS_OmProfileInfo();

      virtual ~ACS_CS_OmProfileInfo();


      void handleOmProfileNotificationSuccess (unsigned phase);

      void handleOmProfileNotificationFailure (unsigned phase);

      void setLocalProfiles (unsigned aptProfile, unsigned apzProfile);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_OmProfileInfo(const ACS_CS_OmProfileInfo &right);

      ACS_CS_OmProfileInfo & operator=(const ACS_CS_OmProfileInfo &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       unsigned m_omProfileCurrent;

       unsigned m_omProfileRequested;

       unsigned m_aptCurrent;

       unsigned m_aptRequested;

       unsigned m_apzCurrent;

       unsigned m_apzRequested;

       unsigned m_notificationPhase;

//       ACS_CS_OmProfileChange m_omProfileChange;

       int m_ph2status;

       unsigned m_nProcRegPh0;

       unsigned m_nProcRegPh1;

       unsigned m_nProcRegPh2;

    // Additional Implementation Declarations

};


// Class ACS_CS_OmProfileInfo 



#endif
