#ifndef ACS_APBM_IPMIAPI_TYPES_H
#define ACS_APBM_IPMIAPI_TYPES_H


/** @file acs_apbm_ipmiapi_types.h
 *	@brief
 *	@author
 *	@date 2012-03-06
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized start_scb_communicationin writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-03-06 |   		 	| Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 *	| R-002 | 2012-06-06 |   		 	| sensor index values changed     	  |
 *	+=======+============+==============+=====================================+
 */

namespace acs_apbm_ipmiapi_ns {

	enum ipmi_operation_level {
		BOOTLOADER_OP_LEVEL     = 0,
		OS_OP_LEVEL             = 1,
		APPL_OP_LEVEL 			= 2
	};

	enum parameter_table_type {
		BOARD_TYPE_PARAMS_TABLE 	= 0,
		BOARD_RUNNING_PARAMS_TABLE  = 1,
		BOARD_SENSOR_TABLE          = 2,
		BOARD_IPMI_FW_TABLE         = 3
	};

	enum led_type_t {
		GREEN_LED 	= 0,
		YELLOW_LED 	= 1,
		RED_LED 	= 2,

                RED_4LED 	= 4,
		GREEN_4LED 	= 5,
		BLUE_4LED 	= 6, //Fourth LED compatibility for new hardware GEP4
		YELLOW_4LED 	= 7
	};
        
        
	enum mac_addres_info_fields_size {
		 MACADDRESS_SIZE = 6
	};

	struct mac_adddress_info {
		unsigned char boot_mac[MACADDRESS_SIZE]; 	// bytes mac = 00:01:02:03:04:05
		unsigned char eri_mac[MACADDRESS_SIZE];  	// bytes mac = 00:01:02:03:04:05
		unsigned short num;       					// number of consecutive eriMac
	};

	enum product_id_fields_size {
		PRODUCT_NUMBER_SIZE			=	25,
		PRODUCT_REVISION_SIZE		=	8,
		PRODUCT_NAME_SIZE			=	13,
		PRODUCT_SERIAL_NUMBER_SIZE	=	13,
		PRODUCT_DATE_SIZE			=	8,
		PRODUCT_VENDOR_NAME_SIZE	=	32
	};

	struct product_id {
		char product_number[PRODUCT_NUMBER_SIZE + 1];  			// ascii, ex: "ROJ 208 362/1"
		char product_revision[PRODUCT_REVISION_SIZE + 1]; 		// ascii, ex: "R1A" or "R99Z/99"
		char product_name[PRODUCT_NAME_SIZE + 1];    			// ascii, ex: "MXB3"
		char product_date[PRODUCT_DATE_SIZE + 1];    			// ascii, ex: "20051224"
		char serial_number[PRODUCT_SERIAL_NUMBER_SIZE + 1];  	// ascii, ex: "P1BA063121284"
		char vendor_name[PRODUCT_VENDOR_NAME_SIZE + 1];     	// ascii, ex: "Ericsson AB"
	};

	struct ipmi_fwinfo {
		unsigned char rev_major; // revision, major number, e.g. 3 in 3.6
		unsigned char rev_minor; // revision, minor number, e.g. 6 in 3.6
	};


	enum sensor_readings_fields_size {
		SENSOR_VALUES_SIZE	= 8
	};

	struct sensor_readings {
		unsigned char sensor_values[SENSOR_VALUES_SIZE];
	};

	enum alarm_panel_values_fields_size {
		PANEL_VALUES_SIZE	=	7
	};

	struct alarm_panel_values {
		unsigned char panel_type;
		unsigned char panel_values[PANEL_VALUES_SIZE];
	};

	enum sensor_index {
		SENSOR_HANDLING_FIRST_32_GEA_CONNECTORS = 0x37,
		SENSOR_HANDLING_FOLLOWING_32_GEA_CONNECTORS = 0x38,
		SENSOR_INDEX_UNDEFINED = -1
	};

	enum error_code_t {
		IPMIAPI_NO_ERRORS					=	0,
		IPMIAPI_ERR_GENERIC					=	-1,
		IPMIAPI_ERR_OP_NOT_SUPPORTED		=	-2,
		IPMIAPI_ERR_OP_NOT_EXECUTED			= 	-3,
		IPMIAPI_ERR_NODE_BUSY				=	-4,
		IPMIAPI_ERR_NOT_INITIALIZED			=	-5,
		IPMIAPI_ERR_ALREADY_INITIALIZED		=	-6,
		IPMIAPI_ERR_INVALID_DEVICE_NAME		=	-7,
		IPMIAPI_ERR_OPEN_DEVICE				=	-8,
		IPMIAPI_ERR_INVALID_PARAMETER		=	-9,
		IPMIAPI_ERR_NO_SYSTEM_RESOURCE		=	-10,
		IPMIAPI_ERR_TEMP_FAILURE			=	-11,
		IPMIAPI_ERR_UNEXPECTED_OP_RESPONSE	=	-12
	};
}
#endif
