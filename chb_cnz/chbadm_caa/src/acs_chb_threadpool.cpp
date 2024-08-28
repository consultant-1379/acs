/*=================================================================== */
   /**
   @file acs_chb_threadpool.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_threadpool.h module

   @version 1.0.0
	*/
	/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/01/2011   XNADNAR   Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <ace/Signal.h>
#include <ace/Log_Msg.h>
#include <ace/ACE.h>
#include <ace/OS_NS_sys_select.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <acs_chb_threadpool.h>
#include <acs_chb_tra.h>
#include <sys/eventfd.h>
#include <poll.h> 
using namespace std;

//namespace
//{
/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
//Error description string constants
/*=================================================================== */
/**
   @brief D_OK
 */
/*=================================================================== */
#define  D_OK "No errors."
/*=================================================================== */
/**
   @brief DERR_CRITICAL_SECTION_INITIALIZE
 */
/*=================================================================== */
#define DERR_CRITICAL_SECTION_INITIALIZE "Failing to initialize internal synchronization system objects! Probably low memory conditions."
/*=================================================================== */
/**
   @brief DERR_CRITICAL_SECTION_ENTER
 */
/*=================================================================== */
#define DERR_CRITICAL_SECTION_ENTER "Failing to enter into critical section: critical section object is corrupt or a possible deadlock detected!"
/*=================================================================== */
/**
   @brief DERR_GENERIC_ERROR
 */
/*=================================================================== */
#define DERR_GENERIC_ERROR "A not specified error occurred!"
/*=================================================================== */
/**
   @brief DERR_WAKEUP_TO_RUN_EVENT_CREATION
 */
/*=================================================================== */
#define DERR_WAKEUP_TO_RUN_EVENT_CREATION "Failing to create the internal wake-up-to-run event!"
/*=================================================================== */
/**
   @brief DERR_WAKEUP_TO_RUN_EVENT_SET
 */
/*=================================================================== */
#define DERR_WAKEUP_TO_RUN_EVENT_SET "Failing to set the internal wake-up-to-run event to signaled state!"
/*=================================================================== */
/**
   @brief DERR_CREATE_POOL_THREAD
 */
/*=================================================================== */
#define DERR_CREATE_POOL_THREAD "Failing to create a new pool thread!"
/*=================================================================== */
/**
   @brief DERR_ALLOCATING_THREAD_INFO_RECORD
 */
/*=================================================================== */
#define DERR_ALLOCATING_THREAD_INFO_RECORD "The new operator failed to allocate a thread activation record! Probably low memory condition."
/*=================================================================== */
/**
   @brief DERR_RESUMING_NEW_THREAD
 */
/*=================================================================== */
#define DERR_RESUMING_NEW_THREAD "Failing to resume a newly created thread."
/*=================================================================== */
/**
   @brief DERR_MAP_INSERTION
 */
/*=================================================================== */
#define DERR_MAP_INSERTION "Failing to insert into internal map the thread info record!"
/*=================================================================== */
/**
   @brief DERR_WAKEUP_TO_EXIT_EVENT_CREATION
 */
/*=================================================================== */
#define DERR_WAKEUP_TO_EXIT_EVENT_CREATION "Failing to create the internal wake-up-to-exit event!"
/*=================================================================== */
/**
   @brief DERR_ACE_COND_CREATION
 */
/*=================================================================== */
#define DERR_ACE_COND_CREATION "Failing to create the ace condition !"
/*=================================================================== */
/**
   @brief DERR_WAIT_FUNCTIONS_FAILED
 */
/*=================================================================== */
#define DERR_WAIT_FUNCTIONS_FAILED "At least one system wait function failed into an internal thread!"
/*=================================================================== */
/**
   @brief DWAR_STOP_REQUESTED
 */
/*=================================================================== */
#define DWAR_STOP_REQUESTED "Stop requested!"

/*=================================================================== */
/**
   @brief SigEvent0
 */
/*=================================================================== */
//static bool SigEvent0=false;
/*=================================================================== */
/**
   @brief SigEvent1
 */
/*=================================================================== */
//static bool SigEvent1=false;

//}
/*=================================================================== */
/**
   @brief ERR_CRITICAL_SECTION_ENTER
 */
/*=================================================================== */
const ACE_INT32 ThreadPool::ERR_CRITICAL_SECTION_ENTER ;
/*=================================================================== */
/**
   @brief ERR_GENERIC_ERROR
 */
/*=================================================================== */
const ACE_INT32 ThreadPool::ERR_GENERIC_ERROR ;


/*===================================================================
   ROUTINE: fxInitialize
=================================================================== */
int ThreadPool::fxInitialize()
{
	DEBUG(1,"%s","....................Inside fxInitialize function.................");
	if (_maxThread <= 0)
	{
		_maxThread = 1;
	}

	DEBUG(1,"Maximum number of permissible threads : %d\n",_maxThread);

	if (_minThread > _maxThread)
	{
		_minThread = _maxThread;
	}
	DEBUG(1,"Minimum number of permissible threads : %d\n",_minThread);

	try
	{

		_accessControl = new ACE_Recursive_Thread_Mutex(); ////ported

		_accessControlInitialized = true;
		DEBUG(1,"%s","Critical section Initialized!!\n");
	}
	catch (...)
	{
		ERROR(1,"%s","Error encountered while initializing critical section...");

		_lastErrorDescription = DERR_CRITICAL_SECTION_INITIALIZE;

		ERROR(1,"Error Description : %s","lastErrorDescription");

		return (_lastErrorNumber = ERR_CRITICAL_SECTION_INITIALIZE);
	}

	DEBUG(1,"%s","Creating WAKEUP_TO_RUN_EVENT\n");

	_wakeupEvents[0] = eventfd(0,0); 

	if ( _wakeupEvents[0] == -1 ) 		
	{

		 ERROR(1,"%s","Error encountered while creating Event for wake-up to run!!");
                _lastErrorDescription = DERR_WAKEUP_TO_RUN_EVENT_CREATION;

                ERROR(1,"Error Description : %s",lastErrorDescription());
                return (_lastErrorNumber = ERR_WAKEUP_TO_RUN_EVENT_CREATION);
	
	}	

	_wakeupEvents[1] = eventfd(0,0); 
	if( _wakeupEvents[1] == -1 )	
	{
		 ERROR(1,"%s","Error encountered while creating Event for wake-up to exit!!");
                _lastErrorDescription = DERR_WAKEUP_TO_EXIT_EVENT_CREATION;

                ERROR(1,"Error Description : %s",lastErrorDescription());
                return (_lastErrorNumber = ERR_WAKEUP_TO_EXIT_EVENT_CREATION);

		
	}
	_lastErrorDescription = D_OK;
	DEBUG(1,"%s","Returning from fxInitialize function ");
	return (_lastErrorNumber = 0);
}//End of fxInitialize

/*===================================================================
   ROUTINE: fxFinalize
=================================================================== */
int ThreadPool::fxFinalize()
{

	DEBUG(1,"%s"," Inside fxFinalize function ");
	DEBUG(1,"%s","Signaling WAKEUP_TO_EXIT_EVENT......\n");

	eventfd_t wakeupEvent1 = 1U;
	int returnFD = eventfd_write(_wakeupEvents[1], wakeupEvent1);
	if(returnFD != 0)
        {
                ERROR(1,"%s"," ThreadPool::fxFinalize Failed in signalling _wakeupEvents[1]  ");
        }
	else
	{
		 DEBUG(1, "%s", " eventfd_write successful in fxFinalize");	

	}	

	{ //BEGIN CRITICAL SECTION
		_accessControl->acquire();


		//1) Signal all the user procedure into running threads in _runningThreads map to stop their execution.
		for (std::map<ACE_UINT32, ThreadInfoRecord *>::iterator it = _runningThreads.begin(); it != _runningThreads.end(); ++it)
		{
			if (it->second && (it->second->userProc))
			{
				DEBUG(1, "%s", "Sigalling thread_jtp_comm operator to stop");
				it->second->userProc->stop();
			}
		}
		if (_autoDelete) //2) Delete all object into _procDeque deque
		{

			for (std::deque<procParamPair_t>::iterator it = _procDeque.begin(); it != _procDeque.end(); ++it)
			{
				try
				{

					if( it->first != 0)
					{
						delete (it->first);
						it->first = 0;
					}
				}
				catch (...)
				{
				}
			}
		}

		_accessControl->release();
	} //END CRITICAL SECTION

	//3) Delete all object into _stoppedThreads deque (this call the CloseHandle function)
	fxDeleteThreadsNotRunning();

	//4) Safe wait: wait a moment, control if all threads into _runningThreads map was stopped.
	for (ACE_INT32 i = 0; (i < 20) && !_runningThreads.empty(); ++i, usleep(50000)) ;

	//5) Terminate all remaining threads into _runningThreads map and delete all elements.
	{ //BEGIN CRITICAL SECTION
		_accessControl->acquire();
//		usleep(50000);
		struct timespec req;
		req.tv_sec =0;
		req.tv_nsec = 50000 * 1000L;
		nanosleep(&req,(struct timespec *)NULL);

		for (std::map<ACE_UINT32, ThreadInfoRecord *>::iterator it = _runningThreads.begin(); it != _runningThreads.end(); ++it)
		{
			if (it->second)
			{
				if (it->second->threadHandle)
				{
					try
					{
						ACE_Thread::cancel(it->second->threadHandle);
						DEBUG(1,"%s","In fxFinalize, closing thread! Done!! ");
						
					}
					catch (...)
					{
					}
				}
				if (it->second->userProc && _autoDelete)
				{
					try
					{
						DEBUG(1,"%s","fxFinalize, deleting ACS_CHB_Thread_JTPcomm object");
						delete it->second->userProc;
						it->second->userProc = 0;
					}
					catch (...)
					{
					}
				}
			}
		}
		_runningThreads.clear();

		_accessControl->release();
	} //END CRITICAL SECTION

	//6) Delete all object into _stoppedThreads deque (this call the CloseHandle function)
	fxDeleteThreadsNotRunning();

	if (_wakeupEvents[0] != -1)
	{
		ACE_OS::close(_wakeupEvents[0]);		

	}
	if (_wakeupEvents[1] != -1 )
	{
		ACE_OS::close(_wakeupEvents[1]);	
	}
	if (_accessControlInitialized)
	{
		try
		{
			_accessControl->remove(); //ported
		}
		catch (...)
		{
		}
	}

	if( _accessControl != 0 )
	{
		delete _accessControl;
		_accessControl = 0;
	}
	DEBUG(1,"%s","Returning from function fxFinalize");
	return 0;
}//End of fxFinalize

/*===================================================================
   ROUTINE: queueUserWorkItem
=================================================================== */
int ThreadPool::queueUserWorkItem(ThreadProcedure * proc, void * procParameters)
{
	DEBUG(1,"%s","Inside function queueUserWorkItem");

	if (!_accessControlInitialized)
	{
		_lastErrorDescription = DERR_CRITICAL_SECTION_INITIALIZE;
		ERROR(1,"accessControlInitialized, error: %s",_lastErrorDescription);
		return (_lastErrorNumber = ERR_CRITICAL_SECTION_INITIALIZE);
	}

	if (_wakeupEvents[0] == -1)
	{
		_lastErrorDescription = DERR_WAKEUP_TO_RUN_EVENT_CREATION;
		ERROR(1,"wakeupEvents[0] , error: %s",_lastErrorDescription);
		return (_lastErrorNumber = ERR_WAKEUP_TO_RUN_EVENT_CREATION);
	}

	if (_wakeupEvents[1] == -1)
	{
		_lastErrorDescription = DERR_WAKEUP_TO_EXIT_EVENT_CREATION;
		ERROR(1,"wakeupEvents[1],error: %s",_lastErrorDescription);
		return (_lastErrorNumber = ERR_WAKEUP_TO_EXIT_EVENT_CREATION);
	}
#if 0
	if (!ace_cond)
	{
		_lastErrorDescription = DERR_ACE_COND_CREATION;
		ERROR(1,"wakeupEvents[1],error: %s",_lastErrorDescription);
		return (_lastErrorNumber = ERR_ACE_COND_FAILED);
	}
#endif
	if (_waitFunctionsFailed)
	{
		_lastErrorDescription = DERR_WAIT_FUNCTIONS_FAILED;
		ERROR(1,"waitFunctionsFailed, error %s",_lastErrorDescription);
		return (_lastErrorNumber = ERR_WAIT_FUNCTIONS_FAILED);
	}

	if (!proc)
	{
		_lastErrorDescription = D_OK;
		ERROR(1,"Thread procedure , error %s",_lastErrorDescription);
		return (_lastErrorNumber = 0);
	}

	int errorLine = 0;
	if (_stopRequested)
	{
		_lastErrorDescription = DWAR_STOP_REQUESTED;
		ERROR(1,"stopRequested ,error: %s",_lastErrorDescription);
		return (_lastErrorNumber = WAR_STOP_REQUESTED);
	}

	try
	{ //BEGIN CRITICAL SECTION
		_accessControl->acquire();

		errorLine = 1;
#if 0
		//Queuing the procedure and its parameters
		int max_connections_supported = 500;
		if (_procDeque.size() > max_connections_supported) {
			ERROR(1," too many connections incoming size =%d",_procDeque.size());
			 _accessControl->release();
			return -1;
		}else {

			_procDeque.push_back(procParamPair_t(proc, procParameters));
		}

#endif
		
		 //Queuing the procedure and its parameters
		_procDeque.push_back(procParamPair_t(proc, procParameters));	
		errorLine = 2;
	
		eventfd_t wakeupEvent0 = 1U;
		int returnFD = eventfd_write(_wakeupEvents[0], wakeupEvent0);
		if( returnFD == -1  ){ 
			ERROR(1,"%s"," ThreadPool::fxCreateThread Failed in signalling _wakeupEvents[0] ");
			_procDeque.pop_back();
			 _accessControl->release();
			return (_lastErrorNumber = ERR_WAKEUP_TO_RUN_EVENT_SET);
		}

		//Try to create a new thread if there aren't sleeping threads.
		if ((_sleepingThreadCount <= 0) && fxCreateThread())
		{
			ERROR(1,"%s","Failure in creating thread!! Popping the _procDeque queue");
			_procDeque.pop_back();
			fxDeleteThreadsNotRunning();
			 _accessControl->release();
			return _lastErrorNumber;
		}
		_accessControl->release();
	}
	catch (...)
	{ //END CRITICAL SECTION
		if (errorLine == 2) _procDeque.pop_back();
		fxDeleteThreadsNotRunning();
		_lastErrorDescription = (!errorLine ? DERR_CRITICAL_SECTION_ENTER : DERR_GENERIC_ERROR);
		_accessControl->release();
		return (_lastErrorNumber = (!errorLine ? ThreadPool::ERR_CRITICAL_SECTION_ENTER : ThreadPool::ERR_GENERIC_ERROR));
	}
	fxDeleteThreadsNotRunning();

	_lastErrorDescription = D_OK;
	DEBUG(1,"%s","Returning from function queueUserWorkItem");
	return (_lastErrorNumber = 0);
}//End of queueUserWorkItem

/*===================================================================
   ROUTINE: fxCreateThread
=================================================================== */
ACE_INT32 ThreadPool::fxCreateThread()
{
	DEBUG(1,"%s","Inside fxCreateThread function");
	if (static_cast<unsigned>(_runningThreads.size()) >= _maxThread)
	{
		_lastErrorDescription = D_OK;
		return (_lastErrorNumber = 0);
	}

	ACE_UINT32 recordId = _recordCounter++;
	ThreadInfoRecord * record = new ThreadInfoRecord(recordId, this);

	if (!record)
	{ 	//memory error
		ERROR(1,"%s","Error creating ThreadInfoRecord object!!");
		_lastErrorDescription = DERR_ALLOCATING_THREAD_INFO_RECORD;
		return (_lastErrorNumber = ERR_ALLOCATING_THREAD_INFO_RECORD);
	}
	//Pre-save the thread info record into running thread map
	std::pair<std::map<ACE_UINT32, ThreadInfoRecord *>::iterator, bool> pr = _runningThreads.insert(std::pair<ACE_UINT32, ThreadInfoRecord *>(recordId, record));

	if (!pr.second)
	{ 	//Insert failed
		ERROR(1,"%s","ThreadInfoRecord object insertion into running thread failed!!");
		delete record;
		record = 0;
		_lastErrorDescription = DERR_MAP_INSERTION;
		return (_lastErrorNumber = ERR_MAP_INSERTION);
	}
	HANDLE tHandle = 0;
	ACE_thread_t tId = 0;
	if (_stopRequested)
	{
		//Remove record from _runningThreads map and delete record
		_runningThreads.erase(pr.first);
		delete record;
		record = 0;
		_lastErrorDescription = DWAR_STOP_REQUESTED;
		return (_lastErrorNumber = WAR_STOP_REQUESTED);
	}

	int ret = 0;

	ret = ACE_OS::thr_create((ACE_THR_FUNC)fxWorkerMain, record, THR_DETACHED, &tId, &tHandle);
	if (ret < 0)
	{
		ERROR(1,"%s","Thread spawn failed!!");
		//Remove record from _runningThreads map and delete record
		_runningThreads.erase(pr.first);
		delete record;
		record = 0;
		_lastErrorDescription = DERR_CREATE_POOL_THREAD;
		return (_lastErrorNumber = ERR_CREATE_POOL_THREAD);
	}
	record->threadHandle = tHandle;
	record->threadId = tId;
	record->threadState = ThreadInfoRecord::RUNNING;

	_lastErrorDescription = D_OK;
	DEBUG(1,"%s","Returning from fxCreateThread function");
	return (_lastErrorNumber = 0);

}//End of fxCreateThread

//This function empties the threads that are not executing from _stoppedThreads queue.
/*===================================================================
   ROUTINE: fxDeleteThreadsNotRunning
=================================================================== */
ACE_INT32 ThreadPool::fxDeleteThreadsNotRunning()
{
	DEBUG(1,"%s","Inside fxDeleteThreadsNotRunning function");
	for (;;)
	{
		_accessControl->acquire();

		if (_stoppedThreads.empty())
		{
			 _accessControl->release();
			break;
		}

	  delete _stoppedThreads.front();
		_stoppedThreads.pop_front();
		_accessControl->release();
	}
		DEBUG(1,"%s","Returning from fxDeleteThreadsNotRunning function");
	return 0;
}//End of fxDeleteThreadsNotRunning

/*===================================================================
   ROUTINE: fxWorkerMain
=================================================================== */
unsigned ThreadPool::fxWorkerMain(void * myInfoRecord)
{

	DEBUG(1,"%s","Inside  ThreadPool::fxWorkerMain function");
	ThreadInfoRecord * record = reinterpret_cast<ThreadInfoRecord *>(myInfoRecord);
	ThreadPool * pool = record->threadPool;

	if (pool->_stopRequested)
	{
		pool->_accessControl->acquire();
		record->threadState = ThreadInfoRecord::STOPPING;
		pool->_stoppedThreads.push_back(record);

		pool->_runningThreads.erase(record->id);

		record->threadState = ThreadInfoRecord::STOPPED;

		pool->_accessControl->release();
		ACE_Thread::exit(0);
		DEBUG(1,"%s","Stop signalled , theads exited");
		return 0;
	}

	//Safe start!!
//	usleep(30000);

	struct timespec req;
	req.tv_sec =0;
	req.tv_nsec = 30000 * 1000L;
	nanosleep(&req,(struct timespec *)NULL);


	for (ACE_INT32 tryCount = 2; tryCount; )
	{
		if (pool->_stopRequested)
		{
			break;
		}
		pool->fxDeleteThreadsNotRunning();
		if (pool->_stopRequested)
		{
			break;
		}

		//Get a user procedure, if any.
		{ //BEGIN CRITICAL SECTION
			pool->_accessControl->acquire();
			if (pool->_procDeque.empty())
			{
				record->userProc = 0;
				record->userPar = 0;

			}
			else
			{
				record->userProc = pool->_procDeque.front().first;
				record->userPar = pool->_procDeque.front().second;

				pool->_procDeque.pop_front();
				DEBUG(1,"%s","queue got popped out");
			}
			pool->_accessControl->release();
		} //END CRITICAL SECTION
		if (pool->_stopRequested)
		{
			if (pool->_autoDelete)
			{
				pool->_accessControl->acquire();

				if (record->userProc)
				{
					delete record->userProc;
					record->userProc = 0;
					record->userPar = 0;
				}
				pool->_accessControl->release();
			}
			break;
		}

		//Try to execute any user procedure found.
		if (record->userProc)
		{
			tryCount = 2;
			try
			{
				if( record->userPar)
				{
					record->userProc->operator()(record->userPar);
				}
				else
				{
					record->userProc->operator()((void *)0);
				}
			}
			catch (...)
			{
				throw;

			}
			DEBUG(1,"_autoDelete : ",(pool->_autoDelete));
			if (pool->_autoDelete)
			{
				pool->_accessControl->acquire();

				if (record->userProc)
				{
					delete record->userProc;
					record->userProc = 0;
					record->userPar = 0;
				}
				pool->_accessControl->release();
			}
		}
		else
		{ //No procedure found. I'm going to sleep.
			if (pool->_stopRequested)
			{
				break;
			}

			{ //BEGIN CRITICAL SECTION

				if ((tryCount <= 1) && (pool->_runningThreads.size() > pool->_minThread))
				{
					break;
				}
				tryCount = 2;
				pool->_accessControl->acquire();
				pool->_sleepingThreadCount++;
				record->threadState = ThreadInfoRecord::SLEEPING;
				pool->_accessControl->release();
			} //END CRITICAL SECTION
		
			{
				ACE_Time_Value tm(pool->_keepAlive);
				const nfds_t nfds = 2;
				struct pollfd fds[nfds];
				ACE_INT32 pollret;
				fds[0].fd = pool->_wakeupEvents[0];
				fds[0].events = POLLIN;
				fds[1].fd = pool->_wakeupEvents[1];
				fds[1].events = POLLIN;

				pollret = ACE_OS::poll(fds, nfds,&tm );
				if( -1 == pollret )
				{
                                	DEBUG(1,"%s","POLL operation failed");
                                	pool->_waitFunctionsFailed = true; //Signal to pool that a wait function failed in a thread.
                                	tryCount = 0;

				}
				else if ((fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) || (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL))) 		
				{

					DEBUG(1,"%s"," Signal to pool that a wait function failed in a thread");
                                	pool->_waitFunctionsFailed = true; //Signal to pool that a wait function failed in a thread.
                                	tryCount = 0;
				}	
			
				else if ( pollret == 0 )
				{
					DEBUG(1,"%s","Time out happened in Wait in select function  ");	
                                	tryCount--;
					DEBUG(1,"tryCount after ,keep alive timeout reached is, tryCount = %d", tryCount);

				}
				else if ( pollret > 0 )
				{

					if(fds[0].revents & POLLIN)	
					{
						tryCount--;
						eventfd_t wakeupEvent0 = 1U;
						DEBUG(1,"%s","A new procedure added to the pool");
                                                DEBUG(1,"%s","First object is signaled!!! ");
						int readFDret = eventfd_read(pool->_wakeupEvents[0], &wakeupEvent0);
						if ( readFDret == 0)
						{
							 DEBUG(1,"%s","eventfd read successful for the first object");
						}
					}else if( fds[1].revents & POLLIN)
                                	{
                                        	DEBUG(1,"%s","Second object is signaled!!! ");
						tryCount = 0;
					
						eventfd_t wakeupEvent1 = 1U;			
						DEBUG(1,"%s","Exit event signalled: the pool is in deleting phase and tryCount is 0 ");	
						int readFDret = eventfd_read(pool->_wakeupEvents[1], &wakeupEvent1);	
						if ( readFDret == 0)
                                                {
                                                         DEBUG(1,"%s","eventfd read successful for exit event signalled");
                                                }
                                	}
				}

			} 

			{ //BEGIN CRITICAL SECTION

				pool->_accessControl->acquire();
				pool->_sleepingThreadCount--;
				record->threadState = ThreadInfoRecord::RUNNING;
				pool->_accessControl->release();

			} //END CRITICAL SECTION

		}
		
	}
	{ //BEGIN CRITICAL SECTION
		pool->_accessControl->acquire();
		record->threadState = ThreadInfoRecord::STOPPING;
		pool->_runningThreads.erase(record->id);
		pool->_stoppedThreads.push_back(record);
		record->threadState = ThreadInfoRecord::STOPPED;
		pool->_accessControl->release();
	} //END CRITICAL SECTION
	INFO(1,"%s","EXITING THREAD..");
	ACE_OS::thr_exit();
	INFO(1,"%s"," fxWorkerMain thread closed successfully..");
	DEBUG(1,"%s","Returning from fxWorkerMain function");
	return 0;
}//End of fxWorkerMain

