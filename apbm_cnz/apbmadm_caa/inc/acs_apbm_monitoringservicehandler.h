#ifndef ACS_APBM_MONITORINGSERVICEHANDLER_H_
#define ACS_APBM_MONITORINGSERVICEHANDLER_H_

/** @file acs_apbm_monitoringservicehandler.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2011-12-20
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
 *	| R-001 | 2011-12-20 | xludesi      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <ace/Thread_Mutex.h>
#include <ace/Event_Handler.h>

#include "acs_apbm_primitivedatahandler.h"

#include "acs_apbm_programconfiguration.h"
#include <acs_apbm_programconstants.h>
#include "acs_apbm_apboardinfo.h"
#include "acs_apbm_switchboardinfo.h"
#define LEFT "left"
#define RIGHT "right"
class acs_apbm_serverworkingset;

class nic_status_data {
public:
  /*
   * CONSTRUCTORS
   */
  inline nic_status_data () : nics(), ethernet_interface() {
  	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(nics); nics[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED) ;
  	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(ethernet_interface); ethernet_interface[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED) ; //used to handle TR HQ56362
  	for (size_t i = 0; i < axe_eth_shelf::MAX_ETH_GEP2;i++){
  		bzero(mac_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
  		bzero(ipv4_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
  	}
  }

  /*
   * DESTRUCTOR
   */
  inline ~nic_status_data () {}

  /*
   * FUNCTIONS
   */
public:
  void set_nic_status (const char * if_name, axe_eth_shelf::nic_status_t status);
  void set_ethernet_interface_status(const char * if_name, axe_eth_shelf::nic_status_t status); //used to handlet TR HQ56362
  void set_mac_address(const char * if_name, char * mac_address);
  void set_ip_address(const char * if_name, char * ipv4_address);
  void reset_nic_status();

  /*
   * FIELDS
   */
public:
  axe_eth_shelf::nic_status_t nics [axe_eth_shelf::MAX_ETH_GEP2];
  axe_eth_shelf::nic_status_t ethernet_interface [axe_eth_shelf::MAX_ETH_GEP2];
  char mac_addresses[axe_eth_shelf::MAX_ETH_GEP2][acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS];
  char ipv4_addresses[axe_eth_shelf::MAX_ETH_GEP2][acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS];
//private:
  static const char * _nic_names [axe_eth_shelf::MAX_ETH_GEP2];


};

class nic_status_data_10g {
public:
  /*
   * CONSTRUCTORS
   */
  inline nic_status_data_10g () : nics(), ethernet_interface(), mac_addresses(), ipv4_addresses() {
  	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(nics); nics[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED) ;
  	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(ethernet_interface); ethernet_interface[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED);
  	for (size_t i = 0; i < axe_eth_shelf::MAX_ETH_GEP5;i++){
  		bzero(mac_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
  		bzero(ipv4_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
  	}
  }

  /*
   * DESTRUCTOR
   */
  inline ~nic_status_data_10g () {}

  /*
   * FUNCTIONS
   */
public:
  void set_nic_status (const char * if_name, axe_eth_shelf::nic_status_t status);
  void set_ethernet_interface_status(const char * if_name, axe_eth_shelf::nic_status_t status);
  void set_mac_address(const char * if_name, char * mac_address);
  void set_ip_address(const char * if_name, char * ipv4_address);
  void reset_nic_status();
  /*
   * FIELDS
   */
public:
  axe_eth_shelf::nic_status_t nics [axe_eth_shelf::MAX_ETH_GEP5];
  axe_eth_shelf::nic_status_t ethernet_interface [axe_eth_shelf::MAX_ETH_GEP5];
  char mac_addresses[axe_eth_shelf::MAX_ETH_GEP5][acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS];
   char ipv4_addresses[axe_eth_shelf::MAX_ETH_GEP5][acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS];

//private:
  static const char * _nic_names [axe_eth_shelf::MAX_ETH_GEP5];

};
class nic_status_data_gep7 {
public:
  /*
   * CONSTRUCTORS
   */
  inline nic_status_data_gep7 () : nics(), ethernet_interface(), mac_addresses(), ipv4_addresses() {
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(nics); nics[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED) ;
	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(ethernet_interface); ethernet_interface[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED);
	for (size_t i = 0; i < axe_eth_shelf::MAX_ETH_GEP7;i++){
		bzero(mac_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
		bzero(ipv4_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
	}
  }

  /*
   * DESTRUCTOR
   */
  inline ~nic_status_data_gep7 () {}

  /*
   * FUNCTIONS
   */
public:
  void set_nic_status (const char * if_name, axe_eth_shelf::nic_status_t status);
  void set_ethernet_interface_status(const char * if_name, axe_eth_shelf::nic_status_t status);
  void set_mac_address(const char * if_name, char * mac_address);
  void set_ip_address(const char * if_name, char * ipv4_address);
  void reset_nic_status();
  /*
   * FIELDS
   */
public:
  axe_eth_shelf::nic_status_t nics [axe_eth_shelf::MAX_ETH_GEP7];
  axe_eth_shelf::nic_status_t ethernet_interface [axe_eth_shelf::MAX_ETH_GEP7];
  char mac_addresses[axe_eth_shelf::MAX_ETH_GEP7][acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS];
  char ipv4_addresses[axe_eth_shelf::MAX_ETH_GEP7][acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS];

//private:
  static const char * _nic_names [axe_eth_shelf::MAX_ETH_GEP7];

};

class virtual_nic_status_data {
public:
  /*
   * CONSTRUCTORS
   */
  inline virtual_nic_status_data () : nics(), ethernet_interface(), mac_addresses(), ipv4_addresses() {
  	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(nics); nics[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED) ;
  	for (size_t i = 0; i < ACS_APBM_ARRAY_SIZE(ethernet_interface); ethernet_interface[i++] = axe_eth_shelf::NIC_STATUS_UNDEFINED);
  	for (size_t i = 0; i < axe_eth_shelf::MAX_ETH_VAPG;i++){
  		bzero(mac_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS);
  		bzero(ipv4_addresses[i],acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS);
  	}
  }

  /*
   * DESTRUCTOR
   */
  inline ~virtual_nic_status_data () {}

  /*
   * FUNCTIONS
   */
public:
  void set_nic_status (const char * if_name, axe_eth_shelf::nic_status_t status);
  void set_ethernet_interface_status(const char * if_name, axe_eth_shelf::nic_status_t status);
  void set_mac_address(const char * if_name, char * mac_address);
  void set_ip_address(const char * if_name, char * ipv4_address);
  void reset_nic_status();
  /*
   * FIELDS
   */
public:
  axe_eth_shelf::nic_status_t nics [axe_eth_shelf::MAX_ETH_VAPG];
  axe_eth_shelf::nic_status_t ethernet_interface [axe_eth_shelf::MAX_ETH_VAPG];
  char mac_addresses[axe_eth_shelf::MAX_ETH_VAPG][acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS];
  char ipv4_addresses[axe_eth_shelf::MAX_ETH_VAPG][acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS];

//private:
  static const char * _nic_names [axe_eth_shelf::MAX_ETH_VAPG];

};

class acs_apbm_monitoringservicehandler : public ACE_Event_Handler
{
	enum checks_result
	{
		MONITORING_SERVICE_CHECKS_OK = 0,
		MONITORING_SERVICE_CHECKS_ERROR = -1,
		MONITORING_SERVICE_LINK_DOWN_FOUND = -2
	};

	enum msh_snmp_link_down_type
	{
		MSH_SNMP_LINK_OK = 0,
		MSH_SNMP_LINK_DOWN_LEFT = 1,
		MSH_SNMP_LINK_DOWN_RIGHT = 2,
		MSH_SNMP_LINK_DOWN_BOTH = 3,
		MSH_SNMP_LINK_CHECK_ERROR = -1
	};

public:

	acs_apbm_monitoringservicehandler(uint32_t initial_delay, uint32_t interval, acs_apbm_serverworkingset * server_working_set)
	: ACE_Event_Handler(), _monitoring_ongoing(0),_operational_state(acs_apbm::PROGRAM_STATE_RUNNING_NODE_UNDEFINED), /*_swithcboard_ongoing_check(0),*/
	  _monitoring_service_on_passive_threshold(0),_update_switchboard_states_threshold(0), _frontport_check_threshold(0), _initial_delay(initial_delay),
	  _interval(interval),_forced_nic_update(false), _server_working_set(server_working_set), _nic_status(), _nic_status_10g(),_nic_status_gep7(),_vnic_status(),
	  _raid_status(axe_eth_shelf::RAID_STATUS_UNDEFINED), _raid_status1(axe_eth_shelf::RAID_STATUS_UNDEFINED),_drbd_status(axe_eth_shelf::DRBD_STATUS_UNDEFINED),_drbd_connection_status (axe_eth_shelf::DRBD_STATUS_UNDEFINED), _local_thumbdrive_status(axe_eth_shelf::THUMBDRIVE_STATUS_UNDEFINED), _other_thumbdrive_status(axe_eth_shelf::THUMBDRIVE_STATUS_UNDEFINED), _switch_left_alarm_raised(0),
	  _switch_right_alarm_raised(0), _both_switch_alarm_raised(0), _disk_unavailable_alarm_raised(0), _opstate_status(0), set_nic_status_value_to_default(0), _10g_nics_down_counter(0)
	#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
		, _raid_rebuild_state(-1)
	#endif
	{}


	inline virtual ~acs_apbm_monitoringservicehandler() {};

	static int check_physical_separation_status ();

	static int check_teaming_status ();
	
	static int check_dataDisk_replication_type ();

	// callback invoked when monitoring service is activated, to perform a periodic monitoring iteration
	virtual int handle_timeout ( const ACE_Time_Value &  current_time,  const void * act);

	// callback invoked when monitoring service is deactivated
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	// activates the monitoring service registering with the APBM reactor
	int open();

	// deactivates the monitoring service canceling it from APBM main reactor
	int close();

	inline int set_raid_status(axe_eth_shelf::raid_status_t raid_status){
		_raid_status = raid_status;
		return 0;
	}

	int check_scx_frontport_lag_admin_status();
	int set_scx_lag_monitoring_flags();
	//start of TR HV50762
	bool get_nic_status_data(){
		return set_nic_status_value_to_default;
	}
	void set_nic_status_data(bool value_to_set){
		set_nic_status_value_to_default = value_to_set;
	}
	//end of TR HV50762

	void reset_scx_lag_monitoring_flags_due_to_restart(int indexOfSCXB);

private:
	// Do checks and eventually updates internal data structures and IMM
	checks_result do_checks();

	checks_result do_checks_on_passive();

	int do_nic_checks ();

	int do_nic_checks_10g ();
	
	int do_nic_checks_gep7 ();

	int do_virtual_nic_checks();

	int do_nic_checks_on_passive();

	checks_result do_disk_checks(int env);

	checks_result do_disk_checks_in_virtual_env(int apub_board_slot);

	checks_result do_raid_checks();
	
	checks_result do_drbd_checks(int env);
	
	checks_result do_thumbdrive_checks();
		
	int do_front_port_check();
	int do_switch_boards_check (int env);
	int do_switch_board_check (acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms = 0);
	int do_switch_board_check (int32_t switch_board_slot, const char * switch_board_ip, const unsigned * timeout_ms = 0);
	int do_dmxswitch_board_check (acs_apbm_switchboardinfo & switch_board, const unsigned * timeout_ms = 0);

	int retrieve_network_info(ifaddrs * ifaddr, nic_status_data &new_nic_info);
	int retrieve_network_info(ifaddrs * ifaddr, nic_status_data_10g &new_nic_info);
	int retrieve_network_info(ifaddrs * ifaddr, nic_status_data_gep7 &new_nic_info);
	int retrieve_network_info(ifaddrs * ifaddr, virtual_nic_status_data &new_nic_info);

	bool check_valid_nic_data(const nic_status_data &new_nic_info);
	bool check_valid_nic_data(const nic_status_data_10g &new_nic_info);
	bool check_valid_nic_data(const nic_status_data_gep7 &new_nic_info);
	bool check_valid_nic_data(const virtual_nic_status_data &new_nic_info);

	bool check_valid_nic_data_on_passive(const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index, const nic_status_data &new_nic_info);
	bool check_valid_nic_data_on_passive(const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index, const nic_status_data_10g &new_nic_info);
	bool check_valid_nic_data_on_passive(const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index, const nic_status_data_gep7 &new_nic_info);
	bool check_valid_nic_data_on_passive(const char * ifa_name, axe_eth_shelf::nic_type_t & nic_index, const virtual_nic_status_data &new_nic_info);

	int update_nic_data(const nic_status_data &new_nic_info);
	int update_nic_data(const nic_status_data_10g &new_nic_info);
	int update_nic_data(const nic_status_data_gep7 &new_nic_info);
	int update_nic_data(const virtual_nic_status_data &new_nic_info);

	int update_nic_data_on_passive(const axe_eth_shelf::nic_type_t nic_index, const nic_status_data &new_nic_info);
	int update_nic_data_on_passive(const axe_eth_shelf::nic_type_t nic_index, const nic_status_data_10g &new_nic_info);
  	int update_nic_data_on_passive(const axe_eth_shelf::nic_type_t nic_index, const nic_status_data_gep7 &new_nic_info);
	int update_nic_data_on_passive(const axe_eth_shelf::nic_type_t nic_index, const virtual_nic_status_data &new_nic_info);


	int update_switch_board_states ();

	int do_snmp_link_down_check_scbrp();
	int do_snmp_link_down_check_scx_or_smx();
	int do_ironside_link_down_check_dmx();
	int check_snmp_link_status(int slot_pos, uint32_t mag_num, const unsigned * timeout_ms = 0);

	int handle_switch_boards_alarms (int scb_link_down_type);

	int do_scx_front_port_operational_status_check ();
	int disk_finder();
	std::string dir_to_devices(const std::string &dirpath);
	int reset_nic_data_passive();

private:
	volatile int _monitoring_ongoing;
	acs_apbm::ProgramStateConstants _operational_state;
//	int _swithcboard_ongoing_check;
	int _monitoring_service_on_passive_threshold;
	int _update_switchboard_states_threshold;
	int _frontport_check_threshold;
	uint32_t _initial_delay;
	uint32_t _interval;
	bool _forced_nic_update;
	acs_apbm_serverworkingset * _server_working_set;
	nic_status_data _nic_status;
	nic_status_data_10g _nic_status_10g;
	nic_status_data_gep7 _nic_status_gep7;
	virtual_nic_status_data _vnic_status;
	axe_eth_shelf::raid_status_t _raid_status;
	axe_eth_shelf::raid_status_t _raid_status1;
	axe_eth_shelf::drbd_status_t _drbd_status,_drbd_connection_status;
  	axe_eth_shelf::thumbdrive_status_t _local_thumbdrive_status, _other_thumbdrive_status;//Redesign as per TR-HS30773
	static const char * _physical_separation_nic_name;
	static const char *  _north_bound_imm_class_name;
	static const char * _axe_functions_imm_class_name;	
	static const char * _datadisk_devname_in_virtual_apgenv;
	static const char * _datadisk_directory_to_test_in_virtual_apgenv;
	static int frontPort1_statusCounter[2];
	static int frontPort2_statusCounter[2];
	static bool disableAdminStatePort1[2];
	static bool disableAdminStatePort2[2];
	static bool bothFrontPortDown[2];
	static bool firstLagMonitoringTaskAfterReset[2];
	static int _count_scb_left_link_down;
	static int _count_scb_right_link_down;
	bool _switch_left_alarm_raised;
	bool _switch_right_alarm_raised;
	bool _both_switch_alarm_raised;
	bool _disk_unavailable_alarm_raised; // TR HU60600: new member variable '_disk_unavailable_alam_raised' to keep track of DISK alarm
	bool _opstate_status; //Redesign as per TR-HS30773
	bool set_nic_status_value_to_default; //TR HV50762
	uint32_t _10g_nics_down_counter;
	int phy_id;
    #ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	int _raid_rebuild_state; // -1: init value - 0: no rebuilding - 1: rebuilding on going
    #endif
	ACE_Recursive_Thread_Mutex _alarm_flag_mutex;
	ACE_Recursive_Thread_Mutex _front_port_flags_mutex;
};

#endif /* ACS_APBM_MONITORINGSERVICEHANDLER_H_ */
