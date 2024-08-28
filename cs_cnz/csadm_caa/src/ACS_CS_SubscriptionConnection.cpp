

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

#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_Primitive.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_SM_SubscribeTable.h"
#include "ACS_CS_SM_SubscribeProfilePhaseChange.h"
#include "ACS_CS_SubscriptionAgent.h"

// ACS_CS_SubscriptionConnection
#include "ACS_CS_SubscriptionConnection.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_SubscriptionConnection_TRACE);

using namespace std;
using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using namespace ACS_CS_Subscription_NS;


// Class ACS_CS_SubscriptionConnection 

ACS_CS_SubscriptionConnection::ACS_CS_SubscriptionConnection (ACS_CS_TCPClient *tcpClient, ACS_CS_SubscriptionAgent *agent)
      : subscriptionAgent(agent),
        shutdownEvent(0),
        tcpClient(0),
        pduEvent(0),
        csParser(0),
        finished(false),
        lock(0)
{
    ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
            "(%t) %s :\n  Constructing a connection instance\n", __FUNCTION__));

    this->tcpClient = tcpClient;

    // Open shutdown event used to handle the signal that the whole service shall shutdown
    shutdownEvent = ACS_CS_Event::OpenNamedEvent(EVENT_NAME_SHUTDOWN);
    if (shutdownEvent == NULL)
    {
        ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                "(%t) %s :\n  Error: Cannot open the shutdown event\n", __FUNCTION__));

        ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event,"Open Event", "Cannot open shutdown event", "");
    }

    // Create pdu event used to wait for the agent to copy notification PDUs into pduQueue
    pduEvent = ACS_CS_Event::CreateEvent(false, false, NULL);
    if (pduEvent == NULL)
    {
        ACS_CS_FTRACE((ACS_CS_SubscriptionConnection_TRACE, LOG_LEVEL_ERROR,
                "(%t) %s :\n  Error: Cannot create the pdu event\n", __FUNCTION__));

        ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event, "Create Event", "Cannot create PDU event", "");
    }

    // Create the Parser object to retrieve PDUs from the TCP stream
    csParser = new ACS_CS_Parser(ACS_CS_Protocol::Protocol_CS);

    // Create the lock to access pduQueue
    lock = new ACS_CS_ReaderWriterLock();
}


ACS_CS_SubscriptionConnection::~ACS_CS_SubscriptionConnection()
{
    ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
            "(%t) %s :\n  Destructing a connection instance\n", __FUNCTION__));

   if (pduEvent)
   {
        ACS_CS_Event::CloseEvent(pduEvent);
   }

   if (tcpClient)
   {
       tcpClient->close();
       delete tcpClient;
   }

   if (csParser)
   {
      delete csParser;
   }

   emptyQueue(); // Call this before DELETE the lock

   delete lock;
}


 void ACS_CS_SubscriptionConnection::emptyQueue ()
{
   lock->start_writing();

   while ( ! pduQueue.empty() )
   {
      ACS_CS_PDU * pdu = pduQueue.front();
      pduQueue.pop();  // Remove pdu from the queue

      if (pdu)
      {
         delete pdu;
      }
   }

   lock->stop_writing();
}

 int ACS_CS_SubscriptionConnection::exec ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Starting a connection thread\n", __FUNCTION__));

   // Check if the objects are valid
   if ( (tcpClient == NULL) || (csParser == NULL) || (lock == NULL) )
   {
       ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
               "(%t) %s :\n  Error: tcpClient, csParser or lock is NULL (tcpClient = %d, csParser = %d, lock = %d)\n",
               __FUNCTION__, tcpClient, csParser, lock));

       ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
               "(%t) %s :\n  Stopped a connection thread\n", __FUNCTION__))

       return 1;
   }

   // Create the event array for the connection thread to WaitForMultipleObjects
   int handleArr[Number_Of_Events] = {shutdownEvent, tcpClient->getHandle(), pduEvent};

   while ( ! exit_ )	// Set to true when the thread shall exit
   {
      // Wait for shutdown, connection (read) and pdu events
      int returnValue = ACS_CS_Event::WaitForEvents(Number_Of_Events, handleArr, Connection_Timeout);

      switch (returnValue)
      {
         case ACS_CS_EVENT_RC_ERROR:       // Event failed
            {
               ACS_CS_FTRACE((ACS_CS_SubscriptionConnection_TRACE, LOG_LEVEL_ERROR,
                       "(%t) %s :\n  Error: Cannot WaitForEvents failed\n", __FUNCTION__));

               ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event, "Waiting for clients",
                       "Cannot wait for some connection event",
                       "");

               exit_ = true;
               break;
            }

         case ACS_CS_EVENT_RC_TIMEOUT: // Time out
            {
               break; // No action needed, just wake up to check if exit_ has been set
            }

         case Shutdown_Event_Index:	// Shutdown event
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                             "(%t) %s :\n  Received a shutdown event\n", __FUNCTION__));

               exit_ = true;
               break;
            }

         case Socket_Event_Index:	// Socket ready for read or close
            {
               if ( handleConnectionEvent() == Result_Client_Disconnected )
               {
                  exit_ = true;
               }

               break;
            }

         case Pdu_Event_Index:           // pduQueue ready for send
            {
               if ( handlePduEvent() == Result_Client_Disconnected )
               {
                  exit_ = true;
               }

               break;
            }

         default:
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                             "(%t) %s :\n  Error: Unexpected event signaled\n", __FUNCTION__));

               break;
            }
      } // End of switch

   } // End of the while loop

   // Disconnect to client before terminating the thread
   if (tcpClient)
   {
       tcpClient->close();
   }

   emptyQueue();

   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Stopped a connection thread\n", __FUNCTION__));

   return 0;
}

 ACS_CS_Subscription_NS::Handle_Event_Results ACS_CS_SubscriptionConnection::handleConnectionEvent ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Received a connection event\n", __FUNCTION__));


   int bytesRead = 0;
   char buffer[BUFSIZ] = {0};
   ACS_CS_TCPClient::LinkStatus_t status;

   // Retrieve data from TCP stream and store it into csParser
   do
   {
      status = tcpClient->read((char*)&buffer, (size_t) BUFSIZ, bytesRead); // Try to read TCP data

      if ( bytesRead > 0 )
      {
         if ( csParser->newData(buffer, bytesRead) )       // Copy data to parser
         {
            ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                          "(%t) %s :\n  Error: Cannot copy data into csParser\n", __FUNCTION__));
         }
      }
   }
   while ( bytesRead > 0 );	// Loop as long as data is returned by the TCP connection

   if ( status == ACS_CS_TCPClient::NotConnected )	// Client close
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: Connection disconnected\n", __FUNCTION__));
      (void)parse();
      return Result_Client_Disconnected;
   }
   else if ( status == ACS_CS_TCPClient::OK )       // Data has been read and status is still OK
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Received data from client\n", __FUNCTION__));

      return parse();  // Parse the retrieved data
   }

   return Result_OK;

}

 ACS_CS_Subscription_NS::Handle_Event_Results ACS_CS_SubscriptionConnection::parse ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Starting to parse data\n", __FUNCTION__));

   int error = 0;

   // Parse the retrieved data, otherwise delete it
   try
   {
      error = csParser->parse();
   }
   catch( ACS_CS_Exception ex )
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: csParser threw an exception\n", __FUNCTION__));

      return Result_Parser_Error;
   }

   if ( error < 0 )
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: Internal error of csParser\n", __FUNCTION__));

      ACS_CS_EVENT(Event_ProgrammingError, ACS_CS_EventReporter::Severity_Event,
                   "Error Parsing Data",
                   "Cannot parse data by ACS_CS_Parser",
                   "");

      return Result_Parser_Error;
   }

   ACS_CS_PDU pdu;

   // Repeatedly get pdu from csParser and handle subscribe request
   while ( csParser->getPDU(pdu) )
   {
      const ACS_CS_Header *header = dynamic_cast<const ACS_CS_Header *>(pdu.getHeader());

      if ( header == NULL )
      {
         ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                       "(%t) %s :\n  Error: Invalid header\n", __FUNCTION__));

         break;
      }

      const ACS_CS_Primitive *primitive = pdu.getPrimitive();

      if ( primitive == NULL )
      {
         ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                       "(%t) %s :\n  Error: Invalid primitive\n", __FUNCTION__));

         break;
      }

      const CS_Primitive_Identifier primitiveType = header->getPrimitiveId();

      // Check primitive and register the subscribers to the tables as they want
      switch (primitiveType)
      {
         case Primitive_SubscribeTable: // Subscribe to CpId, CpGroup and NE table changes
            {
               if ( primitive->getPrimitiveType() == Primitive_SubscribeTable )
               {
                  const ACS_CS_SM_SubscribeTable *newPrimitive =
                     static_cast<const ACS_CS_SM_SubscribeTable *> (primitive);

                  if ( newPrimitive->isSubscription )
                  {
                     ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                                   "(%t) %s :\n  Recieved a table subscribe request\n", __FUNCTION__));

                     subscriptionAgent->addTableSubscriber( newPrimitive->getTableScope(), this );
                  }
                  else  // Unsubscribe request
                  {
                     ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                                   "(%t) %s :\n  Recieved a table unsubscribe request\n", __FUNCTION__));

                     subscriptionAgent->removeTableSubscriber( newPrimitive->getTableScope(), this );
                  }
               }
               else // Header and primitive contain different primitive type
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                                "(%t) %s :\n  Error: Mismatched pritimive types\n", __FUNCTION__));
               }

               break;
            }
         case Primitive_SubscribeProfilePhaseChange: // Subscibe to OmProfile phase changes
            {
               if ( primitive->getPrimitiveType() == Primitive_SubscribeProfilePhaseChange )
               {
                  const ACS_CS_SM_SubscribeProfilePhaseChange *newPrimitive =
                     static_cast<const ACS_CS_SM_SubscribeProfilePhaseChange *> (primitive);

                  if ( newPrimitive->isSubscription )
                  {
                     ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                                   "(%t) %s :\n  Recieved a phase subscribe request\n", __FUNCTION__));

                     subscriptionAgent->addPhaseSubscriber( newPrimitive->getPhase(), this );
                  }
                  else  // Unsubscribe request
                  {
                     ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                                   "(%t) %s :\n  Recieved a phase unsubscribe request\n", __FUNCTION__));

                     subscriptionAgent->removePhaseSubscriber( newPrimitive->getPhase(), this );
                  }
               }
               else // Header and primitive contain different primitive type
               {
                  ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                                "(%t) %s :\n  Error: Mismatched primitive types\n", __FUNCTION__));
               }

               break;
            }
         default: // Not a valid subscribe request
            {
               ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                             "(%t) %s :\n  Error: Incorrect primitive type (%d)\n",
                             __FUNCTION__, primitiveType));

               break;
            }

      } // End of switch case

   } // End of while loop

   return Result_OK;
}

 ACS_CS_Subscription_NS::Handle_Event_Results ACS_CS_SubscriptionConnection::handlePduEvent ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Received a pdu event\n", __FUNCTION__));

   lock->start_writing();

   if ( pduQueue.empty() )
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: Empty pduQueue\n", __FUNCTION__));

      lock->stop_writing();

      return Result_Empty_Queue;
   }

   Handle_Event_Results result = Result_OK;

   // Send all queued PDUs to the client
   while ( ! pduQueue.empty() )
   {
      ACS_CS_PDU * pdu = pduQueue.front();
      pduQueue.pop();  // Remove the pdu from the queue

      if (pdu)
      {
         if ( result != Result_Client_Disconnected )
         {
            result = send(*pdu);   // Send pdu to the client
         }

         delete pdu;   // Delete the pdu created by copyToQueue()
      }
   }

   lock->stop_writing();

   return result;
}

 ACS_CS_Subscription_NS::Handle_Event_Results ACS_CS_SubscriptionConnection::send (const ACS_CS_PDU &pdu) const
{
   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Sending a pdu to client\n", __FUNCTION__));

   const int size = pdu.getLength();

   if ( size <= 0 )
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: Zero pdu length\n", __FUNCTION__));

      return Result_Invalid_PDU;
   }

   char * buffer = new char[size];
   (void) pdu.getBuffer(buffer, size);   // Copy pdu data into the buffer

   size_t bytesSent = 0;           // The size of data that is actually sent
   size_t offset = 0;              // The pointer to the remaining data that should be sent
   ACS_CS_TCPClient::LinkStatus_t status = ACS_CS_TCPClient::Failed;

   // Send the buffered data repeatedly, as tcpClient cannot guarantee to send out all the data at once
   do
   {
      offset += bytesSent;  // Don't send the bytes that already sent
      status = tcpClient->send( (buffer + offset), (size - offset), bytesSent );
   }
   while ( status == ACS_CS_TCPClient::Failed ); // There is still some data remaining to send

   delete [] buffer;

   if ( status == ACS_CS_TCPClient::NotConnected )
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: Client disconnected\n", __FUNCTION__));

      return Result_Client_Disconnected;
   }

   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Sent a pdu to client\n", __FUNCTION__));

   return Result_OK;
}

 void ACS_CS_SubscriptionConnection::disconnect ()
{
   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Disconnected to client\n", __FUNCTION__));

   if (tcpClient)
   {
       tcpClient->close();
   }

   exit_ = true; // Prepare to terminate the connection thread
}

 bool ACS_CS_SubscriptionConnection::hasFinished () const
{
   return finished;
}

 void ACS_CS_SubscriptionConnection::finish ()
{
   finished = true;
}

 void ACS_CS_SubscriptionConnection::copyToQueue (const ACS_CS_PDU &pdu)
{
   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Copying a pdu from agent\n", __FUNCTION__));

   lock->start_writing();

   ACS_CS_PDU *newPDU = new ACS_CS_PDU(pdu); // newPDU will be deleted in handlePduEvent()

   if (newPDU)
   {
      (void) pduQueue.push(newPDU);  // Copy pdu into the queue
   }
   else  // newPDU == NULL
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: Failed to copy a pdu from agent\n", __FUNCTION__));

      lock->stop_writing();

      return;
   }

   ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                 "(%t) %s :\n  Copied a pdu from agent (pduQueue size: %d)\n",
                 __FUNCTION__, pduQueue.size()));

   // Signal a PDU event to wake up the thread
   if (!ACS_CS_Event::SetEvent(pduEvent))
   {
      ACS_CS_TRACE((ACS_CS_SubscriptionConnection_TRACE,
                    "(%t) %s :\n  Error: Cannot signal a pdu event\n", __FUNCTION__));

      ACS_CS_EVENT(Event_SubSystemError, ACS_CS_EventReporter::Severity_Event, "Create Event", "Cannot signal PDU event", "");
   }

   lock->stop_writing();
}
