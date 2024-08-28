#ifndef HEADER_GUARD_CLASS__ACS_DSD_ConfigurationHelper
#define HEADER_GUARD_CLASS__ACS_DSD_ConfigurationHelper ACS_DSD_ConfigurationHelper

/** @file ACS_DSD_ConfigurationHelper.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-01
 *	@version 1.0.0
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.0 | 2010-10-01 | xnicmut      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

#include <string>
#include <vector>

#include "ACS_DSD_Macros.h"
#include "ACS_DSD_Trace.h"

class ACS_DSD_TraTracer;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_DSD_ConfigurationHelper

#ifdef ACS_DSD_API_TRACING_ACTIVE
extern ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__);
#endif

/** @class ACS_DSD_ConfigurationHelper ACS_DSD_ConfigurationHelper.h
 *	@brief ACS_DSD_ConfigurationHelper class
 *	@author xnicmut (Nicola Muto)
 *	@date 2010-10-01
 *	@version 1.0.0
 *
 *	ACS_DSD_ConfigurationHelper Class detailed description
 */
class __CLASS_NAME__ {
	//===============================//
	// Nested Classes and Structures //
	//===============================//
public:
	/** @struct ACS_DSD_ConfigurationHelper::NetworkInfo ACS_DSD_ConfigurationHelper.h
	 *	@brief NetworkInfo struct
	 *	@author xnicmut (Nicola Muto)
	 *	@date 2010-10-01
	 *	@version 1.0.0
	 *
	 *	NetworkInfo Class detailed description
	 */
	struct NetworkInfo {
		inline NetworkInfo()
		: ip4_address(0), ip4_address_str(), ip4_broadcast(0), ip4_broadcast_str(), name() {
			memccpy(ip4_address_str, "0.0.0.0", 0, acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX);
			memccpy(ip4_broadcast_str, "255.255.255.255", 0, acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX);
		}

		uint32_t ip4_address;
		char ip4_address_str[acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX];
		uint32_t ip4_broadcast;
		char ip4_broadcast_str[acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX];
		char name[acs_dsd::CONFIG_NETWORK_NAME_SIZE_MAX];
	};

	/** @struct ACS_DSD_ConfigurationHelper::HostInfo ACS_DSD_ConfigurationHelper.h
	 *	@brief HostInfo struct
	 *	@author xnicmut (Nicola Muto)
	 *	@date 2010-10-01
	 *	@version 1.0.0
	 *
	 *	HostInfo Class detailed description
	 */
	struct HostInfo {
		inline HostInfo() :
		system_id(-1), system_name(), system_type(acs_dsd::SYSTEM_TYPE_UNKNOWN),
		side(acs_dsd::NODE_SIDE_UNDEFINED), node_name(), ip4_adresses_count(0), ip4_addresses(),
		ip4_addresses_str(), ip4_aliases_count(0), ip4_aliases(), ip4_aliases_str() {
			for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) {
				memccpy(ip4_addresses_str[i], "0.0.0.0", 0, acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX);
				memccpy(ip4_aliases_str[i], "0.0.0.0", 0, acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX);
			}
		}

		//For BC; [0, 999] --> BC0, BC1 ... BC999  --> cs_system_type == 0
		//For CP: [1000, 1999] --> CP0, CP1 ... CP999 --> cs_system_type == 1000
		//For AP: [2000, 2999] --> AP0, AP1 ... AP999 --> cs_system_type == 2000
		int32_t system_id; //For AP and CP is cs_system_type + cs_system_num. For BC is cs_system_type + cs_sequence_num
		char system_name[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX];
		acs_dsd::SystemTypeConstants system_type;
		acs_dsd::NodeSideConstants side; //-1 == UNKNOWN, 0 == A, 1 == B, 2 == C ...
		char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX];
		size_t ip4_adresses_count;
		uint32_t ip4_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
		char ip4_addresses_str[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX];
		size_t ip4_aliases_count;
		uint32_t ip4_aliases[acs_dsd::CONFIG_NETWORKS_SUPPORTED];
		char ip4_aliases_str[acs_dsd::CONFIG_NETWORKS_SUPPORTED][acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX];
	};

	//==========//
	// Typedefs //
	//==========//
public:
	typedef HostInfo * HostInfo_pointer_t;
	typedef const HostInfo * HostInfo_const_pointer_t;
	typedef HostInfo & HostInfo_reference_t;
	typedef const HostInfo & HostInfo_const_reference_t;

	//==============//
	// Constructors //
	//==============//
private:
	/** @brief ACS_DSD_ConfigurationHelper Default constructor
	 *
	 *	ACS_DSD_ConfigurationHelper Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	/** @brief ACS_DSD_ConfigurationHelper Copy constructor
	 *
	 *	ACS_DSD_ConfigurationHelper Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ &) { ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__)); }

	//============//
	// Destructor //
	//============//
public:
	/** @brief ACS_DSD_ConfigurationHelper Destructor
	 *
	 *	ACS_DSD_ConfigurationHelper Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
	}

	//===========//
	// Functions //
	//===========//
public:
	/** @brief force_configuration_reload method
	 *
	 *	force_configuration_reload method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static void force_configuration_reload ();

	/** @brief load_ap_nodes_from_cs method
	 *
	 *	load_ap_nodes_from_cs method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int load_ap_nodes_from_cs ();

	/** @brief load_cp_nodes_from_cs method
	 *
	 *	load_cp_nodes_from_cs method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int load_cp_nodes_from_cs ();

	/** @brief load_process_information method
	 *
	 *	load_process_information method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int load_process_information ();

	/** @brief load_my_ap_node method
	 *
	 *	load_my_ap_node method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int load_my_ap_node ();

	/** @brief load_my_ap_partner_node method
	 *
	 *	load_my_ap_partner_node method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static inline int load_my_ap_partner_node () { return load_my_ap_node(); }

	/** @brief get_node_by_ip method
	 *
	 *	get_node_by_ip method detailed description
	 *
	 *	@param[in] net_order_ip Description
	 *	@param[out] node_info_ptr Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_node_by_ip (uint32_t net_order_ip, HostInfo_const_pointer_t & node_info_ptr);
	static int get_node_by_ip (uint32_t net_order_ip, HostInfo & node_info);

	/** @brief get_node_by_node_name method
	 *
	 *	get_node_by_node_name method detailed description
	 *
	 *	@param[in] node_name Description
	 *	@param[out] node_info_ptr Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_node_by_node_name (const char * node_name, HostInfo_const_pointer_t & node_info_ptr);
	static int get_node_by_node_name (const char * node_name, HostInfo & node_info);
	static inline int get_node_by_node_name (const std::string & node_name, HostInfo_const_pointer_t & node_info_ptr) {
		return get_node_by_node_name(node_name.c_str(), node_info_ptr);
	}
	static inline int get_node_by_node_name (const std::string & node_name, HostInfo & node_info) {
		return get_node_by_node_name(node_name.c_str(), node_info);
	}

	/** @brief get_node_by_side method
	 *
	 *	get_node_by_side method detailed description
	 *
	 *	@param[in] system_id Description
	 *	@param[in] side Description
	 *	@param[out] node_info_ptr Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo_const_pointer_t & node_info_ptr);
	static int get_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo & node_info);

	/** @brief get_partner_node_by_side method
	 *
	 *	get_partner_node_by_side method detailed description
	 *
	 *	@param[in] system_id Description
	 *	@param[in] side Description
	 *	@param[out] node_info_ptr Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_partner_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo_const_pointer_t & node_info_ptr);
	static int get_partner_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo & node_info);

	/** @brief get_my_ap_node method
	 *
	 *	get_my_ap_node method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_my_ap_node (HostInfo_const_pointer_t & node_info_ptr);
	static int get_my_ap_node (HostInfo & node_info);

	/** @brief get_my_ap_partner_node method
	 *
	 *	get_my_ap_partner_node method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_my_ap_partner_node (HostInfo_const_pointer_t & node_info_ptr);
	static int get_my_ap_partner_node (HostInfo & node_info);

	/** @brief get_ap_node method
	 *
	 *	get_ap_node method detailed description
	 *
	 *	@param[in] index Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_ap_node (unsigned index, HostInfo_const_pointer_t & node_info_ptr);
	static int get_ap_node (unsigned index, HostInfo & node_info);

	static int get_all_ap_nodes (std::vector<HostInfo> & ap_nodes);

	/** @brief get_cp_node method
	 *
	 *	get_cp_node method detailed description
	 *
	 *	@param[in] index Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int get_cp_node (unsigned index, HostInfo_const_pointer_t & node_info_ptr);
	static int get_cp_node (unsigned index, HostInfo & node_info);

	/** @brief get_ap_id_min method
	 *
	 *	get_ap_id_min method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int32_t get_ap_id_min ();

	/** @brief get_ap_id_max method
	 *
	 *	get_ap_id_max method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int32_t get_ap_id_max ();

	/** @brief get_bc_id_min method
	 *
	 *	get_bc_id_min method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int32_t get_bc_id_min ();

	/** @brief get_bc_id_max method
	 *
	 *	get_bc_id_max method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int32_t get_bc_id_max ();

	/** @brief get_cp_id_min method
	 *
	 *	get_cp_id_min method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int32_t get_cp_id_min ();

	/** @brief get_cp_id_max method
	 *
	 *	get_cp_id_max method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int32_t get_cp_id_max ();

	/** @brief set_alarm_master_in_cs method
	 *
	 *	set_alarm_master_in_cs method detailed description
	 *
	 *	@param[in] system_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int set_alarm_master_in_cs (int32_t system_id);

	/** @brief set_clock_master_in_cs method
	 *
	 *	set_clock_master_in_cs method detailed description
	 *
	 *	@param[in] system_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static int set_clock_master_in_cs (int32_t system_id);

	/** @brief system_ap_nodes_count method
	 *
	 *	system_ap_nodes_count method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static unsigned system_ap_nodes_count ();

	/** @brief system_cp_nodes_count method
	 *
	 *	system_cp_nodes_count method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static unsigned system_cp_nodes_count ();

	/** @brief system_exist method
	 *
	 *	system_exist method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static bool system_exist (int32_t system_id);

	/** @brief my_pid method
	 *
	 *	my_pid method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static pid_t my_process_id ();

	/** @brief my_process_name method
	 *
	 *	my_process_name method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static const char * my_process_name ();

	/** @brief unix_socket_root_path method
	 *
	 *	unix_socket_root_path method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static const char * unix_socket_root_path ();

	/** @brief dsdapi_unix_socket_sap_id method
	 *
	 *	dsdapi_unix_socket_sap_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static const char * dsdapi_unix_socket_sap_id ();

	/** @brief cpap_inet_socket_sap_id method
	 *
	 *	cpap_inet_socket_sap_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static const char * cpap_inet_socket_sap_id ();

	/** @brief dsddsd_inet_socket_sap_id method
	 *
	 *	dsddsd_inet_socket_sap_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static const char * dsddsd_inet_socket_sap_id ();

	/** @brief dsdapi_unix_socket_sap_pathname method
	 *
	 *	dsdapi_unix_socket_sap_pathname method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static const char * dsdapi_unix_socket_sap_pathname ();

	/** @brief system_id _to_system_type method
	 *
	 *	system_id _to_system_type method detailed description
	 *
	 *	@param[in] system_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	static inline acs_dsd::SystemTypeConstants system_id_to_system_type (int system_id) {
		acs_dsd::SystemTypeConstants system_type = acs_dsd::SYSTEM_TYPE_UNKNOWN;
		switch (system_id) {
		case acs_dsd::CONFIG_BC_ID_SET_START ... acs_dsd::CONFIG_BC_ID_SET_END:
			system_type = acs_dsd::SYSTEM_TYPE_BC;
			break;
		case acs_dsd::CONFIG_CP_ID_SET_START ... acs_dsd::CONFIG_CP_ID_SET_END:
		case acs_dsd::SYSTEM_ID_CP_ALARM_MASTER:
		case acs_dsd::SYSTEM_ID_CP_CLOCK_MASTER:
			system_type = acs_dsd::SYSTEM_TYPE_CP;
			break;
		case acs_dsd::CONFIG_AP_ID_SET_START ... acs_dsd::CONFIG_AP_ID_SET_END:
		case acs_dsd::SYSTEM_ID_FRONT_END_AP:
		case acs_dsd::SYSTEM_ID_PARTNER_NODE:
		case acs_dsd::SYSTEM_ID_THIS_NODE:
			system_type = acs_dsd::SYSTEM_TYPE_AP;
			break;
		}

		return system_type;
	}

	static int system_id_to_system_name (int system_id, char * system_name, size_t size);
	static int system_id_to_system_name (int system_id, std::string & system_name);

	static int get_dsd_imm_root_dn (const char * & dn_path);

	static int process_running (const char * process_name, pid_t pid);

	//===========//
	// Operators //
	//===========//
private:
	inline __CLASS_NAME__ & operator= (const __CLASS_NAME__ &) {
		ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));
		return *this;
	}
};

#endif // HEADER_GUARD_CLASS__ACS_DSD_ConfigurationHelper
