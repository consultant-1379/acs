/*=================================================================== */
/**
  @file acs_chb_heartbeat_comm.cpp

  Class method implementationn for CHB module.

  This module contains the implementation of class declared in
  the acs_chb_heartbeat_comm.h module

  @version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       19/01/2011   XNADNAR   Initial Release
 */
/*=================================================================== */

/*===================================================================
  INCLUDE DECLARATION SECTION
  =================================================================== */

#include <iostream>
#include <ace/ACE.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Event_Handler.h>
#include <ace/Synch.h>
#include <ace/Thread_Manager.h>
#include <ACS_JTP.h>
#include <time.h>
#include <ACS_DSD_Client.h>
#include <acs_chb_common.h>
#include <acs_chb_request_handler.h>
#include <acs_chb_heartbeat_comm.h>
#include <acs_chb_heartbeat_def.h>
#include <acs_chb_client_handler.h>
#include <acs_chb_mtzclass.h>
#include <acs_chb_thread_jtp_comm.h>
#include <acs_chb_threadpool.h>
#include <acs_chb_tra.h>

using namespace std;


/*=====================================================================
  GLOBAL METHOD
  ==================================================================== */
/*=================================================================== */
/**
  @brief      requestHandler

 */
/*=================================================================== */
extern ACS_CHB_RequestHandler *requestHandler;


/*===================================================================
ROUTINE: ACS_CHB_HeartBeatComm
=================================================================== */

ACS_CHB_HeartBeatComm::ACS_CHB_HeartBeatComm () :
	ACS_CHB_Event (ACS_CHB_processName),
	pStopEvent (0),
	hbtimeout (0),
	hbinterval (0),
	apnodenumber (0)
{
	DEBUG(1,"%s","Executing ACS_CHB_HeartBeatComm constructor");

	// Set time for event reporting.
	tLastTimeEvent = time(NULL);
	iEventReported = 0;
	if( theOmHandler.Init() == ACS_CC_FAILURE )
	{
		ERROR(1, "%s", "Error occured while initializing OmHandler");
	}

} // End of constructor

/*===================================================================
ROUTINE: ~ACS_CHB_HeartBeatComm
=================================================================== */

ACS_CHB_HeartBeatComm::~ACS_CHB_HeartBeatComm ()
{
	DEBUG(1,"%s","Executing ACS_CHB_HeartBeatComm  destructor");
	if( theOmHandler.Finalize() == ACS_CC_FAILURE )
	{
		ERROR(1, "%s", "Error occured while finalizing OmHandler");
	}

} // End of destructor

/*===================================================================
ROUTINE: getParameters
=================================================================== */

ACS_CC_ReturnType ACS_CHB_HeartBeatComm::getParameters ()
{ 
	DEBUG(1, "%s", "Entering ACS_CHB_HeartBeatComm::getParameters");

	ACS_CC_ReturnType rcode = ACS_CC_SUCCESS;
	char    problemData [PROBLEMDATASIZE] = { 0};

	std::string dnObject("");
	//dnObject.append(HEARTBEAT_PARAM_OBJ_DN);
	//dnObject.append(",");
	dnObject.append(ACS_CHB_Common::dnOfHbeatRootObj);

	ACS_CC_ImmParameter paramToFind ;

	// Parameter `hbTimeout'
	DEBUG(1, "%s", "Fetching hbTimeout.");

	paramToFind.attrName = (char*)ACS_CHB_hbTimeout;

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_SUCCESS )
	{
		hbtimeout = *( reinterpret_cast < ACE_UINT32 *> (*(paramToFind.attrValues))) ;

		if( hbtimeout < 10 || hbtimeout > 60)
		{
			DEBUG(1, "%s", "Received value not in the range. Changing hbtimeout to Dafault Value");
			hbtimeout = 20;
		}

		DEBUG(1, "hbTimeout value is %ld", hbtimeout);
	}
	else
	{
		// Event handling
		ACE_OS::snprintf (problemData, sizeof(problemData)/sizeof(*problemData) - 1,
				("%d:Error when reading the IMM parameter %s"),
				__LINE__,
				ACS_CHB_hbTimeout);

		ACS_CHB_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				problemData,
				"CHB_AEH_TEXT_INT_SUPERVISOR_ERROR");
		rcode = ACS_CC_FAILURE;
	}

	// Parameter `hbInterval'
	DEBUG(1,"%s", "Fetching hbInterval.");

	paramToFind.attrName = (char*)ACS_CHB_hbInterval;

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_SUCCESS )
	{
		hbinterval = *( reinterpret_cast < ACE_UINT32 *> (*(paramToFind.attrValues))) ;

		if(hbinterval < 60 || hbinterval > 180)
		{
			DEBUG(1, "%s", "Receievd value not in the range. Changing hbInterval to Dafault Value");
			hbinterval = 60;
		}

		DEBUG(1, "hbInterval value is %ld", hbinterval);
	}
	else
	{
		// Event handling
		ACE_OS::snprintf (problemData, sizeof(problemData)/sizeof(*problemData) - 1,
				("%d:Error when reading the IMM parameter %s"),
				__LINE__,
				ACS_CHB_hbInterval);

		ACS_CHB_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				problemData,
				"CHB_AEH_TEXT_INT_SUPERVISOR_ERROR");
		rcode = ACS_CC_FAILURE;
	}

	DEBUG(1, "%s", "Leaving ACS_CHB_HeartBeatComm::getParameters");

	return rcode;
} // End of getParameters


/*===================================================================
ROUTINE: JTPcomm
=================================================================== */
//  Note: For the new JTP API

ACE_UINT32 ACS_CHB_HeartBeatComm::JTPcomm (ACE_HANDLE phStopEvent)
{
	int dwFail = 0;
	JTP_HANDLE* Fd = 0;
	int noOfHandles = 32;
	char*	applname = 0; // "APnnxHEARTBEAT"
	char cpObjectItem[ACS_CHB_SMALL_BUFSIZE];
	char cpObjectValue[ACS_CHB_SMALL_BUFSIZE];
	unsigned short MasterCPId = 0;        // CP Clock Master Id.
	CPID cpId;
	CPID MasterId;
	char problemData [PROBLEMDATASIZE];   // Data for event report.
	char stReason [PROBLEMDATASIZE];

	// Prepare StopEvent
	pStopEvent = phStopEvent;

	DEBUG(1,"%s","Entry in 'JTPcomm'\n");
	//
	// Create JTP object. Name: APnnxHEARTBEAT nn=1..99; x=A,B
	//
	applname = getAPapplName();

	if(applname == 0)
	{
		// There will be an event generated in the main loop !
		// Just return here.
		return (1);
	}

	ACS_JTP_Service myDestinator(applname);

	// Receive Clock Master Id by means of CS API
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getClockMaster(cpId);   

	if (result == ACS_CS_API_NS::Result_Success)       // Clock Master Id received.
	{
		DEBUG(1,"%s","JTP CS_API_Result ok");

		MasterCPId = cpId;  // Store Clock Master Id to be used by Threads for HeartBeat.

		DEBUG(1,"************Master CPID =%d",MasterCPId);

		if(myDestinator.jidrepreq() == 0) 
		{
			// Unable to initiate and register JTP name in DSD
			// Write a message in the eventlog and return to main loop.
			// Wait there for 5 seconds and try again.
			JTPEventDSDError();
			return (2);  
		}
	}
	else                                      // Clock Master Id not received.
	{
		DEBUG(1,"JTP CS_API_Result NOT ok %u",result);

		DEBUG(1,"%s", "CS_API failed");

		// Unable to connect to CS API to receive CP Master Id.
		// Write a message in the eventlog and return to main loop.
		// Wait there for 5 seconds and try again to connect to CS.
		ACE_OS::sprintf(problemData,"%s%s","Clock Master Id not received yet. ","Retrying...");
		event(ACS_CHB_CSnotAvailable,
				ACS_CHB_EVENT_STG,
				ACS_CHB_Cause_APfault,
				problemData,
				ACS_CHB_Text_ProblemCS);

		return (2);  
	}

	// We are disconnected. Inform Event Manager !
	ACE_OS::strcpy(cpObjectItem,"CP_connection");
	ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");

	if(requestHandler->sendRequestToEventManager(cpObjectItem,cpObjectValue) == false)
	{
		ACE_OS::sprintf(stReason,"%s","Communication lost with Child Process.");
		ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
		event(ACS_CHB_FatalErr,
				ACS_CHB_EVENT_STG,
				ACS_CHB_Cause_APfault,
				problemData,
				ACS_CHB_Text_ProcTerm);
	}

	DEBUG(1,"%s", "Send Request to EventManager");

	// Get handles for new calls

	// Changes done for TR HR16904
	Fd =  new (nothrow) JTP_HANDLE[32];

	if(Fd)
	{
		myDestinator.getHandles (noOfHandles, Fd);

		DEBUG(1, "noOfHandles = %d",noOfHandles );

		ThreadPool pool(1, 70);

		// Loop for connecting new calls
		while (!dwFail)
		{ 

			DEBUG(1,"%s", "Waiting for new calls...");

			// Wait for new calls
			if(waitUntilSignaled(noOfHandles, Fd) == -1)
			{
				//
				// StopEvent signaled, normal exit.
				//
				DEBUG(1,"%s", "StopEvent signaled, normal exit ...");
				dwFail = 0;
				break; 
			}

			// Receive Clock Master Id by means of CS API
			ACS_CS_API_NS::CS_API_Result resultMasterCP = ACS_CS_API_NetworkElement::getClockMaster(MasterId);
			if (result == ACS_CS_API_NS::Result_Success)       // Clock Master Id received.
			{
				DEBUG(1,"%s","JTP CS_API_Result ok");

				MasterCPId = MasterId;  // Store Clock Master Id to be used by Threads for HeartBeat.

				DEBUG(1,"************Master CPID =%d",MasterCPId);
			}
			else
			{
				if(Fd)
				{
					delete[] Fd;
					Fd = 0;
				}
				DEBUG(1,"JTP CS_API_Result NOT ok %u",result);

				DEBUG(1,"%s", "CS_API failed");

				// Unable to connect to CS API to receive CP Master Id.
				// Write a message in the eventlog and return to main loop.
				// Wait there for 5 seconds and try again to connect to CS.
				ACE_OS::sprintf(problemData,"%s%s","Clock Master Id not received yet. ","Retrying...");
				event(ACS_CHB_CSnotAvailable,
						ACS_CHB_EVENT_STG,
						ACS_CHB_Cause_APfault,
						problemData,
						ACS_CHB_Text_ProblemCS);

				return (2);
			}

			usleep(500000);
			ACS_JTP_Conversation * myConversation = new ACS_JTP_Conversation();

			if (myDestinator.accept(0,myConversation) == false) 
			{
				DEBUG(1, "%s", "JTP accept failed");

				// Unable to accept new calls. Write a message in the Eventlog.
				ACE_OS::sprintf(problemData,"%s","New call not accepted. `");
				event(ACS_CHB_CallnotAccepted,
						ACS_CHB_EVENT_STG,
						ACS_CHB_Cause_APfault,
						problemData,
						ACS_CHB_Text_ProblemAPJTP);

				delete myConversation;
				myConversation = 0;

			}
			else         // New calls are accepted.
			{
				DEBUG(1, "Values of IMM PARAMS hbtimeout = %d, hbinterval = %d",hbtimeout,hbinterval);

                ACS_CHB_Thread_JTPcomm* con = new ACS_CHB_Thread_JTPcomm(hbtimeout, hbinterval, MasterCPId, myConversation);
                if ( con != NULL)
                {
                        if ( pool.queueUserWorkItem(con) != 0 ){
                                delete con;
                                con = 0;
                				delete myConversation;
                				myConversation = 0;
                        }
                } else {
    				delete myConversation;
    				myConversation = 0;
                	return (2);
                }

			}
		} // End while (!dwFail)

		if(Fd)
		{
			delete[] Fd;
			Fd = 0;
		}
	}
	else
	{
		DEBUG(1,"%s", "Fd is NULL Where the new operator is failed to allocate memory for JTP_HANDLE* Fd");
	}

        DEBUG(1,"%s","Exit from 'JTPcomm'\n");

	return dwFail;
} // End of JTPcomm

/*===================================================================
ROUTINE: JTPEventDSDError
=================================================================== */
void ACS_CHB_HeartBeatComm::JTPEventDSDError()
{
	ACE_TCHAR problemData [PROBLEMDATASIZE];
	time_t  tNow;
	double  dTimeSinceLast;
	bool	bDoEventReport = true;		

	// Do not call event if ACS_CHB_TIMEFILTER is active.

	bDoEventReport = false;
	tNow = time(NULL);
	dTimeSinceLast = difftime(tNow, tLastTimeEvent);
	if( dTimeSinceLast > ACS_CHB_TIMEFILTER )
	{
		bDoEventReport = true;
		// Save this time!
		tLastTimeEvent = time(NULL);
		ACE_OS::sprintf(problemData,"%s%s%d%s","JTP unable to register name. Retrying in 5 seconds.",
				" Event situation repeated ", iEventReported,
				" times since last sent event");
		iEventReported = 0;
	}
	else
	{
		iEventReported++;
	}

	if( bDoEventReport )
	{
		event(	ACS_CHB_DiscIndJTP,
				ACS_CHB_EVENT_STG, 
				ACS_CHB_Cause_APfault,
				problemData, 
				ACS_CHB_Text_ProblemJTP);
	}

} // End of JTPEventDSDError


/*===================================================================
ROUTINE: waitUntilSignaled
=================================================================== */
int ACS_CHB_HeartBeatComm::waitUntilSignaled(int no, const ACE_HANDLE * mixedHandles)
{
	// The mixedHandles array can contain a mix of valid
	// and invalid handles. Copy all valid handles to an 
	// array with only valid handles.
	// The last handle is the StopEvent
	JTP_HANDLE validHandles[32];
	int validIndex = 0;
	for (int mixedIndex = 0; mixedIndex < no; mixedIndex++)
	{
		if (mixedHandles[mixedIndex] != ACE_INVALID_HANDLE)
		{
			validHandles[validIndex++] = mixedHandles[mixedIndex];
		}
	}
	validHandles[validIndex++] = pStopEvent;

	// Check if there are any valid handles in the array
	if (validIndex == 0)
	{
		return 0;
	}

	// Wait until one object becomes signaled

	//Create a FD set.
	ACE_Handle_Set readHandleSet;

	for( int i = 0 ;  i < validIndex ; i++)
	{
		readHandleSet.set_bit(validHandles[i]);
	}

	while(1)
	{
		int ret = ACE::select(readHandleSet.max_set() + 1, &readHandleSet, 0, 0);

		//Check for timeout.
		if (ret == 0)
		{
			// HeartBeat TimeOut !
			// No data within time limit.
			// Send msg to eventmanager.
			DEBUG(1,"%s","JTP time out occurred in waiting for new call");
			return 0;
		}
		else if( ret < 0 )
		{
			if( ACE_OS::last_error() == EINTR )
			{
				continue;
			}
			else
			{
				char problemText [PROBLEMDATASIZE];
				ACE_OS::sprintf(problemText, "System error no. =  %d " ,ACE_OS::last_error() );
				event(ACS_CHB_ConnectionLostJTP,
						ACS_CHB_EVENT_STG,
						ACS_CHB_Cause_APfault,
						"select call failed on handles.",
						problemText);
				DEBUG(1,"%s","Error occurred while waiting for multiple handles ,errno: %d",errno);
			}
			return -1;
		}
		else 
		{
			for(int i=0;i<validIndex;i++)
			{
				if( readHandleSet.is_set(validHandles[validIndex -1] ))
				{
					DEBUG(1, "%s", "Stop Event is signalled " );
					//Stop Event is signalled!!
					char buf[] = { 0 ,0};
					ACE_OS::read( validHandles[validIndex -1], buf, sizeof(buf ));
					return -1;
				}
				else
				{
					if(readHandleSet.is_set(validHandles[i]))
					{
						//one of the handle signaled
						return 1;
					}
				}
			}
			return -1;
		}
	}
}// End of waitUntilSignaled

/*===================================================================
ROUTINE: getAPapplName
=================================================================== */
char* ACS_CHB_HeartBeatComm::getAPapplName()
{
	char apNodeLetter=0;
	char * currNode=new char[20];
	static char apApplName[16];
	ACS_DSD_Node node;
	ACS_DSD_Client obj;
	obj.get_local_node(node);
	ACE_OS::strcpy(currNode,node.node_name);
	if(currNode[ACE_OS::strlen(currNode)-1]=='A'||currNode[strlen(currNode)-1]=='a')
	{
		apNodeLetter='A';

	}

	if(currNode[ACE_OS::strlen(currNode)-1]=='B'||currNode[strlen(currNode)-1]=='b')
	{
		apNodeLetter='B';

	}

	char temp[2]={};
	int j=0;
	for(int i=0;currNode[i]!='\0';i++)
	{

		if(isdigit(currNode[i]))
		{

			temp[j]=currNode[i];
			j++;
		}
	}

	apnodenumber=ACE_OS::atoi(temp);
	delete[] currNode;
	currNode = 0;
	ACE_OS::sprintf(apApplName,"AP%02d%c%s",apnodenumber,apNodeLetter,ACS_CHB_HeartBeat);
	return (apApplName);
}// End of getAPapplName
