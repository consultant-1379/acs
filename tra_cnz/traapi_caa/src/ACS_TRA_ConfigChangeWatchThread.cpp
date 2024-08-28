#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <limits.h>
#include <execinfo.h>

#include "log4cplus/configurator.h"

//#define ACS_TRA_WATCH_THREAD_PRINT_ERROR_ENABLED

#include "ACS_TRA_ConfigChangeWatchThread.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_TRA_ConfigChangeWatchThread

namespace {

#ifdef ACS_TRA_WATCH_THREAD_PRINT_ERROR_ENABLED
#define ACS_TRA_PRINT_ERROR_IMPL(...) print_err(__VA_ARGS__)
#define ACS_TRA_PRINT_ERROR(...) ACS_TRA_PRINT_ERROR_IMPL(__VA_ARGS__)
#define ACS_TRA_PRINT_ERROR_BACKTRACE(...) print_err_backtrace(__VA_ARGS__)

	FILE * err_output_file = 0;

	int print_err (int err_code, const char * format, ...) __attribute__ ((format (printf, 2, 3)));
	int print_err (const char * format, ...) __attribute__ ((format (printf, 1, 2)));

	int print_err_i (int err_code, const char * format, va_list ap) {
		::fprintf(err_output_file ?: stderr, "[PID == %d]: ", ::getpid());
		err_code && ::fprintf(err_output_file ?: stderr, "[ERROR]: [err_code == %d]: ", err_code);
		::vfprintf(err_output_file ?: stderr, format, ap);
		::fflush(err_output_file ?: stderr);
		return 0;
	}

	int print_err (int err_code, const char * format, ...) {
		va_list argp;
		va_start(argp, format);
		print_err_i(err_code, format, argp);
		va_end(argp);

		return 0;
	}

	int print_err (const char * format, ...) {
		va_list argp;
		va_start(argp, format);
		print_err_i(0, format, argp);
		va_end(argp);

		return 0;
	}

	int print_err_backtrace (const __CLASS_NAME__ * this_) {
		void * func_addresses [128];
		int func_count = ::backtrace(func_addresses, 128);
		char ** frames_str = ::backtrace_symbols(func_addresses, func_count);

		ACS_TRA_PRINT_ERROR("BACK_TRACE[this == '%p']: There are %d frame on the stack\n", reinterpret_cast<const void *>(this_), func_count);
		for (int i = 0; i < func_count; ++i) {
			ACS_TRA_PRINT_ERROR("  %03d) %s\n", i, frames_str[i]);
		}

		::free(frames_str);

		return 0;
	}

#else
#define ACS_TRA_PRINT_ERROR(...)
#define ACS_TRA_PRINT_ERROR_BACKTRACE(...)
#endif
}

#ifndef ACS_TRA_WATCH_THREAD_USES_INOTIFY
	const unsigned int  __CLASS_NAME__::_watch_fd_interval_between_checks;
#endif

__CLASS_NAME__::__CLASS_NAME__ ()
#ifdef ACS_TRA_WATCH_THREAD_USES_INOTIFY
	: _watched_file(0), _inotify_fd(-1), _inotify_wd(-1), _sync_mutex(), _watch_thread_id(0),
	  _stop_signal_pipe(), _epfd(-1), _stop_requested(0), _properties_read_requested(0)
#else
	: _watched_file(0), _watched_file_last_mt(0), _sync_mutex(), _watch_thread_id(0),
	  _stop_signal_pipe(), _epfd(-1), _stop_requested(0), _properties_read_requested(0)
#endif
{
	_stop_signal_pipe[0] = _stop_signal_pipe[1] = -1;

#ifdef ACS_TRA_WATCH_THREAD_PRINT_ERROR_ENABLED
	if (!err_output_file) {
		char path [4096];
		::snprintf(path, sizeof(path), "/var/log/acs/tra/TRA_WatchThread_%s_%08d.log", program_invocation_short_name, ::getpid());
		err_output_file = ::fopen(path, "at");
	}
#endif

	ACS_TRA_PRINT_ERROR("BEGIN[this == '%p']: CTOR\n", this);

	if (const int errno_save = ::pthread_mutex_init(&_sync_mutex, 0)) { // ERROR:
		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_mutex_init()' failed\n");
	}
}

__CLASS_NAME__::~__CLASS_NAME__ () {
	ACS_TRA_PRINT_ERROR("BEGIN[this == '%p']: DTOR\n", this);

	stop();

	::free(_watched_file);

	if (const int errno_save = ::pthread_mutex_destroy(&_sync_mutex)) {
		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_mutex_destroy()' failed\n");
	}
}

int __CLASS_NAME__::start (const char * watched_file) {
	ACS_TRA_PRINT_ERROR("BEGIN[this == '%p']: start\n", this);

	// Check the parameter
	if (!(watched_file && *watched_file)) return ERR_INVALID_PARAMETER;

	// Put here the synchronization
	if (const int errno_save = ::pthread_mutex_lock(&_sync_mutex)) {
		errno = errno_save;
		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_mutex_lock()' failed: function '%s'\n", __PRETTY_FUNCTION__);
		return ERR_MUTEX_LOCK;
	}

	ACS_TRA_PRINT_ERROR_BACKTRACE(this);

	// Return immediately if already started
#ifdef ACS_TRA_WATCH_THREAD_USES_INOTIFY
	if (_inotify_fd != -1)
#else
	if (_watched_file != 0)
#endif
	{ // YES: already started
		ACS_TRA_PRINT_ERROR("Already started: function '%s'\n", __PRETTY_FUNCTION__);

		if (const int errno_save = ::pthread_mutex_unlock(&_sync_mutex)) {
			errno = errno_save;
			return ERR_MUTEX_UNLOCK;
		}
		return ERR_ALREADY_STARTED;
	}

#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
	// Create a inotify instance descriptor
	errno = 0;
	_inotify_fd = ::inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
	if (_inotify_fd < 0) { // ERROR: creating a inotify interface descriptor
		const int errno_save = errno;

		ACS_TRA_PRINT_ERROR(errno_save, "Call '::inotify_init1()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		_inotify_fd = _inotify_wd = _stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
		_watch_thread_id = 0;
		::pthread_mutex_unlock(&_sync_mutex);
		errno = errno_save;
		return ERR_INOTIFY_INIT;
	}

	errno = 0;
	_inotify_wd = ::inotify_add_watch(_inotify_fd, watched_file, IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVE_SELF | IN_MOVE);
	if (_inotify_wd < 0) { // ERROR: adding watch item for the log4cplus property file
		const int errno_save = errno;

		ACS_TRA_PRINT_ERROR(errno_save, "Call '::inotify_add_watch()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		::close(_inotify_fd);
		_inotify_fd = _inotify_wd = _stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
		_watch_thread_id = 0;
		::pthread_mutex_unlock(&_sync_mutex);
		errno = errno_save;
		return ERR_INOTIFY_ADD_WATCH;
	}
#else
	// get and cache last modification time for watched file
	struct stat watched_file_stats;
	if(::stat(watched_file, & watched_file_stats) == 0)
		_watched_file_last_mt = watched_file_stats.st_mtime;
	else { ACS_TRA_PRINT_ERROR(errno, "Call '::stat()' failed: function '%s'\n", __PRETTY_FUNCTION__); }
#endif

	// Prepare the internal stop signal pipe
	errno = 0;
	if (::pipe2(_stop_signal_pipe, O_NONBLOCK | O_CLOEXEC) < 0) { // ERROR: Creating the stop signal self pipe
		const int errno_save = errno;

		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pipe2()' failed: function '%s'\n", __PRETTY_FUNCTION__);

#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
		::close(_inotify_fd);
		_inotify_fd = _inotify_wd = _stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
#else
		_stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
		_watched_file_last_mt = 0;
#endif
		_watch_thread_id = 0;
		::pthread_mutex_unlock(&_sync_mutex);
		errno = errno_save;
		return ERR_PIPE_CREATE;
	}

	errno = 0;
	int call_result = prepare_epoll_interface();
	if (call_result != ERR_NO_ERROR) { // ERROR: preparing the epoll I/O multiplexing interface
		const int errno_save = errno;

		ACS_TRA_PRINT_ERROR(errno_save, "Call 'prepare_epoll_interface()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		::close(_stop_signal_pipe[1]); ::close(_stop_signal_pipe[0]);
#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
		::close(_inotify_fd);
		_inotify_fd = _inotify_wd = _stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
#else
		_stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
		_watched_file_last_mt = 0;
#endif
		_watch_thread_id = 0;
		::pthread_mutex_unlock(&_sync_mutex);
		errno = errno_save;
		return call_result;
	}

	_watched_file = ::strdup(watched_file);

	// Create the thread that will watch on the change events
	errno = 0;
	if(::pthread_create(&_watch_thread_id, 0, thread_watch_main, this)) { // ERROR: creating the thread
		const int errno_save = errno;

		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_create()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		::close(_epfd);
		::close(_stop_signal_pipe[1]); ::close(_stop_signal_pipe[0]);
#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
		::close(_inotify_fd);
		_inotify_fd = _inotify_wd = _stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
#else
		_stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
		_watched_file_last_mt = 0;
#endif
		::free(_watched_file);
		_watched_file = 0;
		_watch_thread_id = 0;
		::pthread_mutex_unlock(&_sync_mutex);
		errno = errno_save;
		return ERR_PTHREAD_CREATE;
	}

	// All right
	if (const int errno_save = ::pthread_mutex_unlock(&_sync_mutex)) {
		errno = errno_save;

		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_mutex_unlock()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		return ERR_MUTEX_UNLOCK;
	}

	return ERR_NO_ERROR;
}

int __CLASS_NAME__::stop () {
	ACS_TRA_PRINT_ERROR("BEGIN[this == '%p']: stop\n", this);

	// Put here the synchronization
	if (const int errno_save = ::pthread_mutex_lock(&_sync_mutex)) {
		errno = errno_save;

		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_mutex_lock()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		return ERR_MUTEX_LOCK;
	}

	ACS_TRA_PRINT_ERROR_BACKTRACE(this);


	// Return immediately if not already started
#ifdef ACS_TRA_WATCH_THREAD_USES_INOTIFY
	if (_inotify_fd == -1)
#else
	if (_watched_file == 0)
#endif
	{ // YES: not yet started
		ACS_TRA_PRINT_ERROR("Not yet started: function '%s'\n", __PRETTY_FUNCTION__);

		if (const int errno_save = ::pthread_mutex_unlock(&_sync_mutex)) {
			errno = errno_save;
			return ERR_MUTEX_UNLOCK;
		}
		return ERR_NOT_STARTED;
	}

	// Signal the stop request to the watch thread, writing a character into the write end of the pipe _stop_signal_pipe
	ssize_t bytes_wrote = 0;
	for (errno = 0; ((bytes_wrote = ::write(_stop_signal_pipe[1], "S", 1)) < 1) && (errno == EINTR); errno = 0)
		;

	// Now I wait for the watch thread to terminate
	if (const int errno_save = ::pthread_join(_watch_thread_id, 0)) {
		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_join()' failed: function '%s'\n", __PRETTY_FUNCTION__);
	}

	// Now I can close and release all resources and descriptors
	::close(_epfd);
	::close(_stop_signal_pipe[1]); ::close(_stop_signal_pipe[0]);

#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
	::close(_inotify_fd);
#endif

	// release memory allocated by 'start()' method for '_watched_file' attribute
	::free(_watched_file);
	_watched_file = 0;

	// TODO: LOG if any close fails

#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
	_inotify_fd = _inotify_wd = _stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
#else
	_stop_signal_pipe[0] = _stop_signal_pipe[1] = _epfd = -1;
	_watched_file_last_mt = 0;
#endif
	_watch_thread_id = 0;

	// All right
	if (const int errno_save = ::pthread_mutex_unlock(&_sync_mutex)) {
		errno = errno_save;

		ACS_TRA_PRINT_ERROR(errno_save, "Call '::pthread_mutex_unlock()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		return ERR_MUTEX_UNLOCK;
	}

	return ERR_NO_ERROR;
}

int __CLASS_NAME__::prepare_epoll_interface () {
	// Create the epoll interface
	errno = 0;
	if ((_epfd = ::epoll_create1(EPOLL_CLOEXEC)) < 0) { // ERROR: Creating an epoll interface descriptor
		ACS_TRA_PRINT_ERROR(errno, "Call '::epoll_create1()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		_epfd = -1;
		return ERR_EPOLL_CREATE;
	}

	epoll_event event_to_check;
	event_to_check.events = EPOLLIN;

#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
	// Add the file descriptors to this epoll interface
//	event_to_check.data.fd = _inotify_fd;
	event_to_check.data.ptr = &_properties_read_requested;
	errno = 0;
	if (::epoll_ctl(_epfd, EPOLL_CTL_ADD, _inotify_fd, &event_to_check) < 0) { // ERROR: Adding inotify descriptor
		ACS_TRA_PRINT_ERROR(errno, "Call '::epoll_ctl()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		::close(_epfd); _epfd = -1;
		return ERR_EPOLL_CTL_ADD;
	}
#endif

//	event_to_check.data.fd = _stop_signal_pipe[0];
	event_to_check.data.ptr = &_stop_requested;
	errno = 0;
	if (::epoll_ctl(_epfd, EPOLL_CTL_ADD, _stop_signal_pipe[0], &event_to_check) < 0) { // ERROR: Adding read end of the pipe
		ACS_TRA_PRINT_ERROR(errno, "Call '::epoll_ctl()' failed: function '%s'\n", __PRETTY_FUNCTION__);

		::close(_epfd); _epfd = -1;
		return ERR_EPOLL_CTL_ADD;
	}

	return ERR_NO_ERROR;
}

void * __CLASS_NAME__::thread_watch_main (void * args) {
	const int EPOLL_EVENT_COUNT_MAX = 10;
	epoll_event io_events[EPOLL_EVENT_COUNT_MAX];
#ifndef ACS_TRA_WATCH_THREAD_USES_INOTIFY
	struct stat watched_file_stats;
#endif
	__CLASS_NAME__ * this_ = reinterpret_cast<__CLASS_NAME__ *>(args);

	this_->_stop_requested = 0;
	this_->_properties_read_requested = 0;

#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
	const int INOTIFY_BUFFER_SIZE = 16 * (sizeof(inotify_event) + NAME_MAX + 1);
	unsigned char inotify_buffer [INOTIFY_BUFFER_SIZE];
#endif
	ssize_t bytes_read = 0;

	ACS_TRA_PRINT_ERROR("WATCH THREAD[this == '%p']: start looping\n", this_);

	do {

		ACS_TRA_PRINT_ERROR("WATCH THREAD: Waiting events...\n");

		errno = 0;

#ifdef 	ACS_TRA_WATCH_THREAD_USES_INOTIFY
		const int event_count = ::epoll_wait(this_->_epfd, io_events, EPOLL_EVENT_COUNT_MAX, -1);
#else
		const int event_count = ::epoll_wait(this_->_epfd, io_events, EPOLL_EVENT_COUNT_MAX, __CLASS_NAME__::_watch_fd_interval_between_checks);
#endif

		if (event_count < 0) { // ERROR: into epoll interface while waiting for I/O events
			if (errno != EINTR) ::sleep(1);

			ACS_TRA_PRINT_ERROR(errno, "Call '::epoll_wait()' failed: function '%s'\n", __PRETTY_FUNCTION__);

			continue;
		}

		// Check what's happened
		for (int i = 0; i < event_count; (*reinterpret_cast<int *>(io_events[i++].data.ptr)) += 1) ;

#ifndef ACS_TRA_WATCH_THREAD_USES_INOTIFY
		// Check if watched file has changed
		if(0 == this_->_watched_file) {
			ACS_TRA_PRINT_ERROR("Invalid watchfile ! < this_->_watched_file  == NULL > : function '%s'\n", __PRETTY_FUNCTION__);
		}
		else
		{
			if(::stat(this_->_watched_file, & watched_file_stats) < 0) {
				ACS_TRA_PRINT_ERROR(errno, "Call '::stat()' failed: function '%s'\n", __PRETTY_FUNCTION__);
				this_->_properties_read_requested = 1; // re-read watched file
			}
			else if(watched_file_stats.st_mtime != this_->_watched_file_last_mt) {
				ACS_TRA_PRINT_ERROR("Watched file changed ! this_->_watched_file_last_mt == < %ld >,  watched_file_stats.st_mtime == <%ld>\n", this_->_watched_file_last_mt,  watched_file_stats.st_mtime);
				this_->_properties_read_requested = 1;	// re-read watched file
			}
			else
			{
#ifdef ACS_TRA_WATCH_THREAD_PRINT_ERROR_ENABLED
				struct tm local_tm;
				char asctime_buf[512] = {0};
				localtime_r(& this_->_watched_file_last_mt, & local_tm);
				asctime_r( & local_tm, asctime_buf);
				ACS_TRA_PRINT_ERROR("Watched file NOT changed ! LAST MODIFICATION TIME == %s", asctime_buf);
#endif // ACS_TRA_WATCH_THREAD_PRINT_ERROR_ENABLED
			}
		}
#endif

		ACS_TRA_PRINT_ERROR("READ REQUESTED == %d, STOP REQUESTED == %d: function '%s'\n",
				this_->_properties_read_requested, this_->_stop_requested, __PRETTY_FUNCTION__);

		if (this_->_properties_read_requested) {
			ACS_TRA_PRINT_ERROR("Calling 'log4cplus::PropertyConfigurator::doConfigure()': this_->_watched_file == '%s': function '%s'\n",
					this_->_watched_file ?: "(NULL POINTER)", __PRETTY_FUNCTION__);

			// Re-read the log4cplus properties file
			if (!this_->_stop_requested && this_->_watched_file) log4cplus::PropertyConfigurator::doConfigure(this_->_watched_file);

			ACS_TRA_PRINT_ERROR("Function 'log4cplus::PropertyConfigurator::doConfigure()' called: function '%s'\n", __PRETTY_FUNCTION__);

#ifdef ACS_TRA_WATCH_THREAD_USES_INOTIFY
			// Draining the inotify interface buffer
			int inotify_fd = this_->_inotify_fd;
			for (errno = 0; ((bytes_read = ::read(inotify_fd, inotify_buffer, sizeof(inotify_buffer))) > 0) || ((bytes_read < 0) && (errno == EINTR)); errno = 0)
				;

			if (bytes_read == 0) { // FATAL: inotify descriptor returned 0. This is very very strange
				ACS_TRA_PRINT_ERROR(errno, "Call '::read()' failed: (bytes_read == 0) reading events from inotify interface: function '%s'\n", __PRETTY_FUNCTION__);
			} else if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
				ACS_TRA_PRINT_ERROR(errno, "Call '::read()' failed: reading events from inotify interface: no EAGAIN or EWOULDBLOCK: function '%s'\n", __PRETTY_FUNCTION__);
			}

			this_->_properties_read_requested = 0;
#else
			this_->_properties_read_requested = 0;
			this_->_watched_file_last_mt = watched_file_stats.st_mtime;
#endif
		}
	} while (!this_->_stop_requested);

	ACS_TRA_PRINT_ERROR(errno, "WATCH THREAD: terminating: function '%s'\n", __PRETTY_FUNCTION__);

	// TODO: Draining the stop signal pipe
	int pipe_read_end_fd = this_->_stop_signal_pipe[0];
	unsigned char pipe_buffer [4 * 1024];

	for (errno = 0; ((bytes_read = ::read(pipe_read_end_fd, pipe_buffer, sizeof(pipe_buffer))) > 0) || ((bytes_read < 0) && (errno == EINTR)); errno = 0)
		;

	ACS_TRA_PRINT_ERROR(errno, "WATCH THREAD: exiting: function '%s'\n", __PRETTY_FUNCTION__);

	return reinterpret_cast<void *>(ERR_NO_ERROR);
}
