#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <string.h>
#include <fstream>
#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>

//APGCC
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_paramhandling.h>
#include "acs_apbm_serverworkingset.h"
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_CLibTypes.h>

////PRC
//#include <acs_prc_types.h>
//#include <acs_prc_api.h>

#include "acs_apbm_programconstants.h"
#include "acs_apbm_haappmanager.h"
#include "acs_apbm_alarmeventhandler.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_csreader.h"

#include "acs_apbm_shelvesdatamanager.h"
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_operationpipescheduler.h"
#include "acs_apbm_arpping_dmx.h"

#include "acs_apbm_immrepositoryhandler.h"
#include "acs_apbm_monitoringservicehandler.h"
#define IS_PARTNER_PORT_UP(var,pos) ((var & (1 << pos)) == (1 << pos))

const char * nic_status_data::_nic_names[axe_eth_shelf::MAX_ETH_GEP2] = {
		"eth0", "eth1", "eth2", "eth3", "eth4" };
const char * nic_status_data_10g::_nic_names[axe_eth_shelf::MAX_ETH_GEP5] = {
		"eth0", "eth1", "eth2", "eth3", "eth4", "eth5", "eth6", "bond1" };

/*
 *  Please don't remove "dummy" entry from the following definition, even if that interface is not used in virtualized environment.
 *  It's a trick to ensure that: in the class "virtual_nic_status_data", for the "ethernet_interface" and "nics" member arrays,
 *  the position 'axe_eth_shelf::ETH_7' corresponds to "eth7".
*/
const char * virtual_nic_status_data::_nic_names [axe_eth_shelf::MAX_ETH_VAPG] = {"eth0", "eth1", "eth2", "eth3", "eth4", "eth5", "eth6","dummy","eth7","eth8"};

const char * nic_status_data_gep7::_nic_names [axe_eth_shelf::MAX_ETH_GEP7] = {"eth0","eth1","eth2", "eth3", "eth4", "eth5", "eth6","bond1"};//HY47548
const char * acs_apbm_monitoringservicehandler::_physical_separation_nic_name =
		"eth2";

const char * acs_apbm_monitoringservicehandler::_north_bound_imm_class_name =
		"NorthBound";
const char * acs_apbm_monitoringservicehandler::_axe_functions_imm_class_name =
		"AxeFunctions";
const char * acs_apbm_monitoringservicehandler::_datadisk_devname_in_virtual_apgenv = "/dev/eri_disk";
const char * acs_apbm_monitoringservicehandler::_datadisk_directory_to_test_in_virtual_apgenv = "/data/acs";

int acs_apbm_monitoringservicehandler::frontPort1_statusCounter[2] = {0,0};
int acs_apbm_monitoringservicehandler::frontPort2_statusCounter[2] = {0,0};
int acs_apbm_monitoringservicehandler::_count_scb_left_link_down = 0;
int acs_apbm_monitoringservicehandler::_count_scb_right_link_down = 0;
bool acs_apbm_monitoringservicehandler::disableAdminStatePort1 [2] = {false, false};
bool acs_apbm_monitoringservicehandler::disableAdminStatePort2 [2] = {false, false};
bool acs_apbm_monitoringservicehandler::bothFrontPortDown [2] = {false, false};
bool acs_apbm_monitoringservicehandler::firstLagMonitoringTaskAfterReset[2] = {false,false};

void nic_status_data::set_nic_status(const char * if_name,
		axe_eth_shelf::nic_status_t status) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			nics[i] = status;
			break;
		}
}
void nic_status_data::set_ethernet_interface_status(const char * if_name,
		axe_eth_shelf::nic_status_t interface_status) { //used to handle TR HQ56362
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			ethernet_interface[i] = interface_status;
			break;
		}
}
void nic_status_data::set_mac_address(const char * if_name, char * mac_address) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(mac_addresses[i], mac_address,
					acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"mac_address:%s", mac_addresses[i]);
			break;
		}
}
void nic_status_data::set_ip_address(const char * if_name, char * ipv4_address) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(ipv4_addresses[i], ipv4_address,
					acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"ip_address:%s", ipv4_addresses[i]);
			break;
		}
}
void nic_status_data::reset_nic_status() {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
}
void nic_status_data_10g::set_nic_status(const char * if_name,
		axe_eth_shelf::nic_status_t status) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			nics[i] = status;
			break;
		}
}
void nic_status_data_10g::set_ethernet_interface_status(const char * if_name,
		axe_eth_shelf::nic_status_t interface_status) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			ethernet_interface[i] = interface_status;
			break;
		}
}
void nic_status_data_10g::set_mac_address(const char * if_name,
		char * mac_address) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(mac_addresses[i], mac_address,
					acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"mac_address:%s", mac_addresses[i]);
			break;
		}
}
void nic_status_data_10g::reset_nic_status() {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
}
void nic_status_data_10g::set_ip_address(const char * if_name,
		char * ipv4_address) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(ipv4_addresses[i], ipv4_address,
					acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"ip_address:%s", ipv4_addresses[i]);
			break;
		}
}
void nic_status_data_gep7::set_nic_status(const char * if_name,
		axe_eth_shelf::nic_status_t status) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			nics[i] = status;
			break;
		}
}
void nic_status_data_gep7::set_ethernet_interface_status(const char * if_name,
		axe_eth_shelf::nic_status_t interface_status) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			ethernet_interface[i] = interface_status;
			break;
		}
}
void nic_status_data_gep7::set_mac_address(const char * if_name,
		char * mac_address) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(mac_addresses[i], mac_address,
					acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"mac_address:%s", mac_addresses[i]);
			break;	
		}
}
void nic_status_data_gep7::reset_nic_status() {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
}
void nic_status_data_gep7::set_ip_address(const char * if_name,
		char * ipv4_address) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(ipv4_addresses[i], ipv4_address,
				acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"ip_address:%s", ipv4_addresses[i]);
			break;
		}
}

void virtual_nic_status_data::set_nic_status (const char * if_name, axe_eth_shelf::nic_status_t status) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) { nics[i] = status; break; }
}
void virtual_nic_status_data::set_ethernet_interface_status (const char * if_name, axe_eth_shelf::nic_status_t interface_status) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) { ethernet_interface[i] = interface_status; break; }
}
void virtual_nic_status_data::set_mac_address(const char * if_name, char * mac_address){
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(mac_addresses[i], mac_address, acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"mac_address:%s", mac_addresses[i]);
			break;
		}
}
void virtual_nic_status_data::reset_nic_status(){
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
}
void virtual_nic_status_data::set_ip_address(const char * if_name, char * ipv4_address){
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_names); ++i)
		if (!::strcasecmp(if_name, _nic_names[i])) {
			strncpy(ipv4_addresses[i], ipv4_address, acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"ip_address:%s", ipv4_addresses[i]);
			break;
		}
}

int acs_apbm_monitoringservicehandler::retrieve_network_info(ifaddrs * ifaddr,
		nic_status_data &if_status_data) {
	if (!ifaddr) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error empty if_addrs structure!");
		return -1;
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		char mac_address_path[1024];
		::snprintf(mac_address_path, ACS_APBM_ARRAY_SIZE(mac_address_path),
				"/sys/class/net/%s/address", ifa->ifa_name);
		int family = ifa->ifa_addr->sa_family;
		char host[acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS] = { 0 };
		char mac_addr[acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS] = { 0 };
		int res = 0;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%s  address family: %d%s \n",
				ifa->ifa_name, family,
				(family == AF_PACKET) ? " (AF_PACKET)" :
				(family == AF_INET) ? " (AF_INET)" :
				(family == AF_INET6) ? " (AF_INET6)" : "");

		if (!ifa->ifa_addr)
			continue;

		switch (family) {
		case AF_INET:
			/*IP ADDRESS*/
			res = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
					NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (res != 0) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG," Call 'getnameinfo' failed: %s - Error in retrieving IPv4 address of %s interface ", gai_strerror(res), ifa->ifa_name);
				::freeifaddrs(ifaddr);
				return -1;
			}
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"IPv4 address of %s interface retrieved: < %s >", ifa->ifa_name , host);
			if_status_data.set_ip_address(ifa->ifa_name, host);
			/*IP ADDRESS END*/
			break;
		case AF_PACKET:
			/*MAC ADDRESS*/

			if (FILE * f = ::fopen(mac_address_path, "r")) // OK: file exists and was opened successfully
			{
				::fscanf(f, "%50s", mac_addr);
				if (*mac_addr != 0) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG,"MAC address of %s interface retrieved: < %s >", ifa->ifa_name , mac_addr);
					if_status_data.set_mac_address(ifa->ifa_name, mac_addr);
				}
				::fclose(f);
			} else // ERROR: opening file. I report media disconnected
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call 'fopen' failed: cannot open the file '%s'", mac_address_path);
				::freeifaddrs(ifaddr);
				return -1;
			}
			/*MAC ADDRESS END*/
		}
	}
	return 0;
}
int acs_apbm_monitoringservicehandler::retrieve_network_info(ifaddrs * ifaddr,
		nic_status_data_10g &if_status_data_10g) {

	if (!ifaddr) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error empty if_addrs structure!");
		return -1;
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		char mac_address_path[1024];
		::snprintf(mac_address_path, ACS_APBM_ARRAY_SIZE(mac_address_path),
				"/sys/class/net/%s/address", ifa->ifa_name);
		int family = ifa->ifa_addr->sa_family;
		char host[acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS] = { 0 };
		char mac_addr[acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS] = { 0 };
		int res = 0;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%s  address family: %d%s \n",
				ifa->ifa_name, family,
				(family == AF_PACKET) ? " (AF_PACKET)" :
				(family == AF_INET) ? " (AF_INET)" :
				(family == AF_INET6) ? " (AF_INET6)" : "");

		if (!ifa->ifa_addr)
			continue;

		switch (family) {
		case AF_INET:
			/*IP ADDRESS*/
			res = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
					NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (res != 0) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG," Call 'getnameinfo' failed: %s - Error in retrieving IPv4 address of %s interface ", gai_strerror(res), ifa->ifa_name);
				::freeifaddrs(ifaddr);
				return -1;
			}
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"IPv4 address of %s interface retrieved: < %s >", ifa->ifa_name , host);
			if_status_data_10g.set_ip_address(ifa->ifa_name, host);
			/*IP ADDRESS END*/
			break;
		case AF_PACKET:
			/*MAC ADDRESS*/

			if (FILE * f = ::fopen(mac_address_path, "r")) // OK: file exists and was opened successfully
			{
				::fscanf(f, "%50s", mac_addr);
				if (*mac_addr != 0) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG,"MAC address of %s interface retrieved: < %s >", ifa->ifa_name , mac_addr);
					if_status_data_10g.set_mac_address(ifa->ifa_name, mac_addr);
				}

				::fclose(f);
			} else // ERROR: opening file. I report media disconnected
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call 'fopen' failed: cannot open the file '%s'", mac_address_path);
				::freeifaddrs(ifaddr);
				return -1;
			}
			/*MAC ADDRESS END*/
		}
	}
	return 0;
}
int acs_apbm_monitoringservicehandler::retrieve_network_info(ifaddrs * ifaddr,
                nic_status_data_gep7 &if_status_data_gep7) {

        if (!ifaddr) {
                ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error empty if_addrs structure!");
                return -1;
        }

        for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
                char mac_address_path[1024];
                ::snprintf(mac_address_path, ACS_APBM_ARRAY_SIZE(mac_address_path),
                                "/sys/class/net/%s/address", ifa->ifa_name);
                int family = ifa->ifa_addr->sa_family;
                char host[acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS] = { 0 };
                char mac_addr[acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS] = { 0 };
                int res = 0;
                ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%s  address family: %d%s \n",
                                ifa->ifa_name, family,
                                (family == AF_PACKET) ? " (AF_PACKET)" :
                                (family == AF_INET) ? " (AF_INET)" :
                                (family == AF_INET6) ? " (AF_INET6)" : "");

                if (!ifa->ifa_addr)
                        continue;

                switch (family) {
                case AF_INET:
                        /*IP ADDRESS*/
                        res = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
                                        NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
                        if (res != 0) {
                                ACS_APBM_LOG(LOG_LEVEL_DEBUG," Call 'getnameinfo' failed: %s - Error in retrieving IPv4 address of %s interface ", gai_strerror(res), ifa->ifa_name);
                                ::freeifaddrs(ifaddr);
                                return -1;
                        }
                        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"IPv4 address of %s interface retrieved: < %s >", ifa->ifa_name , host);
                        if_status_data_gep7.set_ip_address(ifa->ifa_name, host);
                        /*IP ADDRESS END*/
                        break;
               case AF_PACKET:
                        /*MAC ADDRESS*/

                        if (FILE * f = ::fopen(mac_address_path, "r")) // OK: file exists and was opened successfully
                        {
                                ::fscanf(f, "%50s", mac_addr);
                                if (*mac_addr != 0) {
                                        ACS_APBM_LOG(LOG_LEVEL_DEBUG,"MAC address of %s interface retrieved: < %s >", ifa->ifa_name , mac_addr);
                                        if_status_data_gep7.set_mac_address(ifa->ifa_name, mac_addr);
                                }

                                ::fclose(f);
                        } else // ERROR: opening file. I report media disconnected
                        {
                                ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call 'fopen' failed: cannot open the file '%s'", mac_address_path);
                                ::freeifaddrs(ifaddr);
                                return -1;
                        }
                        /*MAC ADDRESS END*/
                }
        }
        return 0;
}

int acs_apbm_monitoringservicehandler::retrieve_network_info(ifaddrs * ifaddr, virtual_nic_status_data &if_status_data){

	if(!ifaddr)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error empty if_addrs structure!");
		return -1;
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next)
	{
		char mac_address_path [1024];
		::snprintf(mac_address_path, ACS_APBM_ARRAY_SIZE(mac_address_path), "/sys/class/net/%s/address", ifa->ifa_name);
		int family = ifa->ifa_addr->sa_family;
		char host[acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS] = {0};
		char mac_addr[acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS] = {0};
		int res = 0;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%s  address family: %d%s \n",
				ifa->ifa_name, family,
				(family == AF_PACKET) ? " (AF_PACKET)" :
						(family == AF_INET) ?   " (AF_INET)" :
								(family == AF_INET6) ?  " (AF_INET6)" : "");

		if (!ifa->ifa_addr) continue;

		switch(family)
		{
		case AF_INET :
			/*IP ADDRESS*/
			res = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (res != 0) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG," Call 'getnameinfo' failed: %s - Error in retrieving IPv4 address of %s interface ", gai_strerror(res), ifa->ifa_name);
				::freeifaddrs(ifaddr);
				return -1;
			}
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"IPv4 address of %s interface retrieved: < %s >", ifa->ifa_name , host);
			if_status_data.set_ip_address(ifa->ifa_name, host);
			/*IP ADDRESS END*/
			break;
		case AF_PACKET:
			/*MAC ADDRESS*/

			if (FILE * f = ::fopen(mac_address_path, "r"))  // OK: file exists and was opened successfully
			{
				::fscanf(f, "%50s", mac_addr);
				if(*mac_addr != 0){
					ACS_APBM_LOG(LOG_LEVEL_DEBUG,"MAC address of %s interface retrieved: < %s >", ifa->ifa_name , mac_addr);
					if_status_data.set_mac_address(ifa->ifa_name, mac_addr);
				}

				::fclose(f);
			} else  // ERROR: opening file. I report media disconnected
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call 'fopen' failed: cannot open the file '%s'", mac_address_path);
				::freeifaddrs(ifaddr);
				return -1;
			}
			/*MAC ADDRESS END*/
		}
	}
	return 0;
}


int acs_apbm_monitoringservicehandler::check_physical_separation_status() {
	int phys_sep_status = -1;
	OmHandler omManager;
	acs_apgcc_paramhandling pha;
	ACS_CC_ReturnType result;

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'OmHandler Init' failed: cannot get the class name!");
		return phys_sep_status;
	}/*exiting on IMM init failure: generic error*/

	string class_instance_name;
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances(_north_bound_imm_class_name, p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1)) {
		/*exiting on error either an erroe occurred or more than one instance found */
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getClassInstances' failed: cannot get the class name!");
		omManager.Finalize();
		return phys_sep_status;
	}

	class_instance_name = p_dnList[0];
	omManager.Finalize();
	const char * attrib_name = "physicalSeparationStatus";

	ACS_CC_ReturnType imm_result = pha.getParameter(
			class_instance_name.c_str(), attrib_name, &phys_sep_status);
	if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting parameter value from IMM
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getParameter' failed: cannot retrieve the physical separation parameter from IMM: "
				"internal last error == %d: internal laste error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
		return -1;
	}
	return phys_sep_status;
}

int acs_apbm_monitoringservicehandler::check_teaming_status() {
	int teaming_status = -1;
	OmHandler omManager;
	acs_apgcc_paramhandling pha;
	ACS_CC_ReturnType result;

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'OmHandler Init' failed: cannot get the class name!");
		return teaming_status;
	}/*exiting on IMM init failure: generic error*/

	string class_instance_name;
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances(_north_bound_imm_class_name, p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1)) {
		/*exiting on error either an erroe occurred or more than one instance found */
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getClassInstances' failed: cannot get the class name!");
		omManager.Finalize();
		return teaming_status;
	}

	class_instance_name = p_dnList[0];
	omManager.Finalize();
	const char * attrib_name = "teamingStatus";

	ACS_CC_ReturnType imm_result = pha.getParameter(
			class_instance_name.c_str(), attrib_name, &teaming_status);
	if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting parameter value from IMM
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getParameter' failed: cannot retrieve the teaming status parameter from IMM: "
				"internal last error == %d: internal laste error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
		return -1;
	}
	return teaming_status;
}
int acs_apbm_monitoringservicehandler::check_dataDisk_replication_type() {
	int disk_rep_type = -1;
	OmHandler omManager;
	acs_apgcc_paramhandling pha;
	ACS_CC_ReturnType result;
	result = omManager.Init();
	if (result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Call 'OmHandler Init' failed: cannot get the class name!");
		return disk_rep_type;
	}
	string class_instance_name;
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances(_axe_functions_imm_class_name,
			p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getClassInstances' failed: cannot get the class name!");
		omManager.Finalize();
		return disk_rep_type;
	}
	class_instance_name = p_dnList[0];
	omManager.Finalize();
	const char * attrib_name = "dataDiskReplicationType";
	ACS_CC_ReturnType imm_result = pha.getParameter(
			class_instance_name.c_str(), attrib_name, &disk_rep_type);
	if (imm_result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'getParameter' failed: cannot retrieve the datadiskreplicationtype from IMM: "
				"internal last error == %d: internal laste error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
		return -1;
	}
	return disk_rep_type;
}
bool acs_apbm_monitoringservicehandler::check_valid_nic_data(
		const nic_status_data &new_nic_info) {
	bool up_to_date = true;
	if (_forced_nic_update) {
		return (up_to_date = _forced_nic_update = false);
	}

	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_nic_status.nics))
			&& up_to_date; ++i) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"index=%d - Internal NIC: %d == %d retrieved NIC!", i, _nic_status.nics[i], new_nic_info.nics[i]);
		up_to_date = (_nic_status.nics[i] == new_nic_info.nics[i]);
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) %s", ( (up_to_date) ? "No needed update of nic_status_data" : "Needed update of nic_status_data") );

	return up_to_date;
}
bool acs_apbm_monitoringservicehandler::check_valid_nic_data(
		const nic_status_data_10g &new_nic_info) {
	bool up_to_date = true;
	if (_forced_nic_update) {
		return (up_to_date = _forced_nic_update = false);
	}

	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_nic_status_10g.nics))
			&& up_to_date; ++i) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"index=%d - Internal NIC: %d == %d retrieved NIC!", i, _nic_status_10g.nics[i], new_nic_info.nics[i]);
		up_to_date = (_nic_status_10g.nics[i] == new_nic_info.nics[i]);
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) %s!", ( (up_to_date) ? "No needed update of nic_status_data_10g" : "Needed update of nic_status_data_10g") );

	return up_to_date;
}
bool acs_apbm_monitoringservicehandler::check_valid_nic_data(
		const nic_status_data_gep7 &new_nic_info) {
	bool up_to_date = true;
	if (_forced_nic_update) {
		return (up_to_date = _forced_nic_update = false);
	}
	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_nic_status_gep7.nics))
			&& up_to_date; ++i) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"index=%d - Internal NIC: %d == %d retrieved NIC!", i, _nic_status_gep7.nics[i], new_nic_info.nics[i]);
		up_to_date = (_nic_status_gep7.nics[i] == new_nic_info.nics[i]);
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) %s!", ( (up_to_date) ? "No needed update of nic_status_data_gep7" : "Needed update of nic_status_data_gep7") );
	return up_to_date;
}

bool acs_apbm_monitoringservicehandler::check_valid_nic_data(const virtual_nic_status_data &new_nic_info) {
	bool up_to_date = true;
	if(_forced_nic_update) { return (up_to_date = _forced_nic_update = false);}

	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_vnic_status.nics)) && up_to_date; ++i){
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"index=%d - Internal NIC: %d == %d retrieved NIC!", i, _vnic_status.nics[i], _vnic_status.nics[i]);
		up_to_date = (_vnic_status.nics[i] == new_nic_info.nics[i]);
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) %s!", ( (up_to_date) ? "No needed update of virtual_nic_status_data" : "Needed update of virtual_nic_status_data") );

	return up_to_date;
}

bool acs_apbm_monitoringservicehandler::check_valid_nic_data_on_passive(
		const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index,
		const nic_status_data &new_nic_info) {
	bool up_to_date = true;
	axe_eth_shelf::nic_type_t nic_index_tmp = axe_eth_shelf::ETH_UNDEFINED;

	if (_forced_nic_update) {
		return (up_to_date  = false);
	}

	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_nic_status.nics))
			&& up_to_date; ++i) {
		if (!::strcasecmp(ifa_name, nic_status_data::_nic_names[i])) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%d eth on passive board: index=%d - Internal NIC: %d == %d retrieved NIC!",ACS_APBM_ARRAY_SIZE(_nic_status.nics), i, _nic_status.nics[i], new_nic_info.nics[i]);
			!(up_to_date = (_nic_status.nics[i] == new_nic_info.nics[i]))
					&& (nic_index_tmp
							= static_cast<axe_eth_shelf::nic_type_t> (i));
		}
	}
	nic_index = nic_index_tmp;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) %s on passive board of interface %d", ( (up_to_date) ? "No needed update of nic_status" : "Needed update of nic_status"), nic_index );
	return up_to_date;

}
bool acs_apbm_monitoringservicehandler::check_valid_nic_data_on_passive(
		const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index,
		const nic_status_data_10g &new_nic_info) {
	bool up_to_date = true;

	if (_forced_nic_update) {
		return (up_to_date = false);
	}

	axe_eth_shelf::nic_type_t nic_index_tmp = axe_eth_shelf::ETH_UNDEFINED;
	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_nic_status_10g.nics))
			&& up_to_date; ++i) {
		if (!::strcasecmp(ifa_name, nic_status_data_10g::_nic_names[i])) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%d eth on passive board: index=%d - Internal NIC: %d == %d retrieved NIC!", ACS_APBM_ARRAY_SIZE(_nic_status_10g.nics), i, _nic_status_10g.nics[i], new_nic_info.nics[i]);
			!(up_to_date = (_nic_status_10g.nics[i] == new_nic_info.nics[i]))
					&& (nic_index_tmp
							= static_cast<axe_eth_shelf::nic_type_t> (i));
		}
	}
	nic_index = nic_index_tmp;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) %s on passive board of interface %d", ( (up_to_date) ? "No needed update of nic_status_data_10g" : "Needed update of nic_status_data_10g"), nic_index );

	return up_to_date;
}
bool acs_apbm_monitoringservicehandler::check_valid_nic_data_on_passive(
		const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index,
		const nic_status_data_gep7 &new_nic_info) {
	bool up_to_date = true;

	if (_forced_nic_update) {
		return (up_to_date = false);
	}

	axe_eth_shelf::nic_type_t nic_index_tmp = axe_eth_shelf::ETH_UNDEFINED;
	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_nic_status_gep7.nics))
			&& up_to_date; ++i) {
		if (!::strcasecmp(ifa_name, nic_status_data_gep7::_nic_names[i])) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%d eth on passive board: index=%d - Internal NIC: %d == %d retrieved NIC!", ACS_APBM_ARRAY_SIZE(_nic_status_gep7.nics), i, _nic_status_gep7.nics[i], new_nic_info.nics[i]);
			!(up_to_date = (_nic_status_gep7.nics[i] == new_nic_info.nics[i]))
					&& (nic_index_tmp
							= static_cast<axe_eth_shelf::nic_type_t> (i));
		}
	}
	nic_index = nic_index_tmp;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) %s on passive board of interface %d", ( (up_to_date) ? "No needed update of nic_status_data_gep7" : "Needed update of nic_status_data_gep7"), nic_index );

	return up_to_date;
}

bool acs_apbm_monitoringservicehandler::check_valid_nic_data_on_passive(const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index, const virtual_nic_status_data &new_nic_info){
	bool up_to_date = true;

	if(_forced_nic_update) { return (up_to_date = false);}

	axe_eth_shelf::nic_type_t nic_index_tmp = axe_eth_shelf::ETH_UNDEFINED;

	for (size_t i = 0; (i < ACS_APBM_ARRAY_SIZE(_vnic_status.nics)) && up_to_date; ++i)
	{
		if (!::strcasecmp(ifa_name, virtual_nic_status_data::_nic_names[i]))
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%d eth on passive board: index=%d - Internal NIC: %d == %d retrieved NIC!", ACS_APBM_ARRAY_SIZE(_vnic_status.nics), i, _vnic_status.nics[i], new_nic_info.nics[i]);
			!(up_to_date = (_vnic_status.nics[i] == new_nic_info.nics[i])) && (nic_index_tmp = static_cast<axe_eth_shelf::nic_type_t> (i));
		}
	}
	nic_index = nic_index_tmp;
	if(up_to_date)
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) - No needed update of virtual_nic_status_data on passive board");
	else
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call check_valid_nic_data(..) - Needed update of virtual_nic_status_data on passive board of interface %d", nic_index );

	return up_to_date;
}


int acs_apbm_monitoringservicehandler::update_nic_data(
		const nic_status_data &new_nic_info) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status.nics); ++i)
		_nic_status.nics[i] = new_nic_info.nics[i];

	return 0;
}
int acs_apbm_monitoringservicehandler::update_nic_data(
		const nic_status_data_10g &new_nic_info) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status_10g.nics); ++i)
		_nic_status_10g.nics[i] = new_nic_info.nics[i];

	return 0;
}
int acs_apbm_monitoringservicehandler::update_nic_data(
		const nic_status_data_gep7 &new_nic_info) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status_gep7.nics); ++i)
		_nic_status_gep7.nics[i] = new_nic_info.nics[i];

	return 0;
}

int acs_apbm_monitoringservicehandler::update_nic_data (const virtual_nic_status_data &new_nic_info) {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_vnic_status.nics); ++i) _vnic_status.nics[i] = new_nic_info.nics[i];

	return 0;
}
int acs_apbm_monitoringservicehandler::update_nic_data_on_passive(
		const axe_eth_shelf::nic_type_t nic_index,
		const nic_status_data &new_nic_info) {

	int result = 0;

	if (_forced_nic_update) {
		_nic_status.reset_nic_status();
		_forced_nic_update = false;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset info...the next monitoring will update the information");
		return result;
	}
	_nic_status.nics[nic_index] = new_nic_info.nics[nic_index];

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Transmitting update to the other server...");
	char nic_name_tmp[acs_apbm::PCP_FIELD_SIZE_NIC_NAME] = { 0 };
	strncpy(nic_name_tmp, _nic_status._nic_names[nic_index],
			acs_apbm::PCP_FIELD_SIZE_NIC_NAME);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Interface %s: Status=%d, Ipv4 Address:%s, MAC address:%s",
			nic_name_tmp,
			new_nic_info.nics[nic_index],
			new_nic_info.ipv4_addresses[nic_index],
			new_nic_info.mac_addresses[nic_index]);

	int error_code = 0;

	result
			= acs_apbm_shelvesdatamanager::set_other_ap_board_network_interface_card_info(
					error_code, new_nic_info.nics[nic_index], nic_name_tmp,
					new_nic_info.ipv4_addresses[nic_index],
					new_nic_info.mac_addresses[nic_index]);

	if (result != 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error: Call 'set_other_ap_board_network_interface_card_info(...) failed!' - "
				"the interface status will be reset in order to retry in the next check - result :%d",result);
		_nic_status.nics[nic_index] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
		_forced_nic_update = true;
	}
	return result;

}
int acs_apbm_monitoringservicehandler::update_nic_data_on_passive(
		const axe_eth_shelf::nic_type_t nic_index,
		const nic_status_data_10g &new_nic_info) {

	int result = 0;

	if (_forced_nic_update) {
		_nic_status_10g.reset_nic_status();
		_forced_nic_update = false;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset info...the next monitoring will update the information");
		return result;
	}

	_nic_status_10g.nics[nic_index] = new_nic_info.nics[nic_index];

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Transmitting update to the other server...");
	char nic_name_tmp[acs_apbm::PCP_FIELD_SIZE_NIC_NAME] = { 0 };
	strncpy(nic_name_tmp, _nic_status_10g._nic_names[nic_index],
			acs_apbm::PCP_FIELD_SIZE_NIC_NAME);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Interface %s: Status=%d, Ipv4 Address:%s, MAC address:%s",
			nic_name_tmp,
			new_nic_info.nics[nic_index],
			new_nic_info.ipv4_addresses[nic_index],
			new_nic_info.mac_addresses[nic_index]);

	int error_code = 0;
	result
			= acs_apbm_shelvesdatamanager::set_other_ap_board_network_interface_card_info(
					error_code, new_nic_info.nics[nic_index], nic_name_tmp,
					new_nic_info.ipv4_addresses[nic_index],
					new_nic_info.mac_addresses[nic_index]);

	if (result != 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error: Call 'set_other_ap_board_network_interface_card_info(...) failed!' - "
				"the interface status will be reset in order to retry in the next check - result :%d",result);
		_nic_status_10g.nics[nic_index] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
		_forced_nic_update = true;
	}
	return result;
}
int acs_apbm_monitoringservicehandler::update_nic_data_on_passive(
		const axe_eth_shelf::nic_type_t nic_index,
		const nic_status_data_gep7 &new_nic_info) {

	int result = 0;

	if (_forced_nic_update) {
		_nic_status_gep7.reset_nic_status();
		_forced_nic_update = false;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset info...the next monitoring will update the information");
		return result;
	}

	_nic_status_gep7.nics[nic_index] = new_nic_info.nics[nic_index];

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Transmitting update to the other server...");
	char nic_name_tmp[acs_apbm::PCP_FIELD_SIZE_NIC_NAME] = { 0 };
	strncpy(nic_name_tmp, _nic_status_gep7._nic_names[nic_index],
			acs_apbm::PCP_FIELD_SIZE_NIC_NAME);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Interface %s: Status=%d, Ipv4 Address:%s, MAC address:%s",
			nic_name_tmp,
			new_nic_info.nics[nic_index],
			new_nic_info.ipv4_addresses[nic_index],
			new_nic_info.mac_addresses[nic_index]);

	int error_code = 0;
	result
			= acs_apbm_shelvesdatamanager::set_other_ap_board_network_interface_card_info(
			error_code, new_nic_info.nics[nic_index], nic_name_tmp,
			new_nic_info.ipv4_addresses[nic_index],
			new_nic_info.mac_addresses[nic_index]);

	if (result != 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error: Call 'set_other_ap_board_network_interface_card_info(...) failed!' - "
			"the interface status will be reset in order to retry in the next check - result :%d",result);
		_nic_status_gep7.nics[nic_index] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
		_forced_nic_update = true;
	}
	return result;
}

int acs_apbm_monitoringservicehandler::update_nic_data_on_passive(const axe_eth_shelf::nic_type_t nic_index, const virtual_nic_status_data &new_nic_info){

	int result = 0;

	if(_forced_nic_update) {
		_vnic_status.reset_nic_status();
		_forced_nic_update = false;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset info...the next monitoring will update the information");
	return result;
	}

	_vnic_status.nics[nic_index] = new_nic_info.nics[nic_index];

/*	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Transmitting update to the other server...");
	char nic_name_tmp[acs_apbm::PCP_FIELD_SIZE_NIC_NAME] = {0};
	strncpy(nic_name_tmp, _vnic_status._nic_names[nic_index], acs_apbm::PCP_FIELD_SIZE_NIC_NAME); 

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Interface %s: Status=%d, Ipv4 Address:%s, MAC address:%s",
			nic_name_tmp,
			new_nic_info.nics[nic_index],
			new_nic_info.ipv4_addresses[nic_index],
			new_nic_info.mac_addresses[nic_index]);

	int error_code = 0;
	result = acs_apbm_shelvesdatamanager::set_other_ap_board_network_interface_card_info(
			error_code,
			new_nic_info.nics[nic_index],
			nic_name_tmp,
			new_nic_info.ipv4_addresses[nic_index],
			new_nic_info.mac_addresses[nic_index]);

	if (result != 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error: Call 'set_other_ap_board_network_interface_card_info(...) failed!' - "
				"the interface status will be reset in order to retry in the next check - result :%d",result);
		_vnic_status.nics[nic_index] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
		_forced_nic_update = true;
	} */
	return result;
}

int acs_apbm_monitoringservicehandler::handle_timeout(
		const ACE_Time_Value & /*current_time*/, const void * /*act*/) {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Monitoring Service: periodic check ");
	(_server_working_set->program_state
			== acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE) && do_checks();

	(_server_working_set->program_state
			== acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE)
			&& do_checks_on_passive();

	return 0;
}
int acs_apbm_monitoringservicehandler::handle_close(ACE_HANDLE /*handle*/,
		ACE_Reactor_Mask /*close_mask*/) {
	// Here we cannot destroy this object because it should be kept alive  for next opening
	return 0;
}
int acs_apbm_monitoringservicehandler::open() {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Opening Monitoring Service ...");


	ACE_Time_Value delay(_initial_delay);
	ACE_Time_Value interval(_interval);
	int call_result = _server_working_set->main_reactor->schedule_timer(this,
			0, delay, interval);
	if (call_result != -1)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Monitoring Service successfully opened");
		else
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Monitoring Service open failed ! Call 'ACE_Reactor::schedule_timer(..)' returned %d !", call_result);

	return (call_result != -1) ? 0 : -1;
}
int acs_apbm_monitoringservicehandler::close() {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Closing Monitoring Service ...");

	if (_operational_state == acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Signaling closure to active side. Resetting nic information ...");
		acs_apbm_shelvesdatamanager::set_other_ap_board_network_interface_card_info(
				acs_apbm::PCP_ERROR_CODE_RESET_NIC_INFO,
				axe_eth_shelf::NIC_STATUS_UNDEFINED, "empty", "0.0.0.0",
				"00:00:00:00:00:00");
		//reset_nic_data_passive();// to update in internal structure variable _nic_status	
	}
	int n_cancelled = _server_working_set->main_reactor->cancel_timer(this); // Second parameter == 1 (DEFAULT) --> Do not call the handle_close method
	if (n_cancelled == 1)
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Monitoring Service successfully closed");
		else
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Monitoring Service seems to be already closed !");

	return 0;
}
acs_apbm_monitoringservicehandler::checks_result acs_apbm_monitoringservicehandler::do_checks() {
	if (__sync_add_and_fetch(&_monitoring_ongoing, 1) > 1) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "A monitoring DO_CHECK operation is already ongoing...skipping now!");
		__sync_sub_and_fetch(&_monitoring_ongoing, 1);
		return MONITORING_SERVICE_CHECKS_OK;
	}
	// Get APG shelf architecture (SCB, SCX, DMX, VIRTUALIZED, ...)
	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");  // info currently not available. May be a temporary condition.
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	// perform NIC checks (for each NIC, we have to check that:  1] interface is UP; 2] link status is "active", i.e. carrier is detected )
	if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		if (do_virtual_nic_checks()) ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_virtual_nic_checks' failed!");
	}
	else if(_server_working_set->shelves_data_manager->is_hardwaretype_gep7()) 
	{
		if (do_nic_checks_gep7()) ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_nic_checks_gep7' failed!");
	}
	else if(_server_working_set->shelves_data_manager->is_10G())
	{
		if (do_nic_checks_10g())
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_nic_checks_10g' failed!");
	} else {
		if (do_nic_checks())
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_nic_checks' failed!");
	}

	/* perform DATA DISK redundancy checks (RAID or DBRD checks, depending on the technology used to implement data redundancy).
	 * Such checks shouldn't be performed in VIRTUAL environment (because in that case DATA REDUNDANCY should be assured by
	 * CLOUD INFRASTRUCTURE), but they are still necessary in AXE16A project, due to some limitations of ECS15A.
	 */
	// if(env != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	//{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Next is do_raid_or_drbd_checks");

	int data_disk_replication_type = check_dataDisk_replication_type();
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"dataDiskReplicationType value is: %d \n", data_disk_replication_type);

	if (data_disk_replication_type == 2) {
		if (do_drbd_checks(env) < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_drbd_checks' failed!");
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"do_drbd_checks performed");
	} else {
		if (do_raid_checks() < 0)
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_raid_checks' failed!");
	}

	//}

	// perform DISKS availability checks (for NATIVE environment, this means verifying that all the foreseen disks are connected;
	// for VIRTUAL environment it hasn't been yet decided)
	if (do_disk_checks(env) < 0) ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_disk_checks' failed!");


	// perform THUMBDRIVE and SWITCH BOARDS checks, but only for NATIVE environment
	if(env != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{
		// if running on GEP4 or GEP5 hardware perform THUMBDRIVE checks
		acs_apbm_api api;
		if (api.is_gep5_gep7()) {
			if (do_thumbdrive_checks() < 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_thumbdrive_checks' failed!");
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"NOT GEP5/GEP7!");

		// perform SWITCH BOARDS checks (whose details depend on APG shelf architecture)
		int call_result = 0;

		if (env == ACS_CS_API_CommonBasedArchitecture::SCB && (call_result
				= do_switch_boards_check(env)))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_switch_board_checks' failed");

		if (env == ACS_CS_API_CommonBasedArchitecture::DMX && (call_result
				= do_switch_boards_check(env)))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_switch_board_checks' failed");

		if (env == ACS_CS_API_CommonBasedArchitecture::SCX && (call_result
				= do_front_port_check()))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_front_port_check' failed: call_result == %d", call_result);

		if (env == ACS_CS_API_CommonBasedArchitecture::SCX && (call_result
				= do_scx_front_port_operational_status_check()))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_scx_front_port_operational_status_check' failed: call_result == %d", call_result);

		if ((env == ACS_CS_API_CommonBasedArchitecture::SCX || env == ACS_CS_API_CommonBasedArchitecture::SMX ) && (call_result
				= update_switch_board_states()))
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'update_switch_board_states' failed: call_result == %d", call_result);
				
		if ((env == ACS_CS_API_CommonBasedArchitecture::SCX) || (env == ACS_CS_API_CommonBasedArchitecture::SMX)) //CBA architecture
			call_result = do_snmp_link_down_check_scx_or_smx();
		else if (env == ACS_CS_API_CommonBasedArchitecture::DMX)
			call_result = do_ironside_link_down_check_dmx();
		else if(env == ACS_CS_API_CommonBasedArchitecture::SCB)//ACS_CS_API_CommonBasedArchitecture::SCB
			call_result = do_snmp_link_down_check_scbrp();

		if (call_result) {
			if ((env == ACS_CS_API_CommonBasedArchitecture::SCX) || (env == ACS_CS_API_CommonBasedArchitecture::SCX))
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_snmp_link_down_check_scx_or_smx' failed: call_result == %d", call_result);
			else if (env == ACS_CS_API_CommonBasedArchitecture::SCB)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_snmp_link_down_check_scbrp' failed: call_result == %d", call_result);
			else if(env == ACS_CS_API_CommonBasedArchitecture::DMX)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_snmp_link_down_check_dmx' failed: call_result == %d", call_result);
			else if(env == ACS_CS_API_CommonBasedArchitecture::SMX)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_snmp_link_down_check_smx' failed: call_result == %d", call_result);
		}
	}
	__sync_sub_and_fetch(&_monitoring_ongoing, 1);

	return MONITORING_SERVICE_CHECKS_OK;
}

acs_apbm_monitoringservicehandler::checks_result acs_apbm_monitoringservicehandler::do_checks_on_passive() {

	_operational_state = acs_apbm::PROGRAM_STATE_RUNNING_NODE_PASSIVE;

	if (__sync_add_and_fetch(&_monitoring_ongoing, 1) > 1) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "A monitoring DO_CHECK_OM_PASSIVE operation is already ongoing...skipping now!");
		__sync_sub_and_fetch(&_monitoring_ongoing, 1);
		return MONITORING_SERVICE_CHECKS_OK;
	}

	if (--_monitoring_service_on_passive_threshold > 0) { // Repeat monitoring each 9 sec
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Monitoring on passive nics skipped < _monitoring_service_on_passive_threshold == %d >", _monitoring_service_on_passive_threshold);
		__sync_sub_and_fetch(&_monitoring_ongoing, 1);
		return MONITORING_SERVICE_CHECKS_OK;
	}

	_monitoring_service_on_passive_threshold
			= acs_apbm_programconfiguration::monitoring_service_on_passive_threshold;

	if (do_nic_checks_on_passive())
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'do_nic_checks_on_passive' failed!");
	__sync_sub_and_fetch(&_monitoring_ongoing, 1);
	return MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::do_nic_checks() {
	ifaddrs * ifaddr = 0;

	errno = 0;
	if (::getifaddrs(&ifaddr)) { // ERROR: getifaddrs call failed
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "NIC monitoring: call 'getifaddrs' failed");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	nic_status_data if_status_data;

	int physical_separation_active = check_physical_separation_status();

	// int teaming_status = check_teaming_status();

	if (physical_separation_active < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'check_physical_separation_status' failed: cannot retrieve the physical separation status: physical separation status is undefined: "
				"%s interface check is disabled: physical_separation_active == %d", _physical_separation_nic_name, physical_separation_active);
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		if (!::strcasecmp(ifa->ifa_name, _physical_separation_nic_name)) { // For the _physical_separation_nic_name interface the physical separation must be checked
			if (physical_separation_active == 1)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Physical separation is ENABLED");
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Physical separation is %s: %s interface monitoring skipped",
						physical_separation_active ? "UNDEFINED" : "DISABLED", _physical_separation_nic_name);
				if_status_data.set_nic_status(ifa->ifa_name,
						axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				continue;
			}
		}

		// We are interested only in AF_PACKET address family
		if (!ifa->ifa_addr || (ifa->ifa_addr->sa_family != AF_PACKET))
			continue;

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: flags == 0x%08X: interface is%s ACTIVE",
				ifa->ifa_name, ifa->ifa_flags, (ifa->ifa_flags & IFF_UP) ? "" : " NOT");

		if (ifa->ifa_flags & IFF_UP) { // The interface is UP so I have to check its media status
			if_status_data.set_ethernet_interface_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_CONNECTED); //used to handle the TR HQ56362

			char path[1024];
			::snprintf(path, ACS_APBM_ARRAY_SIZE(path),
					"/sys/class/net/%s/carrier", ifa->ifa_name);
			errno = 0;
			if (FILE * f = ::fopen(path, "r")) { // OK: file exists and was opened successfully
				int carrier = -1;

				::fscanf(f, "%5d", &carrier);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: carrier == 0x%08X: there is%s CARRIER on interface",
						ifa->ifa_name, carrier, (carrier == 1) ? "" : " NOT");
				if_status_data.set_nic_status(ifa->ifa_name,
						carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED
								: axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);

				::fclose(f);
			} else { // ERROR: opening file. I report media disconnected
				if_status_data.set_nic_status(ifa->ifa_name,
						axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen' failed: cannot open the file '%s'", path);
			}
		} else { // The interface is down, I report media disconnected
			if_status_data.set_nic_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
			if_status_data.set_ethernet_interface_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED); //used to handle the TR HQ56362
		}
	}

	::freeifaddrs(ifaddr);

	/********************************/
	//used to handle the TR HQ56362
	/********************************/
	if ((if_status_data.ethernet_interface[3]
			== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
			&& if_status_data.ethernet_interface[4]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			|| (if_status_data.nics[3]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
					&& if_status_data.ethernet_interface[4]
							== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)) {

		//WARNING REBOOT SYSTEM
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eht3 and eth4 interfaces are both down!... \n "
				"REBOOTING SYSTEM NOW... \n");
		if (_server_working_set->ha_app_manager->componentReportError(
				ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successfull!");
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
		}
		return MONITORING_SERVICE_CHECKS_OK;
		//  	ACS_PRC_API prcapi;
		//  	prcapi.rebootNode(causedByEvent, "failover due to PRCEVA", true);
		//***END*************************/
	}
	//start of TR HV50762
	if(get_nic_status_data())
	{
		set_nic_status_data(false);
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status.nics); ++i)
			_nic_status.nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
	}
	//end of TR HV50762
	if (check_valid_nic_data(if_status_data)) //No needed update
		return MONITORING_SERVICE_CHECKS_OK;

	update_nic_data(if_status_data); //updating private nic status data

	int32_t slot = -1;
	if (_server_working_set->cs_reader->get_my_slot(slot) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call get_my_slot(..) failed!");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	//handle nic alarm
	_server_working_set->alarmevent_handler->handle_nic_status_data(
			if_status_data, slot);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updating Network Interface Cards: nic0 = %d, nic1 = %d, nic2 = %d, nic3 = %d, nic4 = %d",_nic_status.nics[0], _nic_status.nics[1], _nic_status.nics[2], _nic_status.nics[3], _nic_status.nics[4]);

	if (_server_working_set->shelves_data_manager->update_ap_board_network_interfaces_cards_status(
			_nic_status.nics[0], _nic_status.nics[1], _nic_status.nics[2],
			_nic_status.nics[3], _nic_status.nics[4], true, slot) < 0)
		return MONITORING_SERVICE_CHECKS_ERROR;

	return MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::do_nic_checks_10g() {
	ifaddrs * ifaddr = 0;

	errno = 0;
	if (::getifaddrs(&ifaddr)) {
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "NIC monitoring: call 'getifaddrs' failed");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	nic_status_data_10g if_status_data;

	int physical_separation_active = check_physical_separation_status();

	if (physical_separation_active < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'check_physical_separation_status' failed: cannot retrieve the physical separation status: physical separation status is undefined: "
				"%s interface check is disabled: physical_separation_active == %d", _physical_separation_nic_name, physical_separation_active);
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		if (!::strcasecmp(ifa->ifa_name, _physical_separation_nic_name)) { // For the _physical_separation_nic_name interface the physical separation must be checked
			if (physical_separation_active == 1)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Physical separation is ENABLED");
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Physical separation is %s: %s interface monitoring skipped",
						physical_separation_active ? "UNDEFINED" : "DISABLED", _physical_separation_nic_name);
				if_status_data.set_nic_status(ifa->ifa_name,
						axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				continue;
			}
		}

		// We are interested only in AF_PACKET address family
		if (!ifa->ifa_addr || (ifa->ifa_addr->sa_family != AF_PACKET))
			continue;

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: flags == 0x%08X: interface is%s ACTIVE",
				ifa->ifa_name, ifa->ifa_flags, (ifa->ifa_flags & IFF_UP) ? "" : " NOT");

		if (ifa->ifa_flags & IFF_UP) { // The interface is UP so I have to check its media status
			if_status_data.set_ethernet_interface_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_CONNECTED); //used to handle the TR HQ56362

			char path[1024];
			::snprintf(path, ACS_APBM_ARRAY_SIZE(path),
					"/sys/class/net/%s/carrier", ifa->ifa_name);
			errno = 0;
			if (FILE * f = ::fopen(path, "r")) { // OK: file exists and was opened successfully
				int carrier = -1;

				::fscanf(f, "%5d", &carrier);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: carrier == 0x%08X: there is%s CARRIER on interface",
						ifa->ifa_name, carrier, (carrier == 1) ? "" : " NOT");
				if_status_data.set_nic_status(ifa->ifa_name,
						carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED
								: axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);

				::fclose(f);
			} else { // ERROR: opening file. I report media disconnected
				if_status_data.set_nic_status(ifa->ifa_name,
						axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen' failed: cannot open the file '%s'", path);
			}
		} else { // The interface is down, I report media disconnected
			if_status_data.set_nic_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
			if_status_data.set_ethernet_interface_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED); //used to handle the TR HQ56362
		}
	}

	::freeifaddrs(ifaddr);

	if ((if_status_data.ethernet_interface[axe_eth_shelf::ETH_3]
			== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
			&& if_status_data.ethernet_interface[axe_eth_shelf::ETH_4]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			|| (if_status_data.nics[axe_eth_shelf::ETH_3]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
					&& if_status_data.nics[axe_eth_shelf::ETH_4]
							== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eth3 and eth4 interfaces are both down!... \n REBOOTING SYSTEM NOW... \n");
		if (_server_working_set->ha_app_manager->componentReportError(
				ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successful!");
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
		}
		return MONITORING_SERVICE_CHECKS_OK;
		//  	ACS_PRC_API prcapi;
		//  	prcapi.rebootNode(causedByEvent, "failover due to PRCEVA", true);
	}

	if ((if_status_data.ethernet_interface[axe_eth_shelf::ETH_5]
			== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
			&& if_status_data.ethernet_interface[axe_eth_shelf::ETH_6]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			|| (if_status_data.nics[axe_eth_shelf::ETH_5]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
					&& if_status_data.nics[axe_eth_shelf::ETH_6]
					                       == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)) {
		if (++_10g_nics_down_counter > 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eth5 and eth6 interfaces are both down!... \n");
			axe_eth_shelf::nic_status_t other_nic_5_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
			axe_eth_shelf::nic_status_t other_nic_6_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
			_server_working_set->shelves_data_manager->get_other_ap_board_network_interface_card_status(
					axe_eth_shelf::ETH_5, other_nic_5_status);
			_server_working_set->shelves_data_manager->get_other_ap_board_network_interface_card_status(
					axe_eth_shelf::ETH_6, other_nic_6_status);

			if (other_nic_5_status == axe_eth_shelf::NIC_STATUS_CONNECTED
					&& other_nic_6_status == axe_eth_shelf::NIC_STATUS_CONNECTED) // If the other side is in a stable condition
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "OK. The other side is in sane condition... \n");
				ACS_APBM_LOG(LOG_LEVEL_ERROR, " REBOOTING SYSTEM NOW... \n");
				if (_server_working_set->ha_app_manager->componentReportError(
						ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
					ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successfull!");
				} else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
				}
				return MONITORING_SERVICE_CHECKS_OK;
				//  	ACS_PRC_API prcapi;
				//  	prcapi.rebootNode(causedByEvent, "failover due to PRCEVA", true);
			} else if (other_nic_5_status == axe_eth_shelf::NIC_STATUS_UNDEFINED
					|| other_nic_6_status == axe_eth_shelf::NIC_STATUS_UNDEFINED) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "There is same problem on the other node. It's not possible to send reboot\n");
			}
		}
	}
	else
	{
		_10g_nics_down_counter = 0;
	}
	//start of TR HV50762
	if(get_nic_status_data())
	{
		set_nic_status_data(false);
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status_10g.nics); ++i)
			_nic_status_10g.nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
	}
	//end of TR HV50762

	if (check_valid_nic_data(if_status_data)) //No needed update
		return MONITORING_SERVICE_CHECKS_OK;
	//NOTE: introduced check on bond1 only to hanlde alarm, it don't need to be stored

	update_nic_data(if_status_data); //updating private nic status data

	int32_t slot = -1;
	if (_server_working_set->cs_reader->get_my_slot(slot) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call get_my_slot(..) failed!");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	//handle nic alarm
	_server_working_set->alarmevent_handler->handle_nic_status_data(
			if_status_data, slot);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updating Network Interface Cards: nic0 = %d, nic1 = %d, nic2 = %d, nic3 = %d, nic4 = %d, nic5 = %d, nic6 = %d", _nic_status_10g.nics[0], _nic_status_10g.nics[1], _nic_status_10g.nics[2], _nic_status_10g.nics[3], _nic_status_10g.nics[4], _nic_status_10g.nics[5], _nic_status_10g.nics[6]);

	if (_server_working_set->shelves_data_manager->update_ap_board_network_interfaces_cards_status(
			_nic_status_10g.nics[0], _nic_status_10g.nics[1],
			_nic_status_10g.nics[2], _nic_status_10g.nics[3],
			_nic_status_10g.nics[4], _nic_status_10g.nics[5],
			_nic_status_10g.nics[6], true, slot) < 0)
		return MONITORING_SERVICE_CHECKS_ERROR;

	return MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::do_virtual_nic_checks() {
	ifaddrs * ifaddr = 0;

	errno = 0;
	if (::getifaddrs(&ifaddr)) {
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "NIC monitoring: call 'getifaddrs' failed");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	virtual_nic_status_data if_status_data;

	int physical_separation_active = check_physical_separation_status();

	if (physical_separation_active < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'check_physical_separation_status' failed: cannot retrieve the physical separation status: physical separation status is undefined: "
				"%s interface check is disabled: physical_separation_active == %d", _physical_separation_nic_name, physical_separation_active);
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		if (!::strcasecmp(ifa->ifa_name, _physical_separation_nic_name)) { // For the _physical_separation_nic_name interface the physical separation must be checked
			if (physical_separation_active == 1) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Physical separation is ENABLED");
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Physical separation is %s: %s interface monitoring skipped",
						physical_separation_active ? "UNDEFINED" : "DISABLED", _physical_separation_nic_name);
				if_status_data.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				continue;
			}
		}

		// We are interested only in AF_PACKET address family
		if (!ifa->ifa_addr || (ifa->ifa_addr->sa_family != AF_PACKET)) continue;

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: flags == 0x%08X: interface is%s ACTIVE",
				ifa->ifa_name, ifa->ifa_flags, (ifa->ifa_flags & IFF_UP) ? "" : " NOT");

		if (ifa->ifa_flags & IFF_UP) { // The interface is UP so I have to check its media status
			if_status_data.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_CONNECTED); //used to handle the TR HQ56362

			char path [1024];
			::snprintf(path, ACS_APBM_ARRAY_SIZE(path), "/sys/class/net/%s/carrier", ifa->ifa_name);
			errno = 0;
			if (FILE * f = ::fopen(path, "r")) { // OK: file exists and was opened successfully
				int carrier = -1;

				::fscanf(f, "%5d", &carrier);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: carrier == 0x%08X: there is%s CARRIER on interface",
						ifa->ifa_name, carrier, (carrier == 1) ? "" : " NOT");
				if_status_data.set_nic_status(ifa->ifa_name, carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED : axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);

				::fclose(f);
			} else { // ERROR: opening file. I report media disconnected
				if_status_data.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen' failed: cannot open the file '%s'", path);
			}
		} else { // The interface is down, I report media disconnected
			if_status_data.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
			if_status_data.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED); //used to handle the TR HQ56362
		}
	}

	::freeifaddrs(ifaddr);

/*	if ((if_status_data.ethernet_interface[axe_eth_shelf::ETH_3] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED &&
			if_status_data.ethernet_interface[axe_eth_shelf::ETH_4] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			||
			(if_status_data.nics[axe_eth_shelf::ETH_3] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED &&
					if_status_data.nics[axe_eth_shelf::ETH_4] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED) )
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eth3 and eth4 interfaces are both down!... \n REBOOTING SYSTEM NOW... \n");
		if (_server_working_set->ha_app_manager->componentReportError(ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successful!");
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
		}
		return MONITORING_SERVICE_CHECKS_OK;
	} */

	/*
	if (if_status_data.ethernet_interface[axe_eth_shelf::ETH_5] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED ||
		if_status_data.nics[axe_eth_shelf::ETH_5] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eth5 interface is down!... \n");
		axe_eth_shelf::nic_status_t other_nic_5_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
		_server_working_set->shelves_data_manager->get_other_ap_board_network_interface_card_status(axe_eth_shelf::ETH_5, other_nic_5_status);

		if(other_nic_5_status == axe_eth_shelf::NIC_STATUS_CONNECTED) // If the other side is in a stable condition
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "OK. The other side is in sane condition... \n");
			ACS_APBM_LOG(LOG_LEVEL_ERROR, " REBOOTING SYSTEM NOW... \n");
			if (_server_working_set->ha_app_manager->componentReportError(ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successfull!");
			} else {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
			}
			return MONITORING_SERVICE_CHECKS_OK;
		} else if (other_nic_5_status == axe_eth_shelf::NIC_STATUS_UNDEFINED) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "There is same problem on the other node. It's not possible to send reboot\n");
		}
	}

	if (if_status_data.ethernet_interface[axe_eth_shelf::ETH_6] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED ||
		if_status_data.nics[axe_eth_shelf::ETH_6] == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eth6 interface is down!... \n");
		axe_eth_shelf::nic_status_t other_nic_6_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
		_server_working_set->shelves_data_manager->get_other_ap_board_network_interface_card_status(axe_eth_shelf::ETH_6, other_nic_6_status);

		if(other_nic_6_status == axe_eth_shelf::NIC_STATUS_CONNECTED) // If the other side is in a stable condition
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "OK. The other side is in sane condition... \n");
			ACS_APBM_LOG(LOG_LEVEL_ERROR, " REBOOTING SYSTEM NOW... \n");
			if (_server_working_set->ha_app_manager->componentReportError(ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successfull!");
			} else {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
			}
			return MONITORING_SERVICE_CHECKS_OK;
		} else if (other_nic_6_status == axe_eth_shelf::NIC_STATUS_UNDEFINED) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "There is same problem on the other node. It's not possible to send reboot\n");
		}
	}
	 */
	if (check_valid_nic_data(if_status_data)) //No needed update
		return MONITORING_SERVICE_CHECKS_OK;

	update_nic_data(if_status_data); //updating private nic status data

/*	int32_t slot = -1;
	if (_server_working_set->cs_reader->get_my_slot(slot) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call get_my_slot(..) failed!");
		return MONITORING_SERVICE_CHECKS_ERROR;
	} */

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updating Network Interface Cards: vnic0 = %d, vnic1 = %d, vnic2 = %d, vnic3 = %d, vnic4 = %d, vnic5 = %d, vnic6 = %d, vnic7 = %d, vnic8 = %d",
			_vnic_status.nics[axe_eth_shelf::ETH_0], _vnic_status.nics[axe_eth_shelf::ETH_1], _vnic_status.nics[axe_eth_shelf::ETH_2],
			_vnic_status.nics[axe_eth_shelf::ETH_3], _vnic_status.nics[axe_eth_shelf::ETH_4], _vnic_status.nics[axe_eth_shelf::ETH_5],
			_vnic_status.nics[axe_eth_shelf::ETH_6], _vnic_status.nics[axe_eth_shelf::ETH_7], _vnic_status.nics[axe_eth_shelf::ETH_8]);

/*	if(_server_working_set->shelves_data_manager->update_ap_board_network_interfaces_cards_status(
			_vnic_status.nics[axe_eth_shelf::ETH_0], _vnic_status.nics[axe_eth_shelf::ETH_1], _vnic_status.nics[axe_eth_shelf::ETH_2],
			_vnic_status.nics[axe_eth_shelf::ETH_3], _vnic_status.nics[axe_eth_shelf::ETH_4], _vnic_status.nics[axe_eth_shelf::ETH_5],
			_vnic_status.nics[axe_eth_shelf::ETH_6], _vnic_status.nics[axe_eth_shelf::ETH_7], _vnic_status.nics[axe_eth_shelf::ETH_8],
			true, slot) < 0) {
		return MONITORING_SERVICE_CHECKS_ERROR; 
	} */

	return MONITORING_SERVICE_CHECKS_OK;
}
int acs_apbm_monitoringservicehandler::do_nic_checks_gep7() {
	ifaddrs * ifaddr = 0;

	errno = 0;
	if (::getifaddrs(&ifaddr)) {
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "NIC monitoring: call 'getifaddrs' failed");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	nic_status_data_gep7 if_status_data;

	int physical_separation_active = check_physical_separation_status();

	if (physical_separation_active < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'check_physical_separation_status' failed: cannot retrieve the physical separation status: physical separation status is undefined: "
				"%s interface check is disabled: physical_separation_active == %d", _physical_separation_nic_name, physical_separation_active);
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		if (!::strcasecmp(ifa->ifa_name, _physical_separation_nic_name)) { // For the _physical_separation_nic_name interface the physical separation must be checked
			if (physical_separation_active == 1)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Physical separation is ENABLED");
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Physical separation is %s: %s interface monitoring skipped",
						physical_separation_active ? "UNDEFINED" : "DISABLED", _physical_separation_nic_name);
				if_status_data.set_nic_status(ifa->ifa_name,
						axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				continue;
			}
		}

		// We are interested only in AF_PACKET address family
		if (!ifa->ifa_addr || (ifa->ifa_addr->sa_family != AF_PACKET))
			continue;

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: flags == 0x%08X: interface is%s ACTIVE",
				ifa->ifa_name, ifa->ifa_flags, (ifa->ifa_flags & IFF_UP) ? "" : " NOT");

		if (ifa->ifa_flags & IFF_UP) { // The interface is UP so I have to check its media status
			if_status_data.set_ethernet_interface_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_CONNECTED); //used to handle the TR HQ56362

			char path[1024];
			::snprintf(path, ACS_APBM_ARRAY_SIZE(path),
					"/sys/class/net/%s/carrier", ifa->ifa_name);
			errno = 0;
			if (FILE * f = ::fopen(path, "r")) { // OK: file exists and was opened successfully
				int carrier = -1;

				::fscanf(f, "%5d", &carrier);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: carrier == 0x%08X: there is%s CARRIER on interface",
						ifa->ifa_name, carrier, (carrier == 1) ? "" : " NOT");
				if_status_data.set_nic_status(ifa->ifa_name,
						carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED
								: axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);

				::fclose(f);
			} else { // ERROR: opening file. I report media disconnected
				if_status_data.set_nic_status(ifa->ifa_name,
						axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen' failed: cannot open the file '%s'", path);
			}
		} else { // The interface is down, I report media disconnected
			if_status_data.set_nic_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
			if_status_data.set_ethernet_interface_status(ifa->ifa_name,
					axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED); //used to handle the TR HQ56362
		}
	}

	::freeifaddrs(ifaddr);

	if ((if_status_data.ethernet_interface[axe_eth_shelf::ETH_3]
			== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
			&& if_status_data.ethernet_interface[axe_eth_shelf::ETH_4]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			|| (if_status_data.nics[axe_eth_shelf::ETH_3]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
					&& if_status_data.nics[axe_eth_shelf::ETH_4]
							== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eth3 and eth4 interfaces are both down!... \n REBOOTING SYSTEM NOW... \n");
		if (_server_working_set->ha_app_manager->componentReportError(
				ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successful!");
		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
		}
		return MONITORING_SERVICE_CHECKS_OK;
		//  	ACS_PRC_API prcapi;
		//  	prcapi.rebootNode(causedByEvent, "failover due to PRCEVA", true);
	}

	if ((if_status_data.ethernet_interface[axe_eth_shelf::ETH_5]
			== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
			&& if_status_data.ethernet_interface[axe_eth_shelf::ETH_6]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)
			|| (if_status_data.nics[axe_eth_shelf::ETH_5]
					== axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED
					&& if_status_data.nics[axe_eth_shelf::ETH_6]
					                       == axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED)) {
//		if (++_10g_nics_down_counter > 0)
//		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Warning the status eth5 and eth6 interfaces are both down!... \n");
			axe_eth_shelf::nic_status_t other_nic_5_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
			axe_eth_shelf::nic_status_t other_nic_6_status = axe_eth_shelf::NIC_STATUS_UNDEFINED;
			_server_working_set->shelves_data_manager->get_other_ap_board_network_interface_card_status(
					axe_eth_shelf::ETH_5, other_nic_5_status);
			_server_working_set->shelves_data_manager->get_other_ap_board_network_interface_card_status(
					axe_eth_shelf::ETH_6, other_nic_6_status);

			if (other_nic_5_status == axe_eth_shelf::NIC_STATUS_CONNECTED
					&& other_nic_6_status == axe_eth_shelf::NIC_STATUS_CONNECTED) // If the other side is in a stable condition
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "OK. The other side is in sane condition... \n");
				ACS_APBM_LOG(LOG_LEVEL_ERROR, " REBOOTING SYSTEM NOW... \n");
				if (_server_working_set->ha_app_manager->componentReportError(
						ACS_APGCC_NODE_FAILOVER) == ACS_APGCC_SUCCESS) {
					ACS_APBM_LOG(LOG_LEVEL_INFO, "Call to HA 'componentReportError' successfull!");
				} else {
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call to HA 'componentReportError' failed!");
				}
				return MONITORING_SERVICE_CHECKS_OK;
				//  	ACS_PRC_API prcapi;
				//  	prcapi.rebootNode(causedByEvent, "failover due to PRCEVA", true);
			} else if (other_nic_5_status == axe_eth_shelf::NIC_STATUS_UNDEFINED
					|| other_nic_6_status == axe_eth_shelf::NIC_STATUS_UNDEFINED) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "There is same problem on the other node. It's not possible to send reboot\n");
			}
//		}
	}
	else
	{
//		_10g_nics_down_counter = 0;
	}
	//start of TR HV50762
	if(get_nic_status_data())
	{
		set_nic_status_data(false);
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status_gep7.nics); ++i)
			_nic_status_gep7.nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
	}
	//end of TR HV50762

	if (check_valid_nic_data(if_status_data)) //No needed update
		return MONITORING_SERVICE_CHECKS_OK;
	//NOTE: introduced check on bond1 only to hanlde alarm, it don't need to be stored

	update_nic_data(if_status_data); //updating private nic status data

	int32_t slot = -1;
	if (_server_working_set->cs_reader->get_my_slot(slot) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call get_my_slot(..) failed!");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	//handle nic alarm
	_server_working_set->alarmevent_handler->handle_nic_status_data(
			if_status_data, slot);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updating Network Interface Cards:  nic2 = %d, nic3 = %d, nic4 = %d, nic5 = %d, nic6 = %d", _nic_status_gep7.nics[2], _nic_status_gep7.nics[3], _nic_status_gep7.nics[4], _nic_status_gep7.nics[5], _nic_status_gep7.nics[6]);

	if (_server_working_set->shelves_data_manager->update_ap_board_network_interfaces_cards_status(
			_nic_status_gep7.nics[2], _nic_status_gep7.nics[3],
			_nic_status_gep7.nics[4], _nic_status_gep7.nics[5],
			_nic_status_gep7.nics[6], true, slot) < 0)
		return MONITORING_SERVICE_CHECKS_ERROR;

	return MONITORING_SERVICE_CHECKS_OK;
}


int acs_apbm_monitoringservicehandler::do_nic_checks_on_passive() {
	ifaddrs * ifaddr = 0;
	errno = 0;

	if (::getifaddrs(&ifaddr)) {
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "NIC monitoring: call 'getifaddrs' failed");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}
	nic_status_data if_status_data;
	nic_status_data_10g if_status_data_10g;
	nic_status_data_gep7 if_status_data_gep7;
	virtual_nic_status_data if_status_data_vapg;

	int env = -1;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	bool is_virtual_env = (env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED);

	bool is_10g_system = _server_working_set->shelves_data_manager->is_10G();
	bool is_hwtype_gep7 = _server_working_set->shelves_data_manager->is_hardwaretype_gep7();

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Checking nics on%s 10G board",(is_10g_system) ? "" : " NOT");

	if(is_virtual_env)
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Checking nics in virtualized environment");
	else
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Checking nics on%s 10G board",(is_10g_system) ? "" : " NOT");

	int physical_separation_active = check_physical_separation_status();
	if (physical_separation_active < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"Call 'check_physical_separation_status' failed: cannot retrieve the physical separation status: physical separation status is undefined: "
				"%s interface check is disabled: physical_separation_active == %d", _physical_separation_nic_name, physical_separation_active);
	}
	int32_t slot = -1;
	if (_server_working_set->cs_reader->get_my_slot(slot) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call get_my_slot(..) failed!");
		::freeifaddrs(ifaddr);
		return MONITORING_SERVICE_CHECKS_ERROR;
	}
	if(is_virtual_env) {
		int result = retrieve_network_info(ifaddr, if_status_data_vapg);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'retrieve_network_info' on virtualized environment!. Result: %d",result);
	}
	else if(is_hwtype_gep7) {
		int result = retrieve_network_info(ifaddr, if_status_data_gep7);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'retrieve_network_info' on gep7 structure!. Result: %d",result);
	}
	else if(is_10g_system) {
		int result = retrieve_network_info(ifaddr, if_status_data_10g);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'retrieve_network_info' on 10g structure!. Result: %d",result);

	}
	else {
		int result = retrieve_network_info(ifaddr, if_status_data);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'retrieve_network_info'!. Result: %d",result);
	}

	for (ifaddrs * ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		char carrier_path[1024];
		::snprintf(carrier_path, ACS_APBM_ARRAY_SIZE(carrier_path),
				"/sys/class/net/%s/carrier", ifa->ifa_name);
		int family = ifa->ifa_addr->sa_family;
		if (!::strcasecmp(ifa->ifa_name, _physical_separation_nic_name))// For the _physical_separation_nic_name interface the physical separation must be checked
		{

			if (physical_separation_active == 1)
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Physical separation is ENABLED");
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Physical separation is %s: %s interface monitoring skipped",
						physical_separation_active ? "UNDEFINED" : "DISABLED", _physical_separation_nic_name);

				if(is_virtual_env)
					if_status_data_vapg.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				else if(is_hwtype_gep7)
					if_status_data_gep7.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				else if(is_10g_system)
					if_status_data_10g.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				else
					if_status_data.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);

				axe_eth_shelf::nic_type_t nic_index = axe_eth_shelf::ETH_UNDEFINED;
				if ((is_virtual_env && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data_vapg))
						||
						(!is_virtual_env && is_hwtype_gep7 && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data_gep7))
						||
						(!is_virtual_env && !is_hwtype_gep7 && is_10g_system && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data_10g))
						||
						(!is_virtual_env && !is_10g_system && !is_hwtype_gep7 && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data)))
				{
					ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No need to update internal structure of physical separation!");
					continue;
				}
				//updating private nic status data
				if(is_virtual_env)
					update_nic_data_on_passive(nic_index, if_status_data_vapg);
				else if(is_hwtype_gep7)
					update_nic_data_on_passive(nic_index, if_status_data_gep7);
				else if(is_10g_system)
					update_nic_data_on_passive(nic_index, if_status_data_10g);
				else
					update_nic_data_on_passive(nic_index, if_status_data);

				continue;
			}
		}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%s  address family: %d%s \n",
				ifa->ifa_name, family,
				(family == AF_PACKET) ? " (AF_PACKET)" :
				(family == AF_INET) ? " (AF_INET)" :
				(family == AF_INET6) ? " (AF_INET6)" : "");

		if (!ifa->ifa_addr || (ifa->ifa_addr->sa_family != AF_PACKET))
			continue;

		switch (family) {
		case AF_PACKET:
			/*NIC STATUS*/
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "NIC monitoring: NIC %s: flags == 0x%08X: interface is%s ACTIVE",
					ifa->ifa_name, ifa->ifa_flags, (ifa->ifa_flags & IFF_UP) ? "" : " NOT");
			if (ifa->ifa_flags & IFF_UP) // The interface is UP so I have to check its media status
			{

				if(is_virtual_env)
					if_status_data_vapg.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_CONNECTED);
				else if(is_hwtype_gep7)
					if_status_data_gep7.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_CONNECTED);
				else if(is_10g_system)
					if_status_data_10g.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_CONNECTED); //used to handle the TR HQ56362
				else
					if_status_data.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_CONNECTED); //used to handle the TR HQ56362


				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "setting status up!");
				errno = 0;
				if (FILE * f = ::fopen(carrier_path, "r")) // OK: file exists and was opened successfully
				{
					int carrier = -1;
					::fscanf(f, "%5d", &carrier);

					if(is_virtual_env)
						if_status_data_vapg.set_nic_status(ifa->ifa_name, carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED : axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					else if(is_hwtype_gep7)
						if_status_data_gep7.set_nic_status(ifa->ifa_name, carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED : axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					else if(is_10g_system)
						if_status_data_10g.set_nic_status(ifa->ifa_name, carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED : axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					else
						if_status_data.set_nic_status(ifa->ifa_name, carrier == 1 ? axe_eth_shelf::NIC_STATUS_CONNECTED : axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);

					::fclose(f);
				} else // ERROR: opening file. I report media disconnected
				{
					//if(!is_10g_system) if_status_data.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					//if(is_10g_system) if_status_data_10g.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					if(is_virtual_env)
						if_status_data_vapg.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					else if(is_hwtype_gep7)
                                                if_status_data_gep7.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					else if(is_10g_system)
						if_status_data_10g.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					else
						if_status_data.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);

					ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Call 'fopen' failed: cannot open the file '%s'", carrier_path);
				}
			} else // The interface is down, I report media disconnected
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "setting status down!");

				if(is_virtual_env) {
					if_status_data_vapg.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					if_status_data_vapg.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				}
				else if(is_hwtype_gep7) {
					if_status_data_gep7.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED); //used to handle the TR HQ56362}
					if_status_data_gep7.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				}
				else if(is_10g_system) {
					if_status_data_10g.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED); //used to handle the TR HQ56362}
					if_status_data_10g.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				}
				else {
					if_status_data.set_ethernet_interface_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
					if_status_data.set_nic_status(ifa->ifa_name, axe_eth_shelf::NIC_STATUS_MEDIA_DISCONNECTED);
				}
			}
			/*NIC STATUS END*/
			break;
		}
		//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "retrieve_network_info%s",ifa->ifa_name);
		axe_eth_shelf::nic_type_t nic_index = axe_eth_shelf::ETH_UNDEFINED;
		if ((is_virtual_env && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data_vapg))
				||
                                (!is_virtual_env && is_hwtype_gep7 && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data_gep7))
				||
				(!is_virtual_env && !is_hwtype_gep7 && is_10g_system && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data_10g))
				||
				(!is_virtual_env && !is_10g_system && !is_hwtype_gep7 && check_valid_nic_data_on_passive(ifa->ifa_name, nic_index, if_status_data))) //No needed update
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No need to update internal structure!");
			continue;
		}
		//NOTE: introduced check on bond1 only to hanlde alarm, it don't need to be stored

		//TODO - NOTE: aggiustare le strutture di bond1 nello shelvesdatamanager

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Updating internal structure!");
		is_virtual_env && (nic_index != axe_eth_shelf::ETH_UNDEFINED) && update_nic_data_on_passive(nic_index, if_status_data_vapg); //updating private nic status data
		!is_virtual_env && !is_hwtype_gep7 && !is_10g_system && (nic_index != axe_eth_shelf::ETH_UNDEFINED) && update_nic_data_on_passive(nic_index, if_status_data); //updating private nic status data
		!is_virtual_env && is_hwtype_gep7 && (nic_index != axe_eth_shelf::ETH_UNDEFINED) && update_nic_data_on_passive(nic_index, if_status_data_gep7); //updating private nic status data
		!is_virtual_env && !is_hwtype_gep7 && is_10g_system && (nic_index != axe_eth_shelf::ETH_UNDEFINED) && update_nic_data_on_passive(nic_index, if_status_data_10g); //updating private nic status data

		//handle nic alarm ( but not in virtualized environment)
		!is_virtual_env && !is_hwtype_gep7 && !is_10g_system && _server_working_set->alarmevent_handler->handle_nic_status_data(if_status_data, slot);
		!is_virtual_env && is_hwtype_gep7 && _server_working_set->alarmevent_handler->handle_nic_status_data(if_status_data_gep7, slot);
		!is_virtual_env && !is_hwtype_gep7 && is_10g_system && _server_working_set->alarmevent_handler->handle_nic_status_data(if_status_data_10g, slot);
	}

	::freeifaddrs(ifaddr);

	return MONITORING_SERVICE_CHECKS_OK;
}
acs_apbm_monitoringservicehandler::checks_result acs_apbm_monitoringservicehandler::do_drbd_checks(int env) {
	FILE * fs_drbd = 0;// Check if we are in the right node state to proceed
	char tmp[512] = { 0 };
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND //CNI 33/6-532
	int current_rebuild_state = 0; //no rebuilding
#endif

	if (_server_working_set->program_state
			!= acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE)
		return MONITORING_SERVICE_CHECKS_OK;
	const char * cmd =
			"drbdadm cstate drbd1";
	const char * cmd1 =
			"drbdadm dstate drbd1";
	int drbd_state = axe_eth_shelf::DRBD_STATUS_UNDEFINED;
	int drbd_connection_status = axe_eth_shelf::DRBD_STATUS_UNDEFINED;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"executing command: %s",cmd);
	fs_drbd = popen(cmd, "r");
	if (fs_drbd <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error popen(..) failed! No execution of command: %s",cmd);
		drbd_state = -1; //return 0;
		::pclose(fs_drbd);
		return MONITORING_SERVICE_CHECKS_ERROR;
	}
	fscanf(fs_drbd, "%18s", tmp);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"fscanf found string: %s !\n", tmp);
	if (strcmp(tmp, "Connected") == 0) {
		drbd_state = axe_eth_shelf::DRBD_STATUS_CONNECTED;
		drbd_connection_status = axe_eth_shelf::DRBD_STATUS_CONNECTED;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"OK! DRBD status: %s\n", tmp);
	}
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND  //CNI 33/6-532
	else if (strcmp(tmp, "SyncSource") == 0) {
		current_rebuild_state = 1;  // raid rebuilding ongoing
		drbd_state = axe_eth_shelf::DRBD_STATUS_FAILED;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"OK! DRBD status: %s Rebuilding ongoing\n", tmp);
	}
#endif
	else {
		drbd_state = axe_eth_shelf::DRBD_STATUS_FAILED; // DRBD status is WFConnection
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"OK! DRBD status: %s\n", tmp);
	}
	::pclose(fs_drbd);
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND  //CNI 33/6-532
	if(current_rebuild_state != _raid_rebuild_state ){
		int osCaching_value;
		(current_rebuild_state == 1) && (osCaching_value = 1) && (_raid_rebuild_state == 0 ) &&
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - found raid rebuilding on going (current_rebuild_state == %d).\n",current_rebuild_state);
		(current_rebuild_state == 0) && (osCaching_value = 0) && (_raid_rebuild_state == 1 ) &&
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - raid rebuilding completed!");

		if(_server_working_set->imm_repository_handler->update_osCaching(osCaching_value) == acs_apbm::ERR_NO_ERRORS ){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - osCachingEnabled attribute has been set to '%d'.", osCaching_value );
			// update raid 0rebuild state
			_raid_rebuild_state = current_rebuild_state;
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - Call 'enable_osChaching failed! osCachingEnabled attribute has not been set.");

	}
	else{
		(current_rebuild_state == 1) && ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - raid rebuilding is still on going !");
	}
#endif

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"executing command: %s",cmd1);
	fs_drbd = popen(cmd1, "r");
	if (fs_drbd <= 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error popen(..) failed! No execution of command: %s",cmd1);
		drbd_state = -1; //return 0;
		::pclose(fs_drbd);
		return MONITORING_SERVICE_CHECKS_ERROR;
	}
	fscanf(fs_drbd, "%18s", tmp);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"fscanf found string: %s !\n", tmp);

	if (strcmp(tmp, "UpToDate/UpToDate") == 0) {
		drbd_state = axe_eth_shelf::DRBD_STATUS_UPTODATE;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"OK! DRBD status: %s\n", tmp);
	}
	::pclose(fs_drbd);

	axe_eth_shelf::drbd_status_t drbd_status =
			static_cast<axe_eth_shelf::drbd_status_t> (drbd_state);
	axe_eth_shelf::drbd_status_t drbd_conn_status =
			static_cast<axe_eth_shelf::drbd_status_t> (drbd_connection_status);

	//Fix for TR - HR86342
	if ((_drbd_status == drbd_status) && (_drbd_connection_status == drbd_conn_status)) //Update not needed
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No need to update drbd status!");
		return MONITORING_SERVICE_CHECKS_OK;
	}
	_drbd_status = drbd_status; //Updating drbd status
	_drbd_connection_status = drbd_conn_status; //Updating drbd connection status

	if (drbd_connection_status == axe_eth_shelf::DRBD_STATUS_CONNECTED) {
		if (drbd_status == axe_eth_shelf::DRBD_STATUS_UPTODATE) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Ceasing DRBD alarm!");
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DRBD, -1);
		} else {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Raising DRBD alarm!");
			_server_working_set->alarmevent_handler->raise_alarm(
					acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DRBD, -1);
		}
	} else {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Raising DRBD alarm!");
		_server_working_set->alarmevent_handler->raise_alarm(
				acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DRBD, -1);
	}

	if(env != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
	{ 

		std::list<int32_t> slots;
		uint16_t fbn = ACS_CS_API_HWC_NS::FBN_APUB;
		_server_working_set->shelves_data_manager->search_ap_boards_by_fbn(slots,
			fbn);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found %d slot position", slots.size());

		list<int32_t>::iterator it;
		for (it = slots.begin(); it != slots.end(); ++it) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found disk at slot position : %d ", *it);
			if (_server_working_set->shelves_data_manager->update_ap_board_drbd_status(
				drbd_status, true, *it) < 0)
				return MONITORING_SERVICE_CHECKS_ERROR;
		}
	}
	return MONITORING_SERVICE_CHECKS_OK;
}

acs_apbm_monitoringservicehandler::checks_result acs_apbm_monitoringservicehandler::do_raid_checks() {
	FILE * fs_raid;// Check if we are in the right node state to proceed

	if (_server_working_set->program_state
			!= acs_apbm::PROGRAM_STATE_RUNNING_NODE_ACTIVE)
		return MONITORING_SERVICE_CHECKS_OK;

	const char * cmd = "mdadm --detail /dev/md0";
	char tmp[512] = { 0 };
	char str[32] = { 0 };
	char str2[32] = { 0 };
//	int raid_state = -1;
	int raid_state_a = -1;
	int raid_state_b= -1;

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	const char * rebuilding_pattern1 = "spare";
	const char * rebuilding_pattern2 = "rebuilding";
	char rebuilding_device[32] = {0};
	int current_rebuild_state = 0;  //NO rebuild on going
	char pattern1_str[32] = {0};
	char pattern2_str[32] = {0};
	int p_pattern2 = -1;
	int p_device_name = -1;
#endif
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"executing command: %s",cmd);
	fs_raid = popen(cmd, "r");
	if (fs_raid < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error popen(..) failed! No execution of command: %s",cmd);
	//	raid_state = 0; //return 0;
		raid_state_a = 0; //return 0;
		raid_state_b = 0;

		::pclose(fs_raid);
		return MONITORING_SERVICE_CHECKS_ERROR;
	}
	int p3 = -1;
	int p2 = -1;
	int active_device = -1;
	for (int i = 0; (::fscanf(fs_raid, "%20s", tmp)) != EOF; i++) {
		(strcmp("Active", tmp) == 0) && (p2 = i + 1) && (p3 = i + 3)
				&& (::strcpy(str, tmp));
		(p2 == i) && ::strcpy(str2, tmp); // 'p2' save position of "devices" to be sure of string "Active Devices"
		(p3 == i) && (active_device = ::atoi(tmp));//::strcpy(str3, tmp); // 'p3' save position of active devices number

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
		(strcmp(rebuilding_pattern1,tmp) == 0) && (p_pattern2 = i + 1) && (::strcpy(pattern1_str, tmp));
		(p_pattern2 == i) && (strcmp(rebuilding_pattern2, tmp) == 0) && (p_device_name = i + 1) && (::strcpy(pattern2_str, tmp)) && (current_rebuild_state = 1);
		(p_device_name == i) && (strcpy(rebuilding_device, tmp));
#endif
	}
	::pclose(fs_raid);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"fscanf found string: %s %s = %d !\n",str, str2, active_device);
	//::printf("RAID: %s = %d\n",str, active_device);
	if (active_device == 2) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"active devices are 2 \n");

		    raid_state_a = axe_eth_shelf::RAID_STATUS_ACTIVE;
		    raid_state_b = axe_eth_shelf::RAID_STATUS_ACTIVE;
		}
		else if (active_device == 1) {
			int fdisk;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"active devices are 1:\n");
			fdisk = disk_finder();
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"fdisk value  %d \n",fdisk );

			if (fdisk == axe_eth_shelf::DISK_A) {
				raid_state_a= axe_eth_shelf::RAID_STATUS_NOT_ACTIVE;
				raid_state_b = axe_eth_shelf::RAID_STATUS_ACTIVE;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"raid_state_a%d:\n",raid_state_a);
			}
			if (fdisk == axe_eth_shelf::DISK_B) {
			raid_state_b = axe_eth_shelf::RAID_STATUS_NOT_ACTIVE;
			raid_state_a = axe_eth_shelf::RAID_STATUS_ACTIVE;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"raid_state_b%d:\n",raid_state_b);
			}
		}
		else if (active_device == 0) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"active devices are 0:\n");
			raid_state_a = axe_eth_shelf::RAID_STATUS_NOT_ACTIVE;
			raid_state_b = axe_eth_shelf::RAID_STATUS_NOT_ACTIVE;
		}		
		else {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"active devices are some thing wrong:\n");
			raid_state_a= axe_eth_shelf::RAID_STATUS_UNDEFINED;
			raid_state_b = axe_eth_shelf::RAID_STATUS_UNDEFINED;
		}

#ifndef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
/*	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Raid status: %d\n", (active_device == 0 || active_device == 1 ) ? (raid_state = axe_eth_shelf::RAID_STATUS_NOT_ACTIVE) :
			(active_device == 2 ) ? (raid_state = axe_eth_shelf::RAID_STATUS_ACTIVE) : (raid_state = axe_eth_shelf::RAID_STATUS_UNDEFINED) );*/
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Raid status: %d Raid status1: %d:\n", raid_state_a, raid_state_b);
#else
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Raid status: %d Raid status1: %d rebuilding: %d\n", raid_state_a, raid_state_b, current_rebuild_state);
	/*ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Raid status: %d  rebuilding: %d\n", (active_device == 0 || active_device == 1 ) ? (raid_state = axe_eth_shelf::RAID_STATUS_NOT_ACTIVE) :
				(active_device == 2 ) ? (raid_state = axe_eth_shelf::RAID_STATUS_ACTIVE) : (raid_state = axe_eth_shelf::RAID_STATUS_UNDEFINED), current_rebuild_state );*/
	if(current_rebuild_state != _raid_rebuild_state ){
		int osCaching_value;
		(current_rebuild_state == 1) && (osCaching_value = 1) && (_raid_rebuild_state == 0 ) &&
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - found raid rebuilding on going (current_rebuild_state == %d device state == %s %s dev_name == %s).\n",current_rebuild_state, pattern1_str, pattern2_str, rebuilding_device );
		(current_rebuild_state == 0) && (osCaching_value = 0) && (_raid_rebuild_state == 1 ) &&
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - raid rebuilding completed!");



		const char* raid_mgmt = "raidmgmt --list |grep sd |awk -F : '{print $2}'| awk '{print $1}'";

		FILE* raid_mgmt_file = popen(raid_mgmt, "r");
		char readLine[10];
	    string dev_path = "/dev/";
		string dev_list;
		size_t newbuflen;
		if (raid_mgmt_file) {
			/* Read the output a line at a time and store it. */
			while (fgets(readLine, sizeof(readLine) - 1, raid_mgmt_file) != 0) {

				newbuflen = strlen(readLine);
				if ( (readLine[newbuflen - 1] == '\r') || (readLine[newbuflen - 1] == '\n') ) {
					readLine[newbuflen - 1] = '\0';
				}
				dev_list += dev_path;
				dev_list += readLine;
				dev_list += " ";
			}
			pclose(raid_mgmt_file);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - raidmgmt devList<%s>", dev_list.c_str());

		} else {
			ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - raidmgmt command not be executed!");
		}

		string output;
		const int max_buffer = 100;
		char buffer[max_buffer];
		if (osCaching_value == 1){
			//Enable the write caching parameter
			ostringstream sdparm_set_stream;
			sdparm_set_stream << "sdparm --set WCE=1 "  << dev_list.c_str() << " 2>&1";
			string sdparm_set(sdparm_set_stream.str());
			ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - sdparm command to set WCE %s", sdparm_set.c_str());
			//Enable the write caching parameter
			//const char * sdparm_set = "sdparmr --set WCE=1  /dev/sdd1 /dev/sdc1";
			FILE* sdparm_set_file = popen(sdparm_set.c_str(), "r");

			if (sdparm_set_file) {
				while (!feof(sdparm_set_file))
				if (fgets(buffer, max_buffer, sdparm_set_file) != NULL) output.append(buffer);
				pclose(sdparm_set_file);
				if ((output.find("command not found")!=std::string::npos)||(output.find("error")!=std::string::npos))
					ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - Write caching parameter not enabled: %s!", output.c_str());
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Write caching parameter enabled!");
			}else {
				ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - Write caching parameter not enabled: sdparm command not executed!");
			}

			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Write caching parameter enabled: %s!", output.c_str());

		}else {
			//Disable the write caching parameter
			ostringstream sdparm_unset_stream;
			sdparm_unset_stream << "sdparm --set WCE=0 "  << dev_list.c_str()<< " 2>&1";
			string sdparm_unset(sdparm_unset_stream.str());
			//Disable the write caching parameter
			//const char * sdparm_unset = "sdparm --set WCE=0  /dev/sdd1 /dev/sdc1";
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - sdparm command to unset WCE %s", sdparm_unset.c_str());

			FILE* sdparm_unset_file = popen(sdparm_unset.c_str(), "r");

			if (sdparm_unset_file) {
				while (!feof(sdparm_unset_file))
				if (fgets(buffer, max_buffer, sdparm_unset_file) != NULL) output.append(buffer);
				pclose(sdparm_unset_file);
				if ((output.find("command not found")!=std::string::npos)||(output.find("error")!=std::string::npos))
					ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - Write caching parameter not disabled: %s!", output.c_str());
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Write caching parameter disabled!");
			}else {
				ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - Write caching parameter not disabled: sdparm command not executed!");
			}

			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - Write caching parameter disabled: %s!", output.c_str());

		}



		if(_server_working_set->imm_repository_handler->update_osCaching(osCaching_value) == acs_apbm::ERR_NO_ERRORS ){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - osCachingEnabled attribute has been set to '%d'.", osCaching_value );
			// update raid 0rebuild state
			_raid_rebuild_state = current_rebuild_state;
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_WARN,"RAID MONITORING - Call 'enable_osChaching failed! osCachingEnabled attribute has not been set.");
	}
	else{
		(current_rebuild_state == 1) && ACS_APBM_LOG(LOG_LEVEL_DEBUG,"RAID MONITORING - raid rebuilding is still on going !");
	}

#endif

	axe_eth_shelf::raid_status_t raid_status = static_cast<axe_eth_shelf::raid_status_t> (raid_state_a);

	axe_eth_shelf::raid_status_t raid_status1 = static_cast<axe_eth_shelf::raid_status_t> (raid_state_b);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"is_raid_updated= %d  _raid_status= %d _raid_status1=%d raid_status=%d raid_status1=%d",_server_working_set->shelves_data_manager->is_raid_updated,_raid_status,_raid_status1,raid_status,raid_status1);
 
	if (!_server_working_set->shelves_data_manager->is_raid_updated
			&& ((_raid_status == raid_status) && (_raid_status1 == raid_status1))) //No needed update
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No needed raid status update!");
		return MONITORING_SERVICE_CHECKS_OK;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Updating raid status!");
	_raid_status = raid_status; //Updating raid status for DISKA
	_raid_status1 = raid_status1; //Updating raid status for DISKB
	if (raid_status == axe_eth_shelf::RAID_STATUS_NOT_ACTIVE || raid_status1 == axe_eth_shelf::RAID_STATUS_NOT_ACTIVE) {
		_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_RAID, -1);
	} else if (raid_status == axe_eth_shelf::RAID_STATUS_ACTIVE && raid_status1 == axe_eth_shelf::RAID_STATUS_ACTIVE) {
		_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_RAID, -1);
	}

	std::list<int32_t> slots;
	uint16_t fbn = ACS_CS_API_HWC_NS::FBN_Disk;
	_server_working_set->shelves_data_manager->search_ap_boards_by_fbn(slots,
			fbn);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found %d slot position", slots.size());
	if (slots.size() < 2) { //0 or 1 disk configured
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Found 0 or 1 disk configured, could be a configuration problem! Resetting update");
		_raid_status = axe_eth_shelf::RAID_STATUS_UNDEFINED;
		_raid_status1 = axe_eth_shelf::RAID_STATUS_UNDEFINED;
	}
	slots.sort();
	list<int32_t>::iterator it;
	for (it = slots.begin(); it != slots.end(); ++it) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found disk at slot position : %d ", *it);
		/*if (_server_working_set->shelves_data_manager->update_ap_board_raid_status(
				raid_status, true, *it) < 0)
			return MONITORING_SERVICE_CHECKS_ERROR;*/
		if ((_server_working_set->shelves_data_manager->update_ap_board_raid_status(
				raid_status, true, *it) < 0) || (_server_working_set->shelves_data_manager->update_ap_board_raid_status(
						raid_status1, true, *(++it)) < 0)){
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found %d slot position values *it= %d, *(++it) = %d",*it, *(++it));
				return MONITORING_SERVICE_CHECKS_ERROR;
			}
		}
	}
	_server_working_set->shelves_data_manager->is_raid_updated = false; // reset value - to solve misalignment
	return MONITORING_SERVICE_CHECKS_OK;
}

acs_apbm_monitoringservicehandler::checks_result acs_apbm_monitoringservicehandler::do_disk_checks(int env){

	int32_t slot = -1;
	_server_working_set->cs_reader->get_my_slot(slot);

	if(env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
		return do_disk_checks_in_virtual_env(slot);		// disk checks for virtual APG environment

	// disk checks for native APG environments
	unsigned disk_status = 0;

	if (_server_working_set->shelves_data_manager->get_local_ap_board_disks_status(
			disk_status) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error 'get_local_ap_board_disks_status(..)' failed!");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Found %d disk/disks scsi!", disk_status);

	ACS_APGCC_CommonLib commonlib_obj;
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;
	commonlib_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION | ACS_APGCC_GET_DISKCAPACITY );
	
	if(hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Hardware Version %d !", hwInfo.hwVersion);
		switch(hwInfo.hwVersion){
		case ACS_APGCC_HWVER_GEP1:
		case ACS_APGCC_HWVER_GEP2:
			if (disk_status < 4) //Four disks for GEP1 or GEP2
			{
				//start of TR HU60600
				if(!_disk_unavailable_alarm_raised)		//TR HU60600 - do not raise the alarm again if it has been raised already
				{
					_server_working_set->alarmevent_handler->raise_alarm(
							acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISKCONN, slot);
					_disk_unavailable_alarm_raised = true;
				}
			}
			else
			{
				if(_disk_unavailable_alarm_raised)		//TR HU60600 - cease the alarm only if it has been raised previously
				{
					_server_working_set->alarmevent_handler->cease_alarm(
							acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISKCONN, slot);
					_disk_unavailable_alarm_raised = false;
				}
			}
			break;
		case ACS_APGCC_HWVER_GEP5:
		case ACS_APGCC_HWVER_GEP7:
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"HwInfo disksCapacity %d !", hwInfo.disksCapacity);
			if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_400GB){
				if (disk_status < 1) //Only one disk for GEP5/7-400 series
				{
					if(!_disk_unavailable_alarm_raised)
					{
						_server_working_set->alarmevent_handler->raise_alarm(
								acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE,
								slot);
						_disk_unavailable_alarm_raised = true;
					}
				}
				else {
					if(_disk_unavailable_alarm_raised)
					{
						ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Call cease alarm for gep5/7 has  %d disks!", disk_status);
						_server_working_set->alarmevent_handler->cease_alarm(
								acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE,
								slot);
						_disk_unavailable_alarm_raised = false;
					}
				}
			}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_1200GB){
				if (disk_status < 3) //Three disks for GEP5/7-1200 series
				{
					if(!_disk_unavailable_alarm_raised)
					{
						_server_working_set->alarmevent_handler->raise_alarm(
								acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE,
								slot);
						_disk_unavailable_alarm_raised = true;
					}
				}
				else
				{
					if(_disk_unavailable_alarm_raised)
					{
						_server_working_set->alarmevent_handler->cease_alarm(
								acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE,
								slot);
						_disk_unavailable_alarm_raised = false;
					}
					//end of TR HU60600
				}

				}
			else if(hwInfo.disksCapacity == ACS_APGCC_DISKSCAPACITY_1600GB){
				if (disk_status < 2) //Two disks for GEP7-1600 series
				{
					if(!_disk_unavailable_alarm_raised)
					{
						_server_working_set->alarmevent_handler->raise_alarm(
								acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE,
								slot);
						_disk_unavailable_alarm_raised = true;
					}
				}
				else
				{
					if(_disk_unavailable_alarm_raised)
					{
						_server_working_set->alarmevent_handler->cease_alarm(
								acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE,
								slot);
						_disk_unavailable_alarm_raised = false;
					}
				}
			}
                                break;
			case ACS_APGCC_HWVER_VM:
			default:
				break;
			}
		}
		else {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "do_disk_checks() failed as GetHwInfo unable to return correct value <return code == %d> <hwInfo.hwVersion == '%d'>", hwInfoResult.hwVersionResult, hwInfo.hwVersion);
		return MONITORING_SERVICE_CHECKS_ERROR;
		}
	return MONITORING_SERVICE_CHECKS_OK;
}

acs_apbm_monitoringservicehandler::checks_result acs_apbm_monitoringservicehandler::do_disk_checks_in_virtual_env(int apub_board_slot)
{
	// Disk checks for virtual APG environment: start verifying that the data disk device file is present in '/dev' directory ...
	if(::access(acs_apbm_monitoringservicehandler::_datadisk_devname_in_virtual_apgenv, F_OK) == 0)
	{
		// now check that the data disk is really available, trying to access to a specific directory that should be always present
		if(::access(acs_apbm_monitoringservicehandler::_datadisk_directory_to_test_in_virtual_apgenv, F_OK) == 0)
		{
			// OK, the access attempt was successful. Eventually cease the DISK FAULTY alarm, if active
			_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE, apub_board_slot);
		}
		else
		{
			// unable to access to the selected directory. If the failure is due to an input/output error, or the directory is
			// not present, raise DISK FAULTY alarm
			int errno_save = errno;
			ACS_APBM_LOG_ERRNO(errno_save, LOG_LEVEL_WARN, "Unable to access to directory '%s': 'access()' call returned not zero !", acs_apbm_monitoringservicehandler::_datadisk_directory_to_test_in_virtual_apgenv);
			if((errno_save == EIO) || (errno_save == ENOENT))
			{
				_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE, apub_board_slot);
			}
		}
	}
	else
	{	// data disk device file not found in '/dev' ! Raise DISK FAULTY alarm
		ACS_APBM_LOG_ERRNO(errno, LOG_LEVEL_WARN, "Unable to access to data disk device '%s': 'access()' call returned not zero !", acs_apbm_monitoringservicehandler::_datadisk_devname_in_virtual_apgenv);
		_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_DISK_NOT_AVAILABLE, apub_board_slot);
	}

	return MONITORING_SERVICE_CHECKS_OK;
}

//Redesign as per TR-HS30773 
acs_apbm_monitoringservicehandler::checks_result acs_apbm_monitoringservicehandler::do_thumbdrive_checks() {
	axe_eth_shelf::thumbdrive_status_t local_thumbdrive_status =
			axe_eth_shelf::THUMBDRIVE_STATUS_UNDEFINED;
	axe_eth_shelf::thumbdrive_status_t other_thumbdrive_status =
			axe_eth_shelf::THUMBDRIVE_STATUS_UNDEFINED;

	if (_server_working_set->shelves_data_manager->get_local_ap_board_thumbdrive_status(
			local_thumbdrive_status) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error 'get_local_ap_board_thumbdrive_status(..)' failed!");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	if (_server_working_set->shelves_data_manager->get_other_ap_board_thumbdrive_status(
			other_thumbdrive_status) < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error 'get_other_ap_board_thumbdrive_status(..)' failed!");
		return MONITORING_SERVICE_CHECKS_ERROR;
	}

	bool opstate_status;
	int return_opstate;
	return_opstate
			= _server_working_set->shelves_data_manager->getoperationalStateFromIMM();
	if (return_opstate == 1)
		opstate_status = 1;
	else
		opstate_status = 0;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Operational State value is : %d", opstate_status);

	if (((_local_thumbdrive_status == local_thumbdrive_status)
			&& (_other_thumbdrive_status == other_thumbdrive_status))
			&& (_opstate_status == opstate_status)) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"No need to check thumbdrive status!");
		return MONITORING_SERVICE_CHECKS_OK;
	}

	_local_thumbdrive_status = local_thumbdrive_status;
	_other_thumbdrive_status = other_thumbdrive_status;
	_opstate_status = opstate_status;

	if (((local_thumbdrive_status
			== axe_eth_shelf::THUMBDRIVE_STATUS_NOT_AVAILABLE)
			&& (other_thumbdrive_status
					== axe_eth_shelf::THUMBDRIVE_STATUS_NOT_AVAILABLE))
			&& (opstate_status)) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"THUMBDRIVES ARE NOT AVAILABLE IN BOTH THE SLOTS");
		_server_working_set->alarmevent_handler->raise_alarm(
				acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_THUMBDRIVE_MALFUNCTION,
				-1);
	} else {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"NO THUMBDRIVE MALFUNCTION!");
		_server_working_set->alarmevent_handler->cease_alarm(
				acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_THUMBDRIVE_MALFUNCTION,
				-1);
	}
	return MONITORING_SERVICE_CHECKS_OK;
}
//Redesign as per TR-HS30773

int acs_apbm_monitoringservicehandler::do_front_port_check() {
	if (--_frontport_check_threshold > 0) { // Skipping this loop
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCXB front port check skipped < _frontport_check_threshold == %d >%s", _frontport_check_threshold,
				(_frontport_check_threshold < 0 ? ": this is the first check at APBM startup" : ""));
		return MONITORING_SERVICE_CHECKS_OK;
	}

	_frontport_check_threshold
			= acs_apbm_programconfiguration::front_port_check_trigger_threshold;

	const int call_result =
			_server_working_set->snmp_manager->enable_front_port_scx();

	return call_result ? MONITORING_SERVICE_CHECKS_ERROR
			: MONITORING_SERVICE_CHECKS_OK;
}
int acs_apbm_monitoringservicehandler::do_scx_front_port_operational_status_check() {

	ACS_APBM_LOG(LOG_LEVEL_TRACE,"LAG SCX monitoring: ************************************  do_scx_front_port_operational_status_check  Entering ****************************************");
	// Retrieving IP of my switch boards
	std::vector<acs_apbm_switchboardinfo> board_infos;
	_server_working_set->snmp_manager->get_my_switch_board_info(board_infos);

	if (!board_infos.size()) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG SCX monitoring: no switch boards found in my magazine: cannot ping any switch board ");
		return 1;
	}
	acs_apbm_snmp::frontPort_status_t frontPort1_status [2];
	acs_apbm_snmp::frontPort_status_t frontPort2_status [2];

	int frontPort1Partner_status[2];
	int frontPort2Partner_status[2];

	int call_resultPort1 = 0;
	int call_resultPort2 = 0;
	ACS_APBM_LOG(LOG_LEVEL_TRACE,"Current front port flags of SCX-0:: frontPort1_statusCounter[0] = %d, frontPort2_statusCounter[0] = %d, bothFrontPortDown[0] = %d",frontPort1_statusCounter[0],frontPort2_statusCounter[0], bothFrontPortDown[0]);
	ACS_APBM_LOG(LOG_LEVEL_TRACE,"Current front port flags of SCX-25:: frontPort1_statusCounter[1] = %d, frontPort2_statusCounter[1] = %d, bothFrontPortDown[1] = %d",frontPort1_statusCounter[1],frontPort2_statusCounter[1], bothFrontPortDown[1]);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_front_port_flags_mutex);				//TR HX50455

	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG SCX monitoring: Checking Front port status for both switch boards ... no. of switch boards = %d", board_infos.size());
	unsigned timeout_ms = 500;
	for (size_t i = 0; i < board_infos.size(); ++i) {
		if (_server_working_set->snmp_manager->get_lag_admin_state(board_infos[i].slot_position)==acs_apbm_snmp::DISABLE_LAG)
			continue;

        if(disableAdminStatePort1 [i] == false){
		call_resultPort1 = _server_working_set->snmp_manager->get_front_port_operational_status(
			                        			frontPort1_status[i], SCX_LAG_FRONT_PORT1, board_infos[i], &timeout_ms);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
						"LAG SCX monitoring: Front port operational status fetched from SCX at SLOT= %d: FRONT_PORT= %d: PORT_STATUS= %d: call_result= %d",
						board_infos[i].slot_position, SCX_LAG_FRONT_PORT1, frontPort1_status[i], call_resultPort1);

		call_resultPort1 = _server_working_set->snmp_manager->get_front_partner_port_status(
					                        			frontPort1Partner_status[i], SCX_LAG_FRONT_PORT1, board_infos[i], &timeout_ms);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
								"LAG SCX monitoring: Front PARTNER port status fetched from SCX at SLOT= %d: FRONT_PORT= %d: PARTNER_PORT_STATUS= %x: call_result= %d",
								board_infos[i].slot_position, SCX_LAG_FRONT_PORT1, frontPort1Partner_status[i], call_resultPort1);

		if (call_resultPort1 ) {
					ACS_APBM_LOG(LOG_LEVEL_WARN,
							"LAG SCX monitoring: check failed for the SCX at SLOT==%d: call_resultPort1 == %d:",
							board_infos[i].slot_position, call_resultPort1);
        }
		if((frontPort1_status [i]== acs_apbm_snmp::FRONT_PORT_DOWN || !IS_PARTNER_PORT_UP(frontPort1Partner_status[i],4)) && (frontPort1_statusCounter[i] < 3 ))
		  {
			++frontPort1_statusCounter[i];
			ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG SCX monitoring: SCX at slot:%d, front port:%d DOWN attempt= %d",board_infos[i].slot_position,SCX_LAG_FRONT_PORT1,frontPort1_statusCounter[i]);
		  }
		if(frontPort1_status [i]== acs_apbm_snmp::FRONT_PORT_UP && IS_PARTNER_PORT_UP(frontPort1Partner_status[i],4))
		  {
			frontPort1_statusCounter[i] = 0;
		  }
        }

        if(disableAdminStatePort2 [i] == false){
		call_resultPort2 = _server_working_set->snmp_manager->get_front_port_operational_status(
					                        			frontPort2_status [i], SCX_LAG_FRONT_PORT2, board_infos[i], &timeout_ms);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
						"LAG SCX monitoring: Front port operational status fetched from SCX at SLOT== %d: FRONT_PORT== %d: PORT_STATUS == %d: call_result == %d",
						board_infos[i].slot_position, SCX_LAG_FRONT_PORT2, frontPort2_status [i], call_resultPort2);

		call_resultPort2 = _server_working_set->snmp_manager->get_front_partner_port_status(
				                            frontPort2Partner_status[i], SCX_LAG_FRONT_PORT2, board_infos[i], &timeout_ms);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"LAG SCX monitoring: Front PARTNER port status fetched from SCX at SLOT== %d: FRONT_PORT== %d: PARTNER_PORT_STATUS== %x: call_result == %d",
				board_infos[i].slot_position, SCX_LAG_FRONT_PORT2, frontPort2Partner_status[i], call_resultPort1);

		if (call_resultPort2 ) {
					ACS_APBM_LOG(LOG_LEVEL_WARN,
							"LAG SCX monitoring: check failed for the SCX at SLOT==%d: call_resultPort1 == %d:",
							board_infos[i].slot_position, call_resultPort2);
        }
		if((frontPort2_status [i]== acs_apbm_snmp::FRONT_PORT_DOWN  || !IS_PARTNER_PORT_UP(frontPort2Partner_status[i],4)) && (frontPort2_statusCounter[i] < 3))
		  {
			++frontPort2_statusCounter[i];
			ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG SCX monitoring: SCX at slot:%d, front port:%d DOWN attempt= %d",board_infos[i].slot_position,SCX_LAG_FRONT_PORT2,frontPort2_statusCounter[i]);
		  }
		if(frontPort2_status [i]== acs_apbm_snmp::FRONT_PORT_UP && IS_PARTNER_PORT_UP(frontPort2Partner_status[i],4))
		  {
			frontPort2_statusCounter[i] = 0;
		  }
        }

        if(frontPort1_statusCounter[i] == 3 && frontPort2_statusCounter[i] == 3 && ((bothFrontPortDown[i] == false) || (firstLagMonitoringTaskAfterReset[i] == true))){
        	(void)_server_working_set->snmp_manager->set_front_port_scx_lag_module(board_infos[i],acs_apbm_snmp::FRONT_PORT_DOWN);

        	(void)_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT1);
        	(void)_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT2);
        	bothFrontPortDown[i] = true;
        	if(firstLagMonitoringTaskAfterReset[i] == true)			//TR HX50455 - handle alarms when monitoring flags are reset  - special case
        		firstLagMonitoringTaskAfterReset[i] = false;

        	ACS_APBM_LOG(LOG_LEVEL_ERROR,"LAG SCX monitoring: Raise LAG alarms for both 27 and 28 ports as both are DOWN.");
        	continue;
        }

        if( (frontPort1_statusCounter[i] == 0 ) && (frontPort2_statusCounter[i] == 0 ) && ((bothFrontPortDown[i] == true) || (firstLagMonitoringTaskAfterReset[i] == true))){
        	(void)_server_working_set->snmp_manager->set_front_port_scx_lag_module(board_infos[i],acs_apbm_snmp::FRONT_PORT_UP);
        	(void)_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT1);
        	(void)_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT2);
        	bothFrontPortDown[i] = false;
        	if(firstLagMonitoringTaskAfterReset[i] == true)
        		firstLagMonitoringTaskAfterReset[i] = false;
        	ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG SCX monitoring: Cease LAG alarms for both 27 and 28 ports as both are UP.");
        	continue;
        }

        // Raising alarm for port down and making lag down... due to three consecutive attempts ports were in failed state...
        if(frontPort1_statusCounter[i] == 3 && frontPort2_statusCounter[i] == 0 && disableAdminStatePort2 [i] == false)
        {
        	if(bothFrontPortDown[i] == true) {
        		ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG SCX monitoring: Cease LAG alarm for SCX at SLOT== %d: FRONT_PORT== %d:",board_infos[i].slot_position, SCX_LAG_FRONT_PORT2);
        		(void)_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT2);
        	}
        	(void)_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT1);
        	ACS_APBM_LOG(LOG_LEVEL_ERROR,"LAG SCX monitoring: Raise LAG alarm for SCX at SLOT== %d: FRONT_PORT== %d:",board_infos[i].slot_position, SCX_LAG_FRONT_PORT1);
        	(void)_server_working_set->snmp_manager->set_admin_status_front_port_scx(board_infos[i],SCX_LAG_FRONT_PORT2,acs_apbm_snmp::FRONT_PORT_DOWN);
        	(void)_server_working_set->snmp_manager->set_front_port_scx_lag_module(board_infos[i],acs_apbm_snmp::FRONT_PORT_DOWN);
        	disableAdminStatePort2 [i] = true;
        	if(firstLagMonitoringTaskAfterReset[i] == true)
        		firstLagMonitoringTaskAfterReset[i] = false;
        }

        if(frontPort1_statusCounter[i] == 0 && (disableAdminStatePort2 [i] == true ))
        {
        	(void)_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT1);
        	ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG SCX monitoring: Cease LAG alarm for SCX at SLOT== %d: FRONT_PORT== %d:",board_infos[i].slot_position, SCX_LAG_FRONT_PORT1);
        	(void)_server_working_set->snmp_manager->set_admin_status_front_port_scx(board_infos[i],SCX_LAG_FRONT_PORT2,acs_apbm_snmp::FRONT_PORT_UP);
        	(void)_server_working_set->snmp_manager->set_front_port_scx_lag_module(board_infos[i],acs_apbm_snmp::FRONT_PORT_UP);
        	disableAdminStatePort2 [i]= false;
        	if(firstLagMonitoringTaskAfterReset[i] == true)
        		firstLagMonitoringTaskAfterReset[i] = false;
        }

        if(frontPort2_statusCounter[i] == 3 && frontPort1_statusCounter[i] == 0 && disableAdminStatePort1 [i] == false)
        {
        	if(bothFrontPortDown[i] == true){
        		ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG SCX monitoring: Cease LAG alarm for SCX at SLOT== %d: FRONT_PORT== %d:",board_infos[i].slot_position, SCX_LAG_FRONT_PORT1);
        		(void)_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT1);
        	}
        	(void)_server_working_set->alarmevent_handler->raise_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT2);
        	ACS_APBM_LOG(LOG_LEVEL_ERROR,"LAG SCX monitoring: Raise LAG alarm for SCX at SLOT== %d: FRONT_PORT== %d:",board_infos[i].slot_position, SCX_LAG_FRONT_PORT2);
        	(void)_server_working_set->snmp_manager->set_admin_status_front_port_scx(board_infos[i],SCX_LAG_FRONT_PORT1,acs_apbm_snmp::FRONT_PORT_DOWN);
        	(void)_server_working_set->snmp_manager->set_front_port_scx_lag_module(board_infos[i],acs_apbm_snmp::FRONT_PORT_DOWN);
        	disableAdminStatePort1 [i] = true;
        	if(firstLagMonitoringTaskAfterReset[i] == true)
        		firstLagMonitoringTaskAfterReset[i] = false;
        }

        if(frontPort2_statusCounter[i] == 0 && (disableAdminStatePort1 [i] == true))
        {
        	(void)_server_working_set->alarmevent_handler->cease_alarm(acs_apbm_alarminfo::ACS_APBM_ALARMTYPE_LAG_DISABLED, board_infos[i].slot_position,SCX_LAG_FRONT_PORT2);
        	ACS_APBM_LOG(LOG_LEVEL_WARN,"LAG SCX monitoring: Cease LAG alarm for SCX at SLOT== %d: FRONT_PORT== %d:",board_infos[i].slot_position, SCX_LAG_FRONT_PORT2);
        	(void)_server_working_set->snmp_manager->set_admin_status_front_port_scx(board_infos[i],SCX_LAG_FRONT_PORT1,acs_apbm_snmp::FRONT_PORT_UP);
        	(void)_server_working_set->snmp_manager->set_front_port_scx_lag_module(board_infos[i],acs_apbm_snmp::FRONT_PORT_UP);
        	disableAdminStatePort1 [i]= false;
        	if(firstLagMonitoringTaskAfterReset[i] == true)
        		firstLagMonitoringTaskAfterReset[i] = false;
        }
	}
	ACS_APBM_LOG(LOG_LEVEL_TRACE,"LAG SCX monitoring: ************************************  do_scx_front_port_operational_status_check  Leaving ****************************************");
	return (call_resultPort1 & call_resultPort2);
}
int acs_apbm_monitoringservicehandler::do_switch_boards_check(int env) {

	// Retrieving IP of my switch boards

	std::vector<acs_apbm_switchboardinfo> board_infos;
	_server_working_set->snmp_manager->get_my_switch_board_info(board_infos);

	if (!board_infos.size()) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Monitoring: no switch boards found in my magazine: cannot ping any switch board ");
		return MONITORING_SERVICE_CHECKS_OK;
	}

	int call_result = 0;
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Checking Manager Trap Subscription for both switch boards ...%d", board_infos.size());
	unsigned timeout_ms = 500;
	for (size_t i = 0; i < board_infos.size(); ++i) {
		if (env == ACS_CS_API_CommonBasedArchitecture::SCB) {
			call_result = do_switch_board_check(board_infos[i], &timeout_ms);

			if (call_result) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
						" Manager Trap Subscription check failed for the switch board in slot position %d: call_result == %d",
						board_infos[i].slot_position, call_result);
			}
		}
		else if(env == ACS_CS_API_CommonBasedArchitecture::DMX)
		{
			call_result = do_dmxswitch_board_check(board_infos[i], &timeout_ms);

			if (call_result < 0) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
						" Arping failed for the switch board in slot position %d: call_result == %d",
						board_infos[i].slot_position, call_result);
				int64_t current_time = ::clock();
				if (current_time >= 0) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LINK DOWN Detected for Switch Board in slot %d at time %ld", board_infos[i].slot_position, current_time);
					_server_working_set->snmp_manager->set_scb_link_down_time(
							board_infos[i].slot_position,
							board_infos[i].magazine, current_time);
				}
					else
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'time(0) failed: Cannot set last link down time for Switch Board in slot %d <errno == %d>", board_infos[i].slot_position, errno);

			} else {
				_server_working_set->snmp_manager->reset_scb_link_down_time(
						board_infos[i].slot_position, board_infos[i].magazine);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,
						" Arping Success for the switch board in slot position %d: call_result == %d",
						board_infos[i].slot_position, call_result);
			}

		}
	}


#if 0

	// CHECK if IPMI master is assigned in local magazine.
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Checking IPMI master assignment in local magazine...");
	if (const int call_result = _server_working_set->snmp_manager->find_switch_board_master(&timeout_ms)) {
		// ERROR: finding the master switch board
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'find_switch_board_master' failed: finding the master switch board in local magazine! call_result == %d",
				call_result);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IPMI master is not assigned in local magazine! APBM server will try to restart the communication with both switch boards");
		_server_working_set->snmp_manager->initialize_switch_board_state();

		if (const int call_result = _server_working_set->snmp_manager->start_scb_communication()) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'start_scb_communication' failed: starting communication with switch boards in local magazine! call_result == %d",
					call_result);
			// Nothing to do now: APBM will retry to start communication at next monitoring-cycle.
		}
	} else
	ACS_APBM_LOG(LOG_LEVEL_INFO, "IPMI Master found!");

	// TODO: To be continued
	/*
	 for (size_t i = 0; i < board_infos.size(); ++i) {
	 const acs_apbm_switchboardinfo & sb = board_infos[i];
	 }
	 */
#endif

	return MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::do_switch_board_check(
		acs_apbm_switchboardinfo & switch_board, const unsigned * /*timeout_ms*/) {

	int slot_pos = switch_board.slot_position;
	uint32_t magazine = switch_board.magazine;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "do_switch_board_check(slot == %d) - link down counters -> SCB-0 == %d, SCB-25 == %d", slot_pos, _count_scb_left_link_down, _count_scb_right_link_down);

	//SUBSCRIBERCHECK a restart of SCBRP could reset manager trap subscription)//
	unsigned timeout_ms = 500;

	/*
	 if (!_swithcboard_ongoing_check) {
	 _swithcboard_ongoing_check++;
	 */

	if (const int call_result = _server_working_set->snmp_manager->get_managerTrapSubscrIp(switch_board, &timeout_ms)) {
		// ERROR: getting trap manager subscriberIP
		if (call_result == acs_apbm_snmp::ERR_SNMP_TIMEOUT) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_managerTrapSubscrIp' failed: the switch board_%d is probably down <call_result == %d>", slot_pos,
					call_result);
			if(slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_LEFT)
			{
				_count_scb_left_link_down++;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updated link down counter SCB-RP at slot 0 - _count_scb_left_link_down == %d", _count_scb_left_link_down);
			}
			else if(slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_RIGHT)
			{
				_count_scb_right_link_down++;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Updated link down counter SCB-RP at slot 25 - _count_scb_right_link_down == %d", _count_scb_right_link_down);
			}

			if(((slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_LEFT) && (_count_scb_left_link_down >= 2))
					|| ((slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_RIGHT) && (_count_scb_right_link_down >= 2)))
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Switch from master state if needs!");
				_server_working_set->snmp_manager->switch_from_active_state_scbrp(
						switch_board);
				//_server_working_set->snmp_manager->initialize_switch_board_state(slot_pos);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server will try to restart the communication with switch board at next periodic switch_board_check()!");
				//TODO: ALARM SET!!!
			}
			else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP Link is down - but SCB-RP %d link down counter has not reached 2 - do not take action",slot_pos);


			int64_t current_time = ::clock();
			if (current_time >= 0) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LINK DOWN Detected for Switch Board in slot %d at time %ld", slot_pos, current_time);
				_server_working_set->snmp_manager->set_scb_link_down_time(
						slot_pos, magazine, current_time);
			}
			else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Call 'time(0) failed: Cannot set last link down time for Switch Board in slot %d <errno == %d>", slot_pos, errno);

		} else {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP LINK IS UP - but no trap subscriber");

			if(slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_LEFT)
			{
				_count_scb_left_link_down = 0;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset link down counter SCB-RP at slot 0 - _count_scb_left_link_down == %d", _count_scb_left_link_down);
			}
			else if(slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_RIGHT)
			{
				_count_scb_right_link_down = 0;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset link down counter SCB-RP at slot 25 - _count_scb_right_link_down == %d", _count_scb_right_link_down);
			}

			_server_working_set->snmp_manager->reset_scb_link_down_time(
					slot_pos, magazine);

			// ERROR: the switch board has not a trap manager subscriberIP
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'get_managerTrapSubscrIp' failed: no manager trap subscription found for switch board_%d call_result == %d", slot_pos,
					call_result);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server will try to restart the communication with switch board_%d!", slot_pos);

			_server_working_set->snmp_manager->initialize_switch_board_state(slot_pos);
			if (const int call_result = _server_working_set->snmp_manager->start_scb_communication(slot_pos, true)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'start_scb_communication' failed: starting communication with switch board in slot %d <call_result == %d>", slot_pos,
						call_result);
				// Nothing to do now: APBM will retry to start communication at next monitoring-cycle.
			}
		}
	} else {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP LINK IS UP - successfully fetched MgrTrapSubscriberIP");
		if(slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_LEFT)
		{
			_count_scb_left_link_down = 0;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset link down counter SCB-RP at slot 0 - _count_scb_left_link_down == %d", _count_scb_left_link_down);
		}
		else if(slot_pos == axe_eth_shelf::SCBRP_SLOT_POS_RIGHT)
		{
			_count_scb_right_link_down = 0;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reset link down counter SCB-RP at slot 25 - _count_scb_right_link_down == %d", _count_scb_right_link_down);
		}
		_server_working_set->snmp_manager->reset_scb_link_down_time(slot_pos,
				magazine);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "The new switch board in slot %d is %s! <state == %d>!", slot_pos, (switch_board.state ==
						acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN) ? "undefined" : "healthy" , switch_board.state);

		//			if(switch_board.state != acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_ACTIVE){
		//				// CHECK if IPMI master is assigned in local magazine.
		//				ACS_APBM_LOG(LOG_LEVEL_INFO, "Checking IPMI master assignment in local magazine...");
		//				if (const int call_result = _server_working_set->snmp_manager->find_switch_board_master(&timeout_ms)) {
		//					// ERROR: finding the master switch board
		//					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'find_switch_board_master' failed: finding the master switch board in local magazine! call_result == %d",
		//								call_result);
		//
		//					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IPMI master is not assigned in local magazine! APBM server will try to restart the communication with both switch boards");
		//					_server_working_set->snmp_manager->initialize_switch_board_state();
		//
		//					if (const int call_result = _server_working_set->snmp_manager->start_scb_communication()){
		//						ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'start_scb_communication' failed: starting communication with switch boards in local magazine! call_result == %d",
		//										call_result);
		//						// Nothing to do now: APBM will retry to start communication at next monitoring-cycle.
		//					}
		//				}else{
		//					ACS_APBM_LOG(LOG_LEVEL_INFO, "IPMI Master found!");
		if (switch_board.state
				== acs_apbm_snmp::SWITCH_BOARD_SHELF_MGR_STATE_UNKNOWN) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "APBM server will try to restart the communication with switch board_%d in undefined state!", slot_pos);
			_server_working_set->snmp_manager->initialize_switch_board_state(
					slot_pos);
			if (const int call_result = _server_working_set->snmp_manager->start_scb_communication(slot_pos, true)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'start_scb_communication' failed: starting communication with switch board in slot %d <call_result == %d>", slot_pos,
						call_result);
				// Nothing to do now: APBM will retry to start communication at next monitoring-cycle.
			}
			// Nothing to do now: APBM will retry to start communication at next monitoring-cycle.
		}
		//				}
		//			}//else  - this board is the master
	}

	/*
	 _swithcboard_ongoing_check = 0;//reset check
	 } else ACS_APBM_LOG(LOG_LEVEL_INFO, "IPMI Master check skipped!");
	 */

	/*
	 for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(switch_board_ipn_str_addresses); ++i) {

	 call_result = do_switch_board_check(switch_board.slot_position, switch_board_ipn_str_addresses[i], timeout_ms);
	 }
	 */
	// TODO - add keepalive handling.

	return MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::do_switch_board_check(
		int32_t /*switch_board_slot*/, const char * /*switch_board_ip*/,
		const unsigned * /*timeout_ms*/) {
	// ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Monitoring: checking switch board in the slot %d at IP address '%s'", switch_board_slot, switch_board_ip);



	return MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::do_snmp_link_down_check_scbrp() {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Checking SNMP communication status with switch boards in AP magazine ...");
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "link down counters -> SCB-0 == %d, SCB-25 == %d", _count_scb_left_link_down, _count_scb_right_link_down);

	// Retrieving info of switch boards in local magazine
	std::vector<acs_apbm_switchboardinfo> board_infos;
	_server_working_set->snmp_manager->get_my_switch_board_info(board_infos);
	if (!board_infos.size()) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Monitoring: no switch boards found in my magazine: cannot ping any switch board");
		return MONITORING_SERVICE_CHECKS_OK;
	}

	int scb_left_link_down = 0;
	int scb_right_link_down = 0;
	int call_result = 0;
	for (size_t i = 0; i < board_infos.size(); ++i) {
		call_result = check_snmp_link_status(board_infos[i].slot_position,
				board_infos[i].magazine);
		if (call_result == MONITORING_SERVICE_LINK_DOWN_FOUND) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "SNMP link down detected for the switch board in slot position %d, magazine == 0x%08X, _count_scb_left_link_down == %d, _count_scb_right_link_down == %d",
					board_infos[i].slot_position, board_infos[i].magazine, _count_scb_left_link_down, _count_scb_right_link_down);

			if(board_infos[i].slot_position == acs_apbm_programconfiguration::slot_position_switch_board_left)
			{
				if(_count_scb_left_link_down >= 2)
					scb_left_link_down++;
			}
			else if(board_infos[i].slot_position == acs_apbm_programconfiguration::slot_position_switch_board_right)
			{
				if(_count_scb_right_link_down >= 2)
					scb_right_link_down++;
			}
		}
		else
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'check_snmp_link_status()' for switch board in slot position %d - returns call_result == %d  ", board_infos[i].slot_position, call_result);
	}

	// Check which alarm to raise
	if (scb_left_link_down && scb_right_link_down)
		return handle_switch_boards_alarms(MSH_SNMP_LINK_DOWN_BOTH);
	if (scb_left_link_down)
		return handle_switch_boards_alarms(MSH_SNMP_LINK_DOWN_LEFT);
	if (scb_right_link_down)
		return handle_switch_boards_alarms(MSH_SNMP_LINK_DOWN_RIGHT);
	return handle_switch_boards_alarms(MSH_SNMP_LINK_OK);
}

int acs_apbm_monitoringservicehandler::do_snmp_link_down_check_scx_or_smx() {
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Checking SNMP communication status with all switch boards in configuration ...");
	// Retrieving info of  all switch boards
	std::vector<acs_apbm_switchboardinfo> board_infos;
	_server_working_set->snmp_manager->get_all_switch_board_info(board_infos);

	if (!board_infos.size()) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Monitoring: no switch boards found in my magazine: cannot ping any switch board");
		return MONITORING_SERVICE_CHECKS_OK;
	}

	//1. Find all magazine in the current configuration
	std::multimap<uint32_t, acs_apbm_switchboardinfo *>
			mapMagazineToSwitchBoard;
	std::list<uint32_t> magazinesList;
	pair<multimap<uint32_t, acs_apbm_switchboardinfo *>::iterator, multimap<
			uint32_t, acs_apbm_switchboardinfo *>::iterator> map_sublist;

	for (size_t i = 0; i < board_infos.size(); ++i) {
		mapMagazineToSwitchBoard.insert(pair<uint32_t,
				acs_apbm_switchboardinfo *> (board_infos[i].magazine,
				&board_infos[i]));
		magazinesList.push_back(board_infos[i].magazine);
	}
	magazinesList.sort();
	magazinesList.unique();
	ACS_APBM_LOG(LOG_LEVEL_INFO, "Monitoring: found %d magazines in current configuration ", magazinesList.size());

	// 2. For each magazine find the switch boards in the same  magazine
	multimap<uint32_t, acs_apbm_switchboardinfo *>::iterator map_it;
	list<uint32_t>::iterator list_it;
	uint32_t mag_num;
	acs_apbm_switchboardinfo * switchboardinfo_p;
	std::vector<acs_apbm_switchboardinfo *> magazine_switchboards;
	int switch_link_down_count = 0;

	bool both_switch_alarm_found = false;
	bool single_switch_alarm_found = false;

	for (list_it = magazinesList.begin(); list_it != magazinesList.end()
			&& !both_switch_alarm_found; ++list_it) {
		mag_num = *list_it;
		// find the switch boards in same magazine
		map_sublist = mapMagazineToSwitchBoard.equal_range(mag_num);
		magazine_switchboards.clear();
		for (map_it = map_sublist.first; map_it != map_sublist.second; ++map_it) {
			switchboardinfo_p = (*map_it).second;
			magazine_switchboards.push_back(switchboardinfo_p);
		}
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Monitoring: found %d switch boards in magazine == 0x%08X", magazine_switchboards.size(), mag_num);
		// Check Snmp link status for the switch boards in same magazine
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Checking SNMP communication status with switch boards in magazine == 0x%08X ...", mag_num);
		int call_result = 0;
		switch_link_down_count = 0;
		for (size_t i = 0; i < magazine_switchboards.size(); ++i) {
			call_result = check_snmp_link_status(
					magazine_switchboards[i]->slot_position,
					magazine_switchboards[i]->magazine);
			if (call_result == MONITORING_SERVICE_LINK_DOWN_FOUND) {
				ACS_APBM_LOG(LOG_LEVEL_WARN,
						"SNMP link down detected for the switch board in slot position %d magazine == 0x%08X", magazine_switchboards[i]->slot_position, magazine_switchboards[i]->magazine);
				switch_link_down_count++;
			}
				else
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,
						"Call 'check_snmp_link_status() for switch board in slot position %d ' returns call_result == %d  ", magazine_switchboards[i]->slot_position, call_result);
		}
		if (switch_link_down_count == 2)
			both_switch_alarm_found = true;
		if (switch_link_down_count == 1)
			single_switch_alarm_found = true;
	}
	// Check which alarm to raise
	if (both_switch_alarm_found)
		return handle_switch_boards_alarms(MSH_SNMP_LINK_DOWN_BOTH);
	if (single_switch_alarm_found)
		return handle_switch_boards_alarms(MSH_SNMP_LINK_DOWN_LEFT);
	return handle_switch_boards_alarms(MSH_SNMP_LINK_OK);
}

int acs_apbm_monitoringservicehandler::do_ironside_link_down_check_dmx() {

	// Retrieving info of switch boards in local magazine
	std::vector<acs_apbm_switchboardinfo> board_infos;
	_server_working_set->snmp_manager->get_my_switch_board_info(board_infos);
	if (!board_infos.size()) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Monitoring: no switch boards found in my magazine: cannot ping any switch board");
		return MONITORING_SERVICE_CHECKS_OK;
	}

	int dmx_link_down = 0;

	int call_result = 0;
	for (size_t i = 0; i < board_infos.size(); ++i) {
		call_result = check_snmp_link_status(board_infos[i].slot_position,
				board_infos[i].magazine);
		if (call_result == MONITORING_SERVICE_LINK_DOWN_FOUND) {
			ACS_APBM_LOG(LOG_LEVEL_WARN,
					"SNMP link down detected for DMX");

			dmx_link_down++;
		}
			else
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,
					"Call 'check_snmp_link_status() for DMX returns call_result == %d  ", call_result);
	}

	// Check which alarm to raise
	ACS_APBM_LOG(LOG_LEVEL_INFO, "dmx_link_down ===== %d" , dmx_link_down);
	if (dmx_link_down)
		return handle_switch_boards_alarms(MSH_SNMP_LINK_DOWN_BOTH);
	return handle_switch_boards_alarms(MSH_SNMP_LINK_OK);

	return MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::check_snmp_link_status(int slot_pos,
		uint32_t magazine, const unsigned * /*timeout_ms*/) {

	int64_t link_down_time =
			_server_working_set->snmp_manager->get_scb_link_down_time(slot_pos,
					magazine);

	if (link_down_time <= 0)
		return 0;

	int64_t current_time = ::clock();

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,
			"current_time == %ld link_down_time == %ld: ", current_time, link_down_time);

	if (current_time > 0 && ((current_time - link_down_time)
			> acs_apbm_programconfiguration::switch_board_link_down_max_time)) {

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"if-current_time == %ld link_down_time == %ld: ", current_time, link_down_time);

		return MONITORING_SERVICE_LINK_DOWN_FOUND;
	}

	return 0;
}

int acs_apbm_monitoringservicehandler::update_switch_board_states() {
	if (--_update_switchboard_states_threshold > 0) { // Skipping this loop
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"Updating switch board states skipped: _update_switchboard_states_threshold == %d", _update_switchboard_states_threshold);
		return MONITORING_SERVICE_CHECKS_OK;
	}

	_update_switchboard_states_threshold
			= acs_apbm_programconfiguration::update_switchboard_states_trigger_threshold;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Monitoring Periodic Task: START: Updating switch boards states");
	unsigned timeout_ms = 1000;
	const int call_result =
			_server_working_set->snmp_manager->update_all_switch_board_states(
					&timeout_ms);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Monitoring Periodic Task: END: Updating switch boards states");

	return call_result ? MONITORING_SERVICE_CHECKS_ERROR
			: MONITORING_SERVICE_CHECKS_OK;
}

int acs_apbm_monitoringservicehandler::handle_switch_boards_alarms(
		int switch_link_down_type) {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "handle_switch_boards_alarms() - switch_link_down_type == %d", switch_link_down_type);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_alarm_flag_mutex);
	switch (switch_link_down_type) {
	case MSH_SNMP_LINK_OK:
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP link with both switch boards is working");
		if (_both_switch_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with both switch board is not working' ...");
			// cease alarm A1
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_BOTH_SCB);
			_both_switch_alarm_raised = false;
		} else if (_switch_left_alarm_raised) {
			// cease alarm SCB0
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with switch board on the left is not working' ...");
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_LEFT_SCB);
			_switch_left_alarm_raised = false;
		}

		else if (_switch_right_alarm_raised) {
			// cease alarm SCB25
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with switch board on the right is not working' ...");
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_RIGHT_SCB);
			_switch_right_alarm_raised = false;
		}
		break;

	case MSH_SNMP_LINK_DOWN_LEFT:
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP link with one of switch board is not working");
		// 1. cease alarm for both scb if raised
		if (_both_switch_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with both switch board is not working' ...");
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_BOTH_SCB);
			_both_switch_alarm_raised = false;
		}
		// 2. raise alarm for the unreachable scb

		if (_switch_right_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with switch board on the right is not working' ...");
			// cease alarm SCB25
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_RIGHT_SCB);
			_switch_right_alarm_raised = false;
		}
		if (!_switch_left_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Raising alarm 'SNMP link with switch board on the left is not working' ...");
			// 2. raise alarm for the unreachable scb
			_server_working_set->alarmevent_handler->raise_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_LEFT_SCB);
			_switch_left_alarm_raised = true;
		}
		break;

	case MSH_SNMP_LINK_DOWN_RIGHT:
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP link with one of switch board is not working");

		// 1. cease alarm for both scb if raised
		if (_both_switch_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with both switch board is not working' ...");
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_BOTH_SCB);
			_both_switch_alarm_raised = false;
		}

		if (_switch_left_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with switch board on the left is not working' ...");
			// cease alarm SCB0
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_LEFT_SCB);
			_switch_left_alarm_raised = false;
		}
		if (!_switch_right_alarm_raised) {
			// 2. raise alarm for the unreachable scb
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Raising alarm 'SNMP link with switch board on the right is not working' ...");
			_server_working_set->alarmevent_handler->raise_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_RIGHT_SCB);
			_switch_right_alarm_raised = true;
		}
		break;

	case MSH_SNMP_LINK_DOWN_BOTH:
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP link with both switch boards is not working");
		// 1. cease alarm for single scb
		if (_switch_right_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with switch board on the right is not working' ...");
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_RIGHT_SCB);
			_switch_right_alarm_raised = false;
		}
		if (_switch_left_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm 'SNMP link with switch board on the left is not working' ...");
			_server_working_set->alarmevent_handler->cease_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_LEFT_SCB);
			_switch_left_alarm_raised = false;
		}
		// 2. raise alarm for both scb
		if (!_both_switch_alarm_raised) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Raising alarm 'SNMP link with both switch boards is not working' ...");
			_server_working_set->alarmevent_handler->raise_alarm(
					acs_apbm_alarmeventhandler::ACS_APBM_AEH_SNMP_LINK_DOWN_BOTH_SCB);
			_both_switch_alarm_raised = true;
		}
		break;

	default:
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ERROR - cannot handle switch boards alarms: call 'handle_switch_boards_alarms' passing unknown switch_link_down_type (switch_link_down_type == %d)", switch_link_down_type);
		break;
	}
	return 0;
}

int acs_apbm_monitoringservicehandler::do_dmxswitch_board_check(
		acs_apbm_switchboardinfo & switch_board, const unsigned * /*timeout_ms*/) {

	int retVal = 0;
	ARP_PKT *pkt;
	const char *interface[2] = { "eth3", "eth4" };
	int count = 0;

	char * switch_board_ipn_str_addresses[2] = { switch_board.ipna_str,
			switch_board.ipnb_str };
	int index = 0;
	for (index = 0; index < 2; index++) {
		pkt = (ARP_PKT *) malloc(sizeof(ARP_PKT));
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Checking the DMX line for switch_board_ipn_str_addresses[index]  ...");
		acs_apbm_arpping_dmx arp(interface[index],
				switch_board_ipn_str_addresses[index]);

		/*
		 if(!_swithcboard_ongoing_check) {
		 _swithcboard_ongoing_check++;
		 */

		unsigned timeout_ms = 500;

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Getting the Mac address from Interface ...");

		retVal = arp.setSrcIpAddressFromInf();
		if (retVal < 0) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "SetSrcIpAddress failed ...");
			//				_swithcboard_ongoing_check = 0;//reset check
			free(pkt);
			pkt = NULL;
			//return acs_apbm::ERR_DMX_ARPING_ERR;
			continue;
		}
		ACS_APBM_LOG(LOG_LEVEL_INFO, "getting the Source IP Address of the Interface based on Destination ...");

		retVal = arp.getSrcIpAddressFromDest();
		if (retVal < acs_apbm::ERR_NO_ERRORS) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "SetSrcIpAddressfrom Dest failed  ...");
			//				_swithcboard_ongoing_check = 0;//reset check
			free(pkt);
			pkt = NULL;
			continue;
			//return acs_apbm::ERR_DMX_ARPING_ERR;
		}
		ACS_APBM_LOG(LOG_LEVEL_INFO, "creating the arp packet ...");
		arp.createARPRequest(pkt);
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Sending the arp packet ...");

		if (arp.sendARPPing(pkt, timeout_ms)) {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "error in pinging  ...");
			//				_swithcboard_ongoing_check = 0;//reset check
			free(pkt);
			pkt = NULL;
			//return acs_apbm::ERR_DMX_ARPING_ERR;
			ACS_APBM_LOG(LOG_LEVEL_WARN, "error in pinging  ...");
			continue;
		}

		//			_swithcboard_ongoing_check = 0;//reset check

		count++;

		/*
		 } else
		 {
		 ACS_APBM_LOG(LOG_LEVEL_INFO, "IPMI Master check skipped!");
		 }
		 */

		if (pkt) {
			free(pkt);
			pkt = NULL;
		}
	}

	if (pkt) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "error in pinging 4 ...");
		free(pkt);
	}

	if (count == 0) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "ARPing failed !");
		return acs_apbm::ERR_DMX_ARPING_ERR;
	} else
		return MONITORING_SERVICE_CHECKS_OK;
}
int acs_apbm_monitoringservicehandler::check_scx_frontport_lag_admin_status( ) {
	int lag_admin_status[2]={0,0};

	OmHandler omManager;
	acs_apgcc_paramhandling pha;
	ACS_CC_ReturnType result;
	char objtemp[128] = {0};

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,
				"LAG Call 'OmHandler Init' failed: cannot get the class name!");
		return result;
	}/*exiting on IMM init failure: generic error*/

	string class_instance_name[2] ;
	std::vector<std::string> p_dnList;
    result = omManager.getClassInstances("AxeEquipmentLag", p_dnList);
	if (result != ACS_CC_SUCCESS) {

		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG Call 'getClassInstances' failed: cannot get the class name result:%d, resultText:%s",result,omManager.getInternalLastErrorText());
		omManager.Finalize();
		return 0;
	}
	omManager.Finalize();

	const char * attrib_name = "administrativeState";
	int size = p_dnList.size();
	for (int i = 0; i<size;++i){
	class_instance_name[i] = p_dnList[i];

	ACS_CC_ReturnType imm_result = pha.getParameter(
			class_instance_name[i].c_str(), attrib_name, &lag_admin_status[i]);
	if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting parameter value from IMM
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG Call 'getParameter' failed: cannot retrieve the scx forntport lag admin status from IMM: "
				"internal last error == %d: internal laste error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
		return -1;
	}
	::strcpy(objtemp, class_instance_name[i].c_str());
	char * token = ::strtok(objtemp, "=,");
	token = ::strtok(0, "=,");token = ::strtok(0, "=,");token = ::strtok(0, "=,");


		_server_working_set->snmp_manager->set_lag_admin_state(::atoi(token), lag_admin_status[i]);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"LAG scx frontport admin status SLOT:%d,lag_admin_status:%d",::atoi(token), lag_admin_status[i]);
	}
	return result;
}

void acs_apbm_monitoringservicehandler::reset_scx_lag_monitoring_flags_due_to_restart(int slot_num)			//TR HX50455
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_front_port_flags_mutex);
	int indexOfSCXB = -1;

	if(slot_num == axe_eth_shelf::SCBRP_SLOT_POS_LEFT)
		indexOfSCXB = 0;
	else if (slot_num == axe_eth_shelf::SCBRP_SLOT_POS_RIGHT)
		indexOfSCXB = 1;
	else
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"SCX slot number is neither 0 or 25 - Wrong!!! Skip resetting front port monitor flags & counters");

	if(indexOfSCXB != -1)
	{
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Resetting front port monitor flags & counters for SCX-%d",slot_num);
		frontPort1_statusCounter[indexOfSCXB] = 0;
		frontPort2_statusCounter[indexOfSCXB] = 0;
		disableAdminStatePort1[indexOfSCXB] = false;
		disableAdminStatePort2[indexOfSCXB] = false;

		firstLagMonitoringTaskAfterReset[indexOfSCXB] = true;
	}
}

int acs_apbm_monitoringservicehandler::set_scx_lag_monitoring_flags(){
	acs_apbm_snmp::frontPort_status_t frontPort1_status [2];
	acs_apbm_snmp::frontPort_status_t frontPort2_status [2];
	acs_apbm_snmp::frontPort_status_t virtualPort_status [2];
	int call_result = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard (_front_port_flags_mutex);				//TR HX50455

	(void)check_scx_frontport_lag_admin_status();
	std::vector<acs_apbm_switchboardinfo> board_infos;
	_server_working_set->snmp_manager->get_my_switch_board_info(board_infos);

	if (!board_infos.size()) { // WARNING: No switch board found in my magazine
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG SCX startup: no switch boards found in my magazine: cannot ping any switch board ");
		return 1;
	}
	ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG SCX startup: Checking Front port status for both switch boards ...%d", board_infos.size());

	unsigned timeout_ms = 500;
	for (size_t i = 0; i < board_infos.size(); ++i) {
		if (_server_working_set->snmp_manager->get_lag_admin_state(board_infos[i].slot_position)==acs_apbm_snmp::DISABLE_LAG)
			continue;

		call_result = _server_working_set->snmp_manager->get_front_port_admin_status(frontPort1_status[i], SCX_LAG_FRONT_PORT1, board_infos[i], &timeout_ms);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"LAG SCX startup: Front port ADMIN STATE fetched from SCX at SLOT= %d: FRONT_PORT= %d: PORT_STATUS= %d: call_result= %d",
				board_infos[i].slot_position, SCX_LAG_FRONT_PORT1, frontPort1_status[i], call_result);
		if( frontPort1_status[i] ==  acs_apbm_snmp::FRONT_PORT_DOWN ){
			disableAdminStatePort1[i] = true;
		}

		call_result = _server_working_set->snmp_manager->get_front_port_admin_status(frontPort2_status[i], SCX_LAG_FRONT_PORT1, board_infos[i], &timeout_ms);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"LAG SCX startup: Front port ADMIN STATE fetched from SCX at SLOT= %d: FRONT_PORT= %d: PORT_STATUS= %d: call_result= %d",
				board_infos[i].slot_position, SCX_LAG_FRONT_PORT2, frontPort2_status[i], call_result);

		if( frontPort2_status[i] ==  acs_apbm_snmp::FRONT_PORT_DOWN ){
			disableAdminStatePort2[i] = true;
		}
		call_result = _server_working_set->snmp_manager->get_front_port_admin_status(virtualPort_status[i], SCX_LAG_VIRTUAL_PORT, board_infos[i], &timeout_ms);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"LAG SCX startup: Front port ADMIN STATE fetched from SCX at SLOT= %d: FRONT_PORT= %d: PORT_STATUS= %d: call_result= %d",
				board_infos[i].slot_position, SCX_LAG_VIRTUAL_PORT, virtualPort_status[i], call_result);

		if( frontPort1_status[i] ==  acs_apbm_snmp::FRONT_PORT_UP &&  frontPort2_status[i] ==  acs_apbm_snmp::FRONT_PORT_UP){
			if( virtualPort_status[i] ==  acs_apbm_snmp::FRONT_PORT_DOWN ){
				bothFrontPortDown [i] = true;
			}
		}
		if(call_result){
			call_result = _server_working_set->snmp_manager->configure_front_port_scx_lag(board_infos[i].slot_position,acs_apbm_snmp::ENABLE_LAG);
			ACS_APBM_LOG(LOG_LEVEL_INFO, "LAG SCX startup: Trying to re-configure LAG on SCXB slot == %d ... ", board_infos[i].slot_position);

			if(call_result != 0)
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG SCX startup: re-configuring LAG on SCX-%d failed call_result: %d", board_infos[i].slot_position, call_result);
			else
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "LAG SCX startup: re-configuring LAG on SCX-%d successful", board_infos[i].slot_position);
		}
	}

	return call_result;
}

int acs_apbm_monitoringservicehandler::disk_finder()
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"ENtering disk_finder");
	FILE * fp;
	size_t  len = 0;
	int length = -1;
	size_t pos = 0;
	ssize_t read;
	string diskA;
	string diskB;
	char *line = NULL;
	int fdisk_status = -1;
	ACS_APGCC_HwVer_ReturnTypeT returnCode;
	char hwVariant[64];
	int p_Len = 64;

	ACS_APGCC_CommonLib commonlib_obj;
	returnCode = commonlib_obj.GetHWVariant(hwVariant, p_Len);

	if (returnCode == ACS_APGCC_HWVER_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Hardware Version %s !", hwVariant);
	} else {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "do_disk_checks() failed as GetHWVersion unable to return correct value <return code == %d> <hwVariant == '%s'>", returnCode, hwVariant);
		return -1;
	}
	const char * cmd = "find /sys/devices |grep -E '/sys/devices/.*/host.*/port-.*/end_device-.*/sas_device/end_device-.*/phy_identifier'";
	string phy_id_path = " ";
	string  phy_id_value = " ";

	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"command executed for sys devices is:%s",cmd);

	fp = popen(cmd,"r");
	if (fp < 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error popen(..) failed! No execution of command: %s",cmd);
		::pclose(fp);
		return MONITORING_SERVICE_CHECKS_ERROR;
	}
	while ((read = getline(&line, &len, fp)) != -1){
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"inside while loop getline- %s",line);
		phy_id_path.assign(line);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Node phy_id_path %s", phy_id_path.c_str());
		length = phy_id_path.length();
		string str = phy_id_path.substr(pos, length-1);
		ifstream ifs;
		ifs.open(str.c_str(), std::ifstream::in);
		if (ifs.good()) {
			getline(ifs,phy_id_value);
		}
		ifs.close();
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Node phy_id_value %s", phy_id_value.c_str());
		if(((strcmp(hwVariant, "APG43") == 0) && (strcmp (phy_id_value.c_str(), "0") == 0)) || ((strcmp(hwVariant, "APG43/2") == 0) && (strcmp (phy_id_value.c_str(), "7") == 0))){
			string device = acs_apbm_monitoringservicehandler::dir_to_devices(phy_id_path.c_str());
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"device value is:%s",device.c_str());
			diskA = "/dev/" + device;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"diskA mapping:%s",diskA.c_str());
			fdisk_status = axe_eth_shelf::DISK_B;
		}
		if(((strcmp(hwVariant, "APG43") == 0) && (strcmp (phy_id_value.c_str(), "4") == 0)) || ((strcmp(hwVariant, "APG43/2") == 0) && (strcmp (phy_id_value.c_str(), "3") == 0))){
			string device1 = acs_apbm_monitoringservicehandler::dir_to_devices(phy_id_path.c_str());
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"device1 value is:%s",device1.c_str());
			diskB = "/dev/"+ device1;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"diskB mapping: %s",diskB.c_str());
			fdisk_status = axe_eth_shelf::DISK_A;
		}
	}
	::pclose(fp);


             /* FILE * fp1;
              char tmp[512] = { 0 };
              string cmd1;

              std::string myStr1 = "mdadm --detail /dev/md0 --test --verbose |";
              std::string myStr2 =  "grep ";
              std::string myStr3 = "| awk '{print $5}'";

              cmd1 = myStr1 +myStr2 +diskA + myStr3;

              ACS_APBM_LOG(LOG_LEVEL_DEBUG,"command executed to know device raid status is:%s",cmd1.c_str());

              fp1 = popen(cmd1.c_str(),"r");
                    if (fp1 < 0) {
                        ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error popen(..) failed! No execution of command: %s",cmd1.c_str());
                        ::pclose(fp1);
                        return MONITORING_SERVICE_CHECKS_ERROR;
                        }

              for (int i = 0; (::fscanf(fp1, "%20s", tmp)) != EOF; i++) {
              	if (strcmp("Active", tmp) == 0)
      		         fdisk_status = axe_eth_shelf::DISK_B;
                  else
      		         fdisk_status = axe_eth_shelf::DISK_A;
      		 }
             ::pclose(fp1);*/
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"faulty disk is %d ", fdisk_status);
	return fdisk_status;
}

string acs_apbm_monitoringservicehandler::dir_to_devices(const std::string &dirPath)
{
	FILE * fp;
	size_t posFin = 0;
	char buffer[10];
	string deviceName = "";
	if (dirPath.length() > 0)
	{
		posFin = dirPath.find("sas_device");
		if ( posFin != string::npos )
		{
			std::string cmd1 = "find ";
			std::string absPath("/target*/*:*:*:*/block/sd* -maxdepth 0 -type d | awk -F'/' '{print $NF}'");
			std::string baseDir = dirPath.substr(0, posFin-1);
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"baseDir is %s ", baseDir.c_str());
			std::string cmd2 = cmd1 +baseDir + absPath;

			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"command executed to know dir to device is:%s",cmd2.c_str());

			fp = popen(cmd2.c_str(),"r");
			if (fp < 0) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,"Error popen(..) failed! No execution of command: %s",cmd2.c_str());
				::pclose(fp);
				return "MONITORING_SERVICE_CHECKS_ERROR";
			}
			if((fgets(buffer, 10, fp) != NULL)){
				deviceName.assign(buffer);
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"device name is:%s",deviceName.c_str());
			}		
			::pclose(fp);
		}
	}
	return deviceName;
}
int acs_apbm_monitoringservicehandler::reset_nic_data_passive() {
	int result = 0;
	int env=0;
	if(_server_working_set->cs_reader->get_environment(env) != acs_apbm::ERR_NO_ERRORS) {
		ACS_APBM_LOG(LOG_LEVEL_WARN,"Unable to get APG shelf architecture !");
		return -1;
	}
	
	bool is_10g_system = _server_working_set->shelves_data_manager->is_10G();
	bool is_virtual_env = (env == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED);
	bool is_hwtype_gep7 = _server_working_set->shelves_data_manager->is_hardwaretype_gep7();
	
	if(is_virtual_env)
	{
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_vnic_status.nics); ++i) 
		_vnic_status.nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
	}
	else if(is_hwtype_gep7){
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status_gep7.nics); ++i)
		_nic_status_gep7.nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
	}
	else if(is_10g_system) {
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status_10g.nics); ++i)
		_nic_status_10g.nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;		
	}
	else {
		for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(_nic_status.nics); ++i)
		_nic_status.nics[i] = axe_eth_shelf::NIC_STATUS_UNDEFINED;
	}
	
	result=acs_apbm::ERR_NO_ERRORS;
	return result;
}
