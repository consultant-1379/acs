
/*=================================================================== */
   /**
   @file acs_logm_common.cpp

   Class method implementation for LOGM module.

   This module contains the implementation of class declared in
   the acs_logm_common.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/10/2010     XKUSATI  Initial Release
==================================================================== */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ACS_APGCC_Util.H>
#include <acs_apgcc_omhandler.h>
#include <acs_logm_common.h>



/*===================================================================
                        DEFINE DECLARATION SECTION
=================================================================== */
#define ACS_LOGM_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

/*===================================================================
			GLOBAL VARIABLE
=================================================================== */
//ACE_THR_FUNC_RETURN logmaint_svc_run(void *);
//ACE_Recursive_Thread_Mutex ThrExitHandler::theThrMutex;
//map<ACE_thread_t, bool> ThrExitHandler::theThrStatusMap;
std::string ACS_LOGM_Common::parentObjDNofLOGM = "";

/*===================================================================
   ROUTINE: ACS_LOGM_AMFService
=================================================================== */
ACS_LOGM_AMFService::ACS_LOGM_AMFService(const char* daemon_name, 
					 const char* username)
					:ACS_APGCC_ApplicationManager(daemon_name, username)
{
	readWritePipe[0] = 0;
	readWritePipe[1] = 0;
	DEBUG("%s","Entering ACS_LOGM_AMFService Constructor");
	/* create the pipe for shutdown handler */
#if 0
	Is_terminated = FALSE;
	LOGMServiceThreadId = 0;
	if ( (pipe(readWritePipe)) < 0)
	{
		ERROR("%s", "pipe creation FAILED in ACS_LOGM_AMFService");
	}

	if ( (fcntl(readWritePipe[0], F_SETFL, O_NONBLOCK)) < 0)
	{
		ERROR("%s", "pipe fcntl on read in ACS_LOGM_AMFService");
	}

	if ( (fcntl(readWritePipe[1], F_SETFL, O_NONBLOCK)) < 0)
	{
		ERROR("%s", "pipe fcntl on write in ACS_LOGM_AMFService");
	}
#endif
	m_logmApplObj =0;
	DEBUG("%s","Leaving ACS_LOGM_AMFService Constructor");
}//End of Constructor

/*===================================================================
   ROUTINE:  ACS_LOGM_AMFService Destructor 
=================================================================== */
ACS_LOGM_AMFService::~ACS_LOGM_AMFService()
{
        DEBUG("%s","Entering ACS_LOGM_AMFService::~ACS_LOGM_AMFService()");
        this->passifyApp();
        DEBUG("%s","Leaving ACS_LOGM_AMFService::~ACS_LOGM_AMFService()");
}
/*===================================================================
   ROUTINE: performStateTransitionToActiveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s","Entering ACS_LOGM_AMFService::performStateTransitionToActiveJobs");
#if 0
	ACE_TCHAR state[1] = {'A'};
	if(ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
	{
		DEBUG("%s","HA state is already active.");
		DEBUG("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToActiveJobs ");
		return ACS_APGCC_SUCCESS;
	}

	/* Handle here what needs to be done when you are given ACTIVE State */
	INFO("%s", "Application Component received ACTIVE state assignment!!!");

	if ( ACS_APGCC_AMF_HA_UNDEFINED != previousHAState )
        {
		DEBUG("%s", "HA state is moving to ACTIVE!!");

		state[0] = 'F';
		if( write(readWritePipe[1], &state, sizeof(state)) <= 0 )
		{
			ERROR("%s", "Write into the pipe failed in performStateTransitionToActiveJobs");
			ERROR("%s", "Leaving ACS_LOGM_AMFService::performStateTransitionToActiveJobs");
			return ACS_APGCC_FAILURE;
		}
		return ACS_APGCC_SUCCESS;

        }

	/* Create a thread with the state machine (active, passive, stop states)
	 * and start off with "active" state activities.
	 */

	/* spawn thread */
	 const ACE_TCHAR* thread_name = "LogmApplicationThread";
	 int threadGrpId = ACE_Thread_Manager::instance()->spawn(&logmaint_svc_run,
									(void *)this ,
									THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
									&LOGMServiceThreadId,
									0,
									ACE_DEFAULT_THREAD_PRIORITY,
									-1,
									0,
									ACE_DEFAULT_THREAD_STACKSIZE,
									&thread_name);
	if( threadGrpId == -1 )
	{
		ERROR("%s", "Error creating the application thread");
		ERROR("%s", "Leaving ACS_LOGM_AMFService::performStateTransitionToActiveJobs");
		return ACS_APGCC_FAILURE;
	}

	if( write(readWritePipe[1], &state, sizeof(state)) <= 0 )
	{
		ERROR("%s", "Write into the pipe failed in performStateTransitionToActiveJobs");
		ERROR("%s", "Leaving ACS_LOGM_AMFService::performStateTransitionToActiveJobs");
		return ACS_APGCC_FAILURE;
	}
	return ACS_APGCC_SUCCESS;
#endif
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToActiveJobs ");
        (void) previousHAState;
        return this->activateApp();

}//End of performStateTransitionToActiveJobs


/*===================================================================
   ROUTINE: performStateTransitionToQueisingJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    	DEBUG("%s","Entering ACS_LOGM_AMFService::performStateTransitionToQueisingJobs ");

	DEBUG("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToQueisingJobs ");
    	(void)previousHAState;
	return this->passifyApp();
#if 0
    	ACE_TCHAR state[1] = {'S'};

	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	INFO("%s", "Application Component received QUIESING state assignment!!!");

	/* Inform the thread to go "stop" state */
	if ( !Is_terminated )
	{
		if(write(readWritePipe[1], &state, sizeof(state))<=0)
		{
			ERROR("%s","write into the pipe failed in performStateTransitionToQueisingJobs");
			ERROR("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToQueisingJobs ");
			return ACS_APGCC_FAILURE;
		}
	}
	Is_terminated = TRUE;
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToQueisingJobs ");
	return ACS_APGCC_SUCCESS;
#endif
}//End of performStateTransitionToQueisingJobs

/*===================================================================
   ROUTINE: performStateTransitionToQueiscedJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
    	DEBUG("%s","Entering ACS_LOGM_AMFService::performStateTransitionToQuiescedJobs ");
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToQuiescedJobs ");
	(void)previousHAState;
	return this->passifyApp();

#if 0
	ACE_TCHAR state[1] = {'S'};

	/* We were Active and now losting Active state due to Lock admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */

	INFO("%s", "Application Component received QUIESCED state assignment!");

	/* Inform the thread to go "stop" state */
	if ( !Is_terminated )
	{
		if(write(readWritePipe[1], &state, sizeof(state))<=0)
		{
			ERROR("%s","write into the pipe failed in performStateTransitionToQuiescedJobs");
			ERROR("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToQuiescedJobs ");
			return ACS_APGCC_FAILURE;
		}
	}
	Is_terminated = TRUE;
	sleep(5);
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performStateTransitionToQuiescedJobs ");
	return ACS_APGCC_SUCCESS;
#endif
}//End of performStateTransitionToQueiscedJobs


/*===================================================================
   ROUTINE: performComponentHealthCheck
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::performComponentHealthCheck(void)
{
#if 0
    	DEBUG("%s","Entering ACS_LOGM_AMFService::performComponentHealthCheck ");
    	DEBUG("%s","Application Component received healthcheck query!!!\n");
    	if(LOGMServiceThreadId > 0 && !Is_terminated)
    	{
    		if( ThrExitHandler::CheckAllFuncThreads() == true )
					{
    					DEBUG("%s", "Health Check Success.");
    					DEBUG("%s", "Leaving ACS_Logm_Service::performComponentHealthCheck");
    					return ACS_APGCC_SUCCESS;
    				}
    				else
    				{
    					DEBUG("%s", "Health Check failed ");
    					DEBUG("%s", "Leaving ACS_Logm_Service::performComponentHealthCheck");
    					return ACS_APGCC_FAILURE;
    				}

					DEBUG("%s", "Returning FAILURE for Health Check.");
					DEBUG("%s", "Leaving ACS_Logm_Service::performComponentHealthCheck");
					return ACS_APGCC_FAILURE;
		}


    	DEBUG("%s", "Health Check Success.");
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performComponentHealthCheck ");
#endif	
	return ACS_APGCC_SUCCESS;
}//End of performComponentHealthCheck

/*===================================================================
   ROUTINE: performComponentTerminateJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::performComponentTerminateJobs(void)
{
	DEBUG("%s","Entering ACS_LOGM_AMFService::performComponentTerminateJobs ");
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performComponentTerminateJobs ");
	return this->passifyApp();
#if 0
	INFO( "%s", "Application Component received terminate callback!!!");

	/* Do any cleanup activity */
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performComponentTerminateJobs ");
	return ACS_APGCC_SUCCESS;
#endif
}//End of performComponentTerminateJobs

/*===================================================================
   ROUTINE: performComponentRemoveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::performComponentRemoveJobs(void)
{
	/* Application has received Removal callback. State of the application
	 * is neither Active nor Standby. This is with the result of LOCK admin operation
	 * performed on our SU. Terminate the thread by informing the thread to go "stop" state.
	 */
	DEBUG("%s","Entering ACS_LOGM_AMFService::performComponentRemoveJobs ");

	DEBUG("%s","Leaving ACS_LOGM_AMFService::performComponentRemoveJobs ");
	return this->passifyApp();
#if 0
	ACE_TCHAR state[1] = {'S'};

	INFO("%s", "Application Assignment is removed now");
	/* Inform the thread to go "stop" state */
	if ( !Is_terminated )
	{
		if(write(readWritePipe[1], &state, sizeof(state))<=0)
		{
			ERROR("%s", "write failed in performComponentRemoveJobs");
			ERROR("%s","Leaving ACS_LOGM_AMFService::performComponentRemoveJobs ");
			return ACS_APGCC_FAILURE;
		}
	}

	sleep(5);

	Is_terminated = FALSE;
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performComponentRemoveJobs ");
	return ACS_APGCC_SUCCESS;
#endif
}//End of performComponentRemoveJobs

/*===================================================================
   ROUTINE: performApplicationShutdownJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::performApplicationShutdownJobs()
{
	DEBUG("%s","Entering ACS_LOGM_AMFService::performApplicationShutdownJobs ");

	DEBUG("%s","Leaving ACS_LOGM_AMFService::performApplicationShutdownJobs ");
	return this->passifyApp();

#if 0
	DEBUG("%s", "Shutting down the application");
	ACE_TCHAR state[1] = {'S'};

	if ( !Is_terminated )
	{
		if(write(readWritePipe[1], &state, sizeof(state))<=0)
		{
			ERROR( "%s" ,"write failed");
			ERROR("%s","Leaving ACS_LOGM_AMFService::performApplicationShutdownJobs ");
			return ACS_APGCC_FAILURE;
		}
	}
	sleep(5);

	Is_terminated = FALSE;
	DEBUG("%s","Leaving ACS_LOGM_AMFService::performApplicationShutdownJobs ");
	return ACS_APGCC_SUCCESS;
#endif
}//End of performApplicationShutdownJobs

/*===================================================================
   ROUTINE: logmaint_svc_run
=================================================================== */
#if 0
ACE_THR_FUNC_RETURN logmaint_svc_run(void *ptr)
{
	DEBUG("%s","Entering logmaint_svc_run ");
	ACS_LOGM_AMFService *ACS_LOGM_AMFServiceObj = (ACS_LOGM_AMFService*) ptr;

	if(ACS_LOGM_AMFServiceObj != 0)
	{
		ACS_LOGM_AMFServiceObj->logm_svc();
		DEBUG("%s","Leaving logmaint_svc_run ");
		return 0;
	}
	else
	{
		ERROR("%s","ACS_LOGM_AMFServiceObj is NULL");
		ERROR("%s","Leaving logmaint_svc_run ");
		return 0;
	}
}//End of logmaint_svc_run

/*===================================================================
   ROUTINE: logm_svc
=================================================================== */
ACS_APGCC_ReturnType ACS_LOGM_AMFService::logm_svc()
{
    	DEBUG("%s","Entering logm_svc ");
	struct pollfd fds[1];
	nfds_t nfds = 1;
	ACE_INT32 ret;
	ACE_Time_Value timeout;

    	ACE_INT32 retCode;

	INFO("%s", "Starting LOGM Application Thread");

	fds[0].fd = readWritePipe[0];
	fds[0].events = POLLIN;

	LOGM_Service_Loader *LOGM_Service_LoaderObj =  new LOGM_Service_Loader();

	while(true)
	{
		ret = ACE_OS::poll(fds, nfds, 0); // poll can also be a blocking call, such case timeout = 0

		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			kill(getpid(), SIGTERM);
			ERROR("poll Failed - %s, Exiting...",strerror(errno));
			ERROR("%s","Leaving logm_svc ");
			return ACS_APGCC_FAILURE;
		}
		else if (fds[0].revents & POLLIN)
		{
			ACE_TCHAR ha_state[1] = {'\0'};
			ACE_TCHAR* ptr = (ACE_TCHAR*) &ha_state;
        		ACE_INT32 len = sizeof(ha_state);

	        	while (len > 0)
        		{
        			retCode=read(readWritePipe[0], ptr, len);
	        		if ( retCode < 0 && errno != EINTR)
        			{
        				ERROR( "Read interrupted by error: [%s]",strerror(errno));
        				ERROR("%s","Leaving logm_svc ");
					kill(getpid(), SIGTERM);
	        			return ACS_APGCC_FAILURE;
        			}
        			else if (retCode == 0)
        			{
	        			break;
        			}
        			else
        			{
	        			ptr += retCode;
        				len -= retCode;
        			}
	
        		}

        		if ( len != 0)
	        	{
        			ERROR( "Improper Msg Len Read [%d]", len);
        			ERROR("%s","Leaving logm_svc ");
				kill(getpid(), SIGTERM);
	        		return ACS_APGCC_FAILURE;
        		}
        		len = sizeof(ha_state);


        		if(LOGM_Service_LoaderObj == 0)
        		{
	        		ERROR("%s","LOGM_Service_LoaderObj is NULL");
        			ERROR("%s","Leaving logm_svc ");
				kill(getpid(), SIGTERM);
        			return ACS_APGCC_FAILURE;
	        	}
        		if (ha_state[0] == 'A' )   //|| ha_state[0] == 'P' )
        		{
        			/* start application work */
        			if(LOGM_Service_LoaderObj != 0)
	        		{
        				ACE_INT32 s32GrpId = -1;
        				s32GrpId = LOGM_Service_LoaderObj->logmCoreMiddleWare();
	        			if (s32GrpId == -1)
        				{
                				ERROR("%s","Error in spawning the acs_logmaintd thread");
                				ERROR("%s","Leaving logm_svc");
            					delete LOGM_Service_LoaderObj;
	            				LOGM_Service_LoaderObj = 0;
						kill(getpid(), SIGTERM);
	        	        		return ACS_APGCC_FAILURE;
        				}
        			}
	

        		}
#if 0
			else if( ha_state[0] == 'F' )
			{
						//Start the OI threads here.
						DEBUG("%s"," Application is transforming its state from Passive to Active");
							// Passive to Active
							if(LOGM_Service_LoaderObj != 0)
							{
								DEBUG("%s"," Calling changeLogmFromPassiveToActive ...");
								if(LOGM_Service_LoaderObj->changeLogmFromPassiveToActive() == ACS_CC_SUCCESS)
								{
									DEBUG("%s"," Sending ACS_APGCC_SUCCESS...");
									//return ACS_APGCC_SUCCESS;
								}
								else
								{
									DEBUG("%s"," Sending ACS_APGCC_FAILURE ...");
									kill(getpid(), SIGTERM);
									return ACS_APGCC_FAILURE;
								}
							}

			}
#endif
        		else if (ha_state[0] == 'S')
        		{
        			ERROR("%s", "Thread:: Request to stop application");
	        		/* Request to stop the thread, perform the gracefull activities here */

        			if(LOGM_Service_LoaderObj != 0)
        			{
	        			DEBUG("%s","Calling shutdownHA()");
        				LOGM_Service_LoaderObj->shutdownHA();
					DEBUG("%s","Deleting LOGM_Service_LoaderObj");
        				delete LOGM_Service_LoaderObj;
        				LOGM_Service_LoaderObj = 0;
	        			DEBUG("%s","Deleting LOGM_Service_LoaderObj completed");
        				break;
        			}

        		}
		}
	}

	INFO("%s", "Application Thread Terminated successfully");
	DEBUG("%s","Leaving logm_svc ");
	return ACS_APGCC_SUCCESS;
}//End of logm_svc
#endif
/*====================================================================
	ROUTINE: fetchDnOfRootObjFromIMM
=====================================================================*/
 int ACS_LOGM_Common::fetchDnOfRootObjFromIMM()
{
        DEBUG("%s", "Entering fetchDnOfRootObjFromIMM");

        static int infoAlreadyLoad = 0;
        static char dnOfRootObj[512] = {0};
        int returnCode = 0;

        if (infoAlreadyLoad)
        {
                parentObjDNofLOGM = std::string(dnOfRootObj);
                DEBUG("%s", "Parent DN already loaded");
                DEBUG("%s", "Leaving fetchDnOfRootObjFromIMM");
                return 0;
        }

        OmHandler omHandler;

        if (omHandler.Init() != ACS_CC_SUCCESS)
        {
                ERROR("%s", "Error occured in OMHandler.init()");
                ERROR("%s", "Leaving fetchDnOfRootObjFromIMM");
                return -1;
        }

        std::vector<std::string> dnList;

        if (omHandler.getClassInstances(LOGM_ROOT_CLASS_NAME, dnList) != ACS_CC_SUCCESS)
        {   //ERROR
                ERROR("%s", "Error occured in getting class instances");
                returnCode = -1;
        }
        else
        {
                //OK: Checking how much dn items was found in IMM
		if (dnList.size() ^ 1)
                {
                        //WARNING: 0 (zero) or more than one node found
                        ERROR("%s", "More than one object found.");
                        returnCode = -1;
                }
                else
                {
                        //OK: Only one root node
                        strncpy(dnOfRootObj, dnList[0].c_str(), ACS_LOGM_ARRAY_SIZE(dnOfRootObj));
                        dnOfRootObj[ACS_LOGM_ARRAY_SIZE(dnOfRootObj) - 1] = 0;
                        infoAlreadyLoad = 1;
                        parentObjDNofLOGM = std::string(dnOfRootObj);
                }
        }

        omHandler.Finalize();
        DEBUG("%s", "Leaving fetchDnOfRootObjFromIMM");

        return returnCode;
}



ACS_APGCC_ReturnType ACS_LOGM_AMFService::activateApp() {

        DEBUG("%s","Entering ACS_LOGM_AMFService::activateApp()");
        ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

        if ( 0 != this->m_logmApplObj)
        {
              DEBUG("%s","application is already active");
              rCode = ACS_APGCC_SUCCESS;
        }
        else
        {
                ACE_NEW_NORETURN(this->m_logmApplObj, LOGM_Service_Loader());
                if (0 == this->m_logmApplObj)
                {
                        DEBUG("%s","failed to create the lm instance");
                }
                else
                {
                        this->m_logmApplObj->logmCoreMiddleWare();
                        DEBUG("%s","ha-class: application is now activated by HA");
                        rCode = ACS_APGCC_SUCCESS;
                }
        }
        DEBUG("%s","Exiting ACS_LOGM_AMFService::activateApp()");
        return rCode;
}

ACS_APGCC_ReturnType ACS_LOGM_AMFService::passifyApp() {

        DEBUG("%s","Entering ACS_LOGM_AMFService::passifyApp()");

        ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

        if (0 == this->m_logmApplObj)
        {
                DEBUG("%s","application is already passive");
                rCode = ACS_APGCC_SUCCESS;
        }
        else
        {
                DEBUG("%s","Ordering lm Application to passify");
                this->m_logmApplObj->stop();

                DEBUG("%s","Waiting for lm Application to become passive...");
                this->m_logmApplObj->wait();

                DEBUG("%s","Deleting lm App instance...");
                delete this->m_logmApplObj;
                this->m_logmApplObj=0;
                DEBUG("%s","App is now passivated by HA!!");
                rCode = ACS_APGCC_SUCCESS;
        }
        DEBUG("%s","Exiting ACS_LOGM_AMFService::passifyApp()");
        return rCode;
}












