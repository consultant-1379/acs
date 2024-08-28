/*=================================================================== */
   /**
   @file acs_chb_heartbeat_class.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of the main class for heartbeat.

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       20/04/2011   XTANAGG   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>
#include <ace/Process.h>
#include <ace/Event.h>
#include <algorithm>
#include <sys/stat.h>
#include <acs_prc_api.h>
#include <ACS_APGCC_CommonLib.h>
//#include <ACS_PRC_Process.h>
#include <acs_chb_common.h>
#include <acs_chb_request_handler.h>
#include <acs_chb_heartbeat_comm.h>
#include <acs_chb_tra.h>
#include <acs_chb_aadst_ThreadManager.h>
#include <acs_chb_mtzclass.h>
#include <acs_chb_file_handler.h>
#include <acs_chb_heartbeat_class.h>
using namespace std;


/**
	@brief		heartbeat_svc_run
*/
ACE_THR_FUNC_RETURN heartbeat_svc_run(void *);

bool debug = false;
ACS_APGCC_ReturnType rcode ;
ACS_CHB_CPObjectList *HeadCPObjectList  = 0;
char requestFromHeartBeat[ACS_CHB_SMALL_BUFSIZE];
ACS_CHB_RequestHandler *requestHandler = 0;
ACE_Process process;
ACE_HANDLE pipeHandles[2];
int checkNodeState();
pid_t childProcInfo = 0;
ACE_Recursive_Thread_Mutex ThrExitHandler::theThrMutex;
map<ACE_thread_t, bool> ThrExitHandler::theThrStatusMap;

char ACS_CHB_Common::dnOfHbeatRootObj[512] = {0};

/*=======================================================================
	ROUTINE: acs_chb_heartbeat_class
=========================================================================*/
acs_chb_heartbeat_class::acs_chb_heartbeat_class(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	DEBUG(1, "%s", "In acs_chb_heartbeat_class::acs_chb_heartbeat_class");
}


/*=======================================================================
	ROUTINE: ~acs_chb_heartbeat_class
=========================================================================*/
acs_chb_heartbeat_class::~acs_chb_heartbeat_class()
{
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::~acs_chb_heartbeat_class");
	this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::~acs_chb_heartbeat_class");
}
	


/*===========================================================================
	ROUTINE: performStateTransitionToActiveJobs
=============================================================================*/
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::performStateTransitionToActiveJobs");
	(void) previousHAState;
	rcode = this->activateApp();
	DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::performStateTransitionToActiveJobs");
	return rcode;
}


/*===========================================================================
        ROUTINE: performStateTransitionToPassiveJobs
=============================================================================*/
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
        DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::performStateTransitionToPassiveJobs");

        /* Check if we have received the PASSIVE State Again.
         * This means that, our application is already passive and
         * again we have got a callback from AMF to go passive.
         * Ignore this case anyway. This case should rarely happens
         */
    	(void)previousHAState;
    	rcode = this->passifyApp();
        DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::performStateTransitionToPassiveJobs");

        return rcode;
}



/*========================================================================================
	ROUTINE: performStateTransitionToQueisingJobs
=========================================================================================*/
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::performStateTransitionToQueisingJobs");

	(void)previousHAState;
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::performStateTransitionToQueisingJobs");
	return rcode;
}

/*=====================================================================================
	ROUTINE: performStateTransitionToQuiescedJobs
=======================================================================================*/
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1,"%s","Entering acs_chb_heartbeat_class::performStateTransitionToQuiescedJobs");

	(void)previousHAState;
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::performStateTransitionToQuiescedJobs");
	return rcode;
}

/*====================================================================================
	ROUTINE: performComponentHealthCheck
====================================================================================== */
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performComponentHealthCheck(void)
{
	return ACS_APGCC_SUCCESS;
}

/*====================================================================================
	ROUTINE: performComponentTerminateJob
=====================================================================================*/
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performComponentTerminateJobs(void)
{
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::performComponentTerminateJobs");

	/* Application has received terminate component callback due to 
	 * LOCK-INST admin opreration perform on SU. Terminate the thread if
	 * we have not terminated in performComponentRemoveJobs case or double 
	 * check if we are done so.
	 */
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::performComponentTerminateJobs");
	return rcode;
}

/*====================================================================================
	ROUTINE: performComponentRemoveJobs
=====================================================================================*/
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performComponentRemoveJobs(void)
{

	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::performComponentRemoveJobs");
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::performComponentRemoveJobs");
	return rcode;
}

/*====================================================================================
	ROUTINE: performApplicationShutdownJobs
=====================================================================================*/
ACS_APGCC_ReturnType acs_chb_heartbeat_class::performApplicationShutdownJobs() 
{
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::performApplicationShutdownJobs");
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::performApplicationShutdownJobs");
	return ACS_APGCC_SUCCESS;
}
ACS_APGCC_ReturnType acs_chb_heartbeat_class::activateApp()
{
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::activateApp");

        int threadGroupId =	ACE_Thread_Manager::instance()->spawn(&CHB_serviceMain,
        				(void *)this ,
        				THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
        				0,
        				0,
        				ACE_DEFAULT_THREAD_PRIORITY,
        				-1,
        				0,
        				ACE_DEFAULT_THREAD_STACKSIZE);
        if (threadGroupId == -1)
        {
        	ERROR(1, "%s", "Error occurred while creating the ACS_CHB_HeartBeatThread2");
        	ERROR(1, "%s", "Leaving acs_chb_heartbeat_class::activateApp");
        	kill(getpid(), SIGTERM);
        	return ACS_APGCC_FAILURE;
        }

        DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::activateApp");
        return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType acs_chb_heartbeat_class::passifyApp()
{
	DEBUG(1, "%s", "Entering acs_chb_heartbeat_class::passifyApp");
	ACS_APGCC_ReturnType result = ACS_APGCC_FAILURE;
	result = stop();
	DEBUG(1, "%s", "Leaving acs_chb_heartbeat_class::passifyApp");
	return result;
}

ACS_APGCC_ReturnType stop()
{
	DEBUG(1, "%s", "Entering stop");
	if (ThrExitHandler::CheckNoThreads() == false)
	{
		char buf[] = { 1 , 1};
		int bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
		while( bytes <= 0 )
		{
			ERROR(1, "%s", "Error occurred while signaling stop event ");
			ERROR(1, "%s", "Resending the stop event after 1 sec");
			sleep(1);
			bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
		}
			INFO(1, "%s", "Stop event signaled ");
			//Wait until all threads have stopped.
			do
			{
				sleep(1);
			}
			while( ThrExitHandler::CheckNoThreads() == false );

			ThrExitHandler::cleanup();
	}
	DEBUG(1, "%s", "Leaving stop");
	return ACS_APGCC_SUCCESS;
}
/*=============================================================================
	ROUTINE: CHB_serviceMain
==============================================================================*/
ACE_THR_FUNC_RETURN CHB_serviceMain(void *ptr )
{
	(void)ptr;
	
	DEBUG(1,"%s","Starting NEW CHB_serviceMain thread...");

	if( ACE_OS::pipe(pipeHandles) == -1 )
	{
		ACS_CHB_Common::CHB_serviceError("CANNOT OPEN PIPE FOR END EVENT.", ACE_OS::last_error());
		ERROR(1, "%s", "Error occured while creating pipe for STOP event!!");
		ERROR(1, "%s", "Exiting acs_chbheartbeatd");
		ACS_CHB_Tra::ACS_CHB_Logging.Close();
	}
	//Register the current thread with the Exit Handler.
	if( ThrExitHandler::init() == false)
	{
		ERROR(1, "%s", "Error occured while registering exit handler");
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);
		return 0;
	}

	//Start the MTZ API thread.
	if ( ACS_CHB_mtzclass::MTZInit() == false )
	{
		ERROR(1, "%s", "Error occured while initializing MTZInit");
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);
		return 0;
	}

	 //Fetch the parent object from IMM.

        if(ACS_CHB_Common::fetchDnOfHbeatRootObjFromIMM() == -1)
        {
                ERROR(1, "%s", "Fetching of DN of Heartbeat object from IMM is failed !!");
	
		ACS_CHB_mtzclass::MTZDestroy();
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);

                ERROR(1, "%s", "Exiting CHB_serviceMain");
                return 0;
        }
        ACE_INT32 dwFail = -1;
        // Check for CS availability
        while (dwFail != 0)
        {
                INFO(1,"%s","1 dwfail is not zero.");
                if (isCsOnline())
                {
                        INFO(1, "%s", "CS is available, break ");
                         break;
                }
                ACE_Time_Value tv2(1,0);
                ACE_Handle_Set readHandleSet2;
                readHandleSet2.set_bit(pipeHandles[0]);
                int status2 = ACE::select( readHandleSet2.max_set()+1, &readHandleSet2, 0, 0, &tv2);

                switch(status2)
                {
                        case 0:
                                dwFail = -1;
                                break;
                        case -1: //error
                        {
                                char stReason [PROBLEMDATASIZE] = {0};
                                char problemData [PROBLEMDATASIZE] = {0};
                                ACE_OS::snprintf(stReason, sizeof(stReason) - 1, "event timed wait failed %u", ACE_OS::last_error());
                                ACE_OS::snprintf(problemData, sizeof(problemData) - 1, "%s%s","Process terminated due to a fatal error. Cause :",stReason);
                                dwFail = 0; //terminate while
                                break;
                        }
                        default:
                        {
                                if (readHandleSet2.is_set( pipeHandles[0]))
                                {                                                                                            
                                        char buf[] = { 0 , 0};
                                        ACE_OS::read( pipeHandles[0], buf, sizeof(buf ));
                                }
                                dwFail = 0;
                                break;
                        }
                }
        }
        if (dwFail == 0)
        {
                ACS_CHB_mtzclass::MTZDestroy();
                ACE_OS::close(pipeHandles[0]);
                ACE_OS::close(pipeHandles[1]);
                return 0;
        }
	int result = -1;
	//Check for DSD Dependency.
	result = checkDSDDependencyForHeartBeat() ;
	if( result == -1 )
	{
		ERROR(1, "%s", "DSD Server is not running, exiting");
		ACS_CHB_mtzclass::MTZDestroy();
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);

		return 0;
	}
	else if( result == 0 )
	{
		DEBUG(1, "%s", "Stop Event Signalled");

		ACS_CHB_mtzclass::MTZDestroy();
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);

		ACS_CHB_Common::CHB_serviceError("ACS_CHB_HeartBeatService MAIN THREAD END.",0);
		return 0;
	}

	const ACE_Time_Value tv1(2,0);
	ACE_Handle_Set readHandleSet1;
	readHandleSet1.set_bit( pipeHandles[0]);
	int status1 = ACE::select( readHandleSet1.max_set()+1, readHandleSet1, &tv1);

	INFO(1,"Event time out value = %d", tv1.sec());
	INFO(1,"status of event timed wait = %d",status1);

	if (status1 > 0)
	{
		if( readHandleSet1.is_set(pipeHandles[0]))
		{
			// Stop event signaled.
			char buf[] = { 0, 0 };
			ACE_OS::read( pipeHandles[0], buf, sizeof(buf ));

			ERROR(1, "%s", "Error occurred while reading stop event.");
			ACS_CHB_Common::CHB_serviceError("ACS_CHB_HeartBeatService MAIN THREAD END. Could not initialize MTZ",0);
			ACE_OS::close(pipeHandles[0]);
			ACE_OS::close(pipeHandles[1]);
			ACS_CHB_mtzclass::MTZDestroy();
			return 0;
		}
	}
	// Adding code to set correct permission on acsdata\MTZ library
	if(!setupMTZLib())
	{
		ACS_CHB_Common::CHB_serviceError("ACS_CHB_HeartBeatService MAIN THREAD END. Could not initialize MTZ",0);
		ERROR(1, "%s", "Error occurred while setting up MTZ lib.");
		ACE_OS::close(pipeHandles[0]);
		ACE_OS::close(pipeHandles[1]);
		ACS_CHB_mtzclass::MTZDestroy();
		return 0;
	}

	// START AADST MAIN THREAD //

	ACS_CHB_aadst_ThreadManager aadst_threadManager;
	int ret_code= aadst_threadManager.AADST_start();
	if ( ret_code == ACS_CHB_aadst_ThreadManager::CHB_AADST_TM_FAILURE ||
			ret_code == ACS_CHB_aadst_ThreadManager::CHB_AADST_TM_CANNOT_RETRIEVE_SYS_ID	)
	{
		// a retry mechanism when ret_code== CHB_AADST_TM_CANNOT_RETRIEVE_SYS_ID ?)
		ERROR(1, "%s", "AADST NOT STARTED: An error occurred while starting AADST_start()!");
//		ACE_OS::close(pipeHandles[0]);
//		ACE_OS::close(pipeHandles[1]);
//		return 0;
	}

	//
	// Initialization of HeartBeat
	//
	// Create HeartBeat communication objects.

	ACS_CHB_HeartBeatComm comm;
	static ACS_CHB_RequestHandler Handler;
	requestHandler = &Handler;

	if(startupEventManager() == ACS_CHB_FALSE)
	{
		ERROR(1,"%s","Error occured while creating acs_chbheartbeatchildd.");
		comm.event(ACS_CHB_FatalErr,
			ACS_CHB_EVENT_STG,
			ACS_CHB_Cause_APfault,
			"Could not Create Child Process.",
			ACS_CHB_Text_ProcTerm);
	}
	else
	{
		// Establish communication with event manager.
		if(requestHandler -> initPipeToEventManager() == ACS_CHB_FALSE)
		{
			ERROR(1, "%s", "Error occured while starting communication with acs_chbheartbeatchildd.");
			comm.event(ACS_CHB_FatalErr,
				ACS_CHB_EVENT_STG,
				ACS_CHB_Cause_APfault,
				"Could not establish communication with Child Process.",
				ACS_CHB_Text_ProcTerm);
		}
		else
		{
			// Read parameters for communication interface.
			if (comm.getParameters () == ACS_CC_SUCCESS )
			{
				ACE_INT32 dwFail = -1;
				DEBUG(1, "%s", "HeartBeat IMM parameters fetched successfully");
				//Wait the CS is available
				while (dwFail != 0)
				{
					INFO(1,"%s","1 dwfail is not zero.");
					if (isCsOnline())
					{
						INFO(1, "%s", "CSOnline is true, break ");
						break;
					}
					ACE_Time_Value tv2(1,0);
					ACE_Handle_Set readHandleSet2;
					readHandleSet2.set_bit(pipeHandles[0]);
					int status2 = ACE::select( readHandleSet2.max_set()+1, &readHandleSet2, 0, 0, &tv2);

					switch(status2)
					{
			                case 0:
								dwFail = -1;
								break;
							case -1: //error
							{
								char stReason [PROBLEMDATASIZE] = {0};
								char problemData [PROBLEMDATASIZE] = {0};
								ACE_OS::snprintf(stReason, sizeof(stReason) - 1, "event timed wait failed %u", ACE_OS::last_error());
								ACE_OS::snprintf(problemData, sizeof(problemData) - 1, "%s%s","Process terminated due to a fatal error. Cause :",stReason);
								comm.event(ACS_CHB_FatalErr,
									ACS_CHB_EVENT_STG,
									ACS_CHB_Cause_APfault,
									problemData,
									ACS_CHB_Text_ProcTerm);
								dwFail = 0; //terminate while
								break;
							}
							default:
							{
								if (readHandleSet2.is_set( pipeHandles[0]))
								{
									char buf[] = { 0 , 0};
									ACE_OS::read( pipeHandles[0], buf, sizeof(buf ));
								}
								dwFail = 0;
								break;
							}
					}
			}

			if (dwFail != 0)
			{
				do
				{
					dwFail = comm.JTPcomm(pipeHandles[0]);
					if(dwFail == 2)
					{
						// This case implicitly means that DSD do have a problem,
						// wait for this to be solved.
						// event for this is generated in comm.JTPcomm().
						ACE_Time_Value tv3(5,0);
						ACE_Handle_Set readHandleSet3;
						readHandleSet3.set_bit(pipeHandles[0]);
						int status3 = ACE::select( readHandleSet3.max_set()+1, &readHandleSet3, 0, 0, &tv3);

						if( status3 > 0 )
						{
							if( readHandleSet3.is_set( pipeHandles[0] ))
							{
								char buf[] = { 0, 0 };
								ACE_OS::read( pipeHandles[0], buf, sizeof(buf ));
								//Stop Event signalled.
								dwFail = 0;
							}
						}

					}

				} while (dwFail == 2); //end do
			}

			// dwFail == 2  : Call jidrepreq() failed.
			// Meaning that the publishing of CHB name in DSD failed.
			// dwFail == 0  : Stop event signaled.
			// dwFail == 1  : getAPapplName() failed.

			// Process terminated by stop event or failure.
			if (dwFail != 0)
			{
				// Process terminated
				char stReason [PROBLEMDATASIZE];
				char problemData [PROBLEMDATASIZE];
				switch (dwFail)
				{
					case 1 :
						ACE_OS::sprintf(stReason,"%s","getAPapplName() failed.");
						ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. Cause :",stReason);
						comm.event(ACS_CHB_FatalErr,
									ACS_CHB_EVENT_STG,
									ACS_CHB_Cause_APfault,
									problemData,
									ACS_CHB_Text_ProcTerm);
						break;
					default:
						ACE_OS::sprintf(stReason,"%s%d","dwFail = ",dwFail);
						ACE_OS::sprintf(problemData,"%s%s","Process terminated due to a fatal error. return code ",stReason);
						comm.event(ACS_CHB_FatalErr,
									ACS_CHB_EVENT_STG,
									ACS_CHB_Cause_APfault,
									problemData,
									ACS_CHB_Text_ProcTerm);
						break;
					}
				}
							// Normal exit, free up used resources.
			}

			// Terminate requested
			// We are disconnected. Inform Event Manager !
			char cpObjectItem[ACS_CHB_SMALL_BUFSIZE];
			char cpObjectValue[ACS_CHB_SMALL_BUFSIZE];

			ACE_OS::strcpy(cpObjectItem,"[TERMINATE]");
			ACE_OS::strcpy(cpObjectValue,"");
			requestHandler -> sendRequestToEventManager(cpObjectItem,cpObjectValue);

		}
		int status = 0;
		ACE_OS::waitpid(childProcInfo, &status, 0);
		if(WIFEXITED(status))
		{
			DEBUG(1,"%s", "child process terminated normally" );
		}
	}
	ACE_OS::close(pipeHandles[0]);
	ACE_OS::close(pipeHandles[1]);

	ACS_CHB_mtzclass::MTZDestroy();
	aadst_threadManager.AADST_stop();
	ACS_CHB_Common::CHB_serviceError("ACS_CHB_HeartBeatService MAIN THREAD END.",0);
	return 0;
}

/*============================================================================
	ROUTINE: startupEventManager
==============================================================================*/
ACE_INT32 startupEventManager()
{
	DEBUG(1, "%s", "Entering startUpEventManager");

	ACE_Process_Options options;
	ACE_Time_Value tv(500,0);

	options.process_name(ACS_CHB_processNameChild);
	options.command_line(ACS_CHB_processNameChild);
	options.avoid_zombies();

	//Create the child process.
	int ret = process.spawn(options);

	//Get the pid of the child process.
	childProcInfo = process.getpid();

	if(ret == -1)
	{
		ERROR(1, "%s", "startUpEventManager of ACS_CHB_eventManager failed");
		ERROR(1, "%s", "Leaving startUpEventManager");
		return ACS_CHB_FALSE;
	}

	INFO(1, "Child process created, pid : %d", childProcInfo);
	DEBUG(1, "%s", "Leaving startUpEventManager");
	return ACS_CHB_TRUE;
} // End of startupEventManager

/*============================================================================
	ROUTINE: getacsdata
=============================================================================*/
bool getacsdata(char* pChar, size_t pLen)
{
	DEBUG(1, "%s", "Entering getacsdata");

	ACS_APGCC_CommonLib oComLib;
	char* szPath = new char[FILE_MAX_PATH];

	int dwLen = FILE_MAX_PATH;
	const char * szLogName = "ACS_DATA";

	while (!ACS_CHB_Common::isDataDiskAvailable())
	{
		ERROR( 1, "%s","Error occured while getting data disk path");
		ERROR (1, "%s", "acs_chb_heartbeat_class - getacsdata - DATA DISK PATH or DATA DISK is not configured !!");
		ERROR (1, "%s", "acs_chb_heartbeat_class - getacsdata - Retries after 10 sec");
		const ACE_Time_Value tv1(10,0);
		ACE_Handle_Set readHandleSet1;
		readHandleSet1.set_bit( pipeHandles[0]);

		int status1 = ACE::select( readHandleSet1.max_set()+1, readHandleSet1, &tv1);

		if (status1 > 0)
		{
			if( readHandleSet1.is_set(pipeHandles[0]))
			{
				// Stop event signaled.
				char buf[] = { 0, 0 };
				ACE_OS::read( pipeHandles[0], buf, sizeof(buf ));
				DEBUG(1, "%s", "Stop event signaled while waiting for DATA DISK PATH to come up");
				delete [] szPath;
				return false;
			}
		}
		else if( status1 == 0 )
		{

			DEBUG(1, "%s", "Timeout happened while waiting for DATA DISK PATH to come up");
		}
	}

	oComLib.GetDataDiskPath(szLogName, szPath, dwLen);
	ACE_OS::snprintf(pChar, pLen, "%s", szPath);
	DEBUG(1,"data disk path: %s",szPath);

	//De allocate memoery for szPath.
	delete[] szPath;
	szPath = 0;

	DEBUG(1, "%s", "Leaving getacsdata");
	return true;


}//End of getacsdata

/*=============================================================================
	ROUTINE: setupMTZLib
==============================================================================*/
bool setupMTZLib(void)
{
	DEBUG(1, "%s", "Entering setupMTZLib");

	int nodeState = checkNodeState();

	if( nodeState == 1 )	//If node state is ACTIVE
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

		if( getacsdata(mtzdata, sizeof(mtzdata) - 1) )
		{
			INFO(1,"mtzdata = %s ",mtzdata);
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

			ERROR(1, "%s", problemData);
			ERROR(1, "%s", "Leaving setupMTZLib");
			return false ;
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

				FileMapType timeZoneMap;
				ACS_CHB_filehandler fh;
				int result = fh.WriteZoneFile( &timeZoneMap);
				if( result == false )
				{
					char  problemData[200];
					ACE_OS::sprintf(problemData, "%s File could not be created on the data disk.", FileName);
					ACS_CHB_Common::CHB_serviceError(problemData, ACE_OS::last_error());

					ERROR(1, "%s", problemData);
					ERROR(1, "%s", "Leaving setupMTZLib");
					return false;
				}
				DEBUG(1, "%s", "Successfully created the ACS_TZ_links.cfg file");
				return true;
			}
			else
			{
				DEBUG(1, "%s", "MTZ file is already present on the data disk.");
				return true;
			}
		}
		char   problemData[200];
		ACE_OS::sprintf(problemData, "Data Disk Path not present on the active node.");
		ACS_CHB_Common::CHB_serviceError(problemData , ACE_OS::last_error());

		ERROR(1, "%s", problemData);
		ERROR(1, "%s", "Leaving setupMTZLib");
		DEBUG(1, "%s", "MTZ file is already present on the data disk.");
		return false;
	}
	else if( nodeState == 2 )
	{
			//assume passive node, return true.
			DEBUG(1, "%s", "Leaving setupMTZLib");
			return true;

	}
	else
	{
		ERROR(1,"%s", "Unable to get the node state from PRC API");
		return false;
	}

}


/*============================================================================
	ROUTINE: isCsOnline
=============================================================================*/
bool isCsOnline()
{
	DEBUG(1, "%s", "Entering isCsOnline");

	bool retCode = false;
	ACS_CS_API_CP* cpNameObj = ACS_CS_API::createCPInstance();
	if (cpNameObj)
	{
		ACS_CS_API_IdList cpList;
		ACS_CS_API_NS::CS_API_Result res = cpNameObj->getCPList(cpList);
		if (res == ACS_CS_API_NS::Result_Success)
		{
			retCode = true;
		}
		ACS_CS_API::deleteCPInstance(cpNameObj);
		cpNameObj = 0;
	}
	DEBUG(1, "%s", "Leaving isCsOnline");
	return retCode;
}//End of isCsOnline


/*===============================================================================
	ROUTINE: checkDSDDependencyForHeartBeat
=================================================================================*/
int checkDSDDependencyForHeartBeat()
{
	DEBUG(1, "%s", "Entering checkDSDDependencyForHeartBeat");
	
	//ACS_CHB_Event serviceMainEvents(ACS_CHB_processName);
	//char problemData[512] = { 0 };
	while( ACS_CHB_Common::isDSDServerRunning() == false )
	{
		const ACE_Time_Value tv1(30,0);
		ACE_Handle_Set readHandleSet1;
		readHandleSet1.set_bit( pipeHandles[0]);

		int status1 = ACE::select( readHandleSet1.max_set()+1, readHandleSet1, &tv1);

		if (status1 > 0)
		{
			if( readHandleSet1.is_set(pipeHandles[0]))
			{
				// Stop event signaled.
				char buf[] = { 0, 0 };
				ACE_OS::read( pipeHandles[0], buf, sizeof(buf ));
				DEBUG(1, "%s", "Stop event signalled while waiting for DSD service to come up");
				return 0;
			}
		}
		else if( status1 == 0 )
		{

			DEBUG(1, "%s", "Timeout happened while waiting for DSD server");
		}
	}
	DEBUG(1, "%s", "Leaving checkDSDDependencyForHeartBeat");
	return 1;
}

int checkNodeState()
{
	ACS_PRC_API prcObj;
        int nodeState = 0;
        nodeState = prcObj.askForNodeState();

	return nodeState;
}
	
