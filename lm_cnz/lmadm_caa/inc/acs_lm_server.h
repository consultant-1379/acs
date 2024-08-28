//******************************************************************************
//
//  NAME
//     ACS_LM_SERVER.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR 
//     2008-11-28 by XCSSAGO PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************
#ifndef _ACS_LM_SERVER_H_
#define _ACS_LM_SERVER_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */

//Include file for Sentinel RMS
#include <iostream>
#include <ace/ACE.h>
#include <ace/Synch.h>
//#include <ace/Time_Value_T.h>
//#include <ace/Time_Policy.h>
#include <ace/Monotonic_Time_Policy.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Task_T.h>
#include <ace/Message_Queue.h>
#include <ace/Message_Queue_T.h>
#include <ace/OS.h>
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
#include "acs_lm_common.h"
#include "acs_lm_cmdserver.h"
#include <acs_lm_scheduler.h>
#include "acs_lm_cprestartmonitor.h"
#include "acs_lm_eventhandler.h"
#include "acs_lm_clienthandler.h"
#include <acs_lm_runtimeowner.h>
#include "acs_lm_brfc_interface.h"
#include <acs_lm_electronickey_runtimeowner.h>
#include "acs_lm_root_objectImplementer.h"
#include "acs_lm_managed_objectImplementer.h"
#include "acs_lm_northbound_objectImplementer.h"
#include <ace/Recursive_Thread_Mutex.h>
//#include "acs_prc_process.h"  //Include file for PRC functions
//#include "acs_exceptionhandler.h"   //Include file for Crash routines

ACE_THR_FUNC_RETURN cmdListener(void*);

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_Server : public ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	 =================================================================== */
	/*=================================================================== */
	/**
			@brief       Default constructor for ACS_LM_Server

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	ACS_LM_Server();
	/*=================================================================== */
	/**
			@brief       Default Destructor for ACS_LM_Server

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_Server();

public:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	 =================================================================== */
	/*=================================================================== */
	/**
	 *  @brief run

	 *  @return void
	 */
	/*=================================================================== */
	void run();
	/*=================================================================== */
	/**
	 *  @brief serviceCtrlHandler
	 *
	 *  @param controlCode 				: ACE_UINT64
	 *
	 *  @return void
	 */
	/*=================================================================== */
	//void serviceCtrlHandler(ACE_UINT64 controlCode);
	/*=================================================================== */
	/**
	 *  @brief serviceMain
	 *
	 *  @param argc       				: ACE_UINT64
	 *
	 *  @param argv 					: ACE_TCHAR
	 *
	 *  @return void
	 */
	/*=================================================================== */
	//void serviceMain(ACE_UINT64 argc, ACE_TCHAR ** argv);
	/*=================================================================== */
	/**
	 *  @brief setServiceStatus
	 *
	 *  @param state 					: ACE_UINT64
	 *
	 *  @param waitHint 				: ACE_UINT64
	 *
	 *  @return void
	 */
	/*=================================================================== */
	//void setServiceStatus(ACE_UINT64 state, ACE_UINT64 waitHint = SERVICE_WAIT_HINT);
	/*=================================================================== */
	/**
	 *  @brief svc
	 *
	 *
	 *  @return int
	 */
	/*=================================================================== */
	int svc();
	/*=================================================================== */
	/**
	 *  @brief start
	 *  @return void
	 */
	/*=================================================================== */
	//void start();
	/*=================================================================== */
	/**
	 *  @brief stop

	 *  @return void
	 */
	/*=================================================================== */
	//void stop();
	/*=================================================================== */
	// Service install/uninstall functions
	/**
	 *  @brief install
	 *
	 *  @return bool   	 				: true/false
	 */
	/*=================================================================== */
	//static bool install();
	/*=================================================================== */
	/**
	 *  @brief isInstalled

	 *  @return bool  					: true/false
	 */
	/*=================================================================== */
	//static bool isInstalled();
	/*=================================================================== */
	/**
	 *  @brief uninstall

	 *  @return bool 					: true/false
	 */
	/*=================================================================== */
	//static bool uninstall();
	/*=================================================================== */
	/**
	 *  @brief printUsage

	 *  @return bool 					: true/false
	 */
	/*=================================================================== */
	static void printUsage();

	//	void startLMOIThread();
	/*=================================================================== */
	/**
	 *  @brief setupLMThread

	 *  @return ACS_CC_ReturnType
	 */
	/*=================================================================== */
	//ACS_CC_ReturnType setupLMThread();
	/*=================================================================== */
	/**
	 *  @brief startLMFunctionalThread
	 *
	 *  @param lpvThis 						: void pointer
	 *
	 *  @return ACE_THR_FUNC_RETURN
	 */
	/*=================================================================== */
	//static ACE_THR_FUNC_RETURN startLMFunctionalThread(void * lpvThis);
	// static ACE_THR_FUNC_RETURN lmInstallObjectImplProc(void* lpvThis);

	/*=================================================================== */
	/**
	 *  @brief createLMModeRO
	 *
	 *  @param attrRDN 	 						: ACE_TCHAR
	 *
	 *  @return bool 							: true/false
	 */
	/*=================================================================== */
	bool createLMModeRO(ACE_TCHAR *attrRDN );
	/*=================================================================== */
	/**
	 *  @brief InitializeRunTimeOwner
	 *
	 *  @return bool 							: true/false
	 */
	/*=================================================================== */
	bool InitializeRunTimeOwner();
	/*=================================================================== */
	/**
	 *  @brief InitializeElectronicKeyRunTimeOwner
	 *
	 *  @return bool							: true/false
	 */
	/*=================================================================== */
	bool InitializeElectronicKeyRunTimeOwner();
	/*=================================================================== */
	/**
	 *  @brief createElectronicKeyRunTimeOwner
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void createElectronicKeyRunTimeOwner();
	/*=================================================================== */
	/**
	 *  @brief createAndInitialiseRootOIHandler
	 *
	 *  @return bool							: true/false
	 */
	/*=================================================================== */
	bool createAndInitialiseRootOIHandler(ACS_LM_ClientHandler*,  ACS_LM_ElectronicKey_RuntimeOwner*);

	/*=================================================================== */
	/**
	 *  @brief createAndInitialiseManagedOIHandler
	 *
	 *  @return bool							: true/false
	 */
	/*=================================================================== */
	bool createAndInitialiseManagedOIHandler(ACS_LM_ClientHandler*, ACS_LM_Persistent*,ACS_LM_EventHandler*);

	/*=================================================================== */
		/**
		 *  @brief createAndInitialiseNorthBouudOIHandler
		 *
		 *  @return bool							: true/false
		 */
		/*=================================================================== */
		bool createAndInitialiseNorthBouudOIHandler(ACS_LM_ClientHandler*, ACS_LM_Persistent*,ACS_LM_EventHandler*);
	/*=================================================================== */
	/**
	 *  @brief createAndInitialiseOMHandler
	 *
	 *  @return bool 							: true/false
	 */
	/*=================================================================== */
	bool createAndInitialiseOMHandler();

	/*=================================================================== */
	/**
	 *  @brief finaliseOMHandler
	 *
	 *  @return bool : true/false
	 */
	/*=================================================================== */
	bool finaliseOMHandler();

	bool initializeBrfcRunTimeOwner(acs_lm_root_objectImplementer*);

	void createBrfcRunTimeOwner();

	bool copyFileToBuffer(std::string, char*);

	 /*=================================================================== */
        /**
         *  @brief StopService
         *
         *  @return void             
         */
        /*=================================================================== */

	void stopService();

	/*=================================================================== */
	//ACS_LM_CmdServer cmdServer;
	//ACS_LM_ClientHandler* clientHandler;
	/*=================================================================== */

	void createBrfcThread(acs_lm_root_objectImplementer*);
	int createParticipant(void);
	/*=================================================================== */
		/**
		 *  @brief deleteParticipant
		 *
		 *  @return int
		 */
		/*=================================================================== */
	void deleteParticipant();
	/*=================================================================== */
	/**
	 *  @brief initialDirectorySettings - performs the initial check on LM folders
	 *  @return true in case all the needed folders are created properly.
	 */
	/*=================================================================== */

	bool initialDirectorySettings();
	/*=================================================================== */
	/**
	 *  @brief loadPersistentFile - performs the loading of persistent file
	 *  @return true in case the persistent file is properly loaded
	 */
	/*=================================================================== */
	bool loadPersistentFile();
	/*=================================================================== */
	/**
	 *  @brief loadLicenseFile - performs the loading of License file
	 *  @return true in case the License file is properly loaded
	 */
	/*=================================================================== */
	bool loadLicenseFile();
	/*=================================================================== */
	/**
	 *  @brief createComponents - performs the creation of all components
	 *  @return true in case all components are properly created.
	 */
	/*=================================================================== */
	bool createComponents();
	/*=================================================================== */
	/**
	 *  @brief deleteDirectoryAtOldPrimaryPath - performs the deletion 
	 *  of the folder(ACS-LM) at Old primary path.
	 */
	/*=================================================================== */
	bool deleteDirectoryAtOldPrimaryPath(std::string );
	/*=================================================================== */

	static bool isShutdown;
	static bool isOldDirectoryDeleted; 
	/* The below Variables used to fix the Grace Issue occured during the upgrade from 3.0.5 to 3.2.0 ( TR-HU93530 ) */
        static ACE_UINT32 pp_LKF_Checksum;
        static ACE_UINT16 pp_persistChecksum;
        static ACE_UINT16 pp_calcChecksum;
        static ACE_UINT16 sp_persistChecksum;
        static ACE_UINT16 sp_calcChecksum;

	/*=================================================================== */
	bool getShudownSignalFlag();
	/*=================================================================== */
	/*=================================================================== */
	void setShudownSignalFlag(bool);
	/*=================================================================== */

private:
	// Service register and callback functions

	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	 =================================================================== */
	/*=================================================================== */
	/**
	 *  @brief serviceCtrlHandlerProc
	 *
	 *  @param controlCode 						: ACE_UINT64
	 *
	 *  @return void
	 */
	/*=================================================================== */
	//static void serviceCtrlHandlerProc(ACE_UINT64 controlCode);
	/*=================================================================== */
	/**
	 *  @brief serviceMainProc
	 *
	 *  @param argc 							: ACE_UINT64
	 *
	 *  @param argv 							: ACE_TCHAR
	 *
	 *  @return void
	 */
	/*=================================================================== */
	//static void serviceMainProc(ACE_UINT64 argc, ACE_TCHAR** argv);

	// Data members

	acs_lm_root_objectImplementer * theLMRootObjectImplementer;
	acs_lm_managed_objectImplementer * theLMManagedObjectImplementer;
	acs_lm_northbound_objectImplementer * theLMNorthboundObjectImplementer;
	std::string theCurPersistPath;
	std::string thePSAPath;
	bool thePersistFileValid;
	bool theValidLkfFound;
	bool isShutdownSignalled;
	ACE_Event* theEndEvent;
	//ACE_Event * serviceStopEvent;
	ACS_LM_Persistent* persistFile;
	ACS_LM_Sentinel* sentinelFile;
	ACS_LM_EventHandler* eventHandler;
	ACS_LM_CpRestartMonitor* cpMonitor;
	ACS_LM_Scheduler* scheduler;
	ACS_LM_ClientHandler* clientHandler;
	ACS_LM_CmdServer cmdServer;
	ACS_LM_RuntimeOwner* theLMRuntimeOwnerPtr;
	ACS_LM_RuntimeHandlerThread* theLMRuntimeHandlerThreadPtr;
	ACS_LM_ElectronicKey_RuntimeOwner * theElectronicKeyLMRuntimeOwnerPtr;
	ACS_LM_ElectronicKey_RuntimeHandlerThread* theElectronicKeyLMRuntimeHandlerThreadPtr;
	OmHandler* theOmHandlerPtr;
	ACS_LM_BrfImplementer* theLMBrfcRuntimeOwnerPtr;
	ACS_LM_BrfcThread* theLMBrfcRuntimeHandlerThreadPtr;
	ACE_Recursive_Thread_Mutex lmServerMutex;
};

#endif
