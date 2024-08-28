#ifndef HEADER_GUARD_FILE__ACS_DSD_MacrosConstants
#define HEADER_GUARD_FILE__ACS_DSD_MacrosConstants ACS_DSD_MacrosConstants.h

/** @file ACS_DSD_MacrosConstants.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-06
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
 *	| P0.9.1 | 2010-10-06 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

namespace acs_dsd {
	/** @brief DSD API warning constants.
	 *
	 *	WarningConstants enumeration detailed description
	 */
	enum WarningConstants {
		WAR_NO_WARNINGS															=	0,	///< WAR_NO_WARNINGS description
		WAR_ACCEPTOR_NOT_CLOSED											=	1,	///< WAR_ACCEPTOR_NOT_CLOSED description
		WAR_ACCEPTOR_NOT_OPEN												=	2,	///< WAR_ACCEPTOR_NOT_OPEN description
		WAR_STREAM_ALREADY_IN_USE										=	3,	///< WAR_STREAM_ALREADY_IN_USE description
		WAR_WRONG_STREAM_TYPE												=	4,	///< WAR_WRONG_STREAM_TYPE description
		WAR_WRONG_ADDRESS_TYPE											=	5,	///< WAR_WRONG_ADDRESS_TYPE description
		WAR_REMOVE_NOT_ALLOWED											=	6,	///< WAR_REMOVE_NOT_ALLOWED description
		WAR_IP4_ADDRESS_DOTTED_NOT_VALID						=	7,	///< WAR_IP4_ADDRESS_DOTTED_NOT_VALID description
		WAR_INTERNAL_NET_CONFIG_NOT_LOADED					=	8,	///< WAR_INTERNAL_NET_CONFIG_NOT_LOADED description
		WAR_SERVICE_MODE_UNKNOWN										= 9,	///< WAR_SERVICE_MODE_UNKNOWN description
		WAR_SERVER_NOT_CLOSED												=	10,	///< WAR_SERVER_NOT_CLOSED description
		WAR_SERVICE_ACCESS_POINT_NOT_VALID					=	11,	///< WAR_SERVICE_ACCESS_POINT_NOT_VALID description
		WAR_SERVICE_ACCESS_POINT_OUT_OF_RANGE				=	12,	///< WAR_SERVICE_ACCESS_POINT_OUT_OF_RANGE description
		WAR_SOME_NETWORKS_UNAVAILABLE								=	13,	///< WAR_SOME_NETWORKS_UNAVAILABLE description
		WAR_SOME_INTERNAL_ACCEPTORS_NOT_OPEN				= 14,	///< WAR_SOME_INTERNAL_ACCEPTORS_NOT_OPEN description
		WAR_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE	=	15,	///< WAR_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE description
		WAR_SERVER_NOT_OPEN													=	16,	///< WAR_SERVER_NOT_OPEN description
		WAR_SESSION_ALREADY_IN_USE									=	17,	///< WAR_SESSION_ALREADY_IN_USE description
		WAR_TIMEOUT_EXPIRED													=	18,	///< WAR_TIMEOUT_EXPIRED description
		WAR_NODE_NOT_FOUND													=	19,	///< WAR_NODE_NOT_FOUND description

		WAR_PCP_PRIMITIVE_UNDEFINED						=	20,	///< WAR_PCP_PRIMITIVE_UNDEFINED description
		WAR_PCP_VERSION_NOT_SUPPORTED					=	21,	///< WAR_PCP_VERSION_NOT_SUPPORTED description
		WAR_PCP_DATA_BUFFER_OVERFLOW					=	22,	///< WAR_PCP_DATA_BUFFER_OVERFLOW description

		WAR_SYSTEM_CLOSE	=	25	///< WAR_SYSTEM_CLOSE description
	};

	/** @brief DSD API error constants.
	 *
	 *	ErrorConstants enumeration detailed description
	 */
	enum ErrorConstants {
		ERR_NO_ERRORS														=	0,		///< ERR_NO_ERRORS description (check errno)
		ERR_SYSTEM_ERROR												=	-1,		///< ERR_SYSTEM_ERROR description (check errno)
		ERR_SYSTEM_OPEN													=	-2,		///< ERR_SYSTEM_OPEN description (check errno)
		ERR_SYSTEM_ACCEPT												=	-3,		///< ERR_SYSTEM_ACCEPT description (check errno)
		ERR_SYSTEM_CLOSE												=	-4,		///< ERR_SYSTEM_CLOSE description (check errno)
		ERR_SYSTEM_REMOVE												=	-5,		///< ERR_SYSTEM_REMOVE description (check errno)
		ERR_SYSTEM_GET_ADDRESS									=	-6,		///< ERR_SYSTEM_GET_ADDRESS description (check errno)
		ERR_SYSTEM_CONNECT											=	-7,		///< ERR_SYSTEM_CONNECT description (check errno)
		ERR_SYSTEM_RECEIVE											=	-8,		///< ERR_SYSTEM_RECEIVE description (check errno)
		ERR_SYSTEM_SEND													=	-9,		///< ERR_SYSTEM_SEND description (check errno)
		ERR_SYSTEM_GET_OPTION										=	-10,	///< ERR_SYSTEM_GET_OPTION description (check errno)
		ERR_SYSTEM_SET_OPTION										=	-11,	///< ERR_SYSTEM_SET_OPTION description (check errno)
		ERR_SYSTEM_OPEN_DIRECTORY								=	-12,	///< ERR_SYSTEM_OPEN_DIRECTORY description (check errno)
		ERR_SYSTEM_OUT_OF_MEMORY								=	-13,	///< ERR_SYSTEM_OUT_OF_MEMORY description (check errno)
		ERR_SYSTEM_READ													=	-14,	///< ERR_SYSTEM_READ description (check errno)
		ERR_SYSTEM_CREATE												=	-15,	///< ERR_SYSTEM_CREATE description (check errno)
		ERR_SYSTEM_EPOLL_CTL_ADD								=	-16,	///< ERR_SYSTEM_EPOLL_CTL_ADD description (check errno)
		ERR_SYSTEM_IO_MULTIPLEXING_ACTIVATE			=	-17,	///< ERR_SYSTEM_IO_MULTIPLEXING_ACTIVATE description (check errno)
		ERR_SYSTEM_EPOLL_WAIT										=	-18,	///< ERR_SYSTEM_EPOLL_WAIT description (check errno)
		ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE		=	-19,	///< ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE description (check errno)

		ERR_SERVICE_ACCESS_POINT_NOT_VALID					=	-20,	///< ERR_SERVICE_ACCESS_POINT_NOT_VALID description
		ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE				=	-21,	///< ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE description
		ERR_SERVICE_MODE_UNKNOWN										=	-22,	///< ERR_SERVICE_MODE_UNKNOWN description
		ERR_SERVER_NOT_CLOSED												= -23,	///< ERR_SERVER_NOT_CLOSED description
		ERR_SERVER_NOT_OPEN													=	-24,	///< ERR_SERVER_NOT_OPEN description
		ERR_SESSION_ALREADY_IN_USE									=	-25,	///< ERR_SESSION_ALREADY_IN_USE description
		ERR_HOST_CONFIG_LOAD												=	-26,	///< ERR_HOST_CONFIG_LOAD description
		ERR_GET_PROCESS_INFORMATION									=	-27,	///< ERR_GET_PROCESS_INFORMATION description
		ERR_WRONG_ACCEPTOR_TYPE											=	-28,	///< ERR_WRONG_ACCEPTOR_TYPE description
		ERR_LOAD_AP_NODES_FROM_CS										=	-29,	///< ERR_LOAD_AP_NODES_FROM_CS description
		ERR_FIND_MY_AP_NODE_HOST_INFO								=	-30,	///< ERR_FIND_MY_AP_NODE_HOST_INFO description
		ERR_AP_CONFIGURATION_MISMATCH_FROM_CS				=	-31,	///< ERR_CONFIGURATION_MISMATCH_FROM_CS description
		ERR_CONFIG_NODE_IP_CONFLICT									=	-32,	///< ERR_CONFIG_NODE_IP_CONFLICT description
		ERR_REMOTE_SYSTEM_TYPE_UNKNOWN							=	-33,	///< ERR_REMOTE_SYSTEM_TYPE_UNKNOWN description
		ERR_WRONG_STREAM_TYPE												=	-34,	///< ERR_WRONG_STREAM_TYPE description
		ERR_ACCEPTOR_NOT_OPEN												=	-35,	///< ERR_ACCEPTOR_NOT_OPEN description
		ERR_STREAM_ALREADY_IN_USE										=	-36,	///< ERR_STREAM_ALREADY_IN_USE description
		ERR_NODE_NOT_FOUND													=	-37,	///< ERR_NODE_NOT_FOUND description
		ERR_SERVICE_MODE_IS_PRIVATE									=	-38,	///< ERR_SERVICE_MODE_IS_PRIVATE description
		ERR_PARAMETER_EMPTY_OR_NULL									=	-39,	///< ERR_PARAMETER_EMPTY_OR_NULL description
		ERR_SERVICE_PUBLISH													=	-40,	///< ERR_SERVICE_PUBLISH description
		ERR_NOT_ENOUGH_SPACE												=	-41,	///< ERR_NOT_ENOUGH_SPACE description
		ERR_ACCEPTORS_ADDRESSES_NOT_FOUND						=	-42,	///< ERR_ACCEPTORS_ADDRESSES_NOT_FOUND description
		ERR_PEER_CLOSED_CONNECTION									=	-43,	///< ERR_PEER_CLOSED_CONNECTION description
		ERR_OBJECT_CORRUPTED												=	-44,	///< ERR_OBJECT_CORRUPTED description
		ERR_SERVER_ALREADY_PUBLISHED								=	-45,	///< ERR_SERVER_ALREADY_PUBLISHED description
		ERR_SESSION_DISCONNECTED										=	-46,	///< ERR_SESSION_DISCONNECTED description
		ERR_BAD_PARAMETER_VALUE											=	-47,	///< ERR_BAD_PARAMETER_VALUE description
		ERR_GET_NODE_STATE													=	-48,	///< ERR_GET_NODE_STATE description
		ERR_CS_API_CALL_ERROR												=	-49,	///< ERR_CS_API_CALL_ERROR description
		ERR_FIND_MY_AP_PARTNER_NODE_HOST_INFO				=	-50,	///< ERR_FIND_MY_AP_PARTNER_NODE_HOST_INFO description
		ERR_SAF_IMM_OM_INIT													=	-51,	///< ERR_SAF_IMM_OM_INIT description
		ERR_SAF_IMM_OM_GET													=	-52,	///< ERR_SAF_IMM_OM_GET description
		ERR_SAF_IMM_NULL_ATTRIBUTE_VALUE						=	-53,	///< ERR_SAF_IMM_NULL_ATTRIBUTE_VALUE description
		ERR_BAD_NODE_STATE													=	-54,	///< ERR_BAD_NODE_STATE description
		ERR_BAD_SYSTEM_ID														=	-55,	///< ERR_BAD_SYSTEM_ID description
		ERR_ADDRESS_EMPTY_OR_NULL										=	-56,	///< ERR_ADDRESS_EMPTY_OR_NULL description
		ERR_OCP_SENDING_ACCEPT_INDICATION						=	-57,	///< ERR_OCP_SENDING_ACCEPT_INDICATION description
		ERR_OCP_RECEIVING_ACCEPT_CONFIRMATION				=	-58,	///< ERR_OCP_RECEIVING_ACCEPT_CONFIRMATION description
		ERR_OCP_ACCEPT_CONFIRM_BAD_RESPONSE					=	-59,	///< ERR_OCP_ACCEPT_CONFIRM_BAD_RESPONSE description
		ERR_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER	=	-60,	///< ERR_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER description
		ERR_SAF_IMM_BAD_ATTRIBUTE_VALUE							=	-61,	///< ERR_SAF_IMM_BAD_ATTRIBUTE_VALUE description
		ERR_NODE_NETWORK_CONFIGURATION							=	-62,	///< ERR_NODE_NETWORK_CONFIGURATION description
		ERR_INDEX_OUT_OF_RANGE											=	-63,	///< ERR_INDEX_OUT_OF_RANGE description
		ERR_SAF_IMM_EMPTY_ATTRIBUTE_VALUE						=	-64,	///< ERR_SAF_IMM_EMPTY_ATTRIBUTE_VALUE description

		ERR_CS_CREATE_HWC_INSTANCE									=	-65,	///< ERR_CS_CREATE_HWC_INSTANCE description
		ERR_CS_CREATE_BOARD_SEARCH_INSTANCE					=	-66,	///< ERR_CS_CREATE_BOARD_SEARCH_INSTANCE description
		ERR_CS_GET_BOARD_IDS												=	-67,	///< ERR_CS_GET_BOARD_IDS description
		ERR_CS_GET_SYSTEM_ID												=	-68,	///< ERR_CS_GET_SYSTEM_ID description
		ERR_CS_GET_SYSTEM_NUMBER										=	-69,	///< ERR_CS_GET_SYSTEM_NUMBER description
		ERR_CS_GET_SYSTEM_SEQUENCE									=	-70,	///< ERR_CS_GET_SYSTEM_NUMBER description
		ERR_CS_GET_SIDE															=	-71,	///< ERR_CS_GET_SIDE description
		ERR_CS_GET_IP_ETH_A													=	-72,	///< ERR_CS_GET_IP_ETH_A description
		ERR_CS_GET_IP_ETH_B													=	-73,	///< ERR_CS_GET_IP_ETH_B description
		ERR_CS_GET_ALIAS_ETH_A											=	-74,	///< ERR_CS_GET_ALIAS_ETH_A description
		ERR_CS_GET_ALIAS_ETH_B											=	-75,	///< ERR_CS_GET_ALIAS_ETH_B description
		ERR_CS_SET_ALARM_MASTER											=	-76,	///< ERR_CS_SET_ALARM_MASTER description
		ERR_CS_SET_CLOCK_MASTER											=	-77,	///< ERR_CS_SET_CLOCK_MASTER description

		ERR_PCP_PRIMITIVE_UNDEFINED								=	-78,	///< ERR_PCP_PRIMITIVE_UNDEFINED description
		ERR_PCP_VERSION_NOT_SUPPORTED							=	-79,	///< ERR_PCP_VERSION_NOT_SUPPORTED description
		ERR_PCP_DATA_BUFFER_OVERFLOW							=	-80,	///< ERR_PCP_DATA_BUFFER_OVERFLOW description
		ERR_PCP_BAD_VERSION_IN_PRIMITIVE_DATA			=	-81,	///< ERR_PCP_BAD_VERSION_IN_PRIMITIVE_DATA description
		ERR_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA		=	-82,	///< ERR_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA description
		ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED			=	-83,	///< ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED description
		ERR_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND	=	-84,	///< ERR_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND description

		ERR_CP_STATE_NOTIFIER_NOT_DETACHED				=	-85,	///< ERR_CP_STATE_NOTIFIER_NOT_DETACHED description
		ERR_SENDER_NOT_AVAILABLE									=	-86,	///< ERR_SENDER_NOT_AVAILABLE description
		ERR_RECEIVER_NOT_AVAILABLE								=	-87,	///< ERR_RECEIVER_NOT_AVAILABLE description
		ERR_CP_STATE_NOTIFIER_NOT_ATTACHED				=	-88,	///< ERR_CP_STATE_NOTIFIER_NOT_ATTACHED description
		ERR_OCP_RECEIVED_HEADER_SIZES_MISMATCH		=	-89,	///< ERR_OCP_RECEIVED_HEADER_SIZES_MISMATCH description

		ERR_PCP_UNEXPECTED_ERROR_CODE												=	-90,	///< ERR_PCP_UNEXPECTED_ERROR_CODE description

		ERR_PCP_ERROR_CODE_START_CODE_BASE									=	-90,	///< ERR_PCP_ERROR_CODE_START_CODE_BASE description
		ERR_PCP_ERROR_CODE_NODE_NOT_VALID										=	-91,	///< PCP_ERROR_CODE_NODE_NOT_VALID description
		ERR_PCP_ERROR_CODE_FILE_STORAGE_FAILURE							=	-92,	///< PCP_ERROR_CODE_FILE_STORAGE_FAILURE description
		ERR_PCP_ERROR_CODE_ADDRESS_NOT_VALID								=	-93,	///< PCP_ERROR_CODE_ADDRESS_NOT_VALID description
		ERR_PCP_ERROR_CODE_APPLICATION_NOT_VALID						=	-94,	///< PCP_ERROR_CODE_APPLICATION_NOT_VALID description
		ERR_PCP_ERROR_CODE_NAME_NOT_UNIQUE									=	-95,	///< PCP_ERROR_CODE_NAME_NOT_UNIQUE description
		ERR_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED				=	-96,	///< ERR_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED description
		ERR_PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED						=	-97,	///< PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED description
		ERR_PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED		=	-98,	///< PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED description
		ERR_PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE					=	-99,	///< PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE description
		ERR_PCP_ERROR_CODE_NO_PORT_AVAILABLE								=	-100,	///< PCP_ERROR_CODE_NO_PORT_AVAILABLE description
		ERR_PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE					=	-101,	///< PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE description
		ERR_PCP_ERROR_CODE_REGISTER_FAILURE									=	-102,	///< ERR_PCP_ERROR_CODE_REGISTER_FAILURE description
		ERR_PCP_ERROR_CODE_UNREGISTER_FAILURE								=	-103,	///< PCP_ERROR_CODE_UNREGISTER_FAILURE description
		ERR_PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH	=	-104,	///< PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH description
		ERR_PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT						=	-105,	///< PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT description
		ERR_PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED		=	-106,	///< PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED description
		ERR_PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE						=	-107,	///< PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE description
		ERR_PCP_ERROR_CODE_HOST_UNREACHABLE									=	-108,	///< PCP_ERROR_CODE_HOST_UNREACHABLE description
		ERR_PCP_ERROR_CODE_OPERATION_NOT_ALLOWED						=	-109,	///< ERR_PCP_ERROR_CODE_OPERATION_NOT_ALLOWED description

		ERR_SYSTEM_POLL																			=	-115,	///< ERR_SYSTEM_POLL description (check errno)
		ERR_OCP_BUFFER_SIZE_EXCEEDS_MAXIMUM_ALLOWED					=	-116	///< ERR_OCP_BUFFER_SIZE_EXCEEDS_MAXIMUM_ALLOWED description
	};

	/** @brief DSD API acceptors state constants.
	 *
	 *	AcceptorStateConstants enumeration detailed description
	 */
	enum AcceptorStateConstants {
		ACCEPTOR_STATE_CLOSED	=	0x0,	///< ACCEPTOR_STATE_CLOSED description
		ACCEPTOR_STATE_OPEN		=	0x1		///< ACCEPTOR_STATE_OPEN description
	};

	/** @brief DSD API I/O streams state constants.
	 *
	 *	IOStreamStateConstants enumeration detailed description
	 */
	enum IOStreamStateConstants {
		STREAM_STATE_DISCONNECTED						=	0x0,	///< STREAM_STATE_DISCONNECTED description
		STREAM_STATE_CONNECTED_READER_ONLY	=	0x1,	///< STREAM_STATE_CONNECTED_READER_ONLY description
		STREAM_STATE_CONNECTED_WRITER_ONLY	=	0x2,	///< STREAM_STATE_CONNECTED_WRITER_ONLY description
		STREAM_STATE_CONNECTED							=	0x3		///< STREAM_STATE_CONNECTED description
	};

	/** @brief DSD API service modes constants.
	 *
	 *	ServiceModeConstants enumeration detailed description
	 */
	enum ServiceModeConstants {
		SERVICE_MODE_UNKNOWN							=	0x0,	///< SERVICE_MODE_UNKNOWN description
		SERVICE_MODE_INET_SOCKET					=	0x2,	///< SERVICE_MODE_INET_SOCKET description
		SERVICE_MODE_INET_SOCKET_PRIVATE	=	0x3,	///< SERVICE_MODE_INET_SOCKET_PRIVATE description
		SERVICE_MODE_UNIX_SOCKET					=	0x4,	///< SERVICE_MODE_UNIX_SOCKET description
		SERVICE_MODE_UNIX_SOCKET_PRIVATE	=	0x5		///< SERVICE_MODE_UNIX_SOCKET_PRIVATE description
	};

	/** @brief DSD API server state constants.
	 *
	 *	ServerStateConstants enumeration detailed description
	 */
	enum ServerStateConstants {
		SERVER_STATE_CLOSED			=	ACCEPTOR_STATE_CLOSED,	///< SERVER_STATE_CLOSED description
		SERVER_STATE_OPEN				=	ACCEPTOR_STATE_OPEN,		///< SERVER_STATE_OPEN description
		SERVER_STATE_PUBLISHED														///< SERVER_STATE_PUBLISHED description
	};

	/** @brief DSD API session state constants.
	 *
	 *	SessionStateConstants enumeration detailed description
	 */
	enum SessionStateConstants {
		SESSION_STATE_DISCONNECTED					=	STREAM_STATE_DISCONNECTED,					///< SESSION_STATE_DISCONNECTED description
		SESSION_STATE_CONNECTED_READER_ONLY	=	STREAM_STATE_CONNECTED_READER_ONLY,	///< SESSION_STATE_CONNECTED_READER_ONLY description
		SESSION_STATE_CONNECTED_WRITER_ONLY	=	STREAM_STATE_CONNECTED_WRITER_ONLY,	///< SESSION_STATE_CONNECTED_WRITER_ONLY description
		SESSION_STATE_CONNECTED							=	STREAM_STATE_CONNECTED							///< SESSION_STATE_CONNECTED description
	};

	/** @brief DSD API CP notifier state constants.
	 *
	 *	CPNotifierStateConstants enumeration detailed description
	 */
	enum CPNotifierStateConstants {
		CP_NOTIFIER_STATE_DETACHED	=	0x00,	///< CP_NOTIFIER_STATE_DETACHED description
		CP_NOTIFIER_STATE_ATTACHED	=	0x01	///< CP_NOTIFIER_STATE_ATTACHED description
	};

	/** @brief DSD API node state constants.
	 *
	 *	NodeStateConstants enumeration detailed description
	 */
	enum NodeStateConstants {
		NODE_STATE_ACTIVE			=	0,	///< NODE_STATE_ACTIVE description
		NODE_STATE_EXECUTION	= 0,	///< NODE_STATE_EXECUTION description
		NODE_STATE_PASSIVE		=	1,	///< NODE_STATE_PASSIVE description
		NODE_STATE_STANDBY		=	1,	///< NODE_STATE_STANDBY description
		NODE_STATE_UNDEFINED	=	2		///< NODE_STATE_UNDEFINED description
	};

	/** @brief DSD API node state constants.
	 *
	 *	NodeSideConstants enumeration detailed description
	 */
	enum NodeSideConstants {
		NODE_SIDE_UNDEFINED	=	-1,	///< NODE_SIDE_UNDEFINED description
		NODE_SIDE_A					=	0,	///< NODE_SIDE_A description
		NODE_SIDE_B					=	1		///< NODE_SIDE_B description
	};

	/** @brief DSD API system type constants.
	 *
	 *	SystemTypeConstants enumeration detailed description
	 */
	enum SystemTypeConstants {
		SYSTEM_TYPE_UNKNOWN	=	-1,	///< SYSTEM_TYPE_UNKNOWN description
		SYSTEM_TYPE_AP			=	0,	///< SYSTEM_TYPE_AP description
		SYSTEM_TYPE_CP			=	1,	///< SYSTEM_TYPE_CP description
		SYSTEM_TYPE_BC			=	2		///< SYSTEM_TYPE_BC description
	};

	/** @brief DSD API CP/BC state constants.
	 *
	 *	CPStateConstants enumeration detailed description
	 */
	enum CPStateConstants {
		CP_STATE_UNKNOWN		=	-1,	///< CP_STATE_UNKNOWN description
		CP_STATE_NORMAL			=	0,	///< CP_STATE_NORMAL description
		CP_STATE_SEPARATED	=	1,	///< CP_STATE_SEPARATED description
		CP_STATE_UNDEFINED	=	2		///< CP_STATE_UNDEFINED description
	};

	/** @brief DSD API system well known ID constants.
	 *
	 *	SystemIDConstants enumeration detailed description
	 */
	enum SystemIDConstants {
		SYSTEM_ID_UNKNOWN					=	-1,			///< SYSTEM_ID_UNKNOWN description
		SYSTEM_ID_THIS_NODE				=	50000,	///< SYSTEM_ID_THIS_NODE description
		SYSTEM_ID_PARTNER_NODE		=	50001,	///< SYSTEM_ID_PARTNER_NODE description
		SYSTEM_ID_FRONT_END_AP		=	50002,	///< SYSTEM_ID_FRONT_END_AP description
		SYSTEM_ID_CP_CLOCK_MASTER	=	50003,	///< SYSTEM_ID_CP_CLOCK_MASTER description
		SYSTEM_ID_CP_ALARM_MASTER	=	50004,	///< SYSTEM_ID_CP_ALARM_MASTER description
		SYSTEM_ID_ALL_AP_NODES		=	50005		///< SYSTEM_ID_ALL_AP_NODES description
	};

	/** @brief DSD API service visibility constants for published services.
	 *
	 *	ServiceVisibilityConstants enumeration detailed description
	 */
	enum ServiceVisibilityConstants {
		SERVICE_VISIBILITY_GLOBAL_SCOPE	=	0,	///< SERVICE_VISIBILITY_GLOBAL_SCOPE description
		SERVICE_VISIBILITY_LOCAL_SCOPE	=	1		///< SERVICE_VISIBILITY_LOCAL_SCOPE description
	};

	/** @brief DSD API generics constants.
	 *
	 *	GenericConstants enumeration detailed description
	 */
	enum GenericConstants {
		INVALID_HANDLE	=	-1	///< INVALID_HANDLE description
	};

	/** @brief Type of a connections.
	 *
	 *	ConnectionTypeConstants enumeration detailed description
	 */
	enum ConnectionTypeConstants {
		CONNECTION_TYPE_UNKNOWN	=	0x00,	///< CONNECTION_TYPE_UNKNOWN description
		CONNECTION_TYPE_OCP			=	0x01,	///< CONNECTION_TYPE_OCP description
		CONNECTION_TYPE_INET		=	0x02,	///< CONNECTION_TYPE_INET description
		CONNECTION_TYPE_UNIX		=	0x04	///< CONNECTION_TYPE_UNIX description
	};
}

extern const char * const ACS_DSD_AP_NODE_STATE_STR[];
extern const char * const ACS_DSD_CP_NODE_STATE_STR[];

#endif // HEADER_GUARD_FILE__ACS_DSD_MacrosConstants
