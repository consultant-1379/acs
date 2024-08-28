#ifndef ACS_DSD_TASK_BASE_H_
#define ACS_DSD_TASK_BASE_H_

/** @file ACS_DSD_Task_Base.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-18
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2010-11-18 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ace/Task.h"
#include "ace/Reactor.h"
#include "ACS_DSD_ConfigParams.h"
#include "ace/Thread.h"

/** @class ACS_DSD_Task_Base ACS_DSD_Task_Base.h
 *	@brief ACS_DSD_Task_Base class
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-18
 *	@version 0.1
 *
 *	ACS_DSD_Task_Base Class detailed description
 */
class ACS_DSD_Task_Base: public ACE_Task_Base
{
	friend class Thread_Work_Notifyer;

public:
	// State of the object
	enum ACS_DSD_Task_State {
		UP = 0,
		UP_PARTIALLY = 1,
		STOPPING = 2,
		DOWN = 3,
		DOWN_NO_MEMORY = 4
	};

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_DSD_Task_Base Default constructor
	 *
	 *	ACS_DSD_Task_Base Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	ACS_DSD_Task_Base(const char *task_name = "anonymous", uint n_threads = 1, ACE_Reactor * reactor = 0);


	//==============//
	// Destructor   //
	//==============//

	/** @brief ACS_DSD_Task_Base Destructor
	 *
	 *	ACS_DSD_Task_Base Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_DSD_Task_Base();



	//===========//
	// Functions //
	//===========//

	/// Returns the name of the Task
	const char *get_name() { return _task_name; }


	/** @brief svc method
	 *
	 *	svc method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int svc (void);


	/** @brief start_activity method
	 *
	 *	start_activity method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int start_activity();


	/** @brief start_activity method
	 *
	 *	stop_activity method detailed description
	 *
	 *	@param[in] wait_termination Indicates if the method must wait for thread termination
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int stop_activity(bool wait_termination);


	/** @brief get_num_active_threads method
	 *
	 *	get_num_active_threads method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint32_t get_num_active_threads();


	/** @brief set_num_threads method
	 *
	 *	get_num_active_threads method detailed description
	 *
	 *	@param[in] n_threads Description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_num_threads(uint32_t n_threads) { return ((_state == DOWN) ?  (this->_n_threads = n_threads),0 : -1); }


	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ACS_DSD_Task_State state() { return _state; };


	/** @brief set_dynamic_threads_handling_params method
	 *
	 *	set_dynamic_threads_handling_params method detailed description
	 *
	 *	@param[in] enabled Description
	 *	@param[in] min_num_idle Description
	 *	@param[in] max_num_idle Description
	 *	@param[in]
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_dynamic_threads_handling_params(bool enabled, uint32_t min_num_idle = TASK_DEFAULT_MIN_NUM_THREADS_IDLE, uint32_t max_num_idle = TASK_DEFAULT_MAX_NUM_THREADS_IDLE, uint32_t max_num_threads = TASK_DEFAULT_MAX_NUM_THREADS);

	/** @brief notify_work_start_for_thread method
	 *
	 *	notify_work_start_for_thread method detailed description
	 *
	 *	@param[in] t_id Description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int notify_work_start_for_thread(ACE_thread_t t_id = ACE_Thread::self());


	/** @brief notify_work_end_for_thread method
	 *
	 *	notify_work_end_for_thread method detailed description
	 *
	 *	@param[in] t_id Description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int notify_work_end_for_thread(ACE_thread_t t_id = ACE_Thread::self());


private:
	/** @brief event_hook_func method
	 *
	 *	event_hook_func method detailed description
	 *
	 *	@param[in] reactor Description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int event_hook_func(ACE_Reactor * /*reactor*/) { return 0; }


protected:
	char _task_name[TASK_NAME_SIZE_MAX];		/// Task name
	ACE_Reactor * _reactor;						/// reactor associated to the Task
	ACS_DSD_Task_State _state;					/// object state
	uint32_t _n_threads;						/// number of threads active in the Task
	bool _destroy_reactor;						/// true if <_reactor> attribute has been created by the constructor (and so we have to deallocate)

	/// dynamic handling of threads
	ACE_Recursive_Thread_Mutex _mutex;
	bool _dynamic_handling_enabled;				/// indicates if dynamic threads handling is enabled
	uint32_t _n_threads_idle;					/// number of threads in IDLE state
	uint32_t _n_canceled_threads;				/// number of threads marked for cancellation
	uint32_t _min_num_threads_idle;				/// minimum number of IDLE threads that must be in the task
	uint32_t _max_num_threads_idle;				/// maximum number of IDLE threads that can be in the task
	uint32_t _max_num_threads;					/// maximum number of threads that can be in the task
};


/// Helper Class that can be used in a code block (for example the body of a function) to automatically notify an ACE_DSD_Task_Base object
/// about the <start of work> and the <end of work> for a specific thread
class Thread_Work_Notifyer
{
public:
	inline Thread_Work_Notifyer(ACS_DSD_Task_Base *task, ACE_thread_t t_id = ACE_Thread::self()):_task(task),_t_id(t_id)
	{
		if(task)
			_task->notify_work_start_for_thread(t_id);
	}

	inline virtual ~Thread_Work_Notifyer()
	{
		if(_task)
			_task->notify_work_end_for_thread(_t_id);
	}
private:
	ACS_DSD_Task_Base * _task;
	ACE_thread_t _t_id;
};


#endif /* ACS_DSD_TASK_BASE_H_ */
