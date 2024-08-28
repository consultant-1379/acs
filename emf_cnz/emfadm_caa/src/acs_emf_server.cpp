/*=================================================================== */
/**
   @file acs_emf_root_ObjectImpl.cpp

   Class method implementation for EMF module.

   This module contains the implementation of class declared in
   the EMF Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     XRAMMAT       Initial Release

=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string.h>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/ACE.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ACS_APGCC_Util.H>
#include "acs_emf_server.h"
#include "acs_emf_cmdclient.h"
#include "acs_emf_cmd.h"
#include "acs_emf_cmdserver.h"
#include "acs_emf_dsdserver.h"
#include "acs_emf_nanousbhandler.h"
#include <acs_apgcc_adminoperation.h>

//#include "acs_emf_ntfnotification.h"
using namespace std;

#define INFINITE        (((ACE_INT64)0)-1)  // Infinite timeout

// Local functions
static void PrintUsage(ostream& outs);

/*===================================================================
                        GLOBAL VARIABLES DECLARATION SECTION
=================================================================== */

/**
 * @brief	g_bInteractive
 */
bool  g_bInteractive     = FALSE;

/**
 * @brief	g_dwExecuteTimeout
 *          activeStatus
 */
ACE_UINT32 g_dwExecuteTimeout = INFINITE, activeStatus = 1;

/**
 * @brief	g_nHWVersion
 */
ACE_INT32   g_nHWVersion       = -1;

/**
 * @brief	pEmfHaServiceObj
 */
ACS_EMF_HAService *pEmfHaServiceObj = 0;
ACS_EMF_DSDServer* theDSDServerPtr = NULL;
extern const char* mediaOwnerFile;
//acs_emf_ntfnotification* theNTFNotificationPtr = NULL;
ACE_HANDLE stopThreadFds[2] = {ACE_INVALID_HANDLE, ACE_INVALID_HANDLE};
/*===================================================================
                        STATIC VARIABLES DECLARATION SECTION
=================================================================== */
static EMF_SERVICE_MODULE* g_pModule = NULL;
ACE_thread_t EMF_SERVICE_MODULE::applicationThreadId = 0;
EMF_SERVICE_MODULE* EMF_SERVICE_MODULE::m_poServiceHandler = 0;
ACE_Sig_Set signalSet;
/*===================================================================
   ROUTINE: PrintUsage
=================================================================== */
void PrintUsage(ostream& outs)
{
	outs << "Usage:\nacs_emfserverd -d for debug mode"<<endl;
}//End of PrintUsage

/*===================================================================
   ROUTINE: EMF_SERVICE_MODULE
=================================================================== */
EMF_SERVICE_MODULE::EMF_SERVICE_MODULE()
{
	// Set the service initial status
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EMF_SERVICE_MODULE - Entering");
	m_poServiceHandler = this;
	m_pMediaHandler = NULL;
	m_poReactor = 0;
	thetpReactorImpl = 0;
	m_poObjectImpl = 0;
	adminOperator = 0;
	m_s32TimerId = 0;
	m_haObj = 0;
	theEMFCmdServer = 0;
	theEMFCmdClient = 0;
	theEMFCmdSession = 0;
	// Get the hardware version: APG40C/2, APG40C/4 or APG43
	g_nHWVersion = ACS_EMF_Common::GetHWVersion();
	m_isStopSignalled = false;
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EMF_SERVICE_MODULE - Leaving");
}//End of EMF_SERVICE_MODULE
/*===================================================================
   ROUTINE: ~EMF_SERVICE_MODULE
=================================================================== */
EMF_SERVICE_MODULE::~EMF_SERVICE_MODULE()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::~EMF_SERVICE_MODULE - Entering");
	if(adminOperator != 0)
	{
		delete adminOperator;
		adminOperator = 0;
	}

	if (m_pMediaHandler != 0)
	{
		delete m_pMediaHandler;
		m_pMediaHandler = 0;
	}
	if( m_poObjectImpl != 0)
	{
		delete m_poObjectImpl;
		m_poObjectImpl = 0;
	}

	if(m_poReactor != 0)
	{
		delete m_poReactor;
		m_poReactor = 0;
	}
	if(thetpReactorImpl != 0)
	{
		delete thetpReactorImpl;
		thetpReactorImpl = 0;
	}
	if(theDSDServerPtr != NULL)
	{
		delete theDSDServerPtr;
		theDSDServerPtr = NULL;
	}
	if((ACS_EMF_Common::getNodeState() == 2) && (ACS_EMF_Common::GetHWVariant() >= 3)){
		DEBUG(1,"%s", "EMF_SERVICE_MODULE::~EMF_SERVICE_MODULE - NodeState==2, calling ACS_EMF_Common::unMountUSBMedia().");
		ACS_EMF_Common::umountUSBData();
	}

#if 0
	close(stopThreadFds[0]);
	stopThreadFds[0] = ACE_INVALID_HANDLE;
	close(stopThreadFds[1]);
	stopThreadFds[1] = ACE_INVALID_HANDLE;
#endif
	DEBUG(1,"%s","EMF_SERVICE_MODULE::~EMF_SERVICE_MODULE - Leaving");
}//End of ~EMF_SERVICE_MODULE
/*===================================================================
   ROUTINE: active 
=================================================================== */
int EMF_SERVICE_MODULE::active(ACS_EMF_HAService *haObj)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::active - Entering");
	ACE_OS::pipe(stopThreadFds);
	m_isStopSignalled = false;
	m_haObj = haObj;
	ACS_CC_ReturnType myReturnErrorCode = setupEMFThreadinActive(this);
	if(myReturnErrorCode != ACS_CC_FAILURE)
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::active - Active EMF Application Thread spawned successfully....");
		return ACS_APGCC_SUCCESS;
	}

	DEBUG(1,"%s","EMF_SERVICE_MODULE::active - EMF Application Thread failed to spawn.");
	close(stopThreadFds[0]);
	close(stopThreadFds[1]);
	ERROR(1, "%s", "EMF_SERVICE_MODULE::active - Leaving");
	return ACS_APGCC_FAILURE;
}//end of active
/*===================================================================
   ROUTINE: passive
=================================================================== */
int EMF_SERVICE_MODULE::passive(ACS_EMF_HAService *haObj)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::passive - Entering");
	ACE_OS::pipe(stopThreadFds);
	m_isStopSignalled = false;
	m_haObj = haObj;
	ACS_CC_ReturnType myReturnErrorCode = setupEMFThreadinPassive(this);
	if(myReturnErrorCode != ACS_CC_FAILURE)
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::passive - Passive EMF Application Thread spawned successfully....");
		return ACS_APGCC_SUCCESS;
	}

	DEBUG(1,"%s","EMF_SERVICE_MODULE::passive - EMF Application Thread failed to spawn.");
	close(stopThreadFds[0]);
	close(stopThreadFds[1]);
	DEBUG(1, "%s", "EMF_SERVICE_MODULE::passive - Leaving");
	return ACS_APGCC_FAILURE;

}//end of passive
void EMF_SERVICE_MODULE::stop()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::stop - Entering");
	shutdown();
	//sleep(1);
	DEBUG(1,"%s","EMF_SERVICE_MODULE::stop - Leaving");
}//end of stop
/*===================================================================
   ROUTINE: EmfInfoCommandHandler
=================================================================== */
ACE_THR_FUNC_RETURN EMF_SERVICE_MODULE::EmfCopyCommandHandlerProc(void * lpvThis)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandlerProc - Entering");
	((EMF_SERVICE_MODULE*)lpvThis)->EmfCopyCommandHandler();
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandlerProc - Leaving");
	return 0;
}//End of EmfInfoCommandHandler
/*===================================================================
   ROUTINE: EmfCopyCommandHandler
=================================================================== */
bool EMF_SERVICE_MODULE::EmfCopyCommandHandler()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandler - Entering");
	if (activeStatus == 1/*ACS_EMF_Common::CheckActiveNode()*/)
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandler - Registering of OI for Emfcopy");
		std::string dnName;
		//		dnName.append(theEMFCOPYRDNForNODEA);
		//		dnName.append(",");
		dnName.append(ACS_EMF_Common::parentDNofEMF);
		DEBUG(1,"EMF_SERVICE_MODULE::EmfCopyCommandHandler - Registering DN for EMFCOPY on NodeA is %s",dnName.c_str());
		adminOperator = new acs_emf_commandhandler(m_pMediaHandler,stopThreadFds[0],ACE_TEXT("acs_emf_impl"));
		//	adminOperator->register_object(m_poReactor);
		//adminOperator = new acs_emf_commandhandler(m_pMediaHandler,stopThreadFds[0],dnName,ACE_TEXT("acs_emf_impl"),ACS_APGCC_ONE);
		adminOperator->register_object();
		theDSDServerPtr->setCommandhandler(adminOperator);
		if(adminOperator->activate()== -1)
		{
			DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandler - Command handler thread activation failed");
			DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandler - Leaving");
			return false;
		}
	}
	else
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandler - Node is not Active.Registering OI for EMFcopy is not Done\n");
		DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandler - Leaving");
		return false;
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EmfCopyCommandHandler - Leaving");
	return true;
}//End of EmfCopyCommandHandler
/*===================================================================
   ROUTINE: Start
=================================================================== */
inline void EMF_SERVICE_MODULE::Start()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::Start - Entering");
	if (g_bInteractive)
	{
		// Run interactive or in debug from the command line
		ACE_OS::pipe(stopThreadFds);
		Run();
	}
	else
	{
		// Run as service
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::Start - Leaving");
}//End of Start
/*===================================================================
   ROUTINE: s32Spawn
=================================================================== */
ACE_INT32 EMF_SERVICE_MODULE::s32Spawn(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
		ACE_TCHAR* lpszThreadName,ACE_INT32 s32GrpId)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::s32Spawn - Entering");
	ACE_INT32 s32Result =  ACE_Thread_Manager::instance()->spawn( ThreadFunc,
			Arglist,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			0,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			s32GrpId,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName);

	DEBUG(1,"%s","EMF_SERVICE_MODULE::s32Spawn - Leaving");
	return s32Result;
}//End of s32Spawn
/*===================================================================
   ROUTINE: StartThread
=================================================================== */
ACE_INT32 EMF_SERVICE_MODULE::StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
		void* Arglist,
		const ACE_TCHAR* lpszThreadName,
		ACE_INT32 s32GrpId)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::StartThread - Entering");
	ACE_INT32 s32Result =  s32Spawn( ThreadFunc,
			Arglist,
			lpszThreadName,
			s32GrpId);
	if ( s32Result == -1 )
	{
		// Report error
		ACE_TCHAR szData[256];

		snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Unable to create thread: %s due to the following error:\n %m"),
				lpszThreadName);

		ACE_DEBUG((LM_INFO, ACE_TEXT(szData)));

		//! Send event
		ACS_EMF_AEH::ReportEvent(EMF_EV_EVENTNUM_INTERNALERROR,
				ACE_TEXT("EVENT"),
				EMF_EV_EVENTTEXT_FAULT,
				ACE_TEXT(""),
				szData,
				EMF_EV_EVENTTEXT_INTERNALERROR);
	}
	else
	{
		DEBUG(1,"EMF_SERVICE_MODULE::StartThread - [%s] thread is created successfully",lpszThreadName);
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::StartThread - Leaving");
	return s32Result;
}//End of StartThread
/*===================================================================
   ROUTINE: Run
=================================================================== */
void EMF_SERVICE_MODULE::Run()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Entering");
	/*	theNTFNotificationPtr = new acs_emf_ntfnotification(stopThreadFds[0]);
		if(theNTFNotificationPtr != NULL)
		{
			theNTFNotificationPtr->start();
		}*/
	// WORKARROUND TO AVOID TIPC SUBSCRIPTION ISSUE
	//	OmHandler dummyOm;
	//	if (dummyOm.Init() == ACS_CC_FAILURE)
	//	{
	//		DEBUG(1,"%s","OmHandler Init is failed in Run()");
	//	}
	// END
	DEBUG(1,"EMF_SERVICE_MODULE::Run - g_nHWVersion=[%d]",g_nHWVersion);
	switch (g_nHWVersion)
	{
	case 1:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Hardware version of Node: APG40C/2\n");
		break;
	case 2:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Hardware version of Node: APG40C/4\n");
		break;
	case 3:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Hardware version of Node: APG43\n");
		break;
	default:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Hardware version of Node: Undefined\n");
	}

	if (ACS_EMF_Common::fetchDnOfRootObjFromIMM() == -1)
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Unable to fetch DN of EMF root from IMM.");
		DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - EMF IMM files are not configured properly. Hence exiting from Service ...\n");
		DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Leaving");
		return;
	}
	ACS_EMF_Common::createDirectory(ACS_EMF_MNT_DIR);
	m_pMediaHandler = ACS_EMF_MEDIAHandler::create(ACS_EMF_Common::GetHWVariant(),stopThreadFds[0]);
	if (!m_pMediaHandler)
	{
		// Report error
		ACE_TCHAR szData[256];
		sprintf(szData,
				ACE_TEXT("EMF_SERVICE_MODULE::Run - Failed to create class instance of type \"ACS_EMF_MEDIAHandler\".\r\nError: %s"),
				ACE_TEXT("EMF ERROR")); // ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));
		DEBUG(1,"%s",szData);
		// Send event
		ACS_EMF_AEH::ReportEvent(EMF_EV_EVENTNUM_INTERNALERROR,
				ACE_TEXT("EVENT"),
				EMF_EV_EVENTTEXT_FAULT,
				ACE_TEXT(""),
				szData,
				EMF_EV_EVENTTEXT_INTERNALERROR);
			DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Leaving");
		return;
	}
	thetpReactorImpl = new ACE_TP_Reactor;
	m_poReactor = new ACE_Reactor(thetpReactorImpl);
	//! if m_u64ExecuteTimeout=0 Service runs in interactive mode until terminated
	if( g_bInteractive && ( g_dwExecuteTimeout != 0 ) )
		m_s32TimerId = setTimeoutForInteractiveMode( g_dwExecuteTimeout );
	// Register for IMM callbacks
	if(ACS_EMF_Common::getNodeState() == 1)
	{
		// ACTIVE NODE
		
#if 0 // Inactive code no improvement
		ACE_INT32 s32Result = StartThread(EmfCopyCommandHandlerProc, (void *)this, ACE_TEXT("EmfCopyCommandHandlerProc"));
		if (s32Result == -1)
		{
			DEBUG(1,"%s","Unable to create emfcopy Command Handler thread\n");
			return;
		}
#endif

		if(!EmfCopyCommandHandler())
		{
			DEBUG(1,"%s","Unable to create emfcopy Command Handler thread\n");
			DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Leaving");
			return;
		}
	}
	else //Activation of dsd communication done in lsv14 irrespective of getNodeState()
	{
		if(ACS_EMF_Common::GetHWVariant() >= 3)
		{
			// PASSIVE NODE
			// Start DSD Server.
			theDSDServerPtr = new ACS_EMF_DSDServer(stopThreadFds[0]);
			theDSDServerPtr->setClienthandler(m_pMediaHandler);
			//Activate DSD Communication Thread
			theDSDServerPtr->start();
		}
	}
	if(ACS_EMF_Common::GetHWVariant() < 3)
	{
		// Start DSDS Server on both Node for Dual AP
		theDSDServerPtr = new ACS_EMF_DSDServer(stopThreadFds[0]);
		theDSDServerPtr->setClienthandler(m_pMediaHandler);
		//Activate DSD Communication Thread
		theDSDServerPtr->start();
	}
	//	if(ACS_EMF_Common::GetHWVariant() == 3)
	//	{
	//		if(ACS_EMF_Common::getNodeState() == 1 )
	//		{
	//			m_pMediaHandler->checksumForActive();
	//		}
	//	}
	//! Register to receive shutdown signals
	setupShutdownHandler( m_poReactor );
	//! Start reactor event loop
	m_poReactor->open(1);
	m_poReactor->run_reactor_event_loop();
	//dummyOm.Finalize();
	// Let the threads have some time to terminate gracefully

	DEBUG(1,"%s","EMF_SERVICE_MODULE::Run - Leaving");
}//End of Run
/*===================================================================
   ROUTINE: handle_timeout
=================================================================== */
int EMF_SERVICE_MODULE::handle_timeout(const ACE_Time_Value &current_time, const void *)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::handle_timeout - Entering");
	DEBUG(1,"%ld",current_time.msec());
	if( g_bInteractive )
	{
		DEBUG(1,"EMF_SERVICE_MODULE::handle_timeout - Timeout For Interactive mode received for EMF PID:%d \n",getpid());
		ACE_OS::kill(ACE_OS::getpid(), SIGTERM);
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::handle_timeout - Leaving");
	return 0;
}//End of handle_timeout
/*===================================================================
   ROUTINE: handle_signal
=================================================================== */
int EMF_SERVICE_MODULE::handle_signal(ACE_INT32 signum, siginfo_t * siginfo, ucontext_t * ucontext)
{
	
	DEBUG(1,"%s","EMF_SERVICE_MODULE::handle_signal - Entering");
	DEBUG(1,"EMF_SERVICE_MODULE::handle_signal - Received Signal %d siginfo %d uc_flags %ld",signum,siginfo->si_signo,ucontext->uc_flags);

	if( !m_poServiceHandler )
	{
		ERROR(1,"EMF_SERVICE_MODULE::handle_signal - %s","m_poServiceHandler is NULL\n");
		return 0;
	}
	if(signum == SIGINT && ( g_bInteractive && ( g_dwExecuteTimeout != 0 ) ))
		s32ResetTimer(m_s32TimerId);
	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
		//Report event
		ACE_TCHAR szData[256];
		snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("EMF_SERVICE_MODULE::handle_signal - EMF Monitor service terminated as requested by the user"));
		// Send Event
		ACS_EMF_AEH::ReportEvent(EMF_EV_EVENTNUM_INTERNALERROR,
				ACE_TEXT("EVENT"),
				EMF_EV_EVENTTEXT_FAULT,
				ACE_TEXT(""),
				szData,
				EMF_EV_EVENTTEXT_INTERNALERROR);
		DEBUG(1,"%s\n",szData);
		m_poServiceHandler->shutdown();
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::handle_signal - Leaving");
	return 0;
}//End of handle_signal
/*===================================================================
   ROUTINE: setupIMMCallBacks
=================================================================== */
bool EMF_SERVICE_MODULE::setupIMMCallBacks( ACE_Reactor *poReactor )
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupIMMCallBacks - Entering");
	// Registering OI for EMF ROOT class
	m_poObjectImpl = new ACS_EMF_ObjectImpl(ACS_EMF_ROOT_IMPL_NAME);
	m_poObjectImpl->svc(poReactor);
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupIMMCallBacks - Leaving");
	return true;
}//End of setupIMMCallBacks
/*===================================================================
   ROUTINE: setupShutdownHandler
=================================================================== */
bool EMF_SERVICE_MODULE::setupShutdownHandler( ACE_Reactor *poReactor )
{
	//! Register SIGINT
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupShutdownHandler - Entering");
	if( g_bInteractive )
		signalSet.sig_add(SIGINT);
	signalSet.sig_add( SIGTSTP );
	signalSet.sig_add( SIGTERM );
	//! Signal register for USR1 for running alarm
	signalSet.sig_add( SIGUSR1 );
	//! Register the signal_handler method to process all the signals in sigset
	poReactor->register_handler( signalSet, this );
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupShutdownHandler - ShutdownHandler setup successful\n");
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupShutdownHandler - Leaving");
	return TRUE;
}//End of setupShutdownHandler
/*===================================================================
   ROUTINE: setTimeoutForInteractiveMode
=================================================================== */
ACE_INT32 EMF_SERVICE_MODULE::setTimeoutForInteractiveMode( ACE_UINT64  u64TimeOut)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setTimeoutForInteractiveMode - Entering");
	DEBUG(1,"setTimeOutForInteractiveMode %ld \n",u64TimeOut);
	ACE_Time_Value exptime(u64TimeOut);
	ACE_INT32 s32TimerId = ACE_Reactor::instance()->schedule_timer( this ,
			0,
			exptime
	);

	DEBUG(1,"EMF_SERVICE_MODULE::setTimeoutForInteractiveMode - Timer created: %s \n",((s32TimerId==-1)?"Failure":"Successful"));
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setTimeoutForInteractiveMode - Leaving");
	return s32TimerId;
}//End of setTimeoutForInteractiveMode
/*===================================================================
   ROUTINE: s32ResetTimer
=================================================================== */
ACE_INT32 EMF_SERVICE_MODULE::s32ResetTimer(ACE_INT32 s32Timer)
{
	ACE_ASSERT( s32Timer!=-1 );
	DEBUG(1,"%s","EMF_SERVICE_MODULE::s32ResetTimer - Entering");
	ACE_INT32 s32Result = ACE_Reactor::instance()->cancel_timer(s32Timer);
	DEBUG(1,"EMF EMF_SERVICE_MODULE::bResetTimer :  %s \n",((s32Result==1)?"SUCCESS":"FAILURE"));
	DEBUG(1,"%s","EMF_SERVICE_MODULE::s32ResetTimer - Leaving");
	return s32Result;
}//End of s32ResetTimer
/*===================================================================
   ROUTINE: shutdown
=================================================================== */
int EMF_SERVICE_MODULE::shutdown()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::shutdown - Entering");
	DEBUG(1,"EMF_SERVICE_MODULE::shutdown - trigger for EMF Monitor Service PID: %d \n",getpid());
	char buf[] = { 1 , 1};
	int bytes = 0;
	int ret=0;
	m_isStopSignalled = true;
	ACE_INT32 nodeState = ACS_EMF_Common::getNodeState();
#if 0 
	if ((ACS_EMF_Common::GetHWVariant() < 3) && (m_pMediaHandler->m_mediaAttachedLocally))
		disableMediaOnDvdOwnerNode(nodeState);
#endif
	if(m_pMediaHandler != 0)
		m_pMediaHandler->stopMediaOperation();
	if (ACS_EMF_Common::killAllOutstandingPIDs() == -1)
		ERROR(1,"%s","EMF_SERVICE_MODULE::shutdown - Killing outstanding PIDs failed\n");
	bytes = ACE_OS::write(stopThreadFds[1], buf, sizeof(buf ));
	if( bytes <= 0 )
	{
		ERROR(1,"%s","EMF_SERVICE_MODULE::shutdown - Error occurred while signaling stop event");
	}
	if (ACS_EMF_Common::killAllOutstandingPIDs() == -1)
		ERROR(1,"%s","EMF_SERVICE_MODULE::shutdown - Killing outstanding PIDs failed\n");
	if(adminOperator!=0 )
		adminOperator->shutdown();
	if (ACS_EMF_Common::killAllOutstandingPIDs() == -1)
		ERROR(1,"%s","EMF_SERVICE_MODULE::shutdown - Killing outstanding PIDs failed\n");
	if(theDSDServerPtr != NULL)
	{
		if(theDSDServerPtr->isDSDPublished == true)
		{
			if(theDSDServerPtr->wait() == -1)
			{
				DEBUG(1,"%s","EMF_SERVICE_MODULE::shutdown - Failed to close dsd server thread");
			}
		}
		theDSDServerPtr->stop();
	}
	if(m_poReactor !=0)
		ret = m_poReactor->end_reactor_event_loop();
	if(applicationThreadId != 0)
		ACE_Thread_Manager::instance()->join( applicationThreadId);
	close(stopThreadFds[0]);
	close(stopThreadFds[1]);
	//if (ACS_EMF_Common::getNodeState() == 2){
	if ((2 == nodeState) && (ACS_EMF_Common::GetHWVariant() >= 3)){
		ERROR(1,"%s","EMF_SERVICE_MODULE::shutdown - stop signal received - Disabling EMF function");
		std::string dnNameOfNodeObj;
		dnNameOfNodeObj.append(ACS_EMF_Common::parentDNofEMF);
		acs_apgcc_adminoperation admOp;
		std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
		std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
		int returnValue;
		long long int timeOutVal_3sec = 3*(1000000000LL);
		ACS_CC_ReturnType result;
		result = admOp.init();
		if (result != ACS_CC_SUCCESS)
		{
			DEBUG(1,"%s","EMF_SERVICE_MODULE::shutdown - EMF admin instance initialization is failed!!");
		}
		else
		{
			result = admOp.adminOperationInvoke(dnNameOfNodeObj.c_str(), 0, EMF_CMD_TYPE_CLEANUPANDLOCK, vectorIN, &returnValue, timeOutVal_3sec, vectorOut);
			if (result != ACS_CC_SUCCESS)
			{
				DEBUG(1,"EMF_SERVICE_MODULE::shutdown - FAILED to invoke AdminOperation on %s",dnNameOfNodeObj.c_str());
				DEBUG(1,"EMF_SERVICE_MODULE::shutdown - ERROR CODE = %d",admOp.getInternalLastError());
				DEBUG(1,"EMF_SERVICE_MODULE::shutdown - ERROR MESSAGE = %s",admOp.getInternalLastErrorText());
				admOp.finalize();
				DEBUG(1,"%s", "EMF_SERVICE_MODULE::shutdown - Leaving");
				//return ACS_CC_FAILURE;
			}
			admOp.finalize();
		}
	}

	//During passive node shutdown, NFS has to be removed and /media folder has to be unmounted for GEP5 HW.
	if (ACS_EMF_Common::GetHWVariant() >= 3){
		bool bIsShutDown = true;
		if(ACS_EMF_Common::unmountPassiveDVDData(bIsShutDown) != 0)
                	ERROR(1,"%s","EMF_SERVICE_MODULE::shutdown - Unmounting and removing NFS share on passive failed ");
	}
	DEBUG(1,"%s", "EMF_SERVICE_MODULE::shutdown - Leaving");
	return ret;
}//End of shutdown
/*===================================================================
   ROUTINE: setupEMFThreadinActive
=================================================================== */
ACS_CC_ReturnType EMF_SERVICE_MODULE::setupEMFThreadinActive(EMF_SERVICE_MODULE * aEMFPtr)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupEMFThreadinActive - Entering");
	const ACE_TCHAR* lpszThreadName1 = "EMFFunctionalThread";
	ACE_HANDLE mythread = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;
	mythread = ACE_Thread_Manager::instance()->spawn(&EMFThreadFunctionalMethod,
			(void*)aEMFPtr ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&applicationThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName1);
	if(mythread == -1)
	{
		ERROR(1,"%s","EMF_SERVICE_MODULE::setupEMFThreadinActive - Failure occurred while spawning EMF Functional Thread");
		rc = ACS_CC_FAILURE;
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupEMFThreadinActive - Leaving");
	return rc;
}//End of setupEMFThreadinActive

/*===================================================================
   ROUTINE: setupEMFThreadinPassive
=================================================================== */
ACS_CC_ReturnType EMF_SERVICE_MODULE::setupEMFThreadinPassive(EMF_SERVICE_MODULE * aEMFPtr)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupEMFThreadinPassive - Entering");
	const ACE_TCHAR* lpszThreadName1 = "EMFFunctionalThread";
	ACE_HANDLE mythread = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;
	mythread = ACE_Thread_Manager::instance()->spawn(&EMFPassiveThreadMethod,
			(void*)aEMFPtr ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&applicationThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName1);
	if(mythread == -1)
	{
		ERROR(1,"%s","EMF_SERVICE_MODULE::setupEMFThreadinPassive - Failure occurred while spawning EMF Functional Thread");
		rc = ACS_CC_FAILURE;
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::setupEMFThreadinPassive - Leaving");
	return rc;
}//End of setupEMFThreadinPassive
/*===================================================================
   ROUTINE: EMFThreadFunctionalMethod
=================================================================== */
ACE_THR_FUNC_RETURN EMF_SERVICE_MODULE::EMFPassiveThreadMethod(void* aEMFPtr)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EMFPassiveThreadMethod - Entering");
	EMF_SERVICE_MODULE *myEMFPtr = NULL;
	myEMFPtr	= reinterpret_cast<EMF_SERVICE_MODULE*>(aEMFPtr);
	if(myEMFPtr == NULL)
	{
		return NULL;
	}
	if((myEMFPtr->startEMFFunctionalityinPassive() == ACS_CC_FAILURE))
	{
		ERROR(1,"%s","EMF_SERVICE_MODULE::EMFPassiveThreadMethod - Failure occurred while starting EMF Functionality");
		return NULL;
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EMFPassiveThreadMethod - Leaving");
	return aEMFPtr;
}//End of EMFThreadFunctionalMethod
/*===================================================================
   ROUTINE: EMFThreadFunctionalMethod
=================================================================== */
ACE_THR_FUNC_RETURN EMF_SERVICE_MODULE::EMFThreadFunctionalMethod(void* aEMFPtr)
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EMFThreadFunctionalMethod - Entering");
	EMF_SERVICE_MODULE *myEMFPtr = NULL;
	myEMFPtr	= reinterpret_cast<EMF_SERVICE_MODULE*>(aEMFPtr);
	if(myEMFPtr == NULL)
		return NULL;
	if((myEMFPtr->startEMFFunctionalityinActive() == ACS_CC_FAILURE))
	{
		ERROR(1,"%s","Failure occurred while starting EMF Functionality\n");
		return NULL;
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::EMFThreadFunctionalMethod - Leaving");
	return aEMFPtr;
}//End of EMFThreadFunctionalMethod
/*===================================================================
   ROUTINE: startEMFFunctionalityinActive
=================================================================== */
ACS_CC_ReturnType EMF_SERVICE_MODULE::startEMFFunctionalityinActive()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Entering");
	//////////////////////////////////////////////////////////////
	// WORKARROUND TO AVOID TIPC SUBSCRIPTION ISSUE
	OmHandler dummyOm;
	if (dummyOm.Init() == ACS_CC_FAILURE)
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - OmHandler Init is failed in Run()");
	// END
	switch (g_nHWVersion)
	{
	case 1:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Hardware version of Node: APG40C/2\n");
		break;
	case 2:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Hardware version of Node: APG40C/4\n");
		break;
	case 3:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Hardware version of Node: APG43\n");
		break;
	default:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Hardware version of Node: Undefined\n");
	}
	if (ACS_EMF_Common::fetchDnOfRootObjFromIMM() == -1)
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Unable to fetch DN of EMF root from IMM.");
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Leaving");
		return ACS_CC_FAILURE;
	}
	if(m_isStopSignalled)
	{
		ERROR(1,"%s","Service stop requested ");
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Leaving");
		return ACS_CC_FAILURE;
	}
	ACS_EMF_Common::createDirectory(ACS_EMF_MNT_DIR);
	if(ACS_EMF_Common::createEMFFolder() != 0){
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - call 'createEMFFolder' failed! EMF cannot create <datadiskpath>/emf folder");
	}

	m_pMediaHandler = ACS_EMF_MEDIAHandler::create(ACS_EMF_Common::GetHWVariant(),stopThreadFds[0]);
	if (!m_pMediaHandler)
	{
		// Report error
		ACE_TCHAR szData[256];
		sprintf(szData,
				ACE_TEXT("Failed to create class instance of type \"ACS_EMF_MediaHandler\".\r\nError: %s"),
				ACE_TEXT("EMF ERROR")); // ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));

		DEBUG(1,"EMF_SERVICE_MODULE::startEMFFunctionalityinActive - %s",szData);

		// Send event
		ACS_EMF_AEH::ReportEvent(EMF_EV_EVENTNUM_INTERNALERROR,
				ACE_TEXT("EVENT"),
				EMF_EV_EVENTTEXT_FAULT,
				ACE_TEXT(""),
				szData,
				EMF_EV_EVENTTEXT_INTERNALERROR);
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Leaving");
		return ACS_CC_FAILURE;
	}
	thetpReactorImpl = new ACE_TP_Reactor;
	m_poReactor = new ACE_Reactor(thetpReactorImpl);
	//! if m_u64ExecuteTimeout=0 Service runs in interactive mode until terminated
	if( g_bInteractive && ( g_dwExecuteTimeout != 0 ) )
		m_s32TimerId = setTimeoutForInteractiveMode( g_dwExecuteTimeout );

	//DSD Server is activated even on Active Node for feature of keeping EMF functionality in ENABLED state even after Node Reboots/Switchover.
	//if(ACS_EMF_Common::GetHWVariant() != 3)
	//{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Register to DSD on active node")
 		// Start DSD Server.
        	theDSDServerPtr = new ACS_EMF_DSDServer(stopThreadFds[0]);
		theDSDServerPtr->setClienthandler(m_pMediaHandler);
		theDSDServerPtr->start();
	//}

		theEMFCmdClient = new ACS_DSD_Client();
		theEMFCmdClient->get_local_node(m_LocalNode);
		if(theEMFCmdClient != NULL){
			delete theEMFCmdClient;
			theEMFCmdClient = NULL;
		}
	// Register for IMM callbacks
	// ACTIVE NODE
	if(!EmfCopyCommandHandler())
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Unable to create emfcopy Command Handler thread\n");
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Leaving");
		return ACS_CC_FAILURE;
	}
	
#if 0 // Disabled code
	ACE_INT32 s32Result = StartThread(EmfCopyCommandHandlerProc, (void *)this, ACE_TEXT("EmfCopyCommandHandlerProc"));
	if (s32Result == -1)
	{
		DEBUG(1,"%s","Unable to create emfcopy Command Handler thread");
		return ACS_CC_FAILURE;
	}
#endif

	//! Start reactor event loop
	m_poReactor->open(1);
	m_poReactor->run_reactor_event_loop();
	dummyOm.Finalize();
	/////////////////////////////////////////////////////////////
	DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinActive - Leaving");
	return ACS_CC_SUCCESS;
}//End of startEMFFunctionalityinActive
/*===================================================================
   ROUTINE: startEMFFunctionalityinPassive
=================================================================== */
ACS_CC_ReturnType EMF_SERVICE_MODULE::startEMFFunctionalityinPassive()
{
	DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Entering");
	//////////////////////////////////////////////////////////////
	// WORKARROUND TO AVOID TIPC SUBSCRIPTION ISSUE
	OmHandler dummyOm;
	if (dummyOm.Init() == ACS_CC_FAILURE)
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - OmHandler Init is failed in Run()");
	// END
	switch (g_nHWVersion)
	{
	case 1:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Hardware version of Node: APG40C/2\n");
		break;
	case 2:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Hardware version of Node: APG40C/4\n");
		break;
	case 3:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Hardware version of Node: APG43\n");
		break;
	default:
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Hardware version of Node: Undefined\n");
	}
	if (ACS_EMF_Common::fetchDnOfRootObjFromIMM() == -1)
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Unable to fetch DN of EMF root from IMM.");
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Leaving");
		return ACS_CC_FAILURE;
	}


	// To find ehether EMF Feature is enabled or not
	std::string dnNameOfNodeObj;
	dnNameOfNodeObj.append(ACS_EMF_Common::parentDNofEMF);
	OmHandler* omHandlerPtr = new OmHandler();
	if( omHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - FAILED to init OmHandler for reading operationalState");
		// return ACS_CC_FAILURE;
	}
	ACS_CC_ImmParameter* paramToFindPtr = new ACS_CC_ImmParameter();
	paramToFindPtr->attrName = new ACE_TCHAR [30];
	int operState = 0, mediaState = 0;
	ACS_CC_ReturnType enResult;
	ACE_OS::strcpy( paramToFindPtr->attrName , theEMFOperationalState);
	if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNodeObj.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS ){
		if (paramToFindPtr->attrValuesNum != 0)
			operState = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));
	}
	else
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - failed to fetch operationalState from EMF MO!!");

	ACE_OS::strcpy( paramToFindPtr->attrName , theEMFMediaState);
	if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNodeObj.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS ){
                if (paramToFindPtr->attrValuesNum != 0)
                        mediaState = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));
        }
        else
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - failed to fetch MediaState from EMF MO!!");

	delete [] paramToFindPtr->attrName;
        omHandlerPtr->Finalize();
	omHandlerPtr->Finalize();

	ACS_EMF_Common::createDirectory(ACS_EMF_MNT_DIR);
	m_pMediaHandler = ACS_EMF_MEDIAHandler::create(ACS_EMF_Common::GetHWVariant(),stopThreadFds[0]);
	if (!m_pMediaHandler)
	{
		// Report error
		ACE_TCHAR szData[256];
		sprintf(szData,
				ACE_TEXT("Failed to create class instance of type \"ACS_EMF_DVDHandler\".\r\nError: %s"),
				ACE_TEXT("EMF ERROR")); // ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));

		DEBUG(1,"EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - %s",szData);
		// Send event
		ACS_EMF_AEH::ReportEvent(EMF_EV_EVENTNUM_INTERNALERROR,
				ACE_TEXT("EVENT"),
				EMF_EV_EVENTTEXT_FAULT,
				ACE_TEXT(""),
				szData,
				EMF_EV_EVENTTEXT_INTERNALERROR);
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Leaving");
		return ACS_CC_FAILURE;
	}
	thetpReactorImpl = new ACE_TP_Reactor;
	m_poReactor = new ACE_Reactor(thetpReactorImpl);

	//! if m_u64ExecuteTimeout=0 Service runs in interactive mode until terminated
	if( g_bInteractive && ( g_dwExecuteTimeout != 0 ) )
		m_s32TimerId = setTimeoutForInteractiveMode( g_dwExecuteTimeout );
	// Register for IMM callbacks
	// PASSIVE NODE
	// Start DSD Server.
	theDSDServerPtr = new ACS_EMF_DSDServer(stopThreadFds[0]);
	theDSDServerPtr->setClienthandler(m_pMediaHandler); //this is commented in lsv14 an thread started in startEMFFunctionalityinActive
	theDSDServerPtr->start();//Activate DSD Communication Thread

	theEMFCmdClient = new ACS_DSD_Client();
	theEMFCmdClient->get_local_node(m_LocalNode);
	if(theEMFCmdClient != NULL){
		delete theEMFCmdClient;
		theEMFCmdClient = NULL;
	}

	if(ACS_EMF_Common::GetHWVariant() >= 3) 
	{	
		if((operState == MEDIA_FEATURE_ENABLE) && (mediaState == MEDIA_FEATURE_ENABLE)){
			if(ACS_EMF_Common::checkForUSBStatus() == EMF_RC_OK) {
				if (EMF_RC_OK == ACS_EMF_Common::mountMedia()) {
					DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Media attatched to this node and Waiting for DSD server to publish");
					int retry = 12;
					while(retry--){
						if(theDSDServerPtr->isDSDPublished)
							break;
						else{
							sleep(5);
						}
					}
					DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - DSD Published and Performing Unlock Media");

					if( EMF_RC_OK != m_pMediaHandler->enableMediaOnAccessibleNode(mediaState)){
						DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Unlock Media Failed");
					}
				}
				else{
					DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - mount Media Failed");
				}
			}
                	else {
				DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - No USB Connected to Passive Node.");
			}
		}
		else if ((operState == MEDIA_FEATURE_DISABLE) && (mediaState == MEDIA_FEATURE_ENABLE)){
               		if(ACS_EMF_Common::checkForUSBStatus() == EMF_RC_OK) {
				DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Media attatched to this node and Waiting for DSD server to publish");
				int retry = 12;
				while(retry--){
					if(theDSDServerPtr->isDSDPublished)
						break;
					else{
						sleep(5);
					}
				}
				DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - DSD Published and Performing Unlock Media");

				if( EMF_RC_OK != m_pMediaHandler->enableMediaOnAccessibleNode(mediaState)){
					DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Unlock Media Failed");
				}
			}
                	else {
				DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - No USB Connected to Passive Node.");
			}
		}
	}
	else{
		//DVD case
		std::string dev_info;
		ACE_INT16 iTmp = ACS_EMF_Common::checkForDVDStatus(dev_info);
		if ((iTmp & CHECKDVD_MASK_PRESENT ) != 0){
			m_pMediaHandler->m_mediaAttachedLocally = true;
		}

                if (m_pMediaHandler->m_mediaAttachedLocally) 
		{
			if ( (ACE_OS::access(mediaOwnerFile, F_OK) == 0) || (mediaState == MEDIA_FEATURE_ENABLE) ) {
				DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - DVD attatched to this node and Waiting for DSD server to publish");
				int retry = 12;
				while(retry--){
					if(theDSDServerPtr->isDSDPublished)
						break;
					else{
						sleep(5);
					}
				}
				DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - DSD Published and Performing Unlock Media");

				if( EMF_RC_OK != m_pMediaHandler->enableMediaOnAccessibleNode(mediaState)){
					DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Unlock Media Failed");
				}
			}


		}
	}
	//! Register to receive shutdown signals
	//setupShutdownHandler( m_poReactor );
	//! Start reactor event loop
	m_poReactor->open(1);
	m_poReactor->run_reactor_event_loop();
	dummyOm.Finalize();
	/////////////////////////////////////////////////////////////
		DEBUG(1,"%s","EMF_SERVICE_MODULE::startEMFFunctionalityinPassive - Leaving");
	return ACS_CC_SUCCESS;
}//End of startEMFFunctionalityinActive
/*===================================================================
   ROUTINE: ACE_TMAIN
=================================================================== */
ACE_INT32 ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	if (argc > 1)
	{
		// The "debug" flag is specified. Can be combined with time parameter
		// (seconds), i.e. the number of seconds to execute.
		g_bInteractive = TRUE;
		if ((argc == 2)&& !strcmp(argv[1], "-d"))
		{
			ACS_EMF_Tra::ACS_EMF_Logging.Open("EMF");
			DEBUG(1,"%s","acs_emfserverd execution is started in debug mode\n");
			DEBUG(1,"Execution Timeout  %d \n",g_dwExecuteTimeout);
			// Start the application in debug mode ...
			if (g_bInteractive)
			{
				// FOR DEBUG mode ....
				DEBUG(1,"%s%s%s%s%s%s%s%s","===========================================================",
						"\nExternal Media Function binary build: ", __DATE__ ,"  " , __TIME__,
						"\n===========================================================",
						"\n\nStart \"ACS_EMF_Server\" in interactive mode.",
						"\nDebug session start time: \n\n"); // << ACS_EMF_Common::GetDateTimeString() << endl << endl;


				// Create EMF_SERVICE_MODULE instance
				g_pModule = new EMF_SERVICE_MODULE();

				if (!g_pModule)
					return 1;

#if 0
				g_pModule->dsdServerClient();
#endif

				// Start point
				g_pModule->Start();
				// Clean-up phase
				delete g_pModule;
				if (g_bInteractive)
					DEBUG(1,"%s","\nDebug session end time: \n\n"); //ACS_EMF_Common::GetDateTimeString() << endl << endl;
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
			}

		}
		else
		{
			PrintUsage(cerr);
			return 1;
		}
	}
	else
	{
		// EMF Serivice is started in HA mode
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
		int rCode;
		ACE_NEW_NORETURN(pEmfHaServiceObj,ACS_EMF_HAService("acs_emfserverd", "root"));
		ACS_EMF_Tra::ACS_EMF_Logging.Open("EMF");
		if(pEmfHaServiceObj == 0)
		{
			ERROR(1, "%s", "Unable to allocate memory for acs_emf_haservice");
			ERROR(1, "%s", "Exiting acs_emfserverd");
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			rCode = -2;
			return rCode;
		}
		DEBUG(1, "%s", "Starting acs_emfserverd service with HA.. ");
		errorCode = pEmfHaServiceObj->activate();
		switch(errorCode)
		{
		case ACS_APGCC_HA_FAILURE:
		{
			ERROR(1, "%s", "HA Activation Failed for acs_emfserverd!!");
			rCode = -1;
			break;
		}
		case ACS_APGCC_HA_FAILURE_CLOSE:
		{
			ERROR(1, "%s", "HA Application Failed to close gracefully for acs_emfserverd!!");
			rCode = -2;
			break;
		}
		case ACS_APGCC_HA_SUCCESS:
		{
			ERROR(1, "%s", "HA Application Gracefully closed for acs_emfserverd!!");
			rCode = 0;
			break;
		}
		default:
		{
			ERROR(1, "%s", "Error occurred while integrating acs_emfserverd with HA");
			rCode = -2;
		}
		}
		delete pEmfHaServiceObj;
		pEmfHaServiceObj = NULL;
		ACS_EMF_Tra::ACS_EMF_Logging.Close();
		return rCode;
	}
}

ACE_INT32 EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode(int nodeState)
{

	DEBUG(1,"%s","EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode - Entering");
	bool bIsDvdLocked = false;
	int32_t dvdOwnerSysId = acs_dsd::SYSTEM_ID_UNKNOWN;	
	ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(m_pMediaHandler);
	pClienthandler->getDvdState(&dvdOwnerSysId, &bIsDvdLocked);
	if(bIsDvdLocked == true)
	{
		if((nodeState == 1) && (m_LocalNode.system_id == dvdOwnerSysId)){
			//Nothing to do in this case
			DEBUG(1,"%s","EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode - Leaving");
			return EMF_RC_OK;
		}
		ItsScsiUtilityNamespace::TScsiDev objScsiDev;
		if(objScsiDev.isDirMounted("/media") == true){
			ACS_EMF_Common::unmountPassiveDVDData(false);
		}

		ACS_EMF_CmdClient cmdClient(stopThreadFds[0]);

		if(!cmdClient.connect(dvdOwnerSysId, acs_dsd::NODE_STATE_ACTIVE)){
			ERROR(1, "%s", "EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode - Connection to service on Media-owner node is failed!");
			DEBUG(1,"%s","EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode - Leaving");
			return EMF_RC_UNABLETOCONNECTSERV;
		}

		ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_UnmountActiveMedia);

		if(!cmdClient.send(cmdSend)){
			ERROR(1, "%s", "EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode - Sending message to DVD-owner node is failed!");
			DEBUG(1,"%s","EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode - Leaving");
			return EMF_RC_CANNOTSENDCMDTOSERVER;
		}
	}
	DEBUG(1,"%s","EMF_SERVICE_MODULE::disableMediaOnDvdOwnerNode - Leaving");
	return EMF_RC_OK;
}
