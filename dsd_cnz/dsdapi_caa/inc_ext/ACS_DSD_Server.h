#ifndef HEADER_GUARD_CLASS__ACS_DSD_Server
#define HEADER_GUARD_CLASS__ACS_DSD_Server ACS_DSD_Server

/** @file ACS_DSD_Server.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
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
 *	| P0.9.0 | 2010-09-28 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <string>

#include "ACS_DSD_Communicator.h"
#include "ACS_DSD_Session.h"
#include "netdb.h"

//Forward declarations
class ACS_DSD_Acceptor;
class ACE_Recursive_Thread_Mutex;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_Server

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_Server ACS_DSD_Server.h
 *	@brief ACS_DSD_Server class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-09-28
 *	@version 1.0.0
 *
 *	ACS_DSD_Server Class detailed description
 */
class __CLASS_NAME__ : public ACS_DSD_Communicator {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_Server Default constructor
	 *
	 *	ACS_DSD_Server Constructor detailed description
	 *
	 *	@param[in] service_mode Description
	 *	@remarks Remarks
	 */
	explicit __CLASS_NAME__ (acs_dsd::ServiceModeConstants service_mode = acs_dsd::SERVICE_MODE_UNKNOWN);

private:
	/** @brief ACS_DSD_Server Copy constructor
	 *
	 *	ACS_DSD_Server Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs)
	: ACS_DSD_Communicator(rhs), _state(rhs._state), _service_mode(rhs._service_mode), _acceptors(), _epoll_fd(acs_dsd::INVALID_HANDLE),
	  _app_service_name(), _app_domain_name(), _accept_sync_object_ptr(0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		//This copy constructor is private so I do not copy acceptors pointers.
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_Server Destructor
	 *
	 *	ACS_DSD_Server Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~__CLASS_NAME__ ();

	//===========//
	// Functions //
	//===========//
public:
	/** @brief open method
	 *
	 *	open method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int open () { return open(_service_mode, 0); }

	/** @brief open method
	 *
	 *	open method detailed description
	 *
	 *	@param[in] service_mode Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int open (acs_dsd::ServiceModeConstants service_mode) { return open(service_mode, 0); }

	/** @brief open method
	 *
	 *	open method detailed description
	 *
	 *	@param[in] service_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int open (const char * service_id) { return open(_service_mode, service_id); }

	/** @brief open method
	 *
	 *	open method detailed description
	 *
	 *	@param[in] service_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int open (const std::string & service_id) { return open(_service_mode, service_id.c_str()); }

	/** @brief open method
	 *
	 *	open method detailed description
	 *
	 *	@param[in] service_mode Description
	 *	@param[in] service_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int open (acs_dsd::ServiceModeConstants service_mode, const std::string & service_id) {
		return open(service_mode, service_id.c_str());
	}

	/** @brief open method
	 *
	 *	open method detailed description
	 *
	 *	@param[in] service_mode Description
	 *	@param[in] service_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int open (acs_dsd::ServiceModeConstants service_mode, const char * service_id);

	/** @brief publish method
	 *
	 *	publish method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] visibility Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int publish (const std::string & service_name, const std::string & service_domain, acs_dsd::ServiceVisibilityConstants visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE) {
		return publish(service_name.c_str(), service_domain.c_str(), visibility);
	}

	/** @brief publish method
	 *
	 *	publish method detailed description
	 *
	 *	@param[in] service_name Description
	 *	@param[in] service_domain Description
	 *	@param[in] visibility Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int publish (const char * service_name, const char * service_domain, acs_dsd::ServiceVisibilityConstants visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE);

	/** @brief accept method
	 *
	 *	accept method detailed description
	 *
	 *	@param[out] session Description
	 *	@param[in] timeout_ms Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int accept (ACS_DSD_Session & session, int timeout_ms) {
		return accept(session, timeout_ms, true);
	}

	inline int accept (ACS_DSD_Session & session) {
		return accept(session, 0, false);
	}

	/** @brief unregister method
	 *
	 *	unregister method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int unregister ();

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

	/** @brief service_mode method
	 *
	 *	service_mode method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::ServiceModeConstants service_mode () const { return _service_mode; }

	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::ServerStateConstants state () const { return _state; }

protected:
	/** @brief state method
	 *
	 *	state method detailed description
	 *
	 *	@param[in] new_state Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::ServerStateConstants state (acs_dsd::ServerStateConstants new_state) {
		const acs_dsd::ServerStateConstants old_state = _state;
		return (_state = new_state, old_state);
	}

private:
	/** @brief delete_acceptors method
	 *
	 *	delete_acceptors method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void delete_acceptors ();

	/** @brief open_inet method
	 *
	 *	open_inet method detailed description
	 *
	 *	@param[in] service_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int open_inet(acs_dsd::ServiceModeConstants service_mode, const char * service_id);

	/** @brief open_unix method
	 *
	 *	open_unix method detailed description
	 *
	 *	@param[in] service_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int open_unix(acs_dsd::ServiceModeConstants service_mode, const char * service_id);

	/** @brief accept method
	 *
	 *	accept method detailed description
	 *
	 *	@param[out] session Description
	 *	@param[in] timeout_ms Description
	 *	@param[in] apply_timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int accept (ACS_DSD_Session & session, int timeout_ms, bool apply_timeout);

	/** @brief publish_inet method
	 *
	 *	publish_inet method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int publish_inet (const char * service_name, const char * service_domain, acs_dsd::ServiceVisibilityConstants visibility, ACS_DSD_IOStream & unix_stream, uint8_t & response_code);

	/** @brief publish_unix method
	 *
	 *	publish_unix method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int publish_unix (const char * service_name, const char * service_domain, acs_dsd::ServiceVisibilityConstants visibility, ACS_DSD_IOStream & unix_stream, uint8_t & response_code);

	/** @brief init_epoll_interface method
	 *
	 *	init_epoll_interface method detailed description
	 *
	 *	@param[in] epfd Description
	 *	@param[in] acceptors Description
	 *	@param[in] size Description
	 *	@param[out] errno_save Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int epoll_add_acceptors (int epfd, ACS_DSD_Acceptor * acceptors[], size_t size, int & errno_save, size_t start_index = 0);

	/** @brief epoll_delete_acceptors method
	 *
	 *	epoll_delete_acceptors method detailed description
	 *
	 *	@param[in] epfd Description
	 *	@param[in] acceptors Description
	 *	@param[in] size Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int epoll_delete_acceptors (int epfd, ACS_DSD_Acceptor * acceptors[], size_t size);

	/** @brief isPortReserved method
	 *
	 * isPortReserved method detailed description
	 *
	 *    Returns true if the given port is already defined in the
	 *    /etc/services file, or otherwise improper to use.
	 * Parameters:
	 *    port           Port number to check.
	 *
	 * Return value:
	 *    true           Port number is defined in /etc/services or
	 *                   improper to use.
	 *    false          Port number is free use.
	 */
	bool isPortReserved(int port);

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
	acs_dsd::ServerStateConstants _state;
	acs_dsd::ServiceModeConstants _service_mode;
	ACS_DSD_Acceptor * _acceptors[acs_dsd::CONFIG_NETWORKS_SUPPORTED + 1];
	int _epoll_fd;
	char _app_service_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX];
	char _app_domain_name[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX];
	ACE_Recursive_Thread_Mutex * _accept_sync_object_ptr;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_Server
