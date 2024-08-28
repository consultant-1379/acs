/*=================================================================== */
/**
   @file   acs_chb_subscriber.h

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

#ifndef ACS_CHB_SUBSCRIBER_H 
#define ACS_CHB_SUBSCRIBER_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/OS_NS_sys_socket.h>
#include <ace/Event.h>
#include <sys/socket.h>
#include "acs_chb_heartbeat_def.h"
#include "acs_chb_eventlist.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_Subscriber

*/
/*=================================================================== */
class ACS_CHB_Subscriber
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
	      @brief     Constructor for ACS_CHB_Subscriber class.

		             Default constructor, used to initialize variables.

	*/
	/*=================================================================== */
  ACS_CHB_Subscriber();
  
  /*=====================================================================
    	                        CLASS DESTRUCTOR
  ==================================================================== */
  /*=================================================================== */
  /**
         @brief     Destructor for ACS_CHB_Subscriber class.

   				    The destructor of the class. The object is removed from the
					list of objects and all resources are deleted.

    */
    /*=================================================================== */
  ~ACS_CHB_Subscriber();
  
  /*=================================================================== */
    /**
          @brief     Returns the status for pendig requests.

          @return    int
                     Pending requests

     */
    /*=================================================================== */
  int get_requestPending() const;

  /*=================================================================== */
    /**
          @brief     Sets the request status.

          @param     p
					 Number of requests pending

          @return    void

     */
    /*=================================================================== */
  void set_requestPending(int p);

  /*=================================================================== */
    /**
          @brief     Returns the handle for reports to client

          @return    fd
                     Socket for reports to client

     */
    /*=================================================================== */
  ACE_HANDLE get_sForReportToClient();

  /*=================================================================== */
    /**
          @brief     Sets the handle for reports to client

          @param     fd
					 Socket for reports to client

          @return    void

     */
    /*=================================================================== */
  void set_sForReportToClient(ACE_HANDLE fd);

  /*=================================================================== */
    /**
          @brief     Returns a pointer to the next object in the list of objects.

          @return    ACS_CHB_Subscriber
      			     0-No more objects
  				     *-Pointer to next object

     */
    /*=================================================================== */
  ACS_CHB_Subscriber* get_next();

  /*=================================================================== */
    /**
          @brief     Returns a pointer to the last object in the list of objects.

          @return    *  - Pointer to the last object

     */
    /*=================================================================== */
  ACS_CHB_Subscriber* get_last();

  /*=================================================================== */
    /**
          @brief     Appends a new object to the end of current list.

          @param     newElm
					 The new object to be added to the
                     end of the list

          @return    void

     */
    /*=================================================================== */
  void appendToList(ACS_CHB_Subscriber* newElm);

  /*=================================================================== */
    /**
          @brief     Returns the pointer to the request-buffer.

          @return    char* - Pointer to the request buffer

     */
    /*=================================================================== */
  char* get_requestBufferPtr();

  /*=================================================================== */
    /**
          @brief     Returns the pointer to the first object in the list of events.

          @return    ACS_CHB_EventList
      			      * - Pointer to the first event

     */
    /*=================================================================== */
  ACS_CHB_EventList* get_headEventList();

  /*=====================================================================
                          PRIVATE DECLARATION SECTION
   ==================================================================== */
private:
  /*===================================================================
                            PRIVATE ATTRIBUTE
     =================================================================== */
        /*=================================================================== */
        /**
              @brief   requestPending
  				       Indicates whether a request from
					   client is pending or not.
		 */
		/*=================================================================== */
        int requestPending;
        /*=================================================================== */
        /**
              @brief   sForReportToClient
        			   Handle where to send events
                       that client are subscribing for.
         */
        /*=================================================================== */
        ACE_HANDLE sForReportToClient;
        /*=================================================================== */
        /**
              @brief   requestBuffer
        			   Buffer for temporary storage of
                       requests from Client.
         */
        /*=================================================================== */
        char requestBuffer[ACS_CHB_SMALL_BUFSIZE];         
					// Buffer for temporary storage of
                                        // requests from Client.
        /*=================================================================== */
        /**
              @brief   headEventList
        			   List of event types  that client
                       wants to retrieve as soon as they arrives.
         */
        /*=================================================================== */
        ACS_CHB_EventList *headEventList;

        /*=================================================================== */
        /**
              @brief   Next
        			   Pointers to next elements.
         */
        /*=================================================================== */
        ACS_CHB_Subscriber *Next;
        /*=================================================================== */
        /**
              @brief   Pre
             		   Pointers to previous elements.
         */
        /*=================================================================== */
        ACS_CHB_Subscriber *Pre;
};

#endif

