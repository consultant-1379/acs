//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_API_SubscriptionMgr_R1_h
#define ACS_CS_API_SubscriptionMgr_R1_h 1


#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_OgObserver_R1.h"
#include "ACS_CS_API_NetworkElementObserver_R1.h"
#include "ACS_CS_API_CpTableObserver_R1.h"
#include "ACS_CS_API_OmProfilePhaseObserver_R1.h"
#include "ACS_CS_API_HWCTableObserver_R1.h"


class ACS_CS_API_SubscriptionMgr_R1
{

  public:

      virtual ACS_CS_API_NS::CS_API_Result subscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer);


      virtual ACS_CS_API_NS::CS_API_Result subscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer);

      static  ACS_CS_API_SubscriptionMgr_R1 * getInstance ();

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer);


      virtual ACS_CS_API_NS::CS_API_Result unsubscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer);

  protected:
      ACS_CS_API_SubscriptionMgr_R1();

      virtual ~ACS_CS_API_SubscriptionMgr_R1();

  private:
      ACS_CS_API_SubscriptionMgr_R1(const ACS_CS_API_SubscriptionMgr_R1 &right);

      ACS_CS_API_SubscriptionMgr_R1 & operator=(const ACS_CS_API_SubscriptionMgr_R1 &right);

};

#endif
