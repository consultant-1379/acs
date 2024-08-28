/*
 * acs_apbm_configurationhelper.h
 *
 *  Created on: Sep 7, 2011
 *      Author: xgiufer
 */

#ifndef HEADER_GUARD_CLASS__acs_apbm_configurationhelper
#define HEADER_GUARD_CLASS__acs_apbm_configurationhelper acs_apbm_configurationhelper

#include "acs_apbm_macrosconfig.h"
#include <ace/Time_Value.h>

//TODO - MOVE TO MACROSCONFIG

#ifndef ACS_APBM_CONFIG_BIOS_VERSION_PATH
#define ACS_APBM_CONFIG_BIOS_VERSION_PATH "/sys/devices/virtual/dmi/id/"
#endif

#ifndef ACS_APBM_CONFIG_BIOS_PRODUCT_NAME
#define ACS_APBM_CONFIG_BIOS_PRODUCT_NAME ACS_APBM_CONFIG_BIOS_VERSION_PATH "product_name"
#endif

#ifndef ACS_APBM_CONFIG_BIOS_PRODUCT_VERSION
#define ACS_APBM_CONFIG_BIOS_PRODUCT_VERSION ACS_APBM_CONFIG_BIOS_VERSION_PATH "product_version"
#endif

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_configurationhelper

/** @class acs_apbm_configurationhelper acs_apbm_configurationhelper.h
 *	@brief acs_apbm_configurationhelper class
 *	@author xgiufer (Giuliano Ferraioli)
 *	@date 2011-09-07
 *
 *	acs_apbm_configurationhelper Class detailed description
 */
class  __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
private:
	/** @brief acs_apbm_configurationhelper Default constructor
	 *
	 *	acs_apbm_configurationhelper Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () {}

	/** @brief acs_apbm_configurationhelper Copy constructor
	 *
	 *	acs_apbm_configurationhelper Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ (const __CLASS_NAME__ &);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_configurationhelper Destructor
	 *
	 *	acs_apbm_configurationhelper Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	static const char * unix_socket_root_path ();

	static const char * apbmapi_unix_socket_sap_pathname ();

	static const char * apbmapi_inet_socket_sap ();

	static int get_trap_timeout();

	static int primitive_send_timeout();

	static int primitive_receive_timeout();

	static const char * get_bios_product_name();

	static const char * get_bios_product_version();

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ &);
};

#endif // HEADER_GUARD_CLASS__acs_apbm_configurationhelper
