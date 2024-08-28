#ifndef ACS_APBM_IPMIAPI_IMPL_H
#define ACS_APBM_IPMIAPI_IMPL_H


/** @file acs_apbm_ipmiapi_impl.h
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
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-03-06 |   		 	| Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include "acs_apbm_ipmiapi_common.h"
#include "acs_apbm_ipmiapi_types.h"
#include "acs_apbm_ipmiapi_level2_impl.h"
#include "ace/Mutex.h"


// TRs - HU25795 & HU57639
#define APBMPIP_LIB_PROCESS_MUTEX_NAME 				ACE_TEXT("apbmpip_lib.process_mutex")

class acs_apbm_ipmiapi_impl {

public:
	acs_apbm_ipmiapi_impl();

	virtual ~acs_apbm_ipmiapi_impl();

	int ipmiapi_init(const char* device_name);

	int ipmiapi_finalize();

	int enter_level (unsigned int level);

	int get_mac_address (acs_apbm_ipmiapi_ns::mac_adddress_info* mac_address_info);

	int get_product_id (acs_apbm_ipmiapi_ns::product_id* product_id);

	int get_parameter_tableblock (unsigned int flash_type, unsigned int block_index, unsigned char* buffer);

	int get_ipmi_fwinfo (acs_apbm_ipmiapi_ns::ipmi_fwinfo* ipmi_fwinfo);

	int get_reset_line_state (unsigned char line_index, unsigned char* state);

	int flash_mode_activate (unsigned int flash_type);

	int flash_srecord (const unsigned char* data_record, unsigned char record_size);

	int flash_s0record (const unsigned char* data_record , unsigned char record_size);

	int get_led (unsigned int led_type, unsigned char* led_info_record);

	int set_led(acs_apbm_ipmiapi_ns::led_type_t led_type, int status) { return _set_led(led_type, status); }//GEP4

	int set_redled ( int status) { return _set_led(acs_apbm_ipmiapi_ns::RED_LED, status); }

	int set_yellowled (int status) { return _set_led(acs_apbm_ipmiapi_ns::YELLOW_LED, status); }

	int get_ext_alarms (acs_apbm_ipmiapi_ns::sensor_index sensor_index, acs_apbm_ipmiapi_ns::sensor_readings* sensor_readings);

	int set_alarm_panel (const acs_apbm_ipmiapi_ns::alarm_panel_values* alarm_panel_values);

	int presence_ack();

	int ipmi_restart();

	int get_board_address(unsigned int* board_address);

	//internal methods
	int get_ipmi_hardware_version(const unsigned char hw_block, unsigned char* device_rev);

	int get_default_BIOS_pointer(unsigned int * bios_area);

	int get_default_BIOS_image(unsigned int* bios_image);

	int set_default_BIOS_image(unsigned int bios_image);

	int set_default_BIOS_pointer(unsigned int bios_pointer);

	int ipmi_firmware_reset_status(unsigned char* fwType);

	int ipmi_firmware_erase_finished(unsigned char* chState);

	int upgrade_ipmi_firmvare(std::string imagePath);

	int get_pip_protocol_version(unsigned char* ipmi_fw);

	int get_ipmi_fwinfo (unsigned char * fw, const unsigned int fw_type );

	int ipmi_restart(const unsigned char fw);

// private methods
private:

	int open_ipmi_driver(const char *ipmi_device_name);

	int _ipmifw_query (unsigned int groupcode, unsigned int commandcode, unsigned int req_l3data_size,
	                   const unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data,
	                   unsigned int l3timeout_msec);

	int _ipmifw_query_on_serial_port(unsigned int groupcode, unsigned int commandcode, unsigned int req_l3data_size,
            const unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data,
            unsigned int l3timeout_msec);

	int _ipmifw_query_on_driver(unsigned int groupcode, unsigned int commandcode, unsigned int req_l3data_size,
            const unsigned char* req_l3data, unsigned int res_l3data_size, unsigned char* res_l3data,
            unsigned int l3timeout_msec);

	int _ipmi_command(const unsigned int groupcode, const unsigned int commandcode, const unsigned int cmdL3DataSize,
							   unsigned char* cmdL3Data, unsigned int resL3DataSize, unsigned char* resL3Data);

	int _ipmi_command_on_serial_port(const unsigned int groupcode, const unsigned int commandcode, const unsigned int cmdL3DataSize,
								   unsigned char* cmdL3Data, unsigned int resL3DataSize, unsigned char* resL3Data);

	int _ipmi_command_on_driver(const unsigned int groupcode, const unsigned int commandcode, const unsigned int cmdL3DataSize,
								   unsigned char* cmdL3Data, unsigned int resL3DataSize, unsigned char* resL3Data);

	int _convert_ipmifw_result(char code);

	int _set_led(acs_apbm_ipmiapi_ns::led_type_t, int status);

	bool _lock_device();
	inline void  _unlock_device() {_api_mutex.release();}

	//start of TRs - HU25795 & HU57639
	int init_process_mutex();

	int acquire_process_mutex();

	int release_process_mutex();

	int check_and_adjust_system_V_sem();
	//end of TRs - HU25795 & HU57639

	acs_apbm_ipmiapi_ns::error_code_t _error_code(acs_apbm_ipmiapi_ns::ErrorConstants err);

//private attributes
private:
	ACE_Mutex _api_mutex;
	acs_apbm_ipmiapi_ns::ipmiapistate_t _ipmiapi_state;
	acs_apbm_ipmiapi_ns::communication_modes_t _communication_mode;
	acs_apbm_ipmiapi_level2_impl   *_l2_impl;
	int _ipmi_fd;
	int _epoll_fd;
};

#endif
