#include "ACS_TRA_AutoflushThreadManager.h"
#include <algorithm>


ACE_Atomic_Op<ACE_Mutex, unsigned long> ACS_TRA_AutoflushThreadManager::_next_available_key = 1;


ACS_TRA_AutoflushThreadManager::~ACS_TRA_AutoflushThreadManager()
{
	for(Threads_Collection::iterator it = _autoflush_threads.begin(); (it != _autoflush_threads.end()); ++it)
		if(it->second)
		{
			it->second->stop(true);
			delete it->second;
			it->second = 0;
		}

	_autoflush_threads.clear();
}


ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE  ACS_TRA_AutoflushThreadManager::create_thread(trace_taskp p_task, char delimiter)
{
	if(!p_task) return 0;

	// generate thread handle
	ACS_TRA_THREAD_HANDLE thr_hdl = _next_available_key++;

	// create a new thread for the given trace task
	ACS_TRA_AutoflushThread * pThread = new (std::nothrow) ACS_TRA_AutoflushThread(p_task, delimiter);
	if(pThread)
	{
		_mutex.acquire();
		std::pair<Threads_Collection::iterator, bool> ret  = _autoflush_threads.insert(Thread_Collection_Elem(thr_hdl, pThread));
		if(!ret.second)
		{	// very strange ! Should not occur !
			delete pThread;
			pThread = 0;
		}
		_mutex.release();
	}

	return pThread ? thr_hdl : 0;
}


int ACS_TRA_AutoflushThreadManager::destroy_thread(ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE thr_hdl)
{
	int retval = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> theGuard(_mutex);

	// search for thread object by handle
	Threads_Collection::iterator it = _autoflush_threads.find(thr_hdl);
	if(it == _autoflush_threads.end())
		retval = 1;	// thread object not found !
	else if (it->second == 0)
		retval = -1; // strange ! NULL pointer is associated to the given thread handle
	else if( (retval = it->second->stop(true)) == 0 ) // wait for thread termination
	{
		delete it->second;				// delete thread object
		_autoflush_threads.erase(it);	// remove thread object pointer from collection
	}
	else  retval = -1;

	return retval;
}


int ACS_TRA_AutoflushThreadManager::start_thread(ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE thr_hdl)
{
	int retval = 0;

	if(thr_hdl == 0) return -1;

	ACE_Guard<ACE_Recursive_Thread_Mutex> theGuard(_mutex);

	// search for thread and start it
	Threads_Collection::iterator it = _autoflush_threads.find(thr_hdl);
	if( (it == _autoflush_threads.end()) || (it->second == 0) || (it->second->start() < 0 ))
		retval = -1;

	return retval;
}


int ACS_TRA_AutoflushThreadManager::stop_thread(ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE thr_hdl)
{
	int retval = 0;

	if(thr_hdl == 0) return -1;

	ACE_Guard<ACE_Recursive_Thread_Mutex> theGuard(_mutex);

	// search for thread and stop it
	Threads_Collection::iterator it = _autoflush_threads.find(thr_hdl);
	if( (it == _autoflush_threads.end()) || (it->second == 0) || (it->second->stop(true) < 0 ))
		retval = -1;

	return retval;
}


ACE_thread_t ACS_TRA_AutoflushThreadManager::get_thread_id(ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE thr_hdl)
{
	if(thr_hdl == 0) return -1;

	ACE_Guard<ACE_Recursive_Thread_Mutex> theGuard(_mutex);

	// search for thread
	Threads_Collection::iterator it = _autoflush_threads.find(thr_hdl);
	return (it == _autoflush_threads.end() || it->second == 0 ) ? 0 : it->second->get_thread_id();
}


pid_t ACS_TRA_AutoflushThreadManager::get_OS_thread_id(ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE thr_hdl)
{
	if(thr_hdl == 0) return -1;

	ACE_Guard<ACE_Recursive_Thread_Mutex> theGuard(_mutex);

	// search for thread
	Threads_Collection::iterator it = _autoflush_threads.find(thr_hdl);
	return (it == _autoflush_threads.end() || it->second == 0 ) ? 0 : it->second->get_OS_thread_id();
}


ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE ACS_TRA_AutoflushThreadManager::find_thread_handle_by_thread_id(ACE_thread_t thr_id)
{
	if(thr_id == 0) return 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> theGuard(_mutex);

	for(Threads_Collection::iterator it = _autoflush_threads.begin(); it != _autoflush_threads.end(); ++it)
		if(it->second && (it->second->get_thread_id() == thr_id) )
			return it->first;

	return 0;
}

ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE ACS_TRA_AutoflushThreadManager::find_thread_handle_by_OS_thread_id(pid_t os_tid)
{
	if(os_tid == 0) return 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> theGuard(_mutex);

	for(Threads_Collection::iterator it = _autoflush_threads.begin(); it != _autoflush_threads.end(); ++it)
		if(it->second && (it->second->get_OS_thread_id() == os_tid) )
			return it->first;

	return 0;
}


int ACS_TRA_AutoflushThreadManager::restart_stopped_threads()
{
	unsigned int err_cnt = 0;

	for(Threads_Collection::iterator it = _autoflush_threads.begin(); it != _autoflush_threads.end(); ++it)
		if(it->second && (it->second->thr_count() == 0) && (it->second->start() < 0))
			++err_cnt;

	return err_cnt == 0 ? 0: -1;
}
