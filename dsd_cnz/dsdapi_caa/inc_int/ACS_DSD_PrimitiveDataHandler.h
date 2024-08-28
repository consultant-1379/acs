#ifndef HEADER_GUARD_CLASS__ACS_DSD_PrimitiveDataHandler
#define HEADER_GUARD_CLASS__ACS_DSD_PrimitiveDataHandler ACS_DSD_PrimitiveDataHandler

/** @file ACS_DSD_PrimitiveDataHandler.h
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
 *	| P0.9.2 | 2010-09-22 | xnicmut      | Released for sprint 3               |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.3 | 2010-09-28 | xnicmut      | Released for sprint 4               |
 *	+========+============+==============+=====================================+
 */
#include <cstring>
#include <cstdarg>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#include "ACS_DSD_Macros.h"
#include "ACS_DSD_MessageSender.h"
#include "ACS_DSD_MessageReceiver.h"
#include "ACS_DSD_OCPMessageSender.h"
#include "ACS_DSD_OCPMessageReceiver.h"
#include "ACS_DSD_Session.h"
#include "ACS_DSD_Trace.h"
#include "ACS_DSD_TraTracer.h"

namespace acs_dsd {
	enum PCP_PimitiveIdConstants {
		PCP_PRIMITIVE_ID_UNKNOWN								=	-1,	///< PCP_PRIMITIVE_ID_UNKNOWN description

		PCP_PRIMITIVE_ID_MIN										= 0,	///< PCP_PRIMITIVE_ID_MIN description

		//Primitive identifiers constants for the CP-AP communication protocol.
		PCP_CPAP_UNKNOW_REPLY_ID								=	0,	///< PCP_CPAP_UNKNOW_REPLY_ID description
		PCP_CPAP_ADDRESS_REQUEST_ID							=	1,	///< PCP_CPAP_ADDRESS_REQUEST_ID description
		PCP_CPAP_ADDRESS_REPLY_ID								=	2,	///< PCP_CPAP_ADDRESS_REPLY_ID description
		PCP_CPAP_LIST_APPLICATION_REQUEST_ID		=	18,	///< PCP_CPAP_LIST_APPLICATION_REQUEST_ID description
		PCP_CPAP_LIST_APPLICATION_REPLY_ID			=	19,	///< PCP_CPAP_LIST_APPLICATION_REPLY_ID description
		PCP_CPAP_STARTUP_REQUEST_ID							=	20,	///< PCP_CPAP_STARTUP_REQUEST_ID description
		PCP_CPAP_STARTUP_REPLY_ID								=	21,	///< PCP_CPAP_STARTUP_REPLY_ID description

		//Primitive identifiers constants for the application DSD service communication protocol.
		PCP_DSDAPI_REGISTRATION_INET_REQUEST_ID	=	31,	///< PCP_DSDAPI_REGISTRATION_INET_REQUEST_ID description
		PCP_DSDAPI_REGISTRATION_UNIX_REQUEST_ID	=	32,	///< PCP_DSDAPI_REGISTRATION_UNIX_REQUEST_ID description
		PCP_DSDAPI_REGISTRATION_REPLY_ID				=	33,	///< PCP_DSDAPI_REGISTRATION_REPLY_ID description
		PCP_DSDAPI_UNREGISTRATION_REQUEST_ID		=	34,	///< PCP_DSDAPI_UNREGISTRATION_REQUEST_ID description
		PCP_DSDAPI_UNREGISTRATION_REPLY_ID			=	35,	///< PCP_DSDAPI_UNREGISTRATION_REPLY_ID description
		PCP_DSDAPI_ADDRESS_REQUEST_ID						=	36,	///< PCP_DSDAPI_ADDRESS_REQUEST_ID description
		PCP_DSDAPI_ADDRESS_INET_REPLY_ID				=	37,	///< PCP_DSDAPI_ADDRESS_INET_REPLY_ID description
		PCP_DSDAPI_ADDRESS_UNIX_REPLY_ID				=	38,	///< PCP_DSDAPI_ADDRESS_UNIX_REPLY_ID description
		PCP_DSDAPI_QUERY_REQUEST_ID							=	39,	///< PCP_DSDAPI_QUERY_REQUEST_ID description
		PCP_DSDAPI_QUERY_REPLY_ID								=	40,	///< PCP_DSDAPI_QUERY_REPLY_ID description
		PCP_DSDAPI_LIST_REQUEST_ID							=	41,	///< PCP_DSDAPI_LIST_REQUEST_ID description
		PCP_DSDAPI_LIST_INET_REPLY_ID						=	42,	///< PCP_DSDAPI_LIST_INET_REPLY_ID description
		PCP_DSDAPI_LIST_UNIX_REPLY_ID						=	43,	///< PCP_DSDAPI_LIST_UNIX_REPLY_ID description
		PCP_DSDAPI_NOTIFICATION_REQUEST_ID			=	44,	///< PCP_DSDAPI_NOTIFICATION_REQUEST_ID description
		PCP_DSDAPI_NOTIFICATION_INDICATION_ID		=	45,	///< PCP_DSDAPI_NOTIFICATION_INDICATION_ID description
		PCP_DSDAPI_NODE_REQUEST_ID							=	46,	///< PCP_DSDAPI_NODE_REQUEST_ID description
		PCP_DSDAPI_NODE_REPLY_ID								=	47,	///< PCP_DSDAPI_NODE_REPLY_ID description

		//Primitive identifiers constants for the DSD service-service communication protocol.
		PCP_DSDDSD_ADDRESS_REQUEST_ID						=	51,	///< PCP_DSDDSD_ADDRESS_REQUEST_ID description
		PCP_DSDDSD_ADDRESS_INET_REPLY_ID				=	52,	///< PCP_DSDDSD_ADDRESS_INET_REPLY_ID description
		PCP_DSDDSD_ADDRESS_UNIX_REPLY_ID				=	53,	///< PCP_DSDDSD_ADDRESS_UNIX_REPLY_ID description
		PCP_DSDDSD_NODE_INFO_REQUEST_ID					=	54,	///< PCP_DSDDSD_NODE_INFO_REQUEST_ID description
		PCP_DSDDSD_NODE_INFO_REPLY_ID						=	55,	///< PCP_DSDDSD_NODE_INFO_REPLY_ID description
		PCP_DSDDSD_QUERY_REQUEST_ID							=	56,	///< PCP_DSDDSD_QUERY_REQUEST_ID description
		PCP_DSDDSD_QUERY_REPLY_ID								=	57,	///< PCP_DSDDSD_QUERY_REPLY_ID description
		PCP_DSDDSD_NOTIFY_PUBLISH_ID						=	58,	///< PCP_DSDDSD_NOTIFY_PUBLISH description
		PCP_DSDDSD_NOTIFY_PUBLISH_REPLY_ID			=	59,	///< PCP_DSDDSD_NOTIFY_PUBLISH_REPLY description
		PCP_DSDDSD_NOTIFY_UNREGISTER_ID					=	60,	///< PCP_DSDDSD_NOTIFY_UNREGISTER_ID description
		PCP_DSDDSD_NOTIFY_UNREGISTER_REPLY_ID		=	61,	///< PCP_DSDDSD_NOTIFY_UNREGISTER_REPLY_ID description
		PCP_DSDDSD_LIST_REQUEST_ID							=	62,	///< PCP_DSDDSD_LIST_REQUEST_ID description
		PCP_DSDDSD_LIST_INET_REPLY_ID						=	63,	///< PCP_DSDDSD_LIST_INET_REPLY_ID description
		PCP_DSDDSD_LIST_UNIX_REPLY_ID						=	64,	///< PCP_DSDDSD_LIST_UNIX_REPLY_ID description
		PCP_DSDDSD_STARTUP_REQUEST_ID						=	65,	///< PCP_DSDDSD_STARTUP_REQUEST_ID description
		PCP_DSDDSD_STARTUP_REPLY_ID							=	66,	///< PCP_DSDDSD_STARTUP_REPLY_ID description
		PCP_DSDDSD_HWC_CHANGE_NOTIFY_ID					= 67, ///< PCP_DSDDSD_HWC_CHANGE_NOTIFY_ID description
		PCP_DSDDSD_HWC_CHANGE_NOTIFY_REPLY_ID		= 68, ///< PCP_DSDDSD_HWC_CHANGE_NOTIFY_REPLY_ID description

		PCP_PRIMITIVE_ID_MAX,													///< PCP_PRIMITIVE_ID_MAX description
		PCP_PRIMITIVES_RANGE_COUNT							= PCP_PRIMITIVE_ID_MAX - PCP_PRIMITIVE_ID_MIN	///< PCP_PRIMITIVES_RANGE_COUNT description
	};

	enum PCP_GenericConstants {
		PCP_SUPPORTED_VERSION_COUNT	=	2,	///< PCP_SUPPORTED_VERSION_COUNT description

		PCP_FIELD_COUNT_MIN					=	2,	///< PCP_FIELD_COUNT_MIN description
		PCP_FIELD_COUNT_MAX					=	14	///< PCP_FIELD_COUNT_MAX description
	};

	enum PCP_FieldSizeConstants {
		PCP_FIELD_SIZE_PRIMITIVE_ID			=	1,																					///< PCP_FIELD_SIZE_PRIMITIVE_ID description
		PCP_FIELD_SIZE_VERSION					=	1,																					///< PCP_FIELD_SIZE_VERSION description
		PCP_FIELD_SIZE_NODE_NAME				=	acs_dsd::CONFIG_NODE_NAME_SIZE_MAX,					///< PCP_FIELD_SIZE_NODE_NAME description
		PCP_FIELD_SIZE_APP_DOMAIN_NAME	=	acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX,		///< PCP_FIELD_SIZE_APP_DOMAIN_NAME description
		PCP_FIELD_SIZE_APP_SERVICE_NAME	=	acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX,	///< PCP_FIELD_SIZE_APP_SERVICE_NAME description
		PCP_FIELD_SIZE_PROT_ERROR_CODE	=	1,																					///< PCP_FIELD_SIZE_PROT_ERROR_CODE description
		PCP_FIELD_SIZE_CONNECTION_TYPE	=	2,																					///< PCP_FIELD_SIZE_CONNECTION_TYPE description
		PCP_FIELD_SIZE_CONNECTION_COUNT	=	2,																					///< PCP_FIELD_SIZE_CONNECTION_COUNT description
		PCP_FIELD_SIZE_IPV4_ADDRESS			=	4,																					///< PCP_FIELD_SIZE_IPV4_ADDRESS description
		PCP_FIELD_SIZE_PORT_NUMBER			=	2,																					///< PCP_FIELD_SIZE_PORT_NUMBER description
		PCP_FIELD_SIZE_AP_MASK					=	8,																					///< PCP_FIELD_SIZE_AP_MASK description
		PCP_FIELD_SIZE_CP_MASK					=	4,																					///< PCP_FIELD_SIZE_CP_MASK description
		PCP_FIELD_SIZE_AP_IDENTITY			=	1,																					///< PCP_FIELD_SIZE_AP_IDENTITY description
		PCP_FIELD_SIZE_UNKNOWN					=	1,																					///< PCP_FIELD_SIZE_UNKNOWN description
		PCP_FIELD_SIZE_PROCESS_NAME			=	acs_dsd::CONFIG_PROCESS_NAME_SIZE_MAX,			///< PCP_FIELD_SIZE_PROCESS_NAME description
		PCP_FIELD_SIZE_PROCESS_ID				=	4,																					///< PCP_FIELD_SIZE_PROCESS_ID description
		PCP_FIELD_SIZE_VISIBILITY				=	1,																					///< PCP_FIELD_SIZE_VISIBILITY description
		PCP_FIELD_SIZE_UNIX_SOCK_PATH		=	acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX,		///< PCP_FIELD_SIZE_UNIX_SOCK_PATH description
		PCP_FIELD_SIZE_SYSTEM_ID				=	4,																					///< PCP_FIELD_SIZE_SYSTEM_ID description
		PCP_FIELD_SIZE_NODE_STATE				=	1,																					///< PCP_FIELD_SIZE_NODE_STATE description
		PCP_FIELD_SIZE_QUERY_ORDER			=	1,																					///< PCP_FIELD_SIZE_QUERY_ORDER description
		PCP_FIELD_SIZE_LIST_ORDER				=	1,																					///< PCP_FIELD_SIZE_LIST_ORDER description
		PCP_FIELD_SIZE_SPX_STATE				=	1,																					///< PCP_FIELD_SIZE_SPX_STATE description
		PCP_FIELD_SIZE_RESERVED_1_BYTE	=	1,																					///< PCP_FIELD_SIZE_RESERVED_1_BYTE description
		PCP_FIELD_SIZE_TIMEOUT_4_BYTES	=	4,																					///< PCP_FIELD_SIZE_TIMEOUT_4_BYTES description
		PCP_FIELD_SIZE_FBN							=	2,																					///< PCP_FIELD_SIZE_FBN description
		PCP_FIELD_SIZE_OP_TYPE					=	1,																					///< PCP_FIELD_SIZE_OP_TYPE description
		PCP_FIELD_SIZE_SYSTEM_TYPE			=	2,																					///< PCP_FIELD_SIZE_SYSTEM_TYPE description
		PCP_FIELD_SIZE_NODE_SIDE				= 1,																					///< PCP_FIELD_SIZE_NODE_SIDE description
	};
}

namespace {
	struct byte_buffer_type { typedef uint8_t * type; typedef const uint8_t * const_type; };
	struct byte_buffer_reverse_type { typedef uint8_t * type; typedef const uint8_t * const_type; };
	struct string_type { typedef uint8_t * type; typedef const uint8_t * const_type; };
	struct byte_type { typedef uint8_t type; typedef const uint8_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };

	struct two_bytes_host_order_type { typedef uint16_t type; typedef const uint16_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };
	struct two_bytes_net_order_type { typedef uint16_t type; typedef const uint16_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };
	struct two_bytes_big_endian_order_type { typedef uint16_t type; typedef const uint16_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };
	struct two_bytes_little_endian_order_type { typedef uint16_t type; typedef const uint16_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };

	struct four_bytes_host_order_type { typedef uint32_t type; typedef const uint32_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };
	struct four_bytes_net_order_type { typedef uint32_t type; typedef const uint32_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };
	struct four_bytes_big_endian_order_type { typedef uint32_t type; typedef const uint32_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };
	struct four_bytes_little_endian_order_type { typedef uint32_t type; typedef const uint32_t const_type; typedef unsigned promoted_type; typedef const unsigned const_promoted_type; };

	struct eight_bytes_big_endian_order_type {
		typedef uint64_t type;
		typedef const uint64_t const_type;
	#if __WORDSIZE == 64
		typedef long unsigned promoted_type;
		typedef const long unsigned const_promoted_type;
	#else
		typedef uint64_t promoted_type;
		typedef const uint64_t const_promoted_type;
	#endif
	};
	struct eight_bytes_little_endian_order_type {
		typedef uint64_t type;
		typedef const uint64_t const_type;
	#if __WORDSIZE == 64
		typedef long unsigned promoted_type;
		typedef const long unsigned const_promoted_type;
	#else
		typedef uint64_t promoted_type;
		typedef const uint64_t const_promoted_type;
	#endif
	};
}

struct copier_base {
	inline virtual void operator() (va_list /*from*/, size_t /*size*/, void * /*to*/) const {}
	inline virtual void operator() (const void * /*from*/, size_t /*size*/, va_list /*to*/) const {}
};

template < typename type > struct copier : copier_base {
	inline virtual void operator() (va_list /*from*/, size_t /*size*/, void * /*to*/) const {}
	inline virtual void operator() (const void * /*from*/, size_t /*size*/, va_list /*to*/) const {}
};

template<> struct copier < byte_buffer_type > : copier_base {
	inline virtual void operator() (va_list from, size_t size, void * to) const { memcpy(to, va_arg(from, byte_buffer_type::const_type), size); }
	inline virtual void operator() (const void * from, size_t size, va_list to) const { memcpy(va_arg(to, byte_buffer_type::type), from, size); }
};

template<> struct copier < byte_buffer_reverse_type > : copier_base {
	inline virtual void operator() (va_list from, size_t size, void * to) const {
		copy(reinterpret_cast<byte_buffer_reverse_type::type>(to), va_arg(from, byte_buffer_reverse_type::const_type) + (size - 1), size);
	}
	inline virtual void operator() (const void * from, size_t size, va_list to) const {
		copy(va_arg(to, byte_buffer_reverse_type::type), reinterpret_cast<byte_buffer_reverse_type::const_type>(from), size);
	}
private:
	inline void copy (byte_buffer_reverse_type::type to, byte_buffer_reverse_type::const_type from, size_t size) const { while (size--) *to++ = *from--; }
};

template<> struct copier < string_type > : copier_base {
	inline virtual void operator() (va_list from, size_t size, void * to) const { memccpy(to, va_arg(from, string_type::const_type), 0, size - 1); }
	inline virtual void operator() (const void * from, size_t size, va_list to) const { memccpy(va_arg(to, string_type::type), from, 0, size - 1); }
};

template<> struct copier < byte_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		*reinterpret_cast<byte_type::type *>(to) = static_cast<byte_type::const_type>(va_arg(from, byte_type::promoted_type));
	}
	inline virtual void operator() (const void * from, size_t /*size*/, va_list to) const {
		*va_arg(to, byte_type::type *) = *reinterpret_cast<byte_type::const_type *>(from);
	}
};

template<> struct copier < two_bytes_host_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		*reinterpret_cast<two_bytes_host_order_type::type *>(to) = static_cast<two_bytes_host_order_type::const_type>(va_arg(from, two_bytes_host_order_type::promoted_type));
	}
	inline virtual void operator() (const void * from, size_t /*size*/, va_list to) const {
		*va_arg(to, two_bytes_host_order_type::type *) = *reinterpret_cast<two_bytes_host_order_type::const_type *>(from);
	}
};

template<> struct copier < two_bytes_net_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		*reinterpret_cast<two_bytes_net_order_type::type *>(to) = htons(static_cast<two_bytes_net_order_type::const_type>(va_arg(from, two_bytes_net_order_type::promoted_type)));
	}
	inline virtual void operator() (const void * from, size_t /*size*/, va_list to) const {
		*va_arg(to, two_bytes_net_order_type::type *) = htons(*reinterpret_cast<two_bytes_net_order_type::const_type *>(from));
	}
};

#if 0 //NOT USED FOR NOW
template<> struct copier < two_bytes_big_endian_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		copy(reinterpret_cast<uint8_t *>(to), static_cast<two_bytes_big_endian_order_type::const_type>(va_arg(from, two_bytes_big_endian_order_type::promoted_type)));
	}
private:
	inline void copy (uint8_t * to, two_bytes_big_endian_order_type::const_type from) const {
		*to++ = static_cast<uint8_t>((from & 0xFF00) >> 8);
		*to = static_cast<uint8_t>(from & 0x00FF);
	}
};

template<> struct copier < two_bytes_little_endian_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		copy(reinterpret_cast<uint8_t *>(to), static_cast<two_bytes_little_endian_order_type::const_type>(va_arg(from, two_bytes_little_endian_order_type::promoted_type)));
	}
private:
	inline void copy (uint8_t * to, two_bytes_little_endian_order_type::const_type from) const {
		*to++ = static_cast<uint8_t>(from & 0x00FF);
		*to = static_cast<uint8_t>((from & 0xFF00) >> 8);
	}
};
#endif

template<> struct copier < four_bytes_host_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		*reinterpret_cast<four_bytes_host_order_type::type *>(to) = static_cast<four_bytes_host_order_type::const_type>(va_arg(from, four_bytes_host_order_type::promoted_type));
	}
	inline virtual void operator() (const void * from, size_t /*size*/, va_list to) const {
		*va_arg(to, four_bytes_host_order_type::type *) = *reinterpret_cast<four_bytes_host_order_type::const_type *>(from);
	}
};

template<> struct copier < four_bytes_net_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		*reinterpret_cast<four_bytes_net_order_type::type *>(to) = htonl(static_cast<four_bytes_net_order_type::const_type>(va_arg(from, four_bytes_net_order_type::promoted_type)));
	}
	inline virtual void operator() (const void * from, size_t /*size*/, va_list to) const {
		*va_arg(to, four_bytes_host_order_type::type *) = htonl(*reinterpret_cast<four_bytes_host_order_type::const_type *>(from));
	}
};

#if 0 //NOT USED FOR NOW
template<> struct copier < four_bytes_big_endian_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		uint32_t from_ = static_cast<four_bytes_big_endian_order_type::const_type>(va_arg(from, four_bytes_big_endian_order_type::promoted_type));
		uint8_t * to_ptr = reinterpret_cast<uint8_t *>(to) + 3;
		for ( ; to_ptr >= reinterpret_cast<uint8_t *>(to); from_ >>= 8) *to_ptr-- = static_cast<uint8_t>(from_ & 0xFF);
	}
};

template<> struct copier < four_bytes_little_endian_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		uint32_t from_ = static_cast<four_bytes_little_endian_order_type::const_type>(va_arg(from, four_bytes_little_endian_order_type::promoted_type));
		uint8_t * to_ptr = reinterpret_cast<uint8_t *>(to);
		for (int i = 0; i < 4; ++i, from_ >>= 8) *to_ptr++ = static_cast<uint8_t>(from_ & 0xFF);
	}
};
#endif

template<> struct copier < eight_bytes_big_endian_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		copy(reinterpret_cast<uint8_t *>(to), static_cast<eight_bytes_big_endian_order_type::type>(va_arg(from, eight_bytes_big_endian_order_type::promoted_type)));
	}
	inline virtual void operator() (const void * from, size_t /*size*/, va_list to) const {
		copy(reinterpret_cast<uint8_t *>(va_arg(to, eight_bytes_big_endian_order_type::type *)), *reinterpret_cast<eight_bytes_big_endian_order_type::const_type *>(from));
	}
private:
	inline void copy (uint8_t * to, eight_bytes_big_endian_order_type::type from) const {
		uint8_t * to_ptr = to + 7;
		for ( ; to_ptr >= to; from >>= 8) *to_ptr-- = static_cast<uint8_t>(from & 0xFF);
	}
};

template<> struct copier < eight_bytes_little_endian_order_type > : copier_base {
	inline virtual void operator() (va_list from, size_t /*size*/, void * to) const {
		copy(reinterpret_cast<uint8_t *>(to), static_cast<eight_bytes_big_endian_order_type::type>(va_arg(from, eight_bytes_big_endian_order_type::promoted_type)));
	}
	inline virtual void operator() (const void * from, size_t /*size*/, va_list to) const {
		copy(reinterpret_cast<uint8_t *>(va_arg(to, eight_bytes_big_endian_order_type::type *)), *reinterpret_cast<eight_bytes_big_endian_order_type::const_type *>(from));
	}
private:
	inline void copy (uint8_t * to, eight_bytes_big_endian_order_type::type from) const {
		for (int i = 0; i < 8; ++i, from >>= 8) *to++ = static_cast<uint8_t>(from & 0xFF);
	}
};

extern const copier<void> _null_copier_object;

struct copy_caller {
	inline copy_caller () : _copier(_null_copier_object) {}
	template < typename type >
	inline explicit copy_caller (const copier<type> & copier) : _copier(copier) {}
	inline void operator() (va_list from, size_t size, void * to) const { _copier(from, size, to); }
	inline void operator() (const void * from, size_t size, va_list to) const { _copier(from, size, to); }
private: const copier_base & _copier;
};

extern const uint8_t ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[acs_dsd::PCP_SUPPORTED_VERSION_COUNT][acs_dsd::PCP_PRIMITIVES_RANGE_COUNT];
extern const uint16_t ACS_DSD_PCP_PRIMITIVE_FIELDS_SIZES[acs_dsd::PCP_SUPPORTED_VERSION_COUNT][acs_dsd::PCP_PRIMITIVES_RANGE_COUNT][acs_dsd::PCP_FIELD_COUNT_MAX];
extern const copy_caller ACS_DSD_PCP_PRIMITIVE_FIELDS_COPIER_CALLERS[acs_dsd::PCP_SUPPORTED_VERSION_COUNT][acs_dsd::PCP_PRIMITIVES_RANGE_COUNT][acs_dsd::PCP_FIELD_COUNT_MAX];

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_PrimitiveDataHandler

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_PrimitiveDataHandler ACS_DSD_PrimitiveDataHandler.h
 *	@brief ACS_DSD_PrimitiveDataHandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-14
 *	@version 1.0.0
 *
 *	ACS_DSD_PrimitiveDataHandler Class detailed description
 */
template <size_t buf_size = 1024>
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief ACS_DSD_PrimitiveDataHandler Default constructor
	 *
	 *	ACS_DSD_PrimitiveDataHandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ ()
	: _buffer(), _primitive_data_size(0), _ready_to_send(false), _sender(0), _receiver(0), _session(0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		_buffer[0] = acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN;
	}

	/** @brief ACS_DSD_PrimitiveDataHandler Default constructor
	 *
	 *	ACS_DSD_PrimitiveDataHandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline explicit __CLASS_NAME__ (const ACS_DSD_Session & session)
	: _buffer(), _primitive_data_size(0), _ready_to_send(false), _sender(0), _receiver(0), _session(&session) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		_buffer[0] = acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN;
	}

	/** @brief ACS_DSD_PrimitiveDataHandler Default constructor
	 *
	 *	ACS_DSD_PrimitiveDataHandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const ACS_DSD_MessageSender & sender, const ACS_DSD_MessageReceiver & receiver)
	: _buffer(), _primitive_data_size(0), _ready_to_send(false), _sender(&sender), _receiver(&receiver), _session(0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		_buffer[0] = acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN;
	}

	/** @brief ACS_DSD_PrimitiveDataHandler Default constructor
	 *
	 *	ACS_DSD_PrimitiveDataHandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const ACS_DSD_OCPMessageSender & sender, const ACS_DSD_OCPMessageReceiver & receiver)
	: _buffer(), _primitive_data_size(0), _ready_to_send(false), _sender(&sender), _receiver(&receiver), _session(0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		_buffer[0] = acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN;
	}

private:
	/** @brief ACS_DSD_PrimitiveDataHandler Copy constructor
	 *
	 *	ACS_DSD_PrimitiveDataHandler Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ &)
	: _buffer(), _primitive_data_size(0), _ready_to_send(false), _sender(0), _receiver(0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_PrimitiveDataHandler Destructor
	 *
	 *	ACS_DSD_PrimitiveDataHandler Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//===========//
	// Functions //
	//===========//
public:
	/** @brief get_buffer method
	 *
	 *	get_buffer method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline const uint8_t (& get_buffer () const) [buf_size] { return _buffer; }

	/** @brief buffer_size method
	 *
	 *	buffer_size method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline size_t buffer_size () const { return buf_size; }

	/** @brief primitive_size method
	 *
	 *	primitive_size method detailed description
	 *
	 *	@param[in] primitive_id Description
	 *	@param[in] primitive_version Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int primitive_size (int primitive_id, unsigned primitive_version, size_t & size_in_bytes) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		if ((primitive_version < 1) || (acs_dsd::PCP_SUPPORTED_VERSION_COUNT < primitive_version)) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION((primitive_version < 1) || (acs_dsd::PCP_SUPPORTED_VERSION_COUNT < primitive_version)): Primitive version not supported: primitive_version == %u",
					primitive_version);
			return acs_dsd::WAR_PCP_VERSION_NOT_SUPPORTED;
		}

		size_t fields_count = 0;

		if ((primitive_id < acs_dsd::PCP_PRIMITIVE_ID_MIN) || (acs_dsd::PCP_PRIMITIVE_ID_MAX <= primitive_id)) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION((primitive_id < acs_dsd::PCP_PRIMITIVE_ID_MIN) || (acs_dsd::PCP_PRIMITIVE_ID_MAX <= primitive_id)): Primitive undefined: primitive ID == %d",
					primitive_id);
			return acs_dsd::WAR_PCP_PRIMITIVE_UNDEFINED;
		}

		if (!(fields_count = ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[--primitive_version][primitive_id])) { //ERROR: primitive version not supported
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(!(fields_count = ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[--primitive_version][primitive_id])): Primitive version not supported: primitive_version == %u",
					primitive_version + 1);
			return acs_dsd::WAR_PCP_VERSION_NOT_SUPPORTED;
		}

		size_in_bytes = 0;

		for (size_t i = 0; i < fields_count; size_in_bytes += ACS_DSD_PCP_PRIMITIVE_FIELDS_SIZES[primitive_version][primitive_id][i++]) ;

		return 0;
	}

	/** @brief primitive_data_size method
	 *
	 *	primitive_data_size method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	size_t primitive_data_size () const { return _primitive_data_size; }

	/** @brief primitive_id method
	 *
	 *	primitive_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int primitive_id () const {
		static size_t cpap_address_reply_size = 0;

		int prim_id = ({ typeof(_buffer[0]) _unknown = acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN; _buffer[0] ^ _unknown; })
								? static_cast<int>(_buffer[0])
								: acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN;

		if (prim_id == acs_dsd::PCP_CPAP_ADDRESS_REQUEST_ID) {
			cpap_address_reply_size || primitive_size(acs_dsd::PCP_CPAP_ADDRESS_REPLY_ID, 1, cpap_address_reply_size);
			(primitive_data_size() <= cpap_address_reply_size) && (prim_id = acs_dsd::PCP_CPAP_ADDRESS_REPLY_ID);
		}

		return prim_id;
	}

	/** @brief primitive_version method
	 *
	 *	primitive_version method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline unsigned primitive_version () const {
		const int prim_id = primitive_id();
		const unsigned prim_version = _buffer[1];

		return
				((acs_dsd::PCP_CPAP_UNKNOW_REPLY_ID == prim_id) || (acs_dsd::PCP_CPAP_ADDRESS_REQUEST_ID == prim_id) || (acs_dsd::PCP_CPAP_ADDRESS_REPLY_ID == prim_id) || (acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN == prim_id))
				? 0
				: prim_version;
	}

	/** @brief make_primitive method
	 *
	 *	make_primitive method detailed description
	 *
	 *	@param[in] primitive_id Description
	 *	@param[in] primitive_version Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int make_primitive (int primitive_id, unsigned primitive_version, ...) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "USER_PARAMETER[primitive_id == %d]", primitive_id);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "USER_PARAMETER[primitive_version == %u]", primitive_version);

		_primitive_data_size = 0;

		if (primitive_version > acs_dsd::PCP_SUPPORTED_VERSION_COUNT) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(primitive_version > acs_dsd::PCP_SUPPORTED_VERSION_COUNT): Primitive version not supported: primitive_version == %u",
					primitive_version);
			return acs_dsd::ERR_PCP_VERSION_NOT_SUPPORTED;
		}

		if ((primitive_id < acs_dsd::PCP_PRIMITIVE_ID_MIN) || (acs_dsd::PCP_PRIMITIVE_ID_MAX <= primitive_id)) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION((primitive_id < acs_dsd::PCP_PRIMITIVE_ID_MIN) || (acs_dsd::PCP_PRIMITIVE_ID_MAX <= primitive_id)): Primitive undefined: primitive_id == %d",
					primitive_id);
			return acs_dsd::ERR_PCP_PRIMITIVE_UNDEFINED;
		}

		unsigned prim_index = (primitive_version ? (primitive_version - 1) : 0);

		if (!ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][primitive_id]) { //ERROR: version not supported
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(!ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][primitive_id]): Primitive version not supported: "
					"primitive_version == %u, prim_index == %u, primitive_id == %d",
					primitive_version, prim_index, primitive_id);
			return acs_dsd::ERR_PCP_VERSION_NOT_SUPPORTED;
		}

		memset(_buffer, 0, buf_size);

		size_t buf_index = 0;
		const uint16_t * field_size_ptr = ACS_DSD_PCP_PRIMITIVE_FIELDS_SIZES[prim_index][primitive_id];
		const int fields_count = ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][primitive_id];
		int field_index = 0;

		// Insert the primitive ID after check
		if (buf_index >= buf_size) return acs_dsd::ERR_PCP_DATA_BUFFER_OVERFLOW;
		_buffer[buf_index] = static_cast<uint8_t >((primitive_id ^ acs_dsd::PCP_CPAP_ADDRESS_REPLY_ID) ? primitive_id : acs_dsd::PCP_CPAP_ADDRESS_REQUEST_ID);
		buf_index += *field_size_ptr++;
		++field_index;

		// Insert the version element after check
		// The two number 1 primitives and the unknown reply message (primitive 0) are primitive without a version element.
		if (buf_index >= buf_size) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(buf_index >= buf_size): Data buffer overflow: buf_index == %zu, buf_size == %zu",
					buf_index, buf_size);
			return acs_dsd::ERR_PCP_DATA_BUFFER_OVERFLOW;
		}
		if ((primitive_id ^ acs_dsd::PCP_CPAP_UNKNOW_REPLY_ID) && (primitive_id ^ acs_dsd::PCP_CPAP_ADDRESS_REQUEST_ID) && (primitive_id ^ acs_dsd::PCP_CPAP_ADDRESS_REPLY_ID)) {
			_buffer[buf_index] = static_cast<uint8_t >(primitive_version);
			buf_index += *field_size_ptr++;
			++field_index;
		}

		const copy_caller * copy_caller_ptr = ACS_DSD_PCP_PRIMITIVE_FIELDS_COPIER_CALLERS[prim_index][primitive_id] + field_index;

		va_list argp;
		va_start(argp, primitive_version);

		int result_code = 0;

		for ( ; field_index < fields_count; ++field_index, buf_index += *field_size_ptr++)
			if ((buf_index + *field_size_ptr) > buf_size) {
				ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
						"ERR_ASSERTION((buf_index + *field_size_ptr) > buf_size): Data buffer overflow: buf_index == %zu, *field_size_ptr == %u, buf_size == %zu",
						buf_index, *field_size_ptr, buf_size);
				result_code = acs_dsd::ERR_PCP_DATA_BUFFER_OVERFLOW;
				break;
			}
			else (*copy_caller_ptr++)(argp, *field_size_ptr, _buffer + buf_index);

		va_end(argp);

		result_code || (_primitive_data_size = buf_index, _ready_to_send = true);

		return result_code;
	}

	/** @brief unpack_primitive method
	 *
	 *	unpack_primitive method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int unpack_primitive (int & prim_id, unsigned & prim_version, ...) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		_ready_to_send = false;

		unsigned prim_version_ = primitive_version();

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "prim_version_ == %u", prim_version_);

		if (prim_version_ > acs_dsd::PCP_SUPPORTED_VERSION_COUNT) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(prim_version_ > acs_dsd::PCP_SUPPORTED_VERSION_COUNT): Bad version in primitive data: prim_version_ == %u",
					prim_version_);
			return acs_dsd::ERR_PCP_BAD_VERSION_IN_PRIMITIVE_DATA;
		}

		const int prim_id_ = primitive_id();
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "prim_id_ == %d", prim_id_);
		if ((prim_id_ < acs_dsd::PCP_PRIMITIVE_ID_MIN) || (acs_dsd::PCP_PRIMITIVE_ID_MAX <= prim_id_)) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION((prim_id_ < acs_dsd::PCP_PRIMITIVE_ID_MIN) || (acs_dsd::PCP_PRIMITIVE_ID_MAX <= prim_id_)): Bad primitive ID in primitive data: prim_id_ == %d",
					prim_id_);
			return acs_dsd::ERR_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA;
		}

		unsigned prim_index = (prim_version_ ? prim_version_ - 1 : 0);
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "prim_index == %u", prim_index);

		if (!ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][prim_id_]) { //ERROR: version not supported
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"ERR_ASSERTION(!ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][prim_id_]): Primitive version not supported: "
					"prim_version_ == %u, prim_index == %u, prim_id_ == %d",
					prim_version_, prim_index, prim_id_);
			return acs_dsd::ERR_PCP_VERSION_NOT_SUPPORTED;
		}

		const int fields_count = ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][prim_id_];
		const uint16_t * field_size_ptr = ACS_DSD_PCP_PRIMITIVE_FIELDS_SIZES[prim_index][prim_id_];
		size_t buf_index = *field_size_ptr++;
		int field_index = 1;

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "fields_count == %d", fields_count);

		//If the primitive version is present in the buffer then I should skip this field
		prim_version_ && (buf_index += *field_size_ptr++, ++field_index);

		//ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "Initializing copy_caller_ptr: field_index == %d", field_index);
		const copy_caller * copy_caller_ptr = ACS_DSD_PCP_PRIMITIVE_FIELDS_COPIER_CALLERS[prim_index][prim_id_] + field_index;
		//ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "copy_caller_ptr == 0x%p", copy_caller_ptr);

		va_list argp;
		va_start(argp, prim_version);

		for ( ; field_index < fields_count; ++field_index, buf_index += *field_size_ptr++) (*copy_caller_ptr++)(_buffer + buf_index, *field_size_ptr, argp);

		va_end(argp);

		prim_id = prim_id_;
		prim_version = prim_version_;

		return 0;
	}

	/** @brief send_primitive method
	 *
	 *	send_primitive method detailed description
	 *
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ssize_t send_primitive (const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) const {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;

		if (!_ready_to_send) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(!_ready_to_send): primitive data not ready to be send");
			return acs_dsd::ERR_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND;
		}

		ssize_t bytes_sent = 0;

		if (_session) {
			if (timeout) {
				const unsigned timeout_ms = timeout->msec();
				bytes_sent = _session->sendf(_buffer, _primitive_data_size, &timeout_ms, bytes_transferred, MSG_NOSIGNAL);
			} else bytes_sent = _session->sendf(_buffer, _primitive_data_size, 0, bytes_transferred, MSG_NOSIGNAL);
		} else if (_sender) bytes_sent = _sender->sendf(_buffer, _primitive_data_size, MSG_NOSIGNAL, timeout, bytes_transferred);
		else {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: no sender object available (no _session and no _sender)");
			return acs_dsd::ERR_SENDER_NOT_AVAILABLE;
		}

		return bytes_sent;
	}

	/** @brief recv_primitive method
	 *
	 *	recv_primitive method detailed description
	 *
	 *	@param[in] timeout Description
	 *	@param[out] bytes_transferred
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ssize_t recv_primitive (const ACE_Time_Value * timeout = 0, size_t * bytes_transferred = 0) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

		_ready_to_send = false;

		errno = 0;

		ssize_t bytes_received = 0;
		size_t bytes_transferred_ = 0;

		if (_session) {
			if (timeout) {
				const unsigned timeout_ms = timeout->msec();
				bytes_received = _session->recv(_buffer, buf_size, &timeout_ms, &bytes_transferred_);
			} else bytes_received = _session->recv(_buffer, buf_size, 0, &bytes_transferred_);
		}
		else if (_receiver) bytes_received = _receiver->recv(_buffer, buf_size, timeout, &bytes_transferred_);
		else {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: no receiver object available (no _session and no _receiver)");
			return acs_dsd::ERR_RECEIVER_NOT_AVAILABLE;
		}

		bytes_transferred && (*bytes_transferred = bytes_transferred_);
		_primitive_data_size = (bytes_received >= 0) ? static_cast<size_t>(bytes_received) : bytes_transferred_;

		return bytes_received;
	}

	/** @brief bind method
	 *
	 *	bind method detailed description
	 *
	 *	@param[in] session Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void bind (const ACS_DSD_Session & session) { _session = &session; }

	/** @brief bind method
	 *
	 *	bind method detailed description
	 *
	 *	@param[in] sender Description
	 *	@param[in] receiver Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void bind (const ACS_DSD_MessageSender & sender, const ACS_DSD_MessageReceiver & receiver) { _sender = &sender; _receiver = &receiver; }
	inline void bind (const ACS_DSD_OCPMessageSender & sender, const ACS_DSD_OCPMessageReceiver & receiver) { _sender = &sender; _receiver = &receiver; }

	/** @brief unbind method
	 *
	 *	unbind method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void unbind () { _session = 0; _sender = 0; _receiver = 0; }

	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	uint8_t _buffer[buf_size];
	size_t _primitive_data_size;
	//ssize_t _bytes_received;
	mutable bool _ready_to_send;
	const ACS_DSD_Sender * _sender;
	const ACS_DSD_Receiver * _receiver;
	const ACS_DSD_Session * _session;
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_PrimitiveDataHandler
