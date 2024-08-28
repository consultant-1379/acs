//========================================================================================
/** @file  acs_aeh_signalhandler.cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-6
 *	@version 0.9.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P2.6.1 | 2016-03-23 | xnicmut      | Code refactoring to improve AEH     |
 *	|        |            |              | signal handling and avoid calling   |
 *	|        |            |              | no-async-sygnal-safe functions.     |
 *	|        |            |              | Also a code review is made.         |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-09-08 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2011-02-09 | xfabron      | Released for ITSTEAM2 sprint 15     |
 *	+--------+------------+--------------+-------------------------------------+
 *	| 		 | 			  | 		     | 								       |
 *	+========+============+==============+=====================================+
 *
 * SEE ALSO
 *		-
 *
 */
//========================================================================================

//#define _BSD_SOURCE
//#define _GNU_SOURCE

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include <new>

#include "acs_aeh_types.h"
#include "acs_aeh_evreport.h"

#include "acs_aeh_signalhandler.h"

#if !defined(ERI_ARRAY_SIZE)
#	define ERI_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif

typedef sighandler_t acs_aeh_sa_handler_t;

typedef void (* acs_aeh_sa_sigaction_t) (int sig_num, siginfo_t *, void *);
typedef acs_aeh_sa_sigaction_t * acs_aeh_sa_sigaction_ptr_t;

typedef void * acs_aeh_pthread_routine_t (void *);
typedef acs_aeh_pthread_routine_t * acs_aeh_pthread_routine_ptr_t;

namespace {
	class lock_guard {
		/*=======*/
		/* CTORs */
		/*=======*/
	public:
		explicit inline lock_guard (pthread_mutex_t & __m) : _mutex_device(__m) {
			static_cast<void>(pthread_mutex_lock(&_mutex_device));
		}

	private:
		lock_guard (const lock_guard &);

		/*======*/
		/* DTOR */
		/*======*/
	public:
		inline ~lock_guard () {
			static_cast<void>(pthread_mutex_unlock(&_mutex_device));
		}

		/*===========*/
		/* Operators */
		/*===========*/
	private:
		lock_guard & operator= (const lock_guard &);

	private:
		pthread_mutex_t & _mutex_device;
	};

	class secure_signal_handling_context {
		/*=======*/
		/* CTORs */
		/*=======*/
	private:
		secure_signal_handling_context ();
		secure_signal_handling_context (const secure_signal_handling_context &);

		/*======*/
		/* DTOR */
		/*======*/
	public:
		inline ~secure_signal_handling_context () {}

		/*===========*/
		/* Functions */
		/*===========*/
	public:
		static inline const char * process_name () { return _process_name; }

		static int initialize (const char * process_name_);

	private:
		static inline const char * signal_name (int sig_num_) {
			const char * p = 0;

			switch (sig_num_) {
			case SIGINT: p = "SIGINT"; break;
			case SIGILL: p = "SIGILL"; break;
			case SIGABRT: p = "SIGABRT"; break;
			case SIGFPE: p = "SIGFPE"; break;
			case SIGSEGV: p = "SIGSEGV"; break;
			case SIGTERM: p = "SIGTERM"; break;
			default: p = "SIG_UNKNOWN_SIGNAL"; break;
			}

			return p;
		}

		static inline void process_name (const char * new_val) {
			snprintf(_process_name, ERI_ARRAY_SIZE(_process_name), "%s:%lld",
					((new_val && *new_val) ? new_val : "ACS_AEH_SignalHandler"),
					static_cast<long long int>(getpid()));
			_process_name[ERI_ARRAY_SIZE(_process_name) - 1] = 0;
		}

		static inline pid_t gettid () { return static_cast<pid_t>(syscall(SYS_gettid)); }

		template <size_t _T_sig_count>
		static int initialize_handling_sync (const int (& signals) [_T_sig_count]);

		template <size_t _T_sig_count>
		static int initialize_handling_async (const int (& signals) [_T_sig_count]);

		static int init_handler_thread_attr (pthread_attr_t * attr);

		static int init_event_reporter ();

		static int init_handler_thread ();

		template <size_t _T_sig_count>
		static int init_signals_handler (const int (& signals) [_T_sig_count]);

		static int init_pipes ();

		static void signal_handler (int sig_num, siginfo_t * sig_info, void * context);

		static void * handler_thread_routine (void * arg);

		static int handle_signal (int sig_num);

		/*===========*/
		/* Operators */
		/*===========*/
	private:
		secure_signal_handling_context & operator= (const secure_signal_handling_context &);

		/*========*/
		/* Fields */
		/*========*/
	private:
		// A preallocated stack for the signals handling thread
		static const size_t _HANDLER_THREAD_STACK_SIZE = 128 * 1024;
		static char _handler_thread_stack [_HANDLER_THREAD_STACK_SIZE] __attribute__ ((aligned (8)));

		// The process name as provided by the caller
		static char _process_name [PROCESS_NAME_MAX_LEN];

		// A mutex to synchronize interface caller access to this secure context
		static pthread_mutex_t _caller_mutex;

		static pthread_t _handler_thread_id;

		static bool _handler_thread_created;

		static bool _signals_handler_ok;

		static int _handler_to_thread_pipe [2];
		static int _thread_to_handler_pipe [2];

		static int _signals_handled [6];

		static char _event_reporter_class_block [sizeof(acs_aeh_evreport)];
		static acs_aeh_evreport * _event_reporter;
	};

	// secure_signal_handling_context class static fields initializations
	char
	secure_signal_handling_context::_handler_thread_stack [_HANDLER_THREAD_STACK_SIZE]
		__attribute__ ((aligned (8))) = {0};

	char
	secure_signal_handling_context::_process_name [PROCESS_NAME_MAX_LEN] = "ACS_AEH_SignalHandler";

	pthread_mutex_t
	secure_signal_handling_context::_caller_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

	pthread_t
	secure_signal_handling_context::_handler_thread_id;

	bool
	secure_signal_handling_context::_handler_thread_created = false;

	bool
	secure_signal_handling_context::_signals_handler_ok = false;

	int
	secure_signal_handling_context::_handler_to_thread_pipe [2] = {-1, -1};

	int
	secure_signal_handling_context::_thread_to_handler_pipe [2] = {-1, -1};

	int
	secure_signal_handling_context::_signals_handled [6] = { //SIGINT, // WARNING: Only for test: To be DELETED */
			SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGTERM};

	char
	secure_signal_handling_context::_event_reporter_class_block [sizeof(acs_aeh_evreport)] = {0};

	acs_aeh_evreport *
	secure_signal_handling_context::_event_reporter = 0;

	int secure_signal_handling_context::initialize (const char * process_name_) {
		lock_guard guard(_caller_mutex);

		process_name(process_name_);

		// Initialize synchronous signal handling if I'm running
		// into the process main thread; else asynchronuos
		// signal handling by handlers is adopted.
//			if (gettid() == getpid()) initialize_handling_sync();
//			else initialize_handling_async();

		return initialize_handling_async(_signals_handled);
	}

	template <size_t _T_sig_count>
	int secure_signal_handling_context::initialize_handling_sync (const int (& /*signals*/) [_T_sig_count]) {
		return 0;
	}

	template <size_t _T_sig_count>
	int secure_signal_handling_context::initialize_handling_async (const int (& signals) [_T_sig_count]) {
		// Initialize signals handler from/to thread communication channels
		if (init_pipes()) {
			// ERROR: Initializing the two communication pipes.
			return -1;
		}

		// initialize the event report object
		init_event_reporter();

		// Init and start the signal handler thread
		init_handler_thread();

		// Initialize the signals dispositions
		init_signals_handler(signals);

		return 0;
	}

	int secure_signal_handling_context::init_handler_thread_attr (pthread_attr_t * attr) {
		// Inquire the system memory page size to ensure the thread
		// stack is on the right page memory boundaries
		size_t page_size = sysconf(_SC_PAGESIZE);

		// Bound the stack address on system memory page.
		// First of all calculate the possibile _handler_thread_stack buffer page overflow
		size_t page_overflow = reinterpret_cast<size_t>(_handler_thread_stack) % page_size;

		// Positive hint: suppose we are on the page boundaries
		char * stack_on_page_ptr = _handler_thread_stack;
		size_t stack_on_page_size = _HANDLER_THREAD_STACK_SIZE;

		// If we are not on page boundaries then correct adress and size
		if (page_overflow) {
			stack_on_page_ptr += (page_size - page_overflow);
			stack_on_page_size -= page_size;
		}

		int call_result = 0;

		// Try to enable the pre-allocated stack for the signals handler thread.
		// In case of errors, a default system thread will be used
		if ((call_result = pthread_attr_init(attr))) return call_result;

		if ((call_result = pthread_attr_setstack(attr, stack_on_page_ptr, stack_on_page_size))
				&& (pthread_attr_destroy(attr) || pthread_attr_init(attr)))
			return call_result;

		return 0;
	}

	int secure_signal_handling_context::init_event_reporter () {
		if (_event_reporter) return 0; // Already initialized

		_event_reporter = new (_event_reporter_class_block) acs_aeh_evreport();

		return 0;
	}

	int secure_signal_handling_context::init_handler_thread () {
		// If the handler thread was already initialized, then return doing nothing.
		if (_handler_thread_created) return 0;

		pthread_attr_t thread_attr;
		int init_attr_result = init_handler_thread_attr(&thread_attr);

		// Temporary block all signals so the handler thread will inerith that dispositions to avoid dead locks
		sigset_t all_blocked_signal_mask;
		sigset_t sig_mask_old;

		sigfillset(&all_blocked_signal_mask);
		sigemptyset(&sig_mask_old);

		void * thread_arg = 0;

		if (pthread_sigmask(SIG_BLOCK, &all_blocked_signal_mask, &sig_mask_old)) {
			// ERROR: Error on blocking all signals
			// Notify the created thread by its argument
			thread_arg = reinterpret_cast<void *>(1);
		}

		int thread_create_result =
				pthread_create(&_handler_thread_id, (init_attr_result ? 0 : &thread_attr),
						handler_thread_routine, thread_arg);

		// Reset old signal mask
		pthread_sigmask(SIG_SETMASK, &sig_mask_old, 0);

		// thread_attr are no more useful and must be destroyed?
		init_attr_result || pthread_attr_destroy(&thread_attr);

		return (((_handler_thread_created = (thread_create_result ? false : true))) ? 0 : -1);
	}

	template <size_t _T_sig_count>
	int secure_signal_handling_context::init_signals_handler (const int (& signals) [_T_sig_count]) {
		// If the handler thread was already initialized, then return doing nothing.
		if (_signals_handler_ok) return 0;

		struct sigaction act;
		struct sigaction old_act;

		sigemptyset(&act.sa_mask);
		act.sa_sigaction = signal_handler;

		for (size_t i = 0; i < _T_sig_count; ++i) {
			// WARNING: Only for test purposes, SIGINT signal
			// will not be reset to the default action
			act.sa_flags = SA_SIGINFO | ((signals[i] == SIGINT) ? 0 : SA_RESETHAND);
			sigaction(signals[i], &act, &old_act);
		}

		_signals_handler_ok = true;

		return 0;
	}

	int secure_signal_handling_context::init_pipes () {
		if (_handler_to_thread_pipe[0] != -1) {
			// Already initialized
			return 0;
		}

		if (pipe(_handler_to_thread_pipe)) {
			// ERROR: Creating the pipe
			_handler_to_thread_pipe[0] = _handler_to_thread_pipe[1] = -1;
			return -1;
		}

		if (pipe(_thread_to_handler_pipe)) {
			// ERROR: Creating the pipe
			close(_handler_to_thread_pipe[1]);
			close(_handler_to_thread_pipe[0]);
			_handler_to_thread_pipe[0] = _handler_to_thread_pipe[1] = -1;
			_thread_to_handler_pipe[0] = _thread_to_handler_pipe[1] = -1;
			return -1;
		}

		return 0;
	}

	void secure_signal_handling_context::signal_handler (
			int sig_num,
			siginfo_t * /*sig_info*/,
			void * /*context*/) {
		// Saving errno per-thread global variable
		int errno_save = errno;
		char buf_tmp = 0;

		// BEGIN: Signal handler code

		// Send the signal to the thread
		ssize_t bytes_wrote = 0;
		while ((bytes_wrote = write(_handler_to_thread_pipe[1], &sig_num, 1)) <= 0) ;

		// Wait the thread signal handling termination
		while (read(_thread_to_handler_pipe[0], &buf_tmp, 1) <= 0) ;

		// END: Signal handler code

		if (sig_num == SIGINT) { // WARNING: Only for test purposes
			errno = errno_save;
			return;
		}

		struct sigaction act;

		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		act.sa_handler = SIG_DFL;

		if (sig_num == SIGTERM) {
			// If the signal is SIGTERM, re-raise the same signal
			sigaction(SIGTERM, &act, 0);

			raise(SIGTERM);
		} else {
			// Before aborting the process, the abort signal handler will be reset to default
			sigaction(SIGABRT, &act, 0);

			// Aborting the process
			abort();
		}

		// Restoring the errno variable
		errno = errno_save;
	}

	void * secure_signal_handling_context::handler_thread_routine (void * arg) {
		// NOTICED: this thread should have all signals blocked to avoid dead locks
		// Check for blocked signals
		if (arg) {
			// WARNING: My parent thread was not able to block signals
			// Try block all signals by my self
			sigset_t all_blocked_signal_mask;
			sigfillset(&all_blocked_signal_mask);
			pthread_sigmask(SIG_BLOCK, &all_blocked_signal_mask, 0);
		}

		// This thread must be detached.
		pthread_detach(pthread_self());

		unsigned char sig_num = 0;
		ssize_t bytes_wrote = 0;

		// Wait for a signal to be handled
		for (;;) {
			if (read(_handler_to_thread_pipe[0], &sig_num, 1) <= 0) continue;

			// A signal sig_num was received from the handler and sent to me

			handle_signal(sig_num);

			// Notify the signal handler to exit
			while ((bytes_wrote = write(_thread_to_handler_pipe[1], &sig_num, 1)) <= 0) ;
		}

		return 0;
	}

	int secure_signal_handling_context::handle_signal (int sig_num) {
		char problem_data [PROBLEM_TEXT_MAX_LEN] = "";

		snprintf(problem_data, ERI_ARRAY_SIZE(problem_data),
				"%s (#%d) received", signal_name(sig_num), sig_num);
		problem_data[ERI_ARRAY_SIZE(problem_data) - 1] = 0;

		_event_reporter && _event_reporter->sendEventMessage(process_name(), 8497, "EVENT",
				"AP_INTERNAL_FAULT", "APZ", "", problem_data, "Unexpected signal received");

//		// WARNING: For test purposes
//		printf("[SIGNAL HANDLER THREAD]\n"
//				"\tProcess Name == '%s'\n"
//				"\tproblem_data == '%s'\n",
//				process_name(), problem_data);

		return 0;
	}
}

void
acs_aeh_setSignalExceptionHandler (const char * process_name_) {
	secure_signal_handling_context::initialize(process_name_);
}
