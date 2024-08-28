#ifndef ACS_APBM_SHELVESDATAMANAGER_H_
#define ACS_APBM_SHELVESDATAMANAGER_H_

#include <vector>
#include <list>
#include <map>
#include <ace/Recursive_Thread_Mutex.h>
#include "acs_apbm_apboardinfo.h"
#include "acs_apbm_switchboardinfo.h"
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_programconstants.h"
#include "acs_apbm_primitivedatahandler.h"
#include <acs_apbm_api.h>

#define ACS_APBM_10G_FILE "/storage/system/config/apos/drbd_network_capacity"
#define NEW_GEA_ROJ "ROJ 208 890/2"
#define SCB_R1A "R1A"
#define SCB_R2A "R2A"
#define SCB_R3A "R3A"

#define ACS_EMF_DN "AxeExternalMediaexternalMediaMId=1"

class acs_apbm_shelvesdatamanager
{
	typedef std::vector<acs_apbm_apboardinfo *> APBoardsCollection;
	typedef std::vector<acs_apbm_switchboardinfo *> SwitchBoardsCollection;
	typedef std::map<uint32_t, APBoardsCollection *> MapMagazineToAPBoards;
	typedef std::map<uint32_t, SwitchBoardsCollection *> MapMagazineToSwitchBoards;

public:
        // value is true if the HW supports 4 leds. Else it is false.
	bool is_4led_supported;
	bool is_new_gea_supported;
	bool is_raid_updated;
	int8_t is_cs_operation_completed;
	int8_t is_snmp_data_operation_completed;
	// Constants used as return codes
	enum sdm_constant
	{
		SDM_APBOARD_REPLACED = 1,
		SDM_OK = 0,
		SDM_ERROR = -1,
		SDM_INVALID_MAGAZINE_NUM = -2,
		SDM_INVALID_SLOT_NUM = -3,
		SDM_MAGAZINE_NOT_FOUND = -4,
		SDM_MAGAZINE_ALREADY_PRESENT = -5,
		SDM_NOT_INITIALIZED = -6,
		SDM_MEM_ALLOC_ERROR = -7,
		SDM_OP_NOT_PERMITTED = -8,
		SDM_YET_INITIALIZED = -9,
		SDM_APBOARD_NOT_FOUND = -10,
		SDM_INVALID_MAGAZINE_DATA = -11,
		SDM_APBOARD_STATE_MISSING = -12,
		SDM_APBOARD_STATE_BLOCKED = -13,
		SDM_APBOARD_STATE_UNBLOCKED = -14,
		SDM_APBOARD_STATE_NOT_APPLICABLE = -15,
		SDM_SNMP_ERROR = -16,
		SDM_SET_STATE_ERROR = -17,
		SDM_IMM_ERROR = -18,
		SDM_CONNECT_ERROR = -19,
		SDM_SEND_ERROR = -20,
		SDM_RECEIVE_ERROR = -21,
		SDM_UNEXPECTED_RESPONSE_ERROR = -22,
		SDM_DATA_UNAVAILABLE = -23,
		SDM_INVALID_FILE_NAME = -24,
		SDM_IPMI_ERROR = -25,
		SDM_RAID_ERROR = -26,
		SDM_UNKNOWN_APG_SHELF_ARCHITECTURE = -27
	};

	// Switchboard positions (in a Magazine)
	enum swboard_position
	{
		swb_pos_left = 0,		// SLOT 0
		swb_pos_rigth = 1		// SLOT 25
	};

	// Constructor
        acs_apbm_shelvesdatamanager(acs_apbm_serverworkingset * server_working_set):
        	is_4led_supported(false),
        	is_new_gea_supported(false),
        	is_raid_updated(false),
        	is_cs_operation_completed(0),
        	is_snmp_data_operation_completed(0),
        	is10G(false),
            is_gep7(false),
        	_server_working_set(server_working_set)
	{
	//initialise is10G if 10G feature is enabled
	is_10G_feature();
	is_hardwaretype_gep7();
}
	// Destructor
	virtual ~acs_apbm_shelvesdatamanager();

	// initializes the object reading data from CS
	sdm_constant load_from_CS(bool add_info_to_IMM = true);

	// initializes the object reading data from SNMP Agent
	sdm_constant load_snmp_data(bool add_info_to_IMM = true);

	// get BIOS version info concerning the AP board we're running on
	static int get_local_ap_board_bios_version( char (& bios_product_name) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME], char (& bios_product_version) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_VERSION ] );

	// get status of DISKS connected to the AP board we're running on
	static int get_local_ap_board_disks_status(unsigned & disks_status);

	//Redesign as per TR-HS30773	
	// get status of THUMBDRIVE connected to the AP board we're running on
        static int get_local_ap_board_thumbdrive_status(axe_eth_shelf::thumbdrive_status_t & thumbdrive_status);
	
	// get status of THUMBDRIVE connected to the other AP board
	static int get_other_ap_board_thumbdrive_status(axe_eth_shelf::thumbdrive_status_t & other_ap_board_thumbdrive_status);
	//Redesign as per TR-HS30773

	// set NIC value on other AP board
	static int set_other_ap_board_network_interface_card_info(
		    const int error_code,
			const axe_eth_shelf::nic_status_t nic_status,
			const char nic_name [acs_apbm::PCP_FIELD_SIZE_NIC_NAME],
			const char nic_ipv4_address [acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS],
			const char nic_mac_address [acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS]);

	// get nic status value
	sdm_constant get_other_ap_board_network_interface_card_status(const axe_eth_shelf::nic_type_t type, axe_eth_shelf::nic_status_t & status);

	// copy file input - output if output is 0 copy file in the input directory
	static int fcopy(const char * input, const char * output);

	//------------------------------//
	// Magazines handling methods   //
	//------------------------------//

	// adds a magazine
	sdm_constant add_magazine(uint32_t magazine);

	// remove a magazine
	sdm_constant remove_magazine(uint32_t magazine);

	// get own magazine number
	uint32_t own_magazine() const  { uint32_t mag_num = acs_apbm::BOARD_MAGAZINE_UNDEFINED; _server_working_set->cs_reader->get_my_magazine(mag_num); return mag_num; }


	//----------------------------------//
	// Switch boards handling methods   //
	//----------------------------------//

	// get info about a SwitchBoard, identified by the 'slot position'
	sdm_constant get_switch_board_info(acs_apbm_switchboardinfo & switchboardinfo, swboard_position swb_pos, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED) const;

	// set info about a SwitchBoard, identified by the 'slot position'
	sdm_constant set_switch_board_Info(const acs_apbm_switchboardinfo & switchboardinfo, swboard_position swb_pos, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);


	//----------------------------------//
	// AP board info handling methods   //
	//----------------------------------//

	// get info about an AP Board, identified by the pair: 'magazine number' and 'slot number'
	sdm_constant get_ap_board_info(acs_apbm_apboardinfo & apboardinfo, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED) const;

	// set info about an AP Board, identified by the pair: 'magazine number' and 'slot number'
	sdm_constant set_ap_board_info(const acs_apbm_apboardinfo & apboardinfo, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// reset info about an AP Board, identified by the pair: 'magazine number' and 'slot number'
	sdm_constant reset_ap_board_info(bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update info about on AP Board
	sdm_constant update_ap_board_info(bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// remove the AP Board info, identified by the pair: 'magazine number' and 'slot number'
	sdm_constant remove_ap_board_info(bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// get the status of an AP board
	sdm_constant get_ap_board_status(axe_eth_shelf::status_t & apboard_status, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED) const;

	// set the status of an AP board
	sdm_constant set_ap_board_status(axe_eth_shelf::status_t apboard_status, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// set the status of an AP board, returning the previous state
	sdm_constant set_ap_board_status(axe_eth_shelf::status_t apboard_status, bool update_IMM, int32_t slot, uint32_t magazine, axe_eth_shelf::status_t & old_apboard_status );

	// set the AP board leds data
	sdm_constant set_ap_board_LEDS_status(axe_eth_shelf::led_status_t green_led, axe_eth_shelf::led_status_t red_led, axe_eth_shelf::led_status_t mia_led, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);
 
        // set the AP board leds data four LEDs
        sdm_constant set_ap_board_LEDS_status(axe_eth_shelf::led_status_t green_led, axe_eth_shelf::led_status_t red_led, axe_eth_shelf::led_status_t mia_led, axe_eth_shelf::led_status_t blue_led, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// get the bus type of an AP board
	sdm_constant get_ap_board_bus_type(axe_eth_shelf::bus_type_t & apboard_bus_type, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED) const;

	// get the fbn and the status of an AP board
	sdm_constant get_ap_board_fbn_and_status(uint16_t & fbn, axe_eth_shelf::status_t & apboardstatus, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED) const;

	// get the fbn and the status of an AP board
	sdm_constant get_ap_board_fbn(uint16_t & fbn, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED) const;

	//get the ipmi firmware upgrade informations
	sdm_constant get_ipmifw_status(axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upg_status, char (&ipmifw_lastupgtime) [acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME],int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// get the list of slots in a magazine, containing the requested type of AP boards
	sdm_constant search_ap_boards_by_fbn(std::list<int32_t> & slots, uint16_t fbn, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the green led of an AP board
	sdm_constant update_ap_board_green_led(axe_eth_shelf::led_status_t green_led, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the mia led of an AP board
	sdm_constant update_ap_board_mia_led(axe_eth_shelf::led_status_t mia_led, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the red led of an AP board
	sdm_constant update_ap_board_red_led(axe_eth_shelf::led_status_t red_led, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

        // update the blue led of an AP board
	sdm_constant update_ap_board_blue_led(axe_eth_shelf::led_status_t blue_led, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update default led value only in IMM repository
	sdm_constant update_ap_board_default_led_values(int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	sdm_constant update_ap_board_default_gea_led_values(int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);
	// update default nic value only in IMM repository
	sdm_constant update_ap_board_default_nic_values(int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the fatal event log of an AP board
	sdm_constant update_ap_board_fatal_event_log(char (& fatal_event_log_info)[acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1], bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the bios boot mode of an AP board
	sdm_constant update_ap_board_bios_boot_run_mode(axe_eth_shelf::bios_boot_run_mode_t bios_running_mode, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the ipmi upgrade status of an AP board
	sdm_constant update_ap_board_ipmi_upgrade_status(axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);
	sdm_constant update_ap_board_ipmi_upgrade_status_on_ap(axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);
	sdm_constant update_ap_board_ipmi_upgrade_status_on_scb(axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the ipmi upgrade status of an AP board
	sdm_constant update_ap_board_ipmi_upgrade_last_time( bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);
	sdm_constant update_ap_board_ipmi_upgrade_last_time_on_ap( bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);
	sdm_constant update_ap_board_ipmi_upgrade_last_time_on_scb( bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the temperature status of an AP board
	sdm_constant update_ap_board_temperature_status(char (& temperature_status) [acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1], bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the alarm interface status of an AP board
	sdm_constant update_ap_board_alarm_interface_status(axe_eth_shelf::alarm_status_t alarm_status, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the impi data of an AP board
	sdm_constant update_ap_board_ipmi_data(
			int16_t & ipmi_firmware_type,
			char (& ipmi_product_number) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1],
			char (& ipmi_product_revision) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],
			bool update_IMM,
			int32_t slot,
			uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the snmp data of an AP board
	sdm_constant update_ap_board_snmp_data (
			char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
			char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
			char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
			char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
			char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
			char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
			bool update_IMM,
			int32_t board_slot,
			uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the network_interfaces_cards status of an AP board for virtualized environment
	sdm_constant update_ap_board_network_interfaces_cards_status(
				axe_eth_shelf::nic_status_t nic0,
				axe_eth_shelf::nic_status_t nic1,
				axe_eth_shelf::nic_status_t nic2,
				axe_eth_shelf::nic_status_t nic3,
				axe_eth_shelf::nic_status_t nic4,
				axe_eth_shelf::nic_status_t nic5,
				axe_eth_shelf::nic_status_t nic6,
				axe_eth_shelf::nic_status_t nic7,
				axe_eth_shelf::nic_status_t nic8,
				bool update_IMM,
				int32_t slot,
				uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);


	// update the network_interfaces_cards status of an AP board for ETH5 and ETH6 SUPPORT
	sdm_constant update_ap_board_network_interfaces_cards_status(
			axe_eth_shelf::nic_status_t nic0,
			axe_eth_shelf::nic_status_t nic1,
			axe_eth_shelf::nic_status_t nic2,
			axe_eth_shelf::nic_status_t nic3,
			axe_eth_shelf::nic_status_t nic4,
			axe_eth_shelf::nic_status_t nic5, 
			axe_eth_shelf::nic_status_t nic6,
			bool update_IMM,
			int32_t slot,
			uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);


	// update the network_interfaces_cards status of an AP board
	sdm_constant update_ap_board_network_interfaces_cards_status(
			axe_eth_shelf::nic_status_t nic0,
			axe_eth_shelf::nic_status_t nic1,
			axe_eth_shelf::nic_status_t nic2,
			axe_eth_shelf::nic_status_t nic3,
			axe_eth_shelf::nic_status_t nic4,
			bool update_IMM,
			int32_t slot,
			uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);
	
	// update the network_interfaces_cards status of an AP board
	sdm_constant update_ap_board_network_interface_card_info(
			axe_eth_shelf::nic_status_t nic,
			char (& nic_name) [acs_apbm::PCP_FIELD_SIZE_NIC_NAME + 1],
			char (& nic_ipv4_address) [acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS + 1],
			char (& nic_mac_address) [acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS + 1],
			bool update_IMM,
			int32_t slot,
			uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the update_ap_board_raid_status of an AP board
	sdm_constant update_ap_board_raid_status(axe_eth_shelf::raid_status_t raid_status, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	 // update the update_ap_board_raid_status of an AP board
        sdm_constant update_ap_board_drbd_status(axe_eth_shelf::drbd_status_t drbd_status, bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// update the update_ap_board_bios_version of an AP board
	sdm_constant update_ap_board_bios_version(bool update_IMM, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	//-----------------------------------------//
	// AP board methods implementing ACTIONS   //
	//-----------------------------------------//

	// 'BLOCK' an AP board
	sdm_constant request_block_on_ap_board(int16_t & imm_error_code, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// 'UNBLOSK' an AP board
	sdm_constant request_deblock_on_ap_board(int16_t & imm_error_code, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	//reset board
	sdm_constant request_reset_on_ap_board(int16_t & imm_error_code, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED);

	// perform a 'IPMI firmware upgrade' operation on an AP board
	int request_ipmi_upgrade_on_ap_board(int16_t & imm_error_code, const char * firmware_image_path, int32_t slot, uint32_t magazine = acs_apbm::BOARD_MAGAZINE_UNDEFINED, uint8_t comport = acs_apbm::NO_COM);

	// is 10G
	bool is_10G();
	 
	//Redesign as per TR-HS30773
	int getoperationalStateFromIMM();

	//Check if the hardware type is gep7
	bool is_hardwaretype_gep7();

private:
	acs_apbm_shelvesdatamanager(const acs_apbm_shelvesdatamanager &);
	acs_apbm_shelvesdatamanager & operator=(const acs_apbm_shelvesdatamanager &);

	// get BIOS version info concerning the partner AP board
	static int get_other_ap_board_bios_version( char (& bios_product_name) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME], char (& bios_product_version) [acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_VERSION ] );

	// get status of DISKS connected to the other AP board
	static int get_other_ap_board_disks_status(unsigned & other_ap_board_disks_status);
	

	static int retrieve_nic_type_by_name(axe_eth_shelf::nic_name_t & type, const char * name);

	// check <magazine> and <slot> parameters
	sdm_constant _check_parameters(int32_t slot, uint32_t magazine) const;

        //check raid status
        sdm_constant _check_disks_raid_status(int32_t slot, uint32_t magazine);

        //check drbd status
        sdm_constant _check_disks_drbd_status(int32_t slot, uint32_t magazine);
	
// returns a pointer to the acs_apbm_apboardinfo object identified by <magazine> and <slot>
	sdm_constant _retrieve_ap_board_info_ptr(acs_apbm_apboardinfo * & apboardinfo_ptr, int32_t slot, uint32_t magazine) const;

	// helper methods
	sdm_constant _exec_snmp_common_work_to_block_ap_board(int32_t slot, uint32_t magazine);
	sdm_constant _request_block_on_dvd_board(int32_t slot, uint32_t magazine) { return _exec_snmp_common_work_to_block_ap_board(slot, magazine); }
	sdm_constant _request_block_on_gea_board(int32_t slot, uint32_t magazine) { return _exec_snmp_common_work_to_block_ap_board(slot, magazine); }
	sdm_constant _request_block_on_apub_board(int32_t slot, uint32_t magazine){ return _exec_snmp_common_work_to_block_ap_board(slot, magazine); }
	sdm_constant _request_block_on_disk_board(int32_t slot, uint32_t magazine);
       
        //Used to initialise the variable is_4led_supported.
        sdm_constant get_board_environment(int32_t slot);               
        sdm_constant get_gea_environment(int32_t slot);               
	void is_10G_feature();
private:
	bool is10G;
	bool is_gep7;

	MapMagazineToAPBoards _apboards_map;								// keep info about the AP boards in the system
	mutable ACE_Recursive_Thread_Mutex _apboards_map_mutex;				// used to synchronize access to <_apBoardsMap>
	MapMagazineToSwitchBoards _switchboards_map;						// keep info about the Switch boards in the system
	mutable ACE_Recursive_Thread_Mutex _switchboards_map_mutex;			// used to synchronize access to <_switchBoardsMap>
	acs_apbm_serverworkingset * _server_working_set;
	int8_t _action_execution_state;                         // used to synchronize Action
	static const int32_t MIN_SLOT_NUM = 1;
	static const int32_t MAX_SLOT_NUM = 24;
	static const uint32_t NUM_SWITCH_BOARDS_IN_A_MAGAZINE = 2;
};

#endif /* ACS_APBM_SHELVESDATAMANAGER_H_ */
