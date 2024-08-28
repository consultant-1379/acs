/*=================================================================== */
   /**
   @file acs_chb_thread_jtp_comm.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_thread_jtp_comm.h module

   @version 1.0.0
	*/
	/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   PA1       13/01/2011   XNADNAR   Initial Release
   PA2		 31-Aug-2012  XCSSATA   Mapping of CNI 109 22-APZ 212 55/1-988 to APG/L
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Event_Handler.h>
#include <ace/Synch.h>
#include <ace/Thread_Manager.h>
#include <ace/OS_NS_time.h>
#include <iostream>
#include <ACS_TRA_trace.h>
#include "acs_chb_request_handler.h"
#include "acs_chb_thread_jtp_comm.h"
#include <ACS_JTP.h>
#include "acs_chb_heartbeat_def.h"
#include "acs_chb_heartbeat_comm.h"
#include <acs_chb_mtzclass.h>
#include "acs_chb_common.h"
#include <acs_prc_api.h>
#include <ACS_APGCC_CommonLib.h>
#include <acs_chb_tra.h>
#include  <string>
#include <sys/eventfd.h>


using namespace std;

/*===================================================================
                        GLOBAL DECLARATION
=================================================================== */
extern ACS_CHB_RequestHandler *requestHandler;
const char* rec_text = "REC:";
const char* sent_text = "SENT:";
const char* data1_text = "DATA1:";
const char* data2_text = " DATA2:";
const char* buflen_text = " BUFLEN:";
const char* buf_text = " BUF:";
const char* ending_text = "]\n";
const char* slash = "/";

/*===================================================================
   ROUTINE: ACS_CHB_Thread_JTPcomm
=================================================================== */
//  Constructor of the class, no parameters given
ACS_CHB_Thread_JTPcomm::ACS_CHB_Thread_JTPcomm() : ThreadProcedure (),ACS_CHB_Event (ACS_CHB_processName),
hb_timestamp (0),
prev_hbtimestamp(0), //Mapping of CNI 109 22-APZ 212 55/1-988
time_elapsed (0),
MasterCPId (1001),
FORequestCount(0),
rec_length(0),
rec_data1(0),
rec_data2(0),
send_length(0),
send_data1(0),
send_data2(0),
send_result(0),
cp_conn(0),
hbtimeout(0),
hbinterval(0)
{
	rec_msg='\0';
	myConversation=0;
	memset(bFirstTime,false,sizeof(bFirstTime));
	setMTZLib=false;
	memset(send_buffer,'\0',sizeof(send_buffer));
        memset(&CP_system_time,0,sizeof(CP_system_time));
	CPtime = 0;
	countMTZLib = 10;   // start directly to try set up MTZ directory.
	for (ACE_INT32 i = 0;i < ACS_CHB_NUMBER_OF_TZ;i++)
        {
                tmz_values[i].offset = 0;
                tmz_values[i].detail.timezone_info = 0;
                tmz_values[i].detail.dir = 0;
                tmz_values[i].detail.dlsp = 0;
                // initialise counter for application log.
                // Always report first incident.
                iReported[i] = ACS_CHB_NumberOfTimes;
                bFirstTime[i] = true;
        } // End of for-loop
	m_StopEventHand = eventfd(0,0);
	if(m_StopEventHand == -1)
	{
		ERROR(1,"%s","Failed to create stop event handle");
	}

	
} // End of constructor

/*===================================================================
   ROUTINE: ACS_CHB_Thread_JTPcomm
=================================================================== */
//  Constructor of the class, parameters given
ACS_CHB_Thread_JTPcomm::ACS_CHB_Thread_JTPcomm(ACE_UINT16 timeout, ACE_UINT32 interval, ACE_UINT16 cpid, ACS_JTP_Conversation* conv) :
ThreadProcedure(),
ACS_CHB_Event(ACS_CHB_processName),
hb_timestamp (0),
prev_hbtimestamp(0), //Mapping of CNI 109 22-APZ 212 55/1-988
time_elapsed (0),
MasterCPId (1001),
FORequestCount(0),
rec_length(0),
rec_data1(0),
rec_data2(0),
send_length(0),
send_data1(0),
send_data2(0),
send_result(0),
cp_conn(0)
{
	// set security permissions on the acsdata\MTZ directory.
	setMTZLib = true;
	countMTZLib = 10;   // start directly to try set up MTZ directory.
	rec_msg ='\0';
	myConversation = conv;
    	hbtimeout = timeout;
    	hbinterval = interval;
    	MasterCPId = cpid;
	
    	DEBUG(1,"Executing ACS_CHB_Thread_JTPcomm constructor - MasterCP Id: %u", MasterCPId);

	for (ACE_INT32 i = 0;i < ACS_CHB_NUMBER_OF_TZ;i++)
	{
		tmz_values[i].offset = 0;
		tmz_values[i].detail.timezone_info = 0;
		tmz_values[i].detail.dir = 0; 
		tmz_values[i].detail.dlsp = 0;
		// initialise counter for application log.
		// Always report first incident.	
		iReported[i] = ACS_CHB_NumberOfTimes;		
		bFirstTime[i] = true;
	} // End of for-loop
	
	m_StopEventHand = eventfd(0,0);
	if(m_StopEventHand == -1)
	{
		ERROR(1,"%s","Failed to create stop event handle");
	}
	//HeartBeat_timestamp(); // Mapping of CNI 109 22-APZ 212 55/1-988
	CPtime = -1;  
	memset(send_buffer,'\0',sizeof(send_buffer));
	memset(&CP_system_time,0,sizeof(CP_system_time));
} // End of constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_Thread_JTPcomm
=================================================================== */
ACS_CHB_Thread_JTPcomm::~ACS_CHB_Thread_JTPcomm(void)
{

	DEBUG(1,"%s","Executing ACS_CHB_Thread_JTPcomm Destructor");

	if (m_StopEventHand != -1)
	{
		ACE_OS::close(m_StopEventHand);
	}

}//End of Destructor

void ACS_CHB_Thread_JTPcomm::stop()
{
	DEBUG(1,"%s","ACS_CHB_Tread_JTPcomm::stop called!");
	// Signal the Stop event
        ACE_UINT64 stopEvent=1;
	ssize_t numByte = 0;
	if (m_StopEventHand != -1)
        {
		numByte = ACE_OS::write(m_StopEventHand, &stopEvent, sizeof(ACE_UINT64));
	}

        if(sizeof(ACE_UINT64) != numByte)
        {
		ERROR(1,"%s","ACS_CHB_Thread_JTPcomm::stop, signalling stop event failed ");
        }
}
/*===================================================================
   ROUTINE: operator()
=================================================================== */
int ACS_CHB_Thread_JTPcomm::operator()(void * parameters)
{
	(void)parameters;
	ACE_HANDLE PollFd = ACE_INVALID_HANDLE;

	//Needed for event reporting
	ACE_TCHAR problemData [PROBLEMDATASIZE];   // Data for event report.
 	ACE_TCHAR stReason [PROBLEMDATASIZE];

 	//Node for JTP communication
	ACS_JTP_Conversation::JTP_Node node;  // Structure node with CP systemId.

	//CP Object data.
	ACE_TCHAR cpObjectItem[ACS_CHB_SMALL_BUFSIZE];
	ACE_TCHAR cpObjectValue[ACS_CHB_SMALL_BUFSIZE];

	DEBUG(1, "%s", "New call received!");

	DEBUG(1,"%s","Executing operator for Threads");

	ACE_OS::memset(send_buffer, 0, sizeof(send_buffer));

	cp_conn = ACS_CHB_NOT_CONNECTED;           // CHB not connected to CP.

	DEBUG(1,"myConversation->State():%d", myConversation->State());

	// Try to initiate.   
	if (myConversation->State() != ACS_JTP_Conversation::StateConnected)
	{
		DEBUG(1, "%s", "JTP StateConnected failed");

        // Unable to connect to APJTP. Conversation stare is not connected.
		// Write a message in the Eventlog.
		ACE_OS::sprintf(problemData,"%s","APJTP State not connected. `");
		event(ACS_CHB_StatenotConnected,
			ACS_CHB_EVENT_STG,
			ACS_CHB_Cause_APfault,
			problemData,
			ACS_CHB_Text_ProblemAPJTP);

		delete myConversation;
		myConversation = 0;
		ACE_OS::close(m_StopEventHand);
		return -1;        // The Thread is closed.
	}

	// OK, we are Connected, send jexinitind
	if (myConversation->jexinitind(node, rec_data1, rec_data2) == false )
	{
		DEBUG(1, "%s", "jexinitind failed");
        	delete myConversation;
        	myConversation = 0;
		ACE_OS::close(m_StopEventHand);
		return -1;	        // The Thread is closed.							
	}

	if (node.system_id == MasterCPId)  // CP Master
	{
		initiate(); // Initiate a connection towards the CP. Get jexinitrsp data for Master CP.
		DEBUG(1,"%s", "JTP connection initiated!");
	}
	else
	{
		send_data1 = 0x02;      // Get jexinitrsp data for not Master CP.
		send_data2 = 0;
		send_result = 0;
		DEBUG(1,"%s", "CP not Master. HB stopped.");
		DEBUG(1,"systemId = %d", node.system_id);
	}

	// Send jexinitrsp 
	if (myConversation->jexinitrsp(send_data1, send_data2, send_result) == false)
	{
		DEBUG(1,"%s", "jexinitrsp failed");
		delete myConversation;
		myConversation = 0;
		ACE_OS::close(m_StopEventHand);	
	    	return -1;        // The Thread is closed.	
	}

	PollFd = myConversation->getHandle();

	DEBUG(1,"%s", "Connected - Get data!");

	// Loop: Receive/Receive the data
	while(myConversation->State() == ACS_JTP_Conversation::StateWaitForData )
	{
		// Wait until our handle is signaled

		 //Create a FD set.
		ACE_Time_Value tv;
		tv.sec(hbinterval+30);
		fd_set readHandleSet;
		FD_ZERO(&readHandleSet);
		FD_SET (PollFd, &readHandleSet);
		FD_SET(m_StopEventHand,&readHandleSet);
		int maxFd = (PollFd > m_StopEventHand)?PollFd:m_StopEventHand;

		int ret = ACE_OS::select ((int)(maxFd+1), &readHandleSet, 0, 0, &tv);

		//Check for timeout.
		if (ret == 0)
		{
			// HeartBeat TimeOut !
			// No data within time limit.
			// Send msg to eventmanager.
			DEBUG(1,"%s", "HeartBeat - JTP timeout");

			event(ACS_CHB_ConnectionLostJTP,
				ACS_CHB_EVENT_STG,
				ACS_CHB_Cause_APfault,
				"JTP connection - HeartBeat Time Out",
				ACS_CHB_Text_ProblemJTP);

			send_data1 = send_data2 = send_result = 0;
			myConversation->jexdiscreq(send_data1, send_data2, send_result);

			// We have to give this connection up. We must have a completely 
			// new connection in order to get things running (or exit?)
			delete myConversation;
			myConversation = 0;
			ACE_OS::close(m_StopEventHand);
			return -1;        // The Thread is closed.
		}
		if(FD_ISSET(m_StopEventHand,&readHandleSet))
		{
			DEBUG(1,"%s", "ACS_CHB_Thread_JTPcomm::operator(), stop handle sigaled");
			send_data1 = send_data2 = send_result = 0;
                        myConversation->jexdiscreq(send_data1, send_data2, send_result);

                        delete myConversation;
                        myConversation = 0;
			ACE_OS::close(m_StopEventHand);
                        return -1;        // The Thread is closed.
		}
		// Make sure we are in StateWaitForData before using jexdataind
		if(myConversation->State() != ACS_JTP_Conversation::StateWaitForData)
		{
			DEBUG(1,"%s", "JTP StateWaitForData failed");
			IssueJTPevent(-2);
			delete myConversation;
			myConversation =0;
			ACE_OS::close(m_StopEventHand);
			return -1;        // The Thread is closed.   
		}

		// If CP is not Master, CP starts disconnection. No HB is received.
		if (node.system_id != MasterCPId)
		{
			if (myConversation->jexdiscind(send_data1,send_data2,send_result) == false) 
			{
				delete myConversation;
				myConversation =0;
				ACE_OS::close(m_StopEventHand);
				return -1;       // The Thread is closed.   
			}

			DEBUG(1,"%s", "Not a master CP. CP is Disconnected. No HB  with it");
			delete myConversation;
			myConversation =0;
			ACE_OS::close(m_StopEventHand);
		    	return -1;       // The Thread is closed.
		}
		else                                // CP is Master. Start receiving HB data.
		{          
			// Get the Data from CP
			if ( myConversation->jexdataind(rec_data1, rec_data2, rec_length, rec_msg) == false)
			{
				DEBUG(1,"%s","HB with Master CP is initiated but jexdataind failed");
				IssueJTPevent(-2);
				delete myConversation;
				myConversation = 0;
				ACE_OS::close(m_StopEventHand);
				return -1;        // The Thread is closed.
			}
		}

		// Take care of msg from/to JTP
		// Moving from Sleep tp waitforsingleobject.
		int dwreceiveStatus = receiveMsg();
		if(dwreceiveStatus == 0) 
		{	
			// We are disconnected. Inform Eventmanager and update CP object status.
	        ACE_OS::strcpy(cpObjectItem,"CP_connection");
	        ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");

			if(requestHandler -> sendRequestToEventManager(cpObjectItem,cpObjectValue) == false) 
			{
				sprintf(stReason,"%s","pipe Closed to Child Process.");
				sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
				event(ACS_CHB_FatalErr,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					problemData,
					ACS_CHB_Text_ProcTerm);
			}
			
			send_data1 = send_data2 = send_result = 0;
			myConversation->jexdiscreq(send_data1, send_data2, send_result);
			delete myConversation;
			myConversation =0;
			ACE_OS::close(m_StopEventHand);
			return -1;        // The Thread is closed.
		}

		// Make sure we are in StateJobRunning before using jexdatareq
		if(myConversation->State() != ACS_JTP_Conversation::StateJobRunning)
		{
			DEBUG(1,"%s", "JTP StateJobRunning failed");

			// We are disconnected. Inform Eventmanager and update CP object status.
	        ACE_OS::strcpy(cpObjectItem,"CP_connection");
	        ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");

			if(requestHandler -> sendRequestToEventManager(cpObjectItem,cpObjectValue) == false) 
			{
				ACE_OS::sprintf(stReason,"%s","Connection Closed with Child Process.");
				ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
				event(ACS_CHB_FatalErr,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					problemData,
					ACS_CHB_Text_ProcTerm);

				DEBUG(1,"sendRequestToEventManager ERROR. %s", stReason);
			}
			
			IssueJTPevent(-2);
		    	delete myConversation;
		    	myConversation =0;
			ACE_OS::close(m_StopEventHand);
			return -1;       // The Thread is closed.
		}

		// Send reply to CP
		if ( myConversation->jexdatareq(send_data1, send_data2, send_length, send_buffer) == false)
		{
			DEBUG(1,"%s", "jexdatareq failed");

			// We are disconnected. Inform Eventmanager and update CP object status.
	        	ACE_OS::strcpy(cpObjectItem,"CP_connection");
	        	ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");

			if(requestHandler -> sendRequestToEventManager(cpObjectItem,cpObjectValue) == false) 
			{
				ACE_OS::sprintf(stReason,"%s","Connection Closed with Child Process.");
				ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
				event(ACS_CHB_FatalErr,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					problemData,
					ACS_CHB_Text_ProcTerm);
			}
			
			IssueJTPevent(-2);
			delete myConversation;
			myConversation = 0;
			ACE_OS::close(m_StopEventHand);
			return -1;       // The Thread is closed.       
		} 
		else
		{
		  DEBUG(1,"%s","SENT jexdatareq TO THE CP");
		}
	} // End while(StateWaitForData...
	// 
	// At this point we do not know if we are disconnected but we know
	// that we want an error code and do a disconnect.
	//
	// Make sure we are in StateJobRunning or StateWaitForData
	if(myConversation->State() == ACS_JTP_Conversation::StateJobRunning ||
		myConversation->State() == ACS_JTP_Conversation::StateWaitForData ) 
	{
		send_data1 = send_data2 = send_result = 0;
		if(myConversation->jexdiscreq(send_data1,send_data2,send_result) == false)
		{
			// At this point we realize that we have been disconnected		
			// JexDiscInd is the only way to get an error code from JTP
			send_data1 = send_data2 = send_result = 0;
			if (myConversation->jexdiscind(send_data1,send_data2,send_result) == false) 
			{
				// jexdiscind failed.

               	// We are disconnected. Inform Eventmanager and update CP object status.
				ACE_OS::strcpy(cpObjectItem,"CP_connection");
				ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");

				if(requestHandler -> sendRequestToEventManager(cpObjectItem,cpObjectValue) == false) 
				{
					ACE_OS::sprintf(stReason,"%s","Connection Closed with Child Process.");
					ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
					event(ACS_CHB_FatalErr,
						ACS_CHB_EVENT_STG,
						ACS_CHB_Cause_APfault,
						problemData,
						ACS_CHB_Text_ProcTerm);
				}
				
				IssueJTPevent(-1);
				delete myConversation;
				myConversation =0;
				ACE_OS::close(m_StopEventHand);
			    	return -1;       // The Thread is closed.   
			}
			else 
			{
				// We are disconnected. Inform Eventmanager and update CP object status.
				ACE_OS::strcpy(cpObjectItem,"CP_connection");
				ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");

				if(requestHandler -> sendRequestToEventManager(cpObjectItem,cpObjectValue) == false) 
				{
					ACE_OS::sprintf(stReason,"%s","Connection Closed with Child Process.");
					ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
					event(ACS_CHB_FatalErr,
						ACS_CHB_EVENT_STG,
						ACS_CHB_Cause_APfault,
						problemData,
						ACS_CHB_Text_ProcTerm);
				}
				
				IssueJTPevent(send_result);
				delete myConversation;
				myConversation =0;
				ACE_OS::close(m_StopEventHand);
			    	return -1;       // The Thread is closed.   
			} // End else {
		} // End if(myConversation.jexdiscreq
	} // End if(myConversation.State() ==

	// We are disconnected. Inform Eventmanager and update CP object status.
	ACE_OS::strcpy(cpObjectItem,"CP_connection");
	ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");

	if(requestHandler -> sendRequestToEventManager(cpObjectItem,cpObjectValue) == false) 
	{
		ACE_OS::sprintf(stReason,"%s","Connection Closed with Child Process.");
		ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
		event(ACS_CHB_FatalErr,
			ACS_CHB_EVENT_STG,
			ACS_CHB_Cause_APfault,
			problemData,
			ACS_CHB_Text_ProcTerm);
	}

    	delete myConversation;
    	myConversation = 0;
	ACE_OS::close(m_StopEventHand);
	return -1;
}//End of function operator()

/*===================================================================
   ROUTINE: receiveMsg
=================================================================== */

ACE_UINT32
ACS_CHB_Thread_JTPcomm::receiveMsg()
{
	ACE_TCHAR problemData [PROBLEMDATASIZE];
	ACE_TCHAR problemText [PROBLEMDATASIZE];

	ACE_TCHAR objOfRef[200];
	ACE_TCHAR cpObjectItem[ACS_CHB_SMALL_BUFSIZE];
	ACE_TCHAR cpObjectValue[ACS_CHB_SMALL_BUFSIZE];
	ACE_INT16 tmz_mismatches = 0;
	ACE_INT16 tmz_matches = 0;
	ACE_UINT16 tmz_mismatch[ACS_CHB_NUMBER_OF_TZ];
	ACE_UINT16 tmz_match[ACS_CHB_NUMBER_OF_TZ];
	for (ACE_INT32 i = 0;i < ACS_CHB_NUMBER_OF_TZ;i++)
	{
		tmz_mismatch[i] = 0;
		tmz_match[i] = 0;
	}

	// Trace point.

	DEBUG(1,"%s","Executing receiveMsg");

	log_JTP_data((ACE_TCHAR *)rec_msg, rec_length, rec_data1, rec_data2, 0);

	HeartBeat_timestamp(); // take a time stamp, used for SDT measuring.
	prev_hbtimestamp = hb_timestamp; // store the previous time // // Mapping of CNI 109 22-APZ 212 55/1-988
	
	// set security on MTZ directory!
	if( setMTZLib ) 
	{
		if( countMTZLib == 10 )
		{
			setMTZLib = setupMTZLib();
			countMTZLib = 0;
		}
		else
		{
			++countMTZLib;
		}
	}
	
	// H E A R T B E A T  S I G N A L
	if (rec_data1 == ACS_CHB_HEARTBEAT)
	{

		DEBUG(1,"%s","RECEIVED: HEART BEAT SIGNAL");

		if (cp_conn == ACS_CHB_NOT_CONNECTED) 
		{
			cp_conn = ACS_CHB_CONNECTED;
			ACE_OS::strcpy(cpObjectItem,"CP_connection");
			ACE_OS::strcpy(cpObjectValue,"CONNECTED");

			if(requestHandler -> sendRequestToEventManager(cpObjectItem, cpObjectValue) == 0)
			{
				 return 0;  // WriteFile fails.
			}
		}
		// every heart beat with version identifier 'data2=#CA0n', where n = 1, 2 or 3     uablowe
		// is a pure heartbeat. n is the status of the other node. 0 = NO ECHO,  1 = FAULTY, 2 = OK.
		// this version identifier is only received by the passive node. The node is only taking action if
		//  n = 1, i. e. the active node is faulty.

		// TR HG60673 CHB initiates failover during function change.
		// CHB is made less sensitive by causing failover to occur not on 
		// first occurence of 0xCA01 but on second occurence of 0xCA01.
		
		if (rec_data2 != 0xCA01 )
        {
			FORequestCount = 0;
        } 

		if (rec_data2 == 0xCA01)
		{

			// FORequestCount is used to Count 0xCA01. 
			// if two successive 0xCA01 are obtained then it means that passsive node will do failover 
			// else if one 0xCA01 is obtained then passive node does not do failover.

			// Make sure that we are in passive state!
			if (checkNodeState() == 0) 
			{
				if ( FORequestCount == 1 ) 				               			            
				{
				    // active node faulty - initiate FAILOVER 
				    ACE_OS::strcpy(problemData,
					    "Active node linkstatus is faulty for second time. Initiating Failover!"); 
				    event (ACS_CHB_ActiveNodeFaulty,
							ACS_CHB_EVENT_STG,
							ACS_CHB_Cause_APfault,
							problemData, 
							ACS_CHB_Text_ProblemNode);

					FORequestCount = 0;
				}			
				else
				{
				   ACE_OS::strcpy(problemData,
					    "Active node linkstatus is faulty for first time. It is a warning, Failover is not initiated."); 
			       event (ACS_CHB_ActiveNodeFaultyWarning,
					    ACS_CHB_EVENT_STG,
					    ACS_CHB_Cause_APfault,
					    problemData, 
					    ACS_CHB_Text_ProblemNode);

				   FORequestCount=1;
				}
			}
		}
		// every heart beat with version identifier 'data2=#CC0n', where n = 1, 2 or 3     uablowe
		// contains a time zone table and a CP timestamp in the buffer. Regarding n, see above
		// this version identifier is only received by the active node - uablowe
		// the active node is not taking any action, regardless of the passive node's state - uablowe
		if (rec_data2 == 0xCC00 || rec_data2 == 0xCC01 || rec_data2 == 0xCC02)  
		{
			if (rec_data2 == 0xCC01)
			{
				// Passive node is faulty
				// We repeat this message every 10th heartbeat.
				
				ACE_OS::strcpy(problemData,
					"Passive node link status is faulty.");
				event (ACS_CHB_PassiveNodeFaulty,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					problemData, 
					ACS_CHB_Text_ProblemNode);				
			}
			
			// unpack the buffer data received from the CP
			unpack_tmz_data();

			log_unpacked_tmz_data((ACE_TCHAR *)rec_msg);
			
			DEBUG(1,"after unpack_tmz_data CP system time %d/%d/%d %d:%d:%d:%d:%d:%d:%lu:%s", CP_system_time.tm_year, CP_system_time.tm_mon, CP_system_time.tm_mday, CP_system_time.tm_hour, CP_system_time.tm_min, CP_system_time.tm_sec,CP_system_time.tm_wday,CP_system_time.tm_yday,CP_system_time.tm_isdst,CP_system_time.tm_gmtoff,CP_system_time.tm_zone);
			struct tm* local_time;
			ACS_CHB_mtzclass mtz;
			ACE_OS::memset(&CPtime, 0, sizeof(CPtime));
			//TR_HT72009 Changes Begin
			CPtime = StructTime_to_sec(CP_system_time);
			if (CPtime == -1)
			{
				ACE_OS::snprintf(problemData, sizeof(problemData) - 1, "StructTime_to_sec returned -1. Probably illegal CP-time: %d/%d/%d %d:%d:%d", (CP_system_time.tm_year+1900), CP_system_time.tm_mon, CP_system_time.tm_mday, CP_system_time.tm_hour, CP_system_time.tm_min, CP_system_time.tm_sec);
				event(ACS_CHB_MTZlinkMissing,
						ACS_CHB_EVENT_STG,
						ACS_CHB_Cause_APfault,
						problemData, 
						ACS_CHB_Text_UndefLink);
			}
			//TR_HT72009 Changes End
			else 
			{
				struct tm temp_CP_system_time;
				for (ACE_UINT16 tmz_cat=0; tmz_cat < ACS_CHB_NUMBER_OF_TZ; tmz_cat++)
				{
					DEBUG(1,"For TimeZone : %d",tmz_cat);
					ACE_OS::memset(&temp_CP_system_time, 0, sizeof(temp_CP_system_time));	
					temp_CP_system_time = CP_system_time;
					if (tmz_values[tmz_cat].detail.timezone_info == 0)
					{ // Timezone information is not defined. Assume match
						tmz_match[tmz_matches] = tmz_cat;
						tmz_matches++;
						continue;
					}
					
					// request the local time for a certain time zone
					ACE_OS::memset(&local_time, 0, sizeof(local_time));
					local_time = mtz.makeSystemTimeConversion(&temp_CP_system_time,
						tmz_values[tmz_cat].detail.dlsp, tmz_cat);

					if (local_time)
					{
						// check that the offset is the same in CP and AP
						DEBUG(1,"local_time : %d :%d:%d:%d:%d:%d:%d:%d:%d:%lu:%s",local_time->tm_year,local_time->tm_mon,local_time->tm_mday,local_time->tm_hour,local_time->tm_min,local_time->tm_sec,local_time->tm_wday,local_time->tm_yday,local_time->tm_isdst,local_time->tm_gmtoff,local_time->tm_zone);
						if (check_tmz_offset(local_time,tmz_cat) == 1) // Not equal
						{
							tmz_mismatch[tmz_mismatches] = tmz_cat;
							tmz_mismatches++;
						}
						else // The offset is the same in the CP and the AP
						{
							tmz_match[tmz_matches] = tmz_cat;
							tmz_matches++;
						}
					}
					else // Not possible to obtain local time
					{
						DEBUG(1,"[receiveMsg()] TIME ZONE NOT LINKED IN AP: TZ #%d - offset: %d - timezone_info: %d - dir: %d - dlsp: %d", tmz_cat, tmz_values[tmz_cat].offset, tmz_values[tmz_cat].detail.timezone_info, tmz_values[tmz_cat].detail.dir, tmz_values[tmz_cat].detail.dlsp);

						// Filter the number of times this is reported to the 
						// eventlog
						if( iReported[tmz_matches] == ACS_CHB_NumberOfTimes ) 
						{
							if( bFirstTime[tmz_matches] ) 
							{
								// Format problemData
								ACE_OS::sprintf (problemData,"%s%s",
								ACS_CHB_Text_UndefLink,". Event situation repeated 0 times since last sent event");
							}
							else
							{
								ACE_OS::sprintf (problemData,"%s%s%d%s",
								ACS_CHB_Text_UndefLink,". Event situation repeated ", ACS_CHB_NumberOfTimes,
									" times since last sent event"  ); 
							}
							// Format Obj of Reference
							ACE_OS::sprintf (objOfRef,"%s%d", "TMZ/",tmz_cat);
							// Format problemText
							ACE_OS::sprintf (problemText, "%s", " AP TIMEZONE LINK MISSING");
							event(ACS_CHB_MTZlinkMissing,
								ACS_CHB_EVENT_STG,
								ACS_CHB_Cause_APfault,
								problemData,
								problemText,
								objOfRef);
							iReported[tmz_matches] = 0;
							bFirstTime[tmz_matches] = false;
						}
						else 
						{

							DEBUG(1,"[receiveMsg()] NO EVENT FOR TIME ZONE #%d NOT LINKED IN AP", tmz_cat);
							++iReported[tmz_matches];
						}
						tmz_match[tmz_matches] = tmz_cat;
						tmz_matches++;
					}
				} // End of for-loop

				log_tmz_result(tmz_mismatches,tmz_mismatch,
				tmz_matches,tmz_match);
			} // End if (CPtime==-1) else 
		} // End if (rec_data2 == 0xABCD)
	} // End if (rec_data1 == ACS_CHB_HEARTBEAT)
	// C P  O B J E C T  D A T A
	else 
	{
		DEBUG(1,"%s","RECIEVED: CP object request");

		// Check if CP object change pending.
		if (requestHandler -> isRequestFromCPPending(rec_data1,
														rec_data2,
														(unsigned char *)rec_msg,
														rec_length) == ACS_CHB_OK)
		{
			// Request pending, decode and check if available in list.
			if (requestHandler -> decodeCPObjectMessage(cpObjectItem, cpObjectValue) == ACS_CHB_OK)
			{
				if (!ACE_OS::strcmp(cpObjectItem, "EX_Exchange_identity"))
				{
					updateExchangeFile(cpObjectValue);
				}

				// Send event to the event manager.
				if(requestHandler -> sendRequestToEventManager(cpObjectItem, cpObjectValue) == 0)
				{
					return 0;
				}

			}
		}
	}
	
	data(tmz_mismatches,tmz_mismatch,
		tmz_matches,tmz_match);
	
	return 1;
} // End of receiveMsg

/*===================================================================
   ROUTINE: HeartBeat_timestamp
=================================================================== */

void ACS_CHB_Thread_JTPcomm::HeartBeat_timestamp (void)
{
	// Heart beat received, take a time stamp (hb_timestamp) and
	// calculate elapsed time (time_elapsed) since last message.
	time_t hbtime = 0;
	time(&hbtime);
	if (hb_timestamp !=0) {
		time_elapsed = hbtime - hb_timestamp;
	}
	else
	{
		if(prev_hbtimestamp == 0) // // Mapping of CNI 109 22-APZ 212 55/1-988
		{
			// first time after object is created
			time_elapsed = 0;
		}
		else // // Mapping of CNI 109 22-APZ 212 55/1-988
		{
			// calculate elapsed time from previous heart beat time stamp
			time_elapsed = hbtime - prev_hbtimestamp;
		}
	}
	hb_timestamp = hbtime;
} // End of HeartBeat_timestamp

/*===================================================================
   ROUTINE: log_JTP_data
=================================================================== */

void ACS_CHB_Thread_JTPcomm::log_JTP_data(ACE_TCHAR * bptr,
											ACE_INT32 buflen,
											ACE_INT32 data1,
											ACE_INT32 data2,
											ACE_INT32 type)
{
	ACE_TCHAR tmp[1024];
	ACE_TCHAR tmp1[100];

	ACE_OS::strcpy(tmp,"[");
	if (type == 0) 
	{
		ACE_OS::strcat(tmp,rec_text);
	}
	else
	{
		ACE_OS::strcat(tmp,sent_text);
	}
	ACE_OS::strcat(tmp,data1_text);
	ACE_OS::sprintf(tmp1,"%x",data1);
	ACE_OS::strcat(tmp,tmp1);
	ACE_OS::strcat(tmp,data2_text);
	ACE_OS::sprintf(tmp1,"%x",data2);
	ACE_OS::strcat(tmp,tmp1);
	ACE_OS::strcat(tmp,buflen_text);
	ACE_OS::sprintf(tmp1,"%x",buflen);
	ACE_OS::strcat(tmp,tmp1);
	ACE_OS::strcat(tmp,buf_text);
	for (int ii = 0; ii < buflen;++ii)
	{
		ACE_OS::sprintf(tmp1,"%x%s",*(bptr + ii) & 0xff,"-");
		ACE_OS::strcat(tmp,tmp1);
	} // End of for-loop
	
	ACE_OS::strcat(tmp,ending_text);

	DEBUG(1,"%s",tmp);
} // End of log_JTP_data

/*===================================================================
   ROUTINE: checkNodeState
=================================================================== */
ACE_INT32 ACS_CHB_Thread_JTPcomm::checkNodeState(void)
{
	ACE_UINT16 nodeState = 0;
	ACE_UINT16 ret;
	ACS_PRC_API prcObj;

    ret = prcObj.askForNodeState();

    switch(ret)
    {
		case 1: DEBUG(1,"%s","Node is Active");
				nodeState=1; 
				break;

		case 2: DEBUG(1,"%s","Node is Passive");
				nodeState=0; 
				break;

		default:DEBUG(1,"%s","Failed to get the state of the Node");
				nodeState=-1; 
				break;
    }
    return nodeState;
}//End of checkNodeState

/*===================================================================
   ROUTINE: unpack_tmz_data
=================================================================== */

void ACS_CHB_Thread_JTPcomm::unpack_tmz_data ()
{
	// The received buffer contains the following data:
	// rec_buffer[0]     = year
	// rec_buffer[1]     = month
	// rec_buffer[2]     = day
	// rec_buffer[3]     = hour
	// rec_buffer[4]     = minute
	// rec_buffer[5]     = sec
	// rec_buffer[6]     = spare
	// rec_buffer[7]     = spare
	// rec_buffer[8-9]   = TimeZone 0
	// rec_buffer[10-11] = TimeZone 1
	// . . .
	// rec_buffer[52-53] = TimeZone 22
	// rec_buffer[54-55] = TimeZone 23
	

	//In CP, month is from 1-12
	//Year is only no of years in that century.
	//For example, if the year is 2011, then CP will send year as 
	//2011- 2000(current century).
	ACE_OS::memset(&CP_system_time, 0 ,sizeof(CP_system_time));	
	unsigned char* bufptr = (unsigned char*) rec_msg;
	CP_system_time.tm_year= (int)*(bufptr+0);
	if(CP_system_time.tm_year < 70)
  	{
		CP_system_time.tm_year = CP_system_time.tm_year + 100;
  	}
	CP_system_time.tm_mon = ((int) *(bufptr+1)) - 1;
	CP_system_time.tm_mday = (int)*(bufptr+2);
	CP_system_time.tm_hour = (int)*(bufptr+3);
	CP_system_time.tm_min = (int)*(bufptr+4);
	CP_system_time.tm_sec = (int)*(bufptr+5);
	CP_system_time.tm_isdst = (int)((*(bufptr+9) & 0x40) >>6);

	struct tm temp_CPtime ;
	ACE_OS::memset(&temp_CPtime, 0, sizeof(temp_CPtime));
	temp_CPtime = CP_system_time;

	ACE_OS::mktime(&temp_CPtime);
	CP_system_time.tm_wday = temp_CPtime.tm_wday;
	CP_system_time.tm_yday = temp_CPtime.tm_yday;
	CP_system_time.tm_gmtoff =temp_CPtime.tm_gmtoff;
	CP_system_time.tm_zone = temp_CPtime.tm_zone;
	bufptr = bufptr + 8;
	for (int i = 0; i < ACS_CHB_NUMBER_OF_TZ;i++)
	{
		// Offset + detail is stored in two bytes per TimeZone.
		// Byte n = offset LSB
		// Byte n+1, bit 0-3 = offset MSB
		// Byte n+1, bit 4 = timezone info (0: not defined, 1: defined)
		// Byte n+1, bit 5 = offset dir (0:forward, 1:backward)
		// Byte n+1, bit 6 = DST (0:passive, 1:active)
		// Byte n+1, bit 7 = spare
		int offsetLSB = (int)(*(bufptr));
		int offsetMSB = (int)(((int)(*(bufptr+1)&0x0f))<<8);
		tmz_values[i].offset= offsetLSB + offsetMSB ;
		tmz_values[i].detail.timezone_info=(*(bufptr+1) & 0x10) >>4;
		tmz_values[i].detail.dir=(*(bufptr+1) & 0x20) >>5;
		tmz_values[i].detail.dlsp=(*(bufptr+1) & 0x40) >>6;
		bufptr = bufptr + 2;
		DEBUG(1,"[unpack_tmz_data()] TZ #%d - offset: %d - timezone_info: %d - dir: %d - dlsp: %d", i, tmz_values[i].offset, tmz_values[i].detail.timezone_info, tmz_values[i].detail.dir, tmz_values[i].detail.dlsp);

	} // End of for-loop

	
} // End  of unpack_tmz_data

/*===================================================================
   ROUTINE: log_unpacked_tmz_data
=================================================================== */

void ACS_CHB_Thread_JTPcomm::log_unpacked_tmz_data(ACE_TCHAR* bptr)
{
	ACE_TCHAR tmp[1024];
	ACE_TCHAR tmp1[100];
	ACE_INT32 ii;
	
	ACE_OS::strcpy(tmp,"[MTZ DATA:");
	for (ii = 0; ii < rec_length;++ii) 
	{
		ACE_OS::sprintf(tmp1,"%x%s",*(bptr + ii) & 0xff,"-");
		ACE_OS::strcat(tmp,tmp1);
	} // End of for-loop
	
	ACE_OS::strcat(tmp,ending_text);
	DEBUG(1,"%s",tmp);
} // End of log_unpacked_tmz_data

/*===================================================================
   ROUTINE: check_tmz_offset
=================================================================== */

ACE_INT16 ACS_CHB_Thread_JTPcomm::check_tmz_offset (struct tm* local_time,
                                                    int tmz_cat)
{
	// Fix for TR HC56948
	// To avoid that mktime compensates for isdst in TMZ = 0
	// setup tm_isdst to correct value.
	// Solve this by doing a first call to mktime
	// and copy isdst to the local_time struct.
	
	struct tm temp_local ;
	ACE_OS::memset(&temp_local, 0, sizeof(temp_local));
	temp_local = *local_time;

	ACE_OS::mktime(&temp_local);


	local_time->tm_isdst = temp_local.tm_isdst;

	time_t local ;
	ACE_OS::memset(&local, 0, sizeof(local));
	if (tmz_cat == 0)
		local = ACE_OS::mktime(local_time);
	else
		local = StructTime_to_sec(*local_time);

	DEBUG(1, "CPtime = %lu, localTime = %lu",CPtime,local);
	
	if (local > CPtime)
	{
		DEBUG(1,"%s","local>CPtime");
		if (tmz_values[tmz_cat].detail.dir == 1) // backward?
		{
			DEBUG(1,"%s","local>CPtime & backward->mismatch");
			//return 1; // mismatch	//TR HX99538
		}
	}
	else
	{ 
		if (CPtime > local)
		{
			DEBUG(1,"%s","CPtime > local");
			if (tmz_values[tmz_cat].detail.dir == 0) // forward?
			{
				DEBUG(1,"%s","forward->mismatch");
				//return 1; // mismatch	//TR HX99538
			}
		}
	}
	int offset_sec = (int)difftime(CPtime, local);
	int offset_min = offset_sec/60;
	offset_min = abs(offset_min);
	DEBUG(1,"offset_min : %d",offset_min);
//	if ( (tmz_cat >0) &&(tmz_values[tmz_cat].detail.dlsp || tmz_values[0].detail.dlsp) && (abs(offset_min - tmz_values[tmz_cat].offset) == 60) )
	if ( (tmz_values[tmz_cat].detail.dlsp) && (abs(offset_min - tmz_values[tmz_cat].offset) == 60) )	//TR HX99538
	{
    	DEBUG(1,"%s","Do not raise the alarm due to DST");
    	return 0;
	}
           
	DEBUG(1,"tmz_values[tmz_cat].offset : %d",tmz_values[tmz_cat].offset);
	if (tmz_values[tmz_cat].offset != offset_min)
	{
		//		if (tmz_values[tmz_cat].offset != offset_min + 60)//Do not raise alarm if it a DST shift
		//		{
		//			if (tmz_values[tmz_cat].offset != offset_min - 60)
		//			{
		//				DEBUG(1,"%s","check_tmz_offset,Mismatch Found,May not be a DST case");
		//				cout<<"check_tmz_offset,Mismatch Found,May not be a DST case"<<endl;
		//				return 1;
		//			}
		//		}

		return 1;
		  // mismatch
	}
	
	return 0; // Equal
} // End of check_tmz_offset

/*===================================================================
   ROUTINE: updateExchangeFile
=================================================================== */

void ACS_CHB_Thread_JTPcomm::updateExchangeFile(char cpObjectValue[])
{

	ACE_TCHAR lpExchangeFile[FILE_MAX_PATH] ;
	const char* CHBDir ="chb";
	const char* FileName = "exchangefile.txt";
	FILE *chbExchangeFile = NULL;

	// The Files name.
	// acsdata position
	char acsdata[200] = {0};
	ACE_Time_Value tv(0, 10000);
	int returnValue = false;
	if( getacsdata(acsdata, sizeof(acsdata) - 1) )
	{
		// we have a position for MTZ data
		// Check for existence, try 1 time before assuming passive node.
		// If on passive node we will loop until we get active
		int  LoopNumber = 5;
		// Assume we can't access the Datadisk.

		while (LoopNumber)
		{
			ACE_stat stat_buff;
			if(( ACE_OS::stat(acsdata, &stat_buff)) == 0 )
			{
				returnValue = true;
				break;
			}
			ACE_OS::sleep(tv);
			--LoopNumber;
		}
	}
	else
	{
		char   problemData[200];
		ACE_OS::sprintf(problemData, "Data Disk Path not found in database.");
		ACS_CHB_Common::CHB_serviceError(problemData , ACE_OS::last_error());
		DEBUG(1,"%s",problemData);
		return ;// no registry available.
	}

	if( returnValue == true )
	{

		ACE_OS::strcpy(lpExchangeFile,acsdata);

		ACE_OS::strcat(lpExchangeFile, slash);
		ACE_OS::strcat(lpExchangeFile, CHBDir );

		//Check whether the directory is there or not.If not, then create it,

		ACE_stat statBuff;
		if( ACE_OS::stat(lpExchangeFile, &statBuff)  != 0 )
		{
			// chb directory is not present.
			DEBUG(1, "%s", "CHB directory is not present, creating it now.");
			ACE_OS::mkdir(ACE_TEXT(lpExchangeFile), S_IRWXU | S_IRWXG );
		}

		ACE_OS::strcat(lpExchangeFile, slash);
		ACE_OS::strcat(lpExchangeFile, FileName);

		chbExchangeFile = ACE_OS::fopen(lpExchangeFile, "w");

		if(chbExchangeFile == NULL)
		{
			ACE_TCHAR szData[50];
			ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
					ACE_TEXT("Error: Failed to open /data/acs/data/chb/exchangefile.txt file :%d"),
					errno);

			event(ACS_CHB_SERVICEERROR,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					szData,
					"CHB_serviceError");
			ERROR(1,"%s",szData);
		}

		else
		{
			fprintf(chbExchangeFile, "%s", cpObjectValue);
		}
	}

	if(chbExchangeFile != NULL)
	{
		fclose(chbExchangeFile);
	}

} 	// End of updateExchangeFile

/*===================================================================
   ROUTINE: data
=================================================================== */

void ACS_CHB_Thread_JTPcomm::data (ACE_INT16 mismatches,ACE_UINT16* mismatch,
									ACE_INT16 matches,ACE_UINT16* match)
{
	
	DEBUG(1,"%s","Executing ACS_CHB_Thread_JTPcomm::data()");
	
	// Send data
	
	send_data1 = 0;
	send_length = 0;
	
	// Check if we are the active node
	if (checkNodeState() == 1)
	{
		send_data2 = 0xC100; // Active Node !
	}
	else
	{
		send_data2 = 0xC000; // Passive Node !
	}

	if (rec_data1 == ACS_CHB_HEARTBEAT)
	{
		// if timezone data has been received and that we are the active node
		if ((rec_data2 == 0xCC00 || rec_data2 == 0xCC01 || rec_data2 == 0xCC02) && (send_data2 == 0xC100))  
		{
			// version identifier for time zones
			send_length = ACS_CHB_NUMBER_OF_TZ+3;
			// The data to send is a buffer with the following format:
			// [0]   = Number of mismatched TZ
			// [1]   = First mismatched TZ
			// [2]   = Second mismatched TZ
			// . . .
			// [n]   = Last mismatched TZ
			// [n+1] = Number of matched TZ
			// [n+2] = First matched TZ
			// [n+3] = Second matched TZ
			// . . .
			// [m]   = Last matched TZ
			// [26]  = 0
			send_buffer[0] = (char) mismatches;			// type cast OK
			for (int i = 0; i < mismatches; i++)
			{
				send_buffer[i+1] = (char) mismatch[i];	// type cast OK
			}
			send_buffer[mismatches+1] = (char) matches;	// type cast OK
			for (int j = 0; j < matches; j++)
			{
				send_buffer[j+2+mismatches] = (char) match[j]; // type cast OK
			}
			send_buffer[26] = 0;
		}
	}
	if (time_elapsed > 0)
	{
		ACE_UINT32 highword = 0;
		ACE_UINT32 lowword=0;
		if (time_elapsed > 0xFFFF)
		{
			highword = time_elapsed >> 16;
			lowword = time_elapsed & 0xFFFF;
		} 
		else {
			lowword = time_elapsed;
		}
		highword = highword & 0x00FF;    // truncate to one Byte
		send_data1 = (ushort) lowword;					 // type cast OK
		send_data2 = (ushort) (send_data2 | highword);	 // type cast OK
	}

		log_JTP_data((ACE_TCHAR*)send_buffer, send_length, send_data1, send_data2,1);
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s #mismatches: %u - #matches: %u", ((mismatches > 0) ? "MISMATCHES FOUND" : "MISMATCHES NOT FOUND"), mismatches, matches);
		DEBUG(1,"%s",tracep);

	return;
} // End of data

/*===================================================================
   ROUTINE: initiate
=================================================================== */

void
ACS_CHB_Thread_JTPcomm::initiate ()
{
	ACE_UINT16 nodeStat;   // uablowe

	DEBUG(1,"%s","Executing ACS_CHB_Thread_JTPcomm::initiate ()");
	nodeStat = checkNodeState();      // Check status of node - uablowe

	// Trace point
	
	DEBUG(1,"Executing `jexinitrsp' with nodeState: %u; hbtimeout: %u", nodeStat, hbtimeout);

	if (nodeStat == 1)                
	{
		nodeStat = nodeStat << 12;         
	}
	
	nodeStat = (ACE_UINT16) hbinterval | nodeStat;   // uablowe
	
	// Initiate a session
	send_data1 = nodeStat;		// uablowe
	send_data2 = hbtimeout;		// uablowe
	send_result = 0;
	
	return;
} // End of initiate

/*===================================================================
   ROUTINE: log_tmz_result
=================================================================== */

void ACS_CHB_Thread_JTPcomm::log_tmz_result(ACE_INT16 mismatches,ACE_UINT16* mismatch,
											ACE_INT16 matches,ACE_UINT16* match)
{
	char tmp[1024];
	char tmp1[100];
	int ii;
	
	ACE_OS::strcpy(tmp,"[MTZ RESULT:mis=");
	ACE_OS::sprintf(tmp1,"%d:",mismatches);
	ACE_OS::strcat(tmp,tmp1);
	for (ii = 0; ii < mismatches;++ii) 
	{
		ACE_OS::sprintf(tmp1,"%x,",*(mismatch + ii) & 0xff);
		ACE_OS::strcat(tmp,tmp1);
	} // End of for-loop
	ACE_OS::sprintf(tmp1,"match=%d:",matches);
	ACE_OS::strcat(tmp,tmp1);
	for (ii = 0; ii < matches;++ii) 
	{
		ACE_OS::sprintf(tmp1,"%x,",*(match + ii) & 0xff);
		ACE_OS::strcat(tmp,tmp1);
	} // End of for-loop
	
	ACE_OS::strcat(tmp,"]\n");
	
	DEBUG(1,"%s",tmp);
} // End of log_tmz_result


/*===================================================================
   ROUTINE: setupMTZLib
=================================================================== */
bool ACS_CHB_Thread_JTPcomm::setupMTZLib(void)
{
	if  ( checkNodeState() == 1 )
	{
		// The Directory name where the .cfg file will be stored.
		const char* MTZDir ="mtz";
		// The Files name.
		const char* FileName = "ACS_TZ_links.cfg";

		// acsdata position
		char mtzdata[200] = {0};
		char mtzfile[200] = {0};
		ACE_Time_Value tv(0, 10000);
		int returnValue = false;

		if(getacsdata(mtzdata, sizeof(mtzdata) - 1) )
		{
			// we have a position for MTZ data
			// Check for existence, try 1 time before assuming passive node.
			// If on passive node we will loop until we get active
			int  LoopNumber = 5;
			// Assume we can't access the Datadisk.

			while (LoopNumber)
			{
				ACE_stat stat_buff;
				if(( ACE_OS::stat(mtzdata, &stat_buff)) == 0 )
				{
					returnValue = true;
					break;
				}
				ACE_OS::sleep(tv);
				--LoopNumber;
			}
		}
		else
		{
			char   problemData[200];
			ACE_OS::sprintf(problemData, "Data Disk Path not found in database.");
			ACS_CHB_Common::CHB_serviceError(problemData , ACE_OS::last_error());
			DEBUG(1,"%s",problemData);
			return false ;// no registry available.
		}

		if( returnValue == true )
		{
			// There is a directory to set!
			// Check for MTZDir
			ACE_OS::strcpy(mtzfile,mtzdata);
			ACE_OS::strcat(mtzfile, "/");
			ACE_OS::strcat(mtzfile, MTZDir );
			ACE_OS::strcat(mtzfile, "/");
			ACE_OS::strcat(mtzfile, FileName);

			ACE_stat stat_buff;

			if(ACE_OS::stat(mtzfile, &stat_buff) != 0)
			{
				char  problemData[200];
				ACE_OS::sprintf(problemData, "%s File not present on the data disk.", FileName);
				ACS_CHB_Common::CHB_serviceError(problemData, ACE_OS::last_error());
				DEBUG(1,"%s",problemData);
				return false;
			}
			else
			{
				return true ;
			}
		}
		else
		{
			// assume passive node
			return true;
		}
	}
	return true;

} // end setupMTZLib



/*===================================================================
   ROUTINE: getacsdata
=================================================================== */
bool ACS_CHB_Thread_JTPcomm::getacsdata(char* pChar, size_t pLen)
{
	ACS_APGCC_CommonLib oComLib;
	char*  szPath = new char[FILE_MAX_PATH];
	int dwLen = FILE_MAX_PATH;
	int retCode = 0;
	const char * szLogName = "ACS_DATA";
	retCode = oComLib.GetDataDiskPath(szLogName,szPath,dwLen);

	if (retCode != ACS_APGCC_DNFPATH_SUCCESS)
	{
		delete[] szPath;
		szPath = 0;

		return false;
	}
	else
	{
		ACE_OS::snprintf(pChar, pLen, "%s", szPath);
		delete[] szPath;
		szPath = 0;
	}
	
	return true;
}//End of getacsdata

/*===================================================================
   ROUTINE: IssueJTPevent
=================================================================== */
void ACS_CHB_Thread_JTPcomm::IssueJTPevent(int code)
{
	char problemData [PROBLEMDATASIZE];

	switch (code) 
	{
	case -2:
		ACE_OS::sprintf(problemData,"%s","CP Connection Broken. Reconnecting...`");
		break;
	case -1:
		ACE_OS::sprintf(problemData,"%s%s","jexdiscind failed. `","CP Connection Broken. Reconnecting...`");
		break;
	case 0:
		ACE_OS::sprintf(problemData,"%s%s","Disconnected from CP. `","Reconnecting...`");
		break;
	case 1:
		ACE_OS::sprintf(problemData,"%s%s","Congestion in JTP. `","CP Connection Broken. Reconnecting...`");
		break;
	case 2:
		ACE_OS::sprintf(problemData,"%s%s","Breakdown on transport or lower layer. `","CP Connection Broken. Reconnecting...`");
		break;
	case 3:
		ACE_OS::sprintf(problemData,"%s%s","Application not available. `","CP Connection Broken. Reconnecting...`");
		break;
	default:
		// APMA broke the connection.
		ACE_OS::sprintf(problemData,"%s%d%s","disc code from application: `",send_result," CP Connection Broken. Reconnecting...`");
		break;
	}
	
	event(	ACS_CHB_DiscIndJTP,
		ACS_CHB_EVENT_STG, 
		ACS_CHB_Cause_APfault,
		problemData, 
		ACS_CHB_Text_ProblemJTP);
		
} // End of IssueJTPevent


//TR_HT72009 Changes Begin
/*===================================================================
   ROUTINE: StructTime_to_sec
=================================================================== */
time_t  ACS_CHB_Thread_JTPcomm::StructTime_to_sec(struct tm st)
{
		int leapyear=0,year=0,days=0;
		time_t sec(NULL),newsec(NULL),leapsec(NULL);
		sec = 31536000;
		leapsec = 86400;
        struct tm timeInfo;
        ACE_OS::memset(&timeInfo,0, sizeof(timeInfo));
        if(st.tm_year !=0)
        {
        	timeInfo.tm_year       =       st.tm_year+1900;
        	timeInfo.tm_mon        =       st.tm_mon+1;
        	timeInfo.tm_wday       =       st.tm_wday;
        	timeInfo.tm_mday       =       st.tm_mday;
        	timeInfo.tm_hour       =       st.tm_hour;
        	timeInfo.tm_min        =       st.tm_min;
        	timeInfo.tm_sec        =       st.tm_sec;
        	leapyear = ((timeInfo.tm_year-1968)/4);
        	year = timeInfo.tm_year-1970;
        	if (!(timeInfo.tm_year%4) &&  timeInfo.tm_mon <= 2)
        		leapyear = leapyear - 1;
        	switch (timeInfo.tm_mon-1)
        	{
                case 1:
                        days = 31;
                        break;
                case 2:
                        days = 59;
                        break;
                case 3:
                        days = 90;
                        break;
                case 4:
                        days = 120;
                        break;
                case 5:
                        days = 151;
                        break;
                case 6:
                        days = 181;
                        break;
                case 7:
                        days = 212;
                        break;
                case 8:
                        days = 243;
                        break;
                case 9:
                        days = 273;
                        break;
                case 10:
                        days = 304;
                        break;
                case 11:
                        days = 334;
                        break;
        	}
        	DEBUG(1, "leapyearCount = %d, year = %d ,days = %d",leapyear,year,days);
        	DEBUG(1, " %d : %d : %d : %d : %d : %d ",timeInfo.tm_year,timeInfo.tm_mon,timeInfo.tm_mday,timeInfo.tm_hour,timeInfo.tm_min,timeInfo.tm_sec );
        	newsec = (sec * year) + (leapsec * leapyear) + ((((((days +(timeInfo.tm_mday - 1)) * 24) + timeInfo.tm_hour) * 60) + timeInfo.tm_min) * 60) + timeInfo.tm_sec;
        }
        return newsec;
}
//TR_HT72009 Changes End
