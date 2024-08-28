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

// ACS_CS_API_SubscriptionMgr_R2
#include "ACS_CS_API_SubscriptionMgr_R2.h"
#include "ACS_CS_API_Tracer.h"

ACS_CS_API_TRACER_DEFINE(ACS_CS_API_SubscriptMgr_R2_TRACE);

#include "ace/Mutex.h"


namespace
{
   ACS_CS_API_SubscriptionMgr_R2 * s_instance = NULL;
   
   ACE_Mutex mutex;

   struct Lock1
   {
         Lock1() { mutex.acquire(); };
         ~Lock1() { mutex.release(); };
   };
}




ACS_CS_API_SubscriptionMgr_R2::ACS_CS_API_SubscriptionMgr_R2()
{
}


ACS_CS_API_SubscriptionMgr_R2::~ACS_CS_API_SubscriptionMgr_R2()
{
   // Uncomment the following line if making the d'tor public
   // s_instance = NULL;
}


 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R2::subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer)
{
	 ACS_CS_API_TRACER_MESSAGE("Subscribe R2 Sub_manager_R2......");
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->subscribeCpTableChanges(observer);
}

ACS_CS_API_SubscriptionMgr_R2 * ACS_CS_API_SubscriptionMgr_R2::getInstance ()
{
   {
      Lock1 lock;
      if( s_instance == NULL )
      {
         s_instance = new ACS_CS_API_SubscriptionMgr_R2;
      }
   }

   // give Thread creation a head start
   ACS_CS_API_SubscriptionMgr_Implementation::getInstance();

   return const_cast<ACS_CS_API_SubscriptionMgr_R2 *>(s_instance);
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_R2::unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer)
{
   return ACS_CS_API_SubscriptionMgr_Implementation::getInstance()->unsubscribeCpTableChanges(observer);
}

