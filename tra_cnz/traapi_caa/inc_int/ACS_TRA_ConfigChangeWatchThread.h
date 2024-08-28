#ifndef HEADER_GUARD_CLASS__ACS_TRA_ConfigChangeWatchThread
#define HEADER_GUARD_CLASS__ACS_TRA_ConfigChangeWatchThread ACS_TRA_ConfigChangeWatchThread

/** @file ACS_TRA_ConfigChangeWatchThread.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2012-10-24
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
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-10-24 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <time.h>
#include <pthread.h>

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_TRA_ConfigChangeWatchThread


class __CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
public:
	enum ErrorConstants {
		ERR_NO_ERROR = 0,
		ERR_INVALID_PARAMETER,
		ERR_ALREADY_STARTED,
		ERR_NOT_STARTED,
		ERR_MUTEX_LOCK,
		ERR_MUTEX_UNLOCK,
#ifdef ACS_TRA_WATCH_THREAD_USES_INOTIFY
		ERR_INOTIFY_INIT,
		ERR_INOTIFY_ADD_WATCH,
#else
		ERR_WATCHED_FILE_OPEN,
#endif
		ERR_PTHREAD_CREATE,
		ERR_PIPE_CREATE,
		ERR_EPOLL_CREATE,
		ERR_EPOLL_CTL_ADD
	};


	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ ();

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	~__CLASS_NAME__ ();


	//===========//
	// Functions //
	//===========//
public:
	int start (const char * watched_file);
	int stop ();

private:
	int prepare_epoll_interface ();
	static void * thread_watch_main (void * args);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//===========//
	// Fields //
	//===========//
private:

	char * _watched_file;
#ifdef  ACS_TRA_WATCH_THREAD_USES_INOTIFY
	int _inotify_fd;
	int _inotify_wd;
#else
	time_t _watched_file_last_mt;
	static const unsigned int _watch_fd_interval_between_checks = 3000;  // milliseconds
#endif

	pthread_mutex_t _sync_mutex;
	pthread_t _watch_thread_id;
	int _stop_signal_pipe [2];
	int _epfd;

	int _stop_requested;
	int _properties_read_requested;
};
#endif // HEADER_GUARD_CLASS__ACS_TRA_ConfigChangeWatchThread
