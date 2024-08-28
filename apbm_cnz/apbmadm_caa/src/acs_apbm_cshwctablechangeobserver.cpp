/** @file acs_apbm_cshwctablechangeobserver.cpp
 *      @brief
 *      @author xnicmut (Nicola Muto)
 *      @date 2012-02-08
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
 *      | R-001 | 2012-02-08 | xnicmut      | Creation and first revision.        |
 *      +=======+============+==============+=====================================+
 *      |       | 2014-06-17 | xsunach      | TR HS54933                          |
 *      +=======+============+==============+=====================================+
 */

#include <arpa/inet.h>

#include <ace/Guard_T.h>

#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_hwihandler.h"

#include "acs_apbm_cshwctablechangeobserver.h"

void __CLASS_NAME__::update (const ACS_CS_API_HWCTableChange & observee) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CS HWC table change notification received: notification items count == %d", observee.dataSize);

	for (int i = 0; i < observee.dataSize; ++i) {
		const ACS_CS_API_HWCTableData_R1 & hwc_item = observee.hwcData[i];

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "  HWC table item #%02d: "
				"Operation type == %d, magazine == 0x%08X, slot == %u, sysId == %u, sysType == %u, sysNo == %u, fbn == %u, "
				"side == %u, ipEthA == 0x%08X, ipEthB == 0x%08X",
				i, hwc_item.operationType, hwc_item.magazine, hwc_item.slot, hwc_item.sysId, hwc_item.sysType, hwc_item.sysNo,
				hwc_item.fbn, hwc_item.side, htonl(hwc_item.ipEthA), htonl(hwc_item.ipEthB));
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Resetting CS data into the cs_reader...");
	_server_working_set->cs_reader->reset_info();
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "CS data info into the cs_reader cleared successfully");

	int loading_board_info_todo = 1;
	int isCBA = _server_working_set->cs_reader->is_cba_system();

	for (int i = 0; i < observee.dataSize; ++i) {
		const ACS_CS_API_HWCTableData_R1 & hwc_item = observee.hwcData[i];
		ACS_CS_API_TableChangeOperation::OpType op_type = hwc_item.operationType;
		const uint32_t magazine = htonl(hwc_item.magazine);
		const uint16_t slot = hwc_item.slot;
		char magazine_str [16] = {0};
		const uint16_t fbn = hwc_item.fbn;

		acs_apbm_csreader::uint32_to_ip_format(magazine_str, magazine);
		// Is this board a switch board?
		if ((fbn == acs_apbm_csreader::SWITCH_BOARD_FBN_SCBRP) || (fbn == acs_apbm_csreader::SWITCH_BOARD_FBN_SCXB) 
			|| (fbn == acs_apbm_csreader::SWITCH_BOARD_FBN_SMXB) ) { // YES

			acs_apbm_switchboardinfo switch_board;
			switch_board.magazine = magazine;
			switch_board.slot_position = slot;
			acs_apbm_csreader::uint32_to_ip_format(switch_board.ipna_str, htonl(hwc_item.ipEthA));
			acs_apbm_csreader::uint32_to_ip_format(switch_board.ipnb_str, htonl(hwc_item.ipEthB));

			if (op_type == ACS_CS_API_TableChangeOperation::Delete) { // Deleting a switch board
				ACS_APBM_LOG(LOG_LEVEL_INFO,
						"Deleting this switch board from SNMP manager internal structures: magazine == 0x%08X, slot == %d, IPNA == '%s', IPNB == '%s'",
						magazine, slot, switch_board.ipna_str, switch_board.ipnb_str);

				if (const int call_result = _server_working_set->snmp_manager->switch_board_delete(magazine, slot)) // ERROR: Deleting the switch board
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'switch_board_delete' failed: switch board not deleted correctly: call_result == %d", call_result);
				else ACS_APBM_LOG(LOG_LEVEL_INFO, "Switch board successfully deleted");
			} else if (op_type == ACS_CS_API_TableChangeOperation::Add) { // Adding a new switch board
				ACS_APBM_LOG(LOG_LEVEL_INFO,
						"Adding this new switch board to the SNMP manager internal structures: magazine == 0x%08X, slot == %d, IPNA == '%s', IPNB == '%s'",
						magazine, slot, switch_board.ipna_str, switch_board.ipnb_str);

				if (const int call_result = _server_working_set->snmp_manager->switch_board_add(switch_board)) // ERROR: Adding the switch board
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'switch_board_add' failed: switch board not added correctly: call_result == %d", call_result);
				else ACS_APBM_LOG(LOG_LEVEL_INFO, "Switch board successfully added");
			} else { // Changing a switch board
				ACS_APBM_LOG(LOG_LEVEL_INFO,
						"Changing this switch board into the SNMP manager internal structures: magazine == 0x%08X, slot == %d, IPNA == '%s', IPNB == '%s'",
						magazine, slot, switch_board.ipna_str, switch_board.ipnb_str);

				if (const int call_result = _server_working_set->snmp_manager->switch_board_change(switch_board)) // ERROR: Changing the switch board
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'switch_board_change' failed: switch board not changed correctly: call_result == %d", call_result);
				else ACS_APBM_LOG(LOG_LEVEL_INFO, "Switch board successfully changed");
			}
		} else {
			//INSERT HERE OTHER CUSTOMIZED OPERATION //

			//HS22787: Moved raid status reset and alarm ceasing outside if(loading_board_info_todo)

			//DISK ONLY
			if(fbn == ACS_CS_API_HWC_NS::FBN_Disk){
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Resetting RAID status.");
				_server_working_set->monitoring_service_event_handler->set_raid_status(axe_eth_shelf::RAID_STATUS_UNDEFINED);
			}

			if (op_type == ACS_CS_API_TableChangeOperation::Delete) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Deleting board from configuration! Removing Board Info!");
				_server_working_set->shelves_data_manager->remove_ap_board_info(false, slot, magazine);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Deleting board from configuration! Ceasing possible alarm.");
				_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_BOARDREM, slot);
			}

			if (loading_board_info_todo) //an update in IMM should be ordered?
			{
				// YES: Reload board info into internal data structure and in IMM also only if the board is not a switch board
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ordering a CS_LOAD_BOARD_DATA_IMM operation to load CS board data into internal structures and into IMM");
				// Order a 'load board data from CS with IMM' operation to load and create boards in memory and IMM
				if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_CS_LOAD_BOARD_DATA_IMM))
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'operation_send' failed: cannot order a CS_LOAD_BOARD_DATA_IMM operation to load CS board data into internal structures and into IMM");

				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ordering an SNMP_LOAD_BOARD_DATA_IMM to load SNMP info into internal structures and into IMM");
				if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_LOAD_BOARD_DATA_IMM))
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'operation_send' failed: cannot order a SNMP_LOAD_BOARD_DATA_IMM operation to load SNMP info into internal structures and into IMM");

				loading_board_info_todo = 0;
			}
		}

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "HW Inventory: HWC table notification from CS: op_type == '%s', board_magazine == '%s', board_slot == %u",
				hwc_table_change_op_type_str(op_type), magazine_str, slot);

		if(isCBA){
			// Updating inventory information
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updating hardware inventory data...");
			int (ACS_APBM_HWIHandler::* hwi_store) (const std::string &, const int &) = 0;
			int (ACS_APBM_HWIHandler::* hwi_remove) (const std::string &, const int &) = 0;

			switch (op_type) {
			case ACS_CS_API_TableChangeOperation::Change: //hwi_remove = &ACS_APBM_HWIHandler::removeBoard;
				break;
			case ACS_CS_API_TableChangeOperation::Add: hwi_store = &ACS_APBM_HWIHandler::storeBoard;
				break;
			case ACS_CS_API_TableChangeOperation::Delete: //hwi_remove = &ACS_APBM_HWIHandler::removeBoard;
				break;
			default:
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Hwc table change notification skipped: operation type has an unknown value");
				continue;
				break;
			}


			if ((fbn == acs_apbm_csreader::SWITCH_BOARD_FBN_SCBRP) || (fbn == acs_apbm_csreader::SWITCH_BOARD_FBN_SCXB)
				|| (fbn == acs_apbm_csreader::SWITCH_BOARD_FBN_SMXB)) {
//				_server_working_set->hwi_handler->storeAllData();

				//should be send a new "operation" to reschedule if fails this

				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ordering an SNMP_HWI_RELOAD_DATA to reload inventory data");
				if (_server_working_set->operation_send(acs_apbm::SERVER_WORK_OPERATION_SNMP_HWI_RELOAD_DATA))
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"Call 'operation_send' failed: cannot order a SNMP_HWI_RELOAD_DATA operation to reload hardware inventory data");
			continue;
			}

			if (hwi_remove) { // Deleting the board from internal structures
				const int hwi_call_result = (_server_working_set->hwi_handler->*hwi_remove)(magazine_str, slot);
				if (hwi_call_result) { // ERROR: removing the board from the hardware inventory
					hwi_store = 0; // Canceling the store operation

					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"HWI Update: Removing board from hardware inventory failed: board_magazine == '%s', "
							"board_slot == %u: hwi_call_result == %d", magazine_str, slot, hwi_call_result);
				}
			}

			if (hwi_remove) { // Deleting the board from IMM
				const int hwi_call_result = _server_working_set->hwi_handler->removeBoardFromIMM(magazine_str, slot);
				if (hwi_call_result) { // ERROR: removing the board from the IMM
					hwi_store = 0; // Canceling the store operation

					ACS_APBM_LOG(LOG_LEVEL_ERROR, "HWI Update: call 'RemoveBoardFromImm' failed: board_magazine == '%s', "
							"board_slot == %u: hwi_call_result == %d", magazine_str, slot, hwi_call_result);
				}
			}

			if (hwi_store) { // Proparing to store the board
				const int hwi_call_result = _server_working_set->hwi_handler->startCollectionOnBoard(magazine_str, slot);
				if (hwi_call_result) { // ERROR: getting information on board from SNMP agent
					hwi_store = 0; // Canceling the store operation

					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"HWI Update: call 'startCollectionOnBoard' failed: magazine == 0x%08X, "
							"magazine_str == '%s', board_slot == %u: hwi_call_result == %d",
							magazine, magazine_str, slot, hwi_call_result);
				}
			}

			if (hwi_store) { // Storing the board
				const int hwi_call_result = (_server_working_set->hwi_handler->*hwi_store)(magazine_str, slot);
				if (hwi_call_result) // ERROR: storing the board into the hardware inventory
					ACS_APBM_LOG(LOG_LEVEL_ERROR,
							"HWI Update: Storing board into hardware inventory failed: board_magazine == '%s', "
							"board_slot == %u: hwi_call_result == %d", magazine_str, slot, hwi_call_result);
			}
		}
	}
}

int __CLASS_NAME__::start (ACS_CS_API_SubscriptionMgr * subscription_manager) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (_state == OBSERVER_STATE_STARTED) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Observer " ACS_APBM_STRINGIZE(__CLASS_NAME__) " already started");
		return acs_apbm::ERR_NO_ERRORS;
	}

	// Getting the subscription manager if not provided into the subscription_manager parameter
	subscription_manager || (subscription_manager = ACS_CS_API_SubscriptionMgr::getInstance());

	if (!subscription_manager) { // ERROR: getting the subscription manager from CS API
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getInstance' failed: cannot get the subscription manager from the CS-API");
		return acs_apbm::ERR_CS_GET_INSTANCE;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Subscribing APBM server to the CS HWC table change notification interface...");

	ACS_CS_API_NS::CS_API_Result cs_call_result = subscription_manager->subscribeHWCTableChanges(*this);

	if (cs_call_result != ACS_CS_API_NS::Result_Success) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'subscribeHWCTableChanges' failed: cs_call_result == %d", cs_call_result);
		return acs_apbm::ERR_CS_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully subscribed to the CS HWC table change notification interface");

	_state = OBSERVER_STATE_STARTED;

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::stop (ACS_CS_API_SubscriptionMgr * subscription_manager) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (_state == OBSERVER_STATE_STOPPED) return acs_apbm::ERR_NO_ERRORS;

	// Getting the subscription manager if not provided into the subscription_manager parameter
	subscription_manager || (subscription_manager = ACS_CS_API_SubscriptionMgr::getInstance());

	if (!subscription_manager) { // ERROR: getting the subscription manager from CS API
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getInstance' failed: cannot get the subscription manager from the CS-API");
		return acs_apbm::ERR_CS_GET_INSTANCE;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Unsubscribing APBM server from the CS HWC table change notification interface...");

	ACS_CS_API_NS::CS_API_Result cs_call_result = subscription_manager->unsubscribeHWCTableChanges(*this);

	if (cs_call_result != ACS_CS_API_NS::Result_Success) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'unsubscribeHWCTableChanges' failed: cs_call_result == %d", cs_call_result);
		return acs_apbm::ERR_CS_ERROR;
	}

	ACS_APBM_LOG(LOG_LEVEL_INFO, "APBM server successfully unsubscribed from the CS HWC table change notification interface");

	_state = OBSERVER_STATE_STOPPED;

	return acs_apbm::ERR_NO_ERRORS;
}

const char * __CLASS_NAME__::hwc_table_change_op_type_str (ACS_CS_API_TableChangeOperation::OpType op_type) const {
	switch (op_type) {
	case ACS_CS_API_TableChangeOperation::Add: return "ADD";
	case ACS_CS_API_TableChangeOperation::Change: return "CHANGE";
	case ACS_CS_API_TableChangeOperation::Delete: return "DELETE";
	case ACS_CS_API_TableChangeOperation::Unspecified: return "UNSPECIFIED";
	}

	return "UNKNOWN";
}
