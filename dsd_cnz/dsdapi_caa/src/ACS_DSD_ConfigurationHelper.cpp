#include <cstring>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <map>

#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Guard_T.h"

#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

#include "acs_apgcc_omhandler.h"

#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_ConfigurationHelper.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

namespace {
	typedef std::multimap < uint32_t, __CLASS_NAME__::HostInfo_const_pointer_t > nodes_by_ip_map_t;

	__CLASS_NAME__::HostInfo _host_info_null_object;
	__CLASS_NAME__::HostInfo _system_ap_nodes[acs_dsd::CONFIG_AP_NODES_SUPPORTED] = {__CLASS_NAME__::HostInfo()};
	__CLASS_NAME__::HostInfo _system_cp_nodes[acs_dsd::CONFIG_CP_NODES_SUPPORTED] = {__CLASS_NAME__::HostInfo()};

	nodes_by_ip_map_t _system_nodes_by_ip_map;

	int _my_ap_node_index = -1;
	int _my_ap_partner_node_index = -1;
	unsigned _system_ap_nodes_count = 0;
	unsigned _system_cp_nodes_count = 0;
	int32_t _ap_id_min = -1;
	int32_t _ap_id_max = -1;
	int32_t _bc_id_min = -1;
	int32_t _bc_id_max = -1;
	int32_t _cp_id_min = -1;
	int32_t _cp_id_max = -1;

	volatile int info_already_load_ap_nodes = 0;
	volatile int info_already_load_cp_nodes = 0;
	volatile int info_already_load_process = 0;
	volatile int info_already_load_my_ap_node = 0;

#ifndef _GNU_SOURCE
	volatile char _process_name[acs_dsd::CONFIG_FILENAME_SIZE_MAX] = {0};
#endif

	ACE_Recursive_Thread_Mutex _loading_sync_object;

#ifdef ACS_DSD_LOAD_CONFIGURATION_AT_STARTUP
	struct ACS_DSD_ConfigurationHelperStartupLoader {
		inline ACS_DSD_ConfigurationHelperStartupLoader() {
			__CLASS_NAME__::load_ap_nodes_from_cs();
			__CLASS_NAME__::load_cp_nodes_from_cs();
			__CLASS_NAME__::load_process_information();
			__CLASS_NAME__::load_my_ap_node();
		}
		inline ~ACS_DSD_ConfigurationHelperStartupLoader() {}
	} _startup_loader;
#endif

	int set_host_info_from_cs (
			ACS_CS_API_HWC * hwc,
			unsigned short board_id,
			__CLASS_NAME__::HostInfo * p,
			ACS_CS_API_HWC_NS::HWC_SystemType_Identifier system_type,
			int32_t * (& min_max_id_pointers) [2]) {
		uint16_t system_id = 0;
		uint16_t system_num = 0;
		uint16_t node_side = 0xBAAD;

		uint32_t ip_eths[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};
		uint32_t alias_eths[acs_dsd::CONFIG_NETWORKS_SUPPORTED] = {0};

		char node_side_char = '\0';

		if (hwc->getSysId(system_id, board_id) ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_SYSTEM_ID;

		if (system_type ^ ACS_CS_API_HWC_NS::SysType_BC) {
			if (hwc->getSysNo(system_num, board_id) ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_SYSTEM_NUMBER;
			if (hwc->getSide(node_side, board_id) ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_SIDE;
		} else {
			if (hwc->getSeqNo(system_num, board_id) ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_SYSTEM_SEQUENCE;
		}

		ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_Success;

		if (ACS_DSD_ARRAY_SIZE(ip_eths) >= 1) {
			if (((cs_call_result = hwc->getIPEthA(ip_eths[0], board_id)) == ACS_CS_API_NS::Result_NoValue) && (system_type == ACS_CS_API_HWC_NS::SysType_AP))
				ip_eths[0] = 0;
			else if (cs_call_result ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_IP_ETH_A;
		}
		if (ACS_DSD_ARRAY_SIZE(ip_eths) >= 2) {
			if (((cs_call_result = hwc->getIPEthB(ip_eths[1], board_id)) == ACS_CS_API_NS::Result_NoValue) && (system_type == ACS_CS_API_HWC_NS::SysType_AP))
				ip_eths[1] = 0;
			else if (cs_call_result ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_IP_ETH_B;
		}

		if (ACS_DSD_ARRAY_SIZE(alias_eths) >= 1) {
			if ((cs_call_result = hwc->getAliasEthA(alias_eths[0], board_id)) == ACS_CS_API_NS::Result_NoValue) alias_eths[0] = 0;
			else if (cs_call_result ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_ALIAS_ETH_A;
		}
		if (ACS_DSD_ARRAY_SIZE(alias_eths) >= 2) {
			if ((cs_call_result = hwc->getAliasEthB(alias_eths[1], board_id)) == ACS_CS_API_NS::Result_NoValue) alias_eths[1] = 0;
			else if (cs_call_result ^ ACS_CS_API_NS::Result_Success) return acs_dsd::ERR_CS_GET_ALIAS_ETH_B;
		}

		in_addr addr;

		p->system_id = system_id;

		(p->system_id < *(min_max_id_pointers[0])) && (*(min_max_id_pointers[0]) = p->system_id);
		(p->system_id > *(min_max_id_pointers[1])) && (*(min_max_id_pointers[1]) = p->system_id);

		switch (system_type) {
		case ACS_CS_API_HWC_NS::SysType_BC:
			p->system_type = acs_dsd::SYSTEM_TYPE_BC;
			snprintf(p->system_name, ACS_DSD_ARRAY_SIZE(p->system_name), ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_BC"%u", system_num);
			break;
		case ACS_CS_API_HWC_NS::SysType_CP:
			p->system_type = acs_dsd::SYSTEM_TYPE_CP;
			snprintf(p->system_name, ACS_DSD_ARRAY_SIZE(p->system_name), ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_CP"%u", system_num);
			break;
		case ACS_CS_API_HWC_NS::SysType_AP:
			p->system_type = acs_dsd::SYSTEM_TYPE_AP;
			snprintf(p->system_name, ACS_DSD_ARRAY_SIZE(p->system_name), ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_AP"%u", system_num);
			break;
		default:
			p->system_type = acs_dsd::SYSTEM_TYPE_UNKNOWN;
			snprintf(p->system_name, ACS_DSD_ARRAY_SIZE(p->system_name), "UNKNOWN");
			break;
		}

		uint32_t side_dx = 0;

		switch (node_side) {
		case ACS_CS_API_HWC_NS::Side_A: p->side = acs_dsd::NODE_SIDE_A; node_side_char = 'A'; side_dx = 1; break;
		case ACS_CS_API_HWC_NS::Side_B: p->side = acs_dsd::NODE_SIDE_B; node_side_char = 'B'; break;
		default: p->side = acs_dsd::NODE_SIDE_UNDEFINED; break;
		}

		snprintf(p->node_name, ACS_DSD_ARRAY_SIZE(p->node_name), "%s%c", p->system_name, node_side_char);

		uint32_t host_side_octet = ((system_type == ACS_CS_API_HWC_NS::SysType_AP) ? (((system_id - acs_dsd::CONFIG_AP_ID_SET_START) << 1) - side_dx) : 0);

		p->ip4_adresses_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED;
		for (uint32_t i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) {
			if (!ip_eths[i] && (system_type == ACS_CS_API_HWC_NS::SysType_AP)) ip_eths[i] = (0xC0A8A900U + (i << 8)) + host_side_octet;
			addr.s_addr = p->ip4_addresses[i] = htonl(ip_eths[i]);
			inet_ntop(AF_INET, &addr, p->ip4_addresses_str[i], ACS_DSD_ARRAY_SIZE(p->ip4_addresses_str[i]));
		}

		p->ip4_aliases_count = 0;
		for (uint32_t i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) if (alias_eths[i]) {
			addr.s_addr = p->ip4_aliases[i] = htonl(alias_eths[i]);
			inet_ntop(AF_INET, &addr, p->ip4_aliases_str[i], ACS_DSD_ARRAY_SIZE(p->ip4_aliases_str[i]));
			p->ip4_aliases_count++;
		}

		//Add the host info to the multimap by ip addresses.
		for (size_t i = 0; i < p->ip4_adresses_count; ++i) if(p->ip4_addresses[i])
			_system_nodes_by_ip_map.insert(nodes_by_ip_map_t::value_type(p->ip4_addresses[i], p));

		for (size_t i = 0; i < p->ip4_aliases_count; ++i) if (p->ip4_aliases[i])
			_system_nodes_by_ip_map.insert(nodes_by_ip_map_t::value_type(p->ip4_aliases[i], p));

		return 0;
	}
}

void __CLASS_NAME__::force_configuration_reload () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_loading_sync_object, true);

	_system_nodes_by_ip_map.clear();

	_my_ap_node_index = -1;
	_my_ap_partner_node_index = -1;
	_system_ap_nodes_count = 0;
	_system_cp_nodes_count = 0;
	_ap_id_min = -1;
	_ap_id_max = -1;
	_bc_id_min = -1;
	_bc_id_max = -1;
	_cp_id_min = -1;
	_cp_id_max = -1;

#ifndef _GNU_SOURCE
	*_process_name = 0;
#endif

	info_already_load_ap_nodes = 0;
	info_already_load_cp_nodes = 0;
	info_already_load_process = 0;
	info_already_load_my_ap_node = 0;
}

int __CLASS_NAME__::load_ap_nodes_from_cs () {
	if (info_already_load_ap_nodes) return 0;

	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::load_ap_nodes_from_cs()");

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_loading_sync_object, true);

	if (info_already_load_ap_nodes) return 0;

	if (!guard.locked()) return acs_dsd::ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE;

	int return_code = 0;

#if ACS_DSD_CONFIG_AP_LOADING_FROM_CS == 1

	ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_Success;

	_ap_id_min = acs_dsd::CONFIG_AP_ID_SET_END;

	if (ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance()) {
		ACS_CS_API_IdList board_list;

		if (ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance()) {
			HostInfo * p = _system_ap_nodes;
			unsigned board_count = 0;
			unsigned total_board_count = 0;

			// set Board search criteria
			bs->reset();
			bs->setSysType(ACS_CS_API_HWC_NS::SysType_AP);
			bs->setFBN(ACS_CS_API_HWC_NS::FBN_APUB);

			int32_t * min_max_id_pointers[2] = {&_ap_id_min, &_ap_id_max};

			// get BoardList for selected criteria
			if ((cs_call_result = hwc->getBoardIds(board_list, bs)) == ACS_CS_API_NS::Result_Success) {
				total_board_count += (board_count = board_list.size());
				for (unsigned j = 0;
						 !return_code && (j < board_count);
						 return_code = set_host_info_from_cs(hwc, board_list[j++], p++, ACS_CS_API_HWC_NS::SysType_AP, min_max_id_pointers)
					) ;
			} else return_code = acs_dsd::ERR_CS_GET_BOARD_IDS;

			return_code || ((_system_ap_nodes_count = total_board_count), (info_already_load_ap_nodes = 1));

			ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		} else return_code = acs_dsd::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;

		ACS_CS_API::deleteHWCInstance(hwc);
	} else return_code = acs_dsd::ERR_CS_CREATE_HWC_INSTANCE;

#else

	HostInfo * p = _system_ap_nodes;
	in_addr addr;

	//const int AP_CLUSTER_COUNT = 2;

	_system_ap_nodes_count = acs_dsd::CONFIG_AP_CLUSTERS_SUPPORTED * acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED;

	int ip_addr_last_octet = 1;

	_ap_id_min = acs_dsd::CONFIG_AP_ID_SET_END;

	for (int ap_cluster = 1; ap_cluster <= acs_dsd::CONFIG_AP_CLUSTERS_SUPPORTED; ++ap_cluster)
		for (int ap_node = 0; ap_node < acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED; ++ap_node, ++ip_addr_last_octet, ++p) {
			p->system_id = acs_dsd::CONFIG_AP_ID_SET_START + ap_cluster;

			(p->system_id < _ap_id_min) && (_ap_id_min = p->system_id);
			(p->system_id > _ap_id_max) && (_ap_id_max = p->system_id);

			p->system_type = acs_dsd::SYSTEM_TYPE_AP;
			//p->cs_system_type = acs_dsd::CONFIG_AP_ID_START_BASE;
			//p->cs_system_num = ap_cluster;
			p->side = static_cast<acs_dsd::NodeSideConstants>(ap_node);
			p->ip4_adresses_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED;
			p->ip4_aliases_count = 0; //No IP aliases for the APs

			for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) {
				snprintf(p->ip4_addresses_str[i], acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX, "192.168.%d.%d", 169 + i, ip_addr_last_octet);
				p->ip4_aliases_str[i][0] = 0; //No IP aliases for the APs

				inet_aton(p->ip4_addresses_str[i], &addr);
				p->ip4_addresses[i] = addr.s_addr;
				p->ip4_aliases[i] = 0; //No IP aliases for the APs

				//Insert this ap node into the nodes-by-ip multi-map.
				_system_nodes_by_ip_map.insert(nodes_by_ip_map_t::value_type(addr.s_addr, p));
			}

			snprintf(p->system_name, acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX, ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_AP"%d", ap_cluster);
			snprintf(p->node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX, "%s%c", p->system_name, 'A' + ap_node);
		}

	(_ap_id_min ^ acs_dsd::CONFIG_AP_ID_SET_END) || (_ap_id_min = -1);

	info_already_load_ap_nodes = 1;

#endif

	return return_code;
}

int __CLASS_NAME__::load_cp_nodes_from_cs () {
	if (info_already_load_cp_nodes) return 0;

	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::load_cp_nodes_from_cs()");

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_loading_sync_object, true);

	if (info_already_load_cp_nodes) return 0;

	if (!guard.locked()) return acs_dsd::ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE;

	int return_code = 0;

#if ACS_DSD_CONFIG_CP_LOADING_FROM_CS == 1

	ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_Success;

	_bc_id_min = acs_dsd::CONFIG_BC_ID_SET_END;
	_cp_id_min = acs_dsd::CONFIG_CP_ID_SET_END;

	if (ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance()) {
		ACS_CS_API_IdList board_list;

		if (ACS_CS_API_BoardSearch * bs = ACS_CS_API_HWC::createBoardSearchInstance()) {
			HostInfo * p = _system_cp_nodes;
			unsigned board_count = 0;
			unsigned total_board_count = 0;
			ACS_CS_API_HWC_NS::HWC_SystemType_Identifier system_types[] = {ACS_CS_API_HWC_NS::SysType_BC, ACS_CS_API_HWC_NS::SysType_CP};
			int32_t * min_max_id_pointers[][2] = {{&_bc_id_min, &_bc_id_max}, {&_cp_id_min, &_cp_id_max}};

			for (size_t i = 0; !return_code && (i < ACS_DSD_ARRAY_SIZE(system_types)); ++i) {
				// set Board search criteria
				bs->reset();
				bs->setSysType(system_types[i]);
				bs->setFBN(ACS_CS_API_HWC_NS::FBN_CPUB);

				// get BoardList for selected criteria
				if ((cs_call_result = hwc->getBoardIds(board_list, bs)) == ACS_CS_API_NS::Result_Success) {
					total_board_count += (board_count = board_list.size());
					for (unsigned j = 0;
							 !return_code && (j < board_count);
							 return_code = set_host_info_from_cs(hwc, board_list[j++], p++, system_types[i], min_max_id_pointers[i])
						) ;
				} else return_code = acs_dsd::ERR_CS_GET_BOARD_IDS;
			}

			return_code || ((_system_cp_nodes_count = total_board_count), (info_already_load_cp_nodes = 1));

			ACS_CS_API_HWC::deleteBoardSearchInstance(bs);
		} else return_code = acs_dsd::ERR_CS_CREATE_BOARD_SEARCH_INSTANCE;

		ACS_CS_API::deleteHWCInstance(hwc);
	} else return_code = acs_dsd::ERR_CS_CREATE_HWC_INSTANCE;

	(_bc_id_min ^ acs_dsd::CONFIG_BC_ID_SET_END) || (_bc_id_min = -1);
	(_cp_id_min ^ acs_dsd::CONFIG_CP_ID_SET_END) || (_cp_id_min = -1);

#else

	HostInfo * p = _system_cp_nodes;
	in_addr addr;

	int ip_addr_last_octet = 55;
	int bc_max = 16;

	_bc_id_min = acs_dsd::CONFIG_BC_ID_SET_END;

	for (int bc = 0; bc < bc_max; ++bc, ++ip_addr_last_octet, ++p) {
		p->system_id = acs_dsd::CONFIG_BC_ID_SET_START + bc;

		(p->system_id < _bc_id_min) && (_bc_id_min = p->system_id);
		(p->system_id > _bc_id_max) && (_bc_id_max = p->system_id);

		p->system_type = acs_dsd::SYSTEM_TYPE_BC;
		//p->cs_system_type = acs_dsd::CONFIG_BC_ID_START_BASE;
		//p->cs_system_num = bc;
		p->side = acs_dsd::NODE_SIDE_UNDEFINED;
		p->ip4_adresses_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED;
		p->ip4_aliases_count = 0; //No IP aliases for the BCs

		for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) {
			snprintf(p->ip4_addresses_str[i], acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX, "192.168.%d.%d", 169 + i, ip_addr_last_octet);
			p->ip4_aliases_str[i][0] = 0; //No IP aliases for the BCs

			inet_aton(p->ip4_addresses_str[i], &addr);
			p->ip4_addresses[i] = addr.s_addr;
			p->ip4_aliases[i] = 0; //No IP aliases for the BCs

			//Insert this bc node into the nodes-by-ip multi-map.
			_system_nodes_by_ip_map.insert(nodes_by_ip_map_t::value_type(addr.s_addr, p));
		}

		snprintf(p->system_name, acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX, ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_BC"%d", bc);
		snprintf(p->node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX, ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_BC"%d", bc);
	}

	(_bc_id_min ^ acs_dsd::CONFIG_BC_ID_SET_END) || (_bc_id_min = -1);

	ip_addr_last_octet = 100;
	int alias_addr_last_octet = 10;
	int cp_max = 2;

	_cp_id_min = acs_dsd::CONFIG_CP_ID_SET_END;

	for (int cp = 1; cp <= cp_max; ++cp) {
		for (int cp_node = 0; cp_node < acs_dsd::CONFIG_AP_NODES_PER_CLUSTER_SUPPORTED; ++cp_node, ++ip_addr_last_octet, ++alias_addr_last_octet, ++p) {
			p->system_id = acs_dsd::CONFIG_CP_ID_SET_START + cp;

			(p->system_id < _cp_id_min) && (_cp_id_min = p->system_id);
			(p->system_id > _cp_id_max) && (_cp_id_max = p->system_id);

			p->system_type = acs_dsd::SYSTEM_TYPE_CP;
			//p->cs_system_type = acs_dsd::CONFIG_CP_ID_START_BASE;
			//p->cs_system_num = cp;
			p->side = static_cast<acs_dsd::NodeSideConstants>(cp_node);
			p->ip4_adresses_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED;
			p->ip4_aliases_count = acs_dsd::CONFIG_NETWORKS_SUPPORTED;

			for (int i = 0; i < acs_dsd::CONFIG_NETWORKS_SUPPORTED; ++i) {
				snprintf(p->ip4_addresses_str[i], acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX, "192.168.%d.%d", 169 + i, ip_addr_last_octet);
				snprintf(p->ip4_aliases_str[i], acs_dsd::CONFIG_IP4_ADDRESS_STRING_SIZE_MAX, "172.17.%d.%d", 139 + i, alias_addr_last_octet);

				inet_aton(p->ip4_addresses_str[i], &addr);
				p->ip4_addresses[i] = addr.s_addr;
				_system_nodes_by_ip_map.insert(nodes_by_ip_map_t::value_type(addr.s_addr, p)); //Insert this cp node into the nodes-by-ip multi-map.

				inet_aton(p->ip4_aliases_str[i], &addr);
				p->ip4_aliases[i] = addr.s_addr;
				_system_nodes_by_ip_map.insert(nodes_by_ip_map_t::value_type(addr.s_addr, p)); //Insert this cp node into the nodes-by-ip multi-map.

//OLD: TO BE DELETED
//				//Insert this ap node into the nodes-by-ip multi-map.
//				_system_nodes_by_ip_map.insert(nodes_by_ip_map_t::value_type(addr.s_addr, p));
			}

			snprintf(p->system_name, acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX, ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_CP"%d", cp);
			snprintf(p->node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX, "%s%c", p->system_name, 'A' + cp_node);
		}
	}

	(_cp_id_min ^ acs_dsd::CONFIG_CP_ID_SET_END) || (_cp_id_min = -1);

	_system_cp_nodes_count = bc_max + (cp_max << 1);

	info_already_load_cp_nodes = 1;

#endif

	return return_code;
}

int __CLASS_NAME__::load_process_information () {
	if (info_already_load_process) return 0;

	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::load_process_information()");

	int return_code = 0;

#ifdef _GNU_SOURCE

	info_already_load_process = 1;

#else

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_loading_sync_object, true);

	if (info_already_load_process) return 0;

	if (!guard.locked()) return acs_dsd::ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE;

	FILE * proc_stat_handle = fopen("/proc/self/stat", "r");
	if (!proc_stat_handle) return acs_dsd::ERR_SYSTEM_OPEN;

	int pid = 0;
	char pname[acs_dsd::CONFIG_FILENAME_SIZE_MAX + 1];
	int errno_save = 0;

	errno = 0;
	if (fscanf(proc_stat_handle, "%d (%"ACS_DSD_STRINGIZE(ACS_DSD_CONFIG_FILENAME_SIZE_MAX)"s", &pid, pname) >= 2) {
		if (char * p = reinterpret_cast<char *>(memccpy(_process_name, pname, ')', acs_dsd::CONFIG_FILENAME_SIZE_MAX))) *--p = 0;
		_process_name[acs_dsd::CONFIG_FILENAME_SIZE_MAX - 1] = 0;
	} else {
		errno_save = errno;
		return_code = acs_dsd::ERR_SYSTEM_READ;
	}

	fclose(proc_stat_handle);

	return_code ? (errno = errno_save) : (info_already_load_process = 1);

#endif

	return return_code;
}

int __CLASS_NAME__::load_my_ap_node () {
	if (info_already_load_my_ap_node) return 0;

	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::find_my_ap_node()");

	if (const int call_result = load_ap_nodes_from_cs()) return call_result; //return acs_dsd::ERR_LOAD_AP_NODES_FROM_CS;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_loading_sync_object, true);

	if (info_already_load_my_ap_node) return 0;

	if (!guard.locked()) return acs_dsd::ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE;

	int ap_index = -1;
	int check_socket = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;

	for (int ipn = 0; (ap_index < 0) && (ipn < acs_dsd::CONFIG_NETWORKS_SUPPORTED); ++ipn)
		for (unsigned i = 0; (ap_index < 0) && (i < _system_ap_nodes_count); ++i) {
			addr.sin_addr.s_addr = _system_ap_nodes[i].ip4_addresses[ipn];
			bind(check_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) || (ap_index = i);
		}

	::close(check_socket);

	_my_ap_node_index = ap_index;

	//If my ap node found then I will find my partner AP node
	if (ap_index >= 0) {
		const short my_ap_system_id = _system_ap_nodes[ap_index].system_id;
		for (unsigned i = 0; (_my_ap_partner_node_index < 0) && (i < _system_ap_nodes_count); ++i)
			(_system_ap_nodes[i].system_id == my_ap_system_id) && (i ^ ap_index) && (_my_ap_partner_node_index = i);

		if (_my_ap_partner_node_index < 0) force_configuration_reload();
		else info_already_load_my_ap_node = 1;
	} else {
		//ERROR: No matching IP address found. This machine has a network configuration
		//that does not match any IP address in my lists. This may be a consequence of a partial
		//configuration data loading, so a configuration reload will be forced.
		force_configuration_reload();
		return acs_dsd::ERR_NODE_NETWORK_CONFIGURATION;
	}

	return 0;
}

int __CLASS_NAME__::get_node_by_ip (uint32_t net_order_ip, HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_loading_sync_object, true);

	if ((call_result = load_ap_nodes_from_cs())) return call_result;
	if ((call_result = load_cp_nodes_from_cs())) return call_result;

	//Check for bad configuration about the possibility to have one IP for more nodes.
	if (_system_nodes_by_ip_map.count(net_order_ip) > 1) //ERROR: same IP for more than one system node
		return acs_dsd::ERR_CONFIG_NODE_IP_CONFLICT;

	nodes_by_ip_map_t::const_iterator p = _system_nodes_by_ip_map.find(net_order_ip);

	//Force a configuration reload in case of node not found.
	return (p == _system_nodes_by_ip_map.end()
			? (force_configuration_reload(), (node_info_ptr = 0), acs_dsd::ERR_NODE_NOT_FOUND)
			: ((node_info_ptr = p->second), 0));
}

int __CLASS_NAME__::get_node_by_ip (uint32_t net_order_ip, HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_node_by_ip(net_order_ip, host_info_ptr);

	if (call_result) return call_result;

	return (host_info_ptr ? (node_info = *host_info_ptr, 0) : acs_dsd::ERR_NODE_NOT_FOUND);
}

int __CLASS_NAME__::get_node_by_node_name (const char * node_name, HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check node_name parameter
	if (!node_name || !*node_name) return acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL;

	int call_result = 0;

	if ((call_result = load_ap_nodes_from_cs())) return call_result;
	if ((call_result = load_cp_nodes_from_cs())) return call_result;

	//Search in AP nodes
	for (unsigned i = 0; i < _system_ap_nodes_count; ++i)
		if (!strncasecmp(_system_ap_nodes[i].node_name, node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX)) {
			node_info_ptr = _system_ap_nodes + i;
			return 0;
		}

	//Search in CP/BC nodes
	for (unsigned i = 0; i < _system_cp_nodes_count; ++i)
		if (!strncasecmp(_system_cp_nodes[i].node_name, node_name, acs_dsd::CONFIG_NODE_NAME_SIZE_MAX)) {
			node_info_ptr = _system_cp_nodes + i;
			return 0;
		}

	//Node not found: force a reload configuration also.
	force_configuration_reload();

	return acs_dsd::ERR_NODE_NOT_FOUND;
}

int __CLASS_NAME__::get_node_by_node_name (const char * node_name, HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_node_by_node_name(node_name, host_info_ptr);

	if (call_result) return call_result;

	return (host_info_ptr ? (node_info = *host_info_ptr, 0) : acs_dsd::ERR_NODE_NOT_FOUND);
}

int __CLASS_NAME__::get_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;

	if ((call_result = load_ap_nodes_from_cs())) return call_result;
	if ((call_result = load_cp_nodes_from_cs())) return call_result;

	//Search in AP nodes
	for (unsigned i = 0; i < _system_ap_nodes_count; ++i)
		if ((_system_ap_nodes[i].system_id == system_id) && (_system_ap_nodes[i].side == side)) {
			node_info_ptr = _system_ap_nodes + i;
			return 0;
		}

	//Search in CP/BC nodes
	for (unsigned i = 0; i < _system_cp_nodes_count; ++i)
		if ((_system_cp_nodes[i].system_id == system_id) && (_system_cp_nodes[i].side == side)) {
			node_info_ptr = _system_cp_nodes + i;
			return 0;
		}

	//Node not found: force a reload configuration also.
	force_configuration_reload();

	return acs_dsd::ERR_NODE_NOT_FOUND;
}

int __CLASS_NAME__::get_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_node_by_side(system_id, side, host_info_ptr);

	if (call_result) return call_result;

	return (host_info_ptr ? (node_info = *host_info_ptr, 0) : acs_dsd::ERR_NODE_NOT_FOUND);
}

int __CLASS_NAME__::get_partner_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check parameters
	if (side == acs_dsd::NODE_SIDE_UNDEFINED) return acs_dsd::ERR_BAD_PARAMETER_VALUE;

	return get_node_by_side(system_id, (side ^ acs_dsd::NODE_SIDE_A) ? acs_dsd::NODE_SIDE_A : acs_dsd::NODE_SIDE_B, node_info_ptr);
}

int __CLASS_NAME__::get_partner_node_by_side (int32_t system_id, acs_dsd::NodeSideConstants side, HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_partner_node_by_side(system_id, side, host_info_ptr);

	if (call_result) return call_result;

	return (host_info_ptr ? (node_info = *host_info_ptr, 0) : acs_dsd::ERR_NODE_NOT_FOUND);
}

int __CLASS_NAME__::get_my_ap_node (HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (const int call_result = load_my_ap_node()) return call_result;

	if ( _my_ap_node_index < 0 ) // ERROR: Configuration inconsistency
		return acs_dsd::ERR_AP_CONFIGURATION_MISMATCH_FROM_CS;

	node_info_ptr = _system_ap_nodes + _my_ap_node_index;

	return 0;
}

int __CLASS_NAME__::get_my_ap_node (HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_my_ap_node(host_info_ptr);
	return call_result ?: (node_info = *host_info_ptr, 0);
}

int __CLASS_NAME__::get_my_ap_partner_node (HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (const int call_result = load_my_ap_node()) return call_result;

	if ( _my_ap_partner_node_index < 0 ) // ERROR: Configuration inconsistency
		return acs_dsd::ERR_AP_CONFIGURATION_MISMATCH_FROM_CS;

	node_info_ptr = _system_ap_nodes + _my_ap_partner_node_index;

	return 0;
}

int __CLASS_NAME__::get_my_ap_partner_node (HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_my_ap_partner_node(host_info_ptr);
	return call_result ?: (node_info = *host_info_ptr, 0);
}

int __CLASS_NAME__::get_ap_node (unsigned index, HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (const int call_result = load_ap_nodes_from_cs()) return call_result;

	if ( !info_already_load_ap_nodes ) // ERROR: Configuration inconsistency
		return acs_dsd::ERR_AP_CONFIGURATION_MISMATCH_FROM_CS;

	return (index < _system_ap_nodes_count ? (node_info_ptr = _system_ap_nodes + index, 0) : acs_dsd::ERR_INDEX_OUT_OF_RANGE);
}

int __CLASS_NAME__::get_ap_node (unsigned index, HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_ap_node(index, host_info_ptr);
	return call_result ?: (node_info = *host_info_ptr, 0);
}

int __CLASS_NAME__::get_all_ap_nodes (std::vector<HostInfo> & ap_nodes) {
	if (const int call_result = load_ap_nodes_from_cs()) return call_result;

	if ( !info_already_load_ap_nodes ) // ERROR: Configuration inconsistency
			return acs_dsd::ERR_AP_CONFIGURATION_MISMATCH_FROM_CS;

	ap_nodes.clear();

	for (unsigned i = 0; i < _system_ap_nodes_count; ap_nodes.push_back(_system_ap_nodes[i++])) ;

	return 0;
}

int __CLASS_NAME__::get_cp_node (unsigned index, HostInfo_const_pointer_t & node_info_ptr) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (const int call_result = load_cp_nodes_from_cs()) return call_result;

	if ( !info_already_load_cp_nodes ) // ERROR: Configuration inconsistency
		return acs_dsd::ERR_AP_CONFIGURATION_MISMATCH_FROM_CS;

	return (index < _system_cp_nodes_count ? (node_info_ptr = _system_cp_nodes + index, 0) : acs_dsd::ERR_INDEX_OUT_OF_RANGE);
}

int __CLASS_NAME__::get_cp_node (unsigned index, HostInfo & node_info) {
	HostInfo_const_pointer_t host_info_ptr = 0;
	const int call_result = get_cp_node(index, host_info_ptr);
	return call_result ?: (node_info = *host_info_ptr, 0);
}

int32_t __CLASS_NAME__::get_ap_id_min () { load_ap_nodes_from_cs(); return _ap_id_min; }

int32_t __CLASS_NAME__::get_ap_id_max () { load_ap_nodes_from_cs(); return _ap_id_max; }

int32_t __CLASS_NAME__::get_bc_id_min () { load_cp_nodes_from_cs(); return _bc_id_min; }

int32_t __CLASS_NAME__::get_bc_id_max () { load_cp_nodes_from_cs(); return _bc_id_max; }

int32_t __CLASS_NAME__::get_cp_id_min () { load_cp_nodes_from_cs(); return _cp_id_min; }

int32_t __CLASS_NAME__::get_cp_id_max () { load_cp_nodes_from_cs(); return _cp_id_max; }

int __CLASS_NAME__::set_alarm_master_in_cs (int32_t system_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (
			((system_id < acs_dsd::CONFIG_BC_ID_SET_START) || (acs_dsd::CONFIG_BC_ID_SET_END < system_id)) &&
			((system_id < acs_dsd::CONFIG_CP_ID_SET_START) || (acs_dsd::CONFIG_CP_ID_SET_END < system_id))
	) //CP system id non valid
		return acs_dsd::ERR_BAD_SYSTEM_ID;

	CPID cp_id = static_cast<CPID>(system_id);

	if (ACS_CS_API_Set::setAlarmMaster(cp_id) ^ ACS_CS_API_SET_NS::Result_Success) //Error from CS setting alarm master
		return acs_dsd::ERR_CS_SET_ALARM_MASTER;

	return 0;
}

int __CLASS_NAME__::set_clock_master_in_cs (int32_t system_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (
			((system_id < acs_dsd::CONFIG_BC_ID_SET_START) || (acs_dsd::CONFIG_BC_ID_SET_END < system_id)) &&
			((system_id < acs_dsd::CONFIG_CP_ID_SET_START) || (acs_dsd::CONFIG_CP_ID_SET_END < system_id))
	) //CP system id non valid
		return acs_dsd::ERR_BAD_SYSTEM_ID;

	CPID cp_id = static_cast<CPID>(system_id);

	if (ACS_CS_API_Set::setClockMaster(cp_id) ^ ACS_CS_API_SET_NS::Result_Success) //Error from CS setting clock master
		return acs_dsd::ERR_CS_SET_CLOCK_MASTER;

	return 0;
}

unsigned __CLASS_NAME__::system_ap_nodes_count () {
	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::system_ap_nodes_count()");

	load_ap_nodes_from_cs();

	return _system_ap_nodes_count;
}

unsigned __CLASS_NAME__::system_cp_nodes_count () {
	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::system_cp_nodes_count()");

	load_cp_nodes_from_cs();

	return _system_cp_nodes_count;
}

bool __CLASS_NAME__::system_exist (int32_t system_id) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	int call_result = 0;

	if ((call_result = load_ap_nodes_from_cs())) return call_result;
	if ((call_result = load_cp_nodes_from_cs())) return call_result;

	//Search in AP nodes
	for (unsigned i = 0; i < _system_ap_nodes_count; ++i) if (_system_ap_nodes[i].system_id == system_id) return true;

	//Search in CP/BC nodes
	for (unsigned i = 0; i < _system_cp_nodes_count; ++i) if (_system_cp_nodes[i].system_id == system_id) return true;

	//Node not found
	return false;
}

pid_t __CLASS_NAME__::my_process_id () {
	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::my_process_id()");

	return getpid();
}

const char * __CLASS_NAME__::my_process_name () {
	//ACS_DSD_TRACE(ACS_DSD_STRINGIZE(__CLASS_NAME__)"::my_process_name()");

	load_process_information();

#ifdef _GNU_SOURCE
	return program_invocation_short_name;
#else
	return _process_name;
#endif
}

const char * __CLASS_NAME__::unix_socket_root_path () {
	static const char * const _unix_sockets_root_path = ACS_DSD_CONFIG_UNIX_SOCKETS_ROOT_PATH;
	return _unix_sockets_root_path;
}

const char * __CLASS_NAME__::dsdapi_unix_socket_sap_id () {
	static const char * const _dsdapi_unix_socket_sap_id = ACS_DSD_CONFIG_DSDAPI_UNIX_SOCKET_SAP_ID;
	return _dsdapi_unix_socket_sap_id;
}

const char * __CLASS_NAME__::cpap_inet_socket_sap_id () {
	static const char * const _cpap_inet_socket_sap_id = ACS_DSD_CONFIG_CPAP_INET_SOCKET_SAP_ID;
	return _cpap_inet_socket_sap_id;
}

const char * __CLASS_NAME__::dsddsd_inet_socket_sap_id () {
	static const char * const _dsddsd_inet_socket_sap_id = ACS_DSD_CONFIG_DSDDSD_INET_SOCKET_SAP_ID;
	return _dsddsd_inet_socket_sap_id;
}

const char * __CLASS_NAME__::dsdapi_unix_socket_sap_pathname () {
	static const char * const _dsdapi_unix_socket_sap_pathname = ACS_DSD_CONFIG_UNIX_SOCKETS_ROOT_PATH ACS_DSD_CONFIG_DSDAPI_UNIX_SOCKET_SAP_ID;
	return _dsdapi_unix_socket_sap_pathname;
}

int __CLASS_NAME__::system_id_to_system_name (int system_id, char * system_name, size_t size) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//Check parameters
	if (!system_name) return acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL;
	if (size < acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX) return acs_dsd::ERR_NOT_ENOUGH_SPACE;

	switch (system_id_to_system_type(system_id)) {
	case acs_dsd::SYSTEM_TYPE_AP:
		if (system_id == acs_dsd::SYSTEM_ID_FRONT_END_AP) memccpy(system_name, "FRONT_END_AP", 0, size);
		else if (system_id == acs_dsd::SYSTEM_ID_PARTNER_NODE) memccpy(system_name, "PARTNER_NODE_AP", 0, size);
		else if (system_id == acs_dsd::SYSTEM_ID_THIS_NODE) memccpy(system_name, "THIS_NODE_AP", 0, size);
		else snprintf(system_name, size, ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_AP"%u", (system_id - acs_dsd::CONFIG_AP_ID_SET_START));
		break;
	case acs_dsd::SYSTEM_TYPE_BC:
		snprintf(system_name, size, ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_BC"%u", (system_id - acs_dsd::CONFIG_BC_ID_SET_START));
		break;
	case acs_dsd::SYSTEM_TYPE_CP:
		if (system_id == acs_dsd::SYSTEM_ID_CP_ALARM_MASTER) memccpy(system_name, "CP_ALARM_MASTER", 0, size);
		else if (system_id == acs_dsd::SYSTEM_ID_CP_CLOCK_MASTER) memccpy(system_name, "CP_CLOCK_MASTER", 0, size);
		else snprintf(system_name, size, ACS_DSD_CONFIG_SYSTEM_NAME_PREFIX_CP"%u", (system_id - acs_dsd::CONFIG_CP_ID_SET_START));
		break;
	default:
		memccpy(system_name, "UNKNOWN", 0, size);
		break;
	}
	system_name[size - 1] = 0;

	return 0;
}

int __CLASS_NAME__::system_id_to_system_name (int system_id, std::string & system_name) {
	char system_name_[acs_dsd::CONFIG_SYSTEM_NAME_SIZE_MAX] = {0};

	if (const int call_result = system_id_to_system_name(system_id, system_name_, ACS_DSD_ARRAY_SIZE(system_name_))) return call_result;

	system_name = system_name_;

	return 0;
}

int __CLASS_NAME__::get_dsd_imm_root_dn (const char * & dn_path) {
	static int info_already_load = 0;
	static char dsd_imm_root_dn[acs_dsd::CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};

	if (info_already_load) { dn_path = dsd_imm_root_dn; return 0; }

	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_loading_sync_object, true);

	if (info_already_load) { dn_path = dsd_imm_root_dn; return 0; }

	if (!guard.locked()) return acs_dsd::ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE;

	OmHandler om_handler;
	ACS_CC_ReturnType imm_call_result;

	errno = 0;
	if ((imm_call_result = om_handler.Init()) ^ ACS_CC_SUCCESS) return acs_dsd::ERR_SAF_IMM_OM_INIT;

	std::vector<std::string> dn_list;
	int return_code = 0;

	if ((imm_call_result = om_handler.getClassInstances(ACS_DSD_CONFIG_IMM_DSD_CLASS_NAME, dn_list)) ^ ACS_CC_SUCCESS) { //ERROR
		return_code = acs_dsd::ERR_SAF_IMM_OM_GET;
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'om_handler.getClassInstances(...' failed: retrieving DSD parameters root from IMM: IMM error code == %d",
				om_handler.getInternalLastError());
	} else { //OK: Checking how much dn items was found in IMM
		if (dn_list.size() ^ 1) { //WARNING: 0 (zero) or more than one node found
			return_code = acs_dsd::ERR_SAF_IMM_OM_GET;
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"WARNING: calling 'om_handler.getClassInstances(...': %s",
					dn_list.size() ? "more than one DSD parameters root node was found in IMM" : "DSD parameters root node not found in IMM");
		} else { //OK: Only one root node
			strncpy(dsd_imm_root_dn, dn_list[0].c_str(), ACS_DSD_ARRAY_SIZE(dsd_imm_root_dn));
			dsd_imm_root_dn[ACS_DSD_ARRAY_SIZE(dsd_imm_root_dn) - 1] = 0;
			info_already_load = 1;
			dn_path = dsd_imm_root_dn;
		}
	}

	om_handler.Finalize();

	return return_code;
}

int __CLASS_NAME__::process_running (const char * process_name, pid_t pid) {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	if (!process_name || !*process_name) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DEBUG: process_name is empty or NULL");

		return acs_dsd::ERR_PARAMETER_EMPTY_OR_NULL;
	}

	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DEBUG: process_name == '%s', pid == %d", process_name, pid);

	char proc_info_path [2048] = {0};

	snprintf(proc_info_path, ACS_DSD_ARRAY_SIZE(proc_info_path), ACS_DSD_CONFIG_PROC_FS_PATH"%d/"ACS_DSD_CONFIG_PROC_STATUS_FILENAME, pid);
	FILE * f = fopen(proc_info_path, "rt");
	if (!f) return 0; //Process not running in the system
	char proc_state = 0;
	fscanf(f, "%*s %*s %*s %c", &proc_state);
	fclose(f);

	snprintf(proc_info_path, ACS_DSD_ARRAY_SIZE(proc_info_path), ACS_DSD_CONFIG_PROC_FS_PATH"%d/"ACS_DSD_CONFIG_PROC_CMD_LINE_FILENAME, pid);
	if (!(f = fopen(proc_info_path, "rt"))) return 0; //Process not running in the system
	char cmd_line[ACS_DSD_CONFIG_FILENAME_SIZE_MAX] = {0};
	fscanf(f, "%"ACS_DSD_STRINGIZE(ACS_DSD_CONFIG_FILENAME_SIZE_MAX)"s", cmd_line);
	fclose(f);

	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DEBUG: cmd_line == '%s'", cmd_line);

	return (!strstr(cmd_line, process_name) || (toupper(proc_state) == 'Z')) ? 0 : 1;
}
