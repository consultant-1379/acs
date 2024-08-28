#ifndef HEADER_GUARD_CLASS__ACS_DSD_ConnectorProvider
#define HEADER_GUARD_CLASS__ACS_DSD_ConnectorProvider ACS_DSD_ConnectorProvider

/** @file ACS_DSD_ConnectorProvider.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-23
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
 *	| P0.9.0 | 2010-07-23 | xnicmut      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2010-08-04 | xnicmut      | Released for sprint 2               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.3 | 2010-09-28 | xnicmut      | Released for sprint 4               |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_TraTracer.h"
#include "ACS_DSD_Connector.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_ConnectorProvider

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_ConnectorProvider ACS_DSD_ConnectorProvider.h
 *	@brief ACS_DSD_ConnectorProvider class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-23
 *	@version 1.0.0
 *
 *	ACS_DSD_ConnectorProvider Class detailed description
 */
template <typename connector_t, typename addr_t, typename stream_t>
class __CLASS_NAME__ : public ACS_DSD_Connector {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_ConnectorProvider Default constructor
	 *
	 *	ACS_DSD_ConnectorProvider Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () : ACS_DSD_Connector(), _connector() { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

private:
	/** @brief ACS_DSD_ConnectorProvider Copy constructor
	 *
	 *	ACS_DSD_ConnectorProvider Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ & rhs) : ACS_DSD_Connector(rhs), _connector(rhs._connector) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_ConnectorProvider Destructor
	 *
	 *	ACS_DSD_ConnectorProvider Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief connect method
	 *
	 *	connect method detailed description
	 *
	 *	@param[out] new_stream Description
	 *	@param[in] remote_sap Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int connect (ACS_DSD_IOStream & new_stream, const ACE_Addr & remote_sap, ACE_Time_Value * timeout = 0) {
		stream_t * stream = dynamic_cast<stream_t *>(&new_stream); if (!stream) return acs_dsd::WAR_WRONG_STREAM_TYPE;
		const addr_t * addr = dynamic_cast<const addr_t *>(&remote_sap); if (!addr) return acs_dsd::WAR_WRONG_ADDRESS_TYPE;
		return connect(*stream, *addr, timeout);
	}

	/** @brief connect method
	 *
	 *	connect method detailed description
	 *
	 *	@param[out] new_stream Description
	 *	@param[in] remote_sap Description
	 *	@param[in] timeout Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int connect (stream_t & new_stream, const addr_t & remote_sap, ACE_Time_Value * timeout = 0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		{ /* TRACING*/
			char remote_sap_str [1024] = {0};
			remote_sap.addr_to_string(remote_sap_str, ACS_DSD_ARRAY_SIZE(remote_sap_str));

			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"Connecting to remote host '%s' with timeout == {%ld, %ld}",
					remote_sap_str, (timeout ? timeout->sec() : -1), (timeout ? timeout->usec() : -1));
		}

		//Check if the object new_stream is free and not already used for an established communication session
		if (acs_dsd::STREAM_STATE_DISCONNECTED ^ new_stream.state()) return acs_dsd::WAR_STREAM_ALREADY_IN_USE;

		errno = 0;
		if (_connector.connect(new_stream._stream, remote_sap, timeout) || errno) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: on connecting: errno == %d", errno);

			if (errno != EINPROGRESS) return acs_dsd::ERR_SYSTEM_CONNECT;

			addr_t remote_sap_(remote_sap);

			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "WARNING: errno is EINPROGRES: trying to complete the connection");

			// errno == EINPROGRESS then the connection is in progress and we try to complete it
			errno = 0;
			if (_connector.complete(new_stream._stream, & remote_sap_, timeout) || errno) {
				ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: on completing connection: errno == %d", errno);

				return acs_dsd::ERR_SYSTEM_CONNECT;
			}
		}

		new_stream.state(acs_dsd::STREAM_STATE_CONNECTED);
		return 0;
	}

	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		ACS_DSD_Connector::operator=(rhs);
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	connector_t _connector; ///< Connector internal object.
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_ConnectorProvider
