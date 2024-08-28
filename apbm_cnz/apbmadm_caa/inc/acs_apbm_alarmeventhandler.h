
#ifndef HEADER_GUARD_CLASS__acs_apbm_alarmeventhandler
#define HEADER_GUARD_CLASS__acs_apbm_alarmeventhandler acs_apbm_alarmeventhandler
#endif

/** @file acs_apbm_alarmeventhandler.h
 *	@brief
 *	@author xassore
 *	@date 2012-06-29
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
 *	| R-001 | 2012-06-29 | xassore      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <vector>
#include <ace/Thread_Mutex.h>
#include <acs_aeh_error.h>
#include <acs_aeh_evreport.h>
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_monitoringservicehandler.h"
#include "acs_apbm_alarminfo.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_alarmeventhandler


#define EVENT_PROBLEM_TEXT_MAX_SIZE 256

// AP Events number 10700 - 10799 for APBM
#define NIC_NOT_CONN_NODEA_EVENT_CODE_START_NUM  	10700	//NIC event numbers 10700 + nic number on Node A
#define NIC_NOT_CONN_NODEB_EVENT_CODE_START_NUM  	10705	//NIC event numbers 10705 + nic number on Node B
#define NIC5_NOT_CONN_NODEA_EVENT_CODE			  	10715	//NIC 5 (ETH-2) event numbers  on Node A
#define NIC5_NOT_CONN_NODEB_EVENT_CODE			  	10716	//NIC 5 (ETH-2) event numbers  on Node B
#define TEAM_NODEA_LINK_LOST_EVENT_CODE 			10704
#define TEAM_NODEB_LINK_LOST_EVENT_CODE 			10709
#define RAID_NOT_WORKING_EVENT_CODE 				10710
#define DRBD_NOT_WORKING_EVENT_CODE                             10710 // Using same event code for RAID and DRBD
#define UNSUPPORTED_CONFIGURATION_EVENT_CODE			10747
#define NO_COMMUNICATION_EVENT_CODE				10748
#define DISK_NODEA_NOT_CONNECTED_EVENT_CODE 		10760
#define DISK_NODEB_NOT_CONNECTED_EVENT_CODE 		10765
#define BOARD_FAULTY_MISSING_EVENT_CODE_START_NUM	10720   // Board faulty/missing event code numbers 10720 + slot number
#define SCB0_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE   	10750
#define SCB25_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE   10751
#define SCB_LINK_DOWN_OR_NOT_WORKING_EVENT_CODE   	10752    // Both scb not working event code
#define THUMBDRIVE_MALFUNCTION_EVENT_CODE			10746 //Redesign as per TR-HS30773

#define LAG_DISABLED_SCXA_ALARM_EVENT_CODE   	    10753
#define LAG_DISABLED_SCXB_ALARM_EVENT_CODE   	    10755

#define HIGH_TEMPERATURE_NODEA_EVENT_CODE		10770
#define HIGH_TEMPERATURE_NODEB_EVENT_CODE		10771

#define NIC6_NOT_CONN_NODEA_EVENT_CODE                          10711   //NIC 6 (ETH-5) event numbers  on Node A
#define NIC6_NOT_CONN_NODEB_EVENT_CODE                          10712   //NIC 6 (ETH-5) event numbers  on Node B
#define NIC7_NOT_CONN_NODEA_EVENT_CODE                          10713   //NIC 7 (ETH-6) event numbers  on Node A
#define NIC7_NOT_CONN_NODEB_EVENT_CODE                          10714   //NIC 7 (ETH-6) event numbers  on Node B

#define NIC8_NOT_CONN_NODEA_EVENT_CODE	10715	//BOND1
#define NIC8_NOT_CONN_NODEB_EVENT_CODE	10716	//BOND1

#define AP_EVENT_CAUSE  "AP FAULT"
#define TEAM_EVENT_CAUSE "AP EXTERNAL NETWORK TEAM"
#define LAG_DISABLED_EVENT_CAUSE  "INFRASTRUCTURE LINK AGGREGATION FAULT"
#define LAG_DISABLED_PROBLEM_DATA  "One or more SCX front ports are down"

#define EVENT_NIC_PROBLEM_DATA    "The Network Interface Card is not connected properly"
#define EVENT_RAID_PROBLEM_DATA   "The RAID disk mirroring is not working properly"
#define EVENT_DRBD_PROBLEM_DATA   "The mirrored disks are not redundant"
#define EVENT_THUMBDRIVE_PROBLEM_DATA  "THUMBDRIVE MALFUNCTION"
#define EVENT_DISK_PROBLEM_DATA    "One or more disks may be not connected properly"

#define EVENT_DISK_UNAVAILABLE_PROBLEM_DATA    "One or more disks may not be available"
#define EVENT_UNSUPPORTED_CONFIGURATION_PROBLEM_DATA	"The SCB-RP version does not support hardware with GEP5_7 APUB or GEA(ROJ 208 890/2)"
#define EVENT_NO_COMMUNICATION_PROBLEM_DATA    "OaM board is not be able to communicate with the IPMI processor"

#define EVENT_BOARDFAULTY_PROBLEM_DATA    "The AP board is reported faulty by other function"
#define EVENT_BOARDREM_PROBLEM_DATA    "The AP board is missing"
#define EVENT_TEAM_PROBLEM_DATA    "The last adapter has lost link"
#define EVENT_SCBRP_0_PROBLEM_DATA    "The SNMP communication between the AP and the SCB-RP Board in slot 0 is not working"
#define EVENT_SCBRP_25_PROBLEM_DATA    "The SNMP communication between the AP and the SCB-RP Board in slot 25 is not working"
#define EVENT_BOTH_SCBRP_PROBLEM_DATA    "The SNMP communication between the AP and the SCB-RP Board in slot 0 AND 25 is not working"
#define EVENT_SCX_PROBLEM_DATA    "The SNMP communication between the AP and at least one Switch Board in a magazine is not working"
#define EVENT_BOTH_DMX_PROBLEM_DATA    "The communication between the AP and the DMXC is not working"
#define EVENT_BOTH_SCX_PROBLEM_DATA    "The SNMP communication between the AP and both Switch Boards in at least one magazine is not working"
#define EVENT_APUB_TEMPERATURE_PROBLEM_DATA    "One of the APUB board temperature is high"

#define EVENT_TEAM_PROBLEM_TEXT_HEAD  	   	"PROBLEM                    SLOTNUMBER\n"
#define EVENT_TEAM_PROBLEM_TEXT_TAIL  	   	"AP EXTERNAL NETWORK TEAM "
#define EVENT_NIC_PROBLEM_TEXT_HEAD  	   	"PROBLEM             SLOTNUMBER  CARDNUMBER\n"
#define EVENT_LAG_PROBLEM_TEXT_HEAD  	   	"PROBLEM                         SLOTNUMBER  PORTNUMBER\n"
#define EVENT_LAGFAULTY_PROBLEM_TEXT_TAIL   "SCX-EXTERNAL SWITCH LINK DOWN      "
#define EVENT_NIC_PROBLEM_TEXT_TAIL  		"NIC NOT CONNECTED      "
#define EVENT_BOARD_PROBLEM_TEXT_HEAD  	   	"PROBLEM         SLOTNUMBER\n"
#define EVENT_BOARDREM_PROBLEM_TEXT_TAIL  	"BOARD MISSING "
#define EVENT_BOARDFAULTY_PROBLEM_TEXT_TAIL "BOARD FAULTY  "
#define EVENT_RAID_PROBLEM_TEXT_HEAD   	   	"PROBLEM\n"
#define EVENT_RAID_PROBLEM_TEXT_TAIL 		"MIRRORED DISK NOT REDUNDANT"
#define EVENT_DRBD_PROBLEM_TEXT_HEAD            "PROBLEM\n"
#define EVENT_DRBD_PROBLEM_TEXT_TAIL            "MIRRORED DISKS NOT REDUNDANT"
#define EVENT_DISK_PROBLEM_TEXT_HEAD            "PROBLEM                                   SLOTNUMBER\n"
#define EVENT_DISK_PROBLEM_TEXT_TAIL            "DISK CABLES MAY BE NOT CONNECTED PROPERLY TO SLOT\n"
#define EVENT_THUMBDRIVE_PROBLEM_TEXT_HEAD      "PROBLEM                         \n"
#define EVENT_THUMBDRIVE_PROBLEM_TEXT_TAIL      "THUMBDRIVE MALFUNCTION"

#define EVENT_DISK_UNAVAILABLE_PROBLEM_TEXT                "DISK FAULTY"

#define EVENT_UNSUPPORTED_CONFIGURATION_PROBLEM_TEXT                "UNSUPPORTED CONFIGURATION"
#define EVENT_NO_COMMUNICATION_PROBLEM_TEXT		    "OaM board is not be able to communicate with the IPMI processor"

#define EVENT_SCBRP_PROBLEM_TEXT_HEAD     	"PROBLEM                 SLOT NUMBER\n"
#define EVENT_SCBRP_PROBLEM_TEXT_TAIL  		"AP-SCBRP SNMP LINK      "
#define EVENT_SCX_PROBLEM_TEXT_HEAD        	"PROBLEM\n"
#define EVENT_SCX_PROBLEM_TEXT_TAIL 		"AP-SWITCH SNMP LINK"

#define EVENT_DMX_PROBLEM_TEXT_HEAD             "PROBLEM\n"
#define EVENT_DMX_PROBLEM_TEXT_TAIL             "AP-DMXC LINK"
#define EVENT_APUB_TEMPERATURE_PROBLEM_TEXT_HEAD    "PROBLEM			SLOTNUMBER\n"
#define EVENT_APUB_TEMPERATURE_PROBLEM_TEXT_TAIL    "BOARD HIGH TEMPERATURE"

#define EVENT_CLASS_REFERENCE               ACS_AEH_OBJ_CLASS_REFERENZE_APZ
#define EVENT_OBJ_REFERENCE                 "acs_apbmd/Alarm"
#define OBJ_REFERENCE_FOR_EVENT                 "acs_apbmd/Event"



#define EVENT_BOTH_SCB_NOT_WORKING    		ACS_AEH_PERCEIVED_SEVERITY_A1
#define EVENT_SCB_NOT_WORKING    		ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_BOARD_MISSING_SEVERITY  		ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_BOARD_FAULTY_SEVERITY  		ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_RAID_NOT_WORKING_SEVERITY  	ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_DRBD_NOT_WORKING_SEVERITY         ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_DISK_NOT_WORKING_SEVERITY  	ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_NETWORK_TEAM_DOWN_SEVERITY  	ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_NIC_NOT_CONNECTED_SEVERITY  	ACS_AEH_PERCEIVED_SEVERITY_A3
#define EVENT_CEASING                		ACS_AEH_PERCEIVED_SEVERITY_CEASING
#define EVENT_UNSUPPORTED_CONFIGURATION_SEVERITY 	ACS_AEH_PERCEIVED_SEVERITY_EVENT	
#define EVENT_NO_COMMUNICATION_SEVERITY			ACS_AEH_PERCEIVED_SEVERITY_EVENT
#define EVENT_THUMBDRIVE_MALFUNCTION_SEVERITY         ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_LAG_DISABLED_SEVERITY      	ACS_AEH_PERCEIVED_SEVERITY_A2
#define EVENT_APUB_TEMPERATURE_HIGH    		ACS_AEH_PERCEIVED_SEVERITY_A1
#define EVENT_APUB_TEMPERATURE_LOW    		ACS_AEH_PERCEIVED_SEVERITY_A2
	/* MAPPING design base NIC value with Linux Eth interface
	 * ********  *
	 * NIC - ETH *
	 *  1  -  3  *
	 *  2  -  4  *
	 *  3  -  0  *
	 *  4  -  1  *
	 *  5  -  2  *
	 *  6  -  5  *
         *  7  -  6  *
         *  -  -  -  *
         *       BOND1
         *  8     7
	 *  *  *  *  *
	 * ********* */

#define ETH0  0
#define ETH1  1
#define ETH2  2
#define ETH3  3
#define ETH4  4
#define ETH5  5
#define ETH6  6
#define BOND1 7

#define NIC1 1
#define NIC2 2
#define NIC3 3
#define NIC4 4
#define NIC5 5
#define NIC6 6
#define NIC7 7
#define NIC8 8


class __CLASS_NAME__  {

	enum acs_apbm_alarmeventhandler_constants {
		ACS_APBM_AEH_SUCCESS = 0,
		ACS_APBM_AEH_ALARM_NOT_RAISED,
		ACS_APBM_AEH_ALARM_NOT_CEASED,
		ACS_APBM_AEH_ALARM_NOT_ADDED_IN_ACTIVELIST,
		ACS_APBM_AEH_ALARM_NOT_APPLICABLE
	};

public:

	enum acs_apbm_alarmeventhandler_snmp_link_alarm_type {
		ACS_APBM_AEH_SNMP_LINK_DOWN_LEFT_SCB = 0,
		ACS_APBM_AEH_SNMP_LINK_DOWN_RIGHT_SCB,
		ACS_APBM_AEH_SNMP_LINK_DOWN_BOTH_SCB
	};

	inline __CLASS_NAME__(acs_apbm_serverworkingset * server_working_set): _aeh_error_text(""), _server_working_set(server_working_set){}

	~__CLASS_NAME__();


	int handle_nic_status_data(const nic_status_data &nic_data, int32_t slot_num );

	int handle_nic_status_data(const nic_status_data_10g &nic_data, int32_t slot_num );
	
	int handle_nic_status_data(const nic_status_data_gep7 &nic_data, int32_t slot_num );

	int raise_alarm( int scb_alarm_type);

	int  raise_alarm(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type, int32_t slot_num1, int32_t slot_num2, int nic_num);
	inline  int  raise_alarm (acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type, int32_t slot_num) {
		return raise_alarm(alarm_type, slot_num,-1, -1);
	}
	inline  int  raise_alarm (acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type,  int32_t slot_num, int nic_num) {
			return raise_alarm(alarm_type, slot_num,-1, nic_num);
		}

	int cease_alarm( int scb_alarm_type);

	int cease_alarm (acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type, int32_t slot_num, int nic_num);
	inline int cease_alarm (acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type, int32_t slot_num){
		return cease_alarm (alarm_type, slot_num, -1);
	}
	inline int cease_alarm (acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type) {
			return cease_alarm(alarm_type, -1, -1);
	}

	int cease_all_alarms();				 //TR HX65505

	inline const char * aeh_error_text () const { return ( _aeh_error_text )? _aeh_error_text: ""; }
	int raise_unsupported_config_event();
	int raise_no_communication_event();

private:
	int raise_nic_alarm (int32_t slot_num, int nic_num);
	int raise_team_alarm(int32_t slot_num);
	int raise_raid_alarm(int32_t slot_num);
        int raise_drbd_alarm(int32_t slot_num);
	int raise_boardremoved_alarm (int32_t slot_num);
	int raise_boardfaulty_alarm (int32_t slot_num);
	int raise_scbrp_snmplinkdown_alarm(int32_t slot_num);
	int raise_both_scbrp_snmplinkdown_alarm(int32_t slot_num1, int32_t slot_num2);
	int raise_scx_snmplinkdown_alarm(int32_t slot_num);
	int raise_both_dmx_snmplinkdown_alarm();
	int raise_both_scx_snmplinkdown_alarm();
	int raise_disk_alarm(int32_t slot_num);
        int raise_disk_unavailable_alarm(int32_t slot_num);
	int raise_thumbdrive_alarm(int32_t slot_num); //Redesign as per TR-HS30773
	int raise_lag_disabled_alarm (int32_t slot_num,int port_num);
	int get_lag_alarm_event_code(int32_t slot_num, int port_number);
	int raise_temperature_alarm(int32_t slot_num, int32_t severity);

private:
	int add_alarm_in_activelist(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type,  int32_t slot, int nic_num, long event_code);
	inline int add_alarm_in_activelist(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type,  int32_t slot, long event_code){
		return add_alarm_in_activelist(alarm_type, slot, -1, event_code);
	}
	inline int add_alarm_in_activelist(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type, long event_code){
			return add_alarm_in_activelist(alarm_type, -1, -1, event_code);
		}
	int is_alarm_in_activelist(acs_apbm_alarminfo::acs_apbm_alarm_type_t alarm_type,  int32_t slot, int nic_num);

	int get_nic_alarm_event_code(int32_t slot_num, int nic);

	/* check if the given 'alarm type' is applicable for the given 'APG shelf architecture'.
	 * If the -1 value  is passed for 'apg_shelf_architecture' argument, then the method
	 * will try to obtain the APG shelf architecture  querying the object '_server_working_set->cs_reader'
	 */
	bool is_alarm_applicable(int alarm_type, int apg_shelf_architecture = -1);

private:
	mutable ACE_Thread_Mutex _activealarms_mutex;		// used to synchronize access to active alarms list _active_alarms
	std::vector<acs_apbm_alarminfo *> _active_alarms;

	acs_aeh_evreport _alarmeventHandler;

	const char * _aeh_error_text;

	acs_apbm_serverworkingset * _server_working_set;
};

