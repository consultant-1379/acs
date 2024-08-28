#ifndef HEADER_GUARD_CLASS__acs_apbm_api
#define HEADER_GUARD_CLASS__acs_apbm_api acs_apbm_api

/** @file acs_apbm_api.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-06-27
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
 *	| R-001 | 2011-06-27 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+


 */

#include <string>
#include <vector>
#include <list>

#include "acs_apbm_types.h"
#include "acs_apbm_trapmessage.h"
#include "acs_apbm_macros.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_api

//Forward declaration
class acs_apbm_api_imp;

/** @class acs_apbm_api acs_apbm_api.h
 *	@brief acs_apbm_api class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-06-27
 *
 *	acs_apbm_api Defines the methods that must be used to interact with APBM server
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_api Default constructor
	 */
	__CLASS_NAME__ ();

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_api Destructor
	 */
	virtual ~__CLASS_NAME__ ();

	//===========//
	// Functions //
	//===========//
public:
	/** @brief Subscribes the caller application to trap message notification from APBM server
	 *
	 *	<put description>
	 *
	 *	@param[in] bitmap Description
	 *	@param[out] trap_handle Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int subscribe_trap (int bitmap, acs_apbm::trap_handle_t & trap_handle);

	int unsubscribe_trap (acs_apbm::trap_handle_t trap_handle);

	//## Operation: GetTrap%45408E12035B
	//	------
	//	Provides user with requested trap information
	//	------
	//	In - trapHandle is the handle for requested trap
	//	In/Out - value is a pointer on a string array to receive
	//	the information. Should be created by the user
	//	Out - returns number of elements in array value or -1 if
	//	there was an error
	//	------
	int get_trap (acs_apbm::trap_handle_t trap_handle, acs_apbm_trapmessage & trap_message);

	//## Operation: GetBoardInfo%45408E1203C8
	//	------
	//	Provides user with information about specified board
	//	------
	//	In - slotNr is the slot number for the board
	//	In/Out - BoardInfo is a pointer on a string array to
	//	receive the information. Should be created by the user
	//	Out - returns number of elements in array BoardInfo or
	//	-1 if there was an error
	//	------
//	int get_board_info (int slot_number, std::vector<std::string> & board_info);
//
//	int get_SCB_RP_info (std::vector<std::string> & SCB_RP_info);
//
//	int get_SCXB_info (std::vector<std::string> & SCXB_info);
//
//	int set_board_info (int slot_number, const char * OID, const char * value);
//	inline int set_board_info (int slot_number, const std::string & OID, const std::string & value) {
//		return set_board_info(slot_number, OID.c_str(), value.c_str());
//	}
//
//	int send_BMDS_data_req (int slot_number);
//
//	int set_MIA_led (int slot_number, bool led_on = false);
//
//	int set_red_led (int slot_number, bool led_on = false);

	int get_own_slot ();

//	int set_watchdog (bool own_node = true, bool enable = false, int timeout = -1);
//
//	int reset_board (int slot_number);	// 1..24
//
//	int safe_reset_board (int slot_number, const char * ip_scb_0, const char * ip_scb_25);	// 1..24
//	inline int safe_reset_board (int slot_number, const std::string & ip_scb_0, const std::string & ip_scb_25) { // 1..24
//		return safe_reset_board(slot_number, ip_scb_0.c_str(), ip_scb_25.c_str());
//	}
//
//	int get_FW_inventory (int slot_number, std::vector<std::string> & inventory);
//
//	virtual int init (bool own_node = true);

	int get_switch_board_info(const acs_apbm::architecture_type_t architecture, uint32_t magazine, int32_t & slot, char (& ipna_str) [16], char (& ipnb_str) [16], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn);


	// methods from HWM_API

	/** @brief Check if a board is present
	 *
	 *	Check if a specified board is available in the AP HW configuration.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *		BOARD_NAME_DVD: External media board
	 *		BOARD_NAME_GEA: Alarm board
	 *	@return The function returns one of the following interger codes
	 *	acs_apbm::BOARD_PRESENCE_BOARD_PRESENT			Yes, the board is available in the current configuration.
	 *	acs_apbm::BOARD_PRESENCE_BOARD_NOT_CONFIGURED	No, the board is not available in the current configuration.
	 *	acs_apbm::BOARD_PRESENCE_BOARD_NOT_AVAILABLE	Pending, the board is part of the HW confiuration but is not
	 *													available at the moment. E.g. it can be faulty or beeing
	 *													repaired and can not be used.
	 *	acs_apbm::BOARD_PRESENCE_ERROR					Error
	 *	@remarks Remarks
	 */
	int check_board_presence (const acs_apbm::board_name_t board_name);

	/** Check if a specified board is connected to an other AP in the system.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *		acs_apbm::BOARD_NAME_DVD: External media board
	 *		acs_apbm::BOARD_NAME_GEA: Alarm board
	 *	@param[out] ap_ids
	 *		Array of AP numbers for APs where the board is connected: 1 for AP1, 2 for AP2, etc.
	 *	@return The function returns one of the following interger codes
	 *		 0	OK.
	 *		-1	Error
	 *	@remarks Remarks
	 */
	int get_board_location (const acs_apbm::board_name_t board_name, std::list<int> & ap_ids);

	/** Get the status for an optional board.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *		acs_apbm::BOARD_NAME_DVD: External media board
	 *		acs_apbm::BOARD_NAME_GEA: Alarm board
	 *	@return The function returns one of the following interger codes
	 *		 0	Working, the board is working OK.
	 *		 1	Faulty, the board is faulty.
	 *		 2	Blocked, the board is beeing blocked and is beeing repaired.
	 *		 3	Missing, the board is temporarely missing.
	 *		-1	Error
	 *	@remarks Remarks
	 */
	int get_board_status (const acs_apbm::board_name_t board_name);

	/** Set the status for an optional board.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *		BOARD_NAME_DVD: External media board
	 *		BOARD_NAME_GEA: Alarm board
	 *	@param[in] status
	 *		Specify the status to be set. Valid values are:
	 *			 0	OK
	 *			-1	Faulty
	 *	@return The function returns one of the following interger codes
	 *		 0	OK
	 *		-1	Error
	 *	@remarks Remarks
	 */
	int set_board_status (const acs_apbm::board_name_t board_name, const acs_apbm::reported_board_status_t status);

	int ipmifw_upgrade(uint32_t magazine, int32_t  slot, const char ipmipkg_name [ACS_APBM_IPMIPKG_NAME_MAX_SIZE], uint8_t comport);
	inline int ipmifw_upgrade(uint32_t magazine, int32_t slot, const char ipmipkg_name [ACS_APBM_IPMIPKG_NAME_MAX_SIZE]){
		return ipmifw_upgrade (magazine, slot, ipmipkg_name, acs_apbm::NO_COM);
	}

	int get_ipmifw_status(uint32_t magazine, int32_t  slot, uint8_t ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE] )__attribute__((deprecated));

	int get_ipmi_firmware_status(uint32_t magazine, int32_t  slot, uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE] );

	int get_ipmifw_data(uint32_t magazine, int32_t  slot, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER] ,  char (& revision) [ACS_APBM_IPMI_REVISION]);
	
	bool is_gep4_gep5();
	
	bool is_gep5_gep7();

	bool is_hwtype_gep7();

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	acs_apbm_api_imp * _api_imp;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_api
