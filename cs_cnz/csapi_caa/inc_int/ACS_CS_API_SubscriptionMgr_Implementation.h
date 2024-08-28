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


#ifndef ACS_CS_API_SubscriptionMgr_Implementation_h
#define ACS_CS_API_SubscriptionMgr_Implementation_h 1

#include <vector>
#include <queue>
#include <set>

#include "ace/Thread.h"

#include "ACS_CS_API.h"
#include "ACS_CS_TCPClient.h"
#include "ACS_CS_API_OmProfileChange_R1.h"
#include "ACS_CS_API_HWCTableChange_R1.h"

#include "ACS_CS_API_OgObserver_R1.h"
#include "ACS_CS_API_CpTableObserver_R2.h"
#include "ACS_CS_API_NetworkElementObserver_R1.h"
#include "ACS_CS_API_OmProfilePhaseObserver_R1.h"
#include "ACS_CS_API_HWCTableObserver_R1.h"


class ACS_CS_SM_SubscriptionObject;
struct ACS_CS_CpTableChange;
struct ACS_CS_NetworkElementChange;
struct ACS_CS_CpGroupChange;
struct ACS_CS_HWCChange;
class ACS_CS_Primitive;

class ACS_CS_API_SubscriptionMgr_Implementation 
{
  public:

      virtual ACS_CS_API_NS::CS_API_Result subscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result subscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer);

      static ACS_CS_API_SubscriptionMgr_Implementation * getInstance ();

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer);

      virtual ACS_CS_API_NS::CS_API_Result unsubscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer);

      unsigned int SubMgrThread ();

      uint32_t getNumberOfObservers_CP ();

      uint32_t getNumberOfObservers_NE ();

      uint32_t getNumberOfObservers_OG ();

      uint32_t getNumberOfObservers_Phase (const ACS_CS_API_OmProfilePhase::PhaseValue phase);

      uint32_t getNumberOfObservers_HWC ();


  protected:
      ACS_CS_API_SubscriptionMgr_Implementation();

      virtual ~ACS_CS_API_SubscriptionMgr_Implementation();


  private:
      ACS_CS_API_SubscriptionMgr_Implementation(const ACS_CS_API_SubscriptionMgr_Implementation &right);

      ACS_CS_API_SubscriptionMgr_Implementation & operator=(const ACS_CS_API_SubscriptionMgr_Implementation &right);


      void sendSubscriptionPDU (std::auto_ptr<ACS_CS_SM_SubscriptionObject> primitive);

      void handleSubscription (const int caller);

      void resendAllSubscriptions ();

      void doAllUpdate (const ACS_CS_CpTableChange &cpTableChange);

      void doAllUpdate (const ACS_CS_HWCChange &hwcChange);

      void doAllUpdate (const ACS_CS_NetworkElementChange &neChange);

      void doAllUpdate (const ACS_CS_API_OgChange_R1 &ogChange);

      void doAllUpdate (const ACS_CS_API_OmProfileChange_R1 &profileChange);

      void doAllUpdateFilter (const ACS_CS_CpGroupChange &change);

      void handleNotifyPrimitive (const ACS_CS_Primitive *primitive);

      void handleNotify (const int caller);

      void handleEvent ();

      void putIntoQueue (std::auto_ptr<ACS_CS_SM_SubscriptionObject> obj);

      void setCpChange (const ACS_CS_CpTableChange &cpTableChange, ACS_CS_API_CpTableChange_R1 &change);

      void setCpChange (const ACS_CS_CpTableChange &cpTableChange, ACS_CS_API_CpTableChange_R2 &change);

    static const int STREAM_TIMEOUT = 30000;

    std::set<ACS_CS_API_CpTableObserver_R1*> m_cpObserver_r1;

    std::set<ACS_CS_API_CpTableObserver_R2*> m_cpObserver_r2;

    std::set<ACS_CS_API_OgObserver_R1*> m_ogObserver;

    std::set<ACS_CS_API_NetworkElementObserver_R1*> m_neObserver;

    std::set<ACS_CS_API_HWCTableObserver_R1*> m_hwcObserver;

    std::set<ACS_CS_API_OmProfilePhaseObserver_R1*> m_omProfilePhaseObserver_Validate;

    std::set<ACS_CS_API_OmProfilePhaseObserver_R1*> m_omProfilePhaseObserver_AP;

    std::set<ACS_CS_API_OmProfilePhaseObserver_R1*> m_omProfilePhaseObserver_CP;

    std::set<ACS_CS_API_OmProfilePhaseObserver_R1*> m_omProfilePhaseObserver_Commit;

    std::queue<ACS_CS_SM_SubscriptionObject*> m_queue;

    ACE_thread_t m_threadHandle;

    unsigned int m_threadId;

      ACS_CS_TCPClient *m_socket;

      static ACS_CS_API_SubscriptionMgr_Implementation* s_instance;

};


#endif
