/*
 * snmp_mib.h
 *
 *  Created on: Nov 18, 2011
 *      Author: xassore
 */

#ifndef BIOS_SET_COMMON_H
#define BIOS_SET_COMMON_H
#include <stdio.h>

#define NULL_STRING 0
#define SHELFMGRBOARD_MAXNUM 2
#define BS_CONFIG_IPN_COUNT	2
#define IPNADDRESS_MAXLEN 16




#define FIRST_MAGNUMBER_MAXVALUE 15
#define FIRST_MAGNUMBER_MINVALUE 0
#define SECOND_MAGNUMBER_MAXVALUE 15
#define SECOND_MAGNUMBER_MINVALUE 0
#define THIRD_MAGNUMBER_MAXVALUE 0
#define THIRD_MAGNUMBER_MINVALUE 0
#define FOURTH_MAGNUMBER_MAXVALUE 15
#define FOURTH_MAGNUMBER_MINVALUE 0

#define BS_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))



#ifdef BS_DEBUG
#define BS_LOG_FILE_NAME   "./BIOSSET_CMD.log"
#endif

#ifdef BS_DEBUG
#define BIOSSET_LOG(X, ...)   (cmd_log::bios_set_log(X, __VA_ARGS__))
#define BIOS_SET_OPEN_LOG_FILE() (cmd_log::bios_set_openLogfile())
#define BIOS_SET_CLOSE_LOG_FILE() (cmd_log::bios_set_closeLogfile())
#else
#define BIOSSET_LOG(X, ...)
#define BIOS_SET_OPEN_LOG_FILE()
#define BIOS_SET_CLOSE_LOG_FILE()
#endif


namespace bios_set_snmp_ns {

	typedef enum ShelfMgrStateConstants {
		SHELMGRSTATE_NOT_ACTIVE = 0,
		SHELMGRSTATE_ACTIVE = 1
	} shelf_mgr_state_t;

	typedef enum BoardBiosPointer {
			BIOS_UNDEFINED = -1,
			BIOS1 = 0,
			BIOS2 = 1,
			NOTSUPPORTED = 2
		} board_bios_pointer_t;

}

// this class is an helper class to store information about switch boards

class shelfmgr_info {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief shelfmgr_info constructor
	 */
	inline shelfmgr_info ()
	: slot_position(-1), magazine(~0U), ipna_str(), ipnb_str(), state(bios_set_snmp_ns::SHELMGRSTATE_NOT_ACTIVE) {
		*ipna_str = 0;
		*ipnb_str = 0;
	}

private:

	//============//
	// Destructor //
	//============//
public:
	/** @brief shelfmgr_info Destructor
	 */
	inline ~shelfmgr_info () {}


	//===========//
	// Functions //
	//===========//
public:

	//===========//
	// Operators //
	//===========//

private:

	//========//
	// Fields //
	//========//
public:
	int slot_position;

	unsigned int magazine;

	char ipna_str [IPNADDRESS_MAXLEN];
	char ipnb_str [IPNADDRESS_MAXLEN];

	int  state;
};


#ifdef BS_DEBUG
class cmd_log {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief cmd_log constructor
	 */
	inline cmd_log (){}

	//============//
	// Destructor //
	//============//

	inline ~cmd_log () {}

	//===========//
	// Functions //
	//===========//
public:
    static int bios_set_openLogfile();
	static int bios_set_log (const char * format, ...);
	static void bios_set_closeLogfile();

private:



	//========//
	// Fields //
	//========//
public:

};
#endif

#endif /* BIOS_SET_COMMON_H */
