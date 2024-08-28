

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


#ifndef ACS_CS_SM_SubscribeProfilePhaseChange_h
#define ACS_CS_SM_SubscribeProfilePhaseChange_h 1


#include "ACS_CS_API.h"

// ACS_CS_SM_SubscriptionObject
#include "ACS_CS_SM_SubscriptionObject.h"







class ACS_CS_SM_SubscribeProfilePhaseChange : public ACS_CS_SM_SubscriptionObject  //## Inherits: <unnamed>%4AAFD8050254
{

  public:
      ACS_CS_SM_SubscribeProfilePhaseChange();

      virtual ~ACS_CS_SM_SubscribeProfilePhaseChange();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      ACS_CS_API_OmProfilePhase::PhaseValue getPhase () const;

      void setPhase (const ACS_CS_API_OmProfilePhase::PhaseValue phase);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_SM_SubscribeProfilePhaseChange(const ACS_CS_SM_SubscribeProfilePhaseChange &right);

      ACS_CS_SM_SubscribeProfilePhaseChange & operator=(const ACS_CS_SM_SubscribeProfilePhaseChange &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_API_OmProfilePhase::PhaseValue m_phase;

    // Additional Implementation Declarations

};


// Class ACS_CS_SM_SubscribeProfilePhaseChange 



#endif
