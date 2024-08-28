#ifndef HEADER_GUARD_CLASS__acs_apbm_snmpmanager
#define HEADER_GUARD_CLASS__acs_apbm_snmpmanager acs_apbm_snmpmanager

/** @file acs_apbm_snmpmanager.h
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
 *  |       | 2014-06-17 | xsunach      | TR HS54933                          |
 *  +=======+============+==============+=====================================+
 *  |       | 2015-02-12 | xsunach      | CNI 33/6-441                                  |
 *  +=======+============+==============+=====================================+
 */

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include <ace/Recursive_Thread_Mutex.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/transform_oids.h> // To get using SNMP Version 3

#include "acs_apbm_primitivedatahandler.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_snmpconstants.h"
#include "acs_apbm_switchboardinfo.h"
#include <acs_apbm_snmpsessioncallback.h>
#include <acs_apbm_snmpmanagercallbacks.h>
#include "acs_apbm_snmpmibdefinitions.h"
#include <ace/Event.h>

using namespace std;

namespace axe_eth_shelf = acs_apbm_snmp::axe::eth::shelf;
namespace generic_shelf = acs_apbm_snmp::generic::shelf;


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;
class acs_apbm_snmpsessionhandler;


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_snmpmanager


/** @class acs_apbm_snmpmanager acs_apbm_snmpmanager.h
 *	@brief acs_apbm_snmpmanager class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-09-20
 *
 *	acs_apbm_snmpmanager <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_snmpmanager constructor
	 */
	inline __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	:  _sync_object(), _my_switch_board_infos(), _other_switch_board_infos(), _my_active_switch_board_index(-1),
	   _server_working_set(server_working_set), _shelf_manager_control_left_scb_trap_counter_(0), _shelf_manager_control_right_scb_trap_counter_(0) {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~__CLASS_NAME__ () {}

public:
	inline int SWITCH_BOARD_STATE_ACTIVE (bool isCBA) const {
		if (isCBA)
			return generic_shelf::SHELF_MGR_CTRL_STATE_ACTIVE;
		else  return axe_eth_shelf::SHELF_MGR_STATE_ACTIVE;}

	//===========//
	// Functions //
	//===========//
public:
	static inline void initialize (const char * type) { init_snmp(type); }
	static inline void shutdown (const char * type) { snmp_shutdown(type); }

	const char * get_snmp_community () const;

	int check_ipmi_state(int scbrpSlotPos);
	virtual int send_trap_subscription_request ();
	virtual int send_trap_subscription_request (uint32_t magazine);
	virtual int send_trap_unsubscription_request ();
	virtual int send_trap_unsubscription_request (int switch_board_slot_position);
	virtual int send_trap_unsubscription_request (uint32_t magazine);

	int send_trap_ack (int switch_board_slot_position, const char * ap_ip, const char * switch_board_ip);

	int set_shelf_mngr_ctrl_state_scbrp (
			int switch_board_slot_position,
			const char * switch_board_ip,
			acs_apbm_snmp::shelf_manager_controller_state_t state);

	int get_board_ipmi_data (
			int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	virtual int get_board_ipmi_data (
			int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_data (
			int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_data (
			acs_apbm_snmpsessionhandler & session_handler,
			acs_apbm_snmpsessioncallback & callback,
			const oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_upgrade_status (
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	virtual int get_board_ipmi_upgrade_status (
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_upgrade_status (
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_upgrade_status (
			acs_apbm_snmpsessionhandler & session_handler,
			acs_apbm_snmpsessioncallback & callback,
			const oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_data (
			int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status, // xshmcBoardIPMIUpgradeStatus
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_data (
			int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status, // xshmcBoardIPMIUpgradeStatus
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int get_board_ipmi_data (
			int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status, // xshmcBoardIPMIUpgradeStatus
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int set_board_ipmi_upgrade (
			const char * upgrade_path,
			uint32_t upgrade_ip4,
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	int set_board_ipmi_upgrade (
			const char * upgrade_path,
			uint32_t upgrade_ip4,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int set_board_ipmi_upgrade (
			const char * upgrade_path,
			uint32_t upgrade_ip4,
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int set_board_ipmi_upgrade (
			const char * upgrade_path,
			const char * upgrade_ip4,
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	virtual int set_board_ipmi_upgrade (
			const char * upgrade_path,
			const char * upgrade_ip4,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int set_board_ipmi_upgrade (
			const char * upgrade_path,
			const char * upgrade_ip4,
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int set_board_ipmi_upgrade (
			const char * ipmi_upgrade_value,
			acs_apbm_snmpsessionhandler & session_handler,
			const oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0);

	int get_board_data (
			char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
			char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
			char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
			char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
			char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
			char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	virtual int get_board_data (
			char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
			char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
			char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
			char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
			char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
			char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int get_board_data (
			char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
			char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
			char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
			char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
			char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
			char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_data_scbrp (
			char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
			char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
			char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
			char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
			char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
			char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
			int32_t board_slot,
			const char * board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_data_scx (
			char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
			char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
			char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
			char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
			char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
			char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
			int32_t board_slot,
			const char * board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_leds_status (
			axe_eth_shelf::led_status_t & green_led,
			axe_eth_shelf::led_status_t & red_led,
			axe_eth_shelf::led_status_t & mia_led,
			int32_t board_slot,
			const unsigned * timeout_ms = 0); // Questa funzione deve calcolare solo gli ip delle switch board da contattare e chiamare le successive

	virtual int get_board_leds_status (
			axe_eth_shelf::led_status_t & green_led,
			axe_eth_shelf::led_status_t & red_led,
			axe_eth_shelf::led_status_t & mia_led,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0); // Questa funzione deve calcolare solo gli ip delle switch board da contattare e chiamare le successive

        int get_board_leds_status (
			axe_eth_shelf::led_status_t & green_led,
			axe_eth_shelf::led_status_t & red_led,
			axe_eth_shelf::led_status_t & mia_led,
			axe_eth_shelf::led_status_t & blue_led,
			int32_t board_slot,
			const unsigned * timeout_ms = 0); //4Leds compatibility


	virtual int get_board_leds_status (
			axe_eth_shelf::led_status_t & green_led,
			axe_eth_shelf::led_status_t & red_led,
			axe_eth_shelf::led_status_t & mia_led,
			axe_eth_shelf::led_status_t & blue_led,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0); //4Leds compatibility

	int get_board_green_red_leds_status (
			axe_eth_shelf::led_status_t & green_led,
			axe_eth_shelf::led_status_t & red_led,
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_mia_led_status (
			axe_eth_shelf::led_status_t & mia_led,
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_red_led_status (
				axe_eth_shelf::led_status_t & red_led,
				int32_t board_slot,
				uint32_t board_magazine,
				const char * switch_board_ip,
				const unsigned * timeout_ms = 0);
  
        //4LEDs compatibility
	int get_board_blue_led_status (
     			        axe_eth_shelf::led_status_t & blue_led,
				int32_t board_slot,
				uint32_t board_magazine,
				const char * switch_board_ip,
				const unsigned * timeout_ms = 0);

	int get_board_bios_run_mode (
			axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	virtual int get_board_bios_run_mode (
			axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int get_board_bios_run_mode (
			axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_bios_run_mode (
			acs_apbm_snmpsessionhandler & session_handler,
			acs_apbm_snmpsessioncallback & callback,
			const oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0);

	int get_board_power_status (
			acs_apbm_snmp::bladePwr_status_t & power_status,
			int32_t board_slot,
			const unsigned * timeout_ms = 0);

	virtual int get_board_power_status (
			acs_apbm_snmp::bladePwr_status_t & power_status,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	int get_board_power_status (
			acs_apbm_snmp::bladePwr_status_t & power_status,
			int32_t board_slot,
			uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_board_power_status (
			acs_apbm_snmpsessionhandler & session_handler,
			acs_apbm_snmpsessioncallback & callback,
			const oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0);

	// To Get front port status declaration
	int get_front_port_operational_status (
			acs_apbm_snmp::frontPort_status_t & port_status,
			int32_t port_number,
			acs_apbm_switchboardinfo & switch_board,
			const unsigned * timeout_ms = 0);

	int get_front_port_operational_status (
			acs_apbm_snmp::frontPort_status_t & port_status,
			int32_t port_number,
			const char * switch_board_ip,
			const unsigned * timeout_ms = 0);

	int get_front_port_operational_status (
			acs_apbm_snmpsessionhandler & session_handler,
			acs_apbm_snmpsessioncallback & callback,
			const oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0);
	// To Get admin state of front port
		int get_front_port_admin_status (
				acs_apbm_snmp::frontPort_status_t & port_status,
				int32_t port_number,
				acs_apbm_switchboardinfo & switch_board,
				const unsigned * timeout_ms = 0);

		int get_front_port_admin_status (
				acs_apbm_snmp::frontPort_status_t & port_status,
				int32_t port_number,
				const char * switch_board_ip,
				const unsigned * timeout_ms = 0);

		int get_front_port_admin_status (
				acs_apbm_snmpsessionhandler & session_handler,
				acs_apbm_snmpsessioncallback & callback,
				const oid * oid_name,
				size_t oid_name_size,
				const unsigned * timeout_ms = 0);

	int get_front_partner_port_status (
			int & port_status,
			int32_t port_number,
			//	uint32_t board_magazine,
			acs_apbm_switchboardinfo & switch_board,
			const unsigned * timeout_ms);
	int get_front_partner_port_status (
			int & port_status,
			int32_t port_number,
			//uint32_t board_magazine,
			const char * switch_board_ip,
			const unsigned * timeout_ms);

	// To Get front port status

        /*
	* get_blade_led_type function is used to query the mib for blade_led_type.
	* This info is useful to know whether the HW supports 4 leds or not. 
	*/

        int get_blade_led_type ( 
                        acs_apbm_snmp::blade_led_type_t & blade_led_type,
                        int32_t board_slot,
                        const unsigned * timeout_ms = 0);

        virtual int get_blade_led_type (
                        acs_apbm_snmp::blade_led_type_t & blade_led_type,
                        int32_t board_slot,
                        uint32_t board_magazine,
                        const unsigned * timeout_ms = 0);

        int get_blade_led_type (
                        acs_apbm_snmp::blade_led_type_t & blade_led_type,
                        int32_t board_slot,
                        uint32_t board_magazine,
                        const char * switch_board_ip,
                        const unsigned * timeout_ms = 0);

        int get_blade_led_type (
                        acs_apbm_snmpsessionhandler & session_handler,
                        acs_apbm_snmpsessioncallback & callback,
                        const oid * oid_name,
                        size_t oid_name_size,
                        const unsigned * timeout_ms = 0);

        int get_board_fatal_event_log (

            char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
                        int32_t board_slot,
                        const unsigned * timeout_ms = 0);

        virtual int get_board_fatal_event_log (
            char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
                        int32_t board_slot,
                        uint32_t board_magazine,
                        const unsigned * timeout_ms = 0);

        int get_board_fatal_event_log (
            char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
                        int32_t board_slot,
                        uint32_t board_magazine,
                        const char * switch_board_ip,
                        const unsigned * timeout_ms = 0);

        int get_board_fatal_event_log (
                        acs_apbm_snmpsessionhandler & session_handler,
                        acs_apbm_snmpsessioncallback & callback,
                        const oid * oid_name,
                        size_t oid_name_size,
                        const unsigned * timeout_ms = 0);

        virtual int get_hw_install_status(
        		axe_eth_shelf::hw_installation_status_t &status,
        		int32_t board_slot,
        		uint32_t board_magazine,
        		unsigned int* timeout
        );

    int set_shelf_neighbour_presence();
    int get_shelf_neighbour_presence (acs_apbm_snmp::switch_board_neighbour_state_t & neighbour_presence, acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms = 0);
    int get_shelf_neighbour_presence (acs_apbm_snmp::switch_board_neighbour_state_t & neighbour_presence, int scbrpSlotPos, uint32_t magazine);
    int get_shelf_neighbour_presence (
    		acs_apbm_snmp::switch_board_neighbour_state_t & neighbour_presence,
    		int32_t board_slot,
    		uint32_t board_magazine,
    		const char * switch_board_ip,
    		const unsigned * timeout_ms);

    virtual int get_managerTrapSubscrIp(acs_apbm_switchboardinfo & switch_board,  const unsigned * timeout_ms = 0);
    int get_managerTrapSubscrIp(int & managertrapIp_found, int32_t switch_board_slot, const char * switch_board_ip,  const unsigned * timeout_ms = 0);

	int find_switch_board_master (const unsigned * timeout_ms = 0);
	int find_switch_board_master (int & switch_board_active_found, acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms = 0);
	int find_switch_board_master (int & switch_board_active_found, int32_t switch_board_slot, const char * switch_board_ip, const unsigned * timeout_ms = 0);

	int update_my_switch_board_states (const unsigned * timeout_ms = 0);
	int update_other_switch_board_states (const unsigned * timeout_ms = 0);
	int update_all_switch_board_states (const unsigned * timeout_ms = 0);
	int update_switch_board_states (acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms = 0);

	int get_switch_board_status(acs_apbm_snmp::switch_board_shelf_mgr_state_t & shmgr_state, acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms = 0);
	int get_switch_board_status(acs_apbm_snmp::switch_board_shelf_mgr_state_t & shmgr_state, int32_t switch_board_slot, uint32_t switch_board_magazine, const char * switch_board_ip, const unsigned * timeout_ms = 0);

	int manage_ShelfMgrCtrl_scbrp (int magPlugNumber, int scbrpSlotPos, int shelfMgrState, int neighbour, const char * switch_board_IP);
	int manage_ShelfMgrCtrl_scx (int scbSlotPos, int shelfMgrState, int neighbour, const char * switch_board_ip);

	int manage_SELEntry (int magPlugNumber, int slotPos, const char * message = 0);

	virtual int manage_BoardPresence (int magPlugNumber, int slotPos, int hwBoardPresence, int busType, const char * switch_board_ip, const char * message = 0);

	int manage_IpmiUpgradeResult (int shelfNum, int slotPos, int ipmiUpgradeResultValue);

	virtual int manage_SensorStateChange (int magPlugNumber, int slotPos, int sensorType, int sensorID, int sensorTypeCode, const char * sensorEventData = 0);
	virtual int manage_TemperatureChange (int magPlugNumber, int slotPos, int notificationID, const char * sensorEventData = 0){UNUSED(magPlugNumber);UNUSED(slotPos);UNUSED(notificationID);UNUSED(sensorEventData);return 0;}

	virtual int manage_ColdStart (const char * switch_board_IP);

	inline int manage_WarmStart (const char * switch_board_IP) { return manage_ColdStart(switch_board_IP); }

	virtual int set_board_MIAled (uint32_t board_magazine, int32_t board_slot, int led_status);
	int set_board_REDled (uint32_t board_magazine, int32_t board_slot, bool led_status);
        virtual int set_board_BLUEled (uint32_t board_magazine, int32_t board_slot, int led_status); //4LEDs

	virtual int set_board_Power (uint32_t board_magazine, int32_t board_slot, bool pwr_status);

	int enable_front_port_scx ();
	int enable_front_port_scx (acs_apbm_switchboardinfo & switch_board);

	int configure_front_port_scx_lag (int slot_number,acs_apbm_snmp::lag_flag_type_t command);
	int reschedule_lag_operation (int slot,acs_apbm_snmp::lag_flag_type_t command);
	int configure_front_port_scx_lag (acs_apbm_switchboardinfo & switch_board,acs_apbm_snmp::lag_flag_type_t command);
	int configure_front_port_scx_lag(char * switch_board_ip);
	int disable_front_port_scx_lag(char * switch_board_ip);
	int set_admin_status_front_port_scx (acs_apbm_switchboardinfo & switch_board,int port_number,int oidValue);
	int set_front_port_scx_lag_module (acs_apbm_switchboardinfo & switch_board,int oidValue);
	template <class Type>
	int set_front_port_mib_object(char * switch_board_ip, oid * oid_name,size_t oid_size,Type value);
    static acs_apbm_snmp::lag_flag_type_t operativeStateLeft,operativeStateRight;
    static acs_apbm_snmp::lag_flag_type_t adminStateLeft,adminStateRight;
    int get_lag_operative_state(int slot);
    void set_lag_operative_state(int slot,int state);
    void set_lag_admin_state(int32_t slot,int state);
    int get_lag_admin_state(int slot);
    int get_scx_lag_vitural_port_status (int slot_number,acs_apbm_snmp::frontPort_status_t &status);

	int set_warm_restart_on_all_scx();
	int set_warm_restart_on_other_scx();
	int set_warm_restart_on_my_scx ();
	int set_warm_restart_type (acs_apbm_switchboardinfo & switch_board);

	virtual int set_board_reset(uint32_t board_magazine, int32_t board_slot);

	int set_front_port (char * switch_board_ip, acs_apbm_snmp::frontPort_status_t port_status);

	int set_scx_next_restart_type (char * switch_board_ip, acs_apbm_snmp::restart_type_t restart_type);

	virtual int get_my_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos);
	int get_other_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos);
	virtual int get_all_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos);
	int get_switch_boards_info (std::vector<acs_apbm_switchboardinfo> & board_infos, uint32_t magazine);

	int get_my_active_switch_board (acs_apbm_switchboardinfo & switch_board);

	static int map_net_snmp_error_stat (long net_snmp_error_stat);
	int initialize_switch_board_state();
	int initialize_switch_board_state (int scbrpSlotPos);
	int initialize_switch_board_state(acs_apbm_switchboardinfo & switch_board);
	int send_no_scan_scbrp();
	int start_scb_communication();
	int start_scb_communication (int scbrpSlotPos, bool sendnoscan = false);
	int start_scb_communication(acs_apbm_switchboardinfo & switch_board, bool sendnoscan = false);
	int check_working_state_active_sb();

	int start_switch_communication(acs_apbm_switchboardinfo & switch_board);

	int switch_board_delete (uint32_t magazine, int32_t slot_pos);
	int switch_board_add (acs_apbm_switchboardinfo & new_switch_board);
	int switch_board_change (acs_apbm_switchboardinfo & switch_board);
	void manage_APUB_temperature(int32_t magPlugNumber, int32_t slotPos,int temp_status);

	/*****************************************************************/

	int getHWIData (
			const char * mag_str,
			int32_t board_slot,
			const char * switch_board_ip,
			const unsigned * timeout_ms,
			const int *oidArray,
			const int &oidArraySize,
			char *outVal,
			const int &outValSize,
			const bool &isHexVal);

	int getHWIData (
			const char * mag_str,
			int32_t board_slot,
			const char * switch_board_ip1,
			const char * switch_board_ip2,
			const unsigned * timeout_ms,
			const int *oidArray,
			const int &oidArraySize,
			char *outVal,
			const int &outValSize,
			const bool &isHexVal);

	/*****************************************************************/
		int sendEgem2L2SwitchLog (const char * switch_board_ip);
		bool isEgem2L2Switch(const char * switch_board_ip);
	/*****************************************************************/

		inline uint32_t shelf_manager_control_trap_counter () const { return (_shelf_manager_control_left_scb_trap_counter_ + _shelf_manager_control_right_scb_trap_counter_); }
		inline uint32_t shelf_manager_control_left_trap_counter () const { return (_shelf_manager_control_left_scb_trap_counter_); }
		inline uint32_t shelf_manager_control_right_trap_counter () const { return (_shelf_manager_control_right_scb_trap_counter_); }
		inline uint32_t shelf_manager_control_right_trap_counter (uint32_t new_value) {
			uint32_t old_value = shelf_manager_control_right_trap_counter();
			_shelf_manager_control_right_scb_trap_counter_ = new_value;
			return old_value;
		}
		inline uint32_t shelf_manager_control_left_trap_counter (uint32_t new_value) {
					uint32_t old_value = shelf_manager_control_left_trap_counter();
					_shelf_manager_control_left_scb_trap_counter_ = new_value;
					return old_value;
				}

		int load_snmp_data(bool add_info_to_IMM = true);

		virtual int set_scb_link_down_time (int slot, uint32_t magazine, int64_t link_down_time);
		virtual int reset_scb_link_down_time (int slot, uint32_t magazine);

		virtual int64_t get_scb_link_down_time (int slot, uint32_t magazine);
		int switch_from_active_state_scbrp(acs_apbm_switchboardinfo & switch_board);

private:

	/*****************************************************************/
	//used from scxlog
	int getNode();
	int createDir(std::string dirName);
	/*****************************************************************/


	int load_my_switch_boards_info_from_cs (bool force = false);
	int load_other_switch_boards_info_from_cs (bool force = false);
	int load_switch_boards_info_from_cs (bool force = false);

	int no_my_switch_board_found_check () const;
	int no_other_switch_board_found_check () const;

	int handle_ShelfMgrCtrl_idle (int scbrpSlotPos, int neighbour, const char * switch_board_ip);
	int handle_ShelfMgrCtrl_active (int scbrpSlotPos, int neighbour, const char * switch_board_ip);
	int handle_ShelfMgrCtrl_passive (int scbrpSlotPos, int neighbour, const char * switch_board_ip);
	int send_get_request (acs_apbm_snmpsessionhandler & session_handler, const oid * oid_name, size_t oid_name_size, const unsigned * timeout_ms);

	int send_set_request (acs_apbm_snmpsessionhandler & session_handler, const oid * oid_name, size_t oid_name_size, const char * oid_value, const unsigned * timeout_ms);
	int send_set_request (acs_apbm_snmpsessionhandler & session_handler, const oid * oid_name, size_t oid_name_size, int oid_value, const unsigned * timeout_ms);
	int send_set_request (acs_apbm_snmpsessionhandler & session_handler, const oid * oid_name, size_t oid_name_size, u_char asn1_val_type, const u_char * value, size_t value_size, const unsigned * timeout_ms);

	int send_trap_subscription_request_scbrp ();
	int send_trap_subscription_request_scbrp (uint32_t magazine);
	int send_trap_subscription_request_scbrp (acs_apbm_switchboardinfo & switch_board);
	int send_trap_subscription_request_scbrp (const char * switch_board_ip, const char * trapreceiver_ip);

	int send_trap_unsubscription_request_scbrp ();
	int send_trap_unsubscription_request_scbrp (uint32_t magazine);
	int send_trap_unsubscription_request_scbrp (int scbrpSlotPos);
	int send_trap_unsubscription_request_scbrp (acs_apbm_switchboardinfo & switch_board);
	int send_trap_unsubscription_request_scbrp (const char * switch_board_ip, const char * trapreceiver_ip);

	int send_trap_subscription_request_scx (acs_apbm_switchboardinfo & switch_board);
	int send_trap_subscription_request_scx (uint32_t magazine, int target_timeout, const char * target_tag_list, const char * target_addr_params);
	int send_trap_subscription_request_scx (int target_timeout, const char * target_tag_list, const char * target_addr_params);
	int send_trap_subscription_request_scx (
			acs_apbm_switchboardinfo & switch_board,
			int target_timeout,
			const char * target_tag_list,
			const char * target_addr_params);
	int send_trap_subscription_request_scx (
			const char * switch_board_ip,
			const char * trapreceiver_ip,
			int target_timeout,
			const char * target_tag_list,
			const char * target_addr_params);

	int send_trap_unsubscription_request_scx ();
	int send_trap_unsubscription_request_scx (uint32_t magazine);
	int send_trap_unsubscription_request_scx (acs_apbm_switchboardinfo & switch_board);
	int send_trap_unsubscription_request_scx (const char * switch_board_ip, const char * trapreceiver_ip);

	int set_active_manager_trap_subscription (
			const char * switch_board_ip,
			const char * trapreceiver_ip, int request_value );

	int set_targetAddrRowStatus (
			acs_apbm_snmp::snmptarget::addr_row_status_t addr_row_status,
			acs_apbm_snmpsessionhandler & session_handler,
			oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0
			);

	int set_targetAddrTAddress (
			uint8_t (& ip_octets) [4],
			acs_apbm_snmpsessionhandler & session_handler,
			oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0
			);

	int set_targetTimeout (
			int target_timeout,
			acs_apbm_snmpsessionhandler & session_handler,
			oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0
			);

	int set_targetAddrTagList (
			const char * target_tag_list,
			acs_apbm_snmpsessionhandler & session_handler,
			oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0
			);

	int set_targetAddrParams (
			const char * target_addr_params,
			acs_apbm_snmpsessionhandler & session_handler,
			oid * oid_name,
			size_t oid_name_size,
			const unsigned * timeout_ms = 0
			);

	int get_ipmi_state(int scbrpSlotPos);
	int send_trap_ack_scbrp (int16_t switch_board_slot_position, const char * ap_ip, const char * switch_board_ip);

	int set_switch_board_state(int idx_scb, int state);
	int set_switch_board_state(int slot, uint32_t magazine, int state);

	int set_scb_passive (int scbrpSlotPos, bool updateState = true);
	int set_scb_active (int scbrpSlotPos);
	int switch_active_state_scbrp();
	int set_my_active_switch_board_index (int switch_board_index);
	int initialize_my_active_switch_board_index ();

	int get_my_operational_switch_board_ipns (char (& switch_board_ipna) [16], char (& switch_board_ipnb) [16], int32_t board_slot);

	int get_my_active_switch_board_ipns (char (& switch_board_ipna) [16], char (& switch_board_ipnb) [16]);
	int get_my_active_switch_board_index ();

	int open_snmp_session_handler (acs_apbm_snmpsessionhandler & session_handler, acs_apbm_snmpsessioncallback & callback, const char * switch_board_ip);
	int create_snmp_pdu (netsnmp_pdu * (& snmp_pdu), int command);
	int add_snmp_null_var (netsnmp_pdu * pdu, const oid * name, size_t name_size);

	int get_switch_board_info_ptr_by_ip(acs_apbm_switchboardinfo * & p_switch_board_info_ptr,  bool & scb_is_in_local_magazine, const char * switch_board_ip);

	

	//===================//
	// Utility Functions //
	//===================//
public:
	inline int slot_position_from_index (size_t index) const {
		return ((index < _my_switch_board_infos.size()) ? _my_switch_board_infos[index].slot_position : -1);
	}

	inline size_t index_from_slot_position (int pos) const {
		size_t i = 0;
		while ((i < _my_switch_board_infos.size()) && (_my_switch_board_infos[i].slot_position != pos)) ++i;
		return (i < _my_switch_board_infos.size()) ? i : -1;

//		while ((i < acs_apbm_snmp::SWITCH_BOARD_COUNT_PER_MAGAZINE) && (_my_switch_board_infos[i].slot_position != pos)) ++i;
//		return (i < acs_apbm_snmp::SWITCH_BOARD_COUNT_PER_MAGAZINE) ? i : -1;
	}

	static int oid_to_str (char * buf, size_t size, const oid * id, size_t id_length);
	static int oid_compare (const oid * id1, size_t id1_length, const oid * id2, size_t id2_length);

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
protected:
	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;

	// TODO: Mettere a disposizione dei metodi accessori dei vettori sotto per le
	// switch boards. Le informazioni sulle switch boards e sul loro stato rispetto
	// ai concetti della mastership servono anche all'esterno.
	std::vector<acs_apbm_switchboardinfo> _my_switch_board_infos;
	std::vector<acs_apbm_switchboardinfo> _other_switch_board_infos;

	int _my_active_switch_board_index;

	acs_apbm_serverworkingset * _server_working_set;

	uint32_t _shelf_manager_control_left_scb_trap_counter_;
	uint32_t _shelf_manager_control_right_scb_trap_counter_;

	static const char * _snmp_v3_passphrase;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_snmpmanager
