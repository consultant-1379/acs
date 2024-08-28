/*
 * acs_apbm_apboardinfo.h
 *
 *  Created on: Sep 8, 2011
 *      Author: xgiufer
 */
#ifndef HEADER_GUARD_CLASS__acs_apbm_apboardinfo
#define HEADER_GUARD_CLASS__acs_apbm_apboardinfo acs_apbm_apboardinfo

#include <string.h>

#include <acs_apbm_macros.h>
#include "acs_apbm_snmpmibdefinitions.h"
#include <acs_apbm_programconstants.h>

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_apboardinfo

namespace axe_eth_shelf = acs_apbm_snmp::axe::eth::shelf;


class __CLASS_NAME__ {
	//============//
	// Constructor //
	//============//
public:
	/** @brief acs_apbm_axe_eth_shelf Default constructor
	 *
	 *	acs_apbm_apboardinfo Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ ()
	: product_number(), product_revision_state(), product_name(), serial_number(), manufacture_date(), vendor(),
	  ipmi_firmware_type(-1), ipmi_product_number(), ipmi_product_revision(), ipmi_upgrade_status(axe_eth_shelf::IPMI_UPGRADE_STATUS_READY),
	  ipmi_upgrade_last_time(),green_led(static_cast<axe_eth_shelf::led_status_t>(2)), red_led(static_cast<axe_eth_shelf::led_status_t>(2)),
	  mia_led(static_cast<axe_eth_shelf::led_status_t>(2)),blue_led(static_cast<axe_eth_shelf::led_status_t>(2)),red_led_supported(false),
	  mia_led_supported(false), green_led_supported(false), blue_led_supported(false), raid_disk_status(axe_eth_shelf::RAID_STATUS_UNDEFINED),
	  drbd_disk_status(axe_eth_shelf::DRBD_STATUS_UNDEFINED),
	  nic_0(axe_eth_shelf::NIC_STATUS_UNDEFINED), nic_1(axe_eth_shelf::NIC_STATUS_UNDEFINED),
	  nic_2(axe_eth_shelf::NIC_STATUS_UNDEFINED), nic_3(axe_eth_shelf::NIC_STATUS_UNDEFINED),
	  nic_4(axe_eth_shelf::NIC_STATUS_UNDEFINED), nic_5(axe_eth_shelf::NIC_STATUS_UNDEFINED),
	  nic_6(axe_eth_shelf::NIC_STATUS_UNDEFINED), bond_1(axe_eth_shelf::NIC_STATUS_UNDEFINED),
	  nic_7(axe_eth_shelf::NIC_STATUS_UNDEFINED), nic_8(axe_eth_shelf::NIC_STATUS_UNDEFINED),
	  bios_running_mode(axe_eth_shelf::BIOS_BOOT_RUN_MODE_UNDEFINED), bios_version(),
	  board_status(axe_eth_shelf::BOARD_STATUS_UNDEFINED), alarm_interface_status(axe_eth_shelf::ALARM_INTERFACE_STATUS_UNDEFINED),
	  temperature_status(), fatalEventLogInfo(), watchdog_timeout(0),
	  slot_position(acs_apbm::BOARD_SLOT_UNDEFINED),  // TO VERIFY
	  magazine(acs_apbm::BOARD_MAGAZINE_UNDEFINED),
	  fbn(0),bus_type(axe_eth_shelf::BUS_TYPE_UNDEFINED),
	  ip_address(),
	  mac_address(),
	  snmp_data_valid(true)
	{
                *product_number = 0;
		*product_revision_state = 0;
		*product_name = 0;
		*serial_number = 0;
		*manufacture_date = 0;
		*vendor = 0;
		//*ipmi_firmware_type = 0;
		*ipmi_product_number = 0;
		*ipmi_product_revision = 0;
		*ipmi_upgrade_last_time = 0;
		*ip_address = 0;
		*mac_address = 0;

		::strncpy(temperature_status, "N/A", ACS_APBM_ARRAY_SIZE(temperature_status));
		temperature_status[ACS_APBM_ARRAY_SIZE(temperature_status) - 1] = 0;

		::strncpy(fatalEventLogInfo,"N/A",ACS_APBM_ARRAY_SIZE(fatalEventLogInfo));
		::strncpy(bios_version,"N/A", ACS_APBM_ARRAY_SIZE(bios_version));
		::strncpy(ipmi_upgrade_last_time, "-", ACS_APBM_ARRAY_SIZE(ipmi_upgrade_last_time));
		} //INIZIALIZZAZIONE CON PARAMETRI UTILI AL SERVER


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_apboardinfo Destructor
	 *
	 *	acs_apbm_apboardinfo Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline ~__CLASS_NAME__ () {}


public:

	//---------------------//
	// Product Information //		// xshmcBoardData	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.5
	//---------------------//

	char product_number [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1]; // xshmcBoardData[ 0-23] : [24 ASCII characters]

	char product_revision_state[acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1]; // xshmcBoardData[24-30] : [7 ASCII characters]

	char product_name[acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1]; // xshmcBoardData[31-42] : [12 ASCII characters]

	char serial_number[acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1]; // xshmcBoardData[43-55] :	[13 ASCII characters - 6 factory code 7 individual number]

	char manufacture_date[acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1]; // xshmcBoardData[56-63] : [8 ASCII characters]

	char vendor[acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1]; // xshmcBoardData[64-94] :[31 ASCII characters]

	//---------------------//
	//   IPMI Information  // 			xshmcBoardIPMIData	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.14
	//---------------------//

	int16_t ipmi_firmware_type;		// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)

	char ipmi_product_number[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1]; 	// xshmcBoardIPMIData[1-25]  : [25 ASCII characters]

	char ipmi_product_revision[acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1];	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]

	axe_eth_shelf::ipmi_upgrade_status_t ipmi_upgrade_status;	// xshmcBoardIPMIUpgradeStatus  .1.3.6.1.4.1.193.154.2.1.2.1.1.1.16

	char ipmi_upgrade_last_time[acs_apbm_snmp::SNMP_BOARD_IPMI_UPGRADE_LAST_TIME];

	//---------------------//
	// LED Information    // 			xshmcBoardIPMIData	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.14
	//---------------------//

	axe_eth_shelf::led_status_t green_led;   // xshmcBoardGreenLed	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.6
                                                 //4LEDs: xshmcBoardCommonGreenLed 1. 3. 6. 1.4.1.193.154.2 . 1. 2. 1. 1. 1. 23 

	axe_eth_shelf::led_status_t red_led; // xshmcBoardRedLed	  	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.7
                                             // 4LEDs: xshmcBoardCommonRedLed 1. 3. 6. 1.4.1.193.154.2 . 1. 2. 1. 1. 1. 22 

	axe_eth_shelf::led_status_t mia_led;	// xshmcBoardYellowLed	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.8
                                               // 4LEDs: xshmcBoardCommonYellowLed 1. 3. 6. 1.4.1.193.154.2 . 1. 2. 1. 1. 1. 25 

	axe_eth_shelf::led_status_t blue_led; // 4LEDs: xshmcBoardCommonBlueLed 1. 3. 6. 1.4.1.193.154.2 . 1. 2. 1. 1. 1. 24 


	bool red_led_supported;
	bool mia_led_supported;
	bool green_led_supported;
	bool blue_led_supported;  
	//---------------------//
	//   RAID Information  //
	//---------------------//

	axe_eth_shelf::raid_status_t raid_disk_status;
	axe_eth_shelf::drbd_status_t drbd_disk_status;
	//---------------------//
	//   NIC Information   //
	//---------------------//

	axe_eth_shelf::nic_status_t nic_0;

	axe_eth_shelf::nic_status_t nic_1;

	axe_eth_shelf::nic_status_t nic_2;

	axe_eth_shelf::nic_status_t nic_3;

	axe_eth_shelf::nic_status_t nic_4;
	
	axe_eth_shelf::nic_status_t nic_5; //ETH5 and ETH6 SUPPORT

    axe_eth_shelf::nic_status_t nic_6; //ETH5 and ETH6 SUPPORT

    axe_eth_shelf::nic_status_t bond_1;

    axe_eth_shelf::nic_status_t nic_7;

    axe_eth_shelf::nic_status_t nic_8;


	//---------------------------//
	//  OTHER Board Information  //
	//---------------------------//

	axe_eth_shelf::bios_boot_run_mode_t bios_running_mode; 	// xshmcBoardBiosBootRunMode	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.17

	char bios_version [64 + 1];

	axe_eth_shelf::status_t board_status;

	axe_eth_shelf::alarm_status_t alarm_interface_status;

	char temperature_status[acs_apbm_snmp::SNMP_SENSOR_EVENT_DATA_SIZE + 1];

	char fatalEventLogInfo [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1]; 		// xshmcBoardFatalEventLogInfo	.1.3.6.1.4.1.193.154.2.1.2.1.1.1.13

	u_int32_t watchdog_timeout;

	//-------------------------------------//
	//  Configuration Service Information  //
	//-------------------------------------//

	int32_t slot_position;

	uint32_t magazine;

	uint16_t fbn;

	axe_eth_shelf::bus_type_t bus_type;

	char ip_address[32 + 1];

	char mac_address[32 + 1];

	bool snmp_data_valid;
};



#endif /* HEADER_GUARD_CLASS__acs_apbm_apboardinfo */
