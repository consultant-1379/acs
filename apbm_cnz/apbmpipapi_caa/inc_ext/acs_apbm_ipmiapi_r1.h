#ifndef ACS_APBM_IPMIAPI_R1_H
#define ACS_APBM_IPMIAPI_R1_H


/** @file acs_apbm_ipmiapi_r1.h
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

#include "acs_apbm_ipmiapi_types.h"

class acs_apbm_ipmiapi_impl;

class acs_apbm_ipmiapi_r1
{
public:
	/** @brief acs_apbm_ipmiapi_r1 default constructor
	 *
	 *	acs_apbm_ipmiapi_r1 default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	acs_apbm_ipmiapi_r1();


	/** @brief ACS_DSD_Client destructor
	 *
	 *	acs_apbm_ipmiapi_r1 destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_apbm_ipmiapi_r1();


	/** @brief ipmiapi_init method
	 *
	 *	initializes the API
	 *
	 *	@param[in] device_name the device to use for communication.
	 *			If NULL value is passed, default device name is assumed: '/dev/eri_ipmi'.
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> API successfully initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE  	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_ALREADY_INITIALIZED 	--> API was already initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_DEVICE_NAME 	--> invalid device name
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OPEN_DEVICE			--> error opening device
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int ipmiapi_init (const char* device_name);


	/** @brief ipmiapi_init method
	 *
	 *	uninitializes the API releasing internal resources
	 *
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 	--> API successfully uninitialized
	 *	@remarks
	 */
	int ipmiapi_finalize ();


	/** @brief enter_level method
	 *
	 *	Store information in the IPMI firmware flash about the level of operation.
	 *
	 *	@param[in] level  the level of operation:  BOOTLOADER_OP_LEVEL = 0, OS_OP_LEVEL = 1, APPL_OP_LEVEL = 2
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE		--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'level' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int enter_level (unsigned int level);


	/** @brief get_mac_address method
	 *
	 *	Get information about MAC addresses stored in the IPMI firmware flash.
	 *
	 *	@param[out] mac_address_info container used to store MAC addresses information.
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE		--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'mac_address_info' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int get_mac_address (acs_apbm_ipmiapi_ns::mac_adddress_info* mac_address_info);


	/** @brief get_product_id method
	 *
	 *	Get PRODUCT identification info from the IPMI firmware flash.
	 *
	 *	@param[out] product_id container used to store PRODUCT identification info.
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE 	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'product_id' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int get_product_id (acs_apbm_ipmiapi_ns::product_id* product_id);


	/** @brief get_parameter_tableblock method
	 *
	 *	Get a block (64 bytes) from one of the parameter tables which have been flashed earlier using flash_srecord().
	 *
	 *	@param[in] flash_type specifies the table type: 00 = board table type, 01 = board running parameter table, 02 = sensor table, 03 = IPMI FW table.
	 *	@param[in] block_index is a zero-based block offset (one block is 64 bytes).
	 *	@param[out] buffer container used to store the returned data (64 bytes padded with 0xFF).
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE  			--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'flash_type' parameter or NULL buffer pointer
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int get_parameter_tableblock (unsigned int flash_type, unsigned int block_index, unsigned char* buffer);


	/** @brief get_ipmi_fwinfo method
	 *
	 *  Get IPMI firmware revision information
	 *
	 *	@param[out] ipmi_fwinfo container used to store the retrieved IPMI firmware information
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE  			--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'ipmi_fwinfo' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED       --> the operation required is not implemented
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY              --> resources are temporarily unavailable.
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int get_ipmi_fwinfo (acs_apbm_ipmiapi_ns::ipmi_fwinfo* ipmi_fwinfo);


	/** @brief get_reset_line_state method
	 *
	 *  Get status for a specific reset line
	 *
	 *  @param[in] line_index specifies the reset line
	 *	@param[out] state the state of the specified line; 0 = inactive, 1 = active.
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE  			--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'state' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED       --> the operation required is not implemented
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY              --> resources are temporarily unavailable.
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int get_reset_line_state (unsigned char line_index, unsigned char* state);


	/** @brief flash_mode_activate method
	 *
	 * It is used to prepare IPMI firmware for flashing to begin.
	 *
	 *  @param[in] flash_type specifies the table type 00 = board table type, 01 = board running parameter table, 02 = sensor table, 03 = IPMI FW.
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE  			--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'state' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED       --> the operation required is not implemented
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY              --> resources are temporarily unavailable.
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_EXECUTED        --> error in executing the required operation
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int flash_mode_activate (unsigned int flash_type);


	/** @brief flash_srecord method
	 *
	 * It is used to program binary data into the IPMI firmware flash.
	 * @param[in] data_record contains the binary data to be programmed. It is represented as a string of ascii characters, string may not be zeroterminated.
	 * each string is one Motorola S-RECORD such as: "S30A0000C315FFFFFFFFFF22".
	 * @param[in] record_size is the number of characters in data_record (example above has 24).
	 * @return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE  			--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'state' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED       --> the operation required is not implemented
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY              --> resources are temporarily unavailable.
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_EXECUTED        --> error in executing the required operation
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 * @remarks The binary data may be parameters or firmware.
	 * @remarks Before calling flash_srecord() one or several times, the type of data must be selected first by calling flash_mode_activate().
	 */
	int flash_srecord (const unsigned char* data_record, unsigned char record_size);


	/** @brief get_led method
	 *
	 * Get the current status of a specified LED.
	 *
	 * @param[in] led_type specifies the LED to get status for:  0=green, 1=yellow, 2=red.
	 * @param[out] led_info_record contains the status for the specified led_type
	 * @return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE  			--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'led_type' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 * @remarks The binary data may be parameters or firmware.
	 * @remarks Before calling flash_srecord() one or several times, the type of data must be selected first by calling flash_mode_activate().
	 */
	int get_led (unsigned int led_type, unsigned char* led_info_record);

        /** @brief set_led method for GEP4 Hardware compatibility
         *
         *      Set the status of the LED
         *
         *      @param[in] status  the status to be set: 0 = off, 1 = ON.
         *      @return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS                                  --> Success
         *                      acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY                              --> Node busy (IPMI command processing resources temporary unavailable)
         *                      acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED               --> Operation not supported
         *                      acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE     --> No enough resources are available to complete operation (memory or other resources)
         *                      acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED                --> API is not initialized
         *                      acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER              --> invalid 'status' parameter
         *                      acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC                                --> API internal error
         *      @remarks
         */
        int set_led (acs_apbm_ipmiapi_ns::led_type_t led_type, int status);

	/** @brief set_redled method
	 *
	 *	Set the status of the red LED
	 *
	 *	@param[in] status  the status to be set: 0 = off, 1 = ON.
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE 	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'status' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int set_redled ( int status);


	/** @brief set_yellowled method
	 *
	 *	Set the status of the yellow LED
	 *
	 *	@param[in] status  the status to be set:  0 = off, 1 = ON.
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE 	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'status' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int set_yellowled (int status);


	/** @brief get_ext_alarms  method
	 *
	 *	Get the states of the external alarms sensors.
	 *
	 *	@param[in] sensor_ndex indicates which sensor to get states from.
	 *			   The possible values are :
	 *			   		acs_apbm_ipmiapi_ns::SENSOR_HANDLING_FIRST_32_GEA_CONNECTORS 	-->	(sensor handling the 32 first connectors)
	 *			   		acs_apbm_ipmiapi_ns::SENSOR_HANDLING_FOLLOWING_32_GEA_CONNECTORS --> (sensor handling the 32 following connectors)
	 *	@param[out] sensor_readings used to store sensor states
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE 	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid input parameter ('sensor_index' or 'sensor_readings')
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int get_ext_alarms (acs_apbm_ipmiapi_ns::sensor_index sensor_index, acs_apbm_ipmiapi_ns::sensor_readings* sensor_readings);


	/** @brief set_alarm_panel method
	 *
	 *	Set the status of the lamps and bell of the alarm panel.
	 *
	 *	@param[in] alarm_panel_values contains the values to be sent to the alarm panel
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *	 		acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_EXECUTED		--> Operation not executed by IPMI firmware
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE 	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'alarm_panel_values' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int set_alarm_panel (const acs_apbm_ipmiapi_ns::alarm_panel_values* alarm_panel_values);


	/** @brief presence_ack method
	 *
	 *	Send presence acknowledgement with watchdog interval if watchdog function is enabled
	 *
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *	 		acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE 	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int presence_ack();


	/** @brief ipmi_restart method
	 *
	 *	Restart IPMI firmware. Used after parameter/sensor table update
	 *
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *	 		acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE 	--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int ipmi_restart();


	/** @brief get_board_address method
	 *
	 *	Get the board address of this board on the IPMI bus.
	 *
	 *	@param[out] board_address container used to store board address (i.e. the slot position)
	 *
	 *	@return acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS 					--> Success
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NODE_BUSY				--> Node busy (IPMI command processing resources temporary unavailable)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_OP_NOT_SUPPORTED		--> Operation not supported
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NO_SYSTEM_RESOURCE		--> No enough resources are available to complete operation (memory or other resources)
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_NOT_INITIALIZED		--> API is not initialized
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_INVALID_PARAMETER 		--> invalid 'board_address' parameter
	 *			acs_apbm_ipmiapi_ns::IPMIAPI_ERR_GENERIC				--> API internal error
	 *	@remarks
	 */
	int get_board_address(unsigned int* board_address);


	/// Returns the last error occurred interacting with IPMI device
	int last_impl_error();

private:
	acs_apbm_ipmiapi_ns::error_code_t _error_code(int impl_err);

private:
	acs_apbm_ipmiapi_impl * _ipmiapi_impl;
	int _last_impl_error;
};

#endif
