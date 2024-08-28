#include "ACS_TRA_AutoflushThread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>



ACS_TRA_AutoflushThread::ACS_TRA_AutoflushThread(trace_taskp p_task, char delimiter)
:_thr_id(0), _os_tid(0), _trace_taskp(p_task), _delimiter(delimiter), _stop_working_condition(_thread_mutex, _condition_attributes)
{
	// retrieve 'shm_id', 'taskname' and 'autoflush' parameters from the task pointed by 'p_task'
	bzero(_orig_taskname, MAX_TASK_NAME);
	if (p_task)
	{
		strncpy(_orig_taskname, p_task->name, MAX_TASK_NAME - 1);
		_orig_autoflush_param = p_task->autoflush;
		_orig_task_smid = p_task->shm_id;
		_shmp =  ACS_TRA_lib::attach_task_shm(p_task);
	}
	else
	{
		_orig_autoflush_param = 0;
		_orig_task_smid = -1;
		_shmp = 0;
	}

	/*
	// get reference to the shared memory of the task
	_shmid = shmget ((key_t)(55500 + p_task->refcount), (size_t) p_task->shm_size, 0666);
	if (_shmid == -1)
	{
		ACE_OS::fprintf(stdout, "Failure: unable to get reference to the shared memory of the task '%s' !\n", p_task->name);
		return;
	}

	// attach the shared memory segment of the task into the virtual address space of the process
	_shmp = shmat (_shmid, NULL, 0);
	if (_shmp == (void *) -1)
		ACE_OS::fprintf(stdout, "Failure: unable to get reference to the shared memory of the task '%s' !\n", p_task->name);
	*/

}


int ACS_TRA_AutoflushThread::start()
{
	if(!_trace_taskp)
	{
		ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_AutoflushThread::start(): Invalid attribute: '_trace_taskp' !\n");
		return -1;
	}

	if(_shmp == (void *) -1)
	{
		ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_AutoflushThread::start(): Invalid attribute: '_shmp' !\n");
		return -1;
	}

	if(thr_count() > 0)
	{
		ACE_OS::fprintf(stdout, "WARNING: Autoflush thread already started for task '%s'. THREAD_ID == %lu\n", _orig_taskname, _thr_id);
		return 1;
	}

	// start thread
	if(activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1, 0, ACE_DEFAULT_THREAD_PRIORITY, -1, 0, 0, 0, 0, &_thr_id) != 0)
	{
		ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_AutoflushThread::start(): Unable to start thread !\n");
		return -2;
	}

	// wait until OS thread ID assigned ( max 10 seconds, but this operation should be instantaneous !! )
	for(int i = 0; (i < 100) && !_os_tid; ++i)
		usleep(100000);

	// ACE_OS::fprintf(stdout, "DEBUG: Autoflush thread successfully started for task '%s'. OS THREAD_ID == %d\n", _orig_taskname, _os_tid);

	return (_os_tid != 0) ? 0 : -3;
}


int ACS_TRA_AutoflushThread::stop(bool wait_termination)
{
	if(thr_count() == 0)
	{ // the thread is already stopped
		wait();     // to be sure that thread resources are released
		return 0;
	}

	int retval = _stop_working_condition.signal();
	if ((retval == 0) && wait_termination)
		retval = wait();

/*
	if(retval == 0)
		ACE_OS::fprintf(stdout, "DEBUG: Autoflush thread successfully stopped for task '%s'. OS THREAD_ID == %d\n", _orig_taskname, _os_tid);
	else
		ACE_OS::fprintf(stdout, "ERROR: Autoflush thread NOT stopped for task '%s'. OS THREAD_ID == %d\n", _orig_taskname, _os_tid);
*/
	return retval;
}


int ACS_TRA_AutoflushThread::svc()
{
	int exit_code = 0;

	_os_tid = syscall(SYS_gettid);

	for(;;)
	{
		// Wait 'stop work signal' for 'autoflush_param' seconds
		ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
		timeout = timeout.now() + ACE_Time_Value(_orig_autoflush_param);
		errno = 0;
		if (_stop_working_condition.wait(&timeout) == 0)
			break; 	// a request for stopping thread has been received.

		// we should be here for timeout elapsed (errno == ETIME). Check it
		if(errno != ETIME)
		{
			ACE_OS::fprintf(stdout, "Task %s: failure waiting on 'stop work condition' ! errno == '%d'\n", _orig_taskname, errno);
			exit_code = -1;
			break;
		}

		// check if the task is still alive (searching it into the table of tasks of the top-level shared memory )
		if(ACS_TRA_lib::acquire_shared_memory_mutex() == -1 )
		{
			ACE_OS::fprintf(stdout, "Failure: unable to acquire the shared memory mutex !\n");
			exit_code = -1;
			break;	// going to terminate the thread
		}

		if((_trace_taskp->refcount == -1) || (_trace_taskp->shm_id != _orig_task_smid ))
		{
			// the task is no longer alive
			if(ACS_TRA_lib::release_shared_memory_mutex() == -1)
				ACE_OS::fprintf(stdout, "Failure: unable to release the shared memory mutex !\n");

			// ACE_OS::fprintf(stdout, "DEBUG: Task '%s' handled by this autoflush thread is no more alive ! Going to terminate the thread. THREAD_ID == %lu\n", _orig_taskname, _thr_id);
			exit_code = 0;
			break;	// going to terminate the thread
		}

		// flush the log buffer of the task ( on SCREEN or on FILE)
		if (_trace_taskp->logBuff.typ == PID  ||  _trace_taskp->logBuff.typ == LOG)
		{
			if (_trace_taskp->filename[0] == 0)
				ACE_OS::fprintf(stdout, "\n");

			ACS_TRA_lib::display_log(1, _delimiter, _trace_taskp, 1/*don't acquire mutex*/, _shmp /* pointer to the shared memory of the task */ );
		}
		else
			ACE_OS::fprintf(stdout, "Task filter is not log\n");

		/*
		 * Print the prompt command line if is
		 * active only the AUTOFLUSH option
		 * without the filename option
		 */
		if( (_trace_taskp->autoflush > 0) && (_trace_taskp->filename[0] == 0) )
		{
			// Print on standard output the prompt of the command
			ACE_OS::fputs("trc> ", stdout);
			// Flush the specified trace task name on the standard output
			ACE_OS::fflush (stdout);
		}

		// release exclusive access to top-level shared memory
		if(ACS_TRA_lib::release_shared_memory_mutex() == -1)
			ACE_OS::fprintf(stdout, "Failure: unable to release the shared memory mutex !\n");
	}

	return exit_code;
}
