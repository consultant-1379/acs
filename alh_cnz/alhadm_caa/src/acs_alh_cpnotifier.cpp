/*
 * acs_alh_cpnotifier.cpp
 *
 *  Created on: Nov 14, 2011
 *      Author: xgiopap
 */

#include "acs_alh_cpnotifier.h"

using namespace std;

//========================================================================================
//	Constructor
//========================================================================================
acs_alh_cpnotifier::acs_alh_cpnotifier(ACE_Condition<ACE_Thread_Mutex>* cond, ACE_Condition<ACE_Thread_Mutex>* stop_cond) :
util("CPNOTIFIER_CLASS"),
cp_state_notifier(),
cp_current_state_notifier(),
cond_(cond),
stop_cond_(stop_cond)
{
	cp_state_ = acs_dsd::CP_STATE_UNKNOWN;
	current_CP_state_ = acs_dsd::CP_STATE_UNKNOWN;

	alh_cpNotifier_thread_terminated = false;
	alh_cpNotifier_thread_stop = false;
	alh_cpNotifier_thread_exit_code = 0;
	cpnotifier_waitTermination_ = false;
}


//========================================================================================
//	Destructor
//========================================================================================
acs_alh_cpnotifier::~acs_alh_cpnotifier()
{

}

//========================================================================================
//	get_cpstate_value
//========================================================================================
acs_dsd::CPStateConstants acs_alh_cpnotifier::get_cpstate_value()
{
	//Call trace function
	util.trace("CPNOTIFIER - Method: get_cpstate_value - cp_state_: %i", cp_state_);

	//Call logging function
	log.write(LOG_LEVEL_INFO, "CPNOTIFIER - Method: get_cpstate_value - cp_state_: %i", cp_state_);

	return cp_state_;
}

//========================================================================================
// 	Connect CP
//========================================================================================
bool acs_alh_cpnotifier::CP_Connect()
{

	util.trace("acs_alh_cpnotifier::CP_Connect - enter");
	log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::CP_Connect - enter");

	int32_t cp_system_id = ACS_JTP_Conversation::SYSTEM_ID_CP_ALARM_MASTER;
	//int32_t cp_system_id = 1001;

	// Ask DSD API to be notified about the state changes of the CP having system id == <system_id>
	int call_result = cp_current_state_notifier.attach_to(cp_system_id);
	if(call_result < 0)
	{
		util.trace("acs_alh_cpnotifier::CP_Connect - attach_to(%d) failed ! error_code == %d, error_descr == %s", cp_system_id, cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());
		log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::CP_Connect - attach_to(%d) failed ! error_code == %d, error_descr == %s", cp_system_id, cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());
		return false;
	}

	//Call trace function
	util.trace("acs_alh_cpnotifier::CP_Connect - attach_to(%d) successful !", cp_system_id);

	//Call logging function
	log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::CP_Connect - attach_to(%d) successful !", cp_system_id);

	// CP state change notification arrived !
	if(cp_current_state_notifier.get_cp_state(current_CP_state_) < 0)
	{
		//Call trace function
		util.trace("acs_alh_cpnotifier::CP_Connect - failed ! error_code == %d, error_descr == %s", cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());

		//Call logging function
		log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::CP_Connect - failed ! error_code == %d, error_descr == %s", cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());
		return false;
	}

	call_result = cp_current_state_notifier.detach ();
	if(call_result < 0)
	{
		//Call trace function
		util.trace("acs_alh_cpnotifier::CP_Connect - detach() failed ! error_code == %d, error_descr == %s", cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());

		//Call logging function
		log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::CP_Connect - detach() failed ! error_code == %d, error_descr == %s", cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());
		return false;
	}

	//Call trace function
	util.trace("acs_alh_cpnotifier::CP_Connect - successful! current_CP_state_: %d", current_CP_state_);

	//Call logging function
	log.write(LOG_LEVEL_INFO, "acs_alh_cpnotifier::CP_Connect - successful! current_CP_state_ is: %d", current_CP_state_);

	cp_state_= current_CP_state_;

	util.trace("acs_alh_cpnotifier::CP_Connect - exit");
	log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::CP_Connect - exit");

	return true;
}

//========================================================================================
//	svc
//========================================================================================
int acs_alh_cpnotifier::getCpState(void)
{
	acs_dsd::CPStateConstants current_cp_state_ = acs_dsd::CP_STATE_UNDEFINED;
	alh_cpNotifier_thread_exit_code = 0;
	alh_cpNotifier_thread_terminated = false;

	ACS_DSD_CPStateNotifier cp_state_notifier;

	//Call trace function
	util.trace("acs_alh_cpnotifier::getCpState - running Thread");

	//Call logging function
	log.write(LOG_LEVEL_INFO, "acs_alh_cpnotifier::getCpState - running Thread");

	int32_t cp_system_id = ACS_JTP_Conversation::SYSTEM_ID_CP_ALARM_MASTER;

	// Ask DSD API to be notified about the state changes of the CP having system id == <system_id>
	int call_result = cp_state_notifier.attach_to(cp_system_id);

	if(call_result < 0) {
		//Call trace function
		util.trace("acs_alh_cpnotifier::getCpState - attach_to(%d) failed ! error_code == %d, error_descr == %s", cp_system_id, cp_state_notifier.last_error(), cp_state_notifier.last_error_text());

		//Call logging function
		log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - attach_to(%d) failed ! error_code == %d, error_descr == %s", cp_system_id, cp_state_notifier.last_error(), cp_state_notifier.last_error_text());

		cond_->mutex().acquire();
		alh_cpNotifier_thread_exit_code = -1;
		alh_cpNotifier_thread_terminated = true;
		cond_->signal();
		cond_->mutex().release();
		cpnotifier_waitTermination_ = true;

		log.write(LOG_LEVEL_INFO, "acs_alh_cpnotifier::getCpState - exit ( %i )", alh_cpNotifier_thread_exit_code);

		return -1;
	}

	log.write(LOG_LEVEL_INFO, "acs_alh_cpnotifier::getCpState - exit from while to attach the CP - alh_cpNotifier_thread_stop: %d - alh_cpNotifier_thread_exit_code: %d - alh_cpNotifier_thread_terminated: %d", alh_cpNotifier_thread_stop, alh_cpNotifier_thread_exit_code, alh_cpNotifier_thread_terminated);
	//Call trace function
	util.trace("acs_alh_cpnotifier::getCpState - attach_to(%d) successful !", cp_system_id);

	//Call logging function
	log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - attach_to(%d) successful !", cp_system_id);

	// CP state change notification arrived !

	// get the handle to wait on for CP state notifications
	int handle_count = 1;
	acs_dsd::HANDLE sock_handles [1];

	if ( cp_state_notifier.get_handles(sock_handles, handle_count) < 0 ) {
		//Call trace function
		util.trace("acs_alh_cpnotifier::getCpState - get_handles() - failed ! error_code == %d, error_descr == %s", cp_state_notifier.last_error(), cp_state_notifier.last_error_text());

		//Call logging function
		log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - get_handles() - failed ! error_code == %d, error_descr == %s", cp_state_notifier.last_error(), cp_state_notifier.last_error_text());

		call_result = cp_state_notifier.detach ();

		cond_->mutex().acquire();
		alh_cpNotifier_thread_exit_code = -2;
		alh_cpNotifier_thread_terminated = true;
		cond_->signal();
		cond_->mutex().release();
		cpnotifier_waitTermination_ = true;
		return -1;
	}

	// now wait for CP state changes notification
	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(sock_handles[0], &rfds);

	int counterForPoll = 0;
	log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - before while SELECT");
	while(1)
	{
		log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - into while SELECT");
		if(alh_cpNotifier_thread_stop){

			util.trace("acs_alh_cpnotifier::getCpState - CP_NOTIFIER_THREAD - start termination process");
			log.write(LOG_LEVEL_WARN, "acs_alh_cpnotifier::getCpState - CP_NOTIFIER_THREAD - start termination process");

			call_result = cp_state_notifier.detach();
			if(call_result < 0)
			{
				util.trace("acs_alh_cpnotifier::getCpState - detach() failed ! error_code == %d, error_descr == %s\n", cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());
				log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - detach() failed ! error_code == %d, error_descr == %s\n", cp_current_state_notifier.last_error(), cp_current_state_notifier.last_error_text());
			}

			alh_cpNotifier_thread_exit_code = 0;
			alh_cpNotifier_thread_terminated = true;

			util.trace("acs_alh_cpnotifier::getCpState - exit - Returning: 0");
			log.write(LOG_LEVEL_TRACE, "acs_alh_cpnotifier::getCpState - exit - Returning: 0");
			stop_cond_->mutex().acquire();
			//stop_cond_->signal();
			cpnotifier_waitTermination_ = true;
			stop_cond_->mutex().release();
			break;
		}
		log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - into while SELECT - alh_cpNotifier_thread_stop: %d", alh_cpNotifier_thread_stop);
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		FD_ZERO(&rfds);
		FD_SET(sock_handles[0], &rfds);

		call_result = select(sock_handles[0]+1, &rfds, NULL, NULL, &tv );
		log.write(LOG_LEVEL_WARN, "acs_alh_cpnotifier::getCpState - into while SELECT call_result: %i", call_result);
		if(call_result == -1)
		{
			//Call trace function
			util.trace("acs_alh_cpnotifier::getCpState - select() error ! errno == %d", errno);

			//Call logging function
			log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - select() error ! errno == %d", errno);

			call_result = cp_state_notifier.detach ();

			if(!alh_cpNotifier_thread_stop){
				cond_->mutex().acquire();
				alh_cpNotifier_thread_exit_code = -3;
				alh_cpNotifier_thread_terminated = true;
				cond_->signal();
				cond_->mutex().release();
			}
			cpnotifier_waitTermination_ = true;
			return -1;
		}
		else if(call_result == 0) //TIMEOUT
		{
			//Call logging function
			log.write(LOG_LEVEL_INFO, "CPNOTIFIER - Method: get_cp_state - select() timeout - counterForPoll: %d", counterForPoll);
			counterForPoll++;
			if(counterForPoll == 40){
				log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - into while SELECT TIMEOUT counterForPoll: %i", counterForPoll);
				current_cp_state_ = cp_state_;
				if(cp_state_notifier.get_cp_state(cp_state_)<0){
					//printf("cp_state_notifier.get_cp_state(cp_state_) FAILED\n");
					util.trace("acs_alh_cpnotifier::getCpState - failed ! ");
					log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - failed ! ");

					if(!alh_cpNotifier_thread_stop){
						cond_->mutex().acquire();
						alh_cpNotifier_thread_exit_code = -4;
						alh_cpNotifier_thread_terminated = true;
						cond_->signal();
						cond_->mutex().release();
					}
					//Call trace function
					util.trace("acs_alh_cpnotifier::getCpState - failed ! error_code == %d, error_descr == %s", cp_state_notifier.last_error(), cp_state_notifier.last_error_text());
					log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - failed ! error_code == %d, error_descr == %s", cp_state_notifier.last_error(), cp_state_notifier.last_error_text());

					call_result = cp_state_notifier.detach ();
					cpnotifier_waitTermination_ = true;
					return -1;
				}//end if for cp_state_notifier
				if(current_cp_state_ != cp_state_){
					if(!alh_cpNotifier_thread_stop){
						cond_->mutex().acquire();
						cond_->signal();
						cond_->mutex().release();
					}
					//Call trace function
					util.trace("acs_alh_cpnotifier::getCpState - State changed send notify to manager. New state: %d", cp_state_);
					log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - State changed send notify to manager. New state: %d", cp_state_);
				}else{
					//Call trace function
					util.trace("acs_alh_cpnotifier::getCpState - State not changed CP_state: %d", cp_state_);
					log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - State not changed CP_state: %d", cp_state_);
				}

			counterForPoll = 0;
			}//end if counterForPoll

			//printf("call_result==0\n");
			continue;
		}
		log.write(LOG_LEVEL_WARN, "acs_alh_cpnotifier::getCpState - into while after SELECT - alh_cpNotifier_thread_stop: %d - alh_cpNotifier_thread_exit_code: %d - alh_cpNotifier_thread_terminated: %d", alh_cpNotifier_thread_stop, alh_cpNotifier_thread_exit_code, alh_cpNotifier_thread_terminated);
		// CP state change notification arrived !
		if(cp_state_notifier.get_cp_state(cp_state_) < 0)
		{
			//Call trace function
			util.trace("acs_alh_cpnotifier::getCpState - failed ");

			//Call logging function
			log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - failed ! ");
			log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - alh_cpNotifier_thread_stop: %d", alh_cpNotifier_thread_stop);
			if(!alh_cpNotifier_thread_stop){
				cond_->mutex().acquire();
				alh_cpNotifier_thread_exit_code = -4;
				alh_cpNotifier_thread_terminated = true;
				cond_->signal();
				cond_->mutex().release();
			}
			//Call trace function
			util.trace("acs_alh_cpnotifier::getCpState - failed ! error_code == %d, error_descr == %s", cp_state_notifier.last_error(), cp_state_notifier.last_error_text());

			//Call logging function
			log.write(LOG_LEVEL_ERROR, "acs_alh_cpnotifier::getCpState - failed ! error_code == %d, error_descr == %s", cp_state_notifier.last_error(), cp_state_notifier.last_error_text());

			call_result = cp_state_notifier.detach ();
			cpnotifier_waitTermination_ = true;

			return -1;
		}
		log.write(LOG_LEVEL_DEBUG, "acs_alh_cpnotifier::getCpState - alh_cpNotifier_thread_stop: %d", alh_cpNotifier_thread_stop);
		if(!alh_cpNotifier_thread_stop){
			cond_->mutex().acquire();
			cond_->signal();
			cond_->mutex().release();
		}

		//Call trace function
		util.trace("acs_alh_cpnotifier::getCpState - exit -  New CP state is: %d", cp_state_);

		//Call logging function
		log.write(LOG_LEVEL_INFO, "acs_alh_cpnotifier::getCpState - exit - New CP state is: %d", cp_state_);

	}//end while
	log.write(LOG_LEVEL_WARN, "acs_alh_cpnotifier::getCpState - return(0) - alh_cpNotifier_thread_stop: %d - alh_cpNotifier_thread_exit_code: %d - alh_cpNotifier_thread_terminated: %d", alh_cpNotifier_thread_stop, alh_cpNotifier_thread_exit_code, alh_cpNotifier_thread_terminated);
	return 0;
}
