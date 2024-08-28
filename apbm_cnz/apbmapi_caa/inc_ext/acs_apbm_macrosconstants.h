/*
 * acs_apbm_macrosconstants.h
 *
 *  Created on: Sep 21, 2011
 *      Author: xgiufer
 */

#ifndef HEADER_GUARD_FILE__acs_apbm_macrosconstants
#define HEADER_GUARD_FILE__acs_apbm_macrosconstants acs_apbm_macrosconstants.h


namespace acs_apbm {
	/** @brief APBM warning constants.
	 *
	 *	WarningConstants enumeration detailed description
	 */

	typedef enum SwithcBoardStateConstants {
		SWITCH_BOARD_STATE_UNINTT_STATUS 	= -1,
		SWITCH_BOARD_STATE_PASSIVE  		=  0,
		SWITCH_BOARD_STATE_MASTER 			=  1,
		SWITCH_BOARD_STATE_IDLE 			=  2
	} state_t;

	typedef enum SwitchBoardNeighbourStateConstants{
		SWITCH_BOARD_NEIGHBOUR_STATE_UNINTT 	= -1,
		SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT		=  0,
		SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT	=  1
	} neighbour_state_t;

	typedef enum SwitchBoardFbnConstants{
		SWITCH_BOARD_FBN_UNKNOWN = -1,
		SWITCH_BOARD_FBN_SCB  = 0,
		SWITCH_BOARD_FBN_SCXB = 1,
		SWITCH_BOARD_FBN_SMXB = 2
	} fbn_t;

	typedef enum ArchitectureTypeConstats{
		ARCHITECTURE_TYPE_UNKNOWN 	= -1,
		ARCHITECTURE_TYPE_CBA 		=  0,
		ARCHITECTURE_TYPE_NOT_CBA 	=  1
	} architecture_type_t;

	//enumeration for PFM and IPMI
	enum IPMIUpgradeState {

		IPMIUpgradeState_OK			= 0,
		IPMIUpgradeState_NOT_OK		= 1,
		IPMIUpgradeState_ONGOING	= 2
	};

	enum IPMIUpgradeResult {

		IPMIUpgrade_OK = 0,
		IPMIUpgrade_NOT_OK = 1
	};

	typedef enum COMPortName{
        NO_COM = 0,
		COM2 = 1,
		COM3 = 2
	}comport_name_t;

	enum WarningConstants {
		WAR_NO_WARNINGS =	0,
		WAR_ACCEPTOR_NOT_CLOSED,
		WAR_ACCEPTOR_NOT_OPEN,
		WAR_STREAM_ALREADY_IN_USE,
		WAR_WRONG_STREAM_TYPE,
		WAR_WRONG_ADDRESS_TYPE,
		WAR_REMOVE_NOT_ALLOWED,
		WAR_IP4_ADDRESS_DOTTED_NOT_VALID,
		WAR_INTERNAL_NET_CONFIG_NOT_LOADED,
		WAR_SERVICE_MODE_UNKNOWN,
		WAR_SERVER_NOT_CLOSED,
		WAR_SERVICE_ACCESS_POINT_NOT_VALID,
		WAR_SERVICE_ACCESS_POINT_OUT_OF_RANGE,
		WAR_SOME_NETWORKS_UNAVAILABLE,
		WAR_SOME_INTERNAL_ACCEPTORS_NOT_OPEN,
		WAR_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE,
		WAR_SERVER_NOT_OPEN,
		WAR_SESSION_ALREADY_IN_USE,
		WAR_TIMEOUT_EXPIRED,
		WAR_NODE_NOT_FOUND,

		WAR_PCP_PRIMITIVE_UNDEFINED,
		WAR_PCP_VERSION_NOT_SUPPORTED,
		WAR_PCP_DATA_BUFFER_OVERFLOW,

		WAR_SYSTEM_CLOSE
	};


	/** @brief APBM error constants.
	 *
	 *	ErrorConstants enumeration detailed description
	 */

	enum PCP_ErrorCodesConstants {
		PCP_ERROR_CODE_SUCCESS 				= 0,
		//PCP_ERROR_CODE_GENERIC_FAILURE                = 1,
		PCP_ERROR_CODE_END_OF_DATA 			= 1,
		PCP_ERROR_CODE_INVALID_ARCHITECTURE = 2,
		PCP_ERROR_CODE_RESET_NIC_INFO       = 3,
		PCP_ERROR_CODE_DSD_REPLY_TIME_OUT       = 4
	};

//#define ACS_APBM_ERROR_CONSTANTS_COUNT 44
	enum ErrorConstants {
//		ERR_DEFINITION_START = -ACS_APBM_ERROR_CONSTANTS_COUNT - 1,

		// Start error value definitions

		ERR_ERROR_VALUES_START = -1000,

		// Add here new error codes and adjust the above ACS_APBM_ERROR_CONSTANTS_COUNT macro constant

		ERR_NTF_UNSUBSCRIPTION_FAILURE,
		ERR_NTF_SUBSCRIPTION_FAILURE,
		ERR_NTF_FILTER_ALLOCATION_FAILURE,
		ERR_NTF_FILTER_FREE_FAILURE,
		ERR_NTF_NOTIFIER_NOT_OPEN,
		ERR_NTF_FINALIZE_FAILURE,
		ERR_NTF_SELECTION_OBJECT_GET,
		ERR_NTF_INIT_FAILURE,
		ERR_IMM_CREATE_OBJ,
		ERR_IMM_MODIFY_OBJ, // 990
		ERR_IMM_DELETE_OBJ,
		ERR_IMM_SLOT_POSITION_NOT_FOUND,
		ERR_IMM_MAGAZINE_NOT_FOUND,
		ERR_IMM_ATTRIBUTE_NOT_FOUND,
		ERR_IMM_GET_ATTRIBUTE,
		ERR_IP_ADDRESS_CONVERSION,
		ERR_BAD_SLOT,
		ERR_GET_HANDLE_FAILED,
		ERR_INIT_CONNECTION_FAILED,
		ERR_INIT_UNIX_CONNECTION_FAILED, //980
		ERR_INIT_INET_CONNECTION_FAILED,
		ERR_SEND_PRIMITIVE_SUBSCRIBE,
		ERR_SEND_PRIMITIVE_UNSUBSCRIBE,
		ERR_SEND_PRIMITIVE_GET_TRAP,
		ERR_SEND_PRIMITIVE_SWITCH_BOARD_DATA,
		ERR_SEND_PRIMITIVE_GET_OWN_SLOT,
		ERR_RECEIVE_PRIMITIVE_NOTIFY,
		ERR_RECEIVE_PRIMITIVE_GET_TRAP,
		ERR_RECEIVE_PRIMITIVE_SWITCH_BOARD_DATA,
		ERR_RECEIVE_PRIMITIVE_GET_OWN_SLOT,  //970
		ERR_NO_TRAP_RECEIVED,
                ERR_TRAP_NOT_SCHEDULED,
		ERR_GET_MY_IPN_ADDRESSES,
		ERR_NODE_SIDE_UNDEFINED,
		ERR_DSD_GET_LOCAL_NODE,
		ERR_DSD_SESSION_STATE_DISCONNECTED,
		ERR_CS_GET_INSTANCE,
		ERR_CS_GET_SIDE,
		ERR_CS_GET_FBN,
		ERR_CS_GET_SLOT,   //960
		ERR_CS_GET_MAGAZINE,    
		ERR_CS_GET_IP_ETH,
		ERR_CS_API_BINDING,
		ERR_CS_TOO_MANY_BOARD_FOUND,
		ERR_CS_NO_BOARD_FOUND,
		ERR_CS_GET_BOARD_ID,
		ERR_CS_CREATE_BOARD_SEARCH_INSTANCE,
		ERR_CS_CREATE_HWC_INSTANCE,
		ERR_CS_ERROR,
		ERR_POLL_CALL,    //950
		ERR_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND,   
		ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED,
		ERR_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA,
		ERR_PCP_BAD_VERSION_IN_PRIMITIVE_DATA,
		ERR_PCP_DATA_BUFFER_OVERFLOW,
		ERR_PCP_VERSION_NOT_SUPPORTED,
		ERR_PCP_PRIMITIVE_UNDEFINED,
		ERR_SENDER_NOT_AVAILABLE,
		ERR_RECEIVER_NOT_AVAILABLE,
		ERR_CLOSE_CALL,          //940
		ERR_OPEN_CALL,							
		ERR_READ_CALL,
		ERR_REACTOR_HANDLER_REMOVE_FAILURE,
		ERR_OPERATION_SEND_FAILURE,
		ERR_REACTOR_HANDLER_REGISTER_FAILURE,
		ERR_REACTOR_SCHEDULE_TIMER_FAILURE,
		ERR_FCNTL_CALL,
		ERR_PIPE_CALL,
		ERR_RELEASING_IMM_OBJECTS_OWNERSHIP,
		ERR_TAKING_IMM_OBJECTS_OWNERSHIP,    //930
		ERR_OI_HANDLER_FINALIZE_FAILURE,	//929
		ERR_OI_HANDLER_INIT_FAILURE,
		ERR_CLOSING_IMM_MANAGEMENT_SESSION,
		ERR_OPENING_IMM_MANAGEMENT_SESSION,
		ERR_OM_HANDLER_FINALIZE_FAILURE,
		ERR_OM_HANDLER_INIT_FAILURE,
		ERR_GETTING_NODE_STATE,
		ERR_UNKNOWN_ARCHITECTURE,

		ERR_HWI_IMMCFG_FAILED,
		ERR_HWI_IMMRT_RETRY, //920
		ERR_HWI_IMMRT_FAILED,               //919
		ERR_HWI_IMMLISTENER_FAILED,
		// End error value definitions

		ERR_NULL_POINTER,
		ERR_MEMORY_BAD_ALLOC,
		ERR_CS_BOARD_NOT_FOUND,
		ERR_SNMP_BOARD_NOT_FOUND,
		ERR_GENERIC_ERROR,
		ERR_SEND_PRIMITIVE,
		ERR_RECEIVE_PRIMITIVE,
		ERR_IPMIUPG_INVALID_SLOT,   //910
		ERR_IPMIUPG_INVALID_MAGAZINE ,      //909
		ERR_IPMIUPG_INVALID_FILE_NAME,
		ERR_IPMIUPG_UPGRADE_ALREADY_ONGOING ,
		ERR_IPMIUPG_SNMP_ERROR,
		ERR_IPMIUPG_FUNC_BOARD_NOT_ALLOWED,

		ERR_DMX_GET_IPMI_FAILURE,
		ERR_DMX_GET_BOARD_DATA_FAILURE,
		ERR_DMX_GET_BOARD_LED_STATUS_FAILURE,
		ERR_DMX_GET_BOARD_BIOS_RUN_MODE_FAILURE,
		ERR_DMX_GET_BOARD_POWER_STATUS_FAILURE,   //900
		ERR_DMX_GET_BOARD_FATAL_EVENT_LOG_FAILURE, //899
		ERR_DMX_SET_BOARD_POWER_STATUS_FAILURE,
		ERR_DMX_RESET_BOARD_FAILURE,
		ERR_DMX_SET_SNMP_TARGET_FAILURE,
		ERR_DMX_GET_IPMI_UPG_STATUS_FAILURE,
		ERR_DMX_START_IPMI_UPG_FAILURE,
		ERR_DMX_GET_TRAPSUBSCR_ADDRESS_FAILURE,
		ERR_DMX_ARPING_TIMEOUT,
		ERR_DMX_ARPING_ERR,
		ERR_DMX_SET_BOARD_MIA_LED_FAILURE,   //890
		ERR_DMX_MAGAZINE_NOT_FOUND,                
		ERR_DMX_TENANT_NOT_FOUND,
		ERR_DMX_SET_BOARD_LED_STATUS_FAILURE,
		ERR_DMX_GET_BOARD_HWI_STATUS_FAILURE,

		ERR_CS_GET_SYSNO,
		ERR_NO_ERRORS = 0

		// New error declarations have to not be added here, but at the start of this list
	};
}



#endif /* HEADER_GUARD_FILE__acs_apbm_macrosconstants */
