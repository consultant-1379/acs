#ifndef HEADER_GUARD_CLASS__ACS_DSD_IOStreamProvider
#define HEADER_GUARD_CLASS__ACS_DSD_IOStreamProvider ACS_DSD_IOStreamProvider

/** @file ACS_DSD_IOStreamProvider.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-26
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
 *	| P0.9.0 | 2010-07-26 | xnicmut      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2010-08-04 | xnicmut      | Released for sprint 2               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.3 | 2010-09-28 | xnicmut      | Released for sprint 4               |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_IOStream.h"

template <typename acceptor_t, typename addr_t, typename stream_t> class ACS_DSD_AcceptorProvider;
template <typename connector_t, typename addr_t, typename stream_t> class ACS_DSD_ConnectorProvider;

namespace acs_dsd {
	enum {
#if defined (ACS_DSD_ACE_MSG_PEEK_BUG_WORK_AROUND)
		MSG_PEEK_FLAG = MSG_PEEK
#	warning "ACS DSD compiled to work around the MSG_PEEK ACE bug"
#else
		MSG_PEEK_FLAG = 0x00
#	warning "MSG_PEEK ACE bug work around is not activated"
#endif
#	define MSG_PEEK_FLAG MSG_PEEK_FLAG
	};

	ssize_t
	recv_n_i (
			acs_dsd::HANDLE handle,
			void * buf,
			size_t buf_size,
			int flags,
			const ACE_Time_Value * timeout,
			size_t * bytes_transferred);

	ssize_t
	recv_n_i (
			acs_dsd::HANDLE handle,
			void * buf,
			size_t buf_size,
			int flags,
			size_t * bytes_transferred);
}

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_IOStreamProvider

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_IOStreamProvider ACS_DSD_IOStreamProvider.h
 *	@brief ACS_DSD_IOStreamProvider class tamplate
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-26
 *	@version 1.0.0
 *
 *	ACS_DSD_IOStreamProvider Class template detailed description
 */
template <typename stream_t>
class __CLASS_NAME__ : public ACS_DSD_IOStream {
	//=========//
	// Friends //
	//=========//
	template <typename acceptor_t, typename addr_t, typename dsd_stream_t> friend class ACS_DSD_AcceptorProvider;
	template <typename connector_t, typename addr_t, typename dsd_stream_t> friend class ACS_DSD_ConnectorProvider;

	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_IOStreamProvider Default constructor
	 *
	 *	ACS_DSD_IOStreamProvider Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : ACS_DSD_IOStream(), _stream() { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

private:
	/** @brief ACS_DSD_IOStreamProvider Copy constructor
	 *
	 *	ACS_DSD_IOStreamProvider Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACS_DSD_IOStream(rhs), _stream(rhs._stream)  {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_IOStreamProvider Destructor
	 *
	 *	ACS_DSD_IOStreamProvider Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		close();
	}

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
	virtual inline acs_dsd::HANDLE get_handle () const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		return _stream.get_handle();
	}

	/** @brief get_local_addr method
	 *
	 *	get_local_addr method detailed description
	 *
	 *	@param[out] sa Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int get_local_addr (ACE_Addr & sa) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _stream.get_local_addr(sa) ? acs_dsd::ERR_SYSTEM_GET_ADDRESS : 0;
	}

	/** @brief get_remote_addr method
	 *
	 *	get_remote_addr method detailed description
	 *
	 *	@param[out] sa Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int get_remote_addr (ACE_Addr & sa) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _stream.get_remote_addr(sa) ? acs_dsd::ERR_SYSTEM_GET_ADDRESS : 0;
	}

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
	virtual inline ssize_t recvv (iovec iov[], int n, const ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.recvv(iov, n, timeout);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_RECEIVE);
	};

	/** @brief recvv method
	 *
	 *	recvv method detailed description
	 *
	 *	@param[in,out] io_vec Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline ssize_t recvv (iovec * io_vec, const ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.recvv(io_vec, timeout);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_RECEIVE);
	};

	using ACS_DSD_IOStream::recv;

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
	virtual inline ssize_t recv (void * buf, size_t n, int flags, const ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.recv(buf, n, flags, timeout);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_RECEIVE);
	}

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
	virtual inline ssize_t recv (void * buf, size_t n, const ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.recv(buf, n, timeout);
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
	virtual inline ssize_t sendv (const iovec iov[], int n, const ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.sendv(iov, n, timeout);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_SEND);
	}

	using ACS_DSD_IOStream::send;

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
	virtual inline ssize_t send (const void * buf, size_t n, int flags, const ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.send(buf, n, flags, timeout);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_SEND);
	}

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
	virtual inline ssize_t send (const void * buf, size_t n, const ACE_Time_Value * timeout = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.send(buf, n, timeout);
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
	virtual inline ssize_t recv_n (
			void * buf,
			size_t len,
			int flags,
			const ACE_Time_Value * timeout = 0,
			size_t * bytes_transferred = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		ssize_t result = 0;

		if (flags & acs_dsd::MSG_PEEK_FLAG) {
			// MSG_PEEK ACE bug activated. DSD is providing the patched ACE recv_n function
			errno = 0;
			result = (timeout
					? acs_dsd::recv_n_i(get_handle(), buf, len, flags, timeout, bytes_transferred)
					: acs_dsd::recv_n_i(get_handle(), buf, len, flags, bytes_transferred)
			);
		} else {
			errno = 0;
			result = _stream.recv_n(buf, len, flags, timeout, bytes_transferred);
		}

		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_RECEIVE);
	};

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
	virtual inline ssize_t recv_n (void * buf, size_t len, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.recv_n(buf, len, timeout, bytes_transferred);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_RECEIVE);
	}

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
	virtual inline ssize_t recvv_n (iovec iov[], int iovcnt, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.recvv_n(iov, iovcnt, timeout, bytes_transferred);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_RECEIVE);
	}

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
	virtual inline ssize_t sendv_n (const iovec iov[], int iovcnt, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.sendv_n(iov, iovcnt, timeout, bytes_transferred);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_SEND);
  }

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
	virtual inline ssize_t send_n (const void * buf, size_t len, int flags, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.send_n(buf, len, flags, timeout, bytes_transferred);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_SEND);
	}

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
	virtual inline ssize_t send_n (const void * buf, size_t len, const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const ssize_t result = _stream.send_n(buf, len, timeout, bytes_transferred);
		return (result >= 0) ? result : static_cast<ssize_t>(acs_dsd::ERR_SYSTEM_SEND);
	}

	/** @brief close_reader method
	 *
	 *	close_reader method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int close_reader () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const int result = _stream.close_reader();
		if (!result) switch (state()) {
		case acs_dsd::STREAM_STATE_CONNECTED: state(acs_dsd::STREAM_STATE_CONNECTED_WRITER_ONLY); break;
		case acs_dsd::STREAM_STATE_CONNECTED_READER_ONLY: state(acs_dsd::STREAM_STATE_DISCONNECTED); break;
		default: break;
		}

		return result ? acs_dsd::ERR_SYSTEM_CLOSE : 0;
	}

	/** @brief close_writer method
	 *
	 *	close_writer method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int close_writer () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		const int result = _stream.close_writer();
		if (!result) switch (state()) {
		case acs_dsd::STREAM_STATE_CONNECTED: state(acs_dsd::STREAM_STATE_CONNECTED_READER_ONLY); break;
		case acs_dsd::STREAM_STATE_CONNECTED_WRITER_ONLY: state(acs_dsd::STREAM_STATE_DISCONNECTED); break;
		default: break;
		}

		return result ? acs_dsd::ERR_SYSTEM_CLOSE : 0;
	}

	/** @brief close method
	 *
	 *	close method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int close () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _stream.close() ? acs_dsd::ERR_SYSTEM_CLOSE : (state(acs_dsd::STREAM_STATE_DISCONNECTED), 0);
	}

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
	virtual inline int get_option (int level, int option, void * optval, int * optlen) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _stream.get_option(level, option, optval, optlen) ? acs_dsd::ERR_SYSTEM_GET_OPTION : 0;
	}

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
	virtual inline int set_option (int level, int option, void * optval, int optlen) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;
		return _stream.set_option(level, option, optval, optlen) ? acs_dsd::ERR_SYSTEM_SET_OPTION : 0;
	}


	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		ACS_DSD_IOStream::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	stream_t _stream; ///< Stream internal object.
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_IOStreamProvider
