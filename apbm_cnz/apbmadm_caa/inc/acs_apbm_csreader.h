#ifndef HEADER_GUARD_CLASS__acs_apbm_csreader
#define HEADER_GUARD_CLASS__acs_apbm_csreader acs_apbm_csreader

/** @file acs_apbm_csreader.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-10-10
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
 *	| R-001 | 2011-10-10 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */


#include <vector>

#include <ace/Recursive_Thread_Mutex.h>

#include <ACS_CS_API.h>

#include <ACS_DSD_Macros.h>

#include <acs_apbm_types.h>
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_csboardinfo.h"
#include "acs_apbm_switchboardinfo.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_csreader


/** @class acs_apbm_csreader acs_apbm_csreader.h
 *	@brief acs_apbm_csreader class
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-10-10
 *
 *	acs_apbm_csreader <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {
	//==============//
	// Enumerations //
	//==============//
public:
	enum ACS_APBM_SwitchBoardFbnIdentifiers {
		SWITCH_BOARD_FBN_SCBRP	= ACS_CS_API_HWC_NS::FBN_SCBRP,
// TODO: In the following line the correct SCX FBC enumeration constant should replace the
// 340 constant as in the previous line
		SWITCH_BOARD_FBN_SCXB		= 340,
		SWITCH_BOARD_FBN_SMXB		= 400
	};

	typedef enum ACS_APBM_ApBoardFbnIdentifiers {
			AP_BOARD_FBN_APUB	= ACS_CS_API_HWC_NS::FBN_APUB,
			AP_BOARD_FBN_DVD	= ACS_CS_API_HWC_NS::FBN_DVD,
			AP_BOARD_FBN_DISK	= ACS_CS_API_HWC_NS::FBN_Disk,
			AP_BOARD_FBN_GEA	= ACS_CS_API_HWC_NS::FBN_GEA
	}apBoard_fbn_t;

	enum ACS_APBM_ApBoardNameIdentifiers {
			AP_BOARD_NAME_DVD	= acs_apbm::BOARD_NAME_DVD,
			AP_BOARD_NAME_GEA	= acs_apbm::BOARD_NAME_GEA,
			AP_BOARD_NAME_DISK	= AP_BOARD_NAME_GEA  +1,
			AP_BOARD_NAME_APUB	= AP_BOARD_NAME_DISK +1,
		};


	//==============//
	// Constructors //
	//==============//
public:
	/** @brief acs_apbm_csreader constructor
	 */
	inline explicit __CLASS_NAME__ (acs_apbm_serverworkingset * server_working_set)
	: _my_ap_node(), _my_ipna_str(), _my_ipnb_str(), _my_bgcia_str(), _my_bgcib_str(), _my_magazine(0), _my_magazine_str(), _my_slot_position(-1),
	  _my_partner_slot_position(-1),_my_side(-1), _my_system_number(0), _hwc(0), _my_ap_info_loaded(0), _server_working_set(server_working_set), _sync_object() {
		*_my_ipna_str = 0;
		*_my_ipnb_str = 0;
		*_my_bgcia_str = 0;
		*_my_bgcib_str = 0;
		*_my_magazine_str = 0;
		*_dmxc_ipa_str = 0;
		*_dmxc_ipb_str = 0;
	}


private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_csreader Destructor
	 */
	inline ~__CLASS_NAME__ () { unbind(); }


	//===========//
	// Functions //
	//===========//
public:
	int bind_to_cs ();

	int unbind ();

	int get_ap_sys_no(int ap_board_name, std::vector<uint16_t> & ap_sys_no_list);

	int get_my_ap_node (ACS_DSD_Node & node);

	int get_my_ipn_addresses (char (& ipna_str) [16], char (& ipnb_str) [16]);
	int get_my_bgci_addresses (char (& bgcia_str) [16], char (& bgcib_str) [16]);
	
	int get_my_side (uint16_t & side);
	int get_my_magazine (uint32_t & magazine);
	int get_my_magazine (char (& magazine) [16]);
	int get_other_magazines (std::vector <uint32_t> & magazines);
	int get_all_magazines (std::vector <uint32_t> & magazines);

	int map_magazine_plug_number (uint32_t & magazine, int mag_plug_number);

	int get_my_slot (int32_t & slot);
	int get_my_gea_slot (int32_t & slot);
	int get_my_partner_slot (int32_t & slot);
	int get_my_system_number (uint16_t &sys_no);

	inline int is_ap_board_fbn (uint16_t fbn) const {
		return ((fbn == ACS_CS_API_HWC_NS::FBN_APUB) ||
						(fbn == ACS_CS_API_HWC_NS::FBN_DVD) ||
						(fbn == ACS_CS_API_HWC_NS::FBN_Disk) ||
						(fbn == ACS_CS_API_HWC_NS::FBN_GEA));
	}

	inline  uint16_t ap_boardname_to_fbn (int board_name_id) const {
			  switch(board_name_id){
				  case AP_BOARD_NAME_DVD:
					  return AP_BOARD_FBN_DVD;

				  case AP_BOARD_NAME_GEA:
					  return AP_BOARD_FBN_GEA;

				  case AP_BOARD_NAME_DISK:
					  return AP_BOARD_FBN_DISK;

				  case AP_BOARD_NAME_APUB:
				  	  return AP_BOARD_FBN_APUB;
				  default:
					  return 0;
			}
		}

	int is_cba_system ();

	int get_environment(int & env);

	int get_oam_access_type(int & oam_access_type);

	int is_multicp_system(bool & is_multicp_system);

	int get_switch_board_fbn (uint16_t & fbn);

	int get_my_switch_boards_info (std::vector <acs_apbm_switchboardinfo> & boards);
	int get_other_switch_boards_info (std::vector <acs_apbm_switchboardinfo> & boards);

	int get_apbm_board (int ap_board_name, acs_apbm_csboardinfo & ap_board);
	int get_apbm_boards (std::vector<acs_apbm_csboardinfo> & boards);
	int get_apbm_boards (std::vector<acs_apbm_csboardinfo> & boards, uint32_t magazine);

	int get_dmxc_addresses(char (& ipna_str) [16], char (& ipnb_str) [16]);

	int reset_info ();

	static int uint32_to_ip_format (char (& value_ip) [16], uint32_t value);
	static int uint32_to_ip_format (char (& value_ip) [16], uint32_t value, char separator);
	static int ip_format_to_uint32 (uint32_t & value, const char * value_ip);
	static int uint32_to_ip_fields (uint8_t & ip1, uint8_t & ip2, uint8_t & ip3, uint8_t & ip4, uint32_t value);
	static int uint32_to_ip_fields (uint8_t (& ip_fields) [4], uint32_t value);
	static int ip_format_to_ip_fields (uint8_t & ip1, uint8_t & ip2, uint8_t & ip3, uint8_t & ip4, const char * value_ip);
	static int ip_format_to_ip_fields (uint8_t (& ip_fields) [4], const char * value_ip);



private:
	int get_my_ap_all_info ();

	int get_switch_boards_info (std::vector <acs_apbm_switchboardinfo> & boards, uint32_t magazine, uint16_t switch_board_fbn, bool match);

	int get_ip_eth_addresses (char (& ipna_address) [16], char (& ipnb_address) [16], BoardID board_id);
	int get_ip_eth_addresses (char (& ipna_address) [16], char (& ipnb_address) [16],char (& ipaliasa_address) [16], char (& ipaliasb_address) [16], BoardID board_id);

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	ACS_DSD_Node _my_ap_node;
	char _my_ipna_str [16];
	char _my_ipnb_str [16];
	char _my_bgcia_str [16];
	char _my_bgcib_str [16];
	uint32_t _my_magazine;
	char _my_magazine_str[16];
	int32_t _my_slot_position;
	int32_t _my_partner_slot_position;
	uint16_t _my_side;
	uint16_t _my_system_number;


	char _dmxc_ipa_str [16];
	char _dmxc_ipb_str [16];

	static int _is_cba_system;
	static int _is_multicp_system;
	static int _environment;
	static int _oam_access_type;

	ACS_CS_API_HWC * _hwc;
	int _my_ap_info_loaded;

	acs_apbm_serverworkingset * _server_working_set;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_csreader
