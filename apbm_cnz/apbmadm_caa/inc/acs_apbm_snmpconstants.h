#ifndef HEADER_GUARD_FILE__acs_apbm_snmpconstants
#define HEADER_GUARD_FILE__acs_apbm_snmpconstants acs_apbm_snmpconstants.h

/** @file acs_apbm_snmpconstants.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-09-20
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
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2011-09-20 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include "acs_apbm_snmpmibdefinitions.h"


namespace acs_apbm_snmp {
	/** @brief APBM Server error constants.
	 *
	 *	ErrorConstants enumeration detailed description
	 */
//#define ACS_APBM_SNMP_ERROR_CONSTANTS_COUNT 22
	enum ErrorConstants {
//		ERR_SNMP_DEFINITION_START = -ACS_APBM_SNMP_ERROR_CONSTANTS_COUNT - 1,

		// Start SNMP error value definitions
		ERR_SNMP_ERROR_VALUES_START = -2000,

		// Add here new error codes and adjust the above ACS_APBM_ERROR_CONSTANTS_COUNT macro constant

		ERR_SNMP_NO_OPERATIONAL_SWITCH_BOARD,
		ERR_SNMP_NO_ACTIVE_SWITCH_BOARD,
		ERR_SNMP_IPMI_ACTIVE_NOTWORKING,
		ERR_SNMP_INVALID_SWITCH_BOARD_SLOT_POSITION,
		ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION,
		ERR_SNMP_CALLBACK_NO_MESSAGE_RECEIVED,
		ERR_SNMP_NOT_AXE_TRAP,
		ERR_SNMP_NOT_PDU_TRAP,
		ERR_SNMP_UNKNOWN_TRAP,
		ERR_SNMP_BAD_ASN_TYPE,
		ERR_SNMP_TIMEOUT,
		ERR_SNMP_NOT_ENOUGH_SPACE,
		ERR_SNMP_NULL_PARAMETER,
		ERR_SNMP_SELECT,
		ERR_SNMP_PDU_TOO_MANY_VARIABLES,
		ERR_SNMP_PDU_UNEXPECTED_VARIABLE,
		ERR_SNMP_PDU_VARIABLE_BAD_SIZE,
		ERR_SNMP_PDU_VARIABLE_NULL_SIZE,
		ERR_SNMP_PDU_VARIABLE_NO_SUCH_INSTANCE,
		ERR_SNMP_PDU_VARIABLE_BAD_TYPE,
		ERR_SNMP_PDU_NO_VARIABLE_BOUND,
		ERR_SNMP_PDU_ADD_NULL_VARIABLE,
		ERR_SNMP_PDU_ADD_VARIABLE_VALUE,
		ERR_SNMP_PDU_CREATE,
		ERR_SNMP_SESSION_READ,
		ERR_SNMP_SESSION_NOT_VALID,
		ERR_SNMP_SESSION_SEND,
		ERR_SNMP_SESSION_CLOSE,
		ERR_SNMP_SESSION_NOT_OPEN,
		ERR_SNMP_SESSION_OPEN,
		ERR_SNMP_SESSION_NOT_CLOSED,
		ERR_SNMP_SEND_SET_REQUEST,
		ERR_SNMP_TRAP_SUBSCRIPTION,
		ERR_SNMP_UNSUBSCRIBE_LEFT_SB,
		ERR_SNMP_UNSUBSCRIBE_RIGHT_SB,
		ERR_SNMP_UNSUBSCRIBE_BOTH_SB,
		ERR_SNMP_NO_SWITCH_BOARD_FOUND,

		// ERRORS BY NET-SNMP FOR GET/SET RESPONSE PDU

    /*
     * in SNMPv1, SNMPsec, SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 PDUs
     */
		ERR_SNMP_NETSNMP_ERR_TOOBIG,
		ERR_SNMP_NETSNMP_ERR_NOSUCHNAME,
		ERR_SNMP_NETSNMP_ERR_BADVALUE,
		ERR_SNMP_NETSNMP_ERR_READONLY,
		ERR_SNMP_NETSNMP_ERR_GENERR,
    /*
     * in SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 PDUs
     */
		ERR_SNMP_NETSNMP_ERR_NOACCESS,
		ERR_SNMP_NETSNMP_ERR_WRONGTYPE,
		ERR_SNMP_NETSNMP_ERR_WRONGLENGTH,
		ERR_SNMP_NETSNMP_ERR_WRONGENCODING,
		ERR_SNMP_NETSNMP_ERR_WRONGVALUE,
		ERR_SNMP_NETSNMP_ERR_NOCREATION,
		ERR_SNMP_NETSNMP_ERR_INCONSISTENTVALUE,
		ERR_SNMP_NETSNMP_ERR_RESOURCEUNAVAILABLE,
		ERR_SNMP_NETSNMP_ERR_COMMITFAILED,
		ERR_SNMP_NETSNMP_ERR_UNDOFAILED,
		ERR_SNMP_NETSNMP_ERR_AUTHORIZATIONERROR,
		ERR_SNMP_NETSNMP_ERR_NOTWRITABLE,

    /*
     * in SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 PDUs
     */
		ERR_SNMP_NETSNMP_ERR_INCONSISTENTNAME,

    /*
     * in case the NET-SNMP error is not defined
     */
		ERR_SNMP_NETSNMP_ERROR_UNDEFINED,

		ERR_SNMP_INTERNAL_ERROR,

		// End SNMP error value definitions

		ERR_SNMP_NO_ERRORS = 0
	};

	/** @brief SNMP session state constants.
	 *
	 *	SnmpSessionStateConstants enumeration detailed description
	 */
	enum SnmpSessionStateConstants {
		SNMP_SESSION_STATE_CLOSED	= 0,
		SNMP_SESSION_STATE_OPEN		= 1
	};

	/** @brief SNMP generic constants.
	 *
	 *	SnmpGenericConstants enumeration detailed description
	 */
	enum SnmpGenericConstants {
		TRAP_MESSAGE_LEN	= 15
	};

	/** @brief Switch Board Shelf Manager State constants.
	 *
	 *	SwitchBoardShelfMgrStateConstants enumeration detailed description
	 */
	typedef enum SwitchBoardShelfMgrStateConstants {
		SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN	=	-1,
		SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE	= acs_apbm_snmp::axe::eth::shelf::SHELF_MGR_STATE_PASSIVE,
		SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE		= acs_apbm_snmp::axe::eth::shelf::SHELF_MGR_STATE_ACTIVE,
		SWITCH_BOARD_SHELF_MGR_STATE_IDLE			= acs_apbm_snmp::axe::eth::shelf::SHELF_MGR_STATE_IDLE,
	} switch_board_shelf_mgr_state_t;

	/** @brief Switch Board Neighbour State constants.
	 *
	 *	SwitchBoardNeighbourConstants enumeration detailed description
	 */
	typedef enum SwitchBoardNeighbourStateConstants {
		SWITCH_BOARD_NEIGHBOUR_STATE_UNKNOWN	=	-1,
		SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT		= acs_apbm_snmp::NEIGHBOUR_ABSENT,
		SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT	= acs_apbm_snmp::NEIGHBOUR_PRESENT
	} switch_board_neighbour_state_t;


	/** @brief Shelf Manager Controller generic constants.
	 *
	 *	SmcGenericConstants enumeration detailed description
	 */
	enum SwitchBoardGenericConstants {
		SWITCH_BOARD_COUNT_PER_MAGAZINE = 2
	};

	enum SwitchBoardSubscriptionRequestType {
		SCBRP_UNSUBSCRIPTION_REQUEST = 0,
		SCBRP_SUBSCRIPTION_REQUEST = 1
	};

	enum SnmpIpmiStateCostants  {
		IPMI_NOTWORKING = 0,
		IPMI_WORKING = 1
	};
	/** @brief Shelf Manager Controller State constants.
	 *
	 *	ShelfManagerControllerStateConstants enumeration detailed description
	 */
	typedef enum ShelfManagerControllerStateConstants {
		SHELF_MNGR_CTRL_STATE_NOT_SCAN = acs_apbm_snmp::axe::eth::shelf::XSHMC_SHELF_MGR_CMD_NOT_SCAN,
		SHELF_MNGR_CTRL_STATE_SCAN = acs_apbm_snmp::axe::eth::shelf::XSHMC_SHELF_MGR_CMD_SCAN
	} shelf_manager_controller_state_t;

}

#endif // HEADER_GUARD_FILE__acs_apbm_snmpconstants
