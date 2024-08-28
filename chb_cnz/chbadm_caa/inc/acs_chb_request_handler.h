/*=================================================================== */
/**
   @file   acs_chb_request_handler.h

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
   N/A       26/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_REQUEST_HANDLER_H 
#define ACS_CHB_REQUEST_HANDLER_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_chb_heartbeat_def.h"
#include "acs_chb_cp_objectlist.h"
#include "acs_chb_event.h"
#include <acs_chb_cp_object.h>
#include <ACS_DSD_Session.h>
/*====================================================================
                        FORWARD DECLARATION
==================================================================== */
class ACS_CHB_CPObjectList;
//class ACS_DSD_Session;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_RequestHandler

*/
/*=================================================================== */
class ACS_CHB_RequestHandler : virtual private ACS_CHB_CPObjectList,
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
	      @brief     Constructor for ACS_CHB_RequestHandler class.

		             Default constructor, used to initialize variables.

	*/
	/*=================================================================== */
  ACS_CHB_RequestHandler();
  /*=====================================================================
   	                        CLASS DESTRUCTOR
  ==================================================================== */
  /*=================================================================== */
  /**
        @brief     Destructor for ACS_CHB_RequestHandler class.

				   The destructor of the class.
   */
  /*=================================================================== */

  ~ACS_CHB_RequestHandler();
  /*=================================================================== */
    /**
          @brief     Returns ACS_CHB_OK if pipe is successfully setup else returns ACS_CHB_FAIL.

          @return    ACS_CHB_returnType
      			     ACS_CHB_OK/ACS_CHB_FAIL

     */
    /*=================================================================== */
  ACS_CHB_returnType 
  initPipeFromHeartBeat();

  /*=================================================================== */
      /**
            @brief     Returns ACS_CHB_OK if pipe is successfully setup else returns ACS_CHB_FAIL.

            @return    ACS_CHB_returnType
        			     ACS_CHB_OK/ACS_CHB_FAIL

       */
      /*=================================================================== */
  ACS_CHB_returnType 
  initPipeToEventManager();

  /*=================================================================== */
      /**
            @brief     Returns ACS_CHB_OK if requestCode != 0 else returns ACS_CHB_FAIL.

            @param     requestCode
					   Holds the information on which
					   request that was received from CP

            @return    ACS_CHB_returnType
        			   ACS_CHB_OK-if requestCode != 0
        			   ACS_CHB_FAIL-if requestCode == 0

       */
      /*=================================================================== */
  ACS_CHB_returnType
  isRequestFromHeartBeatPending(int *requestCode);

  /*=================================================================== */
      /**
            @brief     Report whether request from CP is pending or not.

            @param     requestFromCP
					   CP object request.

            @param     dataFromCP
					   CP object data.

            @param     requestBuffer
					   Buffer that holds additional data.

	    @param     bufflen
					   buffer length

            @return    ACS_CHB_returnType
        			     ACS_CHB_OK/ACS_CHB_FAIL

       */
      /*=================================================================== */
  ACS_CHB_returnType
  isRequestFromCPPending(unsigned int requestFromCP,
			 unsigned int dataFromCP,
			 unsigned char *requestBuffer,
                         unsigned int bufflen
			);

  /*=================================================================== */
      /**
            @brief     Extract item and value part of message into supplied buffers.

            @param     cpObjectItem
					   CP object item

            @param     cpObjectValue
					   CP object item value.

            @return    ACS_CHB_returnType
        			     ACS_CHB_OK/ACS_CHB_FAIL

       */
      /*=================================================================== */
  ACS_CHB_returnType
  decodeCPObjectMessage(char cpObjectItem[],char cpObjectValue[]);

  /*=================================================================== */
      /**
            @brief     Find out and update item with value that needs to be updated.

            @param     cpObjectItem
					   CP object item

            @param     cpObjectValue
					   CP object item value.

            @return    void

       */
      /*=================================================================== */
  void 
  searchForAndUpdateObjectItemInList(char cpObjectItem[],char cpObjectValue[]);

  /*=================================================================== */
      /**
            @brief     Search through object list and find out whether any item with updated
					   value exists.If found, return the current value.

            @param     cpObjectItem
					   CP object item

	    @param     cpObjectValue
					   CP object item value.

            @return    ACS_CHB_returnType
        			     ACS_CHB_OK/ACS_CHB_FAIL

       */
      /*=================================================================== */
  ACS_CHB_returnType
  findUpdatedObjectItemInList(char cpObjectItem[],char cpObjectValue[]);

  /*=================================================================== */
      /**
            @brief     Send request to event manager.

            @param     cpObjectItem
					   CP object item

	    @param     cpObjectValue
					   CP object item value.

            @return    ACS_CHB_returnType
        			     ACS_CHB_OK/ACS_CHB_FAIL

       */
      /*=================================================================== */
 ACS_CHB_returnType 
 sendRequestToEventManager(char cpObjectItem[],char cpObjectValue[]);
 /*===================================================================
                           PUBLIC ATTRIBUTE
    =================================================================== */
   /*=================================================================== */
   /**
         @brief   psSession

    */
   /*=================================================================== */

  ACS_DSD_Session *psSession;
  /*=================================================================== */
     /**
           @brief   pcSession

      */
     /*=================================================================== */
  ACS_DSD_Session *pcSession;
  /*=================================================================== */
     /**
           @brief   pchbClient

      */
     /*=================================================================== */
  ACS_DSD_Client  *pchbClient;
  /*=================================================================== */
     /**
           @brief   pchbServer

      */
     /*=================================================================== */
  ACS_DSD_Server *pchbServer;

private:


  /*=================================================================== */
      /**
            @brief     Decode request read from CP.

            @param     requestFromCP
					   CP object request.

	    @param     dataFromCP
					   CP object data.

	    @param     requestBuffer
					   Buffer that holds the request from CP.

	    @param     bufflen
					   buffer length

	    @param     cpObjectItem
					   Name of the CP object.

	    @param     cpObjectValue
					   Value of the CP object.

            @return    ACS_CHB_returnType
        			     ACS_CHB_OK/ACS_CHB_FAIL

       */
      /*=================================================================== */
  ACS_CHB_returnType
  decodeRequestFromCP(unsigned int requestFromCP,
		      unsigned int dataFromCP,
		      unsigned char requestBuffer[],
                      unsigned int bufflen,
		      char cpObjectItem[],
		      char cpObjectValue[]
		     );

  /*=================================================================== */
      /**
            @brief     Find out whether CP object item are in list and have a value change.

            @param     cpObjectItem
					   Name of incoming CP object.

	    @param     cpObjectValue
					   Value of incoming CP object.

	    @param     ptrCPObjectListItem
					   Pointer to start of the CP object list.

            @return    ACS_CHB_returnType
        			     ACS_CHB_OK/ACS_CHB_FAIL

       */
      /*=================================================================== */
  ACS_CHB_returnType
  isCPObjectInListAndValueChanged(ACS_CHB_CPObjectList *ptrCPObjectListItem,
				  char cpObjectItem[],char cpObjectValue[]);

  /*=================================================================== */
      /**
            @brief     Update CP object item link that corresponds to given input.

            @param     cpObjectItem
                       CP object item

            @param     cpObjectValue
		       CP object item value.

            @return    void

       */
      /*=================================================================== */
  void updateCPObjectItem(char cpObjectItem[],char cpObjectValue[]);


};

#endif

