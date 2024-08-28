/*=================================================================== */
   /**
   @file acs_chb_clocksync_class.h

   Header file for class for handling HA related callbacks.

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
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_CLOCKSYNC_CLASS_H
#define ACS_CHB_CLOCKSYNC_CLASS_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ACS_APGCC_ApplicationManager.h>
#include <unistd.h>
#include <ace/Task.h>
//#include <ACS_PRC_Process.h>
#include <ace/Event.h>
#include <ace/Time_Value.h>
#include <fstream>
#include <iostream>
#include <acs_chb_clock_service.h>
#include <acs_chb_clock_comm.h>
#include <acs_chb_clock_sync.h>
#include <acs_chb_tra.h>
#include <acs_chb_param_oihandler.h>
using namespace std;

/*=====================================================================
                        GLOBAL VARIABLE
==================================================================== */
/**
  @brief	evh
*/
extern ACE_Event evh;

/**
  @brief	ClockSyncServiceMain
*/
ACE_THR_FUNC_RETURN ClockSyncServiceMain(void *ptr);

/**
  @brief	stop
*/
ACS_APGCC_ReturnType stop();
/**
 * @brief	globalFlags
 */
extern unsigned long                   globalFlags;

/**
 * @brief	cycletimeInMs
 */
extern int                             cycletimeInMs;

/**
 * @brief	debug
 */
extern  bool    debug;

/**
 * @brief        checkDSDDependencyForClockSync
*/

extern int checkDSDDependencyForClockSync();

extern int checkNodeState();

//extern acs_chb_param_oihandler *ptrClockSyncParamOiHandler;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/**
 * @brief	TRUE
 */
#define TRUE    1

/**
 * @brief	FALSE
 */
#define FALSE   0

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class acs_chb_clocksync_class: public ACS_APGCC_ApplicationManager
{
	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */

   public:
	/**
	 * @brief	acs_chb_clocksync_class
	 * @param daemon_name
	 * @param username
	 * @return
	 */
	acs_chb_clocksync_class(const char* daemon_name, const char* username);
	
	/**
		@brief		~acs_chb_clocksync_class
	*/
	~acs_chb_clocksync_class();
	
	/**	
		@brief		performStateTransitionToActiveJobs

		@param		previousHAState

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	 /**
                @brief          performStateTransitionToPassiveJobs

                @param          previousHAState

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
       ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);


	/**	
		@brief		performStateTransitionToQueisingJobs

		@param		previousHAState

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**	
		@brief		performStateTransitionToQueiscedJobs

		@param		previousHAState

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/**	
		@brief		performComponentHealthCheck

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	ACS_APGCC_ReturnType performComponentHealthCheck(void);


	/**
                @brief          performComponentTerminateJobs

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
	
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);

	/**
                @brief          performComponentRemoveJobs

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);

	
	/**
                @brief          performApplicationShutdownJobs

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);

	/**
                @brief          activateApp

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
	ACS_APGCC_ReturnType activateApp(void);

	/**
                @brief          passifyApp

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
	ACS_APGCC_ReturnType passifyApp(void);

	/**
                @brief          clocksync_svc

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
	ACS_APGCC_ReturnType clocksync_svc(void);
	static bool theStopSignal;
	static ACE_Recursive_Thread_Mutex theMutex;
	static void setStopSignal(bool myStatus);
        static void sendStopToPipe();
	static bool getStopSignal();
	static ACE_thread_t applicationThreadId;	
	static ACS_APGCC_BOOL Is_terminated;
	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */

   private:
#if 0
	/**
		@brief		readWritePipe
	*/
	int readWritePipe[2];

	/**
		@brief		Is_terminated
	*/
	ACS_APGCC_BOOL Is_terminated;

	/**
		@brief		applicationThreadId
	*/
	ACE_thread_t applicationThreadId;	
#endif
	

}; 

#endif /* ACS_CHB_CLOCKSYNC_CLASS_H */
