#include <syslog.h>
#include <errno.h>
#include <openssl/crypto.h>

// Check if OpenSSL supports the execution in multi threaded environment
#define OPENSSL_THREAD_DEFINES
#include <openssl/opensslconf.h>
#if !defined(OPENSSL_THREADS)
# error OpenSSL does not support threads, core dumps are possible if the calls are multithreaded.
#endif

#include "ACS_APGCC_OpenSSLThreading.h"

/************************************/
/* Static variables initializations */
/************************************/
// WARNING: This array of mutexes is needed by OpenSSL library for the support for multithreading.
//          !!! DON'T USE THEM FOR OTHER PURPOSES !!!
pthread_mutex_t * ACS_APGCC_OpenSSLThreading::_openssl_threading_mutexes_pool = 0;

ACS_APGCC_ThreadSupport_ReturnTypeT ACS_APGCC_OpenSSLThreading::register_threading_callbacks_for_openssl () {
	// Retrieve the number of mutexes required by OpenSSL library for implementing multithreading
	const int required_locks = CRYPTO_num_locks();

	// Allocate memory for the pool of mutexes
	_openssl_threading_mutexes_pool = reinterpret_cast<pthread_mutex_t *>(malloc(required_locks * sizeof(pthread_mutex_t)));
	if (!(_openssl_threading_mutexes_pool)) {
		// Memory allocation failed for the thread mutexes pool!
		::syslog(LOG_ERR, "Failed to allocate memory for the mutexes pool!");
		return ACS_APGCC_THREADSUPPORT_NO_ENOUGH_MEMORY;
	}

	// After allocating the requested memory, initialize each mutex in the pool
	for (int i = 0; i < required_locks; ++i) {
		if (const int call_result = ::pthread_mutex_init(&(_openssl_threading_mutexes_pool[i]), 0)) {
			const int errno_save = errno;
			::syslog(LOG_ERR, "Call 'pthread_mutex_init' failed, call_result == %d, errno == %d.", call_result, errno_save);

			// Finalize all the already initialized mutexes
			for (int j = 0; j < i; ++j)	::pthread_mutex_destroy(&(_openssl_threading_mutexes_pool[j]));
			return ACS_APGCC_THREADSUPPORT_INIT_FAILED;
		}
	}

	// Register the two callbacks required by OpenSSL library for its thread safety
	CRYPTO_set_id_callback(&openssl_threading_id_callback);
	CRYPTO_set_locking_callback(&openssl_threading_locking_callback);
	return ACS_APGCC_THREADSUPPORT_OK;
}

ACS_APGCC_ThreadSupport_ReturnTypeT ACS_APGCC_OpenSSLThreading::unregister_threading_callbacks_for_openssl () {
	// Retrieve the number of mutexes required by OpenSSL library for implementing multithreading
	const int required_locks = CRYPTO_num_locks();

	// Unregister the two callbacks required by OpenSSL library for its thread safety
	CRYPTO_set_id_callback(0);
	CRYPTO_set_locking_callback(0);

	// Finalize each mutex in the pool
	for (int i = 0; i < required_locks; ++i) {
		if (const int call_result = ::pthread_mutex_destroy(&(_openssl_threading_mutexes_pool[i]))) {
			const int errno_save = errno;
			::syslog(LOG_WARNING, "WARNING: Call 'pthread_mutex_destroy' failed, call_result == %d, errno == %d.", call_result, errno_save);
		}
	}

	// Free the allocated memory for the pool of mutexes
	::free(_openssl_threading_mutexes_pool);
	_openssl_threading_mutexes_pool = 0;
	return ACS_APGCC_THREADSUPPORT_OK;
}

unsigned long ACS_APGCC_OpenSSLThreading::openssl_threading_id_callback() {
	// The only operation performed by this callback is to return the ID of the caller thread
	return static_cast<unsigned long>(::pthread_self());
}

void ACS_APGCC_OpenSSLThreading::openssl_threading_locking_callback(int mode, int i, const char * /*file*/, int /*line*/) {
	// The purpose of this method is to lock/unlock the mutex having the provided index according to the mode parameter
 	if (mode & CRYPTO_LOCK)	{
 		// Mutex locking requested
		::pthread_mutex_lock(&(_openssl_threading_mutexes_pool[i]));
	}
	else {
 		// Mutex unlocking requested
		::pthread_mutex_unlock(&(_openssl_threading_mutexes_pool[i]));
	}
}
