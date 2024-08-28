/*=================================================================== */
   /**
   @file acs_chb_mtzln_service.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_mtzln_service.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/03/2011   XNADNAR   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_chb_mtzln_service.h>
#include <acs_chb_mtzln_implementer.h>
#include <acs_chb_tra.h>
#include <acs_prc_api.h>
/*===================================================================
			GLOBAL VARIABLE
 =================================================================== */
ACE_THR_FUNC_RETURN mtzln_svc_run(void *);
ACS_APGCC_ReturnType rcode;

/*===================================================================
   ROUTINE: ACS_CHB_MTZLN_Service
=================================================================== */
ACS_CHB_MTZLN_Service::ACS_CHB_MTZLN_Service(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::ACS_CHB_MTZLN_Service");
	theMTZLNImplementer = 0;
#if 0
	Is_terminated = FALSE;
	
        if ( (pipe(readWritePipe)) < 0) 
	{
       		ERROR(1, "%s",  "pipe creation FAILED in ACS_CHB_MTZLN_Service::ACS_CHB_MTZLN_Service");
	}

        if ( (fcntl(readWritePipe[0], F_SETFL, O_NONBLOCK)) < 0) 
	{
        	ERROR(1, "%s", "pipe fcntl failed on read in ACS_CHB_MTZLN_Service::ACS_CHB_MTZLN_Service");
        }

        if ( (fcntl(readWritePipe[1], F_SETFL, O_NONBLOCK)) < 0) 
	{
        	ERROR(1, "%s", "pipe fcntl failed on write in ACS_CHB_MTZLN_Service::ACS_CHB_MTZLN_Service");
        }

	nodeState = ACS_APGCC_AMF_HA_UNDEFINED;
#endif
	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::ACS_CHB_MTZLN_Service");

}//End of Constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_MTZLN_Service
=================================================================== */
ACS_CHB_MTZLN_Service::~ACS_CHB_MTZLN_Service()
{
	this->passifyApp();
#if 0
	//Close the file descriptors.
	close( readWritePipe[0] );
	close ( readWritePipe[1] );
#endif
}


/*===================================================================
   ROUTINE: performStateTransitionToActiveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
	(void) previousHAState;
#if 0
	nodeState = ACS_APGCC_AMF_HA_ACTIVE;

	/* Check if we have received the ACTIVE State Again.
	 * This means that, our application is already Active and
	 * again we have got a callback from AMF to go active.
	 * Ignore this case anyway. This case should rarely happens
	 */
	ACE_TCHAR state[1] = {'A'};
	if(ACS_APGCC_AMF_HA_ACTIVE == previousHAState)
	{
		DEBUG(1, "%s", "HA ACTIVE is already active");
		DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
		return ACS_APGCC_SUCCESS;
	}


	/* Handle here what needs to be done when you are given ACTIVE State */
	INFO(1, "%s", "acs_chbmtzlnd received ACTIVE state assignment!!!");
	
	//Going from passive to active.
        if ( ACS_APGCC_AMF_HA_UNDEFINED != previousHAState )
        {
                /** MTZLN Thread is already running stop it. **/
		/** if transition is from passive to active **/
		//Stop the thread.
		state[0] = 'S';
		if( write(readWritePipe[1], &state, sizeof(state)) <= 0 )
		{
			ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
			return ACS_APGCC_FAILURE;
		}

		state[0] = 'A';

        }

	
	/* Create a thread with the state machine (active, passive, stop states)
	 * and start off with "active" state activities.
	 */
	
	/* spawn thread */
	int threadGroupId = ACE_Thread_Manager::instance()->spawn(&mtzln_svc_run,
									(void *)this ,
									THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
									&applicationThreadId,
									0,
									ACE_DEFAULT_THREAD_PRIORITY,
									-1,
									0,
									ACE_DEFAULT_THREAD_STACKSIZE);
	if (threadGroupId == -1)
	{
		ERROR(1, "%s", "Error creating the application thread for acs_chbmtzlnd");
		ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
		return ACS_APGCC_FAILURE;	
	}
				
	if( write(readWritePipe[1], &state, sizeof(state)) <= 0 )
	{
		//Some problem happened during communication. May be thread got terminated.
		ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
		ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
		return ACS_APGCC_FAILURE;
	}
#endif
	rcode = this->activateApp();
	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToActiveJobs");
	return rcode;

}//End of performStateTransitionToActiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToPassiveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
	(void) previousHAState;
	rcode = this->passifyApp();
#if 0
	nodeState = ACS_APGCC_AMF_HA_STANDBY;

	/* Check if we have received the PASSIVE State Again.
	 * This means that, our application was already Passive and
	 * again we have got a callback from AMF to go passive.
	 * Ignore this case anyway. This case should rarely happens.
	 */

	ACE_TCHAR state[1] = {'P'};
	if(ACS_APGCC_AMF_HA_STANDBY == previousHAState)
	{
		DEBUG(1, "%s", "HA state is already STANBY");
		DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
		return ACS_APGCC_SUCCESS;
	}

	/* Our application has received state PASSIVE from AMF.
	 * Check if the state received is due to State Transistion.
	 * (Active->Passive).
	 */
	if ( ACS_APGCC_AMF_HA_UNDEFINED != previousHAState )
	{
		INFO( 1, "%s", "State Transision happened for acs_chbmtzlnd. Becomming Passive now");
		 /* Inform the thread to go "passive" state*/

		state[0] = 'S';
		if( write(readWritePipe[1], &state, sizeof(state)) <= 0)
		{
			ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
			return ACS_APGCC_FAILURE;
		}
		state[0] = 'P';
	}

	/* Create a thread with the state machine (active, passive, stop states)
         * and start off the thread with "passive" state.
         */
	INFO(1, "%s", "acs_chbmtzlnd received STANDBY state assignment!!!");

	int threadGroupId = ACE_Thread_Manager::instance()->spawn(&mtzln_svc_run,
									(void *)this ,
									THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
									&applicationThreadId,
									0,
									ACE_DEFAULT_THREAD_PRIORITY,
									-1,
									0,
									ACE_DEFAULT_THREAD_STACKSIZE);
	if (threadGroupId == -1)
	{
		ERROR(1, "%s", "Error creating the application thread for acs_chbmtzlnd");
		ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
		return ACS_APGCC_FAILURE;	
	}

	if( write(readWritePipe[1], &state, sizeof(state)) <= 0 )
	{
		ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
		ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
		return ACS_APGCC_FAILURE;
	}
#endif

	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToPassiveJobs");
	return rcode;

}//End of performStateTransitionToPassiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToQueisingJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::performStateTransitionToQueisingJobs");
	(void) previousHAState;
	rcode = this->passifyApp();
#if 0
	(void)previousHAState;
	ACE_TCHAR state[1] = {'S'};

	nodeState = ACS_APGCC_AMF_HA_QUIESCING;

	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU. 
	 * Inform the thread to go to "stop" state
	 */  

	INFO(1, "%s", "acs_chbmtzlnd received QUIESING state assignment!!!");
	
	/* Inform the thread to go "stop" state */	
	if ( !Is_terminated )
	{
		if( write(readWritePipe[1], &state, sizeof(state)) <= 0 )
		{
			ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performStateTransitionToQueisingJobs");
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToQueisingJobs");
			return ACS_APGCC_FAILURE;
		}
	}
	Is_terminated = TRUE;

	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToQueisingJobs");
#endif
	return rcode;

}//End of performStateTransitionToQueisingJobs

/*===================================================================
   ROUTINE: performStateTransitionToQuiescedJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::performStateTransitionToQuiescedJobs");
	(void)previousHAState;
	rcode = this->passifyApp();
#if 0
	ACE_TCHAR state[1] = {'S'};

	nodeState = ACS_APGCC_AMF_HA_QUIESCED;

	/* We were Active and now losting Active state due to Lock admin
	 * operation performed on our SU. 
	 * Inform the thread to go to "stop" state
	 */

	INFO(1, "%s", "acs_chbmtzlnd received QUIESCED state assignment!");

	/* Inform the thread to go "stop" state */	
	if ( !Is_terminated )
	{
		if( write(readWritePipe[1], &state, sizeof(state)) <= 0)
		{
			ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performStateTransitionToQuiescedJobs");
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToQueiscedJobs");
                        return ACS_APGCC_FAILURE;

		}
	}
	Is_terminated = TRUE;
#endif
	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performStateTransitionToQuiescedJobs");
	return rcode;
}

/*===================================================================
   ROUTINE: performComponentHealthCheck
=================================================================== */
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performComponentHealthCheck(void)
{
#if 0
	if( nodeState == ACS_APGCC_AMF_HA_ACTIVE  && (applicationThreadId > 0) )	//Functional Thread will be running on active node.
	{
		if( ThrExitHandler::CheckAllFuncThreads() == false )
		{
			ERROR(1, "%s", "Health Check failed");
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performComponentHealthCheck");
			return ACS_APGCC_FAILURE;
		}
			
	}
#endif
	return ACS_APGCC_SUCCESS;

}//End of performComponentHealthCheck

/*===================================================================
   ROUTINE: performComponentTerminateJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performComponentTerminateJobs(void)
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::performComponentTerminateJobs");
	INFO(1, "%s", "acs_chbmtzlnd received terminate callback!!!");
	rcode = this->passifyApp();
	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performComponentTerminateJobs");
	return rcode;
}//End of performComponentTerminateJobs

/*===================================================================
   ROUTINE: performComponentRemoveJobs
=================================================================== */
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performComponentRemoveJobs(void)
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::performComponentRemoveJobs");
	rcode = this->passifyApp();
#if 0
	ACE_TCHAR state[1] = {'S'};

	INFO(1, "%s", "acs_chbmtzlnd Assignment is removed now");

	/* Inform the thread to go "stop" state */	
	if ( !Is_terminated )
	{
		
		if(write(readWritePipe[1], &state, sizeof(state)) <= 0 )
		{
			ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performComponentRemoveJobs");
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performComponentRemoveJobs");
			return ACS_APGCC_FAILURE;
		}
	}
	Is_terminated = FALSE;
#endif
	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performComponentRemoveJobs");
	return rcode;
}//End of performComponentRemoveJobs

/*========================================================================
	ROUTINE: performApplicationShutdownJobs
========================================================================*/
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::performApplicationShutdownJobs() 
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::performApplicationShutdownJobs");
	rcode = this->passifyApp();
#if 0
	ERROR( 1, "%s", "Shutting down the application");

	ACE_TCHAR state[1] = {'S'};

	if ( !Is_terminated )
	{
		
		if(write(readWritePipe[1], &state, sizeof(state)) <= 0)
		{
			ERROR(1, "%s", "Write failed on pipe in ACS_CHB_MTZLN_Service::performApplicationShutdownJobs");
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performApplicationShutdownJobs");
			return ACS_APGCC_FAILURE;
		}
	}

	Is_terminated = FALSE;
#endif
	DEBUG(1, "%s", "Leaving ACS_CHB_MTZLN_Service::performApplicationShutdownJobs");
	return rcode;
}

ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::activateApp()
{

	DEBUG(1,"%s","Entering ACS_LM_HAService::activateApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if ( 0 != this->theMTZLNImplementer)
	{
		DEBUG(1, "%s","ACS_CHB_MTZLN_Service::activateApp application is already active");
		rCode = ACS_APGCC_SUCCESS;
	}
	else
	{
		ACE_TCHAR MTLNImplName[100], MTZClassName[100];
		ACE_OS::memset(MTLNImplName, 0, sizeof(MTLNImplName));
		ACE_OS::strcpy(MTLNImplName, ACS_CHB_MTZLN_IMPLEMENTER);

		ACE_OS::memset(MTZClassName,0, sizeof(MTZClassName));
		ACE_OS::strcpy(MTZClassName, ACS_CHB_MTZLN_IMM_CLASS_NM );

		ACE_NEW_NORETURN(this->theMTZLNImplementer, ACS_CHB_MTZLN_Implementer(MTZClassName, MTLNImplName, ACS_APGCC_ONE));
		if (0 == this->theMTZLNImplementer)
		{
			DEBUG(1, "%s","ACS_CHB_MTZLN_Service::activateApp failed to create the lm instance");
		}
		else
		{
			this->theMTZLNImplementer->setupMTZThread(theMTZLNImplementer);
			DEBUG(1, "%s","ACS_CHB_MTZLN_Service::activateApp ha-class: application is now activated by HA");
			rCode = ACS_APGCC_SUCCESS;
		}
	}
	DEBUG(1, "%s","Exiting ACS_CHB_MTZLN_Service::activateApp()");
	return rCode;
}


ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::passifyApp()
{

	DEBUG(1, "%s","Entering ACS_CHB_MTZLN_Service::passifyApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;

	if (0 == this->theMTZLNImplementer)
	{
		DEBUG(1, "%s","ACS_CHB_MTZLN_Service::passifyApp application is already passified");
		rCode = ACS_APGCC_SUCCESS;
	}
	else
	{
		DEBUG(1, "%s","ACS_CHB_MTZLN_Service::passifyApp Ordering mtzln Application to passify");
		this->theMTZLNImplementer->shutdown();

		DEBUG(1, "%s","ACS_CHB_MTZLN_Service::passifyApp Waiting for mtlzn Application to become passive...");
		//this->theMTZLNImplementer->wait();
		do
		{
			sleep(1);
		}
		while( ThrExitHandler::CheckNoThreads() == false );

		DEBUG(1, "%s","ACS_CHB_MTZLN_Service::passifyApp Deleting mtzln App instance...");
		delete this->theMTZLNImplementer;
		this->theMTZLNImplementer=0;
		DEBUG(1, "%s","ACS_CHB_MTZLN_Service::passifyApp App is now passivated by HA!!");
		rCode = ACS_APGCC_SUCCESS;
	}
	DEBUG(1, "%s","Exiting ACS_CHB_MTZLN_Service::passifyApp()");
	return rCode;
}
#if 0
/*========================================================================
	ROUTINE: mtzln_svc_run
========================================================================*/
ACE_THR_FUNC_RETURN mtzln_svc_run(void *ptr)
{
	DEBUG(1, "%s", "Entering mtzln_svc_run of acs_chbmtzlnd");

	ACS_CHB_MTZLN_Service *pMTZLNObj = (ACS_CHB_MTZLN_Service*) ptr;
	if( pMTZLNObj != 0)
	{
		DEBUG(1, "%s", "Invoking ACS_CHB_MTZLN_Service::svc");

		pMTZLNObj->mtzln_svc();

		DEBUG(1, "%s", "End of ACS_CHB_MTZLN_Service::svc");
	}
	else
	{
		ERROR(1, "%s", "Pointer to ACS_CHB_MTZLN_Service is NULL");
		ERROR(1, "%s", "Returning from mtzln_svc_run of acs_chbmtzlnd");
		return 0;
	}
	DEBUG(1, "%s", "Leaving mtzln_svc_run of acs_chbmtzlnd");
	return 0;	
}

/*========================================================================
	ROUTINE: mtzln_svc
========================================================================*/
ACS_APGCC_ReturnType ACS_CHB_MTZLN_Service::mtzln_svc()
{
	DEBUG(1, "%s", "Entering ACS_CHB_MTZLN_Service::::mtzln_svc");

	ACS_CC_ReturnType myReturnErrorCode;

	struct pollfd fds[1];
	nfds_t nfds = 1;
	ACE_INT32 ret;
	ACE_Time_Value timeout;

        ACE_INT32 retCode;

	INFO(1, "%s",  "Starting Application Thread for acs_chbmtzlnd");

	__time_t secs = 5;
	__suseconds_t usecs = 0;
	timeout.set(secs, usecs);

			
	fds[0].fd = readWritePipe[0];
	fds[0].events = POLLIN;

	while(true)
	{
		ret = ACE_OS::poll(fds, nfds, &timeout); // poll can also be a blocking call, such case timeout = 0

		if (ret == -1) 
		{
			if (errno == EINTR)
			{
				continue;
			}
			ERROR(1, "poll Failed - %s, Exiting...",strerror(errno));
			ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::::mtzln_svc");
			kill(getpid(), SIGTERM);
			return ACS_APGCC_FAILURE;
		}
		else if (ret == 0)
		{
			continue;
		}
		else if (fds[0].revents & POLLIN)
		{
			ACE_TCHAR ha_state[1] = {'\0'};
			ACE_TCHAR* ptr = (ACE_TCHAR*) &ha_state;
        		ACE_INT32 len = sizeof(ha_state);
			
			while (len > 0)
			{
                		retCode = read(readWritePipe[0], ptr, len);

                		if ( retCode < 0 && errno != EINTR)
				{
                        		ERROR(1, "Read interrupted by error: [%s]",strerror(errno));
					ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::::mtzln_svc");
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
                		ERROR(1, "Improper Msg Len Read [%d]", len);
				ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::::mtzln_svc");
				kill(getpid(), SIGTERM);
                		return ACS_APGCC_FAILURE;
        		}
			len = sizeof(ha_state);

			if (ha_state[0] == 'A')
			{
				INFO(1, "%s", "acs_chbmtzlnd Thread:: Application is Active");
				if(theMTZLNImplementer == 0)
				{
					ACE_TCHAR MTLNImplName[100], MTZClassName[100];
					ACE_OS::memset(MTLNImplName, 0, sizeof(MTLNImplName));
					ACE_OS::strcpy(MTLNImplName, ACS_CHB_MTZLN_IMPLEMENTER);

					ACE_OS::memset(MTZClassName,0, sizeof(MTZClassName));
					ACE_OS::strcpy(MTZClassName, ACS_CHB_MTZLN_IMM_CLASS_NM );

					theMTZLNImplementer = new ACS_CHB_MTZLN_Implementer(MTZClassName, MTLNImplName, ACS_APGCC_ONE);

					if( theMTZLNImplementer != 0 )
					{
					
						DEBUG(1, "%s", "Starting MTZThread");
						myReturnErrorCode = theMTZLNImplementer->setupMTZThread(theMTZLNImplementer);
						if(myReturnErrorCode == ACS_CC_FAILURE)
						{
							ERROR( 1, "%s", "Leaving ACS_CHB_MTZLN_Service::::mtzln_svc");
							delete theMTZLNImplementer;
							theMTZLNImplementer = 0;
							kill(getpid(), SIGTERM);
							return ACS_APGCC_FAILURE;
						}
						
					}
					else
					{
						ERROR(1, "%s", "Memory allocation failed for theMTZLNImplementer");
						ERROR(1, "%s", "Leaving ACS_CHB_MTZLN_Service::::mtzln_svc");
						kill(getpid(), SIGTERM);
						return ACS_APGCC_FAILURE;
					}
				}
				else
				{
					ERROR( 1, "%s", "Leaving ACS_CHB_MTZLN_Service::::mtzln_svc");
					kill(getpid(), SIGTERM);
					return ACS_APGCC_FAILURE;
				}
			}
			else if (ha_state[0] == 'P')
			{
				INFO( 1, "%s",  "acs_chbmtzlnd Thread:: Application is Passive");
				/** do nothing, just wait to get active **/
				continue;
			}
			else if (ha_state[0] == 'S')
			{
				INFO( 1, "%s",  "acs_chbmtzlnd Thread:: Request to stop application");
				/* Request to stop the thread, perform the gracefull activities here */
				if(theMTZLNImplementer != 0)
				{
					theMTZLNImplementer->shutdown();

					 //Wait until all threads have stopped.
                                        do
                                        {
                                                sleep(1);
                                        }
                                        while( ThrExitHandler::CheckNoThreads() == false );


					delete theMTZLNImplementer;
					theMTZLNImplementer = 0;
				}
				break;
			}
		}
		else
		{
			ERROR(1, "%s", "Error occured while listening on pipe read end");
			continue;
		}

	}
	
	INFO(1, "%s",  "Application Thread for acs_chbmtzlnd Terminated successfully");
	DEBUG( 1, "%s", "Leaving ACS_CHB_MTZLN_Service::::mtzln_svc");
	return ACS_APGCC_SUCCESS;
}
#endif
