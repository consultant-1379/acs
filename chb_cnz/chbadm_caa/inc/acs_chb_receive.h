
/*=================================================================== */
/**
   @file   acs_chb_receive.h

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
   N/A       25/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_RECEIVE_H 
#define ACS_CHB_RECEIVE_H

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief MAX_NUMBER_OF_CLIENT_CONNECTIONS
		  Maximum number of allowed
          clients to this server.
 */
/*=================================================================== */

const int MAX_NUMBER_OF_CLIENT_CONNECTIONS = 50;
                                                // Maximum number of allowed
                                                // clients to this server.
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_chb_subscriber.h"
#include <acs_chb_cp_object.h>
#include "acs_chb_error.h"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
 	@brief	ACS_CHB_Receive
 */
/*===================================================================*/

class ACS_CHB_Receive: public virtual ACS_CHB_Error
{
	/*=====================================================================
		                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
		                       CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	   /**

	      @brief           Constructor for ACS_CHB_Receive class.

	                       Default constructor, used to initialize variables..

	   */
	/*=================================================================== */
  ACS_CHB_Receive();

  /*=====================================================================
  		                       CLASS DESTRUCTOR
  	==================================================================== */
  	/*=================================================================== */
  	   /**

  	      @brief           The destructor of the class ACS_CHB_Receive.

  	   */
  	/*=================================================================== */
  ~ACS_CHB_Receive();

  /*=====================================================================*/
  /**
          @brief	Setup server handle communication with clients.

					ListenSocket is set to -1 if the method fails.

          @param	ListenSocket
					Socket used to receive requests from new
   				    clients that wants to register.

          @return	ACS_CHB_returnType
					ACS_CHB_OK/ACS_CHB_FAIL
  */
  /*=====================================================================*/
  ACS_CHB_returnType 
  initReceive(ACE_HANDLE *ListenSocket);

  /*=====================================================================*/
    /**
            @brief	Find out whether any client requests are pending.

            @param	PendingRequest
					Number of requests pending


            @param	ListenSocket
					Socket used to receive requests from new
   				    clients that wants to register.


            @param	headSubscriber
					Head of the subscriber list.

            @return	ACS_CHB_returnType
  					ACS_CHB_OK/ACS_CHB_FAIL
    */
    /*=====================================================================*/
  ACS_CHB_returnType
  isClientRequestPending(int *PendingRequest,
                         ACE_HANDLE ListenSocket,
                         ACS_CHB_Subscriber *headSubscriber);

  /*=====================================================================*/
    /**
            @brief	Send CP object value to client.


            @param	ClientSocket
  					Client socket in use

  			@param	COObjectValue
  					CP object value to be sent back to client.

            @return	ACS_CHB_returnType
  					ACS_CHB_OK/ACS_CHB_FAIL
    */
    /*=====================================================================*/
  ACS_CHB_returnType
  sendValueToClient(ACE_HANDLE ClientSocket,char *COObjectValue);

  /*=====================================================================*/
    /**
            @brief	Close all client sessions


            @param	headSubscriber
  					Head of the subscriber list.

            @return	void
    */
    /*=====================================================================*/
  void 
  closeAllClientSessions(ACS_CHB_Subscriber *headSubscriber);

  /*=====================================================================
  		                        PRIVATE DECLARATION SECTION
  	==================================================================== */
private:
  /*=====================================================================*/
    /**
            @brief	Add new client requests to subscriber list.

            @param	Socket
  					SSocket buffer to be used to find out.

  			@param	ListenSocket
  					Socket used to receive requests from new
   				    clients that wants to register.

  			@param	headSubscriber
  					Head of the subscriber list.

            @return	ACS_CHB_returnType
  					ACS_CHB_OK/ACS_CHB_FAIL
    */
    /*=====================================================================*/
  ACS_CHB_returnType
  addRequestToSubscriberList(struct pollfd *Socket,
			     ACE_HANDLE ListenSocket,
                             ACS_CHB_Subscriber *headSubscriber
			    );

  /*=====================================================================*/
    /**
            @brief	Calculate the current number of clients connected.

            @param	headSubscriber
  					Head of the client subscriber list.

            @return	number of connected clients.
    */
  /*=====================================================================*/
  int numberOfClients(ACS_CHB_Subscriber *headSubscriber);

  /*=====================================================================
   		                        STRUCT DECLARATION SECTION
   	==================================================================== */
  /*=====================================================================*/
      /**
              @struct  Socket
      */
    /*=====================================================================*/
  struct pollfd *Socket;          // Socket
};

#endif

