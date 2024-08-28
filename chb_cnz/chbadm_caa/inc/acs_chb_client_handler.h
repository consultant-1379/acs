/*=================================================================== */
/**
   @file   acs_chb_client_handler.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_CLIENT_HANDLER_H 
#define ACS_CHB_CLIENT_HANDLER_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/OS_NS_sys_socket.h>
#include <ace/Event.h>
#include <sys/socket.h>

#include "acs_chb_heartbeat_def.h"
#include "acs_chb_cp_objectlist.h"
#include "acs_chb_event.h"
#include <acs_chb_cp_object.h>
/*=====================================================================
					FORWARD DECLARATION SECTION
==================================================================== */
class ACS_CHB_Receive;
class ACS_CHB_Subscriber;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_ClientHandler


*/
/*=================================================================== */

class ACS_CHB_ClientHandler : virtual private ACS_CHB_CPObjectList,
                              public ACS_CHB_Event
{
/*=====================================================================
	                        PUBLIC DECLARATION SECTION
==================================================================== */

public:
/*=====================================================================
	                        CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
      @brief     Constructor for ACS_CHB_ClientHandler class.

	             Default constructor, used to initialize variables.


*/
/*=================================================================== */
  ACS_CHB_ClientHandler();
/*=====================================================================
	                        CLASS DESTRUCTOR
==================================================================== */
/*=================================================================== */
/**
      @brief     Destructor for ACS_CHB_ClientHandler class.

				 The destructor of the class. Deletes all allocated resources.

*/
/*=================================================================== */
  ~ACS_CHB_ClientHandler();
/*=================================================================== */
/**
      @brief     Setup server to listen for incoming requests from clients.

      @return    ACS_CHB_returnType
  				 ACS_CHB_OK/ACS_CHB_FAIL

 */
/*=================================================================== */

  ACS_CHB_returnType initClientHandler();

/*=================================================================== */
/**
      @brief     Determines whether a request from Client is pending.

      @param     pending_request
			     Hold the number of pending client requests.

      @return    ACS_CHB_returnType
    			 ACS_CHB_OK/ACS_CHB_FAIL

 */
/*=================================================================== */

  ACS_CHB_returnType isRequestFromClientPending(int *pending_request);
/*=================================================================== */
/**
      @brief     Add new client to subscriber list.

      @return    void

 */
/*=================================================================== */

  void addNewClientToSubscriberList();
/*=================================================================== */
/**
      @brief     Convert incoming client requests into corresponding

      @return    void

 */
/*=================================================================== */
  void convertRequestsIntoActions();

/*=================================================================== */
/**
      @brief     Convert received request into subscription information.

      @param     requestFromClient
				 Buffer that holds the event subscriber
				 information to be converted.

      @return    void

 */
/*=================================================================== */
  void convertRequestIntoEventSubscription(
                                        ACS_CHB_Subscriber *requestFromClient);

/*=================================================================== */
/**
      @brief     Give back reply on the issued request

      @param     requestFromClient
  				 Buffer that holds the event subscriber
  				 information to be converted.

      @return    ACS_CHB_returnType
				 ACS_CHB_OK/ACS_CHB_FAIL

 */
/*=================================================================== */
  ACS_CHB_returnType 
  replyOnRequestToClient(ACS_CHB_Subscriber *requestFromClient);

/*=================================================================== */
/**
      @brief      Send information about updated CP object item values to clients.

      @param      cpObjectItem
    		      cp object item

      @param      cpObjectValue
    			  cp object value

      @return     ACS_CHB_returnType
				  ACS_CHB_OK/ACS_CHB_FAIL

 */
/*=================================================================== */
  ACS_CHB_returnType 
  sendInfoAboutUpdatedObjectItemToSubscribers(char cpObjectItem[],
					      char cpObjectValue[]);

/*=================================================================== */
/**
      @brief     Disconnect all client sessions.

      @return    void

 */
/*=================================================================== */
  void disconnectAllClients();

public:
/*===================================================================
                        PUBLIC ATTRIBUTE
=================================================================== */
/*=================================================================== */
/**
      @brief   pointer to internal object

               Head of the list of possible subscribers.
 */
/*=================================================================== */

        ACS_CHB_Subscriber *headSubscriber;

/*=====================================================================
                       PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================
                        PRIVATE ATTRIBUTE
 =================================================================== */
/*=================================================================== */
/**
      @brief   pointer to internal object
               Class responsible to handle low
               level I/O towards the clients
 */
/*=================================================================== */
        ACS_CHB_Receive *headReceive;

/*=================================================================== */
/**
      @brief   listenSocket
               Socket used for communication with
       	       new clients.
 */
/*=================================================================== */
	ACE_HANDLE listenSocket;

/*=================================================================== */
/**
     @brief   requestBuffer
	          Buffer for temporary storage of
              requests from Clients.
*/
/*=================================================================== */
        char requestBuffer[ACS_CHB_LARGE_BUFSIZE];         

};

#endif

