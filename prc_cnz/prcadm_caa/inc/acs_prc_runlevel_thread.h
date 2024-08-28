/*
 * acs_prc_runlevel_thread.h
 *
 *  Created on: Nov 20, 2011
 *      Author: xlucpet
 */

#ifndef ACS_PRC_RUNLEVEL_THREAD_H_
#define ACS_PRC_RUNLEVEL_THREAD_H_

#include "ace/Task.h"
#include "ACS_TRA_Logging.h"
#include "ace/Condition_Attributes.h"
#include "ace/Monotonic_Time_Policy.h"
#include "ace/Condition_T.h"
#include "ace/Thread_Mutex.h"
#include "acs_prc_runlevel.h"

class acs_prc_runlevel_thread : public ACE_Task_Base {
public:
	acs_prc_runlevel_thread ( );
	virtual ~acs_prc_runlevel_thread();

	void setNodes ( std::string localNode, std::string remoteNode );

	virtual int svc ( void );

	void stop(){
		thread_sleep = true;
		_stop_working_condition.signal();
	};

	void start(){ thread_sleep = false; };

	void updateSUState ( std::string SuName, int SuState );

	void cease_service_stopped_alarm ();
	int enablePBE(bool);
	int isPBEEnabled();
	void run_ntp_config_scripts();
	int create_restore_flag();
	void decreaseTipcTolerance();//CC-21559
        void increaseTipcTolerance();
	static void setRestoreFlag(bool isRestored);
	static bool getRestoreFlag();
private:
	void execute_service_monitoring ();
	void raise_service_stopped_alarm ();
	int getAPNodeNumber();
	std::string local_Node;
	std::string remote_Node;
	bool thread_sleep;
	static bool restore_flag;
	ACE_Condition_Attributes_T<ACE_Monotonic_Time_Policy> _condition_attributes;
	ACE_Thread_Mutex _thread_mutex;
	ACE_Condition<ACE_Thread_Mutex> _stop_working_condition;
	acs_prc_runlevel* RunLevel_thread;
};

#endif /* ACS_PRC_RUNLEVEL_THREAD_H_ */
