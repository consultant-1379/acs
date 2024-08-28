#ifndef ACS_TRA_AUTOFLUSHTHREADMANAGER_HEADER
#define ACS_TRA_AUTOFLUSHTHREADMANAGER_HEADER

#include <sys/types.h>
#include <map>
#include "ace/Recursive_Thread_Mutex.h"
#include "ACS_TRA_AutoflushThread.h"

class ACS_TRA_AutoflushThreadManager
{
public:

	typedef unsigned long ACS_TRA_THREAD_HANDLE;

	ACS_TRA_AutoflushThreadManager() {};

	virtual ~ACS_TRA_AutoflushThreadManager();

	// create an autoflush thread, returning : an internal thread handle ( > 0) on success, 0 on failure.
	ACS_TRA_THREAD_HANDLE create_thread(trace_taskp p_task, char delimiter = ',');

	// start an autoflush thread. Returns 0 on success, -1 on failure.
	int start_thread(ACS_TRA_THREAD_HANDLE thr_hdl);

	// stop an autoflush thread. Returns 0 on success, -1 on failure.
	int stop_thread(ACS_TRA_THREAD_HANDLE thr_hdl);

	// destroy an autoflush thread, waiting for its termination. Returns 0 on success, 1 if thread not found,  -1 on other failure.
	int destroy_thread(ACS_TRA_THREAD_HANDLE thr_hdl);

	// get thread id ( from pthread library) associated to a given internal handle.
	ACE_thread_t get_thread_id(ACS_TRA_THREAD_HANDLE thr_hdl);

	// get OS thread id ( tid given by Linux) associated to a given internal handle.
	pid_t get_OS_thread_id(ACS_TRA_THREAD_HANDLE thr_hdl);

	// search thread handle by thread id
	ACS_TRA_THREAD_HANDLE find_thread_handle_by_thread_id(ACE_thread_t thr_id);

	// search thread handle by OS thread id
	ACS_TRA_THREAD_HANDLE find_thread_handle_by_OS_thread_id(pid_t os_tid);

	// restart all stopped autoflush threads
	int restart_stopped_threads();

private:

	typedef std::map<ACS_TRA_THREAD_HANDLE, ACS_TRA_AutoflushThread *> Threads_Collection;
	typedef Threads_Collection::value_type Thread_Collection_Elem;

	Threads_Collection _autoflush_threads;
	ACE_Recursive_Thread_Mutex _mutex;
	static ACE_Atomic_Op<ACE_Mutex, unsigned long> _next_available_key;
};

#endif
