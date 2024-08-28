/*
 * acs_apbm_ironsidemanager.h
 *
 *  Created on: Dec 7, 2012
 *      Author: estevol
 */

#ifndef HEADER_GUARD_CLASS__acs_apbm_ironsidemanager
#define HEADER_GUARD_CLASS__acs_apbm_ironsidemanager acs_apbm_ironsidemanager

#include "acs_apbm_snmpmanager.h"
#include <acs_nclib_library.h>
#include <map>

class acs_apbm_serverworkingset;
class acs_nclib_session;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_ironsidemanager


class acs_apbm_ironsidemanager: public acs_apbm_snmpmanager {

public:
	inline acs_apbm_ironsidemanager(acs_apbm_serverworkingset * server_working_set):
	acs_apbm_snmpmanager(server_working_set)
	{
		//initialize_dmx_boardinfo();
	}

	virtual inline ~acs_apbm_ironsidemanager(){}

	static inline void initialize () { acs_nclib_library::init(); }
	static inline void shutdown () { acs_nclib_library::exit(); }

	virtual int send_trap_subscription_request ();
	virtual int send_trap_subscription_request (uint32_t /*magazine*/) { return send_trap_subscription_request(); }
	virtual int send_trap_unsubscription_request ();
	virtual int send_trap_unsubscription_request (int /*switch_board_slot_position*/) { return send_trap_unsubscription_request(); }
	virtual int send_trap_unsubscription_request (uint32_t /*magazine*/) { return send_trap_unsubscription_request(); }

	virtual int get_board_ipmi_data (
			int16_t & ipmi_firmware_type,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	virtual int get_board_ipmi_upgrade_status (
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	virtual int set_board_ipmi_upgrade (
			const char * upgrade_path,
			const char * upgrade_ip4,
			int32_t board_slot,
			uint32_t board_magazine,
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
        virtual int get_blade_led_type (
                        acs_apbm_snmp::blade_led_type_t & blade_led_type,
                        int32_t board_slot,
                        const unsigned * timeout_ms);
        virtual int get_blade_led_type (
                        acs_apbm_snmp::blade_led_type_t & blade_led_type,
                        int32_t board_slot,
                        uint32_t board_magazine,
                        const unsigned * timeout_ms);

	virtual int get_board_leds_status (
			axe_eth_shelf::led_status_t & green_led,
			axe_eth_shelf::led_status_t & red_led,
			axe_eth_shelf::led_status_t & mia_led,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	virtual int get_board_leds_status (
			axe_eth_shelf::led_status_t & green_led,
			axe_eth_shelf::led_status_t & red_led,
			axe_eth_shelf::led_status_t & mia_led,
			axe_eth_shelf::led_status_t & blue_led,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0); //4Leds compatibility

	virtual int get_board_bios_run_mode (
			axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	virtual int get_board_power_status (
			acs_apbm_snmp::bladePwr_status_t & power_status,
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	virtual int get_board_fatal_event_log (
			char (& fatalEventLogInfo) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
			int32_t board_slot,
			uint32_t board_magazine,
			const unsigned * timeout_ms = 0);

	virtual int get_managerTrapSubscrIp(
			acs_apbm_switchboardinfo & ,
		const unsigned * timeout_ms = 0);

	virtual int manage_SensorStateChange (
			//	Check whether we still receive this trap
			int magPlugNumber,
			int slotPos,
                        int sensorType, 
                        int sensorID, 
                        int sensorTypeCode,
			const char * sensorEventData = 0);

	virtual int manage_BoardPresence (
			int magPlugNumber,
			int slotPos,
			int BoardadmnState,
			int BoardPresenceState,
			const char * switch_board_ip,
			const char * message = 0);

	int manage_TemperatureChange (
		int magPlugNumber,
		int slotPos,
		int notificationID,
		const char * sensorEventData);

	virtual int manage_ColdStart (
			const char * switch_board_IP);

	virtual int set_board_REDled (
			uint32_t board_magazine,
			int32_t board_slot,
			bool led_status);

	virtual int set_board_MIAled (
			uint32_t board_magazine,
			int32_t board_slot,
			int led_status);

	virtual int set_board_BLUEled (
			uint32_t board_magazine,
			int32_t board_slot,
			int led_status);

	virtual int set_board_Power (
			uint32_t board_magazine,
			int32_t board_slot,
			bool pwr_status);

	virtual int set_board_reset(
			uint32_t board_magazine,
			int32_t board_slot);

	virtual int get_hw_install_status(
			axe_eth_shelf::hw_installation_status_t &status,
			int32_t board_slot,
			uint32_t board_magazine,
			unsigned int* timeout
	);

	virtual int get_my_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos);
	virtual int get_all_switch_board_info (std::vector<acs_apbm_switchboardinfo> & board_infos);
	virtual int set_scb_link_down_time (int slot, uint32_t magazine, int64_t link_down_time);
	virtual int reset_scb_link_down_time (int slot, uint32_t magazine);
	virtual int64_t get_scb_link_down_time (int slot, uint32_t magazine);
       
	int Check_error_ironside(std::string data);
	int initialize_dmx_boardinfo();

private:
	bool send_trap_subscription_all(std::string IpV4NetworkId_str,std::string IpV4ConnectionId_str);

	int shelf_addr_to_physical_addr(const char* shelfaddr);

	int get_shelfId_from_address_loop(int phaddr,std::string& shelfId);
	int get_shelfId_from_address_loop(const char* shelf_addr, std::string& shelfId);

	int dec_to_hex_fatalEvent(const char* int_fatalEvent, char (& hex_fatalEvent) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1]);
	
	int getRunningIpmi(std::string ,std::string &prod ,std::string &rev, int &type);
	
	std::string setmbmcomand(std::string cmd,int octet);

	int get_octet(std::string bgci,int &octet);

	int setbgci(std::string  cmd,int octet);
	int setsubscriber(std::string query);

private:
	acs_apbm_switchboardinfo _my_dmx_board_infos;
	std::map<int,std::string> _shelf_addr_id_map;
};


#endif /* ACS_APBM_IRONSIDEMANAGER_H_ */
