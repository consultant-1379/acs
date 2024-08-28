/*=================================================================== */
/**
   @file   acs_chb_heartbeat_class.h

   @brief  Header file for CHB module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       03/26/2011     XTANAGG        APG43 on Linux.
==================================================================== */
/*=====================================================================
		DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_HEARTBEAT_CLASS_H
#define ACS_CHB_HEARTBEAT_CLASS_H

/*====================================================================
		INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_APGCC_ApplicationManager.h>
#include <unistd.h>
#include <ace/Task.h>
#include <ace/OS_NS_poll.h>
#include <ace/ACE.h>
#include <ace/Process.h>
#include <ace/Event.h>
#include <algorithm>
#include <sys/stat.h>
#include <acs_chb_common.h>
#include <acs_chb_request_handler.h>
#include <acs_chb_heartbeat_comm.h>
#include <acs_chb_tra.h>
#include <ACS_APGCC_CommonLib.h>
//#include <ACS_PRC_Process.h>
#include <acs_chb_param_oihandler.h>
using namespace std;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  ACS_CHB_HeartBeatService

 */
/*=================================================================== */
#define ACS_CHB_HeartBeatService "ACS_CHB_HeartBeat"


/*=====================================================================
                        GLOBAL VARIABLE
==================================================================== */
/*=================================================================== */
/**
   @brief  debug

 */
/*=================================================================== */
extern bool debug;

/*=================================================================== */
/**
   @brief  HeadCPObjectList

 */
/*=================================================================== */
extern ACS_CHB_CPObjectList *HeadCPObjectList;
/*=================================================================== */
/**
   @brief  requestFromHeartBeat

 */
/*=================================================================== */
extern char requestFromHeartBeat[ACS_CHB_SMALL_BUFSIZE];
/*=================================================================== */
/**
   @brief  requestHandler

 */
/*=================================================================== */
extern ACS_CHB_RequestHandler *requestHandler;
/*=================================================================== */

/**
 * @brief	process
 */
extern ACE_Process process;
/*=================================================================== */
/**
   @brief  childProcInfo

 */
/*=================================================================== */
extern pid_t childProcInfo ;
/*=================================================================== */

/**
  @brief	stop
*/
/*=================================================================== */
ACS_APGCC_ReturnType stop();

/*=================================================================== */

/**
  @brief	CHB_serviceMain
*/
/*=================================================================== */
ACE_THR_FUNC_RETURN CHB_serviceMain(void *ptr );

/*=================================================================== */

/**
  @brief	startupEventManager
*/
/*=================================================================== */
extern ACE_INT32 startupEventManager();
/*=================================================================== */

/**
  @brief	getacsdata
*/
/*=================================================================== */
extern bool getacsdata(char* pChar, size_t pLen);
/*=================================================================== */

/**
  @brief	setupMTZLib
*/
/*=================================================================== */
extern bool setupMTZLib(void);
/*=================================================================== */

/**
  @brief	isCsOnline
*/
/*=================================================================== */
extern bool isCsOnline();
/*=================================================================== */

/**
  @brief	pipeHandles
*/
/*=================================================================== */
extern ACE_HANDLE pipeHandles[2];
/*=================================================================== */

/**
  @brief	checkDSDDependencyForHeartBeat
*/

/*=================================================================== */
extern int checkDSDDependencyForHeartBeat();
/*=================================================================== */


/**
   @brief	checkNodeState
*/
/*=================================================================== */
extern int checkNodeState();

/*=================================================================== */

#if 0
extern acs_chb_param_oihandler *ptrHeartBeatParamOiHandler;
#endif


/*=================================================================== */
/**
	@class			acs_chb_heartbeat_class

	@brief			This class handles all HA related activities.
**/
/*=================================================================== */
class acs_chb_heartbeat_class: public ACS_APGCC_ApplicationManager 
{

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

	/**
	 * @brief	activateApp
	 * @return  ACS_APGCC_ReturnType
	 */
	ACS_APGCC_ReturnType activateApp();
	/**
	 * @brief	passifyApp
	 * @return  ACS_APGCC_ReturnType
	 */
	ACS_APGCC_ReturnType passifyApp();

   public:
	/*=====================================================================
				CLASS CONSTRUCTORS
	==================================================================== */

	/**
		@brief		acs_chb_heartbeat_class
	
		@param		daemon_name

		@param		username
		
	*/
	/********************************************************************/	
	acs_chb_heartbeat_class(const char* daemon_name, const char* username);

	/*===================================================================
				CLASS DESTRUCTOR
	=================================================================== */
	/**
		@brief		 ~acs_chb_heartbeat_class

	*/
	/********************************************************************/	
	~acs_chb_heartbeat_class();
	
	/********************************************************************/	
	/**
		@brief		performStateTransitionToActiveJobs

		@param		previousHAState

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	/********************************************************************/	
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/********************************************************************/	

	 /**
                @brief          performStateTransitionToPassiveJobs

                @param          previousHAState

                @return         ACS_APGCC_ReturnType

                @exception      none
        */
        /********************************************************************/
        ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
        /********************************************************************/


	/**
		@brief		performStateTransitionToQueisingJobs

		@param		previousHAState

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	/********************************************************************/	
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/********************************************************************/	

	/**
		@brief		performStateTransitionToQuiescedJobs

		@param		previousHAState

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	
	/********************************************************************/	
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/********************************************************************/	

	/**
		@brief		performComponentHealthCheck

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	/********************************************************************/	
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	/********************************************************************/	

	/**
		@brief		performComponentTerminateJobs

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	/********************************************************************/	
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	/********************************************************************/	

	/**
		@brief		performComponentRemoveJobs

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	/********************************************************************/	
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	/********************************************************************/	

	/**
		@brief		performApplicationShutdownJobs

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	/********************************************************************/	
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);
	/********************************************************************/	

	/**
		@brief		heartbeat_svc

		@return		ACS_APGCC_ReturnType

		@exception	none
	*/
	/********************************************************************/	
	ACS_APGCC_ReturnType heartbeat_svc(void);
	/********************************************************************/	
}; 

#endif	/** end of  ACS_CHB_HEARTBEAT_CLASS_H */
