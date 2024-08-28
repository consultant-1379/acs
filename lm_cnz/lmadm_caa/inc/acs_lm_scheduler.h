#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include "acs_lm_cmd.h"
#include "acs_lm_timer.h"
#include "acs_lm_sentinel.h"
#include "acs_lm_testlkf.h"
#include "acs_lm_persistent.h"
#include "acs_lm_eventhandler.h"
#include <ace/ACE.h>
#include <ace/Thread_Manager.h>
#include <ace/Event.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_Scheduler:public ACS_LM_TimerListener
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
	ACE_HANDLE hThread;
	ACS_LM_Timer * timer;
	ACE_Event * hScheduleCompleted;
	bool threadStopRequested;
	ACS_LM_Sentinel* sentinel;
	ACS_LM_TestLkf* testLkf;
	ACS_LM_Persistent* persistent;
	ACS_LM_EventHandler* eventHandler;

	/*=================================================================== */
	/**
	 *  @brief schedulerThread
	 *
	 *  @param pArgs    				: void pointer
	 *
	 *  @return ACE_UINT64
	 */
	/*=================================================================== */
	ACE_UINT64 static schedulerThread(void* pArgs);

public:
	/*===================================================================
			   				PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
			   				PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
					@brief       Default constructor for ACS_LM_Scheduler

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	ACS_LM_Scheduler();
	/*=================================================================== */
	/**
					@brief       Default destructor for ACS_LM_Scheduler

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_Scheduler();
	/*=================================================================== */
	/**
	 *  @brief  timeout
	 *
	 *  @return void
	 */
	/*=================================================================== */
	virtual void timeout();

	//ACE_UINT64 calculateSecondsToWait();
	/*=================================================================== */
	/**
	 *  @brief  start
	 *
	 *  @param sentinel 				: ACS_LM_Sentinel
	 *
	 *  @param persistent 				: ACS_LM_Persistent
	 *
	 *  @param eventHandler 			: ACS_LM_EventHandler
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void start(ACS_LM_Sentinel* sentinel, ACS_LM_Persistent* persistent,ACS_LM_EventHandler* eventHandler);
	/*=================================================================== */
	/**
	 *  @brief  stop
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void stop();
	/*=================================================================== */
	/**
	 *  @brief  schedulerThreadFunc
	 *
	 *  @param  args 							: void pointer
	 *
	 *  @return ACE_THR_FUNC_RETURN
	 */
	/*=================================================================== */
	static ACE_THR_FUNC_RETURN schedulerThreadFunc(void* args);
	/*=================================================================== */


private:
	/*===================================================================
									 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
									 PRIVATE METHOD
	=================================================================== */
	/*=================================================================== */
	/**
	 *  @brief  schedulerThreadFunc
	 *
	 *  @return time_t
	 */
	/*=================================================================== */
	time_t calculateSecondsToWait();
	//*=================================================================== */
	/**
					@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_LM_Scheduler(const ACS_LM_Scheduler&); // disabled.
	//*=================================================================== */
	/**
					@brief      operator
	 */
	/*=================================================================== */
	const ACS_LM_Scheduler& operator=(const ACS_LM_Scheduler&); // disabled.

};
#endif

