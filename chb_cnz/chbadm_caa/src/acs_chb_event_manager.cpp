/*=================================================================== */
   /**
   @file acs_chb_event_manager.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_event_manager.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY   XNADNAR   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ace/OS_NS_sys_select.h>
//#include <ACS_PRC_Process.h>
#include <acs_chb_tra.h>
#include <acs_chb_heartbeat_def.h>
#include <acs_chb_request_handler.h>
#include <acs_chb_client_handler.h>
#include <acs_chb_event_manager.h>

/*=====================================================================
                        GLOBAL METHOD
==================================================================== */
/*=================================================================== */
     /**
        @brief      searchForRequestFromHeartBeatPending

      */
  /*=================================================================== */
ACE_UINT32 searchForRequestFromHeartBeatPending(ACS_DSD_Session&, int*);
/*=================================================================== */
     /**
        @brief      HeadCPObjectList

      */
  /*=================================================================== */
ACS_CHB_CPObjectList *HeadCPObjectList;
/*=================================================================== */
     /**
        @brief      requestFromHeartBeatPending

      */
  /*=================================================================== */
int requestFromHeartBeatPending;
/*=================================================================== */
     /**
        @brief      requestFromHeartBeat

      */
  /*=================================================================== */
char requestFromHeartBeat[(ACS_CHB_BUFSIZE < MAX_LENGTH_FOR_REQUEST_FROM_HEARTBEAT ? MAX_LENGTH_FOR_REQUEST_FROM_HEARTBEAT : ACS_CHB_BUFSIZE) + 1];
/*
// Handles requests,formatting of data from HeartBeat together with responses.
	ACS_CHB_RequestHandler *requestFromHeartBeatPtr = 0 ;

	// Handles requests,transports data  to clients.
	ACS_CHB_ClientHandler *requestFromClientPtr = 0;

void cleanup(int sigint)
{
	if(requestFromHeartBeatPtr)
	{
		delete requestFromHeartBeatPtr;
		requestFromHeartBeatPtr = 0;
	}
	if(requestFromClientPtr)
	{
		delete requestFromClientPtr;
		requestFromClientPtr = 0;
	}
	DEBUG(1,"%s", "Exit of Event Manager's clean up is successful");

	ACS_CHB_Tra::ACS_CHB_Logging.Close();

	ACE_OS::exit(ACS_AEH_exitOfEventManagerSuccessful);

}*/
/*===================================================================
   ROUTINE: main
=================================================================== */
int  main()
{
	ACS_CHB_Tra::ACS_CHB_Logging.Open("HBEAT");
	// COMMENTED as AP_InitProcess API is removed from ACS
	//	AP_InitProcess(ACS_CHB_processNameChild, AP_SERVICE);
    //ACE_OS::signal(SIGTERM, cleanup);


	// Handles requests,formatting of data from HeartBeat together with responses.
	ACS_CHB_RequestHandler *requestFromHeartBeatPtr = 0 ; 

	// Handles requests,transports data  to clients.
	ACS_CHB_ClientHandler *requestFromClientPtr = 0;     

	// Interface towards HeartBeat.
	requestFromHeartBeatPtr = new ACS_CHB_RequestHandler();
	if( requestFromHeartBeatPtr == 0 )
	{
		ERROR(1, "%s", "Error occured while allocating memory for request handler");
		ACE_OS::exit(ACS_AEH_exitInitClientHandlerFailure);
	}

	// Interface towards clients.
	requestFromClientPtr = new ACS_CHB_ClientHandler();

	if( requestFromClientPtr == 0 )
	{
		ERROR(1, "%s", "Error occured while allocating memory for client handler");

		delete requestFromHeartBeatPtr;
		requestFromHeartBeatPtr = 0;

		ACE_OS::exit(ACS_AEH_exitInitClientHandlerFailure);
	}
	// Init.
	if(requestFromHeartBeatPtr->initPipeFromHeartBeat() != ACS_CHB_OK) 
	{
		// Setup of Heartbeat Pipe failed, terminate.
		ERROR(1,"%s", "Error in connecting to heartbeat server");

		delete requestFromHeartBeatPtr;
		requestFromHeartBeatPtr = 0;

		delete requestFromClientPtr;
		requestFromClientPtr = 0;

		ACE_OS::exit(ACS_AEH_exitInitClientHandlerFailure);
	}

	if(requestFromClientPtr)
	{
		if(requestFromClientPtr->initClientHandler() != ACS_CHB_OK) 
		{
			// Setup of client handler failed, terminate.
			ERROR(1,"%s", "Error in init client handler");

			delete requestFromHeartBeatPtr;
			requestFromHeartBeatPtr = 0;

			delete requestFromClientPtr;
			requestFromClientPtr = 0;

			ACE_OS::exit(ACS_AEH_exitInitClientHandlerFailure);
		}
	}
	DEBUG(1,"%s", "Child initiated - Go into loop... " );

	// Loop until termination requested.

	for(;;) 
	{
		// Request from HeartBeat.
		//
		// Search for request from HeartBeat in Pipe.
		ACE_INT32 dwSuccess;
		int request;
                if (requestFromHeartBeatPtr != 0) //COVERITY 11506
		{
		dwSuccess = searchForRequestFromHeartBeatPending(*requestFromHeartBeatPtr->pcSession, &request);

		// Check if Pipe Read was successful
		if (dwSuccess == ACS_CHB_FALSE) 
		{
			ERROR(1,"%s, errno = %d", ACS_CHB_IPCPipeAccessFailedText, ACE_OS::last_error());
			
			// Termination requested, free up allocated resources and
			// exit gracefully.
			try 
			{
				if (requestFromClientPtr) 
				{
					requestFromClientPtr->disconnectAllClients();
					if (requestFromHeartBeatPtr) 
					{
						delete requestFromHeartBeatPtr;
						requestFromHeartBeatPtr = 0;
					}
					delete requestFromClientPtr;
					requestFromClientPtr = 0;
				}
			} 
			catch(...) 
			{
			}
			break;
		}
		
		}

	        if (requestFromHeartBeatPtr != 0)	////COVERITY 11510
		{
		// Check request code.
			if(requestFromHeartBeatPtr && (requestFromHeartBeatPtr->isRequestFromHeartBeatPending(&request) == ACS_CHB_OK))	
			{
				switch(request)
				{
					case TERMINATE_FROM_HEARTBEAT:
					// Termination requested, free up allocated resources and
					// exit gracefully.
					try 
					{
						if (requestFromClientPtr)
						{
							requestFromClientPtr->disconnectAllClients();
							if (requestFromHeartBeatPtr)
							{
								delete requestFromHeartBeatPtr;
								requestFromHeartBeatPtr = 0;
							}
							delete requestFromClientPtr;
							requestFromClientPtr = 0;
						}
					} 
					catch(...)
					{
					}
					DEBUG(1, "%s","Leaving child process");
					ACE_OS::exit(ACS_AEH_exitOfEventManagerSuccessful);
					break;

					case CPOBJECT_FROM_HEARTBEAT:
						// Request pending, decode and check if available in list.
						char cp_object_item[ACS_CHB_BUFSIZE],
						cp_object_value[ACS_CHB_BUFSIZE];
						DEBUG(1, "%s","Request pending, decode and check if available in list.");
						if(requestFromHeartBeatPtr->decodeCPObjectMessage(cp_object_item,cp_object_value) == ACS_CHB_OK)  
						{
							// Go through object list and update new value in case item 
							// found and old value differs from new value.
							requestFromHeartBeatPtr->searchForAndUpdateObjectItemInList(cp_object_item, cp_object_value);
							// Send CP object with updated value to subscribers.
							if(requestFromHeartBeatPtr->findUpdatedObjectItemInList(cp_object_item, cp_object_value) == ACS_CHB_TRUE)
							{
								// Send info about updated CP object value to clients.
								requestFromClientPtr->sendInfoAboutUpdatedObjectItemToSubscribers(
									cp_object_item, cp_object_value);
							}
						}
						DEBUG(1, "%s"," break; switch");
						break;

					default:
						break;
				}
			}
		}
		//
		// Request from Client.
		//
		DEBUG(1,"%s", "Request from client");
		int client_requests_pending = 0;
	if(requestFromClientPtr != 0) //COVERITY 11506
	{
		if(requestFromClientPtr && (requestFromClientPtr->isRequestFromClientPending(&client_requests_pending) == ACS_CHB_TRUE))
		{
			// Client request pending, convert.
			DEBUG(1,"%s", "Client request pending, convert");
			if(client_requests_pending)
			{
				requestFromClientPtr->addNewClientToSubscriberList();
			}
		}
		DEBUG(1,"%s", "After request from client");
	}
	}// for(;;)

	// Return with successful result.

	DEBUG(1,"%s", "Exit of Event Manager is successful");

	//ACS_CHB_Tra::ACS_CHB_Logging.Close();

	ACE_OS::exit(ACS_AEH_exitOfEventManagerSuccessful);


}//End of main

/*===================================================================
   ROUTINE: searchForRequestFromHeartBeatPending
=================================================================== */

ACE_UINT32 searchForRequestFromHeartBeatPending(ACS_DSD_Session &pcSession, int *requestCode)
{
	DEBUG(1, "%s","searchForRequestFromHeartBeatPending - Entering");
	ACE_UINT32	fSuccess = ACS_CHB_FALSE;
	int ret = 0;

	// Init request code
	*requestCode = ACS_CHB_FALSE;

	ACE_HANDLE handle = ACE_INVALID_HANDLE;
	int handle_count = 1;

	ret = pcSession.get_handles(&handle, handle_count);

	if( ret < 0)
	{
		ERROR(1,"%s","error in get handles for heart beat child, returning.. ");
		return ACS_CHB_FALSE;
	}
	else
	{
		//Now, we have to wait on the handle for some data from the HeartBeat.
		//If some data arrives, we will recieve a signal on the handle and then we can receive the data.

		 //Create a FD set.
		fd_set readHandleSet;
		FD_ZERO(&readHandleSet);
		FD_SET (handle, &readHandleSet);
		DEBUG(1, "%s","searchForRequestFromHeartBeatPending - Before select");
		int ret = ACE_OS::select ((int)(handle+1), &readHandleSet, 0, 0);

		//Check for timeout.
		if (ret >= 0)
		{
		    //Check if the handle is set.
			if ( !FD_ISSET(handle, &readHandleSet))
			{
				ERROR(1,"%s","handle on heartbeat child is not signaled ");
				return ACS_CHB_FALSE;
			}
		}
		else
		{
			ERROR(1,"%s","Error occurred in select system call in event manager");
			return ACS_CHB_FALSE;
		}
		ACE_INT32	dwBytesRead = 0;
		//Now, we can recieve the data from server.
		dwBytesRead = pcSession.recv(requestFromHeartBeat,MAX_LENGTH_FOR_REQUEST_FROM_HEARTBEAT);
		DEBUG(1,"%d bytes of data received from parent", dwBytesRead);

		if (dwBytesRead > 0)
		{
			// Got request successfully, format incoming request string.
			if(ACE_OS::strstr(requestFromHeartBeat,TEXT_TERMINATE_FROM_HEARTBEAT) != (char *) NULL) 
			{
				// Terminate request received from HeartBeat.
				*requestCode = TERMINATE_FROM_HEARTBEAT;
			} 
			else if(ACE_OS::strstr(requestFromHeartBeat,TEXT_CP_OBJECT_FROM_HEARTBEAT) != (char *) NULL) 
			{
					// CP object data received from HeartBeat.
					*requestCode = CPOBJECT_FROM_HEARTBEAT;
			}
			fSuccess = ACS_CHB_TRUE;
		}
			
	}
	DEBUG(1, "%s","searchForRequestFromHeartBeatPending - Leaving");
	return(fSuccess ? ACS_CHB_TRUE : ACS_CHB_FALSE);

} // End of searchForRequestFromHeartBeatPending
