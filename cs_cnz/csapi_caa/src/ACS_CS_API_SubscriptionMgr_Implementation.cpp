//	*********************************************************
//	 COPYRIGHT Ericsson 2010, 2017
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************

#include <cstdlib>
#include <exception>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "ace/Guard_T.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_Exception.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_EventReporter.h"

#include "ACS_CS_Protocol.h"
#include "ACS_CS_Util.h"

#include "ACS_CS_SM_SubscriptionObject.h"
#include "ACS_CS_SM_SubscribeTable.h"
#include "ACS_CS_SM_SubscribeProfilePhaseChange.h"

#include "ACS_CS_SM_CPTableNotify.h"
#include "ACS_CS_SM_CPGroupTableNotify.h"
#include "ACS_CS_SM_NETableNotify.h"
#include "ACS_CS_SM_CPTableNotify.h"
#include "ACS_CS_SM_ProfilePhaseNotify.h"
#include "ACS_CS_SM_HWCTableNotify.h"

#include "ACS_CS_API_NetworkElementChange_R1.h"
#include "ACS_CS_API_OgChange_R1.h"
#include "ACS_CS_API_OmProfileChange_R1.h"

#include "ACS_CS_ParsingException.h"
#include "ACS_CS_Event.h"

//#include "ACS_CS_Trace.h"
#include "ACS_CS_API_Tracer.h"

// ACS_CS_API_SubscriptionMgr_Implementation
#include "ACS_CS_API_SubscriptionMgr_Implementation.h"

//ACS_CS_Trace_TDEF(ACS_CS_API_SubscriptMgr_TRACE);

ACS_CS_API_TRACER_DEFINE(ACS_CS_API_SubscriptMgr_TRACE);

namespace
{
   //volatile bool s_eventThreadReady = false;

   ACE_Recursive_Thread_Mutex mutex;
}


namespace
{
   enum EventIdx {
      subscribeEventIdx = 0,
      notifyEventIdx,
      MAX_ELEMENTS };

   int gEvent[MAX_ELEMENTS];
}


namespace
{
   unsigned int SubMgrThreadHook(void *args)
   {
      ACS_CS_API_SubscriptionMgr_Implementation * p = static_cast<ACS_CS_API_SubscriptionMgr_Implementation *>(args);
      return p->SubMgrThread();
   }
}


ACS_CS_API_SubscriptionMgr_Implementation* ACS_CS_API_SubscriptionMgr_Implementation::s_instance = NULL;

ACS_CS_API_SubscriptionMgr_Implementation::ACS_CS_API_SubscriptionMgr_Implementation()
      : m_threadHandle(0),
        m_threadId(0),
        m_socket(0)
{
}


ACS_CS_API_SubscriptionMgr_Implementation::~ACS_CS_API_SubscriptionMgr_Implementation()
{
}


 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::subscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer)
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

      if( !m_ogObserver.insert(& observer).second )
      {
         return ACS_CS_API_NS::Result_Success; // is ok because it means we have that observer already
      }
   }

   std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
   subTbl->setTableScope(ACS_CS_Protocol::Scope_CPGroupTable);
   subTbl->isSubscription = true;
   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);

   return ACS_CS_API_NS::Result_Success;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::subscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer)
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

      if(!m_neObserver.insert(&observer).second)
      {
         return ACS_CS_API_NS::Result_Success; // is ok because it means we have that observer already
      }
   }

   std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
   subTbl->setTableScope(ACS_CS_Protocol::Scope_NE);
   subTbl->isSubscription = true;
   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);

   return ACS_CS_API_NS::Result_Success;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer)
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
      ACS_CS_API_TRACER_MESSAGE("Subscribing to CP Table Changes R1");

      if(!m_cpObserver_r1.insert(&observer).second)
      {
         return ACS_CS_API_NS::Result_Success; // is ok because it means we have that observer already
      }
   }

   std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
   subTbl->setTableScope(ACS_CS_Protocol::Scope_CPIdTable);
   subTbl->isSubscription = true;
   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);

   return ACS_CS_API_NS::Result_Success;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer)
 {
	 {
		 ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
		ACS_CS_API_TRACER_MESSAGE("Subscribing to CP Table Changes R2");

		 if(!m_cpObserver_r2.insert(&observer).second)
		 {
			 return ACS_CS_API_NS::Result_Success; // is ok because it means we have that observer already
		 }
	 }

	 std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
	 subTbl->setTableScope(ACS_CS_Protocol::Scope_CPIdTable);
	 subTbl->isSubscription = true;
	 putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);

	 return ACS_CS_API_NS::Result_Success;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::subscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer)
{
   // here things are a little different:
   // 1. for now, there is only one subscriber allowed, however, we keep set<> in case in changes again later.
   // 2. we need to do implicit subscriptions (subscribe to 4, if we get subscription of 1,2 or 3)
   // 3. implicit subscriptions make unsubscribe more complicated
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

   switch( phase )
   {
	   case ACS_CS_API_OmProfilePhase::Validate:

		   if( m_omProfilePhaseObserver_Validate.size() > 0 )
		   {
			   ACS_CS_API_TRACER_MESSAGE("Error: More than one observer!");
			   return ACS_CS_API_NS::Result_Failure; // only one subscriber allowed
		   }

		   if(!m_omProfilePhaseObserver_Validate.insert(&observer).second)
		   {
			   ACS_CS_API_TRACER_MESSAGE("Error: Insert of Observer failed!");
			   return ACS_CS_API_NS::Result_Failure; // strange, should not happen
		   }

		   {
			   std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
			   subTbl->setPhase(phase);
			   subTbl->isSubscription = true;
			   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
		   }

		   phase = ACS_CS_API_OmProfilePhase::Commit; // this causes the implicit subscribe to happen in a moment
		   break;

	   case ACS_CS_API_OmProfilePhase::ApNotify:

		   if( m_omProfilePhaseObserver_AP.size() > 0 )
		   {
			   ACS_CS_API_TRACER_MESSAGE("Error: More than one observer!");
			   return ACS_CS_API_NS::Result_Failure; // only one subscriber allowed
		   }

		   if(!m_omProfilePhaseObserver_AP.insert(&observer).second)
		   {
			   return ACS_CS_API_NS::Result_Failure;
		   }

		   {
			   std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
			   subTbl->setPhase(phase);
			   subTbl->isSubscription = true;
			   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
		   }

		   phase = ACS_CS_API_OmProfilePhase::Commit; // this causes the implicit subscribe to happen in a moment
		   break;

	   case ACS_CS_API_OmProfilePhase::CpNotify:

		   if( m_omProfilePhaseObserver_CP.size() > 0 )
		   {
			   ACS_CS_API_TRACER_MESSAGE("Error: More than one observer!");
			   return ACS_CS_API_NS::Result_Failure; // only one subscriber allowed
		   }

		   if(!m_omProfilePhaseObserver_CP.insert(&observer).second)
		   {
			   ACS_CS_API_TRACER_MESSAGE("Error: insert of observer failed !");
			   return ACS_CS_API_NS::Result_Failure; // strange, should not happen
		   }

		   {
			   std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
			   subTbl->setPhase(phase);
			   subTbl->isSubscription = true;
			   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
		   }

		   phase = ACS_CS_API_OmProfilePhase::Commit; // this causes the implicit subscribe to happen in a moment
		   break;

	   case ACS_CS_API_OmProfilePhase::Commit:
	   {
		   ACS_CS_API_TRACER_MESSAGE("ERROR: subscribe to phase COMMIT is not allowed !");
		   return ACS_CS_API_NS::Result_Failure;
	   }
	   break;

	   default: ;
	   // handle other cases in next switch...
   }

   switch( phase )
   {
	   case ACS_CS_API_OmProfilePhase::Commit:

		   // no check >0, because we may need to hold more than 1 observer, e.g. if
		   // observer1 subscribed to phase 0 and observer2 subscribed to phase 1 (result of implicit subscribe)

		   if(!m_omProfilePhaseObserver_Commit.insert(&observer).second)
		   {
			   return ACS_CS_API_NS::Result_Success; // ok, may happen if e.g. observer1 subscribes to phase 1,
			   // but subscribed already earlier to phase 0 (implicit subscription)
			   // or: explicit subscription in addition to implicit subscription
		   }
		   else
		   {
			   std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
			   subTbl->setPhase(phase);
			   subTbl->isSubscription = true;
			   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
		   }
		   break;

	   default:
		   ACS_CS_API_TRACER_MESSAGE("ERROR: invalid phase passed in !");
		   return ACS_CS_API_NS::Result_Failure;
   }

   return ACS_CS_API_NS::Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::subscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer)
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

      if(!m_hwcObserver.insert(&observer).second)
      {
         return ACS_CS_API_NS::Result_Success; // is ok because it means we have that observer already
      }
   }

   std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
   subTbl->setTableScope(ACS_CS_Protocol::Scope_HWCTable);
   subTbl->isSubscription = true;
   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);

   return ACS_CS_API_NS::Result_Success;
}

 ACS_CS_API_SubscriptionMgr_Implementation * ACS_CS_API_SubscriptionMgr_Implementation::getInstance ()
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

      if( s_instance == NULL )
      {
         s_instance = new ACS_CS_API_SubscriptionMgr_Implementation;

         // and this is our entry point to do some more stuff:

         // 1. create events

         gEvent[subscribeEventIdx] = ACS_CS_Event::CreateEvent(false, false, NULL);

         if (gEvent[subscribeEventIdx] == -1)
         {
        	 ACS_CS_API_TRACER_MESSAGE("Error: Can not create event");
         }

         //gEvent[notifyEventIdx] is initialized with the handle of the stream
         gEvent[notifyEventIdx] = ACE_INVALID_HANDLE;

         // 2. create new Thread

         ACS_CS_API_SubscriptionMgr_Implementation * pThat = const_cast<ACS_CS_API_SubscriptionMgr_Implementation *>(s_instance);
         void * pThis = static_cast<void *>(pThat);

         //s_eventThreadReady = false;

         int retval = ACE_OS::thr_create((ACE_THR_FUNC) &SubMgrThreadHook, (void*) pThis, 0, &s_instance->m_threadHandle);

         if( retval < 0 )
         {
        	 ACS_CS_API_TRACER_MESSAGE("Error: Can not create thread");
        	 return NULL;
         }
      }
   }

/* @@@ HM81641
   int i=0;
   while( !s_eventThreadReady )
   {
      i++;
      if(i > 1)
      {
         ACS_CS_API_TRACER_MESSAGE("Error: thread is NOT starting !?\nTrying again in 2 seconds...");
      }
      sleep(2);

   }
*/
   return const_cast<ACS_CS_API_SubscriptionMgr_Implementation *>(s_instance);
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::unsubscribeOgChanges (ACS_CS_API_OgObserver_R1 &observer)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

   std::set<ACS_CS_API_OgObserver_R1 *>::iterator i = m_ogObserver.find(&observer);
   if( i != m_ogObserver.end() )
   {
	   m_ogObserver.erase(i);
   }
   else
   {
	   ACS_CS_API_TRACER_MESSAGE("Warning: observer to unsubscribe does not exist!");
	   return ACS_CS_API_NS::Result_Failure;
   }

   if(m_ogObserver.size() == 0)  // no observers exist anymore
   {
	   std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
	   subTbl->setTableScope(ACS_CS_Protocol::Scope_CPGroupTable);
	   subTbl->isSubscription = false;
	   putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
   }

   return ACS_CS_API_NS::Result_Success;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::unsubscribeNetworkElementChanges (ACS_CS_API_NetworkElementObserver_R1 &observer)
{
	 ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

	 std::set<ACS_CS_API_NetworkElementObserver_R1 *>::iterator i = m_neObserver.find(&observer);
	 if( i != m_neObserver.end() )
	 {
		 m_neObserver.erase(i);
	 }
	 else
	 {
		 ACS_CS_API_TRACER_MESSAGE("Error: observer to unsubscribe does not exist!");
		 return ACS_CS_API_NS::Result_Failure;
	 }

	 if(m_neObserver.size() == 0) // no observers exist anymore
	 {
		 std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
		 subTbl->setTableScope(ACS_CS_Protocol::Scope_NE);
		 subTbl->isSubscription = false;
		 putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
	 }

	 return ACS_CS_API_NS::Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R1 &observer)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

	std::set<ACS_CS_API_CpTableObserver_R1 *>::iterator i = m_cpObserver_r1.find(&observer);
	if( i != m_cpObserver_r1.end() )
	{
		m_cpObserver_r1.erase(i);
	}
	else
	{
		ACS_CS_API_TRACER_MESSAGE("Error: observer to unsubscribe does not exist!");
		return ACS_CS_API_NS::Result_Failure;
	}

	if(getNumberOfObservers_CP() == 0) // no observers exist anymore
	{
		std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
		subTbl->setTableScope(ACS_CS_Protocol::Scope_CPIdTable);
		subTbl->isSubscription = false;
		putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
	}

	return ACS_CS_API_NS::Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

	std::set<ACS_CS_API_CpTableObserver_R2 *>::iterator i = m_cpObserver_r2.find(&observer);
	if( i != m_cpObserver_r2.end() )
	{
		m_cpObserver_r2.erase(i);
	}
	else
	{
		ACS_CS_API_TRACER_MESSAGE("Error: observer to unsubscribe does not exist!");
		return ACS_CS_API_NS::Result_Failure;
	}

	if(getNumberOfObservers_CP() == 0) // no observers exist anymore
	{
		std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
		subTbl->setTableScope(ACS_CS_Protocol::Scope_CPIdTable);
		subTbl->isSubscription = false;
		putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
	}

	return ACS_CS_API_NS::Result_Success;
}


 ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::unsubscribeOmProfilePhaseChanges (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_API_OmProfilePhaseObserver_R1 &observer)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

   switch( phase )
   {
      case ACS_CS_API_OmProfilePhase::Validate:
         {
            std::set<ACS_CS_API_OmProfilePhaseObserver_R1 *>::iterator i = m_omProfilePhaseObserver_Validate.find(&observer);

            if( i == m_omProfilePhaseObserver_Validate.end())
            {
            	ACS_CS_API_TRACER_MESSAGE("Error: observer to unsubscribe does not exist!");
            	return ACS_CS_API_NS::Result_Failure;
            }

            m_omProfilePhaseObserver_Validate.erase(i);

            if( m_omProfilePhaseObserver_Validate.size() == 0) // last observer was removed
            {
               std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
               subTbl->setPhase(phase);
               subTbl->isSubscription = false;
               putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
            }
            else
            {
               // at this point we don't allow more than one subscriber, so we should never get into this branch
            	ACS_CS_API_TRACER_MESSAGE("CODING ERROR: There should be not more than one subscriber!");
            	return ACS_CS_API_NS::Result_Failure;
            }

            // the next code block handles implicit unsubscribe
            if( (m_omProfilePhaseObserver_AP.find(&observer) ==  m_omProfilePhaseObserver_AP.end())
                &&
                (m_omProfilePhaseObserver_CP.find(&observer) ==  m_omProfilePhaseObserver_CP.end())
                )
            {
               m_omProfilePhaseObserver_Commit.erase(&observer);

               if( m_omProfilePhaseObserver_Commit.size() == 0 )
               {
                  std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
                  subTbl->setPhase(ACS_CS_API_OmProfilePhase::Commit);
                  subTbl->isSubscription = false;
                  putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
               }
            }
            return ACS_CS_API_NS::Result_Success;
         }
         break;

      case ACS_CS_API_OmProfilePhase::ApNotify:
         {
            std::set<ACS_CS_API_OmProfilePhaseObserver_R1 *>::iterator i = m_omProfilePhaseObserver_AP.find(&observer);

            if( i == m_omProfilePhaseObserver_AP.end() )
            {
            	ACS_CS_API_TRACER_MESSAGE("Error: observer to unsubscribe does not exist!");
            	return ACS_CS_API_NS::Result_Failure;
            }

            m_omProfilePhaseObserver_AP.erase(i);

            if( m_omProfilePhaseObserver_AP.size() == 0) // last observer was removed
            {
               std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
               subTbl->setPhase(phase);
               subTbl->isSubscription = false;
               putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
            }
            else
            {
               // at this point we don't allow more than one subscriber, so we should never get into this branch
            	ACS_CS_API_TRACER_MESSAGE("ERROR: CODING ERROR!");
            	return ACS_CS_API_NS::Result_Failure;
            }

            // the next code block handles implicit unsubscribe
            if( (m_omProfilePhaseObserver_Validate.find(&observer) ==  m_omProfilePhaseObserver_Validate.end())
                &&
                (m_omProfilePhaseObserver_CP.find(&observer) ==  m_omProfilePhaseObserver_CP.end())
                )
            {
               m_omProfilePhaseObserver_Commit.erase(&observer);

               if( m_omProfilePhaseObserver_Commit.size() == 0 )
               {
                  std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
                  subTbl->setPhase(ACS_CS_API_OmProfilePhase::Commit);
                  subTbl->isSubscription = false;
                  putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
               }
            }
            return ACS_CS_API_NS::Result_Success;
         }
         break;

      case ACS_CS_API_OmProfilePhase::CpNotify:
         {
            std::set<ACS_CS_API_OmProfilePhaseObserver_R1 *>::iterator i = m_omProfilePhaseObserver_CP.find(&observer);

            if( i == m_omProfilePhaseObserver_CP.end() )
            {
            	ACS_CS_API_TRACER_MESSAGE("Error: observer to unsubscribe does not exist!");
            	return ACS_CS_API_NS::Result_Failure;
            }

            m_omProfilePhaseObserver_CP.erase(i);

            if( m_omProfilePhaseObserver_CP.size() == 0) // last observer was removed
            {
               std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
               subTbl->setPhase(phase);
               subTbl->isSubscription = false;
               putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
            }
            else
            {
               // at this point we don't allow more than one subscriber, so we should never get into this branch
            	ACS_CS_API_TRACER_MESSAGE("ERROR: CODING ERROR!");
            	return ACS_CS_API_NS::Result_Failure;
            }

            // the next code block handles implicit unsubscribe
            if( (m_omProfilePhaseObserver_AP.find(&observer) ==  m_omProfilePhaseObserver_AP.end())
                &&
                (m_omProfilePhaseObserver_CP.find(&observer) ==  m_omProfilePhaseObserver_CP.end())
                )
            {
               m_omProfilePhaseObserver_Commit.erase(&observer);

               if( m_omProfilePhaseObserver_Commit.size() == 0 )
               {
                  std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subTbl(new ACS_CS_SM_SubscribeProfilePhaseChange);
                  subTbl->setPhase(ACS_CS_API_OmProfilePhase::Commit);
                  subTbl->isSubscription = false;
                  putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
               }
            }
            return ACS_CS_API_NS::Result_Success;

         }
         break;

      case ACS_CS_API_OmProfilePhase::Commit:
         {
        	 ACS_CS_API_TRACER_MESSAGE("ERROR: unsubscribe to phase COMMIT is not allowed !");

            return ACS_CS_API_NS::Result_Failure;

         }
         break;

      default:
    	  ACS_CS_API_TRACER_MESSAGE("Error: invalid OmProfilePhase value passed in <%d>!", phase);
         return ACS_CS_API_NS::Result_Failure;
   }

   return ACS_CS_API_NS::Result_Success;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_SubscriptionMgr_Implementation::unsubscribeHWCTableChanges (ACS_CS_API_HWCTableObserver_R1 &observer)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

   std::set<ACS_CS_API_HWCTableObserver_R1 *>::iterator i = m_hwcObserver.find(&observer);
   if( i != m_hwcObserver.end() )
   {
      m_hwcObserver.erase(i);
   }
   else
   {
	   ACS_CS_API_TRACER_MESSAGE("Error: observer to unsubscribe does not exist!");

      return ACS_CS_API_NS::Result_Failure;
   }

   if(getNumberOfObservers_HWC() == 0) // no observers exist anymore
   {
      std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
      subTbl->setTableScope(ACS_CS_Protocol::Scope_HWCTable);
      subTbl->isSubscription = false;
      putIntoQueue((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
   }

   return ACS_CS_API_NS::Result_Success;
}

 void ACS_CS_API_SubscriptionMgr_Implementation::sendSubscriptionPDU (std::auto_ptr<ACS_CS_SM_SubscriptionObject> primitive)
{

	 if (! primitive.get()) //Invalid input
		 return;

	 ACS_CS_Header * header = new ACS_CS_Header;

	 header->setVersion(ACS_CS_Protocol::Latest_Version);
	 header->setScope(ACS_CS_Protocol::Scope_Subscription);
	 header->setPrimitive(primitive->getPrimitiveType());
	 header->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);

	 ACS_CS_Primitive * p = primitive.release();
	 ACS_CS_PDU pdu(header, p);

	 size_t tmp = 0;
	 int bytesToSend = pdu.getLength();

	 // prepare buffer and hton conversion
	 std::vector<char> buffer(bytesToSend);   // vector<char> guaranties contigous memory
	 pdu.getBuffer(&buffer[0], bytesToSend);

	 int rc = -1;
	 int bytesSent = 0;
	 while (bytesSent < bytesToSend)
	 {
		 if (bytesSent > 0 || rc < 0)
			 usleep(10 * 1000); // no busy cycling... wait 10 ms

		 rc = m_socket->send(&buffer.at(bytesSent), bytesToSend - bytesSent, tmp);
		 if (rc == ACS_CS_TCPClient::NotConnected)
		 {
			 ACS_CS_API_TRACER_MESSAGE("Error: send failed");
			 /* Fix for TR HW53952(occurence of coredumps).
			  * Generally (ACS_CS_Primitive)primitive object is deleted when (ACS_CS_PDU)pdu object destructor is called,
			  * In this error scenario pdu destructor is being called after throw call.
			  * As primitive object is created only once (when it is received) we should not delete this primitive object along with pdu object,
			  * This primitive object is reused during retry
			  * Hence we are setting  dont_destroy_primitive flag check to true, which ensures primitive object is not deleted along with pdu object.*/
			 pdu.dont_destroy_primitive = true;
			 throw 1; // close connection and reconnect
		 }

		 bytesSent += tmp;
	 }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::handleSubscription (const int caller)
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

      if( m_queue.size() == 0 ) {
         return;
      }

      if(caller > 0)
      {
    	  ACS_CS_API_TRACER_MESSAGE("Oops, there is one subscription waiting, but I did not get an event, caller= %d", caller);
      }

      while( m_queue.size() > 0 )
      {
         std::auto_ptr<ACS_CS_SM_SubscriptionObject> primitive(m_queue.front());

         sendSubscriptionPDU(primitive);

         m_queue.pop();
         // delete primitive; /// pdu already deletes this
      }
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::resendAllSubscriptions ()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

   if( m_ogObserver.size() > 0 )
   {
      std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
      subTbl->setTableScope(ACS_CS_Protocol::Scope_CPGroupTable);
      subTbl->isSubscription = true;

      sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
   }

   if( getNumberOfObservers_HWC() > 0 )
   {
	   std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
	   subTbl->setTableScope(ACS_CS_Protocol::Scope_HWCTable);
	   subTbl->isSubscription = true;

	   sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
   }

   if( getNumberOfObservers_CP() > 0 )
   {
      std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
      subTbl->setTableScope(ACS_CS_Protocol::Scope_CPIdTable);
      subTbl->isSubscription = true;

      sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
   }

   if( m_neObserver.size() > 0 )
   {
      std::auto_ptr<ACS_CS_SM_SubscribeTable> subTbl(new ACS_CS_SM_SubscribeTable);
      subTbl->setTableScope(ACS_CS_Protocol::Scope_NE);
      subTbl->isSubscription = true;

      sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subTbl);
   }

   if( m_omProfilePhaseObserver_Validate.size() > 0 )
   {
      std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subPrf(new ACS_CS_SM_SubscribeProfilePhaseChange);
      subPrf->setPhase(ACS_CS_API_OmProfilePhase::Validate);
      subPrf->isSubscription = true;

      sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subPrf);
   }

   if( m_omProfilePhaseObserver_AP.size() > 0 )
   {
      std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subPrf(new ACS_CS_SM_SubscribeProfilePhaseChange);
      subPrf->setPhase(ACS_CS_API_OmProfilePhase::ApNotify);
      subPrf->isSubscription = true;

      sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subPrf);
   }

   if( m_omProfilePhaseObserver_CP.size() > 0 )
   {
      std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subPrf(new ACS_CS_SM_SubscribeProfilePhaseChange);
      subPrf->setPhase(ACS_CS_API_OmProfilePhase::CpNotify);
      subPrf->isSubscription = true;

      sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subPrf);
   }

   if( m_omProfilePhaseObserver_Commit.size() > 0 )
   {
      std::auto_ptr<ACS_CS_SM_SubscribeProfilePhaseChange> subPrf(new ACS_CS_SM_SubscribeProfilePhaseChange);
      subPrf->setPhase(ACS_CS_API_OmProfilePhase::Commit);
      subPrf->isSubscription = true;

      sendSubscriptionPDU((std::auto_ptr<ACS_CS_SM_SubscriptionObject>) subPrf);
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::doAllUpdate (const ACS_CS_CpTableChange &cpTableChange)
{
	 {//R1 Subscribers
		 ACS_CS_API_CpTableChange_R1 change;
		

		 change.dataSize = static_cast<int>(cpTableChange.cpData.size());
		 change.cpData = new ACS_CS_API_CpTableData_R1[change.dataSize];


		 setCpChange(cpTableChange,change);


		 ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
		 std::set<ACS_CS_API_CpTableObserver_R1 *>::iterator i;
		 i = m_cpObserver_r1.begin();


		 while(i != m_cpObserver_r1.end() )
		 {
	

			 // 1. this try/catch isolates each observer; crash of one observer does not affect all other observers
			 // 2. without this try/catch, if ever an exception would be caught on higher level, it would create a
			 //    memory leak at the delete[] in the following line of code.
			 try
			 {
				 (*i)->update(change);
			 }
			 catch(...)
			 {
				 ACS_CS_API_TRACER_MESSAGE("Warning: Exception caught in application code.");
			 }

			 ++i;
		 }

		 delete [] change.cpData;
	 }

	 {//R2 Subscribers


	 		 ACS_CS_API_CpTableChange_R2 change;

	 		 change.dataSize = static_cast<int>(cpTableChange.cpData.size());
	 		 change.cpData = new ACS_CS_API_CpTableData_R2[change.dataSize];


	 		 setCpChange(cpTableChange,change);


	 		 ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
	 		 std::set<ACS_CS_API_CpTableObserver_R2 *>::iterator i;
	 		 i = m_cpObserver_r2.begin();

	 		 while(i != m_cpObserver_r2.end() )
	 		 {

	 			 // 1. this try/catch isolates each observer; crash of one observer does not affect all other observers
	 			 // 2. without this try/catch, if ever an exception would be caught on higher level, it would create a
	 			 //    memory leak at the delete[] in the following line of code.
	 			 try
	 			 {
	 				 (*i)->update(change);
	 			 }
	 			 catch(...)
	 			 {
	 				 ACS_CS_API_TRACER_MESSAGE("Warning: Exception caught in application code.");
	 			 }

	 			 ++i;
	 		 }

	 		 delete [] change.cpData;
	 	 }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::doAllUpdate (const ACS_CS_NetworkElementChange &neChange)
{
   ACS_CS_API_NetworkElementChange_R1 change;

   // Memberwise copy from internal structure to API structure
   change.neId.setName(neChange.neId.c_str());
   change.omProfile = neChange.omProfile;
   change.clusterMode = neChange.clusterMode;
   change.trafficIsolatedCpId = neChange.trafficIsolatedCpId;
   change.trafficLeaderCpId = neChange.trafficLeaderCpId;
   change.alarmMasterCpId = neChange.alarmMasterCpId;
   change.clockMasterCpId = neChange.clockMasterCpId;

   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
   std::set<ACS_CS_API_NetworkElementObserver_R1 *>::iterator i;
   i = m_neObserver.begin();

   while(i != m_neObserver.end() )
   {
      try
      {
         (*i)->update(change);
      }
      catch(...)
      {
    	  ACS_CS_API_TRACER_MESSAGE("Warning: Exception catched in application code.");
      }

      ++i;
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::doAllUpdate (const ACS_CS_API_OgChange_R1 &ogChange)
{
   ACS_CS_API_OgChange_R1 change = ogChange;

   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
   std::set<ACS_CS_API_OgObserver_R1 *>::iterator i;
   i = m_ogObserver.begin();

   while( i != m_ogObserver.end() )
   {
      try
      {
         (*i)->update(change);
      }
      catch(...)
      {
    	  ACS_CS_API_TRACER_MESSAGE("Warning: Exception catched in application code.");
      }

      ++i;
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::doAllUpdate (const ACS_CS_API_OmProfileChange_R1 &profileChange)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
   std::set<ACS_CS_API_OmProfilePhaseObserver_R1 *>::iterator i;

   try {
      switch( profileChange.phase )
      {
         case ACS_CS_API_OmProfilePhase::Validate:
            i = m_omProfilePhaseObserver_Validate.begin();

            while( i != m_omProfilePhaseObserver_Validate.end() )
            {
               (*i)->update(profileChange);
               ++i;
            }
            break;

         case ACS_CS_API_OmProfilePhase::ApNotify:
            i = m_omProfilePhaseObserver_AP.begin();

            while( i != m_omProfilePhaseObserver_AP.end() )
            {
               (*i)->update(profileChange);
               ++i;
            }
            break;

         case ACS_CS_API_OmProfilePhase::CpNotify:
            i = m_omProfilePhaseObserver_CP.begin();

            while( i != m_omProfilePhaseObserver_CP.end() )
            {

               (*i)->update(profileChange);
               ++i;
            }
            break;

         case ACS_CS_API_OmProfilePhase::Commit:
            i = m_omProfilePhaseObserver_Commit.begin();

            while( i != m_omProfilePhaseObserver_Commit.end() )
            {
               (*i)->update(profileChange);
               ++i;
            }
            break;

         default:
        	 ACS_CS_API_TRACER_MESSAGE("ERROR: Invalid phase value passed in.");
      }
   } catch(...)
   {
	   ACS_CS_API_TRACER_MESSAGE("Warning: Exception catched in application code.");

   }
}

void ACS_CS_API_SubscriptionMgr_Implementation::doAllUpdate (const ACS_CS_HWCChange &hwcChange)
{
	ACS_CS_API_HWCTableChange_R1 change;

	change.dataSize = hwcChange.hwcData.size();
	change.hwcData = new ACS_CS_API_HWCTableData_R1[change.dataSize];

	for(int i = 0; i < change.dataSize; i++)
	{
		change.hwcData[i].aliasEthA = hwcChange.hwcData.at(i).aliasEthA;
		change.hwcData[i].aliasEthB = hwcChange.hwcData.at(i).aliasEthB;
		change.hwcData[i].dhcpMethod = hwcChange.hwcData.at(i).dhcpMethod;
		change.hwcData[i].fbn = hwcChange.hwcData.at(i).fbn;
		change.hwcData[i].ipEthA = hwcChange.hwcData.at(i).ipEthA;
		change.hwcData[i].ipEthB = hwcChange.hwcData.at(i).ipEthB;
		change.hwcData[i].magazine = hwcChange.hwcData.at(i).magazine;
		change.hwcData[i].netmaskAliasEthA = hwcChange.hwcData.at(i).netmaskAliasEthA;
		change.hwcData[i].netmaskAliasEthB = hwcChange.hwcData.at(i).netmaskAliasEthB;
		change.hwcData[i].operationType = hwcChange.hwcData.at(i).operationType;
		change.hwcData[i].seqNo = hwcChange.hwcData.at(i).seqNo;
		change.hwcData[i].side = hwcChange.hwcData.at(i).side;
		change.hwcData[i].slot = hwcChange.hwcData.at(i).slot;
		change.hwcData[i].sysId = hwcChange.hwcData.at(i).sysId;
		change.hwcData[i].sysNo = hwcChange.hwcData.at(i).sysNo;
		change.hwcData[i].sysType = hwcChange.hwcData.at(i).sysType;
		change.hwcData[i].productId = hwcChange.hwcData.at(i).productId;
		change.hwcData[i].productRevision = hwcChange.hwcData.at(i).productRevision;
		change.hwcData[i].softwarePackage = hwcChange.hwcData.at(i).softwarePackage;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
	std::set<ACS_CS_API_HWCTableObserver_R1 *>::iterator i;
	i = m_hwcObserver.begin();

	while (i != m_hwcObserver.end())
	{
		// 1. this try/catch isolates each observer; crash of one observer does not affect all other observers
		// 2. without this try/catch, if ever an exception would be caught on higher level, it would create a
		//    memory leak at the delete[] in the following line of code.
		try
		{
			(*i)->update(change);
		}
		catch(...)
		{
			ACS_CS_API_TRACER_MESSAGE("Warning: Exception caught in application code.");
		}

		++i;
	}
	delete[] change.hwcData;
}

 void ACS_CS_API_SubscriptionMgr_Implementation::doAllUpdateFilter (const ACS_CS_CpGroupChange &change)
{
   ACS_CS_API_OgChange_R1 ogChange;

   for( size_t i = 0; i < change.cpGroupData.size(); i++ )
   {
      if( change.cpGroupData[i].cpGroupName == ACS_CS_NS::CP_GROUP_OPGROUP)
      {
         int size = static_cast<int>(change.cpGroupData[i].cpIdList.size());

         ogChange.ogCpIdList.setSize(size);

         for (int j=0; j < size; j++)
         {
            ogChange.ogCpIdList.setValue(change.cpGroupData[i].cpIdList[j], j);
         }

         ogChange.emptyOgReason = change.cpGroupData[i].emptyOgReason;

         doAllUpdate(ogChange);
      }
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::handleNotifyPrimitive (const ACS_CS_Primitive *primitive)
{
   ACS_CS_Protocol::CS_Primitive_Identifier primId= primitive->getPrimitiveType();

   switch(primId)
   {
   	   case ACS_CS_Protocol::Primitive_NotifyHWCTableChange:
		{
		   const ACS_CS_SM_HWCTableNotify * const myPrimitive = static_cast<const ACS_CS_SM_HWCTableNotify * const>(primitive);

		   ACS_CS_HWCChange hwcChange;
		   myPrimitive->getHWCChange(hwcChange);

		   doAllUpdate(hwcChange);
		   break;
		}
      case ACS_CS_Protocol::Primitive_NotifyCpidTableChange:
         {
            const ACS_CS_SM_CPTableNotify * const myPrimitive = static_cast<const ACS_CS_SM_CPTableNotify * const>(primitive);

            ACS_CS_CpTableChange cpTableChange;
            myPrimitive->getCpTableChange(cpTableChange);

            doAllUpdate(cpTableChange);
            break;
         }
      case ACS_CS_Protocol::Primitive_NotifyNeTableChange:
         {
            const ACS_CS_SM_NETableNotify * const myPrimitive = static_cast<const ACS_CS_SM_NETableNotify * const>(primitive);

            ACS_CS_NetworkElementChange neTableChange;
            myPrimitive->getNeChange(neTableChange);

            doAllUpdate(neTableChange);
            break;
         }
      case ACS_CS_Protocol::Primitive_NotifyProfilePhaseChange:
         {
            const ACS_CS_SM_ProfilePhaseNotify * const myPrimitive = static_cast<const ACS_CS_SM_ProfilePhaseNotify * const>(primitive);

            ACS_CS_API_OmProfileChange omProfileChange;
            myPrimitive->getOmProfileChange(omProfileChange);

            doAllUpdate(omProfileChange);
            break;
         }
      case ACS_CS_Protocol::Primitive_NotifyCpGroupTableChange:
         {
            const ACS_CS_SM_CPGroupTableNotify * const myPrimitive = static_cast<const ACS_CS_SM_CPGroupTableNotify * const>(primitive);

            ACS_CS_CpGroupChange cpgTableChange;
            myPrimitive->getCpGroupChange(cpgTableChange);

            doAllUpdateFilter(cpgTableChange);
            break;
         }
      default:
    	  ACS_CS_API_TRACER_MESSAGE("Error: invalid primitive received.\nprimitive : %d", primId);
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::handleNotify (const int caller)
{
   int bytesRcvd = 1;
   static const int MAX_BUFFER_SIZE = 512;
   int size = MAX_BUFFER_SIZE;
   std::vector<char> buffer(size);
   int rc = 0;
   ACS_CS_Parser parser(ACS_CS_Protocol::Protocol_CS);
   ACS_CS_PDU pdu;

   while( bytesRcvd > 0 )
   {
      ACS_CS_TCPClient::LinkStatus_t status;

      status = m_socket->read(&buffer[0], MAX_BUFFER_SIZE, bytesRcvd);

      if( status == ACS_CS_TCPClient::NotConnected )
      {
         if(caller > 0)
         {
        	 ACS_CS_API_TRACER_MESSAGE("Oops, the connection is down and I didn't get an event !, caller = %d", caller);
         }
         throw 1;  // start reconnect processing
      }
      else if( status == ACS_CS_TCPClient::OK && bytesRcvd == 0 )
      {
         return; // no data present
      }

      rc = parser.newData(&buffer[0], bytesRcvd);
      if( rc < 0 )
      {
    	  ACS_CS_API_TRACER_MESSAGE("Error: error condition signaled.");
         continue;
      }

      try
      {
         rc = parser.parse();
         if( rc < 0 )
         {
        	 ACS_CS_API_TRACER_MESSAGE("Error: error condition signaled from parser.parse().");
            continue;
         }
      }
      catch(ACS_CS_Exception ex)
      {
    	  ACS_CS_API_TRACER_MESSAGE("Error: Parser error.");
      }
      catch(...)
      {
    	  ACS_CS_API_TRACER_MESSAGE("Error: undefined Parser error.");
      }

      while( parser.getPDU(pdu) )
      {
         const ACS_CS_Header * const header = dynamic_cast<const ACS_CS_Header *>(pdu.getHeader());
         const ACS_CS_Primitive * const primitive = pdu.getPrimitive();

         if( header == NULL )
         {
            continue;
         }

         if( primitive == NULL )
         {
            continue;
         }

         handleNotifyPrimitive(primitive);
      }
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::handleEvent ()
{
   //s_eventThreadReady = true;
   int event = ACS_CS_Event::WaitForEvents(MAX_ELEMENTS, gEvent, STREAM_TIMEOUT);

   switch(event)
   {
      case subscribeEventIdx:
         handleSubscription(0);
         break;

      case notifyEventIdx:
         handleNotify(0);
         break;

      case ACS_CS_EVENT_RC_TIMEOUT:
         handleSubscription(__LINE__);
         handleNotify(__LINE__);
         break;

      case ACS_CS_EVENT_RC_ERROR:
      default:
    	  ACS_CS_API_TRACER_MESSAGE("Error: undefined Event, event = %d\n", event);
         throw 1;  // start reconnect processing
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::putIntoQueue (std::auto_ptr<ACS_CS_SM_SubscriptionObject> obj)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
   ACS_CS_SM_SubscriptionObject * p = obj.release();

   if (p)
   {
	   m_queue.push(p);

	   if( !ACS_CS_Event::SetEvent(gEvent[subscribeEventIdx]))
	   {
		   ACS_CS_API_TRACER_MESSAGE("Error: Can not set sendEvent");

		   ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,
				   "CS Event",
				   "SetEvent failed",
				   "");
	   }
   }
}


 int unsigned ACS_CS_API_SubscriptionMgr_Implementation::SubMgrThread ()
{
   uint32_t clusterIPs[] =
   { 
      ACS_CS_NS::Server_Local_IP, ACS_CS_NS::CLUSTER_IP_ETHA, ACS_CS_NS::CLUSTER_IP_ETHB 
   };

   std::vector<uint32_t> agentIPs(clusterIPs, clusterIPs + sizeof(clusterIPs)/sizeof(uint32_t));
   size_t agent = 0;

    bool connection_lost = false;

    while (true)
    {
      /* Build ourselves a Stream socket. This is a connected socket that
       * provides reliable end-to-end communications. stream object is
       * used to send data to the server. */
 	  ACE_SOCK_Stream stream;

 	  /* The connector object established the connection. The
 	   * ACE_SOCK_Connector object provides all of the tools we need to
 	   * establish a connection once we know the server's network address (ACE_INET_Addr). */
 	  ACE_SOCK_Connector connector;

 	 /* ACE_INET_Addr object encapsulate the TCP/IP port
 	  * and hostname of the server we want to connect to. */
      ACE_INET_Addr addr(ACS_CS_NS::CS_Subscription_Port, agentIPs[agent]);

      /* establish the network connection to the server and attach
       * that connection to the stream object.  */
      if (connector.connect(stream, addr) != 0)
      {
    	   ACS_CS_API_TRACER_MESSAGE("Error: Failed to connect to SubscriptionAgent %s:%u",
               addr.get_host_addr(), addr.get_port_number()); 
          
         if (++agent >= agentIPs.size())
         {
            agent = 0;
            sleep(1);
         }
         continue; // try again
      }
      ACS_CS_API_TRACER_MESSAGE("connected to %s:%u\n", addr.get_host_addr(), addr.get_port_number());

      if (m_socket) {
    	  m_socket->close();
    	  delete m_socket;
    	  m_socket=0;
      }

      m_socket = new ACS_CS_TCPClient(stream, addr);
      
      if (agentIPs[agent] != ACS_CS_NS::Server_Local_IP)
      {
         int optval = 1; 
         stream.set_option(SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
      }

      //Get ACE_HANDLE
      gEvent[notifyEventIdx] = stream.get_handle();
      if (gEvent[notifyEventIdx] == ACE_INVALID_HANDLE)
      {
    	  ACS_CS_API_TRACER_MESSAGE("Error: Can not create event");
      }

      try
      {
    	  if (connection_lost)
    	  {
    		  handleSubscription(__LINE__); // in case there are any and we failed previously, or restart
    		  resendAllSubscriptions();
    		  handleNotify(__LINE__);       // in case there are any notifies waiting and we failed previously
    	  }

         while(true) {
            handleEvent();
         }
      }
      catch(...)
      {
    	  connection_lost = true;
    	  ACS_CS_API_TRACER_MESSAGE("Error: Exception caught, current agent %s:%u", 
              addr.get_host_addr(), addr.get_port_number());

        // most likely network error, try next IP address in the list
        if (++agent >= agentIPs.size())
        {
           agent = 0;
           sleep(1);
        }
      }
   }

   return 0; // satisfy compiler
}

 uint32_t ACS_CS_API_SubscriptionMgr_Implementation::getNumberOfObservers_CP ()
{
   return static_cast<int>(m_cpObserver_r1.size() + m_cpObserver_r2.size());
}

 uint32_t ACS_CS_API_SubscriptionMgr_Implementation::getNumberOfObservers_NE ()
{
   return static_cast<int>(m_neObserver.size());
}

 uint32_t ACS_CS_API_SubscriptionMgr_Implementation::getNumberOfObservers_OG ()
{
   return static_cast<int>(m_ogObserver.size());
}

uint32_t ACS_CS_API_SubscriptionMgr_Implementation::getNumberOfObservers_HWC ()
{
	return static_cast<uint32_t>(m_hwcObserver.size());
}

 uint32_t ACS_CS_API_SubscriptionMgr_Implementation::getNumberOfObservers_Phase (const ACS_CS_API_OmProfilePhase::PhaseValue phase)
{
   switch(phase)
   {
      case ACS_CS_API_OmProfilePhase::Validate:
         return static_cast<int>(m_omProfilePhaseObserver_Validate.size());
      case ACS_CS_API_OmProfilePhase::ApNotify:
         return static_cast<int>(m_omProfilePhaseObserver_AP.size());
      case ACS_CS_API_OmProfilePhase::CpNotify:
         return static_cast<int>(m_omProfilePhaseObserver_CP.size());
      case ACS_CS_API_OmProfilePhase::Commit:
         return static_cast<int>(m_omProfilePhaseObserver_Commit.size());
      default:
    	  ACS_CS_API_TRACER_MESSAGE("Error: Invalid phase passed in: %d", phase);
         return 0;
   }
}


 void ACS_CS_API_SubscriptionMgr_Implementation::setCpChange (const ACS_CS_CpTableChange &cpTableChange, ACS_CS_API_CpTableChange_R1 &change)
{
   for (size_t j=0; j < change.dataSize; j++)
   {
      change.cpData[j].cpId            = cpTableChange.cpData[j].cpId;
      change.cpData[j].operationType   = cpTableChange.cpData[j].operationType;
      change.cpData[j].cpName.setName(cpTableChange.cpData[j].cpName.c_str());
      change.cpData[j].cpAliasName.setName(cpTableChange.cpData[j].cpAliasName.c_str());
      change.cpData[j].apzSystem       = cpTableChange.cpData[j].apzSystem;
      change.cpData[j].cpState         = cpTableChange.cpData[j].cpState;
      change.cpData[j].applicationId   = cpTableChange.cpData[j].applicationId;
      change.cpData[j].apzSubstate     = cpTableChange.cpData[j].apzSubstate;
      change.cpData[j].aptSubstate     = cpTableChange.cpData[j].aptSubstate;
      change.cpData[j].stateTransition = cpTableChange.cpData[j].stateTransition;
      change.cpData[j].cpType          = cpTableChange.cpData[j].cpType;
      change.cpData[j].blockingInfo    = cpTableChange.cpData[j].blockingInfo;
      change.cpData[j].cpCapacity      = cpTableChange.cpData[j].cpCapacity;
   }
}

 void ACS_CS_API_SubscriptionMgr_Implementation::setCpChange (const ACS_CS_CpTableChange &cpTableChange, ACS_CS_API_CpTableChange_R2 &change)
 {
    for (size_t j=0; j < change.dataSize; j++)
    {
       change.cpData[j].cpId            = cpTableChange.cpData[j].cpId;
       change.cpData[j].operationType   = cpTableChange.cpData[j].operationType;
       change.cpData[j].cpName.setName(cpTableChange.cpData[j].cpName.c_str());
       change.cpData[j].cpAliasName.setName(cpTableChange.cpData[j].cpAliasName.c_str());
       change.cpData[j].apzSystem       = cpTableChange.cpData[j].apzSystem;
       change.cpData[j].cpState         = cpTableChange.cpData[j].cpState;
       change.cpData[j].applicationId   = cpTableChange.cpData[j].applicationId;
       change.cpData[j].apzSubstate     = cpTableChange.cpData[j].apzSubstate;
       change.cpData[j].aptSubstate     = cpTableChange.cpData[j].aptSubstate;
       change.cpData[j].stateTransition = cpTableChange.cpData[j].stateTransition;
       change.cpData[j].cpType          = cpTableChange.cpData[j].cpType;
       change.cpData[j].blockingInfo    = cpTableChange.cpData[j].blockingInfo;
       change.cpData[j].cpCapacity      = cpTableChange.cpData[j].cpCapacity;
       change.cpData[j].mauType         = cpTableChange.cpData[j].mauType;
    }
 }

