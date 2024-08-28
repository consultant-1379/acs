#include "ace/TP_Reactor.h"

#include "ACS_DSD_ConfigParams.h"
#include "ACS_DSD_Task_Base.h"
//#include "ACS_DSD_Logger.h"
#include "ACS_DSD_Logger_v2.h"

extern ACS_DSD_Logger dsd_logger;

ACS_DSD_Task_Base::ACS_DSD_Task_Base(const char *task_name, uint n_threads, ACE_Reactor * reactor)
{
	if(task_name)
	{
		strncpy(_task_name, task_name, TASK_NAME_SIZE_MAX);
		_task_name[TASK_NAME_SIZE_MAX - 1] = 0;
	}
	else
		bzero(_task_name, sizeof(_task_name));

	_destroy_reactor = false;

	if(reactor)
		_reactor = reactor;
	else
	{
		// if reactor has not been passed, create our private reactor, using ACE_TP_Reactor implementation
		ACE_TP_Reactor *reactor_TP_impl = new (std::nothrow) ACE_TP_Reactor();
		if(reactor_TP_impl)
		{
			_reactor = new (std::nothrow) ACE_Reactor(reactor_TP_impl, true);	// reactor implementation will be deleted when reactor will be destroyed
			if(_reactor)
				_destroy_reactor = true; // take note that we must take care for reactor deallocation
			else
				delete reactor_TP_impl;	// no memory available ...  deallocate memory allocated for reactor TP implementation
		}
	}

	// check if reactor is OK
	if(!_reactor)
	{
		_state = DOWN_NO_MEMORY;
		return;
	}

	// set threads number in the POOL
	_n_threads = n_threads;

	// set attributes for dynamic threads handling
	_dynamic_handling_enabled = false;
	_n_threads_idle = n_threads;
	_n_canceled_threads = 0;
	_min_num_threads_idle = TASK_DEFAULT_MIN_NUM_THREADS_IDLE;
	_max_num_threads_idle = TASK_DEFAULT_MAX_NUM_THREADS_IDLE;
	_max_num_threads = TASK_DEFAULT_MAX_NUM_THREADS;

	// set initial state
	_state = DOWN;
}

ACS_DSD_Task_Base::~ACS_DSD_Task_Base()
{
	if(_destroy_reactor)
		delete _reactor;
}

int ACS_DSD_Task_Base::svc (void)
{
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > - thread starting up"), _task_name);
	if(!_reactor)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);
		--_n_threads_idle;
		--_n_threads;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > -  reactor is NULL, thread is ENDING. The new STATE OF THREADS will be : nTot = < %u >, nIDLE = < %u >, nCANCELED = < %u > !"), _task_name, _n_threads, _n_threads_idle, _n_canceled_threads );
		return -1;	// the task is inconsistent; terminate thread execution returning -1
	}

	// execute reactor event loop using a timeout (to implement cooperative thread cancellation)
	ACE_Time_Value event_loop_timeout(ACS_DSD_TASK_BASE_EV_LOOP_TIMEOUT);
	int result = 0;
	bool bCanceled = false;
	while(!_reactor->reactor_event_loop_done() && !result && !bCanceled)
	{
		ACE_Time_Value tv(event_loop_timeout);
          	// Start of TR IA80344
                try
                {
                        result =_reactor->run_reactor_event_loop(tv);
                }
                catch(const std::exception &e)
                {

                        ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Exception: < %s >"), e.what());
                        ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Exception caught when calling run_reactor_event_loop()"));
                }
                catch(...)
                {

                        ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Unknown exception caught when calling run_reactor_event_loop()"));
                }
		// End of TR IA80344
                bCanceled = (this->thr_mgr_->testcancel(ACE_Thread::self()));
	}

	if(bCanceled)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > - the thread has been CANCELED !"), _task_name);
		--_n_canceled_threads;		// update canceled thread counter
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);
	--_n_threads;
	--_n_threads_idle;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("Task < %s > - the thread is ENDING ( with result == < %d > ), The new STATE OF THREADS will be : nTot = < %u >, nIDLE = < %u >, nCANCELED = < %u > !"), _task_name, result,  _n_threads, _n_threads_idle, _n_canceled_threads );

	return result;
}


int ACS_DSD_Task_Base::start_activity()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Starting TASK < %s > ..."), _task_name);

	// launch threads
	int call_result = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, _n_threads);
	if(call_result < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > NOT STARTED ! ACE_Task_Base::activate() returned < %d >"),_task_name, call_result);
		return -1;
	}

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > successfully STARTED !"), _task_name);

	// update state
	_state = UP;
	return 0;
}


int ACS_DSD_Task_Base::stop_activity(bool wait_termination)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);
	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Stopping TASK < %s > ..."), _task_name);

	if(_state == DOWN)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("TASK < %s > is already STOPPED !"),_task_name);
		return 0;
	}

	if(_state == STOPPING)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_INFO, ACS_DSD_TEXT("TASK < %s > is already STOPPING !"),_task_name);
		return 0;
	}

	// terminate reactor event loop for all threads in the task
	int call_result = _reactor->end_reactor_event_loop();
	if(call_result < 0)
	{
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > NOT STOPPED ! ACE_Reactor::end_reactor_event_loop() returned < %d >"),_task_name, call_result);
		return -1;
	}

	// update state
	_state = STOPPING;

	// release Task Mutex
	_mutex.release();

	if(wait_termination)
	{
		// wait for threads termination
		call_result = this->wait();
		if(call_result == -1)
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("TASK < %s > NOT STOPPED ! Call 'ACE_Task_Base::wait()' returned < %d > ! errno == %d"),_task_name, call_result, errno);
	}

	// update state
	_state = DOWN;

	ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > successfully  STOPPED !"), _task_name);
	return call_result;
}

uint32_t ACS_DSD_Task_Base::get_num_active_threads()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->_mutex);
	return ( ((_state == UP) || (_state == UP_PARTIALLY)) ? _n_threads: 0);
}


int ACS_DSD_Task_Base::set_dynamic_threads_handling_params(bool enabled, uint32_t min_num_idle, uint32_t max_num_idle, uint32_t max_num_threads)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->_mutex);

	_dynamic_handling_enabled = enabled;
	if(enabled)
	{
		if( (min_num_idle > max_num_idle) || (max_num_idle > max_num_threads) )
			return -1;

		_min_num_threads_idle = min_num_idle;
		_max_num_threads_idle = max_num_idle;
		_max_num_threads = max_num_threads;
	}

	return 0;
}

int ACS_DSD_Task_Base::notify_work_start_for_thread(ACE_thread_t /*t_id*/)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->_mutex);

	// update idle threads counter
	--_n_threads_idle;

	// if the task is down or is stopping, do nothing
	if((_state != UP) && (_state != UP_PARTIALLY))
		return 0;

	//ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > - Start of Work notification ! The STATE OF THREADS is :  nTot = < %u >, nIDLE = < %u >, nCANCELED = < %u > !"), _task_name,  _n_threads, _n_threads_idle, _n_canceled_threads);

	if(!_dynamic_handling_enabled)
		return  0;  // dynamic threads handling feature is OFF, we've terminated our work

	// check if we have to add threads to the task
	int n1 = _min_num_threads_idle - _n_threads_idle;

	if(n1 > 0)
	{
		// we have to add threads to the task

		// first, calculate the max number of threads that we can add to the task
		int n2 = _max_num_threads - _n_threads;

		if(n2 <= 0)
		{
			// we cannot add new threads because we've reached the maximum number of threads allowed
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_WARN, ACS_DSD_TEXT("Task < %s > - we should add threads to the task, but we can't because we've reached the maximum number  Of threads allowed, that is < %d > !"), _task_name, _max_num_threads);
			return 0;
		}

		// calculate the number of threads to be added to the task
		int n_to_add = (n1 <= n2 ? n1 : n2);

		// activate the new threads
		int op_res = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, n_to_add, 1);
		if(op_res)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > -  ACE_Task_Base::activate() failure. Return code is < %d > !"),  _task_name, op_res);
			return -1;
		}
		else
		{
			// update the counters
			_n_threads += n_to_add;
			_n_threads_idle += n_to_add;
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > - < %d > new threads ADDED ! The new STATE OF THREADS will be : nTot = < %u >, nIDLE = < %u >, nCANCELED = < %u > !"), _task_name, n_to_add,  _n_threads, _n_threads_idle, _n_canceled_threads );
		}
	}

	return 0;
}

int ACS_DSD_Task_Base::notify_work_end_for_thread(ACE_thread_t t_id)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->_mutex);

	// update idle threads counter
	++_n_threads_idle;

	// if the task is down or is stopping, do nothing
	if((_state != UP) && (_state != UP_PARTIALLY))
		return 0;

	// ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > - End of Work notification !  The STATE OF THREADS is : nTot = < %u >, nIDLE = < %u >, nCANCELED = < %u > !"), _task_name,  _n_threads, _n_threads_idle, _n_canceled_threads );

	// check if DYNAMIC THREADS FEATURE is ON and if we have to remove the thread <t_id> from the task
	if(_dynamic_handling_enabled  && ( (_n_threads_idle - _n_canceled_threads) > _max_num_threads_idle ))
	{
		// ok, we have to cancel the thread from the task
		ACE_Thread_Manager * thr_mgr = this->thr_mgr_;

		// the thread could have been already canceled. so check for this condition
		if(thr_mgr->testcancel(t_id))
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > - the thread < %lu > has been already marked for cancellation !"), _task_name, t_id);
			return 0;
		}

		// cancel the thread
		int callresult = thr_mgr->cancel(t_id);
		if(callresult)
		{
			ACS_DSD_LOG(dsd_logger, LOG_LEVEL_ERROR, ACS_DSD_TEXT("Task < %s > - ACE_Thread_Manager::cancel() failure for thread < %lu >. Return code is < %d > !"),  _task_name, t_id, callresult);
			return -1;
		}

		// update the canceled threads counter
		++_n_canceled_threads;
		ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Task < %s > - the thread < %lu > has been marked for cancellation. The new STATE OF THREADS will be : nTot = < %u >, nIDLE = < %u >, nCANCELED = < %u > !"), _task_name, t_id, _n_threads, _n_threads_idle, _n_canceled_threads );
		return 1;  // communicate the cancellation of the thread
	}

	return 0;
}
