/*=================================================================== */
/**
   @file   acs_chb_heartbeat_comm.h

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
   N/A       19/01/2011   XNADNAR   Initial Release
   N/A	     01/27/2015   XGOUMON   
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_HEARTBEATCOMM_H
#define ACS_CHB_HEARTBEATCOMM_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <time.h>
#include <iostream>
#include<ace/ACE.h>
#include<ace/Event.h>
#include<ace/Process.h>
#include <ACS_CC_Types.h>
#include <acs_apgcc_omhandler.h>

#include <ACS_JTP.h>             // uablowe
#include <acs_chb_event.h>
#include <acs_chb_heartbeat_def.h>
#include <ACS_CS_API.h>



/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ACS_CHB_TIMEFILTER
 */
/*=================================================================== */


const double ACS_CHB_TIMEFILTER = 60*5;		// 5 minutes between reports.

/*=================================================================== */
/**
   @brief Master CP ID
 */
/*=================================================================== */

#define CP_CLOCK_MASTER_ID	1001   //Master CP ID is always 1001 (SPX1) 

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_HeartBeatComm

 */
/*=================================================================== */


class ACS_CHB_HeartBeatComm : public ACS_CHB_Event
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
	      @brief     Constructor for ACS_CHB_HeartBeatComm class.

		             Constructor for the heart beat communication class.

	*/
	/*=================================================================== */
  ACS_CHB_HeartBeatComm ();
  /*=====================================================================
  	                        CLASS DESTRUCTOR
  ==================================================================== */
  /*=================================================================== */
  /**
        @brief     Destructor for ACS_CHB_HeartBeatComm class.

  				   Destructor for the heart beat communication class.

  */
  /*=================================================================== */
  virtual ~ACS_CHB_HeartBeatComm ();
  /*=================================================================== */
  /**
        @brief     Read parameters from IMM.

        @return    ACS_CC_ReturnType
		   ACS_CC_SUCCESS/ACS_CC_FAILURE
   */
  /*=================================================================== */

  ACS_CC_ReturnType getParameters ();

  /*=================================================================== */
    /**
          @brief     Forever loop.
					 Handling the JTP communication with the CP.
					 If Disconnected or Timed-out then the function
					 will try to reconnect.

          @param     phStopEvent

          @return    Fault return code:
					 0		No Error, normal exit.
					 1..98	JTP Error Codes.
					 99		Disconnected.

     */
    /*=================================================================== */
  ACE_UINT32 JTPcomm (ACE_HANDLE phStopEvent);

  /*=================================================================== */
      /**
            @brief     Sends an error event to eventlog in case of DSD error.

            @return    void

       */
      /*=================================================================== */
  void JTPEventDSDError();

  /*=====================================================================
                         PRIVATE DECLARATION SECTION
  ==================================================================== */
private:
	OmHandler theOmHandler;
  /*=================================================================== */
        /**
              @brief     Waits until one of the handles are signaled or the StopEvent
						 is signaled.

              @param     no
						 Number of handles in list.

			  @param     mixedHandles
						 list of handles.

              @return    1		A handle is signaled
						 -1		Stop Event is signaled

         */
        /*=================================================================== */
  int waitUntilSignaled(int no, const ACE_HANDLE *mixedHandles);

  // Comment
  //	If there is no connection within 10 minutes an event is generated.
  /*=================================================================== */
  /**
        @brief     Builds the application name string for JTP connection

        @return    pointer to string	success
                                           0					fail

   */
  /*=================================================================== */
  char * getAPapplName();

  /*===================================================================
                          PRIVATE ATTRIBUTE
   =================================================================== */
  /*=================================================================== */
  /**
        @brief   iEventReported

   */
  /*=================================================================== */
  int					iEventReported;
  /*=================================================================== */
    /**
          @brief   pStopEvent
				   Event handle to stop the JTP communication.
     */
    /*=================================================================== */

  ACE_HANDLE 	   pStopEvent;
  /*=================================================================== */
      /**
            @brief   hbtimeout
  				     Maximum time in seconds to wait
			         for a data indication.
       */
      /*=================================================================== */
  ACE_UINT32          hbtimeout;
  /*=================================================================== */
      /**
            @brief   hbinterval
  				     Time in seconds between two
			         heart beat requests.
       */
      /*=================================================================== */
  ACE_UINT32		    hbinterval;
  /*=================================================================== */
      /**
            @brief   apnodenumber
  				     Own AP node number.
       */
      /*=================================================================== */
  ACE_UINT32        apnodenumber;
  /*=================================================================== */
      /**
            @brief   tLastTimeEvent

       */
      /*=================================================================== */
  time_t				tLastTimeEvent;


};
#endif

