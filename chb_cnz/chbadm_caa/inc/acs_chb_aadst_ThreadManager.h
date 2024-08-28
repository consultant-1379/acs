/*=================================================================== */
/**
	@file		acs_chb_aadst_handler.h

	@brief		Header file for aadst handler.

				This module contains all the declarations useful to
				specify the class ACS_CHB_aadst_ThreadManager.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       22/07/2014    XASSORE	       Initial Release
==================================================================== */
//******************************************************************************
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_aadst_ThreadManager_H
#define ACS_CHB_aadst_ThreadManager_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Thread_Manager.h>
#include <ace/Event.h>
#include "ace/Condition_T.h"
#include "ace/Monotonic_Time_Policy.h"
#include <time.h>
using namespace std;



//#define  AADST_PERIODIC_TIMEOUT_VALUE 50  // seconds
//#define  AADST_SHIFT_TIMEOUT_VALUE 12  // AADST_PERIODIC_TIMEOUT_VALUE + AADST_SHIFT_TIMEOUT_VALUE > 60 sec
#define  AADST_PERIODIC_TIMEOUT_VALUE 60  // seconds
#define  AADST_SHIFT_TIMEOUT_VALUE 0  //
#define  AADST_WAKE_UP_TIME_HOUR  23
#define  AADST_WAKE_UP_TIME_MIN   33
#define  AADST_WAKE_UP_TIME_SEC    0
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*=========================aadst_execute==========================================*/

/*=================================================================== */
class  ACS_CHB_aadst_ThreadManager
{

public:

	enum  aadst_threadmanager_rc{
			CHB_AADST_TM_SUCCESS = 0,
			CHB_AADST_TM_FAILURE = 1,
			CHB_AADST_TM_CANNOT_RETRIEVE_SYS_ID = 2,
			CHB_AADST_TM_AADST_NOT_STARTED = 3
		};

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

		@brief			Class constructor.
					The constructor of the class. Creates an internal object.
	*/
	/*=================================================================== */
	ACS_CHB_aadst_ThreadManager();

	/*=====================================================================
	                        CLASS DESTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

		@brief			Class destructor.
						The destructor of the class. Destroys the internal object.
	*/
	/*=================================================================== */

	 ~ACS_CHB_aadst_ThreadManager();

	/*=====================================================================
		                        PUBLIC DECLARATION SECTION
		==================================================================== */

	/**
			@brief  AADST_start()

	 */
	int AADST_start();


	/**
			@brief	AADST_stop
	 */
	void AADST_stop();

	static void * AADST_mainservice(void* param);

	inline bool get_aadst_is_stopping(){return aadst_is_stopping_;};
	/*===================================================================
	                        PUBLIC ATTRIBUTE
	=================================================================== */



	/*===================================================================
		                        PRIVATE DECLARATION SECTION
	=================================================================== */
	private:
	/*===================================================================
	                        PRIVATE ATTRIBUTE
	=================================================================== */

	//timeval _last_execution_time;
	/**
	 * 	@brief			Handle to Thread stop condition.
	 *
	 */
	bool aadst_is_stopping_;
	ACE_Condition_Attributes_T<ACE_Monotonic_Time_Policy> condition_attributes_;
	ACE_Thread_Mutex aadst_ctrl_mutex_;					  // mutex to control aadst work
	ACE_Condition<ACE_Thread_Mutex> *p_aadst_ctrl_cond_;  // condition to control aadst work
	/**
	*	@brief		Thread Group Id
	*/
	ACE_INT32 threadGrpId;

	/**
	*	@brief	Thread Id
	*/
	ACE_thread_t threadId;

	/*===================================================================
	                        PRIVATE METHOD
	=================================================================== */

};

#endif /* end ACS_CHB_aadst_TreadManager_H*/
