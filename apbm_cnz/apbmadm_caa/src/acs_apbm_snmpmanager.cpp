/** @file acs_apbm_snmpmanager.cpp
 *      @brief
 *      @author xnicmut (Nicola Muto)
 *      @date 2011-09-20
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 *      REVISION INFO
 *      +=======+============+==============+=====================================+
 *      | REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *      +=======+============+==============+=====================================+
 *      | R-001 | 2011-09-20 | xnicmut      | Creation and first revision.        |
 *      +=======+============+==============+=====================================+
 *      |       | 2014-06-17 | xsunach      | TR HS54933                          |
 *      +=======+============+==============+=====================================+
 *      |       | 2015-02-12 | xsunach      | CNI 33/6-441                        |
 *      +=======+============+==============+=====================================+
 *      |       | 2015-06-12 | xsunach      | CNI 33/6-532                        |
 *      +=======+============+==============+=====================================+
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <ace/Guard_T.h>

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <algorithm>

#include <ACS_DSD_Server.h>

#include "acs_apbm_trapmessage.h"

#include "acs_apbm_programconstants.h"
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_macros.h"
#include "acs_apbm_snmpconstants.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_snmpsessionhandler.h"
#include "acs_apbm_snmpsessioninfo.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_snmpmibdefinitions.h"
#include "acs_apbm_trapsubscriptionmanager.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_snmpmanagercallbacks.h"
#include "acs_apbm_immrepositoryhandler.h"
#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_operationpipescheduler.h"
#include "acs_apbm_hwihandler.h"
#include "acs_apbm_scxlagimmhandler.h"
#include "acs_apbm_snmpmanager.h"

//TODO: find a better place for this
#define ASCII_IP_SIZE 10

const char * __CLASS_NAME__::_snmp_v3_passphrase = "APBM Server NMS Passphrase";
bool scxColdRestartFlag = false;
acs_apbm_snmp::lag_flag_type_t __CLASS_NAME__::operativeStateLeft =acs_apbm_snmp::DISABLE_LAG;
acs_apbm_snmp::lag_flag_type_t __CLASS_NAME__::operativeStateRight =acs_apbm_snmp::DISABLE_LAG;

acs_apbm_snmp::lag_flag_type_t __CLASS_NAME__::adminStateLeft =acs_apbm_snmp::DISABLE_LAG;
acs_apbm_snmp::lag_flag_type_t __CLASS_NAME__::adminStateRight =acs_apbm_snmp::DISABLE_LAG;

namespace {
	inline bool fx_compare_slot_ascending (acs_apbm_switchboardinfo left, acs_apbm_switchboardinfo right) {
		return (left.slot_position < right.slot_position);
	}

	inline bool fx_compare_magazine_slot_ascending (acs_apbm_switchboardinfo left, acs_apbm_switchboardinfo right) {
		return ((left.magazine < right.magazine) || ((left.magazine == right.magazine) && (left.slot_position < right.slot_position)));
	}

	class match_sb_magazine_slot {
		//==============//
		// Constructors //
		//==============//
	public:
		inline match_sb_magazine_slot (uint32_t magazine, int32_t slot_position) : _magazine(magazine), _slot_position(slot_position) {}


		//============//
		// Destructor //
		//============//
	public:
		inline ~match_sb_magazine_slot () {}


		//===========//
		// Operators //
		//===========//
	public:
		inline bool operator() (const acs_apbm_switchboardinfo & switch_board) const {
					// BEGIN: DEBUGGING and TRACING: TO BE DELETE
					ACS_APBM_LOG(LOG_LEVEL_TRACE, "TRACING CLASS [match_sb_magazine_slot] : Current switch board item to match is: magazine == %u, slot == %d", switch_board.magazine, switch_board.slot_position);
					ACS_APBM_LOG(LOG_LEVEL_TRACE, "TRACING CLASS [match_sb_magazine_slot] : Parameter to match are: magazine == %u, slot == %d", _magazine, _slot_position);
					ACS_APBM_LOG(LOG_LEVEL_TRACE, "TRACING CLASS [match_sb_magazine_slot] : Match result == %s",
							((switch_board.magazine == _magazine) && (switch_board.slot_position == _slot_position)) ? "TRUE" : "FALSE");
					// END: DEBUGGING and TRACING: TO BE DELETE
					return ((switch_board.magazine == _magazine) && (switch_board.slot_position == _slot_position));
				}


		//========//
		// Fields //
		//========//
	private:
		uint32_t _magazine;
		int32_t _slot_position;
	};
}


const char * __CLASS_NAME__::get_snmp_community () const {
	return _server_working_set->cs_reader->is_cba_system() ? "NETMAN" : "public";
}

int __CLASS_NAME__::send_trap_subscription_request () {
/* TO BE DELETED
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) { // ERROR: loading switch boards info from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'load_switch_boards_info_from_cs' failed: cannot load switch boards info from configuration system (CS): call_result == %d",
				call_result);
		return call_result;
	}
*/

	// TODO: Should it be put here some synchronization system? So more than
	// one subscription at a time can be avoided.
	if(_server_working_set->cs_reader->is_cba_system())
	{
		uint16_t fbn = 0;
		_server_working_set->cs_reader->get_switch_board_fbn(fbn);		
		if(fbn == ACS_CS_API_HWC_NS::FBN_SMXB) //No need to set trap subscriber
			return 0;
		else //for SCX
			return send_trap_subscription_request_scx(150000, "iss", "internet");
	}
	else
		return send_trap_unsubscription_request_scbrp();

}

int __CLASS_NAME__::send_trap_subscription_request (uint32_t magazine) {
	// TODO: Should it be put here some synchronization system? So more than
	// one subscription at a time can be avoided.

	if(_server_working_set->cs_reader->is_cba_system())
	{
		uint16_t fbn = 0;
                _server_working_set->cs_reader->get_switch_board_fbn(fbn);
		
		if(fbn == ACS_CS_API_HWC_NS::FBN_SMXB)
			return 0; 
		else
			return send_trap_subscription_request_scx(magazine, 150000, "iss", "internet");
	}
	else
		return send_trap_unsubscription_request_scbrp(magazine);

}

int __CLASS_NAME__::send_trap_unsubscription_request () {
/* TO BE DELETED
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) { // ERROR: loading switch boards info from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'load_switch_boards_info_from_cs' failed: cannot load switch boards info from configuration system (CS): call_result == %d",
				call_result);
		return call_result;
	}
*/

	// TODO: Should it be put here some synchronization system? So more than
	// one unsubscription at a time can be avoided.
	
	if(_server_working_set->cs_reader->is_cba_system())
	{
		uint16_t fbn = 0;
                _server_working_set->cs_reader->get_switch_board_fbn(fbn);

		if(fbn == ACS_CS_API_HWC_NS::FBN_SMXB)
			return 0;
		else
			return	send_trap_unsubscription_request_scx();
	}
	else
		return send_trap_unsubscription_request_scbrp();		
}




int __CLASS_NAME__::send_trap_unsubscription_request (int switch_board_slot_position) {
/* TO BE DELETED
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) { // ERROR: loading switch boards info from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'load_switch_boards_info_from_cs' failed: cannot load switch boards info from configuration system (CS): call_result == %d",
				call_result);
		return call_result;
	}
*/

	// TODO: Should it be put here some synchronization system? So more than
	// one unsubscription at a time can be avoided.

	if(_server_working_set->cs_reader->is_cba_system())
	{
		uint16_t fbn = 0;
                _server_working_set->cs_reader->get_switch_board_fbn(fbn);
		if(fbn == ACS_CS_API_HWC_NS::FBN_SMXB)
			return 0;
		else
			return  send_trap_unsubscription_request_scx(switch_board_slot_position);
	}
	else
		return send_trap_unsubscription_request_scbrp(switch_board_slot_position);

}

int __CLASS_NAME__::send_trap_unsubscription_request (uint32_t magazine) {
	// TODO: Should it be put here some synchronization system? So more than
	// one unsubscription at a time can be avoided.

	if(_server_working_set->cs_reader->is_cba_system())
        {
		uint16_t fbn = 0;
                _server_working_set->cs_reader->get_switch_board_fbn(fbn);
                if(fbn == ACS_CS_API_HWC_NS::FBN_SMXB)
                        return 0;
                else
                        return  send_trap_unsubscription_request_scx(magazine);
        }
        else
                return send_trap_unsubscription_request_scbrp(magazine);

}

int __CLASS_NAME__::send_trap_ack (int switch_board_slot_position, const char * ap_ip, const char * switch_board_ip) {
/* TO BE DELETED
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) { // ERROR: loading switch boards info from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'load_switch_boards_info_from_cs' failed: cannot load switch boards info from configuration system (CS): call_result == %d",
				call_result);
		return call_result;
	}
*/
	//TODO - Should it be put here some synchronization system!?!
	return _server_working_set->cs_reader->is_cba_system()
			? acs_apbm_snmp::ERR_SNMP_NO_ERRORS
			: send_trap_ack_scbrp(switch_board_slot_position, ap_ip, switch_board_ip);
}

int __CLASS_NAME__::set_shelf_mngr_ctrl_state_scbrp (
		int switch_board_slot_position,
		const char * switch_board_ip,
		acs_apbm_snmp::shelf_manager_controller_state_t state) {
/* TO BE DELETED
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) { // ERROR: loading switch boards info from CS
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'load_switch_boards_info_from_cs' failed: cannot load switch boards info from configuration system (CS): call_result == %d",
				call_result);
		return call_result;
	}
*/

	//Should it be put here some synchronization system!?!
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Sending to the switch board_%d) <%s>!", switch_board_slot_position, (state == acs_apbm_snmp::SHELF_MNGR_CTRL_STATE_SCAN) ? "SCAN": "NOSCAN");

	acs_apbm_snmpsessioninfo session_info;
	session_info.version(SNMP_VERSION_2c);
	session_info.community(const_cast<char *>(get_snmp_community()));
	session_info.peername(const_cast<char *>(switch_board_ip));

	oid oid_name [] = { ACS_APBM_SCB_OID_SHELF_MGR_CMD, 0 };

	// set the switch_board slot position into the oid name
	oid_name[ACS_APBM_ARRAY_SIZE(oid_name) - 1] = switch_board_slot_position;

	// snmp session communication handler used to send request and the callback used to receive the response
	acs_apbm_snmpsessionhandler session_handler;
	acs_apbm_snmploggercallback logger_callback("SNMP Response received for Shelf Manager Controller State setting");

	int call_result = 0;

	if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
				"snmp error text == '%s': call_result == %d",
				session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);

		return acs_apbm_snmp::ERR_SNMP_SESSION_OPEN;
	}

	// Prepare the PDU request APBM will send to the current switch board
	netsnmp_pdu * request_pdu = 0;

	if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on craeting a new PDU structure to carry out the state");

		session_handler.close();

		return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
	}
	// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
	request_pdu->variables = 0;

	long state_ = state;

	snmp_pdu_add_variable(request_pdu, oid_name, ACS_APBM_ARRAY_SIZE(oid_name), ASN_INTEGER, reinterpret_cast<u_char *>(&state_), sizeof(state_));

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending Shelf Manager Controller state...");

	unsigned timeout_ms = 2000; // 2 seconds timeout
        errno=0;
	call_result = session_handler.send_synch(request_pdu, &timeout_ms);
	snmp_free_pdu(request_pdu);
        int errno_save=errno;
	if (call_result) { // ERROR: sending the pdu
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
				"Call 'send_synch' failed: NET-SNMP library internal error on sending the Shelf Manager Controller state: call_result == %d", call_result);
	}

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_data (
		int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return get_board_ipmi_data(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::get_board_ipmi_data (
		int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_board_ipmi_data(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: board data retrieved successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_board_ipmi_data' failed: cannot retrieve board IPMI data info: call_result == %d", call_result);

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_data (
		int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get board IPMI data information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getboardipmidatacallback callback(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, board_magazine_str, board_slot, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scbrp [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_DATA, board_slot };
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_BLADE_IPMI_DATA, board_slot };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;

	if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
	else { oid_name = oid_name_scbrp; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scbrp); }

	call_result = get_board_ipmi_data(session_handler, callback, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_data (
		acs_apbm_snmpsessionhandler & session_handler,
		acs_apbm_snmpsessioncallback & callback,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board IPMI data info into the OID '%s'", oid_str);

	int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board IPMI data");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board IPMI data: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board IPMI data from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_status (
		axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return get_board_ipmi_upgrade_status(ipmi_upgrade_status, board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::get_board_ipmi_upgrade_status (
		axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_board_ipmi_upgrade_status(ipmi_upgrade_status, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: board data retrieved successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_board_ipmi_upgrade_status' failed: cannot retrieve board IPMI upgrade status info: call_result == %d", call_result);

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_status (
		axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get board IPMI upgrade status information about board in slot %d of magazine 0x%08X (%s) from switch board '%s'",
			board_slot, board_magazine, board_magazine_str, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getboardipmidatacallback callback(ipmi_upgrade_status, board_magazine_str, board_slot, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scbrp [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_UPGRADE_STATUS, board_slot };
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_BLADE_IPMI_UPGRADE_STATUS, board_slot };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;

	if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
	else { oid_name = oid_name_scbrp; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scbrp); }

	call_result = get_board_ipmi_upgrade_status(session_handler, callback, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_upgrade_status (
		acs_apbm_snmpsessionhandler & session_handler,
		acs_apbm_snmpsessioncallback & callback,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board IPMI upgrade status value into the OID '%s'", oid_str);

	int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board IPMI upgrade status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board IPMI upgrade status: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board IPMI upgrade status from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_data (
		int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
		axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status, // xshmcBoardIPMIUpgradeStatus
		int32_t board_slot,
		const unsigned * timeout_ms) {
	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	(call_result = get_board_ipmi_data(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, board_slot, timeout_ms)) ||
		(call_result = get_board_ipmi_upgrade_status(ipmi_upgrade_status, board_slot, timeout_ms));

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_data (
		int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
		axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status, // xshmcBoardIPMIUpgradeStatus
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {
	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	(call_result = get_board_ipmi_data(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, board_slot, board_magazine, timeout_ms)) ||
		(call_result = get_board_ipmi_upgrade_status(ipmi_upgrade_status, board_slot, board_magazine, timeout_ms));

	return call_result;
}

int __CLASS_NAME__::get_board_ipmi_data (
		int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
		char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
		char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
		axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status, // xshmcBoardIPMIUpgradeStatus
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	(call_result = get_board_ipmi_data(ipmi_firmware_type, ipmi_product_number, ipmi_product_revision, board_slot, board_magazine, switch_board_ip, timeout_ms)) ||
		(call_result = get_board_ipmi_upgrade_status(ipmi_upgrade_status, board_slot, board_magazine, switch_board_ip, timeout_ms));

	return call_result;
}

int __CLASS_NAME__::set_board_ipmi_upgrade (
		const char * upgrade_path,
		uint32_t upgrade_ip4,
		int32_t board_slot,
		const unsigned * timeout_ms) {
	char upgrade_ip4_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(upgrade_ip4_str, upgrade_ip4)) // ERROR: while converting
		return call_result;

	return set_board_ipmi_upgrade(upgrade_path, upgrade_ip4_str, board_slot, timeout_ms);
}

int __CLASS_NAME__::set_board_ipmi_upgrade (
		const char * upgrade_path,
		uint32_t upgrade_ip4,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {
	char upgrade_ip4_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(upgrade_ip4_str, upgrade_ip4)) // ERROR: while converting
		return call_result;

	return set_board_ipmi_upgrade(upgrade_path, upgrade_ip4_str, board_slot, board_magazine, timeout_ms);
}

int __CLASS_NAME__::set_board_ipmi_upgrade (
		const char * upgrade_path,
		uint32_t upgrade_ip4,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char upgrade_ip4_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(upgrade_ip4_str, upgrade_ip4)) // ERROR: while converting
		return call_result;

	return set_board_ipmi_upgrade(upgrade_path, upgrade_ip4_str, board_slot, board_magazine, switch_board_ip, timeout_ms);
}

int __CLASS_NAME__::set_board_ipmi_upgrade (
		const char * upgrade_path,
		const char * upgrade_ip4,
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return set_board_ipmi_upgrade(upgrade_path, upgrade_ip4, board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::set_board_ipmi_upgrade (
		const char * upgrade_path,
		const char * upgrade_ip4,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_board_ipmi_upgrade(upgrade_path, upgrade_ip4, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: board upgrade set successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'set_board_ipmi_upgrade' failed: cannot set the board IPMI upgrade: call_result == %d", call_result);

	return call_result;
}

int __CLASS_NAME__::set_board_ipmi_upgrade (
		const char * upgrade_path,
		const char * upgrade_ip4,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to set the IPMI upgrade process on the board in slot %d of magazine %s towards the switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_snmploggercallback callback("SNMP Response received for IPMI upgrade process", board_magazine_str, board_slot, switch_board_ip, is_cba);
	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scbrp [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_UPGRADE, board_slot };
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_BLADE_IPMI_UPGRADE, board_slot };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;
	char ipmi_upgrade_value [4 * 1024];

	if (is_cba) {
		oid_name = oid_name_scx;
		oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx);
		::snprintf(ipmi_upgrade_value, ACS_APBM_ARRAY_SIZE(ipmi_upgrade_value), "%s:%s", upgrade_ip4, upgrade_path);
	} else {
		oid_name = oid_name_scbrp;
		oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scbrp);
		::snprintf(ipmi_upgrade_value, ACS_APBM_ARRAY_SIZE(ipmi_upgrade_value), "%s;%s", upgrade_path, upgrade_ip4);
	}

	call_result = set_board_ipmi_upgrade(ipmi_upgrade_value, session_handler, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::set_board_ipmi_upgrade (
		const char * ipmi_upgrade_value,
		acs_apbm_snmpsessionhandler & session_handler,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {
	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU SET request to start the IPMI upgrade process: OID '%s': using this value '%s'",
			oid_str, ipmi_upgrade_value);

	int call_result = send_set_request(session_handler, oid_name, oid_name_size, ipmi_upgrade_value, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
	}

	return call_result;
}

int __CLASS_NAME__::get_board_data (
		char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
		char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
		char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
		char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
		char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
		char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return get_board_data(product_number, product_revision_state, product_name, serial_number, manufacture_date, vendor,
			board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::get_board_data (
		char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
		char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
		char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
		char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
		char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
		char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_board_data(product_number, product_revision_state, product_name, serial_number, manufacture_date, vendor,
				board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: board data retrieved successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_board_data' failed: cannot retrieve board data info: call_result == %d", call_result);

	return call_result;
}

int __CLASS_NAME__::get_board_data (
		char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
		char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
		char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
		char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
		char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
		char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {

	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get BOARD DATA information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	return _server_working_set->cs_reader->is_cba_system()
			? get_board_data_scx(product_number, product_revision_state, product_name, serial_number, manufacture_date, vendor,
					board_slot, board_magazine_str, switch_board_ip, timeout_ms)
			: get_board_data_scbrp(product_number, product_revision_state, product_name, serial_number, manufacture_date, vendor,
					board_slot, board_magazine_str, switch_board_ip, timeout_ms);
}

int __CLASS_NAME__::get_board_data_scbrp (
		char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
		char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
		char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
		char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
		char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
		char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
		int32_t board_slot,
		const char * board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	oid data_oid_name [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_DATA, board_slot };

	acs_apbm_getboarddatacallback callback(product_number, product_revision_state, product_name, serial_number, manufacture_date, vendor,
			board_magazine, board_slot, switch_board_ip, 0);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), data_oid_name, ACS_APBM_ARRAY_SIZE(data_oid_name));

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Sending the PDU GET request to retrieve the board data values into the OID '%s'", oid_str);

	call_result = send_get_request(session_handler, data_oid_name, ACS_APBM_ARRAY_SIZE(data_oid_name), timeout_ms);

	session_handler.close();

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board data values");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board data values: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board data values from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

int __CLASS_NAME__::get_board_data_scx (
		char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
		char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
		char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
		char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
		char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
		char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
		int32_t board_slot,
		const char * board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {

	oid data_oid_names [][17] = {
		{ ACS_APBM_SCX_OID_BLADE_PRODUCT_NUMBER, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_PRODUCT_REVISION_STATE, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_PRODUCT_NAME, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_SERIAL_NUMBER, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_MANUFACTURING_DATE, board_slot },
		{ ACS_APBM_SCX_OID_BLADE_VENDOR_NAME, board_slot }
	};
	ACS_APBM_LOG(LOG_LEVEL_DEBUG," Inside the get_board_data_scx This is the one which collects the board information ");

	acs_apbm_getboarddatacallback callback(product_number, product_revision_state, product_name, serial_number, manufacture_date, vendor,
			board_magazine, board_slot, switch_board_ip, 1);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	char oid_str [512];

	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(data_oid_names)) && !call_result; ++i) {
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), data_oid_names[i], ACS_APBM_ARRAY_SIZE(data_oid_names[i]));

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Sending the PDU GET request to retrieve the board data value into the OID '%s'", oid_str);

		call_result = send_get_request(session_handler, data_oid_names[i], ACS_APBM_ARRAY_SIZE(data_oid_names[i]), timeout_ms);

		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board data value");
		} else if (call_result) { // ERROR: sending the request
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board data value: call_result == %d",
					call_result);
		}

		if (call_result) break;

		// Here a possible error in pdu data should be verified
		if (callback.error_code) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board data value from response PDU: "
					"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
			call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
		}
	}

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::get_board_leds_status (
		axe_eth_shelf::led_status_t & green_led,
		axe_eth_shelf::led_status_t & red_led,
		axe_eth_shelf::led_status_t & mia_led,
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return get_board_leds_status(green_led, red_led, mia_led, board_slot, my_magazine, timeout_ms);
}

//only for 3 LEDs
int __CLASS_NAME__::get_board_leds_status (
		axe_eth_shelf::led_status_t & green_led,
		axe_eth_shelf::led_status_t & red_led,
		axe_eth_shelf::led_status_t & mia_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	axe_eth_shelf::bus_type_t apboard_bus_type = axe_eth_shelf::BUS_TYPE_UNDEFINED;

	_server_working_set->shelves_data_manager->get_ap_board_bus_type(apboard_bus_type, board_slot) && (apboard_bus_type = axe_eth_shelf::BUS_TYPE_UNDEFINED);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "The board at slot %d has a bus of type %d", board_slot, apboard_bus_type);

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "My operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;
	int ipn_index = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	if (apboard_bus_type == axe_eth_shelf::BUS_TYPE_IPMI) {
		// Gets the green and red leds
		ipn_index = ::random() & 0x01;
		for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
			call_result = get_board_green_red_leds_status(green_led, red_led, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

			if (!call_result) break; // OK: green and red leds retrieved successfully
		}

		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'get_board_green_red_leds_status' failed: cannot retrieve Green and Red LEDs status info: call_result == %d", call_result);
			return call_result;
		}
	} else green_led = red_led = static_cast<axe_eth_shelf::led_status_t>(2);//axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;

	// Gets the MIA led
	ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_board_mia_led_status(mia_led, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: MIA led retrieved successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Call 'get_board_mia_led_status' failed: cannot retrieve MIA LED status info: call_result == %d", call_result);

	return call_result;
}

//4 LEDS compatibility
int __CLASS_NAME__::get_board_leds_status (
		axe_eth_shelf::led_status_t & green_led,
		axe_eth_shelf::led_status_t & red_led,
		axe_eth_shelf::led_status_t & mia_led,
		axe_eth_shelf::led_status_t & blue_led,
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return get_board_leds_status(green_led, red_led, mia_led, blue_led, board_slot, my_magazine, timeout_ms);
}

//4 LEDS compatibility
int __CLASS_NAME__::get_board_leds_status (
		axe_eth_shelf::led_status_t & green_led,
		axe_eth_shelf::led_status_t & red_led,
		axe_eth_shelf::led_status_t & mia_led,
		axe_eth_shelf::led_status_t & blue_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	axe_eth_shelf::bus_type_t apboard_bus_type = axe_eth_shelf::BUS_TYPE_UNDEFINED;

	_server_working_set->shelves_data_manager->get_ap_board_bus_type(apboard_bus_type, board_slot) && (apboard_bus_type = axe_eth_shelf::BUS_TYPE_UNDEFINED);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "The board at slot %d has a bus of type %d", board_slot, apboard_bus_type);

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "My operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;
	int ipn_index = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	if (apboard_bus_type == axe_eth_shelf::BUS_TYPE_IPMI) {
		// Gets the green and red leds
		ipn_index = ::random() & 0x01;
		for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
			call_result = get_board_green_red_leds_status(green_led, red_led, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

			if (!call_result) break; // OK: green and red leds retrieved successfully
		}

		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'get_board_green_red_leds_status' failed: cannot retrieve Green and Red LEDs status info: call_result == %d", call_result);
			return call_result;
		}
	} else green_led = red_led = static_cast<axe_eth_shelf::led_status_t>(2);//axe_eth_shelf::LED_STATUS_NOT_AVAILABLE

	// Gets the MIA led
	ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_board_mia_led_status(mia_led, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: MIA led retrieved successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Call 'get_board_mia_led_status' failed: cannot retrieve MIA LED status info: call_result == %d", call_result);

	// Gets the BLUE led
		ipn_index = ::random() & 0x01;
		for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
			call_result = get_board_blue_led_status(blue_led, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

			if (!call_result) break; // OK: BLUE led retrieved successfully
		}

		if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Call 'get_board_blue_led_status' failed: cannot retrieve BLUE LED status info: call_result == %d", call_result);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"In get_board_led_status... green_led = %d, red_led = %d, mia_led = %d, blue_led = %d on slot %d",green_led,red_led,mia_led,blue_led,board_slot);

	return call_result;
}


int __CLASS_NAME__::get_board_green_red_leds_status (
		axe_eth_shelf::led_status_t & green_led,
		axe_eth_shelf::led_status_t & red_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};
        bool is_4leds_supported = _server_working_set->shelves_data_manager->is_4led_supported;
	bool is_new_gea_supported = _server_working_set->shelves_data_manager->is_new_gea_supported;

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get GREEN and RED LEDs status information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	// Prepare the oid arrays
	oid oid_scbrp_green_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_GREEN_LED, board_slot };
	oid oid_scbrp_red_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_RED_LED, board_slot };
	oid oid_scbrp_green_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_GREEN_LED, board_slot };
	oid oid_scbrp_red_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_RED_LED, board_slot };

	oid oid_scx_green_led [] = { ACS_APBM_SCX_OID_BLADE_GREEN_LED, board_slot };
	oid oid_scx_red_led [] = { ACS_APBM_SCX_OID_BLADE_RED_LED, board_slot };
	oid oid_scx_green_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_GREEN_LED, board_slot };
	oid oid_scx_red_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_RED_LED, board_slot };

	int is_cba_system = _server_working_set->cs_reader->is_cba_system();

	oid * oid_red_led = 0;
	size_t oid_red_led_size = 0;

	oid * oid_green_led = 0;
	size_t oid_green_led_size = 0;
        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_4leds_supported value - %d .....",is_4leds_supported);
        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_new_gea_supported value - %d .....",is_new_gea_supported);
        acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
	uint16_t apboard_fbn;
        if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, board_slot, board_magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
                return ret_val;
        }
        if(is_cba_system)
        {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_cba_system");
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
		{
			if(is_new_gea_supported)
	                {
                        	oid_green_led= oid_scx_green_4led;
	                        oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_green_4led);
                        	oid_red_led= oid_scx_red_4led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_4led);
        	        }
                	else
	                {
                        	oid_green_led = oid_scx_green_led;
	                        oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_green_led);
                        	oid_red_led = oid_scx_red_led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_led);
        	        }
		}
		else
		{
		        if(is_4leds_supported)
        	        {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_4leds_supported");
	                        oid_green_led= oid_scx_green_4led;
        	                oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_green_4led);
                        	oid_red_led= oid_scx_red_4led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_4led);
        	        }
                	else
	                {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_4leds_not_supported");
                	        oid_green_led = oid_scx_green_led;
                        	oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_green_led);
                        	oid_red_led = oid_scx_red_led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_led);
      			}
		}
        }
        else
        {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"not is_cba_system");
		 if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
                {
                        if(is_new_gea_supported)
                        {
                        	oid_green_led = oid_scbrp_green_4led;
	                        oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_green_4led);
        	                oid_red_led = oid_scbrp_red_4led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_4led);
                        }
                        else
                        {
                	        oid_green_led = oid_scbrp_green_led;
        	                oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_green_led);
                        	oid_red_led = oid_scbrp_red_led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_led);
                        }
                }
		else
		{
			if(is_4leds_supported)
	                {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_4leds_supported");
                        oid_green_led = oid_scbrp_green_4led;
                        oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_green_4led);

                        oid_red_led = oid_scbrp_red_4led;
                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_4led);
        	        }
                	else
	                {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_4leds_not_supported");
                        oid_green_led = oid_scbrp_green_led;
                        oid_green_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_green_led);

                        oid_red_led = oid_scbrp_red_led;
                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_led);
        	        }
		}
        }
	
	acs_apbm_getboardledsstatuscallback callback(green_led, red_led, board_magazine_str, board_slot, switch_board_ip, is_cba_system, apboard_fbn, is_4leds_supported, is_new_gea_supported);

	acs_apbm_snmpsessionhandler session_handler;

	if (const int call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}


	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_green_led, oid_green_led_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board GREEN LED status information: Green LED oid == '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_green_led, oid_green_led_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request for GREEN LED status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request for GREEN LED status information: call_result == %d",
				call_result);
	}

	if (call_result) { session_handler.close(); return call_result; }

	// Here, if no errors in PDU, the data are already read in the callback object that received the returned pdu from the SNMP agent

	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board GREEN LED status information from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	if (call_result) { session_handler.close(); return call_result; }

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_red_led, oid_red_led_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board RED LED status information: Red LED oid == '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_red_led, oid_red_led_size, timeout_ms);

	session_handler.close();

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request for RED LED status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request for RED LED status information: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here, if no errors in PDU, the data are already read in the callback object that received the returned pdu from the SNMP agent

	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board RED LED status information from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

int __CLASS_NAME__::get_board_mia_led_status (
		axe_eth_shelf::led_status_t & mia_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};
        bool is_4leds_supported = _server_working_set->shelves_data_manager->is_4led_supported;
	bool is_new_gea_supported = _server_working_set->shelves_data_manager->is_new_gea_supported;

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get MIA LED status information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	// Prepare the oid arrays
	oid oid_scbrp_mia_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_YELLOW_LED, board_slot };

	oid oid_scx_mia_led [] = { ACS_APBM_SCX_OID_BLADE_YELLOW_LED, board_slot };
	oid oid_scbrp_mia_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_YELLOW_LED, board_slot };

	oid oid_scx_mia_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_YELLOW_LED, board_slot };
        
	int is_cba_system = _server_working_set->cs_reader->is_cba_system();

	oid * oid_mia_led = 0;
	size_t oid_mia_led_size = 0;

        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_4leds_supported value - %d .....",is_4leds_supported);
        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_new_gea_supported value - %d .....",is_new_gea_supported);
	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
	uint16_t apboard_fbn;
	if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, board_slot, board_magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
		return ret_val;
	}
        if(is_cba_system)
        {
        	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_cba_system");
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
                {
                        if(is_new_gea_supported)
                        {
        	                oid_mia_led= oid_scx_mia_4led;
                	        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_4led);
	                }
        	        else
                	{
                        	oid_mia_led = oid_scx_mia_led;
	                        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_led);
	                }
		}
		else
		{
		        if(is_4leds_supported)
        	        {
        			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_4leds_supported");
                        	oid_mia_led= oid_scx_mia_4led;
	                        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_4led);
        	        }
                	else
	                {
        			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"not is_4leds_supported");
                	        oid_mia_led = oid_scx_mia_led;
                        	oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_led);
	                }
		}
        }
        else
        {
        	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"not is_cba_system");
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
                {
                        if(is_new_gea_supported)
                        {
				oid_mia_led = oid_scbrp_mia_4led;
	                        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_4led);
        	        }
	                else
        	        {
                	        oid_mia_led = oid_scbrp_mia_led;
                        	oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_led);
	                }
		}
		else
		{
	                if(is_4leds_supported)
        	        {
                	        oid_mia_led = oid_scbrp_mia_4led;
                        	oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_4led);
	                }
        	        else
                	{
                        	oid_mia_led = oid_scbrp_mia_led;
	                        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_led);
        	        }
		}
        }
	
	acs_apbm_getboardledsstatuscallback callback(mia_led, board_magazine_str, board_slot, switch_board_ip, is_cba_system, axe_eth_shelf::LED_COLOR_YELLOW, apboard_fbn, is_4leds_supported, is_new_gea_supported);

	acs_apbm_snmpsessionhandler session_handler;

	if (const int call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_mia_led, oid_mia_led_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board MIA LED status information: MIA LED oid == '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_mia_led, oid_mia_led_size, timeout_ms);

	session_handler.close();

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request for MIA LED status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request for MIA LED status information: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here, if no errors in PDU, the data are already read in the callback object that received the returned pdu from the SNMP agent

	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board MIA LED status information from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

//4 LEDS hardware compatibility: maintenance led
int __CLASS_NAME__::get_board_blue_led_status (
		axe_eth_shelf::led_status_t & blue_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};
        
        bool is_4leds_supported = _server_working_set->shelves_data_manager->is_4led_supported;
	bool is_new_gea_supported = _server_working_set->shelves_data_manager->is_new_gea_supported;
/*        if(!is_4leds_supported) {
	    ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get BLUE LED status information from 3 LEDs board which is not supported");
            return  -1;   
        }*/

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get BLUE LED status information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	// Prepare the oid arrays
	oid oid_scbrp_blue_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_BLUE_LED, board_slot };

	oid oid_scx_blue_led [] = { ACS_APBM_SCX_OID_BLADECOMMON_BLUE_LED, board_slot };

	int is_cba_system = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
        uint16_t apboard_fbn;
        if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, board_slot, board_magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
                return ret_val;
        }
	
	acs_apbm_getboardledsstatuscallback callback(blue_led, board_magazine_str, board_slot, switch_board_ip, is_cba_system, axe_eth_shelf::LED_COLOR_BLUE, apboard_fbn, is_4leds_supported, is_new_gea_supported);

	acs_apbm_snmpsessionhandler session_handler;

	if (const int call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid * oid_blue_led = 0;
	size_t oid_blue_led_size = 0;

        if(is_cba_system)
        {
                oid_blue_led= oid_scx_blue_led;
                oid_blue_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_blue_led);
        }
        else
        {
                oid_blue_led = oid_scbrp_blue_led;
                oid_blue_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_blue_led);
        }

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_blue_led, oid_blue_led_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board BLUE LED status information: BLUE LED oid == '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_blue_led, oid_blue_led_size, timeout_ms);

	session_handler.close();

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request for BLUE LED status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request for BLUE LED status information: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here, if no errors in PDU, the data are already read in the callback object that received the returned pdu from the SNMP agent

	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board BLUE LED status information from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

int __CLASS_NAME__::get_board_bios_run_mode (
		axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return get_board_bios_run_mode(bios_run_mode, board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::get_board_bios_run_mode (
		axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_board_bios_run_mode(bios_run_mode, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: board data retrieved successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_board_bios_run_mode' failed: cannot retrieve board BIOS run mode info: call_result == %d", call_result);

	return call_result;
}

int __CLASS_NAME__::get_board_bios_run_mode (
		axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get board BIOS run mode information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getboardbiosrunmodecallback callback(bios_run_mode, board_magazine_str, board_slot, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scbrp [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_BIOS_RUN_MODE, board_slot };
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_BLADE_BIOS_RUN_MODE, board_slot };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;

	if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
	else { oid_name = oid_name_scbrp; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scbrp); }

	call_result = get_board_bios_run_mode(session_handler, callback, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::get_board_bios_run_mode (
		acs_apbm_snmpsessionhandler & session_handler,
		acs_apbm_snmpsessioncallback & callback,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board BIOS run mode value into the OID '%s'", oid_str);

	int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board BIOS run mode");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board BIOS run mode: call_result == %d",
				call_result);
	}

	if (call_result){ return call_result;}

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board BIOS run mode from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

int __CLASS_NAME__::get_board_power_status (
		acs_apbm_snmp::bladePwr_status_t & power_status,
		int32_t board_slot,
		const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
		return call_result;

	return get_board_power_status(power_status, board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::get_board_power_status (
		acs_apbm_snmp::bladePwr_status_t & power_status,
		int32_t board_slot,
		uint32_t board_magazine,
		const unsigned * timeout_ms) {

	char my_operational_switch_board_ipns [2][16];

	if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_board_power_status(power_status, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

		if (!call_result) break; // OK: board data retrieved successfully
	}

	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_board_power_status' failed: cannot retrieve board POWER status info: call_result == %d", call_result);

	return call_result;
}

int __CLASS_NAME__::get_board_power_status (
		acs_apbm_snmp::bladePwr_status_t & power_status,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get board POWER status information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getboardpowerstatuscallback callback(power_status, board_magazine_str, board_slot, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scbrp [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_PWR_ON, board_slot };
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_BLADE_PWR, board_slot };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;

	if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
	else { oid_name = oid_name_scbrp; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scbrp); }

	call_result = get_board_power_status(session_handler, callback, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::get_board_power_status (
		acs_apbm_snmpsessionhandler & session_handler,
		acs_apbm_snmpsessioncallback & callback,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board POWER status value into the OID '%s'", oid_str);

	int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board POWER status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board POWER status: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board POWER status from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}

/***************************************************************

	get_blade_led_type: To query the mib for blade_led_type.

****************************************************************/

int __CLASS_NAME__::get_blade_led_type (
                acs_apbm_snmp::blade_led_type_t & blade_led_type,
                int32_t board_slot,
                const unsigned * timeout_ms) {

        uint32_t my_magazine = 0;

        if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
                return call_result;

        return get_blade_led_type(blade_led_type, board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::get_blade_led_type (
                acs_apbm_snmp::blade_led_type_t & blade_led_type,
                int32_t board_slot,
                uint32_t board_magazine,
                const unsigned * timeout_ms) {

        char my_operational_switch_board_ipns [2][16];

        if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
                return call_result;

        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

        int call_result = 0;

        ::srandom(static_cast<unsigned>(::time(0)));

        int ipn_index = ::random() & 0x01;
        for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
                call_result = get_blade_led_type(blade_led_type, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

                if (!call_result) break; // OK: board data retrieved successfully
        }

        if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_blade_led_type' failed: cannot retrieve blade led type info: call_result == %d", call_result);

        return call_result;
}

int __CLASS_NAME__::get_blade_led_type (
                acs_apbm_snmp::blade_led_type_t & blade_led_type,
                int32_t board_slot,
                uint32_t board_magazine,
                const char * switch_board_ip,
                const unsigned * timeout_ms) {
        char board_magazine_str [16] = {0};

        if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
                return call_result;

        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get blade led type information about board in slot %d of magazine %s from switch board '%s'",
                        board_slot, board_magazine_str, switch_board_ip);

        int is_cba = _server_working_set->cs_reader->is_cba_system();

        acs_apbm_getbladeledtypecallback callback(blade_led_type, board_magazine_str, board_slot, switch_board_ip, is_cba);

        acs_apbm_snmpsessionhandler session_handler;

        int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

        if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip)))            {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
                return call_result;
        }

        oid oid_name_scx[]= { ACS_APBM_SCX_OID_BLADE_LED_TYPE, board_slot };
	oid oid_name_scb[]= { ACS_APBM_SCB_OID_XSHMC_BOARD_LED_TYPE, board_slot };   
        const oid * oid_name = 0;
	size_t oid_name_size = 0;
        if (is_cba) 
        {
		oid_name = oid_name_scx;
        	oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); 
        }
        else { 
		oid_name = oid_name_scb;
                oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scb);
	 }

        call_result = get_blade_led_type(session_handler, callback, oid_name, oid_name_size, timeout_ms);

        session_handler.close();

        return call_result;
}

int __CLASS_NAME__::get_blade_led_type (
                acs_apbm_snmpsessionhandler & session_handler,
                acs_apbm_snmpsessioncallback & callback,
                const oid * oid_name,
                size_t oid_name_size,
                const unsigned * timeout_ms) {
        char oid_str [512];

        oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the blade led type value into the OID '%s'", oid_str);

        int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

        if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve blade led type");
        }
	 else if (call_result) { // ERROR: sending the request
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve blade led type: call_result == %d", call_result);
        }

        if (call_result) return call_result;

        // Here a possible error in pdu data should be verified
        if (callback.error_code) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading blade led type from response PDU: "
                                "callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
                call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
        }

        return call_result;
}

/**********************************/
int __CLASS_NAME__::get_board_fatal_event_log (
                char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
                int32_t board_slot,
                const unsigned * timeout_ms) {

	uint32_t my_magazine = 0;

        if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) // ERROR: getting my magazine number
                return call_result;

        return get_board_fatal_event_log(fatalEventLogInfo, board_slot, my_magazine, timeout_ms);
}

int __CLASS_NAME__::get_board_fatal_event_log (
                char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
                int32_t board_slot,
                uint32_t board_magazine,
                const unsigned * timeout_ms) {

        char my_operational_switch_board_ipns [2][16];

        if (const int call_result = get_my_operational_switch_board_ipns(my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1], board_slot))
                return call_result;

        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operational switch board IPNs are '%s' '%s'", my_operational_switch_board_ipns[0], my_operational_switch_board_ipns[1]);

        int call_result = 0;

        ::srandom(static_cast<unsigned>(::time(0)));

        int ipn_index = ::random() & 0x01;
        for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
                call_result = get_board_fatal_event_log(fatalEventLogInfo, board_slot, board_magazine, my_operational_switch_board_ipns[ipn_index], timeout_ms);

                if (!call_result) break; // OK: board data retrieved successfully
        }

        if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'get_board_fatal_event_log' failed: cannot retrieve board FATAL EVENT LOG info: call_result == %d", call_result);

        return call_result;
}

int __CLASS_NAME__::get_board_fatal_event_log (
                char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
                int32_t board_slot,
                uint32_t board_magazine,
                const char * switch_board_ip,
                const unsigned * timeout_ms) {
        char board_magazine_str [16] = {0};

        if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
                return call_result;

        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get board FATAL EVENT LOG information about board in slot %d of magazine %s from switch board '%s'",
                        board_slot, board_magazine_str, switch_board_ip);

        int is_cba = _server_working_set->cs_reader->is_cba_system();

        acs_apbm_getboardfataleventlogcallback callback(fatalEventLogInfo, board_magazine_str, board_slot, switch_board_ip, is_cba);

        acs_apbm_snmpsessionhandler session_handler;

        int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

        if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
                return call_result;
        }

        oid oid_name_scbrp [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_FATAL_EVENT_LOG_INFO, board_slot };
        oid oid_name_scx [] = { ACS_APBM_SCX_OID_FATAL_EVENT_LOG_INFO, board_slot };

        const oid * oid_name = 0;
        size_t oid_name_size = 0;

        if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
        else { oid_name = oid_name_scbrp; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scbrp); }

        call_result = get_board_fatal_event_log(session_handler, callback, oid_name, oid_name_size, timeout_ms);

        session_handler.close();

        return call_result;
}

int __CLASS_NAME__::get_board_fatal_event_log (
                acs_apbm_snmpsessionhandler & session_handler,
                acs_apbm_snmpsessioncallback & callback,
                const oid * oid_name,
                size_t oid_name_size,
                const unsigned * timeout_ms) {
        char oid_str [512];

        oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board FATAL EVENT LOG info value into the OID '%s'", oid_str);

        int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

        if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board FATAL EVENT LOG");
        } else if (call_result) { // ERROR: sending the request
                ACS_APBM_LOG(LOG_LEVEL_ERROR,
                                "Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board FATAL EVENT LOG info: call_result == %d",
                                call_result);
        }

        if (call_result) return call_result;

        // Here a possible error in pdu data should be verified
        if (callback.error_code) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board FATAL EVENT LOG info from response PDU: "
                                "callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
                call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
        }

        return call_result;
}

/**********************************/

int __CLASS_NAME__::find_switch_board_master (const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;
	int call_result = 0;
	int switch_board_active_found = 0;

	for (size_t i = 0; !switch_board_active_found && (i < _my_switch_board_infos.size()); ++i) {
		call_result = find_switch_board_master(switch_board_active_found, _my_switch_board_infos[i], timeout_ms);
	int active_switch_board_index = -1;
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,
				"Call 'find_switch_board_master' failed: finding the master/slave status failed for the switch board in the slot %d: call_result == %d",
				_my_switch_board_infos[i].slot_position, call_result);
		}
		switch_board_active_found && (active_switch_board_index = static_cast<int>(i));
	}

	return ((switch_board_active_found)? acs_apbm_snmp::ERR_SNMP_NO_ERRORS: acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD) ;
} /*+*/

int __CLASS_NAME__::find_switch_board_master (int & switch_board_active_found, acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms) {
	const char * switch_board_ipn_str_addresses [2] = { switch_board.ipna_str, switch_board.ipnb_str };
	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; !switch_board_active_found && (i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = find_switch_board_master(switch_board_active_found, switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], timeout_ms);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'find_switch_board_master' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}

	return call_result;
}

int __CLASS_NAME__::find_switch_board_master (int & switch_board_active_found, int32_t switch_board_slot, const char * switch_board_ip, const unsigned * timeout_ms) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to send Master/Slave status request to switch board %s in slot %d", switch_board_ip, switch_board_slot);

	switch_board_active_found = 0;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	int state = 0;

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getshelfmgrcontrolstatecallback callback(state, 0, switch_board_slot, switch_board_ip, is_cba);
	acs_apbm_snmpsessionhandler session_handler;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scb [] = { ACS_APBM_SCB_OID_SHELF_MGR_CMD, switch_board_slot };
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_SHELF_MGR_CTRL_STATE, 0 };

	oid * oid_name = 0;
	size_t oid_name_size = 0;

	if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
	else { oid_name = oid_name_scb; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scb); }

	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the switch board Master/Slave status into the OID '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve the switch board Master/Slave status");
	} else if (call_result) { // ERROR: sending the request//
		//oid oid_name_scx [] = { ACS_APBM_SCX_OID_SHELF_MGR_CTRL_STATE, 0 };
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve the switch board Master/Slave status: call_result == %d",
				call_result);
	}

	if (call_result) { session_handler.close(); return call_result; }

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading the switch board Master/Slave status from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	session_handler.close();

	// Check the status value
	call_result || (is_cba ? (switch_board_active_found = (state == acs_apbm_snmp::generic::shelf::SHELF_MGR_CTRL_STATE_ACTIVE))
												 : (switch_board_active_found = (state == acs_apbm_snmp::axe::eth::shelf::XSHMC_SHELF_MGR_CMD_SCAN)));

	return call_result;
}

int __CLASS_NAME__::update_my_switch_board_states (const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	bool isCBA = _server_working_set->cs_reader->is_cba_system();
	int call_result = 0;
	int slot_pos; uint32_t mag_num;
	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		slot_pos = _my_switch_board_infos[i].slot_position;
		mag_num = _my_switch_board_infos[i].magazine;
		call_result = update_switch_board_states(_my_switch_board_infos[i], timeout_ms);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'update_switch_board_states' failed: states not updated for board in slot %d of magazine 0x%08X: call_result == %d",
					slot_pos, mag_num, call_result);
		}
		else
			if(_my_switch_board_infos[i].state == SWITCH_BOARD_STATE_ACTIVE(isCBA)){
					set_my_active_switch_board_index (i);
				}
		// check if snmp link is down
		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			int64_t current_time = ::clock();
			if( current_time >= 0){
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LINK DOWN Detected for Switch Board in slot %d of magazine 0x%08X: at time %ld", slot_pos, mag_num, current_time);
				set_scb_link_down_time(slot_pos, mag_num, current_time);
			}else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'time(0) failed: Cannot set last link down time for Switch Board in slot %d <errno == %d>", slot_pos, errno);
		}
		else
			reset_scb_link_down_time(slot_pos, mag_num);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}


int __CLASS_NAME__::update_other_switch_board_states (const unsigned * timeout_ms) {
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_other_switch_board_found_check()) return call_result;

	int call_result = 0;
	int slot_pos; uint32_t mag_num;
	for (size_t i = 0; i < _other_switch_board_infos.size(); ++i) {
		slot_pos = _other_switch_board_infos[i].slot_position;
		mag_num = _other_switch_board_infos[i].magazine;
		call_result = update_switch_board_states(_other_switch_board_infos[i], timeout_ms);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'update_switch_board_states' failed: states not updated for board in slot %d of magazine 0x%08X: call_result == %d",
					slot_pos, mag_num, call_result);
		}
		// check if snmp link is down
		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			int64_t  current_time = ::clock();
			if( current_time >= 0){
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LINK DOWN Detected for Switch Board in slot %d of magazine 0x%08X: at time %ld", slot_pos, mag_num, current_time);
				set_scb_link_down_time(slot_pos, mag_num, current_time);
			}else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'time(0) failed: Cannot set last link down time for Switch Board in slot %d <errno == %d>", slot_pos, errno);
		}
		else
			reset_scb_link_down_time(slot_pos, mag_num);
	}
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::update_all_switch_board_states (const unsigned * timeout_ms) {
	update_my_switch_board_states(timeout_ms);
	update_other_switch_board_states(timeout_ms);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::set_warm_restart_on_other_scx () {
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_other_switch_board_found_check()) return call_result;

	int call_result = 0;

	for (size_t i = 0; i < _other_switch_board_infos.size(); ++i) {
		call_result = set_warm_restart_type(_other_switch_board_infos[i]);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'set_warm_restart_type' failed: WARM restart type not set for board in slot %d of magazine 0x%08X: call_result == %d",
					_other_switch_board_infos[i].slot_position, _other_switch_board_infos[i].magazine, call_result);
		}
	}
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}


int __CLASS_NAME__::set_warm_restart_on_all_scx () {
	set_warm_restart_on_my_scx();
	set_warm_restart_on_other_scx();

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::update_switch_board_states (acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms) {
	acs_apbm_snmp::switch_board_shelf_mgr_state_t state;

	int call_result = get_switch_board_status(state, switch_board, timeout_ms);
	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'get_switch_board_status' failed: cannot get the state of the switch board in the slot %d of the magazine 0x%08X: call_result == %d",
				switch_board.slot_position, switch_board.magazine, call_result);
		state = acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN;
	}

	switch_board.state = state;

	if (_server_working_set->cs_reader->is_cba_system()) {
		acs_apbm_snmp::switch_board_neighbour_state_t neighbour_state;

		call_result = get_shelf_neighbour_presence(neighbour_state, switch_board, timeout_ms);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_shelf_neighbour_presence' failed: cannot get the state of the neighbour of "
					"the switch board in the slot %d of the magazine 0x%08X: call_result == %d", switch_board.slot_position, switch_board.magazine, call_result);

			neighbour_state = acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_UNKNOWN;
		}

		switch_board.neighbour_state = neighbour_state;
	}

	return call_result;
}

int __CLASS_NAME__::get_switch_board_status(acs_apbm_snmp::switch_board_shelf_mgr_state_t & shmgr_state, acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms ) {

	const char * switch_board_ipn_str_addresses [2] = { switch_board.ipna_str, switch_board.ipnb_str };
	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_switch_board_status(shmgr_state, switch_board.slot_position, switch_board.magazine, switch_board_ipn_str_addresses[ipn_index], timeout_ms);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'get_switch_board_status' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
		else break;
	}

	return call_result;
}

int __CLASS_NAME__::get_switch_board_status(acs_apbm_snmp::switch_board_shelf_mgr_state_t & shmgr_state, int32_t switch_board_slot, uint32_t switch_board_magazine, const char * switch_board_ip, const unsigned * timeout_ms) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to send Master/Slave status request to switch board %s in slot %d", switch_board_ip, switch_board_slot);

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	int state = 0;

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	char mag_str[16] = {0};
	call_result = _server_working_set->cs_reader->uint32_to_ip_format(mag_str, switch_board_magazine, acs_apbm_programconfiguration::magazine_address_imm_separator);
	if(call_result < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'uint32_to_ip_format' failed. call_result == %d", call_result);
		return acs_apbm_snmp::ERR_SNMP_INTERNAL_ERROR;
	}

	acs_apbm_getshelfmgrcontrolstatecallback callback(state, mag_str, switch_board_slot, switch_board_ip, is_cba);
	acs_apbm_snmpsessionhandler session_handler;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scb [] = { ACS_APBM_SCB_OID_SHELF_MGR_CMD, switch_board_slot };
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_SHELF_MGR_CTRL_STATE, 0 };

	oid * oid_name = 0;
	size_t oid_name_size = 0;

	if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
	else { oid_name = oid_name_scb; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scb); }

	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the switch board Master/Slave status into the OID '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve the switch board Master/Slave status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve the switch board Master/Slave status: call_result == %d",
				call_result);
	}

	if (call_result) { session_handler.close(); return call_result; }

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading the switch board Master/Slave status from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	session_handler.close();

	// Check the status value and update the relative SCXB object
	if(call_result == 0) shmgr_state = static_cast<acs_apbm_snmp::switch_board_shelf_mgr_state_t> (state);

	return call_result;
}

int __CLASS_NAME__::get_managerTrapSubscrIp(acs_apbm_switchboardinfo & switch_board,  const unsigned * timeout_ms){

	const char * switch_board_ipn_str_addresses [2] = { switch_board.ipna_str, switch_board.ipnb_str };
	int call_result = 0;

	int manager_trap_subscr_found = 0;

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; !manager_trap_subscr_found && (i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_managerTrapSubscrIp(manager_trap_subscr_found, switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], timeout_ms);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'get_managerTrapSubscrIp' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}

	return call_result;

}

int __CLASS_NAME__::get_managerTrapSubscrIp (int & manager_trap_subscr_found, int32_t switch_board_slot, const char * switch_board_ip, const unsigned * timeout_ms) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to send get Manager Trap Subscription IP request to switch board %s in slot position %d", switch_board_ip, switch_board_slot);

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	acs_apbm_snmp::axe::eth::shelf::ipAddress_t scb_managerIpaddr = {0};


	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getmanagerIpAddresscallback callback(scb_managerIpaddr, 0, switch_board_slot, switch_board_ip, is_cba);
	acs_apbm_snmpsessionhandler session_handler;

	manager_trap_subscr_found = 0;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scb [] = { ACS_APBM_SCB_OID_XSHMC_MANAGER_IP_ADDRESS, 0 };
	// SCX maybe not needed
	oid oid_name_scx [] = { ACS_APBM_SCX_OID_SHELF_MGR_CTRL_STATE, 0 };

	oid * oid_name = 0;
	size_t oid_name_size = 0;

	if (is_cba) { oid_name = oid_name_scx; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx); }
	else { oid_name = oid_name_scb; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scb); }

	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the manager IP address into the OID '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve the manager IP address");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve the manager IP address: call_result == %d",
					call_result);
	}

	if (call_result) {session_handler.close(); return call_result; }

		// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading the manager IP address from response PDU: "
					"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	session_handler.close();

	manager_trap_subscr_found = 1;

	return call_result;
}


/****   snmp-set handling subscription/unsubscrption requests   ****/

int __CLASS_NAME__::send_trap_subscription_request_scbrp () {
	int call_result = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		call_result = send_trap_subscription_request_scbrp(_my_switch_board_infos[i]);
		if (call_result) {
		//	failure_counter++;
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'send_trap_subscription_request_scbrp' failed: sending traps subscription request failed for switch board in slot %d: call_result == %d",
					_my_switch_board_infos[i].slot_position, call_result);
			}
	}
	return call_result;
} /*+*/

int __CLASS_NAME__::send_trap_subscription_request_scbrp (uint32_t magazine) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	uint32_t boards_found = 0;
	int call_result = 0;

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if (_my_switch_board_infos[i].magazine == magazine) {
			boards_found++;

			call_result = send_trap_subscription_request_scbrp(_my_switch_board_infos[i]);
			if (call_result) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'send_trap_subscription_request_scbrp' failed: sending traps subscription request failed for switch board in my "
					"magazine 0x%08X, slot %d: call_result == %d", magazine, _my_switch_board_infos[i].slot_position, call_result);
			}
		}
	}

	for (size_t i = 0; i < _other_switch_board_infos.size(); ++i) {
		if (_other_switch_board_infos[i].magazine == magazine) {
			boards_found++;

			call_result = send_trap_subscription_request_scbrp(_other_switch_board_infos[i]);
			if (call_result) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'send_trap_subscription_request_scbrp' failed: sending traps subscription request failed for switch board in other "
					"magazine 0x%08X, slot %d: call_result == %d", magazine, _other_switch_board_infos[i].slot_position, call_result);
			}
		}
	}

	if (!boards_found) return acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::send_trap_subscription_request_scbrp (acs_apbm_switchboardinfo & switch_board) {
	char my_ipn_addresses [ACS_APBM_CONFIG_IPN_COUNT][16];

	if (const int call_result = _server_working_set->cs_reader->get_my_ipn_addresses(my_ipn_addresses[0], my_ipn_addresses[1]) ) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'get_my_ipn_addresses' failed: cannot load my AP ipn addresses : call_result == %d", call_result);
		return call_result;
	}
	int call_result = -1;
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (i < 2) && call_result; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_active_manager_trap_subscription(switch_board_ipn_str_addresses[ipn_index], my_ipn_addresses[ipn_index], acs_apbm_snmp::SCBRP_SUBSCRIPTION_REQUEST );
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Call 'send_trap_unsubscription_request_scbrp' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
				switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}
	return call_result;
}

int __CLASS_NAME__::set_active_manager_trap_subscription (
		const char * switch_board_ip,
		const char * trapreceiver_ip, int request_value )
{
		int call_result = 0;

		ACS_APBM_LOG(LOG_LEVEL_INFO,
					"SNMP Manager: Trying to send  trap subscription request to the SCB-RP (IP '%s'): my subscription IP is '%s'",
					 switch_board_ip, trapreceiver_ip);

		acs_apbm_snmploggercallback callback("SNMP Response received for Trap subscription request towards SCBRPs");
		acs_apbm_snmpsessionhandler session_handler;

		if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
			return call_result;
		}

		// The last suboid has to be 0 (zero) and not the SCBRP slot number
		oid oid_trap_subscribe [] = { ACS_APBM_SCB_OID_ACTIVE_MANAGER_TRAP_SUBSCR, 0 };
		size_t oid_name_size = ACS_APBM_ARRAY_SIZE(oid_trap_subscribe);
		char trap_subscribe_value[128];
		unsigned timeout_ms = 2000;

		if(request_value == acs_apbm_snmp::SCBRP_SUBSCRIPTION_REQUEST)
			// 0x7FFFFFFU will subscribe to all traps
			::snprintf(trap_subscribe_value, ACS_APBM_ARRAY_SIZE(trap_subscribe_value), "%s-%u",trapreceiver_ip, 0x7FFFFFFU);
		else
			// 0 will unsubscribe for all traps
			::snprintf(trap_subscribe_value, ACS_APBM_ARRAY_SIZE(trap_subscribe_value), "%s-0", trapreceiver_ip);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Active Manager Trap Subscription': value '%s'", trap_subscribe_value);

		call_result = send_set_request(session_handler, oid_trap_subscribe, oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *> (trap_subscribe_value), ::strlen(trap_subscribe_value), &timeout_ms);

		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
		} else if (call_result) { // ERROR: sending the request
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
		}

		session_handler.close();
		return call_result;
}

int __CLASS_NAME__::initialize_switch_board_state()
{
	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "setting switch board state and neighbour state to UNKNOWN ...  !");

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		call_result = initialize_switch_board_state(_my_switch_board_infos[i]);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'initialize_switch_board_state' failed: initialize state of switch board in slot %d: call_result == %d",
					_my_switch_board_infos[i].slot_position, call_result);
			}
	}

	initialize_my_active_switch_board_index();
	return call_result;
} /*+*/

int __CLASS_NAME__::initialize_switch_board_state (int scbrpSlotPos)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int idx_scb = index_from_slot_position(scbrpSlotPos);

	return initialize_switch_board_state(_my_switch_board_infos[idx_scb]);
} //+

int __CLASS_NAME__::initialize_switch_board_state(acs_apbm_switchboardinfo & switch_board)
{
	int scbrpSlotPos = switch_board.slot_position;

	if ((switch_board.state == acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE) ||
		(_my_active_switch_board_index ==  (int)index_from_slot_position(scbrpSlotPos)))
			initialize_my_active_switch_board_index();

	switch_board.state = acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN;
	switch_board.neighbour_state = acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_UNKNOWN;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::send_no_scan_scbrp()
{
	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;
	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	initialize_switch_board_state();
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, " TRy to send NO SCAN to both switch boards !");

	int scbrpSlotPos;
	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		// TODO: NOSCAN has to be sent?
		scbrpSlotPos = _my_switch_board_infos[i].slot_position;
		if(const int call_result = set_scb_passive(scbrpSlotPos, false)){
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'start_scb_communication' failed: sending NO SCAN to switch board in slot %d: call_result == %d",
					scbrpSlotPos, call_result);
		}
	}
	return call_result;
} /*+*/


int __CLASS_NAME__::start_scb_communication()
{
	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "starting communication with both switch board !");

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		// TODO: NOSCAN has to be sent?
		call_result = start_scb_communication(_my_switch_board_infos[i], true);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'start_scb_communication' failed: starting communication with switch board in slot %d: call_result == %d",
					_my_switch_board_infos[i].slot_position, call_result);
			}
	}
	return call_result;
} /*+*/

int __CLASS_NAME__::start_scb_communication (int scbrpSlotPos, bool sendnoscan)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int idx_scb = index_from_slot_position(scbrpSlotPos);

	return start_scb_communication(_my_switch_board_infos[idx_scb], sendnoscan);
} //+

int __CLASS_NAME__::start_scb_communication(acs_apbm_switchboardinfo & switch_board, bool sendnoscan)
{
	int result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	int scbrpSlotPos = switch_board.slot_position;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "starting communication with switch board at slot position %d!", scbrpSlotPos);
	// TODO: send NO SCAN if (a SCAN has been sent) {
	if(sendnoscan && (result = set_scb_passive(scbrpSlotPos, false))) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_passive' failed: cannot let passive the switch board at slot position %d! <result == %d>", scbrpSlotPos, result);
		// TO DO:
		// reset IPMI: send_set_reset_switch_board(reset_value = 99);
	}

	// unsubscribe  APBM old trap receiver on both subnets
	result = send_trap_unsubscription_request_scbrp (switch_board);
	if (result)
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'send_trap_unsubscription_request_scbrp' failed on both ipns for switch board in slot %d <result == %d>",
						scbrpSlotPos, result);
	// TODO:
	// Unsubscribe for other AP node

	//subscribe APBM trap receiver on both subnets
	result = send_trap_subscription_request_scbrp (switch_board);
	if (result)
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'send_trap_subscription_request_scbrp' failed on both ipns for switch board in slot %d <result == %d>",
							scbrpSlotPos, result);

	return result;
}

int __CLASS_NAME__::switch_active_state_scbrp()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int result;
	int idx_scb = get_my_active_switch_board_index();

	if (idx_scb == acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD ) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Cannot switch active state: no current active switch board found!");
		return  acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD;
	}
	int new_passive_scb_pos = _my_switch_board_infos[idx_scb].slot_position;
	int16_t new_active_scb_pos = _my_switch_board_infos[idx_scb ^ 1].slot_position;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to switch active state from the switch board at slot position %d to switch board at slot position %d!", new_passive_scb_pos, new_active_scb_pos );
	//  send NO SCAN  to current scbrp to let it passive on IPMI bus
	if ((result = set_scb_passive(new_passive_scb_pos))) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_passive' failed: cannot let passive the switch board at slot position %d!", new_passive_scb_pos);
		// TO DO ?
	}
	// send SCAN to other switch board to let it master on IPMI bus
	if ((result = set_scb_active(new_active_scb_pos))) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_passive' failed: cannot let passive the switch board at slot position %d!", new_active_scb_pos);
				// TO DO ?
	}
	ACS_APBM_LOG(	LOG_LEVEL_INFO, "initialize_switch_board_state for switch board at slot position %d!", new_passive_scb_pos);
	initialize_switch_board_state(new_passive_scb_pos);
	result = start_scb_communication(new_passive_scb_pos, false);
	result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", new_passive_scb_pos, result);

	return result;
} //+

int __CLASS_NAME__::switch_from_active_state_scbrp(acs_apbm_switchboardinfo & switch_board )
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);


	int idx_active_scb = get_my_active_switch_board_index();
	int scb_active_pos = slot_position_from_index(idx_active_scb);

	if (switch_board.state != acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "The switch_board_%d is not master: current switch board master is switch_board_%d: no switch from active state is executed!", switch_board.slot_position, scb_active_pos);
		return  acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	}
	int slot_pos = switch_board.slot_position;
	int idx_scb = index_from_slot_position(slot_pos);
	int16_t new_active_scb_pos = _my_switch_board_infos[idx_scb ^ 1].slot_position;

	ACS_APBM_LOG(	LOG_LEVEL_INFO, "initialize_switch_board_state of switch board at slot position %d!", slot_pos);
	initialize_switch_board_state(slot_pos);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to switch active state from the switch board_%d to switch board_%d!", slot_pos, new_active_scb_pos );
	// send SCAN to other switch board to let it master on IPMI bus
	int result;
	if ((result = set_scb_active(new_active_scb_pos))) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_active' failed: cannot let active the switch board_%d!", new_active_scb_pos);
				// TO DO ?
	}

	return result;
} //+

/***************************************************/
/****   methods to manage snmp incoming traps  ****/
/***************************************************/

int __CLASS_NAME__::manage_ShelfMgrCtrl_scbrp (int /*magPlugNumber*/, int scbrpSlotPos, int shelfMgrState, int neighbour, const char * switch_board_ip) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Increment the counter that store how many times the shelf manager control trap was sent to APBM when subscribing to SNMP agent
	if(scbrpSlotPos == acs_apbm::BOARD_SCB_LEFT)
	_shelf_manager_control_left_scb_trap_counter_++;
	if(scbrpSlotPos == acs_apbm::BOARD_SCB_RIGHT)
		_shelf_manager_control_right_scb_trap_counter_++;

	int result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int idx_scb = index_from_slot_position(scbrpSlotPos);
	int16_t other_scbrpSlotPos = _my_switch_board_infos[idx_scb ^ 1].slot_position;

	char ipna_str[16], ipnb_str[16], ap_ipn[16];
	_server_working_set->cs_reader->get_my_ipn_addresses(ipna_str, ipnb_str);

	if(::strcmp(switch_board_ip,_my_switch_board_infos[idx_scb].ipna_str) == 0)
		::strncpy(ap_ipn, ipna_str, ACS_APBM_ARRAY_SIZE(ap_ipn));
	else if(::strcmp(switch_board_ip,_my_switch_board_infos[idx_scb].ipnb_str) == 0)
		::strncpy(ap_ipn, ipnb_str, ACS_APBM_ARRAY_SIZE(ap_ipn));
	else{
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"WARNING! - Index == %d, Switch Board source IP == %s,  OtherSlotPosition == '%d', AP_IPNA == '%s', AP_IPNB == '%s', AP_IPN == '%s'",
				idx_scb,  switch_board_ip, other_scbrpSlotPos, ipna_str, ipnb_str, ap_ipn);
		return result;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Index == %d, Switch Board source IP == %s,  OtherSlotPosition == '%d', AP_IPNA == '%s', AP_IPNB == '%s', AP_IPN == '%s'",
			idx_scb,  switch_board_ip, other_scbrpSlotPos, ipna_str, ipnb_str, ap_ipn);

	// send ack to the SCB board using ap_ipn
	send_trap_ack(scbrpSlotPos, ap_ipn, switch_board_ip);

	switch(shelfMgrState) {
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE:
			result = handle_ShelfMgrCtrl_passive( scbrpSlotPos, neighbour, switch_board_ip);
		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE:
			result = handle_ShelfMgrCtrl_active ( scbrpSlotPos, neighbour, switch_board_ip);
		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_IDLE:
			result = handle_ShelfMgrCtrl_idle ( scbrpSlotPos, neighbour, switch_board_ip);
		break;
		default:
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"unexpected shelfMgrState received: shelfMgrState == %d",shelfMgrState);
		break;
	}

	return result;
} //+

int __CLASS_NAME__::handle_ShelfMgrCtrl_idle (int scbrpSlotPos, int neighbour_state, const  char *switch_board_ip)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	int result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Handling ShelfMgrCtrl_idle (slot position == %d neighbour == %d)!", scbrpSlotPos, neighbour_state);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int idx_scb = index_from_slot_position(scbrpSlotPos);
//	acs_apbm_snmp::switch_board_shelf_mgr_state_t scb_current_state = _my_switch_board_infos[idx_scb].state;
//	acs_apbm_snmp::switch_board_neighbour_state_t neighbour_current_state = _my_switch_board_infos[idx_scb].neighbour_state;
	int scb_current_state = _my_switch_board_infos[idx_scb].state;
	int neighbour_current_state = _my_switch_board_infos[idx_scb].neighbour_state;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "The SWITCH BOARD in slot %d has current state == %d and current_neighbour == %d", scbrpSlotPos, scb_current_state, neighbour_current_state);
	// update neighbour state
	_my_switch_board_infos[idx_scb].neighbour_state = (neighbour_state ? acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT: acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT);
	switch(scb_current_state) {
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN:
			//send noscan to this scbrp
			ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM will try to send NOSCAN to switch_board_%d", scbrpSlotPos);
			if ((result = set_shelf_mngr_ctrl_state_scbrp(scbrpSlotPos, switch_board_ip, acs_apbm_snmp::SHELF_MNGR_CTRL_STATE_NOT_SCAN))){
				ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_shelf_mngr_ctrl_state_scbrp' failed: cannot send NO SCAN to the switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
			}
			else
				set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_IDLE);
		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE:
			if (neighbour_current_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT &&
						neighbour_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT ){
				ACS_APBM_LOG(LOG_LEVEL_INFO, "The received neighbour state is different from current one: APBM will try to assign active state to other switch board");
				set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_IDLE);
				//assign IPMI master to other switch board
				result = switch_active_state_scbrp();
				result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to switch master status  <result == %d>!", result);
			}
			else{
				ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM will try to restart communication with switch board");
				//set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
				initialize_switch_board_state(scbrpSlotPos);
				result = start_scb_communication(scbrpSlotPos, true);
				result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);

			}
		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE:
			ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM will try to restart communication with switch board");
			// TODO: verify this behaviour!!!
			//set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
			initialize_switch_board_state(scbrpSlotPos);
			result = start_scb_communication(scbrpSlotPos);
			result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_IDLE:
			ACS_APBM_LOG(LOG_LEVEL_INFO, "The switch_board_%d is already in idle state", scbrpSlotPos);
			break;
		default:
			ACS_APBM_LOG(LOG_LEVEL_WARN, "The switch_board_%d is in unknown state <state == %d>", scbrpSlotPos, scb_current_state);
		break;
	}
	return result;
} //+


int __CLASS_NAME__::handle_ShelfMgrCtrl_active (int scbrpSlotPos, int neighbour_state, const char * switch_board_ip)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Handling ShelfMgrCtrl_active (slot position == %d neighbour == %d)!", scbrpSlotPos, neighbour_state);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	int idx_scb = index_from_slot_position(scbrpSlotPos);
//	acs_apbm_snmp::switch_board_shelf_mgr_state_t scb_current_state = _my_switch_board_infos[idx_scb].state;
//	acs_apbm_snmp::switch_board_neighbour_state_t neighbour_current_state = _my_switch_board_infos[idx_scb].neighbour_state;
	int scb_current_state = _my_switch_board_infos[idx_scb].state;
	int neighbour_current_state = _my_switch_board_infos[idx_scb].neighbour_state;

	//update neighbour state
	_my_switch_board_infos[idx_scb].neighbour_state = ((neighbour_state) ? acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT: acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT);
	int16_t other_scbrpSlotPos = _my_switch_board_infos[idx_scb ^ 1].slot_position;

	// if STATE != MASTER_SET{
	// send scan
	// send no scan to other
	// STATE = MASTER_SET
	// store which scb is active
	//}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "The SWITCH BOARD in slot %d has current state == %d and current_neighbour == %d", scbrpSlotPos, scb_current_state, neighbour_current_state);

	int check_result = check_working_state_active_sb();
	check_result && ACS_APBM_LOG(LOG_LEVEL_INFO, " Call 'check_working_state_active_sb' failed <check_result == %d>!", check_result);

	// The mastership has not been assigned
	if (check_result == acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD){
		ACS_APBM_LOG(LOG_LEVEL_INFO, "The IPMI active state is not assigned: APBM will try to assign active state to switch_board_%d", scbrpSlotPos);
		if((result = set_scb_passive(other_scbrpSlotPos))) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_passive' failed: cannot let passive the switch board at slot position %d <result == %d>!", other_scbrpSlotPos, result);
			// TO DO ?
		}
		if ((result = set_scb_active(scbrpSlotPos))) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_active' failed: cannot let active the switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
			// TO DO ?
		}
		else {
			ACS_APBM_LOG(LOG_LEVEL_INFO, " The IPMI active state has been assigned to switch board in slot %d, APBM will try to get Board data from SNMP agent", scbrpSlotPos);
			// Now I can order a 'load board data from SNMP with IMM' operation to load from SNMP agent and create boards in memory and IMM
			// schedule operation in order to update AP boards info
			const int call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM,2);
			if(call_result < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to schedule operation 'SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM'. Call 'acs_apbm_operationpipescheduler::schedule_operation()' failed. call_result == %d", call_result);
		}
		return result;
	}

	switch (scb_current_state){
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN:
			switch (check_result){
				case acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD:// The mastership has not been assigned
					// it must not be occurs because this case is be handled out of switch!!!
				break;
				case acs_apbm_snmp::ERR_SNMP_IPMI_ACTIVE_NOTWORKING:  // the mastership is assigned to other board but ipmi bus doesn't work
					ACS_APBM_LOG(LOG_LEVEL_INFO, " The IPMI BUS is not working on the active switch board, APBM will try to assign active state to the other switch board");
					result = switch_active_state_scbrp();
					result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to switch active status <result == %d>!", result);
				break;
				case acs_apbm_snmp::ERR_SNMP_NO_ERRORS: // the mastership is assigned to other board and it is working fine
					ACS_APBM_LOG(LOG_LEVEL_INFO, "the IPMI active state is already assigned!");
					if (neighbour_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT )
						ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM will try to send NOSCAN to switch_board_%d", scbrpSlotPos);
						if ((result = set_shelf_mngr_ctrl_state_scbrp(scbrpSlotPos, switch_board_ip, acs_apbm_snmp::SHELF_MNGR_CTRL_STATE_NOT_SCAN))){
							ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_shelf_mngr_ctrl_state_scbrp' failed: cannot send NO SCAN to the switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
						}
						set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_IDLE);
					break;
				default:  // it must not occurs!!
					ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'check_working_state_active_sb'  returns an unexpected error");
				break;
			}
		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE:
			// The other board is but it is not working on IMPI bus
			if (check_result == acs_apbm_snmp::ERR_SNMP_IPMI_ACTIVE_NOTWORKING &&
					neighbour_current_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT &&
					neighbour_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT)
			{
				ACS_APBM_LOG(LOG_LEVEL_INFO, "the IPMI Bus on active switch board is not working fine:  APBM will try to assign active state to the other switch board!");
				result = switch_active_state_scbrp();
				result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to switch active status <result == %d>!", result);
			}
			else if(neighbour_current_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT &&
					neighbour_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT) {
				initialize_switch_board_state(scbrpSlotPos);
				result = start_scb_communication(scbrpSlotPos,true);
				result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
			}
			else

				ACS_APBM_LOG(LOG_LEVEL_INFO, "The other switch board is active and IPMI Bus is working fine: APBM will do ... nothing!");
			// else nothing TO DO

		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_IDLE:
			if (neighbour_current_state != neighbour_state) {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "The received neighbour state is different from current neighbour state: APBM will try to restart communication with switch board!!");
				//set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
				initialize_switch_board_state(scbrpSlotPos);
				result = start_scb_communication(scbrpSlotPos,true);
				result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
				return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
			}
			switch (check_result){
				case acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD:// The mastership has not been assigned
					ACS_APBM_LOG(LOG_LEVEL_INFO, "The IPMI active state is not assigned: APBM will try to assign active state to switch_board_%d", scbrpSlotPos);
					if((result = set_scb_passive(other_scbrpSlotPos))) {
						ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_passive' failed: cannot let passive the switch board at slot position %d <result == %d>!", other_scbrpSlotPos, result);
						// TO DO ?
					}
					if ((result = set_scb_active(scbrpSlotPos))) {
						ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_scb_active' failed: cannot let active the switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
						// TO DO ?
					}
				break;
				case acs_apbm_snmp::ERR_SNMP_IPMI_ACTIVE_NOTWORKING:
					ACS_APBM_LOG(LOG_LEVEL_INFO, "The IPMI BUS is not working on the active switch board, APBM will try to assign active state to the other switch board");
					result = switch_active_state_scbrp();
					result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to switch active status <result == %d>!", result);
				break;
				default:  // the other board is active and working
					if (neighbour_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT ){
						ACS_APBM_LOG(LOG_LEVEL_INFO, "the received neighbour state is %d: APBM will try to restart communication with switch board!", neighbour_state);
						//set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
						initialize_switch_board_state(scbrpSlotPos);
						result = start_scb_communication(scbrpSlotPos,true);
						result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
					}
					else
						ACS_APBM_LOG(LOG_LEVEL_INFO, "The other switch board is active and IPMI Bus is working fine: APBM will do ... nothing!");
				break;
			}
		break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE:

			if(neighbour_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT &&
					neighbour_current_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT){
				//ONLY FOR TEST master manual assignment
				/*if(check_ipmi_state(scbrpSlotPos) == acs_apbm_snmp::ERR_SNMP_IPMI_ACTIVE_NOTWORKING){
				if(check_result == acs_apbm_snmp::ERR_SNMP_IPMI_ACTIVE_NOTWORKING){
					ACS_APBM_LOG(LOG_LEVEL_INFO, "The IPMI BUS is not working on the active switch board, APBM will try to assign active state to the other switch board");
					result = switch_active_state_scbrp();
					result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to switch active status <result == %d>!", result);
				}*/
				//else {
					ACS_APBM_LOG(LOG_LEVEL_INFO, "IPMI bus on the active switch board is working fine: APBM will try to restart communication with other switch board!");
					//set_switch_board_state(idx_scb ^ 1, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
					initialize_switch_board_state(other_scbrpSlotPos);
					result = start_scb_communication(other_scbrpSlotPos, false);
					result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", other_scbrpSlotPos, result);
				//}
			}
			else
				ACS_APBM_LOG(LOG_LEVEL_INFO, "The switch board is already active and IPMI Bus is working fine: APBM will do ... nothing!");
		break;
		default:
			ACS_APBM_LOG(LOG_LEVEL_WARN, "The switch_board_%d is in unknown state <state == %d>", scbrpSlotPos, scb_current_state);
			// TODO: needs to be initialized? (it will be investigated!!!)
		break;
	}

	return result;
} //+

int __CLASS_NAME__::handle_ShelfMgrCtrl_passive( int scbrpSlotPos, int neighbour, const  char * /*switch_board_ip*/)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	int idx_scb = index_from_slot_position(scbrpSlotPos);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Handling ShelfMgrCtrl_passive (slot position == %d neighbour == %d)!", scbrpSlotPos, neighbour);
	//int16_t other_scbrpSlotPos = _my_switch_board_infos[idx_scb ^ 1].slot_position;
//	acs_apbm_snmp::switch_board_shelf_mgr_state_t switch_board_current_state = _my_switch_board_infos[idx_scb].state;
//	acs_apbm_snmp::switch_board_neighbour_state_t neighbour_current_state = _my_switch_board_infos[idx_scb].neighbour_state;
	int switch_board_current_state = _my_switch_board_infos[idx_scb].state;
	int neighbour_current_state = _my_switch_board_infos[idx_scb].neighbour_state;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "The SWITCH BOARD in slot %d has current state == %d and current_neighbour == %d", scbrpSlotPos, switch_board_current_state, neighbour_current_state);
	// update neighbour state
	_my_switch_board_infos[idx_scb].neighbour_state = ((neighbour) ? acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT: acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT);

	switch(switch_board_current_state) {
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE:
			// TODO:
			// 1. send no scan
			// 2. set switch board state to SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN
			// 3. reinitialize SCB-RP communication
			ACS_APBM_LOG(LOG_LEVEL_INFO, "The switch board_%d is no longer active: APBM will try to restart the communication!", scbrpSlotPos);
			//set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
			initialize_switch_board_state(scbrpSlotPos);
			result = start_scb_communication(scbrpSlotPos, true);
			result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
			break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE:
			if (neighbour == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_ABSENT &&
					 neighbour_current_state == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT){
				ACS_APBM_LOG(LOG_LEVEL_INFO, "The received neighbour state is different from current neighbour state: APBM will try to restart the communication!", scbrpSlotPos);
				//set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
				initialize_switch_board_state(scbrpSlotPos);
				result = start_scb_communication(scbrpSlotPos);
				result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
			}
			else
				ACS_APBM_LOG(LOG_LEVEL_INFO, "The switch board is already in passive state: APBM will do ... nothing!");
			break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_IDLE:
			ACS_APBM_LOG(LOG_LEVEL_INFO, "The current state is idle: APBM will do ... nothing!");
			// if current state  is idle
			// nothing to do
			set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE);
			break;
		case acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN:
			if (neighbour == acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_PRESENT) {
				set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE);
			}
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Received neighbour state is 'absent': APBM will try to restart the communication with switch board in the slot %d!", scbrpSlotPos);
				initialize_switch_board_state(scbrpSlotPos);
				result = start_scb_communication(scbrpSlotPos);
				result && ACS_APBM_LOG(LOG_LEVEL_WARN, "Failed to start communication with switch board at slot position %d <result == %d>!", scbrpSlotPos, result);
			}
			break;
		default:
			ACS_APBM_LOG(LOG_LEVEL_WARN, "The switch_board_%d is in undefined state <state == %d>", scbrpSlotPos, switch_board_current_state);
			break;
	}
    return result;
} //+


// Manage ShelfMgrCtrl from SCX board
int __CLASS_NAME__::manage_ShelfMgrCtrl_scx (int /*scbSlotPos */, int shelfMgrState, int neighbour, const char * switch_board_ip) {


	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	acs_apbm_switchboardinfo * p_scxb_info = 0;
	bool scb_is_in_local_magazine = false;
	if((call_result = get_switch_board_info_ptr_by_ip(p_scxb_info, scb_is_in_local_magazine, switch_board_ip)) == acs_apbm::ERR_NO_ERRORS)
	{
		call_result = set_switch_board_state(p_scxb_info->slot_position, p_scxb_info->magazine, shelfMgrState);
		if(call_result) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'update_switch_board_state()' failed ! call_result == %d", call_result);
				call_result = acs_apbm_snmp::ERR_SNMP_INTERNAL_ERROR;
		}

		p_scxb_info->neighbour_state = (neighbour == 0)? acs_apbm_snmp::NEIGHBOUR_ABSENT: acs_apbm_snmp::NEIGHBOUR_PRESENT;

		// schedule operation in order to update AP boards info
		call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM,2);
		if(call_result < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to schedule operation 'SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM'. Call 'acs_apbm_operationpipescheduler::schedule_operation()' failed. call_result == %d", call_result);

		// update HWI data
		bool err_flag = false;
		char mag_str[16] = {0};
		call_result = _server_working_set->cs_reader->uint32_to_ip_format(mag_str, p_scxb_info->magazine, acs_apbm_programconfiguration::magazine_address_imm_separator);
		if((err_flag = (call_result < 0)))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'acs_apbm_csreader::uint32_to_ip_format()' failed. call_result == %d", call_result);

		if(!err_flag)
		{
			// get data from SCXB
			call_result = _server_working_set->hwi_handler->startCollectionOnBoard(mag_str, p_scxb_info->slot_position);
			if((err_flag = (call_result < 0)))
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to start collection of HWI data for magazine %s. Call 'ACS_APBM_HWIHandler::startCollectionOn()' failed. call_result == %d", mag_str, call_result);
		}

		if(!err_flag)
		{
			// store HWI data in IMM
			call_result = _server_working_set->hwi_handler->storeMagazine(mag_str);
			if(call_result < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to store IMM data for magazine %s. Call 'ACS_APBM_HWIHandler::storeMagazine()' failed. call_result == %d", mag_str, call_result);
		}
	}
	else
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_switch_board_info_by_ip()' failed ! Searched IP is %s. call_result == %d ", switch_board_ip, call_result);

	return call_result;
}



int __CLASS_NAME__::manage_SELEntry(int magPlugNumber, int slotPos, const char * message) {
	char scbIP_0[16] = {0};
	char scbIP_1[16] = {0};

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	::strncpy(scbIP_0, _my_switch_board_infos[0].ipna_str, 16);
	::strncpy(scbIP_1, _my_switch_board_infos[1].ipna_str, 16);

	char ipna_str[16], ipnb_str[16];
	_server_working_set->cs_reader->get_my_ipn_addresses(ipna_str, ipnb_str);
	//acs_apbm_utils::get_my_ipn_addresses(ipna_str, ipnb_str);

	// send ack to both SCB boards using IPNA
	send_trap_ack(acs_apbm_programconfiguration::slot_position_switch_board_left, ipna_str, scbIP_0);
	send_trap_ack(acs_apbm_programconfiguration::slot_position_switch_board_right, ipna_str, scbIP_1);

	// prepare TRAP to be notified
	trapdata trap;
	trap.trapType = acs_apbm_trapmessage::SEL_ENTRY;
	trap.values.push_back(magPlugNumber);
	trap.values.push_back(slotPos);
	trap.message_len = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
	message && memcpy(trap.message, message, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);

	// notify the TRAP to the SUBSCRIPTION MANAGER
	_server_working_set->trap_subscription_manager->notifyTrapInfo(slotPos, trap);
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trap notify about the board at slot position %d was successfully sent!", slotPos);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::manage_BoardPresence (int magPlugNumber, int slotPos, int hwBoardPresence, int busType, const char * switch_board_ip, const char * message){
//	char scbIP_0[16] = {0};
//	char scbIP_1[16] = {0};

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	char ipna_str[16], ipnb_str[16], ap_ipn[16];
	_server_working_set->cs_reader->get_my_ipn_addresses(ipna_str, ipnb_str);

	if(::strncmp(switch_board_ip, ACS_APBM_IPNA_STR, ACS_APBM_IPN_STR_LEN) == 0)
		::strncpy(ap_ipn, ipna_str, ACS_APBM_ARRAY_SIZE(ap_ipn));
	else
		::strncpy(ap_ipn, ipnb_str, ACS_APBM_ARRAY_SIZE(ap_ipn));

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"Switch Board source IP == '%s', AP_IPNA == '%s', AP_IPNB == '%s', AP_IPN == '%s'",
			switch_board_ip, ipna_str, ipnb_str, ap_ipn);

	// send ack to the SCB board using ap_ipn
	send_trap_ack(slotPos, ap_ipn, switch_board_ip);

	// prepare TRAP to be notified
	trapdata trap;
	trap.trapType = acs_apbm_trapmessage::BOARD_PRESENCE;
	trap.values.push_back(magPlugNumber);
	trap.values.push_back(slotPos);
	trap.values.push_back(hwBoardPresence);
	trap.values.push_back(busType);
	trap.message_len = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
	message && memcpy(trap.message, message, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);

	// notify the TRAP to the SUBSCRIPTION MANAGER
	_server_working_set->trap_subscription_manager->notifyTrapInfo(slotPos, trap);
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trap notify about the board at slot position %d was successfully sent!", slotPos);

	axe_eth_shelf::status_t status = axe_eth_shelf::BOARD_STATUS_UNDEFINED;

	int isCBA = _server_working_set->cs_reader->is_cba_system();

	if(isCBA && (slotPos == acs_apbm_programconfiguration::slot_position_switch_board_left || slotPos == acs_apbm_programconfiguration::slot_position_switch_board_right ))
	{
		if(!_server_working_set->hwi_handler->isCollectionOngoing()) //FOR SCX or SMX
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ordering an SNMP_HWI_RELOAD_DATA to reload inventory data");
			if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_RELOAD_DATA))
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'operation_send' failed: cannot order a SNMP_HWI_RELOAD_DATA operation to reload hardware inventory data");
			return 0;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Collection already Ongoing...skipped!");
			return 0;
		}
	}


	switch(hwBoardPresence)
	{
	case axe_eth_shelf::HW_BOARD_PRESENCE_INSERTED:
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'INSERTED' in slot '%d'", slotPos);

		if(slotPos != acs_apbm_programconfiguration::slot_position_switch_board_left &&
			slotPos != acs_apbm_programconfiguration::slot_position_switch_board_right )
		{
                     if((_server_working_set->shelves_data_manager->get_ap_board_status(status,slotPos)) != 0 ) break;
                     if(status != axe_eth_shelf::BOARD_STATUS_WORKING){
                         _server_working_set->shelves_data_manager->update_ap_board_info(true, slotPos);
                         _server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slotPos);
                     }
                     else
                       ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No needed update board status in slot '%d'", slotPos);
		} else
		{
		    ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Ignore update board presence trap in slot '%d'", slotPos);
		    /*
		    //This Behaviour must be verified
			initialize_switch_board_state(slotPos);
			int result = start_scb_communication(slotPos);
			result && ACS_APBM_LOG(LOG_LEVEL_WARN,"Cannot start communication with switch board in slot '%d' result == %d", slotPos, result);
			*/
		}
		break;
	case axe_eth_shelf::HW_BOARD_PRESENCE_ACCESSIBLE:
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'ACCESSIBLE' in slot '%d'", slotPos);
		while (isCBA) {
                        int call_result = 0;
                        uint32_t board_magazine = 0;
		
                        if ((call_result = _server_working_set->cs_reader->map_magazine_plug_number(board_magazine, magPlugNumber))) {
                                // ERROR: mapping magazine plug number into CS version
                                ACS_APBM_LOG(LOG_LEVEL_ERROR,
                                                "HWI Update [Board ACCESSIBLE]: Call 'map_magazine_plug_number' failed: mapping magazine plug "
                                                "number into CS version: magPlugNumber == 0x%08X, call_result == %d", magPlugNumber, call_result);
                                break;
                        }

                        char board_magazine_str [16] = {0};

                        if ((call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine))) {
                                // ERROR: while converting
                                ACS_APBM_LOG(LOG_LEVEL_ERROR,
                                                "HWI Update [Board ACCESSIBLE]: Call 'uint32_to_ip_format' failed: converting board magazine into string: "
                                                "magPlugNumber == 0x%08X, board_magazine == 0x%08X, call_result == %d",
                                                magPlugNumber, board_magazine, call_result);
                                break;
                        }

                        call_result = _server_working_set->hwi_handler->startCollectionOnBoard(board_magazine_str, slotPos);
                        if (call_result) { // ERROR: getting information on board from SNMP agent
                                ACS_APBM_LOG(LOG_LEVEL_ERROR,
                                                "HWI Update [Board ACCESSIBLE]: call 'startCollectionOnBoard' failed: magPlugNumber == 0x%08X, "
                                                "board_magazine == 0x%08X, board_magazine_str == '%s', board_slot == %u: call_result == %d",
                                                magPlugNumber, board_magazine, board_magazine_str, slotPos, call_result);
                                break;
                        }

                        call_result = _server_working_set->hwi_handler->storeBoard(board_magazine_str, slotPos);
			if (call_result) { // ERROR: storing the board into the hardware inventory
                                ACS_APBM_LOG(LOG_LEVEL_ERROR,
                                                "HWI Update [Board ACCESSIBLE]: Storing board into hardware inventory failed: magPlugNumber == 0x%08X, "
                                                "board_magazine == 0x%08X, board_magazine_str == '%s', board_slot == %u: call_result == %d",
                                                magPlugNumber, board_magazine, board_magazine_str, slotPos, call_result);
                                break;
                        }

                        ACS_APBM_LOG(LOG_LEVEL_INFO, "HWI Update [Board ACCESSIBLE]: Hardware Inventory successfully updated: "
                                        "board inserted in slot %d of magazine '%s'", slotPos, board_magazine_str);

                        break;
			}
		break;
	case axe_eth_shelf::HW_BOARD_PRESENCE_NOT_ACCESSIBLE:
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'NOT_ACCESSIBLE' in slot '%d'", slotPos);
		break;
	case axe_eth_shelf::HW_BOARD_PRESENCE_REMOVED:
		ACS_APBM_LOG(LOG_LEVEL_INFO,"Board presence received == 'REMOVED' in slot '%d'", slotPos);

		while (isCBA) {
			int call_result = 0;
			uint32_t board_magazine = 0;

			if ((call_result = _server_working_set->cs_reader->map_magazine_plug_number(board_magazine, magPlugNumber))) {
				// ERROR: mapping magazine plug number into CS version
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"HWI Update [Board Removed]: Call 'map_magazine_plug_number' failed: mapping magazine plug "
						"number into CS version: magPlugNumber == 0x%08X, call_result == %d", magPlugNumber, call_result);
				break;
			}

			char board_magazine_str [16] = {0};

			if ((call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine))) {
				// ERROR: while converting
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"HWI Update [Board Removed]: Call 'uint32_to_ip_format' failed: converting board magazine into string: "
						"magPlugNumber == 0x%08X, board_magazine == 0x%08X, call_result == %d",
						magPlugNumber, board_magazine, call_result);
				break;
			}

			call_result = _server_working_set->hwi_handler->removeBoard(board_magazine_str, slotPos);
			if (call_result) // ERROR: storing the board into the hardware inventory
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"HWI Update [Board Removed]: Removing board from hardware inventory failed: magPlugNumber == 0x%08X, "
						"board_magazine == 0x%08X, board_magazine_str == '%s', board_slot == %u: call_result == %d",
						magPlugNumber, board_magazine, board_magazine_str, slotPos, call_result);

			call_result = _server_working_set->hwi_handler->removeBoardFromIMM(board_magazine_str, slotPos);
			if (call_result) // ERROR: removing the board from the IMM
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "HWI Update [Board Removed]: call 'RemoveBoardFromImm' failed: "
						"magPlugNumber == 0x%08X, board_magazine == 0x%08X, board_magazine_str == '%s', "
						"board_slot == %u: call_result == %d", magPlugNumber, board_magazine, board_magazine_str,
						slotPos, call_result);

			ACS_APBM_LOG(LOG_LEVEL_INFO, "HWI Update [Board Removed]: Hardware Inventory successfully updated: "
					"board removed in slot %d of magazine '%s'", slotPos, board_magazine_str);

			break;
		}

		if((_server_working_set->shelves_data_manager->get_ap_board_status(status,slotPos)) != 0 ) break;
		if(status != axe_eth_shelf::BOARD_STATUS_MISSING){
		_server_working_set->shelves_data_manager->reset_ap_board_info(true, slotPos);
		//  The board has been removed raise  BOARD MISSING ALARM !!
		_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slotPos);

		} else {
		  ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No needed reset board status in slot '%d'", slotPos);
		}
		break;
	}
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::manage_SensorStateChange (int magPlugNumber, int slotPos, int sensorType, int sensorID, int sensorTypeCode, const char * sensorEventData) {
	char scbIP_0[16] = {0};
	char scbIP_1[16] = {0};

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	strncpy(scbIP_0, _my_switch_board_infos[0].ipna_str, 16);
	strncpy(scbIP_1, _my_switch_board_infos[1].ipna_str, 16);

	char ipna_str[16], ipnb_str[16];
	_server_working_set->cs_reader->get_my_ipn_addresses(ipna_str, ipnb_str);
	//acs_apbm_utils::get_my_ipn_addresses(ipna_str, ipnb_str);

	// send ack to both SCB boards using IPNA
	send_trap_ack(acs_apbm_programconfiguration::slot_position_switch_board_left, ipna_str, scbIP_0);
	send_trap_ack(acs_apbm_programconfiguration::slot_position_switch_board_right, ipna_str, scbIP_1);

	// prepare TRAP to be notified
	trapdata trap;
	trap.trapType = acs_apbm_trapmessage::SENSOR_STATE_CHANGE;
	trap.values.push_back(magPlugNumber);
	trap.values.push_back(slotPos);
	trap.values.push_back(sensorType);
	trap.values.push_back(sensorID);
	trap.values.push_back(sensorTypeCode);
	trap.message_len = acs_apbm::PCP_FIELD_SIZE_TRAP_MSG;
	sensorEventData && memcpy(trap.message, sensorEventData, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG);

	// notify the TRAP to the SUBSCRIPTION MANAGER
	_server_working_set->trap_subscription_manager->notifyTrapInfo(slotPos, trap);
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trap notify about the board at slot position %d was successfully sent!", slotPos);

	if (sensorType == 1)
	{
		int temp_status = 1;
		char temperature[acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1] = {0};
		memcpy(temperature, sensorEventData, acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SENSOR_EVENT_DATA =='%02X'", ((unsigned char *)temperature)[0]);
		// Set to critical when SensorEventData1 value is 0x02,0x09,0x0B,0x04,0x07 else OK
		if(( *sensorEventData == 0x09)||( *sensorEventData == 0x0B)||( *sensorEventData == 0x04)||( *sensorEventData == 0x02)||( *sensorEventData == 0x07))
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SENSOR_EVENT_DATA CRITICAL");
			strcpy(temperature,"CRT");
			if(( *sensorEventData == 0x02)||( *sensorEventData == 0x07))
				temp_status = 2;
		}
		else
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SENSOR_EVENT_DATA OK");
			strcpy(temperature,"OK");
			temp_status = 0;
		}
		manage_APUB_temperature(magPlugNumber,slotPos,temp_status);
		_server_working_set->shelves_data_manager->update_ap_board_temperature_status(temperature, true, slotPos);
	}
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::manage_ColdStart(const char * switch_board_ip)
{	
	// start SCXB/SMXB log files collection
	scxColdRestartFlag = false;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to start switch log files collection (from switch having IP == %s) ... ", switch_board_ip);
	int count = 0;
	int call_result = 0;
	int env=0;
	_server_working_set->cs_reader->get_environment(env);

	if(isEgem2L2Switch(switch_board_ip))
	{
		if((env == ACS_CS_API_CommonBasedArchitecture::SCX)|| (env == ACS_CS_API_CommonBasedArchitecture::SMX))
		{
			call_result = sendEgem2L2SwitchLog(switch_board_ip);
			if(call_result != 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "SCXB/SMXB log files collection failed ! Call 'sendEgem2L2SwitchLog()' returned %d !", call_result);
			else
				ACS_APBM_LOG(LOG_LEVEL_INFO, "SCXB/SMXB log files collection successfully started !");
		}		
	}
	
	// update COLD/WARM START counter and check if threshold has been reached. If so, we must trigger the following actions :
	//	1) re-initialize communication with the SCXB which originated the trap;
	//	2) schedule operation to reload AP boards info and update IMM;
	//  3) refresh HWI data and update IMM
	acs_apbm_switchboardinfo * p_switchb_info = 0;
	bool scb_is_in_local_magazine = false;
	if((call_result = get_switch_board_info_ptr_by_ip(p_switchb_info, scb_is_in_local_magazine, switch_board_ip)) == acs_apbm::ERR_NO_ERRORS)
	{
		// update COLD/WARM START counter
		//++p_scxb_info->coldstart_cnt;
		//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "COLD/WARM START counter updated for SCXB having IP == '%s'. New value is '%u'", switch_board_ip, p_scxb_info->coldstart_cnt);

		// check if threshold has been reached ( as in baseline)
		// In this new implementation the check has been disabled because we thought it is wrong!!!
		// the behaviour will be observed to decide if it is necessary or not.
	//	if(p_scxb_info->coldstart_cnt == acs_apbm_programconfiguration::scxb_coldstart_cnt_threshold)
	//	{
			// reinitialize communication with SCXBs
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "re-initialize communication with the switch board which has raised the trap");
		call_result = start_switch_communication(*p_switchb_info);
		if(call_result < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'start_switch_communication()' failed ! call_result == %d", call_result);

		if(env == ACS_CS_API_CommonBasedArchitecture::SCX)
		{	
			if(scb_is_in_local_magazine)
			{
				ACS_APBM_LOG(LOG_LEVEL_WARN, "LAG SCX restart: trying to re-configure LAG on SCXB IP %s ... ", switch_board_ip);
				int call_result = -1;
				if(_server_working_set->snmp_manager->get_lag_admin_state(p_switchb_info->slot_position)){
					call_result = _server_working_set->snmp_manager->configure_front_port_scx_lag(p_switchb_info->slot_position,acs_apbm_snmp::ENABLE_LAG);
				}

				if(call_result != 0)
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG SCX restart: re-configuring LAG failed on SCXB %s - call_result: %d", switch_board_ip, call_result);
				else
					ACS_APBM_LOG(LOG_LEVEL_WARN, "LAG SCX restart: re-configuring LAG successful on SCXB %s",switch_board_ip);

				_server_working_set->monitoring_service_event_handler->reset_scx_lag_monitoring_flags_due_to_restart(p_switchb_info->slot_position);
			}
		}
			// schedule operation in order to update AP boards info
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Scheduling the operation of the update AP boards with 2 seconds delay");
		call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM_COLD,2);
		if(call_result < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to schedule operation 'SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM'. Call 'acs_apbm_operationpipescheduler::schedule_operation()' failed. call_result == %d", call_result);

		// Start of TR IA83234
		// The below wait is incorrect as operationpipeeventhandler thread (current thread) itself has to execute the above scheduled operation for SNMP data collection, it does not make sense to wait. The scheduled operation will be executed later, but this thread can now proceed with the rest of the actions in handling cold restart trap.
		//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "waiting for OPERATION_SNMP_LOAD_BOARD_DATA_IMM");
   		//while(!scxColdRestartFlag && count <= 800)
		//{
		//	usleep(500 * 1000); //500ms sleep
		//	count++;
		//}
		// End of TR IA83234

		bool err_flag = false;
		char mag_str[16] = {0};
            	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updating the data of the HWI");
		call_result = _server_working_set->cs_reader->uint32_to_ip_format(mag_str, p_switchb_info->magazine, acs_apbm_programconfiguration::magazine_address_imm_separator);
		if((err_flag = (call_result < 0)))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'acs_apbm_csreader::uint32_to_ip_format()' failed. call_result == %d", call_result);

		if(!err_flag)
		{
			// get data from SCXB/SMXB
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "startCollectionOnBoard is called");
			call_result = _server_working_set->hwi_handler->startCollectionOnBoard(mag_str, p_switchb_info->slot_position);
			if((err_flag = (call_result < 0)))
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to start collection of HWI data for magazine %s. Call 'ACS_APBM_HWIHandler::startCollectionOn()' failed. call_result == %d", mag_str, call_result);
		}

		if(!err_flag)
		{
			// store HWI data in IMM
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeMagazine is called");
			call_result = _server_working_set->hwi_handler->storeMagazine(mag_str);
			if(call_result < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to store IMM data for magazine %s. Call 'ACS_APBM_HWIHandler::storeMagazine()' failed. call_result == %d", mag_str, call_result);
		}

		// reset COLD/WARM START counter

		// } // end if
	}
	else
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_switch_board_info_by_ip()' failed ! Searched IP is %s. call_result == %d ", switch_board_ip, call_result);


	// if we are running on a multiCP system, or the ColdStart trap is coming from an SCXB belonging to our magazine, enable "front port" on that SCXB
/*******************************************************/
// For the moment below part is kept only for SCX
/*******************************************************/	
	if(env == ACS_CS_API_CommonBasedArchitecture::SCX)
	{
		bool is_multicp_system = false;
		char scxb_ip_str[16] = {0};
		strncpy(scxb_ip_str, switch_board_ip,16);
		if(0 != _server_working_set->cs_reader->is_multicp_system(is_multicp_system))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Unable to verify if we're running on a multiCP system. Call 'acs_apbm_csreader::is_multicp_system()' failed !");
		if(is_multicp_system || scb_is_in_local_magazine)
		{
			ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to enable front port on SCXB having IP == %s ... ", switch_board_ip);
			call_result = _server_working_set->snmp_manager->set_front_port(scxb_ip_str,  acs_apbm_snmp::FRONT_PORT_UP);
			if(call_result != 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "SCXB front port enable failed ! Call 'acs_apbm_snmpmanager::set_front_port()' returned %d !", call_result);
			else
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "SCXB front port enable successful ");
		
		}

		// set WARM next restart type for the SCXB originating the trap
		ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to set WARM next restart type on SCXB having IP == %s ... ", switch_board_ip);
		char tmp_str[16] = {0};
		strncpy(tmp_str, switch_board_ip, 16);
		call_result = _server_working_set->snmp_manager->set_scx_next_restart_type(tmp_str, acs_apbm_snmp::RESTART_TYPE_WARM);
		if(call_result != 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "SCXB set WARM next restart type failed ! Call 'acs_apbm_snmpmanager::set_scx_next_restart_type()' returned %d !", call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "SCXB set WARM next restart type successful ");
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::manage_IpmiUpgradeResult (int shelfNum, int slotPos, int ipmiUpgradeResultValue) {

	uint32_t magazine = _server_working_set->shelves_data_manager->own_magazine();

	if (ipmiUpgradeResultValue != axe_eth_shelf::IPMI_UPGRADE_STATUS_ONGOING)
	_server_working_set->imm_repository_handler->notify_asynch_action_completion(slotPos, magazine);
		//update upgrade status
	//_server_working_set->shelves_data_manager->update_ap_board_ipmi_upgrade_status(axe_eth_shelf::)

	//update last update time
	if( ipmiUpgradeResultValue == axe_eth_shelf::IPMI_UPGRADE_STATUS_UNDEFINED &&
			ipmiUpgradeResultValue == axe_eth_shelf::IPMI_UPGRADE_STATUS_READY )
	{
	_server_working_set->shelves_data_manager->update_ap_board_ipmi_upgrade_last_time(true, slotPos, magazine);
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trap received about the board at slot position %d in shelfNum '%d' was update to '%d'!", slotPos, shelfNum, ipmiUpgradeResultValue);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::set_board_MIAled (uint32_t board_magazine, int32_t board_slot, int led_status) {
	char board_magazine_str [16] = {0};
        bool is_4leds_supported = _server_working_set->shelves_data_manager->is_4led_supported;
	bool is_new_gea_supported = _server_working_set->shelves_data_manager->is_new_gea_supported;
        int32_t mialed_info;

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		*board_magazine_str = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to set the board MIA led on board at slot %d of magazine 0x%08X ('%s')",
			board_slot, board_magazine, board_magazine_str);

	acs_apbm_snmpsessioninfo session_info;
	int call_result = 0;
	char switch_board_ipns [2][16] = {{0}};

	if ( const int call_result = get_my_operational_switch_board_ipns(switch_board_ipns[0], switch_board_ipns[1], board_slot))
		return call_result;

	//4 LEDs compatibility
        //supports 1=on, 0=off, 3=fast blink ONLY for 4 leds
	if(is_4leds_supported || is_new_gea_supported)
	{
        	if(led_status != axe_eth_shelf::LED_STATUS_OFF && 
                   led_status != axe_eth_shelf::FOUR_LED_STATUS_FASTBLINK &&
	           led_status != axe_eth_shelf::LED_STATUS_ON)
	 	{
                	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to set the board MIA(YELLOW) led with invalid 4 LEDs status= %d ", led_status);
	                return -1;        

        	}
	}
        //supports 1=on, 0=off ONLY for 3 leds
        else
	{
	        if(led_status != axe_eth_shelf::LED_STATUS_OFF &&
       		   led_status != axe_eth_shelf::LED_STATUS_ON)
		{
	                ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to set the board MIA(YELLOW) led with invalid 3 LEDs status= %d ", led_status);
                	return -1;        
            	}
        }

  	// Prepare the oid arrays

        oid * oid_mia_led = 0;
        size_t oid_mia_led_size = 0;

	oid oid_scbrp_mia_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_YELLOW_LED, board_slot };
        oid oid_scx_mia_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_YELLOW_LED, board_slot };
        oid oid_scbrp_mia_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_YELLOW_LED, board_slot };
        oid oid_scx_mia_led [] = { ACS_APBM_SCX_OID_BLADE_YELLOW_LED, board_slot };
	
	int is_cba = _server_working_set->cs_reader->is_cba_system();
	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
        uint16_t apboard_fbn;
        if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, board_slot, board_magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
                return ret_val;
        }
        if(is_cba)
        {
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
		{
			if(is_new_gea_supported)
			{
				oid_mia_led= oid_scx_mia_4led;
				oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_4led);
			}	
			else
			{
				oid_mia_led = oid_scx_mia_led;
	                        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_led);
			}
		}
		else
		{
			if(is_4leds_supported)
			{
			        oid_mia_led= oid_scx_mia_4led;
        		        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_4led);
			}
			else
			{
		        	oid_mia_led = oid_scx_mia_led;
	        	        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_mia_led);
			}
		}
        }
        else
        {
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
                {
			if(is_new_gea_supported)
        	        {
                	        oid_mia_led= oid_scbrp_mia_4led;
                        	oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_4led);
	                }
        	        else
                	{
                        	oid_mia_led = oid_scbrp_mia_led;
	                        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_led);
        	        }
                }
                else
                {
			if(is_4leds_supported)		
			{
	        	        oid_mia_led = oid_scbrp_mia_4led;
        	        	oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_4led);
			}
			else
			{
		                oid_mia_led = oid_scbrp_mia_led;
        		        oid_mia_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_mia_led);
			}
		}
        }

	char oid_str [1024];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_mia_led, oid_mia_led_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Target MIA led': OID '%s': value %d", oid_str,led_status);

        mialed_info = led_status; //supports 1=on, 0=off, 3=fast blink for 4 leds
                                  //supports 1=on , 0=off for 3 LEDs

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int j = 0; j < 2; ++j, ipn_index = (ipn_index + 1) & 0x01)
	{
		session_info.version(SNMP_VERSION_2c);
		session_info.community(const_cast<char *>(get_snmp_community()));

		// snmp session communication handler used to send request and the callback used to receive the ACK response
		acs_apbm_snmpsessionhandler session_handler;
		acs_apbm_snmploggercallback logger_callback("SNMP Response received for setting MIALED ON/OFF", board_magazine_str, board_slot, 0, is_cba);

		ACS_APBM_LOG(LOG_LEVEL_INFO,
					"SNMP Manager: sending MIALED setting request on board %d to the switch board IP '%s'", board_slot, switch_board_ipns[j]);

		session_info.peername(switch_board_ipns[j]);
		logger_callback.switch_board_ip(switch_board_ipns[j]);

		if ((call_result = session_handler.open(session_info, logger_callback))) 
		{ // ERROR: opening the communication session towards the switch board
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
							"snmp error text == '%s': call_result == %d",
							session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
		}

		// Prepare the PDU subscription request APBM will send to the current switch board
		netsnmp_pdu * request_pdu = 0;

		if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET)))
		{ // ERROR: creating a new PDU structure
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on craeting a new PDU structure to carry out the subscription request");
					session_handler.close();
					call_result = acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
					break;
		}
		// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
		request_pdu->variables = 0;

		snmp_pdu_add_variable(request_pdu, oid_mia_led, oid_mia_led_size, ASN_INTEGER, reinterpret_cast<u_char *>(&mialed_info), sizeof(mialed_info));

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending MIALED SET (value = %d) request...", mialed_info);

		unsigned timeout_ms = 2000; // 2 seconds timeout
                errno=0;
		call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
		snmp_free_pdu(request_pdu);

                int errno_save=errno;
		if (call_result) { // ERROR: sending the request
				ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
							"Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
		}
		session_handler.close();

		if (!call_result) break; // OK: board data retrieved successfully
	}
	if (call_result)
		 ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'set_board_MIAled' failed: cannot set board MIA LED info: call_result == %d", call_result);

	return call_result;
}

int __CLASS_NAME__::set_board_REDled (uint32_t board_magazine, int32_t board_slot, bool led_status) {
	char board_magazine_str [16] = {0};
        bool is_4leds_supported = _server_working_set->shelves_data_manager->is_4led_supported;
	bool is_new_gea_supported = _server_working_set->shelves_data_manager->is_new_gea_supported;
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		*board_magazine_str = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to set the board RED led on board at slot %d of magazine 0x%08X ('%s')",
			board_slot, board_magazine, board_magazine_str);

	acs_apbm_snmpsessioninfo session_info;
	int call_result = 0;
	char switch_board_ipns [2][16] = {{0}};

	if ( const int call_result = get_my_operational_switch_board_ipns(switch_board_ipns[0], switch_board_ipns[1], board_slot))
		return call_result;

  // Prepare the oid arrays
	oid oid_scbrp_red_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_RED_LED, board_slot };
	oid oid_scx_red_led [] = { ACS_APBM_SCX_OID_BLADE_RED_LED, board_slot };
	oid oid_scbrp_red_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_RED_LED, board_slot };
	oid oid_scx_red_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_RED_LED, board_slot };

	oid * oid_red_led = 0;
	size_t oid_red_led_size = 0;

	int is_cba = _server_working_set->cs_reader->is_cba_system();
	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
        uint16_t apboard_fbn;
        if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, board_slot, board_magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
                return ret_val;
        }
        if(is_cba)
        {
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
		{
			if(is_new_gea_supported)
	                {
        	                oid_red_led = oid_scx_red_4led;
                	        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_4led);
	                }
        	        else
                	{
                        	oid_red_led = oid_scx_red_led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_led);
        	        }
		}
		else
		{
			if(is_4leds_supported)
			{
			        oid_red_led = oid_scx_red_4led;
	        	        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_4led);
			}
			else
			{
			        oid_red_led = oid_scx_red_led;
        		        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_led);
			}
		}
        }
        else
        {
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
                {
                        if(is_new_gea_supported)
                        {
				oid_red_led = oid_scbrp_red_4led;
        	                oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_4led);
	                }
        	        else
                	{
                        	oid_red_led = oid_scbrp_red_led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_led);
        	        }
		}
		else
		{
			if(is_4leds_supported)
			{
	        	        oid_red_led = oid_scbrp_red_4led;
        	        	oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_4led);
			}
			else
			{
	                	oid_red_led = oid_scbrp_red_led;
	        	        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_led);
			}
		}
        }

	char oid_str [1024];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_red_led, oid_red_led_size);
	
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Target RED led': OID '%s': value %d", oid_str,led_status);

        //supports only on/off statuses for GEP1/2/4 hw
	int32_t redled_info = (led_status) ? axe_eth_shelf::LED_STATUS_ON : axe_eth_shelf::LED_STATUS_OFF ; 

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int j = 0; j < 2; ++j, ipn_index = (ipn_index + 1) & 0x01)
	{

		session_info.version(SNMP_VERSION_2c);
		session_info.community(const_cast<char *>(get_snmp_community()));

		// snmp session communication handler used to send request and the callback used to receive the ACK response
		acs_apbm_snmpsessionhandler session_handler;
		acs_apbm_snmploggercallback logger_callback("SNMP Response received for setting REDLED ON/OFF", board_magazine_str, board_slot, 0, is_cba);
		ACS_APBM_LOG(LOG_LEVEL_INFO,
					"SNMP Manager: sending REDLED setting request on board %d to the switch board IP '%s'", board_slot, switch_board_ipns[j]);

		session_info.peername(switch_board_ipns[j]);
		logger_callback.switch_board_ip(switch_board_ipns[j]);

		if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
							"snmp error text == '%s': call_result == %d",
							session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
		}

		// Prepare the PDU subscription request APBM will send to the current switch board
		netsnmp_pdu * request_pdu = 0;

		if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on craeting a new PDU structure to carry out the subscription request");
			session_handler.close();
			call_result = acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
			break;
		}
		// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
		request_pdu->variables = 0;

		snmp_pdu_add_variable(request_pdu, oid_red_led, oid_red_led_size, ASN_INTEGER, reinterpret_cast<u_char *>(&redled_info), sizeof(redled_info));

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending REDLED SET (value = %d) request...", redled_info);

		unsigned timeout_ms = 2000; // 2 seconds timeout
                errno=0;
		call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
		snmp_free_pdu(request_pdu);
                int errno_save=errno;
		if (call_result) { // ERROR: sending the request
				ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
							"Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
		}
		session_handler.close();

		if (!call_result) break; // OK: board data retrieved successfully
	}

		if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'set_board_REDled' failed: cannot set board RED LED info: call_result == %d", call_result);

		return call_result;
}

//ONLY for 4 LEDS hw
int __CLASS_NAME__::set_board_BLUEled (uint32_t board_magazine, int32_t board_slot, int led_status) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		*board_magazine_str = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to set the board BLUE led on board at slot %d of magazine 0x%08X ('%s')",
			board_slot, board_magazine, board_magazine_str);

	acs_apbm_snmpsessioninfo session_info;
	int call_result = 0;
	char switch_board_ipns [2][16] = {{0}};

	if ( const int call_result = get_my_operational_switch_board_ipns(switch_board_ipns[0], switch_board_ipns[1], board_slot))
		return call_result;

  // Prepare the oid arrays
	oid oid_scbrp_blue_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_BLUE_LED, board_slot };
	oid oid_scx_blue_led [] = { ACS_APBM_SCX_OID_BLADECOMMON_BLUE_LED, board_slot };

	oid * oid_blue_led = 0;
	size_t oid_blue_led_size = 0;

	int is_cba = _server_working_set->cs_reader->is_cba_system();

        if(is_cba)
        {
                oid_blue_led= oid_scx_blue_led;
                oid_blue_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_blue_led);
        }
        else
        {
                oid_blue_led = oid_scbrp_blue_led;
                oid_blue_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_blue_led);
        }


	char oid_str [1024];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_blue_led, oid_blue_led_size);
	
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Target BLUE led': OID '%s': value %d", oid_str,led_status);

        if(led_status != axe_eth_shelf::LED_STATUS_OFF &&
           led_status != axe_eth_shelf::LED_STATUS_ON &&
           led_status != axe_eth_shelf::FOUR_LED_STATUS_SLOWBLINK) {
                ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to set the board BLUE led with invalid 4 LEDs status= %d ", led_status);
                return -1;        
        }

	int32_t blueled_info = led_status;

	::srandom(static_cast<unsigned>(::time(0)));

	int ipn_index = ::random() & 0x01;
	for (int j = 0; j < 2; ++j, ipn_index = (ipn_index + 1) & 0x01) {

		session_info.version(SNMP_VERSION_2c);
		session_info.community(const_cast<char *>(get_snmp_community()));

		// snmp session communication handler used to send request and the callback used to receive the ACK response
		acs_apbm_snmpsessionhandler session_handler;
		acs_apbm_snmploggercallback logger_callback("SNMP Response received for setting REDLED ON/OFF", board_magazine_str, board_slot, 0, is_cba);

		ACS_APBM_LOG(LOG_LEVEL_INFO,
					"SNMP Manager: sending BLUELED setting request on board %d to the switch board IP '%s'", board_slot, switch_board_ipns[j]);

		session_info.peername(switch_board_ipns[j]);
		logger_callback.switch_board_ip(switch_board_ipns[j]);

		if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
							"snmp error text == '%s': call_result == %d",
							session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
		}

		// Prepare the PDU subscription request APBM will send to the current switch board
		netsnmp_pdu * request_pdu = 0;

		if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on craeting a new PDU structure to carry out the subscription request");
			session_handler.close();
			call_result = acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
			break;
		}
		// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
		request_pdu->variables = 0;

		snmp_pdu_add_variable(request_pdu, oid_blue_led, oid_blue_led_size, ASN_INTEGER, reinterpret_cast<u_char *>(&blueled_info), sizeof(blueled_info));

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending BLUELED SET (value = %d) request...", blueled_info);

		unsigned timeout_ms = 2000; // 2 seconds timeout
                errno=0;
		call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
		snmp_free_pdu(request_pdu);
                int errno_save=errno;
		if (call_result) { // ERROR: sending the request
				ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
							"Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
		}
		session_handler.close();

		if (!call_result) break; // OK: board data retrieved successfully
	}

		if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'set_board_BLUEled' failed: cannot set board BLUE LED info: call_result == %d", call_result);

		return call_result;
}

int __CLASS_NAME__::set_scx_next_restart_type (char * switch_board_ip, acs_apbm_snmp::restart_type_t restart_type){
	acs_apbm_snmpsessioninfo session_info;
	int call_result = 0;

	// Prepare the oid arrays
	oid oid_scbrp_restart_type [] = { ACS_APBM_SCX_OID_NEXT_RESTART_TYPE, 0 };

	oid * oid_rsty = 0;
	size_t oid_rsty_size = 0;

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	if (!is_cba) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "APBM server is running on a not CBA system: Next Restart Type cannot be handled");
		return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	}

	oid_rsty = oid_scbrp_restart_type; oid_rsty_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_restart_type);

	session_info.version(SNMP_VERSION_2c);
	session_info.community(const_cast<char *>(get_snmp_community()));

	// snmp session communication handler used to send request and the callback used to receive the ACK response
	acs_apbm_snmpsessionhandler session_handler;
	acs_apbm_snmploggercallback logger_callback("SNMP Response received for setting nextRestartType");

	//for (j = 0; j < ACS_APBM_CONFIG_IPN_COUNT; ++j, session_handler.close()) {

	ACS_APBM_LOG(LOG_LEVEL_INFO, "SNMP Manager: sending nextRestartType setting request to switch board IP %s", switch_board_ip);

	session_info.peername(switch_board_ip);
	logger_callback.switch_board_ip(switch_board_ip);

	if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
				"snmp error text == '%s': call_result == %d",
				session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
		return acs_apbm_snmp::ERR_SNMP_SESSION_OPEN;
	}

	// Prepare the PDU subscription request APBM will send to the current switch board
	netsnmp_pdu * request_pdu = 0;

	if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on creating a new PDU structure to carry out the subscription request");
		session_handler.close();
		return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
	}
	// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
	request_pdu->variables = 0;

	snmp_pdu_add_variable(request_pdu, oid_rsty, oid_rsty_size,
			ASN_INTEGER, reinterpret_cast<u_char *>(&restart_type), sizeof(restart_type));

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending NEXT_RESTART_TYPE SET (value = %d) request...", restart_type);

	unsigned timeout_ms = 2000; // 2 seconds timeout
        errno=0;
	call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
	snmp_free_pdu(request_pdu);
        int errno_save=errno;  
	if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
				"Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
		session_handler.close();
		return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
	}
	session_handler.close();

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::enable_front_port_scx () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	if (_my_switch_board_infos.size() <= 0) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No switch boards found in my magazine: cannot enable front port on any switch board");
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying to enable Front Port Gi0/27 Status on local magazine...");

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if (get_lag_admin_state(_my_switch_board_infos[i].slot_position)==acs_apbm_snmp::ENABLE_LAG)
			continue;

		const int call_result = enable_front_port_scx(_my_switch_board_infos[i]);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"SCXB front port enable on switch board in slot %d failed ! Call 'acs_apbm_snmpmanager::set_front_port()' returned %d !",
					_my_switch_board_infos[i].slot_position, call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCXB front port enable on switch board in slot %d successful done!", _my_switch_board_infos[i].slot_position);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::enable_front_port_scx (acs_apbm_switchboardinfo & switch_board) {
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to enable front port on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_front_port(switch_board_ipn_str_addresses[ipn_index], acs_apbm_snmp::FRONT_PORT_UP);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Call 'set_front_port' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
				switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}

	return call_result;
}

int __CLASS_NAME__::set_warm_restart_on_my_scx () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	if (_my_switch_board_infos.size() <= 0) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No switch boards found in my magazine: cannot set warm restart type on any switch board");
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Trying  to set next restart type to WARM on local magazine...");

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		const int call_result = set_warm_restart_type(_my_switch_board_infos[i]);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"SCXB set wram restart type on switch board in slot %d failed ! Call 'acs_apbm_snmpmanager::set_warm_restart_type()' returned %d !",
					_my_switch_board_infos[i].slot_position, call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCXB front port enable on switch board in slot %d successful done!", _my_switch_board_infos[i].slot_position);
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::set_warm_restart_type (acs_apbm_switchboardinfo & switch_board) {
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	ACS_APBM_LOG(LOG_LEVEL_INFO, "trying to set next restart_type to WARM on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_scx_next_restart_type (switch_board_ipn_str_addresses[ipn_index], acs_apbm_snmp::RESTART_TYPE_WARM);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Call 'set_scx_next_restart_type' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
				switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}

	return call_result;
}


//.....;

int __CLASS_NAME__::set_board_reset(uint32_t board_magazine, int32_t board_slot){

  char board_magazine_str [16] = {0};

  if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
          *board_magazine_str = 0;

  acs_apbm_snmpsessioninfo session_info;
  int call_result = 0;

  char switch_board_ipns [2][16] = {{0}};

  if ( const int call_result = get_my_operational_switch_board_ipns(switch_board_ipns[0], switch_board_ipns[1], board_slot))
          return call_result;

  // Prepare the oid arrays
oid oid_scbrp_reset [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_RESET, board_slot };
oid oid_scx_reset [] = { ACS_APBM_SCX_OID_BLADE_RESET, board_slot };

  oid * oid_reset = 0;
  size_t oid_reset_size = 0;

  int is_cba = _server_working_set->cs_reader->is_cba_system();

  if (is_cba) {
      oid_reset = oid_scx_reset; oid_reset_size = ACS_APBM_ARRAY_SIZE(oid_scx_reset);
  } else {
      oid_reset = oid_scbrp_reset; oid_reset_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_reset);
  }

  //1 should normally be used as resetValue when resetting a board.(design base)
  int32_t board_reset_value = 1;
  call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int j = 0; (call_result && j < 2); ++j, ipn_index = (ipn_index + 1) & 0x01) {

		session_info.version(SNMP_VERSION_2c);
		session_info.community(const_cast<char *>(get_snmp_community()));

		// snmp session communication handler used to send request and the callback used to receive the ACK response
		acs_apbm_snmpsessionhandler session_handler;
		acs_apbm_snmploggercallback logger_callback("SNMP Response received for reset AP board", board_magazine_str, board_slot, 0, is_cba);

		ACS_APBM_LOG(LOG_LEVEL_INFO,
										"SNMP Manager: sending board Reset request on board %d to switch board IP %s", board_slot, switch_board_ipns[j]);

		session_info.peername(switch_board_ipns[j]);
		logger_callback.switch_board_ip(switch_board_ipns[j]);

		if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
												"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
												"snmp error text == '%s': call_result == %d",
												session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
		}

		// Prepare the PDU subscription request APBM will send to the current switch board
		netsnmp_pdu * request_pdu = 0;

		if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on creating a new PDU structure to carry out the subscription request");
			session_handler.close();
			call_result = acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
			break;

		}
		// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
		request_pdu->variables = 0;

		snmp_pdu_add_variable(request_pdu, oid_reset, oid_reset_size,
										ASN_INTEGER, reinterpret_cast<u_char *>(&board_reset_value), sizeof(board_reset_value));

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending BOARDRESET SET (value = %d) request...", board_reset_value);

		unsigned timeout_ms = 2000; // 2 seconds timeout
                errno=0;
		call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
		snmp_free_pdu(request_pdu);
                int errno_save=errno;
		if (call_result) { // ERROR: sending the request
				ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
																"Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
		}
		session_handler.close();

		if (!call_result) break; // OK: board data retrieved successfully

	}
	if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'set_board_reset' failed: cannot set board RESET info: call_result == %d", call_result);

  return call_result;
}

int __CLASS_NAME__::set_front_port (char * switch_board_ip, acs_apbm_snmp::frontPort_status_t port_status) {
      acs_apbm_snmpsessioninfo session_info;
        int call_result = 0;

        // Prepare the oid arrays
        oid oid_scbrp_front_port [] = { IF_ADMIN_STATUS, 27 };

        oid * oid_fprt = 0;
        size_t oid_fprt_size = 0;

        int is_cba = _server_working_set->cs_reader->is_cba_system();

        if (!is_cba) {
            //not permitted
            return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
        }

        oid_fprt = oid_scbrp_front_port; oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_front_port);

        session_info.version(SNMP_VERSION_2c);
        session_info.community(const_cast<char *>(get_snmp_community()));

        // snmp session communication handler used to send request and the callback used to receive the ACK response
        acs_apbm_snmpsessionhandler session_handler;
        acs_apbm_snmploggercallback logger_callback("SNMP Response received for setting front Port 1GE UP/DOWN");

        //for (j = 0; j < ACS_APBM_CONFIG_IPN_COUNT; ++j, session_handler.close()) {

        ACS_APBM_LOG(LOG_LEVEL_INFO,
                        "SNMP Manager: sending ifAdminStatus/frontPort setting request to switch board IP %s", switch_board_ip);

        session_info.peername(switch_board_ip);
        logger_callback.switch_board_ip(switch_board_ip);

        if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
            ACS_APBM_LOG(LOG_LEVEL_ERROR,
                            "Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
                            "snmp error text == '%s': call_result == %d",
                            session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
            return acs_apbm_snmp::ERR_SNMP_SESSION_OPEN;
        }

        // Prepare the PDU subscription request APBM will send to the current switch board
        netsnmp_pdu * request_pdu = 0;

        if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
            ACS_APBM_LOG(LOG_LEVEL_ERROR,
                  "Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on creating a new PDU structure to carry out the subscription request");
	    session_handler.close();
            return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
        }
        // WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
        request_pdu->variables = 0;

        snmp_pdu_add_variable(request_pdu, oid_fprt, oid_fprt_size,
                        ASN_INTEGER, reinterpret_cast<u_char *>(&port_status), sizeof(port_status));

        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending IFADMINSTATUS/FRONTPORT SET (value = %d) request...", port_status);

        unsigned timeout_ms = 2000; // 2 seconds timeout
        errno=0;
        call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
        snmp_free_pdu(request_pdu);
        int errno_save=errno;
        if (call_result) { // ERROR: sending the request
            ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
                                    "Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
	    session_handler.close();
            return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
        }
        session_handler.close();

        return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::set_board_Power (uint32_t board_magazine, int32_t board_slot, bool pwr_status) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		*board_magazine_str = 0;

	acs_apbm_snmpsessioninfo session_info;
	int call_result = 0;

	char switch_board_ipns [2][16] = {{0}};

	if ( const int call_result = get_my_operational_switch_board_ipns(switch_board_ipns[0], switch_board_ipns[1], board_slot))
		return call_result;

  	// Prepare the oid arrays
  oid oid_scbrp_pwr_on [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_PWR_ON, board_slot };
  oid oid_scbrp_pwr_off [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_PWR_OFF, board_slot };
  oid oid_scx_pwr [] = { ACS_APBM_SCX_OID_BLADE_PWR, board_slot };

	oid * oid_pwr = 0;
	size_t oid_pwr_size = 0;

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	if (is_cba) {
		oid_pwr = oid_scx_pwr; oid_pwr_size = ACS_APBM_ARRAY_SIZE(oid_scx_pwr);
	} else {
		if (pwr_status) { oid_pwr = oid_scbrp_pwr_on; oid_pwr_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_pwr_on); }
		else { oid_pwr = oid_scbrp_pwr_off; oid_pwr_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_pwr_off); }
	}

	// in case of not cba system board_pwr_info can be set to any value
	int32_t board_pwr_info = (pwr_status)? acs_apbm_snmp::BLADE_PWR_ON: acs_apbm_snmp::BLADE_PWR_OFF;
	call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int j = 0; (call_result && j < 2); ++j, ipn_index = (ipn_index + 1) & 0x01) {

		session_info.version(SNMP_VERSION_2c);
		session_info.community(const_cast<char *>(get_snmp_community()));

		// snmp session communication handler used to send request and the callback used to receive the ACK response
		acs_apbm_snmpsessionhandler session_handler;
		acs_apbm_snmploggercallback logger_callback("SNMP Response received for setting board Power ON/OFF", board_magazine_str, board_slot, 0, is_cba);

		ACS_APBM_LOG(LOG_LEVEL_INFO,
				"SNMP Manager: sending board Power setting request on board %d to switch board IP %s", board_slot, switch_board_ipns[j]);

		session_info.peername(switch_board_ipns[j]);
		logger_callback.switch_board_ip(switch_board_ipns[j]);

		if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
							"snmp error text == '%s': call_result == %d",
							session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
		}

		// Prepare the PDU subscription request APBM will send to the current switch board
		netsnmp_pdu * request_pdu = 0;

		if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on creating a new PDU structure to carry out the subscription request");
					session_handler.close();
					call_result = acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
					break;
		}
		// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
		request_pdu->variables = 0;

		snmp_pdu_add_variable(request_pdu, oid_pwr, oid_pwr_size,
				ASN_INTEGER, reinterpret_cast<u_char *>(&board_pwr_info), sizeof(board_pwr_info));

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending BOARDPWR SET (value = %d) request...", board_pwr_info);

		unsigned timeout_ms = 2000; // 2 seconds timeout
                errno=0;
		call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
		snmp_free_pdu(request_pdu);
                int errno_save=errno;
		if (call_result) { // ERROR: sending the request
				ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
							"Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
		}
		session_handler.close();

		if (!call_result) break; // OK: board data retrieved successfully
	}
		if (call_result) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'set_board_Power_status' failed: cannot set board POWER STATUS LOG info: call_result == %d", call_result);

		return call_result;
}

int __CLASS_NAME__::get_my_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	if (_my_switch_board_infos.size()) board_infos.assign(_my_switch_board_infos.begin(), _my_switch_board_infos.end());
	else board_infos.clear();

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::get_other_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos) {
	if (_other_switch_board_infos.size()) board_infos.assign(_other_switch_board_infos.begin(), _other_switch_board_infos.end());
	else board_infos.clear();

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::get_all_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (_other_switch_board_infos.size()) board_infos.assign(_other_switch_board_infos.begin(), _other_switch_board_infos.end());
	else board_infos.clear();

	if (_my_switch_board_infos.size()) board_infos.insert(board_infos.begin(), _my_switch_board_infos.begin(), _my_switch_board_infos.end());

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //-

int __CLASS_NAME__::get_switch_boards_info (std::vector<acs_apbm_switchboardinfo> & board_infos, uint32_t magazine) {
	std::vector<acs_apbm_switchboardinfo> tmp_board_infos;

	get_all_switch_board_info(tmp_board_infos);

	board_infos.clear();

	for (size_t i = 0; i < tmp_board_infos.size(); ++i)
		if (tmp_board_infos[i].magazine == magazine)
			board_infos.push_back(tmp_board_infos[i]);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::get_my_active_switch_board (acs_apbm_switchboardinfo & switch_board) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int active_index = get_my_active_switch_board_index();
	if (active_index == acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD ) {
		// ERROR: No active switch board defined
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "There is no active switch board defined");
		return acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD;
	}

	switch_board = _my_switch_board_infos.at(active_index);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::map_net_snmp_error_stat (long net_snmp_error_stat) {
	int error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERROR_UNDEFINED;

	switch (net_snmp_error_stat) {
	case SNMP_ERR_NOERROR: error_code = acs_apbm_snmp::ERR_SNMP_NO_ERRORS; break;
	case SNMP_ERR_TOOBIG: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_TOOBIG; break;
	case SNMP_ERR_NOSUCHNAME: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_NOSUCHNAME; break;
	case SNMP_ERR_BADVALUE: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_BADVALUE; break;
	case SNMP_ERR_READONLY: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_READONLY; break;
	case SNMP_ERR_GENERR: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_GENERR; break;
	case SNMP_ERR_NOACCESS: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_NOACCESS; break;
	case SNMP_ERR_WRONGTYPE: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_WRONGTYPE; break;
	case SNMP_ERR_WRONGLENGTH: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_WRONGLENGTH; break;
	case SNMP_ERR_WRONGENCODING: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_WRONGENCODING; break;
	case SNMP_ERR_WRONGVALUE: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_WRONGVALUE; break;
	case SNMP_ERR_NOCREATION: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_NOCREATION; break;
	case SNMP_ERR_INCONSISTENTVALUE: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_INCONSISTENTVALUE; break;
	case SNMP_ERR_RESOURCEUNAVAILABLE: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_RESOURCEUNAVAILABLE; break;
	case SNMP_ERR_COMMITFAILED: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_COMMITFAILED; break;
	case SNMP_ERR_UNDOFAILED: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_UNDOFAILED; break;
	case SNMP_ERR_AUTHORIZATIONERROR: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_AUTHORIZATIONERROR; break;
	case SNMP_ERR_NOTWRITABLE: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_NOTWRITABLE; break;
	case SNMP_ERR_INCONSISTENTNAME: error_code = acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_INCONSISTENTNAME; break;
	}

	return error_code;
}

int __CLASS_NAME__::load_my_switch_boards_info_from_cs (bool force) {
	static int info_loaded = 0;

	if (info_loaded && !force) return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "load switch board founds from CS configuration");

	std::vector <acs_apbm_switchboardinfo> boards;

	if (const int cs_call_result = _server_working_set->cs_reader->get_my_switch_boards_info(boards)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'get_my_switch_boards_info' failed: cannot get the switch board info in my magazine: cs_call_result == %d", cs_call_result);
		return cs_call_result;
	}

	if (boards.size() <= 0) { // ERROR: No switch board found in the CS configuration
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No switch board found in my magazine from CS configuration");
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	if (boards.size() > acs_apbm_snmp::SWITCH_BOARD_COUNT_PER_MAGAZINE) { // ERROR: Too many switch board in magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Too many switch board found in my magazine from CS configuration: there are %uz switch board in my magazine but them should be at most %d",
				boards.size(), acs_apbm_snmp::SWITCH_BOARD_COUNT_PER_MAGAZINE);
		return acs_apbm::ERR_CS_TOO_MANY_BOARD_FOUND;
	}

	// Sort the vector by slot position
	std::sort(boards.begin(), boards.end(), fx_compare_slot_ascending);

	_my_switch_board_infos.assign(boards.begin(), boards.end());

	info_loaded = 1;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "load switch boards size = %d\n", (int)_my_switch_board_infos.size());

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::load_other_switch_boards_info_from_cs (bool force) {
	static int info_loaded = 0;

	if (info_loaded && !force) return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "load other switch boards found from CS configuration");
	std::vector <acs_apbm_switchboardinfo> boards;

	if (const int cs_call_result = _server_working_set->cs_reader->get_other_switch_boards_info(boards)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'get_other_switch_boards_info' failed: cannot get the switch board info in other magazines: cs_call_result == %d", cs_call_result);
		return cs_call_result;
	}

	if (boards.size() <= 0) { // ERROR: No switch board found in the CS configuration
		ACS_APBM_LOG(LOG_LEVEL_INFO, "No switch board found in other magazines from CS configuration");	// TR HV75981
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	// Sort the vector by slot position
	std::sort(boards.begin(), boards.end(), fx_compare_magazine_slot_ascending);

	_other_switch_board_infos.assign(boards.begin(), boards.end());

	info_loaded = 1;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::load_switch_boards_info_from_cs (bool force) {
	load_my_switch_boards_info_from_cs(force);

	load_other_switch_boards_info_from_cs(force);

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::no_my_switch_board_found_check () const {
	if (_my_switch_board_infos.size() <= 0) { // ERROR: No switch board found in the CS configuration
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No switch board found in my magazine from CS configuration");
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::no_other_switch_board_found_check () const {
	if (_other_switch_board_infos.size() <= 0) { // ERROR: No switch board found in the CS configuration
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No switch board found in other magazines from CS configuration");
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::send_get_request (
		acs_apbm_snmpsessionhandler & session_handler,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {
	netsnmp_pdu * snmp_pdu = 0;

	// Create the PDU for GET
	if (const int call_result = create_snmp_pdu(snmp_pdu, SNMP_MSG_GET)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'create_snmp_pdu' failed: call_result == %d", call_result);
		return call_result;
	}

	// Add the OID into the PDU request
	if (const int call_result = add_snmp_null_var(snmp_pdu, oid_name, oid_name_size)) { // ERROR: adding the OID
		snmp_free_pdu(snmp_pdu);
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'add_snmp_null_var' failed: call_result == %d", call_result);
		return call_result;
	}

	int call_result = session_handler.send_synch(snmp_pdu, timeout_ms);
	snmp_free_pdu(snmp_pdu);

	if (call_result) {
		char oid_str [512];
                errno=0;
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);
                int errno_save=errno;
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'send_synch' failed: cannot send the GET pdu request for OID '%s'", oid_str);
	}

	return call_result;
}

int __CLASS_NAME__::send_set_request (
		acs_apbm_snmpsessionhandler & session_handler,
		const oid * oid_name,
		size_t oid_name_size,
		const char * oid_value,
		const unsigned * timeout_ms) {
	netsnmp_pdu * snmp_pdu = 0;

	// Create the PDU for GET
	if (const int call_result = create_snmp_pdu(snmp_pdu, SNMP_MSG_SET)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'create_snmp_pdu' failed: call_result == %d", call_result);
		return call_result;
	}

	// Add the OID name and value into the PDU request
	if (!snmp_pdu_add_variable(snmp_pdu, oid_name, oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(oid_value), ::strlen(oid_value))) {
		snmp_free_pdu(snmp_pdu);
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'snmp_pdu_add_variable' failed in NET-SNMP library");
		return acs_apbm_snmp::ERR_SNMP_PDU_ADD_VARIABLE_VALUE;
	}

	// Sending the SET request

	int call_result = session_handler.send_synch(snmp_pdu, timeout_ms);
	snmp_free_pdu(snmp_pdu);

	if (call_result) {
		char oid_str [512];
                errno=0;
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);
                int errno_save=errno;
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'send_synch' failed: cannot send the SET pdu request for OID '%s'", oid_str);
	}

	return call_result;
}

int __CLASS_NAME__::send_set_request (
		acs_apbm_snmpsessionhandler & session_handler,
		const oid * oid_name,
		size_t oid_name_size,
		int oid_value,
		const unsigned * timeout_ms) {
	netsnmp_pdu * snmp_pdu = 0;

	// Create the PDU for GET
	if (const int call_result = create_snmp_pdu(snmp_pdu, SNMP_MSG_SET)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'create_snmp_pdu' failed: call_result == %d", call_result);
		return call_result;
	}

	// Add the OID name and value into the PDU request
	if (!snmp_pdu_add_variable(snmp_pdu, oid_name, oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&oid_value), sizeof(oid_value))) {
		snmp_free_pdu(snmp_pdu);
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'snmp_pdu_add_variable' failed in NET-SNMP library");
		return acs_apbm_snmp::ERR_SNMP_PDU_ADD_VARIABLE_VALUE;
	}

	// Sending the SET request

	int call_result = session_handler.send_synch(snmp_pdu, timeout_ms);
	snmp_free_pdu(snmp_pdu);

	if (call_result) {
		char oid_str [512];
                errno=0;
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);
                int errno_save=errno;
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'send_synch' failed: cannot send the SET pdu request for OID '%s'", oid_str);
	}

	return call_result;
}

int __CLASS_NAME__::send_set_request (
		acs_apbm_snmpsessionhandler & session_handler,
		const oid * oid_name,
		size_t oid_name_size,
		u_char asn1_val_type,
		const u_char * value,
		size_t value_size,
		const unsigned * timeout_ms) {
	netsnmp_pdu * snmp_pdu = 0;

	// Create the PDU for GET
	if (const int call_result = create_snmp_pdu(snmp_pdu, SNMP_MSG_SET)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'create_snmp_pdu' failed: call_result == %d", call_result);
		return call_result;
	}

	// Add the OID name and value into the PDU request
	if (!snmp_pdu_add_variable(snmp_pdu, oid_name, oid_name_size, asn1_val_type, value, value_size)) {
		snmp_free_pdu(snmp_pdu);
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'snmp_pdu_add_variable' failed in NET-SNMP library");
		return acs_apbm_snmp::ERR_SNMP_PDU_ADD_VARIABLE_VALUE;
	}

	// Sending the SET request

	int call_result = session_handler.send_synch(snmp_pdu, timeout_ms);
	snmp_free_pdu(snmp_pdu);

	if (call_result) {
		char oid_str [512];
                errno=0;
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);
                int errno_save=errno;
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Call 'send_synch' failed: cannot send the SET pdu request for OID '%s'", oid_str);
	}

	return call_result;
}

// new scbrp_unsubscribe
int __CLASS_NAME__::send_trap_unsubscription_request_scbrp () {
	int call_result = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int failure_counter[2] = {0};

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		call_result = send_trap_unsubscription_request_scbrp(_my_switch_board_infos[i]);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,
				"Call 'send_trap_unsubscription_request_scbrp' failed: sending traps unsubscription request failed for switch board in slot %d: call_result == %d",
				_my_switch_board_infos[i].slot_position, call_result);
			if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) {
				if (_my_switch_board_infos[i].slot_position == acs_apbm_programconfiguration::slot_position_switch_board_left)
					failure_counter[0]++;
				else
					failure_counter[1]++;
			}
		}
	}
	int ret_value = (failure_counter[0] & failure_counter[1] >> 1);

	if (ret_value == 1)  return acs_apbm_snmp::ERR_SNMP_UNSUBSCRIBE_LEFT_SB;
	if (ret_value == 2)  return acs_apbm_snmp::ERR_SNMP_UNSUBSCRIBE_RIGHT_SB;
	if (ret_value == 3)  return acs_apbm_snmp::ERR_SNMP_UNSUBSCRIBE_BOTH_SB;

	return acs_apbm::ERR_NO_ERRORS;

} /*+*/

int __CLASS_NAME__::send_trap_unsubscription_request_scbrp (uint32_t magazine) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	uint32_t boards_found = 0;
	int call_result = 0;

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if (_my_switch_board_infos[i].magazine == magazine) {
			boards_found++;

			call_result = send_trap_unsubscription_request_scbrp(_my_switch_board_infos[i]);
			if (call_result) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'send_trap_unsubscription_request_scbrp' failed: sending traps unsubscription request failed for switch board in my "
					"magazine 0x%08X, slot %d: call_result == %d", magazine, _my_switch_board_infos[i].slot_position, call_result);
			}
		}
	}

	for (size_t i = 0; i < _other_switch_board_infos.size(); ++i) {
		if (_other_switch_board_infos[i].magazine == magazine) {
			boards_found++;

			call_result = send_trap_unsubscription_request_scbrp(_other_switch_board_infos[i]);
			if (call_result) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'send_trap_unsubscription_request_scbrp' failed: sending traps unsubscription request failed for switch board in other "
					"magazine 0x%08X, slot %d: call_result == %d", magazine, _other_switch_board_infos[i].slot_position, call_result);
			}
		}
	}

	if (!boards_found) return acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::send_trap_unsubscription_request_scbrp (int scbrpSlotPos)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int idx_scb = index_from_slot_position(scbrpSlotPos);

	int call_result = send_trap_unsubscription_request_scbrp(_my_switch_board_infos[idx_scb]);

	return (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT ) ? acs_apbm::ERR_SEND_PRIMITIVE_UNSUBSCRIBE : acs_apbm::ERR_NO_ERRORS;
} //+

int __CLASS_NAME__::send_trap_unsubscription_request_scbrp (acs_apbm_switchboardinfo & switch_board) {
	char my_ipn_addresses [ACS_APBM_CONFIG_IPN_COUNT][16];

	if (const int call_result = _server_working_set->cs_reader->get_my_ipn_addresses(my_ipn_addresses[0], my_ipn_addresses[1])) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'get_my_ipn_addresses' failed: cannot load my AP ipn addresses : call_result == %d", call_result);
		return call_result;
	}

	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	int failure_cnt = 0;
	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2 ; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_active_manager_trap_subscription(switch_board_ipn_str_addresses[ipn_index], my_ipn_addresses[ipn_index], acs_apbm_snmp::SCBRP_UNSUBSCRIPTION_REQUEST );
		if (call_result){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Call 'send_trap_unsubscription_request_scbrp' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
				switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
			if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) failure_cnt++ ;
		}
	}

	return ( failure_cnt) ? acs_apbm_snmp::ERR_SNMP_TIMEOUT : call_result;
}

int __CLASS_NAME__::send_trap_subscription_request_scx (uint32_t magazine, int target_timeout, const char * target_tag_list, const char * target_addr_params) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	uint32_t boards_found = 0;
	int call_result = 0;
	int failure_count = 0;


	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if (!_my_switch_board_infos[i].trap_subscription_state)
		{
			if (_my_switch_board_infos[i].magazine == magazine) {
				boards_found++;
				call_result = send_trap_subscription_request_scx(_my_switch_board_infos[i], target_timeout, target_tag_list, target_addr_params);
				if (call_result) {
					failure_count++;
					ACS_APBM_LOG(LOG_LEVEL_WARN,
							"Call 'send_trap_subscription_request_scx' failed: sending traps subscription request failed for switch board in my "
							"magazine 0x%08X, slot %d: call_result == %d", magazine, _my_switch_board_infos[i].slot_position, call_result);
				}
			}
		}
	}

	for (size_t i = 0; i < _other_switch_board_infos.size(); ++i) {
		if (!_other_switch_board_infos[i].trap_subscription_state)
		{
			if (_other_switch_board_infos[i].magazine == magazine) {
				boards_found++;

				call_result = send_trap_subscription_request_scx(_other_switch_board_infos[i], target_timeout, target_tag_list, target_addr_params);
				if (call_result) {
					failure_count++;
					ACS_APBM_LOG(LOG_LEVEL_WARN,
							"Call 'send_trap_subscription_request_scx' failed: sending traps subscription request failed for switch board in other "
							"magazine 0x%08X, slot %d: call_result == %d", magazine, _other_switch_board_infos[i].slot_position, call_result);
				}
			}
		}
	}

	if (!boards_found) return acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND;

	return (failure_count)? acs_apbm_snmp::ERR_SNMP_TRAP_SUBSCRIPTION: acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::send_trap_subscription_request_scx (int target_timeout, const char * target_tag_list, const char * target_addr_params) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs(true)) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int call_result = 0;
	int failure_counter = 0;

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if (!_my_switch_board_infos[i].trap_subscription_state)
		{
			call_result = send_trap_subscription_request_scx(_my_switch_board_infos[i], target_timeout, target_tag_list, target_addr_params);
			if (call_result) {
				failure_counter++;
				ACS_APBM_LOG(LOG_LEVEL_WARN,
						"Call 'send_trap_subscription_request_scx' failed: sending traps subscription request failed for switch board in slot %d: call_result == %d",
						_my_switch_board_infos[i].slot_position, call_result);
			}
		}
	}

	return (failure_counter) ? acs_apbm_snmp::ERR_SNMP_TRAP_SUBSCRIPTION : acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	//return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} /*+*/

int __CLASS_NAME__::send_trap_subscription_request_scx (acs_apbm_switchboardinfo & switch_board) {
	return send_trap_subscription_request_scx(switch_board, 150000, "iss", "internet");
}

int __CLASS_NAME__::send_trap_subscription_request_scx (
		acs_apbm_switchboardinfo & switch_board,
		int target_timeout,
		const char * target_tag_list,
		const char * target_addr_params) {
	char my_ipn_addresses [2][16];
	int call_result = 0;

	if ((call_result = _server_working_set->cs_reader->get_my_ipn_addresses(my_ipn_addresses[0], my_ipn_addresses[1]))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'get_my_ipn_addresses' failed: cannot load my AP ipn addresses : call_result == %d", call_result);
		return call_result;
	}

	char * switch_board_ipn_str_addresses [2] = { switch_board.ipna_str, switch_board.ipnb_str };

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	call_result = 0;
	for (int i = 0; (i < 2) && !call_result; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = send_trap_subscription_request_scx(switch_board_ipn_str_addresses[ipn_index], my_ipn_addresses[ipn_index], target_timeout, target_tag_list, target_addr_params);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'send_trap_subscription_request_scx' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}

	if (call_result)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'send_trap_subscription_request_scx' failed on both ipns for switch board in slot %d",
				switch_board.slot_position);
	}
	else
	{
		switch_board.trap_subscription_state = true;
	}

	return call_result;
}

int __CLASS_NAME__::send_trap_subscription_request_scx (
		const char * switch_board_ip,
		const char * trapreceiver_ip,
		int target_timeout,
		const char * target_tag_list,
		const char * target_addr_params) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to send trap subscription request to switch board %s", switch_board_ip);

	int call_result = 0;

	acs_apbm_snmploggercallback callback("SNMP Response received for Trap Subscription request towards SCXs");
	acs_apbm_snmpsessionhandler session_handler;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	size_t oid_name_size = 0;
	unsigned timeout_ms = 2000;

	uint8_t trap_receiver_ip_octets [4] = {0};
	acs_apbm_csreader::ip_format_to_ip_fields(trap_receiver_ip_octets, trapreceiver_ip);

	//Convert to uint32...
	uint32_t trapreceiver_ip_uint = 0;
	acs_apbm_csreader::ip_format_to_uint32(trapreceiver_ip_uint, trapreceiver_ip);
	trapreceiver_ip_uint = htonl(trapreceiver_ip_uint);

	//...now convert to hex in ASCII...
	char trapreceiver_ip_ascii[ASCII_IP_SIZE] = {0};
	sprintf(trapreceiver_ip_ascii, "%#0x", trapreceiver_ip_uint);

	//...finally convert each ASCII symbol in int.
	oid trapreceiver_ip_hex[10] = {0};
	for (uint16_t i = 0; i < ASCII_IP_SIZE; i++)
	{
		trapreceiver_ip_hex[i] = oid(trapreceiver_ip_ascii[i]);
		//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASCII_CHAR %d", trapreceiver_ip_hex[i]);
	}

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASCII_ADDRESS %s", trapreceiver_ip_ascii);


	{
		// Setting Target Address Row Status
		oid base_oid[] = { ACS_APBM_SCX_OID_TARGET_ADDR_ROW_STATUS };
		oid oid_name_row_status [ASCII_IP_SIZE + ACS_APBM_ARRAY_SIZE(base_oid)] = {0};

		memcpy(oid_name_row_status, base_oid, sizeof(base_oid));
		memcpy(oid_name_row_status + ACS_APBM_ARRAY_SIZE(base_oid), trapreceiver_ip_hex, sizeof(trapreceiver_ip_hex));

		oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_row_status);

		if ((call_result = set_targetAddrRowStatus(acs_apbm_snmp::snmptarget::CREATE_AND_GO, session_handler, oid_name_row_status, oid_name_size, &timeout_ms))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'set_targetAddrRowStatus' failed: call_result == %d", call_result);
			session_handler.close();
			return call_result;
		}
	}

	{
		// Setting Target Address TAddress
		oid base_oid[] = { ACS_APBM_SCX_OID_TARGET_ADDR_TADDRESS };
		oid oid_name_taddress [ASCII_IP_SIZE + ACS_APBM_ARRAY_SIZE(base_oid)] = {0};
		//oid oid_name_taddress [] = { ACS_APBM_SCX_OID_TARGET_ADDR_TADDRESS, trap_receiver_ip_octets[0], trap_receiver_ip_octets[1], trap_receiver_ip_octets[2], trap_receiver_ip_octets[3] };

		memcpy(oid_name_taddress, base_oid, sizeof(base_oid));
		memcpy(oid_name_taddress + ACS_APBM_ARRAY_SIZE(base_oid), trapreceiver_ip_hex, sizeof(trapreceiver_ip_hex));

		oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_taddress);

		if ((call_result = set_targetAddrTAddress(trap_receiver_ip_octets, session_handler, oid_name_taddress, oid_name_size, &timeout_ms))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'set_targetAddrTAaddress' failed: call_result == %d", call_result);
			session_handler.close();
			return call_result;
		}
	}

	{
		// Setting Target Timeout
		oid base_oid[] = { ACS_APBM_SCX_OID_TARGET_TIMEOUT };
		oid oid_name_timeout [ASCII_IP_SIZE + ACS_APBM_ARRAY_SIZE(base_oid)] = {0};
		//oid oid_name_timeout [] = { ACS_APBM_SCX_OID_TARGET_TIMEOUT, trap_receiver_ip_octets[0], trap_receiver_ip_octets[1], trap_receiver_ip_octets[2], trap_receiver_ip_octets[3] };
		oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_timeout);

		memcpy(oid_name_timeout, base_oid, sizeof(base_oid));
		memcpy(oid_name_timeout + ACS_APBM_ARRAY_SIZE(base_oid), trapreceiver_ip_hex, sizeof(trapreceiver_ip_hex));

		if ((call_result = set_targetTimeout(target_timeout, session_handler, oid_name_timeout, oid_name_size, &timeout_ms))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'set_targetTimeout' failed: call_result == %d", call_result);
			session_handler.close();
			return call_result;
		}
	}

	{
		// Setting Target Address Tag List
		oid base_oid[] = { ACS_APBM_SCX_OID_TARGET_ADDR_TAG_LIST };
		oid oid_name_taglist[ASCII_IP_SIZE + ACS_APBM_ARRAY_SIZE(base_oid)] = {0};
		//oid oid_name_taglist [] = { ACS_APBM_SCX_OID_TARGET_ADDR_TAG_LIST, trap_receiver_ip_octets[0], trap_receiver_ip_octets[1], trap_receiver_ip_octets[2], trap_receiver_ip_octets[3] };
		oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_taglist);

		memcpy(oid_name_taglist, base_oid, sizeof(base_oid));
		memcpy(oid_name_taglist + ACS_APBM_ARRAY_SIZE(base_oid), trapreceiver_ip_hex, sizeof(trapreceiver_ip_hex));

		if ((call_result = set_targetAddrTagList(target_tag_list, session_handler, oid_name_taglist, oid_name_size, &timeout_ms))) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'set_targetAddrTagList' failed: call_result == %d", call_result);
			session_handler.close();
			return call_result;
		}
	}

	{
	// Setting Target Address Params
	oid base_oid[] = { ACS_APBM_SCX_OID_TARGET_ADDR_PARAMS };
	oid oid_name_Params[ASCII_IP_SIZE + ACS_APBM_ARRAY_SIZE(base_oid)] = {0};
	//oid oid_name_Params [] = { ACS_APBM_SCX_OID_TARGET_ADDR_PARAMS, trap_receiver_ip_octets[0], trap_receiver_ip_octets[1], trap_receiver_ip_octets[2], trap_receiver_ip_octets[3] };
	oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_Params);

	memcpy(oid_name_Params, base_oid, sizeof(base_oid));
	memcpy(oid_name_Params + ACS_APBM_ARRAY_SIZE(base_oid), trapreceiver_ip_hex, sizeof(trapreceiver_ip_hex));

	if ((call_result = set_targetAddrParams(target_addr_params, session_handler, oid_name_Params, oid_name_size, &timeout_ms))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'set_targetAddrParams' failed: call_result == %d", call_result);
		session_handler.close();
		return call_result;
	}
	}

	session_handler.close();

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::send_trap_unsubscription_request_scx () {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Inside send_trap_unsubscription_request_scx ");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int call_result = 0;
//	int failure_counter = 0;

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		call_result = send_trap_unsubscription_request_scx(_my_switch_board_infos[i]);
		if (call_result) {
//			failure_counter++;
			ACS_APBM_LOG(LOG_LEVEL_WARN,
				"Call 'send_trap_unsubscription_request_scx' failed: sending traps unsubscription request failed for switch board in slot %d: call_result == %d",
				_my_switch_board_infos[i].slot_position, call_result);
		}
	}

//	return (failure_counter >= _my_switch_board_infos.size()) ? acs_apbm::ERR_SEND_PRIMITIVE_SUBSCRIBE : call_result;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} /*+*/

int __CLASS_NAME__::send_trap_unsubscription_request_scx (uint32_t magazine) {
        ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Inside send_trap_unsubscription_request_scx with parameter as magazine ");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	uint32_t boards_found = 0;
	int call_result = 0;

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if (_my_switch_board_infos[i].magazine == magazine) {
			boards_found++;

			call_result = send_trap_unsubscription_request_scx(_my_switch_board_infos[i]);
			if (call_result) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'send_trap_unsubscription_request_scx' failed: sending traps unsubscription request failed for switch board in my "
					"magazine 0x%08X, slot %d: call_result == %d", magazine, _my_switch_board_infos[i].slot_position, call_result);
			}
		}
	}

	for (size_t i = 0; i < _other_switch_board_infos.size(); ++i) {
		if (_other_switch_board_infos[i].magazine == magazine) {
			boards_found++;

			call_result = send_trap_unsubscription_request_scx(_other_switch_board_infos[i]);
			if (call_result) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
					"Call 'send_trap_unsubscription_request_scx' failed: sending traps unsubscription request failed for switch board in other "
					"magazine 0x%08X, slot %d: call_result == %d", magazine, _other_switch_board_infos[i].slot_position, call_result);
			}
		}
	}

	if (!boards_found) return acs_apbm_snmp::ERR_SNMP_NO_SWITCH_BOARD_FOUND;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::send_trap_unsubscription_request_scx (acs_apbm_switchboardinfo & switch_board) {
	char my_ipn_addresses [2][16];
	int call_result = 0;

	if ((call_result = _server_working_set->cs_reader->get_my_ipn_addresses(my_ipn_addresses[0], my_ipn_addresses[1]))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"Call 'get_my_ipn_addresses' failed: cannot load my AP ipn addresses : call_result == %d", call_result);
		return call_result;
	}

	char * switch_board_ipn_str_addresses [2] = { switch_board.ipna_str, switch_board.ipnb_str };

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = send_trap_unsubscription_request_scx(switch_board_ipn_str_addresses[ipn_index], my_ipn_addresses[ipn_index]);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'send_trap_unsubscription_request_scx' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}

	// set it to false even if the snmp query failed. Maybe the board was never actually subscribed so the unsubscription attempt may have failed.
	switch_board.trap_subscription_state = false;

	return call_result;
}

int __CLASS_NAME__::send_trap_unsubscription_request_scx (
		const char * switch_board_ip,
		const char * trapreceiver_ip) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to send trap unsubscription request to switch board %s", switch_board_ip);

	int call_result = 0;

	acs_apbm_snmploggercallback callback("SNMP Response received for Trap Unsubscription request towards SCXs");
	acs_apbm_snmpsessionhandler session_handler;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	size_t oid_name_size = 0;
	unsigned timeout_ms = 2000;

	uint8_t trap_receiver_ip_octets [4] = {0};
	acs_apbm_csreader::ip_format_to_ip_fields(trap_receiver_ip_octets, trapreceiver_ip);

	//Convert to uint32...
	uint32_t trapreceiver_ip_uint = 0;
	acs_apbm_csreader::ip_format_to_uint32(trapreceiver_ip_uint, trapreceiver_ip);
	trapreceiver_ip_uint = htonl(trapreceiver_ip_uint);

	//...now convert to hex in ASCII...
	char trapreceiver_ip_ascii[ASCII_IP_SIZE] = {0};
	sprintf(trapreceiver_ip_ascii, "%#0x", trapreceiver_ip_uint);

	//...finally convert each ASCII symbol in int.
	oid trapreceiver_ip_hex[10] = {0};
	for (uint16_t i = 0; i < ASCII_IP_SIZE; i++)
	{
		trapreceiver_ip_hex[i] = oid(trapreceiver_ip_ascii[i]);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASCII_CHAR %d", trapreceiver_ip_hex[i]);
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ASCII_ADDRESS %s", trapreceiver_ip_ascii);

	// Setting Target Address Row Status

	oid base_oid[] = { ACS_APBM_SCX_OID_TARGET_ADDR_ROW_STATUS };
	oid oid_name_row_status [ASCII_IP_SIZE + ACS_APBM_ARRAY_SIZE(base_oid)] = {0};

	memcpy(oid_name_row_status, base_oid, sizeof(base_oid));
	memcpy(oid_name_row_status + ACS_APBM_ARRAY_SIZE(base_oid), trapreceiver_ip_hex, sizeof(trapreceiver_ip_hex));

	//oid oid_name_row_status [] = { ACS_APBM_SCX_OID_TARGET_ADDR_ROW_STATUS, trap_receiver_ip_octets[0], trap_receiver_ip_octets[1], trap_receiver_ip_octets[2], trap_receiver_ip_octets[3] };
	oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_row_status);

	if ((call_result = set_targetAddrRowStatus(acs_apbm_snmp::snmptarget::DESTROY, session_handler, oid_name_row_status, oid_name_size, &timeout_ms))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'set_targetAddrRowStatus' failed: call_result == %d", call_result);
	}

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::set_targetAddrRowStatus (
		acs_apbm_snmp::snmptarget::addr_row_status_t addr_row_status,
		acs_apbm_snmpsessionhandler & session_handler,
		oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms
		) {
	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Row Status': OID '%s': value %d", oid_str, addr_row_status);

	int call_result = send_set_request(session_handler, oid_name, oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&addr_row_status), sizeof(addr_row_status), timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
	}

	return call_result;
}

int __CLASS_NAME__::set_targetAddrTAddress (
		uint8_t (& ip_octets) [4],
		acs_apbm_snmpsessionhandler & session_handler,
		oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms
		) {
	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Target Address TAddress': OID '%s': value HEX:%02X.%02X.%02X.%02X",
			oid_str, ip_octets[0], ip_octets[1], ip_octets[2], ip_octets[3]);

	int call_result = send_set_request(session_handler, oid_name, oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(ip_octets), sizeof(ip_octets), timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
	}

	return call_result;
}

int __CLASS_NAME__::set_targetTimeout (
		int target_timeout,
		acs_apbm_snmpsessionhandler & session_handler,
		oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms
		) {
	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Target Timeout': OID '%s': value %d", oid_str, target_timeout);

	int call_result = send_set_request(session_handler, oid_name, oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&target_timeout), sizeof(target_timeout), timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
	}

	return call_result;
}

int __CLASS_NAME__::set_targetAddrTagList (
		const char * target_tag_list,
		acs_apbm_snmpsessionhandler & session_handler,
		oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms
		) {
	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Target Address Tag List': OID '%s': value '%s'", oid_str, target_tag_list);

	int call_result = send_set_request(session_handler, oid_name, oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(target_tag_list), ::strlen(target_tag_list), timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
	}

	return call_result;
}

int __CLASS_NAME__::set_targetAddrParams (
		const char * target_addr_params,
		acs_apbm_snmpsessionhandler & session_handler,
		oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms
		) {
	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending a PDU SET request 'Target Address Params': OID '%s': value '%s'", oid_str, target_addr_params);

	int call_result = send_set_request(session_handler, oid_name, oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(target_addr_params), ::strlen(target_addr_params), timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
	}

	return call_result;
}

int __CLASS_NAME__::send_trap_ack_scbrp (int16_t /*switch_board_slot_position*/, const char * ap_ip, const char * switch_board_ip) {
	acs_apbm_snmpsessioninfo session_info;
	session_info.version(SNMP_VERSION_2c);
	session_info.community(const_cast<char *>(get_snmp_community()));
	session_info.peername(const_cast<char *>(switch_board_ip));

	// The last suboid has to be 0 (zero) and not the SCBRP slot number
	oid oid_trap_ack [] = { ACS_APBM_SCB_OID_ACTIVE_MANAGER_TRAP_ACK, 0 };

	// set the switch_board slot position into the trap oid definition
//	oid_trap_ack[ACS_APBM_ARRAY_SIZE(oid_trap_ack) - 1] = switch_board_slot_position;

	// snmp session communication handler used to send request and the callback used to receive the ACK response
	acs_apbm_snmpsessionhandler session_handler;
	acs_apbm_snmploggercallback logger_callback("SNMP Response received for trap acknowledgment");

	int call_result = 0;

	if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
				"snmp error text == '%s': call_result == %d",
				session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);

		return acs_apbm_snmp::ERR_SNMP_SESSION_OPEN;
	}

	// Prepare the PDU ack response APBM will send to the current switch board
	netsnmp_pdu * request_pdu = 0;

	if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on craeting a new PDU structure to carry out the TRAP ACK");

		session_handler.close();

		return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
	}
	// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
	request_pdu->variables = 0;

	uint8_t trap_ack_value [4] = {0};
	acs_apbm_csreader::ip_format_to_ip_fields(trap_ack_value, ap_ip);

	snmp_pdu_add_variable(request_pdu, oid_trap_ack, ACS_APBM_ARRAY_SIZE(oid_trap_ack),
			ASN_APPLICATION, reinterpret_cast<u_char *>(trap_ack_value), sizeof(trap_ack_value));

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending trap acknowledgment to switch board IP == '%s'", switch_board_ip );

	unsigned timeout_ms = 200; // 200 milliseconds timeout

	call_result = session_handler.send_synch(request_pdu, &timeout_ms);
	snmp_free_pdu(request_pdu);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired: this is not a problem because we are acknowledging a trap
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Timeout expired while acknowledging a trap message towards switch board IP == '%s'", switch_board_ip);
		call_result = 0;
	}
	if (call_result) {
                     errno=0;                
 // ERROR: sending the trap acknowledgment
                  int errno_save = errno;
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
				"Call 'send_synch' failed: NET-SNMP library internal error on sending the TRAP acknowledgment: call_result == %d", call_result);
	}

	session_handler.close();

	return (call_result ? acs_apbm_snmp::ERR_SNMP_SESSION_SEND : acs_apbm_snmp::ERR_SNMP_NO_ERRORS);
}

// NEW METHOD
int __CLASS_NAME__::set_switch_board_state(int idx_scb, int  state) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	// TODO:  insert new check on idx_scb
	_my_switch_board_infos[idx_scb].state = state;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "switch_board_%d is updated: state == %d - neighbour == %d", _my_switch_board_infos[idx_scb].slot_position, state, _my_switch_board_infos[idx_scb].neighbour_state);

	bool isCBA = _server_working_set->cs_reader->is_cba_system();
	if(state == SWITCH_BOARD_STATE_ACTIVE(isCBA)){
		set_my_active_switch_board_index (idx_scb);
	}
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::set_switch_board_state(int slot, uint32_t magazine, int state)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Return values must be substituted with specific values
	uint32_t my_mag_num = acs_apbm::BOARD_MAGAZINE_UNDEFINED;
	int call_result = _server_working_set->cs_reader->get_my_magazine(my_mag_num);

	if(call_result)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "failure updating SCB state for SCB identified by: (SLOT == %d, MAG == 0x%08X). Call 'acs_apbm_csreader::get_my_magazine()' failed !. call_result == %d '", slot, magazine, call_result);
		return acs_apbm::ERR_CS_ERROR;
	}
	bool isCBA = _server_working_set->cs_reader->is_cba_system();
	bool is_my_mag = (my_mag_num == magazine);
	std::vector<acs_apbm_switchboardinfo> * p_scb_infos = (is_my_mag ? & _my_switch_board_infos : & _other_switch_board_infos);
	bool scb_found = false;
	for(std::vector<acs_apbm_switchboardinfo>::iterator it = p_scb_infos->begin(); !scb_found && it != p_scb_infos->end(); ++ it)
	{
		if(it->magazine == magazine && it->slot_position == slot)
		{
			it->state = state;
			scb_found = true;
			if(is_my_mag && (state == SWITCH_BOARD_STATE_ACTIVE(isCBA)))
				set_my_active_switch_board_index(index_from_slot_position(slot));
		}
	}

	if(!scb_found)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No SCB found having: (SLOT == %d, MAG == 0x%08X) !", slot, magazine);
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "switch_board_%d in magazine == %u is updated: state == %d", slot, magazine , state);
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::set_scb_passive (int scbrpSlotPos, bool updateState) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to let passive the switch board at slot position %d!", scbrpSlotPos);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int idx_scb = index_from_slot_position(scbrpSlotPos);

	char *scb_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { _my_switch_board_infos[idx_scb].ipna_str, _my_switch_board_infos[idx_scb].ipnb_str };

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (i < 2) && call_result; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_shelf_mngr_ctrl_state_scbrp(scbrpSlotPos, scb_ipn_str_addresses[ipn_index], acs_apbm_snmp::SHELF_MNGR_CTRL_STATE_NOT_SCAN);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Call 'set_shelf_mngr_ctrl_state_scbrp' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
				scbrpSlotPos, scb_ipn_str_addresses[ipn_index], call_result);
	}
	if(call_result) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_shelf_mngr_ctrl_state_scbrp' failed: NO SCAN not sent to the switch board at slot position %d!", scbrpSlotPos);
		// TODO ...what ?
		if (updateState) set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN);
	}
	else if (updateState ){
		set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_PASSIVE);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "The switch board (at slot position %d) was successfully set to passive state!", scbrpSlotPos);
	}
	return call_result;
} //+

int __CLASS_NAME__::set_scb_active (int scbrpSlotPos) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to let active the switch board at slot position %d!", scbrpSlotPos);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int idx_scb = index_from_slot_position(scbrpSlotPos);

	char *scb_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { _my_switch_board_infos[idx_scb].ipna_str, _my_switch_board_infos[idx_scb].ipnb_str };

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (i < 2) && call_result; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_shelf_mngr_ctrl_state_scbrp(scbrpSlotPos, scb_ipn_str_addresses[ipn_index], acs_apbm_snmp::SHELF_MNGR_CTRL_STATE_SCAN);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Call 'set_shelf_mngr_ctrl_state_scbrp' failed: cannot send SCAN to the switch board slot == %d, switch board IPN = '%s': call_result == %d",
				scbrpSlotPos, scb_ipn_str_addresses[ipn_index], call_result);
	}
	if(call_result) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Call 'set_shelf_mngr_ctrl_state_scbrp' failed: SCAN not sent to the switch board at slot position %d!", scbrpSlotPos);
		// TODO - ...what?
	}
	else {
		set_switch_board_state(idx_scb, acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "The switch board (at slot position %d) was successfully set to active state!", scbrpSlotPos);
	}
	return call_result;
} //+

int __CLASS_NAME__::get_my_operational_switch_board_ipns (char (& switch_board_ipna) [16], char (& switch_board_ipnb) [16], int32_t board_slot) {
	int switch_board_idx;
	axe_eth_shelf::bus_type_t board_bus_type;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	// get board bus_type
	acs_apbm_shelvesdatamanager::sdm_constant result = _server_working_set->shelves_data_manager->get_ap_board_bus_type(board_bus_type, board_slot);

	if (result != acs_apbm_shelvesdatamanager::SDM_OK){
			ACS_APBM_LOG(LOG_LEVEL_WARN,
						"Error in retrieving bus_type for board in the slot (%d) error == %d", board_slot, result);
			return  acs_apbm_snmp::ERR_SNMP_NO_OPERATIONAL_SWITCH_BOARD;
	}

	switch(board_bus_type){
			case axe_eth_shelf::BUS_TYPE_IPMI:
				switch_board_idx = get_my_active_switch_board_index();
				break;
			case axe_eth_shelf::BUS_TYPE_MBUS:
				{
					int32_t shMCslotPos = (board_slot <= acs_apbm_snmp::MAX_SHELF_SLOT_POS_LEFT) ? axe_eth_shelf::SCBRP_SLOT_POS_LEFT
																						 : axe_eth_shelf::SCBRP_SLOT_POS_RIGHT;
					switch_board_idx = index_from_slot_position(shMCslotPos);
				}
				break;
			default:
				ACS_APBM_LOG(LOG_LEVEL_WARN,
							"Call 'get_ap_board_bus_type' failed: bad bus_type (%d) value returned for board in the slot (%d)", board_bus_type, board_slot);
				switch_board_idx = -1;
				break;
	}
	if ((switch_board_idx < 0)) {
	  		// ERROR: No operational switch board retrieved
	  		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in retrieving operational switch board for board in slot %d bus_type = %d", board_slot, board_bus_type);
	  		return acs_apbm_snmp::ERR_SNMP_NO_OPERATIONAL_SWITCH_BOARD;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "operational switch board at position %d retrieved for board in slot %d (bus_type = %d)",switch_board_idx, board_slot, board_bus_type);
	acs_apbm_switchboardinfo & operational_switch_board = _my_switch_board_infos.at(switch_board_idx);

	::strncpy(switch_board_ipna, operational_switch_board.ipna_str, ACS_APBM_ARRAY_SIZE(switch_board_ipna));
	::strncpy(switch_board_ipnb, operational_switch_board.ipnb_str, ACS_APBM_ARRAY_SIZE(switch_board_ipnb));

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::get_my_active_switch_board_ipns (char (& switch_board_ipna) [16], char (& switch_board_ipnb) [16]) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int active_index = get_my_active_switch_board_index();
	if (active_index == acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD) {
		// ERROR: No active switch board defined
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot retrieve active switch board ipns: there is no active switch board defined");
		return active_index;
	}

	acs_apbm_switchboardinfo & active_switch_board = _my_switch_board_infos.at(active_index);

	::strncpy(switch_board_ipna, active_switch_board.ipna_str, ACS_APBM_ARRAY_SIZE(switch_board_ipna));
	::strncpy(switch_board_ipnb, active_switch_board.ipnb_str, ACS_APBM_ARRAY_SIZE(switch_board_ipnb));

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::get_my_active_switch_board_index () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int switch_board_count = _my_switch_board_infos.size();
	if ((_my_active_switch_board_index < 0) || (switch_board_count <= _my_active_switch_board_index)) {
		// ERROR: No active switch board defined
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "There is no any active switch board defined");
		return acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD;
	}

	return _my_active_switch_board_index;
} //+

int __CLASS_NAME__::set_my_active_switch_board_index (int switch_board_index) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Setting active switch board index in local magazine <switch_board_index == %d>!", switch_board_index);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int switch_board_count = _my_switch_board_infos.size();
	if ((switch_board_index < 0) || (switch_board_count <= switch_board_index)) {
		// ERROR: No active switch board defined
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Invalid active switch board index parameter! ");
		return acs_apbm::ERR_INVALID_BOARD_INDEX;
	}

	_my_active_switch_board_index = switch_board_index;
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::initialize_my_active_switch_board_index () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Setting active switch board index in local magazine to '-1'!");

	_my_active_switch_board_index = -1;
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::open_snmp_session_handler (acs_apbm_snmpsessionhandler & session_handler, acs_apbm_snmpsessioncallback & callback, const char * switch_board_ip) {
	acs_apbm_snmpsessioninfo session_info;
	session_info.version(SNMP_VERSION_2c);
	session_info.community(const_cast<char *>(get_snmp_community()));
	session_info.peername(const_cast<char *>(switch_board_ip));

	int call_result = 0;

	if ((call_result = session_handler.open(session_info, callback))) { // ERROR: opening the communication session towards the switch board
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open' failed: cannot open the communication session towards the SNMP agent at address '%s': "
				"system error == %d, snmp error == %d, snmp error text == '%s': call_result == %d",
				switch_board_ip, session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);

		return acs_apbm_snmp::ERR_SNMP_SESSION_OPEN;
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::create_snmp_pdu (netsnmp_pdu * (& snmp_pdu), int command) {
	netsnmp_pdu * pdu = 0;

	if (!(pdu = snmp_pdu_create(command))) { // ERROR: creating a new PDU structure
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on creating a new PDU structure: command == %d", command);

		return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
	}
	// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
	pdu->variables = 0;

	snmp_pdu = pdu;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::add_snmp_null_var (netsnmp_pdu * pdu, const oid * name, size_t name_size) {
	if (!snmp_add_null_var(pdu, name, name_size)) { // ERROR: adding the requested data the PDU
		char name_str [1024] = {0};

		acs_apbm_snmpmanager::oid_to_str(name_str, ACS_APBM_ARRAY_SIZE(name_str), name, name_size);

		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'snmp_add_null_var' failed: cannot add the requested OID '%s' to the PDU request", name_str);

		return acs_apbm_snmp::ERR_SNMP_PDU_ADD_NULL_VARIABLE;
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::get_switch_board_info_ptr_by_ip(acs_apbm_switchboardinfo * & p_switch_board_info_ptr, bool & scb_is_in_local_magazine, const char * switch_board_ip)
{
	if(!switch_board_ip || !*switch_board_ip)
		return acs_apbm::ERR_NULL_POINTER;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// search first in own magazine info
	bool scb_found = false;
	for(std::vector<acs_apbm_switchboardinfo>::iterator it = _my_switch_board_infos.begin(); !scb_found && it != _my_switch_board_infos.end(); ++ it)
	{
		if( !strncmp(switch_board_ip, it->ipna_str, ACS_APBM_ARRAY_SIZE(it->ipna_str)) || !strncmp(switch_board_ip, it->ipnb_str, ACS_APBM_ARRAY_SIZE(it->ipnb_str)))
		{
			p_switch_board_info_ptr = & (*it);
			scb_found = true;
			scb_is_in_local_magazine = true;
		}		
	}

	// ... and then, if we are running on CBA system, search in other magazine info
	if(!scb_found && _server_working_set->cs_reader->is_cba_system())
	{
		for(std::vector<acs_apbm_switchboardinfo>::iterator it = _other_switch_board_infos.begin(); !scb_found && it != _other_switch_board_infos.end(); ++ it)
		{
			if( !strncmp(switch_board_ip, it->ipna_str, ACS_APBM_ARRAY_SIZE(it->ipna_str)) || !strncmp(switch_board_ip, it->ipnb_str, ACS_APBM_ARRAY_SIZE(it->ipnb_str)))
			{
				p_switch_board_info_ptr = & (*it);
				scb_found = true;
				scb_is_in_local_magazine = false;
			}
		}
	}

	if(!scb_found)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No switch board found having IP == '%s'", switch_board_ip);
		return acs_apbm::ERR_CS_BOARD_NOT_FOUND;
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::oid_to_str (char * buf, size_t size, const oid * id, size_t id_length) {
	if (!buf) return acs_apbm_snmp::ERR_SNMP_NULL_PARAMETER;

	*buf = 0;

	if (id)
		for (size_t chars_wrote = 0; id_length-- && (size > 1); buf += chars_wrote, size -= chars_wrote)
			if ((chars_wrote = ::snprintf(buf, size, ".%lu", *id++)) >= size) return acs_apbm_snmp::ERR_SNMP_NOT_ENOUGH_SPACE;

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::oid_compare (const oid * id1, size_t id1_length, const oid * id2, size_t id2_length) {
	if (id1) { if (!id2) return 1;}
	else return id2 ? -1: 0; // id1 is null

	// Here id1 and id2 are both not null.
	size_t length = (id1_length < id2_length ? id1_length : id2_length) + 1;

	while (--length && (*id1++ == *id2++)) ;

	return (length ? ((*(id1 - 1) < *(id2 - 1)) ? -1 : 1) : 0);
}



int __CLASS_NAME__::getHWIData (
		const char * mag_str,
		int32_t board_slot,
		const char * switch_board_ip1,
		const char * switch_board_ip2,
		const unsigned * timeout_ms,
		const int *oidArray,
		const int &oidArraySize,
		char *outVal,
		const int &outValSize,
		const bool &isHexVal) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Mag %s - slot %d - IP1 %s - IP2 %s ",
			mag_str, board_slot, switch_board_ip1, switch_board_ip2);

	int res = this->getHWIData(mag_str, board_slot, switch_board_ip1,
								timeout_ms, oidArray, oidArraySize,
								outVal, outValSize,	isHexVal);

	if (res < 0) {
		//retry with second IP
		res = this->getHWIData(mag_str, board_slot, switch_board_ip2,
									timeout_ms, oidArray, oidArraySize,
									outVal, outValSize,	isHexVal);
	}
	return res;
}

int __CLASS_NAME__::getHWIData (
		const char * mag_str,
		int32_t board_slot,
		const char * switch_board_ip,
		const unsigned * timeout_ms,
		const int *oidArray,
		const int &oidArraySize,
		char *outVal,
		const int &outValSize,
		const bool &isHexVal) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Mag %s - slot %d - IP %s ",
				mag_str, board_slot, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();
	if (! is_cba) return -1;

	acs_apbm_getHWIGenericCallback callback(mag_str, board_slot, switch_board_ip, is_cba,
											outVal, outValSize, isHexVal);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scx [oidArraySize];

	for (int i = 0 ; i < oidArraySize ; i++) {
		oid_name_scx [i] = oidArray[i];
	}


	const oid * oid_name = 0;
	size_t oid_name_size = 0;
	oid_name = oid_name_scx;
	oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx);

	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "getHWIBackplaneProdName OID '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "getHWIBackplaneProdName Timeout expired ");
	}
	else if (call_result != 0) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve board IPMI upgrade status: call_result == %d",
				call_result);
	}

	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::sendEgem2L2SwitchLog (const char * switch_board_ip) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX LOG for ip %s", switch_board_ip);

	int call_result = 0;
	acs_apbm_snmploggercallback callback("SCX/SMX LOG - start configuration and download");
	acs_apbm_snmpsessionhandler session_handler;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	size_t oid_name_size = 0;
	unsigned timeout_ms = 2000;
	std::string tftpRoot = "/data/apz/data";
	std::string tftpPath="";
	int env=0;
	_server_working_set->cs_reader->get_environment(env);
	if(env == ACS_CS_API_CommonBasedArchitecture::SCX)
		tftpPath = "/boot/scx/scx_logs/scx_";
	else if(env == ACS_CS_API_CommonBasedArchitecture::SMX)
		tftpPath = "/boot/smx/smx_logs/smx_";
		
	std::string clusterIP = "192.168.169.";
	std::string tmp(switch_board_ip);

	std::cout << "tmp " << tmp.c_str() << std::endl;

	int nodeNum = getNode();
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Node number = %d, IP addr. = %s", nodeNum, tmp.c_str());

	if (clusterIP.compare(tmp.substr(0,12)) == 0 ) {
		char c[16];
		sprintf(c, "%d", nodeNum);
		clusterIP += std::string(c);
	} else {
		clusterIP = "192.168.170.";
		char c[16];
		sprintf(c, "%d", nodeNum);
		clusterIP += std::string(c);
	}

	std::cout << "clusterIP = " << clusterIP.c_str() << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Cluster IP = %s ", clusterIP.c_str());

	int createRes;
	//createRes = createDir(tftpRoot + "/scx");
	if(env == ACS_CS_API_CommonBasedArchitecture::SCX)
	{
		createRes = createDir(tftpRoot + "/boot/scx");
		std::cout << "createRes = " << createRes << std::endl;

		//createRes = createDir(tftpRoot + "/scx/scx_logs");
		createRes = createDir(tftpRoot + "/boot/scx/scx_logs");
		std::cout << "createRes = " << createRes << std::endl;
	}
	else if(env == ACS_CS_API_CommonBasedArchitecture::SMX)
	{
		createRes = createDir(tftpRoot + "/boot/smx");
		std::cout << "createRes = " << createRes << std::endl;

		createRes = createDir(tftpRoot + "/boot/smx/smx_logs");
		std::cout << "createRes = " << createRes << std::endl;
	}
	
	//replace . with _
	//-- Remove this code if not necessary -- BEGIN
	std::string ipFiltered(switch_board_ip);
	if (ipFiltered.compare("")!=0){
		size_t foundDot;
		foundDot = ipFiltered.find_first_of(".");
		while (foundDot != string::npos){
			ipFiltered[foundDot] = '_';
			foundDot = ipFiltered.find_first_of(".",foundDot+1 );
		}
	}
	//-- Remove this code if not necessary -- END

	std::cout << "ipFiltered: " << ipFiltered.c_str() << std::endl;

	char logPath[256];
	sprintf (logPath, "%s%s%s",tftpRoot.c_str(), tftpPath.c_str(),ipFiltered.c_str());

	std::cout << "logPath = " << logPath << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Folder log path = %s ", logPath);

	createRes = createDir(std::string(logPath));
	std::cout << "createRes = " << createRes << std::endl;

	char snmplogPath[256];
	//sprintf (snmplogPath, "%s%s", tftpPathSCX.c_str(),ipFiltered.c_str());
	sprintf (snmplogPath, "%s%s", tftpPath.c_str(),ipFiltered.c_str());
	std::cout << "snmplogPath = " << snmplogPath << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP LOG Path = %s ", snmplogPath);


	/*
	//OID used in the windows apbm

	std::string oidContTransferSrvIP =             "1.3.6.1.4.1.193.177.2.2.4.4.1.1";
	std::string oidContTransferSrvPath =           "1.3.6.1.4.1.193.177.2.2.4.4.1.2";
	std::string oidContNotificationsCtrl =             "1.3.6.1.4.1.193.177.2.2.4.4.1.3";
	std::string oidContAutomaticTransfer  =             "1.3.6.1.4.1.193.177.2.2.4.4.1.4";
	std::string oidContFileEncryption  =             "1.3.6.1.4.1.193.177.2.2.4.4.1.6";
	std::string oidCreateContFile  =             "1.3.6.1.4.1.193.177.2.2.4.4.6";
	std::string oidTransContFile	=				"1.3.6.1.4.1.193.177.2.2.4.4.3";
	*/

	call_result = 0;
	do {
		//oidContTransferSrvIP
		oid oidContTransferSrvIP [] = {1,3,6,1,4,1,193,177,2,2,4,4,1,1,0};
		oid_name_size = ACS_APBM_ARRAY_SIZE(oidContTransferSrvIP);

		call_result = send_set_request(session_handler, oidContTransferSrvIP,
					oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(clusterIP.c_str()),
					::strlen(clusterIP.c_str()), &timeout_ms);
		/*TR HP95766: retry inserted on timeout*/
		int retryVal=0;

		while( retryVal<3  &&  call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request for oidContTransferSrvIP: retrying for %d times", retryVal);
			call_result = send_set_request(session_handler, oidContTransferSrvIP,
						oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(clusterIP.c_str()),
						::strlen(clusterIP.c_str()), &timeout_ms);
			retryVal++;
		}
		/*end TR HP95766: retry inserted on timeout */

		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
			break;
		} else if (call_result) { // ERROR: sending the request
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
			break;
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PDU SET request for oidContTransferSrvIP: success");


		//oidContTransferSrvPath
		oid oidContTransferSrvPath [] = {1,3,6,1,4,1,193,177,2,2,4,4,1,2,0};
		oid_name_size = ACS_APBM_ARRAY_SIZE(oidContTransferSrvPath);

		call_result = send_set_request(session_handler, oidContTransferSrvPath,
				oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(snmplogPath),
				::strlen(snmplogPath), &timeout_ms);

		/*TR HP95766: retry inserted on timeout*/
		retryVal=0;

		while( retryVal<3  &&  call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request for oidContTransferSrvPath: retrying for %d times", retryVal);
			call_result = send_set_request(session_handler, oidContTransferSrvPath,
					oid_name_size, ASN_OCTET_STR, reinterpret_cast<const u_char *>(snmplogPath),
					::strlen(snmplogPath), &timeout_ms);
			retryVal++;
		}

		/*end TR HP95766: retry inserted on timeout*/


		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
			break;
		} else if (call_result) { // ERROR: sending the request
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
			break;
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PDU SET request for oidContTransferSrvPath: success");

		/*
		Value used for INTEGER type:
			oidContNotificationsCtrl	(0 = no - 1 = yes)
			oidContFileEncryption		(0 = no - 1 = yes)
			oidCreateContFile			(0 = no - 1 = yes)
		*/
		int intValue = 1;

		//oidContNotificationsCtrl
		oid oidContNotificationsCtrl [] = {1,3,6,1,4,1,193,177,2,2,4,4,1,3,0};
		oid_name_size = ACS_APBM_ARRAY_SIZE(oidContNotificationsCtrl);

		call_result = send_set_request(session_handler, oidContNotificationsCtrl,
				oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
				sizeof(intValue), &timeout_ms);

		/*TR HP95766: retry inserted on timeout*/
		retryVal=0;
		while( retryVal<3  &&  call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request for oidContNotificationsCtrl: retrying for %d times", retryVal);
			call_result = send_set_request(session_handler, oidContNotificationsCtrl,
					oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
					sizeof(intValue), &timeout_ms);
			retryVal++;
		}
		/*end TR HP95766: retry inserted on timeout*/

		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
			break;
		} else if (call_result) { // ERROR: sending the request
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
			break;
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PDU SET request for oidContNotificationsCtrl: success");

		//oidContAutomaticTransfer
		oid oidContAutomaticTransfer [] = {1,3,6,1,4,1,193,177,2,2,4,4,1,4,0};
		oid_name_size = ACS_APBM_ARRAY_SIZE(oidContAutomaticTransfer);

		call_result = send_set_request(session_handler, oidContAutomaticTransfer,
				oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
				sizeof(intValue), &timeout_ms);

		/*TR HP95766: retry inserted on timeout*/
		retryVal=0;
		while( retryVal<3  &&  call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request for oidContAutomaticTransfer: retrying for %d times", retryVal);
			call_result = send_set_request(session_handler, oidContAutomaticTransfer,
					oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
					sizeof(intValue), &timeout_ms);
			retryVal++;
		}
		/*end TR HP95766: retry inserted on timeout*/

		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
			break;
		} else if (call_result) { // ERROR: sending the request
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
			break;
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PDU SET request for oidContAutomaticTransfer: success");

		//oidContFileEncryption
		oid oidContFileEncryption [] = {1,3,6,1,4,1,193,177,2,2,4,4,1,6,0};
		oid_name_size = ACS_APBM_ARRAY_SIZE(oidContFileEncryption);

		call_result = send_set_request(session_handler, oidContFileEncryption,
				oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
				sizeof(intValue), &timeout_ms);

		/*TR HP95766: retry inserted on timeout*/
		retryVal=0;
		while( retryVal<3  &&  call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request for oidContFileEncryption: retrying for %d times", retryVal);
			call_result = send_set_request(session_handler, oidContFileEncryption,
					oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
					sizeof(intValue), &timeout_ms);
			retryVal++;
		}
		/*end TR HP95766: retry inserted on timeout*/

		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
			break;
		} else if (call_result) { // ERROR: sending the request
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
			break;
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PDU SET request for oidContFileEncryption: success");

		//oidCreateContFile
		oid oidCreateContFile [] = {1,3,6,1,4,1,193,177,2,2,4,4,6,0};
		oid_name_size = ACS_APBM_ARRAY_SIZE(oidCreateContFile);

		call_result = send_set_request(session_handler, oidCreateContFile,
				oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
				sizeof(intValue), &timeout_ms);

		/*TR HP95766: retry inserted on timeout*/
		retryVal=0;
		while( retryVal<3  &&  call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request for  oidCreateContFile: retrying for %d times", retryVal);
			call_result = send_set_request(session_handler, oidCreateContFile,
					oid_name_size, ASN_INTEGER, reinterpret_cast<const u_char *>(&intValue),
					sizeof(intValue), &timeout_ms);
			retryVal++;
		}
		/*end TR HP95766: retry inserted on timeout*/

		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU SET request");
			break;
		} else if (call_result) { // ERROR: sending the request
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'send_set_request' failed: NET-SNMP library internal error on sending the SET request: call_result == %d", call_result);
			break;
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PDU SET request for oidCreateContFile: success");
	} while (0);

	session_handler.close();


	return call_result ? -1 : acs_apbm::ERR_NO_ERRORS;
}
int __CLASS_NAME__::createDir(std::string dirName) {

	struct stat dirbuf;

	if (::stat(dirName.c_str(), &dirbuf)!= 0) { //ERROR

		if (int resMk = ::mkdir(dirName.c_str(),  0777 ) ) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Impossible to create dir %s result == %d", dirName.c_str(), resMk);
			std::cout << "Creation failed res: " << resMk << std::endl;
			return -1;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Dir %s successfully created result == %d",dirName.c_str(), resMk);
			std::cout << "Dir successfully created " << resMk << std::endl;

			if (int resChmod = ::chmod(dirName.c_str(), 0777 ) ) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Impossible to change permissions dir %s result == %d", dirName.c_str(), resChmod);
				std::cout << "resChmod " << resChmod << std::endl;
				return -1;
			} else {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Permission applied dir %s result == %d", dirName.c_str(), resChmod);
				std::cout << "Chmod ok" << resChmod << std::endl;
			}
		}
	} else {
		std::cout << "Already presents: " << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Dir %s already presents",dirName.c_str());

		if (int resChmod = ::chmod(dirName.c_str(), 0777 ) ) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Try to change permission dir %s result == %d", dirName.c_str(), resChmod);
			std::cout << "resChmod " << resChmod << std::endl;
			return -1;
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Permission applied dir %s result == %d", dirName.c_str(), resChmod);
			std::cout << "Chmod ok" << resChmod << std::endl;
		}
	}
	return 0;
}

int __CLASS_NAME__::getNode(){

	std::string node_id_path = "/etc/cluster/nodes/this/id";

	fstream nodeFile;
	nodeFile.open(node_id_path.c_str(), ios::in);
	if (!nodeFile) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "File %s not found",node_id_path.c_str());
		std::cout << "File Not found" << std::endl;
		return -1;
	}
	char val[256];
	char *c = val;
	nodeFile.getline(c, 256);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "File %s Row: %s",node_id_path.c_str(), c);

	nodeFile.close();
	return ::atoi(c);
}

int __CLASS_NAME__::check_working_state_active_sb () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int result = 0;
	int active_idx = get_my_active_switch_board_index();

	if (active_idx == acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD)
		return acs_apbm_snmp::ERR_SNMP_NO_ACTIVE_SWITCH_BOARD;

	int activeSlotPos =_my_switch_board_infos[active_idx].slot_position;

	// insert new check on active scx
	result = get_ipmi_state( activeSlotPos);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Call 'get_ipmi_state' on the active switch_board_%d <result == %d>", activeSlotPos, result);
		// insert new check on active scx
	return  result;
} //+

int __CLASS_NAME__::start_switch_communication(acs_apbm_switchboardinfo & switch_board) {

	//TODO: must be checked. Error conditions must be handled and must be added the neighbour presence handling
	int result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	int call_result = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "starting communication with switch identified by: magazine == 0x%08X,  slot == %d", switch_board.magazine, switch_board.slot_position );


	// unsubscribe  APBM old trap receiver on both subnets
	//_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE);
	uint16_t fbn = 0;
	_server_working_set->cs_reader->get_switch_board_fbn(fbn);		
	if(fbn != ACS_CS_API_HWC_NS::FBN_SMXB)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "initiated a request to send_trap_unsubscription_request_scx");
		result= send_trap_unsubscription_request_scx(switch_board);
		if (result)
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'send_trap_unsubscription_request_switch' failed on both ips for switch board identified by: magazine == %u,  slot == %d.  <result == %d> !",
				switch_board.magazine,  switch_board.slot_position, result);

			// Reschedule the same operation after a default timeout
			if (( call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to "
						"retry the unsubscribe-subscribe process: call_result == %d", call_result);
			} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation rescheduled successfully");

			if (( call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
				acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE,
				acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_UNSUBSCRIBE_SUBSCRIBE operation to "
						"retry the unsubscribe-subscribe process: call_result == %d", call_result);
			} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_OTHER_MAGAZINES_TRAP_UNSUBSCRIBE_SUBSCRIBE operation rescheduled successfully");
		}
		else
		{
			//subscribe APBM trap receiver on both subnet
			result= send_trap_subscription_request_scx(switch_board);
		
			if (result)
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'send_trap_subscription_request_switch' failed on both ips for switch board identified by: magazine == %u,  slot == %d.  <result == %d> !",
					switch_board.magazine,  switch_board.slot_position, result);

				// Reschedule the same operation after a default timeout
				if ((call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_TRAP_SUBSCRIBE,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
						ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_SUBSCRIBE operation to retry "
							"the subscription process: call_result == %d", call_result);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_TRAP_SUBSCRIBE operation rescheduled successfully");

				if ((call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(
					acs_apbm::SERVER_WORK_OPERATION_SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait))) {
						ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'schedule_operation' failed: cannot reschedule an SNMP_TRAP_SUBSCRIBE operation to retry "
							"the subscription process: call_result == %d", call_result);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP_OTHER_MAGAZINES_TRAP_SUBSCRIBE operation rescheduled successfully");
			}
		}
	}

	// find and set master status
	unsigned timeout_ms = 1000;
	acs_apbm_snmp::switch_board_shelf_mgr_state_t shmgr_state = acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN;
	result = get_switch_board_status(shmgr_state, switch_board.slot_position, switch_board.magazine, switch_board.ipna_str, & timeout_ms);
	if (result)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_switch_board_status()' failed.  <result == %d> !", result);
	else
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Successfully got switch board status for switch board (MAG == 0x%08X, SLOT == %d). Switch board status is '%d'", switch_board.magazine, switch_board.slot_position, shmgr_state);

	call_result = set_switch_board_state(switch_board.slot_position, switch_board.magazine, shmgr_state);
	if(call_result) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'update_switch_board_state()' failed ! call_result == %d", call_result);
		result = acs_apbm_snmp::ERR_SNMP_INTERNAL_ERROR;
	}

	// read and set neighbour state
	acs_apbm_snmp::switch_board_neighbour_state_t neighbour_state = acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_UNKNOWN;
	result = get_shelf_neighbour_presence(neighbour_state, switch_board.slot_position, switch_board.magazine);
	if (result)
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_shelf_neighbour_presence()' failed.  <result == %d> !", result);
	else
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Successfully got switch board neighbour presence value for switch board (MAG == 0x%08X, SLOT == %d). Switch board neighbour value is '%d'", switch_board.magazine, switch_board.slot_position, neighbour_state);

	switch_board.neighbour_state = neighbour_state;

	return result;
}

int __CLASS_NAME__::switch_board_delete (uint32_t magazine, int32_t slot_pos) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// BEGIN: DEBUGGING and TRACING: TO BE DELETE
	ACS_APBM_LOG(LOG_LEVEL_TRACE, "The switch board being deleted is: magazine == %u, slot_position == %d",
			magazine, slot_pos);

	ACS_APBM_LOG(LOG_LEVEL_TRACE, "Dumping _my_switch_board_infos internal vector before deleting a SwitchBoard: _my_switch_board_infos.size() == %zu", _my_switch_board_infos.size());

	for (size_t index = 0; index < _my_switch_board_infos.size(); ++index) {
		acs_apbm_switchboardinfo & sbi_item = _my_switch_board_infos[index];

		ACS_APBM_LOG(LOG_LEVEL_TRACE, "MY SWITCH_BOARD_INFO[%zu] --> magazine == %u, slot_position == %d, IPN-A == '%s', IPN-B == '%s'",
				index, sbi_item.magazine, sbi_item.slot_position, sbi_item.ipna_str, sbi_item.ipnb_str);
	}
	// END: DEBUGGING and TRACING: TO BE DELETE

	// Searching and deleting element in _my_switch_board_infos vector
	std::vector<acs_apbm_switchboardinfo>::size_type old_size = _my_switch_board_infos.size();
	std::vector<acs_apbm_switchboardinfo>::iterator new_end = std::remove_if(_my_switch_board_infos.begin(), _my_switch_board_infos.end(), match_sb_magazine_slot(magazine, slot_pos));
	_my_switch_board_infos.erase(new_end, _my_switch_board_infos.end());
	std::vector<acs_apbm_switchboardinfo>::size_type new_size = _my_switch_board_infos.size();

	if (old_size == new_size) // No elements found and deleted
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No switch boards deleted from internal MY-SWITCH-BOARDS vector");
	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Found and deleted %zu switch board(s) from MY-SWITCH-BOARDS vector", (old_size - new_size));

	// BEGIN: DEBUGGING and TRACING: TO BE DELETE
		ACS_APBM_LOG(LOG_LEVEL_TRACE, "Dumping _other_switch_board_infos internal vector before deleting a SwitchBoard: _other_switch_board_infos.size() == %zu", _other_switch_board_infos.size());

		for (size_t index = 0; index < _other_switch_board_infos.size(); ++index) {
			acs_apbm_switchboardinfo & sbi_item = _other_switch_board_infos[index];

			ACS_APBM_LOG(LOG_LEVEL_TRACE, "OTHER SWITCH_BOARD_INFO[%zu] --> magazine == %u, slot_position == %d, IPN-A == '%s', IPN-B == '%s'",
					index, sbi_item.magazine, sbi_item.slot_position, sbi_item.ipna_str, sbi_item.ipnb_str);
		}
		// END: DEBUGGING and TRACING: TO BE DELETE

	// Searching and deleting element in _other_switch_board_infos vector
	old_size = _other_switch_board_infos.size();
	new_end = std::remove_if(_other_switch_board_infos.begin(), _other_switch_board_infos.end(), match_sb_magazine_slot(magazine, slot_pos));
	_other_switch_board_infos.erase(new_end, _other_switch_board_infos.end());
	new_size = _other_switch_board_infos.size();

	if (old_size == new_size) // No elements found and deleted
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No switch boards deleted from internal OTHER-SWITCH-BOARDS vector");
	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Found and deleted %zu switch board(s) from OTHER-SWITCH-BOARDS vector", (old_size - new_size));

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::switch_board_add (acs_apbm_switchboardinfo & new_switch_board) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// BEGIN: DEBUGGING and TRACING: TO BE DELETE
		ACS_APBM_LOG(LOG_LEVEL_TRACE, "The new switch board being added is: magazine == %u, slot_position == %d, IPN-A == '%s', IPN-B == '%s'",
				new_switch_board.magazine, new_switch_board.slot_position, new_switch_board.ipna_str, new_switch_board.ipnb_str);

		ACS_APBM_LOG(LOG_LEVEL_TRACE, "Dumping _my_switch_board_infos internal vector before adding a new SwitchBoard: _my_switch_board_infos.size() == %zu", _my_switch_board_infos.size());

		for (size_t index = 0; index < _my_switch_board_infos.size(); ++index) {
			acs_apbm_switchboardinfo & sbi_item = _my_switch_board_infos[index];

			ACS_APBM_LOG(LOG_LEVEL_TRACE, "MY SWITCH_BOARD_INFO[%zu] --> magazine == %u, slot_position == %d, IPN-A == '%s', IPN-B == '%s'",
					index, sbi_item.magazine, sbi_item.slot_position, sbi_item.ipna_str, sbi_item.ipnb_str);
		}
		// END: DEBUGGING and TRACING: TO BE DELETE

	int isCBA = _server_working_set->cs_reader->is_cba_system();

	uint32_t my_magazine = ~0;

	if (const int call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine)) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_my_magazine' failed: erro_ getting my magazine address from CS: call_result == %d", call_result);
			return call_result;
	}

	if(isCBA && (new_switch_board.magazine == my_magazine)){
		int env=0;
		_server_working_set->cs_reader->get_environment(env);
		if(env == ACS_CS_API_CommonBasedArchitecture::SCX)
		{
			char myMagazine_str [16] = {0};
			int call_result = -1;
			for (int i = 0; (call_result && i < 3); ++i) {
				call_result = acs_apbm_csreader::uint32_to_ip_format(myMagazine_str, my_magazine);
				if(!call_result){
					call_result = _server_working_set->imm_lag_handler->createLAGRootClass(myMagazine_str,new_switch_board.slot_position);
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG loading lagid objects as otherboard is added in myMagazine:%s slot:%d",myMagazine_str,new_switch_board.slot_position);
					if(call_result == -14)
						break;  // break incase LAG object already exist	
				}
			}
		}
	}

	// The switch board is already into _my_switch_board_infos vector?
	if (std::find_if(_my_switch_board_infos.begin(), _my_switch_board_infos.end(),
			match_sb_magazine_slot(new_switch_board.magazine, new_switch_board.slot_position)) != _my_switch_board_infos.end()) { // YES
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"The new switch board (magazine == 0x%08X, slot == %d) is already into MY-SWITCH-BOARD vector: no action will be performed",
				new_switch_board.magazine, new_switch_board.slot_position);

		return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	}

	// BEGIN: DEBUGGING and TRACING: TO BE DELETE
		ACS_APBM_LOG(LOG_LEVEL_TRACE, "Dumping _other_switch_board_infos internal vector before adding a new SwitchBoard: _other_switch_board_infos.size() == %zu", _other_switch_board_infos.size());

		for (size_t index = 0; index < _other_switch_board_infos.size(); ++index) {
			acs_apbm_switchboardinfo & sbi_item = _other_switch_board_infos[index];

			ACS_APBM_LOG(LOG_LEVEL_TRACE, "OTHER SWITCH_BOARD_INFO[%zu] --> magazine == %u, slot_position == %d, IPN-A == '%s', IPN-B == '%s'",
					index, sbi_item.magazine, sbi_item.slot_position, sbi_item.ipna_str, sbi_item.ipnb_str);
		}
		// END: DEBUGGING and TRACING: TO BE DELETE

	// The switch board is already into _other_switch_board_infos vector?
	if (std::find_if(_other_switch_board_infos.begin(), _other_switch_board_infos.end(),
			match_sb_magazine_slot(new_switch_board.magazine, new_switch_board.slot_position)) != _other_switch_board_infos.end()) { // YES
		ACS_APBM_LOG(LOG_LEVEL_WARN,
				"The new switch board (magazine == 0x%08X, slot == %d) is already into OTHER-SWITCH-BOARD vector: no action will be performed",
				new_switch_board.magazine, new_switch_board.slot_position);

		return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	}


	std::vector<acs_apbm_switchboardinfo> * switch_board_infos = ((new_switch_board.magazine == my_magazine) ? &_my_switch_board_infos : &_other_switch_board_infos);

	// Adding the new switch board
	switch_board_infos->push_back(new_switch_board);

	std::sort(switch_board_infos->begin(), switch_board_infos->end(),
                        (new_switch_board.magazine == my_magazine) ? fx_compare_slot_ascending : fx_compare_magazine_slot_ascending);

	// Starting communication (un/subscription) with the new board
	const int call_result = (isCBA) ? start_switch_communication(switch_board_infos->at(switch_board_infos->size() - 1)):
														start_scb_communication(switch_board_infos->at(switch_board_infos->size() - 1));

	if (call_result) { // ERROR: subscribing to the new switch board
		if(isCBA)
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'start_switch_communication' failed: cannot start the communication (trap subscription) towards the new inserted switch board: "
				"call_result == %d", call_result);
		else
			ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'start_scb_communication' failed: cannot start the communication (trap subscription) towards the new inserted switch board: "
				"call_result == %d", call_result);
	}


	// Reorder the collection
//	std::sort(switch_board_infos->begin(), switch_board_infos->end(),
//			(new_switch_board.magazine == my_magazine) ? fx_compare_slot_ascending : fx_compare_magazine_slot_ascending);

	return call_result;
}

int __CLASS_NAME__::switch_board_change (acs_apbm_switchboardinfo & switch_board) {
	// First delete the switch board ...
	switch_board_delete(switch_board.magazine, switch_board.slot_position);

	// ... next add the switch board
	return switch_board_add(switch_board);
}

int __CLASS_NAME__::get_ipmi_state (int scbrpSlotPos) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	// get own slot
	int32_t own_slot;
	int call_result = _server_working_set->cs_reader->get_my_slot(own_slot);
	if(call_result < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error getting own slot. Call 'acs_apbm_csreader::get_my_slot()' failed ! call_result == %d", call_result);
        return acs_apbm::ERR_OWN_SLOT_UNDEFINED;
	}

	int scb_index = index_from_slot_position(scbrpSlotPos);
	if(scb_index == -1)
		return acs_apbm_snmp::ERR_SNMP_INTERNAL_ERROR;

	char switch_board_ipns [2][16];
	::strncpy(switch_board_ipns[0], _my_switch_board_infos[scb_index].ipna_str, 16);
	::strncpy(switch_board_ipns[1], _my_switch_board_infos[scb_index].ipnb_str, 16);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Getting IPMI Bus state of switch board at slot position == %d with IPNs '%s' '%s'",
			                                                              scbrpSlotPos, switch_board_ipns[0], switch_board_ipns[1]);

	uint32_t my_magazine = 0;
	if ((call_result = _server_working_set->cs_reader->get_my_magazine(my_magazine))) // ERROR: getting my magazine number
		return call_result;

	call_result = 0;
	int ipn_index = 0;
	unsigned timeout_ms = 1000;
	axe_eth_shelf::led_status_t red_led;
        if(! _server_working_set->shelves_data_manager->is_4led_supported || ! _server_working_set->shelves_data_manager->is_new_gea_supported)  
	    red_led = static_cast<axe_eth_shelf::led_status_t>(2); //GEP1/2 axe_eth_shelf::LED_STATUS_NOT_AVAILABLE;
        else
	    red_led = static_cast<axe_eth_shelf::led_status_t>(2);//axe_eth_shelf::LED_STATUS_NOT_AVAILABLE // GEP4
           

	::srandom(static_cast<unsigned>(::time(0)));
	// Gets the red led status
	ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {

		    call_result = get_board_red_led_status(red_led, own_slot, my_magazine, switch_board_ipns[ipn_index], &timeout_ms);

		if (!call_result) break; // OK: mia led retrieved successfully
	}

	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'get_board_red_led_status' failed: cannot retrieve RED led status info: call_result == %d", call_result);
	}

	return (call_result)? acs_apbm_snmp::ERR_SNMP_IPMI_ACTIVE_NOTWORKING: acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
} //+

int __CLASS_NAME__::set_shelf_neighbour_presence () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int call_result = 0;
	int err_cnt = 0;

	//unsigned timeout_ms = 1000;

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		acs_apbm_snmp::switch_board_neighbour_state_t neighbour_state = acs_apbm_snmp::SWITCH_BOARD_NEIGHBOUR_STATE_UNKNOWN;
		call_result = get_shelf_neighbour_presence(neighbour_state, _my_switch_board_infos[i].slot_position, _my_switch_board_infos[i].magazine);
		if (call_result) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,
				"Call 'get_shelf_neighbour_presence' failed: finding the neighbour presence for the switch board in the slot %d: call_result == %d",
				_my_switch_board_infos[i].slot_position, call_result);
			++ err_cnt;
		}
		else
			_my_switch_board_infos[i].neighbour_state = neighbour_state;
	}

	return err_cnt ? acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_GENERR: acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}

int __CLASS_NAME__::get_shelf_neighbour_presence (acs_apbm_snmp::switch_board_neighbour_state_t & neighbour_presence, acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms) {
	const char * switch_board_ipn_str_addresses [2] = { switch_board.ipna_str, switch_board.ipnb_str };
	int call_result = 0;

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_shelf_neighbour_presence(neighbour_presence, switch_board.slot_position, switch_board.magazine, switch_board_ipn_str_addresses[ipn_index], timeout_ms);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'get_shelf_neighbour_presence' failed: magazine == 0x%08X, switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.magazine, switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
		else break;
	}

	return call_result;
}

int __CLASS_NAME__::get_shelf_neighbour_presence (acs_apbm_snmp::switch_board_neighbour_state_t & neighbour_presence, int scbrpSlotPos, uint32_t magazine) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	int scbx_index = index_from_slot_position(scbrpSlotPos);
	if(scbx_index == -1)
		return acs_apbm_snmp::ERR_SNMP_INTERNAL_ERROR;

	char switch_board_ipns [2][16];
	::strncpy(switch_board_ipns[0], _my_switch_board_infos[scbx_index].ipna_str, 16);
	::strncpy(switch_board_ipns[1], _my_switch_board_infos[scbx_index].ipnb_str, 16);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Getting neighbour state of switch board at slot position == %d with IPNs '%s' '%s'",
			                                                              scbrpSlotPos, switch_board_ipns[0], switch_board_ipns[1]);

	int call_result = 0;
	int ipn_index = 0;
	unsigned timeout_ms = 1000;

	::srandom(static_cast<unsigned>(::time(0)));
	// Gets the mia led status
	ipn_index = ::random() & 0x01;
	for (int i = 0; i < 2; ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_shelf_neighbour_presence(neighbour_presence, scbrpSlotPos, magazine, switch_board_ipns[ipn_index], &timeout_ms);

		if (!call_result) break; // OK: neighbour presence retrieved successfully
	}

	if (call_result) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'get_shelf_neighbour_presence' failed: cannot retrieve shelf neighbour presence info: call_result == %d", call_result);
	}

	return call_result;
}

int __CLASS_NAME__::get_shelf_neighbour_presence (
		acs_apbm_snmp::switch_board_neighbour_state_t & neighbour_presence,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};

	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get neighbour presence information from switch board in slot %d of magazine %s using switch board IPN '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	int shelf_neighbour = -1;

	acs_apbm_getshelfneighbourcallback callback(shelf_neighbour, board_magazine_str, board_slot, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_name_scx [] = { ACS_APBM_SCX_OID_SHELF_MGR_NEIGHBOUR_PRESENCE, 0 };
	size_t oid_name_size = ACS_APBM_ARRAY_SIZE(oid_name_scx);

	char oid_str [512];
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name_scx, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve neighbour presence value into the OID '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_name_scx, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request to retrieve board BIOS run mode");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve shelfneighbourpresence: call_result == %d",
				call_result);
	}
	else if (callback.error_code) {
	// Here a possible error in pdu data should be verified
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading shelfneighbourpresence from response PDU: "
					"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}
	else /* callback.error_code == 0*/
		neighbour_presence = static_cast<acs_apbm_snmp::switch_board_neighbour_state_t>(*callback.shelfneighbour);


	session_handler.close();

	return call_result;
}

int __CLASS_NAME__::get_board_red_led_status (
		axe_eth_shelf::led_status_t & red_led,
		int32_t board_slot,
		uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	char board_magazine_str [16] = {0};
        bool is_4leds_supported = _server_working_set->shelves_data_manager->is_4led_supported;
	bool is_new_gea_supported =  _server_working_set->shelves_data_manager->is_new_gea_supported;
	if (const int call_result = acs_apbm_csreader::uint32_to_ip_format(board_magazine_str, board_magazine)) // ERROR: while converting
		return call_result;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Trying to get RED LED status information about board in slot %d of magazine %s from switch board '%s'",
			board_slot, board_magazine_str, switch_board_ip);

	// Prepare the oid arrays
	oid oid_scbrp_red_led [] = { ACS_APBM_SCB_OID_XSHMC_BOARD_RED_LED, board_slot };

	oid oid_scx_red_led [] = { ACS_APBM_SCX_OID_BLADE_RED_LED, board_slot };
	oid oid_scbrp_red_4led [] = { ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_RED_LED, board_slot };

	oid oid_scx_red_4led [] = { ACS_APBM_SCX_OID_BLADECOMMON_RED_LED, board_slot };
/*    if(is_4leds_supported || is_new_gea_supported) {
	    oid_scbrp_red_led[0] = oid_scbrp_red_4led[0];
	    oid_scx_red_led[0] = oid_scx_red_4led[0];

    }*/

	int is_cba_system = _server_working_set->cs_reader->is_cba_system();
	oid * oid_red_led = 0;
        size_t oid_red_led_size = 0;
	acs_apbm_shelvesdatamanager::sdm_constant ret_val = acs_apbm_shelvesdatamanager::SDM_OK;
        uint16_t apboard_fbn;
        if((ret_val = _server_working_set->shelves_data_manager->get_ap_board_fbn(apboard_fbn, board_slot, board_magazine)) != acs_apbm_shelvesdatamanager::SDM_OK)
        {
                ACS_APBM_LOG(LOG_LEVEL_ERROR, "Cannot obtain AP board fbn. Call 'get_ap_board_fbn(..) failed ! call_result == %d", ret_val);
                return ret_val;
        }
	if(is_cba_system)
        {
                ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_cba_system");
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
                {
                        if(is_new_gea_supported)
                        {
				oid_red_led= oid_scx_red_4led;
                                oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_4led);
                        }
                        else
                        {
				oid_red_led = oid_scx_red_led;
                                oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_led);
                        }
                }
                else
                {
                        if(is_4leds_supported)
                        {
				oid_red_led= oid_scx_red_4led;
				oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_4led);
                        }
                        else
                        {
				oid_red_led = oid_scx_red_led;
                                oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_led);
                        }
                }
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_DEBUG,"not is_cba_system");
		if(apboard_fbn == ACS_CS_API_HWC_NS::FBN_GEA)
                {
			if(is_new_gea_supported)
                        {
				oid_red_led = oid_scbrp_red_4led;
                                oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_4led);
                        }
                        else
                        {
				oid_red_led = oid_scbrp_red_led;
                                oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_led);
                        }
                }
                else
                {
                        if(is_4leds_supported)
                        {
				oid_red_led = oid_scbrp_red_4led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_4led);
                        }
                        else
                        {
				oid_red_led = oid_scbrp_red_led;
	                        oid_red_led_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_led);
                        }
                }
        }
	acs_apbm_getboardledsstatuscallback callback(red_led, board_magazine_str, board_slot, switch_board_ip, is_cba_system, axe_eth_shelf::LED_COLOR_RED, apboard_fbn, is_4leds_supported, is_new_gea_supported);

	acs_apbm_snmpsessionhandler session_handler;

	if (const int call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}
	//TR_HU14040 Changes start
	//oid * oid_name = 0;
	//size_t oid_name_size = 0;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
	char oid_str [512];

	// Getting the board RED LED status
	/*is_cba_system ? (oid_name = oid_scx_red_led, oid_name_size = ACS_APBM_ARRAY_SIZE(oid_scx_red_led))
								: (oid_name = oid_scbrp_red_led, oid_name_size = ACS_APBM_ARRAY_SIZE(oid_scbrp_red_led));
	*/

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_red_led, oid_red_led_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sending the PDU GET request to retrieve the board RED LED status information: RED LED oid == '%s'", oid_str);

	call_result = send_get_request(session_handler, oid_red_led, oid_red_led_size, timeout_ms);
	//TR_HU14040 Changes end
	session_handler.close();

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Timeout expired while sending the PDU GET request for RED LED status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request for RED LED status information: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here, if no errors in PDU, the data are already read in the callback object that received the returned pdu from the SNMP agent

	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Callback detected error on reading board RED LED status information from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		if (callback.pdu_error_stat != acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_AUTHORIZATIONERROR &&
				callback.pdu_error_stat != acs_apbm_snmp::ERR_SNMP_NETSNMP_ERR_INCONSISTENTVALUE)
			call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}
	return call_result;
}

int  __CLASS_NAME__::load_snmp_data(bool add_info_to_IMM){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Calling  _server_working_set->shelves_data_manager->load_snmp_data()");

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	int call_result = _server_working_set->shelves_data_manager->load_snmp_data(add_info_to_IMM);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Exiting from  _server_working_set->shelves_data_manager->load_snmp_data()");
	return call_result;
}

int __CLASS_NAME__::check_ipmi_state(int /* scbrpSlotPos*/) {
	return 0;
}

int __CLASS_NAME__::set_scb_link_down_time(int slot, uint32_t magazine, int64_t link_down_time)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	// TODO: Return values must be substituted with specific values
	uint32_t my_mag_num = acs_apbm::BOARD_MAGAZINE_UNDEFINED;
	int call_result = _server_working_set->cs_reader->get_my_magazine(my_mag_num);

	if(call_result)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "failure updating SNMP link down time for SCB identified by: (SLOT == %d, MAG == 0x%08X). Call 'acs_apbm_csreader::get_my_magazine()' failed !. call_result == %d '", slot, magazine, call_result);
		return acs_apbm::ERR_CS_ERROR;
	}

	bool is_my_mag = (my_mag_num == magazine);
	std::vector<acs_apbm_switchboardinfo> * p_scb_infos = (is_my_mag ? & _my_switch_board_infos : & _other_switch_board_infos);
	bool scb_found = false;
	for(std::vector<acs_apbm_switchboardinfo>::iterator it = p_scb_infos->begin(); !scb_found && it != p_scb_infos->end(); ++ it)
	{
		if(it->magazine == magazine && it->slot_position == slot) {
			if ( !it->snmp_link_down_time) {
				it->snmp_link_down_time = link_down_time;
				ACS_APBM_LOG(LOG_LEVEL_INFO, "switch_board_%d in magazine == %u is updated: link down time is == %ld", slot, magazine , link_down_time);
			}
			scb_found = true;
		}
	}

	if(!scb_found)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No SCB found having: (SLOT == %d, MAG == 0x%08X) !", slot, magazine);
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "link down time of switch_board_%d in magazine == %u has been updated", slot, magazine );
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

}

int  __CLASS_NAME__::reset_scb_link_down_time (int slot, uint32_t magazine ){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	// Return values must be substituted with specific values?
	uint32_t my_mag_num = acs_apbm::BOARD_MAGAZINE_UNDEFINED;
	int call_result = _server_working_set->cs_reader->get_my_magazine(my_mag_num);
	if(call_result){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "failure updating SNMP link down time for SCB identified by: (SLOT == %d, MAG == 0x%08X). Call 'acs_apbm_csreader::get_my_magazine()' failed !. call_result == %d '", slot, magazine, call_result);
		return acs_apbm::ERR_CS_ERROR;
	}
	bool is_my_mag = (my_mag_num == magazine);
	std::vector<acs_apbm_switchboardinfo> * p_scb_infos = (is_my_mag ? & _my_switch_board_infos : & _other_switch_board_infos);
	bool scb_found = false;
	for(std::vector<acs_apbm_switchboardinfo>::iterator it = p_scb_infos->begin(); !scb_found && it != p_scb_infos->end(); ++ it)
	{
		if(it->magazine == magazine && it->slot_position == slot)
		{
			if ( it->snmp_link_down_time){
				it->snmp_link_down_time = 0;
				ACS_APBM_LOG(LOG_LEVEL_INFO, "switch_board_%d in magazine == %u is updated: link down time has been reset", slot, magazine);
			}
			scb_found = true;
		}
	}
	if ( !scb_found ) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No SCB found having: (SLOT == %d, MAG == 0x%08X) !", slot, magazine);
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}
	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}


int64_t  __CLASS_NAME__::get_scb_link_down_time (int slot, uint32_t magazine){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	// Return values must be substituted with specific values
	uint32_t my_mag_num = acs_apbm::BOARD_MAGAZINE_UNDEFINED;
	int call_result = _server_working_set->cs_reader->get_my_magazine(my_mag_num);
	if(call_result){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "failure getting SNMP link down time for SCB identified by: (SLOT == %d, MAG == 0x%08X). Call 'acs_apbm_csreader::get_my_magazine()' failed !. call_result == %d '", slot, magazine, call_result);
		return acs_apbm::ERR_CS_ERROR;
	}

	bool is_my_mag = (my_mag_num == magazine);
	std::vector<acs_apbm_switchboardinfo> * p_scb_infos = (is_my_mag ? & _my_switch_board_infos : & _other_switch_board_infos);
	bool scb_found = false;
	int64_t link_down_time = 0;
	for(std::vector<acs_apbm_switchboardinfo>::iterator it = p_scb_infos->begin(); !scb_found && it != p_scb_infos->end(); ++ it)
	{
		if(it->magazine == magazine && it->slot_position == slot)
		{
			// TODO: needed to insert check about idx_scb
			link_down_time = it->snmp_link_down_time;
			scb_found = true;
		}
	}

	if(!scb_found)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, "No SCB found having: (SLOT == %d, MAG == 0x%08X) !", slot, magazine);
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Last link down time of switch_board_%d == %d ", slot, link_down_time);

	return link_down_time;
}

bool __CLASS_NAME__::isEgem2L2Switch(const char * switch_board_ip)
{
	if(!switch_board_ip || !*switch_board_ip)
		return acs_apbm::ERR_NULL_POINTER;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	// search first in own magazine info
	bool switch_found = false;

	for(std::vector<acs_apbm_switchboardinfo>::iterator it = _my_switch_board_infos.begin(); !switch_found && it != _my_switch_board_infos.end(); ++ it)
	{
		if( !strncmp(switch_board_ip, it->ipna_str, ACS_APBM_ARRAY_SIZE(it->ipna_str)) || !strncmp(switch_board_ip, it->ipnb_str, ACS_APBM_ARRAY_SIZE(it->ipnb_str)))
		{
			switch_found = true;
		}
/*********************************************************************/
// for the moment subscription settings will be done based on control and transport trap , so need to check for alias ip

		/*if((!switch_found) && (env == ACS_CS_API_CommonBasedArchitecture::SMX))
		{ //check transport switch IPs
			if( !strncmp(switch_board_ip, it->ipaliasa_str, ACS_APBM_ARRAY_SIZE(it->ipaliasa_str)) || !strncmp(switch_board_ip, it->ipaliasb_str, ACS_APBM_ARRAY_SIZE(it->ipaliasb_str)))
			{
				switch_found = true;
			}
		} */
/**********************************************************************/

	}

	if(!switch_found)
	{
		for(std::vector<acs_apbm_switchboardinfo>::iterator it = _other_switch_board_infos.begin(); !switch_found && it != _other_switch_board_infos.end(); ++ it)
		{
			if( !strncmp(switch_board_ip, it->ipna_str, ACS_APBM_ARRAY_SIZE(it->ipna_str)) || !strncmp(switch_board_ip, it->ipnb_str, ACS_APBM_ARRAY_SIZE(it->ipnb_str)))
			{
				switch_found = true;
			}

/*********************************************************************/
//for the moment subscription settings will be done for both control and transport trap, need to check for alias ip
			/*if((!switch_found) && (env == ACS_CS_API_CommonBasedArchitecture::SMX))
			{
				if( !strncmp(switch_board_ip, it->ipaliasa_str, ACS_APBM_ARRAY_SIZE(it->ipaliasa_str)) || !strncmp(switch_board_ip, it->ipaliasb_str, ACS_APBM_ARRAY_SIZE(it->ipaliasb_str)))
				{
					switch_found = true;
				}
			} */
/*********************************************************************/
		}
	}
	if(!switch_found)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN, " This is not a switch board having IP == '%s'", switch_board_ip);
		return false;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, " switch board found having IP == '%s'", switch_board_ip);
	return true;
}

int __CLASS_NAME__::get_hw_install_status(
		axe_eth_shelf::hw_installation_status_t &,
		int32_t,
		uint32_t,
		unsigned int*)
{
	//TODO Not implemented in SNMP Manager. Only used from IRONSIDE manager
	return acs_apbm::ERR_SNMP_BOARD_NOT_FOUND;
}
int __CLASS_NAME__::configure_front_port_scx_lag (int slot_number,acs_apbm_snmp::lag_flag_type_t command) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	if (_my_switch_board_infos.size() <= 0) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG configure: No switch boards found in my magazine: cannot configure LAG on any switch board");
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG configure: Trying to configure LAG on the SCX on local magazine...");

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if(_my_switch_board_infos[i].slot_position == slot_number){

			const int call_result = configure_front_port_scx_lag (_my_switch_board_infos[i],command);

			if (call_result){
				ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG configure: on SCXB switch board in slot %d failed Call 'acs_apbm_snmpmanager::configure_front_port_scx_lag()' returned %d !",
						_my_switch_board_infos[i].slot_position, call_result);

				if (const int call_result = reschedule_lag_operation(slot_number,command)) {
					ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call reschedule_lag_operation failed: cannot reschedule to retry call_result == %d", call_result);
				} else ACS_APBM_LOG(LOG_LEVEL_ERROR, "configure lag rescheduled successfully for slot:%d, command:%d",slot_number, command);
			}
			else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure: on SCXB switch board in slot %d successful done!", _my_switch_board_infos[i].slot_position);
		}
	}

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}
int __CLASS_NAME__::configure_front_port_scx_lag (acs_apbm_switchboardinfo & switch_board,acs_apbm_snmp::lag_flag_type_t command) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG configure: on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		if(command == acs_apbm_snmp::ENABLE_LAG){
			call_result = configure_front_port_scx_lag(switch_board_ipn_str_addresses[ipn_index]);
		}else if(command == acs_apbm_snmp::DISABLE_LAG){
			call_result = disable_front_port_scx_lag(switch_board_ipn_str_addresses[ipn_index]);
		}
		if (call_result){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"LAG configure: Call 'configure_front_port_scx_lag' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);

			(void)disable_front_port_scx_lag(switch_board_ipn_str_addresses[ipn_index]);
		}
	}
	return call_result;
}
int __CLASS_NAME__::configure_front_port_scx_lag (char * switch_board_ip){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	int call_result = 0;
char oid_str[512];
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_TABLE_ENTRY, 37 }; int oidValue = 5;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	//oid * oid_name = oid_scx_front_port_mib_object;
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure1: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_TABLE_ENTRY_IF_ALIAS, 37 }; char oidValue[] = "po37";
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure2: configure_front_port_scx_lag (oid: %s)(value = %s) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<char*>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_GEN_IF_TYPE, 37 }; int oidValue = 161;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure3: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_GEN_IF_ROW_STATUS, 37 }; int oidValue = 1;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure4: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { IF_ADMIN_STATUS, 37 }; int oidValue = 1;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure5: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_PORT_ACTOR_ADMIN_KEY, 27 }; int oidValue = 37;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure6: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_PORT_MODE_OF_OPERATION, 27 }; int oidValue = 1;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure7: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_PORT_ACTOR_ADMIN_KEY, 28 }; int oidValue = 37;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure8: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_PORT_MODE_OF_OPERATION, 28 }; int oidValue = 1;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure9: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { IF_ADMIN_STATUS, 27 }; int oidValue = 1;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure10: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { IF_ADMIN_STATUS, 28 }; int oidValue = 1;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure11: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_MODULE }; int oidValue = 1;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure12: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_PORT_ACTOR_ADMIN_STATE, 27 }; char oidValue[] = "à";
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure13: configure_front_port_scx_lag (oid: %s)(value = %s) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<char*>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_PORT_ACTOR_ADMIN_STATE, 28 }; char oidValue[] = "à";
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure14: configure_front_port_scx_lag (oid: %s)(value = %s) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<char*>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
if(!call_result){
	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_DIST_ALGORITHM, 37 }; int oidValue = 4;
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure15: configure_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
	call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
}
return call_result;
}
template <class Type>
int __CLASS_NAME__::set_front_port_mib_object (char * switch_board_ip, oid * oid_fprt,size_t oid_fprt_size,Type oidValue){
	acs_apbm_snmpsessioninfo session_info;
	int call_result = 0;

	char oid_str [512];
	session_info.version(SNMP_VERSION_2c);
	session_info.community(const_cast<char *>(get_snmp_community()));

	// snmp session communication handler used to send request and the callback used to receive the ACK response
	acs_apbm_snmpsessionhandler session_handler;
	acs_apbm_snmploggercallback logger_callback("LAG configure: SNMP Response received for LAG");

	ACS_APBM_LOG(LOG_LEVEL_INFO,
			"LAG configure: sending frontPort setting request to switch board IP %s", switch_board_ip);

	session_info.peername(switch_board_ip);
	logger_callback.switch_board_ip(switch_board_ip);

	if ((call_result = session_handler.open(session_info, logger_callback))) { // ERROR: opening the communication session towards the switch board
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"LAG configure: Call 'open' failed: cannot open the communication session towards the above switch board: system error == %d, snmp error == %d, "
				"snmp error text == '%s': call_result == %d",
				session_info.last_sys_error(), session_info.last_snmp_error(), session_info.last_snmp_error_text(), call_result);
		return acs_apbm_snmp::ERR_SNMP_SESSION_OPEN;
	}

	// Prepare the PDU subscription request APBM will send to the current switch board
	netsnmp_pdu * request_pdu = 0;

	if (!(request_pdu = snmp_pdu_create(SNMP_MSG_SET))) { // ERROR: creating a new PDU structure
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"LAG configure: Call 'snmp_pdu_create' failed: memory error (probably out-of-memory) on creating a new PDU structure to carry out the subscription request");
		session_handler.close();
		return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
	}
	// WARNING: NET-SNMP library missed to reset the PDU variable pointer to null
	request_pdu->variables = 0;

	if(typeid(oidValue)== typeid(int)){
		snmp_pdu_add_variable(request_pdu, oid_fprt, oid_fprt_size,ASN_INTEGER, reinterpret_cast<u_char *>(&oidValue), sizeof(oidValue));
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_fprt, oid_fprt_size);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure: Sending FRONTPORT SET for LAG(oid: %s)(value = %d) request...",oid_str, oidValue);
	}
	if(typeid(oidValue)== typeid(char*)){

		snmp_pdu_add_variable(request_pdu, oid_fprt, oid_fprt_size,ASN_OCTET_STR, reinterpret_cast<u_char *>(oidValue),::strlen((const char*)oidValue));
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_fprt, oid_fprt_size);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure: Sending FRONTPORT SET for LAG(oid: %s)(value = %s) request...",oid_str, oidValue);
	}

	unsigned timeout_ms = 2000; // 2 seconds timeout
	errno=0;
	call_result = session_handler.send_synch(request_pdu, &timeout_ms); // 2 sec timeout
	snmp_free_pdu(request_pdu);
	int errno_save=errno;
	if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR,
				"LAG configure: Call 'send_synch' failed: NET-SNMP library internal error on sending the request PDU: call_result == %d", call_result);
		session_handler.close();
		return acs_apbm_snmp::ERR_SNMP_PDU_CREATE;
	}
	session_handler.close();

	return acs_apbm_snmp::ERR_SNMP_NO_ERRORS;
}
int __CLASS_NAME__::disable_front_port_scx_lag (char * switch_board_ip){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	int call_result = 0;
	char oid_str [512];
	if(!call_result){
		oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_MODULE }; int oidValue = 2;
		size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure: disable_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
		call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
	}
	if(!call_result){
		oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_TABLE_ENTRY, 37 }; int oidValue = 6;
		size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);
		oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_scx_front_port_mib_object, oid_fprt_size);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG configure: disable_front_port_scx_lag (oid: %s)(value = %d) request...",oid_str, oidValue);
		call_result = set_front_port_mib_object<int>( switch_board_ip, oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
	}
	return call_result;
}
int __CLASS_NAME__::set_admin_status_front_port_scx (acs_apbm_switchboardinfo & switch_board,int port_number, int oidValue) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	int call_result = -1;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG configure: on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	oid oid_scx_front_port_mib_object [] = { IF_ADMIN_STATUS, port_number };
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {

		call_result = set_front_port_mib_object<int>(switch_board_ipn_str_addresses[ipn_index], oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"LAG configuring: Call 'disable_admin_status_front_port_scx' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}
	return call_result;
}
int __CLASS_NAME__::set_front_port_scx_lag_module (acs_apbm_switchboardinfo & switch_board,int oidValue) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	int call_result = -1;

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG configure: disable LAG on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	oid oid_scx_front_port_mib_object [] = { ACS_APBM_SCX_OID_LAG_MODULE };
	size_t oid_fprt_size = ACS_APBM_ARRAY_SIZE(oid_scx_front_port_mib_object);

	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = set_front_port_mib_object<int>(switch_board_ipn_str_addresses[ipn_index], oid_scx_front_port_mib_object,oid_fprt_size, oidValue);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"LAG configuring: Call 'disable_admin_status_front_port_scx' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}
	return call_result;
}
//////////////////    get port status
int __CLASS_NAME__::get_front_port_operational_status (
		acs_apbm_snmp::frontPort_status_t & port_status,
		int32_t port_number,
		//	uint32_t board_magazine,
		acs_apbm_switchboardinfo & switch_board,
		const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG get info: on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_front_port_operational_status(port_status,port_number,switch_board_ipn_str_addresses[ipn_index],timeout_ms);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"LAG get info: Call 'get_front_port_operational_status' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);

	}

	return call_result;
}

int __CLASS_NAME__::get_front_port_operational_status (
		acs_apbm_snmp::frontPort_status_t & port_status,
		int32_t port_number,
		//uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG get info:Trying to get front port status  port_number: %d of from switch board '%s'",
			port_number, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getfrontportstatuscallback callback(port_status, 0,port_number, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG get info:Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_front_port [] = { IF_OPER_STATUS, port_number };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;

	oid_name = oid_front_port; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_front_port);

	call_result = get_front_port_operational_status(session_handler, callback, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}
int __CLASS_NAME__::get_front_port_operational_status (
		acs_apbm_snmpsessionhandler & session_handler,
		acs_apbm_snmpsessioncallback & callback,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG get info: Sending the PDU GET request to retrieve the board POWER status value into the OID '%s'", oid_str);

	int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG get info: Timeout expired while sending the PDU GET request to retrieve front port status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"LAG get info: Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve front port status: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG get info: Callback detected error on reading front port status from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}
// SCX front partner port
int __CLASS_NAME__::get_front_partner_port_status (
		int & port_status,
		int32_t port_number,
		//	uint32_t board_magazine,
		acs_apbm_switchboardinfo & switch_board,
		const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG get info: on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_front_partner_port_status(port_status,port_number,switch_board_ipn_str_addresses[ipn_index],timeout_ms);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"LAG get info: Call 'get_front_port_operational_status' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);
	}
	return call_result;
}

int __CLASS_NAME__::get_front_partner_port_status (
		int & port_status,
		int32_t port_number,
		//uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG get info: get front port partner status  port_number: %d of from switch board '%s'",
			port_number, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getscxfrontpartnerportstatecallback callback(port_status, 0,port_number, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG get info:Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_front_port [] = { ACS_APBM_SCX_OID_LAG_PARTNER_PORT_ACTOR_STATE, port_number };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;

	oid_name = oid_front_port; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_front_port);

	call_result = get_front_port_operational_status(session_handler, callback, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}
void __CLASS_NAME__::set_lag_operative_state(int slot,int state){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
		switch (slot )
		{
		case acs_apbm::BOARD_SCXB_LEFT:
			operativeStateLeft = (state) ? acs_apbm_snmp::ENABLE_LAG : acs_apbm_snmp::DISABLE_LAG ;
			break;
		case acs_apbm::BOARD_SCXB_RIGHT:
			operativeStateRight = (state) ? acs_apbm_snmp::ENABLE_LAG : acs_apbm_snmp::DISABLE_LAG ;
			break;
		default:
			break;
		}
}
int __CLASS_NAME__::get_lag_operative_state(int slot){
	acs_apbm_snmp::lag_flag_type_t state;
	switch (slot )
	{
	case acs_apbm::BOARD_SCXB_LEFT:
		state = operativeStateLeft;
		break;
	case acs_apbm::BOARD_SCXB_RIGHT:
		state = operativeStateRight;
		break;
	default:
		break;
	}
	return state;
}
void __CLASS_NAME__::set_lag_admin_state(int32_t slot,int state){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	switch (slot )
	{
	case acs_apbm::BOARD_SCXB_LEFT:
		adminStateLeft = (state) ? acs_apbm_snmp::ENABLE_LAG : acs_apbm_snmp::DISABLE_LAG ;
		break;
	case acs_apbm::BOARD_SCXB_RIGHT:
		adminStateRight = (state) ? acs_apbm_snmp::ENABLE_LAG : acs_apbm_snmp::DISABLE_LAG ;
		break;
	default:
		break;
	}
}
int __CLASS_NAME__::get_lag_admin_state(int slot){
	acs_apbm_snmp::lag_flag_type_t state;
	switch (slot )
	{
	case acs_apbm::BOARD_SCXB_LEFT:
		state = adminStateLeft;
		break;
	case acs_apbm::BOARD_SCXB_RIGHT:
		state = adminStateRight;
		break;
	default:
		break;
	}
	return state;
}
int __CLASS_NAME__::get_scx_lag_vitural_port_status (int slot_number,acs_apbm_snmp::frontPort_status_t &status) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	unsigned timeout_ms = 500;
	// Load the switch boards info from configuration
	if (const int call_result = load_switch_boards_info_from_cs()) // ERROR: loading switch board info
		return call_result;

	if (const int call_result = no_my_switch_board_found_check()) return call_result;

	if (_my_switch_board_infos.size() <= 0) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG IMM HANDLING: No switch boards found in my magazine cannot configure LAG on any switch board");
		return acs_apbm::ERR_CS_NO_BOARD_FOUND;
	}
	int call_result = -1;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG IMM HANDLING: Trying to GET SCX virutal port status local magazine...");

	for (size_t i = 0; i < _my_switch_board_infos.size(); ++i) {
		if(_my_switch_board_infos[i].slot_position == slot_number){

			call_result = get_front_port_operational_status(status, SCX_LAG_VIRTUAL_PORT, _my_switch_board_infos[i], &timeout_ms);

			if (call_result)
				ACS_APBM_LOG(LOG_LEVEL_WARN,
						"LAG IMM HANDLING: FAILTED TO get VIRTUAL LAG PORT STATUS from SCX at SLOT= %d: FRONT_PORT= %d: PORT_STATUS= %d: call_result= %d",
						_my_switch_board_infos[i].slot_position, SCX_LAG_VIRTUAL_PORT, status, call_result);
			else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG IMM HANDLING: GETTING VIRTUAL PORT STATUS on SCXB switch board in slot %d successful done!", _my_switch_board_infos[i].slot_position);
		}
	}

	return call_result;
}
int __CLASS_NAME__::reschedule_lag_operation (int slot,acs_apbm_snmp::lag_flag_type_t command){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	int call_result = 0;
	switch (slot )
	{
	case acs_apbm::BOARD_SCXB_LEFT:
		if (command == acs_apbm_snmp::ENABLE_LAG ){
			call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_CONFIGURE_FAILURE_SIDEA,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait);
		}else if (command == acs_apbm_snmp::DISABLE_LAG){
			call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_DISABLE_FAILURE_SIDEA,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait);
		}
		break;
	case acs_apbm::BOARD_SCXB_RIGHT:
		if (command == acs_apbm_snmp::ENABLE_LAG ){
			call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_CONFIGURE_FAILURE_SIDEB,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait);
		}else if (command == acs_apbm_snmp::DISABLE_LAG){
			call_result = _server_working_set->operation_pipe_scheduler->schedule_operation(acs_apbm::SERVER_WORK_OPERATION_SCX_LAG_DISABLE_FAILURE_SIDEB,
					acs_apbm_programconfiguration::operation_pipe_scheduler_default_ticks_to_wait);
		}
		break;
	default:
		break;
	}
	return call_result;
}
// Get front port admin status
int __CLASS_NAME__::get_front_port_admin_status (
		acs_apbm_snmp::frontPort_status_t & port_status,
		int32_t port_number,
		//	uint32_t board_magazine,
		acs_apbm_switchboardinfo & switch_board,
		const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	char * switch_board_ipn_str_addresses [ACS_APBM_CONFIG_IPN_COUNT] = { switch_board.ipna_str, switch_board.ipnb_str };

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG get info: on SCXB having IPNA == %s and IPNB == %s ", switch_board.ipna_str, switch_board.ipnb_str);

	int call_result = -1;
	::srandom(static_cast<unsigned>(::time(0)));
	int ipn_index = ::random() & 0x01;
	for (int i = 0; (call_result && i < 2); ++i, ipn_index = (ipn_index + 1) & 0x01) {
		call_result = get_front_port_operational_status(port_status,port_number,switch_board_ipn_str_addresses[ipn_index],timeout_ms);
		if (call_result)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"LAG get info: Call 'get_front_port_operational_status' failed: switch board slot == %d, switch board IPN = '%s': call_result == %d",
					switch_board.slot_position, switch_board_ipn_str_addresses[ipn_index], call_result);

	}

	return call_result;
}

int __CLASS_NAME__::get_front_port_admin_status (
		acs_apbm_snmp::frontPort_status_t & port_status,
		int32_t port_number,
		//uint32_t board_magazine,
		const char * switch_board_ip,
		const unsigned * timeout_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG get info:Trying to get front port status  port_number: %d of from switch board '%s'",
			port_number, switch_board_ip);

	int is_cba = _server_working_set->cs_reader->is_cba_system();

	acs_apbm_getadminstatefrontportstatuscallback callback(port_status, 0,port_number, switch_board_ip, is_cba);

	acs_apbm_snmpsessionhandler session_handler;

	int call_result = acs_apbm_snmp::ERR_SNMP_NO_ERRORS;

	if ((call_result = open_snmp_session_handler(session_handler, callback, switch_board_ip))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG get info:Call 'open_snmp_session_handler' failed: call_result == %d", call_result);
		return call_result;
	}

	oid oid_front_port [] = { IF_ADMIN_STATUS, port_number };

	const oid * oid_name = 0;
	size_t oid_name_size = 0;

	oid_name = oid_front_port; oid_name_size = ACS_APBM_ARRAY_SIZE(oid_front_port);

	call_result = get_front_port_operational_status(session_handler, callback, oid_name, oid_name_size, timeout_ms);

	session_handler.close();

	return call_result;
}
int __CLASS_NAME__::get_front_port_admin_status (
		acs_apbm_snmpsessionhandler & session_handler,
		acs_apbm_snmpsessioncallback & callback,
		const oid * oid_name,
		size_t oid_name_size,
		const unsigned * timeout_ms) {

	char oid_str [512];

	oid_to_str(oid_str, ACS_APBM_ARRAY_SIZE(oid_str), oid_name, oid_name_size);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LAG get info: Sending the PDU GET request to retrieve the board POWER status value into the OID '%s'", oid_str);

	int call_result = send_get_request(session_handler, oid_name, oid_name_size, timeout_ms);

	if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) { // Timeout expired
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG get info: Timeout expired while sending the PDU GET request to retrieve front port status");
	} else if (call_result) { // ERROR: sending the request
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"LAG get info: Call 'send_get_request' failed: NET-SNMP library internal error on sending the GET request to retrieve front port status: call_result == %d",
				call_result);
	}

	if (call_result) return call_result;

	// Here a possible error in pdu data should be verified
	if (callback.error_code) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG get info: Callback detected error on reading front port status from response PDU: "
				"callback.error_code == %d, callback.pdu_error_stat == %d", callback.error_code, callback.pdu_error_stat);
		call_result = (callback.error_code == acs_apbm_snmp::ERR_SNMP_CALLBACK_ERRORSTAT_INDICATION ? callback.pdu_error_stat : callback.error_code);
	}

	return call_result;
}
void __CLASS_NAME__::manage_APUB_temperature(int32_t magPlugNumber, int32_t slotPos, int temp_status)
{
	uint32_t magazine = 0;
	std::vector<acs_apbm_csboardinfo> boards;

	_server_working_set->cs_reader->map_magazine_plug_number(magazine, magPlugNumber);		//fix for TR HW80638
	if (magazine == acs_apbm::BOARD_MAGAZINE_UNDEFINED){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "failed to get magazine == 0x%08X\n", magazine);
	}
	else
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MAGAZINE == 0x%08X\n", magazine);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MAGAZINE == %ld slot == %ld ", magPlugNumber, slotPos);
		_server_working_set->cs_reader->get_apbm_boards(boards,magazine);
		if (boards.size() == 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "failed in magazine 0x%08X ==>> apbm_boards.size == %zu\n", magazine, boards.size());
		}
		else
		{
			for (size_t i = 0; i < boards.size(); ++i)
			{
				if ( slotPos == boards[i].slot_position)
				{
					if ( boards[i].fbn == ACS_CS_API_HWC_NS::FBN_APUB)
					{
						int severity_A1 = 10, severity_A2 = 20;
						if(temp_status == 0)
						{
							ACS_APBM_LOG(LOG_LEVEL_DEBUG,"sensor_data value is 0");
							_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slotPos, severity_A1);
							_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slotPos, severity_A2);
						}
						else if(temp_status == 2)
						{
							ACS_APBM_LOG(LOG_LEVEL_DEBUG, "sensor_data value is 7 or 2 ");
							_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slotPos, severity_A1);
							_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slotPos, severity_A2);
						}
						else
						{
							ACS_APBM_LOG(LOG_LEVEL_DEBUG, "sensor_data value is  9||b||4||other ");
							_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slotPos, severity_A2);
							_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_APUB_TEMPERATURE, slotPos, severity_A1);
						}
						break;
					}
				}
			}
		}
	}
}
