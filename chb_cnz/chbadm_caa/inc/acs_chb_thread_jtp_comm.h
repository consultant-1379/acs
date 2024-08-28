/*=================================================================== */
/**
   @file   acs_chb_thread_jtp_comm.h

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
   N/A       13/01/2011   XNADNAR   Initial Release
==================================================================== */

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <time.h>
#include <iostream>
#include <ace/ACE.h>

#include "acs_chb_request_handler.h"
#include "acs_chb_thread_procedure.h"
#include "acs_chb_heartbeat_comm.h"
#include "acs_chb_heartbeat_def.h"
#include <acs_chb_cp_object.h>
#include <ACS_JTP.h>




/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ErrorWaitTime
		  Time in seconds to wait before
		  next attempt to connect
 */
/*=================================================================== */
const ACE_INT32 ErrorWaitTime = 20;

/*=================================================================== */
/**
   @brief HBFAULT
		  Fault detected in heart beat
		  application
 */
/*=================================================================== */
const ACE_INT16 HBFAULT = 4;

/*=====================================================================
                        STRUCTURE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
    @struct  tmz

    @brief   This struct stores offset and timezone details

    @par     offset
             offset of that timezone
	@par     details

*/
/*=================================================================== */

/*=====================================================================
                        INTERNAL STRUCTURE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
    @struct  details

    @brief   This struct stores all the details like timezone info,offset direction and day light saving

    @par     timezone_info
             timezone info, 0: not defined, 1: defined.
    @par     dir
             offset direction, 0: forward, 1: backward
	@par     dlsp
             day light saving period, 0: passive, 1: active
*/
/*=================================================================== */
struct tmz 
{
  ACE_INT16 offset;
  struct details 
  {
    unsigned timezone_info :1;
    unsigned dir :1;
    unsigned dlsp :1;
  }detail;
};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_Thread_JTPcomm

*/
/*=================================================================== */


class ACS_CHB_Thread_JTPcomm : public ThreadProcedure, public ACS_CHB_Event
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
	      @brief     Constructor for ACS_CHB_Thread_JTPcomm class.

		             Default constructor, used to initialize variables.
	*/
	/*=================================================================== */
	ACS_CHB_Thread_JTPcomm();

	/*=================================================================== */
	   /**

	      @brief           Constructor with parameters used to initialize variables.

	      @param           timeout
	                       HeartBeat timeout from IMM

	      @param           interval
	                       HeartBeat timeout from IMM

	      @param           cpid
	                       Master CPId from CS

	      @param           conv
	                       Pointer to ACS_JTP_Conversation

	      @return          void

	      @exception       none
	   */
	/*=================================================================== */

	ACS_CHB_Thread_JTPcomm(ACE_UINT16 timeout, ACE_UINT32 interval, ACE_UINT16 cpid, ACS_JTP_Conversation* conv);


	/*=====================================================================
	                        CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief     Destructor for ACS_CHB_Thread_JTPcomm class.

					 The destructor of the class.

	*/
	/*=================================================================== */
	virtual ~ACS_CHB_Thread_JTPcomm();

	/*=================================================================== */
	/**
		 @brief     operator overloading for ()

		 @param     parameters

		 @return    int

	 */
	/*=================================================================== */
	virtual int operator()(void * parameters);
	void stop();

	/*=================================================================== */
	  /**
	        @brief     Recieve msg from cp

	        @return    ACE_UINT32

	   */
	  /*=================================================================== */
	ACE_UINT32 receiveMsg ();

	/*=================================================================== */
		  /**
		        @brief     Initiate a connection towards the CP.

		        @return    void

		   */
		  /*=================================================================== */
	void initiate ();

	/*=================================================================== */
	  /**
	        @brief     Sends an error event to eventlog.
					   No events are repeated within 10 min

	        @return    void

	   */
	  /*=================================================================== */
	void IssueJTPevent(int code);

	/*=====================================================================
		                        PRIVATE DECLARATION SECTION
	==================================================================== */
 private:
	/*=================================================================== */
	  /**
	        @brief     Logs JTP data.

	        @param     bptr
					   pointer to msg.

		@param     buflen
					   buffer length

		@param     data1

		@param     data2

		@param     type

	        @return    void

	   */
	  /*=================================================================== */
	void log_JTP_data(ACE_TCHAR * bptr, ACE_INT32 buflen,ACE_INT32 data1,ACE_INT32 data2,ACE_INT32 type);

	/*=================================================================== */
	  /**
	        @brief     Make a timestamp, used for measuring elapsed time between
					   heartbeats.

	        @return    void

	   */
	  /*=================================================================== */
	void HeartBeat_timestamp (); 

	/*=================================================================== */
	  /**
	        @brief     This method checks the state of this node of the cluster.

	        @return    1 - This node is active.
					   0 - This node is NOT active.

	   */
	  /*=================================================================== */
	ACE_INT32 checkNodeState(void);

	/*=================================================================== */
	  /**
	        @brief     Unpack the buffer stored tmz data received from CP.

	        @return    void

	   */
	  /*=================================================================== */
	void unpack_tmz_data (); 

	/*=================================================================== */
	  /**
	        @brief     Logs unpacked time zone results.

	        @param     bptr

	        @return    void

	   */
	  /*=================================================================== */
	void log_unpacked_tmz_data(ACE_TCHAR* bptr);

	/*=================================================================== */
	  /**
	        @brief     Writes exchange header in file exchangefile.txt

	        @param     cpObjectValue

	        @return    void

	   */
	  /*=================================================================== */
	void updateExchangeFile(char cpObjectValue[]);

	/*=================================================================== */
	  /**
	        @brief     Replies data request from the CP

	        @param     mismatches
					   Number of mismatches found

		@param     mismatch
					   Pointer to array of TZ-values for mismatch

		@param     matches
					   Number of matches found

		@param     match
					   Pointer to array of TZ-values for match

	        @return    void

	   */
	  /*=================================================================== */
	void data (ACE_INT16 mismatches,ACE_UINT16* mismatch,
				ACE_INT16 matches,ACE_UINT16* match);

	/*=================================================================== */
	  /**
	        @brief     Logs time zone results.

	        @param     mismatches
					   number of mismatches

		@param     mismatch
					   pointer to mismatch

		@param     matches
					   number of matches

		@param     match
					   pointer to match

	        @return    void

	   */
	  /*=================================================================== */
	void log_tmz_result(ACE_INT16 mismatches,ACE_UINT16* mismatch,
			            ACE_INT16 matches,ACE_UINT16* match);

	/*=================================================================== */
	  /**
	        @brief     Read by commondll the ACS_DATA$ key

	        @param     pChar

		@param     pLen

	        @return    bool
					   true/false

	   */
	  /*=================================================================== */
	bool getacsdata(char* pChar, size_t pLen);

	/*=================================================================== */
	  /**
	        @brief     This method checks that the offset is the same in CP and AP.
					   The rule for 'the same offset' is that the difference between
					   the offset in the CP and the AP is less than 1 minute.

	        @param     local_time
					   Local time in the time-zone indicated by
	                   the tmz_category

	        @param     tmz_category
					   TimeZone value (0-23)

	        @return    0  - Offset is the same
					   1  -  Offset is not the same

	   */
	  /*=================================================================== */
	ACE_INT16 check_tmz_offset (struct tm* local_time,int tmz_category);

	/*=================================================================== */
	  /**
	        @brief     Sets the security permission on directory MTZ.

	        @return    bool
					   true/false

	   */
	  /*=================================================================== */
	bool setupMTZLib(void);

	/*===================================================================
	 * //TR_HT72009 Changes Begin
	=================================================================== */

	time_t  StructTime_to_sec(struct tm);

	/* //TR_HT72009 Changes End  */
	/*===================================================================
							PRIVATE ATTRIBUTE
	 =================================================================== */
	/*=================================================================== */
	/**
			 @brief   hb_timestamp
					  heart beat timestamp.

	 */
	/*=================================================================== */
	ACE_INT32           hb_timestamp;
	/*=================================================================== */
	/**
	 	 @brief   Previous heart beat timestamp.

	 */
	/*=================================================================== */

	ACE_INT32           prev_hbtimestamp;
	/*=================================================================== */
	/**
			 @brief   time_elapsed
					  time elapsed between two heartbeats

	 */
	/*=================================================================== */
	ACE_UINT32          time_elapsed;
	/*=================================================================== */
	/**
			 @brief   MasterCPId
					  CP Id for Clock Master.

	 */
	/*=================================================================== */
	ACE_UINT16      	MasterCPId;
	/*=================================================================== */
	/**
			 @brief   FORequestCount
					  stands for FailoverRequestCount, it counts the number of failover.

	 */
	/*=================================================================== */
	ACE_UINT32		 	FORequestCount;
	/*=================================================================== */
	/**
			 @brief   rec_length

	 */
	/*=================================================================== */
	ACE_UINT16			rec_length;
	/*=================================================================== */
	/**
			 @brief   rec_data1

	 */
	/*=================================================================== */
	ACE_UINT16			rec_data1;
	/*=================================================================== */
	/**
			 @brief   rec_data2

	 */
	/*=================================================================== */
	ACE_UINT16			rec_data2;
	/*=================================================================== */
	/**
			 @brief   send_length
					  buffer length.

	 */
	/*=================================================================== */
	ACE_UINT16             		send_length;
	/*=================================================================== */
	/**
			 @brief   send_data1

	 */
	/*=================================================================== */
	ACE_UINT16              	send_data1;
	/*=================================================================== */
	/**
			 @brief   send_data2

	 */
	/*=================================================================== */
	ACE_UINT16           		send_data2;
	/*=================================================================== */
	/**
			 @brief   myConversation

	 */
	/*=================================================================== */
	ACS_JTP_Conversation *    	myConversation;
	/*=================================================================== */
	/**
			 @brief   send_buffer
					  JTP buffer to send.

	 */
	/*=================================================================== */
	ACE_TCHAR			send_buffer[ACS_CHB_NUMBER_OF_TZ+3]; // JTP buffer to send.
	/*=================================================================== */
	/**
			 @brief   bFirstTime
					  specific for each timezone
					  A boolean telling if the service is started
					  the first time or if the event log has been written before this time.

	 */
	/*=================================================================== */

	bool				bFirstTime[ACS_CHB_NUMBER_OF_TZ];

	/*=================================================================== */
	/**
			 @brief   iReported
					  specific for each timezone.
					  a counter that contains the number of times
					  "Link missing between tmz and TZ" has occured
					  since it was written i the application log.

	 */
	/*=================================================================== */
	ACE_INT32			iReported[ACS_CHB_NUMBER_OF_TZ];


	/*=================================================================== */
	/**
			 @brief   countMTZLib
					  counter for how many times we wait before
					  tries to set up security for MTZ directory.

	 */
	/*=================================================================== */
	ACE_UINT32			countMTZLib;
	/*=================================================================== */
	/**
			 @brief   setMTZLib
					  intiates to true, set to false when MTZ directory
					  has correct security settings.

	 */
	/*=================================================================== */
	bool				setMTZLib;

	/*===================================================================
							STRUCT DECLARATION
	 =================================================================== */
	/*=================================================================== */
	/**
			 @brief   CP_system_time
					  CP time stamp used to check tmz.

	 */
	/*=================================================================== */
	struct tm           CP_system_time;
	/*=================================================================== */
	/**
			 @brief   tmz_values
					  specific data for
					  the 24 timezones.

	 */
	/*=================================================================== */
	struct tmz          tmz_values[ACS_CHB_NUMBER_OF_TZ];
	/*=================================================================== */
	/**
			 @brief   CPtime
					  CP time in absolute secs since 1970.

	 */
	/*=================================================================== */
	time_t              CPtime;
	/*=================================================================== */
	/**
			 @brief   send_result

	 */
	/*=================================================================== */
	ACE_UINT16			send_result;
	/*=================================================================== */
	/**
			 @brief   cp_conn
					  state of the connection to the CP

	 */
	/*=================================================================== */
	ACE_INT32           cp_conn;
	/*=================================================================== */
	/**
			 @brief   hbtimeout
					  Maximum time in seconds to wait
					  for a data indication.

	 */
	/*=================================================================== */
	ACE_UINT16          hbtimeout;
	/*=================================================================== */
	/**
			 @brief   hbinterval
					  Time in seconds between two
					  heart beat requests.

	 */
	/*=================================================================== */
	ACE_UINT32	hbinterval;
	/*=================================================================== */
		/**
		 @brief   rec_msg
				  received JTP buffer.

		 */
	/*=================================================================== */
	ACE_TCHAR *			rec_msg;

	int m_StopEventHand;
};
