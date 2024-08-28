/*
 * bios_set_snmpmanager.h
 *
 *  Created on: Nov 18, 2011
 *      Author: xassore
 */

#ifndef BIOS_SET_SNMPMANAGER_H_
#define BIOS_SET_SNMPMANAGER_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "bios_set_common.h"

#define BS_SNMP_OID_ERICSSON					SNMP_OID_ENTERPRISES, 193
#define BS_SNMP_OID_ERICSSON_AXE				BS_SNMP_OID_ERICSSON, 154
#define BS_SNMP_OID_ERICSSON_GENERIC			BS_SNMP_OID_ERICSSON, 177


#define AHE_ETH_COMMUNITY                   "public"
#define GEN_SHELF_COMMUNITY                 "NETMAN"

// AXE-ETH-SHELF-MIB definitions for SCB-RP
#define BS_SCB_OID_SHELF_MGR_CMD_0				BS_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 12, 0
#define BS_SCB_OID_SHELF_MGR_CMD_25				BS_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 12, 25
#define BS_SCB_OID_SHELF_MGR_CMD				BS_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 12

#define BS_SCB_OID_BOARD_RESET				 	BS_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 9
#define BS_SCB_OID_BOARD_BIOS_DEFAULT_IMAGE 	BS_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 19
#define BS_SCB_OID_BOARD_BIOS_POINTER       	BS_SNMP_OID_ERICSSON_AXE, 2, 1, 2, 1, 1, 1, 20

// GENERIC SHELF-MIB definitions for SCX
#define BS_SCX_OID_SHELF_MGR_CTRL_STATE    		BS_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 2, 1, 8, 0
#define BS_SCX_OID_BLADE_BIOS_IMAGE_DEFAULT		BS_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 22
#define BS_SCX_OID_BLADE_BIOS_POINTER      		BS_SNMP_OID_ERICSSON_GENERIC, 2, 2, 1, 3, 1, 1, 1, 23


namespace bs_snmpmanager_ns{

	enum error_code  {
			ERR_NO_ERROR = 0,
			ERR_SNMP_CREATE_PDU = - 100,
			ERR_SNMP_PDU_ADD_VARIABLE_VALUE = 101,
			ERR_SNMP_SENDING_GET_REQUEST = - 102,
			ERR_SNMP_SENDING_SET_REQUEST = - 103,
			ERR_SNMP_OPENING_SESSION = -104,
			ERR_SNMP_PDU_VARIABLE_BAD_TYPE = -105,
			ERR_SNMP_PDU_OID_UNEXPECTED = -106,
			ERR_SNMP_PDU_RESOURCE_UNAVAIL = -107,
			ERR_SNMP_SEND_SYNC_TIMEOUT = -108,
			ERR_SNMP_SEND_SYNC_ERROR = -109,
			ERR_SNMP_GETTING_IPMI_MASTER = -110,
			ERR_SNMP_PDU_COMMAND_BAD_TYPE = -111,
			ERR_SNMP_PDU_GENERIC_ERR = -112,
			ERR_SNMP_PDU_SET_RESOURCE_UNAVAIL = -113
		};

	enum scb_shelf_mgr_state {
		SCB_SHELF_MGR_STATE_NO_SCAN = 0,
		SCB_SHELF_MGR_STATE_SCAN = 1
	};

	enum scx_shelf_mgr_state {
		SCX_SHELF_MGR_STATE_IDLE = 0,
		SCX_SHELF_MGR_STATE_PASSIVE = 1,
		SCX_SHELF_MGR_STATE_PASSIVE_DEGRAD = 2,
		SCX_SHELF_MGR_STATE_ACTIVE = 3
	};
}

// this structure is an helper structure
typedef struct  {
	  //oid Oid[MAX_OID_LEN];
	  oid *Oid;
	  int OidLen;
	}  bs_oids;


class bios_set_snmpmanager
{


  public:
		inline bios_set_snmpmanager (void)
		:_bs_session_handler(NULL), _slot (-1), _biosPointer (-1), _biosDefaultImage (-1), _isCBA(0),  _shelf_mgr_ipmi_master(0) {}
		//bios_set_snmpmanager(char *ipAdr, int slotNR, bool isCBA);
		inline ~bios_set_snmpmanager(){};
		inline int shelf_mgr_ipmi_master(){return _shelf_mgr_ipmi_master;}

		int  initialize(shelfmgr_info ( & switch_boards)[SHELFMGRBOARD_MAXNUM], bool iscba);
		void  close();

		int get_shelf_mngr_ctrl_state (int slot_num, const char * switch_board_ip,
					bios_set_snmp_ns::shelf_mgr_state_t & state);

		//SNMPAPI_STATUS ProcessCallback (WPARAM wParam, LPARAM lParam);
		int GetBIOSPointer(int board_slot, int32_t & bios_pointer);
		int GetBIOSDefaultImage(int board_slot, int32_t & bios_image);
		int SetBIOSDefaultImage(int board_slot, int bios_image_value);
        int SetBIOSPointer(int board_slot, int bios_pointer_value);
//		bool UpgradeIPMIFirmvare(string imagePath);
//		int GetIPMIUpgradeStatus(int* input);
//		int GetIPMIBoardData(string &running, string &product, string &revision);
//		bool IsMaster();
		// DA ELIMINARE
		// int get_result (int status, int board_slot, struct snmp_pdu *pdu, int & result);
		 int oid_to_str (char * buf, size_t size, const oid * id, size_t id_length);
		 int sendSetRequest(oid * oid_name, size_t oid_name_len, int oid_value);
		 int sendGetRequest(oid *oid_name, size_t oid_name_len, int &result);
  private:

		 int read_shelf_mgr_state_result(int status, oid * oid_name, size_t oid_name_len, struct snmp_pdu *pdu, int & result);
		 int get_result (int status, oid * oid_name, size_t oid_name_len, struct snmp_pdu *pdu, int & result);
		 int oid_compare (const oid * id1, size_t id1_length, const oid * id2, size_t id2_length);

		//string ipAddress;
		snmp_session * _bs_session_handler;
		int  _slot;
		int _biosPointer;
		int _biosDefaultImage;
		bool _isCBA;
		int _shelf_mgr_ipmi_master;
		shelfmgr_info _operational_switch_board;

};





#endif /* bios_set_snmpmanager_H_ */
