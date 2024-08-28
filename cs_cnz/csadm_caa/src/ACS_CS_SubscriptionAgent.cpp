

//	*********************************************************
//	 COPYRIGHT Ericsson 2010, 2017.
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



#include "ACS_CS_Util.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_TCPClient.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_API_NeHandling.h"
#include "ACS_CS_SM_CPGroupTableNotify.h"
#include "ACS_CS_SM_NETableNotify.h"
#include "ACS_CS_SM_ProfilePhaseNotify.h"
#include "ACS_CS_SubscriptionConnection.h"
#include "ACS_CS_SM_HWCTableNotify.h"

// ACS_CS_SubscriptionAgent
#include "ACS_CS_SubscriptionAgent.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_SubscriptionAgent_TRACE);

using namespace std;
using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using namespace ACS_CS_Subscription_NS;



ACS_CS_SubscriptionAgent::ACS_CS_SubscriptionAgent()
      : shutdownEvent(ACS_CS_EVENT_RC_ERROR),
        pduEvent(ACS_CS_EVENT_RC_ERROR),
        tcpServerLocalhost(0),
        lock(0),
        profileChangeNotificationDisabled(true)
{

   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Constructing the agent instance\n", __FUNCTION__));

   // Open shutdown event used to handle the signal that the whole service shall shutdown
   shutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);
   if (shutdownEvent == ACS_CS_EVENT_RC_ERROR)
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s :\n  Error: Cannot open the shutdown event\n", __FUNCTION__));

      ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event, "Opening of Event", "Cannot open shutdown event", "");
   }

   // Create pdu event used to wait for table handlers to update pduContainer
   pduEvent = ACS_CS_Event::CreateEvent(false, false, NULL);
   if (pduEvent == 0)
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s :\n  Error: Cannot create the pdu event\n", __FUNCTION__));

      ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event, "Creation of pdu event", "Cannot create PDU event", "");
   }

   lock = new ACS_CS_ReaderWriterLock();

   // Initialize values for subscription count
   updatePhaseSubcriberCount();
}


ACS_CS_SubscriptionAgent::~ACS_CS_SubscriptionAgent()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Destructing the agent instance\n", __FUNCTION__));

   if (pduEvent)
   {
      ACS_CS_Event::CloseEvent(pduEvent);
   }

   deleteAllConnections(); // Delete all connections from connectionSet
   emptyPduContainer();    // Cleanup pduContainer (call this before DELETING lock)
   clearMaps();            // Cleanup all Maps (call this before DELETING lock)

   delete lock;

   if (tcpServerLocalhost)
   {
       tcpServerLocalhost->stopListen();
       delete tcpServerLocalhost;
   }
}



 void ACS_CS_SubscriptionAgent::deleteAllConnections ()
{
   std::set<ACS_CS_SubscriptionConnection * >::iterator it = connectionSet.begin(); // No lock needed to access connectionSet

   // Disconnect all connections and delete the finished ones
   while ( it != connectionSet.end() )
   {
      if ( ! (*it)->hasFinished() )    // Connection has not finished
      {
         (*it)->disconnect();          // Disconnect and stop the thread
         ++it;
      }
      else                             // Connection has finished
      {
         ACS_CS_SubscriptionConnection * connection = *it;

         connection->close();          // Close thread handler
         delete connection;

         connectionSet.erase(it);      // Remove finished connection from connectionSet
         it = connectionSet.begin();
      }
   }

   it = connectionSet.begin();

   // Wait for the remaining threads to stop
   while (it != connectionSet.end() )
   {
      ACS_CS_SubscriptionConnection * connection = *it;

      (void) connection->wait(Connection_Timeout);
      connection->close();             // Close thread handler
      delete connection;

      connectionSet.erase(it);    // Remove remaining connection from the set

      it = connectionSet.begin();
   }
}

 void ACS_CS_SubscriptionAgent::emptyPduContainer ()
{
   lock->start_writing();

   // Empty pduContainer
   while ( ! pduContainer.empty() )
   {
      ACS_CS_PDU * pdu = pduContainer.front();
      pduContainer.pop_front();  // Remove pdu

      delete pdu;  // Delete unsent pdu
   }

   lock->stop_writing();

}

 void ACS_CS_SubscriptionAgent::clearMaps ()
{
   lock->start_writing();

   TableSubscriberMapType::iterator itTableMap;

   // Clear the subscriber set for each table
   itTableMap = tableSubscriberMap.find(Scope_CPIdTable);
   if ( itTableMap != tableSubscriberMap.end() )
   {
      itTableMap->second.clear();
   }

   itTableMap = tableSubscriberMap.find(Scope_CPGroupTable);
   if ( itTableMap != tableSubscriberMap.end() )
   {
      itTableMap->second.clear();
   }

   itTableMap = tableSubscriberMap.find(Scope_NE);
   if ( itTableMap != tableSubscriberMap.end() )
   {
      itTableMap->second.clear();
   }

	itTableMap = tableSubscriberMap.find(Scope_HWCTable);
	if ( itTableMap != tableSubscriberMap.end() )
	{
		itTableMap->second.clear();
	}

   // Clear the tableSubscriberMap
   tableSubscriberMap.clear();

   PhaseSubscriberMapType::iterator itPhaseMap;

   // Clear the subscriber set for each phase (of phaseSubscriberMap)
   itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::Validate);
   if ( itPhaseMap != phaseSubscriberMap.end() )
   {
      itPhaseMap->second.clear();
   }

   itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::ApNotify);
   if ( itPhaseMap != phaseSubscriberMap.end() )
   {
      itPhaseMap->second.clear();
   }

   itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::CpNotify);
   if ( itPhaseMap != phaseSubscriberMap.end() )
   {
      itPhaseMap->second.clear();
   }

   itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::Commit);
   if ( itPhaseMap != phaseSubscriberMap.end() )
   {
      itPhaseMap->second.clear();
   }

   // Clear the phaseSubscriberMap
   phaseSubscriberMap.clear();

   // Clear the subscriber set for each phase (of phaseNotificationMap)
   itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::Validate);
   if ( itPhaseMap != phaseNotificationMap.end() )
   {
      itPhaseMap->second.clear();
   }

   itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::ApNotify);
   if ( itPhaseMap != phaseNotificationMap.end() )
   {
      itPhaseMap->second.clear();
   }

   itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::CpNotify);
   if ( itPhaseMap != phaseNotificationMap.end() )
   {
      itPhaseMap->second.clear();
   }

   itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::Commit);
   if ( itPhaseMap != phaseNotificationMap.end() )
   {
      itPhaseMap->second.clear();
   }

   // Clear the phaseNotificationMap
   phaseNotificationMap.clear();

   lock->stop_writing();
}

 ACS_CS_SubscriptionAgent * ACS_CS_SubscriptionAgent::getInstance ()
{
   return instance_::instance();
}

 void ACS_CS_SubscriptionAgent::start ()
{
   ACS_CS_SubscriptionAgent * instance = ACS_CS_SubscriptionAgent::getInstance();

   if (instance)
   {
      (void) instance->activate();
   }
}

 void ACS_CS_SubscriptionAgent::stop ()
{
   ACS_CS_SubscriptionAgent * instance = ACS_CS_SubscriptionAgent::getInstance();

   if (instance)
   {
      (void) instance->deActivate();    // Stop thread
      (void) instance->wait(0);         // Wait for thread to finish
      instance->close();                // Close thread
   }

   instance_::close();
}

 int ACS_CS_SubscriptionAgent::exec ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Starting the agent thread\n", __FUNCTION__));

   // Check if the lock is valid
   if ( ! lock )
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s :\n  Error: lock is NULL\n  Stopped the agent thread\n", __FUNCTION__));

      return 1;
   }

   startListener();

   // Create event array for the agent thread to WaitForMultipleObjects
   size_t numberOfHandles = tcpServerLocalhost->getActiveHandles();
   int handleArr[2 + numberOfHandles]; // two extra for shutdown and pdu events

   if (numberOfHandles == 0 || 
       !(tcpServerLocalhost->getActiveServerHandles(&handleArr[2], numberOfHandles)))
   {
       ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
               "(%t) %s :\n  Error: failed to get server info, numberOfHandles=%zd\n", 
               __FUNCTION__, numberOfHandles));
       return 0;
   }
   enum {ShutdownEventIndex = 0, PduEventIndex = 1};
   handleArr[ShutdownEventIndex] = shutdownEvent;
   handleArr[PduEventIndex] = pduEvent;
   int handleArrSize = sizeof(handleArr) / sizeof(int);

   for (int i = 0; i < handleArrSize; i++)
   {
      char buf[128];
      int len = snprintf(buf, sizeof(buf), "handleArr[%d]=%d", i, handleArr[i]);
      if (i > PduEventIndex)
      {
         struct in_addr addr = { ntohl(tcpServerLocalhost->getActiveServerAddress(handleArr[i])) };
         snprintf(buf + len, sizeof(buf) - len, ", %s", inet_ntoa(addr));
      }
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE, "(%t) %s\n", buf));
   }

   while ( ! exit_ ) // Set to true when the thread shall exit
   {
      // Wait for shutdown, accept and pdu events
      int returnValue = ACS_CS_Event::WaitForEvents(handleArrSize, handleArr, Connection_Timeout);

      switch (returnValue)
      {
         case ACS_CS_EVENT_RC_ERROR:   // Event failed
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                             "(%t) %s :\n  Error: WaitForEvents() failed\n", __FUNCTION__));

               exit_ = true;    // Terminate the agent thread
               break;
            }

         case ACS_CS_EVENT_RC_TIMEOUT:  // Time out
            {
               removeFinishedConnections();
               break;
            }

         case ShutdownEventIndex: // Shutdown event
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                             "(%t) %s :\n  Recieved a shutdown event\n", __FUNCTION__));

               exit_ = true;    // Terminate the agent thread
               break;
            }

         case PduEventIndex: // pduContainer ready for send
            {
               handlePduEvent();
               break;
            }

         default:
            {
               // check TCP handles
               if (returnValue > PduEventIndex && returnValue < handleArrSize)
               {
                  handleAcceptEvent(handleArr[returnValue]);
                  break;
               }

               ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                             "(%t) %s :\n  Error: Unexpected event signaled\n", __FUNCTION__));

               break;
            }
      } // End of switch

   } // End of the while loop

   deleteAllConnections(); // Delete all connections from connectionSet
   emptyPduContainer();           // Cleanup pduContainer
   clearMaps();            // Cleanup all Maps
   disableProfileChangeNotification();      // Allow the agent to temporarily hold all notifications after restarting

   // Disconnect to localhost before terminating the agent thread
   tcpServerLocalhost->stopListen();
   delete tcpServerLocalhost, tcpServerLocalhost = 0;


   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
           "(%t) %s :\n  Stopped the agent thread\n", __FUNCTION__));

   return 0;

}

 void ACS_CS_SubscriptionAgent::startListener ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Starting to listen on the local host\n", __FUNCTION__));

   if (tcpServerLocalhost)
   {
       tcpServerLocalhost->stopListen();
       delete tcpServerLocalhost, tcpServerLocalhost = 0;
   }

   tcpServerLocalhost = new ACS_CS_TCPServer();

   tcpServerLocalhost->addTcpServer(Server_Local_IP, CS_Subscription_Port);
   tcpServerLocalhost->addTcpServer(CLUSTER_IP_ETHA, CS_Subscription_Port);
   tcpServerLocalhost->addTcpServer(CLUSTER_IP_ETHB, CS_Subscription_Port);

   if (tcpServerLocalhost->startListen() < 3)
   {
       ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
               "(%t) %s :\n  Error: Cannot start listen on all servers\n", __FUNCTION__));
   }

   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
           "(%t) %s :\n  Listening on the local host\n", __FUNCTION__));
}

 void ACS_CS_SubscriptionAgent::removeFinishedConnections ()
{
   lock->start_writing();

   std::set<ACS_CS_SubscriptionConnection * >::iterator it = connectionSet.begin();

   // Remove finished connections from client set and subscriber maps
   while ( it != connectionSet.end() )
   {
      if ( ! (*it)->hasFinished() )  // Connection not finished
      {
         ++it;
      }
      else  // Connection finished
      {
         // Remove a finished one from the connection set
         ACS_CS_SubscriptionConnection * connection = *it;
         connectionSet.erase(it);
         it = connectionSet.begin();

         TableSubscriberMapType::iterator itTableMap;

         // Remove a finished one from the subscriber set for each table
         itTableMap = tableSubscriberMap.find(Scope_CPIdTable);
         if ( itTableMap != tableSubscriberMap.end() )
         {
            itTableMap->second.erase(connection);
         }

         itTableMap = tableSubscriberMap.find(Scope_CPGroupTable);
         if ( itTableMap != tableSubscriberMap.end() )
         {
            itTableMap->second.erase(connection);
         }

         itTableMap = tableSubscriberMap.find(Scope_NE);
         if ( itTableMap != tableSubscriberMap.end() )
         {
            itTableMap->second.erase(connection);
         }

         itTableMap = tableSubscriberMap.find(Scope_HWCTable);
		  if ( itTableMap != tableSubscriberMap.end() )
		  {
			 itTableMap->second.erase(connection);
		  }

         PhaseSubscriberMapType::iterator itPhaseMap;

         // Remove a finished one from the subscriber set for each phase (of phaseSubscriberMap)
         itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::Validate);
         if ( itPhaseMap != phaseSubscriberMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::ApNotify);
         if ( itPhaseMap != phaseSubscriberMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::CpNotify);
         if ( itPhaseMap != phaseSubscriberMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         itPhaseMap = phaseSubscriberMap.find(ACS_CS_API_OmProfilePhase::Commit);
         if ( itPhaseMap != phaseSubscriberMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         // Remove a finished one from the subscriber set for each phase (of phaseNotificationMap)
         itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::Validate);
         if ( itPhaseMap != phaseNotificationMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::ApNotify);
         if ( itPhaseMap != phaseNotificationMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::CpNotify);
         if ( itPhaseMap != phaseNotificationMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         itPhaseMap = phaseNotificationMap.find(ACS_CS_API_OmProfilePhase::Commit);
         if ( itPhaseMap != phaseNotificationMap.end() )
         {
            itPhaseMap->second.erase(connection);
         }

         // Delete the finished connection
         delete connection;

         ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                       "(%t) %s :\n  Deleted a finished connection instance (Remaining connections: %d)\n",
                       __FUNCTION__, connectionSet.size()));
      }
   }

   lock->stop_writing();
}

 void ACS_CS_SubscriptionAgent::handleAcceptEvent (int handle)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Recieved an accept event\n", __FUNCTION__));

   ACS_CS_TCPClient *clientConnection = tcpServerLocalhost->acceptConnectionForHandle(handle);

   if (clientConnection == NULL)
   {
       ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
               "(%t) %s :\n  Failed to accept incoming connection from client", __FUNCTION__));

       ACS_CS_EVENT(Event_SocketAPIFailure, ACS_CS_EventReporter::Severity_Event, "Accept client connection", "acceptConnectionForHandle fails", "");
       return;
   }

   // Created too many threads already, so disconnect the new one
   if (connectionSet.size() >= CS_Max_Threads)
   {
       ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
               "(%t) %s :\n  Error: Reached the maximum connection number: %d\n",
               __FUNCTION__, CS_Max_Threads));

       clientConnection->close();
       delete clientConnection;
       return;
   }

   struct in_addr addr = { ntohl(tcpServerLocalhost->getActiveServerAddress(handle)) };
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
           "(%t) %s :\n  Creating connection %s <--> %s\n", 
           __FUNCTION__, inet_ntoa(addr), clientConnection->getFullAddressAsString().c_str()));

   ACS_CS_SubscriptionConnection * connection = new ACS_CS_SubscriptionConnection(clientConnection, this);

   // Start the connection thread and update the connection set
   if (connection)
   {
       (void) connection->activate();
       connectionSet.insert(connection);

       ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
               "(%t) %s :\n  Started a connection thread (Total connections: %d)\n",
               __FUNCTION__, connectionSet.size()));
   }
   else
   {
       ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
               "(%t) %s :\n  Error: Cannot create a connection instance\n", __FUNCTION__));
   }
}

 void ACS_CS_SubscriptionAgent::handlePduEvent ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s,line: %i:\n  Received a pdu event\n",
                 __FUNCTION__,
                 __LINE__));

   lock->start_writing();

   if ( pduContainer.empty() )
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s, line: %i:\n  Warning: pduContainer is empty\n",
                    __FUNCTION__,
                    __LINE__));
   }
   else
   {
      PduContainer::iterator it = pduContainer.begin();
      bool needsDelete;
      while( it != pduContainer.end() )
      {
         needsDelete = dispatchSinglePdu(*it);
         if(needsDelete)
         {
            delete *it;
            it = pduContainer.erase(it);
         }
         else
         {
            ++it;
         }
      }
   }

   lock->stop_writing();
}

 bool ACS_CS_SubscriptionAgent::dispatchSinglePdu (const ACS_CS_PDU * const pdu)
{
   CS_Scope_Identifier scope = Scope_NotSpecified;
   if( pdu == NULL)
   {
      return true; // request delete to remove NULL PDU
   }

   if( pdu->getHeader() == NULL )
   {
      return true; // request delete to remove PDU with NULL header
   }

   CS_Primitive_Identifier primitiveType = pdu->getHeader()->getPrimitiveId();

   // we need to wait until startup supervision timer expired.
   // Leave the PDU in the container for now
   if( profileChangeNotificationDisabled &&
       primitiveType == Primitive_NotifyProfilePhaseChange )
   {
      return false; // no delete
   }

   ACS_CS_API_OmProfilePhase::PhaseValue phase = ACS_CS_API_OmProfilePhase::Idle;

   // Copy pdu to subscribers
   switch (primitiveType)
   {
      case Primitive_NotifyCpidTableChange:
         if ( scope == Scope_NotSpecified )
         {
            scope = Scope_CPIdTable;
         }
         // NO BREAK
      case Primitive_NotifyCpGroupTableChange:
         if ( scope == Scope_NotSpecified )
         {
            scope = Scope_CPGroupTable;
         }
         // NO BREAK
      case Primitive_NotifyHWCTableChange:
	   if ( scope == Scope_NotSpecified )
	   {
		  scope = Scope_HWCTable;
	   }
	   // NO BREAK
      case Primitive_NotifyNeTableChange:
         {
            if ( scope == Scope_NotSpecified )
            {
               scope = Scope_NE;
            }

            // Get the subscriber set for the scope
            TableSubscriberMapType::iterator it = tableSubscriberMap.find(scope);

            if ( it != tableSubscriberMap.end() )  // The subscriber set exists
            {
               if ( ! it->second.empty() ) // At least one subscriber exists
               {
                  std::set<ACS_CS_SubscriptionConnection *>::iterator itConnection = it->second.begin();

                  while ( itConnection != it->second.end() )  // Loop in the set and copy the pdu to each subscriber
                  {
                     if ( *itConnection != NULL )
                     {
                        (*itConnection)->copyToQueue(*pdu);
                        ++itConnection;
                     }
                     else  // Got a NULL SubscriptionConnection pointer (how could this happen?)
                     {
                        it->second.erase(itConnection);
                        itConnection = itConnection++;

                        ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                           "(%t) %s, line: %i:\n  Error: Got a NULL subscriber pointer and removed it from the set\n", __FUNCTION__, __LINE__));
                     }
                  }

                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s, line: %i:\n  Copied a %s pdu to %d connection thread(s)\n",
                                __FUNCTION__,
                                __LINE__,
                                NOTIFICATION_PDU_NAMES[ primitiveType-Primitive_NotifyCpidTableChange ],
                                it->second.size() ));
               }
               else  // No subscriber exists
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s, line: %i:\n  No subscriber for %s\n",
                                __FUNCTION__,
                                __LINE__,
                                TABLE_NAMES[scope]));
               }
            }
            else  // The subscriber set doesn't exist
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                             "(%t) %s, line: %i :\n  Never had a subscriber for %s\n",
                             __FUNCTION__,
                             __LINE__,
                             TABLE_NAMES[scope]));
            }

            break;
         }
      case Primitive_NotifyProfilePhaseChange:
         {
            // Get the phase from pdu
            const ACS_CS_SM_ProfilePhaseNotify *primitive = static_cast<const ACS_CS_SM_ProfilePhaseNotify *>(pdu->getPrimitive());
            ACS_CS_API_OmProfileChange omChange;
            primitive->getOmProfileChange(omChange);
            phase = omChange.phase;

            // Get subscriber set for the phase
            PhaseSubscriberMapType::iterator it = phaseNotificationMap.find(phase); // DO NOT use phaseSubscriberMap

            if ( it != phaseNotificationMap.end() )  // The subscriber set exists
            {
               if ( ! it->second.empty() )  // At least one subscriber exists
               {
                  std::set<ACS_CS_SubscriptionConnection *>::iterator itConnection = it->second.begin();

                  while ( itConnection != it->second.end() )  // Loop in the set and copy the pdu to each subscriber
                  {
                     if ( *itConnection != NULL )
                     {
                        (*itConnection)->copyToQueue(*pdu);
                        ++itConnection;
                     }
                     else  // Got a NULL SubscriptionConnection pointer (how could this happen?)
                     {
                        it->second.erase(itConnection);
                        itConnection = itConnection++;

                        ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                     "(%t) %s, line: %i:\n  Error: Got a NULL subscriber pointer and removed it from the set\n",
                                     __FUNCTION__,
                                     __LINE__));
                     }
                  }

                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s,line: %i :\n  Copied a %s pdu to %d connection thread(s)\n",
                                __FUNCTION__,
                                __LINE__,
                                NOTIFICATION_PDU_NAMES[ primitiveType-Primitive_NotifyCpidTableChange ],
                                it->second.size() ));

                  it->second.clear();  // Clear subscriber set after copying pdu
               }
               else  // No subscriber exists
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s, line: %i:\n  No subscriber for %s\n",
                                __FUNCTION__,
                                __LINE__,
                                NOTIFICATION_PDU_NAMES[ primitiveType-Primitive_NotifyCpidTableChange ] ));
               }
            }
            else  // The subscriber set doesn't exist
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                             "(%t) %s, line: %i:\n  Never had a subscriber for %s\n",
                             __FUNCTION__,
                             __LINE__,
                             NOTIFICATION_PDU_NAMES[ primitiveType-Primitive_NotifyCpidTableChange ] ));
            }

            break;
         }
      default:  // Unsupported or unknown pdu
         ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                       "(%t) %s, line: %i:\n  Error: Incorrect primitive type\n",
                       __FUNCTION__,
                       __LINE__));

         break;
   }

   return true; // delete pdu requested
}

 void ACS_CS_SubscriptionAgent::updatePhaseChange (const ACS_CS_API_OmProfileChange &newProfile)
 {
	 unsigned int notificationCount = 0;
	 return updatePhaseChange (newProfile, notificationCount);
 }

 void ACS_CS_SubscriptionAgent::updatePhaseChange (const ACS_CS_API_OmProfileChange &newProfile, unsigned int &notificationCount)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE, "(%t) %s :\n  Entered the function\n", __FUNCTION__));

   // write lock
   lock->start_writing();

   // check if we have any subscribers to notify for the current phase

   PhaseSubscriberMapType::iterator itrSubscriber = phaseSubscriberMap.find(newProfile.phase);
   if (itrSubscriber != phaseSubscriberMap.end())
   {
      // check if we have any entries in phaseNotificationMap for this phase
      // and delete if there are any
      PhaseSubscriberMapType::iterator itrNotification = phaseNotificationMap.find(newProfile.phase);

      if (itrNotification != phaseNotificationMap.end())
      {
         phaseNotificationMap.erase(itrNotification);
      }
      phaseNotificationMap.insert(PhaseSubscriberMapType::value_type
                                  (itrSubscriber->first, itrSubscriber->second));

      notificationCount = static_cast<unsigned int>(itrSubscriber->second.size());

      int notificationCount = static_cast<int>(itrSubscriber->second.size());
      ACS_CS_API_NeHandling::setNeSubscriberCount(notificationCount);

      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s :\n  Notifying %d subscriber(s) for phase (%d)\n",
                    __FUNCTION__, notificationCount, newProfile.phase));
   }
   else
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s :\n  No subscriber for phase (%d)\n",
                    __FUNCTION__, newProfile.phase));
      // Nothing to notify so no need to send updates
      ACS_CS_API_NeHandling::setNeSubscriberCount(0);
      // write unlock
      lock->stop_writing();

      return;
   }
   ACS_CS_SM_ProfilePhaseNotify* omProfileNotify = new ACS_CS_SM_ProfilePhaseNotify();
   omProfileNotify->setOmProfileChange(newProfile);
   // Construct PDU to send to subscription Manager
   ACS_CS_PDU *omProfileNotifyPDU = createNotificationPDU(
                                                          ACS_CS_Protocol::Scope_Subscription,
                                                          omProfileNotify->getPrimitiveType(),
                                                          omProfileNotify);
   if (omProfileNotifyPDU)
   {
      // insert PDU
      pduContainer.push_back(omProfileNotifyPDU);
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s :\n  Inserted a pdu into pduContainer for phase (%d)\n",
                    __FUNCTION__, newProfile.phase));
      // Signal PDU event to wake up the thread
      signalPduEvent();
   }

   // write unlock
   lock->stop_writing();

}

 void ACS_CS_SubscriptionAgent::addTableSubscriber (ACS_CS_Protocol::CS_Scope_Identifier scope, ACS_CS_SubscriptionConnection *subscriber)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Recieved a subscribe request for scope (%d)\n",
                 __FUNCTION__, scope));

   lock->start_writing();

   switch(scope)
   {
   	   case Scope_HWCTable:
   		   // No break
      case Scope_NE:
         // No break
      case Scope_CPIdTable:
         // No break
      case Scope_CPGroupTable:
         {
            TableSubscriberMapType::iterator it = tableSubscriberMap.find(scope);
            if ( it != tableSubscriberMap.end() )  // Subscriber set exists
            {
               it->second.insert(subscriber);
            }
            else   // No subscriber set exists
            {
               std::set<ACS_CS_SubscriptionConnection *> newSubscriberSet;
               newSubscriberSet.insert(subscriber);
               tableSubscriberMap.insert( TableSubscriberMapType::value_type(scope, newSubscriberSet) );
            }

            ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                          "(%t) %s :\n  Added a subscriber for %s\n",
                          __FUNCTION__, TABLE_NAMES[scope]));

            break;
         }
      default:  // Unsupported or unknown scopes
         {
            ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                          "(%t) %s :\n  Error: Incorrect scope (%d)\n",
                          __FUNCTION__, scope));

            break;
         }
   }

   lock->stop_writing();
}

 void ACS_CS_SubscriptionAgent::removeTableSubscriber (ACS_CS_Protocol::CS_Scope_Identifier scope, ACS_CS_SubscriptionConnection *subscriber)
{

   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Recieved a unsubscribe request for scope (%d)\n",
                 __FUNCTION__, scope));

   lock->start_writing();

   switch(scope)
   {
   	   case Scope_HWCTable:
		// No break
      case Scope_NE:
         // No break
      case Scope_CPIdTable:
         // No break
      case Scope_CPGroupTable:
         {
            TableSubscriberMapType::iterator it = tableSubscriberMap.find(scope);
            if ( it != tableSubscriberMap.end() )  // Subscriber set exists
            {
               std::set<ACS_CS_SubscriptionConnection *>::size_type removedNum = it->second.erase(subscriber);

               if ( removedNum != 0 )  // Removed the subscriber from the existing set
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s :\n  Removed a subscriber for %s\n",
                                __FUNCTION__, TABLE_NAMES[scope]));
               }
               else // Cannot find the subscriber from the existing set
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s :\n  Error: Cannot find a subscriber from the set for %s\n",
                                __FUNCTION__, TABLE_NAMES[scope]));
               }
            }
            else    // No subscriber set exists
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                             "(%t) %s :\n  Error: Cannot find the subscriber set for %s\n",
                             __FUNCTION__, TABLE_NAMES[scope]));
            }

            break;
         }
      default:  // Unsupported or unknown scopes
         {
            ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                          "(%t) %s :\n  Error: Incorrect scope (%d)\n",
                          __FUNCTION__, scope));

            break;
         }
   }

   lock->stop_writing();

}

 void ACS_CS_SubscriptionAgent::addPhaseSubscriber (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_SubscriptionConnection *subscriber)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Recieved a subscribe request for phase (%d)\n",
                 __FUNCTION__, phase));

   lock->start_writing();

   switch(phase)
   {
      case ACS_CS_API_OmProfilePhase::Validate:
         // NO BREAK
      case ACS_CS_API_OmProfilePhase::ApNotify:
         // NO BREAK
      case ACS_CS_API_OmProfilePhase::CpNotify:
         // NO BREAK
      case ACS_CS_API_OmProfilePhase::Commit:
         {
            PhaseSubscriberMapType::iterator it = phaseSubscriberMap.find(phase);
            if ( it != phaseSubscriberMap.end() )  // Subscriber set exists
            {
               it->second.insert(subscriber);
            }
            else   // No subscriber set exists
            {
               std::set<ACS_CS_SubscriptionConnection *> newSubscriberSet;
               newSubscriberSet.insert(subscriber);
               phaseSubscriberMap.insert( PhaseSubscriberMapType::value_type(phase, newSubscriberSet) );
            }

            ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                          "(%t) %s :\n  Added a subscriber for phase (%d)\n",
                          __FUNCTION__, phase));

            break;
         }
      default:  // Unsupported or unknown phases
         {
            ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                          "(%t) %s :\n  Error: Incorrect phase (%d)\n",
                          __FUNCTION__, phase));

            break;
         }
   }

   lock->stop_writing();

   // Update phase subscribers in IMM
   updatePhaseSubcriberCount();
}

 void ACS_CS_SubscriptionAgent::removePhaseSubscriber (ACS_CS_API_OmProfilePhase::PhaseValue phase, ACS_CS_SubscriptionConnection *subscriber)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Recieved a unsubscribe request for phase (%d)\n",
                 __FUNCTION__, phase));

   lock->start_writing();

   switch(phase)
   {
      case ACS_CS_API_OmProfilePhase::Validate:
         // NO BREAK
      case ACS_CS_API_OmProfilePhase::ApNotify:
         // NO BREAK
      case ACS_CS_API_OmProfilePhase::CpNotify:
         // NO BREAK
      case ACS_CS_API_OmProfilePhase::Commit:
         {
            PhaseSubscriberMapType::iterator it = phaseSubscriberMap.find(phase);
            if ( it != phaseSubscriberMap.end() )  // Subscriber set exists
            {
               std::set<ACS_CS_SubscriptionConnection *>::size_type removedNum = it->second.erase(subscriber);

               if ( removedNum != 0 )  // Removed the subscriber from the existing set
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s :\n  Removed a subscriber for phase (%d)\n",
                                __FUNCTION__, phase));
               }
               else // Cannot find the subscriber from the existing set
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                                "(%t) %s :\n  Error: Cannot find a subscriber from the set for phase (%d)\n",
                                __FUNCTION__, phase));
               }
            }
            else   // No subscriber set exists
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                             "(%t) %s :\n  Error: Cannot fine the subscriber set for phase (%d)\n",
                             __FUNCTION__, phase));
            }

            break;
         }
      default:  // Unsupported or unknown phases
         {
            ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                          "(%t) %s :\n  Error: Incorrect phase (%d)\n",
                          __FUNCTION__, phase));

            break;
         }
   }

   lock->stop_writing();

   // Update phase subscribers in IMM
   updatePhaseSubcriberCount();
}

 void ACS_CS_SubscriptionAgent::handleNEUpdates (const ACS_CS_NetworkElementChange &neChange)
{
     ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
            "(%t) %s :\n  Entered the function\n", __FUNCTION__));

    ACS_CS_SM_NETableNotify* neTableNotify = new ACS_CS_SM_NETableNotify();

    if (neTableNotify)
    {
        neTableNotify->setNeChange(neChange);

        // Construct PDU to send to subscription Manager
        ACS_CS_PDU *neNotifyPDU = createNotificationPDU(
                ACS_CS_Protocol::Scope_Subscription,
                neTableNotify->getPrimitiveType(),
                neTableNotify);

        if (neNotifyPDU)
        {
            // insert PDU
            lock->start_writing();

            pduContainer.push_back(neNotifyPDU);

            // Signal PDU event to wake up the thread
            signalPduEvent();

            lock->stop_writing();

            ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                    "(%t) %s :\n  Inserted a pdu into pduContainer\n", __FUNCTION__));
        }
    }
}

 void ACS_CS_SubscriptionAgent::updatePhaseSubcriberCount()
{
     ACS_CS_API_NE_NS::SetPhaseSubscriberData phaseData;
     memset(&phaseData, 0, sizeof(phaseData));

     phaseData.validate = getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Validate);
     phaseData.apNotify = getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::ApNotify);
     phaseData.cpNotify = getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::CpNotify);
     phaseData.commit   = getPhaseSubscriberCount(ACS_CS_API_OmProfilePhase::Commit);

     ACS_CS_API_NeHandling::setPhaseSubscriberCount(phaseData);
}

 ACS_CS_PDU * ACS_CS_SubscriptionAgent::createNotificationPDU (ACS_CS_Protocol::CS_Scope_Identifier scope, ACS_CS_Protocol::CS_Primitive_Identifier primitiveType, ACS_CS_Primitive *primitive)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Entered the function\n", __FUNCTION__ ));

   ACS_CS_Header* header = new ACS_CS_Header();
   ACS_CS_PDU* pdu = 0;

   if (primitive && header)
   {
      header->setPrimitive(primitiveType);
      header->setRequestId(ACS_CS_Protocol::DEFAULT_REQUEST_ID);
      header->setScope(scope);
      header->setVersion(ACS_CS_Protocol::Latest_Version);
      header->setTotalLength(ACS_CS_Protocol::CS_Header_Size + primitive->getLength());

      pdu = new ACS_CS_PDU(header, primitive);
   }
   else
   {
      delete header;
   }

   return pdu;
}

 int unsigned ACS_CS_SubscriptionAgent::getTableSubscriberCount (ACS_CS_Protocol::CS_Scope_Identifier scope) const
{
   unsigned int count = 0;

   if ( (scope == Scope_CPIdTable) || (scope == Scope_CPGroupTable) || (scope == Scope_NE) || (scope == Scope_HWCTable) )
   {
      lock->start_writing();

      TableSubscriberMapType::const_iterator it = tableSubscriberMap.find(scope);

      if ( it != tableSubscriberMap.end() )   // The scope (or subscriber set) exists
      {
         count = static_cast<unsigned int>( it->second.size() );
      }

      lock->stop_writing();
   }

   return count;
}

 unsigned int ACS_CS_SubscriptionAgent::getPhaseSubscriberCount (ACS_CS_API_OmProfilePhase::PhaseValue phase) const
{
   unsigned int count = 0;

   if ( (phase == ACS_CS_API_OmProfilePhase::Validate) ||
        (phase == ACS_CS_API_OmProfilePhase::ApNotify) ||
        (phase == ACS_CS_API_OmProfilePhase::CpNotify) ||
        (phase == ACS_CS_API_OmProfilePhase::Commit) )
   {
      lock->start_writing();

      PhaseSubscriberMapType::const_iterator it = phaseSubscriberMap.find(phase);

      if ( it != phaseSubscriberMap.end() )   // The phase (or subscriber set) exists
      {
         count = static_cast<unsigned int>( it->second.size() );
      }

      lock->stop_writing();
   }

   return count;
}

 void ACS_CS_SubscriptionAgent::enableProfileChangeNotification ()
{
   lock->start_writing();

   profileChangeNotificationDisabled = false;

   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Enabled the agent to handle pdu events\n", __FUNCTION__ ));

   // Signal PDU event to wake up the thread
   signalPduEvent();

   lock->stop_writing();
}

 void ACS_CS_SubscriptionAgent::disableProfileChangeNotification ()
{
   lock->start_writing();

   profileChangeNotificationDisabled = true;

   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Disabled the agent to handle pdu events\n", __FUNCTION__ ));

   lock->stop_writing();
}

 void ACS_CS_SubscriptionAgent::handleGroupUpdates (ACS_CS_CpGroupData &groupData)
{
     ACS_CS_CpGroupChange groupChange;

     ACS_CS_SM_CPGroupTableNotify* groupTableNotify = new ACS_CS_SM_CPGroupTableNotify();

     groupChange.cpGroupData.push_back(groupData);

     if (groupTableNotify)
     {
         groupTableNotify->setCpGroupChange(groupChange);

         // Construct PDU to send to subscription Manager
         ACS_CS_PDU *groupNotifyPDU = createNotificationPDU(
                 ACS_CS_Protocol::Scope_Subscription,
                 groupTableNotify->getPrimitiveType(),
                 groupTableNotify);

         if (groupNotifyPDU)
         {
             // insert PDU
             lock->start_writing();

             pduContainer.push_back(groupNotifyPDU);

             // Signal PDU event to wake up the thread
             signalPduEvent();

             lock->stop_writing();

             ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                     "(%t) %s :\n  Inserted a pdu into pduContainer\n", __FUNCTION__));
         }
     }
}

 void ACS_CS_SubscriptionAgent::handleCPIdTableUpdates (const ACS_CS_CpTableData &cpData)
{
     ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                   "(%t) %s :\n  Entered the function\n", __FUNCTION__));

     ACS_CS_CpTableChange cpTableChange;

     cpTableChange.cpData.push_back(cpData);

     ACS_CS_SM_CPTableNotify* cpTableNotify = new ACS_CS_SM_CPTableNotify();

     if (cpTableNotify)
     {
         cpTableNotify->setCpTableChange(cpTableChange);

         // Construct PDU to send to subscription Manager
         ACS_CS_PDU *cpTableNotifyPDU = createNotificationPDU(
                 ACS_CS_Protocol::Scope_Subscription,
                 cpTableNotify->getPrimitiveType(),
                 cpTableNotify);

         if (cpTableNotifyPDU)
         {
             // insert PDU
             lock->start_writing();

             pduContainer.push_back(cpTableNotifyPDU);

             // Signal PDU event to wake up the thread
             signalPduEvent();

             lock->stop_writing();

             ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                     "(%t) %s :\n  Inserted a pdu into pduContainer\n", __FUNCTION__));
         }
     }
     else
     {
         ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Error: Cannot extract cpData to construct a pdu\n", __FUNCTION__));
     }
}

void ACS_CS_SubscriptionAgent::handleHWCUpdates(const ACS_CS_HWCChange &hwcChange)
{
	ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
				 "(%t) %s :\n  Entered the function\n", __FUNCTION__));

	ACS_CS_SM_HWCTableNotify *hwcTableNotify = new ACS_CS_SM_HWCTableNotify();

	if (hwcTableNotify)
	{
		hwcTableNotify->setHWCChange(hwcChange);

		// Construct PDU to send to subscription Manager
		ACS_CS_PDU *hwcNotifyPDU = createNotificationPDU(
		ACS_CS_Protocol::Scope_Subscription,
		hwcTableNotify->getPrimitiveType(),
		hwcTableNotify);

		if (hwcNotifyPDU)
		{
			// insert PDU
			lock->start_writing();

			pduContainer.push_back(hwcNotifyPDU);

			// Signal PDU event to wake up the thread
			signalPduEvent();

			lock->stop_writing();

			ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
				   "(%t) %s :\n  Inserted a pdu into pduContainer\n", __FUNCTION__));
		}
	}
}

 void ACS_CS_SubscriptionAgent::updateOgClear (const std::string& groupName, const ACS_CS_API_OgChange::ACS_CS_API_EmptyOG_ReasonCode& reason)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                 "(%t) %s :\n  Entered the function\n", __FUNCTION__));

   ACS_CS_CpGroupChange groupChange;

   ACS_CS_CpGroupData group;

   group.cpGroupName = groupName;
   group.operationType = ACS_CS_API_TableChangeOperation::Change;
   group.emptyOgReason = reason;

   // push this group into vector
   groupChange.cpGroupData.push_back(group);

   ACS_CS_SM_CPGroupTableNotify* groupTableNotify = new ACS_CS_SM_CPGroupTableNotify();

   if (groupTableNotify)
   {
      groupTableNotify->setCpGroupChange(groupChange);

      // Construct PDU to send to subscription Manager
      ACS_CS_PDU *groupNotifyPDU = createNotificationPDU(
         ACS_CS_Protocol::Scope_Subscription,
         groupTableNotify->getPrimitiveType(),
         groupTableNotify);

      if (groupNotifyPDU)
      {
         // insert PDU
         lock->start_writing();

         pduContainer.push_back(groupNotifyPDU);

         // Signal PDU event to wake up the thread
         signalPduEvent();

         lock->stop_writing();

         ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
                       "(%t) %s :\n  Inserted a pdu into pduContainer\n", __FUNCTION__));
      }
   }
}

 void ACS_CS_SubscriptionAgent::signalPduEvent ()
{
   // Signal the PDU event to wake up the thread
   if (!ACS_CS_Event::SetEvent(pduEvent))
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionAgent_TRACE,
         "(%t) %s :\n  Error: Cannot signal a pdu event\n",
         __FUNCTION__));
   }
}
