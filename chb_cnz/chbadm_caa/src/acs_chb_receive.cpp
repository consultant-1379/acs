/*=================================================================== */
   /**
   @file acs_chb_receive.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_receive.h module

   @version 1.0.0
	*/
	/*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/01/2011   XNADNAR   Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ace/OS_NS_errno.h>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <poll.h>		
#include <stdlib.h>
#include <signal.h>   
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ace/Time_Value.h>
#include <acs_chb_tra.h>
#include <acs_chb_receive.h>
#include <acs_chb_common.h>

// Trace points.
/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief INVALID_SOCKET

 */
/*=================================================================== */
const int INVALID_SOCKET = 0;
/*=================================================================== */
/**
   @brief SOCKET_ERROR

 */
/*=================================================================== */
const int SOCKET_ERROR = -1;
/*=================================================================== */
/**
   @brief PORT

 */
/*=================================================================== */
const int PORT = 1234;

/*===================================================================
   ROUTINE: ACS_CHB_Receive
=================================================================== */

ACS_CHB_Receive::ACS_CHB_Receive()
{
	// Allocate socket structure to be used when searching for incoming
	// requests 
	Socket = new pollfd[MAX_NUMBER_OF_CLIENT_CONNECTIONS + 1];

} // End of constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_Receive
=================================================================== */

ACS_CHB_Receive::~ACS_CHB_Receive()
{
	// De-allocate previously allocated resource.
	delete[] Socket;
	Socket = 0;
	
} // End of destructor

/*===================================================================
   ROUTINE: initReceive
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_Receive::initReceive(ACE_HANDLE *ListenSocket)
{

	INFO(1,"%s", "In ACS_CHB_Receive::initReceive");
	
	char error_text[ACS_CHB_BUFSIZE];
	struct sockaddr_in sin;
	ACE_OS::memset(&sin, 0, sizeof(sin));       /* Clear struct */

	sin.sin_family = AF_INET;                      /* Internet/IP */
	sin.sin_port = htons(ACS_CHB_serverPort);      /* server port */
	sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  /* IP address */
	
	// Create socket to be used.
	if((*ListenSocket = ACE_OS::socket(AF_INET, SOCK_STREAM, 0)) == ACE_INVALID_HANDLE) 
	{
		// Failed to create listen socket.
		ACE_OS::sprintf(error_text,"%s,errno=%d\n",ACS_CHB_SocketCreationOfListenFailedText,ACE_OS::last_error());

		setErrorText(error_text);
		setError(ACS_CHB_SocketCreationOfListenFailedID);

		ERROR(1,"%s, errno= %d",ACS_CHB_SocketCreationOfListenFailedText, ACE_OS::last_error());
		return(ACS_CHB_FAIL);
	}
	
	// Allow reuse of address
	int optval = true;

	if((ACE_OS::setsockopt( *ListenSocket, SOL_SOCKET , SO_REUSEADDR, (char*)&optval, sizeof(optval)))
		== SOCKET_ERROR) 
	{

		// Tell the user that we could not reuse IP address              
		ACE_OS::sprintf(error_text,"%s,errno=%d\n", ACS_CHB_SocketReUseAdrFailedText, ACE_OS::last_error());
		setErrorText(error_text);
		setError(ACS_CHB_SocketReUseAdrID);

		ERROR(1,"%s, errno= %d",ACS_CHB_SocketReUseAdrFailedText,ACE_OS::last_error());
	}

	// Associate a local address with a socket
	if(ACE_OS::bind(*ListenSocket, (struct sockaddr*)&sin, sizeof (sin)) == SOCKET_ERROR )
	{
		// Failed to create listen socket.
		sprintf(error_text,"%s,errno=%d\n",
			ACS_CHB_BindFailedText,ACE_OS::last_error());
		setErrorText(error_text);
		setError(ACS_CHB_BindFailedID);
		// Trace point
		ERROR(1,"%s, errno= %d, port no= %d",ACS_CHB_BindFailedText,ACE_OS::last_error(),ACS_CHB_serverPort);
                ACE_OS::closesocket(*ListenSocket);
                *ListenSocket = ACE_INVALID_HANDLE;
		return(ACS_CHB_FAIL);
	}
	
	// Listen to incoming request.
	if(ACE_OS::listen(*ListenSocket, 5) == SOCKET_ERROR)
	 {
		// Failed to create listen socket.
		ACE_OS::sprintf(error_text,"%s,errno=%d\n",
		ACS_CHB_ListenFailedText,ACE_OS::last_error());
		setErrorText(error_text);
		setError(ACS_CHB_ListenFailedID);
		// Trace point
		ERROR(1,"%s, errno= %d",ACS_CHB_ListenFailedText,ACE_OS::last_error());
                ACE_OS::closesocket(*ListenSocket);
                *ListenSocket = ACE_INVALID_HANDLE;
		return(ACS_CHB_FAIL);
	}
	
	INFO(1,"%s", "Out ACS_CHB_Receive::initReceive success");
	
	return(ACS_CHB_OK);
}//End of

/*===================================================================
   ROUTINE: isClientRequestPending
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_Receive::isClientRequestPending(int *PendingRequest,
										ACE_HANDLE ListenSocket,
										ACS_CHB_Subscriber *headSubscriber)
{
	DEBUG(1,"%s", "ACS_CHB_Receive::isClientRequestPending - Entering");
	ACS_CHB_Subscriber *ptr = headSubscriber;
	int index = 0;
	
	// Use first position in socket array for new clients.
	
	Socket[0].fd  = ListenSocket;
	ptr->set_sForReportToClient(ListenSocket);
	Socket[0].events = POLLIN;
        // HQ69037 - Timeout is reduced from 5 sec to 2 sec.
	ACE_Time_Value tv(2,0);
	
	// Start from head of the event list and init socket buffer with 
	// session data.
	DEBUG(1,"%s", "ACS_CHB_Receive::isClientRequestPending - Before while.");
	while (ptr != (ACS_CHB_Subscriber *)NULL)
	{
		// Init socket entry.
		Socket[index].fd = ptr->get_sForReportToClient();
		Socket[index].events = POLLIN;
		// Indicate that no client request pending.
		ptr->set_requestPending(0);
		// Increase index, point to next link.
		++index;
		ptr = ptr->get_next();
	} // End of while
	
	// Check if data or request pending on any of the supplied file 
	// descriptors.
	DEBUG(1,"%s", "ACS_CHB_Receive::isClientRequestPending - Before poll");
	ACS_CHB_returnType status = ACS_CHB_FAIL;
	int  rc;
	rc = ACE_OS::poll(Socket,index,&tv);
	if (rc > 0) 
	{
		// Data or request found, add new client to the end of the list,
		// or update subscription information for existing client.
		if (addRequestToSubscriberList(Socket,ListenSocket,
			headSubscriber) == ACS_CHB_OK) 
		{
			status = ACS_CHB_OK;
		}

		// Set the number of pending requests found.
		*PendingRequest = rc;
	} 
	else // rc <= 0
	{ 
		if (rc == 0) 
		{
			// No data or request.
			status = ACS_CHB_FAIL;
			*PendingRequest = rc;			// Indicate no client requests.
		}
		else  // rc < 0
		{
			// Poll failed.
			// Trace point

			ERROR(1,"%s, error no= %d",ACS_CHB_PollFailedText,ACE_OS::last_error());
			status = ACS_CHB_FAIL;
		}
	}
	ACE_Time_Value sleepTime(0, 10000);
	if(status == ACS_CHB_FAIL)
	{
		ACE_OS::sleep(sleepTime); // make sure we dont load to much
	}
	DEBUG(1,"%s", "ACS_CHB_Receive::isClientRequestPending - Leaving");
	return status;
} // End of isClientRequestPending

/*===================================================================
   ROUTINE: sendValueToClient
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_Receive::sendValueToClient(ACE_HANDLE ClientSocket,char *CPObjectValue)
{
	int rc;
	ACS_CHB_returnType status = ACS_CHB_OK;
	char error_text[ACS_CHB_SMALL_BUFSIZE];
	
	// Send back value to client.
	
	rc = ACE_OS::send(ClientSocket,CPObjectValue,ACE_OS::strlen(CPObjectValue),0);
	if (rc == SOCKET_ERROR ) // Write failed.
	{
		ACE_OS::sprintf(error_text,"%s,errno=%d\n",ACS_CHB_WriteFailedText,ACE_OS::last_error());
		setErrorText(error_text);
		setError(ACS_CHB_WriteFailedID);
		// Trace point

			ERROR(1,"%s ,errno= %d",ACS_CHB_WriteFailedText,ACE_OS::last_error());

			status = ACS_CHB_FAIL;   // Indicate failure
	} 
	else // rc >= 0
	{ 
		// Indicate no error
		setErrorText(ACS_CHB_noErrorText);
		setError(ACS_CHB_noErrorID);		
		// Trace point
		DEBUG(1,"Wrote %d bytes of data, data= %s",rc,CPObjectValue);
		status = ACS_CHB_OK; // Indicate success
	}
	
	// Return status of operation
	
	return status;
} // End of sendValueToClient

/*===================================================================
   ROUTINE: closeAllClientSessions
=================================================================== */

void 
ACS_CHB_Receive::closeAllClientSessions(ACS_CHB_Subscriber *headSubscriber)
{
	ACS_CHB_Subscriber *ptr = headSubscriber;
	
	// Disconnect all client sessions.
	
	while (ptr != 0) 
	{
		if (ptr->get_sForReportToClient() != INVALID_SOCKET) 
		{
			// Close session towards client
			ACE_OS::closesocket(ptr->get_sForReportToClient());
			ptr->set_sForReportToClient(INVALID_SOCKET);  // Indicates socket closed
		}
		ptr = ptr->get_next();
	} // End of while
} // End of closeAllClientSessions

/*===================================================================
   ROUTINE: addRequestToSubscriberList
=================================================================== */

ACS_CHB_returnType
ACS_CHB_Receive::addRequestToSubscriberList(struct pollfd      *RequestSocket,
											ACE_HANDLE          ListenSocket,
											ACS_CHB_Subscriber *headSubscriber)
{
	ACS_CHB_Subscriber    *ptr = headSubscriber;
	unsigned int number_of_links_in_list = 0;
	

	// Count the number of links to scan
	
	while (ptr != (ACS_CHB_Subscriber *)NULL)
	{
		++number_of_links_in_list;
		ptr = ptr->get_next();
	} // End of while
	
	// Decode request.
	
	struct sockaddr ClientAdr;
	ACE_HANDLE  AcceptSocket = INVALID_SOCKET;
	int  AdrLen = 0;
	unsigned long index = 0;
	int  rc;
	ACS_CHB_returnType status = ACS_CHB_OK;
	char error_text[ACS_CHB_SMALL_BUFSIZE];
	for (index = 0;index < number_of_links_in_list;++index) 
	{
		// Init socket entry.
		if (RequestSocket[index].revents) 
		{		// Data avail ?
			if (index == 0) 
			{	// This is Listen socket.
				// Setup structure before accept can be done.

				AdrLen = sizeof (ClientAdr);
				ACE_OS::memset((char *)&ClientAdr,0,sizeof(ClientAdr));
				// Perform accept of incoming request.
				AcceptSocket = ACE_OS::accept(ListenSocket,(struct sockaddr *)&ClientAdr,
					&AdrLen);
				if (AcceptSocket < 0) 
				{
					// Poll failed.
					ACE_OS::sprintf(error_text,"%s,errno=%d\n",ACS_CHB_AcceptFailedText,ACE_OS::last_error());
					setErrorText(error_text);
					setError(ACS_CHB_AcceptFailedID);
					// Trace point
					ERROR(1,"%s, errno=%d",ACS_CHB_AcceptFailedText,ACE_OS::last_error());
					status = ACS_CHB_FAIL;		// Operation failed
				}
				// New client. First verify that the number of attached 
				// clients don't exceed that maximum permitted. If not 
				// append new client to the end of the subscriber list.
				if (numberOfClients(headSubscriber)<=MAX_NUMBER_OF_CLIENT_CONNECTIONS)
				{
					if (headSubscriber->get_next() == 0) // Only one element in the list
					{
						ptr = headSubscriber;
						ptr -> set_sForReportToClient(ListenSocket);
					} 
					else // Already several elements in the list. Append to list
					{
						ptr = headSubscriber->get_last();
					}
					ACS_CHB_Subscriber* newElm = new ACS_CHB_Subscriber();
					ptr->appendToList(newElm);
					ptr = newElm; 
					// Setup link data.
					ptr->set_sForReportToClient(AcceptSocket);
				}
				else // Max number of permitted clients exceeded.
				{
					ACE_OS::sprintf(error_text,"%s. Max number of allowed clients are = %d\n",
						ACS_CHB_MaxNumberOfAllowedClientsExceededText,
						MAX_NUMBER_OF_CLIENT_CONNECTIONS);
					setErrorText(error_text);
					setError(ACS_CHB_MaxNumberOfAllowedClientsExceededID);
					// Trace point

					ERROR(1,"%s, Max Value= %d",ACS_CHB_MaxNumberOfAllowedClientsExceededText,MAX_NUMBER_OF_CLIENT_CONNECTIONS);
					status = ACS_CHB_FAIL;	// Operation failed
					if(AcceptSocket > 0)
					{
					ACE_OS::closesocket(AcceptSocket);		// Deny incoming request
					}
					AcceptSocket = INVALID_SOCKET;
				}
			} 
			else // index != 0
			{
				// Request for existing client, store request.
				ptr = headSubscriber;

				while (ptr->get_next() != 0) 
				{
					if (RequestSocket[index].fd == ptr->get_sForReportToClient()) 
						break;                     // Link found continue.
					else 
						ptr = ptr->get_next();
				} // End of while
				
				// Get request.
				rc = ACE_OS::recv(RequestSocket[index].fd,ptr->get_requestBufferPtr(),
					ACS_CHB_SMALL_BUFSIZE,0); 
				int LastErr = ACE_OS::last_error();
				if (rc == SOCKET_ERROR)  // Error
				{
					// Read failed.
					ACE_OS::sprintf(error_text,"%s,errno=%d\n",ACS_CHB_ReadFailedText,ACE_OS::last_error());
					setErrorText(error_text);
					setError(ACS_CHB_ReadFailedID);
					// Trace point

					ERROR(1,"%s, errno =%d",ACS_CHB_ReadFailedText,LastErr);
					status = ACS_CHB_FAIL;		// Operation failed
				} 
				else // rc >= 0
				{ 
					if (rc > 0) // Data read
					{
						// Trace point
						DEBUG(1,"Read %d bytes of data, data= %s",rc, ptr->get_requestBufferPtr());
					}
				}
				// Indicate that client request now is pendning.
				ptr->set_requestPending(1);
				// Check if client
				// no longer exists.
				if (rc == 0 || LastErr == ECONNRESET
					|| LastErr == ENOTCONN
					|| LastErr == ECONNABORTED
					|| LastErr == ETIMEDOUT
					|| LastErr == ESHUTDOWN)
				{
					// close socket.
					ACE_OS::closesocket(RequestSocket[index].fd);
					// Unlink and destroy link element.
					ptr = headSubscriber->get_next(); // Ignore head of the list

					while (ptr != 0) 
					{
						if (ptr->get_sForReportToClient() == RequestSocket[index].fd) 
						{
							// Remove element
							ptr->set_sForReportToClient(INVALID_SOCKET); // Socket already closed
							delete ptr;
							ptr = 0; // Break the while-loop
						} 
						else 
						{
							ptr = ptr->get_next(); // Try next element
						}
					} // End of while
				} // End of if (rc == 0)
			} // End of if (index == 0)
		} // End of if (RequestSocket[index].revents)
	} // End of for-loop

	return status;
} // End of addRequestToSubscriberList

/*===================================================================
   ROUTINE: numberOfClients
=================================================================== */
int
ACS_CHB_Receive::numberOfClients(ACS_CHB_Subscriber *headSubscriber)
{
	ACS_CHB_Subscriber *ptr = headSubscriber;
	
	// Count the current number of clients
	
	int number_of_clients = 0;
	while (ptr != 0) 
	{
		++number_of_clients;
		ptr = ptr->get_next();
	} // End of while
	
	// Return the number of clients found.
	
	return number_of_clients;
} // End of numberOfClients

