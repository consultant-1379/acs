#ifndef BIOS_SET_HANDLER_H
#define BIOS_SET_HANDLER_H

/** @file bios_set.h
 *	@brief
 *	@author xassore
 *	@date 2011-11-11
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
 *	| R-001 | 2011-11-11| xassore       | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include "bios_set_common.h"
#include "bios_set_cmd.h"
#include <ACS_CS_API.h>
#include "acs_apbm_cmd_ironsidemanager.h"
class acs_apbm_ipmiapi_impl;

#define CONFIG_NETWORKS_SUPPORTED 2
#define IMMROOT "AxeFunctions"
#define NODEARCHITECTURE_ATTRNAME "nodeArchitecture"

namespace bios_set_handler_ns {
	enum internal_error_code {
		ERR_NO_ERROR = 0,
		ERR_IN_EXECUTION = - 1,
		ERR_CS_CREATE_HWC_INSTANCE = - 2,
		ERR_CS_CREATE_BOARD_SEARCH_INSTANCE = - 3,
		ERR_CS_GET_BOARD_ID = - 4,
		ERR_CS_NO_SWITCH_BOARDS = -5,
		ERR_CS_NO_APUB_BOARDS = -6,
		ERR_CS_GET_SLOT = -7 ,
		ERR_CS_GET_IP_ETH = -8,
		ERR_CS_GET_MAGAZINE = -9,
		ERR_LOCAL_MAGAZINE_NUMBER_EXEPECTED = -10,
		ERR_GETTING_NODE_ARCHTECTURE = -11,
		ERR_NO_ECHO= -12,
		ERR_IPMIAPI_NOT_EXIST = -13,
		ERR_IPMIAPI_NOT_INIZIALIZED = -14,
		ERR_INVALID_PARAMETER = -15,
		ERR_BOARD_NOT_FOUND = -16,
		ERR_UNEXPECTED_SYSTEM_CONFIGURATION = -17,
		ERR_CS_GET_FBN = -18,
	};
	enum bios_set_node_configuration {
		NOT_CBA_ARCHITECTURE = 0,
		SCX_CBA_ARCHITECTURE = 1,
		SMX_CBA_ARCHITECTURE = 4
	};
	enum bios_set_default_value{
	  NOT_NEEDED = -1000
	};
}


class bios_set_handler {
	//==============//
	// Constructors //
	//==============//
private:
	/** @brief acs_apbm_utils constructor
	 */
	//============//
	// Destructor //
	//============//
public:
	/** @brief bios_set_handler Destructor
	 */
	inline bios_set_handler ()
		: _bios_default_image(-1), _bios_pointer(-1), ipmi_api(NULL){ }

	~bios_set_handler ();

	int get_bios_pointer(int slot,/* char *magazine,*/ int & bios_pointer);
	int get_bios_image(int slot, /*char *magazine,*/ int & bios_image);
	int set_bios_pointer(int slot, /*char *magazine,*/ int bios_pointer);
	int set_bios_image(int slot, /*char *magazine, */int bios_image);
	int init_ipmi(const char* device_name);
	int initialize(bios_set_cmd * bs_cmd);
	int set_command_result(int err_code);
	int get_board_fbn_id(uint16_t & fbn_id, int slot, char * magazine = 0);


        int get_ironside_bios_default_image(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int &bios_image);

	int get_ironside_bios_pointer(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int &bios_pointer);

        int set_ironside_bios_default_image(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int bios_image);

        int set_ironside_bios_pointer(acs_apbm_cmd_ironsidemanager *ironsidemanager,char *magazine,int slot, int bios_pointer);

        int get_ironside_bios_pointer(int slot, char *magazine, int & bios_pointer);

        int get_ironside_bios_image(int slot, char *magazine, int & bios_image);

        int set_ironside_bios_image(int slot, char *magazine, int & bios_image);

        int set_ironside_bios_pointer(int slot, char *magazine, int & bios_pointer);

	int mag_addr_to_physical_addr(const char* magazine);
	
	//===========//
	// Functions //
	//===========//
public:
	bool isCBA;
		//===========//
	// Operators //
	//===========//
private:
	int send_set_ipmi_command(const char * ipmi_cmd, const char * opt, int value);
	int send_get_ipmi_command(const char * ipmi_cmd, const char * opt, int & value);
	int get_info_from_cs();
	int get_local_magazine_from_cs (uint32_t & magazine);
	int get_switch_boards_info_from_cs (char * magazine);
	int uint32_to_ip_format (char (& value_ip) [IPNADDRESS_MAXLEN], uint32_t value);
	int ip_format_to_uint32 (uint32_t & value, const char * value_ip);
	//========//
	// Fields //
	//========//
private:
	bios_set_cmd *_bs_command;
	int _bios_default_image;
	int _bios_pointer;
	int bios_image;
	int bios_value;
	int _cmd_exec_mode;
	shelfmgr_info _switch_boards[SHELFMGRBOARD_MAXNUM];
	acs_apbm_ipmiapi_impl * ipmi_api;
};

#endif
