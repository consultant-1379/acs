#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_IOStreamProvider.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

namespace acs_dsd {
	const int MSG_PEEK_RECV_LOOP_COUNTER_MAX = 10;

	/*
	 * Implementation to solve the MSG_PEEK flagged receive ACE bug.
	 *
	 * The following function will work only if into flags parameter there is the MSG_KEEP flag.
	 * Do not use it without MSG_KEEP bit flag active
	 */
	ssize_t
	recv_n_i (
			acs_dsd::HANDLE handle,
			void * buf,
			size_t buf_size,
			int flags,
			const ACE_Time_Value * timeout,
			size_t * bytes_transferred) {
		size_t bt_temp = 0;
		size_t & bt = (bytes_transferred ? *bytes_transferred : bt_temp);

		ssize_t n;
		ssize_t result = 0;
		int error = 0;

		int val = 0;
		ACE::record_and_set_non_blocking_mode (handle, val);

		register int loop_counter = MSG_PEEK_RECV_LOOP_COUNTER_MAX;

		for (bt = 0; (bt < buf_size) && loop_counter--; bt = n) {
			// Try to transfer as much of the remaining data as possible.
			// Since the socket is in non-blocking mode, this call will not block.
			errno = 0;
			n = ACE_OS::recv(handle, static_cast <char *> (buf), buf_size, flags);

			// Check for errors.
			if ((n == 0) || (n == -1)) {
				// Check for possible blocking.
				if ((n == -1) && (errno == EWOULDBLOCK)) {
					// Wait upto <timeout> for the blocking to subside.
					int const rtn = ACE::handle_read_ready(handle, timeout);

					// Did select() succeed?
					if (rtn != -1) {
						// Blocking subsided in <timeout> period.  Continue data transfer.
						n = 0;
						continue;
					}
				}

				// Wait in select() timed out or other data transfer or select() failures.
				error = 1;
				result = n;
				break;
			}
		}

		ACE::restore_non_blocking_mode(handle, val);

		if (error) return result;
		else return static_cast<ssize_t> (bt);
	}

	/*
	 * Implementation to solve the MSG_PEEK flagged receive ACE bug.
	 *
	 * The following function will work only if into flags parameter there is the MSG_KEEP flag
	 * Do not use it without MSG_KEEP bit flag active
	 */
	ssize_t
	recv_n_i (
			acs_dsd::HANDLE handle,
			void * buf,
			size_t buf_size,
			int flags,
			size_t * bytes_transferred) {
		size_t bt_temp = 0;
		size_t & bt = (bytes_transferred ? *bytes_transferred : bt_temp);
		ssize_t n = 0;

		register int loop_counter = MSG_PEEK_RECV_LOOP_COUNTER_MAX;

		for (bt = 0; (bt < buf_size) && loop_counter--; bt = n) {
			// Try to transfer as much of the remaining data as possible.
			errno = 0;
			n = ACE_OS::recv(handle, static_cast <char *> (buf), buf_size, flags);

			// Check EOF.
			if (n == 0) return 0;

			// Check for other errors.
			if (n == -1) {
				// Check for possible blocking.
				if (errno == EWOULDBLOCK) {
					// Wait for the blocking to subside.
					int const result = ACE::handle_read_ready(handle, 0);

					// Did select() succeed?
					if (result != -1) {
						// Blocking subsided.  Continue data transfer.
						n = 0;
						continue;
					}
				}

				// Other data transfer or select() failures.
				return -1;
			}
		}

		return static_cast<ssize_t> (bt);
	}
}
