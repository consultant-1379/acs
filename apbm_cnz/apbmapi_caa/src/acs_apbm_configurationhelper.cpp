/*
 * acs_apbm_configurationhelper.cpp
 *
 *  Created on: Sep 7, 2011
 *      Author: xgiufer
 */


#include "acs_apbm_configurationhelper.h"


const char * __CLASS_NAME__::unix_socket_root_path () {
	static const char * const _unix_sockets_root_path = ACS_APBM_CONFIG_UNIX_SOCKETS_ROOT_PATH;
	return _unix_sockets_root_path;
}

const char * __CLASS_NAME__::apbmapi_unix_socket_sap_pathname () {
	static const char * const _apbmapi_unix_socket_sap_pathname = ACS_APBM_CONFIG_UNIX_SOCKETS_ROOT_PATH ACS_APBM_CONFIG_APBMAPI_UNIX_SOCKET_SAP_ID;
	return _apbmapi_unix_socket_sap_pathname;
}

const char * __CLASS_NAME__::apbmapi_inet_socket_sap () {
	static const char * const _apbmapi_inet_socket_sap = ACS_APBM_CONFIG_APBMAPI_INET_SOCKET_SAP_ID;
	return _apbmapi_inet_socket_sap;
}

const char * __CLASS_NAME__::get_bios_product_name(){
        static const char * const _bios_product_name = ACS_APBM_CONFIG_BIOS_PRODUCT_NAME;
        return _bios_product_name;
}
const char * __CLASS_NAME__::get_bios_product_version(){
        static const char * const _bios_product_version = ACS_APBM_CONFIG_BIOS_PRODUCT_VERSION;
        return _bios_product_version;
}
int __CLASS_NAME__::get_trap_timeout(){
	static  int _trap_reiceving_timeout = ACS_APBM_GET_TRAP_TIMEOUT;
	return _trap_reiceving_timeout;
}

int __CLASS_NAME__::primitive_send_timeout(){
	static  int _primitive_send_timeout = ACS_APBM_PRIMITIVE_SEND_TIMEOUT;
	return _primitive_send_timeout;
}

int __CLASS_NAME__::primitive_receive_timeout(){
	static  int _primitive_receive_timeout = ACS_APBM_PRIMITIVE_RECEIVE_TIMEOUT;
	return _primitive_receive_timeout;
}
