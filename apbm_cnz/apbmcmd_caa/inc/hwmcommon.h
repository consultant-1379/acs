/*
 * hwmcommon.h
 *
 *  Created on: Dec 7, 2011
 *      Author: xgiufer
 */

#ifndef HEADER_GUARD_CLASS__hwmcommon
#define HEADER_GUARD_CLASS__hwmcommon hwmcommon


#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <set>
#include <ace/Recursive_Thread_Mutex.h>

#include <ACS_CS_API.h>
#include <ACS_CS_API_Set.h>
#include <acs_apbm_api.h>
#include <acs_apbm_macros.h>

#define IMM_ROOT_CLASS "AxeFunctions"

/************************************************/
/************** Manager Shelf State *************/
#define MASTER "master"
#define PASSIVE "passive"
#define IDLE "idle"
#define DEGRADED "passDeg"
/************************************************/
/************** Neighbour State *************/
#define PRESENT "present"
#define ABSENT "absent"
#define UNINIT "uninit"

/************************************************/
/***************** FBN **************************/
#define FBN_SCXB3	"SCXB3"
#define FBN_SCXB	"SCXB"
#define FBN_SCB_RP4	"SCB"
#define FBN_SMXB	"SMXB"

/************************************************/
/*************** NODE ARCHITECTURE **************/
#define EGEM_SCB_RP       0
#define EGEM2_SCX_SA      1
#define EGEM2_SMX_SA	  4

/************************************************/
/*************** HARDWARE VERSION ***************/
#define APG43             1
#define APG43_2           2

/**************** END DEFINTION *****************/
/************************************************/

#define MAX_ERROR_TEXT_SIZE 512


enum ErrorNumberConstants {
	GENERAL_FAULT 					= 1,
	INVALID_USAGE 					= 2,
	SERVER_NOT_AVAILABLE			= 8, //used for hwmscbls: hwm or apbm server not responding
	//HWM_SERVER_NOT_AVAILABLE 		= 8,
	APBM_SERVER_NOT_AVAILABLE 		= 9,
	WRONG_PLATFORM                          = 30,
	ILLEGAL_SYSTEM_CONFIGURATION 	= 115
};

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__hwmcommon

class __CLASS_NAME__ {

public:
	// Constructor //
	inline __CLASS_NAME__ () : _last_error()
	{
		*_last_error = 0;
	}

public:
	// Destructor //
	inline ~__CLASS_NAME__ () {}


private:

	char _last_error [MAX_ERROR_TEXT_SIZE];

public:

	static const char * get_last_error_text(const int error);

	bool check_arg_mag(char* arg);

	static int get_all_magazines(std::set<uint32_t> & magazine_list);

	static int get_imm_class_instance_name(const char * class_name, std::string & class_instance_name);

	static int check_hardware_version();

	static int check_node_architecture();

	static int ip_format_to_uint32 (uint32_t & value, const char * value_ip);

	static int uint32_to_ip_format (char (& value_ip) [16], uint32_t value);


private:

	bool dotted_dec_to_long(std::istringstream &istr, unsigned long &value,
							unsigned long lowerLimit1, unsigned long upperLimit1,
							unsigned long lowerLimit2, unsigned long upperLimit2,
							unsigned long lowerLimit3, unsigned long upperLimit3,
							unsigned long lowerLimit4, unsigned long upperLimit4);
	bool check_figures(std::istringstream &istr);

	unsigned short num_of_char(std::istringstream &istr, char searchChar);

	void reverse_dotted_dec_str(std::istringstream &istr);

};

#endif /* HEADER_GUARD_CLASS__hwmcommon */
