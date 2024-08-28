#ifndef HEADER_GUARD_FILE__acs_apbm_snmpsmcmibdefinitions
#define HEADER_GUARD_FILE__acs_apbm_snmpsmcmibdefinitions acs_apbm_snmpsmcmibdefinitions.h

#include <stdint.h>

/* FROM NET-SNMP library
#define SNMP_OID_INTERNET		1, 3, 6, 1
#define SNMP_OID_ENTERPRISES		SNMP_OID_INTERNET, 4, 1
#define SNMP_OID_MIB2			SNMP_OID_INTERNET, 2, 1
#define SNMP_OID_SNMPV2			SNMP_OID_INTERNET, 6
#define SNMP_OID_SNMPMODULES		SNMP_OID_SNMPV2, 3
*/

#define ACS_APBM_SNMP_OID_ERICSSON					SNMP_OID_ENTERPRISES, 193
#define ACS_APBM_SNMP_OID_ERICSSON_AXE				ACS_APBM_SNMP_OID_ERICSSON, 154
#define ACS_APBM_SNMP_OID_ERICSSON_GENERIC			ACS_APBM_SNMP_OID_ERICSSON, 177
#define ACS_APBM_SNMP_OID_ERICSSON_ALARM			ACS_APBM_SNMP_OID_ERICSSON, 99991

// AXE-ETH-SHELF-MIB definitions for SCB-RP
#define ACS_APBM_SCB_OID_ACTIVE_MANAGER_TRAP_SUBSCR					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 1, 2, 2
#define ACS_APBM_SCB_OID_ACTIVE_MANAGER_TRAP_ACK					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 1, 2, 3
#define ACS_APBM_SCB_OID_XSHMC_MANAGER_IP_ADDRESS					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 1, 2, 1, 1, 2
#define ACS_APBM_SCB_OID_XSHMC_BOARD_DATA							ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 5
#define ACS_APBM_SCB_OID_XSHMC_BOARD_GREEN_LED						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 6
#define ACS_APBM_SCB_OID_XSHMC_BOARD_RED_LED						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 7
#define ACS_APBM_SCB_OID_XSHMC_BOARD_YELLOW_LED						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 8
#define ACS_APBM_SCB_OID_XSHMC_BOARD_RESET                                         ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 9
#define ACS_APBM_SCB_OID_XSHMC_BOARD_RESET                          ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 9
#define ACS_APBM_SCB_OID_XSHMC_BOARD_PWR_OFF						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 10
#define ACS_APBM_SCB_OID_XSHMC_BOARD_PWR_ON							ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 11
#define ACS_APBM_SCB_OID_SHELF_MGR_CMD								ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 12
#define ACS_APBM_SCB_OID_XSHMC_BOARD_FATAL_EVENT_LOG_INFO			ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 13
#define ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_DATA						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 14
#define ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_UPGRADE					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 15
#define ACS_APBM_SCB_OID_XSHMC_BOARD_IPMI_UPGRADE_STATUS			ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 16
#define ACS_APBM_SCB_OID_XSHMC_BOARD_BIOS_RUN_MODE					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 17
//SCB MIBS for 4LED hardware
#define ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_RED_LED					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 22
#define ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_GREEN_LED				ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 23
#define ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_BLUE_LED                 ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 24
#define ACS_APBM_SCB_OID_XSHMC_BOARDCOMMON_YELLOW_LED				ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 25



//Mib Type to check if the HW supports four leds or not 
#define ACS_APBM_SCB_OID_XSHMC_BOARD_LED_TYPE                                                ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 26

// AXE-ETH-SHELF-MIB notifications object OIDs for SCB-RP
#define ACS_APBM_SCB_OID_XSHMC_MAG_PLUG_NUMBER						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 1
#define ACS_APBM_SCB_OID_XSHMC_SCBRP_SLOT_POS						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 2
#define ACS_APBM_SCB_OID_XSHMC_SLOT_POS								ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 3
#define ACS_APBM_SCB_OID_XSHMC_HW_BOARD_PRESENCE					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 4
#define ACS_APBM_SCB_OID_XSHMC_BUS_TYPE								ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 5
#define ACS_APBM_SCB_OID_XSHMC_SENSOR_TYPE							ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 6
#define ACS_APBM_SCB_OID_XSHMC_SENSOR_ID							ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 7
#define ACS_APBM_SCB_OID_XSHMC_SENSOR_TYPE_CODE						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 8
#define ACS_APBM_SCB_OID_XSHMC_SENSOR_EVENT_DATA					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 9
#define ACS_APBM_SCB_OID_XSHMC_SHELF_MGR_STATE						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 10
#define ACS_APBM_SCB_OID_XSHMC_NEIGHBOUR							ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 11
#define ACS_APBM_SCB_OID_XSHMC_SEL_ENTRY							ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 3, 12


//  GENERIC-SHELF-MIB definitions for SCXB
#define ACS_APBM_SCX_OID_BLADE_PRODUCT_NUMBER						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 2
#define ACS_APBM_SCX_OID_BLADE_PRODUCT_REVISION_STATE				ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 3
#define ACS_APBM_SCX_OID_BLADE_PRODUCT_NAME							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 4
#define ACS_APBM_SCX_OID_BLADE_SERIAL_NUMBER						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 5
#define ACS_APBM_SCX_OID_BLADE_MANUFACTURING_DATE					ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 6
#define ACS_APBM_SCX_OID_BLADE_VENDOR_NAME							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 7
#define ACS_APBM_SCX_OID_BLADE_GREEN_LED							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 9
#define ACS_APBM_SCX_OID_BLADE_RED_LED								ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 10
#define ACS_APBM_SCX_OID_BLADE_YELLOW_LED                			ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 11
#define ACS_APBM_SCX_OID_BLADE_RESET                                       ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 12
#define ACS_APBM_SCX_OID_BLADE_RESET                                ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 12
#define ACS_APBM_SCX_OID_BLADE_IPMI_DATA							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 15
#define ACS_APBM_SCX_OID_BLADE_IPMI_UPGRADE							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 16
#define ACS_APBM_SCX_OID_BLADE_IPMI_UPGRADE_STATUS					ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 17
#define ACS_APBM_SCX_OID_BLADE_BIOS_RUN_MODE						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 24
#define ACS_APBM_SCX_OID_FATAL_EVENT_LOG_INFO						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 27
#define ACS_APBM_SCX_OID_BLADE_PWR									ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 43
//SCX MIBs for 4LED hardware
#define ACS_APBM_SCX_OID_BLADECOMMON_RED_LED                        ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 30
#define ACS_APBM_SCX_OID_BLADECOMMON_GREEN_LED                      ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 31
#define ACS_APBM_SCX_OID_BLADECOMMON_BLUE_LED                       ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 32
#define ACS_APBM_SCX_OID_BLADECOMMON_YELLOW_LED                     ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 33

//Mib Type to check if the HW supports four leds or not 
#define ACS_APBM_SCX_OID_BLADE_LED_TYPE                                                                 ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 42

// GENERIC-SHELF-MIB notifications object OIDs for SCXB
#define ACS_APBM_SCX_OID_SHELF_NUM									ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 1
#define ACS_APBM_SCX_OID_SHMC_SLOT_POS								ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 2
#define ACS_APBM_SCX_OID_SLOT_POS									ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 3
#define ACS_APBM_SCX_OID_SHELF_MGR_STATE							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 4
#define ACS_APBM_SCX_OID_NEIGHBOUR									ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 5
#define ACS_APBM_SCX_OID_CONTROL_BRD_IP_ADDRESS						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 6
#define ACS_APBM_SCX_OID_HW_BLADE_PRESENCE							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 7
#define ACS_APBM_SCX_OID_BUS_TYPE									ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 8
#define ACS_APBM_SCX_OID_SENSOR_TYPE								ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 9
#define ACS_APBM_SCX_OID_SENSOR_ID									ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 10
#define ACS_APBM_SCX_OID_SENSOR_TYPE_CODE							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 11
#define ACS_APBM_SCX_OID_SENSOR_EVENT_DATA							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 12
#define ACS_APBM_SCX_OID_IPMI_UPGRADE_RESULT_VALUE					ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 13
#define ACS_APBM_SCX_OID_IPMI_FW_TYPE								ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 20
#define ACS_APBM_SCX_OID_IPMI_FW_VERSION							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 21
#define ACS_APBM_SCX_OID_IPMI_FW_TIMESTAMP							ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 22
#define ACS_APBM_SCX_OID_SEL_ENTRY									ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 23
#define ACS_APBM_SCX_OID_TYPE_OF_BP_ID_ERROR						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 24
#define ACS_APBM_SCX_OID_RESULT_OF_DIAGNOSTIC_TEST					ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 25
#define ACS_APBM_SCX_OID_SHELF_PHYSICAL_ADDRESS						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 1, 26

#define ACS_APBM_SCX_OID_SHELF_MGR_CTRL_STATE						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 2, 1, 8
#define ACS_APBM_SCX_OID_SHELF_MGR_NEIGHBOUR_PRESENCE				ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 2, 1, 9


//ERICSSON-ALARM-MIB notification objects for DMX
#define ACS_APBM_DMX_OID_ALARM_ALERT_INDEX							ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 1
#define ACS_APBM_DMX_OID_ALARM_ALERT_MAJOR_TYPE						ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 2
#define ACS_APBM_DMX_OID_ALARM_ALERT_MINOR_TYPE						ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 3
#define ACS_APBM_DMX_OID_ALARM_ALERT_SPECIFIC_PROBLEM				ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 4
#define ACS_APBM_DMX_OID_ALARM_ALERT_MANAGED_OBJECT					ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 5
#define ACS_APBM_DMX_OID_ALARM_ALERT_EVENT_TYPE						ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 6
#define ACS_APBM_DMX_OID_ALARM_ALERT_EVENT_TIME						ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 7
#define ACS_APBM_DMX_OID_ALARM_ALERT_PROBABLE_CAUSE					ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 8
#define ACS_APBM_DMX_OID_ALARM_ALERT_SEVERITY						ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 9
#define ACS_APBM_DMX_OID_ALARM_ALERT_ADDITIONAL_TEXT				ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 10
#define ACS_APBM_DMX_OID_ALARM_ALERT_RESOURCE_ID					ACS_APBM_SNMP_OID_ERICSSON_ALARM, 1, 4, 5, 1, 11


// SNMPv2-MIB definitions
#define ACS_APBM_SNMP_TRAP_OID SNMP_OID_SNMPMODULES, 1, 1, 4, 1

// SNMP_OID_MIB2 definitions                      // SCX LAG OID MIB DEFINATIONS
#define IF_ADMIN_STATUS	SNMP_OID_MIB2, 2, 2, 1, 7

#define SNMP_LAG_MIB											   1,2,840,10006,300,43
#define IF_OPER_STATUS	                                           SNMP_OID_MIB2, 2, 2, 1, 8
#define ACS_APBM_SCX_OID_LAG_TABLE_ENTRY                           ACS_APBM_SNMP_OID_ERICSSON_GENERIC,2,1,1,2,1,1,3
#define ACS_APBM_SCX_OID_LAG_TABLE_ENTRY_IF_ALIAS 				   SNMP_OID_MIB2,31,1,1,1,18
#define ACS_APBM_SCX_OID_LAG_GEN_IF_TYPE 						   ACS_APBM_SNMP_OID_ERICSSON_GENERIC,2,1,1,2,1,1,2
#define ACS_APBM_SCX_OID_LAG_GEN_IF_ROW_STATUS                     ACS_APBM_SNMP_OID_ERICSSON_GENERIC,2,1,1,2,1,1,3
#define ACS_APBM_SCX_OID_LAG_PORT_ACTOR_ADMIN_KEY    			   SNMP_LAG_MIB,1,2,1,1,4
#define ACS_APBM_SCX_OID_LAG_PORT_MODE_OF_OPERATION  			   ACS_APBM_SNMP_OID_ERICSSON_GENERIC,2,1,2,0,6,1,2
#define ACS_APBM_SCX_OID_LAG_PORT_ACTOR_ADMIN_STATE				   SNMP_LAG_MIB,1,2,1,1,20
#define ACS_APBM_SCX_OID_LAG_DIST_ALGORITHM		                   ACS_APBM_SNMP_OID_ERICSSON_GENERIC,2,1,2,0,5,1,2
#define ACS_APBM_SCX_OID_LAG_PARTNER_PORT_ACTOR_STATE				   SNMP_LAG_MIB,1,2,1,1,23
#define ACS_APBM_SCX_OID_LAG_MODULE                                ACS_APBM_SNMP_OID_ERICSSON_GENERIC,2,1,2,0,1,0
#define SCX_LAG_FRONT_PORT1      27
#define SCX_LAG_FRONT_PORT2      28
#define SCX_LAG_VIRTUAL_PORT     37

// AXE-ETH-SHELF-MIB definitions for SCB-RP TRAPs
#define ACS_APBM_SCB_OID_TRAP_XSHMC_SHELF_MGR_CTRL					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 0, 1
#define ACS_APBM_SCB_OID_TRAP_XSHMC_BOARD_PRESENCE					ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 0, 2
#define ACS_APBM_SCB_OID_TRAP_XSHMC_SENSOR_STATE_CHANGE				ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 0, 3
#define ACS_APBM_SCB_OID_TRAP_XSHMC_SEL_ENTRY						ACS_APBM_SNMP_OID_ERICSSON_AXE, 2, 1, 0, 4

// AXE-ETH-SHELF-MIB definitions for SCX TRAPs
#define ACS_APBM_SCX_OID_TRAP_SHELF_MGR_CTRL						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 0, 1
#define ACS_APBM_SCX_OID_TRAP_BLADE_PRESENCE						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 0, 2
#define ACS_APBM_SCX_OID_TRAP_SENSOR_STATE_CHANGE					ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 0, 3
#define ACS_APBM_SCX_OID_TRAP_IPMI_UPGRADE_RESULT					ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 0, 7
#define ACS_APBM_SCX_OID_TRAP_SEL_ENTRY								ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 0, 9

// AXE-ETH-SHELF-MIB definitions for BSP TRAPs

#define ACS_APBM_BSP_OID_TRAP_BLADE_PRESENCE                                            ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 0, 0, 1
#define ACS_APBM_BSP_OID_TRAP_SENSOR_STATE_CHANGE                                       ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 0, 0, 2


#define ACS_APBM_BSP_OID_SHELF_ID                                                       ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 1, 2
#define ACS_APBM_BSP_OID_SLOT_POS                                                       ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 1, 3
#define ACS_APBM_BSP_OID_BLADE_ADMNSTATE                                                ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 2, 2, 1, 1, 3
#define ACS_APBM_BSP_OID_BLADE_OPERSTATE                                                ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 2, 2, 1, 1, 4
#define ACS_APBM_BSP_OID_BLADE_PRESENCESTATE                                            ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 2, 2, 1, 1, 30


#define ACS_APBM_BSP_OID_SENSOR_TYPE                                                    ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 0, 0, 2
#define ACS_APBM_BSP_OID_SENSOR_ID                                                      ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 2, 3, 1, 1, 1
#define ACS_APBM_BSP_OID_NOTIFICATION_ID                                                ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 1, 1
#define ACS_APBM_BSP_OID_SENSOR_TYPE_CODE                                               ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 2, 3, 1, 1, 2

#define ACS_APBM_BSP_OID_SENSOR_EVENT_DATA                                              ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 2, 3, 1, 1, 3
#define ACS_APBM_BSP_OID_TEMPERATURE_EVENT_DATA                                         ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 8, 1,11 

// GENERIC-SYSTEM_MANAGER-MIB OIDs for SCXB
#define ACS_APBM_SCX_OID_SYSTEM_TIME								ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 5, 5, 3
#define ACS_APBM_SCX_OID_NEXT_RESTART_TYPE   						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 5, 5, 4


// SNMPv2-MIB definitions for SCX TRAPS
#define ACS_APBM_SCX_OID_TRAP_COLD_START							SNMP_OID_SNMPMODULES, 1, 1, 5, 1
#define ACS_APBM_SCX_OID_TRAP_WARM_START   						ACS_APBM_SNMP_OID_ERICSSON_GENERIC, 2, 2, 5, 0, 4
//#define ACS_APBM_SCX_OID_TRAP_WARM_START							SNMP_OID_SNMPMODULES, 1, 1, 5, 2


// SNMP-TARGET-MIB definitions for SCX Target Objects used in trap subscription request
#define ACS_APBM_SCX_OID_TARGET_ADDR_TADDRESS						SNMP_OID_SNMPMODULES, 12, 1, 2, 1, 3
#define ACS_APBM_SCX_OID_TARGET_TIMEOUT								SNMP_OID_SNMPMODULES, 12, 1, 2, 1, 4
#define ACS_APBM_SCX_OID_TARGET_ADDR_TAG_LIST						SNMP_OID_SNMPMODULES, 12, 1, 2, 1, 6
#define ACS_APBM_SCX_OID_TARGET_ADDR_PARAMS							SNMP_OID_SNMPMODULES, 12, 1, 2, 1, 7
#define ACS_APBM_SCX_OID_TARGET_ADDR_ROW_STATUS						SNMP_OID_SNMPMODULES, 12, 1, 2, 1, 9

// APBM MIB objects types and constants
namespace acs_apbm_snmp {
	/** @brief SNMP Board Entry Field Sizes constants.
	 *
	 *	SnmpBoardDataFieldSizes enumeration detailed description
	 */
	enum SnmpBoardEntryFieldSizes {
		SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE					= 24,
		SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE	= 7,
		SNMP_BOARD_DATA_PRODUCT_NAME_SIZE						= 12,
		SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE					= 13,
		SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE				= 8,
		SNMP_BOARD_DATA_VENDOR_SIZE									= 31,

		SNMP_BOARD_DATA_TOTAL_SIZE = SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE +
																 SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE +
																 SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + SNMP_BOARD_DATA_VENDOR_SIZE,

		SNMP_BOARD_FATAL_EVENT_LOG_SIZE = 32,

		SNMP_BOARD_IPMI_DATA_FIRMWARE_TYPE_SIZE			= 1,
		SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE		= 25,
		SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE	= 8,
		SNMP_BOARD_IPMI_UPGRADE_LAST_TIME = 30,

		SNMP_BOARD_IPMI_DATA_TOTAL_SIZE = SNMP_BOARD_IPMI_DATA_FIRMWARE_TYPE_SIZE + SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE +
																			SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE,

		SNMP_SENSOR_EVENT_DATA_SIZE	= 3,
		SNMP_IPADDRESS_FIELD_SIZE = 4
	};

	enum SnmpShelfSlotPosConstants {
		MIN_SHELF_SLOT_POS_LEFT = 0,
		MAX_SHELF_SLOT_POS_LEFT = 12,
		MIN_SHELF_SLOT_POS_RIGHT = 13,
		MAX_SHELF_SLOT_POS_RIGHT = 25
	};

	typedef enum BladePwrStatusConstants {
		BLADE_PWR_OFF = 0,
		BLADE_PWR_ON = 1,
		BLADE_PWR_NOT_SUPPORTED = 2
	} bladePwr_status_t;

        typedef enum BladeLEDTypeConstants {
		BLADE_LED_TYPE_OLD = 0,
		BLADE_LED_TYPE_ON = 1,
		BLADE_LED_TYPE_NOT_SUPPORTED = 2
	} blade_led_type_t;

	typedef enum FrontPortStatusConstants {
                FRONT_PORT_UP = 1,
                FRONT_PORT_DOWN = 2,
                FRONT_PORT_TESTING = 3
	} frontPort_status_t;

	typedef enum LagEnableDisableParameter {
		        DISABLE_LAG = 0,
                ENABLE_LAG = 1,
	} lag_flag_type_t;

        typedef enum RestartTypeConstants {
                RESTART_TYPE_COLD = 0,
                RESTART_TYPE_WARM = 1,
        } restart_type_t;

	typedef enum NeighbourConstants {
			NEIGHBOUR_ABSENT = 0,
			NEIGHBOUR_PRESENT = 1
	} neighbour_t;

	namespace axe {
		namespace eth {
			namespace shelf {
				typedef int32_t mag_plug_number_t;
				typedef int32_t slot_pos_t;
				typedef int32_t sensor_id_t;
				typedef int32_t sensor_type_code_t;
				typedef uint8_t sensor_event_data_t [3];
				typedef uint8_t sel_entry_t [15];
				typedef char ipAddress_t [4];

				typedef enum ScbrpSlotPosConstants {
					SCBRP_SLOT_POS_LEFT = 0,
					SCBRP_SLOT_POS_RIGHT = 25
				} scbrp_slot_pos_t;

				typedef enum HwBoardPresenceConstants {
					HW_BOARD_PRESENCE_REMOVED = 0,
					HW_BOARD_PRESENCE_INSERTED = 1,
					HW_BOARD_PRESENCE_NOT_ACCESSIBLE = 2,
					HW_BOARD_PRESENCE_ACCESSIBLE = 3
				} hw_board_presence_t;

				typedef enum HwInstallationStatusConstants {
					UNKNOWN = 0,
					INSTALLED_AND_ACCEPTABLE = 1,
					INSTALLED_AND_NOT_ACCEPTABLE = 2,
					NOT_INSTALLED = 3
				} hw_installation_status_t;
                                       
				typedef enum BusTypeConstants {
					BUS_TYPE_UNDEFINED = -1,
					BUS_TYPE_IPMI = 0,
					BUS_TYPE_MBUS = 1
				} bus_type_t;

				typedef enum SensorTypeConstants {
					SENSOR_TYPE_TEMPERATURE = 1,
					SENSOR_TYPE_POWER_SUPPLY = 8,
					SENSOR_TYPE_FREQUENCY = 11,
					SENSOR_TYPE_SYSTEM_EVENT = 18
				} sensor_type_t;

				typedef enum ShelfMgrStateConstants {
					SHELF_MGR_STATE_PASSIVE = 0,
					SHELF_MGR_STATE_ACTIVE = 1,
					SHELF_MGR_STATE_IDLE = 2,
				} shelf_mgr_state_t;

				typedef enum XshmcShelfMgrCmdConstants {
					XSHMC_SHELF_MGR_CMD_NOT_SCAN = 0,
					XSHMC_SHELF_MGR_CMD_SCAN = 1
				} xshmc_shelf_mgr_cmd_t;

				typedef enum IpmiUpgradeStatusConstants {
					IPMI_UPGRADE_STATUS_UNDEFINED = -1,
					IPMI_UPGRADE_STATUS_READY = 0,
					IPMI_UPGRADE_STATUS_ONGOING = 1,
					IPMI_UPGRADE_STATUS_NO_SUCH_BOARD = 2,
					IPMI_UPGRADE_STATUS_OPEN_FILE_ERROR = 3,
					IPMI_UPGRADE_STATUS_FAIL = 4,
					IPMI_UPGRADE_STATUS_TFTP_ERROR = 5,
					IPMI_UPGRADE_STATUS_RESET_FAIL_IN_FB = 6,
					IPMI_UPGRADE_STATUS_RESET_FAIL_IN_UPG = 7,
					IPMI_UPGRADE_STATUS_TFTP_INTERNAL_ERROR = 8,
					IPMI_UPGRADE_STATUS_CHECKSUM_ERROR = 9,
					IPMI_UPGRADE_STATUS_NOT_STARTED = 12,
					IPMI_UPGRADE_STATUS_NOT_SUPPORTED = 13
				} ipmi_upgrade_status_t;

				typedef enum BiosBootRunModeConstants {
					BIOS_BOOT_RUN_MODE_UNDEFINED = -1,
					BIOS_BOOT_RUN_MODE_BIOS_1 = 0,
					BIOS_BOOT_RUN_MODE_BIOS_2 = 1
				} bios_boot_run_mode_t;

				typedef enum LedStatusConstants {
					LED_STATUS_OFF = 0,
					LED_STATUS_ON = 1,
					FOUR_LED_STATUS_SLOWBLINK = 2, // supported for FOUR 4 LEDS only
					FOUR_LED_STATUS_FASTBLINK = 3,// supported for FOUR 4 LEDS only
					LED_STATUS_NOT_AVAILABLE = 4
				} led_status_t;
               

				typedef enum LedTypeConstants{
					LED_TYPE_FAULT = 0,
					LED_TYPE_OPERATIONAL = 1,
					LED_TYPE_MAINTENANCE = 2,
					LED_TYPE_STATUS = 3
				} led_type_t;

				typedef enum LedColorConstants {
					LED_COLOR_RED = 0,
					LED_COLOR_GREEN = 1,
					LED_COLOR_YELLOW = 2,
					LED_COLOR_BLUE = 3
				} led_color_t;

				typedef enum LedInfoConstants {
					LED_INFO_NOT_SUPPORTED = 0,
					LED_INFO_SUPPORTED = 1
				} led_info_t;

				typedef enum NicNamesConstants{
					IMM_NIC_0 = 0,
					IMM_NIC_1 = 1,
					IMM_NIC_2 = 2,
					IMM_NIC_3 = 3,
					IMM_NIC_4 = 4,
					IMM_NIC_5 = 5, //ETH5 and ETH6 SUPPORT
					IMM_NIC_6 = 6, //ETH5 and ETH6 SUPPORT
					IMM_NIC_7 = 7,
					IMM_NIC_8 = 8,

					IMM_NIC_UNDEFINED = 999
				} nic_name_t;

				typedef enum NicTypeConstants {
					ETH_0  = IMM_NIC_0,
					ETH_1  = IMM_NIC_1,
					ETH_2  = IMM_NIC_2,
					ETH_3  = IMM_NIC_3,
					ETH_4  = IMM_NIC_4,
					MAX_ETH_GEP1 = ETH_4 + 1,
					MAX_ETH_GEP2 = MAX_ETH_GEP1,
					//gep5 support
					ETH_5  = IMM_NIC_5,
					ETH_6  = IMM_NIC_6,
					BOND_1 = 7,
					MAX_ETH_GEP5 = BOND_1 + 1,
					// vAPG support
					ETH_7 = IMM_NIC_7 + 1, /* the value 7 is already used for BOND_1 type */
					ETH_8 = IMM_NIC_8 + 1,
					MAX_ETH_VAPG = ETH_8 + 1,
					//gep 7 support
					//MAX_ETH_GEP7 = IMM_NIC_6,
					MAX_ETH_GEP7 = BOND_1 + 1, //HY47548
					ETH_UNDEFINED = IMM_NIC_UNDEFINED
				} nic_type_t;


				typedef enum NicStatusConstants {
					NIC_STATUS_UNDEFINED = 0,
					NIC_STATUS_CONNECTED = 1,
					NIC_STATUS_MEDIA_DISCONNECTED = 2
				} nic_status_t;

				typedef enum RaidStatusConstants {
					RAID_STATUS_UNDEFINED = 0,
					RAID_STATUS_ACTIVE = 1,
					RAID_STATUS_NOT_ACTIVE = 2
				} raid_status_t;

				typedef enum DiskStatusConstants {
					DISK_A = 0,
					DISK_B = 1,
				} disk_status_t;

				typedef enum PhyIdConstants{
					PHY_GEP1L = 0,
					PHY_GEP2R = 3,
					PHY_GEP1R = 4,
					PHY_GEP2L = 7
				}phyid_status_t;

				typedef enum HwSide {
					LEFT  = 0,
					RIGHT = 1,
				} hw_side_t;

				typedef enum DRBDStatusConstants {
                                        DRBD_STATUS_UNDEFINED = 0,
                                        DRBD_STATUS_UPTODATE = 1,
                                        DRBD_STATUS_FAILED = 2,
                                        DRBD_STATUS_CONNECTED = 3,
                                        DRBD_STATUS_DISK_LESS = 4,
                                        DRBD_STATUS_ATTACHING = 5,
                                        DRBD_STATUS_NEGOTIATING = 6,
                                        DRBD_STATUS_INCONSISTENT = 7,
                                        DRBD_STATUS_OUTDATED = 8,
                                        DRBD_STATUS_DU_UNKNOWN = 9,
                                        DRBD_STATUS_CONSISTENT = 10
                                } drbd_status_t;
				
				 typedef enum ThumbDriveStatusConstants {
                                        THUMBDRIVE_STATUS_UNDEFINED = 0,
                                        THUMBDRIVE_STATUS_AVAILABLE = 1,
                                        THUMBDRIVE_STATUS_NOT_AVAILABLE = 2
                                } thumbdrive_status_t;

				typedef enum BoardStatusConstants {
					BOARD_STATUS_UNDEFINED = -1,
					BOARD_STATUS_WORKING = 0,
					BOARD_STATUS_FAULTY = 1,
					BOARD_STATUS_MANUALLY_BLOCKED = 2,
					BOARD_STATUS_MISSING = 3,
					BOARD_STATUS_NOT_APPLICABLE = 4,
					BOARD_STATUS_NOT_CONFIGURED = 5
				} status_t;

				typedef enum AlarmInterfaceStatusConstants {
					ALARM_INTERFACE_STATUS_UNDEFINED = -1,
					ALARM_INTERFACE_STATUS_WORKING = 0,
					ALARM_INTERFACE_STATUS_FAULTY = 1
				} alarm_status_t;

				typedef enum TemperatureStatusConstants {
					TEMPERATURE_STATUS_UNDEFINED = -1,
					TEMPERATURE_STATUS_NORMAL_TEMPERATURE = 0,
					TEMPERATURE_STATUS_CRITICAL_TEMPERATURE = 1
				} temperature_status_t;
			}
		}
	}

	namespace dmx{
		namespace minortype {

			typedef enum BoardPresenceConstants {
				BLADE_INSERTED 	= 1001,
				BLADE_REMOVED 	= 1002,
				BLADE_LINK_UP	= 1018,
				BLADE_LINK_DOWN	= 1019

			} dmx_board_presence_t;

			typedef enum SensorStateChangeConstants {
				BLADE_TEMPERATURE_RISING 	= 1003,
				BLADE_TEMPERATURE_FAILING 	= 1004
			} dmx_sensor_state_t;

			typedef enum FirmwareUpgradeResultConstants {
				FIRMWARE_UPG_PROGRESS 	= 1005,
				FIRMWARE_UPG_COMPLETED 	= 1006,
				FIRMWARE_UPG_FAILED 	= 1007
			} dmx_fw_upg_result_t;

		}
	}

	namespace generic {
		namespace shelf {
			typedef enum LedStatusConstants {
				LED_STATUS_OFF = 0,
				LED_STATUS_ON = 1,
				LED_STATUS_NOT_AVAILABLE = 2
			} led_status_t;

			typedef enum ShelfMgrCtrlStateConstants {
				SHELF_MGR_CTRL_STATE_UNKNOWN = -1,
				SHELF_MGR_CTRL_STATE_IDLE = 0,
				SHELF_MGR_CTRL_STATE_PASSIVE = 1,
				SHELF_MGR_CTRL_STATE_PASSIVE_DEGRADED = 2,
				SHELF_MGR_CTRL_STATE_ACTIVE = 3,
				SHELF_MGR_CTRL_STATE_INVENTORY_FINISHED_IPMI = 4,
				SHELF_MGR_CTRL_STATE_INVENTORY_FINISHED_MBUS = 5,
			} shelf_mgr_state_t;
		}
	}

	namespace snmptarget {
		typedef enum AddrRowStatusConstants {
//			 ACTIVATE = 1,
			 CREATE_AND_GO = 4,
			 CREATE_AND_WAIT = 5,
			 DESTROY = 6
		} addr_row_status_t;
	}
}

#endif // HEADER_GUARD_FILE__acs_apbm_snmpsmcmibdefinitions
