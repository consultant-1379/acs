/*=================================================================== */
/**
   @file   acs_chb_clock_comm.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the ACS_CHB_ClockComm class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.

==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_CLOCKCOMM_H
#define ACS_CHB_CLOCKCOMM_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <ACS_JTP.h>
#include <acs_chb_clock_def.h>
#include <acs_chb_event.h>
#include "acs_chb_common.h"

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief JEXDATAREQ_FAILED
 */
/*=================================================================== */


const int JEXDATAREQ_FAILED                             = 1<<0; // Bit flags for checking event replication

/*=================================================================== */
/**
   @brief JEXDATAIND_FAILED
 */
/*=================================================================== */


const int JEXDATAIND_FAILED                             = 1<<1; // Bit flags for checking event replication

/*=================================================================== */
/**
   @brief REMINDER_TIME_S
 */
/*=================================================================== */


const ACE_UINT32 REMINDER_TIME_S = 600; //Remind of unresolved events every 10 minutes
//struct string15 {char str [16];};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @class     ACS_CHB_ClockComm

      @brief     This class implements the clock communication interface for
		 the AP. The tasks for this classis to connect to the clock
		 synchronizer application in the CP and to read the date and
		 time at regular intervals.

 */
/*=================================================================== */

class ACS_CHB_ClockComm : public ACS_CHB_Event
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
              @brief     Constructor for ACS_CHB_ClockComm class.

                             Constructor for the Clock Synchronizer communication class.

        */
        /*=================================================================== */
  	ACS_CHB_ClockComm ();

	/*=====================================================================
                                CLASS DESTRUCTOR
  	==================================================================== */
  	/*=================================================================== */
  	/**
        	@brief     Destructor for ACS_CHB_ClockComm class.

                                   Destructor for the Clock Synchronizer communication class.

  	*/
  	/*=================================================================== */
  	virtual ~ACS_CHB_ClockComm ();

	/*=================================================================== */
  	/**
        	@brief     Read parameters from the parameter handler.

		@param	   aOmHandler 

        	@return    ACS_CC_ReturnType
                	           ACS_CC_SUCCESS/ACS_CC_FAILURE

   	*/
  	/*=================================================================== */
  	ACS_CC_ReturnType getParameters (OmHandler &aOmHandler);

 	/*=================================================================== */
        /**
                @brief     This module reads the CP clock time.

                @param     timebuf
			   Holds the CP time.

		@param	   successiveCommFailures

                @return    bool
				TRUE	=	Success
				FALSE   =	Failure

        */
        /*=================================================================== */

	bool data2(unsigned char* timebuf, int successiveCommFailures);

        /*=================================================================== */
        /**
                @brief     This module initiates the connection to fetch the CP time.

                @return    bool
                                TRUE    =       Success
                                FALSE   =       Failure

        */
        /*=================================================================== */

  	bool initiate2(int);

	/*=================================================================== */
        /**
                @brief     This module terminates the JTP communication

                @return    bool
                                TRUE    =       Success
                                FALSE   =       Failure

        */
        /*=================================================================== */

        bool terminate();


private:

  	/*===================================================================
   	                       PRIVATE ATTRIBUTES
   	=================================================================== */
	/*=================================================================== */
  	/**
        	@brief   ACS_JTP_Conversation object to initiate the JTP connection.

   	*/
  	/*=================================================================== */

	ACS_JTP_Conversation* myInitiator; 

	/*=================================================================== */
        /**
                @brief   Variables used in the JTP Conversation

        */
        /*=================================================================== */

	unsigned short	U1, U2, Result; 

public:
        /*===================================================================
                               PUBLIC  ATTRIBUTES
        =================================================================== */
        /*=================================================================== */
        /**
                @brief   Time between two requests to synchronize the AP clock.

        */
        /*=================================================================== */

	unsigned short	cycletime;	

	/*=================================================================== */
        /**
                @brief   Reminder flags.

        */
        /*=================================================================== */
					
	unsigned long	reminderFlags;

};

#endif
