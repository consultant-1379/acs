

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************




#include "TCPServer.hxx"
#include "TCPClient.hxx"

#include "ACS_CS_PDU.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_TableSignature.h"
#include "ACS_CS_ACKTable.h"
#include "ACS_CS_HostFile.h"

#include <vector>
#include <sstream>


// ACS_CS_MasterReplicator
#include "ACS_CS_MasterReplicator.h"


ACS_CS_Trace_TDEF(ACS_CS_MasterReplicator_TRACE);

using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using std::ostringstream;
using std::vector;
using std::map;
using std::queue;



// Class ACS_CS_MasterReplicator 

ACS_CS_MasterReplicator* ACS_CS_MasterReplicator::replicatorInstance = 0;

ACS_CS_MasterReplicator::ACS_CS_MasterReplicator()
      : tcpServerEthA(0),
        tcpServerEthB(0),
        ethAAvailable(false),
        ethBAvailable(false),
        acceptEventEthA(0),
        acceptEventEthB(0),
        socketEthA(INVALID_SOCKET),
        socketEthB(INVALID_SOCKET),
        shutdownEvent(0),
        clientMap(0),
        tableMap(0),
        queueMap(0),
        lock(0),
        pduEvent(0),
        ackClientIP(0),
        ackParser(0)
{

   // This event is signaled when the service should shutdown
   shutdownEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_SHUTDOWN);

   if (shutdownEvent == 0)
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot open shutdown event (Error code: %d)\n",
         __FUNCTION__, lastError));

      ACS_CS_EVENT(Event_SubSystemError,
         "Open Windows Event",
         "Cannot open a Windows event",
         "");
   }

   pduEvent = CreateEvent(NULL, FALSE, FALSE, EVENT_NAME_REPLICATION_MASTER);

   if (pduEvent == 0)
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot create pdu event (Error code: %d)\n",
         __FUNCTION__, lastError));

      ACS_CS_EVENT(Event_SubSystemError,
         "Create Windows Event",
         "Cannot create a Windows event",
         "");
   }

   lock = new ACS_CS_ReaderWriterLock();
   tableMap = new tableMapType();
   clientMap = new clientMapType();
   queueMap = new queueMapType();
   ackParser = new ACS_CS_Parser();

}


ACS_CS_MasterReplicator::~ACS_CS_MasterReplicator()
{

   if (shutdownEvent)
   {
      (void) CloseHandle(shutdownEvent);
   }

   if (acceptEventEthA)
   {
      (void) CloseHandle(acceptEventEthA);
   }

   if (acceptEventEthB)
   {
      (void) CloseHandle(acceptEventEthB);
   }

   if (pduEvent)
   {
      (void) CloseHandle(pduEvent);
   }

   emptyQueues(); // Don't delete maps before this

   delete clientMap;

   delete tableMap;

   delete queueMap;

   delete ackParser;

   // Delete lock last (it is used in the emptyQueues() function)
   delete lock;

}



 ACS_CS_MasterReplicator * ACS_CS_MasterReplicator::getInstance ()
{

   if (ACS_CS_MasterReplicator::replicatorInstance == 0)
   {
      ACS_CS_MasterReplicator::replicatorInstance = new ACS_CS_MasterReplicator();
   }

   return ACS_CS_MasterReplicator::replicatorInstance;

}

 void ACS_CS_MasterReplicator::start ()
{

   ACS_CS_MasterReplicator * instance = ACS_CS_MasterReplicator::getInstance();

   if (instance)
   {
      (void) instance->activate();
   }

}

 void ACS_CS_MasterReplicator::stop ()
{

   ACS_CS_MasterReplicator * instance = ACS_CS_MasterReplicator::getInstance();

   if (instance)
   {
      (void) instance->deActivate();   // Stop thread
      (void) instance->wait(INFINITE); // Wait for thread to finish
      instance->close();               // Close thread
   }

}

 int ACS_CS_MasterReplicator::exec ()
{

   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n  Started the MasterReplicator thread\n", __FUNCTION__));

   acceptEventEthA = CreateEvent(NULL, FALSE, FALSE, NULL);

   if (acceptEventEthA == 0)
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot create accept event for EthA (Error code: %d)\n",
         __FUNCTION__, lastError));

      ACS_CS_EVENT(Event_SubSystemError,
         "Create Windows Event",
         "Cannot create a Windows event",
         "");
   }

   acceptEventEthB = CreateEvent(NULL, FALSE, FALSE, NULL);

   if (acceptEventEthB == 0)
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot create accept event for EthB (Error code: %d)\n",
         __FUNCTION__, lastError));

      ACS_CS_EVENT(Event_SubSystemError,
         "Create Windows Event",
         "Cannot create a Windows event",
         "");
   }

   // Start the listening sockets
   startListener();

   HANDLE * handleArr = 0;
   int handleArrSize = 0;
   vector<HANDLE> eventVector;                // Vector to hold events for the different listening sockets
   vector<TCPServer *> serverVector;          // Vector to hold TCPServer instances for valid sockets
   vector<TCPServer *>::iterator serverIt;

   if (socketEthA != INVALID_SOCKET)          // Check socket for EthA
   {
      eventVector.push_back(acceptEventEthA); // Add event
      serverVector.push_back(tcpServerEthA);  // Add server
   }

   if (socketEthB != INVALID_SOCKET)          // Check socket for EthB
   {
      eventVector.push_back(acceptEventEthB); // Add event
      serverVector.push_back(tcpServerEthB);  // Add server
   }

   handleArrSize = static_cast<int> (eventVector.size()) + 2; // No of handles to wait for
   handleArr = new HANDLE[handleArrSize];     // Create HANDLE array for WaitForMultipleObjects

   handleArr[0] = shutdownEvent;              // Shutdown event is always of index 0
   handleArr[1] = pduEvent;                   // Pdu event is of index 1

   for (unsigned int i = 0; i < eventVector.size(); i++)      // Loop through events
   {
      handleArr[i+2] = eventVector[i];        // Add into array the event for each available socket
   }

   //-------------------------------------------------------------------------
   // Get event when socket is ready for accept call
   DWORD returnValue = 0;

   // Associate event with the socket of EthA
   if (socketEthA != INVALID_SOCKET)
   {
      returnValue = WSAEventSelect(socketEthA, acceptEventEthA, FD_ACCEPT);
   }

   if (returnValue)	// EventSelect error
   {
      int lastError = WSAGetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot make WSAEventSelect for EthA (Error code: %d)\n",
         __FUNCTION__, lastError));
   }

   // Associate event with the socket of EthB
   if ( socketEthB != INVALID_SOCKET )
   {
      returnValue = WSAEventSelect(socketEthB, acceptEventEthB, FD_ACCEPT);
   }

   if (returnValue)	// EventSelect error
   {
      int lastError = WSAGetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot make WSAEventSelect for EthB (Error code: %d)\n",
         __FUNCTION__, lastError));
   }

   while ( ! exit_ )	// Set to true when the thread shall exit
   {
      // Wait for shutdown, pdu and accept events
      returnValue = WaitForMultipleObjects(handleArrSize, handleArr, FALSE, Connection_Timeout);

      int eventIndex = returnValue - WAIT_OBJECT_0;  // Get event index

      if ( returnValue == WAIT_FAILED )  // Event failed
      {
         DWORD lastError = GetLastError();
         ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
            "(%t) %s :\n  Error: Cannot WaitForMultipleObjects (Error code: %d)\n",
            __FUNCTION__, lastError));

         ACS_CS_EVENT(Event_SubSystemError,
            "Waiting for clients",
            "Unable to wait for event.",
            "");

         Sleep(1000);
      }
      else if ( returnValue == WAIT_TIMEOUT )  // Wait timeout
      {
         checkQueues();
         checkACKs();
         checkECHOs();
      }
      else if ( eventIndex == 0 )  // Shutdown event
      {
         ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
            "(%t) %s :\n  Received a shutdown event\n", __FUNCTION__));

         exit_ = true;  // Terminate the thread
      }
      else if ( eventIndex == 1 )  // PDU event
      {
         ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
            "(%t) %s :\n  Received a pdu event\n", __FUNCTION__));

         checkQueues();
         checkACKs();
      }
      else if ( (eventIndex > 1) && (eventIndex < handleArrSize) )  // Socket ready for accept
      {
         ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
            "(%t) %s :\n  Received an accept event\n", __FUNCTION__));

         TCPServer * currentServer = 0;
         serverIt = serverVector.begin();
         serverIt += (eventIndex - 2);

         if ( serverIt != serverVector.end() )
         {
            currentServer = *serverIt;
         }

         if ( currentServer == 0 )
         {
            continue;
         }

         Media::Status status = Media::Disconnected;

         if (currentServer)
        	status = currentServer->accept();

         if ( status == Media::Connected )  // If we are connected
         {
            // Get connected socket
            TCPClientServer::SocketInformation socketInfo = currentServer->connectedSocket();

            if ( socketInfo.m_socket == INVALID_SOCKET )  // Some error
            {
               int lastError = WSAGetLastError();
               ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
                  "(%t) %s :\n  Error: accept returns an invalid socket (Error code: %d)\n",
                  __FUNCTION__, lastError));
            }
            else  // No error
            {				   
               // Suppress warning for not freeing pointer client,
               // which we shouldn't since it is stored in an STL vector.
               // Vector and content freed in destructor
               // lint --e{429}

               unsigned long localAddress = currentServer->listenIPAddress();

               // Get client address
               unsigned long clientAddress = 0;  // Client address as long
               struct sockaddr_in peer;
               int addr_len = sizeof(peer);
               int error = getpeername(socketInfo.m_socket, (sockaddr *) &peer, &addr_len);  // Get client address from socket

               if ( error == 0 )  // If got address, create log text
               {
                  unsigned long * tempAddr = reinterpret_cast<unsigned long *>( &(peer.sin_addr) );
                  clientAddress = ntohl(*tempAddr);
               }

               if ( (clientAddress) && (clientAddress != localAddress) )  // If client address is valid
               {
                  // -------------------------------------------------
                  // Log entry

                  ostringstream message;
                  message << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port);

                  ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
                     "(%t) %s :\n  Connected by %s\n",
                     __FUNCTION__, message.str().c_str()));
                  
                  // End of log entry
                  // -------------------------------------------------

                  bool clientExists = false;
                  if (clientMap)  // Search map to see if a client with this IP already exists
                  {
                     clientMapType::iterator it = clientMap->find(clientAddress);

                     if ( it != clientMap->end() )  // If address already exists
                     {
                        clientExists = true;
                     }
                  }

                  if (clientExists)
                  {
                     deleteClient(clientAddress);
                  }

                  // Create a new TCP client to handle the socket
                  TCPClient * client = new TCPClient();

                  if (client)
                  {
                     client->useThisSocket(socketInfo);

                     // Store the client in map
                     if (clientMap)
                     {
                        clientMap->insert( clientMapType::value_type(clientAddress, client) );
                     }
                     else
                     {
                        delete client;
                     }
                  }

                  // Create queue for the client
                  createQueue(clientAddress);

                  // Test if the client is a local slave (slave on other node)
                  // This slave will send ACKS
                  if ( isLocalSlave(clientAddress) )
                  {
                     ackClientIP = clientAddress;
                     if (ackParser)
                     {
                        ackParser->flush();
                     }
                  }

                  ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
                     "(%t) %s :\n  A client is connected, so checkQueues for it\n", __FUNCTION__));

                  checkQueues();               
               }
               else  // clientAddress is invalid or localAddress
               {
                  (void) closesocket(socketInfo.m_socket);
               }

            } // End of if ( socketInfo.m_socket == INVALID_SOCKET )

         } // End of if ( status == Media::Connected )

      } // End of else if ( (eventIndex > 1) && (eventIndex < handleArrSize) )

   } // End of while ( ! exit_ )
	
   emptyQueues();

   delete [] handleArr;

   if (tcpServerEthA)
   {
      tcpServerEthA->disconnect();
      delete tcpServerEthA;
      tcpServerEthA = 0;
   }

   if (tcpServerEthB)
   { 
      tcpServerEthB->disconnect();
      delete tcpServerEthB;
      tcpServerEthB = 0;
   }

   if (acceptEventEthA)
   {
      (void) CloseHandle(acceptEventEthA);
      acceptEventEthA = 0;
   }

   if (acceptEventEthB)
   {
      (void) CloseHandle(acceptEventEthB);
      acceptEventEthB = 0;
   }

   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n  Stopped the MasterReplicator thread\n", __FUNCTION__));

   return 0;

}

 int ACS_CS_MasterReplicator::sendRequest (ACS_CS_PDU *pdu)
{

   if ( pdu == 0 )
   {
      return -1;
   }
   else if ( pdu->getLength() == 0 )
   {
      return -1;
   }

   // Suppress warning for not freeing newPdu,
   // which we shouldn't since it is stored in an STL queue.
   // Queue and content freed in destructor
   // lint --e{429}

   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n  Copied a pdu into commonQueue\n", __FUNCTION__));

   lock->start_writing();

   ACS_CS_PDU * newPdu = new ACS_CS_PDU(*pdu);
   this->commonQueue.push(newPdu);  

   if ( SetEvent(pduEvent) == 0 )
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot signal a pdu event (Error code: %d)\n",
         __FUNCTION__, lastError));

      ACS_CS_EVENT(Event_SubSystemError,
         "Create Windows Event",
         "Cannot signal a Windows event",
         "");
   }

   lock->stop_writing();

   return 0;

}

 void ACS_CS_MasterReplicator::updateTable (ACS_CS_Protocol::CS_Scope_Identifier scope, const std::vector<ACS_CS_PDU *> &tableVector)
{

   // Ignoring warning for not freeing Pointer. It is stored and freed elsewhere
   // lint --e{429}

   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n  Got a copy of table for scope (%d)\n",
      __FUNCTION__, scope));

   lock->start_writing();

   vector<ACS_CS_PDU *> * scopeVector = 0;

   if ( scope != ACS_CS_Protocol::Scope_NotSpecified )
   {
      if (tableMap)
      {
         tableMapType::iterator mapIt = tableMap->find(scope); // Search for old entry

         if ( mapIt != tableMap->end() )     // If the table vector already exists,
         {
            scopeVector = mapIt->second;     // get the pointer to vector
            vector<ACS_CS_PDU *>::iterator vecIt;

            for ( vecIt = scopeVector->begin(); vecIt != scopeVector->end(); vecIt++ )
            {
               ACS_CS_PDU * pdu = *vecIt;
               delete pdu;                   // deleted all pdu from the vector
            }

            (void) tableMap->erase(mapIt);   // remove the vector from map
            delete scopeVector;
         }

         scopeVector = new vector<ACS_CS_PDU *>();

         // Copy the table into a new vector
         if (scopeVector)
         {
            vector<ACS_CS_PDU *>::const_iterator vecIt;

            for ( vecIt = tableVector.begin(); vecIt != tableVector.end(); vecIt++ )
            {
               ACS_CS_PDU * pdu = *vecIt;

               if (pdu)
               {
                  scopeVector->push_back(pdu);
               }
            }

            // Copy the new table vector into map
            tableMap->insert( tableMapType::value_type(scope, scopeVector) );
         }
      } // if (tableMap)
   } // if (scope != ACS_CS_Protocol::Scope_NotSpecified)

   if ( SetEvent(pduEvent) == 0 )
   {
      DWORD lastError = GetLastError();
      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot signal a pdu event (Error code: %d)\n",
         __FUNCTION__, lastError));

      ACS_CS_EVENT(Event_SubSystemError,
         "Create Windows Event",
         "Cannot signal a Windows event",
         "");
   }

   lock->stop_writing();

}

 void ACS_CS_MasterReplicator::startListener ()
{

   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n  Starting to listen on EthA and/or EthB\n", __FUNCTION__));

   unsigned short port = CS_Replication_Port;

   if ( socketEthA != INVALID_SOCKET )
   {
      (void) WSAEventSelect(socketEthA, 0, 0);
      socketEthA = INVALID_SOCKET;
   }

   if ( socketEthB != INVALID_SOCKET )
   {
      (void) WSAEventSelect(socketEthB, 0, 0);
      socketEthB = INVALID_SOCKET;
   }

   if(tcpServerEthA)
   {
      delete tcpServerEthA;
      tcpServerEthA = 0;
   }

   if(tcpServerEthB)
   {
      delete tcpServerEthB;
      tcpServerEthB = 0;
   }

   // Create the servers for two backplane IP addresses
   tcpServerEthA = new TCPServer(TCPClientServer::NonBlocking, 0);
   tcpServerEthB = new TCPServer(TCPClientServer::NonBlocking, 0);

   unsigned long ipA1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_A_HOSTNAME);
   unsigned long ipB1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_A_HOSTNAME);
   unsigned long ipA2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_B_HOSTNAME);
   unsigned long ipB2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_B_HOSTNAME);

   // Start to listen on EthA
   TCPServer::Status status = tcpServerEthA->listenOn(port, ipA1);  // Try node A address

   if ( status == TCPServer::BindError )  // Node A address unavailable (probably running on Node B)
   {
      delete tcpServerEthA;
      tcpServerEthA = new TCPServer(TCPClientServer::NonBlocking);

      status = tcpServerEthA->listenOn(port, ipB1);  // Try Node B address
   }

   if ( status == TCPServer::OK )  // EthA OK
   {
      socketEthA = tcpServerEthA->listeningSocket();
      ethAAvailable = true;
   }
   else  // EthA unavailable
   {
      socketEthA = INVALID_SOCKET;
      ethAAvailable = false;

      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot open a socket on EthA\n", __FUNCTION__));
   }

   // Start to listen on EthB
   status = tcpServerEthB->listenOn(port, ipA2);  // Try node A address

   if ( status == TCPServer::BindError )  // Node A address unavailable (probably running on Node B)
   {
      delete tcpServerEthB;
      tcpServerEthB = new TCPServer(TCPClientServer::NonBlocking);

      status = tcpServerEthB->listenOn(port, ipB2);  // Try Node B address
   }

   if (status == TCPServer::OK)  // EthB OK
   {
      socketEthB = tcpServerEthB->listeningSocket();
      ethBAvailable = true;
   }
   else	// EthB unavilable
   {
      socketEthB = INVALID_SOCKET;
      ethBAvailable = false;

      ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
         "(%t) %s :\n  Error: Cannot open a socket on EthB\n", __FUNCTION__));
   }


   // Check if at least one network is accessible
   if ( ( (ethAAvailable || ethBAvailable) ) == false )
   {
      char message[100];
      sprintf(message, "Server cannot open port %d for listening on any network", port);

      ACS_CS_EVENT(Event_SocketAPIFailure,
         "Open TCP port",
         message,
         "");
   }

}

 bool ACS_CS_MasterReplicator::send (unsigned long ipAddress, std::queue<ACS_CS_PDU *> &outQueue) const
{

   bool sendSucceeded = true;
   bool clientDisconnected = false;

   TCPClient * client = 0;
   clientMapType::const_iterator it;

   if (clientMap)
   {
      it = clientMap->find(ipAddress);

      if ( it != clientMap->end() )
      {
         client = it->second;
      }
   }

   while ( ! outQueue.empty() )
   {
      ACS_CS_PDU * pdu = outQueue.front();
      outQueue.pop();

      if ( pdu == 0 )  // Invalid pdu
      {
         continue;     // Keep on processing the next pdu
      }

      if (client)
      {
         if ( ! clientDisconnected )
         {
            int size = pdu->getLength();	// Get pdu length
            char * buffer = 0;

            if ( size > 0 )
            {
               buffer = new char[size];   // Create buffer
            }

            if (buffer)
            {
               u_int32 sizeSent = 0;
               (void) pdu->getBuffer(buffer, size);
               TCPClientServer::Status status;

               ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
                  "(%t) %s :\n  Sending a pdu to client\n", __FUNCTION__));

               // Send pdu to client
               status = client->send(buffer, size, sizeSent);

               u_int32 offset = 0;
               while ( status == TCPClient::NotOK )      // Not all data was sent
               {                                    
                  offset += sizeSent;

                  // Send remaining data
                  status = client->send( (buffer + offset), (size - offset), sizeSent );
               }

               if ( status == TCPClient::Disconnected )  // Client disconnected
               {
                  ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
                     "(%t) %s :\n  Error: Client disconnected\n", __FUNCTION__));

                  char data[512] = {0};
                  _snprintf(data, sizeof(data) - 1, "Slave disconnected. Slave IP address: %s - Last Error: %d", 
                     client->IPAddressToString(ipAddress), GetLastError());

                  ACS_CS_EVENT(Event_SocketAPIFailure, "AP INTERNAL FAULT", data, "");
                  ACS_CS_EventReporter::instance()->resetAllEvents();

                  sendSucceeded = false;
                  clientDisconnected = true;
               }

               delete [] buffer;

            } // End of if (buffer)

         } // End of if ( ! clientDisconnected )

      } // End of if (client)

      delete pdu;

   } // End of while ( ! outQueue.empty() )

   return sendSucceeded;

}

 void ACS_CS_MasterReplicator::createQueue (unsigned long ipAddress)
{

   // Ignoring warning for not freeing queue. It is stored in a map and freed
   // elsewhere
   // lint --e{429}
  
   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n Starting to copy tables into queueMap for a client", __FUNCTION__));      

   if (queueMap)
   {
      queueMapType::iterator mapIt = queueMap->find(ipAddress); // Search for old queue

      if ( mapIt != queueMap->end() )    // if entry exists
      {
         queue<ACS_CS_PDU *> * tempQueue = mapIt->second;

         while ( ! tempQueue->empty() )
         {
            ACS_CS_PDU * pdu = tempQueue->front();

            delete pdu;

            tempQueue->pop();
         }

         delete tempQueue;
         (void) queueMap->erase(mapIt);  // remove from map
      }

      queue<ACS_CS_PDU *> * tempQueue = new queue<ACS_CS_PDU *>();

      if (tempQueue)
      {
         lock->start_reading();

         // Copy all scopes from tableMap into tempQueue, starting with the HWC table
         copyToQueue(ACS_CS_Protocol::Scope_HWCTable, *tempQueue);
         copyToQueue(ACS_CS_Protocol::Scope_CPIdTable, *tempQueue);
         copyToQueue(ACS_CS_Protocol::Scope_CPGroupTable, *tempQueue);
         copyToQueue(ACS_CS_Protocol::Scope_FDTable, *tempQueue);
         copyToQueue(ACS_CS_Protocol::Scope_NE, *tempQueue);

         lock->stop_reading();

         // Create a queue in queueMap for the client
         queueMap->insert( queueMapType::value_type(ipAddress, tempQueue) );
      }
   }  

}

 void ACS_CS_MasterReplicator::copyToQueue (ACS_CS_Protocol::CS_Scope_Identifier scope, std::queue<ACS_CS_PDU *> &targetQueue) const
{

   // Ignoring warning for not freeing pdu. It is stored in a vector and freed
   // elsewhere
   // lint --e{429}

   if (tableMap) // Reading lock has been obtained outside
   {
      tableMapType::iterator mapIt = tableMap->find(scope); // Search for table

      if ( mapIt != tableMap->end() )
      {
         vector<ACS_CS_PDU *> * tableVector = mapIt->second;

         vector<ACS_CS_PDU *>::iterator vecIt;

         for (vecIt = tableVector->begin(); vecIt != tableVector->end(); vecIt++)
         {
            ACS_CS_PDU * pdu = new ACS_CS_PDU( * (*vecIt) );
            targetQueue.push(pdu);
         }
      }
   }

}

 void ACS_CS_MasterReplicator::checkQueues ()
{

   // Ignoring warning for not freeing pdu. It is stored in a queue and freed
   // elsewhere
   // lint --e{429}

   lock->start_writing();

   // Copy PDUs from commonQueue to each individual client's queue
   while ( ! commonQueue.empty() )
   {
      ACS_CS_PDU * pdu = commonQueue.front();
      commonQueue.pop();

      if (pdu)
      {
         if (queueMap)
         {
            queueMapType::iterator queueMapIt;

            for (queueMapIt = queueMap->begin(); queueMapIt != queueMap->end(); queueMapIt++)
            {
               std::queue<ACS_CS_PDU *> * tempQueue = queueMapIt->second;

               if (tempQueue)
               {
                  ACS_CS_PDU * newPdu = new ACS_CS_PDU(*pdu);
                  tempQueue->push(newPdu);
               }
            }
         }

         delete pdu;
      }
   }

   lock->stop_writing();

   queue<ACS_CS_PDU *> * queuePtr;

   queueMapType::iterator queueMapIt;
   vector<unsigned long> failedClients;

   if (queueMap)
   {
      for (queueMapIt = queueMap->begin(); queueMapIt != queueMap->end(); queueMapIt++)
      {
         unsigned long ipAddress = queueMapIt->first;
         queuePtr = queueMapIt->second;
         queue<ACS_CS_PDU *> tempQueue;

         if (queuePtr)
         {
            tempQueue = *queuePtr;

            while ( ! queuePtr->empty() )
            {
               queuePtr->pop();
            }

            if ( send(ipAddress, tempQueue) == false)
            {
               failedClients.push_back(ipAddress);

               ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
                  "(%t) %s :\n  Error: One client failed\n", __FUNCTION__));
            }
         }
      }
   }

   vector<unsigned long>::iterator vecIt;

   // Remove failed clients from queueMap and clientMap
   for (vecIt = failedClients.begin(); vecIt != failedClients.end(); vecIt++)
   {
      unsigned long ipAddress = *vecIt;
      this->deleteClient(ipAddress);
   }

}

 void ACS_CS_MasterReplicator::deleteClient (unsigned long ipAddress)
{

   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n  Removing a failed client\n", __FUNCTION__));

   // Remove from queueMap (no lock needed)
   if (queueMap)
   {
      queueMapType::iterator mapIt = queueMap->find(ipAddress); // Search for old queue

      if ( mapIt != queueMap->end() )        // If entry exists,
      {
         queue<ACS_CS_PDU *> * tempQueue = mapIt->second;
         (void) queueMap->erase(mapIt);      // remove from map.

         while ( ! tempQueue->empty() )
         {
            ACS_CS_PDU * pdu = tempQueue->front();
            tempQueue->pop();

            delete pdu;
         }

         delete tempQueue;
      }
   }

   // Remove from clientMap (no lock needed)
   if (clientMap)
   {
      clientMapType::iterator clientIt;

      clientIt = clientMap->find(ipAddress);

      // Loop through vector with connected clients
      if (clientIt != clientMap->end() )     // If entry exists,
      {
         TCPClient * client = clientIt->second;
         (void) clientMap->erase(clientIt);  // remove from map.

         if (client)
         {
            client->disconnect();   // Disconnect each client
            delete client;          // Delete the client
         }
      }
   }

}

 void ACS_CS_MasterReplicator::emptyQueues ()
{

   ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
      "(%t) %s :\n  Starting to clear all maps and queue\n", __FUNCTION__));

   lock->start_writing();

   if (clientMap)
   {
      clientMapType::iterator clientIt = clientMap->begin();

      // Loop through vector with connected clients
      while ( clientIt != clientMap->end() )
      {
         TCPClient * client = clientIt->second;
         clientIt = clientMap->erase(clientIt);

         if (client)
         {
            client->disconnect();   // Disconnect each client
            delete client;          // Delete the client
         }
      }
   }

   if (tableMap)
   {
      tableMapType::iterator mapIt = tableMap->begin();

      // Loop through map
      while ( mapIt != tableMap->end() )
      {
         vector<ACS_CS_PDU *> * scopeVector = mapIt->second;
         mapIt = tableMap->erase(mapIt);

         if (scopeVector)
         {
            vector<ACS_CS_PDU *>::iterator vecIt;

            for (vecIt = scopeVector->begin(); vecIt != scopeVector->end(); vecIt++)
            {
               ACS_CS_PDU * pdu = *vecIt;
               delete pdu;
            }

            delete scopeVector;
         }
      }
   }

   if (queueMap)
   {
      queueMapType::iterator mapIt = queueMap->begin();

      // Loop through map
      while ( mapIt != queueMap->end() )
      {
         queue<ACS_CS_PDU *> * tempQueue = mapIt->second;
         mapIt = queueMap->erase(mapIt);

         if (tempQueue)
         {
            while ( ! tempQueue->empty() )
            {
               ACS_CS_PDU * pdu = tempQueue->front();
               tempQueue->pop();

               delete pdu;
            }

            delete tempQueue;
         }
      }
   }

   while ( ! commonQueue.empty() )
   {
      ACS_CS_PDU * pdu = commonQueue.front();
      commonQueue.pop();

      delete pdu;
   }

   lock->stop_writing();

}

 int ACS_CS_MasterReplicator::getQueueSize () const
{

   int size = 0;

   lock->start_reading();

   size += static_cast<int> (commonQueue.size() );

   lock->stop_reading();

   if (queueMap) // No lock needed
   {
      queueMapType::iterator queueMapIt;

      for (queueMapIt = queueMap->begin(); queueMapIt != queueMap->end(); queueMapIt++)
      {
         std::queue<ACS_CS_PDU *> * tempQueue = queueMapIt->second;

         if (tempQueue)
            size += static_cast<int> ( tempQueue->size() );
      }
   }

   return size;

}

 void ACS_CS_MasterReplicator::checkACKs ()
{

   if ( (clientMap == 0) || (ackParser == 0) )
   {
      return;
   }

   char buffer[BUFSIZ];

   // Search map to see if a client with this IP already exists
   clientMapType::iterator it = clientMap->find(ackClientIP);

   if ( it != clientMap->end() )       // if address already exists
   {
      TCPClient * client = it->second;
      TCPClientServer::Status status;

      if (client)
      {
         u_int32 bytesRead = 0;
         bool neverReadData = true;

         do
         {
            status = client->poll(buffer, BUFSIZ, bytesRead); // Try to read TCP data

            if ( bytesRead > 0 )
            {
               (void) ackParser->newData(buffer, bytesRead);  // Copy data to parser
               neverReadData = false;  // Retrieved some data
            }
         }
         while ( bytesRead > 0 );      // Repeatly read until all data has been retrieved

         // No ACk received or client discconected
         if ( neverReadData || ( status == TCPClient::Disconnected) )
         {
            return;
         }

         if ( status == TCPClient::OK )  // Data has been read and status is still OK
         {	
            try
            {
               (void) ackParser->parse();
            }
            catch (ACS_CS_Exception ex)
            {
               // Ignore the exceptions which are triggered by ECHO messages
               // sent by SlaveReplicator every 8 Connection_Timeout (8 x 2.5 = 20 sec).
            }
         }

         ACS_CS_PDU pdu;

         while ( ackParser->getPDU(pdu) )
         {
            const ACS_CS_Header * header = pdu.getHeader();

            if (header)
            {
               const ACS_CS_Primitive * primitive = pdu.getPrimitive();
               const ACS_CS_ACKTable * ackTable = 0;

               if ( (header != 0) && (primitive != 0) )
               {
                  if (primitive->getPrimitiveType() == Primitive_ACKTable)
                  {
                     ackTable = reinterpret_cast<const ACS_CS_ACKTable *> (primitive);

                     if (ackTable)
                     {
                        string tableName = ackTable->getTableName();
                        unsigned long timeStamp = ackTable->getTimeStamp();

                        ACS_CS_TableSignature * instance = ACS_CS_TableSignature::getInstance();

                        if (instance)
                        {
                           instance->sign(timeStamp, tableName, ServiceType_SLAVE);

                           ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
                              "(%t) %s :\n  Singed a %s\n",
                              __FUNCTION__, tableName.c_str()));  
                        }
                     }
                  }
               }
            } // End of if (header)
         }
      }  // End of if (client)
   }
   
}

 bool ACS_CS_MasterReplicator::isLocalSlave (unsigned long ipAddress) const
{

   unsigned long ipA1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_A_HOSTNAME);
   unsigned long ipB1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_A_HOSTNAME);
   unsigned long ipA2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_B_HOSTNAME);
   unsigned long ipB2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_B_HOSTNAME);

   if ( (ipAddress == ipA1) || (ipAddress == ipB1) ||
        (ipAddress == ipA2) || (ipAddress == ipB2) )
   {
      return true;
   }
   else
   {
      return false;
   }

}

 void ACS_CS_MasterReplicator::checkECHOs () const
{
	
   if ( clientMap == 0 ) return;
   if ( clientMap->empty() ) return;

   char buffer[BUFSIZ] = {0};
   char echoBuffer[BUFSIZ] = {1};

   for (clientMapType::iterator it = clientMap->begin(); it != clientMap->end(); it++)
   {
      TCPClient * client = it->second;
      unsigned long ipAddress = it->first;
      TCPClientServer::Status status;

      if (client)
      {
         u_int32 bytesRead = 0;

         status = client->poll(buffer, BUFSIZ, bytesRead); // Try to read TCP data

         if ( (status == TCPClient::OK) && (bytesRead == BUFSIZ) && (memcmp(buffer, echoBuffer, bytesRead) == 0) )
         {
            in_addr InAddr;
            InAddr.S_un.S_addr = ntohl(ipAddress);

            ACS_CS_TRACE((ACS_CS_MasterReplicator_TRACE,
               "(%t) %s :\n  Received echo message from %s\n",
               __FUNCTION__, inet_ntoa(InAddr)));
         }
      }
   }

}

// Additional Declarations

