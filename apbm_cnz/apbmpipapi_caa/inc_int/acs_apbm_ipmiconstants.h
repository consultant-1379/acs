#ifndef __IPMICODES_H__
#define __IPMICODES_H__

/** @file acs_apbm_ipmicodes.h
 *	@brief
 *	@author
 *	@date 2012-03-06
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
 *	+=======+============+==============+=====================================+int
 *	| R-001 | 2012-03-06 |   		 	| Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


/*  IPMI REQUEST/RESPONSE MESSAGE DATA SIZES */
#define SIZE_L3DATA_GETMACADDRESS_REQUEST    0
#define SIZE_L3DATA_GETMACADDRESS_RESPONSE  14

#define SIZE_L3DATA_GETPRODUCTID_REQUEST     1
#define SIZE_L3DATA_GETPRODUCTID_0_RESPONSE  34
#define SIZE_L3DATA_GETPRODUCTID_1_RESPONSE  35
#define SIZE_L3DATA_GETPRODUCTID_2_RESPONSE  33

#define SIZE_L3DATA_ENTERLEVEL_REQUEST    	1
#define SIZE_L3DATA_ENTERLEVEL_RESPONSE   	0

#define SIZE_L3DATA_PTABLE_REQUEST    		2
#define SIZE_L3DATA_PTABLE_RESPONSE        64

#define SIZE_L3DATA_GETIPMIFWINFO_REQUEST    0
#define SIZE_L3DATA_GETIPMIFWINFO_RESPONSE   2

#define SIZE_L3DATA_RESETSTATUS_REQUEST   1
#define SIZE_L3DATA_RESETSTATUS_RESPONSE  1

#define SIZE_L3DATA_FLASHMODEACTIVATE_REQUEST   1
#define SIZE_L3DATA_FLASHMODEACTIVATE_RESPONSE  2

#define SIZE_L3DATA_FLASHSREC_RESPONSE  1

#define SIZE_L3DATA_GETLED_REQUEST   1
#define SIZE_L3DATA_GETLED_RESPONSE  2

#define SIZE_L3DATA_SETLED_REQUEST   1
#define SIZE_L3DATA_SETLED_RESPONSE  0

#define SIZE_L3DATA_GETSENSORREADINGS_REQUEST	1
#define SIZE_L3DATA_GETSENSORREADINGS_RESPONSE	8

#define SIZE_L3DATA_REARM_PANEL_REQUEST	8
#define SIZE_L3DATA_REARM_PANEL_RESPONSE 1

#define SIZE_L3DATA_WATCHDOG_REQUEST 0
#define SIZE_L3DATA_WATCHDOG_RESPONSE 0

#define SIZE_L3DATA_IPMIRESTART_REQUEST   0
#define SIZE_L3DATA_IPMIRESTART_RESPONSE  0

#define SIZE_L3DATA_GETADDRESS_REQUEST   0
#define SIZE_L3DATA_GETADDRESS_RESPONSE  1

#define SIZE_L3DATA_GETIPMIHWVERSION_REQUEST  1
#define SIZE_L3DATA_GETIPMIHWVERSION_RESPONSE 2

#define SIZE_L3DATA_SETDEFAULTBIOS_REQUEST 1
#define SIZE_L3DATA_SETDEFAULTBIOS_RESPONSE 0

#define SIZE_L3DATA_GETDEFAULTBIOS_REQUEST 0
#define SIZE_L3DATA_GETDEFAULTBIOS_RESPONSE 1

#define SIZE_L3DATA_READBIOSPOINTER_REQUEST 0
#define SIZE_L3DATA_READBIOSPOINTER_RESPONSE 1

#define SIZE_L3DATA_SETBIOSPOINTER_REQUEST 1
#define SIZE_L3DATA_SETBIOSPOINTER_RESPONSE 0

#define SIZE_L3DATA_IPMIFWRESETSTATUS_COMMAND  1
#define SIZE_L3DATA_IPMIFWRESETSTATUS_RESPONSE 0

#define SIZE_L3DATA_IPMIFWERASEFINISHED_COMMAND  1
#define SIZE_L3DATA_IPMIFWERASEFINISHED_RESPONSE 0

/* IPMI TIMEOUT definitions  */
#define L3TIMEOUT_DEFAULT_MSEC      1000
#define L3TIMEOUT_FLASHREC_MSEC     3000
#define TIMEOUT_SELECT_MSEC         10

/* GROUP and COMMAND CODES */

#define G_SERVICE                   0     // 0x00
#define C_ENTER_LEVEL               2     // 0x02
#define C_LIFT_RESET                3     // 0x03
#define C_FLASH_MODE_ACTIVATE       4     // 0x04
#define C_FLASH_SREC                5     // 0x05
#define C_IPMI_RESTART              6     // 0x06
#define C_PTABLE_BLOCK              7     // 0x07
#define C_POWER_ONOFF               8     // 0x08
#define C_BLADE_FUNCTIONS           9     // 0x09
#define C_RESET                     10    // 0x0A
#define C_BUS_TYPE                  11    // 0x0B
#define C_SET_TIME                  12    // 0x0C
#define C_GET_TIME                  13    // 0x0D
#define C_SET_DEFAULT_BIOS_IMAGE    14    // 0x0E
#define C_GET_DEFAULT_BIOS_IMAGE    15    // 0x0F
#define C_READ_BIOS_POINTER         17    // 0x11
#define C_SET_BIOS_POINTER          10    // 0x0A
#define C_WRITE_BLADE_CONFIG        18    // 0x12
#define C_READ_BLADE_CONFIG         19    // 0x13
#define C_WRITE_BIOS_NVRAM          20    // 0x14
#define C_READ_BIOS_NVRAM           21    // 0x15
#define C_GET_BPID                  22    // 0x16
#define C_SET_BPID                  23    // 0x17
#define C_GET_SHELF_SLOT            24    // 0x18
#define C_SET_SHELF_SLOT            25    // 0x19
#define C_GET_RTFD                  26    // 0x1A
#define C_SET_RTFD                  27    // 0x1B
#define C_READ_UPTIME               28    // 0x1C
#define C_GET_IPMI_FW               29    // 0x1D
#define C_GET_POWER_REGION          30    // 0x1E
#define C_SET_POWER_REGION          31    // 0x1F
#define C_IPMI_COLDSTART_FLAG       32    // 0x20
#define C_WRITE_ATOMIC_CONFIG       33    // 0x21

#define G_INFO                      1     // 0x01
#define C_MAC_ADDRESS               0     // 0x00
#define C_PRODUCT_ID                1     // 0x01
#define C_IPMI_FW_INFO              2     // 0x02
#define C_RESET_STATUS              3     // 0x03
#define C_PIP_PROTOCOL_VERSION      4     // 0x04
#define C_IPMI_HW_VERSION           5     // 0x05
#define C_READ_IPMI_FW_ID           6     // 0x06
#define C_READ_BLADE_ADAPTATION     7     // 0x07
#define G_LED                       2     // 0x02
#define C_LED_GET                   0     // 0x00
#define C_LED_SET                   1     // 0x01
#define C_LED_RED_SET            	1     // 0x01
#define C_LED_YELLOW_SET            2     // 0x02
#define C_START_LED_TEST            3     // 0x03
#define C_4LED_SET                  4     // 0x04
#define C_4LED_GET                  5     // 0x05
#define C_GET_SENSOR_READING		4	  // 0x04
//GEP4 compatibility
#define C_4LED_RED_SET              4     // 0x04
#define C_4LED_GREEN_SET            5     // 0x05
#define C_4LED_BLUE_SET             6     // 0x06
#define C_4LED_YELLOW_SET           7     // 0x07

#define C_4LED_STATUS_OFF           0     // 0x00
#define C_4LED_STATUS_ON            255   // 0xFF
#define C_4LED_STATUS_FAST_BLINK    12    // 0x0C
#define C_4LED_STATUS_SLOW_BLINK    50    // 0x32
#define C_4LED_STATUS_BLINK_HOT     250   // 0xFA
#define C_4LED_STATUS_BLINK_COLD    10    // 0x0A

#define G_DIAG                      3     // 0x03
#define C_DIAG_INIT                 0     // 0x00
#define C_DIAG_CANCEL               1     // 0x01
#define C_DIAG_RESULT               2     // 0x02
#define C_SELFTEST_INIT             3     // 0x03
#define C_SELFTEST_RESULT           4     // 0x04

#define G_SEL                       4     // 0x04
#define C_SENSOR_ALARM              2     // 0x02
#define C_ALARM_SUBSCRIPTION        3     // 0x03
#define C_READ_SENSOR               4     // 0x04
#define C_REARM_ALARM               5     // 0x05
#define C_WRITE_GPR                 6     // 0x06
#define C_READ_GPR                  7     // 0x07
#define C_WRITE_FATAL_EVENT         8     // 0x08
#define C_READ_FATAL_EVENT_LOG_INFO 9     // 0x09
#define C_READ_FATAL_EVENT  		10    // 0x0A
#define C_CLEAR_FATAL_EVENT_LOG     11    // 0x0B
#define C_FATAL_EVENT_LOG_DELETED   12    // 0x0C
#define C_IPMI_FW_ERASE_FINISHED    14    // 0x0E

#define G_WDOG                      5     // 0x05
#define C_WATCHDOG                  0     // 0x00
#define C_PRESENCE                  1     // 0x01int

#define G_SHMC                      6     // 0x06
#define C_SET_ACTIVE                1     // 0x01
#define C_CLEAR_SCAN_TABLE          2     // 0x02
#define C_BLADE_PRESENCE            3     // 0x03
#define C_FW_RESET                  4     // 0x04

#define G_IPMI                      8     // 0x08
#define C_TO_FIRMWARE               0     // 0x00
#define C_TO_PAYLOAD                1     // 0x01

#define G_DEBUG                     10    // 0x0A
#define C_SWITCH_LED                1     // 0x01
#define C_GET_ADDRESS               3     // 0x03
#define C_STOP_WATCHDOG             4     // 0x04
#define C_PPCU_RESET_FUNC           5     // 0x05
#define C_POWER_CYCLE_BLADE         8     // 0x08
#define C_GET_IMC_LOAD              9     // 0x09
#define C_TEST_OFF                  10    // 0x0A
#define C_HW_SET_UP                 19    // 0x13
#define C_GET_FW_START_STRING       20    // 0x14
#define C_GET_SCANTABLE             22    // 0x16
#define C_CONTROL_1                 25    // 0x19
#define C_CONTROL_2                 26    // 0x1A
#define C_CONTROL_TEST              27    // 0x1B
#define C_LOCAL_PIP_EVENT           28    // 0x1C
#define C_TBC                       30    // 0x1E
#define C_CHECKSUM                  31    // 0x1F
#define C_READ_WDOG                 32    // 0x20

#define RESULT_OK                   0     // 0x00
#define RESULT_NOT_SUPPORTED        213   // 0xD5  
#define RESULT_NOT_OK               255   // 0xFF  


/* IPMI COMMAND/RESPONSE CODES IN NETFNS
 ============================================= */

/* Any change in this list should be followed by update of ipmiif_print() 
   in ipmiif.c file                                                       */

#define NETFN_SENSOR_DEVICE_CMD     0x04                                            
#define NETFN_SENSOR_DEVICE_RSP     0x05

#define CMD_EVENT_SENSOR            0x02
#define CMD_GET_SENSOR_READING      0x2D

#define NETFN_DEVICE_GLOBAL_CMD     0x06
#define NETFN_DEVICE_GLOBAL_RSP     0x07

#define CMD_GET_DEVICE_ID           0x01
#define CMD_GET_SELFTEST            0x04
#define CMD_GET_BLADE_INFO          0x70
#define CMD_HELLO                   0x34
#define CMD_FILE                    0x35

#define NETFN_FIRMWARE_CMD          0x08
#define NETFN_FIRMWARE_RSP          0x09

#define CMD_GET_FW_VER              0x01
#define CMD_WARM_RESET              0x03
#define CMD_ENABLE_MSG_CHANNEL      0x32
#define CMD_S_RECORD                0x34

#define NETFN_SEL_CMD               0x0A
#define NETFN_SEL_RSP               0x0B

#define CMD_GET_SEL_INFO            0x40
#define CMD_GET_SEL_ENTRY           0x43
#define CMD_CLR_SEL                 0x47
#define CMD_GET_SEL_TIME            0x48
#define CMD_SET_SEL_TIME            0x49

#define NETFN_ATCA_CMD              0x2C
#define NETFN_ATCA_RSP              0x2D

#define CMD_GET_PICMG_PROPERTIES    0x00
#define CMD_FRU_CONTROL             0x04
#define CMD_SET_LED_STATE           0x07
#define CMD_GET_LED_STATE           0x08
#define CMD_SET_POWER_LEVEL         0x11
#define CMD_GET_POWER_LEVEL         0x12

#define NETFN_CNTRL_SPEC_CMD        0x30
#define NETFN_CNTRL_SPEC_RSP        0x31

#define CMD_DIAGNOSTIC_INIT         0x01
#define CMD_DIAGNOSTIC_CANCEL       0x02
#define CMD_GET_DIAGNOSTIC_RES      0x03
#define CMD_GET_IPMI_FW_ID          0x04
#define CMD_START_EXT_SELFTEST      0x05
#define CMD_WRITE_GPR               0x0F
#define CMD_READ_GPR                0x10
#define CMD_READ_FEL_INFO           0x11
#define CMD_READ_FATAL_EVENT        0x12
#define CMD_CLEAR_FEL               0x13
#define CMD_FEL_DELETED             0x14
#define CMD_SEND_FATAL_EVENT        0x16
#define CMD_SET_DEFAULT_BIOS_IMAGE  0x18
#define CMD_GET_DEFAULT_BIOS_IMAGE  0x19
#define CMD_READ_BIOS_POINTER       0x21
#define CMD_READ_IPMI_PROT_VER      0x22
#define CMD_IPMI_ERASE_FIN          0x23
#define CMD_READ_PARAM_TABLES       0x24
#define CMD_IPMI_DEBUG              0x25
#define CMD_WRITE_BOARD_CONFIG      0x26
#define CMD_READ_BOARD_CONFIG       0x27
#define CMD_READ_BIOS_VERSION       0x29
#define CMD_WRITE_BPID              0x2A
#define CMD_READ_RTFD               0x2B
#define CMD_WRITE_RTFD              0x2C
#define CMD_READ_UPTIME             0x2D
#define CMD_GRACEFUL                0x2E
#define CMD_READ_BLADE_ADAPTATION   0x2F
#define CMD_TRANSFER_SYNC           0x32
#define CMD_WRITE_SOL               0x33
#define CMD_READ_SOL                0x34
#define CMD_WRITE_FRONTPORT         0x35
#define CMD_READ_FRONTPORT          0x36
#define CMD_REBOOT_FINISHED         0x37
#define CMD_WRITE_ATOMIC_CONFIG     0x38
#endif
