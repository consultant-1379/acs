#ifndef HEADER_GUARD_CLASS__ACS_DSD_IOStream
#define HEADER_GUARD_CLASS__ACS_DSD_IOStream ACS_DSD_IOStream

/** @file ACS_DSD_IOStream.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-14
 *	@version 1.0.0
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.0 | 2010-07-14 | xnicmut      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2010-08-04 | xnicmut      | Released for sprint 2               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.3 | 2010-09-28 | xnicmut      | Released for sprint 4               |
 *	+========+============+==============+=====================================+
 */

#include <cstdarg>
#include <sys/types.h>
#include <sys/uio.h>

#include "ace/Time_Value.h"
#include "ace/SOCK_Stream.h"

#include "ACS_DSD_Macros.h"

#if defined(ACS_DSD_HAS_ALLOCA)
#	include <alloca.h>
#else
#	include <new>
#endif // defined(ACS_DSD_HAS_ALLOCA)

#include "ACS_DSD_Trace.h"

class ACS_DSD_TraTracer;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_IOStream

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_IOStream ACS_DSD_IOStream.h
 *	@brief ACS_DSD_IOStream class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-14
 *	@version 1.0.0
 *
 *	ACS_DSD_IOStream Class detailed description
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
protected:
	/** @brief ACS_DSD_IOStream Default constructor
	 *
	 *	ACS_DSD_IOStream Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : _state(acs_dsd::STREAM_STATE_DISCONNECTED) { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	/** @brief ACS_DSD_IOStream Copy constructor
	 *
	 *	ACS_DSD_IOStream Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : _state(rhs._state) { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_IOStream Destructor
	 *
	 *	ACS_DSD_IOStream Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief get_handle method
	 *
	 *	get_handle method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual acs_dsd::HANDLE get_handle () const = 0;

	/** @brief get_local_addr method
	 *
	 *	get_local_addr method detailed description
	 *
	 *	@param[out] sa Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int get_local_addr (ACE_Addr & sa) const = 0;

	/** @brief get_remote_addr method
	 *
	 *	get_remote_addr method detailed description
	 *
	 *	@param[out] sa Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int get_remote_addr (ACE_Addr & sa) const = 0;

	/** @brief recvv method
	 *
	 *	recvv method detailed description
	 *
	 *	@param[in,out] iov Description
	 *	@param[in] n Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recvv (iovec iov[], int n, const ACE_Time_Value * timeout = 0) const = 0;

	/** @brief recvv method
	 *
	 *	recvv method detailed description
	 *
	 *	@param[in,out] io_vec Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recvv (iovec * io_vec, const ACE_Time_Value * timeout = 0) const = 0;

	/** @brief recv method
	 *
	 *	recv method detailed description
	 *
	 *	@param[out] buf Description
	 *	@param[in] n Description
	 *	@param[in] flags Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recv (void * buf, size_t n, int flags, const ACE_Time_Value * timeout = 0) const = 0;

	/** @brief recv method
	 *
	 *	recv method detailed description
	 *
	 *	@param[out] buf Description
	 *	@param[in] n Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recv (void * buf, size_t n, const ACE_Time_Value * timeout = 0) const = 0;

	/** @brief recv method
	 *
	 *	recv method detailed description
	 *
	 *	@param[in] n Description
	 *	@param[in,out] ... Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ssize_t recv (size_t n, ...) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		va_list argp;
		const int total_tuples = static_cast<int>(n >> 1);
		iovec * iovp = 0;

#if defined(ACS_DSD_HAS_ALLOCA)
		iovp = reinterpret_cast<iovec *>(alloca(total_tuples * sizeof(iovec)));
#else
		if (!(iovp = new (::std::nothrow) iovec[total_tuples])) return acs_dsd::ERR_SYSTEM_RECEIVE;
#endif // defined(ACS_DSD_HAS_ALLOCA)

		va_start(argp, n);

		for (int i = 0; i < total_tuples; ++i) {
			iovp[i].iov_base = va_arg(argp, char *);
			iovp[i].iov_len = va_arg(argp, int);
		}

		const ssize_t result = recvv(iovp, total_tuples);

#if !defined(ACS_DSD_HAS_ALLOCA)
		delete[] iovp;
#endif // !defined(ACS_DSD_HAS_ALLOCA)

		va_end(argp);
	  return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_RECEIVE);
	}

	/** @brief sendv method
	 *
	 *	sendv method detailed description
	 *
	 *	@param[in] iov Description
	 *	@param[in] n Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t sendv (const iovec iov[], int n, const ACE_Time_Value * timeout = 0) const = 0;

	/** @brief send method
	 *
	 *	send method detailed description
	 *
	 *	@param[in] buf Description
	 *	@param[in] n Description
	 *	@param[in] flags Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t send (const void * buf, size_t n, int flags, const ACE_Time_Value * timeout = 0) const = 0;

	/** @brief send method
	 *
	 *	send method detailed description
	 *
	 *	@param[in] buf Description
	 *	@param[in] n Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t send (const void * buf, size_t n, const ACE_Time_Value * timeout = 0) const = 0;

	/** @brief send method
	 *
	 *	send method detailed description
	 *
	 *	@param[in] n Description
	 *	@param[in] ... Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ssize_t send (size_t n, ...) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		va_list argp;
		const int total_tuples = static_cast<int>(n >> 1);
		iovec * iovp = 0;

#if defined (ACS_DSD_HAS_ALLOCA)
		iovp = reinterpret_cast<iovec *>(alloca(total_tuples * sizeof(iovec)));
#else
		if (!(iovp = new (::std::nothrow) iovec[total_tuples])) return acs_dsd::ERR_SYSTEM_SEND;
#endif // defined(ACS_DSD_HAS_ALLOCA)

		va_start(argp, n);

		for (int i = 0; i < total_tuples; ++i) {
			iovp[i].iov_base = va_arg(argp, char *);
			iovp[i].iov_len = va_arg(argp, int);
		}

		const ssize_t result = sendv(iovp, total_tuples);

#if !defined(ACS_DSD_HAS_ALLOCA)
		delete[] iovp;
#endif // !defined(ACS_DSD_HAS_ALLOCA)

	  va_end(argp);
	  return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_SEND);
	}

	/** @brief recv_n method
	 *
	 *	recv_n method detailed description
	 *
	 *	@param[out] buf Description
	 *	@param[in] len Description
	 *	@param[in] flags Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recv_n (void * buf, size_t len, int flags, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const = 0;

	/** @brief recv_n method
	 *
	 *	recv_n method detailed description
	 *
	 *	@param[out] buf Description
	 *	@param[in] len Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recv_n (void * buf, size_t len, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const = 0;

	/** @brief recvv_n method
	 *
	 *	recvv_n method detailed description
	 *
	 *	@param[in,out] iov Description
	 *	@param[in] iovcnt Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t recvv_n (iovec iov[], int iovcnt, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const = 0;

	/** @brief sendv_n method
	 *
	 *	sendv_n method detailed description
	 *
	 *	@param[in] iov Description
	 *	@param[in] iovcnt Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t sendv_n (const iovec iov[], int iovcnt, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const = 0;

	/** @brief send_n method
	 *
	 *	send_n method detailed description
	 *
	 *	@param[in] buf Description
	 *	@param[in] len Description
	 *	@param[in] flags Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t send_n (const void * buf, size_t len, int flags, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const = 0;

	/** @brief send_n method
	 *
	 *	send_n method detailed description
	 *
	 *	@param[in] buf Description
	 *	@param[in] len Description
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ssize_t send_n (const void * buf, size_t len, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const = 0;

	/** @brief close_reader method
	 *
	 *	close_reader method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int close_reader () = 0;

	/** @brief close_writer method
	 *
	 *	close_writer method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int close_writer () = 0;

	/** @brief close method
	 *
	 *	close method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int close () = 0;

	/** @brief get_option method
	 *
	 *	get_option method detailed description
	 *
	 *	@param[in] level Description
	 *	@param[in] option Description
	 *	@param[out] optval Description
	 *	@param[in,out] optlen Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int get_option (int level, int option, void * optval, int * optlen) const = 0;

	/** @brief set_option method
	 *
	 *	set_option method detailed description
	 *
	 *	@param[in] level Description
	 *	@param[in] option Description
	 *	@param[in] optval Description
	 *	@param[in] optlen Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int set_option (int level, int option, void * optval, int optlen) const = 0;

	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::IOStreamStateConstants state () const { return _state; }

protected:
	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@param[in] new_state Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::IOStreamStateConstants state (acs_dsd::IOStreamStateConstants new_state) {
		const acs_dsd::IOStreamStateConstants old_state = _state;
		return (_state = new_state, old_state);
	}

	//===========//
	// Operators //
	//===========//
protected:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ &) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	/// IOStreamStateConstants _state brief description.
	acs_dsd::IOStreamStateConstants _state;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_IOStream
