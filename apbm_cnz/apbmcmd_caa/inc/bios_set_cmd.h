#ifndef BIOS_SET_CMD_H
#define BIOS_SET_CMD_H

/** @file bios_set_cmd.h
 *	@brief
 *	@author xassore
 *	@date 2011-11-15
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
 *	| R-001 | 2011-11-15 | xassore      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>


#define CMD_OPTIONS  "mI:iP:pC:B:"
#define COM_PORT_NAME_MAXLEN 32
#define MAGAZINENAME_MAXLEN 16
#define B_OPTION_VALUE_MAXLEN 32
#define DEFAULT_COM_PORT  "COM2"


////eri-ipmitool needed for command
//#define IPMI_CMD "eri-ipmitool"
//#define IPMI_OPT_GET_BIOS_IMAGE "gbios"
//#define IPMI_OPT_GET_BIOS_POINTER "rbiosp"
//#define IPMI_OPT_SET_BIOS_IMAGE "sbios"
//#define IPMI_OPT_SET_BIOS_POINTER "sbiosp"
#define DEFAULF_DEVICE "/dev/eri_ipmi"
#define COM2 "COM2"
#define COM2_DEVICE "/dev/eri_ipmi"
#define COM3 "COM3"
#define COM3_DEVICE "/dev/ttyUSB0"

#define IPMI_CMD_MAX_SIZE 64


namespace bios_set_ns {
	enum CommandExitCodeConstants {
		BS_NO_ERROR= 0,
		BS_GENERIC_ERROR = 1,
		BS_INCORRECT_USAGE = 2,
		BS_ILLEGAL_VALUE_FOR_BIOS_POINTER = 3,
		BS_ILLEGAL_VALUE_FOR_BIOS_IMAGE = 4,
		BS_MISSING_VALUE_FOR_BIOS_POINTER = 5,
		BS_MISSING_VALUE_FOR_BIOS_IMAGE = 6,
		BS_ILLEGAL_CHOICE = 7,
		BS_CANNOT_GET_INFORMATION_FROM_CS = 8,
		BS_UNABLE_TO_CONNECT_TO_CS = 9,
		BS_UNABLE_GET_SCXB_IP_FROM_CS = 10,
		BS_SNMP_ERR_IN_SENDING_REQUEST = 11,
		BS_NO_RESPONSE_FROM_SNMP_AGENT = 12,
		BS_SNMP_BAD_RESPONSE_RECEIVED = 13,
		BS_SNMP_GENERIC_FAILURE = 14,
		BS_SNMP_RESOURCEUNAVAILABLE_ERR = 15,
		BS_SNMP_SET_UNAVAIL_ERR = 16,
		BS_ERR_SNMP_GETTING_IPMI_MASTER = 17,
		BS_ILLEGAL_VALUE_FOR_SLOT_POS = 18,
		BS_ILLEGAL_VALUE_FOR_MAGAZINE = 19,
		BS_MISSING_VALUE_FOR_SLOT_PARAMETER = 20,
		BS_MISSING_VALUE_FOR_COM_PORT_PARAMETER = 21,
		BS_ERR_UNKNOWN = 22,
		BS_NO_ECHO = 23,
		ILLEGAL_OPT_IN_SYSTEM_CONFIG = 24,
		BS_OP_NOT_PERMITTED = 25,
		BS_ERR_GETTING_NODE_ARCHITECTURE = 26,
		BS_BOARD_NOT_FOUND = 30,
		BS_ILLEGAL_COMMAND = 31,
		BS_IRONSIDE_FAILURE = 32
	};
	enum CommandFunction {
		GETBIOSPOINTER = 1,
		GETBIOSIMAGE = 2,
		SETBIOSPOINTER = 3,
		SETBIOSIMAGE = 4 ,
		GETVERSION = 5
	};
	enum CommandExecution {
		SNMP_MODE =1,
		COM_MODE = 2,
                IRONSIDE_MODE = 3
	};
        enum EnvironmentType {  //environment enum
                UNKNOWN                 = 0,
                SINGLECP_NOTCBA         = 1,
                SINGLECP_CBA            = 2,
                MULTIPLECP_NOTCBA       = 3,
                MULTIPLECP_CBA          = 4,
                SINGLECP_DMX            = 5,
                MULTIPLECP_DMX          = 6,
		DMX			= 7,
		VIRTUALIZED		= 8,
		SINGLECP_SMX		= 9,
		MULTIPLECP_SMX		= 10
	};

        EnvironmentType getEnvironment(void);

}


class bios_set_cmd {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief bios_set_cmd constructor
	 */
	 bios_set_cmd (int argc, char * const argv []);

private:

	//============//
	// Destructor //
	//============//
public:
	/** @brief bios_set_cmd Destructor
	 */
	inline ~bios_set_cmd () {BIOS_SET_CLOSE_LOG_FILE();}

	//=================//
	// Field Accessors //
	//=================//
public:
	inline int argc () const { return _argc; }

	inline const char * const * argv () const { return _argv; }
	inline  int bios_image() const {return _bios_default_image;}
	inline  void bios_image(int bi_value){_bios_default_image = bi_value;}
	inline  int bios_pointer() const {return _bios_pointer;}
	inline  void bios_pointer(int bp_value){_bios_pointer = bp_value;}
	inline const char * com_port() const {return _com_port;}
	inline  void com_port(char * cp_value){::strncpy(_com_port, cp_value, COM_PORT_NAME_MAXLEN);}
	inline  int  cmd_execution_mode(){return _cmd_execution_mode ;}
	inline char *magazine() {return _magazine;}

	//inline void error_code(int error){_error_code = error;}
	//inline int error_code(void) const {return _error_code;}
	const char * error_str(int err_code);
	//===========//
	// Functions //
	//===========//
public:
	int printmenu();
	int execute();
	void printUsage(void)const;
	int parse ();

	//===========//
	// Operators //
	//===========//
private:
	bios_set_cmd & operator= (const bios_set_cmd & rhs);
	//int verify_mag_and_slot_paramater(char *mag_slot, char *magazine, int & slot, int &error_code);
	int verify_mag_and_slot_paramater(char * mag_slot, char  magazine[MAGAZINENAME_MAXLEN], int & slot, int &error_code);
	int check_slot_paramater (char * slot_str, int &slot_pos);
	int check_magazine_parameter(char* magazine_name);
	int readChoice();
	//========//
	// Fields //
	//========//
private:
	int _argc;
	char * const * _argv;
	int _bios_default_image;
	int _bios_pointer;
	int _slotnum;
	char _com_port[COM_PORT_NAME_MAXLEN];
//	int _error_code;
	char _magazine[MAGAZINENAME_MAXLEN];
    int  _bios_set_function;
    int  _cmd_execution_mode;

};

#endif // HEADER_GUARD_CLASS__acs_apbm_cmdoptionparser
