/** @file hwiconf.cpp
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
 *	+=====+============+==============+=====================================+
 *	| REV | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=====+============+==============+=====================================+
 *	| 001 | 2011-11-11 | xassore      | File created.                       |
 *	+=====+============+==============+=====================================+
 */

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <iostream>
#include <arpa/inet.h>
#include <ACS_CS_API.h>
#include "bios_set_common.h"
#include <bios_set_handler.h>
#include <bios_set_cmd.h>
// main program
int main(int argc, char* argv[])
{
	int cmd_exit_code = bios_set_ns::BS_NO_ERROR;

	bios_set_cmd cmd(argc, argv);

	BIOS_SET_OPEN_LOG_FILE();
	int ret_code;
	if (( ret_code = cmd.parse()) != 0) { //Error on parsing command line
		BIOSSET_LOG("ERROR - cmd.parse failed ! ret_code == %d\n", ret_code);
		printf ("%s\n", cmd.error_str(ret_code));
		if (ret_code == bios_set_ns::BS_INCORRECT_USAGE)
			cmd.printUsage();
		return ret_code;
	}
	cmd_exit_code = cmd.execute();
	printf ("%s\n", cmd.error_str(cmd_exit_code));
	return cmd_exit_code ;

}

bios_set_cmd::bios_set_cmd(int argc, char * const argv [])
{
	_argc = argc;
	_argv = argv;
	_bios_default_image = -1;
	_bios_pointer = -1;
	_slotnum = -1;
	_cmd_execution_mode = bios_set_ns::COM_MODE;
	strcpy(_com_port, DEFAULF_DEVICE);
	*_magazine =0;
        _bios_set_function= 0; 
}


int bios_set_cmd::parse()
{
	bios_set_ns::EnvironmentType m_environment;
        m_environment = bios_set_ns::getEnvironment();
	if(m_environment == bios_set_ns::VIRTUALIZED)
        {
        	return bios_set_ns::BS_ILLEGAL_COMMAND;
        }

	int opt_m = 0; // menu
	int opt_i = 0; // Get BIOS image
	int opt_I = 0; // Set BIOS image
	int opt_p = 0; // Get BIOS Pointer
	int opt_P = 0; // Set BIOS Pointer
	int opt_C = 0; // Set COM port
	int opt_B = 0; // MAGADDR and/or board slot numgetopt
	char *optvalue = NULL;
	int option;

	// set the default error code
	//error_code(bios_set_ns::BS_INCORRECT_USAGE);

	if ( _argc < 2 ) {   // Too few arguments
		return bios_set_ns::BS_INCORRECT_USAGE;
	}

	opterr = 0; // Preventing the print of a system error message
	int actual_options = 1;

	while ( (option = getopt(_argc, _argv, CMD_OPTIONS)) != -1 ) {
		//analyze cmd line and set flags
		switch (option){
			case 'm':
				if( opt_m ){
					return bios_set_ns::BS_INCORRECT_USAGE;
				}
				opt_m ++;
				actual_options++;
			break;
			case 'p':
				if( opt_p || opt_m){
					return bios_set_ns::BS_INCORRECT_USAGE;
				}
				 _bios_set_function = bios_set_ns::GETBIOSPOINTER;
				opt_p ++;
				actual_options++;
			break;
			case 'i':
				if( opt_i || opt_m ){
					return bios_set_ns::BS_INCORRECT_USAGE;
				}
				 _bios_set_function = bios_set_ns::GETBIOSIMAGE;
				opt_i ++;
				actual_options++;
			break;
			case 'P':
				if( opt_P || opt_m){
					return bios_set_ns::BS_INCORRECT_USAGE;
				}
				opt_P++;
				actual_options += 2;

				//Missing spaces after options
				if (actual_options != optind)
					return bios_set_ns::BS_INCORRECT_USAGE;

				optvalue = optarg;
				BIOSSET_LOG("DEBUG - bios_set_cmd::parse(): option P received with bios_pointer value  == %c\n\n", *optvalue);
				if((strcmp(optvalue, "0") == 0) || (strcmp(optvalue, "1") == 0)){
					 _bios_pointer = ::atoi(optvalue);
					 _bios_set_function = bios_set_ns::SETBIOSPOINTER;
				}
				else  return ((*optvalue == '-')? bios_set_ns::BS_MISSING_VALUE_FOR_BIOS_POINTER : bios_set_ns::BS_ILLEGAL_VALUE_FOR_BIOS_POINTER);

			break;
			case 'I':
				if( opt_I || opt_m){
					return bios_set_ns::BS_INCORRECT_USAGE;
				}
				opt_I++;
				actual_options += 2;

				//Missing spaces after options
				if (actual_options != optind)
					return bios_set_ns::BS_INCORRECT_USAGE;

				optvalue = optarg;
				if((strcmp(optvalue, "0") == 0) || (strcmp(optvalue, "1") == 0)){
					_bios_default_image = ::atoi(optvalue);
					_bios_set_function = bios_set_ns::SETBIOSIMAGE;
				}
				else return ((*optvalue == '-')? bios_set_ns::BS_MISSING_VALUE_FOR_BIOS_IMAGE :  bios_set_ns::BS_ILLEGAL_VALUE_FOR_BIOS_IMAGE);
			break;
			case 'C':
				if( opt_C)
					return bios_set_ns::BS_INCORRECT_USAGE;

				opt_C++;
				actual_options += 2;

				//Missing spaces after options
				if (actual_options != optind)
					return bios_set_ns::BS_INCORRECT_USAGE;

				optvalue = optarg;
				if (strcmp(optvalue, COM2) == 0)
					strncpy(_com_port, COM2_DEVICE, COM_PORT_NAME_MAXLEN);
				else
					return bios_set_ns::BS_INCORRECT_USAGE; //accepted only COM2

				//strncpy(_com_port, optvalue, COM_PORT_NAME_MAXLEN);
				if (*optvalue == '-') return bios_set_ns::BS_MISSING_VALUE_FOR_COM_PORT_PARAMETER;
				break;
			case 'B':
			{
				if( opt_B)
					return bios_set_ns::BS_INCORRECT_USAGE;
				opt_B++;
				actual_options += 2;
				optvalue = optarg;

				//Missing spaces after options
				if (actual_options != optind)
					return bios_set_ns::BS_INCORRECT_USAGE;

				char mag_slot[B_OPTION_VALUE_MAXLEN]={0};
				char magazine_str[MAGAZINENAME_MAXLEN]={0};
				int slot_num, error_code;
				strncpy(mag_slot, optvalue, B_OPTION_VALUE_MAXLEN);
				BIOSSET_LOG("DEBUG - bios_set_cmd::parse(): option B received with optvalue == %s\n\n", mag_slot);
				if (const int ret_code = verify_mag_and_slot_paramater(mag_slot, magazine_str, slot_num, error_code)){
					BIOSSET_LOG("ERROR - bios_set_cmd::parse(): verify_mag_and_slot_paramater  failed  == %d\n\n", error_code);
					return error_code;
				}
				_slotnum = slot_num;
				strncpy(this->_magazine, magazine_str, MAGAZINENAME_MAXLEN);
				BIOSSET_LOG("DEBUG - bios_set_cmd::parse(): option B received with magazine == %s  slot == %d\n\n", magazine_str, slot_num);

				if(m_environment == bios_set_ns::SINGLECP_DMX || m_environment == bios_set_ns::MULTIPLECP_DMX)
				{
					_cmd_execution_mode = bios_set_ns::IRONSIDE_MODE;
				}
				else
				{
					_cmd_execution_mode = bios_set_ns::SNMP_MODE;
				}
			}
			break;
//			case 'v':
//				_bios_set_function = bios_set_ns::GETVERSION;
//				return 0;
	        case '?':  // incorrect usage:missing parameter value or unknown option
	            return bios_set_ns::BS_INCORRECT_USAGE;

		default:
			return bios_set_ns::BS_INCORRECT_USAGE;
		}
	} // end while


	if ( optind < _argc ) {
		//Too many arguments
		return bios_set_ns::BS_INCORRECT_USAGE;
	}

	if ( opt_m + opt_i + opt_I + opt_p + opt_P > 1){ // Invalid mix of arguments
		return bios_set_ns::BS_INCORRECT_USAGE;
	}
	if ( (opt_m + opt_p + opt_i + opt_I + opt_P) < 1){ // missing parameter
		return bios_set_ns::BS_INCORRECT_USAGE;
	}
	if ( (opt_C + opt_B > 1)){ // Invalid mix of argument
		return bios_set_ns::BS_INCORRECT_USAGE;
	}

	if (opt_m) return printmenu();

	return bios_set_ns::BS_NO_ERROR;
 }


int bios_set_cmd::printmenu()
{
	int choice;
//	int res = -1;
	bool exit_from_while_;
	int return_code = bios_set_ns::BS_NO_ERROR;

	do{
		exit_from_while_ = true;
		printf("*****************************\n");
		printf("* Choose type of action     *\n");
		printf("* 1. Get BIOS Pointer       *\n");
		printf("* 2. Set BIOS Pointer       *\n");
		printf("* 3. Get Default BIOS Image *\n");
		printf("* 4. Set Default BIOS Image *\n");
		printf("* 5. Exit                   *\n");
		printf("*****************************\n");
		printf(":");

		// wait for a choice
		if((choice = readChoice()) < 0)
			return bios_set_ns::BS_GENERIC_ERROR;	// EOF encountered or an error has occurred

		BIOSSET_LOG("DEBUG - printmenu() choice == %d\n",  choice);
		switch(choice)
		{
			case '1': _bios_set_function = bios_set_ns::GETBIOSPOINTER;
			break;
			case '2':
			{
				printf("Enter BIOS Pointer (0,1): ");

				// wait for bios_pointer input
				int bios_pointer_value = readChoice();

				if(bios_pointer_value < 0)
					return bios_set_ns::BS_GENERIC_ERROR;	// EOF encountered or an error has occurred

				if (bios_pointer_value != '0' && bios_pointer_value != '1'){
					return_code = bios_set_ns::BS_ILLEGAL_VALUE_FOR_BIOS_POINTER;
				}
				else {
				 _bios_pointer= bios_pointer_value - '0';
				 _bios_set_function = bios_set_ns::SETBIOSPOINTER;
				}
			}
			break;
			case '3':  _bios_set_function = bios_set_ns::GETBIOSIMAGE;

			break;
			case '4':
			{
				printf("Enter Default BIOS image (0,1) : ");

				int bios_image_value = readChoice();
				if(bios_image_value < 0)
					return bios_set_ns::BS_GENERIC_ERROR;

				if (bios_image_value != '0' && bios_image_value != '1'){
					return_code = bios_set_ns::BS_ILLEGAL_VALUE_FOR_BIOS_IMAGE;
				}
				else{
					_bios_default_image= bios_image_value - '0';
					_bios_set_function = bios_set_ns::SETBIOSIMAGE;
				}
			}
			break;

			case '5':
				return_code =  bios_set_ns::BS_NO_ECHO;
			break;
			default:
				printf("ILLEGAL_CHOICE\n");
				exit_from_while_ = false;
			break;
		}
	} while(!exit_from_while_);

	BIOSSET_LOG("DEBUG - printmenu() return_code == %d\n",  return_code);
	return return_code;
}

int bios_set_cmd::readChoice()
{
	int c, choice;

	// discards initial blanks
	for(c = getchar(); isblank(c); c = getchar())
				;

	if(c == EOF) return -1;		// EOF encountered or an error has occurred

	if(c == '\n') return '\n'; 	// no character other than blanks has been found

	choice = c; // a not blank character has been found, cache it !

	// check the remaining characters (they must be only blanks or newline)
	for(c = getchar(); isblank(c); c = getchar())
				;

	int tmp = c; // cache the first character found after blanks. It should be a newline for valid choices

	// consume remaining characters until newline (included )
	while((c != '\n') && (c != EOF))
		c = getchar();

	return ((tmp == '\n') ? choice : '\n');

}

void  bios_set_cmd:: printUsage(void) const {

	printf("Usage: bios_set -m [-C COMPORT | -B [MAGADDR:]SLOT]\n");
	printf("Usage: bios_set -p [-C COMPORT | -B [MAGADDR:]SLOT]\n");
	printf("Usage: bios_set -P BIOS_pointer [-C COMPORT | -B [MAGADDR:]SLOT]\n");
	printf("Usage: bios_set -i [-C COMPORT | -B [MAGADDR:]SLOT]\n");
	printf("Usage: bios_set -I BIOS_image [-C COMPORT | -B [MAGADDR:]SLOT]\n");
	printf("\nCOMMAND OPTIONS:\n");

	printf("-m                 Interactive mode.\n");
	printf("-p                 Get BIOS Pointer.\n");;
	printf("-P BIOS_pointer    Set BIOS Pointer, (0 or 1).\n");
	printf("-i                 Get Default BIOS Image.\n");
	printf("-I BIOS_image      Set Default BIOS Image, (0 or 1).\n");
	printf("-C COMPORT         Execute command locally, using COM port. Default is COM2.\n");
	printf("-B SLOT            Execute command via shelf manager on local magazine.\n");
	printf("                       SLOT    - Slotnumber on board in magazine to execute on (0-26|28).\n");
	printf("-B MAGADDR:SLOT    Execute command via shelf manager.\n");
	printf("                       MAGADDR - Magazine to update.\n");
	printf("                       SLOT    - Slotnumber on board in magazine to execute on (0-26|28).\n");
	printf("(If -C or -B is not specified, locally connection using COM-port COM2 will be used.)\n\n");

}

const char * bios_set_cmd::error_str(int err_code)
{
	switch (err_code)
	{
		case bios_set_ns::BS_NO_ERROR: return ("Command Executed");
		case bios_set_ns::BS_GENERIC_ERROR : return("Command Failed");
		case bios_set_ns::BS_INCORRECT_USAGE : return("Incorrect Usage");
		case bios_set_ns::BS_CANNOT_GET_INFORMATION_FROM_CS : return ("Cannot fetch information from CS");
		case bios_set_ns::BS_UNABLE_TO_CONNECT_TO_CS : return("Unable to connect to CS");
		case bios_set_ns::BS_UNABLE_GET_SCXB_IP_FROM_CS : return("Unable to fetch Switch board IP from CS");
		case bios_set_ns::ILLEGAL_OPT_IN_SYSTEM_CONFIG : return ("Local magazine number expected in this system configuration");
		case bios_set_ns::BS_ILLEGAL_VALUE_FOR_BIOS_POINTER : return ("Illegal value for BIOS POINTER");
		case bios_set_ns::BS_ILLEGAL_VALUE_FOR_BIOS_IMAGE : return ("Illegal value for BIOS IMAGE");
		case bios_set_ns::BS_ILLEGAL_CHOICE:  return ("Illegal choice");
		case bios_set_ns::BS_SNMP_ERR_IN_SENDING_REQUEST: return("SNMP failure: error in sending request to SNMP Agent");
		case bios_set_ns::BS_NO_RESPONSE_FROM_SNMP_AGENT: return ("SNMP failure: no response received from SNMP Agent");
		case bios_set_ns::BS_SNMP_BAD_RESPONSE_RECEIVED: return ("SNMP failure: error in response received from SNMP Agent");
		case bios_set_ns::BS_SNMP_GENERIC_FAILURE: return("SNMP Failure: generic error");
		case bios_set_ns::BS_SNMP_RESOURCEUNAVAILABLE_ERR: return ("SNMP failure: information unavailable for the selected board.");
		case bios_set_ns::BS_ERR_SNMP_GETTING_IPMI_MASTER: return ("SNMP failure: unable to fetch master status");
		case bios_set_ns::BS_ILLEGAL_VALUE_FOR_SLOT_POS: return ("Illegal value for slot parameter: legal values are in the range [0-26|28]");
		case bios_set_ns::BS_MISSING_VALUE_FOR_BIOS_POINTER:return("Missing value for Bios Pointer.");
		case bios_set_ns::BS_MISSING_VALUE_FOR_BIOS_IMAGE: return("Missing value for Bios Image");
		case bios_set_ns::BS_MISSING_VALUE_FOR_SLOT_PARAMETER: return ("Missing value for slot parameter.");
		case bios_set_ns::BS_ILLEGAL_VALUE_FOR_MAGAZINE: return ("Illegal value for magazine number parameter");
		case bios_set_ns::BS_SNMP_SET_UNAVAIL_ERR: return ("SNMP failure: selected board is unavailable.");
		case bios_set_ns::BS_NO_ECHO: return ("");
		case bios_set_ns::BS_OP_NOT_PERMITTED: return ("The command cannot be performed for the selected board.");
		case bios_set_ns::BS_ILLEGAL_COMMAND: return ("Illegal command in this system configuration");
		case bios_set_ns::BS_BOARD_NOT_FOUND: return ("Selected board is unavailable.");
		case bios_set_ns::BS_IRONSIDE_FAILURE: return ("Communication Failure");
		default: return ("Unknown error");

	}
}

int bios_set_cmd::execute()
{
	int ret_code;
	bios_set_handler bios_set_impl;
//	if((ret_code = check_for_gea_board()) == bios_set_ns::BS_INCORRECT_USAGE)
//		return ret_code;	

	// BIOS_SET operations cannot be executed on GEA Disk boards. Try to check for that condition
	if (_slotnum != -1)
	{
		uint16_t fbn_id;
		if((ret_code = bios_set_impl.get_board_fbn_id(fbn_id, _slotnum, _magazine)) ==  bios_set_ns::BS_NO_ERROR)
		{
			if ((fbn_id == ACS_CS_API_HWC_NS::FBN_GEA) || (fbn_id == ACS_CS_API_HWC_NS::FBN_Disk) || (fbn_id == ACS_CS_API_HWC_NS::FBN_SCXB) || (fbn_id == ACS_CS_API_HWC_NS::FBN_CMXB) || (fbn_id == ACS_CS_API_HWC_NS::FBN_IPTB) || (fbn_id == ACS_CS_API_HWC_NS::FBN_EPB1) || (fbn_id == ACS_CS_API_HWC_NS::FBN_EvoET) || (fbn_id == ACS_CS_API_HWC_NS::FBN_IPLB) || (fbn_id == ACS_CS_API_HWC_NS::FBN_MAUB) || (fbn_id == ACS_CS_API_HWC_NS::FBN_CPUB) || (fbn_id == ACS_CS_API_HWC_NS::FBN_DVD))
			{
				BIOSSET_LOG("DEBUG - cannot execute operations on selected board ( _slotnum == %d _magazine == '%s' bios_image_value = %d  command_execution_mode == %d )\n",  this->_slotnum, _magazine, _bios_default_image, this->_cmd_execution_mode);
				return 	bios_set_ns::BS_OP_NOT_PERMITTED;
			}

		}
		else
		{
			BIOSSET_LOG("DEBUG - Cannot find board on HWC Table ( ret_code == %d, _slotnum == %d _magazine == '%s' bios_image_value = %d  command_execution_mode == %d )\n", ret_code, this->_slotnum, _magazine, _bios_default_image, this->_cmd_execution_mode);
			return 	bios_set_impl.set_command_result(ret_code);
		}
	}

	if ((ret_code = bios_set_impl.initialize(this)) != bios_set_ns::BS_NO_ERROR)
	{
		return ret_code;
	}

	switch (this->_bios_set_function)
	{
	case bios_set_ns::SETBIOSIMAGE:
		BIOSSET_LOG("DEBUG - SETBIOSIMAGE ( _slotnum == %d _magazine == '%s' bios_image_value = %d  command_execution_mode == %d )\n",  this->_slotnum, _magazine, _bios_default_image, this->_cmd_execution_mode);
		if(_cmd_execution_mode == bios_set_ns::IRONSIDE_MODE)
		{
			if((ret_code= bios_set_impl.set_ironside_bios_image(_slotnum, _magazine, _bios_default_image)) != bios_set_ns::BS_NO_ERROR){
				return ret_code;
			}
		}
		else
		{
			if((ret_code= bios_set_impl.set_bios_image(_slotnum, /*_magazine, */_bios_default_image)) != bios_set_ns::BS_NO_ERROR){
				//this->_error_code = ret_code;
				return ret_code;
			}
		}
		break;


	case bios_set_ns::SETBIOSPOINTER:

		BIOSSET_LOG(" SETBIOSPOINTER ( _slotnum == %d _magazine == '%s' bios_pointer_value = %d  command_execution_mode == %d \n",  this->_slotnum, _magazine, _bios_pointer, this->_cmd_execution_mode);

		bios_set_ns::EnvironmentType m_environment;
		m_environment = bios_set_ns::getEnvironment();

		//In SCB-RP environment only COM mode is supported
		if ((_cmd_execution_mode != bios_set_ns::COM_MODE)
				&& (m_environment == bios_set_ns::SINGLECP_NOTCBA || m_environment == bios_set_ns::MULTIPLECP_NOTCBA))
		{
			return bios_set_ns::BS_ILLEGAL_COMMAND;
		}

		if(_cmd_execution_mode == bios_set_ns::IRONSIDE_MODE)
		{
			if((ret_code= bios_set_impl.set_ironside_bios_pointer(_slotnum, _magazine, _bios_pointer)) != bios_set_ns::BS_NO_ERROR){
				return ret_code;
			}
		}
		else
		{
			if((ret_code= bios_set_impl.set_bios_pointer( _slotnum, /*_magazine,*/ _bios_pointer)) != bios_set_ns::BS_NO_ERROR){
				return ret_code;
			}
		}
		break;


		case bios_set_ns::GETBIOSIMAGE:
			BIOSSET_LOG("DEBUG - bios_set_cmd::execute() GETBIOSIMAGE - command_execution_mode ==  %d  _slotnum == %d _magazine == '%s'\n",
					this->_cmd_execution_mode, this->_slotnum, _magazine);
			int bios_def_image;

			if(_cmd_execution_mode == bios_set_ns::IRONSIDE_MODE)
			{
				if((ret_code= bios_set_impl.get_ironside_bios_image(_slotnum, _magazine, bios_def_image)) != bios_set_ns::BS_NO_ERROR){
					return ret_code;
				}
			}
			else
			{
				if((ret_code= bios_set_impl.get_bios_image( _slotnum, /*_magazine, */bios_def_image)) != bios_set_ns::BS_NO_ERROR){
					return ret_code;
				}
			}
			_bios_default_image = bios_def_image;
			BIOSSET_LOG("DEBUG - bios_set_cmd::execute() GETBIOSIMAGE  _bios_image == %d\n", bios_def_image);
			printf("BIOS IMAGE = %d\n", bios_def_image);
			break;


		case bios_set_ns::GETBIOSPOINTER:
			BIOSSET_LOG("DEBUG - bios_set_cmd::execute() GETBIOSPOINTER - command_execution_mode ==  %d  _slotnum == %d _magazine == '%s'\n",
					this->_cmd_execution_mode, _slotnum, this->_magazine);
			int bios_p_value;

			if(_cmd_execution_mode == bios_set_ns::IRONSIDE_MODE)
			{
				if((ret_code= bios_set_impl.get_ironside_bios_pointer(_slotnum, _magazine, bios_p_value)) != bios_set_ns::BS_NO_ERROR){
					return ret_code;
				}

			}
			else
			{
				if ((ret_code = bios_set_impl.get_bios_pointer(_slotnum,/* _magazine,*/ bios_p_value))!= bios_set_ns::BS_NO_ERROR){
					return ret_code;
				}
			}
			BIOSSET_LOG("DEBUG - bios_set_cmd::execute() GETBIOSPOINTER  get_bios_pointer return with ret_code == %d\n", ret_code);
			_bios_pointer = bios_p_value;
			BIOSSET_LOG("DEBUG - bios_set_cmd::execute() GETBIOSPOINTER  bios_pointer == %d\n", bios_p_value);
			printf("BIOS Pointer = %d\n", bios_p_value);
			break;
		default:
			BIOSSET_LOG("DEBUG - bios_set_cmd::execute() -  %d FUNCTION UNKNOWN\n", this->_bios_set_function);
			ret_code = bios_set_ns::BS_GENERIC_ERROR;
			break;
	}

	return ret_code;
}


int bios_set_cmd::verify_mag_and_slot_paramater(char * mag_slot, char *magazine, int & slot, int &error_code)
{
	int result_ = 0;

	char *token =0; char *lasts =0;
	char *slot_str = 0;

	//Check that only one colon is present
	unsigned int count = 0;
	for (unsigned int i = 0; i < strlen(mag_slot) && count <= 1; i++)
	{
		count += (mag_slot[i] == ':')? ((i == 0)? 2: 1):0;
	}

	if (count > 1)
	{
		error_code = bios_set_ns::BS_ILLEGAL_VALUE_FOR_MAGAZINE;
		result_ = -1;
		return result_;
	}


	if ((token = strtok(mag_slot, ":")) != NULL)
	    lasts = strtok(NULL, ":");

	if (token == NULL && lasts == NULL) return -1;
	if(token != NULL && lasts == NULL) slot_str = token;
	if(lasts != NULL) slot_str = lasts;

	if (token != NULL && lasts != NULL){
		strncpy(magazine, token, MAGAZINENAME_MAXLEN);
		//check magazine parameter!!!
		const int  ret_code = check_magazine_parameter(token);

		if (ret_code)
		{
			error_code = bios_set_ns::BS_ILLEGAL_VALUE_FOR_MAGAZINE;
			result_ = -1;
		}
	}
	BIOSSET_LOG("DEBUG - check_magazine_parameter() - returns  magazine == %s\n", magazine);
	if (const int ret_code = check_slot_paramater(slot_str, slot)) {
		error_code = bios_set_ns::BS_ILLEGAL_VALUE_FOR_SLOT_POS;
		result_ = -1;
	}

	BIOSSET_LOG("DEBUG - verify_mag_and_slot_paramater() - returns  magazine == %s  <ret_code == %d>\n", magazine, result_);

	return result_;
}


int bios_set_cmd::check_slot_paramater (char * slot_str, int &slot_pos)
{
	char *endptr;
        char *str=NULL;
	long int val;

	BIOSSET_LOG("DEBUG - Calling check_slot_parameter() - slot_str == %s\n", slot_str);
	errno = 0;	 /* To distinguish success/failure after call */
	val = strtol(slot_str, &endptr, 10);

   /* Check for various possible errors */

   if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
	   || (errno != 0 && val == 0)) {
	  return -1;
   }
   if (endptr == str) {
	   BIOSSET_LOG("ERROR - check_slot_paramater() - ERROR: No digits were found in '%s'\n", slot_str);
	  return -1;
   }
   if (*endptr != '\0')	  /* Not necessarily an error... */
	  return -1;

   slot_pos = static_cast<int>(val);
   BIOSSET_LOG("DEBUG - check_slot_parameter() - returns  slot_pos == %d\n", slot_pos);
   if(slot_pos < 0 || slot_pos > 28 || slot_pos == 27){
	   return -1;
   }

   return 0;
}


int bios_set_cmd::check_magazine_parameter(char* magazine_name)
{
	int i = 0 ;
	char *token =0; int magazine_token[4] = {0};

	struct in_addr convaddr;

	if (inet_pton(AF_INET, magazine_name, &convaddr) != 1)
		return -1;

	BIOSSET_LOG("DEBUG - calling check_magazine_parameter(magazine_name == %s)\n", magazine_name);

	token = strtok(magazine_name, ".");
	while(token != NULL){

		magazine_token[i++]  = atoi(token);
		token = strtok(NULL, ".");
	}
	BIOSSET_LOG("DEBUG - check_magazine_parameter() - token1 == %d token2 == %d  token3 == %d token4 == %d\n", magazine_token[0], magazine_token[1], magazine_token[2], magazine_token[3]);

	if (i != 4) return -1;

	if(magazine_token[0] < FIRST_MAGNUMBER_MINVALUE || magazine_token[0] > FIRST_MAGNUMBER_MAXVALUE ) return -1;
	if(magazine_token[1] < SECOND_MAGNUMBER_MINVALUE || magazine_token[1] > SECOND_MAGNUMBER_MAXVALUE ) return -1;
	if(magazine_token[2] < THIRD_MAGNUMBER_MINVALUE || magazine_token[2] > THIRD_MAGNUMBER_MAXVALUE ) return -1;
	if(magazine_token[3] < FOURTH_MAGNUMBER_MINVALUE || magazine_token[3] > FOURTH_MAGNUMBER_MAXVALUE ) return -1;

	return 0;
}


bios_set_ns::EnvironmentType bios_set_ns::getEnvironment(void)
{
        bios_set_ns::EnvironmentType env;
        ACS_CS_API_CommonBasedArchitecture::ArchitectureValue arch;
        bool multipleCPSystem = false;

        if ((ACS_CS_API_NetworkElement::isMultipleCPSystem(multipleCPSystem) == ACS_CS_API_NS::Result_Success) && (ACS_CS_API_NetworkElement::getNodeArchitecture(arch) == ACS_CS_API_NS::Result_Success))
        {
                switch(arch)
                {
                case ACS_CS_API_CommonBasedArchitecture::SCB:

                        if (multipleCPSystem) env = MULTIPLECP_NOTCBA;
                        else env = SINGLECP_NOTCBA;
                        break;

                case ACS_CS_API_CommonBasedArchitecture::SCX:

                        if (multipleCPSystem) env = MULTIPLECP_CBA;
                        else env = SINGLECP_CBA;
                        break;

                case ACS_CS_API_CommonBasedArchitecture::SMX:
                        if (multipleCPSystem) env = MULTIPLECP_SMX;
                        else env = SINGLECP_SMX;
                        break;

                case ACS_CS_API_CommonBasedArchitecture::DMX:

                        if (multipleCPSystem) env = MULTIPLECP_DMX;
                        else env = SINGLECP_DMX;
                        break;
		case ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED:
			env = VIRTUALIZED;
			break;
                default:
                        env = UNKNOWN;
                        break;
                }
        }
        else
        {
               env = UNKNOWN;
        }

        return env;
}
