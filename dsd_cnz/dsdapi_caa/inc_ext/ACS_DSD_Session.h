#ifndef HEADER_GUARD_CLASS__ACS_DSD_Session
#define HEADER_GUARD_CLASS__ACS_DSD_Session ACS_DSD_Session

/** @file ACS_DSD_Session.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-14
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
 *	| P0.9.0 | 2010-10-14 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <sys/types.h>

#include "ACS_DSD_Communicator.h"

//Forward declarations
class ACS_DSD_Acceptor;
class ACS_DSD_IOStream;
class ACS_DSD_Server;
class ACS_DSD_Client;
class ACE_INET_Addr;
class ACE_UNIX_Addr;
class ACE_Time_Value;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Session

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_Session ACS_DSD_Session.h
 *	@brief ACS_DSD_Session class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-14
 *	@version 1.0.0
 *
 *	ACS_DSD_Session Class detailed description
 */
class __CLASS_NAME__ : public ACS_DSD_Communicator {
	//=========//
	// Friends //
	//=========//
	friend class ACS_DSD_Server;
	friend class ACS_DSD_Client;

	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_Session Default constructor
	 *
	 *	ACS_DSD_Session Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : ACS_DSD_Communicator(), _stream(0), _OCP_transfert_active(false), _remote_node(), _remote_ip4_address(0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

private:
	/** @brief ACS_DSD_Session Copy constructor
	 *
	 *	ACS_DSD_Session Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs)
	: ACS_DSD_Communicator(rhs), _stream(0), _OCP_transfert_active(rhs._OCP_transfert_active),
	  _remote_node(rhs._remote_node), _remote_ip4_address(rhs._remote_ip4_address) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Session Destructor
	 *
	 *	ACS_DSD_Session Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~__CLASS_NAME__ ();

	//===========//
	// Functions //
	//===========//
public:
	/** @brief get_remote_node method
	 *
	 *	get_remote_node method detailed description
	 *
	 *	@param[out] node Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_remote_node (ACS_DSD_Node & node) const;

	/** @brief get_remote_ip4_address method
	 *
	 *	get_remote_ip4_address method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline uint32_t get_remote_ip4_address () const { return _remote_ip4_address; }

	/** @brief recv method
	 *
	 *	recv method detailed description
	 *
	 *	@param[out] buf Description
	 *	@param[in] size Description
	 *	@param[in] timeout_ms Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ssize_t recv (void * buf, size_t size, const unsigned * timeout_ms, size_t * bytes_transferred) const;
	inline ssize_t recv (void * buf, size_t size) const { return recv(buf, size, 0, 0); }
	inline ssize_t recv (void * buf, size_t size, unsigned timeout_ms) const { return recv(buf, size, &timeout_ms, 0); }
	inline ssize_t recv (void * buf, size_t size, size_t & bytes_transferred) const { return recv(buf, size, 0, &bytes_transferred); }
	inline ssize_t recv (void * buf, size_t size, unsigned timeout_ms, size_t & bytes_transferred) const { return recv(buf, size, &timeout_ms, &bytes_transferred); }

	/** @brief send method
	 *
	 *	send method detailed description
	 *
	 *	@param[in] buf Description
	 *	@param[in] n Description
	 *	@param[in] timeout_ms Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ssize_t send (const void * buf, size_t size, const unsigned * timeout_ms, size_t * bytes_transferred) const;
	inline ssize_t send (const void * buf, size_t size) const { return send(buf, size, 0, 0); }
	inline ssize_t send (const void * buf, size_t size, unsigned timeout_ms) const { return send(buf, size, &timeout_ms, 0); }
	inline ssize_t send (const void * buf, size_t size, size_t & bytes_transferred) const { return send(buf, size, 0, &bytes_transferred); }
	inline ssize_t send (const void * buf, size_t size, unsigned timeout_ms, size_t & bytes_transferred) const { return send(buf, size, &timeout_ms, &bytes_transferred); }

#if !defined (ACS_DSD_API_FLAGGED_SEND_DISABLED)
	inline ssize_t sendf (const void * buf, size_t size, const unsigned * timeout_ms, size_t * bytes_transferred, int flags) const {
		return sendf_imp(buf, size, timeout_ms, bytes_transferred, flags);
	}
	inline ssize_t sendf (const void * buf, size_t size, int flags) const { return sendf(buf, size, 0, 0, flags); }
	inline ssize_t sendf (const void * buf, size_t size, unsigned timeout_ms, int flags) const {
		return sendf(buf, size, &timeout_ms, 0, flags);
	}
	inline ssize_t sendf (const void * buf, size_t size, size_t & bytes_transferred, int flags) const {
		return sendf(buf, size, 0, &bytes_transferred, flags);
	}
	inline ssize_t sendf (const void * buf, size_t size, unsigned timeout_ms, size_t & bytes_transferred, int flags) const {
		return sendf(buf, size, &timeout_ms, &bytes_transferred, flags);
	}
#endif

	/** @brief close_reader method
	 *
	 *	close_reader method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int close_reader ();

	/** @brief close_writer method
	 *
	 *	close_writer method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int close_writer ();

	/** @brief close method
	 *
	 *	close method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int close ();

	/** @brief get_handles method
	 *
	 *	get_handles method detailed description
	 *
	 *	@param[out] handles Description
	 *	@param[in,out] handle_count Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int get_handles (acs_dsd::HANDLE * handles, int & handle_count) const;

	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_dsd::SessionStateConstants state () const;

	/** @brief Get options on the underlying socket descriptor
	 *
	 *	\c get_option retrieve the options associated with the underlying communication object (socket).
	 *	Options may exists at multiple protocol levels; they are always present at the uppermost socket level.
	 *
	 *	@param[in] level \a level parameter specify the level at which the requested option resides.
	 *	@param[in] option The user can specify the option he is interested in using this parameter.
	 *	@param[out] optval Identify a buffer in which the value for the requested option(s) are to be returned.
	 *	@param[in,out] optlen \a optlen is a value-result parameter, initially containing the size of the buffer
	 *								 pointed to by \a optval, and modified on return to indicate the actual size of the value returned.
	 *								 If no option value is to be supplied or returned, \a optval may be NULL.
	 *	@return On success, zero is returned. On error, one of the following error codes is returned and errno is set appropriately:
	 *					\li \c acs_dsd::ERR_SESSION_DISCONNECTED
	 *					\li \c acs_dsd::ERR_SYSTEM_GET_OPTION
	 *	@sa \a getsockopt in the section 2 of the Linux Programmers Manual.
	 */
	int get_option (int level, int option, void * optval, int * optlen) const;

	/** @brief Set options on the underlying socket descriptor
	 *
	 *	\c set_option activates options associated with the underlying communication object (socket).
	 *	Options may exists at multiple protocol levels; they are always present at the uppermost socket level.
	 *
	 *	@param[in] level \a level parameter specify the level at which the option to activate resides.
	 *	@param[in] option The user can specify the option he is interested in using this parameter.
	 *	@param[in] optval Identify a buffer in which the value of the option to be applied is stored.
	 *	@param[in] optlen \a optlen parameter specify the size of the option value stored in the buffer
	 *						 pointed to by \a optval.
	 *	@return On success, zero is returned. On error, one of the following error codes is returned and errno is set appropriately:
	 *					\li \c acs_dsd::ERR_SESSION_DISCONNECTED
	 *					\li \c acs_dsd::ERR_SYSTEM_SET_OPTION
	 *	@remarks Most socket-level options utilize an \a int parameter for \a optval.
	 *					 For \b set_option, the parameter should be non-zero to enable a boolean option, or zero if the option is to be disabled.
	 *	@sa \a setsockopt in the section 2 of the Linux Programmers Manual.
	 */
	int set_option (int level, int option, void * optval, int optlen) const;

private:
	/** @brief accept_inet method
	 *
	 *	accept_inet method detailed description
	 *
	 *	@param[in] acceptor Description
	 *	@param[out] errno_save Description
	 *	@param[out] remote_addr_str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int accept_inet (ACS_DSD_Acceptor * acceptor, int & errno_save, char (& remote_addr_str)[64]);

	/** @brief accept_unix method
	 *
	 *	accept_unix method detailed description
	 *
	 *	@param[in] acceptor Description
	 *	@param[out] errno_save Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int accept_unix (ACS_DSD_Acceptor * acceptor, int & errno_save);

	/** @brief connect_inet method
	 *
	 *	connect_inet method detailed description
	 *
	 *	@param[in] inet_addresses Description
	 *	@param[in] host_info_ptr Description
	 *	@param[in] timeout Description
	 *	@param[out] errno_save Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int connect_inet (
			const ACE_INET_Addr (& inet_addresses)[acs_dsd::CONFIG_NETWORKS_SUPPORTED],
			acs_dsd::SystemTypeConstants system_type,
			const char * node_name,
			ACE_Time_Value * timeout,
			int & errno_save);

	/** @brief connect_unix method
	 *
	 *	connect_unix method detailed description
	 *
	 *	@param[in] unix_address Description
	 *	@param[in] timeout Description
	 *	@param[out] errno_save Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int connect_unix (const ACE_UNIX_Addr & unix_address, int & errno_save);

	ssize_t sendf_imp (
			const void * buf,
			size_t size,
			const unsigned * timeout_ms,
			size_t * bytes_transferred,
			int flags) const;

	inline void reset_remote_node () { _remote_node.reset(); }

	inline void set_remote_node (short system_id, const char * system_name, acs_dsd::SystemTypeConstants system_type, acs_dsd::NodeStateConstants node_state, const char * node_name, acs_dsd::NodeSideConstants node_side) {
		_remote_node.set(system_id, system_name, system_type, node_state, node_name, node_side);
	}

	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		ACS_DSD_Communicator::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	ACS_DSD_IOStream * _stream;
	bool _OCP_transfert_active;
	ACS_DSD_Node _remote_node;
	uint32_t _remote_ip4_address;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Session
