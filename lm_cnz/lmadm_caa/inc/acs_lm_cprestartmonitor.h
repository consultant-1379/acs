#ifndef _ACS_LM_CPRESTARTMONITOR_H_
#define _ACS_LM_CPRESTARTMONITOR_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include "acs_lm_sentinel.h"
#include "acs_lm_testlkf.h"
#include "acs_lm_persistent.h"
#include <ACS_JTP_Service_R3A.h>
#include <ACS_JTP_Service_R2A.h>
#include "acs_lm_eventhandler.h"
#include <ace/ACE.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <acs_lm_jtp_service.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_CpRestartMonitor:public ACE_Task_Base
{
	/*=====================================================================
						 PRIVATE DECLARATION SECTION
	 ==================================================================== */
private:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	 =================================================================== */
	//ACE_HANDLE hThread;


	//static ACE_HANDLE stopThreadFds;
	/**
	 * @brief The Pointer to LM Sentinel Object
	 */
	ACS_LM_Sentinel* sentinel;
	/**
	 * @brief The Pointer to LM Persistent Object
	 */
	ACS_LM_Persistent* persistent;
	/**
	 * @brief The Pointer to event handler Object
	 */
	ACS_LM_EventHandler* eventHandler;
	/**
	 * @brief The flag which represents whether JTP registration is successful or not
	 */
	bool theRegisterServiceResult ;
	/**
	 * @brief The End Event for stopping the thread
	 */
	ACE_Event * theEndEvent;
	/**
	 * @brief The Stop signal
	 */
	static bool theStopSignal;
	/**
	 * @brief Mutex for controlling the setting of stop signal
	 *
	 */

	static ACE_Recursive_Thread_Mutex theMutex;
	//*=================================================================== */
	/**
				@brief      monitorThread			:Method to monitor the thread

				@param 		args  					:void pointer

				@return 	ACE_UINT32
	 */
	/*=================================================================== */

	//ACE_UINT32  monitorThread(void* args);

public:
	/*=================================================================== */
		/**
					@brief       Default constructor for ACS_LM_CpRestartMonitor

					@par         None

					@pre         None

					@post        None

					@exception   None
		 */
		/*=================================================================== */
	ACS_LM_CpRestartMonitor();
	/*=================================================================== */
		/**
					@brief       Default destructor for ACS_LM_CpRestartMonitor

					@par         None

					@pre         None

					@post        None

					@exception   None
		 */
		/*=================================================================== */
	~ACS_LM_CpRestartMonitor();
	/*=================================================================== */
	/**
	 *  @brief setComponents						:
	 *
	 *  @param sentinel 	      			: ACS_LM_Sentinel
	 *
	 *  @param persistent 					: ACS_LM_Persistent
	 *
	 *  @param eventHandler 				: ACS_LM_EventHandler
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void setComponents(ACS_LM_Sentinel* sentinel, ACS_LM_Persistent* persistent, ACS_LM_EventHandler* eventHandler);
	/*=================================================================== */
	/**
	 *  @brief start

	 *  @return void
	 **/
	/*=================================================================== */
	int svc();
	/*=================================================================== */
	/*=================================================================== */
	/**
	 *  @brief start

	 *  @return void
	 **/
	/*=================================================================== */
	void start();
	/*=================================================================== */
	/**
	 *  @brief stop						:

	 *  @return void
	 **/
	/*=================================================================== */
	void stop();
	/*=================================================================== */
	/**
	 *  @brief getStopSignal					:

	 *  @return Value of theStopSignal attribute
	 **/
	/*=================================================================== */
	bool getStopSignal();
	/*=================================================================== */
	/**
	 *  @brief setStopSignal					:

	 *  @param myStatus to be set in theStopSignal attribute
	 **/
	/*=================================================================== */
	void setStopSignal(bool myStatus);
	/*=================================================================== */
	/**
	 *  @brief getRegisterServiceResult					:

	 *  @return Value of theStopSignal attribute
	 **/
	/*=================================================================== */
	bool getRegisterServiceResult();
	/*=================================================================== */
	/**
	 *  @brief setRegisterServiceResult					:

	 *  @param myStatus to be set in theStopSignal attribute
	 **/
	/*=================================================================== */
	void setRegisterServiceResult(bool myStatus);

	/*=================================================================== */
	/**
	 *  @brief register as JTP server					:

	 *  @param true if successfully registered.
	 **/
	/*=================================================================== */

	bool registerService(ACS_LM_JTP_Service* & jtpService);

	//*=================================================================== */
	/**
			@brief      monitorThreadFunction			:Method to monitor the thread

			@param 		args  					:void pointer

			@return 	ACE_THR_FUNC_RETURN
	 */
	/*=================================================================== */


	//static ACE_THR_FUNC_RETURN monitorThreadFunction(void* args);

};

#endif
