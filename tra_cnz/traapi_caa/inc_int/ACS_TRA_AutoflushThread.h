#ifndef ACS_TRA_AUTOFLUSHTHREAD_HEADER
#define ACS_TRA_AUTOFLUSHTHREAD_HEADER

#include "ace/Condition_Attributes.h"
#include "ace/Monotonic_Time_Policy.h"
#include "ace/Thread_Mutex.h"
#include "ace/Condition_T.h"
#include "ace/Task.h"
#include "ace/Thread.h"
#include "ace/Mutex.h"
#include "ace/Atomic_Op_T.h"

#include "ACS_TRA_common.h"
#include "ACS_TRA_lib.h"


// This class (used by TRAUTIL command) represents a thread whose function is to handle automatic flush of trace data
// for a single trace task having AUTOFLUSH feature activated
class ACS_TRA_AutoflushThread: public ACE_Task_Base
{
	friend class ACS_TRA_AutoflushThreadManager;

private:

	typedef unsigned long threadKey_t;

	// start thread
	int start();

	// stop thread (optionally waiting for thread termination)
	int stop(bool wait_termination = false);

	// get thread ID (as assigned by pthreads library)
	ACE_thread_t get_thread_id() { return _thr_id; }

	// get thread ID (as assigned by OS)
	pid_t get_OS_thread_id() { return _os_tid; }

	//  construct the object assigning to it a key that uniquely identifies it with respect to other object instances
	ACS_TRA_AutoflushThread(trace_taskp p_task, char delimiter = ',');

	// destroy the object after having stopped the associated thread
	inline virtual ~ACS_TRA_AutoflushThread() {stop(false); wait();}

	// deny object copy for not friend classes
	ACS_TRA_AutoflushThread & operator = (const ACS_TRA_AutoflushThread &);

	// thread function
	int svc();

	ACE_thread_t _thr_id;				// thread id ( coming from pthread)
	pid_t		_os_tid;				// thread id ( coming from Operating System)
	trace_taskp _trace_taskp;			// pointer to the trace task (in the top-level shared memory segment)
	char _delimiter;					// character to be used to delimit each row of the task log buffer

	char _orig_taskname[MAX_TASK_NAME];	// task name retrieved (at time of object construction) from the trace task
	int _orig_autoflush_param;			// autoflush parameter retrieved (at time of object construction) from the trace task
	int _orig_task_smid;				// shared memory identifier retrieved (at time of object construction) from the trace task

	void * _shmp;						// address where the shared memory segment of the trace task is attached

	// attributes used to support graceful thread termination
	ACE_Condition_Attributes_T<ACE_Monotonic_Time_Policy> _condition_attributes;
	ACE_Thread_Mutex _thread_mutex;
	ACE_Condition<ACE_Thread_Mutex> _stop_working_condition;
};

#endif // ACS_TRA_AUTOFLUSHTHREAD_HEADER
