//   Read the documentation to learn more about C++ code generator
//   versioning.

//	  %X% %Q% %Z% %W%

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



// this needs to move to intermediate/factory class for maximum decoupling...
#include "ACS_CS_API_SubscriptionMgr_Implementation.h"

// ACS_CS_API_SubscriptionMgr_R1
#include "ACS_CS_API_SubscriptionMgr_R1.h"
#include "ACS_CS_API_Tracer.h"

#include "ace/Mutex.h"


ACS_CS_API_TRACER_DEFINE(ACS_CS_API_SubscriptMgr_R1_TRACE);

namespace
{
   ACS_CS_API_SubscriptionMgr_R1 * s_instance = NULL;
   
   ACE_Mutex mutex;

   struct Lock
   {
         Lock() { mutex.acquire(); };
         ~Lock() { mutex.release(); };
   };
}




ACS_CS_API_SubscriptionMgr_R1::ACS_CS_API_SubscriptionMgr_R1()
{
}


ACS_CS_API_SubscriptionMgr_R1::~ACS_CS_API_SubscriptionMgr_R1()
{
   // Uncomment the following line if making the d'tor public
   // s_instance = NULL;
}


 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::subscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->subscribeOgChanges(observer);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::subscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->subscribeNetworkElementChanges(observer);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer)
{
	 ACS_CS_API_TRACER_MESSAGE("Subscribe R1 Sub_manager_R1......");
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->subscribeCpTableChanges(observer);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::subscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->subscribeOmProfilePhaseChanges(phase, observer);
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::subscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer)
{
	return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->subscribeHWCTableChanges(observer);
}

ACS_CS_API_SubscriptionMgr_R1 * ACS_CS_API_SubscriptionMgr_R1::getInstance ()
{
   {
      Lock lock;
      if( s_instance == NULL )
      {
         s_instance = new ACS_CS_API_SubscriptionMgr_R1;
      }
   }

   // give Thread creation a head start
   ACS_CS_API_SubscriptionMgr_Implementation::getInstance();

   return const_cast<ACS_CS_API_SubscriptionMgr_R1 *>(s_instance);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::unsubscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->unsubscribeOgChanges(observer);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::unsubscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->unsubscribeNetworkElementChanges(observer);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->unsubscribeCpTableChanges(observer);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::unsubscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->unsubscribeOmProfilePhaseChanges(phase, observer);
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R1::unsubscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer)
{
	return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->unsubscribeHWCTableChanges(observer);
}
