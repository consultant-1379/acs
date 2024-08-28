

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "TCPServer.hxx"
#include "TCPClient.hxx"
#include "TCPClientServer.hxx"

#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_PDU.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Header.h"
#include "ACS_CS_Primitive.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_HostFile.h"


// ACS_CS_SlaveReplicator
#include "ACS_CS_SlaveReplicator.h"


ACS_CS_Trace_TDEF(ACS_CS_SlaveReplicator_TRACE);

using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using std::map;
using std::queue;


#define ECHOBUFSIZ 12
// Class ACS_CS_SlaveReplicator 

ACS_CS_SlaveReplicator* ACS_CS_SlaveReplicator::replicatorInstance = 0;

ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()
      : lastWorkingIP(0),
        tableMap(0),
        csParser(0),
        syncInProgress(false),
        syncEventHandle(0),
        hwcReplicationEvent(0),
        cpIdReplicationEvent(0),
        cpGroupReplicationEvent(0),
        fdReplicationEvent(0),
        neReplicationEvent(0),
        vlanReplicationEvent(0),
        lock(0),
        side(ACS_CS_Protocol::Side_NotSpecified)
{

   // Initialize critical section
	InitializeCriticalSection(&ipCriticalSection);
   InitializeCriticalSection(&syncCriticalSection);

   // Object to parse the TCP stream
	csParser = new ACS_CS_Parser();

   // Create table map
   tableMap = new tableMapType();

   lock = new ACS_CS_ReaderWriterLock();

   isAP1 = CS_ProtocolChecker::checkIfAP1();
   side = CS_ProtocolChecker::getNode();

   // Create event to be used to wait for replication
   hwcReplicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_REPLICATION_HWC);

	if (hwcReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   cpIdReplicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_REPLICATION_CPID);

	if (cpIdReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   cpGroupReplicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_REPLICATION_CPGROUP);

	if (cpGroupReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   fdReplicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_REPLICATION_FD);

	if (fdReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   neReplicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_REPLICATION_NE);

	if (neReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   vlanReplicationEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_REPLICATION_VLAN);

	if (vlanReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

    // Create event to be used to wait for replication
   syncEventHandle = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_REPLICATION_SYNC);

	if (syncEventHandle == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::ACS_CS_SlaveReplicator()\n"
			"Error: Cannot create sync event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}


}


ACS_CS_SlaveReplicator::~ACS_CS_SlaveReplicator()
{

   if (lock)
      delete lock;

   if (csParser)
		delete csParser;

   if (tableMap)
   {
      tableMapType::iterator mapIt = tableMap->begin();

      // Loop through map
      while ( mapIt != tableMap->end() )
      {
         queue<ACS_CS_PDU *> * tempQueue = mapIt->second;
         mapIt = tableMap->erase(mapIt);

         if (tempQueue)
         {
            // Loop through each queue
            while ( ! tempQueue->empty() )
            {
               ACS_CS_PDU * pdu = tempQueue->front();
               tempQueue->pop();

               if (pdu)
                  delete pdu;
            }

            delete tempQueue;
         }
      }

	   delete tableMap;			// Delete the map
	}

   if (hwcReplicationEvent)
      (void) CloseHandle(hwcReplicationEvent);

   if (cpIdReplicationEvent)
      (void) CloseHandle(cpIdReplicationEvent);

   if (cpGroupReplicationEvent)
      (void) CloseHandle(cpGroupReplicationEvent);

   if (fdReplicationEvent)
      (void) CloseHandle(fdReplicationEvent);

   if (neReplicationEvent)
      (void) CloseHandle(neReplicationEvent);

   if (vlanReplicationEvent)
      (void) CloseHandle(vlanReplicationEvent);

   if (syncEventHandle)
      (void) CloseHandle(syncEventHandle);

   DeleteCriticalSection(&ipCriticalSection);

   DeleteCriticalSection(&syncCriticalSection);

}



 ACS_CS_SlaveReplicator * ACS_CS_SlaveReplicator::getInstance ()
{

   if (ACS_CS_SlaveReplicator::replicatorInstance == 0)
   {
      ACS_CS_SlaveReplicator::replicatorInstance = new ACS_CS_SlaveReplicator();
   }

   return ACS_CS_SlaveReplicator::replicatorInstance;

}

 int ACS_CS_SlaveReplicator::exec ()
{
	
	ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
		"(%t) ACS_CS_SlaveReplicator::exec()\n"
		"Entering function\n"));
	
	// This event is signaled when the service should shutdown
	HANDLE shutdownEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_SHUTDOWN);
	
	if (shutdownEvent == 0) {
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::exec()\n"
			"Error: Cannot open shutdown event, GetLastError() = %d\n",
			GetLastError()));
		
		ACS_CS_EVENT(Event_SubSystemError,
			"Open Windows Event",
			"Cannot open Windows event",
			"");
	}
	
	// This event is signaled when the service should shutdown
	HANDLE connectionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (connectionEvent == 0) {
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::exec()\n"
			"Error: Cannot create connection event, GetLastError() = %d\n",
			GetLastError()));
		
		ACS_CS_EVENT(Event_SubSystemError,
			"Open Windows Event",
			"Cannot create Windows event",
			"");
	}
	
	bool connected = false;
	TCPClient * tcpClient = 0;
	//int resetConnectionCounter = 0; //used to establish a new connection towards Master CS every 30 minutes 
	int refreshConnectionCounter = 0;
	
	while (!exit_) { // Set to true when the thread shall exit
		if ( ! connected ) {
			ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				"(%t) ACS_CS_SlaveReplicator::exec()\n"
				"Trying to connect to master\n"));
			
			// Try to connect to the master
			while ( ! startSession(&tcpClient, CS_Replication_Port) ) {
				ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
					"(%t) ACS_CS_SlaveReplicator::exec()\n"
					"Failed to connect\n")); 
				
				if (exit_)
					break;

				refreshConnectionCounter = 0;
				//resetConnectionCounter = 0;
				Sleep(100);
			}
		}
		
		if (tcpClient == 0)
			continue;
		
		if (tcpClient->isConnected()) {
			if ( ! connected ) {
				// Get client address
				unsigned long serverIP = lastWorkingIP;
				string serverAddress = tcpClient->IPAddressToString(serverIP);
				
				ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
					"(%t) ACS_CS_SlaveReplicator::exec()\n"
					"Connected to master on %s:%d\n",
					serverAddress.c_str(),
					CS_Replication_Port));
				
				connected = true;
			}
			refreshConnectionCounter++;
			//resetConnectionCounter++;

			//establish a new connection towards Master CS every 30 minutes 
			//if ((resetConnectionCounter % (24 * 30)) == 0) {
			//	resetConnectionCounter = 0;
			//	connected = false;
			//	tcpClient->disconnect();
			//	delete tcpClient;
			//	tcpClient = 0;
			//	continue;
			//}

			if ((refreshConnectionCounter % 8) == 0) { // send echo message
				refreshConnectionCounter = 0;
				if (!isConnectionAlive(tcpClient)) {
					connected = false;
					tcpClient->disconnect();
					delete tcpClient;
					tcpClient = 0;
					continue;
				}
			}
		} else {
			ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				"(%t) ACS_CS_SlaveReplicator::exec()\n"
				"Connection lost\n"));

			refreshConnectionCounter = 0;
			connected = false;
			tcpClient->disconnect();
			delete tcpClient;
			tcpClient = 0;
			continue;
		}
		
		DWORD returnValue = WSAEventSelect(tcpClient->socket(), connectionEvent, FD_READ | FD_CLOSE);
		
		if (returnValue) { // EventSelect error
			ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				"(%t) ACS_CS_SlaveReplicator::exec()\n"
				"Error: WSAEventSelect, WSAGetLastError() = %d\n",
				WSAGetLastError()));
			
			ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				"(%t) ACS_CS_SlaveReplicator::exec()\n"
				"Connection lost\n"));
			connected = false;
			tcpClient->disconnect();
			continue;
		}
		
		// Array to use with WaitForMultipleObjects
		HANDLE handleArr[2] = {shutdownEvent, connectionEvent};

		// Wait for service shutdown and connection events
		returnValue = WaitForMultipleObjects(2, handleArr, FALSE, Connection_Timeout);
		
		int eventIndex = returnValue - WAIT_OBJECT_0;	// Get index for event
		if (returnValue == WAIT_FAILED) { // Event failed
			ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				"(%t) ACS_CS_SlaveReplicator::exec()\n"
				"Error: WaitForMultipleObjects, GetLastError() = %d\n",
				GetLastError()));
			
			tcpClient->disconnect();
			connected = false;
			Sleep(1000);
		} else if (returnValue == WAIT_TIMEOUT) { // Connection timeout
			ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				"(%t) ACS_CS_SlaveReplicator::exec()\n"
				"Connection timeout.\n"));
			
			ACS_CS_PDU * pdu = 0;
			TCPClientServer::Status status;

			while (pdu = this->getFromOutQueue() ) {
				int bytesToSend = pdu->getLength();
				char * outBuffer = new char[bytesToSend];
				
				if (outBuffer) {
					(void) pdu->getBuffer(outBuffer, bytesToSend);
					unsigned int sizeSent = 0;
					unsigned int offset = 0;
					status = tcpClient->send(outBuffer, bytesToSend, sizeSent);
					
					// If all data wasn't sent
					while (status == TCPClient::NotOK) {
						offset += sizeSent;
						sizeSent = 0;
						
						// Send remaining data
						status = tcpClient->send( (outBuffer + offset), (bytesToSend - offset), sizeSent );
					}
					
					delete [] outBuffer;
				}
				
				delete pdu;
			}
		}
		else if ( eventIndex == 0) { // Shutdown event
			ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				"(%t) ACS_CS_SlaveReplicator::exec()\n"
				"Shutdown event signaled. Disconnecting.\n"));
			
			exit_ = true;
		} else if ( eventIndex == 1) { // Socket ready for read or write
			//------------------------------------
			char buffer[BUFSIZ];
			unsigned int bytesRead = 0;
			int error = 0;
			TCPClientServer::Status status;
			
			do {
				status = tcpClient->poll(buffer, BUFSIZ, bytesRead); // Try to read TCP data
				if (bytesRead > 0) {
					//------------------------------------------
					// Debug printout
					ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::exec()\n"
						"Data received, bytes read = %d, data = 0x%s\n",
						bytesRead,
						CS_ProtocolChecker::binToString(buffer, bytesRead).c_str()	));
					//------------------------------------------
					
					error = csParser->newData(buffer, bytesRead); // Copy data to parser
					
					if (error) {
						ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
							"(%t) ACS_CS_SlaveReplicator::exec()\n"
							"Error: Cannot copy data to parser\n"));
					}
				}
			} while (bytesRead > 0); // Loop as long as data is returned by the TCP connection
			
			if (status == TCPClient::OK) {	// Data has been read and status is still OK
											// We can go on and parse the data
				try {
					error = csParser->parse(); // Parse data
				} catch (ACS_CS_Exception ex) {
					ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::exec()\n"
						"Error: Exception thrown by parser\n"));
					
					exit_ = true;
					break;
				}
				
				if (error < 0) { // Unfixable error encountered, disconnect without response
					ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::exec()\n"
						"Error: Internal error in parser. Disconnecting.\n"));
					
					ACS_CS_EVENT(Event_ProgrammingError,
						"Error Parsing Data",
						"Cannot parse incoming data",
						"");
					
					exit_ = true;
					csParser->flush();
					break;
				}
				
				ACS_CS_PDU pdu;
				while ( csParser->getPDU(pdu) )	{ // Get all available full PDUs from parser
					addToqueue(pdu);
				}
			}
			//------------------------------------
		}
	} //while
	
	ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
		"(%t) ACS_CS_SlaveReplicator::exec()\n"
		"Calling lock->start_writing()\n"));
	
	lock->start_writing();
	if (tableMap) {
		tableMapType::iterator mapIt = tableMap->begin();
		
		while ( mapIt != tableMap->end() ) { // Loop through map
			queue<ACS_CS_PDU *> * tempQueue = mapIt->second;
			mapIt = tableMap->erase(mapIt);
			
			if (tempQueue) { // Loop through each queue
				while ( ! tempQueue->empty() ) {
					ACS_CS_PDU * pdu = tempQueue->front();
					tempQueue->pop();
					
					if (pdu) delete pdu;
				}
				delete tempQueue;
			}
		}
	}
	
	ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
		"(%t) ACS_CS_SlaveReplicator::exec()\n"
		"Calling lock->stop_writing()\n"));
	
	lock->stop_writing();
	
	if (tcpClient) {
		tcpClient->disconnect();
		delete tcpClient;
		tcpClient = 0;
	}
	
	if (shutdownEvent)
		(void) CloseHandle(shutdownEvent);
	
	if (connectionEvent)
		(void) CloseHandle(connectionEvent);
	
	return 0;

}

 int ACS_CS_SlaveReplicator::sendRequest (ACS_CS_PDU *pdu)
{

   if (pdu == 0)
      return -1;
   else if (pdu->getLength() == 0)
      return -1;

   int error = -1;

   // This event is signaled when the service should shutdown
	HANDLE shutdownEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME_SHUTDOWN);
	
	if (shutdownEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
			"Error: Cannot open shutdown event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
			  "Open Windows Event",
			  "Cannot open Windows event",
			  "");
	}

   // Create connection event used to wait for the socket
	HANDLE connectionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (connectionEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
			"Error: Cannot create connection event\n"));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Unable to create windows event",
					"");
	}

   // Array to use with WaitForMultipleObjects
   HANDLE handleArr[2] = {shutdownEvent, connectionEvent};

   // Create client
   TCPClient * client = 0;
   TCPClientServer::Status status = TCPClient::OK;

   // Create parser
   ACS_CS_Parser parser;
   
   // Connect to server
   if (startSession(&client, CS_Server_Port))
   {
      int size = pdu->getLength();
      char * outBuffer = new char[size];

      if (outBuffer)
      {
         (void) pdu->getBuffer(outBuffer, size);
         u_int32 sizeSent = 0;
         u_int32 offset = 0;

         // Set event to be signaled when we can write or close
         (void) WSAEventSelect(client->socket(), connectionEvent, FD_WRITE | FD_CLOSE);

         // Wait for socket
         DWORD returnValue = WaitForMultipleObjects(2, handleArr, FALSE, Connection_Timeout);
         
         int eventIndex = returnValue - WAIT_OBJECT_0;	// Get index for event

         if (returnValue == WAIT_FAILED)		// Event failed
		   {
			   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
				   "Error: WaitForMultipleObjects, GetLastError() = %d\n",
				   GetLastError()));

			   error = -1;
		   }
		   else if (returnValue == WAIT_TIMEOUT)	// Connection timeout
		   {
			   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
				   "Error: Connection timeout. Disconnecting.\n"));

			   error = -1;
		   }
		   else if ( eventIndex == 0)			// Shutdown event
		   {
			   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
				   "Shutdown event signaled. Disconnecting client.\n"));

			   error = -1;
		   }
		   else if ( eventIndex == 1)			// Socket ready for write
         {

            status = client->send(outBuffer, size, sizeSent);

            // If all data wasn't sent
            while (status == TCPClient::NotOK)
		      {
               offset += sizeSent;
         		
			      // Send remaining data
			      status = client->send( (outBuffer + offset), (size - offset), sizeSent );
		      }
         }

         delete [] outBuffer;
         outBuffer = 0;

            //Clear the event record associated with the socket 
         (void) WSAEventSelect(client->socket(), NULL, 0);

         // Set event to be signaled when we can read or close
         (void) WSAEventSelect(client->socket(), connectionEvent, FD_READ | FD_CLOSE);

         // Wait for socket
         returnValue = WaitForMultipleObjects(2, handleArr, FALSE, Connection_Timeout);
         
         eventIndex = returnValue - WAIT_OBJECT_0;	// Get index for event

         if (returnValue == WAIT_FAILED)		// Event failed
		   {
			   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
				   "Error: WaitForMultipleObjects, GetLastError() = %d\n",
				   GetLastError()));

			   error = -1;
		   }
		   else if (returnValue == WAIT_TIMEOUT)	// Connection timeout
		   {
			   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
				   "Error: Connection timeout. Disconnecting.\n"));

			   error = -1;
		   }
		   else if ( eventIndex == 0)			// Shutdown event
		   {
			   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
				   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
				   "Shutdown event signaled. Disconnecting client.\n"));

			   error = -1;
		   }
		   else if ( eventIndex == 1)			// Socket ready for read
         {
            char inBuffer[BUFSIZ];
			   unsigned int bytesRead = 0;

            do
			   {
				   status = client->poll(inBuffer, BUFSIZ, bytesRead); // Try to read TCP data
   				
				   if (bytesRead > 0)
				   {
					   error = parser.newData(inBuffer, bytesRead); // Copy data to parser

					   if (error)
					   {
						   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
							   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
							   "Error: Cannot copy data to parser\n"));
					   }
				   }
   					
			   }
			   while (bytesRead > 0);	// Loop as long as data is returned by the TCP connection

            if (status == TCPClient::OK)	// Data has been read and status is still OK
			   {								// We can go on and parse the data
				   try
				   {
					   error = parser.parse();			// Parse data 
				   }
				   catch (ACS_CS_Exception ex)
				   {
					   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						   "(%t) ACS_CS_SlaveReplicator::sendRequest()\n"
						   "Error: Exception thrown by parser\n"));
               }

               ACS_CS_PDU inPdu;

               if ( parser.getPDU(inPdu) )
               {
                  (*pdu) = inPdu;
               }
            }
         }
      }
      else
         error = -1;
   }
   else
      error = -1;

    if (shutdownEvent)
      (void) CloseHandle(shutdownEvent);

   if (connectionEvent)
      (void) CloseHandle(connectionEvent);

   if (client)
      delete client;

   return error;

}

 int ACS_CS_SlaveReplicator::getRequest (ACS_CS_Protocol::CS_Scope_Identifier scope, ACS_CS_PDU &pdu)
{

   int success = -1;

   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::getRequest()\n"
						"Calling lock->start_writing()\n"));

   lock->start_writing();

   if (tableMap)
   {
      tableMapType::iterator mapIt = tableMap->find(scope); // Search for table

    
	   if ( mapIt != tableMap->end() )				// if entry exists
      {
         queue<ACS_CS_PDU *> * tempQueue = mapIt->second;

         if (tempQueue)
         {
            if ( ! tempQueue->empty() )
            {
               ACS_CS_PDU * inPdu = tempQueue->front();
               tempQueue->pop();

               if (inPdu)
               {
                  pdu = (*inPdu);
                  delete inPdu;

                  success = 0;
               }
            }
         }
      }
      else
         success = 0;
   }

   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::getRequest()\n"
						"Calling lock->stop_writing()\n"));

   lock->stop_writing();

   return success;

}

 void ACS_CS_SlaveReplicator::sendACK (ACS_CS_PDU *pdu)
{

   if (pdu == 0)
      return;

      ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::sendACK()\n"
						"Calling lock->start_writing()\n"));

   lock->start_writing();

   ACS_CS_PDU * tempPdu = pdu->clone();
   this->outQueue.push(tempPdu);

   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::sendACK()\n"
						"Calling lock->stop_writing()\n"));

   lock->stop_writing();

}

 bool ACS_CS_SlaveReplicator::startSync ()
{

   bool answer = false;

   EnterCriticalSection(&syncCriticalSection);

   if (syncInProgress == false)
   {
	   syncInProgress = true;
      answer = true;
   }

   LeaveCriticalSection(&syncCriticalSection);

   return answer;

}

 void ACS_CS_SlaveReplicator::stopSync ()
{

   EnterCriticalSection(&syncCriticalSection);

	syncInProgress = false;

   LeaveCriticalSection(&syncCriticalSection);

   if (SetEvent(syncEventHandle) == 0)				// Notify handler
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::stopSync()\n"
			"Error: Cannot signal event, GetLastError() = %d\n",
			GetLastError()));
	}

}

 bool ACS_CS_SlaveReplicator::isSyncing ()
{

   bool answer = false;

   EnterCriticalSection(&syncCriticalSection);

   answer = syncInProgress;

   LeaveCriticalSection(&syncCriticalSection);

   return answer;

}

 void ACS_CS_SlaveReplicator::start ()
{

   ACS_CS_SlaveReplicator * instance = ACS_CS_SlaveReplicator::getInstance();

   if (instance)
      (void) instance->activate();

}

 void ACS_CS_SlaveReplicator::stop ()
{

   ACS_CS_SlaveReplicator * instance = ACS_CS_SlaveReplicator::getInstance();

   if (instance)
   {
      (void) instance->deActivate();   // Stop thread
      (void) instance->wait(INFINITE); // Wait for thread to finish
      instance->close();               // Close thread
   }

}

 bool ACS_CS_SlaveReplicator::startSession (TCPClient **client, unsigned short port)
{

   HANDLE connectionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (connectionEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			"(%t) ACS_CS_SlaveReplicator::startSession()\n"
			"Error: Cannot create connection event\n"));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Unable to create windows event",
					"");
	}

	bool connectionSuccessful = false;

   // Start of critical section
	EnterCriticalSection(&ipCriticalSection);
	unsigned long ipAddress = lastWorkingIP;  // Get last working IP address
   LeaveCriticalSection(&ipCriticalSection);
	// End of critical section

   unsigned long old_ipAddress = ipAddress;

   // Delete old client
   if ( *client)
   {
      delete (*client);
      (*client) = 0;
   }

   //unsigned long ipAddressA1 = 

	for (int i = 0; i < 5; i++)	//try 5 different ip addresses
	{
      //allocate memory for non-blocking TCP client
      if ( (*client) )
		{
			delete (*client);
			(*client) = 0;
		}
      (*client) = new TCPClient(TCPClientServer::NonBlocking, 0);

      unsigned long ipA1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_A_HOSTNAME);
      unsigned long ipB1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_A_HOSTNAME);
      unsigned long ipA2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_B_HOSTNAME);
      unsigned long ipB2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_B_HOSTNAME);

		if (connectionEvent)
		{
			switch (i)
			{
            case 0	:	 break;		//first try the last working IP address (if any)
				case 1	:	ipAddress = ipA1; break;
				case 2	:	ipAddress = ipB1; break;
				case 3	:	ipAddress = ipA2; break;
				case 4	:	ipAddress = ipB2; break;
			}

         if (isAP1)  // If AP1
         {
            bool ownAddress = false;
            // Don't try to connect to own IP address
            if (side == ACS_CS_Protocol::Side_A)
            {
               if (ipAddress == ipA1)
                  ownAddress = true;
               else if (ipAddress == ipA2)
                  ownAddress = true;
            }
            else if (side == ACS_CS_Protocol::Side_B)
            {
               if (ipAddress == ipB1)
                  ownAddress = true;
               else if (ipAddress == ipB2)
                  ownAddress = true;
            }

            if (ownAddress)
            {
               if ( (*client) )
		         {
			         delete (*client);
			         (*client) = 0;
		         }

               continue;
            }
         }

			if (ipAddress != 0)	//only try connect if address isn't 0
			{
				//only try connect if address hasn't been tested before
				if ((i == 0) || (i > 0 && ipAddress != old_ipAddress))
				{
               if ( (*client)->connectOn(ipAddress, port) == TCPClient::OK)
					{
						//Set connection event to be signaled when socket connection is completed
						(void)WSAEventSelect( (*client)->socket(), connectionEvent, FD_CONNECT);

						//Start non-blocking TCP connection to the server
						(void) (*client)->connect();

						//Define time-out value for establishment of TCP connection
						DWORD result = WaitForSingleObject(connectionEvent, ACS_CS_NS::Connection_Timeout);
						
						//Clear the event record associated with the socket 
						(void)WSAEventSelect( (*client)->socket(), NULL, 0);

						if (result == WAIT_OBJECT_0)		//in signaled state
						{
							//successfully connected to the server
							if ( (*client)->connect() == Media::Connected)	
								connectionSuccessful = true;
						}
					}
				}
			}
		}

		if (connectionSuccessful)
      {
         // Start of critical section
	      EnterCriticalSection(&ipCriticalSection);
	      lastWorkingIP = ipAddress;                // Save last working IP address
         LeaveCriticalSection(&ipCriticalSection);
         // End of critical section

			break;
      }
      else
		{
			if ( (*client) )
			{
				delete (*client);
				(*client) = 0;
			}
		}
	}

   if (connectionEvent)
		(void) CloseHandle(connectionEvent);

   return connectionSuccessful;

}

 void ACS_CS_SlaveReplicator::addToqueue (ACS_CS_PDU &pdu)
{

   // Ignoring warning for not freeing pointers. They are stored here and freed elsewhere
   //lint --e{429}

	      ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::addToqueue()\n"
						"Calling lock->start_writing()\n"));

   lock->start_writing();

   CS_Scope_Identifier scope = ACS_CS_Protocol::Scope_NotSpecified;
   const ACS_CS_Header * header = pdu.getHeader();

   if (header)
      scope = header->getScope();

   if (scope != ACS_CS_Protocol::Scope_NotSpecified)
   {
      if (tableMap)
      {
         tableMapType::iterator mapIt = tableMap->find(scope); // Search for table

         queue<ACS_CS_PDU *> * tempQueue = 0;

	      if ( mapIt == tableMap->end() )				// if entry exists
         {
            tempQueue = new queue<ACS_CS_PDU *>();

            // Add queue to the map
            tableMap->insert( tableMapType::value_type(scope, tempQueue) );
         }
         else
            tempQueue = mapIt->second;

         ACS_CS_PDU * newPDU = pdu.clone();

         if (newPDU)
            tempQueue->push(newPDU);

         HANDLE scopeHandle = 0;

         if (scope == ACS_CS_Protocol::Scope_HWCTable)
            scopeHandle = hwcReplicationEvent;
         else if (scope == ACS_CS_Protocol::Scope_CPIdTable)
            scopeHandle = cpIdReplicationEvent;
         else if (scope == ACS_CS_Protocol::Scope_CPGroupTable)
            scopeHandle = cpGroupReplicationEvent;
         else if (scope == ACS_CS_Protocol::Scope_FDTable)
            scopeHandle = fdReplicationEvent;
         else if (scope == ACS_CS_Protocol::Scope_NE)
            scopeHandle = neReplicationEvent;
         else if (scope == ACS_CS_Protocol::Scope_VLAN)
            scopeHandle = vlanReplicationEvent;
         
         if (scopeHandle)
         {
            if (SetEvent(scopeHandle) == 0)				// Notify handler
	         {
		         ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
			         "(%t) ACS_CS_SlaveReplicator::addToqueue()\n"
			         "Error: Cannot signal event, GetLastError() = %d\n",
			         GetLastError()));
	         }
         }
      }
   }

   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::addToqueue()\n"
						"Calling lock->stop_writing()\n"));

   lock->stop_writing();

}

 ACS_CS_PDU * ACS_CS_SlaveReplicator::getFromOutQueue ()
{

   ACS_CS_PDU * pdu = 0;

   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::getFromOutQueue()\n"
						"Calling lock->start_writing()\n"));

   lock->start_writing();

   if ( ! this->outQueue.empty() )
   {
      pdu = this->outQueue.front();
      this->outQueue.pop();
   }

   ACS_CS_TRACE((ACS_CS_SlaveReplicator_TRACE,
						"(%t) ACS_CS_SlaveReplicator::getFromOutQueue()\n"
						"Calling lock->stop_writing()\n"));

   lock->stop_writing();

   return pdu;

}

 bool ACS_CS_SlaveReplicator::isConnectionAlive (TCPClient *tcpClient)
{
	bool alive = false;
	if (tcpClient && tcpClient->isConnected()) {
		char echoBuffer [ECHOBUFSIZ] = {1};
		unsigned int bytesToSend = sizeof(echoBuffer);
		unsigned int sizeSent = 0;
		if (tcpClient->send(echoBuffer, bytesToSend, sizeSent) == TCPClient::OK) alive = true;
	}
	return alive;
}

// Additional Declarations

