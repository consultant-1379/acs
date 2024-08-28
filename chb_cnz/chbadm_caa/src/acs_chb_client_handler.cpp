/*=================================================================== */
   /**
   @file acs_chb_client_handler.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_client_handler.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/01/2011   XNADNAR   Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
/*! port getErrorText (commented) */
#include <string.h>
#include <errno.h>
#include <ace/OS_NS_sys_socket.h>
#include <ace/Event.h>
#include <ACS_APGCC_Util.H>
#include <acs_chb_cp_objectlist.h>
#include <acs_chb_client_handler.h>
#include <acs_chb_tra.h>
#include <acs_chb_receive.h>
//#include "ACS_AEH_EvReport.H"
extern ACS_CHB_CPObjectList *HeadCPObjectList;

/*===================================================================
   ROUTINE: ACS_CHB_ClientHandler
=================================================================== */
 
ACS_CHB_ClientHandler::ACS_CHB_ClientHandler() :
	ACS_CHB_Event (ACS_CHB_processName)
{
	//INFO(1,"%s","Executing ACS_CHB_ClientHandler constructor");
  // Default head for subscriber list.
	memset(requestBuffer,'\0',sizeof(requestBuffer));
	headSubscriber = new ACS_CHB_Subscriber();
	headReceive = 0;
	listenSocket = ACE_INVALID_HANDLE;
} // End of constructor
 
/*===================================================================
   ROUTINE: ~ACS_CHB_ClientHandler
=================================================================== */
 
ACS_CHB_ClientHandler::~ACS_CHB_ClientHandler()
{
	// Delete all items in subscriber list
	//INFO(1,"%s","Executing ACS_CHB_ClientHandler Destructor");
	ACS_CHB_Subscriber *ptr = headSubscriber->get_next(); // Keep first elem

	while (ptr != (ACS_CHB_Subscriber *)0)
	{
		delete ptr;
		ptr = headSubscriber->get_next();
	} // End of while
	
	// Free up Startup low level receive handler;

	if (headReceive)
	{
		delete headReceive;
		headReceive = 0;
	}	
	if (listenSocket != ACE_INVALID_HANDLE)
	{
		ACE_OS::closesocket(listenSocket);
	}
	
	// Free up default head for subscriber list.
	
	delete headSubscriber;
	headSubscriber = 0;
	//INFO(1,"%s","Leaving ACS_CHB_ClientHandler Destructor");

} // End of destructor

/*===================================================================
   ROUTINE: initClientHandler
=================================================================== */
ACS_CHB_returnType ACS_CHB_ClientHandler::initClientHandler()
{
	// Head of the CP object list
	// base class    =  global variable
	INFO(1,"%s","Executing ACS_CHB_ClientHandler::initClientHandler method");
	headCPObjectList = HeadCPObjectList;

	// Startup low level receive handler;

	headReceive = new ACS_CHB_Receive();

	// Setup server to accept incoming requests from clients.
	// return status.

	ACS_CHB_returnType status = ACS_CHB_FAIL;;
	while(status == ACS_CHB_FAIL)
	{
            status = headReceive -> initReceive(&listenSocket);
           // sleep(10);
            sleep(2);
	}

	return status;
} // End of initClientHandler

/*===================================================================
   ROUTINE: isRequestFromClientPending
=================================================================== */

ACS_CHB_returnType ACS_CHB_ClientHandler::isRequestFromClientPending(int *pending_request)
{
	INFO(1,"%s","Executing ACS_CHB_ClientHandler::isRequestFromClientPending method");
	// Find out whether pending client requests exists.

	return headReceive->isClientRequestPending(pending_request,listenSocket,
										 headSubscriber);
} // End of isRequestFromClientPending

/*===================================================================
   ROUTINE: addNewClientToSubscriberList
=================================================================== */

void ACS_CHB_ClientHandler::addNewClientToSubscriberList()
{
	INFO(1,"%s","Executing addNewClientToSubscriberList method");
	ACS_CHB_Subscriber *ptr = headSubscriber;

	// Add new client to list.

	while(ptr != 0) 
	{
		if (ptr ->get_requestPending()) 
		{
		  // Request pending, add it to subscribers object list.
		  convertRequestIntoEventSubscription(ptr);

		  // Send back current object value to the client.
		  convertRequestsIntoActions();

		  replyOnRequestToClient(ptr);
		}
		ptr = ptr->get_next();
	} // End of while
} // End of addNewClientToSubscriberList

/*===================================================================
   ROUTINE: convertRequestsIntoActions
=================================================================== */

void ACS_CHB_ClientHandler::convertRequestsIntoActions()
{
	INFO(1,"%s","Executing ACS_CHB_ClientHandler::convertRequestsIntoActions method");
	ACS_CHB_Subscriber *ptr = headSubscriber;

	// Convert all pending requests into actions.

	while (ptr != 0) 
	{
		// Check whether request pending:
		if (ptr->get_requestPending()) 
		{
			  // Request pending, add it to subscribers object list.
		  convertRequestIntoEventSubscription(ptr);
		}
		// Go to next link.
		ptr = ptr->get_next();
	} // End of while
} // End of convertRequestsIntoActions

/*===================================================================
   ROUTINE: convertRequestIntoEventSubscription
=================================================================== */

void ACS_CHB_ClientHandler::convertRequestIntoEventSubscription(
				ACS_CHB_Subscriber *requestFromClient)
{
	INFO(1,"%s","Executing ACS_CHB_ClientHandler::convertRequestIntoEventSubscription method");
	char tag_item[ACS_CHB_SMALL_BUFSIZE];
	char tag_data[ACS_CHB_SMALL_BUFSIZE];

	// Extract header and data from [header][data]

	char* cptr = requestFromClient->get_requestBufferPtr();
	cptr++; // Add one to step ahead of [
	ACE_OS::strcpy(tag_item,cptr);
	tag_data[0] = 0;
	cptr = ACE_OS::strchr(tag_item,']');
	if (cptr != 0)
	{ 
		*cptr = 0;
		cptr++;    // Points to [
		cptr++;    // Now points to char after [
		ACE_OS::strcpy(tag_data,cptr);
		cptr = ACE_OS::strchr(tag_data,']');
		if (cptr != 0)
		{
			*cptr = 0;
		}
	}


	// Scan subscribers CP object list to see whether requested item
	// already exists, and has a different value. If so send back value

	int CP_object_currently_in_list = ACS_CHB_FAIL;
	ACS_CHB_EventList* ptr = requestFromClient->get_headEventList();
	while (ptr  != (ACS_CHB_EventList *) 0)
	{
		if (!ACE_OS::strcmp(tag_item,ptr->get_CPObjectItemPtr())) // Equal
		{
			CP_object_currently_in_list = ACS_CHB_OK;
			if (ACE_OS::strcmp(tag_data,ptr->get_CPObjectValuePtr())) // Not equal
			{
				ptr->set_CPObjectValueChanged(ACS_CHB_OK);
				ACE_OS::strcpy(ptr->get_CPObjectValuePtr(),
				getCPObjectValueFromCPObjectList(ptr->get_CPObjectItemPtr()));
			}
			else 
			{
				ptr->set_CPObjectValueChanged(ACS_CHB_FAIL);
			}
		}
		ptr = ptr->get_next();
	} // End of while

	// If CP object not found in list, create a new link.

	if (CP_object_currently_in_list == ACS_CHB_FAIL) 
	{
		ptr = requestFromClient->get_headEventList();
		// Save data.
		ACE_OS::strcpy(ptr->get_CPObjectItemPtr(),tag_item);
		ACE_OS::strcpy(ptr->get_CPObjectValuePtr(),
		getCPObjectValueFromCPObjectList(ptr->get_CPObjectItemPtr()));
		if (ptr->get_next() == (ACS_CHB_EventList *)0)
		{
			// Link the first one in the set.
			ptr->set_CPObjectValueChanged(ACS_CHB_OK);
		}
		else 
		{
			// Other event links exists.  Create new link
			ptr->set_CPObjectValueChanged(ACS_CHB_FAIL);
		}
		ACS_CHB_EventList* newElm = new ACS_CHB_EventList();
		ptr->appendToList(newElm);
		ptr = newElm;
	}
} // End of convertRequestIntoEventSubscription

/*===================================================================
   ROUTINE: replyOnRequestToClient
=================================================================== */

ACS_CHB_returnType ACS_CHB_ClientHandler::replyOnRequestToClient( ACS_CHB_Subscriber *requestFromClient)
{
	INFO(1,"%s","Executing ACS_CHB_ClientHandler::replyOnRequestToClient method");
	ACS_CHB_returnType status = ACS_CHB_OK;

	// Scan subscribers CP object list to see whether updated values
	// exists. If so send back value to client.

	ACS_CHB_EventList* ptr = requestFromClient->get_headEventList();
	while (ptr != (ACS_CHB_EventList *) 0)
	{
		if (ptr->get_CPObjectValueChanged() == ACS_CHB_OK) 
		{
			if ((headReceive->sendValueToClient( requestFromClient->get_sForReportToClient(),
												   ptr->get_CPObjectValuePtr())) == ACS_CHB_FAIL) 
			{
				// Event handling
				event(ACS_CHB_sendValueToClientFailedID,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					ACS_APGCC::getSystemErrorText(ACS_APGCC::getLastError()),
					ACS_CHB_sendValueToClientFailedText);
				status = ACS_CHB_FAIL;
			}
		}
		ptr = ptr->get_next();
	} // End of while

	// Return status of operation.

	return status;
} // End of replyOnRequestToClient

/*===================================================================
   ROUTINE: sendInfoAboutUpdatedObjectItemToSubscribers
=================================================================== */

ACS_CHB_returnType ACS_CHB_ClientHandler::sendInfoAboutUpdatedObjectItemToSubscribers(
							char cpObjectItem[],
							char cpObjectValue[])
{
	INFO(1,"%s","Executing ACS_CHB_ClientHandler::sendInfoAboutUpdatedObjectItemToSubscribers method");
	ACS_CHB_Subscriber *ptr_subscriber = headSubscriber;
	ACS_CHB_EventList *ptr_event_list = 0;
	ACS_CHB_returnType status = ACS_CHB_OK;	// Assume that operation successful.

	// Scan through subscribers event list for matching objects.

	while(ptr_subscriber != (ACS_CHB_Subscriber *)0)
	{
		// Search through event list.
		ptr_event_list = ptr_subscriber->get_headEventList();
		while (ptr_event_list != (ACS_CHB_EventList *) 0)
		{
			// Send info to client about the updated CP object value.
			if (((!ACE_OS::strcmp(ptr_event_list ->get_CPObjectItemPtr(),cpObjectItem)) &&
							(ACE_OS::strcmp(ptr_event_list ->get_CPObjectValuePtr(),cpObjectValue))))
			{
				// Indicate that value now updated.
				ptr_event_list->set_CPObjectValueChanged(ACS_CHB_FAIL);
				ACE_OS::strcpy(ptr_event_list->get_CPObjectValuePtr(),cpObjectValue);
				if ((headReceive->sendValueToClient(
						  ptr_subscriber->get_sForReportToClient(),
						  ptr_event_list->get_CPObjectValuePtr())) == ACS_CHB_FAIL)
				{
					// Event handling
					event(ACS_CHB_sendValueToClientFailedID,
						  ACS_CHB_EVENT_STG,
						  ACS_CHB_Cause_APfault,
						  ACS_APGCC::getSystemErrorText(ACS_APGCC::getLastError()),
						  ACS_CHB_sendValueToClientFailedText);
					status = ACS_CHB_FAIL;
				}
			}
			// Correction 2001-03-13 UABBJIS
			// Special handling of "Last_Restart_Value"
			// Should always send to subscribers
			else if ((!ACE_OS::strcmp(ptr_event_list ->get_CPObjectItemPtr(),cpObjectItem)) &&
						(0 == ACE_OS::strcmp(cpObjectItem,"Last_Restart_Value")))
			{
				// Indicate that value now updated.
				ptr_event_list->set_CPObjectValueChanged(ACS_CHB_FAIL);
				ACE_OS::strcpy(ptr_event_list->get_CPObjectValuePtr(),cpObjectValue);
				if ((headReceive->sendValueToClient(
								ptr_subscriber->get_sForReportToClient(),
									ptr_event_list->get_CPObjectValuePtr())) == ACS_CHB_FAIL)
				{
					// Event handling
					event(ACS_CHB_sendValueToClientFailedID,
						  ACS_CHB_EVENT_STG,
						  ACS_CHB_Cause_APfault,
						  ACS_APGCC::getSystemErrorText(ACS_APGCC::getLastError()),
						  ACS_CHB_sendValueToClientFailedText);
					status = ACS_CHB_FAIL;
				}
			}

			// Next event link.
		  ptr_event_list = ptr_event_list->get_next();
		} // End of while ptr_event_list
		// Next subscriber link.
		ptr_subscriber = ptr_subscriber->get_next();
	} // End of while ptr_subscriber

	// Return status of operation.

	return status;
} // End of sendInfoAboutUpdatedObjectItemToSubscribers

/*===================================================================
   ROUTINE: disconnectAllClients
=================================================================== */

void ACS_CHB_ClientHandler::disconnectAllClients()
{
	INFO(1,"%s","Executing ACS_CHB_ClientHandler::disconnectAllClients method");
	// Disconnect all client sessions
	headReceive -> closeAllClientSessions(headSubscriber);
} // End of disconnectAllClients

