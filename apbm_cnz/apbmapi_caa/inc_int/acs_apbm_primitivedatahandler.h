#ifndef HEADER_GUARD_CLASS__acs_apbm_primitivedatahandler
#define HEADER_GUARD_CLASS__acs_apbm_primitivedatahandler acs_apbm_primitivedatahandler

/** @file acs_apbm_primitivedatahandler.h
 *	@brief
 *	@author xgiufer (Giuliano Ferraioli)
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
 *	| 		 | 2011-09-20 | xgiufer      | Released for sprint 16               |
 *	+--------+------------+--------------+-------------------------------------+
 */

#include <cstring>
#include <cstdarg>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ace/Time_Value.h>
#include <acs_apbm_types.h>
#include "acs_apbm_macros.h"
#include <ACS_DSD_Session.h>
#include <syslog.h>
//#include "ACS_DSD_Trace.h" //CREARE MACRO PER LOGGING
//#include "ACS_DSD_TraTracer.h"


namespace acs_apbm {

	enum PCP_PimitiveIdConstants {
		PCP_PRIMITIVE_ID_UNKNOWN		=	-1,	///< PCP_PRIMITIVE_ID_UNKNOWN description

		PCP_PRIMITIVE_ID_MIN				= 0,	///< PCP_PRIMITIVE_ID_MIN description

		PCP_SUBSCRIBE_REQUEST_ID			= 0,
		PCP_UNSUBSCRIBE_REQUEST_ID			= 1,
		PCP_NOTIFICATION_TRAP_MSG_ID			= 2,
		PCP_GET_TRAP_REQUEST_ID				= 3,
		PCP_GET_SEL_TRAP_REPLY_ID			= 4,
		PCP_GET_BOARD_PRESENCE_TRAP_REPLY_ID		= 5,
		PCP_GET_SENSOR_STATE_CHANGE_TRAP_REPLY_ID	= 6,
		PCP_SWITCH_BOARD_DATA_REQUEST_ID		= 7,
		PCP_GET_SCB_DATA_REPLY_ID			= 8,
		PCP_GET_EGEM2L2_SWITCH_DATA_REPLY_ID			= 9,
		PCP_GET_COLD_RESTART_TRAP_REPLY_ID		= 10,
		PCP_GET_WARM_RESTART_TRAP_REPLY_ID		= 11,
		PCP_GET_NIC_MONITOR_NOTIFY_REPLY_ID		= 12,
		PCP_GET_BIOS_VERSION_REQUEST_ID         = 13,
		PCP_GET_BIOS_VERSION_REPLY_ID           = 14,
		PCP_GET_THUMBDRIVE_STATUS_REQUEST_ID          = 15,
		PCP_GET_THUMBDRIVE_STATUS_REPLY_ID            = 16,
		PCP_GET_OWN_SLOT_REQUEST_ID             = 17,
		PCP_GET_OWN_SLOT_REPLY_ID               = 18,
		PCP_BOARD_PRESENCE_REQUEST_ID           = 19,
		PCP_BOARD_PRESENCE_REPLY_ID             = 20,
		PCP_BOARD_LOCATION_REQUEST_ID           = 21,
		PCP_BOARD_LOCATION_REPLY_ID             = 22,
		PCP_BOARD_STATUS_REQUEST_ID             = 23,
		PCP_BOARD_STATUS_REPLY_ID               = 24,
		PCP_SET_BOARD_STATUS_REQUEST_ID         = 25,
		PCP_SET_BOARD_STATUS_REPLY_ID           = 26,
		PCP_GET_IPMIUPG_STATUS_REQUEST_ID       = 27,
		PCP_GET_IPMIUPG_STATUS_REPLY_ID         = 28,
		PCP_IPMIUPG_UPGRADE_REQUEST_ID      	= 29,
		PCP_IPMIUPG_UPGRADE_REPLY_ID       	= 30,
		PCP_GET_IPMIFW_DATA_REQUEST_ID      	= 31,
		PCP_GET_IPMIFW_DATA_REPLY_ID       	= 32,
		PCP_GET_DISK_STATUS_REQUEST_ID	        = 33,
		PCP_GET_DISK_STATUS_REPLY_ID	        = 34,
		PCP_SET_NIC_INFO_REQUEST_ID             = 35,
		PCP_SET_NIC_INFO_REPLY_ID               = 36,
		
		PCP_PRIMITIVE_ID_MAX,
		PCP_PRIMITIVES_RANGE_COUNT	= PCP_PRIMITIVE_ID_MAX - PCP_PRIMITIVE_ID_MIN
	};

	enum PCP_GenericConstants {
		PCP_SUPPORTED_VERSION_COUNT	=	1,
		PCP_FIELD_COUNT_MIN					=	2,
		PCP_FIELD_COUNT_MAX					=	10 	// TODO: verificare da quanti campi e` composta la primitiva piu` grande
	};



	enum PCP_FieldSizeConstants {
		PCP_FIELD_SIZE_PRIMITIVE_ID	        = 1,
		PCP_FIELD_SIZE_VERSION	                = 1,
		PCP_FIELD_SIZE_PROT_RETURN_CODE         = 1,
		PCP_FIELD_SIZE_NOTIFY_NUMBER            = 2,
		PCP_FIELD_SIZE_TRAP_MSG                 = ACS_APBM_TRAP_MSG_MAX_SIZE,
		PCP_FIELD_SIZE_PROT_ERROR_CODE          = 4,
		PCP_FIELD_SIZE_SLOT_BITMAP	        = 4,
		PCP_FIELD_SIZE_MAGAZINE                 = 4,

		PCP_FIELD_SIZE_MAG_PLUG_NUMBER          = 4,   //SEL trap
		PCP_FIELD_SIZE_SLOT_POS                 = 4,

		PCP_FIELD_SIZE_BOARD_NAME		= 1,
		PCP_FIELD_SIZE_BOARD_PRESENCE 	        = 1,
		PCP_FIELD_SIZE_BOARD_STATUS 	        = 1,
		PCP_FIELD_SIZE_AP_SYS_NO		= 4,

		PCP_FIELD_SIZE_HW_BOARD_PRESENCE        = 4, //Board presence trap
		PCP_FIELD_SIZE_BUS_TYPE                 = 4,

		PCP_FIELD_SIZE_SENSOR_TYPE              = 4,		//Sensor trap
		PCP_FIELD_SIZE_SENSOR_ID                = 4,
		PCP_FIELD_SIZE_SENSOR_TYPE_CODE         = 4,

		//PCP_FIELD_SIZE_SLOT_POS               // already defined
		PCP_FIELD_SIZE_IPV4_ADDRESS             = 16,	//Command hwmscbls\hwmxls
		PCP_FIELD_SIZE_MASTER_STATUS            = 4,
		PCP_FIELD_SIZE_NEIGHBOUR_STATUS         = 4,
		PCP_FIELD_SIZE_FBN                      = 4,
		PCP_FIELD_SIZE_ARCHITECTURE_TYPE        = 4,

		PCP_FIELD_SIZE_BIOS_PRODUCT_NAME        = 16,  //get bios version
		PCP_FIELD_SIZE_BIOS_PRODUCT_VERSION     = 16,

		PCP_FIELD_SIZE_DISK_CONNECTED           = 1,

		PCP_FIELD_SIZE_NIC_STATUS               = 4,
		PCP_FIELD_SIZE_NIC_NAME                 = 16,
		PCP_FIELD_SIZE_NIC_MAC_ADDRESS          = 32,
		PCP_FIELD_SIZE_NIC_IPV4_ADDRESS         =16,

		PCP_FIELD_SIZE_IPMIFW_STATUS = 1,	//Command ipmiupgexec
		PCP_FIELD_SIZE_IPMIFW_DATETIME = ACS_APBM_DATA_TIME_SIZE,

		PCP_FIELD_SIZE_IPMIFW_COMPORT = 1,
		PCP_FIELD_SIZE_IPMIFW_PKGNAME = ACS_APBM_IPMIPKG_NAME_MAX_SIZE,

		PCP_FIELD_SIZE_IPMIFW_TYPE = 1,   //Command ipmifwprint
		PCP_FIELD_SIZE_IPMIFW_PRODUCT_NUMBER = 25,
		PCP_FIELD_SIZE_IPMIFW_REVISION = 8
		};

	enum PCP_InternalErrorCodes {
			PCP_ERROR_NO_ERROR 					= 0,
			PCP_ERROR_CODE_GENERIC_FAILURE 		= 1,
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

extern const uint8_t ACS_APBM_PCP_PRIMITIVE_FIELDS_COUNTS[acs_apbm::PCP_SUPPORTED_VERSION_COUNT][acs_apbm::PCP_PRIMITIVES_RANGE_COUNT];
extern const uint16_t ACS_APBM_PCP_PRIMITIVE_FIELDS_SIZES[acs_apbm::PCP_SUPPORTED_VERSION_COUNT][acs_apbm::PCP_PRIMITIVES_RANGE_COUNT][acs_apbm::PCP_FIELD_COUNT_MAX];
extern const copy_caller ACS_APBM_PCP_PRIMITIVE_FIELDS_COPIER_CALLERS[acs_apbm::PCP_SUPPORTED_VERSION_COUNT][acs_apbm::PCP_PRIMITIVES_RANGE_COUNT][acs_apbm::PCP_FIELD_COUNT_MAX];

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_primitivedatahandler

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class acs_apbm_primitivedatahandler acs_apbm_primitivedatahandler.h
 *	@brief acs_apbm_primitivedatahandler class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-07-14
 *	@version 1.0.0
 *
 *	acs_apbm_primitivedatahandler Class detailed description
 */
template <size_t buf_size =1024>
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_primitivedatahandler Default constructor
	 *
	 *	acs_apbm_primitivedatahandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ ()
	: _buffer(), _primitive_data_size(0), _ready_to_send(false), _session(0) {
		//ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));
		_buffer[0] = acs_apbm::PCP_PRIMITIVE_ID_UNKNOWN;
	}

	/** @brief acs_apbm_primitivedatahandler Default constructor
	 *
	 *	acs_apbm_primitivedatahandler Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline explicit __CLASS_NAME__ (const ACS_DSD_Session & session)
	: _buffer(), _primitive_data_size(0), _ready_to_send(false), _session(&session) {
		//ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));
		_buffer[0] = acs_apbm::PCP_PRIMITIVE_ID_UNKNOWN;
	}

private:
	/** @brief acs_apbm_primitivedatahandler Copy constructor
	 *
	 *	acs_apbm_primitivedatahandler Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_primitivedatahandler Destructor
	 *
	 *	acs_apbm_primitivedatahandler Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () { /*ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));*/ }

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
		//ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));

		if ((primitive_version < 1) || (acs_apbm::PCP_SUPPORTED_VERSION_COUNT < primitive_version)) {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//					"ERR_ASSERTION((primitive_version < 1) || (acs_apbm::PCP_SUPPORTED_VERSION_COUNT < primitive_version)): Primitive version not supported: primitive_version == %u",
//					primitive_version);
			return acs_apbm::WAR_PCP_VERSION_NOT_SUPPORTED;
		}

		if ((primitive_id < acs_apbm::PCP_PRIMITIVE_ID_MIN) || (acs_apbm::PCP_PRIMITIVE_ID_MAX <= primitive_id)) {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//					"ERR_ASSERTION((primitive_id < acs_apbm::PCP_PRIMITIVE_ID_MIN) || (acs_apbm::PCP_PRIMITIVE_ID_MAX <= primitive_id)): Primitive undefined: primitive ID == %d",
//					primitive_id);
			return acs_apbm::WAR_PCP_PRIMITIVE_UNDEFINED;
		}

		size_t fields_count = 0;

		if (!(fields_count = ACS_APBM_PCP_PRIMITIVE_FIELDS_COUNTS[--primitive_version][primitive_id])) { //ERROR: primitive version not supported
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//					"ERR_ASSERTION(!(fields_count = ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[--primitive_version][primitive_id])): Primitive version not supported: primitive_version == %u",
//					primitive_version + 1);
			return acs_apbm::WAR_PCP_VERSION_NOT_SUPPORTED;
		}

		size_in_bytes = 0;

		for (size_t i = 0; i < fields_count; size_in_bytes += ACS_APBM_PCP_PRIMITIVE_FIELDS_SIZES[primitive_version][primitive_id][i++]) ;

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
		int prim_id = ({ typeof(_buffer[0]) _unknown = acs_apbm::PCP_PRIMITIVE_ID_UNKNOWN; _buffer[0] ^ _unknown; })
								? static_cast<int>(_buffer[0])
								: acs_apbm::PCP_PRIMITIVE_ID_UNKNOWN;

		return prim_id;
	}

	/** @brief primitive_version method
	 *
	 *	primitive_version method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline unsigned primitive_version () const { return static_cast<unsigned>(_buffer[1]); }

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
//		syslog(LOG_INFO,"PRIMITIVE_ID %d - PRIMITVE_VERSION %d\n",primitive_id,primitive_version);
	//	ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));

//		ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "USER_PARAMETER[primitive_id == %d]", primitive_id);
//		ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "USER_PARAMETER[primitive_version == %u]", primitive_version);

		_primitive_data_size = 0;
		if (primitive_version > acs_apbm::PCP_SUPPORTED_VERSION_COUNT) {
			return acs_apbm::ERR_PCP_VERSION_NOT_SUPPORTED;
		}

		if ((primitive_id < acs_apbm::PCP_PRIMITIVE_ID_MIN) || (acs_apbm::PCP_PRIMITIVE_ID_MAX <= primitive_id)) {
			return acs_apbm::ERR_PCP_PRIMITIVE_UNDEFINED;
		}

		unsigned prim_index = (primitive_version ? (primitive_version - 1) : 0);

		if (!ACS_APBM_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][primitive_id]) { //ERROR: version not supported
			return acs_apbm::ERR_PCP_VERSION_NOT_SUPPORTED;
		}

		memset(_buffer, 0, buf_size);

		size_t buf_index = 0;
		const uint16_t * field_size_ptr = ACS_APBM_PCP_PRIMITIVE_FIELDS_SIZES[prim_index][primitive_id];
		const int fields_count = ACS_APBM_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][primitive_id];
		int field_index = 0;

		// Insert the primitive ID after check
		if (buf_index >= buf_size) return acs_apbm::ERR_PCP_DATA_BUFFER_OVERFLOW;
		_buffer[buf_index] = static_cast<uint8_t >(primitive_id);
		buf_index += *field_size_ptr++;
		++field_index;

		// Insert the version element after check
		if (buf_index >= buf_size) {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//					"ERR_ASSERTION(buf_index >= buf_size): Data buffer overflow: buf_index == %zu, buf_size == %zu",
//					buf_index, buf_size);
			return acs_apbm::ERR_PCP_DATA_BUFFER_OVERFLOW;
		}
		_buffer[buf_index] = static_cast<uint8_t >(primitive_version);
		buf_index += *field_size_ptr++;
		++field_index;

		const copy_caller * copy_caller_ptr = ACS_APBM_PCP_PRIMITIVE_FIELDS_COPIER_CALLERS[prim_index][primitive_id] + field_index;

		va_list argp;
		va_start(argp, primitive_version);

		int result_code = 0;

		for ( ; field_index < fields_count; ++field_index, buf_index += *field_size_ptr++)
			if ((buf_index + *field_size_ptr) > buf_size) {
//				ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//						"ERR_ASSERTION((buf_index + *field_size_ptr) > buf_size): Data buffer overflow: buf_index == %zu, *field_size_ptr == %u, buf_size == %zu",
//						buf_index, *field_size_ptr, buf_size);
				result_code = acs_apbm::ERR_PCP_DATA_BUFFER_OVERFLOW;
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
		//ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));

		_ready_to_send = false;

		unsigned prim_version_ = primitive_version();

//		ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "prim_version_ == %u", prim_version_);

		if (prim_version_ > acs_apbm::PCP_SUPPORTED_VERSION_COUNT) {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//					"ERR_ASSERTION(prim_version_ > acs_apbm::PCP_SUPPORTED_VERSION_COUNT): Bad version in primitive data: prim_version_ == %u",
//					prim_version_);
			return acs_apbm::ERR_PCP_BAD_VERSION_IN_PRIMITIVE_DATA;
		}

		const int prim_id_ = primitive_id();
//		ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "prim_id_ == %d", prim_id_);
		if ((prim_id_ < acs_apbm::PCP_PRIMITIVE_ID_MIN) || (acs_apbm::PCP_PRIMITIVE_ID_MAX <= prim_id_)) {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//					"ERR_ASSERTION((prim_id_ < acs_apbm::PCP_PRIMITIVE_ID_MIN) || (acs_apbm::PCP_PRIMITIVE_ID_MAX <= prim_id_)): Bad primitive ID in primitive data: prim_id_ == %d",
//					prim_id_);
			return acs_apbm::ERR_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA;
		}

		unsigned prim_index = (prim_version_ ? prim_version_ - 1 : 0);
//		ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "prim_index == %u", prim_index);

		if (!ACS_APBM_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][prim_id_]) { //ERROR: version not supported
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__),
//					"ERR_ASSERTION(!ACS_DSD_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][prim_id_]): Primitive version not supported: "
//					"prim_version_ == %u, prim_index == %u, prim_id_ == %d",
//					prim_version_, prim_index, prim_id_);
			return acs_apbm::ERR_PCP_VERSION_NOT_SUPPORTED;
		}

		const int fields_count = ACS_APBM_PCP_PRIMITIVE_FIELDS_COUNTS[prim_index][prim_id_];
		const uint16_t * field_size_ptr = ACS_APBM_PCP_PRIMITIVE_FIELDS_SIZES[prim_index][prim_id_];
		size_t buf_index = *field_size_ptr++;
		int field_index = 1;

//		ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "fields_count == %d", fields_count);

		//Skip this field the primitive version also
		buf_index += *field_size_ptr++; ++field_index;

		//ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "Initializing copy_caller_ptr: field_index == %d", field_index);
		const copy_caller * copy_caller_ptr = ACS_APBM_PCP_PRIMITIVE_FIELDS_COPIER_CALLERS[prim_index][prim_id_] + field_index;
		//ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "copy_caller_ptr == 0x%p", copy_caller_ptr);

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
		//ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));

		errno = 0;

		if (!_ready_to_send) {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(!_ready_to_send): primitive data not ready to be send");
			return acs_apbm::ERR_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND;
		}

		ssize_t bytes_sent = 0;

		if (_session) {
			if (timeout) {
				const unsigned timeout_ms = timeout->msec();
				bytes_sent = _session->send(_buffer, _primitive_data_size, &timeout_ms, bytes_transferred);
			} else bytes_sent = _session->send(_buffer, _primitive_data_size, 0, bytes_transferred);
		} else {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: no sender object available (no _session and no _sender)");
			return acs_apbm::ERR_SENDER_NOT_AVAILABLE;
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
		//ACS_APBM_API_TRACE_FUNCTION(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__));

		_ready_to_send = false;

		errno = 0;

		ssize_t bytes_received = 0;
		size_t bytes_transferred_ = 0;

		if (_session) {
			if (timeout) {
				const unsigned timeout_ms = timeout->msec();
				bytes_received = _session->recv(_buffer, buf_size, &timeout_ms, &bytes_transferred_);
			} else bytes_received = _session->recv(_buffer, buf_size, 0, &bytes_transferred_);
		} else {
//			ACS_DSD_API_TRACE_MESSAGE(ACS_APBM_CLASS_TRACER_NAME(__CLASS_NAME__), "ERROR: no receiver object available (no _session and no _receiver)");
			return acs_apbm::ERR_RECEIVER_NOT_AVAILABLE;
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

	/** @brief unbind method
	 *
	 *	unbind method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void unbind () { _session = 0; }

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	uint8_t _buffer[buf_size];
	size_t _primitive_data_size;
	//ssize_t _bytes_received;
	mutable bool _ready_to_send;
	//const ACS_DSD_Sender * _sender;
	//const ACS_DSD_Receiver * _receiver;
	const ACS_DSD_Session * _session;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_primitivedatahandler
