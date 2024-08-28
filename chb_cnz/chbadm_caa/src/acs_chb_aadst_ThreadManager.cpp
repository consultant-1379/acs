/*=================================================================== */
   /**
   @file acs_chb_aadst_handler.cpp

   This module contains the implementation of methods declared in
   the ACS_CHB_aadst_handler.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       22/07/2014     XASSORE   Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include <sys/time.h>
#include "ace/Time_Value_T.h"
#include  <ACS_CS_API.h>
#include "acs_chb_tra.h"
#include "acs_chb_aadst_tzfile.h"
#include "acs_chb_aadst_common.h"
#include "acs_chb_aadst_alarmHandler.h"
#include "acs_chb_aadst_handler.h"
#include "acs_chb_aadst_ThreadManager.h"
using namespace std;

/*=====================================================================
					DEFINE DECLARATION SECTION
==================================================================== */


/*===================================================================
   ROUTINE: ACS_CHB_aadst_ThreadManager
=================================================================== */
ACS_CHB_aadst_ThreadManager::ACS_CHB_aadst_ThreadManager()
:condition_attributes_(), aadst_ctrl_mutex_()
{
	DEBUG(1, "%s", "Entering ACS_CHB_aadst_ThreadManager constructor");

	p_aadst_ctrl_cond_ = 0;
	threadGrpId = 0;
	threadId = 0;
	aadst_is_stopping_= false;
}

/*===================================================================
   ROUTINE: ~ACS_CHB_aadst_ThreadManager
=================================================================== */
ACS_CHB_aadst_ThreadManager::~ACS_CHB_aadst_ThreadManager()
{
	delete p_aadst_ctrl_cond_;
	p_aadst_ctrl_cond_ = 0;
}

/*=======================================================================
	ROUTINE: AADST_start()
========================================================================*/
int  ACS_CHB_aadst_ThreadManager::AADST_start()
{
	DEBUG(1, "%s", "Entering ACS_CHB_aadst_ThreadManager::AADST_start()");
	ACE_INT32 local_node_sysId;

	if(acs_chb_aadst_common::get_local_node_sysId(local_node_sysId)){
		ERROR(1, "%s", "ACS_CHB_aadst_ThreadManager::AADST_start - Error in getting local node system identifier - AADST NOT STARTED .");
		return CHB_AADST_TM_CANNOT_RETRIEVE_SYS_ID;
	}
	DEBUG(1, "ACS_CHB_aadst_ThreadManager::AADST_start - local_node_sysID == %ld", local_node_sysId);

    if(local_node_sysId != AP1_SYSID){
    	DEBUG(1, "%s", "ACS_CHB_aadst_ThreadManager::AADST_start - AADST thread MUST be started only on AP1 - AADST NOT STARTED.");
    	return CHB_AADST_TM_AADST_NOT_STARTED;
    }

	p_aadst_ctrl_cond_ = new (std::nothrow) ACE_Condition<ACE_Thread_Mutex>(aadst_ctrl_mutex_,condition_attributes_);
	if(!p_aadst_ctrl_cond_)
	{
		ERROR(1, "%s", "ACS_CHB_aadst_ThreadManager::AADST_start: Memory allocation error - AADST  NOT STARTED.");
		return CHB_AADST_TM_FAILURE;
	}


	void* (*func)(void*);		// Set function pointers to the functions
	func = &(ACS_CHB_aadst_ThreadManager::AADST_mainservice); // to execute in separate threads.

	const ACE_TCHAR* lpszThreadName = "AADSTThread";
	threadGrpId = ACE_Thread_Manager::instance()->spawn( func,
			this,
			THR_NEW_LWP | THR_JOINABLE| THR_INHERIT_SCHED,
			&threadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			threadGrpId,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName);
	if (threadGrpId == -1)
	{
		ERROR(1, "%s", "Leaving ACS_CHB_aadst_ThreadManager::AADST_start() failed in starting AADST Thread");
		return CHB_AADST_TM_FAILURE;
	}
	//	}
	//	else {
	//		DEBUG (1, "%s", "ACS_CHB_aadst_ThreadManager::AADST_start(): the local node state is passive or undefined. AADST Thread is not started !");
	//	}
	DEBUG(1, "%s", "ACS_CHB_aadst_ThreadManager::AADST_start:  AADST main thread has been started!");
	DEBUG(1, "%s", "Leaving ACS_CHB_aadst_ThreadManager::AADST_start()");
	return CHB_AADST_TM_SUCCESS;
}

/*======================================================================
	ROUTINE: AADST_stop
========================================================================*/
void ACS_CHB_aadst_ThreadManager::AADST_stop()
{
	DEBUG(1, "%s", "Entering ACS_CHB_aadst_ThreadManager::AADST_stop()");
	if( threadId != 0 )
	{
		if(aadst_ctrl_mutex_.acquire() == -1)
			ERROR(1,"Unable to acquire 'aadst_ctrl_mutex_'! errno == %d ", errno);

		aadst_is_stopping_ = true;

		if(aadst_ctrl_mutex_.release() == -1)
			ERROR(1,"Unable to release 'aadst_ctrl_mutex_'! errno == %d ", errno);

		if(p_aadst_ctrl_cond_)
			p_aadst_ctrl_cond_->signal();

		ACE_Thread_Manager::instance()->join(threadId);
	}

	DEBUG(1, "%s", "Leaving ACS_CHB_aadst_ThreadManager::AADST_stop()");
}

void * ACS_CHB_aadst_ThreadManager::AADST_mainservice(void* param)
{

	DEBUG(1, "%s", "Entering ACS_CHB_aadst_ThreadManager::AADST_mainservice()");
	ACS_CHB_aadst_ThreadManager* pT = (ACS_CHB_aadst_ThreadManager*)param;

	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> tm;

	ACS_CHB_aadst_handler aadst_handler(pT);
	acs_chb_aadst_alarmHandler alarmHandler;

	int aadst_execute_cnt=0;
	bool aadst_exit=false;

	do {
		int aadst_wait_time = AADST_PERIODIC_TIMEOUT_VALUE;
		// Checking if it's time to run ...
		if (aadst_execute_cnt != 0){
			timeval tv;
			struct tm broken_down_time;
			// Check current time
			if (gettimeofday(&tv, 0) !=0 || localtime_r(&tv.tv_sec, &broken_down_time) == NULL){
				ERROR(1, "%s","ACS_CHB_aadst_ThreadManager::AADST_mainservice:  call 'gettimeofday' or 'localtime_r' failed - AADST cannot get current time ");
			}
			else {

				//set AADST wakeup Timeout to 23:33 of current day
				struct tm wakeup_broken_data_time = broken_down_time;
				if(broken_down_time.tm_hour == AADST_WAKE_UP_TIME_HOUR &&
						(broken_down_time.tm_min == AADST_WAKE_UP_TIME_MIN -1 || broken_down_time.tm_min == AADST_WAKE_UP_TIME_MIN)){

					// calculate how many seconds it has to wait for wake up time matching
					wakeup_broken_data_time.tm_hour = AADST_WAKE_UP_TIME_HOUR;
					wakeup_broken_data_time.tm_min = AADST_WAKE_UP_TIME_MIN;
					wakeup_broken_data_time.tm_sec = AADST_WAKE_UP_TIME_SEC;
					// START- ONLY FOR DEBUG SCOPE
					char buffer[8*1024];
					DEBUG(1,"%s", "ACS_CHB_aadst_ThreadManager::AADST_mainservice: wake_up time is: ");
					snprintf(buffer,ACS_ARRAY_SIZE(buffer), "%s %d-%02d-%02d %02d:%02d:%02d.%06ld ", "---->",
							TM_YEAR_BASE + wakeup_broken_data_time.tm_year, wakeup_broken_data_time.tm_mon+1, wakeup_broken_data_time.tm_mday,
							wakeup_broken_data_time.tm_hour, wakeup_broken_data_time.tm_min, wakeup_broken_data_time.tm_sec, tv.tv_usec);
					ACS_CHB_Tra::helperTraceDebugCHB(1,buffer);
					// -END
					// convert broken_data_time to Simple time time_t
					time_t wake_up_time = mktime(&wakeup_broken_data_time);
					time_t current_time = time(0);
					double wait_time = difftime(wake_up_time, current_time);
					DEBUG(1,"ACS_CHB_aadst_ThreadManager::AADST_mainservice: difftime to wake-up time== %f ", wait_time);
					if ( wait_time <= 0){
						// Execute AADST Task only on  the active node
						if( acs_chb_aadst_common::getNodeState() == AADST_NODE_STATE_ACTIVE &&
								alarmHandler.check_for_alarm_presence() != CHB_AADST_ALARM_A1){
							aadst_execute_cnt++;
							DEBUG(1, "ACS_CHB_aadst_ThreadManager::AADST_mainservice: It is time to run 'aadst_execute_cnt' == %d ", aadst_execute_cnt);
							// Execute Thread Task

							int ret_code = aadst_handler.aadst_execute();
							if (ret_code > 0 ){
								DEBUG(1, "ACS_CHB_aadst_ThreadManager::AADST_mainservice: 'aadst_handler.adst_execute()'failed: result == %d ", ret_code);
							}
							// avoid to execute a second time in the 23.32 - 23.33 interval.
							//aadst_wait_time += AADST_SHIFT_TIMEOUT_VALUE;
						}
					}else{
						// wait until it'time to run 23:33
						aadst_wait_time = wait_time ;
					}
				}else
					DEBUG(1,"%s", "ACS_CHB_aadst_ThreadManager::AADST_mainservice: It's not time to run ...");
			}

		}else if( acs_chb_aadst_common::getNodeState() == AADST_NODE_STATE_ACTIVE &&
				alarmHandler.check_for_alarm_presence() != CHB_AADST_ALARM_A1){

			aadst_execute_cnt++;
			DEBUG(1, "ACS_CHB_aadst_ThreadManager::AADST_mainservice: It is first execution 'aadst_execute_cnt' == %d ", aadst_execute_cnt);
			// Execute Thread Task
			int ret_code = aadst_handler.aadst_execute();
			if (ret_code > 0 ){
				DEBUG(1, "ACS_CHB_aadst_ThreadManager::AADST_mainservice: 'aadst_handler.adst_execute()'failed: result == %d ", ret_code);
			}
		}

		//} endIF
		// wait for stop event is signaled or aadst wake-up timeout expired
		tm = tm.now() + ACE_Time_Value(aadst_wait_time);
		DEBUG(1, "ACS_CHB_aadst_ThreadManager::AADST_mainservice: waiting for 'aadst_ctrl_cond_' stop signaling or wake-up Timeout (%d sec)", aadst_wait_time );
		pT->aadst_ctrl_mutex_.acquire();
		while(!pT->aadst_is_stopping_ ) {
			if( pT->p_aadst_ctrl_cond_->wait(&tm) == -1){
				if ( errno == ETIME){

					DEBUG(1, "%s","ACS_CHB_aadst_ThreadManager::AADST_mainservice: Timeout Expired while waiting for 'aadst_ctrl_cond' condition signaling ");break;
					break;
				}
				ERROR(1, "ACS_CHB_aadst_ThreadManager::AADST_mainservice: An error occurred while waiting for AADST ctrl signaling, errno == %d ", errno);
			}
		}

		if(!pT->aadst_is_stopping_ ){
			pT->aadst_ctrl_mutex_.release();
			DEBUG(1, "%s","ACS_CHB_aadst_ThreadManager::AADST_mainservice: continue and check if it's time to run ... ");
			continue;
		}

		pT->aadst_ctrl_mutex_.release();
		aadst_exit = true;
		DEBUG(1,"%s", "ACS_CHB_aadst_ThreadManager::AADST_mainservice: 'aadst_ctrl_cond' condition has been signaled, exit from do-while loop!");
	}
	while(!aadst_exit);

	DEBUG(1, "%s", "Leaving ACS_CHB_aadst_ThreadManager::AADST_mainservice)");
	return 0;
}
